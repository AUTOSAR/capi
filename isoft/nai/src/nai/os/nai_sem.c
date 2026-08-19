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
/// @file       nai_sem.c
/// @brief      
/// @details
/// @date       2021-11-04
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_sem.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include <stdarg.h>


#if defined(_WIN32)


#include "nai/os/nai_system.h"
#include "win/nai_windows.h"

#include <windef.h>
#include <winbase.h>

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )
#define WAIT_ABANDONED      ((STATUS_ABANDONED_WAIT_0 ) + 0 )
#endif


static nai_int_t nai_psult(nai_int_t n)
{
    nai_int_t r;


    if (!n) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_wsult(nai_int_t n)
{
    nai_int_t r;


    switch (n) {
    case WAIT_OBJECT_0:
        r = 0;
        break;
    case WAIT_ABANDONED:
        r = -1;
        break;
    case WAIT_TIMEOUT:
        nai_errno = ETIMEDOUT;
        r = -1;
        break;
    default:
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        break;
    };

    return r;
};


nai_int_t nai_sem_open(nai_sem_t* p, 
    const char* name, uint32_t init, nai_int_t flags, ...)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mode;
    nai_int_t create;
    va_list va;
    nai_wcs_t ws = { .buf = 0 };
    HANDLE h;
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR* sd;
    char sdbuf[SECURITY_DESCRIPTOR_MIN_LENGTH];


    switch (flags & (NAI_O_CREAT|NAI_O_EXCL)) {
    case 0:
    case NAI_O_CREAT:
    case NAI_O_CREAT|NAI_O_EXCL:
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (flags & NAI_O_TMPFILE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create) {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);
    } else {
        mode = 0;
    };

    if (name == 0) {

        if (flags & NAI_SEM_SHARED) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };

        sd = (SECURITY_DESCRIPTOR*)sdbuf;
        InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(sd, -1, 0, 0);
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = sd;
        sa.bInheritHandle = FALSE;

        h = CreateSemaphoreW(&sa, init, INT_MAX, 0);

    } else {

        /* to wchar_t* */
        r = nai_wcs_from_path(&ws, name);
        if (r < 0) {
            goto _end;
        };

        /* try create semaphore */
        if (flags & NAI_O_CREAT) {
            /* */
            sd = (SECURITY_DESCRIPTOR*)sdbuf;
            InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
            SetSecurityDescriptorDacl(sd, -1, 0, 0);
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = sd;
            sa.bInheritHandle = FALSE;

            h = CreateSemaphoreW(&sa, init, INT_MAX, ws.str);
            if (h == 0) {
                ec = GetLastError();
                if (ec != ERROR_ALREADY_EXISTS) {
                    nai_errno = nai_errno_from_oserr(ec);
                    r = -1;
                    goto _end;
                };
                if (flags & NAI_O_EXCL) {
                    nai_errno = EEXIST;
                    r = -1;
                    goto _end;
                };
            };
        } else {
            h = 0;
        };

        /* try open exists */
        if (h == 0) {
            h = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, ws.str);
        };
    };
    if (h == 0) {
        r = nai_psult(0);
        goto _end;
    };

    p->sem = h;
    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_sem_trywait(nai_sem_t* p)
{
    nai_int_t r;


    r = nai_wsult(WaitForSingleObject(p->sem, 0));

    return r;
};


nai_int_t nai_sem_wait(nai_sem_t* p)
{
    nai_int_t r;


    r = nai_wsult(WaitForSingleObject(p->sem, -1));

    return r;
};


nai_int_t nai_sem_timedwait(nai_sem_t* p, uint64_t abstime)
{
    nai_int_t r;
    int64_t span;
    uint64_t now;


    now = nai_tick_to_usec();
    span = abstime - now;
    if (span < 0) {
        span = 0;
    } else {
        span = span / 1000 + !!(span % 1000);
    };

    r = nai_wsult(WaitForSingleObject(p->sem, (uint32_t)span));

    return r;
};


nai_int_t nai_sem_post(nai_sem_t* p)
{
    nai_int_t r;


    r = nai_psult(ReleaseSemaphore(p->sem, 1, 0));

    return r;
};


nai_int_t nai_sem_close(nai_sem_t* p)
{
    nai_int_t r;


    if (p->sem == 0) {
        r = 0;
        goto _end;
    };

    r = nai_psult(CloseHandle(p->sem));
    if (r < 0) {
        goto _end;
    };

    p->sem = NULL;

_end:
    return r;
};


#elif (NAI_HAVE_SEMAPHORE_H)


#include "nai/os/nai_system.h"
#include <time.h>


#if (__darwin__)


typedef struct nai_mach_lock_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_mutex_t lock;
} nai_mach_lock_t;


static nai_mach_lock_t nai_mach_lock = {
    0, 0, NAI_ONCE_INIT
};


static void nai_mach_lock_init()
{
    nai_int_t r;


    r = nai_mutex_open(&nai_mach_lock.lock, 0);
    if (r < 0) {
        nai_mach_lock.error = nai_errno;
    };

    nai_memory_barrier();
    nai_mach_lock.inited = 1;
    return;
};


static void nai_mach_lock_term()
{
    if (nai_mach_lock.inited) {
        nai_mutex_close(&nai_mach_lock.lock);
    };

    return;
};


static nai_mutex_t* nai_mach_sem_lock(nai_sem_t* p)
{
    nai_mutex_t* m;


    if (p->shared) {
        m = &p->data.mtx;
    } else {
        assert(nai_mach_lock.inited);
        m = &nai_mach_lock.lock;
    };

    return m;
};


static nai_int_t nai_mach_sem_init(
    nai_sem_t* p, nai_int_t flags, uint32_t init)
{
    nai_int_t r;
    nai_int_t ec;


    p->shared = !!(flags & NAI_SEM_SHARED);
    p->data.count = init;

    if (p->shared) {
        r = nai_mutex_open(&p->data.mtx, 1);
        if (r < 0) {
            goto _end;
        };

        r = nai_cond_open(&p->data.cv, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_mutex_close(&p->data.mtx);
            nai_errno = ec;
            goto _end;
        };
    } else {
        if (nai_mach_lock.inited == 0) {
            nai_once(&nai_mach_lock.once, nai_mach_lock_init);
        };
        if (nai_mach_lock.error) {
            nai_errno = nai_mach_lock.error;
            r = -1;
            goto _end;
        };

        r = nai_cond_open(&p->data.cv, 1);
        if (r < 0) {
            goto _end;
        };
    };


_end:
    return r;
};


static nai_int_t nai_mach_sem_trywait(nai_sem_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_mutex_t* m;


    m = nai_mach_sem_lock(p);
    r = nai_mutex_lock(m);
    if (r < 0) {
        goto _end;
    };

    if (p->data.count > 0) {
        p->data.count --;
        r = 0;
    } else {
        r = -1;
    };

    n = nai_mutex_unlock(m);
    if (n < 0) {
        /* fixme: ignored error */
        ;
    };

    if (r < 0) {
        nai_errno = EAGAIN;
    };

_end:
    return r;
};


static nai_int_t nai_mach_sem_wait(nai_sem_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_int_t wait;
    nai_int_t signal;
    nai_mutex_t* m;


    m = nai_mach_sem_lock(p);
    r = nai_mutex_lock(m);
    if (r < 0) {
        goto _end;
    };

    wait = 0;
    for (;;) {
        if (p->data.count > 0) {
            p->data.count --;
            r = 0;
            signal = wait && (p->data.count > 0);
            break;
        };

        r = nai_cond_wait(&p->data.cv, m);
        if (r < 0) {
            ec = nai_errno;
            break;
        };

        wait = 1;
    };

    n = nai_mutex_unlock(m);
    if (n < 0) {
        /* fixme: ignored error */
        ;
    };

    if (r < 0) {
        nai_errno = ec;
        goto _end;
    };

    if (signal) {
        n = nai_cond_signal(&p->data.cv);
        if (n < 0) {
            /* fixme: ignored error */
            ;
        };
    };

_end:
    return r;
};


static nai_int_t nai_mach_sem_timedwait(nai_sem_t* p, uint64_t abstime)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_int_t wait;
    nai_int_t signal;
    nai_mutex_t* m;


    m = nai_mach_sem_lock(p);
    r = nai_mutex_lock(m);
    if (r < 0) {
        goto _end;
    };

    wait = 0;
    for (;;) {
        if (p->data.count > 0) {
            p->data.count --;
            r = 0;
            signal = wait && (p->data.count > 0);
            break;
        };

        r = nai_cond_timedwait(&p->data.cv, m, abstime);
        if (r < 0) {
            ec = nai_errno;
            break;
        };

        wait = 1;
    };

    n = nai_mutex_unlock(m);
    if (n < 0) {
        /* fixme: ignored error */
        ;
    };

    if (r < 0) {
        nai_errno = ec;
        goto _end;
    };

    if (signal) {
        n = nai_cond_signal(&p->data.cv);
        if (n < 0) {
            /* fixme: ignored error */
            ;
        };
    };


_end:
    return r;
};


static nai_int_t nai_mach_sem_post(nai_sem_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t signal;
    nai_mutex_t* m;


    m = nai_mach_sem_lock(p);
    r = nai_mutex_lock(m);
    if (r < 0) {
        goto _end;
    };

    p->data.count ++;
    signal = p->data.count <= 1;

    n = nai_mutex_unlock(m);
    if (n < 0) {
        /* fixme: ignored error */
        ;
    };

    if (signal) {
        n = nai_cond_signal(&p->data.cv);
        if (n < 0) {
            /* fixme: ignored error */
            ;
        };
    };

_end:
    return r;
};


static nai_int_t nai_mach_sem_destroy(nai_sem_t* p)
{
    nai_int_t r;


    r = nai_cond_close(&p->data.cv);

    return r;
};


#endif



nai_int_t nai_sem_open(nai_sem_t* p, 
    const char* name, uint32_t init, nai_int_t flags, ...)
{
    nai_int_t r;
    nai_int_t mode;
    nai_int_t create;
    va_list va;
    sem_t* s;


    switch (flags & (NAI_O_CREAT|NAI_O_EXCL)) {
    case 0:
    case NAI_O_CREAT:
    case NAI_O_CREAT|NAI_O_EXCL:
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (flags & NAI_O_TMPFILE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create) {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);
    } else {
        mode = 0;
    };

    if (name == 0) {
#if (__darwin__)
        r = nai_mach_sem_init(p, flags, init);
        if (r < 0) {
            goto _end;
        };
#else
        s = &p->data;
        r = sem_init(s, flags & NAI_SEM_SHARED, init);
        if (r < 0) {
            goto _end;
        };

        p->shared = !!(flags & NAI_SEM_SHARED);
#endif

        p->type = 1;

    } else {
        s = sem_open(name, flags & ~0xfff00000, mode, init);
        if (s == SEM_FAILED) {
            r = -1;
            goto _end;
        };

        p->shared = 0;
        p->type = 2;
        p->sem = s;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_sem_trywait(nai_sem_t* p)
{
    nai_int_t r;
    sem_t* s;


    switch (p->type) {
    case 1:
#if (__darwin__)
        r = nai_mach_sem_trywait(p);
        goto _end;
#else
        s = &p->data;
        break;
#endif
    case 2:
        s = (sem_t*)p->sem;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    do {
        r = sem_trywait(s);
    } while (r < 0 && nai_errno == EINTR);

_end:
    return r;
};


nai_int_t nai_sem_wait(nai_sem_t* p)
{
    nai_int_t r;
    sem_t* s;


    switch (p->type) {
    case 1:
#if (__darwin__)
        r = nai_mach_sem_wait(p);
        goto _end;
#else
        s = &p->data;
        break;
#endif
    case 2:
        s = (sem_t*)p->sem;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    do {
        r = sem_wait(s);
    } while (r < 0 && nai_errno == EINTR);

_end:
    return r;
};


nai_int_t nai_sem_timedwait(nai_sem_t* p, uint64_t abstime)
{
    nai_int_t r;
    sem_t* s;
#if (NAI_HAVE_SEM_TIMEDWAIT)
    struct timespec ts;
    uint64_t realtime;
#endif


    switch (p->type) {
    case 1:
#if (__darwin__)
        r = nai_mach_sem_timedwait(p, abstime);
        goto _end;
#else
        s = &p->data;
        break;
#endif
    case 2:
        s = (sem_t*)p->sem;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#if (NAI_HAVE_SEM_TIMEDWAIT)
    realtime = abstime - nai_tick_to_usec();
    realtime = nai_time() + realtime;
    ts.tv_sec = realtime / 1000000;
    ts.tv_nsec = realtime % 1000000 * 1000;

    do {
        r = sem_timedwait(s, &ts);
    } while (r < 0 && nai_errno == EINTR);
#else
    (void)s;
    (void)abstime;
    nai_errno = ENOTSUP;
    r = -1;
#endif

_end:
    return r;
};


nai_int_t nai_sem_post(nai_sem_t* p)
{
    nai_int_t r;
    sem_t* s;


    switch (p->type) {
    case 1:
#if (__darwin__)
        r = nai_mach_sem_post(p);
#else
        s = &p->data;
        r = sem_post(s);
#endif
        break;
    case 2:
        s = (sem_t*)p->sem;
        r = sem_post(s);
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


_end:
    return r;
};


nai_int_t nai_sem_close(nai_sem_t* p)
{
    nai_int_t r;
    sem_t* s;


    switch (p->type) {
    case 1:
#if (__darwin__)
        r = nai_mach_sem_destroy(p);
#else
        s = &p->data;
        r = sem_destroy(s);
#endif
        break;
    case 2:
        s = (sem_t*)p->sem;
        r = sem_close(s);
        break;
    case 0:
        r = 0;
        goto _end;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (r < 0) {
        goto _end;
    };

    p->type = 0;
    r = 0;

_end:
    return r;
};


#else

#error "nai_sem is not implemented, unsupported platform!"

#endif


void nai_sem_term()
{
#if (NAI_HAVE_SEMAPHORE_H)
#if (__darwin__)
    nai_mach_lock_term();
#endif
#endif

    return;
};

