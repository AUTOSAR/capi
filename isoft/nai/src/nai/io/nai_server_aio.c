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
/// @file       nai_server_aio.c
/// @brief      
/// @details
/// @date       2023-09-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// aio server


static nai_int_t nai_server_aio_open(nai_server_t* s, nai_evloop_t* l);
static nai_int_t nai_server_aio_close(nai_server_t* s);
static nai_int_t nai_server_aio_bind(
    nai_server_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen);
static nai_int_t nai_server_aio_setopt(
    nai_server_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_server_aio_getopt(
    nai_server_t* s, nai_int_t opt, intptr_t* value);
static nai_int_t nai_server_aio_shutdown(nai_server_t* s, nai_int_t how);
static nai_int_t nai_server_aio_cache(
    nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len);
static nai_fd_t nai_server_aio_accept(
    nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen);


#define NAI_AIO_FEAT (NAI_EV_FEAT_ASYNC|NAI_EV_FEAT_BLOCK)


nai_server_ops_t nai_server_sock_aio = {
    "sock-server-aio", 
    NAI_AIO_FEAT|NAI_EV_FEAT_SOCK,
    nai_server_aio_open, 
    nai_server_aio_setopt,
    nai_server_aio_getopt, 
    nai_server_aio_shutdown, 
    nai_server_aio_close, 
    nai_server_aio_bind, 
    nai_server_aio_cache, 
    nai_server_aio_accept, 
};


extern nai_server_ops_t nai_server_sock_nb;


//////////////////////////////////////////////////////////////////////////////
// open, bind, close, setopt and getopt 


#define NAI_LISTEN_AIO_DONE         NAI_IOBASE_AIO_DONE
#define NAI_LISTEN_AIO_PENDING      NAI_IOBASE_AIO_PENDING
#define NAI_LISTEN_AIO_COMPLETED    NAI_IOBASE_AIO_COMPLETED
#define NAI_LISTEN_AIO_ERROR        NAI_IOBASE_AIO_ERROR


typedef struct nai_iobase_aio_s nai_server_aio_t;


#define nai_server_aio_free(a)                  \
    nai_iobase_aio_free(a)                      \

#define nai_server_aio_init(s)                  \
    nai_iobase_aio_init(s)                      \

#define nai_server_aio_add_evloop(s, l, a)      \
    nai_iobase_aio_add_evloop(s, l, a)          \

#define nai_server_aio_wait(a, w, t)            \
    nai_iobase_aio_wait(a, w, t)                \


static nai_int_t nai_server_aio_open(nai_server_t* s, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t value;
    nai_fd_t fd;


    if (!nai_aiofeat.acceptex) {
        value = 0;
    } else {
#if defined(SO_DOMAIN)
        nai_int_t n;
        fd = nai_server_get_fd(s);
        n = sizeof(value);
        r = nai_sock_get_opt(fd, SOL_SOCKET, SO_DOMAIN, (char*)&value, &n);
#else
        nai_socknbuf_t nbuf;
        fd = nai_server_get_fd(s);
        nbuf.len = sizeof(nbuf.storage);
        r = nai_sock_get_sockname(fd, &nbuf.addr, &nbuf.len);
        if (r < 0) {
            goto _end;
        };
        value = nbuf.addr.sa_family;
#endif
    };

    r = nai_iobase_aio_open(s, l, value);

#if !defined(SO_DOMAIN)
_end:
#endif
    return r;
};


static nai_int_t nai_server_aio_close(nai_server_t* s)
{
    return nai_iobase_aio_close(s);
};


static nai_int_t nai_server_aio_bind(
    nai_server_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_iobase_aio_bind(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_STREAM);
};


static nai_int_t nai_server_aio_getopt(
    nai_server_t* s, nai_int_t opt, intptr_t* value)
{
    return nai_iobase_aio_getopt(s, opt, value);
};


static nai_int_t nai_server_aio_setopt(
    nai_server_t* s, nai_int_t opt, intptr_t value)
{
    return nai_iobase_aio_setopt(s, opt, value);
};


static nai_int_t nai_server_aio_shutdown(nai_server_t* s, nai_int_t how)
{
    (void)s;
    (void)how;

    return 0;
};


static nai_fd_t nai_server_aio_result(
    nai_server_t* s, nai_server_aio_t* a, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t nlen;
    nai_fd_t fd;
    nai_fd_t fn;
    nai_sockaddr_t* n;

    if (name) {
        if (namelen == 0) {
            fn = NAI_FD_INVALID;
            nai_errno = EINVAL; 
            goto _end;
        };
    };

    if (nai_aiofeat.acceptex) {
        fd = nai_evnode_get_fd(&s->ev);
        nai_aio_acceptex_end(fd, a->sock, a->bufs, 
            0, NAI_LISTEN_BUFSIZE, 
            0, 0, &n, &nlen);
    } else {
        n = (nai_sockaddr_t*)a->bufs;
        nlen = a->anamelen;
    };

    if (name) {
        if (nlen > *namelen) {
            fn = NAI_FD_INVALID;
            nai_errno = ENOBUFS;
            goto _end;
        };

        nai_memcpy(name, n, nlen);
        namelen[0] = nlen;
    };

    fn = a->sock;
    if (nai_aiofeat.acceptex) {
        a->last = a->sock;
        a->sock = NAI_FD_INVALID;
    };

_end:
    return fn;
};


static nai_fd_t nai_server_aio_accept(
    nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;
    nai_fd_t fd;
    nai_fd_t fn = NAI_FD_INVALID;
    nai_server_aio_t* a;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        goto _end;
    };


    a = (nai_server_aio_t*)s->st.ctx;
    switch (a->readstat) {
    case NAI_LISTEN_AIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            goto _end;
        };
        r = nai_server_aio_wait(a, 0, s->st.timeout[0]);
        if (r < 0) {
            goto _end;
        };
        break;
    default:
        break;
    };

    switch (a->readstat) {
    case NAI_LISTEN_AIO_COMPLETED:
        a->readstat = NAI_LISTEN_AIO_DONE;
        r = a->readsult;
        if (r == -1) {
            r = nai_aio_result(&a->readop, 0);
            if (r < 0) {
                goto _end;
            };
        };

        goto _done;

    case NAI_LISTEN_AIO_ERROR:
        nai_errno = (nai_int_t)a->readsult;
        goto _end;
    default:
        break;
    };


    /* prepare aio */
    if (s->st.blocking) {
        fn  = nai_server_sock_nb.accept(s, name, namelen);
        a->last = NAI_FD_INVALID;
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    /* start accept */
    fd = nai_evnode_get_fd(&s->ev);
    if (!nai_aiofeat.acceptex) {

        if (namelen) {
            a->anamelen = *namelen;
        } else {
            a->anamelen = NAI_LISTEN_BUFSIZE;
        };

        /* start accept */
        r = (intptr_t)nai_aio_accept(&a->readop, 
            fd, (nai_sockaddr_t*)a->bufs, &a->anamelen);

    } else {
        /* create socket before accept */
        if (a->sock == NAI_FD_INVALID) {
            a->sock = nai_sock_open(a->af, 
                nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_STREAM, 0);
            if (a->sock == NAI_FD_INVALID) {
                goto _end;
            };
        };

        /* start accept */
        r = nai_aio_acceptex(&a->readop, 
            fd, a->sock, a->bufs, 0, NAI_LISTEN_BUFSIZE);
    };
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _end;
        };
    } else if (a->finishskip) {
        goto _done;
    };


    /* non-blocking return immediately */
    if (!s->st.blocking) {

        /* mark async io is pending */
        a->readstat = NAI_LISTEN_AIO_PENDING;
        a->refs ++;

        /* set timer */
        r = nai_server_blocked(s, NAI_EV_READ);
        if (r < 0) {
            /* do nothing */
            ;
        };

        nai_errno = EINPROGRESS;
        goto _end;
    };


    /* try get result to avoid getting stuck in the kernel */
    r = nai_aio_result(&a->readop, 0);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _end;
        };

        /* wait result */
        r = nai_aio_wait(&a->readop, s->st.timeout[0]);
        if (r < 0) {
            /* whether is timedout or failed, aio_op should be canceled */
            nai_aio_cancel(&a->readop);
        };

        /* wait until finish */
        r = nai_aio_result(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
    };


_done:
    if (!nai_aiofeat.acceptex) {
        a->sock = (nai_fd_t)r;
    };

    fn = nai_server_aio_result(s, a, name, namelen);


_end:
    return fn;
};


static nai_int_t nai_server_aio_cache(
    nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len)
{
    nai_int_t r;
    nai_int_t nlen;
    nai_fd_t fd;
    nai_sockaddr_t* n;
    nai_server_aio_t* a;


    if (!nai_aiofeat.acceptex) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    a = (nai_server_aio_t*)s->st.ctx;
    if (a->last == NAI_FD_INVALID) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    r = 0;
    if (buf) {
        fd = nai_evnode_get_fd(&s->ev);
        if (opt == NAI_IO_CACHE_SOCKNAME) {
            nai_aio_acceptex_end(fd, a->last, a->bufs, 
                0, NAI_LISTEN_BUFSIZE, 
                &n, &nlen, 0, 0);
        } else {
            nai_aio_acceptex_end(fd, a->last, a->bufs, 
                0, NAI_LISTEN_BUFSIZE, 
                0, 0, &n, &nlen);
        };

        if (nlen > len) {
            nai_errno = ENOBUFS;
            r = -1;
            goto _end;
        };

        nai_memcpy(buf, n, nlen);
        r = nlen;
    };

_end:
    return r;
};


