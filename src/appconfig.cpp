// AppConfig.cpp: implementation of the CAppConfig class.
/////////////////////////////////////////////////////////////////////////////
// Name:        appconfig.cpp
// Purpose:     Application configuration class, deal with configuration
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.20
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by: 
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "appconfig.h"
#endif

#include "appconfig.h"
#include "configfile.h"
#include "termdata.h"
#include "mainframe.h"
#include "stringutil.h"
#include <stdio.h>

CAppConfig AppConfig;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
void CTermHyperLink::Open( const char* URL )
{
	if( m_pMainFrm && 0 == strncmpi( URL, "telnet://", 9) )
	{
		URL += 9;
		m_pMainFrm->NewCon( URL, URL, &AppConfig.m_DefaultSite );
	}
	else
		CHyperLink::Open( URL );
}
*/

CAppConfig::CAppConfig() : CConfigFile("pcmanx") /*, HyperLinkColor(255,102,0)*/
{
}

CAppConfig::~CAppConfig()
{
//	WX_CLEAR_ARRAY(Favorites);
//	ReleaseBlowfish();
}

//	Author: PCMan (HZY) 2004/07/22	07:51 AM
//	I finally came up with a really smart way to maintain ini file.
//	Every time I add a variable to CAppConfig, all I need to do is 
//	adding the variable in my "Config Table," and all the data will
//	be load and save automatically.  This is not the most efficient way. 
//	In my first version I use some more efficient method, but at last I change 
//	my code to what it is now.  Because I think in a program not time-critical,
//	easy-maintaining is much more important.
bool CAppConfig::DoDataExchange(bool bLoad)
{
/*	if( bLoad )
	{

		HyperLink.Alloc(3);

		HyperLink.Add( "http:mozilla" );
		HyperLink.Add( "ftp:mozilla" );
		HyperLink.Add( "mailto:mozilla" );

	}
*/

	BEGIN_CONFIG_SECT(Window)
		CFG_INT( MainWndX )
		CFG_INT( MainWndY )
		CFG_INT( MainWndW )
		CFG_INT( MainWndH )
		CFG_INT( MainWndState )
		CFG_INT( EditorX )
		CFG_INT( EditorY )
		CFG_INT( EditorW )
		CFG_INT( EditorH )
		CFG_INT( EditorState )
	END_CONFIG_SECT()

	BEGIN_CONFIG_SECT(General)
//		CFG_STR ( Shadow )
		CFG_BOOL( QueryOnExit)
		CFG_BOOL( QueryOnCloseCon)
		CFG_BOOL( CancelSelAfterCopy)
		CFG_BOOL( CopyTrimTail)
		CFG_BOOL( BeepOnBell )
		CFG_BOOL( ShowTrayIcon )
		CFG_STR (WebBrowser)
	END_CONFIG_SECT()

	BEGIN_CONFIG_SECT(Display)
		CFG_STR ( FontFamily )
		CFG_INT ( FontSize )
		CFG_BOOL( AntiAliasFont )
		CFG_BOOL( HCenterAlign )
		CFG_INT ( CharPaddingX)
		CFG_INT ( CharPaddingY)
	END_CONFIG_SECT()
	
	BEGIN_CONFIG_SECT(Color)
		_CFG_CLR( "Black", CTermCharAttr::m_DefaultColorTable[0] )
		_CFG_CLR( "DarkRed", CTermCharAttr::m_DefaultColorTable[1] )
		_CFG_CLR( "DarkGreen", CTermCharAttr::m_DefaultColorTable[2] )
		_CFG_CLR( "Brown", CTermCharAttr::m_DefaultColorTable[3] )
		_CFG_CLR( "DarkBlue", CTermCharAttr::m_DefaultColorTable[4] )
		_CFG_CLR( "DarkMagenta", CTermCharAttr::m_DefaultColorTable[5] )
		_CFG_CLR( "DarkCyan", CTermCharAttr::m_DefaultColorTable[6] )
		_CFG_CLR( "LightGray", CTermCharAttr::m_DefaultColorTable[7] )
		_CFG_CLR( "Gray", CTermCharAttr::m_DefaultColorTable[8] )
		_CFG_CLR( "Red", CTermCharAttr::m_DefaultColorTable[9] )
		_CFG_CLR( "Green", CTermCharAttr::m_DefaultColorTable[10] )
		_CFG_CLR( "Yellow", CTermCharAttr::m_DefaultColorTable[11] )
		_CFG_CLR( "Blue", CTermCharAttr::m_DefaultColorTable[12] )
		_CFG_CLR( "Magenta", CTermCharAttr::m_DefaultColorTable[13] )
		_CFG_CLR( "Cyan", CTermCharAttr::m_DefaultColorTable[14] )
		_CFG_CLR( "White", CTermCharAttr::m_DefaultColorTable[15] )
		CFG_CLR( HyperLinkColor )
	END_CONFIG_SECT()

	BEGIN_CONFIG_SECT(Site)
		_CFG_INT ( "AutoReconnect", m_DefaultSite.m_AutoReconnect )
		_CFG_INT ( "AntiIdle", m_DefaultSite.m_AntiIdle )
		_CFG_STR ( "AntiIdleStr", m_DefaultSite.m_AntiIdleStr )
		_CFG_INT ( "Cols", m_DefaultSite.m_ColsPerPage )
		_CFG_INT ( "Rows", m_DefaultSite.m_RowsPerPage )
		_CFG_STR ( "TermType", m_DefaultSite.m_TermType )
		_CFG_INT ( "CRLF", m_DefaultSite.m_CRLF )
		_CFG_STR ( "ESCConv", m_DefaultSite.m_ESCConv )
	END_CONFIG_SECT()

	BEGIN_CONFIG_SECT(HyperLink)
//		_CFG_STR( "Type1", this->HyperLink[0] )
//		_CFG_STR( "Type2", this->HyperLink[1] )
//		_CFG_STR( "Type3", this->HyperLink[2] )
	END_CONFIG_SECT()


	BEGIN_CONFIG_FILE( ConfigFile )
		CFG_SECT( Window )
		CFG_SECT( General )
		CFG_SECT( Display )
		CFG_SECT( Color )
		CFG_SECT( Site )
//		CFG_SECT( HyperLink )
	END_CONFIG_FILE()

	SetRoot(ConfigFile);

	bool ret = CConfigFile::DoDataExchange(bLoad);

	if( bLoad )
		AfterLoad();

	return ret;
}


void CAppConfig::LoadFavorites()
{
	FILE* fi = fopen( GetConfigPath("favorites").c_str() ,"r" );
	if( fi )
	{
		char line[1024];
		CSite* pSite = NULL;
//		BLOWFISH_CTX* bfc = NULL;
		while( fgets( line, 1024, fi ) )
		{
			char* pname = strtok( line, " =\r\n" );
			if( !pname )
				continue;
			if( pname[0]=='[' )
			{
				pname = strtok(pname, "[]");
				Favorites.push_back(CSite(pname));
				pSite = &Favorites.back();
				
				continue;
			}
			char *pval = strtok( NULL, "\r\n" );
			if( !pSite || !pval )
				continue;

			if(	0 == strcmp( pname, "URL" ) )
				pSite->m_URL = pval;
			else if( 0 == strcmp( pname, "AutoReconnect" ) )
				pSite->m_AutoReconnect = atoi(pval);
			else if( 0 == strcmp( pname, "AntiIdle" ) )
				pSite->m_AntiIdle = atoi(pval);
			else if( 0 == strcmp( pname, "Antiidlestr" ) )
				pSite->m_AntiIdleStr = pval;
			else if( 0 == strcmp( pname, "Encoding" ) )
				pSite->m_Encoding = pval;
			else if( 0 == strcmp( pname, "Rows" ) )
				pSite->m_RowsPerPage = atoi(pval);
			else if( 0 == strcmp( pname, "Cols" ) )
				pSite->m_ColsPerPage = atoi(pval);
			else if( 0 == strcmp( pname, "TermType" ) )
				pSite->m_TermType = pval;
			else if( 0 == strcmp( pname, "ESCConv" ) )
				pSite->m_ESCConv = pval;
			else if( 0 == strcmp( pname, "Startup" ) )
				pSite->m_Startup = (bool)atoi(pval);
#ifdef USE_EXTERNAL
			else if( 0 == strcmp( pname, "UseExternalSSH" ) )
				pSite->m_UseExternalSSH = (bool)atoi(pval);
			else if( 0 == strcmp( pname, "UseExternalTelnet" ) )
				pSite->m_UseExternalTelnet = (bool)atoi(pval);
#endif
			else if( 0 == strcmp( pname, "PreLoginPrompt" ) )
				pSite->SetPreLoginPrompt( pval );
			else if( 0 == strcmp( pname, "PreLogin" ) )
				pSite->SetPreLogin( pval );
			else if( 0 == strcmp( pname, "PostLogin" ) )
				pSite->SetPostLogin( pval );
			else if( 0 == strcmp( pname, "LoginPrompt" ) )
				pSite->SetLoginPrompt( pval );
			else if( 0 == strcmp( pname, "Login" ) )
				pSite->SetLogin( pval );
			else if( 0 == strcmp( pname, "PasswdPrompt" ) )
				pSite->SetPasswdPrompt( pval );
			else if( 0 == strcmp( pname, "Passwd" ) )
			{
/*				if( !*pval )
					continue;
				if( m_IsLoggedIn )
				{
					bfc = GetBlowfish();
					char passwd_buf[17];
					memset( passwd_buf, 0, sizeof(passwd_buf) );
					unsigned long l = 0;
					unsigned long r = 0;
					unsigned long l2 = 0;
					unsigned long r2 = 0;
					sscanf( pval, "%X,%X,%X,%X", &l, &r, &l2, &r2 );
					Blowfish_Decrypt( bfc, &l, &r );
					memcpy( passwd_buf, &l, 4 );
					memcpy( passwd_buf+4, &r, 4);

					Blowfish_Decrypt( bfc, &l2, &r2 );
					memcpy( passwd_buf+8, &l2, 4 );
					memcpy( passwd_buf+12, &r2, 4);
					pSite->SetPasswd( passwd_buf );

				}
				else
					pSite->SetPasswd( pval );
*/			}
		}
//		ReleaseBlowfish();
		fclose(fi);
	}
}

void CAppConfig::SaveFavorites()
{
	FILE* fo = fopen( GetConfigPath("favorites").c_str() , "w" );
	if( fo )
	{
		bool has_sensitive_data = false;
		vector<CSite>::iterator it;
		for( it = Favorites.begin(); it != Favorites.end(); ++it )
		{
			CSite& site = *it;
			site.SaveToFile(fo);
			fputc( '\n', fo );

//			if( pSite->GetPasswd().length() )
//				has_sensitive_data = true;
		}
		fclose(fo);

//		if( ! has_sensitive_data )	// No data needs to be encrypted, cancel password.
//			SetUserPasswd( wxEmptyString );
	}
}

void CAppConfig::SetToDefault()
{
	RowsPerPage=24;
	ColsPerPage=80;
	MainWndX = MainWndY = EditorX = EditorY = 40;
	MainWndW = EditorW = 640;	MainWndH = EditorH = 480;
	MainWndState = AppConfig.EditorState = WS_NORMAL;
	QueryOnExit = 1;
	QueryOnCloseCon = 1;
	CancelSelAfterCopy =1;
	CopyTrimTail = 1;

	CharPaddingX = 0;
	CharPaddingY = 0;
	m_IsLoggedIn = false;

	BeepOnBell = true;

	FontSize = 14;
	FontFamily = "Sans";
	AntiAliasFont = true;
	HCenterAlign = false;

	WebBrowser = "mozilla";
	ShowTrayIcon = true;

	HyperLinkColor.red = 65535;
	HyperLinkColor.green = 65536*102/256;
	HyperLinkColor.blue = 0;
}


void CAppConfig::AfterLoad()
{
/*
	if( 0 == Shadow.length() )
		return;
	unsigned long l = 0, r = 0;
	sscanf( Shadow.c_str(), "%X,%X", &l, &r );
	BLOWFISH_CTX* bfc = GetBlowfish();
	if( bfc )
	{
		Blowfish_Decrypt( bfc, &l, &r );
		m_IsLoggedIn = ( l == SHADOWER_L && r == SHADOWER_R );
		ReleaseBlowfish();
	}
*/
}

/*
void CAppConfig::SetUserPasswd( string passwd )
{
	m_UserPasswd = passwd;
	if( passwd.empty() )
	{
		Shadow.Clear();
		return;
	}

	BLOWFISH_CTX* bfc = GetBlowfish();
	unsigned long l = SHADOWER_L , r = SHADOWER_R;
	Blowfish_Encrypt( bfc, &l, &r );
	m_IsLoggedIn = true;
	Shadow = string::Format("%X,%X", l, r );
	ReleaseBlowfish();

}
*/

/*
string CAppConfig::GetTelnetPath()
{
    static string path;
	if( path.empty() )
		path = GetExecPath("telnet");
	return path;
}


string CAppConfig::GetSSHPath()
{
    static string path;
	if( path.empty() )
		path = GetExecPath("ssh");
	return path;
}
*/

