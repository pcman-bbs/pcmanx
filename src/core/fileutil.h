#ifndef  FILEUTIL_H
#define  FILEUTIL_H

#include "pcmanx_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

	/* 2005.08.22  Written by Hong Jen Yee (PCMan) */
	int copyfile(const char* src, const char* dest, int overwrite);

#ifdef __cplusplus
}
#endif

#endif

