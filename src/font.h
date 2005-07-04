/***************************************************************************
 *   Copyright (C) 2005 by PCMan   *
 *   hzysoft@sina.com.tw   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FONT_H
#define FONT_H

#include <string>
#include <X11/Xft/Xft.h>

using namespace std;

/**
@author PCMan
*/
class CFont{
public:
    CFont();
    ~CFont();
     CFont( string name, int pt_size, bool anti_alias = true );
     CFont( string name, int width, int height, bool anti_alias = true );
    void SetFont( string name, int pt_size, bool anti_alias = true );
    void SetFont( string name, int width, int height, bool anti_alias = true );
	int GetHeight(){	return m_XftFont->ascent + m_XftFont->descent;	}
	int GetMaxWidth(){	int w = m_XftFont->max_advance_width;	return w %2 ? w : (w + 1);	}
	XftFont* GetXftFont(){	return m_XftFont;	}
	string GetName(){	return m_Name;	}
	bool GetAntiAlias(){	return m_AntiAlias;	}
    void SetFontFamily(string name);
protected:
	XftFont* m_XftFont;
	string m_Name;
	int m_PointSize;
	bool m_AntiAlias;
private:
    XftFont* CreateXftFont(string name, int size, bool anti_alias, bool is_point_size );
    XftFont* CreateXftFont(string name, int width, int height, bool anti_alias);
};

#endif
