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
/// @file       nai_io_uring.c
/// @brief      
/// @details
/// @date       2022-06-02
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_aio.h"


#if (NAI_AIO_USE_URING)


#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/unix/nai_linux.h"


typedef struct nai_io_uring_s {

    nai_uring_t u;
    nai_aio_port_t port;
    nai_aio_t wait1;
    nai_aio_t wait2;
    nai_evbase_t back;
    nai_evsignal_t sig;
    nai_int_t flags;
    nai_int_t polling;

} nai_io_uring_t;


static void* nai_io_uring_open(nai_int_t flags);
static nai_int_t nai_io_uring_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_io_uring_del(void* e, nai_evnode_t* h);
static nai_int_t nai_io_uring_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_io_uring_wait(void* e, uint32_t msec);
static nai_int_t nai_io_uring_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_io_uring_submit(void* e);
static nai_int_t nai_io_uring_signal(void* e);
static nai_int_t nai_io_uring_close(void* e);
static nai_int_t nai_io_uring_rearm(void* e);
static nai_fd_t nai_io_uring_get_fd(void* e);
static nai_aio_port_t* nai_io_uring_get_port(void* e);


#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_EDGE        \
    | NAI_EV_FEAT_SOCK                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_EV_FEAT_FILE | NAI_EV_FEAT_SOCK               \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


nai_evbase_ops_t nai_io_uring = {
    "io_uring", 
    NAI_IMPL_FEAT_ASYNC, 
    NAI_IMPL_FEAT_POLL, 
    nai_io_uring_open, 
    nai_io_uring_add, 
    nai_io_uring_del, 
    nai_io_uring_set, 
    nai_io_uring_wait, 
    nai_io_uring_fetch, 
    nai_io_uring_submit, 
    nai_io_uring_signal, 
    nai_io_uring_close, 
    nai_io_uring_rearm, 
    nai_io_uring_get_fd, 
    nai_io_uring_get_port
};


extern nai_evbase_ops_t nai_epoll;


static nai_int_t nai_io_uring_handle_sig(
    nai_aio_t* a, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_io_uring_t* s;


    (void)err;
    (void)bytes;

    s = nai_containof(a, nai_io_uring_t, wait1);
    r = nai_evsignal_reset(&s->sig, 0);

    return r;
};


static nai_int_t nai_io_uring_handle_poll(
    nai_aio_t* a, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_io_uring_t* s;


    (void)err;
    (void)bytes;

    s = nai_containof(a, nai_io_uring_t, wait2);
    s->polling = 1;
    r = 0;

    return r;
};


static nai_int_t nai_io_uring_init(nai_io_uring_t* s, nai_int_t flags)
{
    nai_int_t r;
    nai_aio_t* a;
    nai_uring_t* u;


    /* iniit */
    nai_uring_init(&s->u);
    nai_evbase_init(&s->back);
    nai_evsignal_init(&s->sig);
    s->flags = flags;
    s->polling = 0;


    /* open uring */
    s->port.ctx = &s->u;
    r = nai_uring_open(&s->u, NAI_URING_QSIZE, 0);
    if (r < 0) {
        goto _end;
    };

    /* setup */
    if (!(s->flags & NAI_EVBASE_BACKEND)) {
        u = &s->u;

        /* setup signal */
        r = nai_evsignal_open(&s->sig);
        if (r < 0) {
            goto _end;
        };

        a = &s->wait1;
        nai_aio_init(a);
        nai_aio_set_cb(a, nai_io_uring_handle_sig);
        r = nai_uring_poll(u, a, s->sig.in, 0);
        if (r < 0) {
            goto _end;
        };

        /* setup backend */
        s->back.ops = &nai_epoll;
        s->back.ud = s->back.ops->open(NAI_EVBASE_BACKEND);
        if (s->back.ud == 0) {
            r = -1;
            goto _end;
        };

        a = &s->wait2;
        nai_aio_init(a);
        nai_aio_set_cb(a, nai_io_uring_handle_poll);
        r = nai_uring_poll(u, a, nai_evbase_get_fd(&s->back), 0);
    };


_end:
    return r;
};


static nai_int_t nai_io_uring_term(nai_io_uring_t* s)
{
    nai_int_t r;


    nai_evbase_close(&s->back);
    nai_evsignal_close(&s->sig);
    nai_uring_close(&s->u);
    r = 0;

    return r;
};


static void* nai_io_uring_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_io_uring_t* s;


    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        r = -1;
        goto _end;
    };

    r = nai_io_uring_init(s, flags);
    if (r < 0) {
        ec = nai_errno;
        nai_io_uring_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_io_uring_close(void* e)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;


    nai_io_uring_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_io_uring_rearm(void* e)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    nai_io_uring_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_io_uring_init(s, s->flags);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_io_uring_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_io_uring_get_fd(void* e)
{
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    return s->u.queue.ring_fd;
};


static nai_aio_port_t* nai_io_uring_get_port(void* e)
{
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    return &s->port;
};


static nai_int_t nai_io_uring_signal(void* e)
{
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    return nai_evsignal_set(&s->sig);
};


static nai_int_t nai_io_uring_add(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    nai_evloop_ent_t* ent;


    (void)s;

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

    h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
    r = nai_io_uring_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_io_uring_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;
    nai_evloop_ent_t* ent;


    (void)s;

    if ((events | h->st.seted) & NAI_EV_IOE) {
        if (nai_evbase_is_opened(&s->back)) {
            r = nai_evbase_set(&s->back, h, events & NAI_EV_IOE);
        } else {
            nai_errno = ENOTSUP;
            r = -1;
        };
        if (r < 0) {
            goto _end;
        };
    };

    if (events & NAI_EV_ASYNC) {
        h->st.seted |= NAI_EV_ASYNC;
    } else {
        h->st.seted &= ~NAI_EV_ASYNC;
    };

    ent = h->ent;
    if (ent->key == -1) {
        ent->key = 0;
    };
    r = 0;

_end:
    return r;
};


static nai_int_t nai_io_uring_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_io_uring_set(e, h, 0);
    if (r >= 0) {
        h->ent->key = -1;
    };

    return r;
};


static nai_int_t nai_io_uring_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;


    r = nai_uring_wait(&s->u, msec);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


static nai_int_t nai_io_uring_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;


    /* clear event lists */
    list[0] = 0;
    list[1] = 0;


    /* handle async opeartions */
    r = nai_uring_handle(&s->u);
    if (r < 0) {
        goto _end;
    };

    /* fetch event list */
    if (s->polling) {
        s->polling = 0;
        r = nai_evbase_wait(&s->back, 0);
        if (r < 0) {
            goto _end;
        };

        r = nai_evbase_fetch(&s->back, list);
        if (r < 0) {
            goto _end;
        };
    };

_end:
    return r;
};


static nai_int_t nai_io_uring_submit(void* e)
{
    nai_int_t r;
    nai_io_uring_t* s = (nai_io_uring_t*)e;


    r = nai_uring_submit(&s->u);

    return r;
};


#endif

