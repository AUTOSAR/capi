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
/// @file       nai_thread.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/os/nai_task.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_proc.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"


#if defined(_WIN32)


#include "win/nai_windows.h"
#include <processthreadsapi.h>
#include <synchapi.h>
#include <handleapi.h>

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


typedef struct nai_thread_priority_s {
    nai_int_t inherit;
    nai_int_t priority;
} nai_thread_priority_t;


static nai_int_t nai_win_thread_priority(
    nai_thread_priority_t* p, nai_int_t sched ,nai_int_t priority)
{
    nai_int_t r;


    if (sched == NAI_SCHED_UNSET) {
        p->inherit = 1;
        p->priority = 0;
        r = 0;
        goto _end;
    };

    if (sched != NAI_SCHED_NORMAL) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (priority < 0) {
        priority = -priority;
    };
    if (priority < NAI_PRIORITY_LOWEST || 
        priority > NAI_PRIORITY_HIGHEST) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (priority < NAI_PRIORITY_LOWER) {
        p->priority = THREAD_PRIORITY_LOWEST;
    } else if (priority < NAI_PRIORITY_NORMAL) {
        p->priority = THREAD_PRIORITY_BELOW_NORMAL;
    } else if (priority < NAI_PRIORITY_HIGHER) {
        p->priority = THREAD_PRIORITY_NORMAL;
    } else if (priority < NAI_PRIORITY_HIGHEST) {
        p->priority = THREAD_PRIORITY_ABOVE_NORMAL;
    } else {
        p->priority = THREAD_PRIORITY_HIGHEST;
    };

    p->inherit = 0;
    r = 1;

_end:
    return r;
};


static nai_int_t nai_win_thread_affinity(size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t units;
    nai_int_t group;


    units = (nai_int_t)(size*8 + NAI_CPUMASK_BITS - 1) / NAI_CPUMASK_BITS;
    assert(units >= 1);

    /* find group, not support more than one group */
    group = -1;
    for (n = 0; n < units; n ++) {
        if (!set->bits[n]) {
            continue;
        };

        if (group >= 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        group = n;
    };

    r = group;

_end:
    return r;
};



///////////////////////////////////////////////////////////////////////////
// thread attr


nai_int_t nai_thread_attr_open(nai_thread_attr_t* a)
{
    nai_thread_attr_init(a);
    return 0;
};


nai_int_t nai_thread_attr_set_stacksize(
    nai_thread_attr_t* a, size_t stack_size)
{
    nai_int_t r;

    a->stack_size = stack_size;
    r = 0;

    return r;
};


nai_int_t nai_thread_attr_set_guardsize(
    nai_thread_attr_t* a, size_t guard_size)
{
    nai_int_t r;

    a->guard_size = guard_size;
    r = 0;

    return r;
};


nai_int_t nai_thread_attr_set_sched(
    nai_thread_attr_t* a, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_thread_priority_t p;


    r = nai_win_thread_priority(&p, sched, priority);
    if (r < 0) {
        goto _end;
    };

    a->sched = sched;
    a->priority = priority;
    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_attr_set_affinity(
    nai_thread_attr_t* a, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;


    if (set) {
        r = nai_win_thread_affinity(size, set);
        if (r < 0) {
            goto _end;
        };
    };

    a->cpu.size = size;
    a->cpu.set = set;
    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_attr_close(nai_thread_attr_t* a)
{
    nai_thread_attr_init(a);
    return 0;
};



///////////////////////////////////////////////////////////////////////////
// thread


nai_int_t nai_thread_create(nai_thread_t* t, 
    const nai_thread_attr_t* a, nai_thread_entry_f ent, void* arg)
{
    nai_int_t r;
    nai_int_t ec;
    HANDLE h;
    DWORD flags = 0;
    nai_thread_priority_t p;


    if (a != 0) {
        r = nai_win_thread_priority(&p, a->sched, a->priority);
        if (r < 0) {
            goto _end;
        };

        if (p.inherit == 0) {
            flags |= CREATE_SUSPENDED;
        };
        if (a->cpu.set) {
            flags |= CREATE_SUSPENDED;
        };
    };

    h = CreateThread(0, a ? a->stack_size : 0, ent, arg, flags, 0);
    if (h == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    if (flags & CREATE_SUSPENDED) {
        if (p.inherit == 0) {
            r = SetThreadPriority(h, p.priority);
            if (r == 0) {
                nai_errno = nai_errno_from_oserr(GetLastError());
                r = -1;
                goto _fail;
            };
        };
        if (a->cpu.set) {
            r = nai_thread_set_affinity(&h, a->cpu.size, a->cpu.set);
            if (r < 0) {
                goto _fail;
            };
        };

        r = ResumeThread(h);
        if (r == 0) {
            goto _fail;
        };
    };

    t[0] = h;
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno_from_oserr(GetLastError());
    TerminateThread(h, 0);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_thread_set_sched(
    nai_thread_t* t, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_thread_priority_t p;


    r = nai_win_thread_priority(&p, sched, priority);
    if (r <= 0) {
        goto _end;
    };

    r = SetThreadPriority(*t, p.priority);
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_get_affinity(
    nai_thread_t* t, size_t size, nai_cpuset_t* set)
{
    nai_int_t r;
    nai_int_t units;
    GROUP_AFFINITY aff;


    if (set == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    units = (nai_int_t)(size*8 + NAI_CPUMASK_BITS - 1) / NAI_CPUMASK_BITS;
    assert(units > 0);


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    if (nai_wapi.GetThreadGroupAffinity) {
        if (!nai_wapi.GetThreadGroupAffinity(*t, &aff)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        if (units <= (nai_int_t)aff.Group) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        nai_cpuset_zero_s(size, set);
        set->bits[aff.Group] = aff.Mask;

    } else {
        if (units <= 1) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        /* fixme: wrong api */
        if (!GetProcessAffinityMask(0, &set->bits[0], 0)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        nai_memset(&set->bits[1], 0, (units-1) * sizeof(set->bits[0]));
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_set_affinity(
    nai_thread_t* t, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;
    nai_int_t group;
    GROUP_AFFINITY aff;


    if (set == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_win_thread_affinity(size, set);
    if (r < 0) {
        goto _end;
    };


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    group = r;
    if (nai_wapi.SetThreadGroupAffinity) {
        if (group < 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        aff.Mask = set->bits[group];
        aff.Group = group;
        if (!nai_wapi.SetThreadGroupAffinity(*t, &aff, 0)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

    } else {
        if (group != 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if (!SetThreadAffinityMask(*t, set->bits[group])) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_join(nai_thread_t* t)
{
    nai_int_t r;

    if (t[0]) {
        r = nai_wsult(WaitForSingleObject(*t, -1));
        if (r < 0) {
            goto _end;
        };

        CloseHandle(*t);
        t[0] = 0;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_detach(nai_thread_t* t)
{
    nai_int_t r;

    if (t[0]) {
        r = nai_psult(CloseHandle(*t));
        if (r < 0) {
            goto _end;
        };

        t[0] = 0;
    };

    r = 0;

_end:
    return r;
};


nai_tid_t nai_thread_id()
{
    return GetCurrentThreadId();
};



///////////////////////////////////////////////////////////////////////////
// thread storage


nai_int_t nai_thread_key_open(nai_thread_key_t* t, nai_thread_key_free_f cb)
{
    nai_int_t r;
    DWORD k;


    k = TlsAlloc();
    if (k == NAI_THREAD_KEY_NIL) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    t[0] = k;
    r = 0;

_end:
    return r;
};


nai_int_t nai_thread_key_set(nai_thread_key_t* t, void* value)
{
    return nai_psult(TlsSetValue(*t, value));
};


void* nai_thread_key_get(nai_thread_key_t* t)
{
    return TlsGetValue(*t);
};


nai_int_t nai_thread_key_close(nai_thread_key_t* t)
{
    nai_int_t r;


    if (t[0] != NAI_THREAD_KEY_NIL) {
        r = nai_psult(TlsFree(*t));
        if (r < 0) {
            goto _end;
        };

        t[0] = NAI_THREAD_KEY_NIL;
    };

_end:
    return 0;
};



///////////////////////////////////////////////////////////////////////////
// mutex


nai_int_t nai_mutex_open(nai_mutex_t* l, nai_int_t shared)
{
    nai_int_t r;
    nai_mutex_t h;


    if (shared) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    h = (nai_mutex_t)nai_malloc(sizeof(CRITICAL_SECTION));
    if (h == 0) {
        nai_errno = ENOMEM;
        r = -1;
        goto _end;
    };

    InitializeCriticalSectionAndSpinCount(h, 300);
    l[0] = h;
    r = 0;

_end:
    return r;
};


nai_int_t nai_mutex_trylock(nai_mutex_t* l)
{
    nai_int_t r;


    if (!TryEnterCriticalSection(*l)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_mutex_lock(nai_mutex_t* l)
{
    EnterCriticalSection(*l);
    return 0;
};


nai_int_t nai_mutex_unlock(nai_mutex_t* l)
{
    LeaveCriticalSection(*l);
    return 0;
};


nai_int_t nai_mutex_close(nai_mutex_t* l)
{
    if (l[0]) {
        DeleteCriticalSection(*l);
        nai_free(*l);
        l[0] = 0;
    };

    return 0;
};



///////////////////////////////////////////////////////////////////////////
// cond


nai_int_t nai_cond_open(nai_cond_t* c, nai_int_t shared)
{
    nai_int_t r;
    HANDLE h;


    if (shared) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    h = CreateEvent(0, 0, 0, 0);
    if (h == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    c[0] = h;
    r = 0;

_end:
    return r;
};


nai_int_t nai_cond_wait(nai_cond_t* c, nai_mutex_t* m)
{
    nai_int_t r;


    r = nai_mutex_unlock(m);
    if (r < 0) {
        goto _end;
    }

    r = nai_wsult(WaitForSingleObject(*c, -1));
    if (r < 0) {
        goto _end;
    }

    r = nai_mutex_lock(m);
    if (r < 0) {
        goto _end;
    }

    r = 0;

_end:
    return r;
};


nai_int_t nai_cond_timedwait(nai_cond_t* c, nai_mutex_t* m, uint64_t abstime)
{
    nai_int_t r;
    uint64_t now;
    int64_t span;


    r = nai_mutex_unlock(m);
    if (r < 0) {
        goto _end;
    }

    now = nai_tick_to_usec();
    span = abstime - now;
    if (span < 0) {
        span = 0;
    } else {
        span = span / 1000 + !!(span % 1000);
        if (span > UINT32_MAX) {
            span = UINT32_MAX;
        };
    };
    r = nai_wsult(WaitForSingleObject(*c, (uint32_t)span));
    if (r < 0) {
        if (nai_errno != ETIMEDOUT) {
            goto _end;
        };

        r = nai_mutex_lock(m);
        if (r < 0) {
            goto _end;
        }

        nai_errno = ETIMEDOUT;
        r = -1;

    } else {

        r = nai_mutex_lock(m);
        if (r < 0) {
            goto _end;
        }

        r = 0;
    };

_end:
    return r;
};


nai_int_t nai_cond_signal(nai_cond_t* c)
{
    return nai_psult(SetEvent(*c));
};


nai_int_t nai_cond_close(nai_cond_t* c)
{
    nai_int_t r;


    if (c[0]) {
        r = nai_psult(CloseHandle(*c));
        if (r < 0) {
            goto _end;
        };

        c[0] = 0;
    };

    r = 0;

_end:
    return r;
};



///////////////////////////////////////////////////////////////////////////
// spinlock


nai_int_t nai_spin_open(nai_spin_t* l, nai_int_t shared)
{
    nai_int_t r;
    nai_spin_t h;


    if (shared) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    h = (nai_spin_t)nai_malloc(sizeof(CRITICAL_SECTION));
    if (h == 0) {
        nai_errno = ENOMEM;
        r = -1;
        goto _end;
    };

    InitializeCriticalSectionAndSpinCount(h, 300);
    l[0] = h;
    r = 0;

_end:
    return r;
};


nai_int_t nai_spin_trylock(nai_spin_t* l)
{
    nai_int_t r;


    if (!TryEnterCriticalSection(*l)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_spin_lock(nai_spin_t* l)
{
    EnterCriticalSection(*l);
    return 0;
};


nai_int_t nai_spin_unlock(nai_spin_t* l)
{
    LeaveCriticalSection(*l);
    return 0;
};


nai_int_t nai_spin_close(nai_spin_t* l)
{
    if (l[0]) {
        DeleteCriticalSection(*l);
        nai_free(*l);
        l[0] = 0;
    };

    return 0;
};



///////////////////////////////////////////////////////////////////////////
// rwlock


typedef struct nai_rwlock_s {
    union {
        SRWLOCK rwlock;
        struct {
            nai_mutex_t wlock;
            nai_mutex_t rlock;
            nai_cond_t cond;
            nai_int_t wait;
            nai_int_t read;
            nai_int_t write;
        };
    };
} nai_rwlock_h;



nai_int_t nai_rwlock_open(nai_rwlock_t* p, nai_int_t shared)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rwlock_h* l;


    if (shared) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    l = (nai_rwlock_h*)nai_malloc(sizeof(*l));
    if (l == 0) {
        r = -1;
        goto _end;
    };

    if (nai_wapi.InitializeSRWLock) {
        nai_wapi.InitializeSRWLock(&l->rwlock);
    } else {

        l->wait = 0;
        l->read = 0;
        l->write = 0;
        nai_cond_init(&l->cond);
        nai_mutex_init(&l->rlock);
        nai_mutex_init(&l->wlock);

        r = nai_cond_open(&l->cond, 0);
        if (r < 0) {
            goto _fail;
        };

        r = nai_mutex_open(&l->rlock, 0);
        if (r < 0) {
            goto _fail;
        };

        r = nai_mutex_open(&l->wlock, 0);
        if (r < 0) {
            goto _fail;
        };
    };


    p[0] = l;
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_cond_close(&l->cond);
    nai_mutex_close(&l->rlock);
    nai_mutex_close(&l->wlock);
    nai_free(l);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_rwlock_rd_trylock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        r = nai_wapi.TryAcquireSRWLockShared(&l->rwlock);
        if (!r) {
            nai_errno = EBUSY;
            r = -1;
            goto _end;
        };

        r = 0;
    } else {

        while (1) {
            r = nai_mutex_trylock(&l->wlock);
            if (r >= 0) {
                break;
            };

            ec = nai_errno;
            if (ec != EBUSY) {
                goto _end;
            };

            if (l->write) {
                nai_errno = EBUSY;
                r = -1;
                goto _end;
            };
        };

        r = nai_mutex_lock(&l->rlock);
        if (r < 0) {
            goto _fail;
        };

        l->read ++;

        r = nai_mutex_unlock(&l->wlock);
        if (r < 0) {
            goto _term;
        };

        r = nai_mutex_unlock(&l->wlock);
        if (r < 0) {
            goto _term;
        };
    };

_end:
    return r;

_term:
    /* unrecoverable */
    goto _end;

_fail:
    ec = nai_errno;
    nai_mutex_unlock(&l->wlock);
    nai_errno = ec;
    goto _end;

};


nai_int_t nai_rwlock_rd_lock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        nai_wapi.AcquireSRWLockShared(&l->rwlock);
    } else {

        r = nai_mutex_lock(&l->wlock);
        if (r < 0) {
            goto _end;
        };

        r = nai_mutex_lock(&l->rlock);
        if (r < 0) {
            goto _fail;
        };

        l->read ++;

        r = nai_mutex_unlock(&l->wlock);
        if (r < 0) {
            goto _term;
        };

        r = nai_mutex_unlock(&l->wlock);
        if (r < 0) {
            goto _term;
        };
    };

_end:
    return r;

_term:
    /* unrecoverable */
    goto _end;

_fail:
    ec = nai_errno;
    nai_mutex_unlock(&l->wlock);
    nai_errno = ec;
    goto _end;

};


nai_int_t nai_rwlock_rd_unlock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        nai_wapi.ReleaseSRWLockShared(&l->rwlock);
    } else {

        r = nai_mutex_lock(&l->rlock);
        if (r < 0) {
            goto _end;
        };

        l->read --;

        if (l->wait) {
            nai_cond_signal(&l->cond);
        };

        r = nai_mutex_unlock(&l->rlock);
        if (r < 0) {
            goto _term;
        };
    };

_end:
    return r;

_term:
    /* unrecoverable */
    goto _end;
};


nai_int_t nai_rwlock_wr_trylock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        r = nai_wapi.TryAcquireSRWLockExclusive(&l->rwlock);
        if (!r) {
            nai_errno = EBUSY;
            r = -1;
            goto _end;
        };

        r = 0;
    } else {

        r = nai_mutex_trylock(&l->wlock);
        if (r < 0) {
            goto _end;
        };

        if (l->read <= 0) {
            r = 0;
            goto _end;
        };

        r = nai_mutex_unlock(&l->wlock);
        if (r < 0) {
            goto _term;
        };

        nai_errno = EBUSY;
        r = -1;
    };

_end:
    return r;

_term:
    /* unrecoverable */
    goto _end;
};


nai_int_t nai_rwlock_wr_lock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        nai_wapi.AcquireSRWLockExclusive(&l->rwlock);
        r = 0;
    } else {

        r = nai_mutex_lock(&l->wlock);
        if (r < 0) {
            goto _end;
        };

        r = nai_mutex_lock(&l->rlock);
        if (r < 0) {
            goto _fail;
        };

        if (l->read > 0) {
            l->wait = 1;

            do {
                nai_cond_wait(&l->cond, &l->rlock);
            } while (l->read > 0);

            l->wait = 0;
        };

        r = nai_mutex_unlock(&l->rlock);
        if (r < 0) {
            goto _term;
        };

        l->write = 1;
    };

_end:
    return r;

_term:
    /* unrecoverable */
    goto _end;

_fail:
    ec = nai_errno;
    nai_mutex_unlock(&l->wlock);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_rwlock_wr_unlock(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (nai_wapi.InitializeSRWLock) {
        nai_wapi.ReleaseSRWLockExclusive(&l->rwlock);
        r = 0;
    } else {
        l->write = 0;
        r = nai_mutex_unlock(&l->wlock);
    };

    return r;
};


nai_int_t nai_rwlock_close(nai_rwlock_t* p)
{
    nai_int_t r;
    nai_rwlock_h* l;


    l = *(nai_rwlock_h**)p;
    if (l != 0) {
        nai_cond_close(&l->cond);
        nai_mutex_close(&l->rlock);
        nai_mutex_close(&l->wlock);
        nai_free(l);
        p[0] = 0;
    };

    r = 0;

    return r;
};



///////////////////////////////////////////////////////////////////////////
// once


nai_int_t nai_once(nai_once_t* p, nai_once_entry_f once)
{
    nai_int_t r;
    nai_int_t stat;


    if (p == 0 || once == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    }

    while (1) {
        stat = nai_atomic32_add(p, 0); /* MBR fence */
        if (stat == 2) { // done
            break;
        };
        if (stat == 1) { // waiting
            nai_yield();
            continue;
        };

        // 
        assert(stat == 0);
        if (nai_atomic32_cas(p, 0, 1) != 0) {
            continue;
        };

        // run
        once();

        // done
        p[0] = 2;
        break;
    };

    r = 0;

_end:
    return r;
};



///////////////////////////////////////////////////////////////////////////
// atomic


#if (defined(_MSC_VER) && _MSC_VER >= 1400)
#if !defined(InterlockedIncrement)
#define InterlockedIncrement _InterlockedIncrement
#endif
#if !defined(InterlockedDecrement)
#define InterlockedDecrement _InterlockedDecrement
#endif
#if !defined(InterlockedExchangeAdd)
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#endif
#if !defined(InterlockedCompareExchange)
#define InterlockedCompareExchange _InterlockedCompareExchange
#endif
#if !defined(InterlockedIncrement64)
#define InterlockedIncrement64 _InterlockedIncrement64
#endif
#if !defined(InterlockedDecrement64)
#define InterlockedDecrement64 _InterlockedDecrement64
#endif
#if !defined(InterlockedExchangeAdd64)
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#endif
#if !defined(InterlockedCompareExchange64)
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#endif
#endif


nai_atomic_t nai_atomic_inc(nai_atomic_t* x)
{
#if NAI_SIZEOF_VOID_P == 4
    return InterlockedIncrement(x);
#else
    return InterlockedIncrement64(x);
#endif
};


nai_atomic_t nai_atomic_dec(nai_atomic_t* x)
{
#if NAI_SIZEOF_VOID_P == 4
    return InterlockedDecrement(x);
#else
    return InterlockedDecrement64(x);
#endif
};


nai_atomic_t nai_atomic_add(nai_atomic_t* x, nai_atomic_t a)
{
#if NAI_SIZEOF_VOID_P == 4
    return InterlockedExchangeAdd(x, a) + a;
#else
    return InterlockedExchangeAdd64(x, a) + a;
#endif
};


nai_atomic_t nai_atomic_cas(
    nai_atomic_t* x, nai_atomic_t c, nai_atomic_t n)
{
#if NAI_SIZEOF_VOID_P == 4
    return InterlockedCompareExchange(x, n, c);
#else
    return InterlockedCompareExchange64(x, n, c);
#endif
};


nai_atomic32_t nai_atomic32_inc(nai_atomic32_t* x)
{
    return InterlockedIncrement(x);
};


nai_atomic32_t nai_atomic32_dec(nai_atomic32_t* x)
{
    return InterlockedDecrement(x);
};


nai_atomic32_t nai_atomic32_add(nai_atomic32_t* x, nai_atomic32_t a)
{
    return InterlockedExchangeAdd(x, a) + a;
};


nai_atomic32_t nai_atomic32_cas(
    nai_atomic32_t* x, nai_atomic32_t c, nai_atomic32_t n)
{
    return InterlockedCompareExchange(x, n, c);
};



#else 



#if (NAI_HAVE_PTHREAD_H)


#include "unix/nai_unix.h"


static nai_int_t nai_psult(nai_int_t e)
{
    nai_int_t r;


    if (e) {
        nai_errno = e;
        r = -1;
        goto _end;
    }

    r = 0;

_end:
    return r;
};

static nai_int_t nai_pinited(const void* p, size_t s)
{
    nai_int_t r;
    size_t n;
    const uint8_t* m = (const uint8_t*)p;


    for (n = 0; n < s; n ++) {
        if (m[n] != 0) {
            r = 1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
}



///////////////////////////////////////////////////////////////////////////
// thread attr


nai_int_t nai_thread_attr_open(nai_thread_attr_t* a)
{
    return nai_psult(pthread_attr_init(a));
};


nai_int_t nai_thread_attr_set_stacksize(
    nai_thread_attr_t* a, size_t stack_size)
{
    return nai_psult(pthread_attr_setstacksize(a, stack_size));
};


nai_int_t nai_thread_attr_set_guardsize(
    nai_thread_attr_t* a, size_t guard_size)
{
    return nai_psult(pthread_attr_setguardsize(a, guard_size));
};


nai_int_t nai_thread_attr_set_sched(
    nai_thread_attr_t* a, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_int_t set;
    struct sched_param sp;
    nai_sched_priority_t si;


    r = nai_sched_query_priority(&si, sched, priority);
    if (r < 0) {
        goto _end;
    };

    set = r;
    r = nai_psult(pthread_attr_setinheritsched(
        a, set ? PTHREAD_EXPLICIT_SCHED : PTHREAD_INHERIT_SCHED));
    if (r < 0) {
        goto _end;
    };

    if (set) {
        r = nai_psult(pthread_attr_setschedpolicy(a, si.sched));
        if (r < 0) {
            goto _end;
        };

        if (si.sched == SCHED_OTHER) {
            sp.sched_priority = 0;
        } else {
            sp.sched_priority = si.priority;
        };
        r = nai_psult(pthread_attr_setschedparam(a, &sp));
    };

_end:
    return r;
};


nai_int_t nai_thread_attr_set_affinity(
    nai_thread_attr_t* a, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;


#if (NAI_HAVE_PTHREAD_ATTR_SETAFFINITY_NP)
#if (__linux__) || (__sylixos__)
    r = nai_psult(pthread_attr_setaffinity_np(a, size, (cpu_set_t*)set));
#else /* *bsd */
    r = nai_psult(pthread_attr_setaffinity_np(a, size, (cpuset_t*)set));
#endif
#else
    (void)a;
    (void)size;
    (void)set;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_thread_attr_close(nai_thread_attr_t* a)
{
    return nai_psult(pthread_attr_destroy(a));
};



///////////////////////////////////////////////////////////////////////////
// thread


nai_int_t nai_thread_create(nai_thread_t* t, 
    const nai_thread_attr_t* a, nai_thread_entry_f ent, void* arg)
{
    return nai_psult(pthread_create(t, a, ent, arg));
};


nai_int_t nai_thread_set_sched(
    nai_thread_t* t, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    struct sched_param sp;
    nai_sched_priority_t si;


    r = nai_sched_query_priority(&si, sched, priority);
    if (r <= 0) {
        goto _end;
    };

    if (si.sched == SCHED_OTHER) {
        sp.sched_priority = 0;
    } else {
        sp.sched_priority = si.priority;
    };
    r = nai_psult(pthread_setschedparam(*t, si.sched, &sp));

_end:
    return r;
};


nai_int_t nai_thread_get_affinity(
    nai_thread_t* t, size_t size, nai_cpuset_t* set)
{
    nai_int_t r;


#if (NAI_HAVE_PTHREAD_GETAFFINITY_NP)
#if (__linux__) || (__sylixos__)
    r = nai_psult(pthread_getaffinity_np(*t, size, (cpu_set_t*)set));
#else /* *bsd */
    r = nai_psult(pthread_getaffinity_np(*t, size, (cpuset_t*)set));
#endif
#else
    (void)t;
    (void)size;
    (void)set;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_thread_set_affinity(
    nai_thread_t* t, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;


#if (NAI_HAVE_PTHREAD_SETAFFINITY_NP)
#if (__linux__) || (__sylixos__)
    r = nai_psult(pthread_setaffinity_np(*t, size, (cpu_set_t*)set));
#else /* *bsd */
    r = nai_psult(pthread_setaffinity_np(*t, size, (cpuset_t*)set));
#endif
#else
    (void)t;
    (void)size;
    (void)set;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_thread_join(nai_thread_t* t)
{
    nai_int_t r;


    r = nai_psult(pthread_join(*t, 0));
    if (r < 0) {
        goto _end;
    };

    t[0] = 0;

_end:
    return r;
};


nai_int_t nai_thread_detach(nai_thread_t* t)
{
    nai_int_t r;


    r = nai_psult(pthread_detach(*t));
    if (r < 0) {
        goto _end;
    };

    t[0] = 0;

_end:
    return r;
};


nai_tid_t nai_thread_id()
{
    return pthread_self();
};



///////////////////////////////////////////////////////////////////////////
// thread storage


nai_int_t nai_thread_key_open(nai_thread_key_t* t, nai_thread_key_free_f cb)
{
    return nai_psult(pthread_key_create(t, cb));
};


nai_int_t nai_thread_key_set(nai_thread_key_t* t, void* value)
{
    return nai_psult(pthread_setspecific(*t, value));
};


void* nai_thread_key_get(nai_thread_key_t* t)
{
    return pthread_getspecific(*t);
};


nai_int_t nai_thread_key_close(nai_thread_key_t* t)
{
    nai_int_t r = 0;


    if (*t != NAI_THREAD_KEY_NIL) {
        r = nai_psult(pthread_key_delete(*t));
        if (r >= 0) {
            t[0] = NAI_THREAD_KEY_NIL;
        };
    };

    return r;
};



///////////////////////////////////////////////////////////////////////////
// attributes for shared


typedef struct nai_pthread_attrs_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    pthread_mutexattr_t pmutex;
    pthread_condattr_t pcond[2];
    pthread_rwlockattr_t prwlock;
} nai_pthread_attrs_t;


static nai_pthread_attrs_t nai_pthread_attrs = {
    0, 0, NAI_ONCE_INIT, 
};


static void nai_pthread_attrs_init()
{
    nai_int_t r;
    nai_pthread_attrs_t* p;


    p = &nai_pthread_attrs;

    r = pthread_mutexattr_init(&p->pmutex);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_mutexattr_setpshared(&p->pmutex, PTHREAD_PROCESS_SHARED);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_condattr_init(&p->pcond[0]);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_condattr_init(&p->pcond[1]);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_condattr_setpshared(&p->pcond[1], PTHREAD_PROCESS_SHARED);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

#if (NAI_HAVE_PTHREAD_CONDATTR_SETCLOCK)
    r = pthread_condattr_setclock(&p->pcond[0], CLOCK_MONOTONIC);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_condattr_setclock(&p->pcond[1], CLOCK_MONOTONIC);
    if (r != 0) {
        p->error = r;
        goto _end;
    };
#endif

    r = pthread_rwlockattr_init(&p->prwlock);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

    r = pthread_rwlockattr_setpshared(&p->prwlock, PTHREAD_PROCESS_SHARED);
    if (r != 0) {
        p->error = r;
        goto _end;
    };

_end:
    nai_memory_barrier();
    nai_pthread_attrs.inited = 1;
    return;
};


static void nai_pthread_attrs_term()
{
    if (nai_pthread_attrs.inited) {
        pthread_mutexattr_destroy(&nai_pthread_attrs.pmutex);
        pthread_condattr_destroy(&nai_pthread_attrs.pcond[0]);
        pthread_condattr_destroy(&nai_pthread_attrs.pcond[1]);
        pthread_rwlockattr_destroy(&nai_pthread_attrs.prwlock);
    };

    return;
};


///////////////////////////////////////////////////////////////////////////
// mutex


nai_int_t nai_mutex_open(nai_mutex_t* m, nai_int_t shared)
{
    nai_int_t r;
    pthread_mutexattr_t* a = 0;


    if (shared) {
        if (nai_pthread_attrs.inited == 0) {
            nai_once(&nai_pthread_attrs.once, nai_pthread_attrs_init);
        };
        if (nai_pthread_attrs.error) {
            nai_errno = nai_pthread_attrs.error;
            r = -1;
            goto _end;
        };

        a = &nai_pthread_attrs.pmutex;
    };

    r = nai_psult(pthread_mutex_init(m, a));

_end:
    return r;
};


nai_int_t nai_mutex_trylock(nai_mutex_t* m)
{
    return nai_psult(pthread_mutex_trylock(m));
};


nai_int_t nai_mutex_lock(nai_mutex_t* m)
{
    return nai_psult(pthread_mutex_lock(m));
};


nai_int_t nai_mutex_unlock(nai_mutex_t* m)
{
    return nai_psult(pthread_mutex_unlock(m));
};


nai_int_t nai_mutex_close(nai_mutex_t* m)
{
    nai_int_t r = 0;

    if (nai_pinited((void*)m, sizeof(*m))) {
        r = nai_psult(pthread_mutex_destroy(m));
    };

    return r;
};



///////////////////////////////////////////////////////////////////////////
// cond


nai_int_t nai_cond_open(nai_cond_t* c, nai_int_t shared)
{
    nai_int_t r;
    pthread_condattr_t* a = 0;


    if (nai_pthread_attrs.inited == 0) {
        nai_once(&nai_pthread_attrs.once, nai_pthread_attrs_init);
    };
    if (nai_pthread_attrs.error) {
        nai_errno = nai_pthread_attrs.error;
        r = -1;
        goto _end;
    };

    a = &nai_pthread_attrs.pcond[!!shared];

    r = nai_psult(pthread_cond_init(c, a));

_end:
    return r;
};


nai_int_t nai_cond_wait(nai_cond_t* c, nai_mutex_t* m)
{
    return nai_psult(pthread_cond_wait(c, m));
};


nai_int_t nai_cond_timedwait(nai_cond_t* c, nai_mutex_t* m, uint64_t abstime)
{
    struct timespec ts;
    uint64_t realtime;


#if (NAI_HAVE_PTHREAD_CONDATTR_SETCLOCK)
    realtime = abstime;
#else
    realtime = abstime - nai_tick_to_usec();
    realtime = nai_time() + realtime;
#endif

    ts.tv_sec = realtime / 1000000;
    ts.tv_nsec = realtime % 1000000 * 1000;
    return nai_psult(pthread_cond_timedwait(c, m, &ts));
};


nai_int_t nai_cond_signal(nai_cond_t* c)
{
    return nai_psult(pthread_cond_signal(c));
};


nai_int_t nai_cond_close(nai_cond_t* c)
{
    nai_int_t r = 0;

    if (nai_pinited((void*)c, sizeof(*c))) {
        r =  nai_psult(pthread_cond_destroy(c));
    };

    return r;
};



///////////////////////////////////////////////////////////////////////////
// spinlock


nai_int_t nai_spin_open(nai_spin_t* l, nai_int_t shared)
{
    nai_int_t r;


#if (NAI_HAVE_PTHREAD_SPINLOCK)
    r = nai_psult(pthread_spin_init(
        l, shared ? PTHREAD_PROCESS_SHARED : 0));
#else
    r = nai_mutex_open(l, shared);
#endif

    return r;
};


nai_int_t nai_spin_trylock(nai_spin_t* l)
{
#if (NAI_HAVE_PTHREAD_SPINLOCK)
    return nai_psult(pthread_spin_trylock(l));
#else
    return nai_mutex_trylock(l);
#endif
};


nai_int_t nai_spin_lock(nai_spin_t* l)
{
#if (NAI_HAVE_PTHREAD_SPINLOCK)
    return nai_psult(pthread_spin_lock(l));
#else
    return nai_mutex_lock(l);
#endif
};


nai_int_t nai_spin_unlock(nai_spin_t* l)
{
#if (NAI_HAVE_PTHREAD_SPINLOCK)
    return nai_psult(pthread_spin_unlock(l));
#else
    return nai_mutex_unlock(l);
#endif
};


nai_int_t nai_spin_close(nai_spin_t* l)
{
    nai_int_t r;


#if (NAI_HAVE_PTHREAD_SPINLOCK)
    r = 0;
    if (nai_pinited((void*)l, sizeof(*l))) {
        r = nai_psult(pthread_spin_destroy(l));
    };
#else
    r = nai_mutex_close(l);
#endif

    return r;
};



///////////////////////////////////////////////////////////////////////////
// rwlock


nai_int_t nai_rwlock_open(nai_rwlock_t* l, nai_int_t shared)
{
    nai_int_t r;
    pthread_rwlockattr_t* a = 0;


    if (shared) {
        if (nai_pthread_attrs.inited == 0) {
            nai_once(&nai_pthread_attrs.once, nai_pthread_attrs_init);
        };
        if (nai_pthread_attrs.error) {
            nai_errno = nai_pthread_attrs.error;
            r = -1;
            goto _end;
        };

        a = &nai_pthread_attrs.prwlock;
    };

    r = nai_psult(pthread_rwlock_init(l, a));

_end:
    return r;
};


nai_int_t nai_rwlock_rd_trylock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_tryrdlock(l));
};


nai_int_t nai_rwlock_rd_lock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_rdlock(l));
};


nai_int_t nai_rwlock_rd_unlock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_unlock(l));
};


nai_int_t nai_rwlock_wr_trylock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_trywrlock(l));
};


nai_int_t nai_rwlock_wr_lock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_wrlock(l));
};


nai_int_t nai_rwlock_wr_unlock(nai_rwlock_t* l)
{
    return nai_psult(pthread_rwlock_unlock(l));
};


nai_int_t nai_rwlock_close(nai_rwlock_t* l)
{
    nai_int_t r = 0;

    if (nai_pinited((void*)l, sizeof(*l))) {
        r = nai_psult(pthread_rwlock_destroy(l));
    };

    return r;
};



///////////////////////////////////////////////////////////////////////////
// once


nai_int_t nai_once(nai_once_t* p, nai_once_entry_f once)
{
    return nai_psult(pthread_once(p, once));
};



#else


#error "nai_thread is not implemented, unsupported platform!"


#endif



///////////////////////////////////////////////////////////////////////////
// atomic


#if (NAI_HAVE_GCC_ATOMIC)



nai_atomic_t nai_atomic_inc(nai_atomic_t* x)
{
    return __sync_add_and_fetch(x, 1);
};


nai_atomic_t nai_atomic_dec(nai_atomic_t* x)
{
    return __sync_add_and_fetch(x, -1);
};


nai_atomic_t nai_atomic_add(nai_atomic_t* x, nai_atomic_t a)
{
    return __sync_add_and_fetch(x, a);
};


nai_atomic_t nai_atomic_cas(
    nai_atomic_t* x, nai_atomic_t c, nai_atomic_t n)
{
    return __sync_val_compare_and_swap(x, c, n);
};


nai_atomic32_t nai_atomic32_inc(nai_atomic32_t* x)
{
    return __sync_add_and_fetch(x, 1);
};


nai_atomic32_t nai_atomic32_dec(nai_atomic32_t* x)
{
    return __sync_add_and_fetch(x, -1);
};


nai_atomic32_t nai_atomic32_add(nai_atomic32_t* x, nai_atomic32_t a)
{
    return __sync_add_and_fetch(x, a);
};


nai_atomic32_t nai_atomic32_cas(
    nai_atomic32_t* x, nai_atomic32_t c, nai_atomic32_t n)
{
    return __sync_val_compare_and_swap(x, c, n);
};



#elif (NAI_HAVE_ATOMIC_OPS)



#include "atomic_ops.h"



nai_atomic_t nai_atomic_inc(nai_atomic_t* x)
{
    return AO_fetch_and_add1((AO_t*)x) + 1;
};


nai_atomic_t nai_atomic_dec(nai_atomic_t* x)
{
    return AO_fetch_and_sub1((AO_t*)x) - 1;
};


nai_atomic_t nai_atomic_add(nai_atomic_t* x, nai_atomic_t a)
{
    return AO_fetch_and_add((AO_t*)x, a) + a;
};


nai_atomic_t nai_atomic_cas(
    nai_atomic_t* x, nai_atomic_t c, nai_atomic_t n)
{
    return AO_fetch_compare_and_swap((AO_t*)x, c, n);
};


nai_atomic32_t nai_atomic32_inc(nai_atomic32_t* x)
{
    return AO_int_fetch_and_add1((unsigned*)x) + 1;
};


nai_atomic32_t nai_atomic32_dec(nai_atomic32_t* x)
{
    return AO_int_fetch_and_sub1((unsigned*)x) - 1;
};


nai_atomic32_t nai_atomic32_add(nai_atomic32_t* x, nai_atomic32_t a)
{
    return AO_int_fetch_and_add((unsigned*)x, a) + a;
};


nai_atomic32_t nai_atomic32_cas(
    nai_atomic32_t* x, nai_atomic32_t c, nai_atomic32_t n)
{
    return AO_int_fetch_compare_and_swap((unsigned*)x, c, n);
};


#else


#error "nai_atomic is not implemented!"


#endif



#endif



float nai_atomic_addf(float* p, float a)
{
    union bits { float f; nai_atomic32_t i; };
    union bits oldVal, newVal;


    do {
        oldVal.f = *p;
        newVal.f = oldVal.f + a;
    } while (nai_atomic32_cas(
        (nai_atomic32_t*)p, oldVal.i, newVal.i) != oldVal.i);

    return newVal.f;
};


float nai_atomic_subf(float* p, float a)
{
    /* floating point calculation error
     * float a = 1.0f;
     * r = r - a;
     * not equal
     * float a = -1.0f
     * r = r + a;
     **/
#if 1
    return nai_atomic_addf(p, -a);
#else
    union bits { float f; nai_atomic32_t i; };
    union bits oldVal, newVal;

    do {
        oldVal.f = *p;
        newVal.f = oldVal.f - a;
    } while (nai_atomic32_cas(
        (nai_atomic32_t*)p, oldVal.i, newVal.i) != oldVal.i);

    return newVal.f;
#endif
};



#define nai_atomic_mark         1
#define nai_atomic_spin         2048



nai_int_t nai_atomic_trylock(nai_atomic_t* x)
{
    return nai_atomic32_trylock((nai_atomic32_t*)x);
};


nai_int_t nai_atomic_lock(nai_atomic_t* x)
{
    return nai_atomic32_lock((nai_atomic32_t*)x);
};


nai_int_t nai_atomic_unlock(nai_atomic_t* x)
{
    return nai_atomic32_unlock((nai_atomic32_t*)x);
};


nai_int_t nai_atomic32_trylock(nai_atomic32_t* x)
{
    nai_int_t r;
    nai_atomic32_t n;


    n = nai_atomic32_cas(x, 0, nai_atomic_mark);
    if (n != 0) {
        nai_errno = EBUSY;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_atomic32_lock(nai_atomic32_t* x)
{
    nai_int_t r;
    nai_int_t t, s;
    nai_atomic32_t n;


    for (;;) {
        n = nai_atomic32_cas(x, 0, nai_atomic_mark);
        if (n == 0) {
            break;
        };

        for (s = 1; s < 2048; s <<= 1) {
            for (t = 0; t < s; t ++) {
                nai_cpu_pause();
            };

            if (*x != 0) {
                continue;
            };

            n = nai_atomic32_cas(x, 0, nai_atomic_mark);
            if (n == 0) {
                break;
            };
        };

        if (n == 0) {
            break;
        };

        nai_yield();
    };

    r = 0;

    return r;
};


nai_int_t nai_atomic32_unlock(nai_atomic32_t* x)
{
    nai_int_t r;
    nai_atomic32_t n;


    n = nai_atomic32_cas(x, nai_atomic_mark, 0);
    if (n != nai_atomic_mark) {
        nai_errno = EPERM;
        r = 0;
        goto _end;
    };

    r = 0;

_end:
    return r;
};



typedef struct nai_thread_global_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_mutex_t lock;
} nai_thread_global_t;


static nai_thread_global_t nai_thread_global = {
    0, 0, NAI_ONCE_INIT
};


static void nai_thread_global_init()
{
    nai_int_t r;


    r = nai_mutex_open(&nai_thread_global.lock, 0);
    if (r < 0) {
        nai_thread_global.error = nai_errno;
    };

    nai_memory_barrier();
    nai_thread_global.inited = 1;
    return;
};


static void nai_thread_global_term()
{
    if (nai_thread_global.inited) {
        nai_mutex_close(&nai_thread_global.lock);
    };

    return;
};


nai_mutex_t* nai_thread_mutex()
{
    nai_mutex_t* r;


    if (nai_thread_global.inited == 0) {
        nai_once(&nai_thread_global.once, nai_thread_global_init);
    };
    if (nai_thread_global.error) {
        nai_errno = nai_thread_global.error;
        r = 0;
        goto _end;
    };

    r = &nai_thread_global.lock;

_end:
    return r;
};


nai_cond_t* nai_thread_local_cond()
{
    nai_int_t r;
    nai_cond_t* c;
    nai_tlocal_t* t;


    t = nai_tlocal_get();
    if (t == 0) {
        c = 0;
        goto _end;
    };

    c = &t->cond;
    if (t->valid_cond == 0) {
        r = nai_cond_open(c, 0);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_cond_open() failed");
            c = 0;
            goto _end;
        };

        t->valid_cond = 1;
    };

_end:
    return c;
};


void* nai_thread_local_mem(size_t size)
{
    void* r;
    nai_tlocal_t* t;


    t = nai_tlocal_get();
    if (t == 0) {
        r = 0;
        goto _end;
    };

    r = nai_tlocal_mem(t, size, 0);


_end:
    return r;
};


nai_int_t nai_thread_io_errno()
{
    nai_int_t ec;


    ec = nai_errno;
    if (ec == EINTR && nai_thread_io_canceling()) {
        ec = nai_errno = ECANCELED;
    };

    return ec;
};


nai_int_t nai_thread_io_canceling()
{
    nai_int_t r;
    nai_tlocal_t* t;


    t = nai_tlocal_get();
    if (t == 0) {
        r = 0;
        goto _end;
    };

    r = nai_tlocal_io_canceling(t);


_end:
    return r;
};


nai_int_t nai_thread_io_interrupt(nai_task_t* t, nai_thread_t tid)
{
    nai_int_t r;


    (void)t;

#if defined(_WIN32)
    r = 0;
    if (nai_wapi.CancelSynchronousIo) {
        if (!nai_wapi.CancelSynchronousIo(tid)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
        };
    };
#else
    r = pthread_kill(tid, SIGURG);
    if (r) {
        nai_errno = r;
        r = -1;
    };
#endif

    return r;
};



void* nai_tlocal_mem(nai_tlocal_t* t, size_t size, nai_int_t lock)
{
    void* r;


    if ((intptr_t)t->buf & 0x1) {
        t->buf = 0;
    };
    if (t->buf && t->buflen >= size) {
        r = t->buf;
    } else {
        r = nai_malloc(size);
        if (r == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_malloc() failed");
            goto _end;
        };

        if (t->buf) {
            nai_free(t->buf);
        };

        t->buf = r;
        t->buflen = size;
    };
    if (lock) {
        /* lock */
        t->buf = (void*)((intptr_t)t->buf | 0x1);
    };


_end:
    return r;
};


nai_int_t nai_tlocal_mem_unlock(nai_tlocal_t* t, void* ptr)
{
    nai_int_t r;


    if (t->buf == (void*)((intptr_t)ptr | 0x1)) {
        /* unlock, put back */
        t->buf = ptr;
    } else {
        if ((intptr_t)t->buf & 0x1) {
            /* lost memory: some code without unlock memory */
            assert(0);
        };

        /* exists a new memory, free old */
        nai_free(ptr);
    };

    r = 0;

    return r;
};


nai_int_t nai_tlocal_cleanup(nai_tlocal_t* t)
{
    nai_int_t r;


    if (t->valid_cond) {
        r = nai_cond_close(&t->cond);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_cond_close() failed");
            goto _end;
        };

        t->valid_cond = 0;
    };

    if (t->buf) {
        nai_free(t->buf);
        t->buf = 0;
        t->buflen = 0;
    };

    r = 0;

_end:
    return r;
};


void nai_thread_term()
{
    nai_thread_global_term();

#if !defined(_WIN32)
#if (NAI_HAVE_PTHREAD_H)
    nai_pthread_attrs_term();
#endif
#endif

    return;
};


