/**
 * Copyright (c) 2014 Roy Lu <roymercadian@gmail.com>
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

#ifndef EDITOR_H
#define EDITOR_H

#ifdef __GNUG__
  #pragma interface "editor.h"
#endif

#include "termdata.h"
#include "telnetcon.h"
#include "termview.h"
#include "telnetview.h"
#include "editorview.h"
#include "site.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
using namespace std;

class CEditorView;
class CEditor: public CTelnetCon
{
public:
    CEditor(CTelnetView* tView, CSite& SiteInfo);
    ~CEditor();

    void ApplyAnsiColor(int bright, int blink, int fg, int bg);
    int Send( void* buf, int len );

    CEditorView* GetEditorView() {    return (CEditorView*) m_pView;    }

    enum EditorAction
    {
        Init_Ansi_Editor,
        Move_Up,
        Move_Down,
        Move_Left,
        Move_Right,
        Move_To_Home,
        Move_To_End,
        Move_To_Prev_Page,
        Move_To_Next_Page,
        New_Line,
        Delete_Text,
        Backspace_Text,
        Set_Display_Frame_Plus,
        Set_Display_Frame_Minus,
        Set_Caret_Pos_X,
        Set_Caret_Pos_Y,
        Load_Editor_Text,
        Load_Ansi_File,
        Save_Ansi_File,
        Paste_To_Editor,
        Clear_Screen
    };

    void EditorActions(int action, string arg = "");
    //void UpdateEditor();

private:
    void DoInsertText(string &text, const string &newText, int insertCol);
    void DoDeleteText(string &line, int col);
    void DoBackspaceText();

    int GetTextCharCount(const string &str);

    void InitAnsiEditor();
    void SetDisplayFrame(int offset = 0);
    void SetCaretPosX();
    void SetCaretPosY();
    void PasteToEditor(const string &str);
    int ParseToRawPos(const string &text, int col, bool checkDBCS = true);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void MoveToHome();
    void MoveToEnd();
    void MoveToPrevPage();
    void MoveToNextPage();
    void NewLine();
    void DeleteText();
    void BackspaceText();
    void LoadEditorText();
    void LoadAnsiFile(string filename);
    void SaveAnsiFile(string filename);
    void ClearScreen();

    void SetAnsiColor(int bright, int blink, int fg, int bg);
    void SetColorToSelection(int bright, int blink, int fg, int bg);
    void SetSelection();
    void SetTextColor(int screenRow, int startCol, int endCol);
    void SetTextColor(int screenRow, int startCol, int endCol, int bright, int blink, int fg, int bg);

    string GetLineText(int row, bool trim);

    // Member Fields
    vector<string> m_EditorText;
    int m_DisplayStart;		// indicate which row of m_EditorText should be loaded into screen.
    int m_DisplayEnd;

    bool m_AnsiBright;
    bool m_AnsiBlink;
    int m_AnsiFg;
    int m_AnsiBg;

    int m_SelectStartRow;
    int m_SelectStartCol;
    int m_SelectEndRow;
    int m_SelectEndCol;
};


#endif // EDITOR_H
