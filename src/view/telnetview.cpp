/* -*- coding: utf-8; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
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

#ifdef __GNUG__
  #pragma implementation "telnetview.h"
#endif

#include <unistd.h>
#include <errno.h>

#include <glib/gi18n.h>

#include <cstring>

#include <gdk/gdkkeysyms.h>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <algorithm>

#include "telnetview.h"
#include "telnetcon.h"
#include "uao241.h"
#include "uao250.h"

#if !defined(MOZ_PLUGIN)
#include "mainframe.h"
#include "stringutil.h"
#include "appconfig.h"

CMainFrame* CTelnetView::m_pParentFrame = NULL;
#endif /* !defined(MOZ_PLUGIN) */

CTelnetView::CTelnetView()
	: CTermView()
{
#if defined(USE_IPLOOKUP) && !defined(MOZ_PLUGIN)
  m_pIpSeeker = seeker_new(AppConfig.GetConfigDirPath().append("/qqwry.dat").c_str());
#endif
}

CTelnetView::~CTelnetView()
{
#if defined(USE_IPLOOKUP) && !defined(MOZ_PLUGIN)
  if (m_pIpSeeker) seeker_delete(m_pIpSeeker);
#endif
}

string CTelnetView::m_WebBrowser;
string CTelnetView::m_MailClient;

static GtkWidget* input_menu_item = NULL;
static GtkWidget* websearch_menu_item = NULL;

void CTelnetView::OnTextInput(const gchar* text)
{
	gsize l;
	gchar* _text = NULL;
	/* UAO input support */
	switch (m_UAO) {
		case 2:
			_text = uao250_u2b(text, strlen(text), &l);
			break;
		case 1:
			_text = uao241_u2b(text, strlen(text), &l);
			break;
		default:
			_text = g_convert(text, strlen(text), GetCon()->m_Site.m_Encoding.c_str(), "UTF-8", NULL, &l, NULL);
			break;
	}
	if( _text )
	{
		((CTelnetCon*)m_pTermData)->Send(_text, l);
		g_free(_text);
	}
	// clear the old selection
	// Workaround FIXME please
	if (!m_pTermData->m_Sel->Empty())
	{
		GdkEventButton t_PseudoEvent;
		t_PseudoEvent.x = 0;
		t_PseudoEvent.y = 0;
		t_PseudoEvent.type = GDK_BUTTON_PRESS;
		CTermView::OnLButtonDown(&t_PseudoEvent);
		CTermView::OnLButtonUp(&t_PseudoEvent);
	}
}

#define	GDK_MODIFIER_DOWN(key, mod)	(key & (mod|(~GDK_SHIFT_MASK&~GDK_CONTROL_MASK&~GDK_MOD1_MASK)))

static int DrawCharWrapper( int row, int col, void *data )
{
	CTermView *tv = (CTermView *) data;

	return tv->DrawChar( row, col );
}

void EnterWrapper( CTelnetCon* Con, CTermCharAttr* Attr, int pos)
{
	if( Con->DetectDBChar() && Attr[pos].GetCharSet() == CTermCharAttr::CS_MBCS1 )
		Con->SendString("\x1bOC\x1bOC");
	else
		Con->SendString("\x1bOC");
}

void LeaveWrapper( CTelnetCon* Con, CTermCharAttr* Attr, int pos)
{
	if( Con->DetectDBChar() && pos > 0 && Attr[pos-1].GetCharSet() == CTermCharAttr::CS_MBCS2 )
		Con->SendString("\x1bOD\x1bOD");
	else
		Con->SendString("\x1bOD");
}

bool CTelnetView::OnKeyDown(GdkEventKey* evt)
{
	INFO("CTelnetView::OnKeyDown (keyval=0x%x, state=0x%x)", evt->keyval, evt->state);
	CTermCharAttr* pAttr = m_pTermData->GetLineAttr(
			m_pTermData->m_Screen[m_pTermData->m_CaretPos.y] );
	int x = m_pTermData->m_CaretPos.x;
	bool clear = true;
	bool reconnect = false;

	if( evt->keyval < 127 && GDK_MODIFIER_DOWN(evt->state, GDK_CONTROL_MASK))// Ctrl down
	{
		char ch = toupper(char(evt->keyval));
		if( ch >= '@' && ch <= '_'	&& !isdigit(ch) )
		{
			// clear the old selection
			if (!m_pTermData->m_Sel->Empty())
				ClearSelection();

			ch -= '@';
			GetCon()->SendRawString(&ch,1);
			return true;
		}
	}

	switch(evt->keyval)
	{
	case GDK_Left:
	case GDK_KP_Left:
		LeaveWrapper(GetCon(), pAttr, x);
		break;
	case GDK_Right:
	case GDK_KP_Right:
		EnterWrapper(GetCon(), pAttr, x);
		break;
	case GDK_Up:
	case GDK_KP_Up:
		GetCon()->SendString("\x1bOA");
		break;
	case GDK_Down:
	case GDK_KP_Down:
		GetCon()->SendString("\x1bOB");
		break;
	case GDK_BackSpace:
		if (GetCon()->DetectDBChar() && x > 0 && pAttr[x-1].GetCharSet() == CTermCharAttr::CS_MBCS2)
			GetCon()->SendString("\b\b");
		else
			GetCon()->SendString("\b");
		break;
	case GDK_Return:
	case GDK_KP_Enter:
		reconnect = GetCon()->IsClosed();
		GetCon()->SendString("\r");
		break;
	case GDK_Delete:
	case GDK_KP_Delete:
		if (GetCon()->DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1)
			GetCon()->SendString("\x1b[3~\x1b[3~");
		else
			GetCon()->SendString("\x1b[3~");
		break;
	case GDK_Insert:
	case GDK_KP_Insert:
		GetCon()->SendString("\x1b[2~");
		break;
	case GDK_Home:
	case GDK_KP_Home:
		GetCon()->SendString("\x1b[1~");
		break;
	case GDK_End:
	case GDK_KP_End:
		GetCon()->SendString("\x1b[4~");
		break;
//	case GDK_Prior:
	case GDK_Page_Up:
	case GDK_KP_Page_Up:
		GetCon()->SendString("\x1b[5~");
		break;
//	case GDK_Next:
	case GDK_Page_Down:
	case GDK_KP_Page_Down:
		GetCon()->SendString("\x1b[6~");
		break;
	case GDK_Tab:
		GetCon()->SendString("\t");
		break;
	case GDK_Escape:
		GetCon()->SendString("\x1b");
		break;
// F1-F12 keys
	case GDK_F1:
	case GDK_KP_F1:
		GetCon()->SendString("\x1bOP");
		break;
	case GDK_F2:
	case GDK_KP_F2:
		GetCon()->SendString("\x1bOQ");
		break;
	case GDK_F3:
	case GDK_KP_F3:
		GetCon()->SendString("\x1bOR");
		break;
	case GDK_F4:
	case GDK_KP_F4:
		GetCon()->SendString("\x1bOS");
		break;
	case GDK_F5:
		GetCon()->SendString("\x1b[15~");
		break;
	case GDK_F6:
		GetCon()->SendString("\x1b[17~");
		break;
	case GDK_F7:
	    GetCon()->SendString("\x1b[18~");
		break;
	case GDK_F8:
		GetCon()->SendString("\x1b[19~");
		break;
	case GDK_F9:
		GetCon()->SendString("\x1b[20~");
		break;
	case GDK_F10:
		GetCon()->SendString("\x1b[21~");
		break;
	case GDK_F11:
		GetCon()->SendString("\x1b[23~");
		break;
	case GDK_F12:
		GetCon()->SendString("\x1b[24~");
		break;
	default:
		clear = false;
	}

	// Only clear selection if we handled the key
	if (clear)
		ClearSelection();

	if (reconnect)
		GetCon()->Reconnect();

	return true;
}

static void on_hyperlink_copy(GtkMenuItem* item UNUSED, bool *do_copy)
{
	*do_copy = true;
}

#if defined(USE_IPLOOKUP) && !defined(MOZ_PLUGIN)
static inline unsigned int ipstr2int(const char *str)
{
	unsigned char ip[4];
	if (sscanf(str, " %hhu . %hhu . %hhu . %hhu"
				, ip + 3, ip + 2, ip + 1, ip) != 4)
		return 0;
	return *((unsigned int*)ip);
}
#endif

void CTelnetView::OnMouseMove(GdkEventMotion* evt)
{
  if( !m_pTermData )
    return;

  int x = (int)evt->x;
  int y = (int)evt->y;
  bool left;

  INFO("x=%d, y=%d, grab=%d", x, y, HasCapture());

  this->PointToLineCol( &x, &y, &left );
  if( HasCapture() )	//	Selecting text.
    {
      if ( m_pTermData->m_Sel->m_End.row != y
	   || m_pTermData->m_Sel->m_End.col != x
	   || m_pTermData->m_Sel->m_End.left != left )
	{
	  // Always remember to hide the caret before drawing.
	  m_Caret.Hide();

	  m_pTermData->m_Sel->ChangeEnd( y, x, left, DrawCharWrapper, this );

	  // Show the caret again but only set its visibility without
	  // display it immediatly.
	  m_Caret.Show( false );
#ifdef USE_MOUSE
	  {gdk_window_set_cursor(m_Widget->window, NULL);m_CursorState=0;}
#endif
	}
    }
#if !defined(MOZ_PLUGIN)
  else
  {
    CTermCharAttr* pattr = m_pTermData->GetLineAttr(m_pTermData->m_Screen[ y ]);

#if defined(USE_IPLOOKUP)
    // Update status bar for ip address lookup.
    m_pParentFrame->PopStatus("show ip");
    if( x > 0 && x < m_pTermData->m_ColsPerPage && pattr[x].IsIpAddr() )
    {
      int ip_beg, ip_end;
      for (ip_beg = x; ip_beg >= 0 && pattr[ip_beg].IsIpAddr(); ip_beg--);
      ip_beg++;
      for (ip_end = x; ip_end < m_pTermData->m_ColsPerPage && pattr[ip_end].IsIpAddr(); ip_end++);
      string ipstr(m_pTermData->m_Screen[y] + ip_beg, ip_end - ip_beg);
      string::iterator star = find(ipstr.begin(), ipstr.end(), '*');
      while (star != ipstr.end())
      {
	*star = '0';
	star = find(star + 1, ipstr.end(), '*');
      }

      char buf[255];
      if (m_pIpSeeker)
      {
	seeker_lookup(m_pIpSeeker, ipstr2int(ipstr.c_str()), buf, sizeof(buf));
	gchar *location = g_convert_with_fallback(buf, -1, "utf8", "gbk", "?", NULL, NULL, NULL);
	snprintf(buf, sizeof(buf), "%s %s (%s)"
	    , _("Detected IP address:"), ipstr.c_str(), location);
	g_free(location);
      }
      else
	snprintf(buf, sizeof(buf), "%s %s (%s)"
	    , _("Detected IP address:"), ipstr.c_str()
	    , _("Download qqwry.dat to get IP location lookup"));

      m_pParentFrame->PushStatus("show ip", buf);
    }
#endif // defined(USE_IPLOOKUP)

#if defined(USE_MOUSE)
    if ( AppConfig.MouseSupport == true )
    {
      if( x > 0 && x < m_pTermData->m_ColsPerPage && pattr[x].IsHyperLink() )
      {gdk_window_set_cursor(m_Widget->window, m_HandCursor);m_CursorState=-1;}
      else
      {
	switch( ((CTelnetCon*)m_pTermData)->GetPageState() )
	{
	  case -1: //NORMAL
	    gdk_window_set_cursor(m_Widget->window, NULL);
	    m_CursorState=0;
	    break;
	  case 1: //LIST
	    if ( y>2 && y < m_pTermData->m_RowsPerPage-1 )
	    {
	      if ( x <= 6 )
	      {gdk_window_set_cursor(m_Widget->window, m_ExitCursor);m_CursorState=1;}
	      else if ( x >= m_pTermData->m_ColsPerPage-16 )
	      {
		if ( y > m_pTermData->m_RowsPerPage /2 )
		{gdk_window_set_cursor(m_Widget->window, m_PageDownCursor);m_CursorState=3;}
		else
		{gdk_window_set_cursor(m_Widget->window, m_PageUpCursor);m_CursorState=4;}
	      }
	      else
	      {gdk_window_set_cursor(m_Widget->window, m_BullsEyeCursor);m_CursorState=2;}
	    }
	    else if ( y==1 || y==2 )
	    {gdk_window_set_cursor(m_Widget->window, m_PageUpCursor);m_CursorState=4;}
	    else if ( y==0 )
	    {gdk_window_set_cursor(m_Widget->window, m_HomeCursor);m_CursorState=6;}
	    else //if ( y = m_pTermData->m_RowsPerPage-1)
	    {gdk_window_set_cursor(m_Widget->window, m_EndCursor);m_CursorState=5;}
	    break;
	  case 2: //READING
	    if ( y == m_pTermData->m_RowsPerPage-1)
	    {gdk_window_set_cursor(m_Widget->window, m_EndCursor);m_CursorState=5;}
	    else if ( x<7 )
	    {gdk_window_set_cursor(m_Widget->window, m_ExitCursor);m_CursorState=1;}
	    else if ( y < (m_pTermData->m_RowsPerPage-1)/2 )
	    {gdk_window_set_cursor(m_Widget->window, m_PageUpCursor);m_CursorState=4;}
	    else
	    {gdk_window_set_cursor(m_Widget->window, m_PageDownCursor);m_CursorState=3;}
	    break;
	  case 0: //MENU
	    if ( y>0 && y < m_pTermData->m_RowsPerPage-1 )
	    {
	      if (x>7)
	      {gdk_window_set_cursor(m_Widget->window, m_BullsEyeCursor);m_CursorState=2;}
	      else
	      {gdk_window_set_cursor(m_Widget->window, m_ExitCursor);m_CursorState=1;}
	    }
	    else
	    {gdk_window_set_cursor(m_Widget->window, NULL);m_CursorState=0;}
	    break;
	  default:
	    break;
	}
      }
    }
    else
    {
      CTermCharAttr* pattr = m_pTermData->GetLineAttr(m_pTermData->m_Screen[ y ]);
      if( x > 0 && x < m_pTermData->m_ColsPerPage && pattr[x].IsHyperLink() )
	gdk_window_set_cursor(m_Widget->window, m_HandCursor);
      else
	gdk_window_set_cursor(m_Widget->window, NULL);;
      m_CursorState=0;
    }
#endif // defined(USE_MOUSE)
  }
#endif // !defined(MOZ_PLUGIN)
}

#if defined(USE_MOUSE) && !defined(MOZ_PLUGIN)
void CTelnetView::OnMouseScroll(GdkEventScroll* evt)
{
	if( !m_pTermData )
		return;

	if ( AppConfig.MouseSupport != true )
		return;

	GdkScrollDirection i = evt->direction;;
	if ( i == GDK_SCROLL_UP )
		GetCon()->SendString("\x1bOA");
	if ( i == GDK_SCROLL_DOWN )
		GetCon()->SendString("\x1bOB");
}

void CTelnetView::OnMButtonDown(GdkEventButton* evt)
{
	if ( AppConfig.MouseSupport != true )
	{
		PasteFromClipboard(true);
		return;
	}

	if ( AppConfig.WithMiddleButton != true )
		return;

	CTermCharAttr* pAttr = m_pTermData->GetLineAttr(
			m_pTermData->m_Screen[m_pTermData->m_CaretPos.y] );
	int x = m_pTermData->m_CaretPos.x;
	LeaveWrapper(GetCon(), pAttr, x);
}

void CTelnetView::OnLButtonUp(GdkEventButton* evt)
{
	CTermView::OnLButtonUp(evt);

	if( !m_pTermData )
		return;

	if ( AppConfig.MouseSupport != true )
		return;

	int x = (int)evt->x;
	int y = (int)evt->y;
	bool left;
	this->PointToLineCol( &x, &y, &left );

  int start, end;
  // Don't send mouse action when the user click on hyperlinks
  if( HyperLinkHitTest( x, y, &start, &end ) )
		  return;

	//some text is selected
	if ( m_CancelSel
	     || m_pTermData->m_Sel->m_End.row != y
	     || m_pTermData->m_Sel->m_End.col != x
	     || m_pTermData->m_Sel->m_End.left != left
	     || m_pTermData->m_Sel->m_Start.row != y
	     || m_pTermData->m_Sel->m_Start.col != x
	     || m_pTermData->m_Sel->m_Start.left != left )
	  return;

	if ( AppConfig.WithMiddleButton == true ){
		CTermCharAttr* pAttr = m_pTermData->GetLineAttr(
				m_pTermData->m_Screen[m_pTermData->m_CaretPos.y] );
		int x = m_pTermData->m_CaretPos.x;
		EnterWrapper(GetCon(), pAttr, x);
	}
	else
	{
	  int cur = m_CursorState;
	  int ps = ((CTelnetCon*)m_pTermData)->GetPageState();

	  if ( cur == 2 ) // mouse on entering mode
	  {
		switch (ps)
		{
	      case 1: // list
		  {
		    int n = y - m_pTermData->m_CaretPos.y;
		    if ( n>0 )
		      while(n)
		      {
				GetCon()->SendString("\x1bOB");
				n--;
		      }
		    if ( n<0 )
		    {
			  n=-n;
		      while(n)
			  {
			    GetCon()->SendString("\x1bOA");
			    n--;
			  }
		    }
			GetCon()->SendString("\r"); //return key
			break;
		  }
		  case 0: // menu
		  {
			char cMenu = ((CTelnetCon*)m_pTermData)->GetMenuChar(y);
			GetCon()->SendRawString( &cMenu, 1 );
			GetCon()->SendString("\r");
			break;
		  }
		  case -1: // normal
			GetCon()->SendString("\r");
		  break;
			default:
		  break;
		}
	  }
	  else if (cur == 1)
		GetCon()->SendString("\x1bOD"); //exiting mode
	  else if (cur == 6)
		GetCon()->SendString("\x1b[1~"); //home
	  else if (cur == 5)
		GetCon()->SendString("\x1b[4~"); //end
	  else if (cur == 4)
		GetCon()->SendString("\x1b[5~"); //pageup
	  else if (cur == 3)
		GetCon()->SendString("\x1b[6~"); //pagedown
	  else
		GetCon()->SendString("\r");
	}
}
#endif  // defined(USE_MOUSE) && !defined(MOZ_PLUGIN)

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
			GtkWidget* icon = gtk_image_new_from_stock (GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
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
						"utf-8", m_pTermData->m_Encoding.c_str(), (gchar *) "?", NULL, &wl, NULL);
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
	if( input_menu_item )
		gtk_widget_destroy( input_menu_item );
	input_menu_item = gtk_menu_item_new_with_mnemonic (_("Input _Methods"));
	gtk_widget_show (input_menu_item);
	GtkWidget* submenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (input_menu_item), submenu);

	// Show Web Search only when text selected
	if(websearch_menu_item)
		gtk_widget_destroy(websearch_menu_item);
	string selected_text = m_pTermData->GetSelectedText(false);
	if(! selected_text.empty()) {
		gsize wl = 0;
		gchar websearch_text[128];
		gchar* search_content = g_convert_with_fallback(
							selected_text.c_str(), selected_text.length(),
							"utf-8", m_pTermData->m_Encoding.c_str(),
							(gchar *) "?", NULL, &wl, NULL);
		if(g_utf8_strlen(search_content, selected_text.length()) > 15) {
			g_utf8_strncpy(search_content, search_content, 15);
			strcat(search_content, _("..."));
		}
		websearch_menu_item = gtk_menu_item_new_with_mnemonic(_("_Web Search: \"%s\""));
		sprintf(websearch_text, gtk_menu_item_get_label(GTK_MENU_ITEM(websearch_menu_item)), search_content);
		gtk_menu_item_set_label(GTK_MENU_ITEM(websearch_menu_item), websearch_text);

		gtk_widget_show(websearch_menu_item);
		g_signal_connect(G_OBJECT(websearch_menu_item), "activate", G_CALLBACK(CTelnetView::OnWebSearch), this);
		gtk_menu_shell_append(GTK_MENU_SHELL(m_ContextMenu), websearch_menu_item );
		g_free(search_content);
	}

	gtk_menu_shell_append (GTK_MENU_SHELL (m_ContextMenu), input_menu_item);

	gtk_im_multicontext_append_menuitems (GTK_IM_MULTICONTEXT (m_IMContext),
					GTK_MENU_SHELL (submenu));
	gtk_menu_popup( m_ContextMenu, NULL, NULL, NULL, NULL, evt->button, evt->time );
#endif
}

bool CTelnetView::PreKeyDown(GdkEventKey* evt UNUSED)
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
			if( m_s_CharSet != GetCon()->m_Site.m_Encoding.c_str() )
			{
			  INFO("Charset Conversion from %s to %s",m_s_CharSet.c_str(),GetCon()->m_Site.m_Encoding.c_str());

			  gsize convl;
			  gchar* locale_text = g_convert(text.c_str(), text.length(),GetCon()->m_Site.m_Encoding.c_str(),m_s_CharSet.c_str(), NULL, &convl, NULL);
			  if( !locale_text )
				return;

			  const char* p = locale_text;
			  while(*p)
			  {
				if(*p == '\x1b')
					text2 += esc;
				else
					text2 += *p;
				p++;
			  }
			  g_free(locale_text);
			}
			else
			{
				INFO("color text: %s",text.c_str());
				const char* p = text.c_str();
				const char* crlf = GetCon()->m_Site.GetCRLF();
				while(*p)
				{
					if(*p == '\x1b')
						text2 += esc;
					else if( *p == '\n' )
						text2 += crlf;
					else
						text2 += *p;
					p++;
				}
			}
			GetCon()->SendRawString(text2.c_str(),text2.length());
		}
		else
		{
			// Only when no control character is in this string can
			// autowrap be enabled
			unsigned int len = 0, max_len = GetCon()->m_Site.m_AutoWrapOnPaste;
			gsize convl = 0;
			gchar* locale_text = NULL;

			/* UAO paste support */
			switch (m_UAO) {
				case 2:
					locale_text = uao250_u2b(text.c_str(), 0, &convl);
					break;
				case 1:
					locale_text = uao241_u2b(text.c_str(), 0, &convl);
					break;
				default:
					locale_text = g_convert(text.c_str(), text.length(), GetCon()->m_Site.m_Encoding.c_str(), "UTF-8", NULL, &convl, NULL);
					break;
			}
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

void CTelnetView::OnHyperlinkClicked(string sURL)
{
	gchar *cmdAndURL[3] = {0, 0, 0};
	GError *err = NULL;

#if !defined(MOZ_PLUGIN)
	if( 0 == strncmpi( sURL.c_str(), "telnet:", 7) )
	{
		const char* psURL = sURL.c_str() + 7;
		while( *psURL == '/' )
			++psURL;
		if( !*psURL )
			return;
		sURL = psURL;
		if( '/' == sURL[sURL.length()-1] )
			sURL = string( sURL.c_str(), 0, sURL.length()-1 );
		m_pParentFrame->NewCon( sURL, sURL );
		return;
	}
#endif /* !defined(MOZ_PLUGIN) */

	string app;
	if( !strstr( sURL.c_str(), "://") && strchr(sURL.c_str(), '@'))
	{
		app = m_MailClient;
		if( strncmpi( sURL.c_str(), "mailto:", 7 ) )
			sURL.insert( 0, "mailto:" );
	}
	else
		app = m_WebBrowser;

	// Remove %s for backward compatibility, the legacy setting is "xdg-open %s"
	size_t legacyAppSymOffset = string::npos;

	legacyAppSymOffset = app.find(" %s");
	if ( legacyAppSymOffset != string::npos)
	{
		app.erase(legacyAppSymOffset, 3);
	}

	// Launch app
	INFO("Launch app with URL: %s %s", app.c_str(), sURL.c_str());
	cmdAndURL[0] = (gchar *) app.c_str();
	cmdAndURL[1] = (gchar *) sURL.c_str();

	bool rval = g_spawn_async(NULL, cmdAndURL, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &err);
	if (!rval)
	{
		g_print("can not run %s: %s\n", app.c_str(), err->message);
	}
}

void CTelnetView::OnWebSearch(GtkMenuItem* mitem UNUSED, CTelnetView* _this)
{
	_this->OnWebSearchSelected();
}

#define  SEARCH_URL ("http://www.google.com.tw/search?&ie=UTF-8&q=")
void CTelnetView::OnWebSearchSelected()
{
	gchar *cmdAndURL[3] = {0, 0, 0};
	GError *err = NULL;
	string selectedText = CTermView::m_pTermData->GetSelectedText(false);

	// Convert to utf8
    // FIXME: Search keyword (Big5-UAO) may not be converted to UTF8 correctly
	gsize wl = 0;
	gchar* selectedTextUTF8 = g_convert_with_fallback(
				   selectedText.c_str(), selectedText.length(),
				   "utf-8", m_pTermData->m_Encoding.c_str(),
				   (gchar *) "?", NULL, &wl, NULL);

	if (selectedTextUTF8 == NULL)
	{
		   return;
	}

	INFO("Try to OnWebSearchSelected: %s (%d)", selectedTextUTF8, (int) g_utf8_strlen(selectedTextUTF8, -1) );

	// Compose URL
	string searchURL;
	searchURL.append(SEARCH_URL);
	searchURL.append(selectedTextUTF8);

	string app = m_WebBrowser;

	// Remove %s for backward compatibility.
	// the legacy setting is "xdg-open %s"
	size_t legacyAppSymOffset = string::npos;

	legacyAppSymOffset = app.find(" %s");
	if ( legacyAppSymOffset != string::npos)
	{
		// copy on write, will not pollute parent m_WebBrowser
		app.erase(legacyAppSymOffset, 3);
	}

	INFO("Seach App with URL: %s %s", app.c_str(), searchURL.c_str());

	// Launch browser
	cmdAndURL[0] = (gchar *) app.c_str();
	cmdAndURL[1] = (gchar *) searchURL.c_str();

	bool rval = g_spawn_async(NULL, cmdAndURL, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &err);
	if (!rval)
	{
		g_print("can not run %s: %s\n", app.c_str(), err->message);
	}

	g_free((void*)selectedTextUTF8);
}


/* vim: set fileencodings=utf-8 tabstop=4 noexpandtab shiftwidth=4 softtabstop=4: */
