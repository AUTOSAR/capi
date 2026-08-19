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
/// @file       nai_osloop.h
/// @brief      
/// @details
/// @date       2023-10-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _OSLOOP_H_NAI
#define _OSLOOP_H_NAI

#pragma once

#include "nai/io/nai_event.h"
#include "nai/os/nai_thread.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef struct nai_osloop_s nai_osloop_t;

#if (__darwin__)

    #include "nai/os/unix/nai_macos.h"

    struct nai_osloop_s
    {
        CFRunLoopRef loop;
        CFRunLoopSourceRef sig;
        nai_atomic32_t refs;
        nai_int_t started;
        nai_int_t done;
        nai_int_t term;
        nai_int_t error;
        nai_cond_t* cond;
        nai_thread_t thread;
    };

#else

struct nai_osloop_s
{
    nai_atomic32_t refs;
    nai_int_t term;
    nai_cond_t* cond;
    nai_evloop_t* loop;
    nai_thread_t thread;
};

#endif

    nai_osloop_t* nai_osloop_get();

    nai_int_t nai_osloop_release(nai_osloop_t* p);

    nai_int_t nai_osloop_lock();

    nai_int_t nai_osloop_unlock();

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
