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


#ifdef __GNUG__
  #pragma implementation "editor.h"
#endif

#include "mainframe.h"

#include "editor.h"
#include <algorithm>	// for std::replace
#include <fstream>

#define RECV_BUF_SIZE (4097)


typedef struct {
	CTermData* pTermData;
	string*    text;
	int        lines;
	const char*      eol;
} ReadStatus;

CEditor::CEditor(CTelnetView* tView, CSite& SiteInfo)
	: CTelnetCon(tView, SiteInfo)
{
	// do nothing.
}

CEditor::~CEditor()
{
	// do nothing.
}

void CEditor::InitAnsiEditor()
{
	m_EditorText.resize(1);

	m_AnsiBright = false;
	m_AnsiBlink = false;
	m_AnsiFg = 37;
	m_AnsiBg = 40;

	m_DisplayStart = m_DisplayEnd = 0;

	m_SelectStartRow = m_SelectStartCol = m_SelectEndRow = m_SelectEndCol = 0;

	string str = "\x1b[m";
	for(int r = 0; r < 24; ++r){
		m_EditorText.push_back(str);
	}

	SetDisplayFrame();
}

/**
 * @brief Deal with user input. Store the text in TermData.
 */
int CEditor::Send( void* buf, int length )
{
	// save text into data
	string text = (char*)buf;
	DoInsertText(m_EditorText[m_DisplayStart + m_CaretPos.y], text, m_CaretPos.x);

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();

	SetTextColor(m_CaretPos.y, m_CaretPos.x, m_CaretPos.x + length - 1);

	//save back to vector
	string newText = GetLineText(m_CaretPos.y, false);
	m_EditorText[m_DisplayStart + m_CaretPos.y] = newText;

	m_Sel->Unselect();

	// Move caret to next position
	m_CaretPos.x += length;
	if(m_CaretPos.x >= m_ColsPerPage - 1){
		m_CaretPos.x = m_ColsPerPage - 1;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();

	return 0;
}

void CEditor::DeleteText()
{

	DoDeleteText( m_EditorText[m_DisplayStart + m_CaretPos.y], m_CaretPos.x);

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::BackspaceText()
{
	DoBackspaceText();

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::DoBackspaceText()
{
	if(m_CaretPos.x < 0){
		return;
	}

	if(m_CaretPos.x == 0){
		if(m_DisplayStart + m_CaretPos.y > 0){
			MoveUp();
			MoveToEnd();
			//append current line text to end of previous line
			stringstream ss;
			ss << m_EditorText[m_DisplayStart + m_CaretPos.y];
			ss << m_EditorText[m_DisplayStart + m_CaretPos.y + 1];

			m_EditorText[m_DisplayStart + m_CaretPos.y] = ss.str();

			//remove (m_DisplayStart + m_CaretPos.y + 1)
			vector<string>::iterator itor = m_EditorText.begin();

			m_EditorText.erase(itor + m_DisplayStart + m_CaretPos.y + 1);

			SetDisplayFrame();
		}
		return;
	}

	//move caret to left.
	MoveLeft();

	DoDeleteText( m_EditorText[m_DisplayStart + m_CaretPos.y], m_CaretPos.x);
}

/**
 * @brief delete char from text of ansi editor.
 */
void CEditor::DoDeleteText(string &line, int col)
{
	int pos = ParseToRawPos(line, col);
	if(pos >= static_cast<int>(line.length()) || pos < 0){
		return;		//do nothing
	}

	stringstream ss;
	// Deal with DBCS
	if(line[pos] < 0){		//it's a DBCS
		ss << line.substr(0, pos);
		ss << line.substr(pos + 2, -1);
	}else{
		ss << line.substr(0, pos);
		ss << line.substr(pos + 1, -1);
	}
	line = ss.str();

	if(line.length() == 0)
		line = " ";
}

void CEditor::LoadEditorText()
{
	//load text block from data to screem
	stringstream ss;
	for(int r = m_DisplayStart; r <= m_DisplayEnd; ++r){
		ss.clear();
		ss << m_EditorText[r];
		if(r < m_DisplayEnd)		// Don't add \r\n at last row
			ss << "\r\n";
	}

	static unsigned char recv_buf[RECV_BUF_SIZE];
	strcpy( (char *)recv_buf, ss.str().c_str());
	m_pRecvBuf = recv_buf;
	gsize rlen = ss.str().size();

	m_pRecvBuf[rlen] = '\0';
	m_pBuf = m_pRecvBuf;
	m_pLastByte = m_pRecvBuf + rlen;

	ParseReceivedData();

	UpdateDisplay();
	//GetView()->UpdateEditor();
}


void CEditor::EditorActions(int action, string arg)
{
	switch (action)
	{
	case Init_Ansi_Editor:
		InitAnsiEditor();
		break;
	case Move_Up:
		MoveUp();
		break;
	case Move_Down:
		MoveDown();
		break;
	case Move_Left:
		MoveLeft();
		break;
	case Move_Right:
		MoveRight();
		break;
	case Move_To_Home:
		MoveToHome();
		break;
	case Move_To_End:
		MoveToEnd();
		break;
	case Move_To_Prev_Page:
		MoveToPrevPage();
		break;
	case Move_To_Next_Page:
		MoveToNextPage();
		break;
	case New_Line:
		NewLine();
		break;
	case Delete_Text:
		DeleteText();
		break;
	case Backspace_Text:
		BackspaceText();
		break;
	case Set_Display_Frame_Plus:
		SetDisplayFrame(1);
		break;
	case Set_Display_Frame_Minus:
		SetDisplayFrame(-1);
		break;
	case Set_Caret_Pos_X:
		SetCaretPosX();
		break;
	case Set_Caret_Pos_Y:
		SetCaretPosY();
		break;
	case Load_Editor_Text:
		LoadEditorText();
		break;
	case Load_Ansi_File:
		LoadAnsiFile(arg);
		break;
	case Save_Ansi_File:
		SaveAnsiFile(arg);
		break;
	case Paste_To_Editor:
		PasteToEditor(arg);
		break;
	case Clear_Screen:
		ClearScreen();
		break;
	default:
		cout << "error: EditorActions" << endl;
		break;
	}
}

void CEditor::MoveUp()
{
	if(m_DisplayStart + m_CaretPos.y <= 0){
		return;
	}

	m_CaretPos.y--;

	// Move display frame up if m_CaretPos.y < 0
	if(m_CaretPos.y < 0){
		m_CaretPos.y = 0;
		SetDisplayFrame(-1);

	}

	// Detect dbcs and set caret position X
	SetCaretPosX();

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::MoveDown()
{
	if( (m_DisplayStart + m_CaretPos.y) >= static_cast<int>(m_EditorText.size()) - 1){
		//already in the last line, do nothing
		return;
	}

	m_CaretPos.y++;

	// Move screen pointers down if m_CaretPos.y > (m_RowsPerPage - 1)
	if( m_CaretPos.y > (m_RowsPerPage - 1) ){
		m_CaretPos.y = m_RowsPerPage - 1;
		SetDisplayFrame(1);
	}

	//detect dbcs and set caret position X
	SetCaretPosX();

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::MoveLeft()
{
	CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );
	int x = m_CaretPos.x;

	if(DetectDBChar() && x > 0 && pAttr[x - 1].GetCharSet() == CTermCharAttr::CS_MBCS2){
		m_CaretPos.x -= 2;	//move 2 chars
	}else{
		m_CaretPos.x--;		//move 1 char
	}

	if(m_CaretPos.x < 0)
		m_CaretPos.x = 0;

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditorCaretPos();
}

void CEditor::MoveRight()
{
	CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );
	int x = m_CaretPos.x;


	if(DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS1){
		m_CaretPos.x += 2;	//move 2 chars
	}else{
		m_CaretPos.x++;		//move 1 char
	}

	if(m_CaretPos.x > m_ColsPerPage - 1){
		m_CaretPos.x = m_ColsPerPage - 1;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditorCaretPos();
}

void CEditor::MoveToHome()
{
	m_CaretPos.x = 0;

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::MoveToEnd()
{
	m_CaretPos.x = GetTextCharCount( m_EditorText[m_DisplayStart + m_CaretPos.y]);

	if(m_CaretPos.x > m_ColsPerPage - 1)
		m_CaretPos.x = m_ColsPerPage -1;

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::MoveToPrevPage()
{
	SetDisplayFrame(-m_RowsPerPage);

	//bound check
	if(m_DisplayStart + m_CaretPos.y < 0){
		m_CaretPos.y = 0;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();

	//detect dbcs and set caret position x
	CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );
	int x = m_CaretPos.x;

	if(DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS2){
		m_CaretPos.x--;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::MoveToNextPage()
{
	SetDisplayFrame(m_RowsPerPage);

	//bound check
	if(m_DisplayStart + m_CaretPos.y >= static_cast<int>(m_EditorText.size())){
		m_CaretPos.y = m_EditorText.size() - 1;
		if(m_CaretPos.y > m_RowsPerPage - 1)
			m_CaretPos.y = m_RowsPerPage - 1;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();		//update screen first time

	//detect dbcs
	CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );
	int x = m_CaretPos.x;

	if(DetectDBChar() && pAttr[x].GetCharSet() == CTermCharAttr::CS_MBCS2){
		m_CaretPos.x--;
	}

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();		//update screen second time
}

/**
 * @brief every time when user press enter-key, the m_EditorText add a new string data.
 */
void CEditor::NewLine()
{
	//partition the string behind cursor to the next line
	string str0, str1;
	int pos = ParseToRawPos(m_EditorText[m_DisplayStart + m_CaretPos.y], m_CaretPos.x);
	if(pos < static_cast<int>(m_EditorText[m_DisplayStart + m_CaretPos.y].size())){
		str0 = m_EditorText[m_DisplayStart + m_CaretPos.y].substr(0, pos);
		str1 = m_EditorText[m_DisplayStart + m_CaretPos.y].substr(pos, -1);
		if(str0.size() == 0)
			str0 = "\0";
		if(str1.size() == 0)
			str1 = "\0";
	}else{
		str0 = m_EditorText[m_DisplayStart + m_CaretPos.y];
		str1 = "\0";
	}

	m_EditorText[m_DisplayStart + m_CaretPos.y] = str0;

	if(m_DisplayStart + m_CaretPos.y < static_cast<int>(m_EditorText.size()) - 1){
		vector<string>::iterator itor = m_EditorText.begin();
		m_EditorText.insert(itor + m_DisplayStart + m_CaretPos.y + 1, str1);
	}else{
		m_EditorText.push_back(str1);
	}

	//set new caret position
	m_CaretPos.x = 0;

	if(m_CaretPos.y == m_RowsPerPage - 1){
		SetDisplayFrame(1);
	}else{
		m_CaretPos.y++;
		SetDisplayFrame();
	}
	SetCaretPosX();

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

int CEditor::GetTextCharCount(const string &str)
{
	int count = 0;
	int i = 0;
	while(i < static_cast<int>(str.length())){
		if(str[i] == '\x1b'){
			while(str[i] != 'm')	++i;
			++i;
			continue;
		}
		if(str[i] < 0){	//it's a DBCS
			i += 2;
			count += 2;
		}else{			//SBCS
			i++;
			count++;
		}
	}
	return count;
}

void CEditor::ApplyAnsiColor(int bright, int blink, int fg, int bg)
{
	SetAnsiColor(bright, blink, fg, bg);

	SetSelection();

	if(m_SelectStartRow < 0 || m_SelectEndRow < 0){
		return;
	}

	SetColorToSelection(bright, blink, fg, bg);	// method of parent class CTermData

	GetEditorView()->GetParentFrame()->GetCurEditorView()->UpdateEditor();
}

void CEditor::LoadAnsiFile(string filename)
{
	m_EditorText.clear();
	fstream fs;
	fs.open(filename.c_str());
	if(fs.is_open()){
		string line;
		while(std::getline(fs, line)){
			// Remove carriage return
			std::replace(line.begin(), line.end(), '\r', ' ');
			m_EditorText.push_back(line);
		}

		SetDisplayFrame();

		fs.close();
	}else{
		printf("fail to open file.\n");
	}
}

void CEditor::SaveAnsiFile(string filename)
{
	stringstream ss;
	ss << filename << ".ans";
	filename = ss.str();

	ofstream fs(filename.c_str());
	if(fs.is_open()){

		for(unsigned int i = 0; i < m_EditorText.size(); ++i){
			fs << m_EditorText[i] << "\r\n";
		}

		fs.close();
	}else{
		printf("fail to save file.\n");
	}
}

void CEditor::SetSelection()
{
	m_Sel->GetCanonicalMarks(m_SelectStartRow, m_SelectStartCol, m_SelectEndRow, m_SelectEndCol);

	//check range
	if(m_EditorText.size() > m_RowsPerPage){
		//return;
	}else{
		if(m_SelectEndRow > static_cast<int>(m_EditorText.size()) - 1){
			m_SelectEndRow = m_EditorText.size() - 1;
			m_SelectEndCol = m_ColsPerPage - 1;
		}
	}
}

void CEditor::SetDisplayFrame(int offset)
{
	if(m_EditorText.size() <= m_RowsPerPage){
		m_DisplayStart = 0;
		m_DisplayEnd = m_EditorText.size() - 1;
	}else{
		m_DisplayStart += offset;

		if(m_DisplayStart < 0){
			m_DisplayStart = 0;
			m_DisplayEnd = m_RowsPerPage - 1;
		}else{
			m_DisplayEnd = m_DisplayStart + m_RowsPerPage - 1;

			if(m_DisplayEnd > static_cast<int>(m_EditorText.size()) - 1){
				m_DisplayEnd = m_EditorText.size() - 1;
				m_DisplayStart = m_DisplayEnd - m_RowsPerPage + 1;
			}
		}
	}
}

void CEditor::SetCaretPosX()
{
	CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );

	//DetectDBChar() should be added here.
	if( DetectDBChar() && pAttr[m_CaretPos.x].GetCharSet() == CTermCharAttr::CS_MBCS2){
		m_CaretPos.x--;
	}
}

void CEditor::SetCaretPosY()
{
	int lineCount = m_DisplayEnd - m_DisplayStart + 1;

	if(m_CaretPos.y >= lineCount){
		m_CaretPos.y = lineCount - 1;
	}
}

/**
 * Paste text to AnsiEditor.
 */
void CEditor::PasteToEditor(const string &text)
{
	string temp = text;
	int lineCount = 0;
	int found = -1;
	do{
		lineCount++;
		found = temp.find("\n");

		string line = temp.substr(0, found);

		if(lineCount == 1){
			int insertPos = ParseToRawPos(m_EditorText[m_DisplayStart + m_CaretPos.y], m_CaretPos.x);
			int length = m_EditorText[m_DisplayStart + m_CaretPos.y].length();
			if( insertPos > length){
				int spaceCount = insertPos - length;
				for(int i = 0; i < spaceCount; ++i){
					m_EditorText[m_DisplayStart + m_CaretPos.y] += " ";
				}
				m_EditorText[m_DisplayStart + m_CaretPos.y] += line;
			}else{
				string result = m_EditorText[m_DisplayStart + m_CaretPos.y].substr(0, insertPos);
				result += line;
				result += m_EditorText[m_DisplayStart + m_CaretPos.y].substr(insertPos, -1);
				m_EditorText[m_DisplayStart + m_CaretPos.y] = result;
			}
		}else{
			vector<string>::iterator itor;
			itor = m_EditorText.begin();
			m_EditorText.insert(itor + m_DisplayStart + m_CaretPos.y + lineCount -1, line);
		}

		if(found != static_cast<int>(string::npos))
			temp = temp.substr(found + 1, -1);
	}while(found != static_cast<int>(string::npos));
}

int CEditor::ParseToRawPos(const string &text, int col, bool checkDBCS)
{
	if(col == -1)	return -1;

	int t = 0;	//index
	int i = 0;
	while(i < static_cast<int>(text.length())){
		if(text[i] == '\x1b'){
			while(text[i] != 'm')	++i;
			++i;
			continue;
		}
		if(t == col){
			return i;
		}

		if(checkDBCS){
			if(text[i] < 0){	//it's a DBCS
				i += 2;
				t += 2;
			}else{			//it's a SBCS
				i++;
				t++;
			}
		}else{
			i++;
			t++;
		}
	}

	while(t != col){
		++i;
		++t;
	}
	return i;
}

/**
 * @brief Insert string into text at insertCol position.
 * @param insertPos -1: insert to the end of text.
 */
void CEditor::DoInsertText(string &text, const string &newText, int insertCol)
{
	int insertPos = ParseToRawPos(text, insertCol);

	stringstream ss("");
	if(insertPos < static_cast<int>(text.length())){
		if(insertPos == -1){		//insert newText to the end of text
			ss << text << newText;
		}else{
			ss << text.substr(0, insertPos);
			ss << newText;
			ss << text.substr(insertPos, -1);
		}
	}else{
		//need to add spaces
		ss << text;
		int spaceCount = insertPos - text.length();
		for(int i = 0; i < spaceCount; ++i){
			ss << " ";
		}
		ss << newText;
	}

	text = ss.str();
}

void CEditor::SetAnsiColor(int bright, int blink, int fg, int bg)
{
	if(bright == 0 || bright == 1)
		m_AnsiBright = bright;

	if(blink == 0 || blink == 1)
		m_AnsiBlink = blink;

	if (fg >= 0 && fg <= 7)
		m_AnsiFg = fg;

	if (bg >= 0 && bg <= 7)
		m_AnsiBg = bg;
}

void CEditor::SetTextColor(int row, int startCol, int endCol)
{
	SetTextColor(row, startCol, endCol, m_AnsiBright, m_AnsiBlink, m_AnsiFg, m_AnsiBg);
}

/**
 * Set color attributes for each char between [row, startCol] and [row, endCol]
 */
void CEditor::SetTextColor(int row, int startCol, int endCol, int bright, int blink, int fg, int bg)
{
	CTermCharAttr *pAttr = GetLineAttr( m_Screen[row] );
	for(int c = startCol; c <= endCol; ++c){
		if(bright != -1)
			pAttr[c].SetBright(bright);

		if(blink != -1)
			pAttr[c].SetBlink(blink);

		if(fg != -1)
			pAttr[c].SetForeground(fg);

		if(bg != -1)
			pAttr[c].SetBackground(bg);

		pAttr[c].SetNeedUpdate(true);
	}
}

/**
 * Set ANSI Color to current selection.
 */
void CEditor::SetColorToSelection(int bright, int blink, int fg, int bg)
{
	if(m_SelectStartRow == m_SelectEndRow && m_SelectStartCol > m_SelectEndCol){
		return;
	}

	for(int row = m_SelectStartRow; row <= m_SelectEndRow; ++row){
		int textStart = (row == m_SelectStartRow)? m_SelectStartCol: 0;
		int textEnd = (row == m_SelectEndRow)? m_SelectEndCol: m_ColsPerPage - 1;

		int rawEnd = ParseToRawPos(m_EditorText[m_DisplayStart + row], textEnd, false);

		// add space if rawEnd >= m_EditorText[m_DisplayStart + row].length()
		if(rawEnd >= static_cast<int>(m_EditorText[m_DisplayStart + row].length())){
			int spaceCount = rawEnd - m_EditorText[m_DisplayStart + row].length() + 1;
			for(int i = 0; i < spaceCount; ++i){
				m_EditorText[m_DisplayStart + row] += " ";
			}
		}
		SetTextColor(row, textStart, textEnd, bright, blink, fg, bg);

		//save back to vector
		m_Sel->m_Start.row = row;
		m_Sel->m_Start.col = 0;
		m_Sel->m_End.row = row;
		m_Sel->m_End.col = m_ColsPerPage - 1;

		m_EditorText[m_DisplayStart + row] = GetLineText(row, false);

		m_Sel->Unselect();
	}
}


/**
 * Get text with Ansi color codes.
 */
string CEditor::GetLineText(int row, bool trim)
{
	m_Sel->m_Start.row = row;
	m_Sel->m_Start.col = 0;
	m_Sel->m_Start.left = true;
	m_Sel->m_End.row = row;
	m_Sel->m_End.col = m_ColsPerPage;
	m_Sel->m_End.left = true;
	string text = GetSelectedTextWithColor(trim);
	return text;
}

void CEditor::ClearScreen()
{
	m_EditorText.clear();
	m_EditorText.resize(1);
	m_DisplayStart = m_DisplayEnd = 0;
	m_SelectStartRow = m_SelectStartCol = m_SelectEndRow = m_SelectEndCol = 0;

	string str = "\x1b[m";
	for(int r = 0; r < 24; ++r){
		m_EditorText.push_back(str);
	}

	SetDisplayFrame();
}
