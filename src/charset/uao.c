/* -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*- */
/* vim:set fileencodings=utf-8 tabstop=4 expandtab shiftwidth=4 softtabstop=4: */

#include <glib.h>

#include "uao.h"

gchar* uao_b2u(const gchar* big5, const gunichar2* utf16, const gint table_size, const gchar* input, gsize* size)
{
    gint i = 0;

    if ((*input & 0x80) != 0x80) {
        return NULL;
    }

    for (i = 0; i < table_size; i++) {
        gchar high = input[0] & 0xFF;
        gchar low = input[1] & 0xFF;
        if (big5[2*i] == high && big5[2*i + 1] == low) {
            return g_utf16_to_utf8(utf16 + 2*i, -1, NULL, (glong*) size, NULL);
        }
    }

    return NULL;
}

gchar* uao_u2b(const gunichar2* utf16, const gchar* big5, const gint table_size, const gchar* input, gsize* size)
{
    gint i = 0;

    gunichar2* pattern = g_utf8_to_utf16(input, -1, NULL, NULL, NULL);

    if (pattern == NULL) {
        return NULL;
    }

    for (i = 0; i < table_size; i++) {
        if (utf16[2*i] == *pattern) {
            if (size != NULL) {
                *size = 2;
            }
            g_free(pattern);
            return g_strndup(big5 + 2*i, 2);
        }
    }

    g_free(pattern);

    return NULL;
}
