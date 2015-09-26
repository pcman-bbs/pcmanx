// Config.h: interface for the CConfig class.
//
/////////////////////////////////////////////////////////////////////////////
// Name:        config.h
// Purpose:     Application configuration class, deal with configuration automatically
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.22
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by: 
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_)
#define AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_

#ifdef __GNUG__
  #pragma interface "configfile.h"
#endif

//	Copyright (C) 2004 PCMan
//	Author: PCMan (HZY) 2004/07/22	07:51 AM
//	I finally came up with a really smart way to maintain ini file.
//	Every time I add a variable to CAppConfig, all I need to do is 
//	adding the variable in my "Config Table," and all the data will
//	be load and save automatically.  This is not the most efficient way. 
//	In my first version I use some more efficient method, but at last I change 
//	my code to what it is now.  Because I think in a program not time-critical,
//	easy-maintaining is much more important.

#include <string>

using namespace std;

struct CConfigEntry
{
	const char* m_Name;
	enum DataType { VT_END=0, VT_BOOL=1, VT_INT, VT_SHORT, VT_STR, VT_ESTR, VT_COLOR, VT_SECT } m_DataType;
	void* m_pData;
};

class CConfigFile
{
public:
	// Get extra files the app needs.
	string GetDataPath( string FileName );
	string GetDataDirPath(){	return m_DataDirPath;	}

	string GetConfigDirPath(){	return m_ConfigDirPath;	}

	// Get config files of the app.
	string GetConfigPath( string FileName );

	// get the main config file of the app.
	string GetConfigPath(){return GetConfigPath(m_AppName);}

	bool Save();
	bool Load();
	CConfigFile( string AppName, int LineBufSize = 4096);
	virtual bool DoDataExchange( bool bLoad );
	virtual ~CConfigFile();
	inline void SetRoot(CConfigEntry* pRootMap){	m_pCurSect = m_pRootMap = pRootMap;	}
protected:
	bool DoSave();
	bool DoLoad();
//    static string GetExecPath(const char* exec_name);

	CConfigEntry* m_pCurSect;
	CConfigEntry* m_pRootMap;
	int m_LineBufSize;
	string m_ConfigDirPath;
	string m_DataDirPath;
	string m_AppName;

};


#define		BEGIN_CONFIG_SECT( mapname )		CConfigEntry mapname[] = {
#define		BEGIN_CONFIG_FILE		BEGIN_CONFIG_SECT

		#define	_CFG_BOOL( name, variable)	{ name, CConfigEntry::VT_BOOL, (void*)&variable},
		#define	CFG_BOOL( variable)		_CFG_BOOL( ""#variable"", variable)

		#define	_CFG_INT( name, variable)	{ name, CConfigEntry::VT_INT, (void*)&variable},
		#define	CFG_INT( variable)		_CFG_INT( ""#variable"", variable)

		#define	_CFG_SHORT( name, variable)	{ name, CConfigEntry::VT_SHORT, (void*)&variable},
		#define	CFG_SHORT( variable)	_CFG_SHORT( ""#variable"", variable)

		#define	_CFG_STR( name, variable)	{ name, CConfigEntry::VT_STR,  (void*)&variable},
		#define	CFG_STR( variable)		_CFG_STR( ""#variable"", variable)

		#define	_CFG_ESTR( name, variable)	{ name, CConfigEntry::VT_ESTR,  (void*)&variable},
		#define	CFG_ESTR( variable)		_CFG_ESTR( ""#variable"", variable)

		#define	_CFG_CLR( name, variable)	{ name, CConfigEntry::VT_COLOR,  (void*)&variable},
		#define	CFG_CLR( variable)		_CFG_CLR( ""#variable"", variable)

		#define	CFG_SECT( variable)		{ ""#variable"", CConfigEntry::VT_SECT, (void*)&variable},

#define		END_CONFIG_SECT()		{0, CConfigEntry::VT_END, 0} };
#define		END_CONFIG_FILE		END_CONFIG_SECT



#endif // !defined(AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_)
