/**
 * Copyright (c) 2005 PCMan <pcman.tw@gmail.com>
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

#ifndef FONT_H
#define FONT_H

#ifdef __GNUG__
  #pragma interface "font.h"
#endif

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
    CFont( string name, int pt_size, bool compact = false, bool anti_alias = true );
    CFont( string name, int width, int height, bool compact = false, bool anti_alias = true );
    void SetFont( string name, int pt_size, bool compact = false, bool anti_alias = true );
    void SetFont( string name, int width, int height, bool compact = false, bool anti_alias = true );
    void SetFontFamily( string name );
    inline int GetHeight(){	return m_XftFont->ascent + m_XftFont->descent;	};
    inline int GetWidth(){	return m_XftFont->max_advance_width / 2;	};
    XftFont* GetXftFont(){	return m_XftFont;	}
    string GetName(){		return m_Name;		}
    bool GetAntiAlias(){	return m_AntiAlias;	}
    bool GetCompact(){		return m_Compact;	}
protected:
    XftFont* m_XftFont;
    string m_Name;
    int m_PointSize;
    int m_MaxWidth;
    int m_MaxHeight;
    bool m_Compact;
    bool m_AntiAlias;
private:
    XftFont* CreateXftFont( string name, int size, bool anti_alias );
    XftFont* CreateXftFont( string name, int width, int heigh, bool anti_alias  );
    void CloseXftFont( XftFont* font );
    void RecalculateMetrics( XftFont* font );
};

#endif
