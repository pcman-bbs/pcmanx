//
// C++ Interface: autologinpage
//
// Description: 
//
//
// Author: Hong Jen Yee (PCMan) <pcman.tw@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUTOLOGINPAGE_H
#define AUTOLOGINPAGE_H

#include "widget.h"
#include <glib/gi18n.h>

class CSite;

class CAutoLoginPage : public CWidget
{
public:
    CAutoLoginPage(CSite& site);

    ~CAutoLoginPage();
    bool OnOK();

	GtkWidget *AutoLoginPage;
	GtkWidget *m_PreLoginPrompt;
	GtkWidget *m_PreLogin;
	GtkWidget *m_PostLogin;
	GtkWidget *m_LoginPrompt;
	GtkWidget *m_Login;
	GtkWidget *m_PasswdPrompt;
	GtkWidget* m_Passwd;

	CSite& m_Site;
};

#endif
