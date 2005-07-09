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

#include <glib/gi18n.h>
#include "generalprefpage.h"
#include "appconfig.h"

CGeneralPrefPage::CGeneralPrefPage()
 : CWidget()
{	
	m_Widget = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (m_Widget);

	PostCreate();
  
	GtkWidget *hbox19;
	GtkWidget* label27;
	
	m_QueryOnCloseCon = gtk_check_button_new_with_mnemonic (_("Confirm before closing connected connections"));
	gtk_widget_show (m_QueryOnCloseCon);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnCloseCon, FALSE, FALSE, 0);
	
	m_QueryOnExit = gtk_check_button_new_with_mnemonic (_("Confirm before exiting the program if there are still connections"));
	gtk_widget_show (m_QueryOnExit);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnExit, FALSE, FALSE, 0);
	
	m_CancelSelAfterCopy = gtk_check_button_new_with_mnemonic (_("Cancel selection after copying text"));
	gtk_widget_show (m_CancelSelAfterCopy);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_CancelSelAfterCopy, FALSE, FALSE, 0);

	m_ShowTrayIcon = gtk_check_button_new_with_mnemonic (_("Show System Tray Icon (Docklet) (Take effect after restart)"));

	gtk_widget_show (m_ShowTrayIcon);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_ShowTrayIcon, FALSE, FALSE, 0);

	hbox19 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox19);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox19, FALSE, FALSE, 0);

	label27 = gtk_label_new (_("Web Browser: "));
	gtk_widget_show (label27);
	gtk_box_pack_start (GTK_BOX (hbox19), label27, FALSE, FALSE, 0);

	m_WebBrowser = gtk_entry_new ();
	gtk_widget_show (m_WebBrowser);
	gtk_box_pack_start (GTK_BOX (hbox19), m_WebBrowser, TRUE, TRUE, 0);


	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon), 
								AppConfig.QueryOnCloseCon);
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnExit), 
								AppConfig.QueryOnExit);
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy), 
								AppConfig.CancelSelAfterCopy);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_ShowTrayIcon), 
								AppConfig.ShowTrayIcon);
								
	gtk_entry_set_text(GTK_ENTRY(m_WebBrowser), AppConfig.WebBrowser.c_str());

}


CGeneralPrefPage::~CGeneralPrefPage()
{
}


void CGeneralPrefPage::OnOK()
{
	AppConfig.QueryOnCloseCon = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon));
	AppConfig.QueryOnExit = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnExit));
	AppConfig.CancelSelAfterCopy = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy));
	AppConfig.ShowTrayIcon = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_ShowTrayIcon));
	AppConfig.WebBrowser = gtk_entry_get_text(GTK_ENTRY(m_WebBrowser));
}
