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

#include "prefdlg.h"
#include "notebook.h"
#include "generalprefpage.h"
#include "sitepage.h"
#include "appconfig.h"

CPrefDlg::CPrefDlg(CWidget* parent)
 : CDialog(parent, _("Preference"), true)
{
	gtk_window_set_type_hint (GTK_WINDOW (m_Widget), GDK_WINDOW_TYPE_HINT_DIALOG);
	GtkDialog* dlg = GTK_DIALOG(m_Widget);

	m_pNotebook = new CNotebook();
	m_pGeneralPrefPage = new CGeneralPrefPage();
	m_pSitePage = new CSitePage( AppConfig.m_DefaultSite );

	m_pNotebook->AddPage( m_pGeneralPrefPage, _("General"), NULL);
	m_pNotebook->AddPage( m_pSitePage, _("Site Settings"), NULL);

	gtk_box_pack_start( GTK_BOX (dlg->vbox), m_pNotebook->m_Widget, FALSE, FALSE, 4);

	SetResizable(false);
}


CPrefDlg::~CPrefDlg()
{
}


bool CPrefDlg::OnOK()
{
	if( m_pSitePage->OnOK() )
	{
		AppConfig.m_DefaultSite = m_pSitePage->m_Site;
		m_pGeneralPrefPage->OnOK();
	}
	return true;
}
