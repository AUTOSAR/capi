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
/// @file       nai_osloop.c
/// @brief      
/// @details
/// @date       2023-10-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_osloop.h"
#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


typedef struct nai_osloop_global_s nai_osloop_global_t;


struct nai_osloop_global_s {
    nai_osloop_t* loop;
    nai_atomic32_t cs_lock;
    nai_atomic32_t cs_loop;
    nai_atomic32_t cs_cond;
#if (__darwin__)
    nai_atomic32_t forked;
#endif
};


static nai_osloop_global_t nai_osloop = { 0 };


#if (__darwin__)


static void nai_osloop_sig_handle(void* arg)
{
    nai_osloop_t* l;


    l = (nai_osloop_t*)arg;
    nai_mapi.CFRunLoopStop(l->loop);

    return;
};


static nai_thread_api nai_osloop_thread(void* arg)
{
    nai_mutex_t* m;
    nai_osloop_t* l;
    nai_osloop_global_t* g;
    CFRunLoopSourceContext ctx;


    /* init */
    nai_memset(&ctx, 0, sizeof(ctx));
    ctx.info = arg;
    ctx.perform = nai_osloop_sig_handle;

    m = nai_thread_mutex();
    l = (nai_osloop_t*)arg;
    l->loop = nai_mapi.CFRunLoopGetCurrent();
    l->sig = nai_mapi.CFRunLoopSourceCreate(0, 0, &ctx);
    if (l->sig == 0) {
        nai_mutex_lock(m);
        l->loop = 0;
        l->started = 1;
        l->error = ENOMEM;
        nai_mutex_unlock(m);
        nai_cond_signal(l->cond);
        goto _end;
    };

    nai_mapi.CFRunLoopAddSource(
        l->loop, l->sig, *nai_mapi.kCFRunLoopDefaultMode);


    /* wake up the creation thread */
    nai_mutex_lock(m);
    l->started = 1;
    l->error = 0;
    nai_mutex_unlock(m);
    nai_cond_signal(l->cond);


    /* loop */
    g = &nai_osloop;
    for (;;) {
        nai_mapi.CFRunLoopRun();
        if (l->done) {
            break;
        };

        /* make sure the fork thread is waiting */
        nai_mutex_lock(m);
        nai_mutex_unlock(m);

        /* wake up the fork thread */
        assert(l->cond);
        nai_atomic32_lock(&g->cs_cond);
        nai_cond_signal(l->cond);
        nai_atomic32_unlock(&g->cs_cond);

        /* suspend until fork finished */
        nai_yield();
        nai_atomic32_lock(&g->cs_loop);
        nai_atomic32_unlock(&g->cs_loop);
    };


    /* done */
    nai_mapi.CFRunLoopRemoveSource(
        l->loop, l->sig, *nai_mapi.kCFRunLoopDefaultMode);
    nai_mapi.CFRelease(l->sig);
    l->sig = 0;

_end:
    return 0;
};


static nai_int_t nai_osloop_wait(nai_osloop_t* l)
{
    (void)l;
    return 0;
};


static nai_int_t nai_osloop_break(nai_osloop_t* l, nai_int_t pause)
{
    nai_int_t r;


    l->done = !pause;
    nai_mapi.CFRunLoopSourceSignal(l->sig);
    nai_mapi.CFRunLoopWakeUp(l->loop);
    r = 0;

    return r;
};


nai_osloop_t* nai_osloop_get()
{
    nai_int_t r;
    nai_int_t ec;
    nai_mutex_t* m;
    nai_cond_t* c;
    nai_osloop_t* l;
    nai_osloop_t* p;
    nai_osloop_global_t* g;


    if (nai_mapi.inited == 0) {
        nai_mapi_init();
    };
    if (nai_mapi.error) {
        nai_errno = nai_mapi.error;
        p = 0;
        goto _end;
    };


    g = &nai_osloop;
    nai_atomic32_lock(&g->cs_lock);


    l = g->loop;
    if (l == 0 || l->term) {
        /* is forked, CoreFoundation can not be used in forked process */
        if (g->forked) {
            ec = EBUSY;
            goto _fail;
        };

        l = 0;

        m = nai_thread_mutex();
        if (m == 0) {
            ec = nai_errno;
            goto  _fail;
        };
        c = nai_thread_local_cond();
        if (c == 0) {
            ec = nai_errno;
            goto _fail;
        };

        l = (nai_osloop_t*)nai_malloc(sizeof(*l));
        if (l == 0) {
            ec = nai_errno;
            goto _fail;
        };

        nai_thread_init(&l->thread);
        l->loop = 0;
        l->sig = 0;
        l->cond = c;
        l->refs = 0;
        l->started = 0;
        l->done = 0;
        l->term = 0;
        l->error = 0;

        r = nai_thread_create(&l->thread, 0, nai_osloop_thread, l);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };


        nai_mutex_lock(m);

        while (!l->started) {
            nai_cond_wait(c, m);
        };

        nai_mutex_unlock(m);


        if (l->error) {
            ec = l->error;
        };

        g->loop = l;
    };

    l->refs ++;


    nai_atomic32_unlock(&g->cs_lock);

    p = l;

_end:
    return p;

_fail:
    nai_atomic32_unlock(&g->cs_lock);

    if (l != 0) {
        nai_free(l);
    };

    nai_errno = ec;
    p = 0;
    goto _end;
};


nai_int_t nai_osloop_release(nai_osloop_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    g = &nai_osloop;
    nai_atomic32_lock(&g->cs_lock);


    /* release loop */
    l = p;
    if (l == 0) {
        ec = EINVAL;
        r = -1;
    } else {
        l->refs --;
        if (l->refs > 0) {
            l = 0;
        } else if (l == g->loop) {
            g->loop = 0;
        } else {
            /* nothing */
            ;
        };

        r = 0;
    };


    nai_atomic32_unlock(&g->cs_lock);


    /* free loop */
    if (r < 0) {
        nai_errno = ec;
    } else if (l != 0) {
        if (!l->term) {
            nai_osloop_break(l, 0);
            nai_thread_join(&l->thread);
        };
        nai_free(l);
    } else {
        /* nothing */
        ;
    };

    return r;
};



#else


static nai_thread_api nai_osloop_thread(void* arg)
{
    nai_int_t r;
    nai_mutex_t* m;
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    /* loop */
    m = nai_thread_mutex();
    g = &nai_osloop;
    l = (nai_osloop_t*)arg;
    for (;;) {
        r = nai_evloop_dispatch(l->loop);
        if (r == 0) {
            break;
        };

        /* make sure the fork thread is waiting */
        nai_mutex_lock(m);
        nai_mutex_unlock(m);

        /* wake up the fork thread */
        assert(l->cond);
        nai_atomic32_lock(&g->cs_cond);
        nai_cond_signal(l->cond);
        nai_atomic32_unlock(&g->cs_cond);

        /* suspend until fork finished */
        nai_yield();
        nai_atomic32_lock(&g->cs_loop);
        nai_atomic32_unlock(&g->cs_loop);
    };


    /* done */
    nai_evloop_done(l->loop);


    return 0;
};


static nai_int_t nai_osloop_wait(nai_osloop_t* l)
{
    return nai_evloop_wait(l->loop, NAI_EV_WAIT_RUNNING);
};


static nai_int_t nai_osloop_break(nai_osloop_t* l, nai_int_t pause)
{
    return nai_evloop_break(l->loop, pause);
};


nai_osloop_t* nai_osloop_get()
{
    nai_int_t r;
    nai_int_t ec;
    nai_osloop_t* l;
    nai_osloop_t* p;
    nai_osloop_global_t* g;


    g = &nai_osloop;
    nai_atomic32_lock(&g->cs_lock);


    l = g->loop;
    if (l == 0 || l->term) {
        l = (nai_osloop_t*)nai_malloc(sizeof(*l));
        if (l == 0) {
            goto _fail;
        };

        nai_thread_init(&l->thread);
        l->refs = 0;
        l->term = 0;
        l->cond = 0;
        l->loop = nai_evloop_new();
        if (l->loop == 0) {
            goto _fail;
        };

        r = nai_evloop_open(l->loop, 0, 0, 0);
        if (r < 0) {
            goto _fail;
        };

        r = nai_thread_create(&l->thread, 0, nai_osloop_thread, l);
        if (r < 0) {
            goto _fail;
        };

        nai_evloop_wait(l->loop, NAI_EV_WAIT_RUNNING);

        /* set new loop */
        g->loop = l;
    };

    l->refs ++;


    nai_atomic32_unlock(&g->cs_lock);

    p = l;

_end:
    return p;

_fail:
    ec = nai_errno;
    if (l != 0) {
        if (l->loop) {
            nai_evloop_close(l->loop);
        };
        nai_free(l);
    };
    nai_atomic32_unlock(&g->cs_lock);
    nai_errno = ec;
    p = 0;
    goto _end;
};


nai_int_t nai_osloop_release(nai_osloop_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    g = &nai_osloop;
    nai_atomic32_lock(&g->cs_lock);


    /* release loop */
    l = p;
    if (l == 0) {
        ec = EINVAL;
        r = -1;
    } else {
        l->refs --;
        if (l->refs > 0) {
            l = 0;
        } else if (g->loop == l) {
            g->loop = 0;
        } else {
            /* nothing */
            ;
        };

        r = 0;
    };


    nai_atomic32_unlock(&g->cs_lock);


    /* free loop */
    if (r < 0) {
        nai_errno = ec;
    } else if (l != 0) {
        if (!l->term) {
            nai_evloop_break(l->loop, 0);
            nai_thread_join(&l->thread);
        };
        nai_evloop_close(l->loop);
        nai_free(l);
    } else {
        /* nothing */
        ;
    };

    return r;
};


#endif



nai_int_t nai_osloop_lock()
{
    nai_int_t r;


    r = nai_atomic32_lock(&nai_osloop.cs_lock);

    return r;
};


nai_int_t nai_osloop_unlock()
{
    nai_int_t r;


    r = nai_atomic32_unlock(&nai_osloop.cs_lock);

    return r;
};



#if (NAI_HAVE_FORK)


static void nai_osloop_fork_pre()
{
    nai_cond_t* c;
    nai_mutex_t* m;
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    /* enter fork */
    g = &nai_osloop;
    nai_atomic32_lock(&g->cs_lock);


    m = nai_thread_mutex();
    if (m == 0) {
        assert(0);
        goto  _end;
    };

    c = nai_thread_local_cond();
    if (c == 0) {
        assert(0);
        goto  _end;
    };


    /* pause the loop thread */
    l = g->loop;
    if (l != 0 && !l->term) {
        l->cond = c;

        /* lock to prevent the loop thread running */
        nai_atomic32_lock(&g->cs_loop);


        /* lock and suspend the loop thread */
        nai_mutex_lock(m);

        /* wakup the loop thread */
        nai_osloop_break(l, 1);

        /* wait the signal from the loop thread */
        nai_cond_wait(l->cond, m);

        /* make sure the loop thread is suspended */
        nai_atomic32_lock(&g->cs_cond);
        nai_atomic32_unlock(&g->cs_cond);

        /* unlock */
        nai_mutex_unlock(m);
    };


_end:
    return;
};


static void nai_osloop_fork_parent()
{
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    /* run the loop thread */
    g = &nai_osloop;
    l = g->loop;
    if (l != 0 && !l->term) {
        /* unlock to run the loop thread */
        nai_atomic32_unlock(&g->cs_loop);

        /* wait the loop is running */
        nai_osloop_wait(l);
    };

    /* exit fork */
    nai_atomic32_unlock(&g->cs_lock);
};


static void nai_osloop_fork_child()
{
    nai_osloop_t* l;
    nai_osloop_global_t* g;


    /* run the loop thread */
    g = &nai_osloop;
    l = g->loop;
    if (l != 0 && !l->term) {
        /* unlock to run the loop thread */
        nai_atomic32_unlock(&g->cs_loop);

        /* mark the loop is dead */
        l->term = 1;
    };

#if (__darwin__)
    /* mark forked, CoreFoundation can not be used in forked process */
    if (nai_mapi.inited) {
        g->forked = 1;
    };
#endif
    /* exit fork */
    nai_atomic32_unlock(&g->cs_lock);
};


void nai_osloop_fork(nai_int_t at)
{
    switch (at) {
    case 0: /* at prepare */
        nai_osloop_fork_pre();
        break;
    case 1: /* at parent */
        nai_osloop_fork_parent();
        break;
    case 2: /* at child */
        nai_osloop_fork_child();
        break;

    default:
        break;
    };

    return;
};


#endif


