/////////////////////////////////////////////////////////////////////////////
// Name:        stringutil.h
// Purpose:     Some string related utilities
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      pcman.tw@gmail.com
// Created:     2004.7.24
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////

#ifndef	__STRINGUTIL_H__
#define	__STRINGUTIL_H__

#include <string>
using namespace std;

	string EscapeStr(const char* pstr);
	string UnEscapeStr(const char* pstr);
	inline void EscapeStr(string& str){ str = EscapeStr(str.c_str()); }
	inline void UnEscapeStr(string& str){ str = UnEscapeStr(str.c_str()); }

	string ConvertFromCRLF(const char* pstr);
	string ConvertToCRLF(const char* pstr);
	inline void ConvertFromCRLF(string& str){ str = ConvertFromCRLF(str.c_str()); }
	inline void ConvertToCRLF(string& str){ str = ConvertToCRLF(str.c_str()); }
	int strncmpi(const char* str1, const char* str2, size_t len);
#endif
