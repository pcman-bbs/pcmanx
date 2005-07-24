//
// C++ Implementation: autologinpage
//
// Description: 
//
//
// Author: Hong Jen Yee (PCMan) <hzysoft@sina.com.tw>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "autologinpage.h"
#include "site.h"

CAutoLoginPage::CAutoLoginPage(CSite& site) : m_Site(site)
{
	GtkWidget *hbox21;
	GtkWidget *label30;
	GtkWidget *label36;
	GtkWidget *hbox22;
	GtkWidget *label31;
	GtkWidget *label34;
	GtkWidget *hbox23;
	GtkWidget *label32;
	GtkWidget *label35;
	GtkWidget *hbox24;
	GtkWidget *label37;
	GtkWidget *label38;

	m_Widget = gtk_vbox_new (FALSE, 4);
	PostCreate();

	gtk_widget_show (m_Widget);
	gtk_container_set_border_width (GTK_CONTAINER (m_Widget), 2);

	hbox21 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox21);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox21, FALSE, TRUE, 0);
	
	label30 = gtk_label_new (_("Prelogin Prompt: "));
	gtk_widget_show (label30);
	gtk_box_pack_start (GTK_BOX (hbox21), label30, FALSE, FALSE, 0);
	
	m_PreLoginPrompt = gtk_entry_new ();
	gtk_widget_show (m_PreLoginPrompt);
	gtk_box_pack_start (GTK_BOX (hbox21), m_PreLoginPrompt, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_PreLoginPrompt, 100, -1);
	
	label36 = gtk_label_new (_("Prelogin String: "));
	gtk_widget_show (label36);
	gtk_box_pack_start (GTK_BOX (hbox21), label36, FALSE, FALSE, 0);
	
	m_PreLogin = gtk_entry_new ();
	gtk_widget_show (m_PreLogin);
	gtk_box_pack_start (GTK_BOX (hbox21), m_PreLogin, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_PreLogin, 100, -1);
	
	hbox22 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox22);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox22, FALSE, TRUE, 0);
	
	label31 = gtk_label_new (_("Login Promp: "));
	gtk_widget_show (label31);
	gtk_box_pack_start (GTK_BOX (hbox22), label31, FALSE, FALSE, 0);
	
	m_LoginPrompt = gtk_entry_new ();
	gtk_widget_show (m_LoginPrompt);
	gtk_box_pack_start (GTK_BOX (hbox22), m_LoginPrompt, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_LoginPrompt, 100, -1);
	
	label34 = gtk_label_new (_("User Name:"));
	gtk_widget_show (label34);
	gtk_box_pack_start (GTK_BOX (hbox22), label34, FALSE, FALSE, 0);
	
	m_Login = gtk_entry_new ();
	gtk_widget_show (m_Login);
	gtk_box_pack_start (GTK_BOX (hbox22), m_Login, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_Login, 100, -1);
	
	hbox23 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox23);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox23, FALSE, TRUE, 0);
	
	label32 = gtk_label_new (_("Password Prompt: "));
	gtk_widget_show (label32);
	gtk_box_pack_start (GTK_BOX (hbox23), label32, FALSE, FALSE, 0);
	
	m_PasswdPrompt = gtk_entry_new ();
	gtk_widget_show (m_PasswdPrompt);
	gtk_box_pack_start (GTK_BOX (hbox23), m_PasswdPrompt, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_PasswdPrompt, 100, -1);
	
	label35 = gtk_label_new (_("Password: "));
	gtk_widget_show (label35);
	gtk_box_pack_start (GTK_BOX (hbox23), label35, FALSE, FALSE, 0);
	
	m_Passwd = gtk_entry_new ();
	gtk_widget_show (m_Passwd);
	gtk_box_pack_start (GTK_BOX (hbox23), m_Passwd, TRUE, TRUE, 0);
	gtk_widget_set_size_request (m_Passwd, 100, -1);
	
	hbox24 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox24);
	gtk_box_pack_start (GTK_BOX (m_Widget), hbox24, FALSE, TRUE, 0);
	
	label37 = gtk_label_new (_("Post Login String: "));
	gtk_widget_show (label37);
	gtk_box_pack_start (GTK_BOX (hbox24), label37, FALSE, FALSE, 0);
	
	m_PostLogin = gtk_entry_new ();
	gtk_widget_show (m_PostLogin);
	gtk_box_pack_start (GTK_BOX (hbox24), m_PostLogin, TRUE, TRUE, 0);

	label38 = gtk_label_new (_("<big>WARNING:</big> Your password will be stored in <u>plain text</u> without any encryption.  Use this at your own risk.  Although UNIX-like systems all have file permissions, there is no guarantee that others won't get your password."));
	gtk_widget_show (label38);
	gtk_box_pack_start (GTK_BOX (m_Widget), label38, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label38), TRUE);
	gtk_label_set_line_wrap (GTK_LABEL (label38), TRUE);


	gtk_widget_show_all(m_Widget);


	gtk_entry_set_text(GTK_ENTRY(m_PreLoginPrompt), 
						m_Site.GetPreLoginPrompt().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_PreLogin), m_Site.GetPreLogin().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_LoginPrompt), m_Site.GetLoginPrompt().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Login), m_Site.GetLogin().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_PasswdPrompt), m_Site.GetPasswdPrompt().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_Passwd), m_Site.GetPasswd().c_str());
	gtk_entry_set_text(GTK_ENTRY(m_PostLogin), m_Site.GetPostLogin().c_str());

	gtk_entry_set_visibility(GTK_ENTRY(m_Passwd), false);
}


CAutoLoginPage::~CAutoLoginPage()
{
}


bool CAutoLoginPage::OnOK()
{
	m_Site.SetPreLoginPrompt( gtk_entry_get_text(GTK_ENTRY(m_PreLoginPrompt)) );
	m_Site.SetPreLogin( gtk_entry_get_text(GTK_ENTRY(m_PreLogin)) );
	m_Site.SetLoginPrompt(gtk_entry_get_text(GTK_ENTRY(m_LoginPrompt)) );
	m_Site.SetLogin(gtk_entry_get_text(GTK_ENTRY(m_Login)) );
	m_Site.SetPasswdPrompt(gtk_entry_get_text(GTK_ENTRY(m_PasswdPrompt)) );
	m_Site.SetPasswd(gtk_entry_get_text(GTK_ENTRY(m_Passwd)) );
	m_Site.SetPostLogin(gtk_entry_get_text(GTK_ENTRY(m_PostLogin)) );

	return true;
}
