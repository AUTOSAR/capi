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
/// @file       cpi_buf.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_BUF_H
#define __CPI_BUF_H

#include "npc/cpi/cpi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_BUF_SIZE 1024

    typedef struct cpi_buf_s cpi_buf_t;

    struct cpi_buf_s
    {
        cpi_u8_t data[CPI_BUF_SIZE];
        cpi_u32_t len;
        cpi_u8ptr_t pos;
        struct cpi_buf_s *prev;
        struct cpi_buf_s *next;
    };

#define cpi_buf_get_data(b)       ((b)->data)
#define cpi_buf_get_avail_size(b) (CPI_BUF_SIZE - ((b)->pos - (b)->data))
#define cpi_buf_get_left_size(b)  ((b)->len - ((b)->pos - (b)->data))
#define cpi_buf_get_pos(b)        ((b)->pos)
#define cpi_buf_inc_pos(b, n)     ((b)->pos += (n))
#define cpi_buf_inc_len(b, n)     ((b)->len += (n))
#define cpi_buf_dec_len(b, n)     ((b)->len -= (n))

    cpi_buf_t *cpi_buf_new(void);
    void cpi_buf_free(cpi_buf_t *b);
    CPI_CHAIN_FUNC_DECLARE(cpi_buf, cpi_buf_t, void, );

#ifdef __cplusplus
}
#endif

#endif
