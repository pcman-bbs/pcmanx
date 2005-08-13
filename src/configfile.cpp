// Config.cpp: implementation of the CConfigFile class.
//
/////////////////////////////////////////////////////////////////////////////
// Name:        config.cpp
// Purpose:     Application configuration class, deal with configuration automatically
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.22
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "configfile.h"
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

#include "configfile.h"
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <gdk/gdk.h>
#include <glib.h>

#include "stringutil.h"

#include "debug.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConfigFile::CConfigFile( string AppName, int LineBufSize )
{
	m_AppName = AppName;
	m_LineBufSize = LineBufSize;

	m_ConfigDirPath = getenv("HOME");
	if( m_ConfigDirPath[m_ConfigDirPath.length()-1] != '/' )
		m_ConfigDirPath += '/';

	m_ConfigDirPath += '.';
	m_ConfigDirPath += m_AppName;

	if(!g_file_test(m_ConfigDirPath.c_str(),
		GFileTest(G_FILE_TEST_IS_DIR|G_FILE_TEST_EXISTS)) )
			mkdir(m_ConfigDirPath.c_str(), 0777);

	m_ConfigDirPath += "/";

	m_DataDirPath = DATADIR "/" + AppName;

	if( g_file_test(m_DataDirPath.c_str(),
		GFileTest(G_FILE_TEST_IS_DIR|G_FILE_TEST_EXISTS)) )
		m_DataDirPath += "/";
	else
		m_DataDirPath = m_ConfigDirPath;
}

CConfigFile::~CConfigFile()
{
}



bool CConfigFile::Load()
{
	return DoDataExchange(true);	// call virtual function defined in derived class.
}

bool CConfigFile::DoLoad()
{
	FILE* fp = fopen( GetConfigPath().c_str() ,"r");
	if(fp)
	{
		char* line = new char[m_LineBufSize];
		while( fgets( line, m_LineBufSize, fp) )
		{
			char* keyname = strtok( line, " =\n");
			if( !keyname )	continue;
			if( keyname[0] == '[' )	// This line doesn't contain any value.
			{
				keyname = strtok( line, "[]\n" );
				if( keyname )	// Find a section.
				{
					CConfigEntry* pent = m_pRootMap;
					for( ; pent->m_Name; pent++ )
					{
						if( 0 == strcmp(keyname, pent->m_Name ) )
						{
							m_pCurSect = (CConfigEntry*)pent->m_pData;
							break;
						}
					}
					continue;
				}
			}
			char* pstrval = strtok( NULL, "=\n");
			if( !pstrval )	continue;

			CConfigEntry* pent = m_pCurSect;
			for( ; pent->m_Name; pent++ )
			{
				if( 0 == strcmp(keyname, pent->m_Name ) )
					break;
			}

			if( pent->m_Name )
			{
				switch( pent->m_DataType )
				{
				case CConfigEntry::VT_BOOL:
					*((bool*)pent->m_pData) = atoi(pstrval);
					break;
				case CConfigEntry::VT_INT:
					*((int*)pent->m_pData) = atoi(pstrval);
					break;
				case CConfigEntry::VT_STR:
					*((string*)pent->m_pData) = pstrval;
					break;
				case CConfigEntry::VT_ESTR:
					*((string*)pent->m_pData) = UnEscapeStr( pstrval );
					break;
				case CConfigEntry::VT_COLOR:
					{
						int r,g,b;
						if( 3 == sscanf( pstrval, "%d,%d,%d", &r, &g, &b ) )
						{
//							((wxColour*)pent->m_pData)->Set( r, g, b );
							GdkColor* clr = (GdkColor*)pent->m_pData;
							clr->red = r*(65536/256);
							clr->green = g*(65536/256);
							clr->blue = b*(65536/256);
						}
					}
					break;
				case CConfigEntry::VT_SHORT:
					*((short*)pent->m_pData) = atoi(pstrval);
					break;
				default:
					break;	//	This shouldn't happen.
				}
			}
		}
		fclose(fp);
		delete []line;
	}
	return fp;
}

bool CConfigFile::Save()
{
	return DoDataExchange(false);	// call virtual function defined in derived class.
}

bool CConfigFile::DoSave()
{
	FILE* fp = fopen( GetConfigPath().c_str() ,"w");
	if(fp)
	{
		string esc_str;
		for( ; m_pRootMap->m_Name; m_pRootMap++ )
		{
			m_pCurSect = (CConfigEntry*)m_pRootMap->m_pData;
			fprintf(fp, "[%s]\n", m_pRootMap->m_Name );

			char strval[32];	const char* pstrval;
			for( ; m_pCurSect->m_Name; m_pCurSect++ )
			{
				pstrval = strval;
				switch( m_pCurSect->m_DataType )
				{
				case CConfigEntry::VT_BOOL:
					sprintf(strval,"%d", (int)*((bool*)m_pCurSect->m_pData));
					break;
				case CConfigEntry::VT_INT:
					sprintf(strval,"%d", *((int*)m_pCurSect->m_pData) );
					break;
				case CConfigEntry::VT_STR:
					pstrval = ((string*)m_pCurSect->m_pData)->c_str();
					break;
				case CConfigEntry::VT_ESTR:
					esc_str = EscapeStr( ((string*)m_pCurSect->m_pData)->c_str() );
					pstrval = esc_str.c_str();
					break;
				case CConfigEntry::VT_COLOR:
					{
//						wxColour& clr = *((wxColour*)m_pCurSect->m_pData);
						GdkColor& clr = *((GdkColor*)m_pCurSect->m_pData);
						sprintf( strval, "%d,%d,%d", clr.red*256/65536, clr.green*256/65536, clr.blue*256/65536 );
					}
					break;
				case CConfigEntry::VT_SHORT:
					sprintf(strval,"%d", *((short*)m_pCurSect->m_pData) );
					break;
				default:	// This shouldn't happen.
					break;
				}
				fprintf(fp, "%s=%s\n", m_pCurSect->m_Name, pstrval );
			}
			fputs( "\n", fp );
		}
		fclose(fp);
	}
	return fp;
}

bool CConfigFile::DoDataExchange( bool bLoad )
{
	return bLoad ? DoLoad():DoSave();
}

string CConfigFile::GetConfigPath( string FileName )
{
	string path = m_ConfigDirPath;
	path += FileName;

	return path;
}

string CConfigFile::GetDataPath( string FileName )
{
	// Windows NT or UNIX-like systems
	string path = GetConfigPath(FileName);
//	if( ::wxFileExists( path ) )	// Find the same file in users' home dir first.
	FILE *fp;
	if( fp = fopen(path.c_str(), "r") )
	{
		fclose(fp);
		return path;
	}
	path = m_DataDirPath;
	path += FileName;
	return path;
}

/*
string CConfigFile::GetExecPath(const char* exec_name)
{
	const char* ppaths = getenv("PATH");
	if( !ppaths || !*ppaths )
		ppaths = "/usr/local/bin:/usr/bin:/bin";
	char *dirs = strdup(ppaths);
	for( char* dir = strtok(dirs, ":"); dir && *dir; dir=strtok(NULL, ":") )
	{
		string path(dir);
		if( '/' != path[path.length()-1] )
			path += '/';
		path += exec_name;
		if( g_file_test( path.c_str(), G_FILE_TEST_EXISTS ) 
			&& g_file_test( path.c_str(), G_FILE_TEST_IS_EXECUTABLE ) )
		{
			free(dirs);
			return path;
		}
	}
	free(dirs);
}
*/
