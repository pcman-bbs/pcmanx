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

#include "inputdialog.h"

#include <gdk/gdkkeysyms.h>

static gboolean on_key_press(GtkWidget* wnd, GdkEventKey *evt, CInputDialog* _this)
{
	switch(evt->keyval)
	{
	case GDK_Return:
		gtk_dialog_response( (GtkDialog*)_this->m_Widget, GTK_RESPONSE_ACCEPT );
		g_signal_handler_disconnect(G_OBJECT(wnd), _this->m_KeyPressHandlerId);
		break;
	case GDK_Escape:
		gtk_dialog_response( (GtkDialog*)_this->m_Widget, GTK_RESPONSE_REJECT );
		g_signal_handler_disconnect(G_OBJECT(wnd), _this->m_KeyPressHandlerId);
		break;
	}
	return false;
}


CInputDialog::CInputDialog(CWidget* parent, const char* title, const char* prompt, const char* text, bool can_be_empty)
        : CDialog(parent, title, true), m_CanBeEmpty(can_be_empty)
{
	m_Text = NULL;
	GtkWidget* label = gtk_label_new(prompt);
	gtk_widget_show(label);

	m_Entry = (GtkEntry*)gtk_entry_new();
	if( text )
	{
		gtk_entry_set_text( m_Entry, text );
		gtk_editable_select_region( GTK_EDITABLE(m_Entry), 0, -1 );
	}

	gtk_entry_set_activates_default(m_Entry, true);
	gtk_widget_show((GtkWidget*)m_Entry);

	GtkDialog* dlg = GTK_DIALOG(m_Widget);

	gtk_box_pack_start( GTK_BOX (dlg->vbox), label, FALSE, FALSE, 4);
	gtk_box_pack_start( GTK_BOX (dlg->vbox), (GtkWidget*)m_Entry, FALSE, FALSE, 4);

	gtk_widget_show_all(m_Widget);

//	m_KeyPressHandlerId = g_signal_connect(G_OBJECT(m_Entry), "key_press_event", G_CALLBACK(on_key_press), this );

}


CInputDialog::~CInputDialog()
{
}


bool CInputDialog::OnOK()
{
	m_Text = gtk_entry_get_text(m_Entry);
	return ( m_CanBeEmpty || (m_Text && *m_Text) );
}
