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
#ifndef SITEPAGE_H
#define SITEPAGE_H

#include <widget.h>

/**
@author PCMan
*/
class CSite;

class CSitePage : public CWidget
{
public:
    CSitePage(CSite& site);

    ~CSitePage();
    bool OnOK();

	GtkWidget *m_Name;
	GtkWidget *m_Address;
	GtkWidget *m_Reconnect;
	GtkWidget *m_Idle;
	GtkWidget *m_IdleStr;
	GtkWidget *m_TermType;
	GtkWidget *m_ESCConv;
	GtkWidget *m_Line;
	GtkWidget *m_Col;
	GtkWidget *m_PasteWrap;
	GtkWidget *m_Encoding;
	GtkWidget *m_Startup;
    CSite&  m_Site;
};

#endif
