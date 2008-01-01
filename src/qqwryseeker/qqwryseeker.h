/*
 * helper functions to lookup IP location information.
 *
 * Copyright (C) 2007 Jason Xia <jasonxh@gmail.com>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */ 

#ifndef _QQWRYSEEKER_H
#define _QQWRYSEEKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct
{
	FILE		*fp;
	unsigned int	idx_beg;
	unsigned int	idx_end;
} QQWrySeeker;

/* initialize and free a seeker */
QQWrySeeker* seeker_new(const char *file);
void seeker_delete(QQWrySeeker *seeker);

/* IP inquiries */
int seeker_lookup(QQWrySeeker *seeker, unsigned int ip,
		char location[], int loc_size);

#ifdef __cplusplus
}
#endif

#endif
