// site.h: interface for the CSite class.
/////////////////////////////////////////////////////////////////////////////
// Name:        site.h
// Purpose:     Site Settings
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.07.15
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////


#if !defined(PCMANX_SITE_H)
#define PCMANX_SITE_H

#ifdef __GNUG__
  #pragma interface "site.h"
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include <string>

using namespace std;

class CSite
{
public:
	bool m_Startup;
	void SaveToFile(FILE* fo);
	// Name of site
	string m_Name;

	// IP : port
	string m_URL;

	// Time duration in seconds during which should we reconnect
	// automatically when disconnected from server, and 0 means disabled.
	unsigned int m_AutoReconnect;

	// We send this string, m_AntiIdleStr, to the server every 'm_AntiIdle'
	// seconds to prevent being kicked by the server.
	string m_AntiIdleStr;	// empty string means '\0'
	unsigned int m_AntiIdle;	// 0 means disabled

	// Site Encoding
	string m_Encoding;

	// Terminal settings
	// Rows per page
	unsigned int m_RowsPerPage;
	// Cols per page
	unsigned int m_ColsPerPage;

	// When pasting long articles, especially those from webpages, wrap lines 
	// automatically when there are more than 'm_AutoWrapOnPaste' characters per line.
	unsigned int m_AutoWrapOnPaste;	// 0 means disabled.

	// Convert ESC characters in ANSI color to m_ESCConv
	string m_ESCConv;

	// Terminal type
	string m_TermType;

	int m_CRLF;
    GtkWidget*  m_MenuItem;
	// Send CR, LF, or CRLF when Enter is pressed
	inline const char* GetCRLF()
	{
		const char* crlf[3] = { "\r", "\n", "\r\n" };
		return (m_CRLF > 3 ? "\r" : crlf[m_CRLF]);
	}

#ifdef	USE_SSH
	//	If SSH is supported
	bool m_UseSSH;
	string m_PublicKeyFile;
	string m_PrivateKeyFile;
#endif

	CSite(const char* Name = "");
	~CSite();

    string& GetPasswd(){	return m_Passwd;	}
    void SetPasswd( string passwd ){	m_Passwd = passwd;	}
    
    string& GetPasswdPrompt(){	return m_PasswdPrompt;	}
    void SetPasswdPrompt( string passwd_prompt ){	m_PasswdPrompt = passwd_prompt;	}
    
	string& GetLogin(){	return m_Login;	}
    void SetLogin( string login ){	m_Login = login;	}

	string& GetLoginPrompt(){	return m_LoginPrompt;	}
    void SetLoginPrompt( string login_prompt ){	m_LoginPrompt = login_prompt;	}

    string& GetPreLogin(){	return m_PreLogin;	}
    void SetPreLogin(string prelogin){	m_PreLogin = prelogin;	}
    
	string& GetPreLoginPrompt(){	return m_PreLoginPrompt;	}
    void SetPreLoginPrompt( string prelogin_prompt ){	m_PreLoginPrompt = prelogin_prompt;	}

	string& GetPostLogin(){	return m_PostLogin;	}
    void SetPostLogin(string postlogin){	m_PostLogin = postlogin;	}

protected:
    string m_Passwd;
    string m_Login;
    string m_LoginPrompt;
    string m_PasswdPrompt;
    string m_PreLogin;
    string m_PreLoginPrompt;
	string m_PostLogin;
};

#endif // !defined(PCMANX_SITE_H)
