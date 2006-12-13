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
  #pragma implementation "notebook.h"
#endif


#include "notebook.h"

#include "debug.h"

CNotebook::CNotebook()
        : CWidget()
{
	m_Widget = gtk_notebook_new();
	CWidget::PostCreate();

	Show();
}


CNotebook::~CNotebook()
{}

/*
void CNotebook::InsertPage(int pos, CWidget* page, string title)
{

}
*/

int CNotebook::AddPage( CWidget* page, string title, GdkPixbuf* icon)
{
	GtkWidget* text_label = gtk_label_new(NULL);
	gtk_widget_show(text_label);
	gtk_label_set_markup (GTK_LABEL(text_label), title.c_str());
	INFO("label = %X\n", text_label);
	GtkWidget* label = text_label;
	if(icon)
	{
		GtkWidget* hbox = gtk_hbox_new(false, 4);
		gtk_widget_show(hbox);
//		g_object_ref(icon);
		GtkWidget* image = gtk_image_new_from_pixbuf(icon);
		gtk_widget_show(image);
		gtk_box_pack_start(GTK_BOX(hbox), image, false, false, 4);
		gtk_box_pack_start(GTK_BOX(hbox), text_label, false, false, 4);
		label = hbox;
	}
	int ret = gtk_notebook_append_page( GTK_NOTEBOOK(m_Widget), page->m_Widget, label );
	return ret;
}


void CNotebook::SetPageTitle(CWidget* page, string title)
{
	GtkWidget* label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(m_Widget), page->m_Widget);

	if( !GTK_IS_LABEL(label) )
	{
		GList *list = gtk_container_get_children(GTK_CONTAINER(label));
		label = GTK_WIDGET(g_list_last(list)->data);
		g_list_free(list);
	}
	gtk_label_set_markup( GTK_LABEL(label), title.c_str() );
}
