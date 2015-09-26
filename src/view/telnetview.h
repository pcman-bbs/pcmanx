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

#ifndef TELNETVIEW_H
#define TELNETVIEW_H

#ifdef __GNUG__
  #pragma interface "telnetview.h"
#endif

#include "pcmanx_utils.h"

#include "termview.h"

#ifndef MOZ_PLUGIN
#include "qqwryseeker.h"
#endif

/**
@author PCMan
*/
class CTelnetCon;
class CMainFrame;
class CTelnetView : public CTermView
{
friend class CMainFrame;
public:
    CTelnetView();
    ~CTelnetView();

    void OnTextInput(const gchar* text);
    void OnHyperlinkClicked(string url);
    CTelnetCon* GetCon(){  return (CTelnetCon*)m_pTermData;  }

#if !defined(MOZ_PLUGIN)
	static void SetParentFrame(CMainFrame* frame){	m_pParentFrame = frame;	}
	static CMainFrame* GetParentFrame(){	return m_pParentFrame;	}
#endif /* !defined(MOZ_PLUGIN) */

	static void SetWebBrowser(string browser){	m_WebBrowser = browser;	}
	static void SetMailClient(string mailclient){	m_MailClient = mailclient;	}
#ifdef USE_WGET
	static void setWgetFiles(const bool bBool){ m_bWgetFiles = bBool; }
#endif

protected:
    bool OnKeyDown(GdkEventKey* evt);
protected:
    void OnMouseMove(GdkEventMotion* evt);
#if defined(USE_MOUSE) && !defined(MOZ_PLUGIN)    
    void OnMouseScroll(GdkEventScroll* evt);
    void OnLButtonUp(GdkEventButton* evt);
#endif // defined(USE_MOUSE) && !defined(MOZ_PLUGIN)
    void OnRButtonDown(GdkEventButton* evt);
    bool PreKeyDown(GdkEventKey* evt);
    virtual void DoPasteFromClipboard(string text, bool contain_ansi_color);
    void OnDestroy();

#if !defined(MOZ_PLUGIN)
protected:
    static CMainFrame* m_pParentFrame;
    QQWrySeeker *m_pIpSeeker;
#endif /* !defined(MOZ_PLUGIN) */
    static string m_WebBrowser;
    static string m_MailClient;
#ifdef USE_WGET
	static bool m_bWgetFiles;
#endif
};

#endif
