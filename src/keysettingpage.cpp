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

#ifdef __GNUG__
  #pragma implementation "keysettingpage.h"
#endif

#include <glib/gi18n.h>
#include "appconfig.h"
#include "keysettingpage.h"

GtkWidget *CKeySettingPage::m_CurrentEntry;
GtkWidget *CKeySettingPage::m_Parent;


CKeySettingPage::CKeySettingPage(GtkWidget *parent): CWidget()
{
	m_Parent = parent;
	// create gtk table layout
	m_Widget = gtk_table_new(10, 4, TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(m_Widget), 0);
	gtk_table_set_col_spacings(GTK_TABLE(m_Widget), 0);

	gtk_widget_show (m_Widget);
	PostCreate();

	// set Reset Button
	m_BtnReset = gtk_button_new_with_label("Reset Key Settings");
	gtk_widget_show(m_BtnReset);
	gtk_table_attach_defaults(GTK_TABLE(m_Widget), m_BtnReset, 3, 4, 0, 1);

	// set labels
	m_LabelInfo = gtk_label_new ("Click the textbox and press your hotkeys.");
	gtk_widget_show(m_LabelInfo);
	gtk_table_attach_defaults(GTK_TABLE(m_Widget), m_LabelInfo, 0, 3, 0, 1);

	string labelNames[KEY_SIZE] = {
		"Site List", "New Connection 0", "New Connection 1", "Reconnection 0", "Reconnection 1",
		"Close 0", "Close 1", "Next Page", "Previous Page", "First Page",
		"Last Page", "Copy 0", "Copy 1", "Paste 0", "Paste 1",
		"Paste from clipboard", "Emotions", "Full screen mode", "Show main window"
	};

	for (int i = 0; i < KEY_SIZE; ++i) {
		m_Labels[i] = gtk_label_new (_(labelNames[i].c_str()));
		gtk_widget_show(m_Labels[i]);
		// add labels into gtk table layout
		gtk_table_attach_defaults(GTK_TABLE(m_Widget),
				m_Labels[i],
				(int)(i / 10) * 2,
				(int)(i / 10) * 2 + 1,
				(int)(i % 10) + 1,
				(int)(i % 10) + 2);
	}

	// set entries
	for (int i = 0; i < KEY_SIZE; ++i) {
		m_Entries[i] = gtk_entry_new();
		gtk_widget_show(m_Entries[i]);
		gtk_table_attach_defaults(GTK_TABLE(m_Widget),
				m_Entries[i],
				(int)(i / 10) * 2 + 1,
				(int)(i / 10) * 2 + 2,
				(int)(i % 10) + 1,
				(int)(i % 10) + 2);
		gtk_editable_set_editable(GTK_EDITABLE(m_Entries[i]), FALSE);
	}

	// set entry values
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keySiteList]), AppConfig.keySiteList.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyNewConn0]), AppConfig.keyNewConn0.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyNewConn1]), AppConfig.keyNewConn1.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyReconn0]), AppConfig.keyReconn0.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyReconn1]), AppConfig.keyReconn1.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyClose0]), AppConfig.keyClose0.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyClose1]), AppConfig.keyClose1.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyNextPage]), AppConfig.keyNextPage.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyPrevPage]), AppConfig.keyPrevPage.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyFirstPage]), AppConfig.keyFirstPage.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyLastPage]), AppConfig.keyLastPage.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyCopy0]), AppConfig.keyCopy0.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyCopy1]), AppConfig.keyCopy1.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyPaste0]), AppConfig.keyPaste0.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyPaste1]), AppConfig.keyPaste1.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyPasteClipboard]), AppConfig.keyPasteClipboard.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyEmotions]), AppConfig.keyEmotions.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyFullscreen]), AppConfig.keyFullscreen.c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Entries[keyShowMainWindow]), AppConfig.keyShowMainWindow.c_str());


	// set event handler
	// connect every entries with event handler
	for(int i = 0; i < KEY_SIZE; ++i){
		g_signal_connect(GTK_OBJECT(m_Entries[i]), "button-press-event", G_CALLBACK(showInputDialogProxy), m_Entries[i]);
	}

	// set Reset Button event handler
	g_signal_connect(G_OBJECT (m_BtnReset), "button_press_event", G_CALLBACK (onBtnResetPressProxy), m_Entries);
}

/**
 * Get value from gtk entries and save them into AppConfig
 */
void CKeySettingPage::OnOK()
{
	// show message dialog
	GtkWidget *MessageDialog = gtk_message_dialog_new(
			GTK_WINDOW(m_Parent),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"The new settings will apply after restart PCManX."
	);
	gtk_window_set_title(GTK_WINDOW(MessageDialog), "Caution");
	gtk_dialog_run(GTK_DIALOG(MessageDialog));
	gtk_widget_destroy(MessageDialog);

	AppConfig.keySiteList = gtk_entry_get_text(GTK_ENTRY(m_Entries[keySiteList]));
	AppConfig.keyNewConn0 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyNewConn0]));
	AppConfig.keyNewConn1 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyNewConn1]));
	AppConfig.keyReconn0 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyReconn0]));
	AppConfig.keyReconn1 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyReconn1]));
	AppConfig.keyClose0 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyClose0]));
	AppConfig.keyClose1 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyClose1]));
	AppConfig.keyNextPage = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyNextPage]));
	AppConfig.keyPrevPage = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyPrevPage]));
	AppConfig.keyFirstPage = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyFirstPage]));
	AppConfig.keyLastPage = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyLastPage]));
	AppConfig.keyCopy0 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyCopy0]));
	AppConfig.keyCopy1 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyCopy1]));
	AppConfig.keyPaste0 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyPaste0]));
	AppConfig.keyPaste1 = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyPaste1]));
	AppConfig.keyPasteClipboard = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyPasteClipboard]));
	AppConfig.keyEmotions = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyEmotions]));
	AppConfig.keyFullscreen = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyFullscreen]));
	AppConfig.keyShowMainWindow = gtk_entry_get_text(GTK_ENTRY(m_Entries[keyShowMainWindow]));
}

gboolean CKeySettingPage::onKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	GtkWidget *inputDialog = (GtkWidget *) data;
	string pressedKey = "";

	// get modifier keys
	if (event->state & GDK_SHIFT_MASK) {
		pressedKey += "<Shift>";
	}
	if (event->state & GDK_CONTROL_MASK) {
		pressedKey += "<Ctrl>";
	}
	if (event->state & GDK_MOD1_MASK) {
		pressedKey += "<Alt>";
	}

	int keyValue = event->keyval;
	bool isLegalKey = false;

	switch (keyValue) {
	case GDK_KEY_Escape:	//press esc key to close dialog.
		gtk_widget_destroy(inputDialog);
		return false;
	case GDK_KEY_F1:
		pressedKey += "F1";
		isLegalKey = true;
		break;
	case GDK_KEY_F2:
		pressedKey += "F2";
		isLegalKey = true;
		break;
	case GDK_KEY_F3:
		pressedKey += "F3";
		isLegalKey = true;
		break;
	case GDK_KEY_F4:
		pressedKey += "F4";
		isLegalKey = true;
		break;
	case GDK_KEY_F5:
		pressedKey += "F5";
		isLegalKey = true;
		break;
	case GDK_KEY_F6:
		pressedKey += "F6";
		isLegalKey = true;
		break;
	case GDK_KEY_F7:
		pressedKey += "F7";
		isLegalKey = true;
		break;
	case GDK_KEY_F8:
		pressedKey += "F8";
		isLegalKey = true;
		break;
	case GDK_KEY_F9:
		pressedKey += "F9";
		isLegalKey = true;
		break;
	case GDK_KEY_F10:
		pressedKey += "F10";
		isLegalKey = true;
		break;
	case GDK_KEY_F11:
		pressedKey += "F11";
		isLegalKey = true;
		break;
	case GDK_KEY_F12:
		pressedKey += "F12";
		isLegalKey = true;
		break;
	default:	//other characters: 0-9, a-z, A-Z
		if ((keyValue >= 65 && keyValue <= 90) ||
				(keyValue >= 97 && keyValue <= 122) ||
				(keyValue >= 48 && keyValue <= 57))
		{
			pressedKey.append(1u, (char)keyValue);
			isLegalKey = true;
		}
		break;
	}
	if(isLegalKey){
		gtk_entry_set_text(GTK_ENTRY(m_CurrentEntry), pressedKey.c_str());	//set hot keys
		gtk_widget_destroy(inputDialog);									//and destroy the input dialog
	}

	return FALSE;
}

/**
 * proxy function of onKeyPress since GTK restricted support for class method.
 */
gboolean CKeySettingPage::onKeyPressProxy(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	CKeySettingPage *_this = static_cast<CKeySettingPage*>(data);
	return _this->onKeyPress(widget, event, data);
}

/**
 * @brief CKeySettingPage::onBtnResetPress reset hot key setting to default value.
 */
gboolean CKeySettingPage::onBtnResetPress(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	//show question dialog
	GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(m_Parent),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,
			"Do you want to reset the key setting?"
	);
	gtk_window_set_title(GTK_WINDOW(dialog), "Question");
	switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
	case GTK_RESPONSE_YES:
		break;
	default:
		gtk_widget_destroy(dialog);
		return FALSE;
	}

	gtk_widget_destroy(dialog);

	GtkWidget **entries = (GtkWidget **)data;

	gtk_entry_set_text(GTK_ENTRY(entries[keySiteList]), "<Alt>S");
	gtk_entry_set_text(GTK_ENTRY(entries[keyNewConn0]), "<Alt>Q");
	gtk_entry_set_text(GTK_ENTRY(entries[keyNewConn1]), "<Ctrl><Shift>T");
	gtk_entry_set_text(GTK_ENTRY(entries[keyReconn0]), "<Alt>R");
	gtk_entry_set_text(GTK_ENTRY(entries[keyReconn1]), "<Ctrl>Insert");
	gtk_entry_set_text(GTK_ENTRY(entries[keyClose0]), "<Alt>W");
	gtk_entry_set_text(GTK_ENTRY(entries[keyClose1]), "<Ctrl>Delete");
	gtk_entry_set_text(GTK_ENTRY(entries[keyNextPage]), "<Alt>X");
	gtk_entry_set_text(GTK_ENTRY(entries[keyPrevPage]), "<Alt>Z");
	gtk_entry_set_text(GTK_ENTRY(entries[keyFirstPage]), "<Ctrl>Home");
	gtk_entry_set_text(GTK_ENTRY(entries[keyLastPage]), "<Ctrl>End");
	gtk_entry_set_text(GTK_ENTRY(entries[keyCopy0]), "<Alt>O");
	gtk_entry_set_text(GTK_ENTRY(entries[keyCopy1]), "<Ctrl><Shift>C");
	gtk_entry_set_text(GTK_ENTRY(entries[keyPaste0]), "<Alt>P");
	gtk_entry_set_text(GTK_ENTRY(entries[keyPaste1]), "<Ctrl><Shift>V");
	gtk_entry_set_text(GTK_ENTRY(entries[keyPasteClipboard]), "<Shift>Insert");
	gtk_entry_set_text(GTK_ENTRY(entries[keyEmotions]), "<Ctrl>Return");
	gtk_entry_set_text(GTK_ENTRY(entries[keyFullscreen]), "<ALT>Return");
	gtk_entry_set_text(GTK_ENTRY(entries[keyShowMainWindow]), "<Alt>M");

	return FALSE;
}

/**
 * static proxy function of onBtnResetPress
 */
gboolean CKeySettingPage::onBtnResetPressProxy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	CKeySettingPage *_this = static_cast<CKeySettingPage*>(data);
	return _this->onBtnResetPress(widget, event, data);
}


gboolean CKeySettingPage::showInputDialog(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	// set current entry.
	m_CurrentEntry = (GtkWidget *)data;

	// create a input dialog
	GtkWidget *inputDialog = gtk_message_dialog_new(
			GTK_WINDOW(m_Parent),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_NONE,
			"Please press the key combination.\n\nPress ESC to leave key setting."
	);
	gtk_window_set_title(GTK_WINDOW(inputDialog), "Hot key input dialog");
	gtk_window_set_resizable(GTK_WINDOW(inputDialog), false);

	//pass inputDialog to the callback function so that the callback function can destroy this dialog.
	g_signal_connect(G_OBJECT(inputDialog), "key_press_event", G_CALLBACK (onKeyPressProxy), inputDialog);

	gtk_dialog_run(GTK_DIALOG(inputDialog));

	return false;
}

/**
 * @brief proxy function for showInputDialog()
 */
gboolean CKeySettingPage::showInputDialogProxy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	CKeySettingPage *_this = static_cast<CKeySettingPage*>(data);
	return _this->showInputDialog(widget, event, data);
}
