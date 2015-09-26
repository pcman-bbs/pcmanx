/////////////////////////////////////////////////////////////////////////////
// Name:        telnetcon.h
// Purpose:     Class dealing with telnet connections, parsing telnet commands.
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.16
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////


#ifndef TELNETCON_H
#define TELNETCON_H

#ifdef __GNUG__
  #pragma interface "telnetcon.h"
#endif

#include <gtk/gtk.h>

#include "termdata.h"
#include "site.h"

#if !defined(MOZ_PLUGIN)

#ifdef USE_NANCY
#include "nancy_bot/api.h"
#endif

#endif /* !defined(MOZ_PLUGIN) */

#include <string>
#include <vector>
#include <list>

#include <sys/types.h>
#include <netinet/in.h>

using namespace std;

// Telnet commands
#define TC_SE                   (unsigned char)240
#define TC_NOP                  (unsigned char)241
#define TC_DATA_MARK            (unsigned char)242
#define TC_BREAK                (unsigned char)243
#define TC_INTERRUPT_PROCESS    (unsigned char)244
#define TC_ABORT_OUTPUT         (unsigned char)245
#define TC_ARE_YOU_THERE        (unsigned char)246
#define TC_ERASE_CHARACTER      (unsigned char)247
#define TC_ERASE_LINE           (unsigned char)248
#define TC_GO_AHEAD	            (unsigned char)249
#define TC_SB                   (unsigned char)250
// Option commands                             
#define TC_WILL                 (unsigned char)251
#define TC_WONT                 (unsigned char)252
#define TC_DO                   (unsigned char)253
#define TC_DONT                 (unsigned char)254
#define TC_IAC                  (unsigned char)255
                                 
// Telnet options)               
#define TO_ECHO                 (unsigned char)1
#define TO_SUPRESS_GO_AHEAD     (unsigned char)3
#define TO_TERMINAL_TYPE        (unsigned char)24
#define	TO_IS                   (unsigned char)0
#define	TO_SEND                 (unsigned char)1
#define TO_NAWS                 (unsigned char)31

/*
 * A class for Telnet Connections, used to store screen buffer of 
 * every connections and their sockets, etc.
 */

class CDNSRequest;
class CTelnetView;
class CTelnetCon : public CTermData
{
public:
	virtual int Send( void* buf, int len );
	virtual void Bell();	// called from CTermData to process beep.
	void OnTimer();
	static gboolean OnSocket(GIOChannel *channel, GIOCondition type, CTelnetCon* _this);
	CTelnetView* GetView(){	return (CTelnetView*)m_pView;	}

	// A flag used to indicate connecting state;
	enum{TS_CONNECTING, TS_CONNECTED, TS_CLOSED} m_State;

	void Disconnect();
	// class constructor
	CTelnetCon(CTermView* pView, CSite& SiteInfo);
	// class destructor
	~CTelnetCon();
	// No description
	virtual bool Connect();
    void Reconnect();
	// Connecting duration
	unsigned int m_Duration;
	// Idle time, during which the user doesn't have any key input
	unsigned int m_IdleTime;

	CSite m_Site;
    static list<CDNSRequest*> m_DNSQueue;

	virtual void OnClose();
	void OnConnect(int code);
	bool OnRecv();

	// Parse received data, process telnet command, and ANSI escape sequence.
	void ParseReceivedData();

	// Parse telnet command.
	inline void ParseTelnetCommand();

	void SendRawString(const char* pdata, int len)	{	Send( (void*)pdata, len);	}
	void SendUnEscapedString(string str);
	void SendString(string str);
 	
	bool IsValid(){	return m_SockFD >= 0;	}
	bool IsClosed(){	return (m_State & TS_CLOSED);	}

	bool IsBellReceived(){	return (m_BellTimeout != 0);	}
    void Close();

	static void Cleanup();
    static bool OnBellTimeout( CTelnetCon* _this );
    void OnNewIncomingMessage(const char* line);

	static void SetSocketTimeout(int timeout){	m_SocketTimeout=timeout;	}
    bool DetectDBChar(){	return m_Site.m_DetectDBChar;   }
    void ConnectAsync();

    static void Init();
#if !defined(MOZ_PLUGIN)
#ifdef USE_NANCY
    void set__UseNancy( bool usenancy )
    {
	    use_nancy = usenancy;
    }

    static void set__OpenConnectionWithNancySupport( bool nancy_support )
    {
	    with_nancy_support = nancy_support;
    }

    bool get__UseNancy()
    {
	    return use_nancy;
    }

    static bool get__OpenConnectionWithNancySupport()
    {
	    return with_nancy_support;
    }
#endif
#endif /* !defined(MOZ_PLUGIN) */


protected:

#if !defined(MOZ_PLUGIN)
#ifdef USE_NANCY
    	NancyBot *bot;
	static bool with_nancy_support;
	bool use_nancy;
#endif
#endif /* !defined(MOZ_PLUGIN) */

	GIOChannel* m_IOChannel;
	guint m_IOChannelID;

	// Buffer to receive socket incoming data
	unsigned char* m_pRecvBuf;
	unsigned char* m_pBuf;
	unsigned char* m_pLastByte;
	enum AUTO_LOGIN_STATE
	{
		ALS_OFF=0, 
		ALS_PRELOGIN=1, 
		ALS_LOGIN=2, 
		ALS_PASSWD=3,
		ALS_END=4
	};
	unsigned int m_AutoLoginStage;	// 0 means turn off auto-login.

	// Client socket
    int m_SockFD;
	int m_Pid;
protected:
	guint m_BellTimeout;
	bool m_IsLastLineModified;
    static GThread* m_DNSThread;
    string m_PreLoginPrompt;
    string m_LoginPrompt;
    string m_PasswdPrompt;
    static int m_SocketTimeout;
    in_addr m_InAddr;
	unsigned short m_Port;
	void PreConnect(string& address, unsigned short& port);
    void CheckAutoLogin(int row);
    void SendStringAsync(string str);
    static void DoDNSLookup( CDNSRequest* data );
    void OnLineModified(int row);
    static gboolean OnDNSLookupEnd(CTelnetCon* _this);
    static gboolean OnConnectCB(GIOChannel *channel, GIOCondition type, CTelnetCon* _this);
    static void ProcessDNSQueue(gpointer unused);
private:
    static GMutex* m_DNSMutex;
private:
    static bool OnProcessDNSQueueExit(gpointer unused);
};

class CDNSRequest
{
public:
	CDNSRequest(CTelnetCon* con, string address, int port) 
		: m_pCon(con), m_Address(address), m_Running(false)
	{
	}
	CTelnetCon* m_pCon;
	string m_Address;
	bool m_Running;
};



#endif // TELNETCON_H

