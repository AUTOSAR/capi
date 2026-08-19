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
/// @file       nai_task.c
/// @brief      
/// @details
/// @date       2023-09-23
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_task.h"

#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"

#if defined(_WIN32)
    #include <handleapi.h>
    #include <processthreadsapi.h>

    #include "win/nai_windows.h"
#endif

#define NAI_TASK_NONE     0
#define NAI_TASK_RUNNING  1
#define NAI_TASK_DONE     2
#define NAI_TASK_CANCELED 3

/* Task item */
typedef struct nai_task_entry_s
{
    nai_list_entry_t ent;
    nai_cond_t* wait;
    nai_task_f fn;
    nai_task_pool_t* pool;
    nai_thread_t thread;
    nai_tid_t tid;
    nai_atomic32_t key;
    nai_atomic32_t stat;
    int8_t refs;
    int8_t handling;
    int8_t cancelable;
    int8_t copy;
    union
    {
        void* args;
        uint8_t abuf[1];
    };
} nai_task_entry_t;

static nai_thread_api nai_task_entry(void* arg)
{
    nai_int_t r;
    nai_int_t wakeup = 0;
    nai_tid_t tid    = nai_thread_id();
    nai_thread_t thread;
    nai_task_pool_t* p = (nai_task_pool_t*)arg;
    nai_task_entry_t* t;

#if defined(_WIN32)
    thread = OpenThread(THREAD_ALL_ACCESS, 0, tid);
    assert(thread != 0);
#else
    thread = tid;
#endif

    /* enter */
    nai_mutex_lock(&p->lock);

    /* loop */
    for (;;) {
        /* check */
        if (p->ending) {
            nai_cond_signal(&p->cond);
            break;
        };
        if (nai_list_is_empty(&p->queue)) {
            nai_cond_wait(&p->cond, &p->lock);
            continue;
        };

        /* enter task */
        t           = (nai_task_entry_t*)p->queue.next;
        t->tid      = tid;
        t->thread   = thread;
        t->handling = 1;
        nai_list_entry_remove(&t->ent);

        /* has queued task, need wakeup other threads */
        wakeup = !nai_list_is_empty(&p->queue);

        /* queue in wait list for lookup by task_complete */
        if (t->wait) {
            nai_list_insert_head(&p->wait, &t->ent);
        };

        nai_mutex_unlock(&p->lock);

        /* wakeup others */
        if (wakeup) {
            wakeup = 0;
            nai_cond_signal(&p->cond);
        };
        (void)wakeup;

        /* get task stat */
        if (t->cancelable) {
            r = nai_atomic32_cas(&t->stat, t->key, NAI_TASK_RUNNING);
        } else {
            r = NAI_TASK_CANCELED + 1;
        };

        /* check task stat */
        switch (r) {
            case NAI_TASK_DONE:
            case NAI_TASK_NONE:
            case NAI_TASK_RUNNING:
                nai_log_crit(NAI_LOG_CORE, 0, "task with incorrect stat %d", r);
                break;

            case NAI_TASK_CANCELED:
                break;

            default:
                /* exec task */
                if (t->copy == 0) {
                    t->fn(t->args);
                } else {
                    t->fn(t->abuf);
                };

                t->stat = NAI_TASK_DONE;
                break;
        };

        /* leave task */
        nai_mutex_lock(&p->lock);

        if (t->wait) {
            if (t->fn) { /* has completed ? */
                /* the waitor already in cond_wait, 
                 * so here unneed lock before set 'fn'
                 **/
                t->fn = 0;
                nai_cond_signal(t->wait);
            };
            nai_list_entry_remove(&t->ent);
        };

        /* free */
        t->refs--;
        if (t->refs <= 0) {
            nai_fixedpool_free(&p->pool, t);
        };

        /* complete notify */
        p->count--;
        if (p->count <= 0 && p->wall) {
            nai_cond_signal(&p->complete);
        };
    };

    /* leave */
    nai_mutex_unlock(&p->lock);

#if defined(_WIN32)
    if (thread) {
        CloseHandle(thread);
    };
#endif

    /* exit */
    return 0;
};

static nai_int_t nai_task_pool_queue_impl(
    nai_task_pool_t* p, nai_task_f entry, void* args, nai_int_t wait, nai_int_t copy, nai_task_t* pt)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t key;
    nai_int_t empty = 0;
    nai_task_entry_t* t;

    if (p->ending) {
        nai_errno = NAI_ECLOSED;
        r         = -1;
        goto _end;
    };

    nai_mutex_lock(&p->lock);

    if (p->max_count && p->max_count <= (p->count - (intptr_t)p->threads.count)) {
        ec = ENOSPC;
        r  = -1;
        goto _end;
    };

    t = (nai_task_entry_t*)nai_fixedpool_alloc(&p->pool);
    if (t == 0) {
        ec = nai_errno;
        r  = -1;
        goto _fail;
    };

    /* init task */
    t->refs     = 1;
    t->handling = 0;
    t->pool     = p;
    t->wait     = 0;
    t->fn       = entry;
    t->copy     = !!copy;
    if (t->copy == 0) {
        t->args = args;
    } else {
        nai_memcpy(t->abuf, args, p->arg_size);
    };

    /* prepare for cancel */
    if (pt && pt->cancelable) {
        for (;;) {
            key = p->next++;
            key &= NAI_INT_T_MAX / 2;
            if (key > NAI_TASK_CANCELED) {
                break;
            };

            p->next = NAI_TASK_CANCELED + 1;
        };

        t->key        = key;
        t->stat       = key;
        t->cancelable = 1;
    } else {
        t->key        = 0;
        t->stat       = 0;
        t->cancelable = 0;
    };

    /* fill key */
    if (pt) {
        pt->data      = t->abuf;
        pt->key       = t->key;
        pt->canceling = 0;
    };

    /* check need notify */
    empty = nai_list_is_empty(&p->queue);

    /* queue task */
    nai_list_insert_tail(&p->queue, &t->ent);
    p->count++;

    /* block task */
    if (wait) {
        t->wait = nai_thread_local_cond();
        if (t->wait == 0) {
            ec = nai_errno;
            nai_log_alert(NAI_LOG_CORE, ec, "nai_thread_local_cond() failed");

            r = -1;
            goto _fail;
        };

        if (empty) {
            empty = 0;
            nai_cond_signal(&p->cond);
        };

        /* */
        t->refs++;
        while (t->fn) {
            nai_cond_wait(t->wait, &p->lock);
        };

        /* free */
        t->refs--;
        if (t->refs <= 0) {
            nai_fixedpool_free(&p->pool, t);
        };
    };

    r = 0;

_fail:
    nai_mutex_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    } else if (empty) {
        nai_cond_signal(&p->cond);
    } else {
        /* nothing */
        ;
    };

_end:
    return r;
};

nai_int_t nai_task_pool_init(nai_task_pool_t* p, nai_int_t arg_size)
{
    nai_int_t r;

    assert(arg_size >= 0);

    nai_mutex_init(&p->lock);
    nai_cond_init(&p->cond);
    nai_cond_init(&p->complete);
    nai_list_init(&p->queue);
    nai_list_init(&p->wait);
    nai_array_init(&p->threads, sizeof(nai_thread_t), 0);
    nai_fixedpool_init(&p->pool,
                       sizeof(nai_task_entry_t) + (arg_size > (nai_int_t)sizeof(void*) ? arg_size - sizeof(void*) : 0),
                       0, 0);

    p->next      = NAI_TASK_CANCELED + 1;
    p->count     = 0;
    p->max_count = 0;
    p->arg_size  = arg_size;
    p->flags     = 0;
    r            = 0;

    return r;
};

nai_int_t nai_task_pool_open(nai_task_pool_t* p, nai_int_t threads, const nai_thread_attr_t* a)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_thread_t t;

    /* create mt */
    if (nai_mutex_open(&p->lock, 0) < 0) {
        r = -1;
        goto _fail;
    };
    if (nai_cond_open(&p->cond, 0) < 0) {
        r = -1;
        goto _fail;
    };
    if (nai_cond_open(&p->complete, 0) < 0) {
        r = -1;
        goto _fail;
    };

    p->ending = 0;
    for (n = 0; n < threads; n++) {
        if (nai_thread_create(&t, a, nai_task_entry, p) < 0) {
            r = -1;
            goto _fail;
        };

        *(nai_thread_t*)nai_array_push(&p->threads) = t;
    };

    r = 0;

_fail:
    if (r < 0) {
        ec = nai_errno;
        nai_task_pool_close(p);
        nai_errno = ec;
    };
    return r;
}

nai_int_t nai_task_pool_close(nai_task_pool_t* p)
{
    nai_int_t r;
    nai_int_t n;

    /* free threads */
    if (p->threads.count > 0) {
        /* mark ending */
        nai_mutex_lock(&p->lock);
        p->ending = 1;
        nai_mutex_unlock(&p->lock);
        nai_cond_signal(&p->cond);

        /* waiting */
        for (n = 0; n < (intptr_t)p->threads.count; n++) {
            nai_thread_t* t = (nai_thread_t*)p->threads.elts + n;
            nai_thread_join(t);
        };
        nai_array_close(&p->threads);
    };

    nai_fixedpool_close(&p->pool);
    nai_mutex_close(&p->lock);
    nai_cond_close(&p->cond);
    nai_cond_close(&p->complete);
    nai_list_close(&p->queue);
    nai_list_close(&p->wait);
    p->count = 0;
    r        = 0;

    return r;
};

nai_int_t nai_task_pool_get_queued(nai_task_pool_t* p) { return p->count; };

nai_int_t nai_task_pool_get_max_queued(nai_task_pool_t* p) { return p->max_count; };

nai_int_t nai_task_pool_set_max_queued(nai_task_pool_t* p, nai_int_t max_queued)
{
    nai_int_t r;

    if (max_queued < 0) {
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };

    p->max_count = max_queued;
    r            = 0;

_end:
    return r;
};

nai_int_t nai_task_pool_queue(nai_task_pool_t* p, nai_task_f cb, void* ud, nai_int_t wait, nai_task_t* t)
{
    return nai_task_pool_queue_impl(p, cb, ud, wait, 0, t);
};

nai_int_t nai_task_pool_queue_copy(nai_task_pool_t* p, nai_task_f cb, void* ud, nai_int_t wait, nai_task_t* t)
{
    return nai_task_pool_queue_impl(p, cb, ud, wait, 1, t);
};

nai_int_t nai_task_pool_complete(nai_task_pool_t* p)
{
    nai_int_t r;
    nai_int_t waiting;
    nai_tid_t tid;
    nai_list_entry_t* e;
    nai_task_entry_t* f;
    nai_task_entry_t* t;

    t   = 0;
    tid = nai_thread_id();
    nai_mutex_lock(&p->lock);

    e = p->wait.next;
    for (; e != &p->wait; e = e->next) {
        f = (nai_task_entry_t*)e;
        if (f->tid == tid) {
            waiting = f->fn != 0;
            f->fn   = 0;
            t       = f;
            break;
        };
    };

    nai_mutex_unlock(&p->lock);

    if (t == 0) {
        nai_errno = ENOENT;
        r         = -1;
    } else {
        if (waiting) {
            /* the waitor already in cond_wait, 
             * so here unneed lock before set 'fn'
             **/
            nai_cond_signal(t->wait);
        };
        r = 0;
    };

    return r;
};

nai_int_t nai_task_pool_wait(nai_task_pool_t* p)
{
    nai_int_t r;

    nai_mutex_lock(&p->lock);

    p->wall++;

    while (p->count > 0) {
        nai_cond_wait(&p->complete, &p->lock);
    };

    p->wall--;
    if (p->wall > 0) {
        nai_cond_signal(&p->complete);
    };

    nai_mutex_unlock(&p->lock);

    r = 0;

    return r;
};

nai_int_t nai_task_wait(nai_task_t* t, int32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_task_pool_t* p;
    nai_task_entry_t* e;
    uint64_t abstime;

    if (t->data == 0 || t->data == (void*)1 || t->key == 0) {
        if (t->key == 0) {
            nai_errno = EINVAL;
            r         = -1;
        } else {
            r = 0;
        };
        goto _end;
    };

    e = nai_containof(t->data, nai_task_entry_t, abuf);
    if (e->key != t->key) {
        t->data = 0;
        r       = 0;
        goto _end;
    };

    if (msec < -1) {
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    } else if (msec == -1) {
        abstime = INT64_MAX;
    } else {
        abstime = nai_tick_to_usec() + msec * 1000;
    };

    p = e->pool;
    nai_mutex_lock(&p->lock);

    /* test the key of task */
    if (e->key != t->key) {
        t->data = 0;
        r       = 0;
        goto _done;
    };

    /* test the stat of task */
    switch (e->stat) {
        case NAI_TASK_NONE:
        case NAI_TASK_DONE:
            t->data = 0;
            /* fallthrough */

        case NAI_TASK_CANCELED:
            r = 0;
            goto _done;

        case NAI_TASK_RUNNING:
            break;

        default:
            break;
    };

    /* check */
    if (e->wait != 0) {
        ec = EBUSY;
        r  = -1;
        goto _done;
    };

    /* prepare */
    e->wait = nai_thread_local_cond();
    if (e->wait == 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, ec, "nai_thread_local_cond() failed");

        r = -1;
        goto _done;
    };

    if (e->handling) {
        nai_list_insert_tail(&p->wait, &e->ent);
    };

    /* waiting */
    r  = 0;
    ec = 0;
    e->refs++;
    while (e->fn) {
        r = nai_cond_timedwait(e->wait, &p->lock, abstime);
        if (r < 0) {
            ec = nai_errno;
            break;
        };
    };

    /* cleanup */
    e->wait = 0;
    e->refs--;
    if (e->refs <= 0) {
        t->data = 0;
        nai_fixedpool_free(&p->pool, e);
    } else if (e->handling && e->fn) {
        nai_list_entry_remove(&e->ent);
    } else {
        /* nothing */
        ;
    };

_done:
    nai_mutex_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};

nai_int_t nai_task_cancel(nai_task_t* t, nai_task_break_f fn)
{
    nai_int_t r;
    nai_int_t stat;
    nai_task_pool_t* p;
    nai_task_entry_t* e;

    if (t->data == 0 || t->data == (void*)1 || t->key == 0) {
        if (t->key == 0) {
            nai_errno = EINVAL;
            r         = -1;
        } else if (t->data == 0) {
            nai_errno = ENOENT;
            r         = -1;
        } else {
            r = 0;
        };
        goto _end;
    };

    e    = nai_containof(t->data, nai_task_entry_t, abuf);
    stat = e->stat;
    nai_memory_barrier();

    if (e->key != t->key) {
        nai_errno = ENOENT;
        t->data   = 0;
        r         = -1;
        goto _end;
    };

    if (stat == t->key) {
        stat = nai_atomic32_cas(&e->stat, t->key, NAI_TASK_CANCELED);
    };
    if (stat == t->key) {
        t->data = (void*)1;
        r       = 0;
    } else {
        switch (stat) {
            case NAI_TASK_RUNNING:
                t->canceling = 1;

                if (fn != 0) {
                    p = e->pool;
                    nai_mutex_lock(&p->lock);

                    if (e->key == t->key && e->stat == NAI_TASK_RUNNING) {
                        fn(t, e->thread);
                    };

                    nai_mutex_unlock(&p->lock);
                };

                nai_errno = EINPROGRESS;
                r         = -1;
                break;
            case NAI_TASK_CANCELED:
                r = 0;
                break;
            case NAI_TASK_DONE:
                nai_errno = ENOENT;
                t->data   = 0;
                r         = -1;
                break;
            default:
                nai_errno = ENOENT;
                t->data   = 0;
                r         = -1;
                break;
        };
    };

_end:
    return r;
};
