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
  #pragma implementation "dialog.h"
#endif

#include "dialog.h"

#include <gdk/gdkkeysyms.h>

void CDialog::OnResponse(GtkDialog* dlg, gint arg, CDialog* _this)
{
	switch(arg)
	{
	case GTK_RESPONSE_OK:
		if(!_this->OnOK())
			g_signal_stop_emission_by_name(dlg, "response");
		break;
	case GTK_RESPONSE_CANCEL:
		if( !_this->OnCancel() )
			g_signal_stop_emission_by_name(dlg, "response");
		break;
	default:
		_this->OnCommand(arg);
	}
}


CDialog::CDialog(CWidget* parent, const char* title, bool show_okcancel)
        : CWidget()
{
	if(show_okcancel)
	{
		m_Widget = gtk_dialog_new_with_buttons(title, GTK_WINDOW(parent->m_Widget),
		 GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT), GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		gtk_dialog_set_default_response((GtkDialog*)m_Widget, GTK_RESPONSE_OK);
	}
	else
		m_Widget = gtk_dialog_new_with_buttons(title, GTK_WINDOW(parent->m_Widget),
		GTK_DIALOG_MODAL, NULL, NULL);

	gtk_window_set_type_hint (GTK_WINDOW (m_Widget), GDK_WINDOW_TYPE_HINT_DIALOG);

	PostCreate();

	g_signal_connect(G_OBJECT(m_Widget), "response", G_CALLBACK(CDialog::OnResponse), this);
}


CDialog::~CDialog()
{
}


int CDialog::ShowModal()
{
	int response = gtk_dialog_run(GTK_DIALOG(m_Widget));
	return response;
}


bool CDialog::OnOK()
{
	return true;
}


bool CDialog::OnCancel()
{
	return true;
}


void CDialog::OnCommand(int id)
{

}
