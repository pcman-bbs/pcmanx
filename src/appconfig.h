// AppConfig.h: interface for the CAppConfig class.
//
// AppConfig.cpp: implementation of the CAppConfig class.
/////////////////////////////////////////////////////////////////////////////
// Name:        appconfig.h
// Purpose:     Application configuration class, deal with configuration
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.20
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by: 
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_APPCONFIG_H__B5A0D4F8_2425_4EDE_88DA_C720587B86DE__INCLUDED_)
#define AFX_APPCONFIG_H__B5A0D4F8_2425_4EDE_88DA_C720587B86DE__INCLUDED_

#ifdef __GNUG__
  #pragma interface "appconfig.h"
#endif

#include "configfile.h"
#include "site.h"

#include <string>
#include <vector>

#include <gdk/gdk.h>

#include "blowfish/blowfish.h"

using namespace std;

class CAppConfig : public CConfigFile
{
public:
	enum	{ SHADOWER_L = 0xf30a439c, SHADOWER_R = 0x204be7a1 };
	void SetToDefault();
	CSite m_DefaultSite;
	void SaveFavorites();
	void LoadFavorites();
	CAppConfig();
	virtual ~CAppConfig();
	virtual bool DoDataExchange(bool bLoad);
//    static string GetTelnetPath();
//    static string GetSSHPath();

/*
    void SetUserPasswd( string passwd );

    string GetUserPasswd();

	BLOWFISH_CTX* GetBlowfish()
	{
		if( ! m_Blowfish && ! GetUserPasswd().empty() )
		{
			m_Blowfish = new BLOWFISH_CTX;
			Blowfish_Init( m_Blowfish, (unsigned char*)m_UserPasswd.c_str(),
				 m_UserPasswd.length() );
		}
		return m_Blowfish;
    }

	void ReleaseBlowfish()
	{
		if( m_Blowfish )
			delete m_Blowfish;
		m_Blowfish = NULL;
	}

    bool IsLoggedIn(){  return m_IsLoggedIn;  }
*/

//	Window
	int MainWndX;
	int MainWndY;
	int MainWndW;
	int MainWndH;
	int MainWndState;

	int EditorX;
	int EditorY;
	int EditorW;
	int EditorH;
	int EditorState;

//	General Setting
	bool QueryOnExit;
	bool QueryOnCloseCon;
	bool CancelSelAfterCopy;
	bool CopyTrimTail;
	bool ShowTrayIcon;
	bool PopupNotifier;
	int PopupTimeout;
	
//	Terminal Settings
	int RowsPerPage;
	int ColsPerPage;
	bool BeepOnBell;

//	Display Settings
	bool AntiAliasFont;
	int CharPaddingX;
	int CharPaddingY;
	bool HCenterAlign;

//	bool TwoDiffFonts;
    int FontSize;
    string FontFamily;

	GdkColor HyperLinkColor;

//	Favorite sites
	vector<CSite> Favorites;
    string WebBrowser;
    int SocketTimeout;

protected:
    string m_UserPasswd;
    BLOWFISH_CTX* m_Blowfish;
    string Shadow;
    bool m_IsLoggedIn;

protected:
    void AfterLoad();

};

extern CAppConfig AppConfig;

#endif // !defined(AFX_APPCONFIG_H__B5A0D4F8_2425_4EDE_88DA_C720587B86DE__INCLUDED_)
