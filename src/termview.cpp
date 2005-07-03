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
#include "termview.h"
#include "termdata.h"


#include <string>

#include <gdk/gdkx.h>
#include <pango/pangoxft.h>
#include "font.h"

#include "appconfig.h"	//	FIXME: This should be removed in the future.


#if 0
XftFont* xft_font_from_pango_font_desc( GdkDrawable* drawable, PangoFontDescription* pangofd )
{
	if(pango_font_description_get_size(pangofd) != 0)
	{
		XftFont* xft_font = NULL;

		Display *xdisplay = GDK_WINDOW_XDISPLAY(drawable);
		int weight, slant, screen = DefaultScreen (xdisplay);

		weight = pango_font_description_get_weight (pangofd);
		/* from pangoft2-fontmap.c */
		if (weight < (PANGO_WEIGHT_NORMAL + PANGO_WEIGHT_LIGHT) / 2)
			weight = XFT_WEIGHT_LIGHT;
		else if (weight < (PANGO_WEIGHT_NORMAL + 600) / 2)
			weight = XFT_WEIGHT_MEDIUM;
		else if (weight < (600 + PANGO_WEIGHT_BOLD) / 2)
			weight = XFT_WEIGHT_DEMIBOLD;
		else if (weight < (PANGO_WEIGHT_BOLD + PANGO_WEIGHT_ULTRABOLD) / 2)
			weight = XFT_WEIGHT_BOLD;
		else
			weight = XFT_WEIGHT_BLACK;

		slant = pango_font_description_get_style (pangofd);
		if (slant == PANGO_STYLE_ITALIC)
			slant = XFT_SLANT_ITALIC;
		else if (slant == PANGO_STYLE_OBLIQUE)
			slant = XFT_SLANT_OBLIQUE;
		else
			slant = XFT_SLANT_ROMAN;

		xft_font = XftFontOpen (xdisplay, screen,
						XFT_FAMILY, XftTypeString, pango_font_description_get_family(pangofd),
						/*XFT_ENCODING, XftTypeString, "glyphs-fontspecific",*/
						XFT_CORE, XftTypeBool, False,
						XFT_SIZE, XftTypeDouble, (double)pango_font_description_get_size (pangofd)/PANGO_SCALE,
						XFT_WEIGHT, XftTypeInteger, weight,
						XFT_SLANT, XftTypeInteger, slant,
						NULL);
		if( xft_font )
			return xft_font;
	}
	return NULL;
}


XftFont* xft_font_from_pango_font_desc_string( GdkDrawable* drawable, const char* name )
{
	PangoFontDescription* pangofd = pango_font_description_from_string( name );
	XftFont* xft_font = xft_font_from_pango_font_desc( drawable, pangofd );
	pango_font_description_free( pangofd );
	return xft_font;
}
#endif

using namespace std;

static gboolean on_key_pressed(GtkWidget* wnd, GdkEventKey *evt, CTermView* _this)
{
	bool ret = gtk_im_context_filter_keypress(_this->m_IMContext, evt );
	if( !_this->PreKeyDown(evt) && !ret )
		ret = _this->OnKeyDown(evt);
	return ret;
}

static void on_im_commit(GtkIMContext *im, gchar *arg, CTermView* _this)
{
	_this->OnTextInput(arg);
}

static gboolean on_mouse_down(GtkWidget* widget, GdkEventButton* evt, CTermView* _this)
{
	switch(evt->button)
	{
	case 1:
		_this->OnLButtonDown(evt);
		break;
	case 2:
		_this->OnMButtonDown(evt);
		break;
	case 3:
		_this->OnRButtonDown(evt);
	}
	return true;
}

static gboolean on_mouse_up(GtkWidget* widget, GdkEventButton* evt, CTermView* _this)
{
	switch(evt->button)
	{
	case 1:
		_this->OnLButtonUp(evt);
		break;
	case 3:
		_this->OnRButtonUp(evt);
	}
	return true;
}

static gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion* evt, CTermView* _this)
{
	if (evt->is_hint)
	{
		int x, y;	GdkModifierType state;
    	gdk_window_get_pointer (evt->window, &x, &y, &state);
		evt->x = x;	evt->y = y;
		evt->state = state;
	}
	_this->OnMouseMove(evt);
	return true;
}

CTermView::CTermView()
        : CView(), m_pColorTable(CTermCharAttr::GetDefaultColorTable())
{
	m_pTermData = NULL;
	m_GC = NULL;
	m_ShowBlink = false;
	m_Font = NULL;
	m_XftDraw = NULL;

//	m_PangoLayout = gtk_widget_create_pango_layout(m_Widget, NULL);

	gtk_widget_add_events(m_Widget, GDK_EXPOSURE_MASK
		 | GDK_KEY_PRESS_MASK
		 | GDK_BUTTON_PRESS_MASK
		 | GDK_BUTTON_MOTION_MASK
		 | GDK_BUTTON_RELEASE_MASK
		 | GDK_POINTER_MOTION_MASK
		 | GDK_POINTER_MOTION_HINT_MASK
		 | GDK_ALL_EVENTS_MASK);


	GTK_WIDGET_SET_FLAGS(m_Widget, GTK_CAN_FOCUS);
	gtk_widget_set_double_buffered(m_Widget, false);

	g_signal_connect(G_OBJECT(m_Widget), "key_press_event", G_CALLBACK(on_key_pressed), this);

	g_signal_connect(G_OBJECT(m_Widget), "button_press_event", G_CALLBACK(on_mouse_down), this);

	g_signal_connect(G_OBJECT(m_Widget), "button_release_event", G_CALLBACK(on_mouse_up), this);

	g_signal_connect(G_OBJECT(m_Widget), "motion_notify_event", G_CALLBACK(on_mouse_move), this);

	m_CharPaddingX = m_CharPaddingY = 0;
	m_AutoFontSize = true;

	m_IMContext = gtk_im_multicontext_new();
	gtk_im_context_set_use_preedit( m_IMContext, FALSE );
	g_signal_connect( G_OBJECT(m_IMContext), "commit", G_CALLBACK(on_im_commit), this );

}


CTermView::~CTermView()
{
//	if( m_PangoLayout )
//		g_object_unref( G_OBJECT(m_PangoLayout) );
//	if( m_GC )
//		g_object_unref( G_OBJECT(m_GC) );
//	if( m_IMContext )
//		g_object_unref( G_OBJECT(m_IMContext) );
//	if( m_Font )
//		pango_font_description_free(m_Font);
	if( m_Font )
		delete m_Font;
	XftDrawDestroy(m_XftDraw);
	m_XftDraw = NULL;
	g_print("XftDrawDestroy\n");
}

void CTermView::OnPaint(GdkEventExpose* evt)
{
	// Hide the caret to prevent drawing problems.
	m_Caret.Hide();

	PrepareDC();

	GdkDrawable* dc = m_Widget->window;
	if(!GDK_IS_DRAWABLE(dc))
	{
		g_print("WARNNING! Draw on DELETED widget!\n");
		return;
	}

	int w = m_Widget->allocation.width, h = m_Widget->allocation.height;

	if( m_pTermData )
	{
		int top = 0, left = 0;

		int iline = m_pTermData->m_FirstLine;
		int ilast = m_pTermData->m_FirstLine+m_pTermData->m_RowsPerPage-1;

/*		int top = evt->area.y;		int bottom = top + evt->area.height;
		int left = evt->area.x;		int right = left + evt->area.width;
		this->PointToLineCol( &left, &top );
		this->PointToLineCol( &right, &bottom );

		if(right < m_pTermData->m_ColsPerPage)	right++;
		if(left > 0)	left-=left>1?2:1;
		if(bottom < m_pTermData->m_RowsPerPage-1)	bottom++;
		if(top > 0)	top-=top>1?2:1;
		int iline = m_pTermData->m_FirstLine + top;
		int ilast = m_pTermData->m_FirstLine + bottom;
		top *= m_CharH;
*/
		for( ; iline <= ilast; iline++ , top += m_CharH )
		{
			for( int col = 0; col < m_pTermData->m_ColsPerPage; )
//			for( int col = left; col < right; )
				col += DrawChar( iline, col, top );
		}

		gdk_gc_set_rgb_fg_color(m_GC, CTermCharAttr::GetDefaultColorTable(0));
		left = m_pTermData->m_ColsPerPage*m_CharW-2;

		gdk_draw_rectangle(dc, m_GC, true, left, 0, w-left, h );

		top = m_pTermData->m_RowsPerPage*m_CharH;
		gdk_draw_rectangle(dc, m_GC, true, 0, top, w, h-top );

		m_Caret.Show();
	}
	else
	{
		gdk_gc_set_rgb_bg_color(m_GC, CTermCharAttr::GetDefaultColorTable(0));
		gdk_draw_rectangle(dc, m_GC, true,0, 0, w, h );
	}

}

void CTermView::OnSetFocus(GdkEventFocus* evt)
{
	gtk_im_context_focus_in(m_IMContext);
}


bool CTermView::OnKeyDown(GdkEventKey* evt)
{
	return true;
}


void CTermView::OnTextInput(gchar* string)
{
    /// @todo implement me
}

void CTermView::OnCreate()
{
	if(!GDK_IS_DRAWABLE(m_Widget->window))
	{
		g_print("Draw on DELETED widget!\n");
		return;
	}
	CWidget::OnCreate();
	gtk_im_context_set_client_window(m_IMContext, m_Widget->window);

	m_XftDraw = XftDrawCreate(
		GDK_WINDOW_XDISPLAY(m_Widget->window),
		GDK_WINDOW_XWINDOW(m_Widget->window),
		GDK_VISUAL_XVISUAL(gdk_drawable_get_visual(m_Widget->window)),
		GDK_COLORMAP_XCOLORMAP (gdk_drawable_get_colormap(m_Widget->window)));
	XftDrawSetSubwindowMode(m_XftDraw, IncludeInferiors);
	g_print("Create\n");

	if( !m_Font )
		m_Font = new CFont("Sans", 18);

	m_GC = gdk_gc_new(m_Widget->window);
	gdk_gc_copy(m_GC, m_Widget->style->black_gc);

	m_Caret.Create(this, m_GC);
	m_Caret.Show();
}


int CTermView::DrawChar(int line, int col, int top)
{
	GdkDrawable* dc = m_Widget->window;
	if(!GDK_IS_DRAWABLE(dc) && m_XftDraw == NULL)
	{
		g_print("Draw on DELETED widget!\n");
		return 1;
	}

	const char* pLine = m_pTermData->m_Screen[line];
	CTermCharAttr* pAttr = m_pTermData->GetLineAttr(pLine);
	int w = 2;
	switch( pAttr[col].GetCharSet() )
	{
	case CTermCharAttr::CS_MBCS1:
		break;
	case CTermCharAttr::CS_MBCS2:
		col--;
//		This will not cause any problem at any time because 'col' always > 0.
//		In CTermData_this->DetectCharSets() I've done some checks to ensure that the first
//		character of every lines cannot be marked as second bytes of MBCS characters.
		break;
	default:
//	case CTermCharAttr::CS_ASCII:
		w = 1;
	}

	const char* pChar = pLine + col;
	pAttr += col;
	int left = m_CharW*col;

	bool bSel[2];	bSel[0]= IsPosInSel( col, line );
	if( w > 1 )		bSel[1] = IsPosInSel( col+1, line );

	GdkColor iFg, iBg;

	for( int i=0; i < w; i++ )	//	do the drawing
	{
		GdkColor* Fg = pAttr[i].GetFgColor( m_pColorTable );
		GdkColor* Bg = pAttr[i].GetBgColor( m_pColorTable );
		// if it's property is inverse, GetFgColor & GetBgColor will swap Bg & Fg for us.

		if( bSel[i] )	// if it's selected, reverse two colors.
		{
			iFg.red = ~Fg->red;
			iFg.green = ~Fg->green;
			iFg.blue = ~Fg->blue;
			Fg = &iFg;
			iBg.red = ~Bg->red;
			iBg.green = ~Bg->green;
			iBg.blue = ~Bg->blue;
			Bg = &iBg;

		}

		gdk_gc_set_rgb_fg_color( m_GC, Bg );


		
		XftColor xftclr;
		xftclr.pixel = 0;
		xftclr.color.red = Fg->red;
		xftclr.color.green = Fg->green;
		xftclr.color.blue = Fg->blue;
		xftclr.color.alpha = 0xffff;

		int bgw = m_CharW*w;
		gdk_draw_rectangle(dc, m_GC, true, left, top, bgw, m_CharH );

		gdk_gc_set_rgb_fg_color( m_GC, Fg );

		if( !pAttr[i].IsBlink() || m_ShowBlink )	// If text shold be draw.
		{
			if( ' ' != *pChar && '\0' != *pChar )
			{
				gsize wl;
//				gchar* utf8_ch = g_locale_to_utf8(pChar, w, NULL, &wl, NULL);
				//TODO Encoding should be set from config somewhere, not from locale, nor default to Big5.
				gchar *utf8_ch = g_convert(pChar, w, "UTF-8", "Big5", NULL, &wl, NULL);
				if( w > 0 && utf8_ch )
				{
//					pango_layout_set_text(m_PangoLayout, utf8_ch, -1 );
//					gdk_draw_layout( dc, m_GC, left, top, m_PangoLayout );

//				pango_xft_render_layout(m_XftDraw, &xftclr, m_PangoLayout, left, top);
				XftFont* font = m_Font->GetXftFont();
				XftDrawStringUtf8(m_XftDraw, &xftclr, font, left, top + font->ascent, (FcChar8*)utf8_ch, strlen(utf8_ch));
					g_free(utf8_ch);
				}
			}
			if( pAttr[i].IsUnderLine() )
			{
//				dc.SetPen(wxPen(Fg, 1, wxSOLID));
//				int bottom = top + m_CharH - 1;
//				dc.DrawLine(left, bottom, left+bgw, bottom);
				int bottom = top + m_CharH - 1;
				gdk_draw_line(dc, m_GC, left, bottom, left+bgw, bottom);
			}
		}
		// 2004.08.07 Added by PCMan: Draw the underline of hyperlinks.
		if( pAttr[i].IsHyperLink() ) 
		{
//			dc.SetPen(wxPen(m_HyperLinkColor, 1, wxSOLID));
//			int bottom = top + m_CharH - 1;
//			dc.DrawLine(left, bottom, left+bgw, bottom);

// 			gdk_gc_set_rgb_fg_color( m_GC, m_HyperLinkColor );
			int bottom = top + m_CharH - 1;
			gdk_draw_line(dc, m_GC, left, bottom, left+bgw, bottom);
		}

		if( w == 1 || i>=1 || (pAttr[i].IsSameAttr(pAttr[i+1].AsShort()) && bSel[0] == bSel[1]) )
			break;
	
		gdk_gc_set_clip_rectangle( m_GC, NULL);
		GdkRectangle rect;	rect.x=(left + m_CharW); rect.y=top; rect.width=m_CharW; rect.height=m_CharH;
		gdk_gc_set_clip_origin( m_GC, rect.x, rect.y );
		gdk_gc_set_clip_rectangle( m_GC, &rect);
	}
	gdk_gc_set_clip_rectangle( m_GC, NULL);
	return w;
}


bool CTermView::IsPosInSel(int x, int y)
{
//  If the user selected from bottom to top or from right to left, we should swap the
//	coordinates to make them correct.
	int selstartx = m_pTermData->m_SelStart.x, selstarty = m_pTermData->m_SelStart.y;
	int selendx = m_pTermData->m_SelEnd.x, selendy = m_pTermData->m_SelEnd.y;
	bool one_line_selected = m_pTermData->m_SelStart.y == selendy;
/*	if( selstarty > selendy )
	{
		int tmp = selstarty; selstarty = selendy; selendy = tmp;
		if( selstartx > selendx )
		{	int tmp = selstartx; selstartx = selendx; selendx = tmp;	}
	}
	else if( (one_line_selected||m_pTermData->m_SelBlock) && selstartx > selendx )
	{	int tmp = selstartx; selstartx = selendx; selendx = tmp;	}	*/
//-------------------------------------------------------------------
	CorrectSelPos(selstartx, selstarty, selendx, selendy );
//			Check if specified position is in selected area;
	if( y < selstarty || y > selendy )
		return false;

	if( m_pTermData->m_SelBlock )	// If selected area is a block
		return (x >= selstartx && x < selendx );
	else	// normal selection with multiple lines selected
	{
		bool x_larger_than_startx = (x >= selstartx);
		bool x_smaller_than_endx = (x < selendx);
		if( one_line_selected )	// if only one line is selected
			return ( x_larger_than_startx && x_smaller_than_endx );
		if( y == selstarty )	// If in first selected line
			return x_larger_than_startx;
		else if( y == selendy )	// If in last selected line
			return x_smaller_than_endx;
	}
	return true;
}


void CTermView::CorrectSelPos(int &selstartx, int &selstarty, int &selendx, int &selendy)
{
//  If the user selected from bottom to top or from right to left, we should swap the
//	coordinates to make them correct.
	bool one_line_selected = m_pTermData->m_SelStart.y == selendy;
	if( selstarty > selendy )
	{
		int tmp = selstarty; selstarty = selendy; selendy = tmp;
		tmp = selstartx; selstartx = selendx; selendx = tmp;
	}
	if( (one_line_selected ||m_pTermData->m_SelBlock) && selstartx > selendx )
	{	int tmp = selstartx; selstartx = selendx; selendx = tmp;	}
}

void CTermView::PrepareDC()
{
//	pango_layout_set_text(m_PangoLayout, " ", 1);

	int w=0, h=0;
//	pango_layout_get_pixel_size(m_PangoLayout, &w, &h);

//	XGlyphInfo inf;
//	XftTextExtentsUtf8(GDK_WINDOW_XDISPLAY(m_Widget->window), m_XftFont, (FcChar8*)" ", 1, &inf);
//	w = inf.xOff;
	w = m_Font->GetMaxWidth();
	h = m_Font->GetHeight();

	m_CharW = w/2 + m_CharPaddingX + (w % 2 ? 0 : 1);
//	m_CharW = h/2 + m_CharPaddingX + (h % 2 ? 0 : 1);
	m_CharH = h + m_CharPaddingY;

	gdk_gc_set_clip_origin( m_GC, 0, 0 );
	gdk_gc_set_clip_rectangle( m_GC, NULL);
}


void CTermView::PointToLineCol(int *x, int *y)
{
	*x /= m_CharW;
	if (*x < 0)
		*x = 0;
	else if( *x > m_pTermData->m_ColsPerPage )
		*x = m_pTermData->m_ColsPerPage;

	*y /= m_CharH;
	if(*y <0 )
		*y = 0;
	else if( (*y+1) > m_pTermData->m_RowsPerPage )
		*y = m_pTermData->m_RowsPerPage-1;
}

void CTermView::OnSize(GdkEventConfigure* evt)
{
	GdkRectangle rc;
	rc.x = evt->x;	rc.y = evt->y; 	rc.width = 	evt->width;	rc.height = evt->height;
	gtk_im_context_set_cursor_location(m_IMContext, &rc);

	if( !m_AutoFontSize || !m_pTermData )
		return;

//	pango_layout_set_text(m_PangoLayout, " ", 1);

	int desire_w = (evt->width / m_pTermData->m_ColsPerPage) - m_CharPaddingX;
	int desire_h = (evt->height / m_pTermData->m_RowsPerPage) - m_CharPaddingY;

	m_Font->SetFont(m_Font->GetName(), desire_w, desire_h);
//    PangoFontDescription* font = pango_font_description_copy(m_Font);

/*		Display *display = GDK_WINDOW_XDISPLAY(m_Widget->window);
		int screen = DefaultScreen (display);

		if( m_XftFont )
			XftFontClose(display, m_XftFont);

		m_XftFont = XftFontOpen (display, screen,
						XFT_FAMILY, XftTypeString, m_FontFamily.c_str(),
						XFT_CORE, XftTypeBool, False,
						XFT_PIXEL_SIZE, XftTypeDouble, (double)desire_h,
						XFT_WEIGHT, XftTypeInteger, XFT_WEIGHT_MEDIUM,
						XFT_ANTIALIAS, XftTypeBool, True,
						NULL);

		int w = m_XftFont->max_advance_width/2;
		int h = m_XftFont->ascent + m_XftFont->descent;

		while( (w > 2 && h > 2) && ( w > desire_w*2 || h > desire_h) )
		{
			if( m_XftFont )
				XftFontClose(display, m_XftFont);

			h-=2;

			m_XftFont = XftFontOpen (display, screen,
							XFT_FAMILY, XftTypeString, m_FontFamily.c_str(),
							XFT_CORE, XftTypeBool, False,
							XFT_PIXEL_SIZE, XftTypeDouble, (double)h,
							XFT_WEIGHT, XftTypeInteger, XFT_WEIGHT_MEDIUM,
							XFT_ANTIALIAS, XftTypeBool, True,
							NULL);

			w = m_XftFont->max_advance_width;
			h = m_XftFont->ascent + m_XftFont->descent;
		}*/
/*	int max = 40, min = 6, w = 0, h = 0;
	int point = 6 + (40-6)/2;
	while( max > point && point > min )
	{
		pango_font_description_set_size(m_Font, point*PANGO_SCALE);
		pango_layout_set_font_description(m_PangoLayout, m_Font);
		pango_layout_get_pixel_size(m_PangoLayout, &w, &h);
		w = h/2 + (h % 2 ? 0 : 1);
		if( w < desire_w || h < desire_h )	// too narrow or too low
			min	= point;
		else if( w > desire_w || h > desire_h )	// too wide or too high
			max = point;
		else
			break;
		point = min + (max-min)/2;
	}
	pango_font_description_set_size(m_Font, min*PANGO_SCALE);
	pango_layout_set_font_description(m_PangoLayout, m_Font);
*/

	m_CharW = m_Font->GetMaxWidth()/2;// w/2 + m_CharPaddingX + (w % 2 ? 0 : 1); //	h/2 + m_CharPaddingX + (h % 2 ? 0 : 1);
	m_CharH = m_Font->GetHeight() + m_CharPaddingY;

	m_Caret.SetSize(m_CharW, 2);
	m_Caret.Show();

	m_pTermData->UpdateCaret();
//	pango_font_description_free(m_Font);
//	m_Font = font;
}


void CTermView::OnLButtonDown(GdkEventButton* evt)
{
	SetFocus();

	if( !m_pTermData )
		return;
	SetCapture();

	int x = (int)evt->x;
	int y = (int)evt->y;

//	g_print("x=%d, y=%d, grab=%d\n", x, y, HasCapture());

	PointToLineCol( &x, &y );
	bool need_refresh = ( m_pTermData->m_SelStart != m_pTermData->m_SelEnd );
	m_pTermData->m_SelStart.x = m_pTermData->m_SelEnd.x = x;
	m_pTermData->m_SelStart.y = m_pTermData->m_SelEnd.y = y;
	if( need_refresh )
		Refresh();
	m_pTermData->m_SelBlock = (evt->state &
		(GDK_SHIFT_MASK|GDK_MOD1_MASK|GDK_CONTROL_MASK) );
}


void CTermView::OnRButtonDown(GdkEventButton* evt)
{
    /// @todo implement me
}

void CTermView::OnLButtonUp(GdkEventButton* evt)
{
	if( !m_pTermData )
		return;
	ReleaseCapture();

//	Correct the selected area when users stop selecting.
//  If the user selected from bottom to top or from right to left, we should swap the
//	coordinates to make them correct.
	CorrectSelPos(m_pTermData->m_SelStart.x, m_pTermData->m_SelStart.y, 
		m_pTermData->m_SelEnd.x, m_pTermData->m_SelEnd.y);

	//	2004.08.07 Added by PCMan.  Hyperlink detection.
	//	If no text is selected, consider hyperlink.
	if( m_pTermData->m_SelStart == m_pTermData->m_SelEnd )
	{
		int x = (int)evt->x;
		int y = (int)evt->y;
		this->PointToLineCol( &x, &y );
		char* pline = m_pTermData->m_Screen[y];
		CTermCharAttr* pattr = m_pTermData->GetLineAttr(pline);
		if( x > 0 && x < m_pTermData->m_ColsPerPage && pattr[x].IsHyperLink()
		/* && m_pHyperLink */)
		{
			int start, end;
			for( start = x-1; start > 0 && pattr[start].IsHyperLink(); start-- );
			if( !pattr[start].IsHyperLink() )
				start++;
			for( end = x+1; end < m_pTermData->m_ColsPerPage && pattr[end].IsHyperLink(); end++ );
			string URL( (pline+start), (int)(end-start) );

	//	FIXME: Temporary hack. This must be rewritten in the future.
	//		system(("mozilla " + URL).c_str());
			const char* argv[] = {AppConfig.WebBrowser.c_str(), URL.c_str(), NULL};
			g_spawn_async(NULL, (gchar**)argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
//			m_pHyperLink->Open(URL);
		}
	}
	else	// if there is a selected area
	{
		CopyToClipboard(true, false);
	}

}

void CTermView::OnRButtonUp(GdkEventButton* evt)
{
    /// @todo implement me
}


void CTermView::OnMouseMove(GdkEventMotion* evt)
{
	if( !m_pTermData )
		return;

	int x = (int)evt->x;
	int y = (int)evt->y;

//	g_print("x=%d, y=%d, grab=%d\n", x, y, HasCapture());

	this->PointToLineCol( &x, &y );
	if( HasCapture() )	//	Selecting text.
	{
		int oldx = m_pTermData->m_SelEnd.x;
		int oldy = m_pTermData->m_SelEnd.y;
		//	Store new SelPos
		m_pTermData->m_SelEnd.x = x;
		m_pTermData->m_SelEnd.y = y;
		//	Update Display
		RedrawSel( oldx, oldy, x, y );
	}
	else	//	Consider hyperlink detection.
	{
		/* Todo: hyperlink detection. */
		CTermCharAttr* pattr = m_pTermData->GetLineAttr(m_pTermData->m_Screen[ y ]);
//		if( x > 0 && x < m_pTermData->m_ColsPerPage && pattr[x].IsHyperLink() )
//			SetCursor(m_HandCursor);
//		else
//			SetCursor(*wxSTANDARD_CURSOR);
	}
}


//	This function is used to update selected region.
//	However, this piece of code is not only inefficient but also ugly.
//	Although it works, by no means can this be a good solution.
//	Leaving it temporarily because I don't have much time to do optimization.
//	Maybe someday there'll be someone who can fix it .
#define iMAX(x,y) ((x)>(y)?(x):(y))
#define iMIN(x,y) ((x)>(y)?(y):(x))
#define MAX3(x,y,z) iMAX(iMAX(x,y),z)
#define MIN3(x,y,z) iMIN(iMIN(x,y),z)
void CTermView::RedrawSel(int oldx, int oldy, int newx, int newy)
{
	//	Always remember to hide the caret before drawing.
	m_Caret.Hide();

	GdkDrawable* dc = m_Widget->window;
	PrepareDC();

	int starty;	int	endy;
	if( m_pTermData->m_SelBlock )
	{
		starty = MIN3( m_pTermData->m_SelStart.y, oldy, m_pTermData->m_SelEnd.y );
		endy = MAX3(m_pTermData->m_SelStart.y, oldy, m_pTermData->m_SelEnd.y );
	}
	else
	{
		if( oldy < newy )	// select downward
		{	starty = oldy;	endy = newy;	}
		else	//	upward
		{	starty = newy;	endy = oldy;	}
	}

	// This is not efficient at all.  Apparaently it needs some optimization.
	int top = starty*m_CharH;
	for( int y= starty; y <= endy; y++, top+=m_CharH )
	{
//		CTermCharAttr* pAttr = m_pTermData->GetLineAttr(m_pTermData->m_Screen[starty]);
		for( int x=0; x< m_pTermData->m_ColsPerPage;  )
			x += DrawChar( y, x, top );
	}

	//	Show the caret again but only set its visibility without display it immediatly.
	m_Caret.Show( false );
}
#undef MAX3
#undef MIN3
#undef iMIN
#undef iMAX

void CTermView::OnKillFocus(GdkEventFocus *evt)
{
	gtk_im_context_focus_out(m_IMContext);
}

// Preview the GdkEventKey before it's sent to OnKeyDown
// regardless of the returned value of im_context_filter_key_press.
bool CTermView::PreKeyDown(GdkEventKey *evt)
{
	return false;
}

void CTermView::OnBlinkTimer()
{
	m_ShowBlink = !m_ShowBlink;
	if(m_pTermData)
	{
		PrepareDC();
		int top = 0;
		int iline = m_pTermData->m_FirstLine;
		int ilast = m_pTermData->m_FirstLine+m_pTermData->m_RowsPerPage-1;
//		const int left = 0;	const int right = m_pTermData->m_ColsPerPage;
		for( ; iline <= ilast; iline++ , top += m_CharH )
		{
//			DrawLine( dc, i, y, true , left, right );
			CTermCharAttr* attr = m_pTermData->GetLineAttr( m_pTermData->m_Screen[iline] );
			for( int col = 0; col < m_pTermData->m_ColsPerPage; )
			{
				if( attr[col].IsBlink())
					col += DrawChar( iline, col, top );
				else
					col++;
			}
		}
	}
	m_Caret.Blink();
}

void CTermView::OnMButtonDown(GdkEventButton* evt)
{
	PasteFromClipboard(true);
}


string CTermView::m_s_ANSIColorStr;

void CTermView::PasteFromClipboard(bool primary)
{
	string text;
	if(m_s_ANSIColorStr.empty())
	{
		GtkClipboard* clipboard = gtk_clipboard_get( primary ? GDK_SELECTION_PRIMARY : GDK_NONE);
//		g_print("paste\n");
		gchar* utext = gtk_clipboard_wait_for_text(clipboard);
		if( !utext )
			return;
//		g_print("%s\n", utext);
		gsize wl;

		const gchar* locale;
		const gchar* ltext = utext;
		if( !g_get_charset(&locale) )
		{
			ltext = g_convert_with_fallback( utext, strlen(utext),
					locale, "utf-8", "?", NULL, &wl, NULL);
			if(!ltext)
				return;
		//	text = g_locale_from_utf8(utext, strlen(utext), NULL, &wl, NULL);
			g_free(utext);
		}
		DoPasteFromClipboard( string(ltext), false);
		g_free((void*)ltext);
	}
	else
		DoPasteFromClipboard(m_s_ANSIColorStr, true);
}

void CTermView::DoPasteFromClipboard(string text, bool contain_ansi_color)
{

}

void CTermView::CopyToClipboard(bool primary, bool with_color)
{
	if(!m_pTermData)
		return;
	string text = m_pTermData->GetSelectedText(with_color);
	if( with_color )
		m_s_ANSIColorStr = text;
	else
	{
		m_s_ANSIColorStr.clear();

		gsize wl = 0;
		const gchar* locale;
		const gchar* utext = text.c_str();
		if( !g_get_charset(&locale) )
		{
			utext = g_convert_with_fallback( text.c_str(), text.length(),
					"utf-8", locale, "?", NULL, &wl, NULL);
			if(!utext)
				return;
		}
	//			utext = g_locale_to_utf8( text.c_str(), text.length(), NULL, &wl, NULL);
		GtkClipboard* clipboard = gtk_clipboard_get(  primary ? GDK_SELECTION_PRIMARY : GDK_NONE );
		gtk_clipboard_set_text(clipboard, utext, wl );
		g_free((void*)utext);
	}
}

void CTermView::SetFont(CFont* font)
{
	if( font )
	{
		if( m_Font )
			delete m_Font;
		m_Font = font;
	}
}


void CTermView::OnDestroy()
{
}


void CTermView::SetFontFamily(string name)
{
	m_Font->SetFontFamily(name);
}
