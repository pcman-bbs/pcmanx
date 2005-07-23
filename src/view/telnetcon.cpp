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

#if !defined(MOZ_PLUGIN)
#include "mainframe.h"

#ifdef USE_NOTIFIER
#include "notifier/api.h"
#endif

#endif /* !defined(MOZ_PLUGIN) */

#include <string.h>
#include <glib/gi18n.h>

#include "stringutil.h"

#if !defined(MOZ_PLUGIN)
#include "appconfig.h"
#endif /* !defined(MOZ_PLUGIN) */

// socket related headers
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>

// pseudo tty headers
#ifdef USING_LINUX
#include <pty.h>
#endif
#include <utmp.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef USING_FREEBSD
#include <sys/ioctl.h>
#include <termios.h>
#include <libutil.h>
#endif

#define RECV_BUF_SIZE (4097)


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
	m_IOChannelID = 0;
	m_Pid = 0;

	m_BellTimeout = 0;
	m_IsLastLineModified = false;
}
GThreadPool* CTelnetCon::m_ThreadPool = NULL;

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

#ifdef USE_EXTERNAL
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
#endif
	{
		CConnectThread* connect_thread = new CConnectThread(this, address, port);
		m_ConnectThreads.push_back( connect_thread );
		if( !m_ThreadPool )
		{
			m_ThreadPool = g_thread_pool_new((GFunc)&CTelnetCon::ConnectThread, 
											NULL, 
											MAX_CONCURRENT_CONS, 
											FALSE, 
											NULL);
//			g_print("pool created, %x\n", m_ThreadPool);
		}
		g_thread_pool_push(m_ThreadPool, connect_thread, NULL);
//		g_print("thread pushed, %x\n", connect_thread);
	}

    return true;
}

// No description
bool CTelnetCon::OnRecv()
{
	static unsigned char recv_buf[RECV_BUF_SIZE];
	m_pRecvBuf = recv_buf;

	if( !m_IOChannel || m_SockFD == -1 )
		return false;

	gsize rlen = 0;
	g_io_channel_read(m_IOChannel, (char*)m_pRecvBuf, (RECV_BUF_SIZE - 1), &rlen);

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
#if !defined(MOZ_PLUGIN)
		((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConConnect((CTelnetView*)m_pView);
#endif
		m_IOChannel = g_io_channel_unix_new(m_SockFD);
		m_IOChannelID = g_io_add_watch( m_IOChannel, 
				GIOCondition(G_IO_ERR|G_IO_HUP|G_IO_IN), (GIOFunc)OnSocket, this );
		g_io_channel_set_encoding(m_IOChannel, NULL, NULL);
		g_io_channel_set_buffered(m_IOChannel, false);
	}
	else
	{
//		g_print("connection failed.\n");
		m_State = TS_CLOSED;
		Close();
#if !defined(MOZ_PLUGIN)
		((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConClose((CTelnetView*)m_pView);
#endif
		const char failed_msg[] = "Unable to connect.";
		memcpy( m_Screen[0], failed_msg, sizeof(failed_msg) );
#if !defined(MOZ_PLUGIN)
		if( GetView()->GetParentFrame()->GetCurView() == m_pView )
		{
			for( int col = 0; col < sizeof(failed_msg); )
				col += m_pView->DrawChar( 0, col, 0 );
		}
#endif
	}
}

void CTelnetCon::OnClose()
{
	m_State = TS_CLOSED;
	Close();
#if !defined(MOZ_PLUGIN)
	((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConClose((CTelnetView*)m_pView);
#endif
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
#if !defined(MOZ_PLUGIN)
	((CTelnetView*)m_pView)->GetParentFrame()->OnTelnetConBell((CTelnetView*)m_pView);
#endif
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

void CTelnetCon::ConnectThread( CConnectThread* data, gpointer _data )
{
//	g_print("thread entered: %x\n", data);
	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(data->m_Port);

	in_addr addr;
	addr.s_addr = INADDR_NONE;

	G_LOCK_DEFINE (gethostbyname_mutex);
	if( ! inet_aton(data->m_Address.c_str(), &addr) )
	{
		G_LOCK( gethostbyname_mutex );
//		g_print("thread locked: %x\n", data);
		hostent* host = gethostbyname(data->m_Address.c_str());

		if( host )
			addr = *(in_addr*)host->h_addr_list[0];

		G_UNLOCK( gethostbyname_mutex );
//		g_print("thread unlocked: %x\n", data);

/*		if( (addr.s_addr == INADDR_NONE) && data->m_DNSTry > 0 )
		{
			--data->m_DNSTry;	// retry
			if( m_ThreadPool )	// Theoraticallly, this mustn't be NULL, but...
			{
				g_thread_pool_push(m_ThreadPool, data, NULL);
				return;
			}

		}
*/
	}

	if( data->m_pCon && addr.s_addr != INADDR_NONE )
	{
//		g_print("thread connect: %x\n", data);
		sock_addr.sin_addr = addr;

		int sock_fd;
		for( int i =0; i < 3 ; ++i )
		{
			sock_fd = socket(PF_INET, SOCK_STREAM, 0);
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
		// Since 0 means success and all known error codes are > 0, 
		// I use error code that < 0 to mean host not found.

//	g_print("thread connected: %x, code=%d\n", data, data->m_Code);
	g_idle_add((GSourceFunc)OnMainIdle, data);
//	g_print("thread exit: %x\n", data);
}


void CTelnetCon::Close()
{
	m_State = TS_CLOSED;

	if( m_IOChannel )
	{
		g_source_remove(m_IOChannelID);
		m_IOChannelID = 0;
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

	if( m_ThreadPool  )
	{
		g_thread_pool_stop_unused_threads();
//		g_print("on connect, pending=%d\n", g_thread_pool_unprocessed(m_ThreadPool) );
		if( 0 == g_thread_pool_unprocessed(m_ThreadPool) )
		{
			g_thread_pool_free(m_ThreadPool, TRUE, FALSE);
			m_ThreadPool = NULL;
//			g_print("pool freed\n");
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
	if(m_ThreadPool)
	{
//		g_print("on cleanup, pending=%d\n", g_thread_pool_unprocessed(m_ThreadPool) );
		g_thread_pool_free(m_ThreadPool, FALSE, TRUE);
	}
	// If g_thread_pool_free is called with immediate=TRUE, 
	// the funtion will hang and never return even there is
	// no pending task.  After lots of tests, unfortunately, I 
	// found this is a bug of glib.

	m_ThreadPool = NULL;
	vector<CConnectThread*>::iterator it;
	for( it = m_ConnectThreads.begin(); it != m_ConnectThreads.end(); ++it)
	{
		g_idle_remove_by_data(*it);
		CConnectThread* thread = *it;
		delete *it;
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

#if !defined(MOZ_PLUGIN)

#ifdef USE_NOTIFIER

void popup_win_clicked(GtkWidget* widget, CTelnetCon* con)
{
	g_print("popup clicked\n");
	CMainFrame* mainfrm = con->GetView()->GetParentFrame();
	mainfrm->SwitchToCon(con);
	gtk_widget_destroy( gtk_widget_get_parent(widget) );
	gtk_window_present(GTK_WINDOW(mainfrm->m_Widget));
	return;
}

#endif

#endif /* !defined(MOZ_PLUGIN) */

// When new incoming message is detected, this function gets called.
void CTelnetCon::OnNewIncomingMessage(char* line)
{
#if !defined(MOZ_PLUGIN)

#ifdef USE_NOTIFIER
	if ( !AppConfig.PopupNotifier || !*line )
		return;

	CMainFrame* mainfrm = ((CTelnetView*)m_pView)->GetParentFrame();
	if( mainfrm->IsActivated() && mainfrm->GetCurCon() == this )
		return;

	/* We need to convert the incoming message into UTF-8 encoding from
	 * the original one.
	 */
	gsize l;
	gchar *utf8_text = g_convert(
		line, strlen(line), 
		"UTF-8", m_Site.m_Encoding.c_str(), 
		NULL, &l, NULL);

	gchar **column = g_strsplit(utf8_text, " ", 2);
	GtkWidget* popup_win = popup_notifier_notify(
		g_strdup_printf("%s - %s",
			m_Site.m_Name.c_str(),
			g_strchomp(column[0])),
		g_strchomp(column[1]),
		m_pView->m_Widget, 
		G_CALLBACK(popup_win_clicked), 
		this);
	g_strfreev(column);
	g_free(utf8_text);
#endif

#endif /* !defined(MOZ_PLUGIN) */
}
