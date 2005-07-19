// Caret.h: interface for the CCaret class.
//
/////////////////////////////////////////////////////////////////////////////
// Name:        caret.h
// Purpose:     Show a cursor on the black terminal screen
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:		2004.7.17
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
//			Neversay(neversay.misher@gmail.com) Jan/18/2005
/////////////////////////////////////////////////////////////////////////////


#if !defined(PCMANX_CARET_H)
#define PCMANX_CARET_H

#ifdef __GNUG__
  #pragma interface "caret.h"
#endif

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
	~CCaret();
	//Getter by neversay Jan/18/2005
	inline bool IsShow(){return m_IsShow;}
	inline bool IsVisible(){return m_IsVisible;}
	inline int GetPositionX(){return m_Pos.x;}
	inline int GetPositionY(){return m_Pos.y;}
	inline int GetWidth(){return m_Width;}
	inline int GetHeight(){return m_Height;}
	//Setter by neversay Jan/18/2005
	inline void SetShow(bool flag){m_IsShow = flag;}
	inline void SetVisible(bool flag){m_IsVisible = flag;}
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
	void Hide();
	//Unhide the caret.
	void Show(bool bImmediately = true);

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
