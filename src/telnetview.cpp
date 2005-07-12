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

#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <ctype.h>
#include <string.h>

#include "telnetview.h"
#include "telnetcon.h"
#include "mainframe.h"

CMainFrame* CTelnetView::m_pParentFrame = NULL;

CTelnetView::CTelnetView()
        : CTermView()
{}

CTelnetView::~CTelnetView()
{
	if( m_pTermData )
		delete m_pTermData;
}


void CTelnetView::OnTextInput(const gchar* text)
{
	gsize l;
	gchar* _text = g_convert(text, strlen(text), GetCon()->m_Site.m_Encoding.c_str(), "UTF-8", NULL, &l, NULL);
	((CTelnetCon*)m_pTermData)->Send(_text, l);
	g_free(_text);
}

#define	GDK_MODIFIER_DOWN(key, mod)	(key & (mod|(~GDK_SHIFT_MASK&~GDK_CONTROL_MASK&~GDK_MOD1_MASK)))

bool CTelnetView::OnKeyDown(GdkEventKey* evt)
{
//	g_print("CTelnetView::OnKeyDown\n");
	CTermCharAttr* pAttr = m_pTermData->GetLineAttr(
			m_pTermData->m_Screen[m_pTermData->m_CaretPos.y] );
	int x = m_pTermData->m_CaretPos.x;

	if( evt->keyval < 127 && GDK_MODIFIER_DOWN(evt->state, GDK_CONTROL_MASK))// Ctrl down
	{
		char ch = toupper(char(evt->keyval));
		if( ch >= '@' && ch <= '_'	&& !isdigit(ch) )
		{
			ch -= '@';
			GetCon()->SendString(&ch,1);
			return true;
		}
	}

	switch(evt->keyval)
	{
	case GDK_Left:
	case GDK_KP_Left:
		GetCon()->SendString("\x1bOD\x1bOD",( x > 0 && pAttr[x-1].GetCharSet() == CTermCharAttr::CS_MBCS2 ) ? 6 : 3);
		break;
	case GDK_Right:
	case GDK_KP_Right:
		GetCon()->SendString("\x1bOC\x1bOC",( pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1 ) ? 6 : 3);
		break;
	case GDK_Up:
	case GDK_KP_Up:
		GetCon()->SendString("\x1bOA",3);
		break;
	case GDK_Down:
	case GDK_KP_Down:
 		GetCon()->SendString("\x1bOB",3);
		break;
	case GDK_BackSpace:
		GetCon()->SendString("\b\b", ( x > 0 && pAttr[x-1].GetCharSet() == CTermCharAttr::CS_MBCS2 ) ? 2 : 1);
		break;
	case GDK_Return:
	case GDK_KP_Enter:
		GetCon()->SendString("\r",1);
		break;
	case GDK_Delete:
	case GDK_KP_Delete:
		GetCon()->SendString("\x1b[3~\x1b[3~",( pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1 ) ? 8 : 4);
		break;
	case GDK_Insert:
	case GDK_KP_Insert:
		GetCon()->SendString("\x1b[2~",4);
		break;
	case GDK_Home:
	case GDK_KP_Home:
		GetCon()->SendString("\x1b[1~",4);
		break;
	case GDK_End:
	case GDK_KP_End:
		GetCon()->SendString("\x1b[4~",4);
		break;
//	case GDK_Prior:
	case GDK_Page_Up:
	case GDK_KP_Page_Up:
		GetCon()->SendString("\x1b[5~",4);
		break;
//	case GDK_Next:
	case GDK_Page_Down:
	case GDK_KP_Page_Down:
		GetCon()->SendString("\x1b[6~",4);
		break;
	case GDK_Tab:
		break;
	}
	return true;
}


void CTelnetView::OnRButtonDown(GdkEventButton* evt)
{
	if( !m_ContextMenu )
		return;
	gtk_menu_popup( m_ContextMenu, NULL, NULL, NULL, NULL, evt->button, evt->time );
}

bool CTelnetView::PreKeyDown(GdkEventKey* evt)
{
/*	if( GDK_MODIFIER_DOWN( evt->state, GDK_MOD1_MASK)
			|| GDK_MODIFIER_DOWN( evt->state, GDK_CONTROL_MASK)
			&& ((evt->keyval > GDK_0 && evt->keyval > GDK_9)
				 || (evt->keyval > GDK_KP_0 && evt->keyval > GDK_KP_9) )
			)
	{
		int i = evt->keyval > GDK_KP_0 ? (evt->keyval - GDK_KP_0):(evt->keyval - GDK_0);
		m_pParentFrame->SwitchToTab(i);
		return true;
	}
*/	return false;
}

void CTelnetView::DoPasteFromClipboard(string text, bool contain_ansi_color)
{
	if( GetCon() )
	{
		string text2;
		if( contain_ansi_color )
		{
			const char* p = text.c_str();
			while(*p)
			{
				if(*p == '\x1b')
					text2 += GetCon()->m_Site.m_ESCConv;
				else
					text2 += *p;
				p++;
			}
			GetCon()->SendString(text2);
		}
		else
			GetCon()->SendString(text);
	}
}
