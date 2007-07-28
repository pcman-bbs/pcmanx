/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


#include <X11/Xlib.h>

#include "plugin.h"
#include "nsIServiceManager.h"
#include "nsISupportsUtils.h" // some usefule macros are defined here

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <ctype.h>

#define MIME_TYPES_HANDLED  "application/pcmanx"
#define PLUGIN_NAME         "PCManX Plug-in for Mozilla/Firefox"
#define MIME_TYPES_DESCRIPTION  MIME_TYPES_HANDLED":pcmanx:"PLUGIN_NAME
#define PLUGIN_DESCRIPTION  PLUGIN_NAME " (Telnet Plug-in)"

#include "site.h"
#include "font.h"
#include "telnetview.h"
#include "telnetcon.h"

static int strcmp_ci( register const char* str1, register const char* str2)
{
	for(; *str1 && *str2; ++str1, ++str2)
	{
		register int ret;
		if( ret = tolower(*str1) - tolower(*str2) )
			return ret;
	}
	return 0;
}

char* NPP_GetMIMEDescription(void)
{
    return(MIME_TYPES_DESCRIPTION);
}

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
}

// get values per plugin
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
  NPError err = NPERR_NO_ERROR;
  switch (aVariable) {
    case NPPVpluginNameString:
      *((char **)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((char **)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      err = NPERR_INVALID_PARAM;
      break;
  }
  return err;
}


/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;

  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(nsPluginCreateData * aCreateDataStruct) : nsPluginInstanceBase(),
  mInstance(aCreateDataStruct->instance),
  mInitialized(FALSE),
  mWindow(0),
  m_pView(NULL),
  m_pCon(NULL),
  m_GtkWidget(NULL),
  mScriptablePeer(NULL)
{
	mString[0] = '\0';
	if( aCreateDataStruct->mode==NP_EMBED )
	{
//		g_print("mode = NP_EMBED\n");
		for( int i=0; i < aCreateDataStruct->argc; ++i )
		{
//			g_print("arg[%d]: %s = %s\n", i, aCreateDataStruct->argn[i], aCreateDataStruct->argv[i]);
			if( 0 == strcmp_ci( "URL", aCreateDataStruct->argn[i] ) )
				m_URL = aCreateDataStruct->argv[i];
			else if( 0 == strcmp_ci( "FontFace", aCreateDataStruct->argn[i] ) )
				m_FontFace = aCreateDataStruct->argv[i];
		}
	}
}

nsPluginInstance::~nsPluginInstance()
{
  // mScriptablePeer may be also held by the browser 
  // so releasing it here does not guarantee that it is over
  // we should take precaution in case it will be called later
  // and zero its mPlugin member
  mScriptablePeer->SetInstance(NULL);
  NS_IF_RELEASE(mScriptablePeer);
}


void nsPluginInstance::draw()
{
}

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
  if(aWindow == NULL)
    return FALSE;

  CTelnetCon::Init();

  if (SetWindow(aWindow))
    mInitialized = TRUE;

  return mInitialized;
}

NPError nsPluginInstance::SetWindow(NPWindow* aWindow)
{
  if(aWindow == NULL)
    return FALSE;

  mX = aWindow->x;
  mY = aWindow->y;
  mWidth = aWindow->width;
  mHeight = aWindow->height;
  if (mWindow != (Window) aWindow->window)
  {
    mWindow = (Window) aWindow->window;
    NPSetWindowCallbackStruct *ws_info = (NPSetWindowCallbackStruct *)aWindow->ws_info;

	printf("set window\n");
	
	m_GtkWidget = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_widget_realize(m_GtkWidget);
	gdk_flush();
	gtk_widget_set_events(m_GtkWidget, GDK_BUTTON_PRESS_MASK);
	gtk_widget_set_events(m_GtkWidget, GDK_KEY_PRESS_MASK);

	gtk_widget_set_size_request(m_GtkWidget, aWindow->width,
				    aWindow->height);

	gdk_flush();

    printf("all setup and ready to reparent and map, m_GtkWidget = %x\n", m_GtkWidget);
 	gtk_widget_show_all(m_GtkWidget);

	XReparentWindow(GDK_WINDOW_XDISPLAY(m_GtkWidget->window),
			GDK_WINDOW_XWINDOW(m_GtkWidget->window), mWindow, 0,
			0);
    printf("reparent and map\n");
	gtk_widget_map(m_GtkWidget);
    printf("gtk_widget_show_all(m_GtkWidget);\n");


	NewCon();
  }
  else if(m_GtkWidget)
  {
	gtk_widget_set_size_request(m_GtkWidget, aWindow->width,
				    aWindow->height);
    gtk_widget_show(m_GtkWidget);
    XResizeWindow(GDK_WINDOW_XDISPLAY(m_GtkWidget->window),
		  GDK_WINDOW_XWINDOW(m_GtkWidget->window),
		  aWindow->width, aWindow->height);
    XResizeWindow(GDK_WINDOW_XDISPLAY(m_GtkWidget->window), mWindow, aWindow->width, aWindow->height);
  }
  return TRUE;
}


gboolean nsPluginInstance::OnBlinkTimer(nsPluginInstance* _this)
{
	if(_this->m_pView && _this->m_pView->IsVisible() )
		_this->m_pView->OnBlinkTimer();
	return true;
}

gboolean nsPluginInstance::OnEverySecondTimer(nsPluginInstance* _this)
{
	if( _this->m_pCon )
		_this->m_pCon->OnTimer();
	return true;
}

void nsPluginInstance::NewCon()
{
	m_BlinkTimer = g_timeout_add(600, (GSourceFunc)nsPluginInstance::OnBlinkTimer, this );
	m_EverySecondTimer = g_timeout_add(1000, (GSourceFunc)nsPluginInstance::OnEverySecondTimer, this );

	m_pView = new CTelnetView;
//	gtk_label_new("PCMan plug-in for Mozilla/Firefox")
	gtk_container_add( GTK_CONTAINER(m_GtkWidget), m_pView->m_Widget);

	CSite site;
	m_pCon = new CTelnetCon( m_pView, site );

	m_pView->SetTermData( m_pCon );
//	m_pView->SetContextMenu(m_EditMenu);
	CFont* font = new CFont(m_FontFace, 12, true);
	m_pView->SetFont(font);
	static GdkColor HyperLinkColor;
	HyperLinkColor.red = 65535;
	HyperLinkColor.green = 65536*102/256;
	HyperLinkColor.blue = 0;
	m_pView->SetHyperLinkColor( &HyperLinkColor );
//	m_pView->SetHorizontalCenterAlign( AppConfig.HCenterAlign );
//	m_pView->m_CharPaddingX = AppConfig.CharPaddingX;
//	m_pView->m_CharPaddingY = AppConfig.CharPaddingY;

	m_pCon->m_Site.m_Name = "";
	m_pCon->m_Site.m_URL = m_URL;
//	g_print("url = %s\n", m_URL.c_str());
	m_pCon->m_Encoding = m_pCon->m_Site.m_Encoding;

	m_pCon->AllocScreenBuf( site.m_RowsPerPage, site.m_RowsPerPage, site.m_ColsPerPage );

	m_pView->SetFocus();

	m_pCon->Connect();
}



void nsPluginInstance::shut()
{
  g_source_remove( m_BlinkTimer );
  g_source_remove( m_EverySecondTimer );

  CTelnetCon::Cleanup();
  mInitialized = FALSE;
}

NPBool nsPluginInstance::isInitialized()
{
  return mInitialized;
}

// this will force to draw a version string in the plugin window
void nsPluginInstance::showVersion()
{
  const char *ua = NPN_UserAgent(mInstance);
  strcpy(mString, ua);
  draw();
}

// this will clean the plugin window
void nsPluginInstance::clear()
{
  strcpy(mString, "");
  draw();
}

bool nsPluginInstance::queryOnExit()
{
	return true;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	nsPluginInstance::GetValue(NPPVariable aVariable, void *aValue)
{
  NPError rv = NPERR_NO_ERROR;

  if (aVariable == NPPVpluginScriptableInstance) {
      // addref happens in getter, so we don't addref here
    PCManX_interface * scriptablePeer = getScriptablePeer();
      if (scriptablePeer) {
        *(nsISupports **)aValue = scriptablePeer;
      } else
        rv = NPERR_OUT_OF_MEMORY_ERROR;
    }
  else if (aVariable == NPPVpluginScriptableIID) {
    static nsIID scriptableIID =  PCMANX_INTERFACE_IID;
      nsIID* ptr = (nsIID *)NPN_MemAlloc(sizeof(nsIID));
      if (ptr) {
          *ptr = scriptableIID;
          *(nsIID **)aValue = ptr;
      } else
        rv = NPERR_OUT_OF_MEMORY_ERROR;
  }

  return rv;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// this method will return the scriptable object (and create it if necessary)
nsScriptablePeer* nsPluginInstance::getScriptablePeer()
{
  if (!mScriptablePeer) {
    mScriptablePeer = new nsScriptablePeer(this);
    if(!mScriptablePeer)
      return NULL;

    NS_ADDREF(mScriptablePeer);
  }

  // add reference for the caller requesting the object
  NS_ADDREF(mScriptablePeer);
  return mScriptablePeer;
}
