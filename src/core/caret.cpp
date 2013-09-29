/**
 * caret.cpp: implementation of the CCaret class.
 *            Show a cursor on the black terminal screen.
 *
 * Copyright (c) 2004-2005 PCMan <pcman.tw@gmail.com>
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

#ifdef __GNUG__
  #pragma implementation "caret.h"
#endif

#include "caret.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaret::CCaret()
{
	m_pParent = (GtkWidget*)NULL;
	m_Pos.x = m_Pos.y = 0;
	m_Width = m_Height = 0;
	m_IsVisible = m_IsShow = false;
#if GTK_CHECK_VERSION(2,22,0)
	m_Cairo = NULL;
#endif
	m_GC = (GdkGC *)NULL;
}

CCaret::~CCaret()
{
#if GTK_CHECK_VERSION(2,22,0)
	if (m_Cairo) {
		cairo_destroy(m_Cairo);
		m_Cairo = NULL;
	}
#endif
}

//Inverse the color of caret when it sets visible flag.
void CCaret::Blink()
{
	if( m_IsVisible ){
		m_IsShow = !m_IsShow;
		DrawInverse();
	}
}

//Hide this caret
void CCaret::Hide()
{
	if((m_IsShow && m_IsVisible )){
		m_IsShow = m_IsVisible = false;
		DrawInverse();
	}
}

//Unhide the caret
void CCaret::Show(bool bImmediately)
{
	m_IsVisible = true;
	if( bImmediately && !m_IsShow ){
		m_IsShow = true;
		DrawInverse();
	}
}

//Reset the position X and Y, perform hiding and reshowing if necessary
void CCaret::Move( int x, int y )
{
	bool bNeedRestore = m_IsVisible;
	if( m_IsVisible )
		Hide();
	m_Pos.x = x;	m_Pos.y = y;
	if( bNeedRestore )
		Show();
}

//Reset the position X and Y, perform hiding and reshowing if necessary
void CCaret::SetSize( int Width, int Height )
{
	bool bNeedRestore = m_IsVisible;
	if( m_IsVisible )
		Hide();
	m_Width = Width;	m_Height = Height;
	if( bNeedRestore )
		Show();
}

void CCaret::DrawInverse()
{
	if( !m_pParent )
		return;
	if( !GDK_IS_DRAWABLE(m_pParent->window))
	{
//		g_warning("Warring! Draw on DELETED widget!\n");
		return;
	}

#if GTK_CHECK_VERSION(2,22,0)
	if (m_Cairo == NULL) {
		m_Cairo = gdk_cairo_create(m_pParent->window);
		cairo_destroy(m_Cairo);
		m_Cairo = gdk_cairo_create(m_pParent->window);
	}
	cairo_set_source_rgb(m_Cairo, 1, 1, 1);
	cairo_set_operator(m_Cairo, CAIRO_OPERATOR_DIFFERENCE);
	cairo_rectangle(m_Cairo, m_Pos.x, m_Pos.y, m_Width, m_Height);
	cairo_fill(m_Cairo);
#else
	gdk_gc_set_function(m_GC, GDK_INVERT);
	gdk_draw_drawable(m_pParent->window, m_GC, m_pParent->window,
		m_Pos.x, m_Pos.y, m_Pos.x, m_Pos.y, m_Width, m_Height);
	gdk_gc_set_function(m_GC, GDK_COPY);
#endif
}

