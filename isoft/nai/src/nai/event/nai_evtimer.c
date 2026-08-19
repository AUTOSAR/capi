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
/// @file       nai_evtimer.c
/// @brief      
/// @details
/// @date       2023-03-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evtimer.h"
#include "nai_evloop.h"
#include "nai/runtime/nai_errno.h"



#if (NAI_HAVE_TIMERFD_CREATE)


#include "nai/os/nai_file.h"
#include <sys/timerfd.h>


static nai_int_t nai_evtimer_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    uint64_t exp;
    nai_evtimer_t* p;


    (void)events;

    p = (nai_evtimer_t*)e;
    if (!p->et) {
        r = nai_file_read(p->fd, &exp, sizeof(exp));
    } else {
        r = 0;
    };

    return r;
};


nai_int_t nai_evtimer_init(nai_evtimer_t* p)
{
    nai_evnode_init(&p->ev);
    p->fd = NAI_FD_INVALID;
    p->et = 0;
    return 0;
};


nai_int_t nai_evtimer_open(nai_evtimer_t* p, nai_evbase_ops_t* ops, void* e)
{
    nai_int_t r;
    nai_int_t oflags;
    nai_fd_t fd;

#if !defined(TFD_CLOEXEC)
    nai_int_t ec;
    oflags = 0;
#else
    oflags = TFD_CLOEXEC;
#endif


    fd = timerfd_create(CLOCK_MONOTONIC, oflags);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

#if !defined(TFD_CLOEXEC)
    r = nai_file_set_cloexec(fd, 1);
    if (r < 0) {
        ec = nai_errno;
        nai_file_close(fd);
        nai_errno = ec;
        goto _end;
    };
#endif

    p->et = (ops->feature_m | ops->feature_e) & NAI_EV_FEAT_EDGE;
    p->fd = fd;
    p->ent.h = &p->ev;
    p->ent.key = -1;
    p->ent.mt = 0;
    p->ent.mail = 0;
    p->ent.catching = 0;
    p->ent.priority = 0;
    p->ent.events = 0;
    p->ent.sig = 0;
    p->ent.signext = 0;
    p->ent.ttype = 0;
    p->ent.timer.value = 0;
    p->ev.ent = &p->ent;

    nai_evnode_set_event(&p->ev, NAI_EV_SET, NAI_EV_READ);
    nai_evnode_set_fd(&p->ev, fd, NAI_FD_TYPE_DEVC);
    nai_evnode_set_cb(&p->ev, (nai_evnode_cb_f)nai_evtimer_handle);

    r = ops->add(e, &p->ev, p->ev.st.seted);


_end:
    return r;
};


nai_int_t nai_evtimer_close(nai_evtimer_t* p)
{
    nai_int_t r;


    if (p->fd != NAI_FD_INVALID) {
        nai_file_close(p->fd);
        p->fd = NAI_FD_INVALID;
    };

    nai_evnode_init(&p->ev);
    r = 0;

    return r;
};


nai_int_t nai_evtimer_set(nai_evtimer_t* p, uint64_t usec)
{
    nai_int_t r;
    struct itimerspec its;


    its.it_value.tv_sec = usec / (1000 * 1000);
    its.it_value.tv_nsec = usec % (1000 * 1000) * 1000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    r = timerfd_settime(p->fd, TFD_TIMER_ABSTIME, &its, 0);

    return r;
};



#elif (NAI_HAVE_TIMER_CREATE)


#include "nai/runtime/nai_log.h"
#include <signal.h>


typedef struct nai_evtimer_signal_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
} nai_evtimer_signal_t;


static nai_evtimer_signal_t nai_evtimer_signal = {
    0, 0, NAI_ONCE_INIT
};


static void nai_evtimer_signal_handle(nai_int_t sig, siginfo_t* si, void* ud)
{
    nai_int_t r;
    nai_evtimer_t* p;


    (void)sig;
    (void)ud;

    p = (nai_evtimer_t*)si->si_value.sival_ptr;
    r = nai_evbase_signal(&p->base);
    if (r < 0) {
        /* fixme: ignored error */
        ;
    };

    return;
};


static void nai_evtimer_signal_init()
{
    nai_int_t r;
    nai_int_t ec;
    struct sigaction sa;


    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = nai_evtimer_signal_handle;
    sigemptyset(&sa.sa_mask);
    r = sigaction(SIGALRM, &sa, 0);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "sigaction() failed");
        goto _fail;
    };

    r = 0;

_fail:
    if (r < 0) {
        ec = nai_errno;
        nai_evtimer_signal.error = ec;
    };

    nai_memory_barrier();
    nai_evtimer_signal.inited = 1;
    return;
};


nai_int_t nai_evtimer_init(nai_evtimer_t* p)
{
    nai_evbase_init(&p->base);
    p->tid = 0;
    return 0;
};


nai_int_t nai_evtimer_open(nai_evtimer_t* p, nai_evbase_ops_t* ops, void* e)
{
    nai_int_t r;
    timer_t tid;
    struct sigevent se;

#if defined(SIGEV_KEVENT)
    nai_aio_port_t* port;
#endif

    if (nai_evtimer_signal.inited == 0) {
        nai_once(&nai_evtimer_signal.once, nai_evtimer_signal_init);
    };
    if (nai_evtimer_signal.error) {
        nai_errno = nai_evtimer_signal.error;
        r = -1;
        goto _end;
    };

#if defined(SIGEV_KEVENT)
    port = ops->get_port(e);
    if (port) {
        se.sigev_notify = SIGEV_KEVENT;
        se.sigev_notify_kqueue = port->fd;
        se.sigev_value.sival_ptr = p;
    } else {
#endif
        se.sigev_notify = SIGEV_SIGNAL;
        se.sigev_signo = SIGALRM;
        se.sigev_value.sival_ptr = p;
#if defined(SIGEV_KEVENT)
    };
#endif

    r = timer_create(CLOCK_MONOTONIC, &se, &tid);
    if (r < 0) {
        goto _end;
    };

    p->base.ops = ops;
    p->base.ud = e;
    p->tid = tid;
    r = 0;

_end:
    return r;
};


nai_int_t nai_evtimer_close(nai_evtimer_t* p)
{
    nai_int_t r;


    if (p->tid != 0) {
        timer_delete(p->tid);
        p->tid = 0;
    };

    nai_evbase_init(&p->base);
    r = 0;

    return r;
};


nai_int_t nai_evtimer_set(nai_evtimer_t* p, uint64_t usec)
{
    nai_int_t r;
    struct itimerspec its;


    its.it_value.tv_sec = usec / (1000 * 1000);
    its.it_value.tv_nsec = usec % (1000 * 1000) * 1000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    r = timer_settime(p->tid, TIMER_ABSTIME, &its, 0);

    return r;
};



#elif (NAI_HAVE_TIMER_WIN32)


#include "nai/os/win/nai_windows.h"
#include <windef.h>
#include <winbase.h>


static void WINAPI nai_evtimer_handle(void* data, BOOLEAN timedout)
{
    nai_int_t r;
    nai_evtimer_t* p;


    (void)timedout;

    p = (nai_evtimer_t*)data;
    r = nai_evbase_signal(&p->base);
    if (r < 0) {
        /* fixme: ignored error */
        ;
    };

    return;
};


nai_int_t nai_evtimer_init(nai_evtimer_t* p)
{
    nai_evbase_init(&p->base);
    p->timer = 0;
    p->wait = 0;
    return 0;
};


nai_int_t nai_evtimer_open(nai_evtimer_t* p, nai_evbase_ops_t* ops, void* e)
{
    nai_int_t r;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };


    p->timer = CreateWaitableTimer(&sa, 0, 0);
    if (p->timer == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    if (!RegisterWaitForSingleObject(
        &p->wait, p->timer, nai_evtimer_handle, (void*)(intptr_t)p, -1,
        WT_EXECUTEINWAITTHREAD)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    p->base.ops = ops;
    p->base.ud = e;
    r = 0;

_end:
    return r;
};


nai_int_t nai_evtimer_close(nai_evtimer_t* p)
{
    nai_int_t r;


    if (p->wait) {
        UnregisterWaitEx(p->wait, INVALID_HANDLE_VALUE);
        p->wait = 0;
    };
    if (p->timer) {
        CloseHandle(p->timer);
        p->timer = 0;
    };

    nai_evbase_init(&p->base);
    r = 0;

    return r;
};


nai_int_t nai_evtimer_set(nai_evtimer_t* p, uint64_t usec)
{
    nai_int_t r;
    uint64_t now;
    LARGE_INTEGER expr;


    if (usec) {
        now = nai_tickcache_to_usec();
        if (usec < now) {
            usec = now;
        };
        expr.QuadPart = -10 * (usec-now);
        r = SetWaitableTimer(p->timer, &expr, 0, 0, 0, 0);
    } else {
        r = CancelWaitableTimer(p->timer);
    };
    if (!r) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


#else



nai_int_t nai_evtimer_init(nai_evtimer_t* p)
{
    (void)p;

    return 0;
};


nai_int_t nai_evtimer_open(nai_evtimer_t* p, nai_evbase_ops_t* ops, void* e)
{
    (void)p;
    (void)ops;
    (void)e;

    return 0;
};


nai_int_t nai_evtimer_close(nai_evtimer_t* p)
{
    (void)p;

    return 0;
};


nai_int_t nai_evtimer_set(nai_evtimer_t* p, uint64_t usec)
{
    (void)p;
    (void)usec;

    return 0;
};


#endif


