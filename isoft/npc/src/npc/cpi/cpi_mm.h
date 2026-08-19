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
/// @file       cpi_mm.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_MM_H
#define __CPI_MM_H

#include "npc/cpi/cpi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef void *(*cpi_malloc_t)(cpi_size_t);
    typedef void *(*cpi_calloc_t)(cpi_size_t, cpi_size_t);
    typedef void *(*cpi_realloc_t)(void *, cpi_size_t);
    typedef void (*cpi_free_t)(void *);

    extern cpi_malloc_t cpi_malloc;
    extern cpi_calloc_t cpi_calloc;
    extern cpi_realloc_t cpi_realloc;
    extern cpi_free_t cpi_free;

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
