/////////////////////////////////////////////////////////////////////////////
// Name:        telnetcon.cpp
// Purpose:     Class dealing with telnet connections, parsing telnet commands.
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.16
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "telnetcon.h"
#endif

#include "telnetcon.h"
#include "telnetview.h"
#include "mainframe.h"

#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>

       
#include "stringutil.h"
#include "appconfig.h"

// class constructor
CTelnetCon::CTelnetCon(CTermView* pView, CSite& SiteInfo)
	: CTermData(pView), m_Site(SiteInfo)
{
    m_pBuf = m_pLastByte = m_pRecvBuf = NULL;
    m_pCmdLine = m_CmdLine;
	m_pCmdLine[0] = '\0';

	m_State = TS_CONNECTING;
	m_Duration = 0;
	m_IdleTime = 0;
	m_AutoLoginStage = 0;

	m_SockFD = -1;
	m_IOChannel = 0;
}

// class destructor
CTelnetCon::~CTelnetCon()
{
//	if(m_Conn)
//		g_object_unref(m_Conn);
	if(m_SockFD != -1)
	{
		shutdown(m_SockFD, 0);
		close(m_SockFD);
	}
}

gboolean CTelnetCon::OnSocket(GIOChannel *channel, GIOCondition type, CTelnetCon* _this)
{
	switch(type)
	{
	case G_IO_IN:
		_this->OnRecv();
		break;
	case G_IO_HUP:
		_this->OnClose();
		break;
	case G_IO_NVAL:
		_this->OnClose();
		return false;
	default:
		g_print("socket io error.\n");
	}
	return true;
}

// No description
bool CTelnetCon::Connect()
{
	string address;
	unsigned short port = 23;

	PreConnect( address, port );

	// If this site has auto-login settings, activate auto-login
	// and set it to stage 1, waiting for prelogin prompt or stage 2,
	// waiting for login prompt.
//	if( !m_Site.GetLogin().empty() && AppConfig.IsLoggedIn() )
//		m_AutoLoginStage = m_Site.GetPreLogin().empty() ? 2 : 1 ;
//	else
		m_AutoLoginStage = 0;

	CConnectThread* connect_thread = new CConnectThread(this, address, port);
	m_ConnectThreads.push_back( connect_thread );
	connect_thread->m_pThread = g_thread_create(
					(GThreadFunc)&CTelnetCon::ConnectThread, 
					connect_thread, true, NULL);

//	ConnectThread(connect_thread);
    return true;
}

// No description
void CTelnetCon::OnRecv()
{
	unsigned char buffer[4097];
	m_pRecvBuf = buffer;

	gsize rlen = 0;
	g_io_channel_read(m_IOChannel, (char*)m_pRecvBuf, sizeof(buffer)-1, &rlen);

	if(rlen == 0)
	{
		OnClose();
		return;
	}

    m_pRecvBuf[rlen] = '\0';
    m_pBuf = m_pRecvBuf;
    m_pLastByte = m_pRecvBuf + rlen;

    ParseReceivedData();

	if( m_AutoLoginStage > 0 )
		CheckAutoLogin();

	UpdateDisplay();
}

void CTelnetCon::OnConnect()
{
	m_State = TS_CONNECTED;
	m_IOChannel = g_io_channel_unix_new(m_SockFD);
	g_io_add_watch( m_IOChannel, 
		GIOCondition(G_IO_ERR|G_IO_NVAL|G_IO_HUP|G_IO_IN), (GIOFunc)OnSocket, this );
	g_io_channel_set_encoding(m_IOChannel, NULL, NULL);
	g_io_channel_set_buffered(m_IOChannel, false);
}

void CTelnetCon::OnClose()
{
	Close();
	//	if disconnected by the server too soon, reconnect automatically.
//	if( m_Duration < m_Site.m_AutoReconnect )
//		Connect();
}

/*
 * Parse received data, process telnet command 
 * and ANSI escape sequence.
 */
void CTelnetCon::ParseReceivedData()
{
    for( m_pBuf = m_pRecvBuf; m_pBuf < m_pLastByte; m_pBuf++ )
    {
		if( m_CmdLine[0] == TC_IAC )	// IAC, in telnet command mode.
		{
			ParseTelnetCommand();
			continue;
		}

        if( *m_pBuf == TC_IAC )    // IAC, in telnet command mode.
        {
            m_CmdLine[0] = TC_IAC;
            m_pCmdLine = &m_CmdLine[1];
            continue;
		}
		// *m_pBuf is not a telnet command, let genic terminal process it.
		CTermData::PutChar( *m_pBuf );
    }
}

// Process telnet command.
void CTelnetCon::ParseTelnetCommand()
{
    *m_pCmdLine = *m_pBuf;
    m_pCmdLine++;
	switch( m_CmdLine[1] )
	{
	case TC_WILL:
		{
			if( 3 > (m_pCmdLine-m_CmdLine) )
				return;
			char ret[]={TC_IAC,TC_DONT,*m_pBuf};
			switch(*m_pBuf)
			{
			case TO_ECHO:
			case TO_SUPRESS_GO_AHEAD:
				ret[1] = TC_DO;
				break;
			}
			SendString(ret, 3);
			break;
		}
	case TC_DO:
		{
			if( 3 > (m_pCmdLine-m_CmdLine) )
				return;
			char ret[]={TC_IAC,TC_WILL,*m_pBuf};
			switch(*m_pBuf)
			{
			case TO_TERMINAL_TYPE:
			case TO_NAWS:
				break;
			default:
				ret[1] = TC_WONT;
			}
			SendString(ret,3);
			if( TO_NAWS == *m_pBuf )	// Send NAWS
			{
				unsigned char naws[]={TC_IAC,TC_SB,TO_NAWS,0,80,0,24,TC_IAC,TC_SE};
				naws[3] = m_ColsPerPage >>8;	// higher byte
				naws[4] = m_ColsPerPage & 0xff; // lower byte
				naws[5] = m_RowsPerPage >> 8;	// higher byte
				naws[6] = m_RowsPerPage & 0xff; // lower byte
				SendString( (const char*)naws,sizeof(naws));
			}
			break;
		}
	case TC_WONT:
	case TC_DONT:
		if( 3 > (m_pCmdLine-m_CmdLine) )
			return;
		break;
	case TC_SB:	// sub negotiation
		if( *m_pBuf == TC_SE )	// end of sub negotiation
		{
			switch( m_CmdLine[2] )
			{
			case TO_TERMINAL_TYPE:
				{
					// Return terminal type.  2004.08.05 modified by PCMan.
					unsigned char ret_head[] = { TC_IAC, TC_SB, TO_TERMINAL_TYPE, TO_IS };
					unsigned char ret_tail[] = { TC_IAC, TC_SE };
					int ret_len = 4 + 2 + m_Site.m_TermType.length();
					unsigned char *ret = new unsigned char[ret_len];
					memcpy( ret, ret_head, 4);
					memcpy( ret + 4, m_Site.m_TermType.c_str(), m_Site.m_TermType.length() );
					memcpy( ret + 4 + m_Site.m_TermType.length() , ret_tail, 2);
					SendString( (const char*)ret, ret_len);
					delete []ret;
				}
			}
		}
		else
			return;	// prevent m_CmdLine from being cleard.
	}
	m_CmdLine[0] = '\0';
	m_pCmdLine = m_CmdLine;
}

void CTelnetCon::Disconnect()
{
	if( m_State != TS_CONNECTED )
		return;
	Close();
}

void CTelnetCon::OnTimer()
{
	if( m_State == TS_CLOSED )
		return;
	m_Duration++;
	m_IdleTime++;
//	Note by PCMan:
//	Here is a little trick.
//	Since we have increased m_IdleTime by 1, it's impossible for 
//	m_IdleTime to equal zero.
//	When 'Anti Idle' is disabled, m_Site.m_AntiIdle must = 0.
//	So m_Site.m_AntiIdle != m_IdleTimeand, and the following SendString() won't be called.
//	Hence we don't need to check if 'Anti Idle' is enabled or not.
	if( m_Site.m_AntiIdle == m_IdleTime )
	{
		//	2004.8.5 Added by PCMan.	Convert non-printable control characters.
//		string aistr = UnEscapeStr( m_Site.m_AntiIdleStr.c_str() );
//		SendString( aistr.c_str(), aistr.length() );
	}
	//	When SendString() is called, m_IdleTime is set to 0 automatically.
}


//	Virtual function called from parent class to let us determine
//	whether to beep or show a visual indication instead.
void CTelnetCon::Bell()
{
	((CTelnetView*)m_pView)->GetParentFrame()->OnConBell((CTelnetView*)m_pView);
}


void CTelnetCon::CheckAutoLogin()
{
	int last_line = m_FirstLine + m_RowsPerPage;
	const char* prompts[] = {
		NULL,	//	Just used to increase array indices by one.
		m_Site.GetPreLoginPrompt().c_str(),	//	m_AutoLoginStage = 1
		m_Site.GetLoginPrompt().c_str(),	//	m_AutoLoginStage = 2
		m_Site.GetPasswdPrompt().c_str() };	//	m_AutoLoginStage = 3

	bool prompt_found = false;
	for( int line = m_FirstLine; line < last_line; line++ )
	{
		if( strstr(m_Screen[line], prompts[m_AutoLoginStage] ) )
		{
			prompt_found = true;
			break;
		}
	}

	if( prompt_found )
	{
		const char* responds[] = {
			NULL,	//	Just used to increase array indices by one.
			m_Site.GetPreLogin().c_str(),	//	m_AutoLoginStage = 1
			m_Site.GetLogin().c_str(),	//	m_AutoLoginStage = 2
			m_Site.GetPasswd().c_str(),	//	m_AutoLoginStage = 3
			""	//	m_AutoLoginStage = 4, turn off auto-login
			};

		string respond = responds[m_AutoLoginStage];
		respond += m_Site.GetCRLF();
		SendString(respond);

		if( !responds[ ++m_AutoLoginStage ][0] )	// Go to next stage.
		{
			m_AutoLoginStage = 0;	// turn off auto-login after all stages end.
			respond = m_Site.GetPostLogin();	// Send post-login string
			if( respond.length() > 0 )
			{
				// Unescape all control characters.
//				UnEscapeStr(respond);
				SendString(respond);
			}
		}
	}
}

void CTelnetCon::SendString(string str)
{
//	str.Replace( "\n", m_Site.GetCRLF(), true);
	SendString(str.c_str(), str.length());
}


void CTelnetCon::PreConnect(string& address, unsigned short& port)
{
	m_Duration = 0;
	m_IdleTime = 0;
	m_State = TS_CONNECTING;

	int p = m_Site.m_URL.find(':',true);
	if( p >=0 )		// use port other then 23;
	{
		port = (unsigned short)atoi(m_Site.m_URL.c_str()+p+1);
		address = m_Site.m_URL.substr(0, p);
	}
	else
		address = m_Site.m_URL;
}

int CTelnetCon::Send(void *buf, int len)
{
	if( m_IOChannel && m_State & TS_CONNECTED )
	{
		gsize wlen = 0;
		g_io_channel_write(m_IOChannel, (char*)buf, len, &wlen);
		return wlen;
	}
	return 0;
}


void CTelnetCon::Connect(const struct sockaddr *serv_addr, socklen_t addrlen)
{
	m_SockFD = socket(PF_INET, SOCK_STREAM, 0);

//	bind( m_SockFD, serv_addr, addrlen );
	connect( m_SockFD, serv_addr, addrlen );
}

vector<CConnectThread*> CTelnetCon::m_ConnectThreads;

void CTelnetCon::ResolveHostName(string name, int port)
{

}

gpointer CTelnetCon::ConnectThread(CConnectThread* data)
{
	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(data->m_Port);

	in_addr addr;
	if( ! inet_aton(data->m_Address.c_str(), &addr) )
	{
		hostent* host = gethostbyname(data->m_Address.c_str());
		if( !host )
			return NULL;
		addr = *(in_addr*)host->h_addr_list[0];
	}
	if( addr.s_addr == INADDR_NONE )
		return NULL;

	sock_addr.sin_addr = addr;

	data->m_pCon->Connect((sockaddr*)&sock_addr, sizeof(sock_addr));
	data->m_IsFinished = true;

	return data;
}


void CTelnetCon::Close()
{
	m_State = TS_CLOSED;

	if( m_SockFD != -1 )
	{
		shutdown( m_SockFD, 0 );
		close( m_SockFD );
		g_io_channel_shutdown(m_IOChannel, true, NULL);
		g_io_channel_unref(m_IOChannel);
		m_IOChannel = NULL;
	}

}

gboolean CTelnetCon::OnMainIdle(gpointer data)
{
	vector<CConnectThread*>::iterator it;
	for( it = m_ConnectThreads.begin(); it != m_ConnectThreads.end(); )
	{
		CConnectThread* thread = *it;
		if( thread->m_IsFinished )
		{
			CTelnetCon* pCon = thread->m_pCon;
			pCon->OnConnect();
			delete thread;
			it = m_ConnectThreads.erase(it);
//			g_print("delete thread\n");
		}
		else
			++it;
	}
	usleep(100);
	return true;
}
