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

#include "sitepage.h"
#include <glib/gi18n.h>
#include "site.h"

CSitePage::CSitePage(CSite& site)
 : CWidget(), m_Site(site)
{
	GtkWidget *hbox1;
	GtkWidget *label1;
	GtkWidget *hbox2;
	GtkWidget *label2;
	GtkWidget *hbox3;
	GtkWidget *label3;
	GtkWidget *label4;
	GtkWidget *hbox6;
	GtkWidget *label10;
	GtkWidget *label11;
	GtkWidget *hbox7;
	GtkWidget *label12;
	GtkWidget *fixed1;
	GtkWidget *hbox13;
	GtkWidget *hbox8;
	GtkWidget *label17;
	GtkWidget *hbox14;
	GtkWidget *label22;
	GtkWidget *label23;
	GtkWidget *hbox10;
	GtkWidget *hbox11;
	GtkWidget *label20;
	GtkWidget *hbox12;
	GtkWidget *label21;
	GtkWidget *hbox15;
	GtkWidget *label24;
	GtkWidget *label25;

	m_Widget = gtk_vbox_new(FALSE, 2);

	gtk_widget_show (m_Widget);

	hbox1 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox1);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox1, FALSE, TRUE, 2);
	
	label1 = gtk_label_new (_("Name: "));
	gtk_widget_show (label1);
	gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 2);
	
	m_Name = gtk_entry_new ();
	gtk_widget_show (m_Name);
	gtk_box_pack_start (GTK_BOX (hbox1), m_Name, TRUE, TRUE, 2);
	
	hbox2 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox2, FALSE, TRUE, 2);
	
	label2 = gtk_label_new (_("Address: "));
	gtk_widget_show (label2);
	gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 2);
	
	m_Address = gtk_entry_new ();
	gtk_widget_show (m_Address);
	gtk_box_pack_start (GTK_BOX (hbox2), m_Address, TRUE, TRUE, 2);

	hbox3 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox3);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox3, FALSE, TRUE, 2);
	label3 = gtk_label_new (_("Reconnect when disconnected in specified duration : "));
	gtk_widget_show (label3);
	gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 2);
	
	m_Reconnect = gtk_entry_new ();
	gtk_widget_show (m_Reconnect);
	gtk_box_pack_start (GTK_BOX (hbox3), m_Reconnect, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_Reconnect, 40, -1);
	
	label4 = gtk_label_new (_("Sec"));
	gtk_widget_show (label4);
	gtk_box_pack_start (GTK_BOX (hbox3), label4, FALSE, FALSE, 2);
	
	hbox6 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox6);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox6, FALSE, TRUE, 2);
	
	label10 = gtk_label_new (_("Anti-Idle: Send specified string to server when idling for  "));
	gtk_widget_show (label10);
	gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 2);
	
	m_Idle = gtk_entry_new ();
	gtk_widget_show (m_Idle);
	gtk_box_pack_start (GTK_BOX (hbox6), m_Idle, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_Idle, 40, -1);

	label11 = gtk_label_new (_("Sec"));
	gtk_widget_show (label11);
	gtk_box_pack_start (GTK_BOX (hbox6), label11, FALSE, FALSE, 2);
	
	hbox7 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox7);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox7, FALSE, TRUE, 2);
	
	label12 = gtk_label_new (_("Anti-Idle string to be sent :"));
	gtk_widget_show (label12);
	gtk_box_pack_start (GTK_BOX (hbox7), label12, FALSE, FALSE, 2);
	
	m_IdleStr = gtk_entry_new ();
	gtk_widget_show (m_IdleStr);
	gtk_box_pack_start (GTK_BOX (hbox7), m_IdleStr, TRUE, TRUE, 2);
	
	fixed1 = gtk_fixed_new ();
	gtk_widget_show (fixed1);
	gtk_box_pack_start (GTK_BOX (m_Widget), fixed1, FALSE, TRUE, 2);
	
	hbox13 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox13);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox13, FALSE, TRUE, 2);
	
	hbox8 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox8);
	gtk_box_pack_start (GTK_BOX (hbox13), hbox8, TRUE, TRUE, 2);
	
	label17 = gtk_label_new (_("Terminal Screen Size"));
	gtk_widget_show (label17);
	gtk_box_pack_start (GTK_BOX (hbox8), label17, FALSE, FALSE, 2);
	
	hbox14 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox14);
	gtk_box_pack_start (GTK_BOX (hbox13), hbox14, FALSE, TRUE, 2);
	
	label22 = gtk_label_new (_("Line: "));
	gtk_widget_show (label22);
	gtk_box_pack_start (GTK_BOX (hbox14), label22, FALSE, FALSE, 2);
	
	m_Line = gtk_entry_new ();
	gtk_widget_show (m_Line);
	gtk_box_pack_start (GTK_BOX (hbox14), m_Line, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_Line, 40, -1);
	
	label23 = gtk_label_new (_("Col: "));
	gtk_widget_show (label23);
	gtk_box_pack_start (GTK_BOX (hbox14), label23, FALSE, FALSE, 2);
	
	m_Col = gtk_entry_new ();
	gtk_widget_show (m_Col);
	gtk_box_pack_start (GTK_BOX (hbox14), m_Col, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_Col, 40, -1);
	
	hbox10 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox10);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox10, FALSE, TRUE, 2);
	
	hbox11 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox11);
	gtk_box_pack_start (GTK_BOX (hbox10), hbox11, TRUE, TRUE, 2);
	
	label20 = gtk_label_new (_("Terminal Type: "));
	gtk_widget_show (label20);
	gtk_box_pack_start (GTK_BOX (hbox11), label20, FALSE, FALSE, 2);
	
	m_TermType = gtk_entry_new ();
	gtk_widget_show (m_TermType);
	gtk_box_pack_start (GTK_BOX (hbox11), m_TermType, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_TermType, 60, -1);

	hbox12 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox12);
	gtk_box_pack_start (GTK_BOX (hbox10), hbox12, FALSE, TRUE, 2);
	
	label21 = gtk_label_new (_("Convert ESC in ANSI color code to  "));
	gtk_widget_show (label21);
	gtk_box_pack_start (GTK_BOX (hbox12), label21, FALSE, FALSE, 2);
	
	m_ESCConv = gtk_entry_new ();
	gtk_widget_show (m_ESCConv);
	gtk_box_pack_start (GTK_BOX (hbox12), m_ESCConv, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_ESCConv, 60, -1);
	
	hbox15 = gtk_hbox_new (FALSE, 4);
	gtk_widget_show (hbox15);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox15, FALSE, TRUE, 2);
	
	label24 = gtk_label_new (_("Wrap pasted text when there's too many characters per line :"));
	gtk_widget_show (label24);
	gtk_box_pack_start (GTK_BOX (hbox15), label24, FALSE, FALSE, 2);
	
	m_PasteWrap = gtk_entry_new ();
	gtk_widget_show (m_PasteWrap);
	gtk_box_pack_start (GTK_BOX (hbox15), m_PasteWrap, FALSE, TRUE, 2);
	gtk_widget_set_size_request (m_PasteWrap, 40, -1);
	
	label25 = gtk_label_new (_("0 = disabled"));
	gtk_widget_show (label25);
	gtk_box_pack_start (GTK_BOX (hbox15), label25, FALSE, FALSE, 2);

	gtk_entry_set_text(GTK_ENTRY(m_Name), m_Site.m_Name.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Address), m_Site.m_URL.c_str());

	if( m_Site.m_Name.empty() )
	{
		gtk_widget_set_sensitive(m_Name, false);
		gtk_widget_set_sensitive(m_Address, false);
	}
	
	char num[20];
	sprintf( num, "%d", m_Site.m_AntiIdle );
	gtk_entry_set_text(GTK_ENTRY(m_Idle), num);
	gtk_entry_set_text(GTK_ENTRY(m_IdleStr), m_Site.m_AntiIdleStr.c_str());
	
	sprintf( num, "%d", m_Site.m_AutoReconnect );
	gtk_entry_set_text(GTK_ENTRY(m_Reconnect), num);

	sprintf( num, "%d", m_Site.m_RowsPerPage );
	gtk_entry_set_text(GTK_ENTRY(m_Line), num);
	sprintf( num, "%d", m_Site.m_ColsPerPage );
	gtk_entry_set_text(GTK_ENTRY(m_Col), num);

	gtk_entry_set_text(GTK_ENTRY(m_TermType), m_Site.m_TermType.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_ESCConv), m_Site.m_ESCConv.c_str());
	
	sprintf( num, "%d", m_Site.m_AutoWrapOnPaste );
	gtk_entry_set_text(GTK_ENTRY(m_PasteWrap), num);
}


CSitePage::~CSitePage()
{
}

bool CSitePage::OnOK()
{
	m_Site.m_Name = gtk_entry_get_text(GTK_ENTRY(m_Name));
	m_Site.m_URL = gtk_entry_get_text(GTK_ENTRY(m_Address));

	m_Site.m_AntiIdle = atoi( gtk_entry_get_text(GTK_ENTRY(m_Idle)) );
	m_Site.m_AntiIdleStr = gtk_entry_get_text(GTK_ENTRY(m_IdleStr));

	m_Site.m_AutoReconnect = atoi( gtk_entry_get_text(GTK_ENTRY(m_Reconnect)));

	m_Site.m_RowsPerPage = atoi( gtk_entry_get_text(GTK_ENTRY(m_Line)) );
	m_Site.m_ColsPerPage = atoi( gtk_entry_get_text(GTK_ENTRY(m_Col)) );

	m_Site.m_TermType = gtk_entry_get_text(GTK_ENTRY(m_TermType));
	m_Site.m_ESCConv = gtk_entry_get_text(GTK_ENTRY(m_ESCConv));

	m_Site.m_AutoWrapOnPaste = atoi( gtk_entry_get_text(GTK_ENTRY(m_PasteWrap)) );

	return true;
}
