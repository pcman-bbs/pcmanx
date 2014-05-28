/**
 * Copyright (c) 2014 Shock <henryandjay@gmail.com>
 * Copyright (c) 2014 Var <chuangchihchiang@gmail.com>
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

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#ifdef __GNUG__
  #pragma interface "editorview.h"
#endif

#include "pcmanx_utils.h"

#include "telnetview.h"

/**
@author Var, Shock
*/

class CEditor;
class CEditorView : public CTelnetView
{
public:
    CEditorView();
    ~CEditorView();

    CEditor* GetEditor(){  return (CEditor*) m_pTermData;  }

    void UpdateEditor();
    void UpdateEditorCaretPos();

protected:
    bool OnKeyDown(GdkEventKey* evt);
protected:

#if defined(USE_MOUSE) && !defined(MOZ_PLUGIN)
    void OnMouseScroll(GdkEventScroll* evt);
    void OnLButtonDown(GdkEventButton* evt);
#endif // defined(USE_MOUSE) && !defined(MOZ_PLUGIN)
    void DoPasteFromClipboard(string text, bool contain_ansi_color);


};

#endif
