/**
 * site.cpp - implementation of the CSite class.
 *            Site Settings
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

	m_bHorizontalCenterAlign = false;
	m_bVerticalCenterAlign = false;

	m_MenuItem = NULL;

	// Detect double-byte characters by default
	m_DetectDBChar = true;

#ifdef USE_PROXY
	// Disable proxy by default
	m_ProxyType = 0;
	m_ProxyPort = 0;
#endif
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
	fprintf( fo, "DetectDBChar=%d\n", m_DetectDBChar );
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
	fprintf( fo, "HorizontalCenterAlign=%d\n", m_bHorizontalCenterAlign );
	fprintf( fo, "VerticalCenterAlign=%d\n", m_bVerticalCenterAlign );

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

#ifdef USE_PROXY
	fprintf( fo, "ProxyType=%d\n", m_ProxyType );
	fprintf( fo, "ProxyAddr=%s\n", m_ProxyAddr.c_str() );
	fprintf( fo, "ProxyPort=%d\n", m_ProxyPort );
	fprintf( fo, "ProxyUser=%s\n", m_ProxyUser.c_str() );
	fprintf( fo, "ProxyPass=%s\n", m_ProxyPass.c_str() );
#endif
}


