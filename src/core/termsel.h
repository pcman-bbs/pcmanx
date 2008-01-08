/*
 * CTermSelection, selection management for CTermData
 *
 * Copyright (C) 2005 Chia I Wu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef TERMSEL_H
#define TERMSEL_H

#ifdef __GNUG__
  #pragma interface "termsel.h"
#endif

#include "pcmanx_utils.h"

#include <cstdlib>

class CTermData;
class CTermSelection
{
public:
	typedef struct {
		int row;
		int col;
		bool left;  // start at the left half of the cell?
	} Mark;

	typedef int (*foreach_func)( int row, int col, void* data );
	// col2 is not included
	// Note that in non-block mode, there are phantom points before the
	// first row and after the last row, one for each. If they are
	// selected, the column range would be (m_ColsPerPage, m_ColsPerPage)
	// for the former case, (0, 0) for the latter one.
	// For example, select row N and N + 1 with nothing selected on row N + 1,
	// then the latter phantom point is selected.
	typedef void (*foreachline_func)( int row, int col1, int col2, void* data );

	CTermSelection(CTermData* term) : m_pTermData( term )
	{
		NewStart( 0, 0 );
	}

	~CTermSelection() {}

	// begin a new selection process
	X_EXPORT void NewStart( int row, int col, bool left = true, bool block = false );

	// update m_End and optionally run foreach_func on all points changed
	X_EXPORT void ChangeEnd( int row2, int col2, bool left2, foreach_func ff = NULL , void* data = NULL );

	// bound the point to page or buffer
	void PageBound( int& row, int& col, bool& left );
	void Bound( int& row, int& col, bool& left );

	// arrange m_Start and m_End appropriately for loop
	void Canonicalize();

	X_EXPORT void SelectPage( foreach_func ff = NULL, void* data = NULL );
	void SelectAll( foreach_func ff = NULL, void* data = NULL );
	void Unselect( foreach_func ff = NULL, void* data = NULL )
	{
		ChangeEnd( m_Start.row, m_Start.col, m_Start.left, ff, data );
	}

	// true if nothing selected
	X_EXPORT bool Empty();

	// see if the point specified is selected
	bool Has( int row, int col );

	// run function on each point selected
	// call Canonicalize() first!
	void ForEachLine( foreachline_func ff, void* data );
	void ForEach( foreach_func ff, void* data );

	// get values useful for loop
	// note that one should call Canonicalize() first and 
	// if Empty(), col may be -1 or m_ColsPerPage
	void GetStart( int& row, int& col );
	void GetEnd( int& row, int& col );
	// canonicalize (without modifying m_Start and m_End) before getting
	// the values
	void GetCanonicalMarks( int& row1, int& col1, int& row2, int& col2 );

	Mark m_Start;
	Mark m_End;

	// select in block mode
	bool m_BlockMode;

private:
	CTermData* m_pTermData;
};

#endif // TERMSEL_H
