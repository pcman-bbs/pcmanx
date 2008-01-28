/* -- vi: set ts=4 sw=4: --
 * ===========================================================================
 *
 *       Filename:  proxy.c
 *
 *    Description:  Simple implementation of SOCKS client
 *
 *        Version:  1.0
 *        Created:  27/01/08 05:23:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Xia (jasonxh), jasonxh@gmail.com
 *        Company:  
 *
 * ===========================================================================
 */

#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "proxy.h"

#define SOCKS_CMD_CONNECT        0x01

#define SOCKS4_VER               0x04
#define SOCKS4_CMD_CONNECT       SOCKS_CMD_CONNECT
#define SOCKS4_REP_SUCCESS       0x5a

#define SOCKS5_VER               0x05
#define SOCKS5_AUTH_NONE         0x00
#define SOCKS5_AUTH_PASS         0x02
#define SOCKS5_AUTH_UNSUPPORTED  0xff
#define SOCKS5_CMD_CONNECT       SOCKS_CMD_CONNECT
#define SOCKS5_ATYP_IP4          0x01
#define SOCKS5_ATYP_DOMAINNAME   0x03
#define SOCKS5_ATYP_IP6          0x04
#define SOCKS5_REP_SUCCESS       0x00


static int hs_socks4(int sockfd, const char *user, const char *pass);
static int hs_socks5(int sockfd, const char *user, const char *pass);
static int connect_socks4(int sockfd, const struct sockaddr_in *serv_addr
		, const char *user);
static int connect_socks5(int sockfd, const struct sockaddr_in *serv_addr);

/* 
 * ===  FUNCTION  ============================================================
 *         Name:  proxy_connect
 *  Description:  Connect to a proxy server and initiate a CONNECT request.
 *       Return:  Socket to communicate with server. -1 for error.
 * ===========================================================================
 */
int proxy_connect(const struct sockaddr_in *serv_addr
		, int proxy_type, const struct sockaddr_in *proxy_addr
		, const char *user, const char *pass)
{
	int sockfd;
   	
	errno = 0;  /* reset errno just in case */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return -1;
	if (connect(sockfd, (struct sockaddr*)proxy_addr, sizeof(struct sockaddr_in)) != 0)
		goto release;
	
	switch (proxy_type)
	{
		case PROXY_SOCKS4:
			/* handshake */
			if (hs_socks4(sockfd, user, pass) != 0)
				goto release;
			/* initiate connect request */
			if (connect_socks4(sockfd, serv_addr, user) != 0)
				goto release;
			break;
		case PROXY_SOCKS5:
			/* handshake */
			if (hs_socks5(sockfd, user, pass) != 0)
				goto release;
			/* initiate connect request */
			if (connect_socks5(sockfd, serv_addr) != 0)
				goto release;
			break;
		default:
			goto release;
	}

	return sockfd;

release:
	close(sockfd);
	return -1;
}

/* 
 * ===  FUNCTION  ============================================================
 *         Name:  hs_socks4
 *  Description:  Handshake with a SOCKS4 server
 * ===========================================================================
 */
static int hs_socks4(int sockfd, const char *user, const char *pass)
{
	/* nothing to do */
	return 0;
}

/* 
 * ===  FUNCTION  ============================================================
 *         Name:  hs_socks5
 *  Description:  Handshake with a SOCKS5 server
 * ===========================================================================
 */
static int hs_socks5(int sockfd, const char *user, const char *pass)
{
	unsigned char buf[16];

	/* socks version */
	buf[0] = SOCKS5_VER;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* # of supported auth methods */
	buf[0] = 2;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* supported auth methods */
	buf[0] = SOCKS5_AUTH_NONE;
	buf[1] = SOCKS5_AUTH_PASS;
	if (write(sockfd, buf, 2) != 2) return -1;

	/* socks version */
	if (read(sockfd, buf, 1) != 1) return -1;
	if (buf[0] != SOCKS5_VER)
		return -1;
	/* chosen auth method */
	if (read(sockfd, buf, 1) != 1) return -1;
	switch (buf[0])
	{
		case SOCKS5_AUTH_NONE: /* no auth needed */
			break;
		case SOCKS5_AUTH_PASS: /* user/pass auth */
			if (user == NULL || pass == NULL)
				return -1;
			buf[0] = 0x01;
			if (write(sockfd, buf, 1) != 1) return -1;
			/* username */
			buf[0] = strlen(user);
			if (write(sockfd, buf, 1) != 1) return -1;
			if (write(sockfd, user, buf[0]) != buf[0]) return -1;
			/* password */
			buf[0] = strlen(pass);
			if (write(sockfd, buf, 1) != 1) return -1;
			if (write(sockfd, pass, buf[0]) != buf[0]) return -1;

			/* auth result */
			if (read(sockfd, buf, 2) != 2) return -1;
			if (buf[1] != 0x00)
				return -1;
			break;
		default:               /* unsupported auth method */
			return -1;
	}

	return 0;
}

/* 
 * ===  FUNCTION  ============================================================
 *         Name:  connect_socks4
 *  Description:  Initiate a CONNECT request to SOCKS4 server
 * ===========================================================================
 */
static int connect_socks4(int sockfd, const struct sockaddr_in *serv_addr
		, const char *user)
{
	unsigned char buf[16], len;

	/* socks version */
	buf[0] = SOCKS4_VER;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* connect command */
	buf[0] = SOCKS4_CMD_CONNECT;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* port */
	if (write(sockfd, &serv_addr->sin_port, 2) != 2) return -1;
	/* address */
	if (write(sockfd, &serv_addr->sin_addr, 4) != 4) return -1;
	/* user */
	len = strlen(user) + 1;
	if (write(sockfd, user, len) != len) return -1;

	/* null byte */
	if (read(sockfd, buf, 1) != 1) return -1;
	/* reply */
	if (read(sockfd, buf, 1) != 1) return -1;
	if (buf[0] != SOCKS4_REP_SUCCESS)
		return -1;
	/* ignore */
	if (read(sockfd, buf, 6) != 6) return -1;

	return 0;
}

/* 
 * ===  FUNCTION  ============================================================
 *         Name:  connect_socks5
 *  Description:  Initiate a CONNECT command to SOCKS5 server
 * ===========================================================================
 */
static int connect_socks5(int sockfd, const struct sockaddr_in *serv_addr)
{
	unsigned char buf[255], len;

	/* socks version */
	buf[0] = SOCKS5_VER;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* connect command */
	buf[0] = SOCKS5_CMD_CONNECT;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* reserved */
	buf[0] = 0x00;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* address type */
	buf[0] = SOCKS5_ATYP_IP4;
	if (write(sockfd, buf, 1) != 1) return -1;
	/* address */
	if (write(sockfd, &serv_addr->sin_addr, 4) != 4) return -1;
	/* port */
	if (write(sockfd, &serv_addr->sin_port, 2) != 2) return -1;

	/* socks version */
	if (read(sockfd, buf, 1) != 1) return -1;
	if (buf[0] != SOCKS5_VER)
		return -1;
	/* reply */
	if (read(sockfd, buf, 1) != 1) return -1;
	if (buf[0] != SOCKS5_REP_SUCCESS)
		return -1;
	/* reserved */
	if (read(sockfd, buf, 1) != 1) return -1;
	/* address type */
	if (read(sockfd, buf, 1) != 1) return -1;
	switch (buf[0])
	{
		case SOCKS5_ATYP_IP4:
			/* ipv4 address */
			if (read(sockfd, buf, 4) != 4) return -1;
			break;
		case SOCKS5_ATYP_DOMAINNAME:
			/* domainname length */
			if (read(sockfd, &len, 1) != 1) return -1;
			/* domainname */
			if (read(sockfd, buf, len) != len) return -1;
			break;
		case SOCKS5_ATYP_IP6:
			/* ipv6 address */
			if (read(sockfd, buf, 16) != 16) return -1;
			break;
		default:
			return -1;
	}
	/* port */
	if (read(sockfd, buf, 2) != 2) return -1;

	return 0;
}
