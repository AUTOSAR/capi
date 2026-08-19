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
/// @file       nai_select.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if (NAI_HAVE_SELECT) || defined(_WIN32)

#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai_evport.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"


#if defined(_WIN32)

#include "nai/os/win/nai_wsock.h"
typedef SOCKET nai_fdos_t;

#else

#include <sys/select.h>
typedef int nai_fdos_t;

#endif


typedef struct nai_select_s {

    // handle lookup table
    nai_int_t alloc;
    nai_int_t count;
    nai_fd_t* fds;
    nai_evloop_ent_t** handles;
    nai_evsignal_t sig;
    nai_evport_t port;
    nai_evloop_ent_t* list[2];

    // fd sets
    nai_int_t max_fd;
    nai_int_t except_fd;
    fd_set set_read;
    fd_set set_write;
    fd_set set_except;

} nai_select_t;


static void* nai_select_open(nai_int_t flags);
static nai_int_t nai_select_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_select_del(void* e, nai_evnode_t* h);
static nai_int_t nai_select_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_select_wait(void* e, uint32_t msec);
static nai_int_t nai_select_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_select_submit(void* e);
static nai_int_t nai_select_signal(void* e);
static nai_int_t nai_select_close(void* e);
static nai_int_t nai_select_rearm(void* e);
static nai_fd_t nai_select_no_fd(void* e);
static nai_aio_port_t* nai_select_get_port(void* e);


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


nai_evbase_ops_t nai_select = {
    "select", 
    NAI_IMPL_FEAT_POLL, 
    NAI_IMPL_FEAT_ASYNC, 
    nai_select_open, 
    nai_select_add, 
    nai_select_del, 
    nai_select_set, 
    nai_select_wait, 
    nai_select_fetch, 
    nai_select_submit, 
    nai_select_signal, 
    nai_select_close, 
    nai_select_rearm, 
    nai_select_no_fd, 
    nai_select_get_port
};


static nai_int_t nai_select_init(nai_select_t* s)
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
    s->max_fd = -1;
    s->except_fd = 0;
    FD_ZERO(&s->set_read);
    FD_ZERO(&s->set_write);
    FD_ZERO(&s->set_except);


    // alloc fd lookup tables
    s->alloc = NAI_EV_INITSIZE;
    s->fds = (nai_fd_t*)nai_malloc(s->alloc * sizeof(*s->fds));
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
#if !defined(_WIN32)
    s->max_fd = s->sig.in;
#endif
    FD_SET((nai_fdos_t)s->sig.in, &s->set_read);


    /* setup aio port */
    r = nai_evport_open(&s->port, &nai_select, s);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


static nai_int_t nai_select_term(nai_select_t* s)
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


static void* nai_select_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_select_t* s;


    (void)flags;

    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        goto _end;
    };

    r = nai_select_init(s);
    if (r < 0) {
        ec = nai_errno;
        nai_select_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_select_close(void* e)
{
    nai_int_t r;
    nai_select_t* s = (nai_select_t*)e;


    nai_select_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_select_rearm(void* e)
{
    nai_int_t r;
    nai_select_t* s = (nai_select_t*)e;
    nai_select_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_select_init(s);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_select_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_select_no_fd(void* e)
{
    (void)e;
    return NAI_FD_INVALID;
};


static nai_aio_port_t* nai_select_get_port(void* e)
{
    nai_select_t* s = (nai_select_t*)e;
    return nai_evport_get_port(&s->port);
};


static nai_int_t nai_select_signal(void* e)
{
    nai_select_t* s = (nai_select_t*)e;
    return nai_evsignal_set(&s->sig);
};


static nai_int_t nai_select_add(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_evloop_ent_t* ent;


    ent = h->ent;
    if (ent == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    }
    if (ent->key != -1) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };
#if !defined(_WIN32)
    if (h->fd >= FD_SETSIZE) {
        nai_errno = EBADF;
        r = -1;
        goto _end;
    };
#endif

    h->st.seted &= ~(NAI_EV_IOE|NAI_EV_ASYNC);
    r = nai_select_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_select_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t seted;
    nai_int_t set;
    nai_int_t unset;
    nai_int_t alloc;
    nai_fdos_t fd;
    nai_fd_t* fds;
    nai_evloop_ent_t** handles;
    nai_evloop_ent_t* ent;
    nai_select_t* s = (nai_select_t*)e;


    if ((events | h->st.seted) & NAI_EV_ASYNC) {
        r = nai_evport_set(&s->port, h, events & NAI_EV_ASYNC);
        if (r < 0) {
            goto _end;
        };
    };


    fd = (nai_fdos_t)h->fd;
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
        if (s->count > FD_SETSIZE - 1) {
            nai_errno = EMFILE;
            r = -1;
            goto _end;
        };

        if (events & NAI_EV_READ) {
            FD_SET(fd, &s->set_read);
        };
        if (events & NAI_EV_WRITE) {
            FD_SET(fd, &s->set_write);
        };
        if (events & NAI_EV_EXCEPT) {
            FD_SET(fd, &s->set_except);
            s->except_fd ++;
        };

        // add handle
        if (s->count >= s->alloc) {
            alloc = s->alloc * 2;
            fds = (nai_fd_t*)nai_malloc(alloc*sizeof(*fds));
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

#if !defined(_WIN32)
        if (s->max_fd < fd) {
            s->max_fd = fd;
        };
#endif
        ent->key = s->count;
        s->fds[s->count] = (nai_fd_t)fd;
        s->handles[s->count] = ent;
        s->count ++;

    } else if (events == 0) {   /* only unset events */

        if (seted & NAI_EV_READ) {
            FD_CLR(fd, &s->set_read);
        };
        if (seted & NAI_EV_WRITE) {
            FD_CLR(fd, &s->set_write);
        };
        if (seted & NAI_EV_EXCEPT) {
            FD_CLR(fd, &s->set_except);
            s->except_fd --;
        };

        if (s->max_fd == fd) {
            s->max_fd = -1;
        };
        s->count --;
        s->fds[ent->key] = s->fds[s->count];
        s->handles[ent->key] = s->handles[s->count];
        s->handles[ent->key]->key = ent->key;

        /* mark the entry is added and no event seted */
        ent->key = 0;

    } else {                    /* set and unset events */

        set = events & ~seted;
        unset = seted & ~events;
        if (set & NAI_EV_READ) {
            FD_SET(fd, &s->set_read);
        } else if (unset & NAI_EV_READ) {
            FD_CLR(fd, &s->set_read);
        } else {
            /* nothing */
        };
        if (set & NAI_EV_WRITE) {
            FD_SET(fd, &s->set_write);
        } else if (unset & NAI_EV_WRITE) {
            FD_CLR(fd, &s->set_write);
        } else {
            /* nothing */
        };
        if (set & NAI_EV_EXCEPT) {
            FD_SET(fd, &s->set_except);
            s->except_fd ++;
        } else if (unset & NAI_EV_EXCEPT) {
            FD_CLR(fd, &s->set_except);
            s->except_fd --;
        } else {
            /* nothing */
        };
    };

    h->st.seted = events | (h->st.seted & ~NAI_EV_IOE);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_select_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_select_set(e, h, 0);
    if (r >= 0) {
        /* mark deleted */
        h->ent->key = -1;
    };

    return r;
};


static nai_int_t nai_select_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t events;
    nai_int_t optval;
    nai_int_t optlen;
    struct timeval tv, *tp;
    fd_set set_read;
    fd_set set_write;
    fd_set set_except;
    fd_set* set_excptr;
    nai_fd_t fd;
    nai_evloop_ent_t* ent;
    nai_evloop_ent_t** lptr[2];
    nai_select_t* s = (nai_select_t*)e;


    /* init list pointers */
    lptr[0] = &s->list[0];
    lptr[1] = &s->list[1];


    /* submit pending requests */
    r = nai_evport_submit(&s->port);
    if (r < 0) {
        goto _end;
    };


#if !defined(_WIN32)
    nai_int_t max;
    if (s->max_fd == -1) {
        max = s->sig.in;
        for (n = 0; n < s->count; n ++) {
            if (max < s->fds[n]) {
                max = s->fds[n];
            };
        };
        s->max_fd = max;
    };
#endif

    if (msec == (uint32_t)-1) {
        tp = 0;
    } else {
        tv.tv_sec = (long)(msec / 1000);
        tv.tv_usec = (long)(msec % 1000 * 1000);
        tp = &tv;
    };

    set_read = s->set_read;
    set_write = s->set_write;

    if (s->except_fd > 0) {
        set_except = s->set_except;
        set_excptr = &set_except;
    } else {
        set_excptr = 0;
    };

    /* wait for events */
#if !defined(_WIN32)
    r = select(s->max_fd + 1, &set_read, &set_write, set_excptr, tp);
#else
    r = select(0, &set_read, &set_write, set_excptr, tp);
#endif
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

    for (n = 0; n < s->count; n ++) {
        fd = s->fds[n];

        events = 0;
        if (FD_ISSET((nai_fd_t)fd, &set_read)) {
            events = NAI_EV_READ;
        };
        if (FD_ISSET((nai_fd_t)fd, &set_write)) {
            events |= NAI_EV_WRITE;
        };
        if (s->except_fd > 0 && FD_ISSET((nai_fd_t)fd, &set_except)) {
            events |= NAI_EV_EXCEPT;
        };

        if (events) {
            ent = s->handles[n];
            ent->events = events;

            /* get last errno of socket */
            if ((events & (NAI_EV_READ|NAI_EV_EXCEPT)) && ent->catching) {
                optlen = sizeof(optval);
                r = nai_sock_get_opt(
                    fd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
                if (r < 0) {
                    optval = nai_errno;
                };
                if (optval) {
                    ent->events |= nai_ev_error_from(optval);
                    ent->catching = 0;
                    ent->h->st.catching = 0;
                };
            };

            lptr[ent->priority][0] = ent;
            lptr[ent->priority] = &ent->next;
        };
    };

    if (FD_ISSET((nai_fdos_t)s->sig.in, &set_read)) {
        r = nai_evsignal_reset(&s->sig, 0);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    lptr[0][0] = 0;
    lptr[1][0] = 0;
    return r;
};


static nai_int_t nai_select_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_select_t* s = (nai_select_t*)e;


    /* fetch event lists */
    list[0] = s->list[0];
    list[1] = s->list[1];
    s->list[0] = 0;
    s->list[1] = 0;
    r = 0;

    return r;
};


static nai_int_t nai_select_submit(void* e)
{
    (void)e;

    return 0;
};


#endif

