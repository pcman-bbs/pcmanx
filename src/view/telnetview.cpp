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

#ifdef __GNUG__
  #pragma implementation "telnetview.h"
#endif

#include <glib/gi18n.h>

#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <ctype.h>
#include <string.h>

#include "telnetview.h"
#include "telnetcon.h"

#if !defined(MOZ_PLUGIN)
#include "mainframe.h"

CMainFrame* CTelnetView::m_pParentFrame = NULL;
#endif /* !defined(MOZ_PLUGIN) */

CTelnetView::CTelnetView()
        : CTermView()
{}


void CTelnetView::OnTextInput(const gchar* text)
{
	gsize l;
	gchar* _text = g_convert(text, strlen(text), GetCon()->m_Site.m_Encoding.c_str(), "UTF-8", NULL, &l, NULL);
	if( _text )
	{
		((CTelnetCon*)m_pTermData)->Send(_text, l);
		g_free(_text);
	}
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
		GetCon()->SendString("\t");
		break;
	case GDK_Escape:
		GetCon()->SendString("\x1b");
		break;
	}
	return true;
}

static void on_hyperlink_copy(GtkMenuItem* item, bool *do_copy)
{
	*do_copy = true;
}

void CTelnetView::OnRButtonDown(GdkEventButton* evt)
{
	if( !m_ContextMenu )
		return;

	if( m_pTermData )	// Copy URL popup menu.
	{
		int x = (int)evt->x;
		int y = (int)evt->y;
		PointToLineCol( &x, &y );
		int start, end;
		if( HyperLinkHitTest( x, y, &start, &end ) )
		{
			char* pline = m_pTermData->m_Screen[y];
			bool do_copy = false;
			// Show the "Copy Hyperlink" menu.
			GtkWidget* popup = gtk_menu_new();
			GtkWidget* item = gtk_image_menu_item_new_with_mnemonic( _("_Copy URL to Clipboard") );
			GtkWidget* icon = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_MENU);
			gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), icon);
			g_signal_connect( G_OBJECT(item), "activate", 
							G_CALLBACK(on_hyperlink_copy), &do_copy);

			gtk_menu_shell_append  ((GtkMenuShell *)popup, item );
			gtk_widget_show_all(popup);
			g_signal_connect( G_OBJECT(popup), "deactivate", 
							G_CALLBACK(gtk_main_quit), this);
			gtk_menu_popup( (GtkMenu*)popup, NULL, NULL, NULL, NULL, evt->button, evt->time );
			gtk_main();		// Don't return until the menu is closed.

			if( do_copy )
			{
				// Note by Hong Jen Yee (PCMan):
				// Theoratically, there is no non-ASCII characters in standard URL, 
				// so we don't need to do UTF-8 conversion at all.
				// However, users are always right.
				string url( (pline+start), (int)(end-start) );
				gsize wl = 0;
				const gchar* purl = g_convert_with_fallback( url.c_str(), url.length(),
						"utf-8", m_pTermData->m_Encoding.c_str(), "?", NULL, &wl, NULL);
				if(purl)
				{
					m_s_ANSIColorStr = "";
					GtkClipboard* clipboard = gtk_clipboard_get( GDK_NONE );
					gtk_clipboard_set_text(clipboard, purl, wl );
					clipboard = gtk_clipboard_get(  GDK_SELECTION_PRIMARY);
					gtk_clipboard_set_text(clipboard, purl, wl );
					g_free((void*)purl);
				}
			}
			gtk_widget_destroy(popup);
			return;
		}
	}
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
			string esc = GetCon()->m_Site.GetEscapeChar();
			const char* p = text.c_str();
			while(*p)
			{
				if(*p == '\x1b')
					text2 += esc;
				else
					text2 += *p;
				p++;
			}
			GetCon()->SendString(text2);
		}
		else
		{
			// Only when no control character is in this string can 
			// autowrap be enabled
			unsigned int len = 0, max_len = GetCon()->m_Site.m_AutoWrapOnPaste;
			if( GetCon()->m_Site.m_AutoWrapOnPaste > 0 )
			{
				string str2;
				const char* pstr = text.c_str();
				for( ; *pstr; pstr++ )
				{
					size_t word_len = 1;
					const char* pword = pstr;
					if( ((unsigned char)*pstr) < 128 )		// This is a ASCII character
					{
						if( *pstr == '\n' || *pstr == '\r' )
							len = 0;
						else
						{
							while( *pstr && ((unsigned char)*(pstr+1)) && ((unsigned char)*(pstr+1)) < 128  && !strchr(" \t\n\r", *pstr) )
								pstr++;
							word_len = (pstr - pword) + (*pstr != '\t' ? 1 : 4);	// assume tab width = 4, may be changed in the future
						}
					}
					else
					{
						pstr++;
						word_len = ( *pstr ? 2 : 1 );
					}
		
					if( (len + word_len) > max_len )
					{
						len = 0;
						str2 += '\n';
					}
					len += word_len;
					while( pword <= pstr )
					{
						str2 += *pword;
						pword ++;
					}
					if( *pstr == '\n' || *pstr == '\r' )
						len = 0;
				}
				text = str2;
			}

			GetCon()->SendString(text);
		}
	}
}


void CTelnetView::OnDestroy()
{
	if( m_pTermData )
	{
		delete m_pTermData;
		m_pTermData = NULL;
	}
}
