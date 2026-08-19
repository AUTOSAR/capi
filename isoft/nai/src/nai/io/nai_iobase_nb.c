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
/// @file       nai_iobase_nb.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// iobase non-block


static nai_int_t nai_iobase_nb_free(nai_iobase_nb_t* a)
{
    nai_int_t r;


    if (a->poolmem) {
        r = nai_evloop_extra_free(a->loop, a, a->syncmem);
    } else {
        nai_free(a);
        r = 0;
    };

    return r;
};


static nai_int_t nai_iobase_nb_call(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_nb_t* n;


    if (s->cb == 0) {
        r = 0;
        goto _end;
    };

    n = (nai_iobase_nb_t*)s->st.ctx;
    n->refs ++;
    s->cb(s, events);

    /* check stream closed */
    r = n->closed;
    n->refs --;
    if (n->refs <= 0) {
        /* free */
        nai_iobase_nb_free(n);
    };

_end:
    return r;
};


static nai_int_t nai_iobase_nb_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t io;
    nai_int_t timer;
    nai_int_t signum;
    uint32_t now;
    nai_iobase_t* s = (nai_iobase_t*)e;


    /* check events */
    switch (events & NAI_EV_MASK) {
    case NAI_EV_NOTIFY:
        /* process notify */
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            /* by nai_iobase_return_loop */
            if (s->st.blocking) {
                r = nai_iobase_set_blocking(s, 0);
                if (r < 0) {
                    goto _fail;
                };
            };

            events = s->st.mode && ~s->st.blocked;
        };

        if (events) {
            if (nai_iobase_nb_call(s, events)) {
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
            if (nai_iobase_nb_call(s, timer|NAI_EV_TIMEOUT)) {
                /* closed */
                goto _end;
            };
        };
        if (!s->st.timerevt && s->st.timerset) {
            r = nai_iobase_update_timer(s, now);
            if (r < 0) {
                goto _fail;
            };
        };

        break;

    default:
        /* connected or disconnected, unset check */
        if (s->ev.st.catching) {
            nai_evnode_set_catching(&s->ev, 0);
        };

        /* unset blocked flags */
        io = (events & (NAI_EV_READ|NAI_EV_WRITE));
        if (io) {
            r = nai_iobase_unblocked(s, io);
            if (r < 0) {
                goto _fail;
            };

            /* handle wants */
            if (s->st.wants & io) {
                s->st.wants = 0;
                events |= (NAI_EV_READ|NAI_EV_WRITE) & s->st.mode;
            };
        };

        if (nai_iobase_nb_call(s, events)) {
            /* user closed */
            goto _end;
        };

        break;
    };


_end:
    return 0;

_fail:
    nai_iobase_nb_call(s, nai_ev_error_from(nai_errno));
    goto _end;
};


static nai_int_t nai_iobase_nb_complex_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t io;
    nai_int_t set;
    nai_int_t timer;
    nai_int_t signum;
    uint32_t now;
    nai_iobase_t* s = (nai_iobase_t*)e;
    nai_iobase_nb_t* n;


    /* check events */
    switch (events & NAI_EV_MASK) {
    case NAI_EV_NOTIFY:
        /* process notify */
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            /* by nai_iobase_return_loop */
            if (s->st.blocking) {
                r = nai_iobase_set_blocking(s, 0);
                if (r < 0) {
                    goto _fail;
                };
            };

            events = s->st.mode && ~s->st.blocked;
        };

        if (events) {
            if (nai_iobase_nb_call(s, events)) {
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
            if (nai_iobase_nb_call(s, timer|NAI_EV_TIMEOUT)) {
                /* closed */
                goto _end;
            };
        };
        if (!s->st.timerevt && s->st.timerset) {
            r = nai_iobase_update_timer(s, now);
            if (r < 0) {
                goto _fail;
            };
        };

        break;

    default:
        /* connected or disconnected, unset check */
        if (s->ev.st.catching) {
            nai_evnode_set_catching(&s->ev, 0);
        };

        /* unset blocked flags */
        io = (events & (NAI_EV_READ|NAI_EV_WRITE));
        if (io) {
            r = nai_iobase_unblocked(s, io);
            if (r < 0) {
                goto _fail;
            };

            /* handle wants */
            if (s->st.wants & io) {
                s->st.wants = 0;
                events |= (NAI_EV_READ|NAI_EV_WRITE) & s->st.mode;
            };
        };

        /* set the mark of autoset */
        n = (nai_iobase_nb_t*)s->st.ctx;
        n->autoset = 1;

        if (nai_iobase_nb_call(s, events)) {
            /* user closed */
            goto _end;
        };

        /* unset the mark of autoset */
        n->autoset = 0;

        /* reset events for level tigger */
        set = s->ev.st.seted;
        set &= ~io;
        set |= s->st.blocked;
        if (set != (nai_int_t)s->ev.st.seted) {
            r = nai_evnode_set_event(&s->ev, NAI_EV_SET, set);
            if (r < 0) {
                goto _fail;
            };
        };

        break;
    };


_end:
    return 0;

_fail:
    nai_iobase_nb_call(s, nai_ev_error_from(nai_errno));
    goto _end;
};



nai_int_t nai_iobase_nb_init(nai_iobase_t* s)
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

    /* set resuse port if options is seted before open */
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
    r = nai_iobase_set_fd_blocking_init(s, s->st.blocking);
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    return r;
};


extern nai_int_t nai_stream_nb_sel_iofn(nai_stream_t* s);


nai_int_t nai_iobase_nb_add_evloop(
    nai_iobase_t* s, nai_evloop_t* l, nai_int_t af)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t lt;
    nai_int_t mt = 0;
    nai_int_t pool = 0;
    nai_int_t events;
    nai_iobase_nb_t* n;


    /* set socket family */
    if (af != AF_UNSPEC) {
        r = nai_iobase_sock_set_family(s, af);
        if (r < 0) {
            goto _end;
        };
    };


    /* alloc nb struct */
    if ((l && nai_evloop_is_asyncio(l, 0))) {
        n = (nai_iobase_nb_t*)nai_malloc(sizeof(*n));
    } else {
        pool = l != 0;
        n = (nai_iobase_nb_t*)(pool ? 
            nai_evloop_extra_alloc(l, &mt) : nai_malloc(sizeof(*n)));
    };
    if (n == 0) {
        r = -1;
        goto _end;
    };

    /* initialize nb struct */
    s->st.ctx = n;
    n->io = s;
    n->loop = l;
    n->wait = 0;
    n->load = 0;
    n->init = 0;
    n->refs = 1;
    n->proto = NAI_IOBASE_CTX_NB;
    n->closed = 0;
    n->type = s->st.type;
    n->loadstat = NAI_ASYNC_LOAD_UNINIT;
    n->readstat = NAI_IOBASE_STAT_DONE;
    n->sendstat = NAI_IOBASE_STAT_DONE;
    n->poolmem = !!pool;
    n->syncmem = !!mt;

    /* initialize for type */
    switch (s->st.type) {
    case NAI_IO_TYPE_SERVER:
    case NAI_IO_TYPE_DGRAM:
        break;
    default:
        nai_stream_nb_sel_iofn(s);
        break;
    };


    /* add the event loop */
    if (l == 0) {
        r = nai_iobase_unblocked(s, NAI_IO_READWRITE);
    } else {
        /* update blocked */
        if (s->st.blocking) {
            events = NAI_IO_READWRITE;
        } else {
            events = ~s->st.mode & NAI_IO_READWRITE;
            events &= s->st.blocked;
        };
        if (events) {
            r = nai_iobase_unblocked(s, events);
            if (r < 0) {
                goto _fail;
            };
        };

        /* set events */
        if (!s->st.blocking) {
            r = nai_evnode_set_event(&s->ev, 
                NAI_EV_SET, s->st.mode | nai_iobase_def_event(s));
            if (r < 0) {
                goto _fail;
            };
        };

        /* level tigger */
        lt = !!(nai_evloop_get_feature(
            l, NAI_EV_FEAT_POLL) & NAI_EV_FEAT_LEVEL);

        /* set the event node handler */
        if (!lt) {
            n->autoset = 1;
            nai_evnode_set_cb(&s->ev, nai_iobase_nb_handle);
        } else {
            nai_evnode_set_cb(&s->ev, nai_iobase_nb_complex_handle);
        };

        /* join the event loop */
        r = nai_evnode_open(&s->ev, l);
    };
    if (r < 0) {
        goto _fail;
    };


_end:
    return r;

_fail:
    ec = nai_errno;
    n = (nai_iobase_nb_t*)s->st.ctx;
    nai_iobase_nb_free(n);
    nai_evnode_set_event(&s->ev, NAI_EV_SET, 0);
    s->st.ctx = 0;
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_iobase_nb_open(
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
    r = nai_iobase_nb_init(s);
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

    r = nai_iobase_nb_add_evloop(s, l, af);
    if (r < 0) {
        goto _end;
    };

_end:
    if (r < 0) {
        if (l == 0) {
            s->st.blocking = st.blocking;
        };
    };
    return r;
};


nai_int_t nai_iobase_nb_bind(
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
        fn = nai_sock_open(name->sa_family, type, 0);
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
    r = nai_iobase_nb_init(s);
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

    r = nai_iobase_nb_add_evloop(s, l, name->sa_family);
    if (r < 0) {
        goto _end;
    };


_end:
    if (r < 0) {
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


nai_int_t nai_iobase_nb_connect(
    nai_iobase_t*s, nai_evloop_t* l, 
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
        fn = nai_sock_open(name->sa_family, type, 0);
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
    r = nai_iobase_nb_init(s);
    if (r < 0) {
        goto _end;
    };

    r = nai_sock_connect(fd, name, namelen);
    if (r < 0) {
        if (nai_errno != EINPROGRESS && nai_errno != EAGAIN) {
            goto _end;
        };

        if (s->st.blocking) {
            r = nai_stream_sock_wait(s, 1);
            if (r < 0) {
                goto _end;
            };

            /* check socket error */
            r = nai_iobase_sock_get_error(s);
            if (r < 0) {
                goto _end;
            };
        } else {
            r = nai_iobase_blocked(s, NAI_EV_WRITE);
            if (r < 0) {
                goto _end;
            };

            /* tell the event loop do more check on read event */
            nai_evnode_set_catching(&s->ev, 1);
        };
    };

    r = nai_iobase_nb_add_evloop(s, l, name->sa_family);
    if (r < 0) {
        goto _end;
    };


_end:
    if (r < 0) {
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


nai_int_t nai_iobase_nb_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_nb_t* n;


    /* close the event node */
    r = nai_evnode_close(&s->ev);
    if (r < 0) {
        goto _end;
    };

    /* free context */
    if (s->st.ctx) {
        n = (nai_iobase_nb_t*)s->st.ctx;
        n->closed = 1;

        nai_iobase_load_close((nai_iobase_ctx_t*)n);

        n->refs --;
        if (n->refs <= 0) {
            nai_iobase_nb_free(n);
        };
    };

    s->st.ops = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_nb_getopt(
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
#if defined(_WIN32)
            value[0] = 0;
#else
            value[0] = nai_iofeat.sendfile;
#endif
        } else {
            value[0] = 0;
        };
        r = 0;
        break;

    case NAI_IO_FEAT_VECTORIO:
        if (nai_iobase_is_sock(s)) {
            value[0] = nai_iofeat.sendv;
        } else {
            value[0] = nai_iofeat.writev;
        };
        r = 0;
        break;

    case NAI_IO_FEAT_MODEL:
        if (nai_iobase_get_loop(s)) {
            value[0] = NAI_EV_FEAT_EDGE;
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


nai_int_t nai_iobase_nb_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t set, uns;
    nai_iobase_nb_t* a;


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

        set = value & ~(s->st.mode);
        if (set) {
            r = nai_iobase_blocked(s, set);
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
        r = nai_evnode_set_event(&s->ev, 
            NAI_EV_SET, (nai_int_t)value | nai_iobase_def_event(s));
        if (r < 0) {
            break;
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
            /* will become nonblock, restore event */
            r = nai_evnode_set_event(&s->ev, 
                NAI_EV_SET, (nai_int_t)value | nai_iobase_def_event(s));
            if (r < 0) {
                break;
            };

            /* retore blocked mask */
            r = nai_iobase_blocked(s, s->st.mode);
            if (r < 0) {
                break;
            };
        } else {
            /* will become blocking, unset event */
            r = nai_evnode_set_event(&s->ev, NAI_EV_SET, 0);
            if (r < 0) {
                break;
            };

            /* unset blocked mask */
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

        s->st.blocking = !s->st.blocking;
        break;

    case NAI_IO_RECVTIMEO:
    case NAI_IO_SENDTIMEO:

        n = opt - NAI_IO_RECVTIMEO;
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
            /* timeout has been changed, update blocking,
             * if system unsupported io timedout and any timeout be set
             * then change to non-block mode, otherwise change to block mode
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
            a = (nai_iobase_nb_t*)s->st.ctx;
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


nai_int_t nai_iobase_nb_blocked(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_nb_t* n;


    if (events != (nai_int_t)(s->ev.st.seted & events)) {
        n = (nai_iobase_nb_t*)s->st.ctx;
        if (!n->autoset) {
            r = nai_evnode_set_event(&s->ev, NAI_EV_ADD, events);
            if (r < 0) {
                goto _end;
            };
        };
    };

    s->st.blocked |= events;
    r = nai_iobase_set_timer(s, events);

_end:
    return r;
};


