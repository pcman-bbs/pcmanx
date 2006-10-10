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
  #pragma implementation "editfavdlg.h"
#endif


#include "editfavdlg.h"
#include "site.h"
#include "listbox.h"

#include "sitedlg.h"

#include "debug.h"

CEditFavDlg::CEditFavDlg(CWidget* parent, vector<CSite>& sites) : m_Sites(sites)
{
	m_Widget = gtk_dialog_new_with_buttons(_("Edit Favorites"),
		GTK_WINDOW(parent->m_Widget),
		GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

	PostCreate();

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *fav_list_scrl;
	GtkWidget *fav_list;
	GtkWidget *vbtn_box;
	GtkWidget *add_btn;
	GtkWidget *edit_btn;
	GtkWidget *remove_btn;
	GtkWidget *up_btn;
	GtkWidget *down_btn;

	vbox = GTK_DIALOG (m_Widget)->vbox;
	gtk_widget_show (vbox);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_end (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	
	fav_list_scrl = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (fav_list_scrl);
	gtk_box_pack_start (GTK_BOX (hbox), fav_list_scrl, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (fav_list_scrl), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (fav_list_scrl), GTK_SHADOW_IN);
	
	vbtn_box = gtk_vbutton_box_new ();
	gtk_widget_show (vbtn_box);
	gtk_box_pack_start (GTK_BOX (hbox), vbtn_box, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (vbtn_box), GTK_BUTTONBOX_START);

	add_btn = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (add_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), add_btn);
	GTK_WIDGET_SET_FLAGS (add_btn, GTK_CAN_DEFAULT);
	
	edit_btn = gtk_button_new_from_stock ("gtk-edit");
	gtk_widget_show (edit_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), edit_btn);
	GTK_WIDGET_SET_FLAGS (edit_btn, GTK_CAN_DEFAULT);
	
	remove_btn = gtk_button_new_from_stock ("gtk-remove");
	gtk_widget_show (remove_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), remove_btn);
	GTK_WIDGET_SET_FLAGS (remove_btn, GTK_CAN_DEFAULT);
	
	up_btn = gtk_button_new_from_stock ("gtk-go-up");
	gtk_widget_show (up_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), up_btn);
	GTK_WIDGET_SET_FLAGS (up_btn, GTK_CAN_DEFAULT);
	
	down_btn = gtk_button_new_from_stock ("gtk-go-down");
	gtk_widget_show (down_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), down_btn);
	GTK_WIDGET_SET_FLAGS (down_btn, GTK_CAN_DEFAULT);
	
	g_signal_connect ((gpointer) add_btn, "clicked",
					G_CALLBACK (CEditFavDlg::OnAdd),
					this);
	g_signal_connect ((gpointer) edit_btn, "clicked",
					G_CALLBACK (CEditFavDlg::OnEdit),
					this);
	g_signal_connect ((gpointer) remove_btn, "clicked",
					G_CALLBACK (CEditFavDlg::OnRemove),
					this);
	g_signal_connect ((gpointer) up_btn, "clicked",
					G_CALLBACK (CEditFavDlg::OnUp),
					this);
	g_signal_connect ((gpointer) down_btn, "clicked",
					G_CALLBACK (CEditFavDlg::OnDown),
					this);

	gtk_window_set_default_size((GtkWindow*)m_Widget, 400, 300);

	m_EditBtn = edit_btn;

	m_List = new CListBox;
	fav_list = m_List->m_Widget;
	gtk_widget_show (fav_list);
	gtk_container_add (GTK_CONTAINER (fav_list_scrl), fav_list);
	
	g_signal_connect ((gpointer) m_List->m_Widget, "row-activated",
					G_CALLBACK (&CEditFavDlg::OnRowActivated),
					this);

	vector<CSite>::iterator it;
	int i = 0;
	for( it = m_Sites.begin(); it != m_Sites.end(); ++it )
	{
		CSite& site = *it;
		++i;
		m_List->Append(site.m_Name);
	}

}


void CEditFavDlg::OnAdd(GtkWidget* btn, CEditFavDlg* _this)
{
	int i = _this->m_List->GetCurSel() + 1;

	CSite site(_("New Site"));
	CSiteDlg* dlg = new CSiteDlg( _this, _("Add New Site Settings"), 
	site );
	if( dlg->ShowModal() == GTK_RESPONSE_OK )
	{
		_this->m_Sites.insert( _this->m_Sites.begin()+i, dlg->m_Site );
		_this->m_List->Insert( i, dlg->m_Site.m_Name );
		_this->m_List->SetCurSel( i );
	}
	dlg->Destroy();
}


void CEditFavDlg::OnEdit(GtkWidget* btn, CEditFavDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel != -1 )
	{
		CSiteDlg* dlg = new CSiteDlg( _this, _("Edit Site Settings"), 
		_this->m_Sites[sel] );
		if( dlg->ShowModal() == GTK_RESPONSE_OK )
		{
			_this->m_Sites[sel] = dlg->m_Site;
			_this->m_List->SetItemText( sel, dlg->m_Site.m_Name );
		}
		dlg->Destroy();
	}
}


void CEditFavDlg::OnRemove(GtkWidget* btn, CEditFavDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel >= 0 )
	{
		_this->m_List->Delete(sel);
		_this->m_Sites.erase(_this->m_Sites.begin()+sel);
		if( sel >= (_this->m_List->Count()-1) )
			sel--;
		_this->m_List->SetCurSel(sel);
	}
}


void CEditFavDlg::OnUp(GtkWidget* btn, CEditFavDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel != -1 && sel > 0 )
	{
		_this->m_List->MoveUp(sel);
		CSite tmp = _this->m_Sites[sel];
		_this->m_Sites[sel] = _this->m_Sites[sel-1];
		_this->m_Sites[sel-1] = tmp;
	}
}


void CEditFavDlg::OnDown(GtkWidget* btn, CEditFavDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel != -1 && (sel+1) < _this->m_List->Count() )
	{
		_this->m_List->MoveDown(sel);
		CSite tmp = _this->m_Sites[sel];
		_this->m_Sites[sel] = _this->m_Sites[sel+1];
		_this->m_Sites[sel+1] = tmp;
	}
}


void CEditFavDlg::OnRowActivated(GtkTreeView *tree_view, GtkTreePath* path,  
	GtkTreeViewColumn* col, CEditFavDlg* _this)
{
	gtk_button_clicked( GTK_BUTTON(_this->m_EditBtn) );
}
