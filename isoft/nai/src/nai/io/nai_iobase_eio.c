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
/// @file       nai_iobase_eio.c
/// @brief      
/// @details
/// @date       2022-05-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"
#include "nai/event/nai_evloop.h"
#include "nai/os/nai_tlocal.h"


//////////////////////////////////////////////////////////////////////////////
// iobase block emulate non-block


static nai_int_t nai_iobase_eio_free(nai_iobase_eio_t* a)
{
    nai_int_t r;


    nai_free(a);
    r = 0;

    return r;
};


static nai_int_t nai_iobase_eio_call(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_eio_t* a;


    if (s->cb == 0) {
        r = 0;
        goto _end;
    };

    a = (nai_iobase_eio_t*)s->st.ctx;
    a->refs ++;
    s->cb(s, events);

    /* check stream closed */
    r = a->closed;
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_eio_free(a);
    };

_end:
    return r;
};


static nai_int_t nai_iobase_eio_cancel(nai_iobase_t* s, nai_int_t what)
{
    nai_int_t r;
    nai_iobase_eio_t* a;


    a = (nai_iobase_eio_t*)s->st.ctx;

    /* cancel read */
    if ((what & NAI_IO_READ) && a->readstat == NAI_IOBASE_EIO_PENDING) {
        r = nai_task_cancel(&a->readop.op, nai_thread_io_interrupt);
        if (r >= 0) {
            a->readstat = NAI_IOBASE_EIO_ERROR;
            a->readop.err = ECANCELED;
            a->refs --;
            nai_iobase_unblocked(s, NAI_EV_READ);
        };
    };

    /* cancel write */
    if ((what & NAI_IO_WRITE) && a->sendstat == NAI_IOBASE_EIO_PENDING) {
        r = nai_task_cancel(&a->sendop.op, nai_thread_io_interrupt);
        if (r >= 0) {
            a->sendstat = NAI_IOBASE_EIO_ERROR;
            a->sendop.err = ECANCELED;
            a->refs --;
            nai_iobase_unblocked(s, NAI_EV_WRITE);
        };
    };

    r = 0;

    return r;
};


static nai_int_t nai_iobase_eio_get_blocked(nai_iobase_t* s)
{
    nai_int_t blocked;
    nai_iobase_eio_t* a;


    a = (nai_iobase_eio_t*)s->st.ctx;
    blocked = 0;
    blocked |= (a->readstat == NAI_IOBASE_EIO_PENDING) ? NAI_IO_READ : 0;
    blocked |= (a->sendstat == NAI_IOBASE_EIO_PENDING) ? NAI_IO_WRITE : 0;

    return blocked;
};


static nai_int_t nai_iobase_eio_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t timer;
    nai_int_t signum;
    uint32_t now;
    nai_iobase_t* s = (nai_iobase_t*)e;


    switch (events & NAI_EV_MASK) {
    case NAI_EV_NOTIFY:
        /* process notify */
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            /* by nai_iobase_return_loop */
            if (s->st.blocking) {
                r = nai_iobase_set_blocking(s, 0);
                if (r < 0) {
                    ec = nai_errno;
                    goto _fail;
                };
            };

            events = s->st.mode & ~s->st.blocked;
        };

        if (events) {
            if (nai_iobase_eio_call(s, events)) {
                /* closed */
                goto _end;
            };
        };

        break;

    case NAI_EV_TIMEOUT:
        /* process timeout */
        s->st.timerevt = 0;
        now = nai_tickcache_to_msec32();
        while (1) {
            timer = nai_iobase_pop_timer(s, now);
            if (timer == 0) {
                break;
            };
            if (nai_iobase_eio_call(s, timer|NAI_EV_TIMEOUT)) {
                /* closed */
                goto _end;
            };
        };
        if (!s->st.timerevt && s->st.timerset) {
            r = nai_iobase_update_timer(s, now);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };

        break;

    default:
        if (nai_iobase_eio_call(s, events)) {
            /* closed */
            goto _end;
        };

        break;
    };


_end:
    return 0;

_fail:
    nai_iobase_eio_call(s, nai_ev_error_from(ec));
    return r;
};


nai_int_t nai_iobase_eio_read_complete(nai_iobase_eio_t* a)
{
    nai_int_t r;
    nai_int_t events;
    nai_iobase_t* s;


    /* save result */
    if (a->readop.err) {
        if (a->readop.err == EAGAIN) {
            a->readop.err = EBUSY;
        };
        a->readstat = NAI_IOBASE_EIO_ERROR;
    } else {
        a->readstat = NAI_IOBASE_EIO_COMPLETED;
    };


    /* check iobase, refs is 0 means iobase closed */
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_eio_free(a);
        r = 0;
        goto _end;
    };
    if (a->closed) {
        r = 0;
        goto _end;
    };

    /* get iobase */
    s = a->io;


    /* block mode: need send a signal to wakeup blocked read */
    if (s->st.blocking) {
        r = nai_iobase_eio_signal(a);
        if (r < 0) {
            /* we can't do callback in block mode 
             * because callback will make a concurrency issues
             * igrone this error
             **/
            ;
        };
        goto _end;
    };


    /* unset blocked stat */
    r = nai_iobase_unblocked(s, NAI_EV_READ);
    if (r < 0) {
        if (!a->finalize) {
            goto _fail;
        };
    };


    /* set events */
    events = NAI_EV_READ;

    /* handle wants */
    if (s->st.wants & NAI_EV_READ) {
        s->st.wants = 0;
        events = NAI_EV_READ | NAI_EV_WRITE;
    };

    /* test whether it is finalizable */
    if (a->finalize) {
        if (!nai_iobase_ctx_pending((nai_iobase_ctx_t*)a)) {
            events = NAI_EV_FINALIZE;
        } else {
            r = 0;
            goto _end;
        };
    } else {
        events &= s->st.mode;
        if (events == 0) {
            r = 0;
            goto _end;
        };
    };

    /* user callback */
    r = nai_iobase_eio_call(s, events);


_end:
    return r;

_fail:
    nai_iobase_eio_call(s, nai_ev_error_from(nai_errno));
    return r;
};


nai_int_t nai_iobase_eio_send_complete(nai_iobase_eio_t* a)
{
    nai_int_t r;
    nai_int_t events;
    nai_iobase_t* s;


    /* save result */
    if (a->sendop.err) {
        if (a->sendop.err == EAGAIN) {
            a->sendop.err = EBUSY;
        };
        a->sendstat = NAI_IOBASE_EIO_ERROR;
    } else {
        a->sendstat = NAI_IOBASE_EIO_COMPLETED;
    };


    /* check iobase, refs is 0 means iobase closed */
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_eio_free(a);
        r = 0;
        goto _end;
    };
    if (a->closed) {
        r = 0;
        goto _end;
    };

    /* get iobase */
    s = a->io;


    /* block mode: need send a signal to wakeup blocked write */
    if (s->st.blocking) {
        r = nai_iobase_eio_signal(a);
        if (r < 0) {
            /* we can't do callback in block mode 
             * because callback will make a concurrency issues
             * igrone this error
             **/
            ;
        };
        goto _end;
    };


    /* unset blocked stat */
    r = nai_iobase_unblocked(s, NAI_EV_WRITE);
    if (r < 0) {
        if (!a->finalize) {
            goto _fail;
        };
    };


    events = NAI_EV_WRITE;

    /* handle wants */
    if (s->st.wants & NAI_EV_WRITE) {
        s->st.wants = 0;
        events = NAI_EV_READ | NAI_EV_WRITE;
    };

    /* test whether it is finalizable */
    if (a->finalize) {
        if (!nai_iobase_ctx_pending((nai_iobase_ctx_t*)a)) {
            events = NAI_EV_FINALIZE;
        } else {
            r = 0;
            goto _end;
        };
    } else {
        events &= s->st.mode;
        if (events == 0) {
            r = 0;
            goto _end;
        };
    };

    /* user callback */
    r = nai_iobase_eio_call(s, events);


_end:
    return r;

_fail:
    nai_iobase_eio_call(s, nai_ev_error_from(nai_errno));
    return r;
};



nai_int_t nai_iobase_eio_init(nai_iobase_t* s)
{
    nai_int_t r;
    nai_int_t n;


    /* check timeout support, 
     * if false, blocking mode should use non-block and select
     */
    s->st.timeosup = 0;
#if defined(SO_RCVTIMEO)
    if (nai_evnode_get_type(&s->ev) == NAI_FD_TYPE_SOCK && 
        s->st.type != NAI_IO_TYPE_SERVER)  {
        s->st.timeosup = 1;
    };
#endif

    /* set timeout if blocking mode is on */
    if (s->st.blocking && s->st.timeochg) {
        for (n = 0; n < 2; n ++) {
            if (s->st.timeochg & (1<<n)) {
                r = nai_iobase_set_fd_timeo(s, n);
                if (r < 0) {
                    goto _end;
                };
            };
        };
        s->st.timeochg = 0;
    };

    /* set blocking mode */
    r = nai_iobase_set_fd_blocking_init(s, s->st.blocking);
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_eio_add_evloop(
    nai_iobase_t* s, nai_evloop_t* l, nai_int_t af)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t refed;
    nai_iobase_eio_t* a;


    (void)af;

    /* default eio events is unblocked */
    if (l != 0) {
        if (s->st.blocked) {
            r = nai_iobase_unblocked(s, s->st.blocked);
            if (r < 0) {
                goto _end;
            };
        };
    };

    /* alloc eio struct */
    a = (nai_iobase_eio_t*)nai_malloc(sizeof(*a));
    if (a == 0) {
        r = -1;
        goto _end;
    };

    /* initialize eio struct */
    s->st.ctx = a;
    a->io = s;
    a->loop = l;
    a->wait = 0;
    a->load = 0;
    a->init = 0;
    a->refs = 1;
    a->proto = NAI_IOBASE_CTX_EIO;
    a->closed = 0;
    a->type = s->st.type;
    a->loadstat = NAI_ASYNC_LOAD_UNINIT;
    a->readstat = NAI_IOBASE_EIO_DONE;
    a->sendstat = NAI_IOBASE_EIO_DONE;
    a->iofn = nai_iobase_is_file(s) ? 
        NAI_IOBASE_IOFN_FILE : NAI_IOBASE_IOFN_PIPE;
    a->message = nai_stream_is_message(s);
    a->fd = nai_stream_get_fd(s);
    nai_task_init(&a->readop.op, 1);
    nai_task_init(&a->sendop.op, 1);

    /* add loop */
    if (l) {
        /* reference io threads */
        r = nai_evloop_ref_io_threads(l);
        if (r < 0) {
            refed = 0;
            goto _fail;
        };

        /* set the event node handler */
        nai_evnode_set_cb(&s->ev, nai_iobase_eio_handle);
        nai_evnode_set_event(&s->ev, NAI_EV_SET, 0);

        /* join the event loop */
        r = nai_evnode_open(&s->ev, l);
        if (r < 0) {
            refed = 1;
            goto _fail;
        };

    } else {
        r = 0;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    a = (nai_iobase_eio_t*)s->st.ctx;
    nai_iobase_eio_free(a);
    s->st.ctx = 0;

    if (refed) {
        nai_evloop_unref_io_threads(l);
    };

    nai_errno = ec;
    goto _end;
};


nai_int_t nai_iobase_eio_open(
    nai_iobase_t* s, nai_evloop_t* l, nai_int_t af)
{
    nai_int_t r;

    struct {
        uint32_t blocking:1;
    } st = { 0 };


    /* save status for restore on fails */
    st.blocking = s->st.blocking;


    /* initial blocking mode */
    if (l == 0) {
        s->st.blocking = 1;
    };

    /* initial file descriptor */
    r = nai_iobase_eio_init(s);
    if (r < 0) {
        goto _end;
    };

    /* send a signal for emulate read/write event */
    if (!s->st.blocking && s->st.mode) {
        r = nai_iobase_post_signal(s);
        if (r < 0) {
            goto _end;
        };
    };

    /* add into the event loop */
    r = nai_iobase_eio_add_evloop(s, l, af);


_end:
    if (r < 0) {
        /* clear signal bit */
        nai_iobase_clear_signal(s);

        /* clear other status */
        if (l == 0) {
            s->st.blocking = st.blocking;
        };
    };
    return r;
};


nai_int_t nai_iobase_eio_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_eio_t* a;


    /* close the event node */
    r = nai_evnode_close(&s->ev);
    if (r < 0) {
        goto _end;
    };

    /* free context */
    if (s->st.ctx) {
        a = (nai_iobase_eio_t*)s->st.ctx;
        a->closed = 1;
        a->fd = NAI_FD_INVALID;

        /* cancel opeartions */
        r = nai_iobase_eio_cancel(s, NAI_IO_READWRITE);
        assert(r >= 0);
        (void)r;

        /* unref io thread pool */
        r = nai_evloop_unref_io_threads(a->loop);
        assert(r >= 0);
        (void)r;

        a->refs --;
        if (a->refs <= 0) {
            nai_iobase_eio_free(a);
        };
    };

    s->st.ops = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_eio_getopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;


    switch (opt) {
    case NAI_IO_MODE:
        value[0] = s->st.mode;
        r = 0;
        break;

    case NAI_IO_BLOCKING:
        value[0] = s->st.blocking;
        r = 0;
        break;

    case NAI_IO_RECVTIMEO:
    case NAI_IO_SENDTIMEO:
        value[0] = s->st.timeout[opt-NAI_IO_RECVTIMEO];
        r = 0;
        break;

    case NAI_IO_SENDFILE:
        value[0] = s->st.sendfile;
        r = 0;
        break;

    case NAI_IO_FEAT_SENDFILE:
        value[0] = 0;
        r = 0;
        break;

    case NAI_IO_FEAT_VECTORIO:
        value[0] = nai_iofeat.writev;
        r = 0;
        break;

    case NAI_IO_FEAT_MODEL:
        if (nai_iobase_get_loop(s)) {
            value[0] = NAI_EV_FEAT_ASYNC;
        } else {
            value[0] = NAI_EV_FEAT_BLOCK;
        };
        r = 0;
        break;

    default:
        r = nai_iobase_default_getopt(s, opt, value);
        break;
    };

    return r;
};


nai_int_t nai_iobase_eio_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t blocked;
    nai_int_t set, uns;


    switch (opt) {
    case NAI_IO_MODE:

        value &= (s->st.type == NAI_IO_TYPE_SERVER) ? 
            NAI_IO_READ : NAI_IO_READWRITE;
        if (s->st.mode == value) {
            r = 0;
            break;
        };

        if (s->st.blocking) {
            s->st.mode = (nai_int_t)value;
            r = 0;
            break;
        };

        if (s->ev.loop == 0) {
            nai_errno = EPERM;
            r = -1;
            break;
        };
        if (!nai_evloop_in_dispatch(s->ev.loop)) {
            nai_errno = EPERM;
            r = -1;
            break;
        };

        /* update block stat */
        set = value & ~(s->st.mode);
        if (set) {
            /* get still blocked */
            blocked = nai_iobase_eio_get_blocked(s);
            r = nai_iobase_blocked(s, set & blocked);
            if (r < 0) {
                break;
            };
        };
        uns = s->st.mode & ~(value);
        if (uns) {
            r = nai_iobase_unblocked(s, uns);
            if (r < 0) {
                break;
            };
        };

        s->st.mode = (nai_int_t)value;
        r = 0;
        break;

    case NAI_IO_BLOCKING:

        if (s->st.blocking == !!value) {
            r = 0;
            break;
        };

        if (s->ev.loop == 0) {
            nai_errno = EPERM;
            r = -1;
            break;
        };
        if (!nai_evloop_in_dispatch(s->ev.loop)) {
            nai_errno = EPERM;
            r = -1;
            break;
        };

        if (s->st.blocking) {
            /* get still blocked */
            blocked = nai_iobase_eio_get_blocked(s);

            /* will become nonblock */
            r = nai_iobase_blocked(s, s->st.mode & blocked);
            if (r < 0) {
                break;
            };

        } else {
            /* will become blocking, unset blocked mask */
            r = nai_iobase_unblocked(s, s->st.mode);
            if (r < 0) {
                break;
            };

            /* auto kill timer */
            r = nai_iobase_set_timeout(s, NAI_TIMEOP_SET, -1);
            if (r < 0) {
                break;
            };

            /* restore fd timeout */
            if (s->st.timeochg) {
                for (n = 0; n < 2; n ++) {
                    if (s->st.timeochg & (1<<n)) {
                        r = nai_iobase_set_fd_timeo(s, n);
                        if (r < 0) {
                            goto _end;
                        };
                    };
                };
                s->st.timeochg = 0;
            };
        };

        r = nai_iobase_set_fd_blocking(s, !s->st.blocking);
        if (r < 0) {
            break;
        };

        s->st.blocking = !!value;
        break;

    case NAI_IO_RECVTIMEO:
    case NAI_IO_SENDTIMEO:

        n = opt-NAI_IO_RECVTIMEO;
        if (s->st.timeout[n] == (uint32_t)value) {
            r = 0;
            break;
        };
        if (value != -1) {
            s->st.timeout[n] = (nai_int_t)value;
            s->st.timeoset |= (1<<n);
            if (s->ev.loop && s->st.blocked & (1<<n)) {
                r = nai_iobase_set_timer(s, (NAI_EV_READ<<n));
                if (r < 0) {
                    break;
                };
            };
        } else {
            s->st.timeout[n] = -1;
            s->st.timeoset &= ~(1<<n);
            if (s->ev.loop) {
                r = nai_iobase_kill_timer(s, (NAI_EV_READ<<n));
                if (r < 0) {
                    break;
                };
            };
        };
        if (s->st.blocking) {
            r = nai_iobase_set_fd_timeo(s, n);
            if (r < 0) {
                break;
            };
            /* timeout has changed, update blocking 
             * if system unsupported io timedout and any timeout be set
             * then change block mode
             **/
            r = nai_iobase_set_fd_blocking(s, s->st.blocking);
            if (r < 0) {
                break;
            };
        } else {
            /* unneed set fd timeout in non-block mode
             * because we are not use system timeout, just mark changed
             */
            s->st.timeochg |= (NAI_EV_READ<<n);
            r = 0;
        };
        break;

    case NAI_IO_SENDFILE:
        s->st.sendfile = !!value;
        r = 0;
        break;

    case NAI_IO_CANCEL:
        if (!nai_iobase_in_dispatch(s)) {
            nai_errno = EPERM;
            r = -1;
            break;
        };

        r = nai_iobase_eio_cancel(s, (nai_int_t)value);
        break;

    default:
        r = nai_iobase_default_setopt(s, opt, value);
        break;
    };

_end:
    return r;
};


nai_int_t nai_iobase_eio_wait(
    nai_iobase_eio_t* a, nai_int_t which, uint32_t timeout)
{
    return nai_iobase_ctx_wait((nai_iobase_ctx_t*)a, which, timeout);
};


nai_int_t nai_iobase_eio_signal(nai_iobase_eio_t* a)
{
    return nai_iobase_ctx_signal((nai_iobase_ctx_t*)a);
};



