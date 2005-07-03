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
#include "widget.h"
#include <unistd.h>

CWidget::CWidget()
{
	m_Widget = NULL;
}


CWidget::~CWidget()
{
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
//	g_print("post create: %s\n", gtk_widget_get_name(m_Widget));
	g_signal_connect( G_OBJECT(m_Widget), "destroy", G_CALLBACK (on_destroy), this );
	g_signal_connect( G_OBJECT(m_Widget), "realize", G_CALLBACK (on_create), this );
	return true;
}



void CWidget::OnCreate()
{
//	g_print("realize %s\n", gtk_widget_get_name(m_Widget));
}


void CWidget::Refresh()
{
	GdkRectangle rect;
	rect.x = rect.y = 0;
	rect.width = m_Widget->allocation.width;
	rect.height = m_Widget->allocation.height;
	gdk_window_invalidate_rect(m_Widget->window, &rect, true);
}

void CWidget::Destroy()
{
	if(m_Widget)
	{
		gtk_widget_destroy(m_Widget);
		m_Widget = NULL;
	}
}

static gboolean delete_CWidget(CWidget* obj)
{
	delete obj;
	return false;
}

void CWidget::OnDestroy()
{
	g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, (GSourceFunc)&delete_CWidget, this, NULL );
}
