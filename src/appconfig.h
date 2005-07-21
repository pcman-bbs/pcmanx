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

using namespace std;

//	#include "hyperlink.h"

//	#include "blowfish/blowfish.h"

/*
class CMainFrame;
class CTermHyperLink : public CHyperLink
{
public:
	CMainFrame* m_pMainFrm;
	CTermHyperLink() : m_pMainFrm(NULL){}
	~CTermHyperLink(){}
	void Open( const char* URL );
};
*/

enum { WS_NORMAL = 0, WS_MAXIMIZED = 1, WS_FULLSCR = 2 };
// These Window-State flags can be combined with OR.

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
    string GetUserPasswd()
	{
		if( m_UserPasswd.empty() )
			m_UserPasswd = ::wxGetPasswordFromUser(_("Enter Your Password :"),
				 _("Password") );
		return m_UserPasswd;
	}

   BLOWFISH_CTX* GetBlowfish()
    {
		if( ! m_Blowfish && ! GetUserPasswd().empty() )
		{
			m_Blowfish = new BLOWFISH_CTX;
			Blowfish_Init( m_Blowfish, (unsigned char*)GetUserPasswd().c_str(),
				 GetUserPasswd().length() );
		}
		return m_Blowfish;
    }

	void ReleaseBlowfish()
	{
		if( m_Blowfish )
			delete m_Blowfish;
		m_Blowfish = NULL;
	}
*/

    bool IsLoggedIn(){  return m_IsLoggedIn;  }

//    wxSize GetToolbarIconSize(){  return m_ToolbarIconSize;  }

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

//	HyperLinks
//	CTermHyperLink HyperLink;

protected:
    string m_UserPasswd;
//    BLOWFISH_CTX* m_Blowfish;
    string Shadow;
    bool m_IsLoggedIn;
//    wxSize m_ToolbarIconSize;


protected:
    void AfterLoad();

};

extern CAppConfig AppConfig;

#endif // !defined(AFX_APPCONFIG_H__B5A0D4F8_2425_4EDE_88DA_C720587B86DE__INCLUDED_)
