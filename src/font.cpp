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
#include "font.h"
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

CFont::CFont()
{
	m_PointSize = 0;
	m_XftFont = NULL;
}

CFont::~CFont()
{
	if( m_XftFont )
	{
		Display *display = gdk_x11_get_default_xdisplay();
		XftFontClose(display, m_XftFont );
	}
}

CFont::CFont( string name, int pt_size, bool anti_alias )
{
   	m_PointSize = pt_size;
	m_Name = name;
	m_AntiAlias = anti_alias;
	m_XftFont = CreateXftFont( name, pt_size, anti_alias, true );
}

CFont::CFont( string name, int width, int height, bool anti_alias )
{
    m_PointSize = 0;
	m_Name = name;
	m_AntiAlias = anti_alias;
	m_XftFont = CreateXftFont( name, width, height, anti_alias );
}

void CFont::SetFont( string name, int pt_size, bool anti_alias )
{
   	m_PointSize = pt_size;
	m_Name = name;
	m_AntiAlias = anti_alias;
	if( m_XftFont )
	{
		Display *display = gdk_x11_get_default_xdisplay();
		XftFontClose(display, m_XftFont );
	}		
	m_XftFont = CreateXftFont( name, pt_size, anti_alias, true );
}

void CFont::SetFont( string name, int width, int height, bool anti_alias )
{
	m_Name = name;
	if( m_XftFont )
	{
		Display *display = gdk_x11_get_default_xdisplay();
		XftFontClose(display, m_XftFont );
	}
	m_AntiAlias = anti_alias;
	m_XftFont = CreateXftFont( name, width, height, anti_alias );
	m_PointSize = 0;
}

XftFont* CFont::CreateXftFont( string name, int size, bool anti_alias, bool is_point_size )
{
	Display *display = gdk_x11_get_default_xdisplay();
	int screen = DefaultScreen (display);

	XftFont* font = XftFontOpen (display, screen,
					XFT_FAMILY, XftTypeString, name.c_str(),
					XFT_CORE, XftTypeBool, False,
					(is_point_size ? XFT_SIZE:XFT_PIXEL_SIZE), XftTypeDouble, (double)size,
					XFT_WEIGHT, XftTypeInteger, XFT_WEIGHT_MEDIUM,
					XFT_ANTIALIAS, XftTypeBool, anti_alias,
					NULL);
	return font;
}

XftFont* CFont::CreateXftFont( string name, int width, int height, bool anti_alias )
{
	Display *display = gdk_x11_get_default_xdisplay();
	int screen = DefaultScreen (display);

	XftFont* font = XftFontOpen (display, screen,
					XFT_FAMILY, XftTypeString, name.c_str(),
					XFT_CORE, XftTypeBool, False,
					XFT_PIXEL_SIZE, XftTypeDouble, (double)height,
					XFT_WEIGHT, XftTypeInteger, XFT_WEIGHT_MEDIUM,
					XFT_ANTIALIAS, XftTypeBool, anti_alias,
					NULL);

	int w = font->max_advance_width;
	int h = font->ascent + font->descent;

	// TODO: must use new method to determine font size
	while( (w > 2 && h > 2) && ( w > width*2 || h > height*2) )
	{
		if( font )
			XftFontClose(display, font);

		h--;

		font = XftFontOpen (display, screen,
						XFT_FAMILY, XftTypeString, name.c_str(),
						XFT_CORE, XftTypeBool, False,
						XFT_PIXEL_SIZE, XftTypeDouble, (double)h,
						XFT_WEIGHT, XftTypeInteger, XFT_WEIGHT_MEDIUM,
						XFT_ANTIALIAS, XftTypeBool, anti_alias,
						NULL);

		w = font->max_advance_width;
	}

	return font;
}

void CFont::SetFontFamily(string name)
{
    m_Name = name;
	if( m_XftFont )
	{
		Display *display = gdk_x11_get_default_xdisplay();
		XftFontClose(display, m_XftFont );
	}
	if( m_PointSize > 0 )
		m_XftFont = CreateXftFont( name, m_PointSize, m_AntiAlias, true );
	else
		m_XftFont = CreateXftFont( name, GetMaxWidth()/2, GetHeight(), m_AntiAlias );
}
