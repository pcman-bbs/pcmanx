/**
 * caret.h: interface for the CCaret class.
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

#if !defined(PCMANX_CARET_H)
#define PCMANX_CARET_H

#ifdef __GNUG__
  #pragma interface "caret.h"
#endif

#include "pcmanx_utils.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "widget.h"
/*
*	 There are some problems with wxCaret, so I decide to implement caret myself.
*/

class CCaret
{
public:
	//Constructor/Destructor
	CCaret();
	X_EXPORT ~CCaret();
	//Getter by neversay Jan/18/2005
	bool IsShow(){return m_IsShow;}
	bool IsVisible(){return m_IsVisible;}
	int GetPositionX(){return m_Pos.x;}
	int GetPositionY(){return m_Pos.y;}
	int GetWidth(){return m_Width;}
	int GetHeight(){return m_Height;}
	//Setter by neversay Jan/18/2005
	void SetShow(bool flag){m_IsShow = flag;}
	void SetVisible(bool flag){m_IsVisible = flag;}
	void Move( int x, int y );
	void SetSize( int Width, int Height );
	void Create(CWidget* pParent, GdkGC* gc)
	{
		m_pParent = pParent->m_Widget;
		m_GC = gc;
	}
	void Create(GtkWidget* pParent, GdkGC* gc)
	{
		m_pParent = pParent;
		m_GC = gc;
	}

	//Draw a the same shape caret with invsersed color at the same position.
	void DrawInverse();
	//Blink this caret	
	void Blink();
	//Hide this caret
	X_EXPORT void Hide();
	//Unhide the caret.
	X_EXPORT void Show(bool bImmediately = true);

private:
	//Flag if the caret is show up.
	bool m_IsShow;
	//Flag if the caret is visible.
	bool m_IsVisible;
	//Flag of the position of caret on screen.
	GdkPoint m_Pos;
	//The width of caret
	int m_Width;
	//The height of caret
	int m_Height;
	//The parent widget compoment.
	GtkWidget* m_pParent;
	GdkGC* m_GC;
};

#endif // !defined(PCMANX_CARET_H)
