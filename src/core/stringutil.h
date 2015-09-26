/**
 * stringutil.h - Some string related utilities
 *
 * Copyright (c) 2004 PCMan <pcman.tw@gmail.com>
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

#ifndef	__STRINGUTIL_H__
#define	__STRINGUTIL_H__

#include "pcmanx_utils.h"

#include <string>
#include <strings.h>
using namespace std;

X_EXPORT
string EscapeStr(const char* pstr);
X_EXPORT
string UnEscapeStr(const char* pstr);
X_EXPORT
inline void EscapeStr(string& str){ str = EscapeStr(str.c_str()); }
X_EXPORT
inline void UnEscapeStr(string& str){ str = UnEscapeStr(str.c_str()); }

string ConvertFromCRLF(const char* pstr);
string ConvertToCRLF(const char* pstr);
inline void ConvertFromCRLF(string& str){ str = ConvertFromCRLF(str.c_str()); }
inline void ConvertToCRLF(string& str){ str = ConvertToCRLF(str.c_str()); }
#define strncmpi(str1,str2,len)  strncasecmp(str1,str2,len)
#endif
