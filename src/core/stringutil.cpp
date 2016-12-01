/**
 * stringutil.cpp - Some string related utilities
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

#include "stringutil.h"
#include <cctype>
#include <cstdio>
#include <regex.h>

// Note by PCMan:
// EscapeStr() & UnEscapeStr() :
// Encode non-printable characters with '^' and '\\'
// \x?? means a hexadecimal integer. ex: \x0d = '\r' and \x0a = '\n'
// \\ means \ character
// ^ symbol means CTRL, ex: ^U means CTRL+U, or '\x15'
// \^ means ^ character

// 2004/08/05 modified by PCMan
string EscapeStr(const char* pstr)
{
	string _str;
	for( ; *pstr ; pstr++ )
	{
		unsigned ch = (unsigned char)*pstr;
		if( ' ' > ch  ) // if it's not a printable character.
		{
			char hex[4];
			sprintf(hex, "\\%02x", (unsigned char)*pstr );
			_str += hex;
			pstr++;
			continue;
		}
		else if( '^' == ch || '\\' == ch )
			_str += '\\';
		_str += ch;
	}
	return _str;
}

// 2004/08/05 modified by PCMan
string UnEscapeStr(const char* pstr)
{
	string _str;
	for( ; *pstr ; pstr++ )
	{
		unsigned ch = (unsigned char)*pstr;
		if( '\\' == ch )
		{
			switch( pstr[1] )
			{
			case '\\':	//	"\\" means '\' character
				pstr++;
				_str += '\\';
				break;
			case '^':	//	"\^" means '^' character
				pstr++;
				_str += '^';
				break;
			default:	//	\x?? means a hexadecimal integer. ex: \x0d = '\r' and \x0a = '\n'
				char hex[4] = {pstr[2], pstr[3], '\0', '\0'};
				int val;
				sscanf(hex, "%2x", &val);
				_str += (char)val;
				pstr += 3;
				continue;
			}
		}
		else if( '^' == ch )		// ^ symbol means CTRL, ex: ^U means CTRL+U, or '\x15'
		{
			ch = (unsigned char)pstr[1];
			if( ch >= '@' && ch <= '_' )
			{
				pstr ++;
				_str += (char)(ch - '@');
				continue;
			}
		}
		else
		{
			_str += *pstr;
		}
	}
	return _str;
}


string ConvertFromCRLF(const char* pstr)
{
	string ret;
	for( ; *pstr; pstr++ )
	{
		if( *pstr == '\r' )	// CR
		{
			ret += '\n';
			if(*(pstr+1) == '\n')
				pstr++;
		}
		else
			ret += *pstr;
	}
	return ret;
}

string ConvertToCRLF(const char* pstr)
{
	string ret;
	for( ; *pstr; pstr++ )
	{
		if( *pstr == '\r' )	// CR
		{
			ret += "\r\n";
			if(*(pstr+1) == '\n')
				pstr++;
		}
		else if( *pstr == '\n' )	// LF
			ret += "\r\n";
		else
			ret += *pstr;
	}
	return ret;
}

bool IsMatch(const char* str, const char* regex_str)
{
	regex_t regex;
	int res;
	const int nmatch = 1;
	regmatch_t pmatch[nmatch];
	if(regcomp(&regex, regex_str, REG_EXTENDED) != 0) {
		regfree(&regex);
		return false;
	}
	res = regexec(&regex, str, nmatch, pmatch, 0);
	regfree(&regex);
	if(res == REG_NOMATCH){
		return false;
	} else {
		return true;
	}
}
