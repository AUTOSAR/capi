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
/// @file       nai_unix.c
/// @brief      
/// @details
/// @date       2021-06-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if !defined(_WIN32)


#include "nai_unix.h"
#include "nai/os/nai_proc.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"


#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if (NAI_HAVE_SCHED_H)
#include <sched.h>
#endif



typedef struct nai_unix_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_thread_key_t tlocal;
} nai_unix_t;


static nai_unix_t nai_unix = {
    0, 0, NAI_ONCE_INIT, NAI_THREAD_KEY_NIL
};


static void nai_unix_tlocal_release(void* h);


extern void nai_aio_fork(nai_int_t at);
extern void nai_signal_fork(nai_int_t at);
extern void nai_osloop_fork(nai_int_t at);
static void nai_unix_fork(nai_int_t at)
{
    nai_aio_fork(at);
    nai_signal_fork(at);
    nai_osloop_fork(at);
    return;
};


static void nai_unix_fork_pre()
{
    nai_unix_fork(0);
    return;
};


static void nai_unix_fork_parent()
{
    nai_unix_fork(1);
    return;
};


static void nai_unix_fork_child()
{
    nai_unix_fork(2);
    return;
};


static void nai_unix_io_interrupt_handle(nai_int_t sig)
{
    (void)sig;
    return;
};


static void nai_unix_init()
{
    nai_int_t r;
    uint32_t n;
    uint32_t size;
    uint32_t shift;


#if (NAI_HAVE_GETPAGESIZE)
    size = getpagesize();
#else
    size = sysconf(_SC_PAGESIZE);
#endif

    n = size;
    for (shift = 0; n >>= 1; shift ++) {
    };

    nai_pagesize = size;
    nai_pagesize_shift = shift;

    r = nai_thread_key_open(&nai_unix.tlocal, nai_unix_tlocal_release);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_thread_key_open() failed");
        goto _fail;
    };

    r = nai_fork_at(nai_unix_fork_pre, 
        nai_unix_fork_parent, nai_unix_fork_child);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_fork_at() failed");
        goto _fail;
    };

#if defined(SIGPIPE)
    if (nai_signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        r = -1;
        goto _fail;
    };
#endif
#if defined(SIGSYS)
    if (nai_signal(SIGSYS, SIG_IGN) == SIG_ERR) {
        r = -1;
        goto _fail;
    };
#endif
#if defined(SIGURG)
    if (nai_signal(SIGURG, nai_unix_io_interrupt_handle) == SIG_ERR) {
        r = -1;
        goto _fail;
    };
#endif

    r = 0;

_fail:
    if (r < 0) {
        nai_unix.error = nai_errno;
    };

    nai_memory_barrier();
    nai_unix.inited = 1;
    return;
};


nai_int_t nai_init()
{
    nai_int_t r;


    if (nai_unix.inited == 0) {
        nai_once(&nai_unix.once, nai_unix_init);
    };
    if (nai_unix.error) {
        nai_errno = nai_unix.error;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


#if (__darwin__)
extern void nai_mapi_term();
#endif


void nai_os_term()
{
    if (nai_unix.inited) {
        nai_unix_tlocal_release(nai_thread_key_get(&nai_unix.tlocal));
        nai_thread_key_close(&nai_unix.tlocal);
    };

#if (__darwin__)
    nai_mapi_term();
#endif

    return;
};



//////////////////////////////////////////////////////////////////////////////
// unix thread local


typedef struct nai_unix_tlocal_s {
    /* common */
    nai_tlocal_t base;

    /* unix private */

} nai_unix_tlocal_t;


#define nai_unix_tlocal_init(t)                         \
    nai_tlocal_init(&(t)->base);                        \


static void nai_unix_tlocal_release(void* h)
{
    nai_int_t r;
    nai_unix_tlocal_t* t = (nai_unix_tlocal_t*)h;


    if (t == 0) {
        return;
    };

    r = nai_tlocal_cleanup(&t->base);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_tlocal_cleanup() failed");
    };

    nai_free(t);
    return;
};


nai_tlocal_t* nai_tlocal_get()
{
    nai_unix_tlocal_t* t;


    if (nai_unix.inited == 0) {
        nai_once(&nai_unix.once, nai_unix_init);
    };
    if (nai_unix.error) {
        nai_errno = nai_unix.error;
        t = 0;
        goto _end;
    };

    t = (nai_unix_tlocal_t*)nai_thread_key_get(&nai_unix.tlocal);
    if (t == 0) {
        t = (nai_unix_tlocal_t*)nai_malloc(sizeof(*t));
        if (t == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "malloc failed");
            goto _end;
        };

        nai_unix_tlocal_init(t);
        nai_thread_key_set(&nai_unix.tlocal, t);
    };

_end:
    return (nai_tlocal_t*)t;
};



//////////////////////////////////////////////////////////////////////////////
// sched priority


#ifndef NZERO
#define NZERO 20
#endif


nai_int_t nai_sched_query_priority(
    nai_sched_priority_t* p, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_int_t sched_class;
    nai_int_t sched_priority;
    nai_int_t sched_priority_min;
    nai_int_t sched_priority_max;
    nai_int_t sched_priority_range;


    if (sched == NAI_SCHED_UNSET) {
        p->sched = SCHED_OTHER;
        p->priority = 0;
        r = 0;
        goto _end;
    };

    if (priority > NAI_PRIORITY_HIGHEST) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (sched) {
    case NAI_SCHED_NORMAL:
        sched_class = SCHED_OTHER;
        sched_priority_max = -NZERO;
#if (__freebsd__) || (__darwin__)
        sched_priority_min = NZERO;     /* linux 19, bsd 20 */
#else
        sched_priority_min = NZERO-1;   /* linux 19, bsd 20 */
#endif
        break;
#if (NAI_HAVE_SCHED_CLASS_PRIORITY)
    case NAI_SCHED_FIFO:
        sched_class = SCHED_FIFO;
        sched_priority_max = sched_get_priority_max(sched_class);
        sched_priority_min = sched_get_priority_min(sched_class);
        break;
    case NAI_SCHED_RR:
        sched_class = SCHED_RR;
        sched_priority_max = sched_get_priority_max(sched_class);
        sched_priority_min = sched_get_priority_min(sched_class);
        break;
#else
    case NAI_SCHED_FIFO:
    case NAI_SCHED_RR:
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
#endif
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    sched_priority_range = sched_priority_max - sched_priority_min;
    switch (priority) {
    case NAI_PRIORITY_NORMAL:
        sched_priority = sched_priority_min + sched_priority_range/2;
        break;
    default:
        if (priority >= 0) {
            sched_priority = sched_priority_min;
            sched_priority += sched_priority_range * 
                (priority - NAI_PRIORITY_LOWEST) / 
                (NAI_PRIORITY_HIGHEST - NAI_PRIORITY_LOWEST);
        } else {
            sched_priority = sched_priority_min;
            if (sched_priority_range < 0) {
                sched_priority += priority;
                if (sched_priority < sched_priority_max) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _end;
                };
            } else {
                sched_priority -= priority;
                if (sched_priority > sched_priority_max) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _end;
                };
            };
        };
        break;
    };

    p->sched = sched_class;
    p->priority = sched_priority;
    r = 1;

_end:
    return r;
};


nai_int_t nai_sigprocmask(nai_int_t op, sigset_t* newset, sigset_t* oldset)
{
    nai_int_t r;

#if (NAI_HAVE_PTHREAD_H)
    nai_int_t n;


    n = pthread_sigmask(op, newset, oldset);
    if (n != 0) {
        nai_errno = n;
        r = -1;
    } else {
        r = 0;
    };

#else

    r = sigprocmask(op, newset, oldset);

#endif

    return r;
}


#endif

