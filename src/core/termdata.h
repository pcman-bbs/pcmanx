/////////////////////////////////////////////////////////////////////////////
// Name:        termdata.h
// Purpose:     Store terminal screen data and parse ANSI escape sequence
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      pcman.tw@gmail.com
// Created:     2004.7.16
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
//		Neversay 2005.1.8 (neversay.misher@gmail.com)
/////////////////////////////////////////////////////////////////////////////


#ifndef TERMDATA_H
#define TERMDATA_H

#ifdef __GNUG__
  #pragma interface "termdata.h"
#endif

#include "pcmanx_utils.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "termsel.h"

#include <string>

#include <sys/types.h>
#include <regex.h>

using namespace std;

#define BIT_NUMBER_OF_BOOL 1
#define BIT_NUMBER_OF_CHARSET 2
#define BIT_NUMBER_OF_COLOR 3
#define SIZE_OF_COLOR_TABLE 16

// Flags used by CTermCharAttr::SetTextAttr() & CTermData::SetTextAttr().
#define		STA_FG			1
#define		STA_BG			2
#define 	STA_BRIGHT		4
#define 	STA_BLINK		8
#define		STA_UNDERLINE	16
#define		STA_INVERSE		32
#define		STA_INVISIBLE	64
#define		STA_ALL			255


// Character attributes for text displayed in CTermView.
class CTermCharAttr
{
    public:
	X_EXPORT static GdkColor m_DefaultColorTable[SIZE_OF_COLOR_TABLE];

	enum charset {CS_ASCII=0, CS_MBCS1=1, CS_MBCS2=2};	// MBCS : multi-byte character set

	// Important note:
	// I'm not sure whether view this class as 'short' is allowed or not.
	// Maybe there'll be some bugs due to the difference in byte order between systems.
	// So, I didn't use type casts frequently to speed up some simple tasks.

	CTermCharAttr(){}
	~CTermCharAttr(){}

	//Public getter:Neversay 15/Jan/2005
	inline static GdkColor* GetDefaultColorTable(int index){
		if(index >= 0 && index < SIZE_OF_COLOR_TABLE) 
			return &m_DefaultColorTable[index];
		return NULL;
	}
	inline static GdkColor* GetDefaultColorTable(){return m_DefaultColorTable;}
	inline short GetForeground(){ return (short)m_Fg;}
	inline short GetBackground(){return (short)m_Bg;}
	inline bool IsBright(){return (m_Bright==1?true:false);}
	inline bool IsBlink(){return (m_Blink==1?true:false);}
	inline bool IsUnderLine(){return (m_UnderLine==1?true:false);}
	inline bool IsInverse(){return (m_Inverse==1?true:false);}
	inline bool IsInvisible(){return (m_Invisible==1?true:false);}
	inline bool IsHyperLink(){return (m_HyperLink==1?true:false);}
#ifdef USE_IPLOOKUP
	inline bool IsIpAddr(){return (m_IpAddr==1?true:false);}
#endif
	inline bool IsNeedUpdate(){return (m_NeedUpdate==1?true:false);}
	inline short GetCharSet(){return (short)m_CharSet;}
	//Public setter:Neversay 15/Jan/2005
	inline static bool SetDefaultColorTable(int index, GdkColor* newColor){
		if(index >= 0 && index < SIZE_OF_COLOR_TABLE){
			m_DefaultColorTable[index] = *newColor;
			return true;
		}return false;
	}
	inline bool SetForeground(int foreground){ 
		if((foreground >=0) && (foreground < 1<<BIT_NUMBER_OF_COLOR)){
			m_Fg = foreground;
			return true;
		}
		return false;
	}
	inline bool SetBackground(int background){ 
		if((background >=0) && (background < 1<<BIT_NUMBER_OF_COLOR)){
			m_Bg = background;
			return true;
		}
		return false;
	}
	inline bool SetBright(bool flag){m_Bright = flag?1:0;return flag;}
	inline bool SetBlink(bool flag){m_Blink = flag?1:0;return flag;}
	inline bool SetUnderLine(bool flag){m_UnderLine = flag?1:0;return flag;}
	inline bool SetInverse(bool flag){m_Inverse = flag?1:0;return flag;}
	inline bool SetInvisible(bool flag){m_Invisible = flag?1:0;return flag;}
	inline bool SetHyperLink(bool flag){m_HyperLink = flag?1:0;return flag;}
#ifdef USE_IPLOOKUP
	inline bool SetIpAddr(bool flag){m_IpAddr = flag?1:0;return flag;}
#endif
	inline bool SetNeedUpdate(bool flag){m_NeedUpdate = flag?1:0;return flag;}
	inline bool SetCharSet(enum charset charset){
		if(charset >= 0){
			m_CharSet = charset;
			return true;
		}return false;
	}

	//Transform the CTermCharAttr into short value.
	inline short AsShort(){	return *(short*)this;	}

	//Get default setting of CTermCharAttr.
	static short GetDefVal();
	//Set this to default value.
	void SetToDefault();

	//Get the color of foreground attribute.
	inline GdkColor* GetFgColor(GdkColor* pColorTable ){
		return &pColorTable[ m_Bright ? (( m_Inverse ? m_Bg : m_Fg )+8):( m_Inverse ? m_Bg : m_Fg ) ];
	}
	//Get the color of background attribute.
	inline GdkColor* GetBgColor(GdkColor* pColorTable ){
		return &pColorTable[( m_Inverse ? m_Fg : m_Bg )];
	}

	bool IsSameAttr(short val2);

	// Overloaded operator ==
	bool operator==(CTermCharAttr& attr);

	//Set CtermCharAttr by input attr and flags.
	void SetTextAttr( CTermCharAttr attr, int flags );

private:

	//------------bit fields----------
	unsigned char m_Fg:BIT_NUMBER_OF_COLOR;		//The value of foreground,0~7
	unsigned char m_Bg:BIT_NUMBER_OF_COLOR;		//The value of background,0~7
	unsigned char m_Bright:BIT_NUMBER_OF_BOOL;		//The flag of light color
	unsigned char m_Blink:BIT_NUMBER_OF_BOOL;		//The flag of blink text.
	//------------ 1 byte ------------
	unsigned char m_UnderLine:BIT_NUMBER_OF_BOOL;	//The flag of text underline, usually indicated hyperlink.
	unsigned char m_Inverse:BIT_NUMBER_OF_BOOL;	//Flag for exchanging color of background and foreground.
	unsigned char m_Invisible:BIT_NUMBER_OF_BOOL;	// May be removed in the future
	unsigned char m_HyperLink:BIT_NUMBER_OF_BOOL;	// For hyperlink detection
#ifdef USE_IPLOOKUP
	unsigned char m_IpAddr:BIT_NUMBER_OF_BOOL;	// For ip address detection
#endif
	unsigned char m_NeedUpdate:BIT_NUMBER_OF_BOOL;	// a flag indicate the need for re-drawing
	unsigned char m_CharSet:BIT_NUMBER_OF_CHARSET;	// character set, = CS_ASCII, CS_MBCS1, or CS_MBCS2
	//-------------- 1 byte ----------
};

/*
 * Used with CTermView to hold screen buffer, caret position, etc.
 */
class CTermView;

class X_EXPORT CTermData
{
public:
	//Detect if the specified line is empty.
	bool IsLineEmpty( int iLine );
	// Set attributes of all text in specified range.
	void SetTextAttr( CTermCharAttr attr, int flags, GdkPoint start, GdkPoint end, bool block );
	// Insert n space characters at specified position (line, col).
	void InsertChar( int line, int col, int n );
	// Delete n characters at specified position (line, col).
	void DeleteChar( int line, int col , int n = 1 );
	int HyperLinkHitTest( const char* line, int col, int* len = NULL );

	unsigned char GetCharClass( int line, int col );

	string GetSelectedText( bool trim = true );
	string GetSelectedTextWithColor( bool trim = true );

	string GetText( CTermSelection* sel, bool trim, bool color );

	// Get text attributes of the line.
	inline CTermCharAttr* GetLineAttr( const char* pLine, const int ColsPerPage )
	{
		return ( CTermCharAttr* ) ( pLine + ColsPerPage + 1 );
	}
	inline CTermCharAttr* GetLineAttr( const char* pLine )
	{
		return ( CTermCharAttr* ) ( pLine + m_ColsPerPage + 1 );
	}

	//Get all text from (0,0) to (maxX, maxY), if trim is true, it return text
	//without tail black spaces.
	inline string GetAllText( bool trim = true )
	{
		CTermSelection sel( this );
		sel.SelectAll();
		return GetText( &sel, trim, false );
	}
	//The same as GetAllText but get color too.
	inline string GetAllTextWithColor( bool trim = true )
	{
		CTermSelection sel( this );
		sel.SelectAll();
		return GetText( &sel, trim, true );
	}

	void GetLine( int x1, int x2, int y, void* data );
	string GetLineWithColor( char* pLine, int start, int end );
	void DetectCharSets();
	void DetectHyperLinks();
#ifdef USE_IPLOOKUP
	void DetectIpAddrs();
#endif
	void UpdateDisplay();
	void DoUpdateDisplay();
	static void memset16( void* dest, short val, size_t n );
	inline void ParseAnsiColor( const char* pParam );
	inline void EraseLine( int p );
	void ClearScreen( int p );
	inline void GoToXY( int x, int y );
	inline void ScrollUp( int n = 1 );
	inline void ScrollDown( int n = 1 );
	void InsertNewLine( int y, int count = 1 );
	inline void SetLineUpdate( char* pLine, short start, short end )
	{
		CTermCharAttr * pAttr = GetLineAttr( pLine );
		for ( ; start < end; start++ )
			pAttr[ start ].SetNeedUpdate( true );
	}

	// Set update region of lines.  Whole line will be marked as invalid.
	inline void SetWholeLineUpdate( char* pLine )
	{
		SetLineUpdate( pLine, 0, m_ColsPerPage );
	}

	// Put characters on the terminal screen.
	void PutChar( unsigned char ch );
	inline void Tab();
	inline void CarriageReturn();

	//Call virtual function in dirived class to determine
	//whether to beep or show a visual indication instead.
	virtual void Bell();

	void Back();
	void LineFeed();
	// Parse ANSI escape sequence
	inline void ParseAnsiEscapeSequence( const char* CmdLine, char type );
	// class constructor
	CTermData( CTermView* pView );
	// class destructor
	virtual ~CTermData();

	// Set sizes of screen buffer, reallocate buffer automatically when needed.
	void SetScreenSize( int RowCount, unsigned short RowsPerPage, unsigned short ColsPerPage );
	// Change row count of screen buffer
	void SetRowCount( int RowCount );

	// Initialize new lines
	void InitNewLine( char* NewLine, const int ColsPerPage );

	// Allocate new lines
	inline char* AllocNewLine( const int ColsPerPage )
	{
		// struct{ char[ColsPerPage], char='\0', CTermCharAttr[ColsPerPage]}
		// Neversay:The structure is show below:
		// [ ColsPerPage*char + '\0' + ColsPerPage*CTermCharAttr ]
		// so size is: ColsPerPage*1 + 1 + ColsPerPage*sizeof(CTermCharAttr)
		char * NewLine = new char[ 1 + ColsPerPage * ( 1 + sizeof( short ) ) ];

		InitNewLine( NewLine, ColsPerPage );
		return NewLine;
	}


	// Allocate screen buffer.
	void AllocScreenBuf( int RowCount, unsigned short RowsPerPage, unsigned short ColsPerPage );
	virtual void OnLineModified( int row );


	///////////////////////////////////////////////////////////////////
	//Data Field Section
	///////////////////////////////////////////////////////////////////

	int m_FirstLine;
	CTermCharAttr m_CurAttr;
	CTermCharAttr m_SavedAttr;
	unsigned short m_ScrollRegionBottom;
	unsigned short m_ScrollRegionTop;

	// Pointor to CTermView window, which is responsible for display.
	CTermView* m_pView;
	// selection management
	CTermSelection* m_Sel;
	// Caret Position
	GdkPoint m_CaretPos;
	GdkPoint m_OldCaretPos;

	// command line buffer, used to store telnet commands and ANSI escape sequence
	unsigned char m_CmdLine[ 33 ];
	unsigned char* m_pCmdLine;

	// Point to screen buffer of terminal
	char** m_Screen;
	// m_Screen = new (char*)[m_RowCount];
	// The total number of rows in buffer.
	int m_RowCount;
	// Rows per page
	unsigned short m_RowsPerPage;
	// Cols per page
	unsigned short m_ColsPerPage;

	// Encoding
	string m_Encoding;

	bool m_WaitUpdateDisplay;
	bool m_NeedDelayedUpdate;
	guint m_DelayedUpdateTimeout;
private:
#ifdef USE_IPLOOKUP
	regex_t m_RegIp;
#endif
};
inline bool operator == (GdkPoint& pt1, GdkPoint& pt2)
{	return (pt1.x == pt2.x && pt1.y == pt2.y);	}

inline bool operator != (GdkPoint& pt1, GdkPoint& pt2)
{	return !(pt1==pt2);	}

#endif // TERMDATA_H

