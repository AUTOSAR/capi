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
/// @file       nai_iobase_aio.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"

#if defined(_WIN32)
#include "nai/os/win/nai_windows.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// iobase async


static nai_int_t nai_iobase_aio_free(nai_iobase_aio_t* a)
{
    nai_int_t r;


    if (a->type == NAI_IO_TYPE_SERVER) {
        if (nai_aiofeat.acceptex) {
            if (a->sock != NAI_FD_INVALID) {
                nai_sock_close(a->sock);
            };
        } else {
            if (a->readstat == NAI_IOBASE_AIO_COMPLETED && 
                a->readsult != (size_t)-1) {
                nai_sock_close((nai_fd_t)a->readsult);
            };
        };
    } else {
        if (a->name && 
            a->name != (nai_socknbuf_t*)1) {
            nai_free(a->name);
        };
    };

    if (a->poolmem) {
        r = nai_evloop_extra_free(a->loop, a, a->syncmem);
    } else {
        nai_free(a);
        r = 0;
    };

    return r;
};


static nai_int_t nai_iobase_aio_call(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_aio_t* a;


    if (s->cb == 0) {
        r = 0;
        goto _end;
    };

    a = (nai_iobase_aio_t*)s->st.ctx;
    a->refs ++;
    s->cb(s, events);

    /* check stream closed */
    r = a->closed;
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_aio_free(a);
    };

_end:
    return r;
};


static nai_int_t nai_iobase_aio_cancel(nai_iobase_t* s, nai_int_t what)
{
    nai_int_t r;
    nai_iobase_aio_t* a;


    a = (nai_iobase_aio_t*)s->st.ctx;

    /* cancel read */
    if ((what & NAI_IO_READ) && a->readstat == NAI_IOBASE_AIO_PENDING) {
        r = nai_aio_cancel(&a->readop);
        if (r >= 0) {
            a->readstat = NAI_IOBASE_AIO_ERROR;
            a->readsult = ECANCELED;
            a->refs --;
            nai_iobase_unblocked(s, NAI_EV_READ);
        };
    };

    /* cancel write */
    if ((what & NAI_IO_WRITE) && a->sendstat == NAI_IOBASE_AIO_PENDING) {
        r = nai_aio_cancel(&a->sendop);
        if (r >= 0) {
            a->sendstat = NAI_IOBASE_AIO_ERROR;
            a->sendsult = ECANCELED;
            a->refs --;
            nai_iobase_unblocked(s, NAI_EV_WRITE);
        };
    };

    r = 0;

    return r;
};


static nai_int_t nai_iobase_aio_get_blocked(nai_iobase_t* s)
{
    nai_int_t blocked;
    nai_iobase_aio_t* a;

    a = (nai_iobase_aio_t*)s->st.ctx;
    blocked = 0;
    blocked |= (a->readstat == NAI_IOBASE_AIO_PENDING) ? NAI_IO_READ : 0;
    blocked |= (a->sendstat == NAI_IOBASE_AIO_PENDING) ? NAI_IO_WRITE : 0;

    return blocked;
};


static nai_int_t nai_iobase_aio_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t timer;
    nai_int_t signum;
    uint32_t now;
    nai_iobase_t* s = (nai_iobase_t*)e;
    nai_iobase_aio_t* a = (nai_iobase_aio_t*)s->st.ctx;
    nai_socknbuf_t* name;


    switch (events & NAI_EV_MASK) {
    case NAI_EV_NOTIFY:
        /* process notify */
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            if (a->type != NAI_IO_TYPE_SERVER && a->name) {
                if (a->name == (nai_socknbuf_t*)1) {
                    ec = ENOMEM;
                    r = -1;
                    goto _fail;
                };

                name = a->name;
                a->name = 0;
                r = nai_iobase_aio_start_connect(s, &name->addr, name->len);
                if (r < 0) {
                    ec = nai_errno;
                };

                nai_free(name);

                if (r < 0) {
                    if (ec != EINPROGRESS) {
                        goto _fail;
                    };
                };

                break;
            };

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
            if (nai_iobase_aio_call(s, events)) {
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
            if (nai_iobase_aio_call(s, timer|NAI_EV_TIMEOUT)) {
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
        if (nai_iobase_aio_call(s, events)) {
            /* closed */
            goto _end;
        };

        break;
    };


_end:
    return 0;

_fail:
    nai_iobase_aio_call(s, nai_ev_error_from(ec));
    return r;
};


static nai_int_t nai_iobase_aio_read_complete(
    nai_aio_t* readop, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_int_t events;
    nai_iobase_t* s;
    nai_iobase_aio_t* a = nai_containof(
        readop, nai_iobase_aio_t, readop);


    /* save result */
    if (err) {
        a->readstat = NAI_IOBASE_AIO_ERROR;
        a->readsult = err;
    } else {
        a->readstat = NAI_IOBASE_AIO_COMPLETED;
        a->readsult = bytes;
    };


    /* check iobase, refs is 0 means iobase closed */
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_aio_free(a);
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
        r = nai_iobase_aio_signal(a);
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
    r = nai_iobase_aio_call(s, events);


_end:
    return r;

_fail:
    nai_iobase_aio_call(s, nai_ev_error_from(nai_errno));
    return r;
};


static nai_int_t nai_iobase_aio_send_complete(
    nai_aio_t* sendop, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_int_t events;
    nai_iobase_t* s;
    nai_iobase_aio_t* a = nai_containof(
        sendop, nai_iobase_aio_t, sendop);


    /* save result */
    if (err) {
        a->sendstat = NAI_IOBASE_AIO_ERROR;
        a->sendsult = err;
    } else {
        a->sendstat = NAI_IOBASE_AIO_COMPLETED;
        a->sendsult = bytes;
    };


    /* check iobase, refs is 0 means iobase closed */
    a->refs --;
    if (a->refs <= 0) {
        nai_iobase_aio_free(a);
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
        if (a->connecting) {
            a->connecting = 0;
            /**
             * seted in nai_iobase_aio_start_connect. 
             * in blocking mode, the mask of blocked is already clear, 
             * unneed clear it.
             * s->st.blocked &= ~NAI_EV_READ;
             */
            if (err == 0) {
                a->sendstat = NAI_IOBASE_AIO_DONE;
            } else {
                a->readsult = a->sendsult;
                a->readstat = NAI_IOBASE_AIO_ERROR;
            };
        };

        r = nai_iobase_aio_signal(a);
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


    /* is connecting */
    if (a->connecting) {
        a->connecting = 0;

        /* seted in nai_iobase_aio_start_connect */
        s->st.blocked &= ~NAI_EV_READ;
        if (err) {
            events = nai_ev_error_from(err);
        } else {
            events = NAI_EV_READ | NAI_EV_WRITE;
            a->sendstat = NAI_IOBASE_AIO_DONE;
        };
    } else {
        events = NAI_EV_WRITE;

        /* handle wants */
        if (s->st.wants & NAI_EV_WRITE) {
            s->st.wants = 0;
            events = NAI_EV_READ | NAI_EV_WRITE;
        };
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
    r = nai_iobase_aio_call(s, events);


_end:
    return r;

_fail:
    nai_iobase_aio_call(s, nai_ev_error_from(nai_errno));
    return r;
};


nai_int_t nai_iobase_aio_init(nai_iobase_t* s)
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

    /* set resuse address if options is seted before open */
    if (s->st.reuseaddr != 3) {
        if (nai_evnode_get_type(&s->ev) == NAI_FD_TYPE_SOCK) {
            r = nai_iobase_sock_setopt(s, NAI_IO_REUSEADDR, s->st.reuseaddr);
            if (r < 0) {
                goto _end;
            };
        };
        s->st.reuseaddr = 3;
    };
    if (s->st.reuseport != 3) {
        if (nai_evnode_get_type(&s->ev) == NAI_FD_TYPE_SOCK) {
            r = nai_iobase_sock_setopt(s, NAI_IO_REUSEPORT, s->st.reuseport);
            if (r < 0) {
                goto _end;
            };
        };
        s->st.reuseport = 3;
    };

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
    if (s->st.type == NAI_IO_TYPE_SERVER && !s->st.blocking) {
        /* accept of io_uring will return EAGAIN 
         * when socket is non-blocking, keep in blocking mode.
         */
        if (s->st.blockset != 1) {
            r = nai_iobase_set_fd_blocking_impl(s, 1);
        } else {
            r = 0;
        };
    } else {
        r = nai_iobase_set_fd_blocking_init(s, s->st.blocking);
    };
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    return r;
};


extern nai_int_t nai_stream_aio_sel_iofn(nai_stream_t* s);


static nai_int_t nai_iobase_aio_add_evloop_impl(
    nai_iobase_t* s, nai_evloop_t* l, 
    nai_int_t af, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt = 0;
    nai_int_t pool = 0;
    nai_int_t listen;
    nai_int_t bufsize;
    nai_iobase_aio_t* a;
    nai_aio_port_t* port;


    /* default aio events is unblocked */
    if (l != 0) {
        if (s->st.blocked) {
            r = nai_iobase_unblocked(s, s->st.blocked);
            if (r < 0) {
                goto _end;
            };
        };
    };

    /* set socket family */
    if (af != AF_UNSPEC) {
        r = nai_iobase_sock_set_family(s, af);
        if (r < 0) {
            goto _end;
        };
    };


    /* alloc aio struct */
    listen = s->st.type == NAI_IO_TYPE_SERVER;
    if (listen || (l && !nai_evloop_is_asyncio(l, 0))) {
        bufsize = listen ? NAI_LISTEN_BUFSIZE : 0;
        a = (nai_iobase_aio_t*)nai_malloc(sizeof(*a) + bufsize);
    } else {
        pool = l != 0;
        a = (nai_iobase_aio_t*)(pool ? 
            nai_evloop_extra_alloc(l, &mt) : nai_malloc(sizeof(*a)));
    };
    if (a == 0) {
        r = -1;
        goto _end;
    };

    /* initialize aio struct */
    s->st.ctx = a;
    a->io = s;
    a->loop = l;
    a->wait = 0;
    a->load = 0;
    a->init = 0;
    a->refs = 1;
    a->proto = NAI_IOBASE_CTX_AIO;
    a->closed = 0;
    a->type = s->st.type;
    a->loadstat = NAI_ASYNC_LOAD_UNINIT;
    a->readstat = NAI_IOBASE_AIO_DONE;
    a->sendstat = NAI_IOBASE_AIO_DONE;
    a->connecting = 0;
    a->finishskip = 0;
    a->poolmem = !!pool;
    a->syncmem = !!mt;
    a->af = af;
    nai_aio_init(&a->readop);
    nai_aio_init(&a->sendop);
    nai_aio_set_in_loop(&a->readop, 1);
    nai_aio_set_in_loop(&a->sendop, 1);
    nai_aio_set_cb(&a->readop, nai_iobase_aio_read_complete);
    nai_aio_set_cb(&a->sendop, nai_iobase_aio_send_complete);

    /* initialize for type */
    switch (s->st.type) {
    case NAI_IO_TYPE_SERVER:
        a->sock = NAI_FD_INVALID;
        a->last = NAI_FD_INVALID;
        break;
    case NAI_IO_TYPE_DGRAM:
        a->name = 0;
        a->namelen = 0;
        a->ctrllen = 0;

        /* fallthrough */

    default:
        /* for async connect */
        if (name == 0 || s->st.blocking || nai_evloop_in_dispatch(l)) {
            a->name = 0;
        } else {
            /* save sockaddr */
            a->name = (nai_socknbuf_t*)nai_malloc(
                nai_offsetof(nai_socknbuf_t, storage) + namelen);
            if (a->name == 0) {
                r = -1;
                goto _fail;
            };

            a->name->len = namelen;
            nai_memcpy(&a->name->addr, name, namelen);

            /* send a signal for connect */
            r = nai_iobase_post_signal(s);
            if (r < 0) {
                goto _fail;
            };
        };

        if (s->st.type != NAI_IO_TYPE_DGRAM) {
            nai_stream_aio_sel_iofn(s);
        };
        break;
    };


    /* add loop */
    if (l) {
#if defined(_WIN32)
        if (nai_wapi.inited == 0) {
            nai_wapi_init();
        };
        if (nai_wapi.SetFileCompletionNotificationModes) {
            if (s->st.type != NAI_IO_TYPE_DGRAM) {
                r = nai_wapi.SetFileCompletionNotificationModes(
                    nai_iobase_get_fd(s), 
                    FILE_SKIP_COMPLETION_PORT_ON_SUCCESS | 
                    FILE_SKIP_SET_EVENT_ON_HANDLE);
                if (r) {
                    a->finishskip = 1;
                };
            } else {
                r = nai_wapi.SetFileCompletionNotificationModes(
                    nai_iobase_get_fd(s), 
                    FILE_SKIP_SET_EVENT_ON_HANDLE);
                if (r) {
                    ;
                };
            };
        };
#else
        a->finishskip = 1;
#endif

        port = nai_evloop_get_aio_port(l);
        if (port) {
            nai_aio_set_port(&a->readop, port);
            nai_aio_set_port(&a->sendop, port);
        };

        /* set the event node handler */
        nai_evnode_set_cb(&s->ev, nai_iobase_aio_handle);
        nai_evnode_set_event(&s->ev, 
            NAI_EV_SET, NAI_EV_ASYNC | nai_iobase_def_event(s));

        /* join the event loop */
        r = nai_evnode_open(&s->ev, l);
        if (r < 0) {
            goto _fail;
        };

    } else {
        r = 0;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    a = (nai_iobase_aio_t*)s->st.ctx;
    nai_iobase_aio_free(a);
    s->st.ctx = 0;
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_iobase_aio_add_evloop(
    nai_iobase_t* s, nai_evloop_t* l, nai_int_t af)
{
    return nai_iobase_aio_add_evloop_impl(s, l, af, 0, 0);
};


nai_int_t nai_iobase_aio_open(
    nai_iobase_t* s, nai_evloop_t* l, nai_int_t af)
{
    nai_int_t r;
    nai_int_t backlog;

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
    r = nai_iobase_aio_init(s);
    if (r < 0) {
        goto _end;
    };

    if (s->st.type == NAI_IO_TYPE_SERVER) {
        backlog = s->backlog;
        if (backlog == -1) {
            backlog = 100;
        };
        r = nai_sock_listen(nai_iobase_get_fd(s), backlog);
        if (r < 0) {
            goto _end;
        };
    };

    /* send a signal for emulate read/write event */
    if (!s->st.blocking && s->st.mode) {
        r = nai_iobase_post_signal(s);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_iobase_aio_add_evloop(s, l, af);


_end:
    if (r < 0) {
        /* clear signal bit */
        nai_iobase_clear_signal(s);

        /* clear other status */
        if (l != 0) {
            s->st.blocking = st.blocking;
        };
    };
    return r;
};


nai_int_t nai_iobase_aio_bind(
    nai_iobase_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t backlog;
    nai_fd_t fd;
    nai_fd_t fn = NAI_FD_INVALID;

    struct {
        uint32_t blocking:1;
        uint32_t reuseaddr:2;
        uint32_t reuseport:2;
    } st = { 0 };


    fd = nai_evnode_get_fd(&s->ev);
    if (fd == NAI_FD_INVALID) {
        fn = nai_sock_open(name->sa_family, type, IPPROTO_IP);
        if (fn == NAI_FD_INVALID) {
            r = -1;
            goto _end;
        };
        fd = fn;
        s->st.fdown = 1;
        nai_evnode_set_fd(&s->ev, fd, NAI_FD_TYPE_SOCK);

    } else if (nai_evnode_get_type(&s->ev) != NAI_FD_TYPE_SOCK) {
        nai_errno = ENOTSOCK;
        r = -1;
        goto _end;
    };


    /* save status for restore on fails */
    st.blocking = s->st.blocking;
    st.reuseaddr = s->st.reuseaddr;
    st.reuseport = s->st.reuseport;


    /* initial blocking mode */
    if (l == 0) {
        s->st.blocking = 1;
    };

    /* initial file descriptor */
    r = nai_iobase_aio_init(s);
    if (r < 0) {
        goto _end;
    };

    r = nai_sock_bind(fd, name, namelen);
    if (r < 0) {
        goto _end;
    };

    if (s->st.type == NAI_IO_TYPE_SERVER) {
        backlog = s->backlog;
        if (backlog == -1) {
            backlog = 100;
        };
        r = nai_sock_listen(fd, backlog);
        if (r < 0) {
            goto _end;
        };
    };

    /* send a signal for emulate read/write event */
    if (!s->st.blocking && s->st.mode) {
        r = nai_iobase_post_signal(s);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_iobase_aio_add_evloop(s, l, name->sa_family);
    if (r < 0) {
        goto _end;
    };

_end:
    if (r < 0) {
        /* clear signal bit */
        nai_iobase_clear_signal(s);

        /* clear other status */
        s->st.family = 0;
        s->st.blocking = st.blocking;
        if (fn != NAI_FD_INVALID) {
            ec = nai_errno;
            s->st.reuseaddr = st.reuseaddr;
            s->st.reuseport = st.reuseport;
            s->st.timeochg = s->st.timeoset;
            s->st.fdown = 0;
            nai_evnode_set_fd(&s->ev, NAI_FD_INVALID, NAI_FD_TYPE_NONE);
            nai_sock_close(fn);
            nai_errno = ec;
        };
    };
    return r;
};


nai_int_t nai_iobase_aio_connect(
    nai_iobase_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_fd_t fn = NAI_FD_INVALID;

    struct {
        uint32_t blocking:1;
        uint32_t reuseaddr:2;
        uint32_t reuseport:2;
    } st = { 0 };


    fd = nai_evnode_get_fd(&s->ev);
    if (fd == NAI_FD_INVALID) {
        fn = nai_sock_open(name->sa_family, type, IPPROTO_IP);
        if (fn == NAI_FD_INVALID) {
            r = -1;
            goto _end;
        };
        fd = fn;
        s->st.fdown = 1;
        nai_evnode_set_fd(&s->ev, fd, NAI_FD_TYPE_SOCK);

    } else if (nai_evnode_get_type(&s->ev) != NAI_FD_TYPE_SOCK) {
        nai_errno = ENOTSOCK;
        r = -1;
        goto _end;
    };


    /* save status for restore on fails */
    st.blocking = s->st.blocking;
    st.reuseaddr = s->st.reuseaddr;
    st.reuseport = s->st.reuseport;


    /* initial blocking mode */
    if (l == 0) {
        s->st.blocking = 1;
    };

    /* initial file descriptor */
    r = nai_iobase_aio_init(s);
    if (r < 0) {
        goto _end;
    };

    if (s->st.type == NAI_IO_TYPE_DGRAM && 
        s->st.subtype == NAI_IO_SUBT_DEFAULT) {

        /* connect before join the event loop */
        r = nai_sock_connect(fd, name, namelen);
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

        r = nai_iobase_aio_add_evloop(s, l, name->sa_family);
        if (r < 0) {
            goto _end;
        };

    } else {

        /* blocking mode, connect before join the event loop */
        if (s->st.blocking) {
            r = nai_sock_connect(fd, name, namelen);
            if (r < 0) {
                if (nai_errno != NAI_EINPROGRESS) {
                    goto _end;
                };
                r = nai_stream_sock_wait(s, 1);
                if (r < 0) {
                    goto _end;
                };

                /* check socket error */
                r = nai_iobase_sock_get_error(s);
                if (r < 0) {
                    goto _end;
                };
            };
        };

        /* add into the event loop */
        r = nai_iobase_aio_add_evloop_impl(
            s, l, name->sa_family, name, namelen);
        if (r == 0) {
            if (!s->st.blocking) {
                /* r is 0 means we unneed send a connect signal */
                r = nai_iobase_aio_start_connect(s, name, namelen);
                if (r < 0) {
                    if (nai_errno != EINPROGRESS) {
                        ec = nai_errno;
                        nai_evnode_close(&s->ev);
                        nai_errno = ec;
                        goto _end;
                    };
                };

                r = 0;
            };
        };
    };

_end:
    if (r < 0) {
        /* clear signal bit */
        nai_iobase_clear_signal(s);

        /* clear other status */
        s->st.family = 0;
        s->st.blocking = st.blocking;
        if (fn != NAI_FD_INVALID) {
            ec = nai_errno;
            s->st.reuseaddr = st.reuseaddr;
            s->st.reuseport = st.reuseport;
            s->st.timeochg = s->st.timeoset;
            s->st.fdown = 0;
            nai_evnode_set_fd(&s->ev, NAI_FD_INVALID, NAI_FD_TYPE_NONE);
            nai_sock_close(fn);
            nai_errno = ec;
        };
    };
    return r;
};


nai_int_t nai_iobase_aio_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_aio_t* a;


    /* close the event node */
    r = nai_evnode_close(&s->ev);
    if (r < 0) {
        goto _end;
    };


    /* free context */
    if (s->st.ctx) {
        a = (nai_iobase_aio_t*)s->st.ctx;
        a->closed = 1;

        /* cancel opeartions */
        nai_iobase_aio_cancel(s, NAI_IO_READWRITE);

        /* close loader */
        nai_iobase_load_close((nai_iobase_ctx_t*)a);

        a->refs --;
        if (a->refs <= 0) {
            nai_iobase_aio_free(a);
        };
    };

    s->st.ops = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_aio_getopt(
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

    case NAI_IO_PENDING:
        r = nai_iobase_default_getopt(s, opt, value);
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

    case NAI_IO_LOADFILE:
        value[0] = s->st.loadfile;
        r = 0;
        break;

    case NAI_IO_FEAT_SENDFILE:
        if (nai_iobase_is_sock(s) && 
            s->st.type == NAI_IO_TYPE_STREAM) {
            value[0] = nai_aiofeat.sendfile;
        } else {
            value[0] = 0;
        };
        r = 0;
        break;

    case NAI_IO_FEAT_VECTORIO:
        if (nai_iobase_is_sock(s)) {
            value[0] = nai_aiofeat.sendv;
        } else {
            value[0] = nai_aiofeat.writev;
        };
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
        r = nai_iobase_sock_getopt(s, opt, value);
        break;
    };

    return r;
};


nai_int_t nai_iobase_aio_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t blocked;
    nai_int_t set, uns;
    nai_iobase_aio_t* a;


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
            blocked = nai_iobase_aio_get_blocked(s);
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
            blocked = nai_iobase_aio_get_blocked(s);

            /* reset default event */
            r = nai_evnode_set_event(&s->ev, 
                NAI_EV_SET, NAI_EV_ASYNC | nai_iobase_def_event(s));
            if (r < 0) {
                break;
            };

            /* will become nonblock, restore blocked mask */
            r = nai_iobase_blocked(s, s->st.mode & blocked);
            if (r < 0) {
                break;
            };

            a = (nai_iobase_aio_t*)s->st.ctx;
            nai_aio_set_waitable(&a->readop, 0);
            nai_aio_set_waitable(&a->sendop, 0);

        } else {
            /* unset default event */
            r = nai_evnode_set_event(&s->ev, NAI_EV_SET, NAI_EV_ASYNC);
            if (r < 0) {
                break;
            };

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

        if (s->st.type == NAI_IO_TYPE_SERVER && !value) {
            /* accept of io_uring will return EAGAIN 
             * when socket is non-blocking, keep in blocking mode.
             */
            if (s->st.blockset != 1) {
                r = nai_iobase_set_fd_blocking_impl(s, 1);
            };
        } else {
            r = nai_iobase_set_fd_blocking(s, !s->st.blocking);
        };
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

    case NAI_IO_LOADFILE:
        if (s->st.loadfile != !!value) {
            s->st.loadfile = !!value;
            a = (nai_iobase_aio_t*)s->st.ctx;
            if (s->st.loadfile == 0 && 
                a->loadstat == NAI_ASYNC_LOAD_DONE) {
                nai_iobase_load_close((nai_iobase_ctx_t*)s->st.ctx);
            };
        };
        r = 0;
        break;

    case NAI_IO_CANCEL:
        if (!nai_iobase_in_dispatch(s)) {
            nai_errno = EPERM;
            r = -1;
            break;
        };

        nai_iobase_aio_cancel(s, (nai_int_t)value);
        if (value & NAI_IO_WRITE) {
            nai_iobase_load_cancel((nai_iobase_ctx_t*)s->st.ctx);
        };

        r = 0;
        break;

    default:
        r = nai_iobase_sock_setopt(s, opt, value);
        break;
    };

_end:
    return r;
};


nai_int_t nai_iobase_aio_start_connect(
    nai_iobase_t* s, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_fd_t fd;
    nai_iobase_aio_t* a = (nai_iobase_aio_t*)s->st.ctx;


    /* start connect now */
    if (!s->st.blocking) {
        a->sendstat = NAI_IOBASE_AIO_PENDING;
        a->refs ++;
    } else {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _fail;
        };
    };

    fd = nai_evnode_get_fd(&s->ev);
    r = nai_aio_connect(&a->sendop, fd, name, namelen);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            if (!s->st.blocking) {
                a->sendstat = NAI_IOBASE_AIO_DONE;
                a->refs --;
            };
            goto _fail;
        };
    };

    if (!s->st.blocking) {
        /* set timer */
        r = nai_iobase_blocked(s, NAI_EV_WRITE);
        if (r < 0) {
            /* do nothing */
            ;
        };

        /* will be unseted in nai_iobase_send_complete */
        s->st.blocked |= NAI_EV_READ;
        a->connecting = 1;

        nai_errno = EINPROGRESS;
        r = -1;
        goto _fail;
    };

    // try get result
    r = (nai_int_t)nai_aio_result(&a->sendop, 0);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _fail;
        };

        // wait result
        r = nai_aio_wait(&a->sendop, s->st.timeout[1]);
        if (r < 0) {
            /* whether timedout or failed, op should be canceled */
            nai_aio_cancel(&a->sendop);
        };

        // wait result
        r = (nai_int_t)nai_aio_result(&a->sendop, 1);
        if (r < 0) {
            goto _fail;
        };
    };

_fail:
    return r;
};


nai_int_t nai_iobase_aio_wait(
    nai_iobase_aio_t* a, nai_int_t which, uint32_t timeout)
{
    return nai_iobase_ctx_wait((nai_iobase_ctx_t*)a, which, timeout);
};


nai_int_t nai_iobase_aio_signal(nai_iobase_aio_t* a)
{
    return nai_iobase_ctx_signal((nai_iobase_ctx_t*)a);
};


