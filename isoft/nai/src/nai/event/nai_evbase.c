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
/// @file       nai_evbase.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evbase.h"
#include "nai/nai_config.h"
#include "nai/os/nai_aio.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"


#if defined(_WIN32)
#ifndef NAI_HAVE_POLL
#define NAI_HAVE_POLL  1
#endif
#ifndef NAI_HAVE_SELECT
#define NAI_HAVE_SELECT  1
#endif
extern nai_evbase_ops_t nai_iocp;
extern nai_evbase_ops_t nai_wpoll;
#endif

#if (NAI_AIO_USE_URING)
extern nai_evbase_ops_t nai_io_uring;
#endif

#if (NAI_HAVE_EPOLL)
extern nai_evbase_ops_t nai_epoll;
#endif

#if (NAI_HAVE_KQUEUE)
extern nai_evbase_ops_t nai_kqueue;
#endif

#if (NAI_HAVE_POLL)
extern nai_evbase_ops_t nai_poll;
#endif

#if (NAI_HAVE_SELECT)
extern nai_evbase_ops_t nai_select;
#endif


static nai_evbase_ops_t* nai_events[] = {
#if (NAI_AIO_USE_URING)
    &nai_io_uring, 
#endif
#if defined(_WIN32)
    &nai_iocp, 
    &nai_wpoll, 
#endif
#if (NAI_HAVE_EPOLL)
    &nai_epoll, 
#endif
#if (NAI_HAVE_KQUEUE)
    &nai_kqueue, 
#endif
#if (NAI_HAVE_POLL)
    &nai_poll, 
#endif
#if (NAI_HAVE_SELECT)
    &nai_select, 
#endif
    0
};


nai_int_t nai_evbase_init(nai_evbase_t* e)
{
    e->ops = 0;
    e->ud = 0;
    return 0;
};


nai_int_t nai_evbase_open(nai_evbase_t* e, 
    nai_int_t optional, nai_int_t require, const char* name)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t found;
    nai_evbase_ops_t* ops;
    nai_evbase_ops_t* last;


    for (n = 0; ; n ++) {
        last = 0;
        found = 0;
        for ( ; n < (intptr_t)nai_countof(nai_events); n ++) {
            ops = nai_events[n];
            if (ops == 0) {
                break;
            };

            if ((ops->feature_m & require) != require) {
                continue;
            };
            if (name && name[0]) {
                if (strcmp(ops->name, name) != 0) {
                    continue;
                };
            } else {
                if (!(ops->feature_m & optional) && 
                    !(ops->feature_e & optional)) {
                    if (last == 0) {
                        last = ops;
                    };
                    continue;
                };
            };

            found = 1;
            break;
        };
        if (!found) {
            if (!last) {
                nai_errno = ENOTSUP;
                r = -1;
                break;
            };

            ops = last;
        };

        e->ops = ops;
        e->ud = e->ops->open(0);
        if (e->ud == 0) {
            e->ops = 0;
            r = -1;
            if (nai_errno != ENOTSUP) {
                break;
            };
            continue;
        };

        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_evbase_close(nai_evbase_t* e)
{
    nai_int_t r;


    if (e->ud && e->ops) {
        r = e->ops->close(e->ud);
        if (r < 0) {
            goto _end;
        };
    };

    e->ops = 0;
    e->ud = 0;
    r = 0;

_end:
    return r;
};

