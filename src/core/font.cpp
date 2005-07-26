/**
 * Copyright (c) 2005 PCMan <hzysoft@sina.com.tw>
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
 
/*
 2005-07-26  Chia I Wu  <b90201047@ntu.edu.tw>

	Use FC_PIXEL_SIZE when opening font by pixel height.
	Use XftFontOpenPattern so that we don't have to match the pattern
	every time.
	s/XFT_/FC_/ and s/XftType/FcType/ where suitable.
*/

#ifdef __GNUG__
  #pragma implementation "font.h"
#endif


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
					FC_FAMILY, FcTypeString, name.c_str(),
					(is_point_size ? FC_SIZE:FC_PIXEL_SIZE), FcTypeDouble, (double)size,
					FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
					FC_ANTIALIAS, FcTypeBool, anti_alias,
					XFT_CORE, FcTypeBool, False,
					NULL);
	return font;
}

XftFont* CFont::CreateXftFont( string name, int width, int height, bool anti_alias )
{
	Display *display = gdk_x11_get_default_xdisplay();
	int screen = DefaultScreen (display);
	int size = height;

	FcPattern* pattern = FcPatternBuild( NULL,
			FC_FAMILY, FcTypeString, name.c_str(),
			FC_PIXEL_SIZE, FcTypeDouble, (double)size,
			FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
			FC_ANTIALIAS, FcTypeBool, anti_alias,
			XFT_CORE, FcTypeBool, False,
			NULL );
	if ( !pattern )
		return NULL;

	FcResult result;
	FcPattern* match = XftFontMatch( display, screen, pattern, &result );
	FcPatternDestroy( pattern );
	if ( !match )
		return NULL;

	XftFont* font = XftFontOpenPattern( display, match );
	if ( !font )
	{
		FcPatternDestroy( match );

		return NULL;
	}

	int w = font->max_advance_width;
	int h = font->ascent + font->descent;

	/* double-width */
	width *= 2;

	while ( size > 4 && ( w > width || h > height) )
	{
		size --;

		if ( font )
		{
			match = FcPatternDuplicate( font->pattern );
			XftFontClose(display, font);
		}

		FcPatternDel( match, FC_PIXEL_SIZE );
		FcPatternAddDouble( match, FC_PIXEL_SIZE, (double)size );

		font = XftFontOpenPattern( display, match );
		if ( !font )
			return NULL;

		w = font->max_advance_width;
		h = font->ascent + font->descent;
	}
	
#if 0	// Deprecated: Create XftFont by pixel size
	XftFont* font = XftFontOpen (display, screen,
					FC_FAMILY, FcTypeString, name.c_str(),
					XFT_CORE, FcTypeBool, False,
					FC_PIXEL_SIZE, FcTypeDouble, (double)height,
					FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
					FC_ANTIALIAS, FcTypeBool, anti_alias,
					NULL);

	int w = font->max_advance_width;
	int h = font->ascent + font->descent;

	int max_width = width * 2;
	// TODO: must use new method to determine font size
	while( (w > 4 && h > 4) && ( w > max_width || h > height) )
	{
		if( font )
			XftFontClose(display, font);

		int old_h = h;
		h--;

		font = XftFontOpen (display, screen,
						FC_FAMILY, FcTypeString, name.c_str(),
						XFT_CORE, FcTypeBool, False,
						FC_PIXEL_SIZE, FcTypeDouble, (double)h,
						FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
						FC_ANTIALIAS, FcTypeBool, anti_alias,
						NULL);

		w = font->max_advance_width;
		h = font->ascent + font->descent;
		if( h >= old_h )
			h--;
	}
#endif
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
