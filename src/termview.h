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
#ifndef TERMVIEW_H
#define TERMVIEW_H

#include "view.h"
#include "caret.h"

#include <string>

#include <X11/Xft/Xft.h>

using namespace std;

/**
@author PCMan
*/
class CTermData;
class CHyperLink;

class CTermView : public CView
{
friend class CTermData;
public:
    CTermView();

    ~CTermView();
    virtual bool PreKeyDown(GdkEventKey *evt);
    virtual bool OnKeyDown(GdkEventKey* evt);
    virtual void OnTextInput(gchar* string);
    int DrawChar(int line, int col, int top);
    void PrepareDC();
    void PointToLineCol(int *x, int *y);

    GtkIMContext* m_IMContext;

    virtual void OnLButtonDown(GdkEventButton* evt);
    virtual void OnRButtonDown(GdkEventButton* evt);
    virtual void OnLButtonUp(GdkEventButton* evt);
    virtual void OnRButtonUp(GdkEventButton* evt);
    virtual void OnMouseMove(GdkEventMotion* evt);
    void OnBlinkTimer();
    virtual void OnMButtonDown(GdkEventButton* evt);
    void PasteFromClipboard(bool primary);
    virtual void DoPasteFromClipboard(string text, bool contain_ansi_color);
    void CopyToClipboard(bool primary, bool with_color);
protected:
    void OnPaint(GdkEventExpose* evt);
    void OnSetFocus(GdkEventFocus* evt);
    bool IsPosInSel(int x, int y);
    void CorrectSelPos(int &selstartx, int &selstarty, int &selendx, int &selendy);
    void OnCreate();
    void OnSize(GdkEventConfigure* evt);
    void RedrawSel(int oldx, int oldy, int newx, int newy);
    void OnKillFocus(GdkEventFocus *evt);
    void SetFont(const char* name, int size);
    void OnDestroy();
protected:
	CTermData* m_pTermData;
//#ifdef NO_XFT
    PangoLayout* m_PangoLayout;
    PangoFontDescription* m_Font;
//#else
    XftDraw* m_XftDraw;
    XftFont* m_XftFont;
//#endif
	int m_CharW;
	int m_CharH;
	CCaret m_Caret;
	CHyperLink* m_pHyperLink;

	bool m_ShowBlink;
	int m_CharPaddingX;
	int m_CharPaddingY;
    GdkColor* m_pColorTable;
    GdkGC* m_GC;
    bool m_AutoFontSize;

    static string m_s_ANSIColorStr;
    int m_FontSize;
    string m_FontFamily;
};

#endif
