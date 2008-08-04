/**                                                                             
 * termdata.cpp - Store terminal screen data and parse
 *                ANSI escape sequence.
 *
 * Copyright (c) 2004-2005 PCMan <pcman.tw@gmail.com>
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
  #pragma implementation "termdata.h"
#endif


#include "termdata.h" // class's header file
#include "termview.h" // class's header file
#include "termsel.h" // class's header file
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
//The functions section of CTermAttr class.
//
/////////////////////////////////////////////////////////////////////////////

//GdkColor = (red, green, blue)

GdkColor 
CTermCharAttr::m_DefaultColorTable[SIZE_OF_COLOR_TABLE] = {
	//Darker color
	{0,0,0,0}, 		//0;30m		Black
	{0,65536/2,0,0},		//0;31m		Dark red
	{0,0,65536/2,0},		//0;32m		Dark green
	{0,65536/2,65536/2,0},	//0;33m		Brown
	{0,0,0,65536/2},		//0;34m		Dark blue
	{0,65536/2,0,65536/2},	//0;35m		Dark magenta
	{0,0,65536/2,65536/2},	//0;36m		Dark cyan
	{0,65536*192/256,65536*192/256,65536*192/256},	//0;37m		Light gray
	//Bright color
	{0,65536/2,65536/2,65536/2},	//1;30m		Gray
	{0,65535,0,0},		//1;31m		Red
	{0,0,65535,0},		//1;32m		Green
	{0,65535,65535,0},	//1;33m		Yellow
	{0,0,0,65535},		//1;34m		Blue
	{0,65535,0,65535},	//1;35m		Magenta
	{0,0,65535,65535},	//1;36m		Cyan
	{0,65535,65535,65535}	//1;37m		White
};

//Update this old attribute of character with new one which filter by [flags].
//e.g., when [flags] = STA_FG | STA_BRIGHT, the old attribute updates
//attributes Foreground and Bright only.
//After updating, set Need Update flag as true.
void 
CTermCharAttr::SetTextAttr( CTermCharAttr attr, int flags )
{
	if( flags & STA_FG )
		m_Fg = attr.m_Fg;
	if( flags & STA_BG )
		m_Bg = attr.m_Bg;
	if( flags & STA_BRIGHT )
		m_Bright = attr.m_Bright;
	if( flags & STA_BLINK )
		m_Blink = attr.m_Blink;
	if( flags & STA_UNDERLINE )
		m_UnderLine = attr.m_UnderLine;
	if( flags & STA_INVERSE )
		m_Inverse = attr.m_Inverse;
	if( flags & STA_INVISIBLE )
		m_Invisible = attr.m_Invisible;
	m_NeedUpdate = 1;
}

// I don't know whether assign an 'short' to this 'object' directly will cause 
// problems or not hence preventing using it.  Otherwise I can return 7 directly;
short 
CTermCharAttr::GetDefVal(){
	CTermCharAttr attr;
	*(short*)&attr=0;
	attr.m_Fg = 7;
	return *(short*)&attr;
}


void 
CTermCharAttr::SetToDefault(){ *(short*)this = 0;	m_Fg=7;}
// We cannot use == to compare two CTermCharAttr directly because of some special flags,
//so us use this function to compare.
bool 
CTermCharAttr::IsSameAttr(short val2)
{
		CTermCharAttr* pAttr = (CTermCharAttr*)&val2;
		pAttr->m_CharSet = m_CharSet;
		pAttr->m_NeedUpdate = m_NeedUpdate;
		return val2 == this->AsShort();
}
bool
CTermCharAttr::operator==(CTermCharAttr& attr){
	if(IsSameAttr(attr.AsShort()))
		return true;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
//The functions section of CTermData class.
//
/////////////////////////////////////////////////////////////////////////////

// class constructor
CTermData::CTermData(CTermView* pView) : m_pView(pView), m_Screen(NULL)
{
	m_CaretPos.x = m_CaretPos.y = 0;
	m_OldCaretPos = m_CaretPos;
	m_FirstLine = 0;
	m_RowCount = 0;
	m_RowsPerPage = m_ColsPerPage = 0;
	m_ScrollRegionBottom = m_ScrollRegionTop = 0;
	m_CurAttr.SetToDefault();
	m_SavedAttr.SetToDefault();
	m_CmdLine[0] = '\0';
	m_WaitUpdateDisplay = false;
	m_NeedDelayedUpdate = false;
	m_DelayedUpdateTimeout = 0;
	m_Sel = new CTermSelection(this);
#ifdef USE_IPLOOKUP
	regcomp( &m_RegIp, "([0-9]{1,3}\\.){3}([0-9]{1,3}|\\*)", REG_EXTENDED );
#endif
	m_LineCounter = 0;
}

// class destructor
CTermData::~CTermData()
{
	delete m_Sel;

	if( m_DelayedUpdateTimeout )
		g_source_remove(m_DelayedUpdateTimeout);

	if( m_Screen )
	{
		for(int i=0; i<m_RowCount; i++)
			delete []m_Screen[i];
		delete []m_Screen;
	}

#ifdef USE_IPLOOKUP
	regfree(&m_RegIp);
#endif
}


// Set sizes of screen buffer, reallocate buffer automatically when needed.
void CTermData::SetScreenSize( int RowCount, unsigned short RowsPerPage,
     unsigned short ColsPerPage)
{
    m_RowsPerPage = RowsPerPage;
    // if cols per page change, reallocate all existing rows.
    if( m_ColsPerPage != ColsPerPage )
    {
        for(int i=0; i < m_RowCount; i++)
        {
            char* NewLine = AllocNewLine(ColsPerPage);
            unsigned short Cols = (ColsPerPage < m_ColsPerPage)?ColsPerPage:m_ColsPerPage;
            //Copy context of old into new one.
            memcpy(NewLine, m_Screen[i], Cols);
            memcpy(GetLineAttr(NewLine, ColsPerPage), GetLineAttr(m_Screen[i]), sizeof(short)*Cols);
            delete []m_Screen[i];
            m_Screen[i] = NewLine;
        }
        m_ColsPerPage = ColsPerPage;
    }
    SetRowCount(RowCount);
}

// Change row count of screen buffer
void CTermData::SetRowCount(int RowCount)
{
    if( RowCount == m_RowCount )
        return;

    char** NewScreen = new char* [RowCount];
    if( RowCount > m_RowCount )    // increase row count
    {
        memcpy(NewScreen, m_Screen, sizeof(char**)*m_RowCount);
        for( int i = m_RowCount; i < RowCount; i++ )
                NewScreen[i] = AllocNewLine(m_ColsPerPage);
    }
    else        // decrease row count
    {
        memcpy(NewScreen, m_Screen, sizeof(char**)*RowCount);
        for( int i = RowCount; i < m_RowCount; i++ )
                delete []m_Screen[i];
    }
    delete []m_Screen;
    m_Screen = NewScreen;
    m_RowCount = RowCount;       
}

// Allocate screen buffer.
void CTermData::AllocScreenBuf(int RowCount, unsigned short RowsPerPage,unsigned short ColsPerPage){
    m_RowCount = RowCount;
    m_RowsPerPage = RowsPerPage;
    m_ColsPerPage = ColsPerPage;

    m_Screen = new char* [m_RowCount];
    for(int i=0; i < m_RowCount; i++)
        m_Screen[i] = AllocNewLine(m_ColsPerPage);

    m_FirstLine = m_RowCount - m_RowsPerPage;
    m_ScrollRegionTop = 0;
    m_ScrollRegionBottom = m_RowsPerPage-1;
}

// Initialize new lines
void CTermData::InitNewLine(char* NewLine, const int ColsPerPage){
		memset( NewLine, ' ', ColsPerPage);
		NewLine[ColsPerPage] = '\0';
		CTermCharAttr DefAttr;	DefAttr.SetToDefault();	DefAttr.SetNeedUpdate(true);
		memset16( GetLineAttr(NewLine, ColsPerPage), DefAttr.AsShort(), ColsPerPage);
}

// LF handler
void CTermData::LineFeed()
{
	int top;
	int bottom = m_FirstLine + m_ScrollRegionBottom;

	m_LineCounter++;

	if(m_CaretPos.y < bottom)
	{
		m_CaretPos.y++;
		return;
	}
	else if( m_ScrollRegionBottom != (m_RowsPerPage-1) || m_ScrollRegionTop != 0 )
	{
		top = m_FirstLine + m_ScrollRegionTop;
	//	bottom = m_FirsLine + m_ScrollRegionBottom-1;
	}
	else
	{
		top = 0;
		bottom = m_RowCount-1;
	}

	char* tmp = m_Screen[top];
	InitNewLine(tmp, m_ColsPerPage);
	for( int i = top; i < bottom; i++ )
	{
		m_Screen[i] = m_Screen[i+1];
		SetWholeLineUpdate(m_Screen[i]);
	}
	m_Screen[bottom] = tmp;

	m_NeedDelayedUpdate = true;
}

// BS handler
void CTermData::Back()
{
	if(m_CaretPos.x >0 )
		m_CaretPos.x-- ;
}

// BEL handler
void CTermData::Bell()	//	virtual
{
	//	Call virtual function in dirived class to determine
	//	whether to beep or show a visual indication instead.
}

// CR handler
void CTermData::CarriageReturn()
{
	m_CaretPos.x = 0;
}

// TAB handler
void CTermData::Tab()
{
//	m_CaretPos.x += ((m_CaretPos.x/8)*8)+8;
	m_CaretPos.x += ((m_CaretPos.x/4)*4)+4;
}

void CTermData::PutChar(unsigned char ch)
{
	// C0 control charcters have higher precedence than ANSI escape sequences.
	// interpret them first whether in ANSI escape sequence or not
	if( ch < ' ' )    // if this is a control character
	{
		switch( ch )
		{
		case '\x1b':      // ESC
			m_CmdLine[0] = '\x1b';
			m_pCmdLine = &m_CmdLine[1];
			break;
		case '\n':        // LF, line feed
			LineFeed();
			break;
		case '\r':        // CR, carriage return
			CarriageReturn();
			break;
		case '\b':        // BS, backspace
			Back();
			break;
		case '\a':        // BEL, bell
			Bell();
			break;
		case '\t':        // HT, horizontal tab
			Tab();
			break;
		}
    }
	else	// not C0 control characters, check if we're in control sequence.
	{
		switch( m_CmdLine[0] )
		{
		// m_CmdLine[0] == '\0' means "not in control sequence," and *m_pBuf 
		// is normal text, write to screen buffer directly.
		case '\0':
			{
				if( m_CaretPos.x >= m_ColsPerPage )	// if we are at the bottom of screen
				{
					//LineFeed();
					//CarriageReturn();	//	scroll up and move to a new line
					break;
				}

				m_Screen[m_CaretPos.y][m_CaretPos.x] = ch;

				CTermCharAttr* pAttr = GetLineAttr( m_Screen[m_CaretPos.y] );

				// Check if we've changed a character which is part of a URL.
				bool bHyperLink = pAttr[m_CaretPos.x].IsHyperLink();
				pAttr[m_CaretPos.x] = m_CurAttr;
				// Set char attributes of current character out put to screen

				// Important:
				// Set the update flag to indicate "redrawing needed."
				pAttr[m_CaretPos.x].SetNeedUpdate(true);
				// 2004.08.07 Added by PCMan:
				// If we've changed a character which is originally part of a URL,
				// whole URL must be redrawn or underline won't be updated correctly.
				if( bHyperLink )
				{
					int col;
					for( col = m_CaretPos.x-1; col > 0 && pAttr[col].IsHyperLink(); col-- )
						pAttr[col].SetNeedUpdate(true);
					for( col = m_CaretPos.x+1; col < m_ColsPerPage && pAttr[col].IsHyperLink(); col++ )
						pAttr[col].SetNeedUpdate(true);
				}
				//	Advance the caret after character input.
				m_CaretPos.x++;
			break;
			}
		// m_CmdLine[0] == '\0' means we're currently in ANSI control sequence.
		// Store ch to CmdLine, and parse ANSI escape sequence when ready.
		case '\x1b':        // ESC, in ANSI escape mode
			if( m_pCmdLine < (m_CmdLine +sizeof(m_CmdLine)) )
			{
				*m_pCmdLine = ch;
				m_pCmdLine++;
			}

			if( m_CmdLine[1] == '[' )
			{
				if( ch < '@' || ch == '[' || ch > '~' )
					break;
			}
			else
			{
				if( ch < '0' || ch > '_' )
					break;
			}

			if( m_pCmdLine < (m_CmdLine +sizeof(m_CmdLine)) )
				*m_pCmdLine = '\0';
			// Current ANSI escape type is stored in *m_pBuf.
			ParseAnsiEscapeSequence( (const char*)m_CmdLine, ch);
			m_CmdLine[0] = '\0';
			m_pCmdLine = m_CmdLine;
		} // end switch( m_CmdLine[0] )
	}
}

void CTermData::InsertNewLine(int y, int count)
{
	short tmp = m_ScrollRegionTop;
	m_ScrollRegionTop = y;
	ScrollDown( count );
	m_ScrollRegionTop = tmp;
}

/*
inline void swap(char*& x, char*& y)
{	char* t=x;	x=y;	y=t;	}
*/

void CTermData::ScrollUp(int n /*=1*/)
{
	int maxn = m_ScrollRegionBottom - m_ScrollRegionTop +1;
	if( n > maxn )
		n = maxn;

	int start = m_FirstLine + m_ScrollRegionTop;
	int end = m_FirstLine + m_ScrollRegionBottom - n;
	int i;
	for( i = start; i <= end; i++ )
	{
		// Swap two lines to prevent memmory reallocation.
		char* tmp = m_Screen[i];
		m_Screen[i] = m_Screen[i+n];
		m_Screen[i+n] = tmp;
		SetWholeLineUpdate(m_Screen[i]);
	}
	for( i = 1; i <= n; i++ )
	{
		memset( m_Screen[end+i], ' ', m_ColsPerPage-1 );
		memset16( GetLineAttr(m_Screen[end+i]), m_CurAttr.AsShort(), m_ColsPerPage-1 );	
		SetWholeLineUpdate(m_Screen[end+i]);
	}
}

void CTermData::ScrollDown(int n /*=1*/)
{
	int maxn = m_ScrollRegionBottom - m_ScrollRegionTop +1;
	if( n > maxn )
		n = maxn;

	int start = m_FirstLine + m_ScrollRegionBottom;
	int end = m_FirstLine + m_ScrollRegionTop + n;
	int i;
	for( i = start; i >= end; i-- )
	{
		// Swap two lines to prevent memmory reallocation.
		char* tmp = m_Screen[i];
		m_Screen[i] = m_Screen[i-n];
		m_Screen[i-n] = tmp;
		SetWholeLineUpdate(m_Screen[i]);
	}
	for( i = 1; i <= n; i++ )
	{
		memset( m_Screen[end-i], ' ', m_ColsPerPage-1 );
		memset16( GetLineAttr(m_Screen[end-i]), m_CurAttr.AsShort(), m_ColsPerPage-1 );	
		SetWholeLineUpdate(m_Screen[end-i]);
	}
}

// Parse ANSI escape sequence
void CTermData::ParseAnsiEscapeSequence(const char* CmdLine, char type)
{
	// Current ANSI escape type is stored in *m_pBuf.
	if( m_CmdLine[1] == '[' )	// ESC[, CSI: control sequence introducer
	{
		//	CmdLine[] = {'\x1b', '[', ...'\0'};
		const char* pParam = &CmdLine[2];
		if(type == 'm' )	// multiple parameters, view as a special case.
			ParseAnsiColor(pParam);
		else
		{
			int p1=0, p2=0;
			int n = sscanf(pParam, "%d;%d",&p1,&p2);
			if( p1 < 0 )	p1 = 0;
			if( p2 < 0 )	p2 = 0;
			switch(type)
			{
			case 'K':	//	Clear Line
				EraseLine(p1);
				break;
			case 'H':	// Set Caret Pos
			case 'f':
				GoToXY(p2-1, p1-1);
				break;
			case 'J':	// Clear Screen
				ClearScreen(p1);
				break;
			case 'E':
				break;
			case 'L':
				InsertNewLine(m_CaretPos.y, p1);
				break;
			case 'A':
				if(p1 <= 0)
					p1=1;
				GoToXY( m_CaretPos.x, m_CaretPos.y - p1 );
				break;
			case 'B':
				if( p1<=0 )
					p1=1;
				GoToXY( m_CaretPos.x, m_CaretPos.y + p1 );
				break;
			case 'C':
				if( p1<=0 )
					p1=1;
				GoToXY( m_CaretPos.x + p1, m_CaretPos.y );
				break;
			case 'D':
				if( p1<=0 )
					p1=1;
				GoToXY( m_CaretPos.x - p1, m_CaretPos.y );
				break;
			case 'r':	// Set Scroll Region
				switch(n)
				{
				case 0:	// Turn off scroll region
					m_ScrollRegionTop = 0; m_ScrollRegionBottom = m_RowsPerPage-1;
					break;
				case 2:
					p2--;
					if( p2 > 0 && p2 < m_RowsPerPage && p2 >= m_ScrollRegionTop )
						m_ScrollRegionBottom = p2;
				case 1:
					p1--;
					if( p1 <= m_ScrollRegionBottom )
						m_ScrollRegionTop = p1;
					break;
				}
//				printf("scroll region: %d, %d\n", m_ScrollRegionTop, m_ScrollRegionBottom );
				break;
			case 's':	//save cursor pos
				break;
			case 'u':	//restore cursor pos
				break;
			case '@':	//insert char
				break;
			case 'M':	//delete n line
				break;
			case 'P':	//delete char
				break;
/*
			case 'U':	//next n page
				break;
			case 'V':	//previous n page
				break;
*/
			case 'Z':	//cursor back tab
				break;
			case 'h':	//set mode
				break;
			case 'l':	//reset mode
				break;
			case 'n':	//Device Status Report
				break;
			}
		}
	}
	else
	{
		switch(type)
		{
 		case 'D':	//	scroll up
			ScrollUp();
 			break;
 		case 'M':	//	scroll down
			ScrollDown();
 			break;
 		case 'E':
 			break;
		case '7':
			m_OldCaretPos = m_CaretPos;
//			printf("save cursor: %d, %d\n", (int)m_CaretPos.x, (int)m_CaretPos.y );
			m_SavedAttr = m_CurAttr;
			break;
		case '8':
			m_CaretPos = m_OldCaretPos;
//			printf("restored cursor: %d, %d\n", m_CaretPos.x, m_CaretPos.y );
			m_CurAttr = m_SavedAttr;
			m_pView->UpdateCaretPos();
			break;
 		}	//end switch
	}
}


void CTermData::GoToXY(int x, int y)
{
	if( x < 0)
		x = 0;
	else if( x >= m_ColsPerPage )
		x= m_ColsPerPage-1;

	if( y < 0 )
		y = 0;
	else if( y >= m_RowsPerPage )
		y= m_RowsPerPage-1;

	m_CaretPos.x = x;
	m_CaretPos.y = m_FirstLine + y;
}

void CTermData::ClearScreen(int p)
{
	m_NeedDelayedUpdate = true;

	// Scroll down a page
	int bottom = m_RowCount-m_RowsPerPage;
	int i;
	char* tmp;
	for( i = 0; i < bottom; i++ )
	{
		tmp = m_Screen[i];
		int src = i+m_RowsPerPage;
		m_Screen[i] = m_Screen[src];
		m_Screen[src] = tmp;
	}
	for( i = bottom; i< m_RowCount; i++ )
		InitNewLine( m_Screen[i], m_ColsPerPage);

	switch(p)
	{
//	case 2:	// Erase entire display
//		break;
	case 1:	// Erase from beginning to current position (inclusive)
		tmp = m_Screen[m_CaretPos.y];
		if( m_CaretPos.x < m_ColsPerPage && m_CaretPos.y > m_RowsPerPage )
		{
			memcpy( &tmp[m_CaretPos.x], 
				&m_Screen[m_CaretPos.y-m_RowsPerPage][m_CaretPos.x],
				m_ColsPerPage-m_CaretPos.x);
			memcpy( &GetLineAttr(tmp)[m_CaretPos.x],
				&GetLineAttr(m_Screen[m_CaretPos.y-m_RowsPerPage])[m_CaretPos.x],
				m_ColsPerPage-m_CaretPos.x);
		}
		for( i = m_CaretPos.y + 1; i < m_RowCount; i++)
		{
			tmp = m_Screen[i];
			if( i < m_RowsPerPage)
				break;
			memcpy( tmp, m_Screen[i-m_RowsPerPage],m_ColsPerPage);
			memcpy( GetLineAttr(tmp),GetLineAttr(m_Screen[i-m_RowsPerPage]),m_ColsPerPage);		}
		break;
	case 0:	// Erase from current position to end (inclusive)
	default:
		tmp = m_Screen[m_CaretPos.y];
		if( m_CaretPos.x > 0 && m_CaretPos.y > m_RowsPerPage )
		{
			memcpy( tmp, &m_Screen[m_CaretPos.y-m_RowsPerPage],m_CaretPos.x-1);
			memcpy( GetLineAttr(tmp), GetLineAttr(m_Screen[m_CaretPos.y-m_RowsPerPage]),
				m_CaretPos.x-1);
		}
		for( i = bottom; i < m_CaretPos.y; i++)
		{
			tmp = m_Screen[i];
			if( i < m_RowsPerPage)
				break;
			memcpy( tmp, m_Screen[i-m_RowsPerPage],m_ColsPerPage);
			memcpy( GetLineAttr(tmp),GetLineAttr(m_Screen[i-m_RowsPerPage]),m_ColsPerPage);		}
		break;
	}
}

void CTermData::EraseLine(int p)
{
	char* pLine = m_Screen[m_CaretPos.y];
	CTermCharAttr* pAttr = GetLineAttr(pLine);
	switch(p)
	{
	case 0:		// Clear from current position to end of line.
		memset(&pLine[m_CaretPos.x],' ',m_ColsPerPage-m_CaretPos.x);
		//memset16(&pAttr[m_CaretPos.x],CTermCharAttr::GetDefVal(),m_ColsPerPage-m_CaretPos.x);
		memset16(&pAttr[m_CaretPos.x],*(short*)&m_CurAttr,m_ColsPerPage-m_CaretPos.x);
		SetLineUpdate(pLine, m_CaretPos.x, m_ColsPerPage );
		break;
	case 1:	// Clear from head of line to current position.
		memset(&pLine, ' ',m_CaretPos.x);
		memset16(&pAttr ,CTermCharAttr::GetDefVal(),m_CaretPos.x);
		SetLineUpdate(pLine, 0, m_CaretPos.x+1);
		break;
	default:
	case 2:	// Clear whole line.
		InitNewLine( pLine, m_ColsPerPage);
		break;
	}
}

void CTermData::ParseAnsiColor(const char *pParam)
{
	while(*pParam)
	{
		int param = 0;
		while( isdigit(*pParam) )
		{
			param *= 10;
			param += *pParam - '0';
			pParam++;
		}
		if( param < 30 )	// property code
		{
			switch(param)
			{
			case 0:		// normal
				m_CurAttr.SetToDefault();
				break;
			case 1:		// bright foreground
				m_CurAttr.SetBright(true);
				break;
			case 4:		// underscore
				m_CurAttr.SetUnderLine(true);
				break;
			case 5:		// blink
			case 6:
				m_CurAttr.SetBlink(true);
				break;
			case 7:		// reverse
				m_CurAttr.SetInverse(true);
				break;
			case 8:		// invisible text (fore=back)
				m_CurAttr.SetInvisible(true);
				break;
			}
		}
		else	// color code
		{
			if( param >= 40 && param <= 47)	//	background
				m_CurAttr.SetBackground(param-40);
			else if(param <= 37)	// foreground
				m_CurAttr.SetForeground(param-30);
			// else
				// Undefined parameter!
		}
		pParam++;
	}
}

void CTermData::memset16(void *dest, short val, size_t n)
{
	short* dest16 = (short*)dest;
	short* end = dest16 + n;
	for( ; dest16 < end; dest16++ )
		*dest16 = val;
}


static gboolean update_view(CTermData* _this)
{
	if(_this->m_pView)
		_this->DoUpdateDisplay();
	INFO("do update");
	_this->m_DelayedUpdateTimeout = 0;	// Simply returning false will remove the source.
	return false;	// remove timeout source
}

void CTermData::UpdateDisplay()
{
	DetectCharSets();
	DetectHyperLinks();
#ifdef USE_IPLOOKUP
	DetectIpAddrs();
#endif

 	if( m_pView && m_pView->IsVisible() && !m_WaitUpdateDisplay )
	{
		INFO("waiting update");
		m_WaitUpdateDisplay = true;

		if( m_NeedDelayedUpdate )
		{
			if( m_DelayedUpdateTimeout )
				g_source_remove(m_DelayedUpdateTimeout);
			m_DelayedUpdateTimeout = g_timeout_add( 80, (GSourceFunc)&update_view, this);
		}
		else
			DoUpdateDisplay();
	}
	m_NeedDelayedUpdate = false;
}

void CTermData::DoUpdateDisplay()
{
	m_WaitUpdateDisplay = false;

	m_pView->m_Caret.Hide();
	for( int row = 0; row < m_RowsPerPage; row++ )
	{
		int col = 0;
		CTermCharAttr* attr = GetLineAttr( m_Screen[m_FirstLine + row] );
		bool callback_has_been_called = false;
		for( ; col < m_ColsPerPage; col++  )
		{
			if( attr[col].IsNeedUpdate() )
			{
				if( ! callback_has_been_called )
				{
					OnLineModified( m_FirstLine + row );
					callback_has_been_called = true;
				}

				if( col>0 && attr[col].GetCharSet()==CTermCharAttr::CS_MBCS2 )
					col--;
				m_pView->DrawChar( row, col );
				attr[col].SetNeedUpdate(false);
				// Check if this is a MBCS char.
				if( attr[col].GetCharSet()==CTermCharAttr::CS_MBCS1 )
				{
					attr[col+1].SetNeedUpdate(false);
					col ++;
				}
			}
		}
	}
	m_pView->UpdateCaretPos();
	m_pView->m_Caret.Show();
}

// Detect character sets here.  This is for MBCS support.
void CTermData::DetectCharSets()
{
	int iline = m_FirstLine;
	int ilast_line = iline + m_RowsPerPage;
	for( ; iline < ilast_line; iline++ )
	{
		char* line = m_Screen[iline];
		CTermCharAttr* attr = GetLineAttr( line );
		int col = 0;
		while( col < m_ColsPerPage )
		{
			if( ((unsigned char)line[col]) > 128 && (col+1)< m_ColsPerPage)
			{
				if( attr[col].IsNeedUpdate() != attr[col+1].IsNeedUpdate() )
					attr[col].SetNeedUpdate(attr[col+1].SetNeedUpdate(true));

				attr[col].SetCharSet(CTermCharAttr::CS_MBCS1);
				col++;
				attr[col].SetCharSet(CTermCharAttr::CS_MBCS2);
			}
			else
				attr[col].SetCharSet(CTermCharAttr::CS_ASCII);
			col++;
		}
	}
}


// 2004/08/03 modified by PCMan
// Check if 'ch' is an valid character in URL.
inline bool isurl(int ch)
{	return isalnum(ch) || strchr("!$&'*+,-./:;=?@_|~%#", ch);	}
// Though '(' ,')', '<', and '>' are legal characters in URLs, I ignore them because
// they are frequently used to enclose URLs. ex: (http://pcmanx.csie.net/)
inline bool isurlscheme(int ch)
{	return isalnum(ch) || strchr("+-.", ch);	}


// 2004/08/06 modified by PCMan
// This function is used to detect E-mails and called from UpdateDisplay().
inline void DetectEMails( const char *line, CTermCharAttr *attr, int len )
{
	int ilink = 0, stage = 0;
	for( int col = 0; col < len; col += (CTermCharAttr::CS_ASCII==attr[col].GetCharSet()?1:2) )
	{
		unsigned char ch = line[col];
		switch( stage )
		{
		case 0:	// a URL character is found, beginning of URL.
			if( isurl(ch) )
			{
				stage = 1;
				ilink = col;
			}
			break;
		case 1:	// '@' is found.
			if( !isurl(ch) )
				stage = 0;
			else if( '@' == ch )
				stage = 2;
			break;
		case 2:	// URL characters are found after '@'.
			if( !isurl(ch) )
				stage = 0;
			else if( '.' == ch )
				stage = 3;
			break;
		case 3:	//  This is a valid URL.
			if( !isurl(ch) )
			{
				for( ; ilink < col; ilink++ )
				{
					attr[ilink].SetHyperLink(true);
					attr[ilink].SetNeedUpdate(true);
				}
				stage = 0;
			}
		}
	}
}

// 2004/08/06 added by PCMan
// This function is used to detect URLs other than E-mails and called from UpdateDisplay().
inline void DetectCommonURLs( const char *line, CTermCharAttr *attr, int len )
{
	int ilink = 0, stage = 0;
	for( int col = 0; col < len; col += (CTermCharAttr::CS_ASCII==attr[col].GetCharSet()?1:2) )
	{
		unsigned char ch = line[col];
		switch( stage )
		{
		case 0:	// a URL scheme character is found, beginning of URL.
			if( isurlscheme(ch) )
			{
				stage = 1;
				ilink = col;
			}
			break;
		case 1:	// "://" is found.
			if( 0 == strncmp( line+col, "://", 3 ) && isurl( line[col+3] ) )
			{
				stage = 2;
				col += 3;
			}
			else if( !isurlscheme(ch) )
				stage = 0;
			break;
		case 2:	// This is a valid URL.
			if( !isurl(ch) )
			{
				for( ; ilink < col; ilink++ )
				{
					attr[ilink].SetHyperLink(true);
					attr[ilink].SetNeedUpdate(true);
				}
				stage = 0;
			}
		}
	}
}

// 2004/08/03 modified by PCMan
// This function is used to detect hyperlinks and called from UpdateDisplay().
void CTermData::DetectHyperLinks()
{
	int iline = m_FirstLine;
	int ilast_line = iline + m_RowsPerPage;
	for( ; iline < ilast_line; iline++ )
	{
		char* line = m_Screen[iline];
		CTermCharAttr* attr = GetLineAttr( line );
		// Clear all marks.
		for( int col = 0; col < m_ColsPerPage; col ++ )
			attr[col].SetHyperLink(false);
		DetectEMails( line, attr, m_ColsPerPage );	// Search for E-mails.
		DetectCommonURLs( line, attr, m_ColsPerPage );	// Search for URLs other than E-mail.
	}
}

#ifdef USE_IPLOOKUP

/* detect ipv4 addresses. */
inline void DetectIpPatterns( const char *line, CTermCharAttr *attr, int len, const regex_t *regip)
{
	regmatch_t match;
	const char *p = line;

	while ( p < line + len && regexec( regip, p, 1, &match, 0 ) == 0 )
	{
		int offset = p - line;
		if ( CTermCharAttr::CS_ASCII == attr[offset + match.rm_so].GetCharSet()
		  && CTermCharAttr::CS_ASCII == attr[offset + match.rm_eo - 1].GetCharSet() )
			for ( int i = match.rm_so; i < match.rm_eo; i++ )
			{
				attr[offset + i].SetIpAddr(true);
				attr[offset + i].SetNeedUpdate(true);
			}
		p += match.rm_eo + 1;
	}
}

/* Detect IP addresses (called from UpdateDisplay()) */
void CTermData::DetectIpAddrs()
{
	int iline = m_FirstLine;
	int ilast_line = iline + m_RowsPerPage;
	for( ; iline < ilast_line; iline++ )
	{
		char* line = m_Screen[iline];
		CTermCharAttr* attr = GetLineAttr( line );
		// Clear all marks.
		for( int col = 0; col < m_ColsPerPage; col ++ )
			attr[col].SetIpAddr(false);
		DetectIpPatterns( line, attr, m_ColsPerPage, &m_RegIp );
	}
}

#endif

typedef struct {
	CTermData* pTermData;
	string*    text;
	int        lines;
	char*      eol;
} ReadStatus;

string GetChangedAttrStr(CTermCharAttr oldattr, CTermCharAttr newattr)
{
	string text = "\x1b[";	// Control sequence introducer.
	bool reset = false;
	 // If we want to cancel bright attribute, we must reset all attributes.
	bool bright_changed = (newattr.IsBright() != oldattr.IsBright());
	if( bright_changed && 1 == oldattr.IsBright() )
		reset = true;
	// Blink attribute changed.
	// We must reset all attributes to remove 'blink' attribute.
	bool blink_changed = (newattr.IsBlink() != oldattr.IsBlink());
	if( blink_changed && 1 == oldattr.IsBlink() )
		reset = true;
	// Underline attribute changed.
	// We must reset all attributes to remove 'underline' attribute.
	bool underline_changed = (newattr.IsUnderLine() != oldattr.IsUnderLine());
	if( underline_changed && 1 == oldattr.IsUnderLine() )
		reset = true;
	// Inverse attribute changed.
	// We must reset all attributes to remove 'inverse' attribute.
	bool inverse_changed = (newattr.IsInverse() != oldattr.IsInverse());
	if( inverse_changed && 1 == oldattr.IsInverse() )
		reset = true;

	if(reset)
		text += ';';	// remove all attributes
	if( bright_changed && newattr.IsBright() )	// Add bright attribute
		text += "1;";
	if( blink_changed && newattr.IsBlink() )	// Add blink attribute
		text += "5;";
	if( underline_changed && newattr.IsUnderLine() ) // Add underline attributes.
		text += "4;";
	if( inverse_changed && newattr.IsInverse() ) // Add inverse attributes.
		text += "7;";
	if( reset || newattr.GetBackground() != oldattr.GetBackground())	// If reset or color changed.
	{
		char color[] = {'4', ('0'+ newattr.GetBackground()), ';', '\0' };
		text += color;
	}
	if( reset || newattr.GetForeground() != oldattr.GetForeground() )
	{
		char color[] = {'3', ('0' + newattr.GetForeground()), ';', '\0' };
		text += color;
	}
	if( ';' == text[ text.length()-1 ] )	// Don't worry about access violation because text.Len() always > 1.
		text = text.substr(0, text.length()-1);
	text += 'm';	// Terminate ANSI escape sequence
	return text;
}

static void read_line_with_color( int row, int col1, int col2, void* data )
{
	ReadStatus* rs = (ReadStatus*)data;
	string* text = rs->text;

	if ( rs->lines )
	{
		*text += rs->eol;

		if ( col1 == col2 )
			return;
	}

	CTermData* td = rs->pTermData;
	char* pLine = td->m_Screen[row];
	CTermCharAttr* pAttr = td->GetLineAttr( pLine );
	CTermCharAttr attr;
	attr.SetToDefault();

	// GetLineWithColor
	{
		string line;
		for ( int i = col1; i < col2; i++ )
		{
			if( !attr.IsSameAttr( pAttr[i].AsShort() ) )
			{
				// Here we've got a characters with different attributes.
				line += GetChangedAttrStr( attr, pAttr[i] );
				attr = pAttr[i];
			}
			// Append characters with the same attributes.
			if ( pLine[i] )
				line += pLine[i];
		}

		// if current background is black,
		if ( attr.GetBackground() == 0 && line.length() )
		{
			size_t n = line.find_last_not_of( ' ' );
			if( n != text->npos )
				line = line.substr( 0, n + 1 );
		}

		*text += line;
	}


	rs->lines++;
}

static void read_line( int row, int col1, int col2, void* data )
{
	ReadStatus* rs = (ReadStatus*)data;
	string* text = rs->text;

	CTermData* td = rs->pTermData;

	if ( rs->lines )
	{
		if ( rs->lines == 1 && text->length() > 0 )
		{
			// the old code does this
			//if ( !td->m_Sel->m_BlockMode )
				//*text = text->substr( 0, text->length() - 1 );

			size_t n = text->find_last_not_of( ' ' );
			if ( n != text->npos )
				*text = text->substr( 0, n + 1 );
		}

		*text += rs->eol;
	}

	string line( td->m_Screen[row] + col1, col2 - col1 );

	if ( line.length() )
	{
		// first line is handled in next call
		if ( rs->lines )
		{
			size_t n = line.find_last_not_of( ' ' );
			if ( n != line.npos )
				line = line.substr( 0, n + 1 );
			else if ( !td->m_Sel->m_BlockMode )
				line = "";
		}

		*text += line;
	}
	rs->lines++;
}

string CTermData::GetText( CTermSelection* sel, bool trim, bool color )
{
	string text;
	int endrow;
	ReadStatus rs = { this, &text, 0 };

#ifdef	__WXMSW__
	rs.eol = "\r\n";
#else
	rs.eol = "\n";
#endif

	if ( trim )
	{
		endrow = sel->m_End.row;

		for ( ; sel->m_End.row > sel->m_Start.row; sel->m_End.row-- )
		{
			if( !IsLineEmpty( sel->m_End.row ) )
				break;
		}
	}

	if ( color )
	{
		text = "\x1b[m";
		sel->ForEachLine( read_line_with_color, (void*)&rs );
		if ( rs.lines > 1 && m_Sel->m_BlockMode )
			text += rs.eol;
		text += "\x1b[m";
	}
	else
	{
		sel->ForEachLine( read_line, (void*)&rs );
		if ( rs.lines == 1 )
		{
			size_t n = text.find_last_not_of( ' ' );
			if ( n != text.npos )
				text = text.substr( 0, n + 1 );
		}
		else if ( rs.lines > 1 && m_Sel->m_BlockMode )
			text += rs.eol;
	}

	if ( trim )
		sel->m_End.row = endrow;

	return text;
}

string CTermData::GetSelectedText(bool trim)
{
	return GetText( m_Sel, trim, false );
}

string CTermData::GetSelectedTextWithColor(bool trim)
{
	return GetText( m_Sel, trim, true );
}

// 2004/08/03  Modified by PCMan
// If line[col] is a character in a hyperlink, return the start index of whole hyperlink, 
// and store its length in 'len' which is optional and can be NULL.
// If there is no hyperlink found at specified index 'col', the return value will be -1.
int CTermData::HyperLinkHitTest(const char *line, int col, int *len/*= NULL*/ )
{
	CTermCharAttr* pattr = GetLineAttr( line );
	if( !pattr[col].IsHyperLink() )
		return -1;
	int start = col;
	while( pattr[start].IsHyperLink() && start > 0 )
		start--;
	if( len )
	{
		while( pattr[col].IsHyperLink() && col < m_ColsPerPage )
			col++;
		*len = col-start;
	}
	return start;
}

// 2004/08/12	Modified by PCMan
// Delete n characters at specified position (line, col).
void CTermData::DeleteChar(int line, int col, int n)
{
	if( col > m_ColsPerPage || col < 0 )	return;
	if( line < 0 || line >= m_RowCount )	return;
	if( (col + n) > m_ColsPerPage )
		n = (m_ColsPerPage - col);

	char* pline = m_Screen[line];
	CTermCharAttr* pattr = GetLineAttr( pline );
	int col2 = m_ColsPerPage - n;
	for( ; col < col2; col++ )
	{
		pline[ col ] = pline[ col + n ];
		pattr[ col ] = pattr[ col + n ];
		pattr[ col ].SetNeedUpdate(true);
	}
	for( ; col < m_ColsPerPage; col++ )
	{
		pline[ col ] = ' ';
		pattr[ col ].SetToDefault();
		pattr[ col ].SetNeedUpdate(true);
	}
}

// 2004/08/12	Modified by PCMan
// Insert n space characters at specified position (line, col).
void CTermData::InsertChar( int line, int col, int n )
{
	if( col > m_ColsPerPage || col < 0 )	return;
	if( line < 0 || line >= m_RowCount )	return;
	if( (col + n) > m_ColsPerPage )
		n = (m_ColsPerPage - col);
	char* pline = m_Screen[line];
	CTermCharAttr* pattr = GetLineAttr( pline );

	int coln = col + n;
	for( int end = m_ColsPerPage; end >= coln; end-- )
	{
		pline[ end ] = pline[ end - n ];
		pattr[ end ] = pattr[ end - n ];
		pattr[ end ].SetNeedUpdate(true);
	}
	for( int x = col; x < coln; x++ )
	{
		pline[ x ] = ' ';
		pattr[ x ] = m_CurAttr;
		pattr[ x ].SetNeedUpdate(true);
	}
}

#define CHAR_CLASS_WORD		0x1
#define CHAR_CLASS_DELIMITER	0x2
#define CHAR_CLASS_ASCII	0x80
unsigned char CTermData::GetCharClass( int line, int col )
{
	if( col >= m_ColsPerPage || col < 0 )
		return 0;
	if( line >= m_RowCount || line < 0 )
		return 0;

	const char* pLine = m_Screen[line];
	CTermCharAttr* pAttr = GetLineAttr( pLine );
	unsigned char ret = 0;
	bool ascii;

	switch( pAttr[col].GetCharSet() )
	{
	case CTermCharAttr::CS_MBCS2:
		col--;
	case CTermCharAttr::CS_MBCS1:
		ascii = false;
		break;
	case CTermCharAttr::CS_ASCII:
		ascii = true;
		ret |= CHAR_CLASS_ASCII;
		break;
	}

	if( ascii )
	{
		if( ( 'A' <= pLine[col] && pLine[col] <= 'Z' ) ||
		    ( 'a' <= pLine[col] && pLine[col] <= 'z' ) ||
		    ( '0' <= pLine[col] && pLine[col] <= '9' ) )
			ret |= CHAR_CLASS_WORD;
		else
		{
			switch(pLine[col])
			{
			case '#':
			case '$':
			case '%':
			case '-':
			case '+':
			case '_':
			case '.':
			case '/':
				ret |= CHAR_CLASS_WORD;
				break;
			case ' ':
				ret |= CHAR_CLASS_DELIMITER;
				break;
			default:
				break;
			}
		}
	}
	else
	{
		ret |= CHAR_CLASS_WORD;
	}

	return ret;
}

// 2004/08/25	Added by PCMan
// Set attributes of all text in specified range.
void CTermData::SetTextAttr( CTermCharAttr attr, int flags, GdkPoint start, GdkPoint end, bool block)
{
	if( block || start.y == end.y )
	{
		if( end.x < start.x ){int tmp=end.y; end.y=end.x; end.x=tmp;	}
		for( int iline = start.y; iline <= end.y; iline++ )
		{
			CTermCharAttr* pattr = GetLineAttr( m_Screen[iline]);
			for( int col = start.x; col < end.x; col++ )
				pattr[col].SetTextAttr(attr, flags);
		}
	}
	else
	{
		CTermCharAttr* pattr = GetLineAttr( m_Screen[start.y]);
		for( int col = start.x; col < m_ColsPerPage; col++ )
			pattr[col].SetTextAttr(attr, flags);
		for( int iline = start.y+1; iline < end.y; iline++ )
		{
			pattr = GetLineAttr( m_Screen[iline]);
			for( int col = 0; col < m_ColsPerPage; col++ )
				pattr[col].SetTextAttr(attr, flags);	
		}
		if( start.y != end.y )
		{
			pattr = GetLineAttr( m_Screen[end.y]);
			for( int col = 0; col < end.x; col++ )
				pattr[col].SetTextAttr(attr, flags);			
		}
	}
}


//If the specified line on screen has a non space and 
//a non '\0' character or its background is not color 0 
//(usually black), return false.
//Question: how about selected block which color is inversed?
bool CTermData::IsLineEmpty(int iLine)
{
	const char* pLine = m_Screen[iLine];
	CTermCharAttr* pAttr = GetLineAttr( pLine );
	for( int col = 0; col < m_ColsPerPage; col++ )
		if( (pLine[col] && pLine[col] != ' ') || pAttr[col].GetBackground() )
			return false;
	return true;
}

// This is a callback function called from CTermData::DoUpdateDisplay().
// When new characters are written to a line in the screen buffer, 
// this function will be called with the line number passed in 'row'.
void CTermData::OnLineModified(int row)
{
    // This function can be overriden in derived class.
}

