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
/// @file       nai_kqueue.c
/// @brief      
/// @details
/// @date       2021-02-17
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/nai_config.h"


#if (NAI_HAVE_KQUEUE)

#include "nai_evloop.h"
#include "nai_evbase.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_aio.h"
#include "nai/os/nai_proc.h"
#include <sys/event.h>



typedef struct nai_kqueue_s {

    union {
        nai_fd_t kq;
        nai_aio_port_t port;
    };
    nai_pid_t pid;

#if !defined(EVFILT_USER)
    nai_evsignal_t sig;
#else
    nai_int_t signaled;
    struct kevent signal;
#endif

    nai_int_t nchange;
    struct kevent changes[32];

    nai_int_t nops;
    nai_aio_t* aio[32];
    nai_evloop_ent_t* list[2];

} nai_kqueue_t;


static void* nai_kqueue_open(nai_int_t flags);
static nai_int_t nai_kqueue_add(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_kqueue_del(void* e, nai_evnode_t* h);
static nai_int_t nai_kqueue_set(void* e, nai_evnode_t* h, nai_int_t events);
static nai_int_t nai_kqueue_wait(void* e, uint32_t msec);
static nai_int_t nai_kqueue_fetch(void* e, nai_evloop_ent_t* list[2]);
static nai_int_t nai_kqueue_submit(void* e);
static nai_int_t nai_kqueue_signal(void* e);
static nai_int_t nai_kqueue_close(void* e);
static nai_int_t nai_kqueue_rearm(void* e);
static nai_fd_t nai_kqueue_get_fd(void* e);
static nai_aio_port_t* nai_kqueue_get_port(void* e);


#define NAI_IMPL_FEAT_POLL      NAI_EV_FEAT_EDGE        \
    | NAI_EV_FEAT_SOCK                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \


#if defined(SIGEV_KEVENT)
#define NAI_IMPL_FEAT_ASYNC     NAI_EV_FEAT_ASYNC       \
    | NAI_EV_FEAT_FILE                                  \
    | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC               \

#else

#define NAI_IMPL_FEAT_ASYNC     0

#endif


nai_evbase_ops_t nai_kqueue = {
    "kqueue", 
    NAI_IMPL_FEAT_POLL, 
    NAI_IMPL_FEAT_ASYNC, 
    nai_kqueue_open, 
    nai_kqueue_add, 
    nai_kqueue_del, 
    nai_kqueue_set, 
    nai_kqueue_wait, 
    nai_kqueue_fetch, 
    nai_kqueue_submit, 
    nai_kqueue_signal, 
    nai_kqueue_close, 
    nai_kqueue_rearm, 
    nai_kqueue_get_fd, 
    nai_kqueue_get_port
};


static nai_int_t nai_kqueue_add_change(
    nai_kqueue_t* s, nai_evnode_t* h, nai_int_t op, nai_int_t ev)
{
    nai_int_t r;
    struct kevent* e;
    struct timespec ts;


    if (s->nchange >= (intptr_t)nai_countof(s->changes)) {
        ts.tv_sec = 0;
        ts.tv_nsec = 0;

        r = kevent(s->kq, s->changes, s->nchange, 0, 0, &ts);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "kevent add events failed");
            goto _end;
        };

        s->nchange = 0;
    };

    e = &s->changes[s->nchange];
    e->ident = h->fd;
    e->filter = ev;
    e->flags = op;
    e->udata = op != EV_DELETE ? h->ent : 0;
    s->nchange ++;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_kqueue_init(nai_kqueue_t* s)
{
    nai_int_t r;


    /* init */
#if !defined(EVFILT_USER)
    nai_evsignal_init(&s->sig);
#endif

    s->list[0] = 0;
    s->list[1] = 0;
    s->nops = 0;
    s->nchange = 0;
    nai_memset(s->aio, 0, sizeof(s->aio));
    nai_memset(s->changes, 0, sizeof(s->changes));


    /* create kqueue */
    s->pid = nai_get_pid();
    s->kq = kqueue();
    if (s->kq == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_file_set_cloexec(s->kq, 1);
    if (r < 0) {
        goto _end;
    };


#if !defined(EVFILT_USER)

    r = nai_evsignal_open(&s->sig);
    if (r < 0) {
        goto _end;
    };

    s->nchange ++;
    EV_SET(s->changes, s->sig.in, 
        EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

#else

    EV_SET(&s->signal, 0, 
        EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, 0);
    r = kevent(s->kq, &s->signal, 1, 0, 0, 0);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "kevent add user event failed");
    };

    s->signaled = 0;
    s->signal.flags = 0;
    s->signal.fflags = NOTE_TRIGGER;

#endif

    r = 0;


_end:
    return r;
};


static nai_int_t nai_kqueue_term(nai_kqueue_t* s)
{
    nai_int_t r;


    if (s->kq != NAI_FD_INVALID) {
        if (s->pid == nai_get_pid()) {
            nai_file_close(s->kq);
        };
    };

#if !defined(EVFILT_USER)
    nai_evsignal_close(&s->sig);
#endif
    r = 0;

    return r;
};


static void* nai_kqueue_open(nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_kqueue_t* s;


    (void)flags;

    s = nai_malloc(sizeof(*s));
    if (s == 0) {
        goto _end;
    };

    r = nai_kqueue_init(s);
    if (r < 0) {
        ec = nai_errno;
        nai_kqueue_close(s);
        nai_errno = ec;
        s = 0;
    };

_end:
    return s;
};


static nai_int_t nai_kqueue_close(void* e)
{
    nai_int_t r;
    nai_kqueue_t* s = (nai_kqueue_t*)e;


    nai_kqueue_term(s);
    nai_free(s);
    r = 0;

    return r;
};


static nai_int_t nai_kqueue_rearm(void* e)
{
    nai_int_t r;
    nai_kqueue_t* s = (nai_kqueue_t*)e;
    nai_kqueue_t t[2];


    /* backup */
    nai_memcpy(&t[0], s, sizeof(*s));

    /* init */
    r = nai_kqueue_init(s);
    if (r < 0) {
        /* restore */
        nai_memcpy(s, &t[0], sizeof(*s));
    } else {
        /* cleanup */
        nai_memcpy(&t[1], s, sizeof(*s));
        nai_memcpy(s, &t[0], sizeof(*s));
        nai_kqueue_term(s);
        nai_memcpy(s, &t[1], sizeof(*s));
    };

    return r;
};


static nai_fd_t nai_kqueue_get_fd(void* e)
{
    nai_kqueue_t* s = (nai_kqueue_t*)e;
    return s->kq;
};


static nai_aio_port_t* nai_kqueue_get_port(void* e)
{
#if defined(SIGEV_KEVENT)
    nai_kqueue_t* s = (nai_kqueue_t*)e;
    return &s->port;
#else
    (void)e;
    return 0;
#endif
};


static nai_int_t nai_kqueue_signal(void* e)
{
    nai_int_t r;
    nai_kqueue_t* s = (nai_kqueue_t*)e;


#if defined(EVFILT_USER)
    if (s->signaled) {
        r = 0;
    } else {
        s->signaled = 1;
        r = kevent(s->kq, &s->signal, 1, 0, 0, 0);
        if (r < 0) {
            s->signaled = 0;
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "kevent send user event failed");
        };
    };
#else
    r = nai_evsignal_set(&s->sig);
#endif

    return r;
};


static nai_int_t nai_kqueue_add(void* e, nai_evnode_t* h, nai_int_t events)
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
    r = nai_kqueue_set(e, h, events);

_end:
    return r;
};


static nai_int_t nai_kqueue_set(void* e, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;
    nai_int_t seted;
    nai_int_t set;
    nai_int_t unset;
    nai_int_t op;
    nai_int_t ev;
    nai_evloop_ent_t* ent;
    nai_kqueue_t* s = (nai_kqueue_t*)e;


#if defined(SIGEV_KEVENT)
    if (events & NAI_EV_ASYNC) {
        h->st.seted |= NAI_EV_ASYNC;
    } else {
        h->st.seted &= ~NAI_EV_ASYNC;
    };
#else
    if (events & NAI_EV_ASYNC) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

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

        op = EV_ADD | EV_CLEAR;
        if (events & NAI_EV_READ) {
            ev = EVFILT_READ;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
        if (events & NAI_EV_WRITE) {
            ev = EVFILT_WRITE;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#if defined(EVFILT_EXCEPT)
        if (events & NAI_EV_EXCEPT) {
            ev = EVFILT_EXCEPT;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#endif

    } else if (events == 0) {   /* only unset events */

        op = EV_DELETE;
        if (seted & NAI_EV_READ) {
            ev = EVFILT_READ;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
        if (seted & NAI_EV_WRITE) {
            ev = EVFILT_WRITE;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#if defined(EVFILT_EXCEPT)
        if (seted & NAI_EV_EXCEPT) {
            ev = EVFILT_EXCEPT;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#endif

    } else {                    /* set and unset events */

        set = events & ~seted;
        op = EV_ADD | EV_CLEAR;
        if (set & NAI_EV_READ) {
            ev = EVFILT_READ;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
        if (set & NAI_EV_WRITE) {
            ev = EVFILT_WRITE;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#if defined(EVFILT_EXCEPT)
        if (set & NAI_EV_EXCEPT) {
            ev = EVFILT_EXCEPT;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#endif

        unset = seted & ~events;
        op = EV_DELETE;
        if (unset & NAI_EV_READ) {
            ev = EVFILT_READ;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
        if (unset & NAI_EV_WRITE) {
            ev = EVFILT_WRITE;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#if defined(EVFILT_EXCEPT)
        if (unset & NAI_EV_EXCEPT) {
            ev = EVFILT_EXCEPT;
            r = nai_kqueue_add_change(s, h, op, ev);
            if (r < 0) {
                goto _end;
            };
        };
#endif
    };


    h->st.seted = events | (h->st.seted & ~NAI_EV_IOE);
    h->ent->key = 0;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_kqueue_del(void* e, nai_evnode_t* h)
{
    nai_int_t r;


    r = nai_kqueue_set(e, h, 0);
    if (r >= 0) {
        h->ent->key = -1;
    };

    return r;
};


static nai_int_t nai_kqueue_wait(void* e, uint32_t msec)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t events;
    nai_aio_t* a;
    nai_evloop_ent_t* ent;
    nai_evloop_ent_t** lptr[2];
    nai_kqueue_t* s = (nai_kqueue_t*)e;
    struct timespec ts, *tp;
    struct kevent ea[32];


    /* init list pointers */
    lptr[0] = &s->list[0];
    lptr[1] = &s->list[1];


    /* init time */
    if (msec == (uint32_t)-1) {
        tp = 0;
    } else {
        ts.tv_sec = (long)(msec / 1000);
        ts.tv_nsec = (long)(msec % 1000 * 1000 * 1000);
        tp = &ts;
    };

    /* wait for events */
    r = kevent(s->kq, s->changes, s->nchange, ea, nai_countof(ea), tp);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "kevent poll events failed");
        goto _end;
    };

    /* changes has been submit */
    s->nchange = 0;


    count = r;
    for (n = 0; n < count; n ++) {
#if defined(EV_FLAG2)
        if (ea[n].flags & (EV_ERROR | EV_FLAG2)) {
            if (ea[n].flags & EV_FLAG2) {
                nai_log_debug(NAI_LOG_CORE, 0, 
                    "kevent return a removed event %x %x", 
                    ea[n].flags, ea[n].filter);
                continue;
            };
#endif
            if (ea[n].flags & EV_ERROR) {
                nai_log_debug(NAI_LOG_CORE, 
                    (nai_int_t)ea[n].data, "kevent return an error");
                continue;
            };
#if defined(EV_FLAG2)
        };
#endif

        ent = (nai_evloop_ent_t*)ea[n].udata;
        if (ent == 0) {
            /* notify */
#if defined(EVFILT_USER)
            s->signaled = 0;
#else
            if (ea[n].ident == s->sig.in) {
                r = nai_evsignal_reset(&s->sig, 1);
                if (r < 0) {
                    /* ignored error */
                    ;
                };
            };
#endif
            continue;
        };

        switch (ea[n].filter) {
        case EVFILT_READ:
            if (ea[n].flags & EV_EOF && ea[n].fflags && 
                ent->catching) {
                events = NAI_EV_READ | nai_ev_error_from(ea[n].fflags);
            } else {
                events = NAI_EV_READ;
            };
            break;
        case EVFILT_WRITE:
            events = NAI_EV_WRITE;
            break;
#if defined(EVFILT_EXCEPT)
        case EVFILT_EXCEPT:
            events = NAI_EV_EXCEPT;
            break;
#endif
        case EVFILT_AIO:
            a = (nai_aio_t*)ea[n].udata;
            s->aio[s->nops] = a;
            s->nops ++;
            continue;
        case EVFILT_VNODE:
            /* see nai_fs_watch_process_file */
            events = 0;
            if (ea[n].fflags & (NOTE_ATTRIB | NOTE_EXTEND | 
                NOTE_LINK | NOTE_REVOKE | NOTE_WRITE)) {
                events |= nai_ev_error_from(NAI_EV_READ);
            };
            if (ea[n].fflags & (NOTE_RENAME | NOTE_DELETE)) {
                events |= nai_ev_error_from(NAI_EV_WRITE);
            };
            if (events == 0) {
                assert(0);
                continue;
            };
            break;
        default:
            continue;
        };

        if (ent->h == 0) {
            nai_log_alert(NAI_LOG_CORE, 0, 
                "kevent return a removed object %x %x", 
                ea[n].flags, ea[n].filter);

            continue;
        };

        if (ent->events == 0) {
            ent->events = events;
            lptr[ent->priority][0] = ent;
            lptr[ent->priority] = &ent->next;
        } else {
            ent->events |= events;
            nai_log_debug(NAI_LOG_CORE, 0, 
                "kevent return more than two events of the same object");
        };
    };

    r = 0;

_end:
    lptr[0][0] = 0;
    lptr[1][0] = 0;
    return r;
};


static nai_int_t nai_kqueue_fetch(void* e, nai_evloop_ent_t* list[2])
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t ec;
    intptr_t bytes;
    nai_aio_t* a;
    nai_kqueue_t* s = (nai_kqueue_t*)e;


    /* fetch event lists */
    list[0] = s->list[0];
    list[1] = s->list[1];
    s->list[0] = 0;
    s->list[1] = 0;


    /* handle async opeartions */
    if (s->nops <= 0) {
        r = 0;
        goto _end;
    };

    count = s->nops;
    s->nops = 0;
    for (n = 0; n < count; n ++) {
        a = s->aio[n];

        /* fetch result */
        ec = aio_error(&a->cb);
        if (ec < 0) {
            ec = nai_errno;
            bytes = 0;
        } else {
            /* on freebsd, aio_return must be called */
            bytes = aio_return(&a->cb);
            if (bytes == (intptr_t)-1) {
                ec = nai_errno;
            };
        };
        a->code = ec;
        a->cb.aio_nbytes = bytes;

        /* call aio */
        if (a->handle) {
            a->handle(a, ec, bytes);
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_kqueue_submit(void* e)
{
    (void)e;

    return 0;
};


#endif

