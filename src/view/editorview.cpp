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

#ifdef __GNUG__
  #pragma implementation "editorview.h"
#endif

#include <gdk/gdkkeysyms.h>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include "editor.h"
#include "editorview.h"

#if !defined(MOZ_PLUGIN)
#include "mainframe.h"
#include "stringutil.h"
#include "appconfig.h"
#endif // !defined(MOZ_PLUGIN)

CEditorView::CEditorView()
	: CTelnetView()
{
	//do nothing.
}

CEditorView::~CEditorView()
{
	//do nothing.
}

#define	GDK_MODIFIER_DOWN(key, mod)	(key & (mod|(~GDK_SHIFT_MASK&~GDK_CONTROL_MASK&~GDK_MOD1_MASK)))

bool CEditorView::OnKeyDown(GdkEventKey* evt)
{

	switch(evt->keyval)
	{
		case GDK_Left:
		case GDK_KP_Left:
			GetEditor()->EditorActions(CEditor::Move_Left);
			break;
		case GDK_Right:
		case GDK_KP_Right:
			GetEditor()->EditorActions(CEditor::Move_Right);
			break;
		case GDK_Up:
		case GDK_KP_Up:
			GetEditor()->EditorActions(CEditor::Move_Up);
			break;
		case GDK_Down:
		case GDK_KP_Down:
			GetEditor()->EditorActions(CEditor::Move_Down);
			break;
		case GDK_BackSpace:
			GetEditor()->EditorActions(CEditor::Backspace_Text);
			break;
		case GDK_Return:
		case GDK_KP_Enter:
			GetEditor()->EditorActions(CEditor::New_Line);
			break;
		case GDK_Delete:
		case GDK_KP_Delete:
			GetEditor()->EditorActions(CEditor::Delete_Text);
			break;
		case GDK_Home:
		case GDK_KP_Home:
			GetEditor()->EditorActions(CEditor::Move_To_Home);
			break;
		case GDK_End:
		case GDK_KP_End:
			GetEditor()->EditorActions(CEditor::Move_To_End);
			break;
		case GDK_Page_Up:
		case GDK_KP_Page_Up:
			GetEditor()->EditorActions(CEditor::Move_To_Prev_Page);
			break;
		case GDK_Page_Down:
		case GDK_KP_Page_Down:
			GetEditor()->EditorActions(CEditor::Move_To_Next_Page);
			break;
		default:
			break;
	}

	return true;
}

#if defined(USE_MOUSE) && !defined(MOZ_PLUGIN)
void CEditorView::OnMouseScroll(GdkEventScroll* evt)
{
	GdkScrollDirection i = evt->direction;
	if ( i == GDK_SCROLL_UP )
	{
		GetEditor()->EditorActions(CEditor::Set_Display_Frame_Minus);
		UpdateEditor(); //it's necessary
		GetEditor()->EditorActions(CEditor::Set_Caret_Pos_X);
		UpdateEditor();
		return;
	}

	if ( i == GDK_SCROLL_DOWN )
	{
		GetEditor()->EditorActions(CEditor::Set_Display_Frame_Plus);
		UpdateEditor(); //it's necessary
		GetEditor()->EditorActions(CEditor::Set_Caret_Pos_X);
		UpdateEditor();
		return;
	}
}

void CEditorView::OnLButtonDown(GdkEventButton* evt)
{
	CTermView::OnLButtonDown(evt);

	int x = (int)evt->x;
	int y = (int)evt->y;
	PointToLineCol( &x, &y );

	if( !(evt->type == GDK_3BUTTON_PRESS) && !(evt->type == GDK_2BUTTON_PRESS) )
	{
		//set caret to current position
		m_pTermData->m_CaretPos.x = x;
		m_pTermData->m_CaretPos.y = y;
		GetEditor()->EditorActions(CEditor::Set_Caret_Pos_Y);
		GetEditor()->EditorActions(CEditor::Set_Caret_Pos_X);
		UpdateEditor();
	}
}

#endif  // defined(USE_MOUSE) && !defined(MOZ_PLUGIN)

void CEditorView::DoPasteFromClipboard(string text, bool contain_ansi_color)
{
	string locale_str;
	int lines_count = 0, last_line_count = 0;
	if( !ConvStr2SiteEncoding(text, contain_ansi_color, locale_str, lines_count, last_line_count) )
		return;

	int x = m_pTermData->m_CaretPos.x;
	int y = m_pTermData->m_CaretPos.y;

	GetEditor()->EditorActions(CEditor::Paste_To_Editor, locale_str);
	GetEditor()->EditorActions(CEditor::Load_Editor_Text);

	m_pTermData->m_CaretPos.x = lines_count==0 ? x+last_line_count : last_line_count;
	m_pTermData->m_CaretPos.y = y + lines_count;

	UpdateEditor();
}

void CEditorView::UpdateEditor()
{
	//save current caret position.
	int x = m_pTermData->m_CaretPos.x;
	int y = m_pTermData->m_CaretPos.y;

	m_pTermData->m_CaretPos.x = 0;
	m_pTermData->m_CaretPos.y = 0;

	//clear screen.
	m_pTermData->ClearScreen(2);
	GetEditor()->EditorActions(CEditor::Load_Editor_Text);

	//reset caret position
	m_pTermData->m_CaretPos.x = x;
	m_pTermData->m_CaretPos.y = y;
	UpdateCaretPos();
}

void CEditorView::UpdateEditorCaretPos()
{
	CTermView::UpdateCaretPos();
}
