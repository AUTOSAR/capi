// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       cpi_string.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_string.h"
#include "npc/cpi/cpi_mm.h"
#include <string.h>

cpi_string_t *cpi_string_new(const char *s)
{
    cpi_string_t *str = (cpi_string_t *)cpi_malloc(sizeof(cpi_string_t));
    if (str == NULL)
        goto out;

    if (s == NULL) {
        str->data = NULL;
        str->len = 0;
        str->data_ref = 0;
        str->ref = 1;
        goto out;
    }
    cpi_s32_t len = strlen(s);
    if ((str->data = (cpi_u8ptr_t)cpi_malloc(len + 1)) == NULL) {
        cpi_free(str);
        str = NULL;
        goto out;
    }
    memcpy(str->data, s, len);
    str->data[len] = 0;
    str->len = len;
    str->data_ref = 0;
    str->ref = 1;
out:
    return str;
}

cpi_string_t *cpi_string_dup(cpi_string_t *str)
{
    cpi_string_t *s = (cpi_string_t *)cpi_malloc(sizeof(cpi_string_t));
    if (s == NULL)
        goto out;
    if ((s->data = (cpi_u8ptr_t)cpi_malloc(str->len + 1)) == NULL) {
        cpi_free(s);
        s = NULL;
        goto out;
    }
    memcpy(s->data, str->data, str->len);
    s->data[str->len] = 0;
    s->len = str->len;
    s->data_ref = 0;
    s->ref = 1;
out:
    return s;
}

int cpi_string_strcmp(cpi_string_t *s1, cpi_string_t *s2)
{
    int r;
    cpi_u32_t *i1 = (cpi_u32_t *)(s1->data), *i2 = (cpi_u32_t *)(s2->data), i;

    if (s1 == s2 || s1->data == s2->data) {
        r = 0;
    } else if (s1->len > s2->len) {
        r = 1;
    } else if (s1->len < s2->len) {
        r = -1;
    } else {
        if (s1->len > 280 || (s1->len % sizeof(cpi_u32_t))) {
            r = memcmp(s1->data, s2->data, s1->len);
        } else {
            for (i = 0; i < s1->len; ) {
                if ((r = (*i1++ - *i2++)) != 0)
                    goto out;
                i += sizeof(cpi_u32_t);
            }
        }
    }
out:
    return r;
}

