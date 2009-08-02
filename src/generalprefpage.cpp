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
  #pragma implementation "generalprefpage.h"
#endif

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
	GtkWidget *label27;
	GtkWidget *hbox20;
	GtkObject *m_PopupTimeout_adj;
	GtkWidget *label29;
	GtkWidget *hbox21;
	GtkWidget *label30;

	m_QueryOnCloseCon = gtk_check_button_new_with_mnemonic (_("Confirm before closing connected connections"));
	gtk_widget_show (m_QueryOnCloseCon);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnCloseCon, FALSE, FALSE, 0);

	m_QueryOnExit = gtk_check_button_new_with_mnemonic (_("Confirm before exiting the program if there are still connections"));
	gtk_widget_show (m_QueryOnExit);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnExit, FALSE, FALSE, 0);

	m_CancelSelAfterCopy = gtk_check_button_new_with_mnemonic (_("Cancel selection after copying text"));
	gtk_widget_show (m_CancelSelAfterCopy);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_CancelSelAfterCopy, FALSE, FALSE, 0);

#ifdef USE_MOUSE
	m_MouseSupport = gtk_check_button_new_with_mnemonic (_("Enable Mouse Support"));
	gtk_widget_show (m_MouseSupport);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_MouseSupport, FALSE, FALSE, 0);
#endif

#ifdef USE_DOCKLET
	m_ShowTrayIcon = gtk_check_button_new_with_mnemonic (_("Show System Tray Icon (Docklet)"));
	gtk_widget_show (m_ShowTrayIcon);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_ShowTrayIcon, FALSE, FALSE, 0);
#endif

	m_ShowStatusBar = gtk_check_button_new_with_mnemonic (_("Show Status Bar on bottom"));
	gtk_widget_show (m_ShowStatusBar);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_ShowStatusBar, FALSE, FALSE, 0);

	m_AAFont = gtk_check_button_new_with_mnemonic (_("Use Anti-Aliasing Fonts (Take effect after restart)"));
	gtk_widget_show (m_AAFont);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_AAFont, FALSE, FALSE, 0);

#ifdef USE_WGET
	m_pWgetFiles = gtk_check_button_new_with_mnemonic (_("Use \"wget\" to download files."));
	gtk_widget_show (m_pWgetFiles);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_pWgetFiles, FALSE, FALSE, 0);
#endif

	hbox20 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox20);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox20, FALSE, FALSE, 0);

	m_PopupNotifier = gtk_check_button_new_with_mnemonic (_("Display popup notifier for "));
	gtk_widget_show (m_PopupNotifier);
	gtk_box_pack_start (GTK_BOX (hbox20), m_PopupNotifier, FALSE, FALSE, 0);

	m_PopupTimeout_adj = gtk_adjustment_new (0, 0, 100, 1, 0, 0);
	m_PopupTimeout = gtk_spin_button_new (GTK_ADJUSTMENT (m_PopupTimeout_adj), 1, 0);
	gtk_widget_show (m_PopupTimeout);
	gtk_box_pack_start (GTK_BOX (hbox20), m_PopupTimeout, FALSE, TRUE, 0);

	label29 = gtk_label_new (_("seconds"));
	gtk_widget_show (label29);
	gtk_box_pack_start (GTK_BOX (hbox20), label29, FALSE, FALSE, 2);

	m_MidClickAsClose = gtk_check_button_new_with_mnemonic (_("Middle mouse click as tab close (similar to Firefox)"));
	gtk_widget_show (m_MidClickAsClose);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_MidClickAsClose, FALSE, FALSE, 0);

	hbox19 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox19);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox19, FALSE, FALSE, 0);

	label27 = gtk_label_new (_("Web Browser: "));
	gtk_widget_show (label27);
	gtk_box_pack_start (GTK_BOX (hbox19), label27, FALSE, FALSE, 0);

	m_WebBrowser = gtk_entry_new ();
	gtk_widget_show (m_WebBrowser);
	gtk_box_pack_start (GTK_BOX (hbox19), m_WebBrowser, TRUE, TRUE, 0);

	hbox21 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox21);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox21, FALSE, FALSE, 0);
	label30 = gtk_label_new (_("Mail Client: "));
	gtk_widget_show (label30);
	gtk_box_pack_start (GTK_BOX (hbox21), label30, FALSE, FALSE, 0);

	m_MailClient = gtk_entry_new ();
	gtk_widget_show (m_MailClient);
 	gtk_box_pack_start (GTK_BOX (hbox21), m_MailClient, TRUE, TRUE, 0);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon), AppConfig.QueryOnCloseCon);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnExit), AppConfig.QueryOnExit);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy), AppConfig.CancelSelAfterCopy);
#ifdef USE_MOUSE
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_MouseSupport), AppConfig.MouseSupport);
#endif
#ifdef USE_DOCKLET
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_ShowTrayIcon), AppConfig.ShowTrayIcon);
#endif
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_ShowStatusBar), AppConfig.ShowStatusBar);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_AAFont), AppConfig.AntiAliasFont);
#ifdef USE_WGET
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_pWgetFiles), AppConfig.UseWgetFiles);
#endif
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_PopupNotifier), AppConfig.PopupNotifier);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_PopupTimeout), AppConfig.PopupTimeout);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_MidClickAsClose),
	                             AppConfig.MidClickAsClose);

	gtk_entry_set_text(GTK_ENTRY(m_WebBrowser), AppConfig.WebBrowser.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_MailClient), AppConfig.MailClient.c_str());
}


void CGeneralPrefPage::OnOK()
{
	AppConfig.QueryOnCloseCon = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon));
	AppConfig.QueryOnExit = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnExit));
	AppConfig.CancelSelAfterCopy = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy));
#ifdef USE_MOUSE
	AppConfig.MouseSupport = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_MouseSupport));
#endif
#ifdef USE_DOCKLET
	AppConfig.ShowTrayIcon = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_ShowTrayIcon));
#endif
	AppConfig.ShowStatusBar = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_ShowStatusBar));
	AppConfig.AntiAliasFont = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_AAFont));
#ifdef USE_WGET
	AppConfig.UseWgetFiles = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_pWgetFiles));
#endif
	AppConfig.PopupNotifier =  gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(m_PopupNotifier));
	AppConfig.PopupTimeout = (int)gtk_spin_button_get_value( GTK_SPIN_BUTTON(m_PopupTimeout));
	AppConfig.MidClickAsClose = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(m_MidClickAsClose));
	AppConfig.WebBrowser = gtk_entry_get_text(GTK_ENTRY(m_WebBrowser));
	AppConfig.MailClient = gtk_entry_get_text(GTK_ENTRY(m_MailClient));
}
