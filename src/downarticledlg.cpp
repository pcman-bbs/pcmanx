/**
 * Copyright (c) 2008 Jason Xia <jasonxh@gmail.com>
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

#include "downarticledlg.h"
#include <fstream>
#include <cerrno>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>

#define CUSTOM_RESPONSE_COPY 1
#define CUSTOM_RESPONSE_SAVE 2

#define DOWN_ARTICLE_POLL_DELAY 100000  // in microsecond


CDownArticleDlg::CDownArticleDlg(CWidget *parent, CTelnetCon *connection)
	: CDialog(parent, _("Download Article"), false)
	  , m_connection(connection), m_thread(NULL), m_stop(false)
{
	// Build the dialog
	m_textbuf = gtk_text_buffer_new(NULL);
	m_textview = (GtkTextView*) gtk_text_view_new_with_buffer(m_textbuf);
	gtk_text_view_set_editable(m_textview, FALSE);
	gtk_text_buffer_set_text(m_textbuf, _("Downloading ..."), -1);
	GtkScrolledWindow *scroll = (GtkScrolledWindow*) gtk_scrolled_window_new
		(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(m_textview));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(m_Widget)->vbox), 
			GTK_WIDGET(scroll));
	gtk_widget_show_all(GTK_DIALOG(m_Widget)->vbox);

	m_btncopy = (GtkButton*) gtk_dialog_add_button(GTK_DIALOG(m_Widget),
			GTK_STOCK_COPY, CUSTOM_RESPONSE_COPY);
	m_btncancel = (GtkButton*) gtk_dialog_add_button(GTK_DIALOG(m_Widget),
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	m_btnsave = (GtkButton*) gtk_dialog_add_button(GTK_DIALOG(m_Widget),
			GTK_STOCK_SAVE_AS, CUSTOM_RESPONSE_SAVE);
	gtk_widget_set_sensitive(GTK_WIDGET(m_btncopy), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(m_btnsave), FALSE);

	gtk_window_set_default_size(GTK_WINDOW(m_Widget), 600, 600);
	gtk_window_set_modal(GTK_WINDOW(m_Widget), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(m_Widget), TRUE);
}

static inline string& AppendLine(const char *line, const char *enc, string &str)
{
	unsigned int len;
	char *buf = g_convert_with_fallback(line, -1, "UTF-8", enc, (gchar *) "?", 
			NULL, &len, NULL);
	if (buf)
	{
		// Trim trailing spaces
		while (len > 0 && buf[len - 1] == ' ')
			len--;
		str.append(buf, len);
		g_free(buf);
	}
	str.push_back('\n');

	return str;
}

void CDownArticleDlg::DownArticleFunc(CDownArticleDlg *_this)
{
	CTelnetCon *con = _this->m_connection;
	string str;

	// Save the current screen
	for (int i = 0; i < con->m_RowsPerPage - 1; i++)
		AppendLine(con->m_Screen[con->m_FirstLine + i], 
				con->m_Site.m_Encoding.c_str(), str);

	// Check if we are at the end
	char *p;
	while ((p = strchr(con->m_Screen[con->m_FirstLine + con->m_RowsPerPage - 1], '%'))
			&& (*(--p) != '0' || *(--p) != '0' || *(--p) != '1'))
	{
		// Scroll down one line at a time until the end of article
		unsigned int line = con->m_LineCounter;
		con->SendRawString("\x1bOB",3); // Down

		// Wait for the new line to be received in full
		while (line == con->m_LineCounter 
				|| con->m_CaretPos.y < con->m_FirstLine + con->m_RowsPerPage - 1 
				|| con->m_CaretPos.x < 50)
		{
			if (_this->m_stop)  // We've been called off
				goto _exit;
			usleep(DOWN_ARTICLE_POLL_DELAY);
		}

		AppendLine(con->m_Screen[con->m_FirstLine + con->m_RowsPerPage - 2], 
				con->m_Site.m_Encoding.c_str(), str);
	}

	gdk_threads_enter();
	gtk_text_buffer_set_text(_this->m_textbuf, str.data(), str.size());
	gtk_widget_set_sensitive(GTK_WIDGET(_this->m_btncopy), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(_this->m_btnsave), TRUE);
	gtk_widget_grab_default(GTK_WIDGET(_this->m_btnsave));
	gdk_threads_leave();

_exit:
	return;
}

int CDownArticleDlg::ShowModal()
{
	//Create the download thread
	m_thread = g_thread_create((GThreadFunc)DownArticleFunc, this, true, 
			NULL);

	return CDialog::ShowModal();
}

void CDownArticleDlg::OnCommand(int id)
{
	switch (id)
	{
		case CUSTOM_RESPONSE_COPY: // Copy to clipboard
			if (!CopyToClipboard())
				g_signal_stop_emission_by_name(m_Widget, "response");
			break;

		case CUSTOM_RESPONSE_SAVE: // Save as
			if (!SaveAs())
				g_signal_stop_emission_by_name(m_Widget, "response");
			break;
	}
}

void CDownArticleDlg::OnDestroy()
{
	// Stop the download thread and exit
	if (m_thread)
	{
		m_stop = true;
		g_thread_join(m_thread);
		m_thread = NULL;
	}

	CDialog::OnDestroy();
}

bool CDownArticleDlg::CopyToClipboard()
{
	GtkTextIter begin, end;

	gtk_text_buffer_get_iter_at_offset(m_textbuf, &begin, 0);
	gtk_text_buffer_get_iter_at_offset(m_textbuf, &end, -1);
	gtk_text_buffer_select_range(m_textbuf, &begin, &end);
	gtk_text_buffer_copy_clipboard(m_textbuf, 
			gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

	return true;
}

bool CDownArticleDlg::SaveAs()
{
	bool ret = false;
	GtkDialog *dlg = (GtkDialog*) gtk_file_chooser_dialog_new(_("Save As"),
			GTK_WINDOW(m_Widget), GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_OK,
			NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg),
			TRUE);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), 
			getenv("HOME"));
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg), "untitled.txt");

	int resp = gtk_dialog_run(dlg);
	char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
	gtk_widget_destroy(GTK_WIDGET(dlg));

	if (resp == GTK_RESPONSE_OK)
	{
		ofstream ofs(fname, ios_base::out | ios_base::trunc);
		if (ofs)
		{
			GtkTextIter begin, end;
			gtk_text_buffer_get_iter_at_offset(m_textbuf, &begin, 0);
			gtk_text_buffer_get_iter_at_offset(m_textbuf, &end, -1);
			char *text = gtk_text_buffer_get_text(m_textbuf, &begin, &end, 
					FALSE);
			ofs << text;
			g_free(text);
			ret = ofs.good();
		}
		if (!ret)
		{	// Something bad happened
			GtkDialog *msgdlg = (GtkDialog*) gtk_message_dialog_new(
					GTK_WINDOW(m_Widget), GTK_DIALOG_DESTROY_WITH_PARENT, 
					GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, 
					_("Error saving file '%s': %s"), 
					fname, g_strerror(errno));
			gtk_dialog_run(msgdlg);
			gtk_widget_destroy(GTK_WIDGET(msgdlg));
		}
	}
	g_free(fname);

	return ret;
}
