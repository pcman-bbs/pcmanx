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
  #pragma implementation "view.h"
#endif


#include "view.h"

#include "debug.h"

static gboolean on_expose( GtkWidget* widget, GdkEventExpose *evt, CView* _this )
{
	_this->OnPaint(evt);
	return false;
}


static gboolean on_configure(GtkWidget *widget, GdkEventConfigure *evt, CView* _this)
{
	_this->OnSize(evt);
	return false;
}



static gboolean on_focus_in(GtkWidget *widget, GdkEventFocus *evt, CView* _this)
{
	_this->OnSetFocus(evt);
	return true;
}

static gboolean on_focus_out(GtkWidget *widget, GdkEventFocus *evt, CView* _this)
{
	_this->OnKillFocus(evt);
	return true;
}

CView::CView(): CWidget()
{
	m_Widget = gtk_drawing_area_new();

	PostCreate();

	m_ContextMenu = NULL;

	g_signal_connect(G_OBJECT(m_Widget), "expose_event", G_CALLBACK(on_expose), this);
	g_signal_connect( G_OBJECT(m_Widget), "configure_event", G_CALLBACK (on_configure), this );

	g_signal_connect(G_OBJECT(m_Widget), "focus_in_event", G_CALLBACK(on_focus_in), this);
	g_signal_connect(G_OBJECT(m_Widget), "focus_out_event", G_CALLBACK(on_focus_out), this);

	GTK_WIDGET_SET_FLAGS(m_Widget, GTK_CAN_FOCUS);

	Show();
}


void CView::OnSize(GdkEventConfigure* evt)
{
    /// @todo implement me
}

void CView::OnSetFocus(GdkEventFocus *evt)
{
    /// @todo implement me
}

void CView::OnKillFocus(GdkEventFocus *evt)
{
    /// @todo implement me
}

