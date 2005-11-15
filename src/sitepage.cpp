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
  #pragma implementation "sitepage.h"
#endif

#include <glib/gi18n.h>
#include "appconfig.h"
#include "sitepage.h"
#include "site.h"

#include "debug.h"

CSitePage::CSitePage(CSite& site)
 : CWidget(), m_Site(site)
{
	GtkWidget *hbox1;
	GtkWidget *label1;
	GtkWidget *label40;
	GtkWidget *hbox3;
	GtkWidget *label3;
	GtkObject *m_Reconnect_adj;
	GtkWidget *label4;
	GtkWidget *hbox6;
	GtkWidget *label10;
	GtkObject *m_Idle_adj;
	GtkWidget *label11;
	GtkWidget *hbox7;
	GtkWidget *label12;
	GtkWidget *hbox13;
	GtkWidget *label17;
	GtkWidget *hbox14;
	GtkWidget *label22;
	GtkObject *m_Line_adj;
	GtkWidget *label23;
	GtkObject *m_Col_adj;
	GtkWidget *hbox10;
	GtkWidget *label20;
	GtkWidget *label21;
	GtkWidget *hbox15;
	GtkWidget *label24;
	GtkObject *m_PasteWrap_adj;
	GtkWidget *label25;
	GtkWidget *hbox25;
	GtkWidget *label39;
	GtkWidget *t_pScreenAlignBox;
	
	m_Widget = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (m_Widget);
	PostCreate();
	
	hbox1 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox1);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox1, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox1), 1);
	
	label1 = gtk_label_new (_("Name: "));
	gtk_widget_show (label1);
	gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
	
	m_Name = gtk_entry_new ();
	gtk_widget_show (m_Name);
	gtk_box_pack_start (GTK_BOX (hbox1), m_Name, TRUE, TRUE, 2);
	gtk_widget_set_size_request (m_Name, 100, -1);
	
	label40 = gtk_label_new (_("Address: "));
	gtk_widget_show (label40);
	gtk_box_pack_start (GTK_BOX (hbox1), label40, FALSE, FALSE, 2);
	
	m_Address = gtk_entry_new ();
	gtk_widget_show (m_Address);
	gtk_box_pack_start (GTK_BOX (hbox1), m_Address, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_Address, 100, -1);
	
	hbox3 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox3);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox3, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox3), 1);
	
	label3 = gtk_label_new (_("Reconnect when disconnected in specified duration : "));
	gtk_widget_show (label3);
	gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
	
	m_Reconnect_adj = gtk_adjustment_new (0, 0, 65535, 1, 10, 10);
	m_Reconnect = gtk_spin_button_new (GTK_ADJUSTMENT (m_Reconnect_adj), 1, 0);
	gtk_widget_show (m_Reconnect);
	gtk_box_pack_start (GTK_BOX (hbox3), m_Reconnect, FALSE, TRUE, 0);
	
	label4 = gtk_label_new (_("Sec (0 = Disabled)"));
	gtk_widget_show (label4);
	gtk_box_pack_start (GTK_BOX (hbox3), label4, FALSE, FALSE, 0);
	
	hbox6 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox6);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox6, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox6), 1);
	
	label10 = gtk_label_new (_("Anti-Idle: Send specified string to server when idling for  "));
	gtk_widget_show (label10);
	gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);
	
	m_Idle_adj = gtk_adjustment_new (0, 0, 65535, 1, 10, 10);
	m_Idle = gtk_spin_button_new (GTK_ADJUSTMENT (m_Idle_adj), 1, 0);
	gtk_widget_show (m_Idle);
	gtk_box_pack_start (GTK_BOX (hbox6), m_Idle, FALSE, TRUE, 0);
	
	label11 = gtk_label_new (_("Sec (0 = Disabled)"));
	gtk_widget_show (label11);
	gtk_box_pack_start (GTK_BOX (hbox6), label11, FALSE, FALSE, 0);
	
	hbox7 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox7);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox7, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox7), 1);
	
	label12 = gtk_label_new (_("Anti-Idle string to be sent :"));
	gtk_widget_show (label12);
	gtk_box_pack_start (GTK_BOX (hbox7), label12, FALSE, FALSE, 0);
	
	m_IdleStr = gtk_entry_new ();
	gtk_widget_show (m_IdleStr);
	gtk_box_pack_start (GTK_BOX (hbox7), m_IdleStr, TRUE, TRUE, 0);
	
	hbox13 = gtk_hbox_new (FALSE, 16);
	gtk_widget_show (hbox13);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox13, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox13), 1);
	
	label17 = gtk_label_new (_("Terminal Screen Size"));
	gtk_widget_show (label17);
	gtk_box_pack_start (GTK_BOX (hbox13), label17, FALSE, TRUE, 0);
	
	hbox14 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox14);
	gtk_box_pack_start (GTK_BOX (hbox13), hbox14, TRUE, TRUE, 0);
	
	label22 = gtk_label_new (_("Line: "));
	gtk_widget_show (label22);
	gtk_box_pack_start (GTK_BOX (hbox14), label22, FALSE, FALSE, 0);
	
	m_Line_adj = gtk_adjustment_new (24, 24, 96, 1, 10, 10);
	m_Line = gtk_spin_button_new (GTK_ADJUSTMENT (m_Line_adj), 1, 0);
	gtk_widget_show (m_Line);
	gtk_box_pack_start (GTK_BOX (hbox14), m_Line, FALSE, TRUE, 0);
	
	label23 = gtk_label_new (_("Col: "));
	gtk_widget_show (label23);
	gtk_box_pack_start (GTK_BOX (hbox14), label23, FALSE, FALSE, 0);
	
	m_Col_adj = gtk_adjustment_new (80, 40, 240, 1, 10, 10);
	m_Col = gtk_spin_button_new (GTK_ADJUSTMENT (m_Col_adj), 1, 0);
	gtk_widget_show (m_Col);
	gtk_box_pack_start (GTK_BOX (hbox14), m_Col, FALSE, TRUE, 0);
	
	hbox10 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox10);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox10, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox10), 1);
	
	label20 = gtk_label_new (_("Terminal Type: "));
	gtk_widget_show (label20);
	gtk_box_pack_start (GTK_BOX (hbox10), label20, FALSE, FALSE, 0);
	
	m_TermType = gtk_entry_new ();
	gtk_widget_show (m_TermType);
	gtk_box_pack_start (GTK_BOX (hbox10), m_TermType, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_TermType, 60, -1);
	
	label21 = gtk_label_new (_("Convert ESC in ANSI color code to  "));
	gtk_widget_show (label21);
	gtk_box_pack_start (GTK_BOX (hbox10), label21, FALSE, FALSE, 0);
	
	m_ESCConv = gtk_entry_new ();
	gtk_widget_show (m_ESCConv);
	gtk_box_pack_start (GTK_BOX (hbox10), m_ESCConv, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_ESCConv, 60, -1);
	
	hbox15 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox15);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox15, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox15), 1);
	
	label24 = gtk_label_new (_("Wrap pasted text when there's too many characters per line :"));
	gtk_widget_show (label24);
	gtk_box_pack_start (GTK_BOX (hbox15), label24, FALSE, FALSE, 0);
	
	m_PasteWrap_adj = gtk_adjustment_new (78, 0, 100, 1, 10, 10);
	m_PasteWrap = gtk_spin_button_new (GTK_ADJUSTMENT (m_PasteWrap_adj), 1, 0);
	gtk_widget_show (m_PasteWrap);
	gtk_box_pack_start (GTK_BOX (hbox15), m_PasteWrap, FALSE, TRUE, 0);
	
	label25 = gtk_label_new (_("(0 = Disabled)"));
	gtk_widget_show (label25);
	gtk_box_pack_start (GTK_BOX (hbox15), label25, FALSE, FALSE, 0);
	
	hbox25 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox25);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox25, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox25), 1);
	
	label39 = gtk_label_new (_("Site Encoding: "));
	gtk_widget_show (label39);
	gtk_box_pack_start (GTK_BOX (hbox25), label39, FALSE, FALSE, 0);
	
	m_Encoding = gtk_entry_new ();
	gtk_widget_show (m_Encoding);
	gtk_box_pack_start (GTK_BOX (hbox25), m_Encoding, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_Encoding, 60, -1);
	
	m_DetectDBChar = gtk_check_button_new_with_mnemonic (_("Detect double-byte characters on typing"));
	gtk_widget_show (m_DetectDBChar);
	gtk_box_pack_start (GTK_BOX (hbox25), m_DetectDBChar, TRUE, TRUE, 0);
	
	m_Startup = gtk_check_button_new_with_mnemonic (_("Connect automatically on program startup"));
	gtk_widget_show (m_Startup);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_Startup, FALSE, FALSE, 0);

	t_pScreenAlignBox = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(t_pScreenAlignBox);
	gtk_box_pack_start(GTK_BOX(m_Widget), t_pScreenAlignBox, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(t_pScreenAlignBox), 1);

	m_pVerticalAlign = gtk_check_button_new_with_mnemonic(_("Let screen align center vertically."));
	gtk_widget_show(m_pVerticalAlign);
	gtk_box_pack_start(GTK_BOX(t_pScreenAlignBox), m_pVerticalAlign, FALSE, FALSE, 0);

	m_pHorizontalAlign = gtk_check_button_new_with_mnemonic(_("Let screen align center horizontally."));
	gtk_widget_show(m_pHorizontalAlign);
	gtk_box_pack_start(GTK_BOX(t_pScreenAlignBox), m_pHorizontalAlign, FALSE, FALSE, 0);

	if( m_Site.m_Name.empty() )
	{
		gtk_widget_hide(hbox1);
		gtk_widget_hide(m_Name);
		gtk_widget_hide(m_Address);
		gtk_widget_hide(m_Startup);
//		gtk_widget_set_sensitive(m_Name, false);
//		gtk_widget_set_sensitive(m_Address, false);
	}

	gtk_entry_set_text(GTK_ENTRY(m_Name), m_Site.m_Name.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Address), m_Site.m_URL.c_str());

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_Idle), m_Site.m_AntiIdle);
	gtk_entry_set_text(GTK_ENTRY(m_IdleStr), m_Site.m_AntiIdleStr.c_str());
	
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_Reconnect), m_Site.m_AutoReconnect);

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_Line), m_Site.m_RowsPerPage);

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_Col), m_Site.m_ColsPerPage);

	gtk_entry_set_text(GTK_ENTRY(m_TermType), m_Site.m_TermType.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_ESCConv), m_Site.m_ESCConv.c_str());
	
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_PasteWrap), m_Site.m_AutoWrapOnPaste);

	gtk_entry_set_text(GTK_ENTRY(m_Encoding), m_Site.m_Encoding.c_str());

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_DetectDBChar), 
	 								m_Site.m_DetectDBChar);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_Startup), 
	 								m_Site.m_Startup);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_pVerticalAlign), AppConfig.VCenterAlign);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_pHorizontalAlign), AppConfig.HCenterAlign);
}


CSitePage::~CSitePage()
{
}

bool CSitePage::OnOK()
{
	m_Site.m_Name = gtk_entry_get_text(GTK_ENTRY(m_Name));
	m_Site.m_URL = gtk_entry_get_text(GTK_ENTRY(m_Address));
	if( GTK_WIDGET_VISIBLE(m_Name) && (m_Site.m_Name.empty() || m_Site.m_URL.empty()) )
		return false;	// Empty name and URL are not allowed.
	m_Site.m_AntiIdle = atoi( gtk_entry_get_text(GTK_ENTRY(m_Idle)) );
	m_Site.m_AntiIdleStr = gtk_entry_get_text(GTK_ENTRY(m_IdleStr));

	m_Site.m_Encoding = gtk_entry_get_text(GTK_ENTRY(m_Encoding));

	m_Site.m_AutoReconnect = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_Reconnect));

	m_Site.m_RowsPerPage = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_Line));
	m_Site.m_ColsPerPage = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_Col));

	m_Site.m_TermType = gtk_entry_get_text(GTK_ENTRY(m_TermType));
	m_Site.m_ESCConv = gtk_entry_get_text(GTK_ENTRY(m_ESCConv));

	m_Site.m_AutoWrapOnPaste = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_PasteWrap));

	m_Site.m_DetectDBChar = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_DetectDBChar));
	m_Site.m_Startup = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_Startup));

	AppConfig.VCenterAlign = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_pVerticalAlign));
	AppConfig.HCenterAlign = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_pHorizontalAlign));

	return true;
}
