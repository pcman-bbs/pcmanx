/* -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*- */
/* vim:set fileencodings=utf-8 tabstop=4 expandtab shiftwidth=4 softtabstop=4: */

#ifndef __UAO250_H__
#define __UAO250_H__

#include <glib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Convert BIG-5 to UTF-8.
     *
     * @param input BIG-5 encoding sequence.
     * @param size The size of UTF-8 encoding sequence.
     *
     * @return UTF-8 encoding sequence.
     * @retval NULL If input doesn't exist at UAO 2.50
     */
    extern gchar* uao250_b2u(const gchar* input, gsize* size);

    /** 
     * @brief Convert UTF-8 to BIG-5.
     * 
     * @param input UTF-8 encoding sequence.
     * @param size The size of BIG-5 encoding sequence.
     * 
     * @return BIG-5 encoding sequence.
     * @retval NULL If input doesn't exist at UAO 2.50
     */
    extern gchar* uao250_u2b(const gchar* input, gsize* size);

#ifdef __cplusplus
}
#endif

#endif /* __UAO250_H__ */
