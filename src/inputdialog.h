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

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#ifdef __GNUG__
  #pragma interface "inputdialog.h"
#endif

#include "dialog.h"
#include <string>
using namespace std;

/**
@author PCMan
*/
class CInputDialog : public CDialog
{
public:
    CInputDialog(CWidget* parent, const char* title, const char* prompt, const char* text=NULL, bool can_be_empty=false);

    bool OnOK();

	string GetText(){	return m_Text;	}
	void SetText(string text){	if(m_Entry)gtk_entry_set_text(m_Entry, text.c_str());	}
	GtkEntry* GetEntry(){	return GTK_ENTRY(m_Entry);	}
    gulong m_KeyPressHandlerId;

protected:
    GtkEntry* m_Entry;
    string m_Text;
    bool m_CanBeEmpty;
};

#endif
