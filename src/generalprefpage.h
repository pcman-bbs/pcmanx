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

#ifndef GENERALPREFPAGE_H
#define GENERALPREFPAGE_H

#ifdef __GNUG__
  #pragma interface "generalprefpage.h"
#endif

#include <widget.h>

/**
@author PCMan
*/
class CGeneralPrefPage : public CWidget
{
public:
    CGeneralPrefPage();
    void OnOK();
public:
	GtkWidget *m_QueryOnCloseCon;
	GtkWidget *m_QueryOnExit;
	GtkWidget *m_CancelSelAfterCopy;
	GtkWidget *m_ShowTrayIcon;
	GtkWidget *m_WebBrowser;
	GtkWidget *m_AAFont;
	GtkWidget *m_PopupNotifier;
	GtkWidget *m_PopupTimeout;

};

#endif
