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
/// @file       nai_evloop.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evloop.h"
#include "nai_evtimer.h"
#include "nai_evmsg.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_time.h"
#include "../io/nai_iobase.h"
#include <time.h>



//////////////////////////////////////////////////////////////////////////////
// time cache


static const char nai_months[12][4] = {
    "Jan", "Feb", "Mar", 
    "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", 
    "Oct", "Nov", "Dec"
};

static nai_timedata_t nai_timedata[128];
static nai_timefmts_t nai_timefmts[64];


nai_timecache_t nai_timecache = {
    .access = NAI_EV_TIME_UPDATE_FREQ, 
    .slot_data = 0, 
    .slot_fmts = 0, 
    .data = nai_timedata, 
    .fmts = nai_timefmts, 
    .lock = 0, 
};


nai_int_t nai_timecache_update()
{
    nai_timecache_update_i();
    return 0;
};


uint64_t nai_timecache_to_usec()
{
    uint64_t usec;

    nai_timecache_access();
    usec = (uint64_t)nai_timecache_get_utime();

    return usec;
};


uint64_t nai_tickcache_to_usec()
{
    uint64_t msec;

    nai_timecache_access();
    msec = (uint64_t)nai_timecache_get_utick();

    return msec;
};


uint64_t nai_tickcache_to_msec()
{
    uint64_t msec;

    nai_timecache_access();
    msec = (uint64_t)nai_timecache_get_mtick();

    return msec;
};


uint32_t nai_tickcache_to_msec32()
{
    uint32_t msec;

    nai_timecache_access();
    msec = (uint32_t)nai_timecache_get_mtick();

    return msec;
};


nai_timefmts_t* nai_timecache_fmts()
{
    nai_timefmts_t* fmts;

    nai_timecache_access();
    fmts = nai_timecache.fmts;

    return fmts;
};


void nai_timecache_update_impl()
{
    nai_int_t slot;
    nai_int_t gmtoff;
    char gmtsign;
    time_t sec;
    struct tm tm;
    nai_timedata_t* data;
    nai_timefmts_t* fmts;


    nai_atomic32_lock(&nai_timecache.lock);


    /* update time */
    slot = nai_timecache.slot_data;
    slot ++;
    if (slot >= (nai_int_t)nai_countof(nai_timedata)-1) {
        slot = 0;
    };

    data = &nai_timedata[slot];
    data->utime = nai_time();
    data->utick = nai_tick_to_usec();
    data->mtick = data->utick / 1000 + (data->utick % 1000 >= 500);

    nai_memory_barrier();
    nai_timecache.slot_data = slot;
    nai_timecache.data = data;


    sec = data->utime / (1000 * 1000);
    fmts = nai_timecache.fmts;
    if (fmts->sec == sec) {
        goto _end;
    };

    /* update formats */
    slot = nai_timecache.slot_fmts;
    slot ++;
    if (slot >= (nai_int_t)nai_countof(nai_timefmts)-1) {
        slot = 0;
    };

    fmts = &nai_timefmts[slot];
    fmts->sec = sec;

#if (NAI_HAVE_LOCALTIME_R)
    localtime_r(&sec, &tm);
#else
    tm = *localtime(&sec);
#endif
#if (NAI_HAVE_TM_GMTOFF)
    gmtoff = tm.tm_gmtoff / 60;
#else
    gmtoff = nai_time_get_zone(tm.tm_isdst) / 60;
#endif
    gmtsign = '+';
    if (gmtoff < 0) {
        gmtoff = -gmtoff;
        gmtsign = '-';
    };
    gmtoff %= 12 * 60;

    nai_snprintf(fmts->err_log_time, 
        sizeof(fmts->err_log_time), 
        "%4d/%02d/%02d %02d:%02d:%02d",
        tm.tm_year, tm.tm_mon,
        tm.tm_mday, tm.tm_hour, 
        tm.tm_min,  tm.tm_sec);

    nai_snprintf(fmts->http_log_time, 
        sizeof(fmts->http_log_time), 
        "%02d/%s/%d:%02d:%02d:%02d %c%02d%02d",
        tm.tm_mday, nai_months[tm.tm_mon - 1],
        tm.tm_year, tm.tm_hour,
        tm.tm_min,  tm.tm_sec,
        gmtsign, gmtoff / 60, (uint32_t)gmtoff % 60);

    nai_snprintf(fmts->iso8601_time, 
        sizeof(fmts->iso8601_time), 
        "%4d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
        tm.tm_year, tm.tm_mon,
        tm.tm_mday, tm.tm_hour,
        tm.tm_min,  tm.tm_sec,
        gmtsign, gmtoff / 60, (uint32_t)gmtoff % 60);

    nai_time_to_rfc822(sec, 
        fmts->rfc822_time, sizeof(fmts->rfc822_time));


    nai_memory_barrier();
    nai_timecache.slot_fmts = slot;
    nai_timecache.fmts = fmts;


_end:
    nai_atomic32_unlock(&nai_timecache.lock);
};


//////////////////////////////////////////////////////////////////////////////
// time queue


static nai_int_t nai_timequeue_add(nai_timequeue_t* t, nai_timenode_t* l)
{
    nai_rbnode_t** n = &nai_rbtree_root(&t->root);
    nai_rbnode_t* parent = nai_rbtree_end(&t->root);
    nai_timenode_t* e;
    uint64_t timeval = l->value;


    while (*n) {
        parent = *n;
        e = (nai_timenode_t*)parent;
        if (timeval < e->value) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    nai_rbtree_link(&t->root, &l->node, parent, n);
    nai_rbtree_color(&t->root, &l->node);
    return 0;
};


#define nai_timequeue_init(t)                           \
    nai_rbtree_init(&(t)->root)                         \

#define nai_timequeue_remove(t, l)                      \
    nai_rbtree_erase(&(t)->root, &(l)->node)            \

#define nai_timequeue_begin(t)                          \
    ((nai_timenode_t*)nai_rbtree_begin(&(t)->root))     \

#define nai_timequeue_end(t)                            \
    ((nai_timenode_t*)nai_rbtree_end(&(t)->root))       \

#define nai_timequeue_next(e)                           \
    ((nai_timenode_t*)nai_rbtree_next(&(e)->node))      \

#define nai_timequeue_prev(e)                           \
    ((nai_timenode_t*)nai_rbtree_prev(&(e)->node))      \



//////////////////////////////////////////////////////////////////////////////
// event loop


#define NAI_EV_SLOT_DYNAMIC     (NAI_EV_SLOT_STATIC_MAX-1)


typedef union nai_evloop_data_u {
    nai_evnode_t h;
    nai_evloop_ent_t e;
} nai_evloop_data_t;


static void* nai_evloop_pool_alloc(nai_evloop_t* l, nai_int_t mt)
{
    nai_int_t ec;
    void* r;


    if (!mt) {
        r = nai_fixedpool_alloc(&l->pools);
    } else {
        nai_spin_lock(&l->lock);

        r = nai_fixedpool_alloc(&l->poolm);
        if (r == 0) {
            ec = nai_errno;
        };

        nai_spin_unlock(&l->lock);

        if (r == 0) {
            nai_errno = ec;
        };
    };

    return r;
};


static nai_int_t nai_evloop_pool_free(nai_evloop_t* l, void* p, nai_int_t mt)
{
    if (!mt) {
        nai_fixedpool_free(&l->pools, p);
    } else {
        nai_spin_lock(&l->lock);
        nai_fixedpool_free(&l->poolm, p);
        nai_spin_unlock(&l->lock);
    };
    return 0;
};


static nai_int_t nai_evloop_ent_free(nai_evloop_t* l, nai_evloop_ent_t* ent)
{
    return nai_evloop_pool_free(l, ent, ent->mt);
};


static nai_int_t nai_evloop_is_concurrent(nai_evloop_t* l)
{
    nai_int_t r;
    nai_tid_t tid;


    tid = l->tid;
    r = (tid != 0 && tid != nai_thread_id());

    return r;
};


static nai_int_t nai_evloop_add(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;

#if (NAI_HAVE_FORK)
    nai_int_t alloc;
    nai_evnode_t** fds;


    assert(h->fd != NAI_FD_INVALID);
    assert(h->key == 0);
    assert(h->ent->key == -1);

    if (l->fcount >= l->fsize) {
        alloc = l->fsize << 1;
        if (alloc < 512) {
            alloc = 512;
        };

        fds = (nai_evnode_t**)nai_malloc(alloc * sizeof(*fds));
        if (fds == 0) {
            r = -1;
            goto _end;
        };

        if (l->fds != 0) {
            nai_memcpy(l->fds, fds, l->fcount * sizeof(*fds));
            nai_free(l->fds);
        };

        l->fds = fds;
        l->fsize = alloc;
        if (l->fcount == 0) {
            l->fcount = 1;
            l->fds[0] = 0;
        };
    };
#endif

    r = nai_evbase_add(&l->ev, h, events);
    if (r < 0) {
        goto _end;
    };

#if (NAI_HAVE_FORK)
    h->key = l->fcount;
    l->fds[h->key] = h;
    l->fcount ++;
#endif


_end:
    return r;
};


static nai_int_t nai_evloop_del(nai_evloop_t* l, nai_evnode_t* h)
{
    nai_int_t r;


#if (NAI_HAVE_FORK)
    if (h->key > 0) {
        l->fcount --;
        l->fds[h->key] = l->fds[l->fcount];
        l->fds[h->key]->key = h->key;
        h->key = 0;
    };
#endif

    r = nai_evbase_del(&l->ev, h);

    return r;
};


static nai_int_t nai_evloop_update_timer(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t mt)
{
    uint32_t w;
    uint32_t n;
    uint64_t timeval;
    int64_t diff;
    nai_evloop_ent_t* e;


    e = h->ent;
    assert(e);


    if (mt) {
        nai_spin_lock(&l->lock);
        timeval = h->timeval;
        nai_spin_unlock(&l->lock);
    } else {
        timeval = h->timeval;
    };

    n = h->st.ttype;
    w = e->ttype;
    if (w != n) {
        if (e->timer.value) {
            e->timer.value = 0;
            nai_timequeue_remove(&l->timeq[w], &e->timer);
        };
    };
    if (e->timer.value != timeval) {
        if (e->timer.value) {
            if (timeval && w == 0) {
                diff = timeval - e->timer.value;
                if (diff < 0) {
                    diff = -diff;
                };
                if (diff < NAI_EV_TIME_MAX_ERROR * 1000) {
                    goto _end;
                };
            };
            e->timer.value = 0;
            nai_timequeue_remove(&l->timeq[w], &e->timer);
        };
        if (timeval) {
            w = n;
            e->ttype = n;
            e->timer.value = timeval;
            nai_timequeue_add(&l->timeq[w], &e->timer);

            /* update next time */
            if (l->timenext[w] > e->timer.value) {
                l->timenext[w] = e->timer.value;
                l->timechg[w] = 1;
            };
        };
    };

_end:
    return 0;
};


static nai_int_t nai_evloop_remove_sigent(
    nai_evloop_t* l, nai_evloop_ent_t* r)
{
    nai_evloop_ent_t* e;
    nai_evloop_ent_t** prev;


    prev = &l->sigq.list;
    e = l->sigq.list;
    while (e) {
        if (e == r) {
            prev[0] = r->signext;
            break;
        };

        prev = &e->signext;
        e = e->signext;
    };

    return 0;
};


static nai_int_t nai_evloop_add_signals(
    nai_evloop_t* l, nai_evloop_ent_t* e, nai_int_t sigbits)
{
    nai_int_t empty;


    empty = l->sigq.list == 0;
    /* insert queue */
    if (e->sig == 0) {
        e->signext = l->sigq.list;
        l->sigq.list = e;
        l->sigq.count ++;
    };
    e->sig |= sigbits;

    return empty;
};


static nai_int_t nai_evloop_add_signals_locked(
    nai_evloop_t* l, nai_evloop_ent_t* e, nai_int_t sigbits, nai_int_t mt)
{
    nai_int_t r;
    nai_int_t empty;


    /* add signals */
    empty = nai_evloop_add_signals(l, e, sigbits);

    /* unlock queue */
    nai_spin_unlock(&l->lock);


    /* send signal to dispatch thread */
    if (empty) {
        if (mt == -1) {
            mt = nai_evloop_is_concurrent(l);
        };
        if (mt) {
            r = nai_evbase_signal(&l->ev);
            if (r < 0) {
                goto _end;
            };
        };
    };


    r = 0;

_end:
    return r;
};


static nai_int_t nai_evloop_set_expire_impl(
    nai_evloop_t* l, nai_evnode_t* h, uint64_t expire)
{
    nai_int_t r;
    nai_evloop_ent_t* e;


    e = h->ent;
    assert(e);

    if (nai_evloop_is_concurrent(l)) {
        nai_spin_lock(&l->lock);
        h->timeval = expire;
        r = nai_evloop_add_signals_locked(l, e, 1<<NAI_EV_SIG_TIMER, 1);
    } else {
        h->timeval = expire;
        if (e->timer.value == h->timeval) {
            r = 0;
        } else {
            r = nai_evloop_update_timer(l, h, 0);
        };
    };

    return r;
};


static nai_int_t nai_evloop_post_handler(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t c;
    nai_evloop_t* l;
    nai_evloop_task_t* t;
    nai_evloop_watch_t* w;
    nai_list_t list;
    nai_list_entry_t* n;


    l = nai_evnode_get_loop(e);
    c = nai_ev_notify_code(events);
    switch (c) {
    case 0: /* run tasks */
        nai_list_init(&list);
        nai_spin_lock(&l->lock);
        nai_list_add_tail(&list, &l->taskq);
        nai_spin_unlock(&l->lock);

        n = list.next;
        for ( ; n != &list; ) {
            t = (nai_evloop_task_t*)n;
            n = n->next;
            t->cb(t->ud);
        };

        if (!nai_list_is_empty(&list)) {
            nai_spin_lock(&l->lock);

            n = list.next;
            for ( ; n != &list; ) {
                t = (nai_evloop_task_t*)n;
                n = n->next;
                nai_fixedpool_free(&l->poolm, t);
            };

            nai_spin_unlock(&l->lock);
        };
        break;

    case 1: /* add watchs */
        nai_list_init(&list);
        nai_spin_lock(&l->lock);
        nai_list_add_tail(&list, &l->watchs);
        nai_spin_unlock(&l->lock);


        n = list.next;
        for ( ; n != &list; ) {
            w = (nai_evloop_watch_t*)n;
            n = n->next;
            nai_list_entry_remove(&w->ent);
            nai_list_insert_tail(&l->stages[w->stage], &w->ent);
        };
        break;

    default:
        assert(0);
        break;
    };

    return 0;
};


static nai_int_t nai_evloop_func_handler(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t c;
    nai_evloop_func_t* f;
    nai_evloop_link_t* a;


    if (events & NAI_EV_NOTIFY) {
        f = (nai_evloop_func_t*)e;
        c = nai_ev_notify_code(events);
        switch (c) {
        case NAI_EV_NOTIFY_INTERNAL:
            f->func(f->ud);
            break;

        default:
            a = f->link;
            nai_evloop_drop_func(a->slot->loop, a->sig);
            break;
        };
    };

    r = 0;

    return r;
};


static nai_int_t nai_evloop_close_impl(nai_evloop_t* l)
{
    nai_int_t r;
    nai_list_entry_t* e;
    nai_evmsg_port_t* p;


    if (l->ecount > 
        l->pcount + nai_evnode_is_opened(&l->post)) {
        nai_errno = EBUSY;
        r = -1;
        goto _end;
    };

    r = nai_evnode_close(&l->post);
    if (r < 0) {
        goto _end;
    };

    /* close message ports */
    e = l->ports.next;
    for ( ; e != &l->ports; ) {
        p = nai_containof(e, nai_evmsg_port_t, entl);
        e = e->next;
        l->pcount --;
        nai_list_entry_remove(&p->entl);
        nai_evmsg_port_release(p);
    };
    assert(l->ecount <= 0);
    assert(l->pcount <= 0);

    /* close high-res timer */
    if (l->timer) {
        nai_evtimer_close(l->timer);
        nai_free(l->timer);
        l->timer = 0;
    };

    /* free the fd array */
    if (l->fds != 0) {
        nai_free(l->fds);
        l->fds = 0;
        l->fsize = 0;
        l->fcount = 0;
    };

    /* close all */
    nai_spin_close(&l->lock);
    nai_cond_close(&l->cond);
    nai_mutex_close(&l->mux);
    nai_evbase_close(&l->ev);
    nai_fixedpool_close(&l->pools);
    nai_fixedpool_close(&l->poolm);
    nai_fixedpool_close(&l->pooles);
    nai_fixedpool_close(&l->poolem);
    nai_fixedpool_close(&l->poolus);
    nai_fixedpool_close(&l->poolum);
    nai_timequeue_init(&l->timeq[0]);
    nai_timequeue_init(&l->timeq[1]);
    nai_list_close(&l->taskq);
    nai_list_close(&l->watchs);
    nai_list_close(&l->setups);
    nai_list_close(&l->stages[0]);
    nai_list_close(&l->stages[1]);
    nai_rbtree_close(&l->slots);
    nai_rbtree_close(&l->mails);
    l->tid = 0;
    l->timechg[0] = 0;
    l->timechg[1] = 0;
    l->timenext[0] = -1;
    l->timenext[1] = -1;
    l->closing = 0;
    l->backwork = 0;
    l->run = 0;
    l->run_last = 0;
    l->wait_running = 0;
    l->wait_break = 0;
    l->nexts = NAI_EV_SLOT_DYNAMIC;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_evloop_remove_mail(nai_evloop_t* l, nai_evloop_ent_t* e);


static nai_atomic32_t nai_evloop_atom = 0;


nai_evloop_t* nai_evloop_new()
{
    size_t size;
    size_t alignment;
    nai_evloop_t* l;


    l = (nai_evloop_t*)nai_malloc(sizeof(*l));
    if (l == 0) {
        goto _end;
    };

    /*  align allocation with half of cacheline size.
     *  the size of nai_evloop_data_t is 64 on 64bits os, 
     *  and the value of alignment is 64 and equal with cacheline size.
     *  the size of nai_evloop_data_t is 40 on 32bits os, 
     *  and the value of alignment also is 64, waste 24 bytes of memory.
     */
    size = sizeof(nai_evloop_data_t);
    alignment = nai_align(size, 32);

    nai_evbase_init(&l->ev);
    nai_evnode_init(&l->post);
    nai_mutex_init(&l->mux);
    nai_cond_init(&l->cond);
    nai_spin_init(&l->lock);
    nai_list_init(&l->taskq);
    nai_list_init(&l->ports);
    nai_list_init(&l->watchs);
    nai_list_init(&l->setups);
    nai_list_init(&l->stages[0]);
    nai_list_init(&l->stages[1]);
    nai_list_init(&l->mail.list);
    nai_rbtree_init(&l->slots);
    nai_rbtree_init(&l->mails);
    nai_timequeue_init(&l->timeq[0]);
    nai_timequeue_init(&l->timeq[1]);
    nai_fixedpool_init(&l->pools, size, alignment, 0);
    nai_fixedpool_init(&l->poolm, size, alignment, 0);
    nai_fixedpool_init(&l->pooles, sizeof(nai_int_t), 0, 0);
    nai_fixedpool_init(&l->poolem, sizeof(nai_int_t), 0, 0);
    nai_fixedpool_init(&l->poolus, sizeof(nai_int_t), 0, 0);
    nai_fixedpool_init(&l->poolum, sizeof(nai_int_t), 0, 0);
    l->sigq.list = 0;
    l->sigq.count = 0;
    l->mail.loop = l;
    l->mail.keyp = 0;
    l->tid = 0;
    l->timer = 0;
    l->timechg[0] = 0;
    l->timechg[1] = 0;
    l->timenext[0] = -1;
    l->timenext[1] = -1;
    l->timeomax = (int64_t)INT_MAX;
    l->closing = 1;
    l->backwork = 0;
    l->ecount = 0;
    l->pcount = 0;
    l->run = 0;
    l->run_last = 0;
    l->wait_running = 0;
    l->wait_break = 0;
    l->nexts = NAI_EV_SLOT_DYNAMIC;
    l->threads_refs = 0;
    l->threads = 0;
    l->fsize = 0;
    l->fcount = 0;
    l->fds = 0;

    nai_timecache_update_i();

_end:
    return l;
};


const char* nai_evloop_get_name(nai_evloop_t* l)
{
    return nai_evbase_name(&l->ev);
};


nai_aio_port_t* nai_evloop_get_aio_port(nai_evloop_t* l)
{
    nai_aio_port_t* r;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = 0;
        goto _end;
    };

    r = nai_evbase_get_port(&l->ev);
    if (r == 0) {
        nai_errno = ENOTSUP;
    };

_end:
    return r;
};


nai_int_t nai_evloop_get_feature(nai_evloop_t* l, nai_int_t which)
{
    nai_int_t r;
    nai_evbase_ops_t* ops;


    ops = l->ev.ops;
    if (!ops) {
        r = 0;
        goto _end;
    };

    switch (which & (NAI_EV_FEAT_POLL|NAI_EV_FEAT_ASYNC)) {
    case 0:
        r = ops->feature_m;
        break;
    default: /* NAI_EV_FEAT_POLL | NAI_EV_FEAT_ASYNC */
        if (which & ops->feature_m) {
            r = ops->feature_m;
        } else if (which & ops->feature_e) {
            r = ops->feature_e;
        } else {
            r = 0;
        };
        break;
    };

_end:
    return r;
};


nai_int_t nai_evloop_get_fd_feature(nai_evloop_t* l, nai_int_t type)
{
    nai_int_t r;
    nai_int_t feature;
    nai_evbase_ops_t* ops;


    ops = l->ev.ops;
    if (!ops) {
        r = 0;
        goto _end;
    };

    switch (type) {
    case NAI_FD_TYPE_NONE:
        r = ops->feature_m;
        break;
    case NAI_FD_TYPE_FILE:
    case NAI_FD_TYPE_PIPE:
    case NAI_FD_TYPE_DEVC:
    case NAI_FD_TYPE_SOCK:
        feature = NAI_EV_FEAT_FDSTART << type;
        if (ops->feature_m & feature) {
            r = ops->feature_m;
        } else if (ops->feature_e & feature) {
            r = ops->feature_e;
        } else {
            r = 0;
        };
        break;

    default:
        r = 0;
        break;
    };

_end:
    return r;
};


nai_int_t nai_evloop_get_timeval(nai_evloop_t* l)
{
    return (nai_int_t)(l->timeomax);
};


nai_int_t nai_evloop_set_timeval(nai_evloop_t* l, int32_t msec)
{
    nai_int_t r;


    if (msec < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    l->timeomax = (int64_t)msec;
    r = 0;

_end:
    return r;
}


nai_task_pool_t* nai_evloop_get_io_threads(nai_evloop_t* l)
{
    return l->threads;
};


nai_int_t nai_evloop_ref_io_threads(nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t ec;


    nai_atomic32_lock(&nai_evloop_atom);

    if (l->threads == 0) {
        ec = ENOENT;
        r = -1;
    } else {
        l->threads_refs ++;
        r = 0;
    };

    nai_atomic32_unlock(&nai_evloop_atom);

    if (r < 0) {
        nai_errno = ec;
    };
    return r;
};


nai_int_t nai_evloop_unref_io_threads(nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t ec;


    nai_atomic32_lock(&nai_evloop_atom);

    if (l->threads == 0) {
        ec = ENOENT;
        r = -1;
    } else if (l->threads_refs <= 0) {
        ec = ERANGE;
        r = -1;
    } else {
        l->threads_refs --;
        r = 0;
    };

    nai_atomic32_unlock(&nai_evloop_atom);

    if (r < 0) {
        nai_errno = ec;
    };
    return r;
};


nai_int_t nai_evloop_back_available(nai_evloop_t* l)
{
    return l->backwork;
};


nai_int_t nai_evloop_set_io_threads(nai_evloop_t* l, nai_task_pool_t* p)
{
    nai_int_t r;
    nai_int_t ec;


    nai_atomic32_lock(&nai_evloop_atom);


    if (l->threads_refs == 0) {
        l->threads = p;
        r = 0;
    } else {
        if (l->threads != p) {
            ec = EBUSY;
            r = -1;
        } else {
            r = 0;
        };
    };

    l->backwork = (l->threads != 0) || (nai_evbase_get_port(&l->ev) != 0);

    nai_atomic32_unlock(&nai_evloop_atom);

    if (r < 0) {
        nai_errno = ec;
    };
    return r;
};


nai_int_t nai_evloop_open(nai_evloop_t* l, 
    nai_int_t optional, nai_int_t require, const char* name)
{
    nai_int_t r;
    nai_int_t ec;
    size_t eltsize;


    if (nai_evbase_is_opened(&l->ev)) {
        r = 0;
        goto _end;
    };

    l->ecount = 0;
    l->closing = 0;

    r = nai_spin_open(&l->lock, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_cond_open(&l->cond, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_mutex_open(&l->mux, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_evbase_open(&l->ev, optional, require, name);
    if (r < 0) {
        goto _end;
    };

    nai_evnode_set_cb(&l->post, nai_evloop_post_handler);
    r = nai_evnode_open(&l->post, l);
    if (r < 0) {
        goto _fail;
    };


    l->timer = nai_malloc(sizeof(*l->timer));
    if (l->timer == 0) {
        r = -1;
        goto _fail;
    };

    nai_evtimer_init(l->timer);
    r = nai_evtimer_open(l->timer, l->ev.ops, l->ev.ud);
    if (r < 0) {
        goto _fail;
    };


    /* setup extra pool */
    eltsize = (nai_evbase_feature(&l->ev) & NAI_EV_FEAT_ASYNC) ? 
        sizeof(nai_iobase_aio_t) : 
        sizeof(nai_iobase_nb_t);
    nai_fixedpool_init(&l->pooles, eltsize, 0, 0);
    nai_fixedpool_init(&l->poolem, eltsize, 0, 0);

    /* setup ultra pool */
    eltsize = sizeof(nai_async_load_t);
    nai_fixedpool_init(&l->poolus, eltsize, 0, 0);
    nai_fixedpool_init(&l->poolum, eltsize, 0, 0);

    /* set mark of backwork */
    l->backwork = (l->threads != 0) || (nai_evbase_get_port(&l->ev) != 0);

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_evloop_close_impl(l);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_evloop_rearm(nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t mt;
#if (NAI_HAVE_FORK)
    nai_int_t n;
    nai_evnode_t* h;
    nai_list_entry_t* e;
    nai_evmsg_port_t* p;
#endif


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    mt = nai_evloop_is_concurrent(l);
    if (mt) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

#if (NAI_HAVE_FORK)
    r = nai_evbase_rearm(&l->ev);
    if (r < 0) {
        goto _end;
    };

    /* reset all event nodes */
    for (n = 1; n < l->fcount; n ++) {
        h = l->fds[n];
        h->ent->key = -1;
    };

    /* reset timer */
    if (l->timer) {
        nai_evtimer_close(l->timer);
        r = nai_evtimer_open(l->timer, l->ev.ops, l->ev.ud);
        if (r < 0) {
            goto _end;
        };

        /* make sure timer will be seted */
        l->timechg[1] = 1;
    };

    /* rearm all ports */
    e = l->ports.next;
    for ( ; e != &l->ports; ) {
        p = nai_containof(e, nai_evmsg_port_t, entl);
        e = e->next;
        r = nai_evmsg_port_rearm(p);
        if (r < 0) {
            goto _end;
        };
    };

    nai_spin_lock(&l->lock);

    /* rearm all event nodes */
    for (n = 1; n < l->fcount; n ++) {
        h = l->fds[n];
        h->key = 0;
        if (h->fd != NAI_FD_INVALID) {
            nai_evloop_add_signals(l, h->ent, 1<<NAI_EV_SIG_ADD);
        };
    };

    /* reset */
    l->fcount = 1;

    nai_spin_unlock(&l->lock);

#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_evloop_close(nai_evloop_t* l)
{
    nai_int_t r;


    if (l->tid) {
        if (l->tid == nai_thread_id()) {
            l->closing = 1;
            r = 0;
            goto _end;
        } else {
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };
    };

    r = nai_evloop_close_impl(l);
    if (r < 0) {
        goto _end;
    };

    nai_free(l);
    r = 0;

_end:
    return r;
};



nai_int_t nai_evloop_add_watch(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t name)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_evloop_watch_t* w;


    if (name < 0 || 
        name >= (nai_int_t)nai_countof(l->stages)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (h->ent != 0) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };


    w = (nai_evloop_watch_t*)nai_evloop_pool_alloc(l, 1);
    if (w == 0) {
        r = -1;
        goto _end;
    };

    w->locked = 1;
    w->stage = name;
    w->h = h;
    h->extend = NAI_EV_EXTEND_WATCH;
    h->extval = (uint8_t)name;

    /* move to list of setuping before add handle */
    nai_spin_lock(&l->lock);
    nai_list_insert_tail(&l->setups, &w->ent);
    nai_spin_unlock(&l->lock);


    /* add handle */
    r = nai_evloop_add_handle(l, h);
    if (r < 0) {
        ec = nai_errno;
        w->h = 0;
        h->extend = 0;
        h->extval = 0;
        nai_spin_lock(&l->lock);
        nai_list_entry_remove(&w->ent);
    };


    mt = r;
    nai_spin_lock(&l->lock);

    if (w->h == 0) {
        /* is already removed */
        nai_fixedpool_free(&l->poolm, w);
        nai_spin_unlock(&l->lock);
    } else if (!mt) {
        w->locked = 0;
        nai_list_entry_remove(&w->ent);
        nai_list_insert_tail(&l->stages[name], &w->ent);
        nai_spin_unlock(&l->lock);
    } else {
        w->locked = 0;
        nai_list_entry_remove(&w->ent);
        nai_list_insert_tail(&l->watchs, &w->ent);
        nai_evloop_add_signals_locked(l, 
            l->post.ent, 1<<(NAI_EV_SIG_NOTIFY_1), 0);
    };

    if (r < 0) {
        nai_errno = ec;
    };


_end:
    return r;
};


nai_int_t nai_evloop_del_watch(nai_evloop_t* l, nai_evnode_t* h)
{
    nai_int_t r;
    nai_int_t name;
    nai_int_t locked;
    nai_list_t* list;
    nai_list_entry_t* e;
    nai_evloop_watch_t* w;


    r = nai_evloop_del_handle(l, h);
    if (r < 0) {
        goto _end;
    };

    locked = 0;
    name = h->extval;
    list = &l->stages[name];
    for (;;) {
        e = list->next;
        for ( ; e != list; e = e->next) {
            w = (nai_evloop_watch_t*)e;
            if (w->h == h) {
                break;
            };
        };
        if (e != list) {
            break;
        };

        /* get next list */
        if (list == &l->setups) {
            break;
        } else if (list != &l->watchs) {
            list = &l->watchs;
        } else {
            list = &l->setups;
        };

        if (!locked) {
            locked = 1;
            nai_spin_lock(&l->lock);
        };
    };

    if (e == list) {
        assert(0);
    } else if (w->locked) {
        w->h = 0;
    } else {
        nai_list_entry_remove(&w->ent);

        if (locked == 0) {
            locked = 1;
            nai_spin_lock(&l->lock);
        };

        nai_fixedpool_free(&l->poolm, w);
    };

    if (locked) {
        nai_spin_unlock(&l->lock);
    };

    h->extend = 0;
    h->extval = 0;
    r = 0;


_end:
    return r;
};


nai_int_t nai_evloop_add_handle(
    nai_evloop_t* l, nai_evnode_t* h)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t exist;
    nai_int_t events;
    nai_int_t sigbits;
    nai_evloop_ent_t* e;


    if (h == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    exist = h->ent != 0;
    if (exist) {
        /* the node is already opened.
         * we can reopening with file handle,
         * this is useful for complex connection processes
         */
        e = h->ent;
        if (e->key != -1 || h->fd == NAI_FD_INVALID) {
            /* we can't reopen 
             * if no file handle or previous opened with file handle,
             */
            nai_errno = EEXIST;
            r = -1;
            goto _end;
        };

        mt = nai_evloop_is_concurrent(l);
        sigbits = 0;

    } else {

        mt = nai_evloop_is_concurrent(l);
        e = (nai_evloop_ent_t*)nai_evloop_pool_alloc(l, mt);
        if (e == 0) {
            r = -1;
            goto _end;
        };

        /* initialize ent */
        e->h = h;
        e->key = -1;
        e->mt = mt;
        e->events = 0;
        e->mail = 0;
        e->sig = 0;
        e->signext = 0;
        e->ttype = 0;
        e->timer.value = 0;

        /* add handle */
        h->loop = l;
        h->ent = e;
        sigbits = (h->st.sigbits << NAI_EV_SIG_NOTIFY);
    };

    e->catching = h->st.catching;
    e->priority = h->st.priority;

    if (mt) {
        if (h->fd != NAI_FD_INVALID) {
            sigbits |= (1 << NAI_EV_SIG_ADD);
        };
        if (h->timeval && !exist) {
            sigbits |= (1 << NAI_EV_SIG_TIMER);
        };
        if (sigbits) {
            nai_spin_lock(&l->lock);
            r = nai_evloop_add_signals_locked(l, e, sigbits, 1);
            if (r < 0) {
                /* fixme: ignored error */
            };
        };
    } else {
        if (h->fd != NAI_FD_INVALID) {
            events = h->st.seted;
            r = nai_evloop_add(l, h, events);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
        if (sigbits) {
            nai_spin_lock(&l->lock);
            r = nai_evloop_add_signals_locked(l, e, sigbits, 0);
            if (r < 0) {
                ec = nai_errno;
                goto _fail_del;
            };
        };
        if (h->timeval && !exist) {
            r = nai_evloop_update_timer(l, h, 0);
            if (r < 0) {
                ec = nai_errno;
                goto _fail_del;
            };
        };
    };

    if (exist == 0) {
        nai_atomic32_inc(&l->ecount);
    };
    h->st.sigbits = 0;
    r = mt;

_end:
    return r;

_fail_del:
    if (h->fd != NAI_FD_INVALID) {
        nai_evloop_del(l, h);
    };
    if (sigbits && !mt) {
        nai_spin_lock(&l->lock);
        nai_evloop_remove_sigent(l, e);
        nai_spin_unlock(&l->lock);
    };

_fail:
    if (!exist) {
        h->loop = 0;
        h->ent = 0;
        nai_evloop_ent_free(l, e);
    };
    nai_errno = ec;
    return r;
};


nai_int_t nai_evloop_set_handle(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_evloop_ent_t* e;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (nai_evloop_is_concurrent(l)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    assert(h->ent);
    e = h->ent;

    if (e->key != -1) {
        r = nai_evbase_set(&l->ev, h, events);
    } else {
        if (h->fd == NAI_FD_INVALID) {
            nai_errno = EBADF;
            r = -1;
            goto _end;
        };

        r = nai_evloop_add(l, h, events);
    };

_end:
    return r;
};


nai_int_t nai_evloop_del_handle(
    nai_evloop_t* l, nai_evnode_t* h)
{
    nai_int_t r;
    nai_evloop_ent_t* e;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (nai_evloop_is_concurrent(l)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    assert(h->ent);
    e = h->ent;
    if (e->key != -1) {
        r = nai_evloop_del(l, h);
        if (r < 0) {
            goto _end;
        };
    };

    /* detach handle */
    e->h = 0;
    h->ent = 0;
    h->loop = 0;

    /* remove from signal queue and mail slot */
    if (e->sig || e->mail) {
        nai_spin_lock(&l->lock);

        /* remove from inprogress queue */
        if (e->sig) {
            nai_evloop_remove_sigent(l, e);
        };

        /* remove mail */
        if (e->mail) {
            nai_evloop_remove_mail(l, e);
        };

        nai_spin_unlock(&l->lock);
    };

    /* remove from time queue */
    if (e->timer.value) {
        e->timer.value = 0;
        nai_timequeue_remove(&l->timeq[e->ttype], &e->timer);
    };

    /* events not equal 0 means in event queue, don't free now  */
    if (e->events == 0) {
        nai_evloop_ent_free(l, e);
    };

    nai_atomic32_dec(&l->ecount);
    r = 0;

_end:
    return r;
};


nai_int_t nai_evloop_set_signals(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t sigbits)
{
    nai_int_t r;
    nai_evloop_ent_t* e;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    e = h->ent;
    assert(e);


    /* lock queue */
    nai_spin_lock(&l->lock);

    /* set signals */
    r = nai_evloop_add_signals_locked(l, e, sigbits, -1);


_end:
    return r;
};


nai_int_t nai_evloop_set_signal(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t sig)
{
    return nai_evloop_set_signals(l, h, (1<<sig));
};


nai_int_t nai_evloop_set_expire(
    nai_evloop_t* l, nai_evnode_t* h, uint64_t usec)
{
    nai_int_t r;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    r = nai_evloop_set_expire_impl(l, h, usec);

_end:
    return r;
};


nai_int_t nai_evloop_set_timeout(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t op, int64_t usec)
{
    nai_int_t r;
    uint64_t expire;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (usec == -1) {
        expire = 0;
    } else if (op == NAI_TIMEOP_ADD && h->timeval) {
        expire = h->timeval + usec;
    } else {
        if (h->st.ttype) {
            expire = nai_tick_to_usec() + usec;
        } else {
            nai_timecache_access();
            expire = nai_timecache_get_mtick() * 1000 + usec;
        };
        switch (op) {
        case NAI_TIMEOP_SET:
        case NAI_TIMEOP_ADD:
            break;
        case NAI_TIMEOP_MIN:
            if (h->timeval && h->timeval <= expire) {
                r = 1;
                goto _end;
            };
            break;
        case NAI_TIMEOP_MAX:
            if (h->timeval && h->timeval >= expire) {
                r = 1;
                goto _end;
            };
            break;
        default:
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    r = nai_evloop_set_expire_impl(l, h, expire);

_end:
    return r;
};



nai_int_t nai_evloop_in_dispatch(nai_evloop_t* l)
{
    return !nai_evloop_is_concurrent(l);
};


nai_int_t nai_evloop_wait(nai_evloop_t* l, nai_int_t until)
{
    nai_int_t r;
    nai_int_t stat;
    nai_int_t waited;
    nai_int_t wait_break;
    nai_int_t wait_running;


    switch (until) {
    case NAI_EV_WAIT_RUNNING:
        wait_break = 0;
        wait_running = 1;
        break;
    case NAI_EV_WAIT_BREAK:
        wait_break = 1;
        wait_running = 0;
        break;
    case NAI_EV_WAIT_CHANGED:
        wait_break = 1;
        wait_running = 1;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    waited = 0;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    nai_mutex_lock(&l->mux);

    l->wait_break += wait_break;
    l->wait_running += wait_running;

    for (;;) {
        if (l->tid == 0) {
            stat = (l->run != l->run_last) ? NAI_EV_WAIT_BREAK : 0;
            if (stat && waited && l->wait_break > wait_break) {
                nai_cond_signal(&l->cond);
            };
        } else {
            stat = NAI_EV_WAIT_RUNNING;
            if (waited && l->wait_running > wait_running) {
                nai_cond_signal(&l->cond);
            };
        };

        if (stat & until) {
            break;
        };

        nai_cond_wait(&l->cond, &l->mux);

        waited = 1;
    };

    l->wait_break -= wait_break;
    l->wait_running -= wait_running;

    /* update 'run_last' */
    if (l->wait_break <= 0 && stat & NAI_EV_WAIT_BREAK) {
        l->run_last = l->run;
    };

    nai_mutex_unlock(&l->mux);

    r = 0;

_end:
    return r;
};


nai_int_t nai_evloop_break(nai_evloop_t* l, int16_t code)
{
    nai_int_t r;
    nai_evnode_t* h;


    if (code < 0) {
        nai_errno = EINTR;
        r = -1;
        goto _end;
    };
    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    h = &l->post;
    h->st.code = code;

    /* send signal */
    r = nai_evloop_set_signal(l, h, NAI_EV_SIG_INTR);


_end:
    return r;
};



nai_int_t nai_evloop_queue(nai_evloop_t* l, nai_task_f cb, void* ud)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t empty;
    nai_int_t sigbits;
    nai_evloop_ent_t* e;
    nai_evloop_task_t* t;


    if (cb == 0) {
        nai_errno = EINTR;
        r = -1;
        goto _end;
    };
    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    e = l->post.ent;
    sigbits = (1<<NAI_EV_SIG_NOTIFY_0);


    /* lock queue */
    nai_spin_lock(&l->lock);


    /* alloc task entry */
    t = (nai_evloop_task_t*)nai_fixedpool_alloc(&l->poolm);
    if (t == 0) {
        r = -1;
        goto _fail;
    };

    /* add task entry */
    t->cb = cb;
    t->ud = ud;
    nai_list_insert_tail(&l->taskq, &t->ent);

    /* add signals */
    empty = nai_evloop_add_signals(l, e, sigbits);

    /* unlock queue */
    nai_spin_unlock(&l->lock);


    /* send signal to dispatch thread */
    if (empty && nai_evloop_is_concurrent(l)) {
        r = nai_evbase_signal(&l->ev);
        if (r < 0) {
            /* fixme: ignored error */
        };
    };

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_spin_unlock(&l->lock);
    nai_errno = ec;
    goto _end;
};


typedef struct nai_evloop_exec_s {
    nai_cond_t* wait;
    nai_task_f cb;
    void* ud;
    nai_int_t done;
    nai_int_t sult;
    nai_int_t error;
} nai_evloop_exec_t;


static nai_int_t nai_evloop_async_exec(void* ud)
{
    nai_int_t r;
    nai_int_t ec;
    nai_mutex_t* m;
    nai_evloop_exec_t* w;


    /* exec */
    w = (nai_evloop_exec_t*)ud;
    r = w->cb(w->ud);
    ec = r < 0 ? nai_errno : 0;


    /* notify */
    m = nai_thread_mutex();
    assert(m != 0);
    nai_mutex_lock(m);

    w->done = 1;
    w->sult = r;
    w->error = ec;
    nai_cond_signal(w->wait);

    nai_mutex_unlock(m);

    r = 0;

    return r;
};


nai_int_t nai_evloop_exec(nai_evloop_t* l, nai_task_f cb, void* ud)
{
    nai_int_t r;
    nai_mutex_t* m;
    nai_evloop_exec_t w;


    if (cb == 0) {
        nai_errno = EINTR;
        r = -1;
        goto _end;
    };
    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    if (!nai_evloop_is_concurrent(l)) {
        r = cb(ud);
    } else {
        m = nai_thread_mutex();
        if (m == 0) {
            r = -1;
            goto _end;
        };

        w.cb = cb;
        w.ud = ud;
        w.error = 0;
        w.done = 0;
        w.wait = nai_thread_local_cond();
        if (w.wait == 0) {
            r = -1;
            goto _end;
        };

        r = nai_evloop_queue(l, nai_evloop_async_exec, &w);
        if (r < 0) {
            goto _end;
        };

        /* wait */
        nai_mutex_lock(m);

        while (!w.done) {
            nai_cond_wait(w.wait, m);
        };

        nai_mutex_unlock(m);

        /* completed */
        r = w.sult;
        if (w.sult < 0) {
            nai_errno = w.error;
        };
    };

_end:
    return r;
};



static nai_rbnode_t** nai_evloop_find_slot(
    nai_rbtree_t* t, intptr_t key, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_evloop_slot_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_evloop_slot_t, node);
        if (e->key == key) {
            break;
        } else if (e->key >= key) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_rbnode_t* nai_evloop_max_slot(nai_rbtree_t* t, intptr_t key)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_rbnode_t* l = nai_rbtree_end(t);
    nai_evloop_slot_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_evloop_slot_t, node);
        if (e->key > key) {
            n = &parent->rb_left;
        } else {
            l = parent;
            n = &parent->rb_right;
        };
    };

    return l;
};


static nai_int_t nai_evloop_remove_mail(nai_evloop_t* l, nai_evloop_ent_t* e)
{
    nai_evloop_slot_t* s;
    nai_evloop_mail_t* m;
    nai_evloop_link_t* a;
    nai_list_entry_t* ent;
    nai_rbnode_t** n;


    n = nai_evloop_find_slot(&l->mails, (intptr_t)e, 0);
    if (n[0] == 0) {
        goto _end;
    };

    m = nai_containof(n[0], nai_evloop_mail_t, node);
    ent = m->list.next;
    for ( ; ent != &m->list; ) {
        a = nai_containof(ent, nai_evloop_link_t, entm);
        ent = ent->next;
        s = a->slot;
        nai_list_entry_remove(&a->entm);
        nai_list_entry_remove(&a->ents);
        nai_fixedpool_free(&l->poolm, a);

        /* remove empty slot */
        if (nai_list_is_empty(&s->list) && s->temp) {
            nai_rbtree_erase(&l->slots, &s->node);
            nai_fixedpool_free(&l->poolm, s);
        };
    };

    nai_rbtree_erase(&l->mails, &m->node);
    nai_fixedpool_free(&l->poolm, m);
    e->mail = 0;

_end:
    return 0;
};


nai_int_t nai_evloop_make_slot(nai_evloop_t* l, nai_int_t count)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t slot;
    nai_int_t start;
    nai_int_t end;
    nai_evloop_slot_t* s;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_rbnode_t* ub;


    if (count <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    /* lock */
    nai_spin_lock(&l->lock);


    while (1) {
        l->nexts ++;
        if (l->nexts < 0) {
            l->nexts = NAI_EV_SLOT_DYNAMIC + 1;
        };

        start = l->nexts;
        end = start + count;
        if (end < start) {
            continue;
        };

        ub = nai_evloop_max_slot(&l->slots, end - 1);
        if (ub != nai_rbtree_end(&l->slots)) {
            s = nai_containof(ub, nai_evloop_slot_t, node);
            if (s->key >= start) {
                l->nexts = (nai_int_t)s->key;
                continue;
            };
        };

        break;
    };

    r = start;
    slot = start;
    for ( ; slot < end; slot ++) {
        s = (nai_evloop_slot_t*)nai_fixedpool_alloc(&l->poolm);
        if (s == 0) {
            ec = nai_errno;
            r = -1;
            break;
        };

        s->key = slot;
        s->loop = l;
        s->temp = 0;
        nai_list_init(&s->list);
        n = nai_evloop_find_slot(&l->slots, slot, &parent);
        nai_rbtree_link(&l->slots, &s->node, parent, n);
        nai_rbtree_color(&l->slots, &s->node);
    };

    if (r >= 0) {
        l->nexts = end;
    } else {
        end = slot;
        slot = start;
        for ( ; slot < end; slot ++) {
            n = nai_evloop_find_slot(&l->slots, slot, 0);
            s = nai_containof(n[0], nai_evloop_slot_t, node);
            nai_rbtree_erase(&l->slots, &s->node);
            nai_fixedpool_free(&l->poolm, s);
        };
    };


    nai_spin_unlock(&l->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


nai_int_t nai_evloop_drop_slot(
    nai_evloop_t* l, nai_int_t slot, nai_int_t count)
{
    nai_int_t r;
    nai_int_t end;
    nai_evloop_slot_t* s;
    nai_rbnode_t** n;


    if (count <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    /* lock */
    nai_spin_lock(&l->lock);


    end = slot + count;
    for ( ; slot < end; slot ++) {
        n = nai_evloop_find_slot(&l->slots, slot, 0);
        if (n[0] != 0) {
            s = nai_containof(n[0], nai_evloop_slot_t, node);
            s->temp = 1;
            if (nai_list_is_empty(&s->list)) {
                nai_rbtree_erase(&l->slots, &s->node);
                nai_fixedpool_free(&l->poolm, s);
            };
        };
    };


    nai_spin_unlock(&l->lock);
    r = 0;

_end:
    return r;
};


nai_int_t nai_evloop_join_slot(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t slot, nai_int_t sig)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evloop_ent_t* e;
    nai_evloop_slot_t* s;
    nai_evloop_mail_t* m;
    nai_evloop_link_t* a;
    nai_rbnode_t** n1;
    nai_rbnode_t** n2;
    nai_rbnode_t* parent1;
    nai_rbnode_t* parent2;
    nai_list_entry_t* ent;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    e = h->ent;
    assert(e);

    /* check size */
    #define msize nai_align(sizeof(nai_evloop_data_t), 32)
    nai_static_assert(sizeof(*m) <= msize);
    nai_static_assert(sizeof(*s) <= msize);
    nai_static_assert(sizeof(*a) <= msize);
    #undef msize

    /* lock */
    nai_spin_lock(&l->lock);


    /* find */
    n1 = nai_evloop_find_slot(&l->mails, (intptr_t)e, &parent1);
    if (n1[0] == 0) {
        m = 0;
    } else {
        m = nai_containof(n1[0], nai_evloop_mail_t, node);

        /* lookup slot */
        ent = m->list.next;
        for ( ; ent != &m->list; ent = ent->next) {
            a = nai_containof(ent, nai_evloop_link_t, entm);
            if (a->slot->key == slot) {
                a->sig = sig;
                r = 0;
                goto _find;
            };
        };
    };

    n2 = nai_evloop_find_slot(&l->slots, slot, &parent2);
    if (n2[0] == 0) {
        s = 0;
    } else {
        s = nai_containof(n2[0], nai_evloop_slot_t, node);
    };


    /* allocate */
    a = (nai_evloop_link_t*)nai_fixedpool_alloc(&l->poolm);
    if (a == 0) {
        ec = nai_errno;
        goto _fail;
    };

    if (n1[0] == 0) {
        m = (nai_evloop_mail_t*)nai_fixedpool_alloc(&l->poolm);
        if (m == 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    if (n2[0] == 0) {
        s = (nai_evloop_slot_t*)nai_fixedpool_alloc(&l->poolm);
        if (s == 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


    /* setup */
    if (n1[0] == 0) {
        e->mail = 1;
        m->keyp = e;
        m->loop = l;
        nai_list_init(&m->list);
        nai_rbtree_link(&l->mails, &m->node, parent1, n1);
        nai_rbtree_color(&l->mails, &m->node);
    };

    if (n2[0] == 0) {
        s->key = slot;
        s->loop = l;
        s->temp = 1;
        nai_list_init(&s->list);
        nai_rbtree_link(&l->slots, &s->node, parent2, n2);
        nai_rbtree_color(&l->slots, &s->node);
    };

    a->slot = s;
    a->dest = e;
    a->type = 0;
    a->sig = sig;
    nai_list_insert_tail(&s->list, &a->ents);
    nai_list_insert_tail(&m->list, &a->entm);


_find:
    nai_spin_unlock(&l->lock);
    r = 0;

_end:
    return r;

_fail:
    nai_fixedpool_free(&l->poolm, a);

    if (n1[0] == 0 && m) {
        nai_fixedpool_free(&l->poolm, m);
    };
    if (n2[0] == 0 && s) {
        nai_fixedpool_free(&l->poolm, s);
    };

    nai_spin_unlock(&l->lock);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_evloop_part_slot(
    nai_evloop_t* l, nai_evnode_t* h, nai_int_t slot)
{
    nai_int_t r;
    nai_evloop_ent_t* e;
    nai_evloop_slot_t* s;
    nai_evloop_mail_t* m;
    nai_evloop_link_t* a;
    nai_rbnode_t** n;
    nai_list_entry_t* ent;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    e = h->ent;
    assert(e);


    /* lock */
    nai_spin_lock(&l->lock);


    /* lookup mail */
    n = nai_evloop_find_slot(&l->mails, (intptr_t)e, 0);
    if (n[0] == 0) {
        /* not exist */
        r = 0;
        goto _exit;
    };

    /* lookup slot */
    m = nai_containof(n[0], nai_evloop_mail_t, node);
    ent = m->list.next;
    for ( ; ent != &m->list; ent = ent->next) {
        a = nai_containof(ent, nai_evloop_link_t, entm);
        if (a->slot->key == slot) {
            break;
        };
    };
    if (ent == &m->list) {
        /* not exist */
        r = 0;
        goto _exit;
    };

    s = a->slot;

    /* remove it */
    nai_list_entry_remove(&a->ents);
    nai_list_entry_remove(&a->entm);
    nai_fixedpool_free(&l->poolm, a);

    /* remove empty slot */
    if (nai_list_is_empty(&s->list) && s->temp) {
        nai_rbtree_erase(&l->slots, &s->node);
        nai_fixedpool_free(&l->poolm, s);
    };

    /* remove empty mail */
    if (nai_list_is_empty(&m->list)) {
        nai_rbtree_erase(&l->mails, &m->node);
        nai_fixedpool_free(&l->poolm, m);
        e->mail = 0;
    };

    r = 0;

_exit:
    nai_spin_unlock(&l->lock);

_end:
    return r;
};


nai_int_t nai_evloop_make_func(
    nai_evloop_t* l, nai_int_t slot, nai_task_f cb, void* ud)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t func;
    nai_evloop_slot_t* s;
    nai_evloop_slot_t* m;
    nai_evloop_link_t* a;
    nai_evloop_ent_t* e;
    nai_evloop_func_t* f;
    nai_evnode_t* h;
    nai_fixedpool_t* pool;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t* i;


    if (cb == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    e = 0;

    /* check size */
    #define msize nai_align(sizeof(nai_evloop_data_t), 32)
    nai_static_assert(sizeof(*s) <= msize);
    nai_static_assert(sizeof(*a) <= msize);
    nai_static_assert(sizeof(*f) <= sizeof(e->timer));
    #undef msize

    /* lock */
    nai_spin_lock(&l->lock);

    /* allocate a task id */
    func = 1;
    m = &l->mail;
    i = m->list.next;
    for ( ; i != &m->list; i = i->next) {
        a = nai_containof(i, nai_evloop_link_t, entm);
        if (a->sig != func) {
            break;
        };

        func ++;
        if (func < 0) {
            func = 1;
        };
    };
    a = 0;

    /* lookup slot */
    n = nai_evloop_find_slot(&l->slots, slot, &parent);
    if (n[0] != 0) {
        s = nai_containof(n[0], nai_evloop_slot_t, node);
    } else {
        s = 0;
    };


    /* get pool */
    mt = nai_evloop_is_concurrent(l);
    pool = mt ? &l->poolm : &l->pools;

    /* alloc */
    e = (nai_evloop_ent_t*)nai_fixedpool_alloc(pool);
    if (e == 0) {
        goto _fail;
    };

    a = (nai_evloop_link_t*)nai_fixedpool_alloc(pool);
    if (a == 0) {
        goto _fail;
    };

    if (s == 0) {
        s = (nai_evloop_slot_t*)nai_fixedpool_alloc(&l->poolm);
        if (s == 0) {
            goto _fail;
        };

        s->key = slot;
        s->loop = l;
        s->temp = 1;
        nai_list_init(&s->list);
        nai_rbtree_link(&l->slots, &s->node, parent, n);
        nai_rbtree_color(&l->slots, &s->node);
    };

    /* because signal processing only accesses the node's callback function, 
     * we provide fake nodes that are consistent in memory layout to 
     * reduce memory
     */
    f = (nai_evloop_func_t*)&e->timer;
    h = (nai_evnode_t*)f;

    /* init entry */
    e->h = h;
    e->key = -1;
    e->mt = 1;
    e->events = 0;
    e->mail = 0;
    e->sig = 0;
    e->signext = 0;
    e->ttype = 0;
    e->timer.value = 0;

    /* init function */
    f->cb = nai_evloop_func_handler;
    f->func = cb;
    f->ud = ud;
    f->link = a;

    /* init link */
    a->dest = e;
    a->slot = s;
    a->type = 1;
    a->sig = func;
    nai_list_insert_tail(&s->list, &a->ents);
    nai_list_insert_before(i, &a->entm);


    /* unlock */
    nai_spin_unlock(&l->lock);
    r = func;


_end:
    return r;

_fail:
    ec = nai_errno;

    if (n[0] == 0 && s) {
        nai_fixedpool_free(&l->poolm, s);
    };
    if (a != 0) {
        nai_fixedpool_free(pool, a);
    };
    if (e != 0) {
        nai_fixedpool_free(pool, e);
    };

    nai_spin_unlock(&l->lock);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_evloop_drop_func(nai_evloop_t* l, nai_int_t func)
{
    nai_int_t r;
    nai_int_t empty;
    nai_evloop_slot_t* s;
    nai_evloop_slot_t* m;
    nai_evloop_link_t* a;
    nai_evloop_ent_t* e;
    nai_fixedpool_t* pool;
    nai_list_entry_t* i;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    /* lock */
    nai_spin_lock(&l->lock);


    /* find the link of bind */
    m = &l->mail;
    i = m->list.next;
    for ( ; i != &m->list; i = i->next) {
        a = nai_containof(i, nai_evloop_link_t, entm);
        if (a->sig == func) {
            break;
        };
        if (a->sig > func) {
            i = &m->list;
            break;
        };
    };
    if (i == &m->list) {
        /* not find */
        nai_spin_unlock(&l->lock);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    empty = 0;
    if (nai_evloop_is_concurrent(l)) {
        empty = nai_evloop_add_signals(l, a->dest, 1<<NAI_EV_SIG_NOTIFY_0);
    } else {
        s = a->slot;
        e = a->dest;

        /* detach */
        e->h = 0;

        /* get pool */
        pool = e->mt ? &l->poolm : &l->pools;

        /* remove from inprogress queue */
        if (e->sig) {
            nai_evloop_remove_sigent(l, e);
        };

        /* events not equal 0 means in event queue, don't free now  */
        if (e->events == 0) {
            nai_fixedpool_free(pool, e);
        };

        /* free link */
        nai_list_entry_remove(&a->ents);
        nai_list_entry_remove(&a->entm);
        nai_fixedpool_free(pool, a);

        /* free slot */
        if (nai_list_is_empty(&s->list) && s->temp) {
            nai_rbtree_erase(&l->slots, &s->node);
            nai_fixedpool_free(&l->poolm, s);
        };
    };

    /* unlock */
    nai_spin_unlock(&l->lock);


    /* send signal to dispatch thread */
    if (empty) {
        r = nai_evbase_signal(&l->ev);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_evloop_post(nai_evloop_t* l, nai_int_t slot)
{
    nai_int_t r;
    nai_int_t empty;
    nai_evloop_slot_t* s;
    nai_evloop_link_t* a;
    nai_rbnode_t** n;
    nai_list_entry_t* ent;


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    /* lock */
    nai_spin_lock(&l->lock);


    /* lookup slot */
    n = nai_evloop_find_slot(&l->slots, slot, 0);
    if (n[0] == 0) {
        /* not exist */
        nai_spin_unlock(&l->lock);
        r = 0;
        goto _end;
    };

    empty = 0;
    s = nai_containof(n[0], nai_evloop_slot_t, node);
    ent = s->list.next;
    for ( ; ent != &s->list; ent = ent->next) {
        a = nai_containof(ent, nai_evloop_link_t, ents);
        if (a->type == 0) {
            empty |= nai_evloop_add_signals(l, a->dest, 1<<a->sig);
        } else {
            empty |= nai_evloop_add_signals(l, a->dest, 1<<NAI_EV_SIG_INTERNAL);
        };
    };


    /* unlock */
    nai_spin_unlock(&l->lock);


    /* send signal to dispatch thread */
    if (empty && nai_evloop_is_concurrent(l)) {
        r = nai_evbase_signal(&l->ev);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};



static uint32_t nai_evloop_timenext(nai_evloop_t* l);
static nai_int_t nai_evloop_process_stage(nai_evloop_t* l, nai_int_t name);
static nai_int_t nai_evloop_process_events(
    nai_evloop_t* l, nai_evloop_ent_t* list);
static nai_int_t nai_evloop_process_timers(nai_evloop_t* l, nai_int_t w);
static nai_int_t nai_evloop_process_signals(
    nai_evloop_t* l, nai_int_t* intr, nai_int_t* intrcode);


static nai_int_t nai_evloop_dispatch_impl(nai_evloop_t* l, nai_int_t sigonly)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_int_t intr = 0;
    nai_int_t intrcode = 0;
    uint32_t timeout;
    nai_evloop_ent_t* list[2];


    if (!nai_evbase_is_opened(&l->ev)) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };


    nai_mutex_lock(&l->mux);

    if (l->tid != 0) {
        nai_mutex_unlock(&l->mux);
        nai_errno = EBUSY;
        r = -1;
        goto _end;
    };

    /* save current thread id */
    l->tid = nai_thread_id();

    /* start running */
    l->run ++;
    if (l->wait_running) {
        nai_cond_signal(&l->cond);
    };

    nai_mutex_unlock(&l->mux);


    /* process signals only? */
    if (sigonly) {
        /* process signals */
        (void)nai_evloop_process_signals(l, &intr, &intrcode);

        /* is interrupted */
        if (intr) {
            r = intrcode;
        } else {
            r = 0;
        };

        /* to disable warning for gcc */
        ec = 0;

        goto _done;
    };


    /* update time cache before enter loop */
    nai_timecache_update_i();

    /* event loop */
    for (;;) {

        if (l->closing) {
            r = 0;
            break;
        };


        /* process 'pre' stage */
        (void)nai_evloop_process_stage(l, NAI_EV_STAGE_PRE);


        /* get next time */
        timeout = nai_evloop_timenext(l);

        /* wait events */
        r = nai_evbase_wait(&l->ev, timeout);
        if (r < 0) {
            ec = nai_errno;
            if (ec != EINTR) {
                nai_log_alert(NAI_LOG_CORE, 
                    ec, "nai_evbase_wait() failed, backend is %s", 
                    nai_evloop_get_name(l));

                (void)nai_evloop_process_stage(l, NAI_EV_STAGE_POST);
                break;
            };
        };

        nai_timecache_update_i();

        /* process high resolution timers */
        (void)nai_evloop_process_timers(l, 1);

        /* process normal timers */
        (void)nai_evloop_process_timers(l, 0);


        /* fetch events */
        r = nai_evbase_fetch(&l->ev, list);
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NAI_LOG_CORE, 
                ec, "nai_evbase_fetch() failed, backend is %s", 
                nai_evloop_get_name(l));

            /* fallthrough */
        };

        /* process high priority events */
        (void)nai_evloop_process_events(l, list[1]);


        /* process 'post' stage */
        (void)nai_evloop_process_stage(l, NAI_EV_STAGE_POST);


        /* process low priority events */
        (void)nai_evloop_process_events(l, list[0]);


        /* if an error occurs, stop the loop */
        if (r < 0) {
            break;
        };


        /* process signals */
        (void)nai_evloop_process_signals(l, &intr, &intrcode);


        /* is interrupted */
        if (intr) {
            r = intrcode;
            break;
        };
    };


_done:
    nai_mutex_lock(&l->mux);

    l->tid = 0;

    if (l->wait_break) {
        nai_cond_signal(&l->cond);
    };

    nai_mutex_unlock(&l->mux);

    if (r < 0) {
        nai_errno = ec;
    };

    if (r >= 0 && l->closing) {
        n = nai_evloop_close(l);
        if (n < 0) {
            r = n;
        };
    };

_end:
    return r;
};


nai_int_t nai_evloop_dispatch(nai_evloop_t* l)
{
    return nai_evloop_dispatch_impl(l, 0);
};


nai_int_t nai_evloop_done(nai_evloop_t* l)
{
    return nai_evloop_dispatch_impl(l, 1);
};



static nai_int_t nai_evnode_call(nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_evnode_cb_f cb = h->cb;


    if (cb) {
        r = cb(h, events);
    } else {
        r = 0;
    };

    return r;
};


static uint32_t nai_evloop_timenext(nai_evloop_t* l)
{
    uint64_t now;
    uint64_t timeexpr;
    int64_t timenext;


#if (NAI_HAVE_TIMER_WIN32)
    now = 0;
#endif

    /* test whether there is a signal in the queue.
     * the access is not consistent protected, but because subsequent 
     * events trigger re-access, even if an error occurs, it will not cause 
     * the signal queue to not be processed.
     */
    if (l->sigq.count) {
        timenext = 0;
    } else {
        timeexpr = l->timenext[0];
        if (timeexpr == (uint64_t)-1 || 
            timeexpr > l->timenext[1]) {
            timeexpr = l->timenext[1];
        };

        if (timeexpr == (uint64_t)-1) {
            timenext = l->timeomax;
        } else {
            nai_timecache_update_i();
            now = nai_timecache_get_mtick() * 1000;

            timenext = timeexpr - now;
            if (timenext < 0) {
                timenext = 0;
            } else if (timenext > (int64_t)l->timeomax * 1000) {
                timenext = l->timeomax;
            } else {
                timenext = (timenext + 500) / 1000;
            };
        };
    };

    if (l->timechg[1] && l->timenext[1] != (uint64_t)-1) {
#if (NAI_HAVE_TIMER_WIN32)
        if (now == 0) {
            nai_timecache_update_i();
        };
#endif
        l->timechg[1] = 0;
        nai_evtimer_set(l->timer, l->timenext[1]);
    };

    return (uint32_t)timenext;
};


static nai_int_t nai_evloop_process_stage(nai_evloop_t* l, nai_int_t name)
{
    nai_int_t r;
    nai_list_t* list;
    nai_list_entry_t* e;
    nai_evnode_t* h;
    nai_evloop_watch_t* w;
    nai_evloop_watch_t* n;


    list = &l->stages[name];
    e = list->next;
    for ( ; e != list; ) {
        w = (nai_evloop_watch_t*)e;
        e = e->next;

        /* handle has detached, free ent */
        if (w->h == 0) {
            nai_evloop_pool_free(l, w, 1);
            continue;
        };

        /* lock the next entry */
        if (e != list) {
            n = (nai_evloop_watch_t*)e;
            n->locked = 1;
        };

        w->locked = 0;
        h = w->h;
        nai_evnode_call(h, nai_ev_stage_from(name));
    };

    r = 0;

    return r;
};


static nai_int_t nai_evloop_process_events(
    nai_evloop_t* l, nai_evloop_ent_t* list)
{
    nai_int_t events;
    nai_evnode_t* h;
    nai_evloop_ent_t* ent;


    while (list) {
        ent = list;
        list = list->next;

        /* handle has detached, free ent */
        h = ent->h;
        if (h == 0) {
            nai_evloop_ent_free(l, ent);
            continue;
        };

        events = ent->events;
        events &= h->st.seted | (
            NAI_EV_TIMEOUT | NAI_EV_ERROR | NAI_EV_VALUE_MASK);

        /* handle event 'except' separately */
        if (events & NAI_EV_EXCEPT) {
            nai_evnode_call(h, NAI_EV_EXCEPT);
            if (ent->h == 0) {
                nai_evloop_ent_free(l, ent);
                continue;
            };

            events &= ~NAI_EV_EXCEPT;
        };

        /* handle other events */
        ent->events = 0;
        if (events) {
            nai_evnode_call(h, events);
        };
    };

    return 0;
};


static nai_int_t nai_evloop_process_signals(
    nai_evloop_t* l, nai_int_t* intr, nai_int_t* intrcode)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t events;
    uint16_t sigexec;
    nai_evnode_t* h;
    nai_evloop_ent_t* e;
    nai_evloop_ent_t* ent;
    nai_evloop_ent_t* list = 0;


    /* test whether there is a signal in the queue.
     * the access is not consistent protected, but because subsequent 
     * events trigger re-access, even if an error occurs, it will not cause 
     * the signal queue to not be processed.
     */
    if (l->sigq.count > 0) {

        /* lock queue */
        nai_spin_lock(&l->lock);

        /* move queue to process list, and set event */
        e = l->sigq.list;
        while (e) {
            ent = e;
            e = e->signext;

            /* prevent free by nai_evloop_del_handle */
            ent->events = ent->sig;
            ent->sig = 0;
            ent->next = list;
            list = ent;
        };

        /* clear queue count */
        l->sigq.list = 0;
        l->sigq.count = 0;

        /* unlock queue */
        nai_spin_unlock(&l->lock);
    };

    /* process signals */
    while (list) {
        ent = list;
        list = list->next;

        sigexec = ent->events;
        for (n = 0; n < NAI_EV_SIG_MAX; n ++) {
            /* handle has detached, free ent */
            if (ent->h == 0) {
                break;
            };
            if (!(sigexec & (1<<n))) {
                continue;
            };

            h = ent->h;
            switch (n) {
            case NAI_EV_SIG_ADD:
                if (ent->key == -1) {
                    events = h->st.seted;
                    events &= NAI_EV_IOE|NAI_EV_ASYNC;
                    r = nai_evloop_add(l, h, events);
                    if (r < 0) {
                        nai_evnode_call(h, nai_ev_error_from(nai_errno));
                        if (ent->h == 0) {
                            break;
                        };

                        /* clear special bit which is used to do next step */
                        sigexec &= ~(1<<NAI_EV_SIG_INTERNAL);
                    };
                };
                break;
            case NAI_EV_SIG_INTR:
                *intr = 1;
                *intrcode = h->st.code;
                break;
            case NAI_EV_SIG_TIMER:
                nai_evloop_update_timer(l, h, 1);
                break;
            case NAI_EV_SIG_NOTIFY_0:
            case NAI_EV_SIG_NOTIFY_1:
            case NAI_EV_SIG_NOTIFY_2:
            case NAI_EV_SIG_NOTIFY_3:
            case NAI_EV_SIG_INTERNAL:
                nai_evnode_call(h, nai_ev_notify_from(n-NAI_EV_SIG_NOTIFY));
                break;
            default:
                break;
            };

            /* check the remaining events */
            if (!(sigexec & ((uint16_t)-1 << (n+1)))) {
                break;
            };
        };
        ent->events = 0;
        if (ent->h == 0) {
            nai_evloop_ent_free(l, ent);
        };
    };

    return 0;
}; 


static nai_int_t nai_evloop_process_timers(nai_evloop_t* l, nai_int_t w)
{
    uint64_t now;
    uint64_t next = -1;
    nai_timenode_t* t;
    nai_timequeue_t* tq;
    nai_evloop_ent_t* ent;


    if (l->timenext[w] == (uint64_t)-1) {
        goto _end;
    };

    nai_timecache_access();
    now = w ? 
        (nai_timecache_get_utick()) : 
        (nai_timecache_get_mtick() * 1000);

    tq = &l->timeq[w];
    t = nai_timequeue_begin(tq);
    while (t != nai_timequeue_end(tq)) {
        ent = nai_containof(t, nai_evloop_ent_t, timer);
        t = nai_timequeue_next(t);
        if (ent->timer.value > now) {
            next = ent->timer.value;
            break;
        };

        nai_timequeue_remove(tq, &ent->timer);
        ent->timer.value = 0;
        if (ent->h != 0) {
            nai_evnode_call(ent->h, NAI_EV_TIMEOUT);
        };

        /* if the time queue is changed, then we stop process */
        if (t != nai_timequeue_begin(tq)) {
            t = nai_timequeue_begin(tq);
            if (t != nai_timequeue_end(tq)) {
                ent = nai_containof(t, nai_evloop_ent_t, timer);
                next = ent->timer.value;
            };
            break;
        };
    };

    /* update next time */
    if (l->timenext[w] != next) {
        l->timenext[w] = next;
        l->timechg[w] = 1;
    };

_end:
    return 0;
};



void* nai_evloop_extra_alloc(nai_evloop_t* l, nai_int_t* pmt)
{
    void* r;
    nai_int_t ec;
    nai_int_t mt;


    if (l->closing) {
        nai_errno = NAI_ECLOSED;
        r = 0;
        goto _end;
    };

    mt = nai_evloop_is_concurrent(l);
    if (pmt) {
        pmt[0] = mt;
    };
    if (!mt) {
        r = nai_fixedpool_alloc(&l->pooles);
    } else {
        nai_spin_lock(&l->lock);

        r = nai_fixedpool_alloc(&l->poolem);
        if (r == 0) {
            ec = nai_errno;
        };

        nai_spin_unlock(&l->lock);

        if (r == 0) {
            nai_errno = ec;
        };
    };

_end:
    return r;
};


nai_int_t nai_evloop_extra_free(nai_evloop_t* l, void* e, nai_int_t mt)
{
    nai_int_t r;


    if (l->closing) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (!mt) {
        nai_fixedpool_free(&l->pooles, e);
    } else {
        nai_spin_lock(&l->lock);
        nai_fixedpool_free(&l->poolem, e);
        nai_spin_unlock(&l->lock);
    };

    r = 0;

_end:
    return r;
};


void* nai_evloop_ultra_alloc(nai_evloop_t* l, nai_int_t* pmt)
{
    void* r;
    nai_int_t ec;
    nai_int_t mt;


    if (l->closing) {
        nai_errno = NAI_ECLOSED;
        r = 0;
        goto _end;
    };

    mt = nai_evloop_is_concurrent(l);
    if (pmt) {
        pmt[0] = mt;
    };
    if (!mt) {
        r = nai_fixedpool_alloc(&l->poolus);
    } else {
        nai_spin_lock(&l->lock);

        r = nai_fixedpool_alloc(&l->poolum);
        if (r == 0) {
            ec = nai_errno;
        };

        nai_spin_unlock(&l->lock);

        if (r == 0) {
            nai_errno = ec;
        };
    };

_end:
    return r;
};


nai_int_t nai_evloop_ultra_free(nai_evloop_t* l, void* e, nai_int_t mt)
{
    nai_int_t r;


    if (l->closing) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (!mt) {
        nai_fixedpool_free(&l->poolus, e);
    } else {
        nai_spin_lock(&l->lock);
        nai_fixedpool_free(&l->poolum, e);
        nai_spin_unlock(&l->lock);
    };

    r = 0;

_end:
    return r;
};


