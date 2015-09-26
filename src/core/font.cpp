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

#include "debug.h"

CFont::CFont()
{
	m_XftFont = NULL;
	m_PointSize = 0;
	m_MaxWidth = 0;
	m_MaxHeight = 0;
	m_Compact = false;
	m_AntiAlias = false;
}

CFont::~CFont()
{
	CloseXftFont( m_XftFont );
}

CFont::CFont( string name, int pt_size, bool compact, bool anti_alias )
{
	m_XftFont = NULL;
	m_Name = name;
   	m_PointSize = pt_size;
	m_Compact = compact;
	m_AntiAlias = anti_alias;

	m_XftFont = CreateXftFont( name, pt_size, m_AntiAlias );
}

CFont::CFont( string name, int width, int height, bool compact, bool anti_alias )
{
	m_Name = name;
	m_PointSize = 0;
	m_MaxWidth = width;
	m_MaxHeight = height;
	m_Compact = compact;
	m_AntiAlias = anti_alias;

	m_XftFont = CreateXftFont( name, width, height, m_AntiAlias );
}

void CFont::SetFont( string name, int pt_size, bool compact, bool anti_alias )
{
	m_Name = name;
   	m_PointSize = pt_size;
	m_Compact = compact;
	m_AntiAlias = anti_alias;

	CloseXftFont( m_XftFont );
	m_XftFont = CreateXftFont( name, pt_size, m_AntiAlias );
}

void CFont::SetFont( string name, int width, int height, bool compact, bool anti_alias )
{
	m_Name = name;
	m_PointSize = 0;
	m_MaxWidth = width;
	m_MaxHeight = height;
	m_Compact = compact;
	m_AntiAlias = anti_alias;

	CloseXftFont( m_XftFont );
	m_XftFont = CreateXftFont( name, width, height, m_AntiAlias );
}

void CFont::CloseXftFont( XftFont* font )
{
	if( font )
	{
		Display *display = gdk_x11_get_default_xdisplay();
		XftFontClose(display, font );
	}
}

XftFont* CFont::CreateXftFont( string name, int size, bool anti_alias )
{
	Display *display = gdk_x11_get_default_xdisplay();
	int screen = DefaultScreen (display);

	XftFont* font = XftFontOpen (display, screen,
					FC_FAMILY, FcTypeString, name.c_str(),
					FC_SIZE, FcTypeDouble, (double)size,
					FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
					FC_ANTIALIAS, FcTypeBool, anti_alias,
					XFT_CORE, FcTypeBool, False,
					NULL);

	return font;
}

void CFont::RecalculateMetrics( XftFont* font )
{
	FT_Face face = XftLockFace( font );

	if( !face )
		return;

	if( face->face_flags & FT_FACE_FLAG_SCALABLE )
	{
		FT_Fixed x_scale = face->size->metrics.x_scale;
		FT_Fixed y_scale = face->size->metrics.y_scale;

		FT_Pos asc = FT_MulFix( face->ascender, y_scale );
		FT_Pos des = -FT_MulFix( face->descender, y_scale );
		FT_Pos adv = FT_MulFix( face->max_advance_width, x_scale );

		adv = ( adv + 32 ) >> 6;
		if( m_Compact )
		{
			/* ceil unless the fractional part < 0.0625 */
			font->height = ( asc  + des  + 60 ) >> 6;

			font->ascent = ( asc  + 32 ) >> 6;
			font->descent = font->height - font->ascent;
			font->max_advance_width = adv;
		}
		else
		{
			font->ascent = ( asc + 63 ) >> 6;
			font->descent = ( des + 63 ) >> 6;
			font->height = font->ascent + font->descent;
			//font->max_advance_width = ( adv + 1 ) & ~1;
			font->max_advance_width = adv;
		}
	}

	XftUnlockFace( font );
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
	if( !pattern )
		return NULL;

	FcResult result;
	FcPattern* match = XftFontMatch( display, screen, pattern, &result );
	FcPatternDestroy( pattern );
	if( !match )
		return NULL;

	XftFont* font = XftFontOpenPattern( display, match );
	if( !font )
	{
		FcPatternDestroy( match );

		return NULL;
	}
	RecalculateMetrics( font );

	// width is single width
	int w = font->max_advance_width / 2;
	int h = font->height;

	while( size > 4 && ( w > width || h > height ) )
	{
		size--;

		if( font )
		{
			match = FcPatternDuplicate( font->pattern );
			XftFontClose( display, font );
		}

		FcPatternDel( match, FC_PIXEL_SIZE );
		FcPatternAddDouble( match, FC_PIXEL_SIZE, (double)size );

		font = XftFontOpenPattern( display, match );
		if( !font )
		{
			FcPatternDestroy( match );
			break;
		}
		RecalculateMetrics( font );

		w = font->max_advance_width / 2;
		h = font->height;
	}

	return font;
}

void CFont::SetFontFamily( string name )
{
	if( m_PointSize > 0 )
		SetFont( name, m_PointSize, m_Compact, m_AntiAlias );
	else
		SetFont( name, m_MaxWidth, m_MaxHeight, m_Compact, m_AntiAlias );
}
