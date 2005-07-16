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

#include <string.h>

#include "stringutil.h"
#include "appconfig.h"

// socket related headers
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>

// pseudo tty headers
#include <pty.h>
#include <utmp.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>


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
	m_Pid = 0;

	m_BellTimeout = 0;
	m_IsLastLineModified = false;
}

// class destructor
CTelnetCon::~CTelnetCon()
{
	Close();

	vector<CConnectThread*>::iterator it;
	for( it = m_ConnectThreads.begin(); it != m_ConnectThreads.end(); ++it)
	{
		CConnectThread* thread = *it;
		if( thread->m_pCon == this )
		{
			thread->m_pCon = NULL;
			break;
		}
	}

	if( m_BellTimeout )
		g_source_remove( m_BellTimeout );

}

gboolean CTelnetCon::OnSocket(GIOChannel *channel, GIOCondition type, CTelnetCon* _this)
{
	bool ret = false;
	if( type & G_IO_IN )
		ret = _this->OnRecv();
	if( type & G_IO_HUP )
	{
		_this->OnClose();
		ret = false;
	}
	return ret;
}



// No description
bool CTelnetCon::Connect()
{
	m_State = TS_CONNECTING;

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

	// Run external program to handle connection.
	if( m_Site.m_UseExternalTelnet || m_Site.m_UseExternalSSH )
	{
		// Suggestion from kyl <kylinx@gmail.com>
		// Call forkpty() to use pseudo terminal and run an external program.
		const char* prog = m_Site.m_UseExternalSSH ? "ssh" : "telnet";
		setenv("TERM", m_Site.m_TermType.c_str() , 1);
		// Current terminal emulation is buggy and only suitable for BBS browsing.
		// Both xterm or vt??? terminal emulation has not been fully implemented.
		m_Pid = forkpty (& m_SockFD, NULL, NULL, NULL );
		if ( m_Pid == 0 )
		{
			// Child Process;
			if( m_Site.m_UseExternalSSH )
				execlp ( prog, prog, address.c_str(), NULL ) ;
			else
				execlp ( prog, prog, "-8", address.c_str(), NULL ) ;
		}
		else
		{
			// Parent process
		}
		OnConnect(0);
	}
	else	// Use built-in telnet command handler
	{
		CConnectThread* connect_thread = new CConnectThread(this, address, port);
		m_ConnectThreads.push_back( connect_thread );
		connect_thread->m_pThread = g_thread_create(
						(GThreadFunc)&CTelnetCon::ConnectThread, 
						connect_thread, true, NULL);
	}

    return true;
}

// No description
bool CTelnetCon::OnRecv()
{
	if( !m_IOChannel || m_SockFD == -1 )
		return false;

	unsigned char buffer[4097];
	m_pRecvBuf = buffer;

	gsize rlen = 0;
	g_io_channel_read(m_IOChannel, (char*)m_pRecvBuf, sizeof(buffer)-1, &rlen);

	if(rlen == 0 && !(m_State & TS_CLOSED) )
	{
		OnClose();
		return false;
	}

    m_pRecvBuf[rlen] = '\0';
    m_pBuf = m_pRecvBuf;
    m_pLastByte = m_pRecvBuf + rlen;
//printf("recv: %s", m_pRecvBuf);
    ParseReceivedData();

	if( m_AutoLoginStage > 0 )
		CheckAutoLogin();

	UpdateDisplay();

//	((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConRecv((CTelnetView*)m_pView);
	return true;
}

void CTelnetCon::OnConnect(int code)
{
	if( 0 == code )
	{
		m_State = TS_CONNECTED;
		((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConConnect((CTelnetView*)m_pView);
		m_IOChannel = g_io_channel_unix_new(m_SockFD);
		g_io_add_watch( m_IOChannel, 
			GIOCondition(G_IO_ERR|G_IO_HUP|G_IO_IN), (GIOFunc)OnSocket, this );
		g_io_channel_set_encoding(m_IOChannel, NULL, NULL);
		g_io_channel_set_buffered(m_IOChannel, false);
	}
	else
	{
		g_print("connection failed.\n");
		OnClose();
	}
}

void CTelnetCon::OnClose()
{
	m_State = TS_CLOSED;
	Close();
	((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConClose((CTelnetView*)m_pView);

	//	if disconnected by the server too soon, reconnect automatically.
	if( m_Duration < m_Site.m_AutoReconnect )
		Reconnect();
}

/*
 * Parse received data, process telnet command 
 * and ANSI escape sequence.
 */
void CTelnetCon::ParseReceivedData()
{
    for( m_pBuf = m_pRecvBuf; m_pBuf < m_pLastByte; m_pBuf++ )
    {
		if( 0 == m_Pid ) // No external program.  Handle telnet commands ourselves.
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
//		g_print("AntiIdle: %s\n", m_Site.m_AntiIdleStr.c_str() );
		string aistr = UnEscapeStr( m_Site.m_AntiIdleStr.c_str() );
		SendString( aistr.c_str(), aistr.length() );
	}
	//	When SendString() is called, m_IdleTime is set to 0 automatically.
}


//	Virtual function called from parent class to let us determine
//	whether to beep or show a visual indication instead.
void CTelnetCon::Bell()
{
	((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConBell((CTelnetView*)m_pView);
	if( m_BellTimeout )
		g_source_remove( m_BellTimeout );

	m_BellTimeout = g_timeout_add( 500, (GSourceFunc)CTelnetCon::OnBellTimeout, this );
}


bool CTelnetCon::OnBellTimeout( CTelnetCon* _this )
{
//	g_print("on bell timer\n");
	if( _this->m_IsLastLineModified )
	{
		char* line = _this->m_Screen[ _this->m_RowsPerPage-1 ];
		_this->OnNewIncomingMessage( line );
		_this->m_IsLastLineModified = false;
	}
	_this->m_BellTimeout = 0;
	return false;
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
	string str2;
	const char* crlf = m_Site.GetCRLF();
	for( const char* pstr = str.c_str(); *pstr; ++pstr )
		if( *pstr == '\n' )
			str2 += crlf;
		else
			str2 += *pstr;
	SendString(str2.c_str(), str2.length());
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

		if( wlen > 0 )
			m_IdleTime = 0;	// Since data has been sent, we are not idle.

		return wlen;
	}
	return 0;
}


vector<CConnectThread*> CTelnetCon::m_ConnectThreads;

gpointer CTelnetCon::ConnectThread(CConnectThread* data)
{
	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(data->m_Port);

	in_addr addr;
	addr.s_addr = INADDR_NONE;

	if( ! inet_aton(data->m_Address.c_str(), &addr) )
	{
		hostent* host = gethostbyname(data->m_Address.c_str());
		if( host )
			addr = *(in_addr*)host->h_addr_list[0];
	}
	if( addr.s_addr != INADDR_NONE )
	{
		sock_addr.sin_addr = addr;

		int sock_fd;
		for( int i =0; i < 3 ; ++i )
		{
			sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	//		bind( sock_fd, serv_addr, addrlen );
			if( 0 == (data->m_Code = connect( sock_fd, (sockaddr*)&sock_addr, sizeof(sock_addr) )) )
				break;
			close(sock_fd);
			g_thread_yield();
		}
		if( data->m_pCon )
			data->m_pCon->m_SockFD = sock_fd;
	}
	else
		data->m_Code = -1;

	g_idle_add((GSourceFunc)OnMainIdle, data);

	return data;
}


void CTelnetCon::Close()
{
	m_State = TS_CLOSED;

	if( m_IOChannel )
	{
		g_io_channel_shutdown(m_IOChannel, true, NULL);
		g_io_channel_unref(m_IOChannel);
		m_IOChannel = NULL;
	}

	if( m_SockFD != -1 )
	{
		if( m_Pid )
		{
			int kill_ret = kill( m_Pid, 1 );	// SIG_HUP Is this correct?
			int status = 0;
			pid_t wait_ret = waitpid(m_Pid, &status, 0);
//			g_print("pid=%d, kill=%d, wait=%d\n", m_Pid, kill_ret, wait_ret);
			m_Pid = 0;
		}
		close( m_SockFD );
		m_SockFD = -1;
	}
}

gboolean CTelnetCon::OnMainIdle(CConnectThread* data)
{
	vector<CConnectThread*>::iterator it;
	for( it = m_ConnectThreads.begin(); it != m_ConnectThreads.end(); ++it)
	{
		if( *it == data )
		{
			m_ConnectThreads.erase(it);
//			g_print("delete thread from vector\n");
			break;
		}
	}

	CTelnetCon* pCon = data->m_pCon;
	if( pCon )
		pCon->OnConnect(data->m_Code);
	delete data;
//	g_print("delete thread obj\n");
	return false;
}

void CTelnetCon::Cleanup()
{
	vector<CConnectThread*>::iterator it;
	for( it = m_ConnectThreads.begin(); it != m_ConnectThreads.end(); ++it)
	{
		g_idle_remove_by_data(*it);
		CConnectThread* thread = *it;
		g_thread_join(thread->m_pThread);
		m_ConnectThreads.erase(it);
		delete *it;
//		g_print("delete thread\n");
		break;
	}
}

void CTelnetCon::Reconnect()
{
	ClearScreen(2);
	m_CaretPos.x = m_CaretPos.y = 0;
	Connect();
}

void CTelnetCon::OnLineModified(int row)
{
    /// @todo implement me
//	g_print("line %d is modified\n", row);
	if( row == (m_RowsPerPage-1) )	// If last line is modified
		m_IsLastLineModified = true;
}

// When new incoming message is detected, this function gets called.
void CTelnetCon::OnNewIncomingMessage(char* line)
{
    /// @todo implement me
//	g_print("New Message: %s\n", line);
}
