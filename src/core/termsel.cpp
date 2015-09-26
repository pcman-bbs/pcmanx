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
#ifdef __GNUG__
  #pragma implementation "termsel.h"
#endif


#include "termsel.h" // class's header file
#include "termdata.h" // class's header file
#include "debug.h"

void CTermSelection::NewStart( int row, int col, bool left, bool block )
{
	m_Start.row = m_End.row = row;
	m_Start.col = m_End.col = col;
	m_Start.left = m_End.left = left;
	m_BlockMode = block;
}

#define iMAX(x,y) ((x)>(y)?(x):(y))
#define iMIN(x,y) ((x)>(y)?(y):(x))
#define MAX3(x,y,z) iMAX(iMAX(x,y),z)
#define MIN3(x,y,z) iMIN(iMIN(x,y),z)
#define SWAP(x,y) do { tmp = (x); (x) = (y); (y) = tmp; } while ( 0 )
void CTermSelection::ChangeEnd( int row2, int col2, bool left2, foreach_func ff, void* data )
{
	int tmp;
	int row1 = m_End.row, col1 = m_End.col;
	bool left1 = m_End.left;

	m_End.row = row2;
	m_End.col = col2;
	m_End.left = left2;

	if ( !ff )
		return;

	if ( row1 == row2 )
	{
		if ( col1 == col2 && !left1 && left2 )
		{
			SWAP( left1, left2 );
		}
		else if ( col1 > col2 )
		{
			SWAP( col1, col2 );
			SWAP( left1, left2 );
		}

		if ( m_BlockMode )
		{
			if ( row1 > m_Start.row )
			{
				row2 = row1;
				row1 = m_Start.row;
			}
			else
				row2 = m_Start.row;
		}
		else
			row2 = row1;

		if ( !left1 )
			col1++;
		if ( left2 )
			col2--;

		for ( int row = row1; row <= row2; row++ )
			for ( int col = col1; col <= col2; )
				col += ff( row, col, data );
	}
	// this can be optimized
	else if ( m_BlockMode )
	{
		tmp  = MIN3( m_Start.row, row1, row2 );
		row2 = MAX3( m_Start.row, row1, row2 );
		row1 = tmp;

		tmp  = MIN3( m_Start.col, col1, col2 );
		col2 = MAX3( m_Start.col, col1, col2 );
		col1 = tmp;

		for ( int row = row1; row <= row2; row++ )
			for ( int col = col1; col <= col2; )
				col += ff( row, col, data );
	}
	else
	{
		if ( row1 > row2 )
		{
			SWAP( col1, col2 );
			SWAP( row1, row2 );
			SWAP( left1, left2 );
		}

		if ( !left1 )
			col1++;
		for ( int col = col1; col < m_pTermData->m_ColsPerPage; )
			col += ff( row1, col, data );

		for ( int row = row1 + 1; row < row2; row++ )
			for ( int col = 0; col < m_pTermData->m_ColsPerPage; )
				col += ff( row, col, data );

		if ( left2 )
			col2--;
		for ( int col = 0; col <= col2; )
			col += ff( row2, col, data );
	}
}
#undef SWAP
#undef MAX3
#undef MIN3
#undef iMIN
#undef iMAX

inline void CTermSelection::PageBound( int& row, int& col, bool& left )
{
	if ( row >= m_pTermData->m_RowsPerPage )
		row = m_pTermData->m_RowsPerPage - 1;
	if ( row < 0 )
		row = 0;

	if ( col >= m_pTermData->m_ColsPerPage )
	{
		col = m_pTermData->m_ColsPerPage - 1;
		left = false;
		if ( col < 0 )
			col = 0;
	}
	else if ( col < 0 )
	{
		col = 0;
		left = true;
	}
}

inline void CTermSelection::Bound( int& row, int& col, bool& left )
{
	if ( row >= m_pTermData->m_RowCount )
		row = m_pTermData->m_RowCount - 1;
	if ( row < 0 )
		row = 0;

	if ( col >= m_pTermData->m_ColsPerPage )
	{
		col = m_pTermData->m_ColsPerPage - 1;
		left = false;
		if ( col < 0 )
			col = 0;
	}
	else if ( col < 0 )
	{
		col = 0;
		left = true;
	}
}

#define SWAP(x,y) do { tmp = (x); (x) = (y); (y) = tmp; } while ( 0 )
void CTermSelection::Canonicalize()
{
	int tmp;

	if ( m_Start.row > m_End.row )
	{
		SWAP( m_Start.row, m_End.row );
		SWAP( m_Start.col, m_End.col );
		SWAP( m_Start.left, m_End.left );
	}

	if (( m_Start.row == m_End.row || m_BlockMode ) && m_Start.col >= m_End.col )
	{
		if ( m_Start.col > m_End.col )
		{
			SWAP( m_Start.col, m_End.col );
			SWAP( m_Start.left, m_End.left );
		}
		// start on right half, end on left half
		else if ( !m_Start.left && m_End.left )
		{
			m_Start.left = true;
			m_End.left = false;
		}
	}
}
#undef SWAP

void CTermSelection::SelectPage( foreach_func ff, void* data )
{
	m_Start.row = m_pTermData->m_FirstLine;
	m_Start.col = 0;
	m_Start.left = true;
	m_End.row = m_pTermData->m_FirstLine + m_pTermData->m_RowsPerPage - 1;
	m_End.col = m_pTermData->m_ColsPerPage - 1;
	m_End.left = false;
	m_BlockMode = true;

	ForEach( ff, data );
}

void CTermSelection::SelectAll( foreach_func ff, void* data )
{
	m_Start.row = 0;
	m_Start.col = 0;
	m_Start.left = true;
	m_End.row = m_pTermData->m_RowCount - 1;
	m_End.col = m_pTermData->m_ColsPerPage - 1;
	m_End.left = false;
	m_BlockMode = true;

	ForEach( ff, data );
}

bool CTermSelection::Empty()
{
	if ( m_BlockMode )
	{
		if ( m_Start.col == m_End.col && m_Start.left == m_End.left )
			return true;
		else
			return false;
	}

	return ( m_Start.row == m_End.row &&
		 m_Start.col == m_End.col &&
		 m_Start.left == m_End.left );
}

bool CTermSelection::Has( int row, int col )
{
	int row1, col1, row2, col2;

	if ( Empty() )
		return false;

	GetCanonicalMarks( row1, col1, row2, col2 );

	if ( row < row1 || row > row2 )
		return false;
	if ( m_BlockMode || row1 == row2 )
		return ( col >= col1 && col <= col2 );
	else
	{
		if ( row == row1 )
			return col >= col1;
		else if ( row == row2 )
			return col <= col2;
		else
			return true;
	}
}

void CTermSelection::ForEachLine( foreachline_func ff, void* data )
{
	int row1, col1;
	int row2, col2;

	if ( !ff || Empty() )
		return;

	GetStart( row1, col1 );
	GetEnd( row2, col2 );

	// phantom point before the first row
	if ( row1 != m_Start.row )
		ff( m_Start.row, 1, 1, data );

	if ( m_BlockMode || row1 == row2 )
	{
		for ( int i = row1; i <= row2; i++ )
			ff( i, col1, col2 + 1, data );
	}
	else
	{
		ff( row1, col1, m_pTermData->m_ColsPerPage, data );
		for ( int i = row1 + 1; i < row2; i++ )
			ff( i, 0, m_pTermData->m_ColsPerPage, data );
		ff( row2, 0, col2 + 1, data );
	}

	// phantom point after the last row
	if ( row2 != m_End.row )
		ff( m_End.row, 0, 0, data );
}

void CTermSelection::ForEach( foreach_func ff, void* data )
{
	int row1, col1;
	int row2, col2;

	if ( !ff || Empty() )
		return;

	GetStart( row1, col1 );
	GetEnd( row2, col2 );

	if ( m_BlockMode )
	{
		for ( int i = row1; i <= row2; i++ )
			for ( int j = col1; j <= col2; )
				j += ff( i, j, data );
	}
	else if ( row1 == row2 )
	{
		for ( int i = col1; i <= col2; )
			i += ff( row1, i, data );
	}
	else
	{
		for ( int i = col1; i < m_pTermData->m_ColsPerPage; )
			i += ff( row1, i, data );

		for ( int i = row1 + 1; i < row2; i++ )
			for ( int j = 0; j < m_pTermData->m_ColsPerPage; )
				j += ff( i, j, data );
		for ( int i = 0; i <= col2; )
			i += ff( row2, i, data );
	}
}

inline void CTermSelection::GetStart( int& row, int& col )
{
	if ( m_Start.left )
	{
		row = m_Start.row;
		col = m_Start.col;
	}
	else if ( m_Start.col < m_pTermData->m_ColsPerPage - 1 )
	{
		row = m_Start.row;
		col = m_Start.col + 1;
	}
	else if ( m_Start.row < m_pTermData->m_RowCount - 1 )
	{
		row = m_Start.row + 1;
		col = 0;
	}
	else // last row and last column
		col = m_pTermData->m_RowCount;
}

inline void CTermSelection::GetEnd( int& row, int& col )
{
	if ( !m_End.left )
	{
		row = m_End.row;
		col = m_End.col;
	}
	else if ( m_End.col > 0 )
	{
		row = m_End.row;
		col = m_End.col - 1;
	}
	else if ( m_Start.row > 0 )
	{
		row = m_End.row - 1;
		col = m_pTermData->m_ColsPerPage - 1;
	}
	else // (0, 0)
		col = -1;
}

void CTermSelection::GetCanonicalMarks( int& row1, int& col1, int& row2, int& col2 )
{
	Mark s = m_Start, e = m_End;

	Canonicalize();
	GetStart( row1, col1 );
	GetEnd( row2, col2 );

	m_Start = s;
	m_End = e;
}



