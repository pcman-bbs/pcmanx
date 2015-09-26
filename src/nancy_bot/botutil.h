/**
 * Copyright (c) 2005 Youchen Lee ( utcr.org )
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

#ifndef __INCLUDE_BOTUTIL_H__
#define __INCLUDE_BOTUTIL_H__

#include <cstring>
using namespace std;

inline int
replaceString (string & modify_me, string & find_me, string & replace_with_me)
{
	unsigned int search_here = 0;
	int num_replaced = 0;
	if (find_me == replace_with_me)
		return 0;
	while (1) {
		search_here = modify_me.find (find_me, search_here);
		if (search_here != string::npos && search_here < modify_me.length ())	// found
		{
			modify_me.replace (search_here, find_me.length (),
					   replace_with_me);
			num_replaced++;
			search_here++;
		}
		else
			break;
	}
	return num_replaced;
}

inline string
trim (const string & str)
{
	if (str.length () == 0)
		return str;
	int first = str.find_first_not_of (" \t");
	int end = str.find_last_not_of (" \t");
	if (first == -1)
		return "";
	return std::string (str, first, end - first + 1);
}

#endif // #define __INCLUDE_BOTUTIL_H__
