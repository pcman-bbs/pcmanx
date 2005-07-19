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

#ifndef DIALOG_H
#define DIALOG_H

#ifdef __GNUG__
  #pragma interface "dialog.h"
#endif

#include <widget.h>
#include <glib/gi18n.h>

/**
@author PCMan
*/

class CDialog : public CWidget
{
public:
	CDialog(){}
    CDialog(CWidget* parent, const char* title, bool show_okcancel);
    void SetResizable(bool can_resize){ gtk_window_set_resizable(GTK_WINDOW(m_Widget), can_resize); }
    int ShowModal();
	static void OnResponse(GtkDialog* dlg, gint arg, CDialog* _this);
    virtual bool OnOK();
    virtual bool OnCancel();
    virtual void OnCommand(int id);
};

#endif
