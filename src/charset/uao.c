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
    gunichar2* ptr = pattern;
    gchar* result = NULL;
    gsize counter = 0;

    if (pattern == NULL) {
        return NULL;
    }

    do {
        gchar* word = NULL;
        for (i = 0; i < table_size; i++) {
            if (utf16[2*i] == *ptr) {
                word = g_strndup(big5 + 2*i, 2);
                break;
            }
        }
        /* In UAO table */
        if (word != NULL) {
            gchar* tmp = NULL;
            if (result == NULL) {
                tmp = g_strconcat(word, NULL);
            }
            else {
                tmp = g_strconcat(result, word, NULL);
                g_free(result);
            }
            counter += 2;
            result = tmp;
        }
        /* Not in UAO table */
        else {
            gunichar2 str[2] = {0, 0};
            gchar* utf8 = NULL;
            gchar* tmp = NULL;

            str[0] = *ptr;
            utf8 = g_utf16_to_utf8(str, -1, NULL, NULL, NULL);

            /* Not in ASCII table */
            if ((*utf8 & 0xFF) > 0x7E && *(utf8+1) != 0) {
                g_free(utf8);
                utf8 = g_strndup("\xA1\xBC", 2);
                counter += 2;
            } else {
                counter += 1;
            }

            if (result == NULL) {
                tmp = g_strconcat(utf8, NULL);
            }
            else {
                tmp = g_strconcat(result, utf8, NULL);
                g_free(result);
            }
            g_free(utf8);
            result = tmp;
        }
        ptr++;
    } while (*ptr != 0);

    g_free(pattern);

    if (size != NULL) {
        *size = counter;
    }

    return result;
}
