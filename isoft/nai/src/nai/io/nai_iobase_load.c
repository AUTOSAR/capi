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
/// @file       nai_iobase_load.c
/// @brief      
/// @details
/// @date       2022-06-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// load file


static nai_int_t nai_iobase_load_handle(nai_async_load_t* l, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_t* s;
    nai_iobase_ctx_t* p;


    p = (nai_iobase_ctx_t*)l->ud;
    p->loadstat = (uint8_t)l->stat;
    if (p->closed) {
        r = 0;
        goto _end;
    };


    s = p->io;
    if (s->st.blocking) {
        /* do singal */
        r = nai_iobase_ctx_signal(p);
        if (r < 0) {
            /* nothing */
            ;
        };
        goto _end;
    };

    /* unset blocked stat */
    if (l->sending && l->meth == NAI_ASYNC_LOAD_AIO) {
        r = nai_iobase_unblocked(s, NAI_EV_WRITE);
        if (r < 0) {
            if (!p->finalize) {
                events = nai_ev_error_from(nai_errno);
                goto _call;
            };
        };
    };


    /* test whether it is finalizable */
    if (p->finalize) {
        if (!nai_iobase_ctx_pending(p)) {
            events = NAI_EV_FINALIZE;
        } else {
            r = 0;
            goto _end;
        };
    } else {
        events &= s->st.mode;
        if (l->sending && l->error == ETIMEDOUT) {
            assert(l->stat == NAI_ASYNC_LOAD_COMPLETED);
            nai_iobase_load_and_flush(s, 0, 0, 0, 0);

            if (events) {
                events |= NAI_EV_TIMEOUT;
            };
        };
        if (events == 0) {
            r = 0;
            goto _end;
        };
    };


_call:
    /* user callback */
    if (s->cb) {
        s->cb(s, events);
    };
#if 0
    if (p->proto != NAI_IOBASE_CTX_NB) {
        if (s->cb) {
            s->cb(s, events);
        };
    } else {
        if (s->ev.cb) {
            s->ev.cb(&s->ev, events);
        };
    };
#endif

    r = 0;

_end:
    return r;
};


static intptr_t nai_iobase_load_start(
    nai_iobase_ctx_t* p, nai_buflist_t* list, size_t bytes, 
    nai_iobase_output_f sendfn, nai_int_t gather)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t vmax;
    nai_int_t message;
    nai_iobase_t* s;
    nai_async_load_t* l;


    assert(p->loadstat == NAI_ASYNC_LOAD_DONE || 
        p->loadstat == NAI_ASYNC_LOAD_UNINIT);

    /* let write functions ignore load */
    p->loadstat = NAI_ASYNC_LOAD_UNINIT;


    /* is message type */
    s = p->io;
    vmax = gather ? NAI_BUFV_MAX : 1;
    message = nai_iobase_is_message(s) || 
        nai_iobase_get_type(s) == NAI_IO_TYPE_DGRAM;


    /* setup for sendfile */
    l = p->load;
    if (l != 0) {
        if (nai_iobase_is_sock(s)) {
            l->fd = nai_iobase_get_fd(s);
            l->timeout = s->st.timeout[1];
            l->sendfile = s->st.sendfile;
        } else {
            l->fd = NAI_FD_INVALID;
        };
    };


    /* try output */
    r = nai_async_load_tryout(l, 
        list, bytes, message, (nai_async_load_sendv_f)sendfn, s, vmax);
    if (r != NAI_INTPTR_T_MAX) {
        if (l != 0) {
            p->loadstat = (uint8_t)l->stat;
        };
        goto _end;
    };


    /* initialize loadop */
    if (l == 0) {
        assert(l == 0);
        l = nai_async_load_create(p->loop, nai_iobase_load_handle, p);
        if (l == 0) {
            r = -1;
            goto _end;
        };

        p->load = l;

        /* setup for sendfile */
        if (nai_iobase_is_sock(s)) {
            l->fd = nai_iobase_get_fd(s);
            l->timeout = s->st.timeout[1];
            l->sendfile = s->st.sendfile;
        } else {
            l->fd = NAI_FD_INVALID;
        };
    };


    /* start load */
    r = nai_async_load_start(l, list, bytes, message, 
        (nai_async_load_sendv_f)sendfn, s, vmax, s->st.blocking);

    /* handling error */
    l->restore = 0;
    if (r < 0 && l->sending) {
        ec = nai_errno;
        if (ec == EINPROGRESS) {
            /* start timer */
            if (l->meth == NAI_ASYNC_LOAD_AIO) {
                nai_iobase_blocked(s, NAI_EV_WRITE);
            };
            /* unset event */
            if (s->ev.st.seted & NAI_EV_WRITE) {
                nai_evnode_set_event(&s->ev, NAI_EV_DEL, NAI_EV_WRITE);
                nai_errno = ec;
                l->restore = 1;
            };
        };
    };


    /* update stat */
    p->loadstat = (uint8_t)l->stat;


_end:
    return r;
};


static intptr_t nai_iobase_load_flush(
    nai_iobase_ctx_t* p, nai_buflist_t* list, nai_iobase_output_f sendfn)
{
    intptr_t r;
    nai_iobase_t* s;
    nai_async_load_t* l;


    assert(p->loadstat == NAI_ASYNC_LOAD_COMPLETED);

    /* let write functions ignore load */
    p->loadstat = NAI_ASYNC_LOAD_UNINIT;

    /* flush */
    s = p->io;
    l = p->load;
    r = nai_async_load_flush(l, list, (nai_async_load_sendv_f)sendfn, s);

    /* update stat */
    p->loadstat = (uint8_t)l->stat;


    return r;
};


static nai_int_t nai_iobase_load_done(nai_iobase_ctx_t* p, intptr_t sult)
{
    nai_int_t r;
    nai_int_t ec;
    nai_iobase_t* s;
    nai_async_load_t* l;


    if (sult < 0) {
        ec = nai_errno;
    };

    s = p->io;
    l = p->load;

    /* restore event */
    if (!nai_iobase_is_blocking(s) && 
        l->restore && (
        s->st.mode & NAI_IO_WRITE & ~s->ev.st.seted)) {
        nai_evnode_set_event(&s->ev, NAI_EV_ADD, NAI_IO_WRITE);
    };

    l->restore = 0;

    /* close */
    if (s->st.loadfile == 0) {
        nai_iobase_load_close(p);
    };

    if (sult < 0) {
        nai_errno = ec;
    };

    r = 0;

    return r;
};


intptr_t nai_iobase_load_and_flush(
    nai_iobase_t* s, nai_buflist_t* list, size_t bytes, 
    nai_iobase_output_f sendfn, nai_int_t gather)
{
    intptr_t r;
    nai_iobase_ctx_t* p;


    p = (nai_iobase_ctx_t*)s->st.ctx;
    switch (p->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
    case NAI_ASYNC_LOAD_DONE:
        r = nai_iobase_load_start(p, list, bytes, sendfn, gather);
        break;

    case NAI_ASYNC_LOAD_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };

        /* wait */
        r = nai_iobase_ctx_wait(p, 2, -1);
        if (r < 0) {
            goto _end;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_COMPLETED:
        if (p->load->error) {
            p->loadstat = NAI_ASYNC_LOAD_DONE;
            p->load->stat = p->loadstat;
            nai_errno = p->load->error;
            r = -1;
        } else {
            r = nai_iobase_load_flush(p, list, sendfn);
        };

        if (p->loadstat == NAI_ASYNC_LOAD_DONE) {
            nai_iobase_load_done(p, r);
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


nai_int_t nai_iobase_load_cancel(nai_iobase_ctx_t* p)
{
    nai_int_t r;
    nai_async_load_t* l;


    switch (p->loadstat) {
    case NAI_ASYNC_LOAD_PENDING:
        l = p->load;
        r = nai_async_load_cancel(l);
        p->loadstat = (uint8_t)l->stat;

        if (p->loadstat == NAI_ASYNC_LOAD_DONE) {
            nai_iobase_load_done(p, r);
        };
        break;

    default:
        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_iobase_load_discard(nai_iobase_ctx_t* p, nai_int_t blocking)
{
    nai_int_t r;
    nai_async_load_t* l;


    switch (p->loadstat) {
    case NAI_ASYNC_LOAD_PENDING:
        l = p->load;
        r = nai_async_load_discard(l);
        if (r < 0) {
            break;
        };

        if (blocking) {
            /* wait */
            r = nai_iobase_ctx_wait(p, 2, -1);
            if (r < 0) {
                break;
            };
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_COMPLETED:
        l = p->load;
        nai_async_load_discard(l);
        p->loadstat = (uint8_t)l->stat;

        if (p->loadstat == NAI_ASYNC_LOAD_DONE) {
            nai_iobase_load_done(p, 0);
        };

        r = 0;
        break;

    default:
        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_iobase_load_close(nai_iobase_ctx_t* p)
{
    nai_int_t r;
    nai_async_load_t* l;


    if (p->loadstat == NAI_ASYNC_LOAD_UNINIT) {
        r = 0;
        goto _end;
    };

    l = p->load;
    r = nai_async_load_close(l);
    if (r < 0) {
        goto _end;
    };

    p->loadstat = NAI_ASYNC_LOAD_UNINIT;
    p->load = 0;
    r = 0;

_end:
    return r;
};


