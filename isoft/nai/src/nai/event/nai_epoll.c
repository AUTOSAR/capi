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
/// @file       nai_epoll.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if (NAI_HAVE_EPOLL)

#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai_evport.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/nai_file.h"
#include <sys/epoll.h>


typedef struct nai_epoll_s {

    nai_fd_t ep;
    nai_evsignal_t sig;
    nai_evport_t port;
    nai_evloop_ent_t* list[2];
    nai_int_t flags;

} nai_epoll_t;


static void* nai_epoll_open(nai_int_t flags);
static nai_int_t nai_epoll_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_epoll_del(void* e, nai_evnode_t* h);
static nai_int_t nai_epoll_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_epoll_wait(void* e, uint32_t msec);
static nai_int_t nai_epoll_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_epoll_submit(void* e);
static nai_int_t nai_epoll_signal(void* e);
static nai_int_t nai_epoll_close(void* e);
static nai_int_t nai_epoll_rearm(void* e);
static nai_fd_t nai_epoll_get_fd(void* e);
static nai_aio_port_t* nai_epoll_get_port(void* e);


#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_EDGE        \
    | NAI_EV_FEAT_SOCK                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


#if (NAI_HAVE_AIO_PORT)
#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_AIO_PORT_IMPL_FEAT                            \

#else
#define NAI_IMPL_FEAT_ASYNC     0
#endif


nai_evbase_ops_t nai_epoll = {
    "epoll", 
    NAI_IMPL_FEAT_POLL, 
    NAI_IMPL_FEAT_ASYNC, 
    nai_epoll_open, 
    nai_epoll_add, 
    nai_epoll_del, 
    nai_epoll_set, 
    nai_epoll_wait, 
    nai_epoll_fetch, 
    nai_epoll_submit, 
    nai_epoll_signal, 
    nai_epoll_close, 
    nai_epoll_rearm, 
    nai_epoll_get_fd, 
    nai_epoll_get_port
};


static nai_int_t nai_epoll_init(nai_epoll_t* s, nai_int_t flags)
{
    nai_int_t r;
    struct epoll_event ee;


    /* init */
    nai_evport_init(&s->port);
    nai_evsignal_init(&s->sig);
    s->list[0] = 0;
    s->list[1] = 0;
    s->flags = flags;


    /* open epoll */
    s->ep = epoll_create(NAI_EV_INITSIZE);
    if (s->ep == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_file_set_cloexec(s->ep, 1);
    if (r < 0) {
        goto _end;
    };


    if (!(s->flags & NAI_EVBASE_BACKEND)) {
        /* open signal */
        r = nai_evsignal_open(&s->sig);
        if (r < 0) {
            goto _end;
        };

        ee.events = EPOLLIN|EPOLLET;
        ee.data.ptr = 0;
        r = epoll_ctl(s->ep, EPOLL_CTL_ADD, s->sig.in, &ee);
        if (r < 0) {
            goto _end;
        };


        /* open backend */
        r = nai_evport_open(&s->port, &nai_epoll, s);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_epoll_term(nai_epoll_t* s)
{
    nai_int_t r;


    if (s->ep != NAI_FD_INVALID) {
        nai_file_close(s->ep);
    };

    nai_evport_close(&s->port);
    nai_evsignal_close(&s->sig);
    r = 0;

    return r;
};


static void* nai_epoll_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_epoll_t* s;


    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        r = -1;
        goto _end;
    };

    r = nai_epoll_init(s, flags);
    if (r < 0) {
        ec = nai_errno;
        nai_epoll_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_epoll_close(void* e)
{
    nai_int_t r;
    nai_epoll_t* s = (nai_epoll_t*)e;


    nai_epoll_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_epoll_rearm(void* e)
{
    nai_int_t r;
    nai_epoll_t* s = (nai_epoll_t*)e;
    nai_epoll_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_epoll_init(s, s->flags);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_epoll_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_epoll_get_fd(void* e)
{
    nai_epoll_t* s = (nai_epoll_t*)e;
    return s->ep;
};


static nai_aio_port_t* nai_epoll_get_port(void* e)
{
    nai_aio_port_t* r;
    nai_epoll_t* s = (nai_epoll_t*)e;


    if (nai_evport_is_opened(&s->port)) {
        r = nai_evport_get_port(&s->port);
    } else {
        r = 0;
    };

    return r;
};


static nai_int_t nai_epoll_signal(void* e)
{
    nai_epoll_t* s = (nai_epoll_t*)e;
    return nai_evsignal_set(&s->sig);
};


static nai_int_t nai_epoll_add(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_evloop_ent_t* ent;


    ent = h->ent;
    if (ent == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (ent->key != -1) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    h->st.seted &= ~(NAI_EV_IOE | NAI_EV_ASYNC);
    r = nai_epoll_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_epoll_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t seted;
    nai_fd_t fd;
    nai_evloop_ent_t* ent;
    nai_epoll_t* s = (nai_epoll_t*)e;

    nai_int_t op;
    struct epoll_event ee;


    if ((events | h->st.seted) & NAI_EV_ASYNC) {
        if (nai_evport_is_opened(&s->port)) {
            r = nai_evport_set(&s->port, h, events & NAI_EV_ASYNC);
            if (r < 0) {
                goto _end;
            };
        } else {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };
    };


    fd = h->fd;
    ent = h->ent;
    seted = h->st.seted & NAI_EV_IOE;
    events &= NAI_EV_IOE;

    if (seted == events) {
        if (ent->key == -1) {
            ent->key = 0;
        };
        r = 0;
        goto _end;
    };

    if (seted == 0) {           /* only set events */

        op = EPOLL_CTL_ADD;
        ee.events = EPOLLET;
        if (events & NAI_EV_READ) {
            ee.events |= EPOLLIN;
        };
        if (events & NAI_EV_WRITE) {
            ee.events |= EPOLLOUT;
        };
        if (events & NAI_EV_EXCEPT) {
            ee.events |= EPOLLPRI;
        };

    } else if (events == 0) {   /* only unset events */

        op = EPOLL_CTL_DEL;
        ee.events = 0;

    } else {                    /* set and unset events */

        op = EPOLL_CTL_MOD;
        ee.events = EPOLLET;
        if (events & NAI_EV_READ) {
            ee.events |= EPOLLIN;
        };
        if (events & NAI_EV_WRITE) {
            ee.events |= EPOLLOUT;
        };
        if (events & NAI_EV_EXCEPT) {
            ee.events |= EPOLLPRI;
        };
    };

    ee.data.ptr = ent;
    r = epoll_ctl(s->ep, op, fd, &ee);
    if (r < 0) {
        goto _end;
    };

    h->st.seted = events | (h->st.seted & ~NAI_EV_IOE);
    h->ent->key = 0;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_epoll_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_epoll_set(e, h, 0);
    if (r >= 0) {
        h->ent->key = -1;
    };

    return r;
};


static nai_int_t nai_epoll_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t ev;
    nai_int_t events;
    nai_int_t optval;
    nai_int_t optlen;
    nai_evloop_ent_t* ent;
    nai_evloop_ent_t** lptr[2];
    nai_epoll_t* s = (nai_epoll_t*)e;
    struct epoll_event ea[32];


    /* init list pointers */
    lptr[0] = &s->list[0];
    lptr[1] = &s->list[1];


    /* submit pending requests */
    if (nai_evport_is_opened(&s->port)) {
        r = nai_evport_submit(&s->port);
        if (r < 0) {
            goto _end;
        };
    };


    /* wait for events */
    r = epoll_wait(s->ep, ea, nai_countof(ea), msec);
    if (r < 0) {
        goto _end;
    };

    count = r;
    for (n = 0; n < count; n ++) {
        ent = (nai_evloop_ent_t*)ea[n].data.ptr;
        if (ent == 0) {
            /* notify */
            r = nai_evsignal_reset(&s->sig, 1);
            if (r < 0) {
                goto _end;
            };

            continue;
        };

        ev = ea[n].events;
        events = 0;
        if (ev & EPOLLIN) {
            events |= NAI_EV_READ;
        };
        if (ev & EPOLLOUT) {
            events |= NAI_EV_WRITE;
        };
        if (ev & EPOLLPRI) {
            events |= NAI_EV_EXCEPT;
        };
        if (ev & (EPOLLERR|EPOLLHUP)) {
            /* get last errno of socket */
            if (!(ev & EPOLLERR)) {
                events |= NAI_EV_READ|NAI_EV_WRITE;
            } else if (!ent->catching) {
                events |= nai_ev_error_from(0);
            } else {
                ent->catching = 0;
                ent->h->st.catching = 0;
                optlen = sizeof(optval);
                r = nai_sock_get_opt(
                    ent->h->fd, 
                    SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
                if (r < 0) {
                    optval = nai_errno;
                };
                if (optval) {
                    events |= nai_ev_error_from(optval);
                } else {
                    events |= nai_ev_error_from(0);
                };
            };
        };

        if (ent->events == 0) {
            ent->events = events;
            lptr[ent->priority][0] = ent;
            lptr[ent->priority] = &ent->next;
        } else {
            ent->events |= events;
            nai_log_error(NAI_LOG_CORE, EIO, 
                "epoll return more than two events of the same object");
        };
    };

    r = 0;

_end:
    lptr[0][0] = 0;
    lptr[1][0] = 0;
    return r;
};


static nai_int_t nai_epoll_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_epoll_t* s = (nai_epoll_t*)e;


    /* fetch event lists */
    list[0] = s->list[0];
    list[1] = s->list[1];
    s->list[0] = 0;
    s->list[1] = 0;
    r = 0;

    return r;
};


static nai_int_t nai_epoll_submit(void* e)
{
    (void)e;

    return 0;
};


#endif

