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
#include <glib/gi18n.h>
#include "generalprefpage.h"
#include "appconfig.h"

CGeneralPrefPage::CGeneralPrefPage()
 : CWidget()
{	
	m_Widget = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (m_Widget);

	PostCreate();
	
	m_QueryOnCloseCon = gtk_check_button_new_with_mnemonic (_("Confirm before closing connected connections."));
	gtk_widget_show (m_QueryOnCloseCon);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnCloseCon, FALSE, FALSE, 0);
	
	m_QueryOnExit = gtk_check_button_new_with_mnemonic (_("Confirm before closing connected connections."));
	gtk_widget_show (m_QueryOnExit);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_QueryOnExit, FALSE, FALSE, 0);
	
	m_CancelSelAfterCopy = gtk_check_button_new_with_mnemonic (_("Cancel selection after copying text"));
	gtk_widget_show (m_CancelSelAfterCopy);
	gtk_box_pack_start (GTK_BOX (m_Widget), m_CancelSelAfterCopy, FALSE, FALSE, 0);


	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon), 
	 								AppConfig.QueryOnCloseCon);
	 
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_QueryOnExit), 
	 								AppConfig.QueryOnExit);
	 
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy), 
	 								AppConfig.CancelSelAfterCopy);
}


CGeneralPrefPage::~CGeneralPrefPage()
{
}


void CGeneralPrefPage::OnOK()
{
	AppConfig.QueryOnCloseCon = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnCloseCon));
	AppConfig.QueryOnExit = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_QueryOnExit));
	AppConfig.CancelSelAfterCopy = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_CancelSelAfterCopy));
}
