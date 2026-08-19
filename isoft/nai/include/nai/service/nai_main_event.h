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
/// @file       nai_main_event.h
/// @brief      
/// @details
/// @date       2021-02-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MAIN_EVENT_H_NAI
#define _MAIN_EVENT_H_NAI

#pragma once

#include "nai/runtime/nai_string.h"
#include "nai_listening.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_MAIN_EVENT_T
    #define _NAI_TYPEDEF_MAIN_EVENT_T
    typedef struct nai_main_event_s nai_main_event_t;
#endif

    struct nai_main_event_s
    {
        nai_evloop_t* loop;         /**< pointer to the event loop, 
                                     which can be set as user-provided */
        nai_task_pool_t* task_pool; /**< pointer to the task pool */
        nai_listening_map_t ls;     /**< all listening */
        nai_str_t name;             /**< the backend name of event loop */
        nai_int_t required;         /**< the required features of event loop */
        nai_int_t optional;         /**< the optional features of event loop */
        nai_int_t timeval;          /**< the time interval of pre-loop */
        nai_int_t max_connections;  /**< the max connections */
        nai_atomic_t connections;   /**< the number of current connections */
        union
        {
            struct
            {
                uint32_t own : 1; /**< is owned event loop */
            };
            uint32_t flags;
        };
    };

    /**
 * get event of the main
 * @param   m       pointer to the main
 * @return  the address of the event on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_main_event_t* nai_main_event_get(nai_main_t* m);

    NAI_EXTERN
    nai_int_t nai_main_event_inc_connection(nai_main_event_t* e);

    NAI_EXTERN
    nai_int_t nai_main_event_dec_connection(nai_main_event_t* e);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
