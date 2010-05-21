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

#include <stdlib.h>
#include "qqwryseeker.h"


/* initialize a seeker */
QQWrySeeker* seeker_new(const char *file)
{
	FILE *fp = fopen(file, "r");
	if (!fp)
		return NULL;

	QQWrySeeker *seeker = (QQWrySeeker*) malloc(sizeof(QQWrySeeker));
	seeker->fp = fp;
	fread(&seeker->idx_beg, 4, 1, fp);
	fread(&seeker->idx_end, 4, 1, fp);

	return seeker;
}

/* free a seeker */
void seeker_delete(QQWrySeeker *seeker)
{
	if (seeker)
	{
		fclose(seeker->fp);
		free(seeker);
	}
}

/* read a null terminated string from file.
 * "exaust" indicates we should seek till '\0' when running out of buffer.
 * return the number of bytes read, excluding '\0'. returned buf[] is null terminated
 * which means the return value will be at most (buf_size - 1).
 */
static inline int read_str(FILE *fp, char buf[], int buf_size, char exaust)
{
	if (buf_size <= 0)
		return -1;

	int i = 0;
	for (; i < buf_size - 1 && fread(buf + i, 1, 1, fp) && buf[i]; i++);
	buf[i] = 0;

	if (i == buf_size - 1 && exaust) /* we run out of buffer */
	{
		char c = 0;
		while (fread(&c, 1, 1, fp) && c); /* still we have to seek to string end,
						     although we don't keep the content */
	}

	return i;
}

/* read a 3-byte int */
static inline int read_int3(FILE *fp)
{
	int buf = 0;
	fread(&buf, 3, 1, fp);
	return buf;
}

/* read a 4-byte int */
static inline int read_int4(FILE *fp)
{
	int buf;
	fread(&buf, 4, 1, fp);
	return buf;
}

/* read a location piece: country or area.
 * the return value and policy of buf[] is the same as read_str().
 * stream pointer will be put in correct place when done.
 */
static inline int read_location_piece(FILE *fp, char buf[], int buf_size)
{
	if (buf_size <= 0)
		return -1;

	buf[0] = 0;
	if (!fread(buf, 1, 1, fp))
		return 0;

	int cur = -1;
	if (buf[0] == 0x02 || buf[0] == 0x01)
		cur = ftell(fp) + 3;

	/* seek to the true content */
	while (buf[0] == 0x02 || buf[0] == 0x01)
	{
		fseek(fp, read_int3(fp), SEEK_SET);
		if (!fread(buf, 1, 1, fp))  /* should not happen */
		{
			buf[0] = 0;
			return 0;
		}
	}

	fseek(fp, -1, SEEK_CUR); /* "spit" the byte we just read */
	int i;
	if (cur != -1)  /* we have jumped */
	{
		i = read_str(fp, buf, buf_size, 0); /* read the real string */
		fseek(fp, cur, SEEK_SET); /* seek back to orig pos */
	}
	else
		i = read_str(fp, buf, buf_size, 1); /* read the real string */

	return i;
}

/* read a full location including country and area.
 * the return value and policy of buf[] is the same as read_str().
 */
static int read_location(FILE *fp, char buf[], int buf_size)
{
	if (buf_size <= 0)
		return -1;

	buf[0] = 0;
	if (!fread(buf, 1, 1, fp))
		return 0;

	if (buf[0] == 0x01)
	{
		fseek(fp, read_int3(fp), SEEK_SET);
		return read_location(fp, buf, buf_size);
	}

	fseek(fp, -1, SEEK_CUR);
	int i = read_location_piece(fp, buf, buf_size);  /* read country */
	i += read_location_piece(fp, buf + i, buf_size - i); /* read area */

	return i;
}

/* test if ip matches current idx.
 * return values: -1, 0, 1 standing for ip smaller, in range, or greater.
 */
static inline int ip_match(FILE *fp, unsigned int ip, int idx)
{
	fseek(fp, idx, SEEK_SET);
	unsigned int n = read_int4(fp); /* ip range start */
	if (ip < n)
		return -1;
	fseek(fp, read_int3(fp), SEEK_SET);
	n = read_int4(fp); /* ip range end */
	return (ip <= n) ? 0:1;
}

/* ip inquiries.
 * ip is little endian 4-byte int.
 * note that location encoding is not converted, which will most likely be gb2312.
 */
int seeker_lookup(QQWrySeeker *seeker, unsigned int ip, char location[], int loc_size)
{
	if (loc_size <= 0)
		return -1;

	int low = 0, up = (seeker->idx_end - seeker->idx_beg) / 7;
	while (low <= up)
	{
		int mid = (low + up) / 2;
		int compare = ip_match(seeker->fp, ip, seeker->idx_beg + mid * 7);
		if (compare == 0) /* we found the record */
			return read_location(seeker->fp, location, loc_size);
		if (compare < 0)
			up = mid - 1;
		else
			low = mid + 1;
	}

	location[0] = 0;
	return 0;
}
