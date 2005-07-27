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
  #pragma implementation "sitelistdlg.h"
#endif

#include "sitelistdlg.h"
#include "appconfig.h"
#include "mainframe.h"
#include <stdio.h>
#include <string.h>
#include <string>

#include "debug.h"

using namespace std;

CSiteListDlg::CSiteListDlg(CWidget* parent)
 : CDialog(parent, _("Site List"), false), m_Store(NULL), m_pParent((CMainFrame*)parent)
{
	GtkWidget *dialog_vbox3;
	GtkWidget *hbox16;
	GtkWidget *vbox4;
	GtkWidget *hbox17;
	GtkWidget *scrolledwindow1;
	GtkWidget *vbuttonbox1;
	GtkWidget *connect_btn;
	GtkWidget *alignment1;
	GtkWidget *hbox18;
	GtkWidget *image349;
	GtkWidget *label26;
	GtkWidget *close_btn;
	GtkWidget *dialog_action_area3;
	
	dialog_vbox3 = GTK_DIALOG (m_Widget)->vbox;
	gtk_widget_show (dialog_vbox3);
	
	hbox16 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox16);
	gtk_box_pack_start (GTK_BOX (dialog_vbox3), hbox16, TRUE, TRUE, 0);
	
	vbox4 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox4);
	gtk_box_pack_start (GTK_BOX (hbox16), vbox4, TRUE, TRUE, 0);
	
	hbox17 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox17);
	gtk_box_pack_start (GTK_BOX (vbox4), hbox17, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox17), 1);
	
	m_Keyword = gtk_entry_new ();
	gtk_widget_show (m_Keyword);
	gtk_box_pack_start (GTK_BOX (hbox17), m_Keyword, TRUE, TRUE, 2);
	
	m_SearchBtn = gtk_button_new_from_stock ("gtk-find");
	gtk_widget_show (m_SearchBtn);
	gtk_box_pack_start (GTK_BOX (hbox17), m_SearchBtn, FALSE, FALSE, 2);
	
	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow1);
	gtk_box_pack_start (GTK_BOX (vbox4), scrolledwindow1, TRUE, TRUE, 2);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 1);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);
	
	m_Tree = gtk_tree_view_new ();
	gtk_widget_show (m_Tree);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), m_Tree);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_Tree), FALSE);
	
	vbuttonbox1 = gtk_vbutton_box_new ();
	gtk_widget_show (vbuttonbox1);
	gtk_box_pack_start (GTK_BOX (hbox16), vbuttonbox1, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbuttonbox1), 1);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (vbuttonbox1), GTK_BUTTONBOX_START);
	
	connect_btn = gtk_button_new ();
	gtk_widget_show (connect_btn);
	gtk_container_add (GTK_CONTAINER (vbuttonbox1), connect_btn);
	GTK_WIDGET_SET_FLAGS (connect_btn, GTK_CAN_DEFAULT);
	
	alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment1);
	gtk_container_add (GTK_CONTAINER (connect_btn), alignment1);
	
	hbox18 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox18);
	gtk_container_add (GTK_CONTAINER (alignment1), hbox18);
	
	image349 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image349);
	gtk_box_pack_start (GTK_BOX (hbox18), image349, FALSE, FALSE, 0);
	
	label26 = gtk_label_new_with_mnemonic (_("C_onnect"));
	gtk_widget_show (label26);
	gtk_box_pack_start (GTK_BOX (hbox18), label26, FALSE, FALSE, 0);
	
	close_btn = gtk_button_new_from_stock ("gtk-close");
	gtk_widget_show (close_btn);
	gtk_container_add (GTK_CONTAINER (vbuttonbox1), close_btn);
	GTK_WIDGET_SET_FLAGS (close_btn, GTK_CAN_DEFAULT);

	g_signal_connect ((gpointer) m_SearchBtn, "clicked",
					G_CALLBACK (&CSiteListDlg::OnSearch),
					this);
	g_signal_connect ((gpointer) connect_btn, "clicked",
					G_CALLBACK (&CSiteListDlg::OnConnect),
					this);
	g_signal_connect ((gpointer) close_btn, "clicked",
					G_CALLBACK (&CSiteListDlg::OnClose),
					this);

	g_signal_connect ((gpointer) m_Tree, "row-activated",
					G_CALLBACK (&CSiteListDlg::OnRowActivated),
					this);

	g_signal_connect ((gpointer) m_Keyword, "activate",
					G_CALLBACK (&CSiteListDlg::OnKeywordActivate),
					this);


	gtk_window_set_default_size((GtkWindow*)m_Widget, 512, 384);

	m_ConnectBtn = connect_btn;

	LoadSiteList();
}


static const char ITEM_SEP[]="        ";

enum
{
	COL_ICON,
	COL_TEXT,
	COL_NUM
};


void CSiteListDlg::OnSearch(GtkButton* btn, CSiteListDlg* _this)
{
	const gchar* keyword = gtk_entry_get_text( GTK_ENTRY(_this->m_Keyword) );
	if( !*keyword )
		return;

	GtkTreeView* tree_view = GTK_TREE_VIEW(_this->m_Tree);
	GtkTreeSelection* sel = gtk_tree_view_get_selection(tree_view);
	GtkTreeModel* model = GTK_TREE_MODEL(_this->m_Store);
	GtkTreeIter it;
	gchar* text;
	bool found = false;

	if( !gtk_tree_selection_get_selected( sel, &model, &it) )
	{
		if( !gtk_tree_model_get_iter_first(model, &it) )
			return;
	}
	else	// Get next iter
	{
get_next_iter:
		if( gtk_tree_model_iter_has_child( model, &it ) ) // Has children?
		{
			GtkTreeIter child_it;
			while( gtk_tree_model_iter_children(model, &child_it, &it) )
				it = child_it;
		}
		else if( !gtk_tree_model_iter_next( model, &it) ) // Has sibling?
		{
		get_parent_iter:
			// No sibling. Has Parent?
			GtkTreeIter parent_it;
			if( !gtk_tree_model_iter_parent( model, &parent_it, &it ) )
				goto keyword_not_found;	// No parent, stop!

			// Does parent has next sibling?
			if( !gtk_tree_model_iter_next( model, &parent_it)  )
			{
				it = parent_it;
				goto get_parent_iter;
			}
			it = parent_it;
		}
	}

	gtk_tree_model_get(model, &it, COL_TEXT, &text, -1);
	found = strstr(text, keyword);
	g_free(text);
	if( found )
	{
		GtkTreePath* path = gtk_tree_model_get_path(model, &it);
		gtk_tree_view_expand_to_path( tree_view, path );
		gtk_tree_selection_select_iter(sel, &it);
		gtk_tree_view_scroll_to_cell( tree_view, path, NULL, false, 0, 0 );
	//	FIXME: GtkTreeView doesn't scroll to the selected path.  Is it a bug?
		gtk_tree_path_free(path);
		return;
	}
	goto get_next_iter;

keyword_not_found:
	GtkWidget* dlg = gtk_message_dialog_new( GTK_WINDOW(_this->m_Widget), 
									GTK_DIALOG_MODAL, 
									GTK_MESSAGE_INFO,
									GTK_BUTTONS_OK,
									_("Keyword \"%s\" not found!"), keyword);
	gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
}

void CSiteListDlg::OnConnect(GtkButton* btn, CSiteListDlg* _this)
{
	GtkTreeView* tree_view = GTK_TREE_VIEW(_this->m_Tree);
	GtkTreeSelection* sel = gtk_tree_view_get_selection(tree_view);
	GtkTreeModel* model;
	GtkTreeIter it;
	if( gtk_tree_selection_get_selected( sel, &model, &it) )
	{
		gchar* text;
		gtk_tree_model_get(GTK_TREE_MODEL(_this->m_Store), &it, COL_TEXT, &text, -1);
		INFO_ON("%s", text);
		char* url = strstr(text, ITEM_SEP);
		if( url )
		{
			*url = '\0';
			url += (sizeof(ITEM_SEP)-1);
			_this->m_pParent->NewCon(text, url, &AppConfig.m_DefaultSite);
		}
		g_free(text);
	}
	gtk_dialog_response( GTK_DIALOG(_this->m_Widget), GTK_RESPONSE_OK );
}

void CSiteListDlg::OnClose(GtkButton* btn, CSiteListDlg* _this)
{
	gtk_dialog_response( GTK_DIALOG(_this->m_Widget), GTK_RESPONSE_CANCEL );
}

void CSiteListDlg::LoadSiteList()
{
	GtkIconSet* icon_set = gtk_icon_factory_lookup_default(GTK_STOCK_NETWORK);
	m_SiteIcon = gtk_icon_set_render_icon(icon_set, m_Tree->style, GTK_TEXT_DIR_NONE, GTK_STATE_NORMAL, GTK_ICON_SIZE_MENU, NULL, NULL);

	/* Workaround of new GTK_STOCK_DIRECTORY macro introduced in GTK+ 2.6 */
	#if !GTK_CHECK_VERSION(2,6,0)
	#define GTK_STOCK_DIRECTORY "gtk-directory"
	#endif
	icon_set = gtk_icon_factory_lookup_default(GTK_STOCK_DIRECTORY);
	m_FolderIcon = gtk_icon_set_render_icon(icon_set, m_Tree->style, GTK_TEXT_DIR_NONE, GTK_STATE_NORMAL, GTK_ICON_SIZE_MENU, NULL, NULL);
//	g_object_unref(icon_set);	??

	GtkTreeView* tree = GTK_TREE_VIEW(m_Tree);

	m_Store = gtk_tree_store_new(COL_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);

	string fpath = AppConfig.GetDataPath("sitelist");
	FILE *file = fopen( fpath.c_str(), "r");
	if( file )
	{
		char buffer[1024];
		LoadSiteList( NULL, file, buffer );
		fclose(file);
	}
	else
	{
//		wxMessageBox( _("Unable to open site list file!"), _("Error"), wxICON_STOP, m_pMainFrame );
	}

	GtkCellRenderer *renderer = GTK_CELL_RENDERER (gtk_cell_renderer_pixbuf_new ());
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes
		("", renderer,
		 "pixbuf", COL_ICON,
//		 "pixbuf-expander-open", COL_ICON,
//		 "pixbuf-expander-closed", COL_ICON,
		 NULL);	

	gtk_tree_view_append_column( tree, col );
	gtk_tree_view_column_pack_start (col, renderer, FALSE);

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("", renderer, "text", COL_TEXT, NULL);
	gtk_tree_view_append_column( tree, col );
	gtk_tree_view_column_pack_start (col, renderer, TRUE);

	gtk_tree_view_set_model(tree, GTK_TREE_MODEL(m_Store));
	g_object_unref(m_Store);

}


void CSiteListDlg::LoadSiteList(GtkTreeIter *parent, FILE *file, char *line)
{
	GtkTreeIter it;
	while( fgets( line, 1024, file ) )
	{
		char* name = strtok( line, "\t\r\n" );
		if( !name || '\0' == *name )
			return;

		if( 'd' == *name )
		{
			gtk_tree_store_append(m_Store, &it, parent);
			//	gtk_tree_store_set_value(m_Store, &it, 0, (GValue*)_("Site List"));
			//	Why can't I use this? What causes the segmentation fault?
			const char* text = name+1;
			gtk_tree_store_set(m_Store, &it, COL_ICON, m_FolderIcon, COL_TEXT, (GValue*)text, -1);
			LoadSiteList( &it, file, line );
		}
		else if( 's' == *name )
		{
			char* URL = strtok( NULL, "\t\r\n" );
			if( URL )
			{
				string text = name+1;
//				text.Replace(" ", "");
				text += ITEM_SEP;
				text += URL;

				gtk_tree_store_append(m_Store, &it, parent);
				gtk_tree_store_set(m_Store, &it, COL_ICON, m_SiteIcon, 1, (GValue*)text.c_str(), -1);
			}
		}
	}
}


void CSiteListDlg::OnRowActivated(GtkTreeView *tree_view, GtkTreePath* path,  
	GtkTreeViewColumn* col, CSiteListDlg* _this)
{
	gtk_button_clicked( GTK_BUTTON(_this->m_ConnectBtn) );
}

void CSiteListDlg::OnKeywordActivate(GtkEntry* entry, CSiteListDlg* _this)
{
	gtk_button_clicked( GTK_BUTTON(_this->m_SearchBtn) );
}
