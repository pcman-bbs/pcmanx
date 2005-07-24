// site.cpp: implementation of the CSite class.
//////////////////////////////////////////////////////////////////////
// Name:        site.cpp
// Purpose:     Site Settings
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.07.15
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "site.h"
#endif

#include "site.h"
#include <stdio.h>
#include <string.h>

//	#include "appconfig.h"

//	#include "blowfish/blowfish.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSite::CSite(string Name)
{
	m_Name = Name;
	// Time duration in seconds during which should we reconnect 
	// automatically when disconnected from server, and 0 means disabled.
	m_AutoReconnect = 20;

	// We send this string, m_AntiIdleStr, to the server every 'm_AntiIdle' 
	// seconds to prevent being kicked by the server.
	m_AntiIdleStr = "^[OB";	// empty string means '\0'
	m_AntiIdle = 180;	// 0 means disabled

	// Site Encoding
	m_Encoding = "Big5";

	// Terminal settings
	// Rows per page
	m_RowsPerPage = 24;
	// Cols per page
	m_ColsPerPage = 80;

	// When pasting long articles, especially those from webpages, wrap lines 
	// automatically when there are more than 'm_AutoWrapOnPaste' characters per line.
	m_AutoWrapOnPaste = 78;	// 0 means disabled.

	// Terminal type
	m_TermType = "vt100";

	// Convert ESC characters in ANSI color to m_ESCConv
	m_ESCConv = "^U";

	// Send CR or CRLF when Enter is pressed.
	m_CRLF = 0;	// 0 = CR, 1 = LF, 2 = CRLF

	// Connect automatically when startup.
	m_Startup = false;

#ifdef USE_EXTERNAL
	m_UseExternalSSH = false;
	m_UseExternalTelnet = false;
#endif

	m_MenuItem = NULL;
}

CSite::~CSite()
{
}


void CSite::SaveToFile(FILE *fo)
{
	fprintf( fo, "[%s]\n", m_Name.c_str() );
	fprintf( fo, "URL=%s\n", m_URL.c_str() );
	fprintf( fo, "AutoReconnect=%d\n", m_AutoReconnect );
	fprintf( fo, "AntiIdle=%d\n", m_AntiIdle );
	fprintf( fo, "AntiIdleStr=%s\n", m_AntiIdleStr.c_str() );
	fprintf( fo, "Encoding=%s\n", m_Encoding.c_str() );
	fprintf( fo, "Rows=%d\n", m_RowsPerPage );
	fprintf( fo, "Cols=%d\n", m_ColsPerPage );
	fprintf( fo, "TermType=%s\n", m_TermType.c_str() );
	fprintf( fo, "ESCConv=%s\n", m_ESCConv.c_str() );
	fprintf( fo, "CRLF=%d\n", m_CRLF );
	fprintf( fo, "Startup=%d\n", m_Startup );

#ifdef USE_EXTERNAL
	fprintf( fo, "UseExternalSSH=%d\n", m_UseExternalSSH );
	fprintf( fo, "UseExternalTelnet=%d\n", m_UseExternalTelnet );
#endif

	fprintf( fo, "PreLoginPrompt=%s\n", m_PreLoginPrompt.c_str() );
	fprintf( fo, "PreLogin=%s\n", m_PreLogin.c_str() );
	fprintf( fo, "PostLogin=%s\n", m_PostLogin.c_str() );
	fprintf( fo, "LoginPrompt=%s\n", m_LoginPrompt.c_str() );
	fprintf( fo, "Login=%s\n", m_Login.c_str() );
	fprintf( fo, "PasswdPrompt=%s\n", m_PasswdPrompt.c_str() );

	if( m_Passwd.length() /*&& AppConfig.IsLoggedIn()*/ )
	{
/*		BLOWFISH_CTX *bfc = AppConfig.GetBlowfish();
		if( bfc )
		{
			char buf[16];
			memset( buf, 0, sizeof(buf) );
			strcpy(buf, m_Passwd.c_str());
			unsigned long l, r;
			memcpy(&l, buf, 4 );
			memcpy(&r, buf + 4, 4 );
			Blowfish_Encrypt( bfc, &l, &r );
			fprintf( fo, "Passwd=%X,%X,", l, r );
			memcpy(&l, buf + 8, 4 );
			memcpy(&r, buf + 12, 4 );
			Blowfish_Encrypt( bfc, &l, &r );
			fprintf( fo, "%X,%X\n", l, r );
			AppConfig.ReleaseBlowfish();
		}

	}
	else
*/
		fprintf( fo, "Passwd=%s\n", m_Passwd.c_str() );
	}
}


