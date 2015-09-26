/* -- vi: set ts=4 sw=4: --
 *
 * proxy.h - Simple implementation of SOCKS client
 *
 * Copyright (C) 2008 Jason Xia <jasonxh@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

X_EXPORT int proxy_connect(const struct sockaddr_storage *serv_addr
		, int proxy_type, const struct sockaddr_in *proxy_addr
		, const char *user, const char *pass);

#ifdef __cplusplus
}
#endif

#endif
