/**
 * Copyright (c) 2005 PCMan <pcman.tw@gmail.com>
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

#include "fileutil.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int copyfile(const char* src, const char* dest, int overwrite)
{
	int fdsrc;
	int fddest;
	char buf[4096];
	size_t rlen = 0;
	struct stat file_st;
	mode_t fmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if( -1 != (fdsrc = open(src, O_RDONLY)) )
	{
		/* File already exists */
		if( !overwrite && !access( dest, F_OK) )
			return 0;
		if( -1 != (fddest = open(dest,
		                         O_CREAT | O_WRONLY | O_TRUNC,
					 fmode) ) )
		{
			while( (rlen = read( fdsrc, buf, sizeof(buf) )) )
				rlen = write( fddest, buf, rlen );
			close(fddest);
			close(fdsrc);

			stat(src, &file_st);
			chmod(dest, file_st.st_mode);
			return 0;
		}
		close(fdsrc);
	}
	return -1;
}

