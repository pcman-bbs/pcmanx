#include <Python.h>
#include <cstring>
#include <vector>
#include "script/api.h"
#include "telnetcon.h"

static PyObject*
SendUnEscapedString(PyObject *self, PyObject *args)
{
	char *pstr;
	long lp;

	if (!PyArg_ParseTuple(args, "ls", &lp, &pstr))
		return NULL;

	string str(pstr);

	((CTelnetCon*)lp)->SendUnEscapedString(str);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
SendString(PyObject *self, PyObject *args)
{
	char *pstr;
	long lp;

	if (!PyArg_ParseTuple(args, "ls", &lp, &pstr))
		return NULL;

	string str(pstr);

	((CTelnetCon*)lp)->SendString(str);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef PCManXMethods[] = {
	{"SendString", SendString, METH_VARARGS,
		"Send String to Instance Window."},
	{"SendUnEscapedString", SendUnEscapedString, METH_VARARGS,
		"Send un escaped string to Instance Window."},
	{NULL, NULL, 0, NULL}
};

vector<PyObject*> pModules;
void InitScriptInterface(char *path)
{
	char *cmd;
	PyObject *pName;

	Py_Initialize();
	PyRun_SimpleString("import sys\n");
	cmd = (char*)malloc(sizeof(char)*(strlen(path) + strlen("sys.path.insert(0,'')")));
	sprintf(cmd, "sys.path.insert(0,'%s')", path);
	PyRun_SimpleString(cmd);
	free(cmd);

	Py_InitModule("PCManX", PCManXMethods);

	/* FIXME: Load Plugins from _path_ */

	pName = PyString_FromString("orz");
	pModules.push_back(PyImport_Import(pName));
	Py_DECREF(pName);
	if (PyErr_Occurred())
		PyErr_Print();
}

void ScriptOnNewIncomingMessage(void *handle, char *text)
{
	PyObject *pHandle, *pFunc, *pArgs, *pMsg, *pDict;

	vector<PyObject*>::iterator pModule;
	for( pModule = pModules.begin(); pModule != pModules.end(); ++pModule )
	{
		if (*pModule != NULL)
			pDict = PyModule_GetDict(*pModule);
		else
		{
			printf("Failed to PyImport_Import\n");
		}

		if(pDict == NULL )
		{
			printf("Failed to PyModule_GetDict\n");
		}

		if(!*pModule || !pDict)
		{
			printf("Failed to load script\n");
			return;
		}
		pFunc = PyDict_GetItemString(pDict, "OnNewIncomingMessage");

		if (pFunc && PyCallable_Check(pFunc)) {
			pArgs = PyTuple_New(2);
			pHandle = PyInt_FromLong((long)handle);
			pMsg = PyString_FromString(text);
			if (!pMsg || !pHandle) {
				Py_DECREF(pArgs);
				fprintf(stderr, "Cannot convert argument\n");
				return;
			}
			PyTuple_SetItem(pArgs, 0, pHandle);
			PyTuple_SetItem(pArgs, 1, pMsg);
			PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);
			Py_DECREF(pMsg);
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
			fprintf(stderr, "Cannot find function \"%s\"\n", "OnNewIncomingMessage");
		}
	}
}

void FinalizeScriptInterface()
{
	vector<PyObject*>::iterator pModule;
	for( pModule = pModules.begin(); pModule != pModules.end(); ++pModule )
		if(*pModule)
			Py_DECREF(*pModule);
	Py_Finalize();
}
