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

#ifndef WIDGET_H
#define WIDGET_H

#ifdef __GNUG__
  #pragma interface "widget.h"
#endif

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
    bool IsVisible(){	return m_Widget && GTK_WIDGET_VISIBLE(m_Widget);	}

    bool PostCreate();


    virtual void OnCreate();
    void Refresh();
public:
    GtkWidget* m_Widget;
    virtual void OnDestroy();
protected:
    virtual ~CWidget();
private:
	static gboolean delete_CWidget(CWidget* obj);
};

#endif
