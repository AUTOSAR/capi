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
/// @file       nai_evtimer.h
/// @brief      
/// @details
/// @date       2023-03-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _EVTIMER_H_NAI
#define _EVTIMER_H_NAI

#pragma once

#include "nai/event/nai_evloop.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_EVTIMER_T
    #define _NAI_TYPEDEF_EVTIMER_T
    typedef struct nai_evtimer_s nai_evtimer_t;
#endif

#if defined(_WIN32)
    #define NAI_HAVE_TIMER_WIN32 0
#endif

#if (NAI_HAVE_TIMERFD_CREATE)

    /**
 * the structure of the event timer
 */
    struct nai_evtimer_s
    {
        nai_evnode_t ev;      /**< the event node */
        nai_evloop_ent_t ent; /**< the entry of the event loop */
        nai_fd_t fd;          /**< the file descriptor of the system timer */
        nai_int_t et;         /**< is worked in edge trigger mode */
    };

#elif (NAI_HAVE_TIMER_CREATE)

/**
 * the structure of the event timer
 */
struct nai_evtimer_s
{
    nai_evbase_t base; /**< the event base */
    timer_t tid;       /**< the id of the system timer */
};

#elif (NAI_HAVE_TIMER_WIN32)

/**
 * the structure of the event timer
 */
struct nai_evtimer_s
{
    nai_evbase_t base; /**< the event base */
    HANDLE timer;      /**< the handle of the system timer */
    HANDLE wait;       /**< the handle of the waitor */
};

#else

/**
 * the structure of the event timer
 */
struct nai_evtimer_s
{
    nai_int_t unused; /**< unused */
};

#endif

    /**
 * initial the event timer
 * @param   p       pointer to the event timer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_evtimer_init(nai_evtimer_t* p);

    /**
 * open the event timer
 * @param   p       pointer to the event timer
 * @param   ops     pointer to the event base opeartions
 * @param   e       pointer to the context of the event base
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evtimer_open(nai_evtimer_t* p, nai_evbase_ops_t* ops, void* e);

    /**
 * close the event timer
 * @param   p       pointer to the event timer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evtimer_close(nai_evtimer_t* p);

    /**
 * start a high-res system timer
 * @param   p       pointer to the event timer
 * @param   usec    the value of absolute tick, in micro-seconds, 0 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evtimer_set(nai_evtimer_t* p, uint64_t usec);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
