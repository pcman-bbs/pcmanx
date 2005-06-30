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
#include "sitedlg.h"
#include "sitepage.h"
#include "notebook.h"

CSiteDlg::CSiteDlg(CWidget* parent, const char* title, CSite& site)
 : CDialog(parent, title, true), m_Site(site)
{
	gtk_window_set_type_hint (GTK_WINDOW (m_Widget), GDK_WINDOW_TYPE_HINT_DIALOG);
	GtkDialog* dlg = GTK_DIALOG(m_Widget);
	
	m_pNotebook = new CNotebook();
	m_pSitePage = new CSitePage(m_Site);

	m_pNotebook->AddPage( m_pSitePage, _("Site Settings"), NULL);

	gtk_box_pack_start( GTK_BOX (dlg->vbox), m_pNotebook->m_Widget, FALSE, FALSE, 4);

	SetResizable(false);
}


CSiteDlg::~CSiteDlg()
{
}


bool CSiteDlg::OnOK()
{
	return m_pSitePage->OnOK();
}
