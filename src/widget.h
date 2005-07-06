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
#ifndef WIDGET_H
#define WIDGET_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

/**
GTK+ GtkWidget wrapper.

@author PCMan
*/
	
class CWidget{
public:
    CWidget();

    void Show()  {	gtk_widget_show(m_Widget);  }
    void Hide()  {	gtk_widget_hide(m_Widget);  }
	void Destroy();
	void SetFocus()
	{
		if(!GTK_WIDGET_HAS_FOCUS(m_Widget))
			 gtk_widget_grab_focus(m_Widget);
	}
	void SetCapture(){	if(!HasCapture())	gtk_grab_add(m_Widget);	}
	void ReleaseCapture(){	gtk_grab_remove(m_Widget);	}
	bool HasCapture()	{	return (gtk_grab_get_current() == m_Widget);	}
    bool IsVisible(){	return GTK_WIDGET_VISIBLE(m_Widget);	}

    bool PostCreate();


    virtual void OnCreate();
    void Refresh();
public:
    GtkWidget* m_Widget;
    virtual ~CWidget();
    virtual void OnDestroy();
};

#endif
