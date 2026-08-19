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
/// @file       nai_signal.c
/// @brief      
/// @details
/// @date       2023-09-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_signal.h"
#include "nai/os/nai_pipe.h"
#include "nai/os/nai_proc.h"
#include "nai/event/nai_evloop.h"
#include "nai/event/nai_evmsg.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"



#ifndef _NAI_TYPEDEF_SIGNAL_SERV_T
#define _NAI_TYPEDEF_SIGNAL_SERV_T
typedef struct nai_signal_serv_s nai_signal_serv_t;
#endif
#ifndef _NAI_TYPEDEF_SIGNAL_PORT_T
#define _NAI_TYPEDEF_SIGNAL_PORT_T
typedef struct nai_signal_port_s nai_signal_port_t;
#endif
#ifndef _NAI_TYPEDEF_SIGNAL_NODE_T
#define _NAI_TYPEDEF_SIGNAL_NODE_T
typedef struct nai_signal_node_s nai_signal_node_t;
#endif
#ifndef _NAI_TYPEDEF_SIGNAL_HANDLE_T
#define _NAI_TYPEDEF_SIGNAL_HANDLE_T
typedef struct nai_signal_handle_s nai_signal_handle_t;
#endif
#ifndef _NAI_TYPEDEF_SIGNAL_SERV_GLOBAL_T
#define _NAI_TYPEDEF_SIGNAL_SERV_GLOBAL_T
typedef struct nai_signal_serv_global_s nai_signal_serv_global_t;
#endif



#if defined(_WIN32)


struct nai_signal_serv_s {
    nai_evmsg_serv_t serv;
};


struct nai_signal_port_s {
    nai_evmsg_port_t port;
};


#else


struct nai_signal_serv_s {
    nai_evmsg_serv_t serv;
    nai_atomic32_t sem;         /**< the lock used in signal handler */
    sigset_t saved;             /**< the saved signal set */
};


struct nai_signal_port_s {
    nai_evmsg_port_t port;
    nai_evnode_cb_f cb;         /**< the origin callback */
    nai_fd_t pipe[2];           /**< the pipes to transport signal */
    uint32_t invalid;           /**< is invalid after fork */
    size_t buflen;              /**< the length of receive buffer */
    int8_t buf[128];            /**< the buffer */
};


#endif



struct nai_signal_node_s {
    nai_evmsg_node_t node;      /**< the signal node */
    nai_int_t signum;           /**< the signal number */
};


struct nai_signal_handle_s {
    nai_evmsg_handle_t h;       /**< the signal handle */
    nai_int_t signum;           /**< the signal number */
};


struct nai_signal_serv_global_s {
    nai_int_t inited;           /**< the mark of initialized */
    nai_int_t error;            /**< the last error */
    nai_once_t once;            /**< the once */
    nai_signal_serv_t serv;     /**< the service */
};



static nai_signal_serv_global_t nai_sigserv = {
    0, 0, NAI_ONCE_INIT
};



//////////////////////////////////////////////////////////////////////////////
// platform-independent implementation


static void nai_signal_handle(nai_int_t signum)
{
    nai_int_t r;
    nai_signal_serv_t* s;


    s = &nai_sigserv.serv;
    r = nai_evmsg_serv_send(&s->serv, &signum, &signum, sizeof(signum));
    if (r < 0) {
        goto _fail;
    };

_end:
    return;

_fail:
    /* failed */
    goto _end;
};


static nai_rbnode_t** nai_signal_find(
    nai_rbtree_t* t, void* key, nai_rbnode_t** pparent)
{
    nai_int_t signum;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_signal_node_t* e;


    signum = *(nai_int_t*)key;
    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_signal_node_t, node.node);
        if (e->signum == signum) {
            break;
        } else if (e->signum >= signum) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_int_t nai_signal_serv_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_int_t signum;
    nai_sighandle_f h;
    nai_signal_node_t* g;


    signum = *(nai_int_t*)key;
    g = (nai_signal_node_t*)e;
    g->signum = signum;
    h = nai_signal(g->signum, nai_signal_handle);
    if (h == SIG_ERR) {
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_signal_serv_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_sighandle_f h;
    nai_signal_node_t* g;


    g = (nai_signal_node_t*)e;
    h = nai_signal(g->signum, SIG_DFL);
    if (h == SIG_ERR) {
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_signal_port_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_signal_node_t* n;
    nai_signal_node_t* g;


    (void)key;

    n = (nai_signal_node_t*)e;
    g = (nai_signal_node_t*)e->glob;
    n->signum = g->signum;
    n->node.share = 1;
    r = 0;

    return r;
};


static nai_int_t nai_signal_port_node_close(nai_evmsg_node_t* e)
{
    (void)e;
    return 0;
};


static nai_int_t nai_signal_handle_init(nai_evmsg_handle_t* p, void* key)
{
    nai_int_t r;
    nai_int_t signum;
    nai_signal_handle_t* h;


    signum = *(nai_int_t*)key;
    h = (nai_signal_handle_t*)p;
    h->signum = signum;
    r = 0;

    return r;
};


static nai_int_t nai_signal_handle_call(
    nai_evmsg_handle_t* p, void* data, size_t len)
{
    nai_int_t r;
    nai_signal_handle_t* h;


    (void)len;
    (void)data;

    h = (nai_signal_handle_t*)p;
    if (h->h.handle->cb) {
        h->h.handle->cb(h->h.handle, nai_ev_signal_from(h->signum));
    };

    r = 0;

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// platform-related implementation


#if defined(_WIN32)



static nai_int_t nai_signal_serv_lock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    nai_int_t r;


    (void)block;

    r = nai_spin_lock(&s->lock);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_signal_serv_unlock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    nai_int_t r;


    (void)block;

    r = nai_spin_unlock(&s->lock);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_signal_port_init(nai_evmsg_port_t* p)
{
    (void)p;
    return 0;
};


static nai_int_t nai_signal_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    (void)p;
    (void)l;
    return 0;
};


static nai_int_t nai_signal_port_close(nai_evmsg_port_t* p)
{
    (void)p;
    return 0;
};


static nai_int_t nai_signal_port_rearm(nai_evmsg_port_t* p)
{
    (void)p;
    return 0;
};


#else


#include "nai/os/unix/nai_unix.h"


typedef struct nai_signal_msg_s {
    nai_evmsg_node_t* e;
    int32_t token;
    nai_int_t signum;
} nai_signal_msg_t;



static nai_int_t nai_signal_serv_lock_map(
    nai_evmsg_serv_t* p, nai_int_t block)
{
    nai_int_t r;
    nai_signal_serv_t* s;
    sigset_t set;


    s = (nai_signal_serv_t*)p;
    if (block) {
        sigfillset(&set);
        sigemptyset(&s->saved);

        r = nai_sigprocmask(SIG_SETMASK, &set, &s->saved);
        if (r < 0) {
            goto _fail;
        };
    };

    r = nai_atomic32_lock(&s->sem);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_signal_serv_unlock_map(
    nai_evmsg_serv_t* p, nai_int_t block)
{
    nai_int_t r;
    nai_signal_serv_t* s;


    s = (nai_signal_serv_t*)p;
    r = nai_atomic32_unlock(&s->sem);
    if (r < 0) {
        goto _fail;
    };

    if (block) {
        r = nai_sigprocmask(SIG_SETMASK, &s->saved, 0);
        if (r < 0) {
            goto _fail;
        };
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static void nai_signal_serv_child()
{
    nai_signal_serv_t* s;
    nai_signal_port_t* p;
    nai_list_entry_t* n;


    if (nai_sigserv.inited) {
        s = &nai_sigserv.serv;
        n = s->serv.ports.next;
        for ( ; n != &s->serv.ports; ) {
            p = nai_containof(n, nai_signal_port_t, port.ents);
            n = n->next;
            p->invalid = 1;
        };
    };

    return;
};


static nai_int_t nai_signal_port_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_signal_port_t* p;


    p = nai_containof(e, nai_signal_port_t, port.ev);
    switch (events & NAI_EV_MASK) {
    case NAI_EV_READ:
        p->cb(e, nai_ev_notify_from(NAI_EVMSG_PORT_READ));
        break;

    default:
        p->cb(e, events);
        break;
    };

    r = 0;

    return r;
};


static nai_int_t nai_signal_port_init(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_signal_port_t* s;


    s = (nai_signal_port_t*)p;
    s->cb = 0;
    s->invalid = 0;
    s->buflen = 0;
    s->pipe[0] = NAI_FD_INVALID;
    s->pipe[1] = NAI_FD_INVALID;
    r = 0;

    return r;
};


static nai_int_t nai_signal_port_wait(nai_evmsg_port_t* p, uint32_t msec)
{
    intptr_t r;
    nai_int_t ec;
    nai_signal_port_t* s;


    s = (nai_signal_port_t*)p;
    while (1) {
        r = nai_file_read(s->pipe[0], 
            s->buf + s->buflen, sizeof(s->buf) - s->buflen);
        if (r < 0) {
            ec = nai_errno;
            if (ec != NAI_EAGAIN) {
                break;
            };
            if (!msec) {
                r = 0;
                break;
            };

            r = nai_file_poll(s->pipe[0], NAI_POLL_READ, msec);
            if (r < 0) {
                break;
            };

            continue;
        };
        if (r == 0) {
            break;
        };

        s->buflen += r;
        if (s->buflen >= sizeof(nai_signal_msg_t)) {
            r = 1;
            break;
        };
    };

    return r;
};


static nai_int_t nai_signal_port_read(nai_evmsg_port_t* p)
{
    nai_int_t r;
    size_t len;
    nai_signal_msg_t* m;
    nai_signal_port_t* s;
    nai_evmsg_hdr_t h;


    s = (nai_signal_port_t*)p;

    /* handling signals */
    len = s->buflen;
    m = (nai_signal_msg_t*)s->buf;
    for ( ; len >= sizeof(*m); m ++, len -= sizeof(*m)) {
        h.e = m->e;
        h.token = m->token;
        h.len = sizeof(m->signum);
        nai_evmsg_port_call_msg(p, &h, &m->signum);
    };

    s->buflen = len;
    if (len > 0) {
        nai_memcpy(s->buf, m, len);
    };

    r = 0;

    return r;
};


static nai_int_t nai_signal_port_send(
    nai_evmsg_port_t* p, nai_evmsg_node_t* e, void* data, size_t len)
{
    intptr_t r;
    nai_signal_port_t* s;
    nai_signal_msg_t msg;


    (void)len;

    s = (nai_signal_port_t*)p;
    if (s->invalid) {
        r = 0;
    } else {
        msg.e = e;
        msg.token = e->token;
        msg.signum = *(nai_int_t*)data;
        r = nai_file_write(s->pipe[1], &msg, sizeof(msg));
        if (r < 0) {
            goto _fail;
        };
    };

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_signal_port_close(nai_evmsg_port_t* p);
static nai_int_t nai_signal_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t ec;
    nai_signal_port_t* s;


    (void)l;

    s = (nai_signal_port_t*)p;
    r = nai_pipe(s->pipe, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_file_set_blocking(s->pipe[0], 0);
    if (r < 0) {
        goto _fail;
    };

    s->invalid = 0;
    s->cb = nai_evnode_get_cb(&s->port.ev);
    nai_evnode_set_cb(&s->port.ev, nai_signal_port_handle);
    nai_evnode_set_fd(&s->port.ev, s->pipe[0], NAI_FD_TYPE_PIPE);
    nai_evnode_set_event(&s->port.ev, NAI_EV_SET, NAI_EV_READ);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_signal_port_close(p);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_signal_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_signal_port_t* s;


    s = (nai_signal_port_t*)p;
    for (n = 0; n < (nai_int_t)nai_countof(s->pipe); n ++) {
        if (s->pipe[n] != NAI_FD_INVALID) {
            nai_file_close(s->pipe[n]);
            s->pipe[n] = NAI_FD_INVALID;
        };
    };

    nai_evnode_set_cb(&s->port.ev, s->cb);
    nai_evnode_set_fd(&s->port.ev, NAI_FD_INVALID, 0);
    r = 0;

    return r;
};


static nai_int_t nai_signal_port_rearm(nai_evmsg_port_t* p)
{
    nai_int_t r;


    nai_signal_port_close(p);
    r = nai_signal_port_open(p, 0);

    return r;
};


#endif /* !_WIN32 */



//////////////////////////////////////////////////////////////////////////////
// signal


static nai_evmsg_ops_t nai_signal_ops = {
    "signal", 
    sizeof(nai_signal_port_t) - sizeof(nai_evmsg_port_t), 
    sizeof(nai_signal_node_t) - sizeof(nai_evmsg_node_t), 
    sizeof(nai_signal_handle_t) - sizeof(nai_evmsg_handle_t), 
    nai_signal_find, 
    nai_signal_serv_lock_map, 
    nai_signal_serv_unlock_map, 
    nai_signal_port_init, 
    nai_signal_port_open, 
#if defined(_WIN32)
    0, 0, 0, 
#else
    nai_signal_port_wait, 
    nai_signal_port_read, 
    nai_signal_port_send, 
#endif
    nai_signal_port_close, 
    nai_signal_port_rearm, 
    nai_signal_serv_node_open, 
    nai_signal_serv_node_close, 
    nai_signal_port_node_open, 
    nai_signal_port_node_close, 
    nai_signal_handle_init, 
    nai_signal_handle_call, 
};


static void nai_signal_serv_global_init()
{
    nai_int_t r;
    nai_signal_serv_t* s;


    s = &nai_sigserv.serv;
    r = nai_evmsg_serv_init(&s->serv);
    if (r < 0) {
        nai_sigserv.error = nai_errno;
        goto _end;
    };

    r = nai_evmsg_serv_open(&s->serv, &nai_signal_ops);
    if (r < 0) {
        nai_sigserv.error = nai_errno;
        goto _end;
    };

_end:
    nai_memory_barrier();
    nai_sigserv.inited = 1;
    return;
};


static void nai_signal_serv_global_term()
{
    if (nai_sigserv.inited) {
        nai_evmsg_serv_close(&nai_sigserv.serv.serv);
    };

    return;
};


static nai_signal_serv_t* nai_signal_serv_get()
{
    nai_signal_serv_t* r;


    if (nai_sigserv.inited == 0) {
        nai_once(&nai_sigserv.once, nai_signal_serv_global_init);
    };
    if (nai_sigserv.error) {
        nai_errno = nai_sigserv.error;
        r = 0;
        goto _end;
    };

    r = &nai_sigserv.serv;

_end:
    return r;
};


static nai_evmsg_port_t* nai_signal_get_port(nai_evloop_t* l, nai_int_t na)
{
    nai_evmsg_port_t* p;
    nai_signal_serv_t* s;


    s = nai_signal_serv_get();
    if (s == 0) {
        p = 0;
        goto _end;
    };

    p = nai_evmsg_port_get(&s->serv, l, na);

_end:
    return p;
};


nai_int_t nai_signal_open(nai_signal_t* s, nai_evloop_t* l, nai_int_t signum)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evmsg_port_t* p;
    nai_evmsg_handle_t* h;


    if (signum <= 0 || 
        signum > UINT8_MAX) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (nai_signal_is_opened(s)) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    p = nai_signal_get_port(l, NAI_EVMSG_PORT_GET);
    if (p == 0) {
        r = -1;
        goto _end;
    };


    s->extend = NAI_EV_EXTEND_SIGNAL;
    s->extval = signum;

    h = nai_evmsg_port_emit_make(p, s, NAI_EVMSG_ADD, &signum);
    if (h == 0) {
        ec = nai_errno;
        r = -1;
        goto _fail;
    };

    if (l != 0) {
        r = nai_evnode_open(s, l);
        if (r < 0) {
            ec = nai_errno;
            nai_evmsg_port_emit_cancel(p, h);
            goto _fail;
        };
    };

    r = nai_evmsg_port_emit_exec(p, h);
    if (r < 0) {
        ec = nai_errno;

        if (l != 0) {
            r = nai_evnode_close(s);
            assert(r >= 0);
        };
        goto _fail;
    };

_end:
    return r;

_fail:
    s->extend = 0;
    s->extval = 0;
    nai_evmsg_port_release(p);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_signal_ext_close(nai_signal_t* s)
{
    nai_int_t r;
    nai_int_t signum;
    nai_evloop_t* l;
    nai_evmsg_port_t* p;


    assert(s->extend == NAI_EV_EXTEND_SIGNAL);
    assert(s->extval != 0);

    l = s->loop;
    p = nai_signal_get_port(l, NAI_EVMSG_PORT_GET_NOREF);
    if (p == 0) {
        r = -1;
        goto _end;
    };

    if (l != 0) {
        r = nai_evloop_del_handle(l, s);
    } else {
        r = 0;
    };
    if (r < 0) {
        goto _end;
    };

    signum = s->extval;
    r = nai_evmsg_port_emit(p, s, NAI_EVMSG_DEL, &signum);
    if (r < 0) {
        goto _end;
    };

    r = nai_evmsg_port_release(p);
    if (r < 0) {
        goto _end;
    };

    s->extend = 0;
    s->extval = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_signal_wait(uint32_t msec)
{
    nai_int_t r;
    nai_signal_serv_t* s;


    s = nai_signal_serv_get();
    if (s == 0) {
        r = -1;
        goto _end;
    };

    r = nai_evmsg_serv_wait(&s->serv, msec);

_end:
    return r;
};


void nai_signal_term()
{
    nai_signal_serv_global_term();

    return;
};


void nai_signal_fork(nai_int_t at)
{
    switch (at) {
    case 2: /* at child */
#if !defined(_WIN32)
        nai_signal_serv_child();
#endif
        break;

    default:
        break;
    };

    return;
};


