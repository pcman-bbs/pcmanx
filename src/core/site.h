/**
 * site.h - interface for the CSite class.
 *                                                                              
 * Copyright (c) 2004-2005 PCMan <pcman.tw@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#if !defined(PCMANX_SITE_H)
#define PCMANX_SITE_H

#ifdef __GNUG__
  #pragma interface "site.h"
#endif

#include "pcmanx_utils.h"

#include <gtk/gtk.h>

#include <cstdio>
#include <string>

#include "stringutil.h"

using namespace std;

class CSite
{
public:
	bool m_Startup;
	X_EXPORT void SaveToFile(FILE* fo);
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

	// Detect Double-byte characters?
	bool m_DetectDBChar;
	
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
	const char* GetCRLF()
	{
		const char* crlf[3] = { "\r", "\n", "\r\n" };
		return (m_CRLF > 3 ? "\r" : crlf[m_CRLF]);
	}

	string GetEscapeChar()	{	return UnEscapeStr(m_ESCConv.c_str());	}

#ifdef USE_EXTERNAL
	bool m_UseExternalSSH;
	bool m_UseExternalTelnet;
#endif
	bool m_bHorizontalCenterAlign;
	bool m_bVerticalCenterAlign;

#ifdef USE_PROXY
	// Proxy settings
	int    m_ProxyType;
	string m_ProxyAddr;
	string m_ProxyPort;
	string m_ProxyUser;
	string m_ProxyPass;
#endif

	X_EXPORT CSite(string Name = "");
	X_EXPORT ~CSite();

	X_EXPORT string& GetPasswd(){	return m_Passwd;	}
	X_EXPORT void SetPasswd( string passwd ){	m_Passwd = passwd;	}

	X_EXPORT string& GetPasswdPrompt(){	return m_PasswdPrompt;	}
	X_EXPORT void SetPasswdPrompt( string passwd_prompt ){	m_PasswdPrompt = passwd_prompt;	}

	X_EXPORT string& GetLogin(){	return m_Login;	}
	X_EXPORT void SetLogin( string login ){	m_Login = login;	}

	X_EXPORT string& GetLoginPrompt(){	return m_LoginPrompt;	}
	X_EXPORT void SetLoginPrompt( string login_prompt ){	m_LoginPrompt = login_prompt;	}

	X_EXPORT string& GetPreLogin(){	return m_PreLogin;	}
	X_EXPORT void SetPreLogin(string prelogin){	m_PreLogin = prelogin;	}

	X_EXPORT string& GetPreLoginPrompt(){	return m_PreLoginPrompt;	}
	X_EXPORT void SetPreLoginPrompt( string prelogin_prompt ){	m_PreLoginPrompt = prelogin_prompt;	}

	X_EXPORT string& GetPostLogin(){	return m_PostLogin;	}
	X_EXPORT void SetPostLogin(string postlogin){	m_PostLogin = postlogin;	}

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
