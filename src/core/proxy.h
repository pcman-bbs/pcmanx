/* -- vi: set ts=4 sw=4: --
 * ===========================================================================
 *
 *       Filename:  proxy.h
 *
 *    Description:  Simple implementation of SOCKS client
 *
 *        Version:  1.0
 *        Created:  27/01/08 05:32:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Xia (jasonxh), jasonxh@gmail.com
 *        Company:  
 *
 * ===========================================================================
 */

#ifndef _PROXY_H
#define _PROXY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include "pcmanx_utils.h"

#define PROXY_NONE   0
#define PROXY_SOCKS4 1
#define PROXY_SOCKS5 2

X_EXPORT int proxy_connect(const struct sockaddr_in *serv_addr
		, int proxy_type, const struct sockaddr_in *proxy_addr
		, const char *user, const char *pass);

#ifdef __cplusplus
}
#endif

#endif
