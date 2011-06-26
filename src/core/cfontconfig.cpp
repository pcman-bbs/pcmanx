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
 
#ifdef __GNUG__
  #pragma implementation "cfontconfig.h"
#endif

#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>
#include <gdk/gdkx.h>

#include "cfontconfig.h"

CFontConfig::CFontConfig(void)
{
    FcInit();

    FcPattern* pattern = FcPatternCreate();
    FcConfigSubstitute(0, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcFontSet* font_set = FcFontSetCreate();
    FcFontSet* patterns = FcFontSort(0, pattern, FcTrue, 0, NULL);

    for (int i = 0; i < patterns->nfont; i++) {
        FcPattern* font_pattern = FcFontRenderPrepare(NULL, pattern, patterns->fonts[i]);
        if (font_pattern) {
            FcFontSetAdd(font_set, font_pattern);
        }
    }

    FcFontSetSortDestroy(patterns);
    FcPatternDestroy(pattern);

    Display *display = gdk_x11_get_default_xdisplay();
    gint screen = gdk_x11_get_default_screen();

    for (int i = 0; i < font_set->nfont; i++) {
        FcPattern* font = FcPatternFilter(font_set->fonts[i], NULL);
        FcChar8* family = NULL;
        if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch) {
            XftFont* xft_font = XftFontOpen(display, screen,
                    FC_FAMILY, FcTypeString, family,
                    FC_SIZE, FcTypeDouble, (double) 16,
                    FC_WEIGHT, FcTypeInteger, FC_WEIGHT_MEDIUM,
                    FC_ANTIALIAS, FcTypeBool, FcTrue,
                    XFT_CORE, FcTypeBool, FcFalse,
                    NULL);
            if (xft_font) {
                fonts.push_back(xft_font);
            }
        }
        FcPatternDestroy(font);
    }

    FcFontSetDestroy(font_set);
}

CFontConfig::~CFontConfig(void)
{
    Display *display = gdk_x11_get_default_xdisplay();
    for (vector<XftFont*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
        XftFontClose(display, *it);
    }
    FcFini();
}

CFontConfig* CFontConfig::Instance(void)
{
    static CFontConfig* instance = new CFontConfig();
    return instance;
}

XftFont* CFontConfig::SearchFontFor(FcChar32 ucs4)
{
    Display *display = gdk_x11_get_default_xdisplay();
    for (vector<XftFont*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
        if (XftCharExists(display, *it, ucs4) == FcTrue) {
            return *it;
        }
    }
    return NULL;
}
