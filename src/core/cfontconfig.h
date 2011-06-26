/* -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*- */
/* vim:set fileencodings=utf-8 tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
/**
 * Copyright (c) 2011 PCManX Development Group <pcmanx@googlegroups.com>
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

#ifndef C_FONT_CONFIG_H
#define C_FONT_CONFIG_H

#ifdef __GNUG__
  #pragma interface "cfontconfig.h"
#endif

#include <X11/Xft/Xft.h>
#include <vector>

using namespace std; 

class CFontConfig {
    private:
        CFontConfig(void);
        vector<XftFont*> fonts;
    public:
        ~CFontConfig(void);
        static CFontConfig* Instance(void);
        XftFont* SearchFontFor(FcChar32 ucs4);
};

#endif
