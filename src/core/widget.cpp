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
  #pragma implementation "widget.h"
#endif

#include "widget.h"
#include <unistd.h>

CWidget::CWidget()
	: m_Widget(NULL)
{
}

CWidget::~CWidget()
{
	// All CWidget derived class should do its destruction in 
	// OnDestroy(), not in C++ destructor.
	// Because GTK+ calls CWidget::Destroy() on GtkObject destruction, 
	// all resources should be released at that time, otherwise some 
	// obscure bug will be generated.
	// CWidget derived objects used as wrapper of GtkObjects are destructed
	// on idle handler, which is called later then GtkObject destruction.

	// Don't use destructor in CWidget derived classes.
	// Override CWidget::OnDestroy() instead.
	// Don't forget to call OnDestroy() of parent class.
	// Otherwise, parent class won't be destructed.
}

static void on_create(GtkWidget* widget, CWidget* _this)
{
	_this->OnCreate();
}

static void on_destroy(GtkWidget *widget, CWidget* _this)
{
	_this->OnDestroy();
}

bool CWidget::PostCreate()
{
	INFO("post create: %s", gtk_widget_get_name(m_Widget));
	g_signal_connect( G_OBJECT(m_Widget), "destroy", G_CALLBACK (on_destroy), this );
	g_signal_connect( G_OBJECT(m_Widget), "realize", G_CALLBACK (on_create), this );
	return true;
}

void CWidget::OnCreate()
{
	INFO("realize %s", gtk_widget_get_name(m_Widget));
}

void CWidget::Refresh()
{
	GdkRectangle t_Rect;
	t_Rect.x = t_Rect.y = 0;
	t_Rect.width = m_Widget->allocation.width;
	t_Rect.height = m_Widget->allocation.height;
	gdk_window_invalidate_rect(m_Widget->window, &t_Rect, true);
}

void CWidget::Destroy()
{
	if(m_Widget != NULL)
	{
		gtk_widget_destroy(m_Widget);
		m_Widget = NULL;
	}
	else
	{
		DEBUG("destroy at NULL");
	}
}

gboolean CWidget::delete_CWidget(CWidget* obj)
{
	if (obj != NULL)
	{
		delete obj;
		obj = NULL;
	}
	return false;
}

void CWidget::OnDestroy()
{
	g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, (GSourceFunc)&delete_CWidget, this, NULL );
}
