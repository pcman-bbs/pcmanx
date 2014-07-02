/**
 * Copyright (C) 2013 Roy Lu <roymercadian@gmail.com>
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

#ifndef KEYSETTINGPAGE_H
#define KEYSETTINGPAGE_H

#ifdef __GNUG__
  #pragma interface "keysettingpage.h"
#endif
#include <gdk/gdkkeysyms.h>
#include <string>
#include <widget.h>

using std::string;

#define KEY_SIZE 19

class CKeySettingPage: public CWidget
{
public:
	CKeySettingPage(GtkWidget *parent);
	void OnOK();
public:
	enum KeyNames {
		keySiteList, keyNewConn0, keyNewConn1, keyReconn0, keyReconn1,
		keyClose0, keyClose1, keyNextPage, keyPrevPage, keyFirstPage,
		keyLastPage, keyCopy0,	keyCopy1, keyPaste0, keyPaste1,
		keyPasteClipboard, keyEmotions, keyFullscreen, keyShowMainWindow
	};

	static GtkWidget *m_Parent;
	GtkWidget *m_BtnReset;
	GtkWidget *m_LabelInfo;
	GtkWidget *m_Labels[KEY_SIZE];
	GtkWidget *m_Entries[KEY_SIZE];
	GtkWidget *m_InputDialog;
	static GtkWidget *m_CurrentEntry;

	// reset button event handler
	gboolean onBtnResetPress(GtkWidget *widget, GdkEvent *event, gpointer data);
	static gboolean onBtnResetPressProxy(GtkWidget *widget, GdkEvent *event, gpointer user_data);

	// show hot key input dialog
	gboolean showInputDialog(GtkWidget *widget, GdkEvent *event, gpointer data);
	static gboolean showInputDialogProxy(GtkWidget *widget, GdkEvent *event, gpointer data);

	// hot key input event handler
	gboolean onKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer data);
	static gboolean onKeyPressProxy(GtkWidget *widget, GdkEventKey *event, gpointer data);
};

#endif
