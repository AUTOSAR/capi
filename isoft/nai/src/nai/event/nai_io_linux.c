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
/// @file       nai_io_linux.c
/// @brief      
/// @details
/// @date       2022-06-02
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_aio.h"


#if (NAI_AIO_USE_LINUX)


#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/unix/nai_linux.h"


#if (NAI_HAVE_POLL)
#include <poll.h>
#endif
#if (NAI_HAVE_EVENTFD)
#include <sys/eventfd.h>
#endif


typedef struct nai_io_linux_s {

    nai_aio_port_t port;
    nai_int_t flags;
    nai_int_t signaled;

} nai_io_linux_t;


static void* nai_io_linux_open(nai_int_t flags);
static nai_int_t nai_io_linux_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_io_linux_del(void* e, nai_evnode_t* h);
static nai_int_t nai_io_linux_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_io_linux_wait(void* e, uint32_t msec);
static nai_int_t nai_io_linux_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_io_linux_submit(void* e);
static nai_int_t nai_io_linux_signal(void* e);
static nai_int_t nai_io_linux_close(void* e);
static nai_int_t nai_io_linux_rearm(void* e);
static nai_fd_t nai_io_linux_get_fd(void* e);
static nai_aio_port_t* nai_io_linux_get_port(void* e);


#define NAI_IMPL_FEAT_POLL      0


#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_EV_FEAT_FILE                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


nai_evbase_ops_t nai_io_linux = {
    "io_linux", 
    NAI_IMPL_FEAT_ASYNC, 
    NAI_IMPL_FEAT_POLL, 
    nai_io_linux_open, 
    nai_io_linux_add, 
    nai_io_linux_del, 
    nai_io_linux_set, 
    nai_io_linux_wait, 
    nai_io_linux_fetch, 
    nai_io_linux_submit, 
    nai_io_linux_signal, 
    nai_io_linux_close, 
    nai_io_linux_rearm, 
    nai_io_linux_get_fd, 
    nai_io_linux_get_port
};


static nai_int_t nai_io_linux_init(nai_io_linux_t* s, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t oflags;


    /* init */
    s->flags = flags;
    s->signaled = 0;
    s->port.ctx = 0;


    /* open eventfd */
#if !defined(EFD_CLOEXEC)
    oflags = 0;
#else
    oflags = EFD_CLOEXEC;
#endif

    s->port.fd = eventfd(0, oflags);
    if (s->port.fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

#if !defined(EFD_CLOEXEC)
    r = nai_file_set_cloexec(s->port.fd, 1);
    if (r < 0) {
        goto _end;
    };
#endif


    /* open */
    r = io_setup(NAI_AIO_LINUX_MAXREQ, &s->port.ctx);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "io_setup() failed");
        goto _end;
    };


_end:
    return r;
};


static nai_int_t nai_io_linux_term(nai_io_linux_t* s)
{
    nai_int_t r;


    if (s->port.ctx != 0) {
        io_destroy(s->port.ctx);
        s->port.ctx = 0;
    };
    if (s->port.fd != NAI_FD_INVALID) {
        nai_file_close(s->port.fd);
        s->port.fd = NAI_FD_INVALID;
    };

    r = 0;

    return r;
};


static void* nai_io_linux_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_io_linux_t* s;


    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        r = -1;
        goto _end;
    };

    r = nai_io_linux_init(s, flags);
    if (r < 0) {
        ec = nai_errno;
        nai_io_linux_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_io_linux_close(void* e)
{
    nai_int_t r;
    nai_io_linux_t* s = (nai_io_linux_t*)e;


    nai_io_linux_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_io_linux_rearm(void* e)
{
    nai_int_t r;
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    nai_io_linux_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_io_linux_init(s, s->flags);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_io_linux_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_io_linux_get_fd(void* e)
{
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    return s->port.fd;
};


static nai_aio_port_t* nai_io_linux_get_port(void* e)
{
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    return &s->port;
};


static nai_int_t nai_io_linux_signal(void* e)
{
    nai_int_t r;
    uint64_t c;
    nai_io_linux_t* s = (nai_io_linux_t*)e;


    if (s->signaled) {
        r = 0;
    } else {
        s->signaled = 1;
        c = 1;
        r = (nai_int_t)nai_file_write(s->port.fd, &c, sizeof(c));
        if (r < 0) {
            s->signaled = 0;
        };
    };

    return r;
};


static nai_int_t nai_io_linux_add(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_io_linux_t* s = (nai_io_linux_t*)e;
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
    r = nai_io_linux_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_io_linux_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    nai_evloop_ent_t* ent;


    (void)s;

    if (events & NAI_EV_IOE) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
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


static nai_int_t nai_io_linux_del(void* e, nai_evnode_t* h)
{
    nai_evloop_ent_t* ent;


    (void)e;

    h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
    ent = h->ent;
    ent->key = -1;
    return 0;
};


static nai_int_t nai_io_linux_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    struct pollfd set;


    if (!s->signaled && !(s->flags & NAI_EVBASE_BACKEND)) {
        set.fd = s->port.fd;
        set.events = POLLIN;
        set.revents = 0;
        r = poll(&set, 1, msec);
        if (r <= 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_io_linux_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_int_t n;
    uint64_t ready;
    nai_aio_t* a;
    nai_io_linux_t* s = (nai_io_linux_t*)e;
    struct timespec ts;
    struct io_event ev[64];


    /* clear event lists */
    list[0] = 0;
    list[1] = 0;


    /* handle async opeartions */
    r = (nai_int_t)nai_file_read(s->port.fd, &ready, sizeof(ready));
    if (r < 0) {
        goto _end;
    };

    s->signaled = 0;

    while (ready) {
        ts.tv_sec = 0;
        ts.tv_nsec = 0;

        r = io_getevents(s->port.ctx, 1, nai_countof(ev), ev, &ts);
        if (r < 0) {
            goto _end;
        };
        if (r == 0) {
            break;
        };

        ready -= r;

        for (n = 0; n < r; n ++) {
            a = (nai_aio_t*)ev[n].data;
            if (a == 0) {
                continue;
            };

            if (ev[n].res < 0) {
                a->cb.aio_nbytes = -1;
                a->code = -ev[n].res;
            } else {
                a->cb.aio_nbytes = ev[n].res;
                a->code = 0;
            };

            if (a->handle) {
                a->handle(a, a->code, a->cb.aio_nbytes);
            };
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_io_linux_submit(void* e)
{
    (void)e;

    return 0;
};


#endif


