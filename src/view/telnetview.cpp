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
#include <stdlib.h>

#include "telnetview.h"
#include "telnetcon.h"

#if !defined(MOZ_PLUGIN)
#include "mainframe.h"
#include "stringutil.h"

CMainFrame* CTelnetView::m_pParentFrame = NULL;
#endif /* !defined(MOZ_PLUGIN) */

#include "debug.h"

CTelnetView::CTelnetView()
        : CTermView()
{}
string CTelnetView::m_WebBrowser;
string CTelnetView::m_MailClient;

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
	INFO("CTelnetView::OnKeyDown\n");
	CTermCharAttr* pAttr = m_pTermData->GetLineAttr(
			m_pTermData->m_Screen[m_pTermData->m_CaretPos.y] );
	int x = m_pTermData->m_CaretPos.x;

	if( evt->keyval < 127 && GDK_MODIFIER_DOWN(evt->state, GDK_CONTROL_MASK))// Ctrl down
	{
		char ch = toupper(char(evt->keyval));
		if( ch >= '@' && ch <= '_'	&& !isdigit(ch) )
		{
			ch -= '@';
			GetCon()->SendRawString(&ch,1);
			return true;
		}
	}

	switch(evt->keyval)
	{
	case GDK_Left:
	case GDK_KP_Left:
		GetCon()->SendRawString("\x1bOD\x1bOD",( GetCon()->DetectDBChar() && x > 0 && pAttr[x-1].GetCharSet() == CTermCharAttr::CS_MBCS2 ) ? 6 : 3);
		break;
	case GDK_Right:
	case GDK_KP_Right:
		GetCon()->SendRawString("\x1bOC\x1bOC",( GetCon()->DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1 ) ? 6 : 3);
		break;
	case GDK_Up:
	case GDK_KP_Up:
		GetCon()->SendRawString("\x1bOA",3);
		break;
	case GDK_Down:
	case GDK_KP_Down:
 		GetCon()->SendRawString("\x1bOB",3);
		break;
	case GDK_BackSpace:
		GetCon()->SendRawString("\b\b", ( GetCon()->DetectDBChar() && x > 0 && pAttr[x-1].GetCharSet() == CTermCharAttr::CS_MBCS2 ) ? 2 : 1);
		break;
	case GDK_Return:
	case GDK_KP_Enter:
		GetCon()->SendRawString("\r",1);
		break;
	case GDK_Delete:
	case GDK_KP_Delete:
		GetCon()->SendRawString("\x1b[3~\x1b[3~",( GetCon()->DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1 ) ? 8 : 4);
		break;
	case GDK_Insert:
	case GDK_KP_Insert:
		GetCon()->SendRawString("\x1b[2~",4);
		break;
	case GDK_Home:
	case GDK_KP_Home:
		GetCon()->SendRawString("\x1b[1~",4);
		break;
	case GDK_End:
	case GDK_KP_End:
		GetCon()->SendRawString("\x1b[4~",4);
		break;
//	case GDK_Prior:
	case GDK_Page_Up:
	case GDK_KP_Page_Up:
		GetCon()->SendRawString("\x1b[5~",4);
		break;
//	case GDK_Next:
	case GDK_Page_Down:
	case GDK_KP_Page_Down:
		GetCon()->SendRawString("\x1b[6~",4);
		break;
	case GDK_Tab:
		GetCon()->SendRawString("\t", 1);
		break;
	case GDK_Escape:
		GetCon()->SendRawString("\x1b", 1);
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
#if !defined(MOZ_PLUGIN)
	if( !m_ContextMenu )
		return;
#endif

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
#if !defined(MOZ_PLUGIN)
	gtk_menu_popup( m_ContextMenu, NULL, NULL, NULL, NULL, evt->button, evt->time );
#endif
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
			gsize convl;
			gchar* locale_text = g_convert(text.c_str(), text.length(), GetCon()->m_Site.m_Encoding.c_str(), "UTF-8", NULL, &convl, NULL);
			if( !locale_text )
				return;
			// FIXME: Convert UTF-8 string to locale string.to prevent invalid UTF-8 string
			// caused by the auto-wrapper.
			// Just a workaround.  This needs to be modified in the future.
			const char* ptext = locale_text;
			const char* crlf = GetCon()->m_Site.GetCRLF();
			if( GetCon()->m_Site.m_AutoWrapOnPaste > 0 )
			{
				string str2;
				const char* pstr = locale_text;
				for( ; *pstr; ++pstr )
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
								++pstr;
							word_len = (pstr - pword) + (*pstr != '\t' ? 1 : 4);	// assume tab width = 4, may be changed in the future
						}
					}
					else
					{
						++pstr;
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
				ptext = text.c_str();
			}

			string text2;
			for( const char* pstr = ptext; *pstr; ++pstr )
				if( *pstr == '\n' )
					text2 += crlf;
				else
					text2 += *pstr;

			GetCon()->SendRawString(text2.c_str(), text2.length() );

			g_free( locale_text );
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

void CTelnetView::OnHyperlinkClicked(string url)
{
#if !defined(MOZ_PLUGIN)
	if( 0 == strncmpi( url.c_str(), "telnet:", 7) )
	{
		const char* purl = url.c_str() + 7;
		while( *purl == '/' )
			++purl;
		if( !*purl )
			return;
		url = purl;
		if( '/' == url[url.length()-1] )
			url = string( url.c_str(), 0, url.length()-1 );
		m_pParentFrame->NewCon( url, url );
		return;
	}
#endif /* !defined(MOZ_PLUGIN) */

	// In URL, the char "&" will be read as "background execution" when run the browser command without " "
	url.insert(0,"\"");
	url.append("\"");

	string app;
	if( !strstr( url.c_str(), "://") && strchr(url.c_str(), '@'))
	{
		app = m_MailClient;
		if( strncmpi( url.c_str(), "mailto:", 7 ) )
			url.insert( 0, "mailto:" );
	}
	else
		app = m_WebBrowser;

	char *cmdline = new char[ app.length() + url.length() + 10 ];
	if( strstr(app.c_str(), "%s") )
		sprintf( cmdline, app.c_str(), url.c_str() );
	else
	{
		memcpy(cmdline, app.c_str(), app.length());
		cmdline[app.length()] = ' ';
		memcpy( &cmdline[app.length() + 1], url.c_str(), url.length() + 1);
	}
	strcat(cmdline, " &");	// launch the browser in background.
	system(cmdline);	// Is this portable?
	delete []cmdline;
}

