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
/// @file       cpi_string.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_STRING_H
#define __CPI_STRING_H

#include "npc/cpi/cpi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct cpi_string_s cpi_string_t;

    struct cpi_string_s
    {
        cpi_u8ptr_t data;
        cpi_size_t len;
        cpi_u32_t data_ref : 1;
        cpi_u32_t ref : 31;
    };

#define cpi_string(s)                                                                                                  \
    {                                                                                                                  \
        (cpi_u8ptr_t) s, sizeof(s) - 1, 1, 1                                                                           \
    }
#define cpi_string_set(pstring, s)                                                                                     \
    ;                                                                                                                  \
    {                                                                                                                  \
        (pstring)->data     = (cpi_u8ptr_t)(s);                                                                        \
        (pstring)->len      = strlen(s);                                                                               \
        (pstring)->data_ref = 1;                                                                                       \
        (pstring)->ref      = 1;                                                                                       \
    }
#define cpi_string_nset(pstring, s, n)                                                                                 \
    ;                                                                                                                  \
    {                                                                                                                  \
        (pstring)->data     = (cpi_u8ptr_t)(s);                                                                        \
        (pstring)->len      = (n);                                                                                     \
        (pstring)->data_ref = 1;                                                                                       \
        (pstring)->ref      = 1;                                                                                       \
    }
#define cpi_string_ref(pstring) (++(pstring)->ref, (pstring))

#define cpi_string_free(pstr)                                                                                          \
    ({                                                                                                                 \
        if ((pstr) != NULL) {                                                                                          \
            if ((pstr)->ref-- <= 1) {                                                                                  \
                if (!(pstr)->data_ref && (pstr)->data != NULL) {                                                       \
                    cpi_free((pstr)->data);                                                                            \
                }                                                                                                      \
                cpi_free((pstr));                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    })

    cpi_string_t *cpi_string_new(const char *s);
    cpi_string_t *cpi_string_dup(cpi_string_t *str);
    int cpi_string_strcmp(cpi_string_t *s1, cpi_string_t *s2);

#ifdef __cplusplus
}
#endif

#endif
