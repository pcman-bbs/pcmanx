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

#ifndef DOWNARTICLEDLG_H
#define DOWNARTICLEDLG_H

#include "dialog.h"
#include "telnetcon.h"


class CDownArticleDlg : public CDialog
{
public:
	CDownArticleDlg(CWidget *parent, CTelnetCon *connection);
	
	int ShowModal();

private:
	void OnCommand(int id);
	void OnDestroy();
	bool CopyToClipboard();
	bool SaveAs();
	static void DownArticleFunc(CDownArticleDlg *_this);

	GtkTextBuffer	*m_textbuf;
	GtkTextView		*m_textview;
	GtkButton		*m_btncopy;
	GtkButton		*m_btnsave;
	GtkButton		*m_btncancel;

	CTelnetCon		*m_connection;
	GThread			*m_thread;
	bool			m_stop;
};

#endif // DOWNARTICLEDLG_H
