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
/// @file       cpi_exec.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_EXEC_H
#define __CPI_EXEC_H


#include "nai/io/nai_event.h"
#include "npc/cpi/cpi_types.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct cpi_exec_s cpi_exec_t;

    typedef int (*cpi_exec_cb_t)(void *, void **);

    struct cpi_exec_s
    {
        void *app;
        cpi_exec_cb_t cb;
        void *data;
        pthread_mutex_t lock;
        pthread_cond_t cond;
        cpi_u32_t transfer : 1;
        cpi_u32_t nowait : 1;
        cpi_u32_t padding : 30;
        int error;
        struct cpi_exec_s *next;
        struct cpi_exec_s *prev;
    };

#define cpi_exec_get_app(e)        ((cpi_app_t *)((e)->app))
#define cpi_exec_set_error(e, err) ((e)->error = (err))

    int cpi_exec(void *app, cpi_exec_cb_t cb, void *data, cpi_u32_t nowait);
    int cpi_exec_cb(nai_evnode_t *node, int events);
    void cpi_exec_feedback(cpi_exec_t *e);
    void cpi_exec_transfer(cpi_exec_t *e, void *ep);
    void cpi_exec_no_transfer(cpi_exec_t *e, void *ep);

#ifdef __cplusplus
}
#endif

#endif
