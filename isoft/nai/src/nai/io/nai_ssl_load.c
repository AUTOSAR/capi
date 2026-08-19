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
/// @file       nai_ssl_load.c
/// @brief      
/// @details
/// @date       2022-10-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"
#include "nai/io/nai_async_load.h"
#include "nai/io/nai_iobase.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"



#if (NAI_HAVE_SSL)


static nai_int_t nai_ssl_is_blocking(nai_ssl_t* s)
{
    nai_int_t r;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops->get_opt) {
        r = ops->get_opt(s->ud, NAI_SSL_OPT_BLOCKING);
    } else {
        r = 0;
    };

    return r;
};


static nai_int_t nai_ssl_load_handle(nai_async_load_t* l, nai_int_t event)
{
    nai_int_t r;
    nai_int_t blocking;
    nai_ssl_t* s;
    nai_ssl_ops_t* ops;


    s = (nai_ssl_t*)l->ud;
    s->loadstat = l->stat;

    blocking = nai_ssl_is_blocking(s);
    if (blocking) {
        /* do singal */
        r = nai_async_load_signal(l);
        if (r < 0) {
            /* nothing */
            ;
        };
        goto _end;
    };

    ops = s->ops;
    if (ops && ops->emit) {
        ops->emit(s->ud, event);
    };
    r = 0;

_end:
    return r;
};


static nai_int_t nai_ssl_load_create(nai_ssl_t* s)
{
    nai_int_t r;
    nai_ssl_ops_t* ops;
    nai_evloop_t* loop;


    ops = s->ops;
    if (ops->get_loop) {
        loop = ops->get_loop(s->ud);
    } else {
        loop = 0;
    };

    s->load = nai_async_load_create(loop, nai_ssl_load_handle, s);
    if (s->load == 0) {
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static intptr_t nai_ssl_load_start(
    nai_ssl_t* s, nai_buflist_t* list, size_t bytes)
{
    intptr_t r;
    nai_int_t blocking;
    nai_async_load_t* l;
    nai_async_load_sendv_f sendfn = (nai_async_load_sendv_f)nai_ssl_writev;


    assert(s->loadstat == NAI_ASYNC_LOAD_DONE || 
        s->loadstat == NAI_ASYNC_LOAD_UNINIT);

    /* let write functions ignore load */
    s->loadstat = NAI_ASYNC_LOAD_UNINIT;


    /* try output */
    l = s->load;
    r = nai_async_load_tryout(l, 
        list, bytes, !s->tls, sendfn, s, NAI_BUFV_MAX);
    if (r != NAI_INTPTR_T_MAX) {
        if (l != 0) {
            s->loadstat = l->stat;
        };
        goto _end;
    };


    /* initialize loadop */
    if (l == 0) {

        r = nai_ssl_load_create(s);
        if (r < 0) {
            goto _end;
        };

        l = s->load;
    };


    blocking = nai_ssl_is_blocking(s);

    /* start load */
    r = nai_async_load_start(l, 
        list, bytes, !s->tls, sendfn, s, NAI_BUFV_MAX, blocking);


    /* update stat */
    s->loadstat = l->stat;


_end:
    return r;
};


static intptr_t nai_ssl_load_flush(nai_ssl_t* s, nai_buflist_t* list)
{
    intptr_t r;
    nai_async_load_t* l;
    nai_async_load_sendv_f sendfn = (nai_async_load_sendv_f)nai_ssl_writev;


    assert(s->loadstat == NAI_ASYNC_LOAD_COMPLETED);

    /* let write functions ignore load */
    s->loadstat = NAI_ASYNC_LOAD_UNINIT;

    /* flush */
    l = s->load;
    r = nai_async_load_flush(l, list, sendfn, s);

    /* update stat */
    s->loadstat = l->stat;


    return r;
};


static nai_int_t nai_ssl_load_drop(nai_ssl_t* s, intptr_t sult)
{
    nai_int_t r;
    nai_int_t ec;


    if (sult < 0) {
        ec = nai_errno;
    };

    nai_async_load_close(s->load);
    s->load = 0;
    s->loadstat = NAI_ASYNC_LOAD_UNINIT;

    if (sult < 0) {
        nai_errno = ec;
    };

    r = 0;

    return r;
};


nai_int_t nai_ssl_load_available(nai_ssl_t* s)
{
    nai_int_t r;
    nai_ssl_ops_t* ops;
    nai_evloop_t* loop;


    ops = s->ops;
    if (ops->get_loop) {
        loop = ops->get_loop(s->ud);
    } else {
        loop = 0;
    };

    if (loop == 0 && 
        nai_evloop_back_available(loop)) {
        r = 1;
    } else {
        r = 0;
    };

    return r;
};



intptr_t nai_ssl_load_and_flush(
    nai_ssl_t* s, nai_buflist_t* list, size_t bytes)
{
    intptr_t r;
    nai_int_t blocking;
    nai_async_load_t* l;


    switch (s->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
    case NAI_ASYNC_LOAD_DONE:
        r = nai_ssl_load_start(s, list, bytes);
        break;

    case NAI_ASYNC_LOAD_PENDING:
        blocking = nai_ssl_is_blocking(s);
        if (!blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };

        /* wait */
        l = (nai_async_load_t*)s->load;
        r = nai_async_load_wait(l);
        if (r < 0) {
            goto _end;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_COMPLETED:
        l = (nai_async_load_t*)s->load;
        if (l->error) {
            l->stat = NAI_ASYNC_LOAD_DONE;
            s->loadstat = l->stat;
            nai_errno = l->error;
            r = -1;
        } else {
            r = nai_ssl_load_flush(s, list);
        };

        if (s->loadfile == 0 && 
            s->loadstat == NAI_ASYNC_LOAD_DONE) {
            nai_ssl_load_drop(s, r);
        };
        break;

    default:
        assert(0);
        nai_errno = EINVAL;
        r = -1;
        break;
    };

_end:
    return r;
};


nai_int_t nai_ssl_load_discard(nai_ssl_t* s)
{
    nai_int_t r;
    nai_int_t blocking;
    nai_async_load_t* l;


    switch (s->loadstat) {
    case NAI_ASYNC_LOAD_PENDING:
        blocking = nai_ssl_is_blocking(s);
        if (!blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };

        /* wait */
        l = (nai_async_load_t*)s->load;
        r = nai_async_load_wait(l);
        if (r < 0) {
            goto _end;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_COMPLETED:
        l = (nai_async_load_t*)s->load;
        nai_async_load_discard(l);
        s->loadstat = l->stat;

        if (s->loadfile == 0 && 
            s->loadstat == NAI_ASYNC_LOAD_DONE) {
            nai_ssl_load_drop(s, 0);
        };
        break;

    default:
        break;
    };

    r = 0;

_end:
    return r;
};



#endif

