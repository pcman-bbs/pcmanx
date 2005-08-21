#include "fileutil.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
/* 2005.08.22  Written by Hong Jen Yee (PCMan) */
int copyfile(const char* src, const char* dest, int overwrite)
{
	int fdsrc;
	int fddest;
	char buf[4096];
	size_t rlen = 0;
	struct stat file_st;
	if( -1 != (fdsrc = open(src, O_RDONLY)) )
	{
		/* File already exists */
		if( !overwrite && !access( dest, F_OK) )
			return 0;
		if( -1 != (fddest = open(dest, O_CREAT|O_WRONLY|O_TRUNC) ) )
		{
			while( (rlen = read( fdsrc, buf, sizeof(buf) )) )
				write( fddest, buf, rlen );
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

