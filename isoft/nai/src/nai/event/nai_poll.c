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
/// @file       nai_poll.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if (NAI_HAVE_POLL) || defined(_WIN32)


#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai_evport.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"


#if defined(_WIN32)

#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT  0x0600

#include "nai/os/win/nai_wsock.h"

typedef int (WINAPI *WSAPoll_f)(
    LPWSAPOLLFD fdArray,
    ULONG fds,
    INT timeout
);

static WSAPoll_f poll;

typedef SOCKET nai_fdos_t;


#else

#include <poll.h>
typedef int nai_fdos_t;

#endif



typedef struct nai_poll_s {

    // handle lookup table
    nai_int_t alloc;
    nai_int_t count;
    struct pollfd* fds;
    nai_evloop_ent_t** handles;
    nai_evsignal_t sig;
    nai_evport_t port;
    nai_evloop_ent_t* list[2];

} nai_poll_t;


static void* nai_poll_open(nai_int_t flags);
static nai_int_t nai_poll_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_poll_del(void* e, nai_evnode_t* h);
static nai_int_t nai_poll_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_poll_wait(void* e, uint32_t msec);
static nai_int_t nai_poll_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_poll_submit(void* e);
static nai_int_t nai_poll_signal(void* e);
static nai_int_t nai_poll_close(void* e);
static nai_int_t nai_poll_rearm(void* e);
static nai_fd_t nai_poll_no_fd(void* e);
static nai_aio_port_t* nai_poll_get_port(void* e);


#if defined(_WIN32)
#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_LEVEL       \
    | NAI_EV_FEAT_SOCK                                  \

#else
#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_LEVEL       \
    | NAI_EV_FEAT_SOCK                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \

#endif


#if (NAI_HAVE_AIO_PORT)
#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_AIO_PORT_IMPL_FEAT                            \

#else
#define NAI_IMPL_FEAT_ASYNC     0
#endif


nai_evbase_ops_t nai_poll = {
    "poll", 
    NAI_IMPL_FEAT_POLL, 
    NAI_IMPL_FEAT_ASYNC, 
    nai_poll_open, 
    nai_poll_add, 
    nai_poll_del, 
    nai_poll_set, 
    nai_poll_wait, 
    nai_poll_fetch, 
    nai_poll_submit, 
    nai_poll_signal, 
    nai_poll_close, 
    nai_poll_rearm, 
    nai_poll_no_fd, 
    nai_poll_get_port
};


static nai_int_t nai_poll_init(nai_poll_t* s)
{
    nai_int_t r;
    nai_int_t n;


    /* init */
    nai_evport_init(&s->port);
    nai_evsignal_init(&s->sig);
    s->alloc = 0;
    s->count = 0;
    s->fds = 0;
    s->handles = 0;
    s->list[0] = 0;
    s->list[1] = 0;


    // alloc fd lookup tables
    s->alloc = NAI_EV_INITSIZE;
    s->fds = (struct pollfd*)nai_malloc(s->alloc * sizeof(*s->fds));
    if (s->fds == 0) {
        r = -1;
        goto _end;
    };

    s->handles = (nai_evloop_ent_t**)nai_malloc(s->alloc * sizeof(*s->handles));
    if (s->handles == 0) {
        r = -1;
        goto _end;
    };

    // clear
    nai_memset(s->fds, 0x0, s->alloc * sizeof(*s->fds));
    for (n = 0; n < (nai_int_t)s->alloc; n ++) {
        s->handles[n] = 0;
    };


    /* setup signal fd */
    r = nai_evsignal_open(&s->sig);
    if (r < 0) {
        goto _end;
    };

    // add signal
    s->fds[0].fd = (nai_fdos_t)s->sig.in;
    s->fds[0].events = POLLIN;
    s->count = 1;


    /* setup aio port */
    r = nai_evport_open(&s->port, &nai_poll, s);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


static nai_int_t nai_poll_term(nai_poll_t* s)
{
    nai_int_t r;


    if (s->fds) {
        nai_free(s->fds);
    };
    if (s->handles) {
        nai_free(s->handles);
    };

    nai_evport_close(&s->port);
    nai_evsignal_close(&s->sig);
    r = 0;

    return r;
};


static void* nai_poll_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_poll_t* s;


    (void)flags;

#if defined(_WIN32)
    if (poll == 0) {
        HMODULE ws2 = GetModuleHandleA("ws2_32.dll");
        poll = (WSAPoll_f)GetProcAddress(ws2, "WSAPoll");
    };
    if (poll == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        s = 0;
        goto _end;
    };
#endif

    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        goto _end;
    };

    r = nai_poll_init(s);
    if (r < 0) {
        ec = nai_errno;
        nai_poll_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_poll_close(void* e)
{
    nai_int_t r;
    nai_poll_t* s = (nai_poll_t*)e;


    nai_poll_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_poll_rearm(void* e)
{
    nai_int_t r;
    nai_poll_t* s = (nai_poll_t*)e;
    nai_poll_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_poll_init(s);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_poll_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_poll_no_fd(void* e)
{
    (void)e;
    return NAI_FD_INVALID;
};


static nai_aio_port_t* nai_poll_get_port(void* e)
{
    nai_poll_t* s = (nai_poll_t*)e;
    return nai_evport_get_port(&s->port);
};


static nai_int_t nai_poll_signal(void* e)
{
    nai_poll_t* s = (nai_poll_t*)e;
    return nai_evsignal_set(&s->sig);
};


static nai_int_t nai_poll_add(void* e, nai_evnode_t* h, nai_int_t events)
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

    h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
    r = nai_poll_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_poll_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t ev;
    nai_int_t seted;
    nai_int_t set;
    nai_int_t unset;
    nai_int_t alloc;
    nai_fd_t fd;
    struct pollfd* fds;
    nai_evloop_ent_t** handles;
    nai_evloop_ent_t* ent;
    nai_poll_t* s = (nai_poll_t*)e;


    if ((events | h->st.seted) & NAI_EV_ASYNC) {
        r = nai_evport_set(&s->port, h, events & NAI_EV_ASYNC);
        if (r < 0) {
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
        if (s->count > 0x7ffe - 1) {
            nai_errno = EMFILE;
            r = -1;
            goto _end;
        };

        // add handle
        if (s->count >= s->alloc) {
            alloc = s->alloc * 2;
            fds = (struct pollfd*)nai_malloc(alloc*sizeof(*fds));
            if (fds == 0) {
                r = -1;
                goto _end;
            };
            handles = (nai_evloop_ent_t**)nai_malloc(alloc*sizeof(*handles));
            if (handles == 0) {
                ec = nai_errno;
                nai_free(fds);
                nai_errno = ec;
                r = -1;
                goto _end;
            };

            nai_memcpy(fds, s->fds, s->count*sizeof(*fds));
            nai_memcpy(handles, s->handles, s->count*sizeof(*handles));

            nai_free(s->fds);
            nai_free(s->handles);

            s->alloc = alloc;
            s->fds = fds;
            s->handles = handles;
        };

        ev = 0;
        if (events & NAI_EV_READ) {
            ev |= POLLIN;
        };
        if (events & NAI_EV_WRITE) {
            ev |= POLLOUT;
        };
        if (events & NAI_EV_EXCEPT) {
            ev |= POLLPRI;
        };

        ent->key = s->count;
        s->fds[s->count].fd = (nai_fdos_t)fd;
        s->fds[s->count].events = ev;
        s->handles[s->count] = ent;
        s->count ++;

    } else if (events == 0) {   /* only unset events */

        s->count --;
        s->fds[ent->key] = s->fds[s->count];
        s->handles[ent->key] = s->handles[s->count];
        s->handles[ent->key]->key = ent->key;

        /* mark the entry is added and no event seted */
        ent->key = 0;

    } else {                    /* set and unset events */

        set = events & ~seted;
        unset = seted & ~events;

        ev = s->fds[ent->key].events;
        if (set & NAI_EV_READ) {
            ev |= POLLIN;
        } else if (unset & NAI_EV_READ) {
            ev &= ~POLLIN;
        } else {
            /* nothing */
        };
        if (set & NAI_EV_WRITE) {
            ev |= POLLOUT;
        } else if (unset & NAI_EV_WRITE) {
            ev &= ~POLLOUT;
        } else {
            /* nothing */
        };
        if (set & NAI_EV_EXCEPT) {
            ev |= POLLPRI;
        } else if (unset & NAI_EV_EXCEPT) {
            ev &= ~POLLPRI;
        } else {
            /* nothing */
        };
        s->fds[ent->key].events = ev;
    };

    h->st.seted = events | (h->st.seted & ~NAI_EV_IOE);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_poll_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_poll_set(e, h, 0);
    if (r >= 0) {
        /* mark deleted */
        h->ent->key = -1;
    };

    return r;
};


static nai_int_t nai_poll_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t ev;
    nai_int_t events;
    nai_int_t optval;
    nai_int_t optlen;
    struct pollfd* fd;
    nai_evloop_ent_t* ent;
    nai_evloop_ent_t** lptr[2];
    nai_poll_t* s = (nai_poll_t*)e;


    /* init list pointers */
    lptr[0] = &s->list[0];
    lptr[1] = &s->list[1];


    /* submit pending requests */
    r = nai_evport_submit(&s->port);
    if (r < 0) {
        goto _end;
    };


    /* wait for events */
    r = poll(s->fds, s->count, msec);
    if (r < 0) {
#if defined(_WIN32)
        nai_errno = nai_errno_from_oserr(GetLastError());
#endif
        goto _end;
    };
    if (r == 0) {
        if (msec != (uint32_t)-1) {
            goto _end;
        };

        nai_errno = NAI_EFAILED;
        r = -1;
        goto _end;
    };


    count = r;
    for (n = 0; n < s->count; n ++) {
        fd = &s->fds[n];
        ev = fd->revents;
        if (ev == 0) {
            continue;
        };

        ent = s->handles[n];
        if (ent == 0) {
            r = nai_evsignal_reset(&s->sig, 0);
            if (r < 0) {
                goto _end;
            };

            count --;
            if (count <= 0) {
                break;
            };
            continue;
        };

        events = 0;
        if (ev & POLLIN) {
            events |= NAI_EV_READ;
        };
        if (ev & POLLOUT) {
            events |= NAI_EV_WRITE;
        };
        if (ev & POLLPRI) {
            events |= NAI_EV_EXCEPT;
        };
        if (ev & (POLLERR|POLLHUP|POLLNVAL)) {
            /* get last errno of socket */
            if (!(ev & POLLERR)) {
                events |= NAI_EV_READ|NAI_EV_WRITE;
            } else if (!ent->catching) {
                events |= nai_ev_error_from(0);
            } else {
                ent->catching = 0;
                ent->h->st.catching = 0;
                optlen = sizeof(optval);
                r = nai_sock_get_opt((nai_fd_t)
                    fd->fd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
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

        if (events) {
            ent->events = events;
            lptr[ent->priority][0] = ent;
            lptr[ent->priority] = &ent->next;
        };

        count --;
        if (count <= 0) {
            break;
        };
    };

    r = 0;

_end:
    lptr[0][0] = 0;
    lptr[1][0] = 0;
    return r;
};


static nai_int_t nai_poll_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_poll_t* s = (nai_poll_t*)e;


    /* fetch event lists */
    list[0] = s->list[0];
    list[1] = s->list[1];
    s->list[0] = 0;
    s->list[1] = 0;
    r = 0;

    return r;
};


static nai_int_t nai_poll_submit(void* e)
{
    (void)e;

    return 0;
};


#endif

