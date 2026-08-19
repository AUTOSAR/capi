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
/// @file       nai_dgram_nb.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// nonblock stream


static nai_int_t nai_dgram_nb_open(nai_dgram_t* s, nai_evloop_t* l);
static nai_int_t nai_dgram_nb_close(nai_dgram_t* s);
static nai_int_t nai_dgram_nb_setopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_dgram_nb_getopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t* value);


static nai_int_t nai_dgram_nb_bind(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);
static nai_int_t nai_dgram_nb_connect(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);


static intptr_t nai_dgram_nb_read(
    nai_dgram_t* s, void* buf, size_t len);
static intptr_t nai_dgram_nb_readv(
    nai_dgram_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_dgram_nb_readq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit);

static intptr_t nai_dgram_nb_write(
    nai_dgram_t* s, const void* buf, size_t len);
static intptr_t nai_dgram_nb_writev(
    nai_dgram_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_dgram_nb_writeq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit);


extern intptr_t nai_dgram_nb_recv(nai_dgram_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen);
extern intptr_t nai_dgram_nb_recvm(nai_dgram_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen);

extern intptr_t nai_dgram_nb_send(nai_dgram_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen);
extern intptr_t nai_dgram_nb_sendm(nai_dgram_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen);
extern intptr_t nai_dgram_nb_sendmm(nai_dgram_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen);


#define nai_dgram_nb_shutdown   nai_iobase_nb_shutdown


#define NAI_NB_FEAT (NAI_EV_FEAT_EDGE|NAI_EV_FEAT_LEVEL|NAI_EV_FEAT_BLOCK)


nai_dgram_ops_t nai_dgram_sock_nb = {
    "sock-dgram-nb", 
    NAI_NB_FEAT|NAI_EV_FEAT_SOCK,
    nai_dgram_nb_open, 
    nai_dgram_nb_setopt,
    nai_dgram_nb_getopt, 
    nai_dgram_nb_shutdown, 
    nai_dgram_nb_close, 
    nai_dgram_nb_bind, 
    nai_dgram_nb_connect, 
    nai_dgram_nb_read, 
    nai_dgram_nb_readv, 
    nai_dgram_nb_readq, 
    nai_dgram_nb_write, 
    nai_dgram_nb_writev, 
    nai_dgram_nb_writeq, 
    nai_dgram_nb_recv, 
    nai_dgram_nb_recvm, 
    nai_dgram_nb_send, 
    nai_dgram_nb_sendm, 
    nai_dgram_nb_sendmm, 
};


//////////////////////////////////////////////////////////////////////////////
// open, connect, close, setopt and getopt 


typedef struct nai_iobase_nb_s nai_dgram_nb_t;


#define nai_dgram_nb_init(s)                        \
    nai_iobase_nb_init((nai_iobase_t*)s)            \


#define nai_dgram_nb_add_evloop(s, l)               \
    nai_iobase_nb_add_evloop((nai_iobase_t*)s, l)   \


#undef  nai_dgram_blocked
#define nai_dgram_blocked       nai_iobase_nb_blocked



static nai_int_t nai_dgram_nb_open(nai_dgram_t* s, nai_evloop_t* l)
{
    return nai_iobase_nb_open(s, l, 0);
};


static nai_int_t nai_dgram_nb_bind(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;


    if (s->st.subtype != NAI_IO_SUBT_DEFAULT) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = nai_iobase_nb_bind(s, l, name, namelen, SOCK_DGRAM);

_end:
    return r;
};


static nai_int_t nai_dgram_nb_connect(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_iobase_nb_connect(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_DGRAM);
};


static nai_int_t nai_dgram_nb_close(nai_dgram_t* s)
{
    return nai_iobase_nb_close(s);
};


static nai_int_t nai_dgram_nb_getopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t* value)
{
    return nai_iobase_nb_getopt(s, opt, value);
};


static nai_int_t nai_dgram_nb_setopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t value)
{
    return nai_iobase_nb_setopt(s, opt, value);
};



//////////////////////////////////////////////////////////////////////////////
// read


static intptr_t nai_dgram_nb_read(
    nai_dgram_t* s, void* buf, size_t len)
{
    return nai_dgram_nb_recv(s, buf, len, 0, 0, 0);
};


static intptr_t nai_dgram_nb_readv(
    nai_dgram_t* s, nai_bufvec_t* v, nai_int_t count)
{
    return nai_dgram_nb_recvm(s, v, count, 0, 0, 0, 0, 0);
};


static intptr_t nai_dgram_nb_readq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_stream_get_event(s) & NAI_EV_READ)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    if (nai_buflist_is_empty(l)) {
        r = 0;
        goto _end;
    };

    r = nai_buflist_to_wbufarray(
        l, &ba, v, nai_countof(v), limit, 1);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        goto _end;
    };


#if defined(MSG_WAITALL)
    flags = nai_dgram_is_message(s) ? MSG_WAITALL : 0;
#else
    flags = 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    while (1) {
        r = nai_sock_recvv(fd, ba.v, ba.count, flags);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;


_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};


intptr_t nai_dgram_nb_recv(nai_dgram_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_dgram_get_event(s) & NAI_EV_READ)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


#if defined(MSG_WAITALL)
    flags |= nai_dgram_is_message(s) ? MSG_WAITALL : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_recvfrom(fd, buf, len, flags, name, namelen);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_dgram_sock_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_dgram_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};


intptr_t nai_dgram_nb_recvm(nai_dgram_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_dgram_get_event(s) & NAI_EV_READ)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


#if defined(MSG_WAITALL)
    flags |= nai_dgram_is_message(s) ? MSG_WAITALL : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_recvm(fd, 
            v, count, flags, name, namelen, ctrl, ctrllen);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_dgram_sock_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_dgram_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};



//////////////////////////////////////////////////////////////////////////////
// write


#define nai_dgram_nb_load_and_flush                     \
    nai_iobase_load_and_flush                           \


static intptr_t nai_dgram_nb_write(
    nai_dgram_t* s, const void* buf, size_t len)
{
    return nai_dgram_nb_send(s, buf, len, 0, 0, 0);
};


static intptr_t nai_dgram_nb_writev(
    nai_dgram_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    return nai_dgram_nb_sendm(s, v, count, 0, 0, 0, 0, 0);
};


static intptr_t nai_dgram_nb_writeq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_int_t gather;
    nai_fd_t fd;
    nai_dgram_nb_t* n;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_stream_get_event(s) & NAI_EV_WRITE)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    gather = nai_iofeat.sendv | nai_iofeat.writev;

    /* try load files */
    n = (nai_dgram_nb_t*)s->st.ctx;
    switch (n->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        if (!s->st.loadfile) {
            break;
        };
        if (s->ev.loop && 
            !nai_evloop_back_available(s->ev.loop)) {
            break;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_DONE:
        /* fallthrough */

    default:
        r = nai_dgram_nb_load_and_flush(
            s, l, limit, nai_dgram_nb_writev, gather);
        goto _end;
    };


    if (nai_buflist_is_empty(l)) {
        r = 0;
        goto _end;
    };

    if (gather) {
        r = nai_buflist_to_rbufarray(
            l, &ba, v, nai_countof(v), limit, 1);
    } else {
        ba.v = v;
        ba.count = 1;
        r = nai_buflist_to_rbufvec(l, v, limit, 1);
    };
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };


#if defined(MSG_EOR)
    flags = nai_dgram_is_message(s) ? MSG_EOR : 0;
#else
    flags = 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    while (1) {
        r = nai_sock_sendv(fd, ba.v, ba.count, flags);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    nai_buflist_rcommit(l, r);


_end:
    return r;


_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};


intptr_t nai_dgram_nb_send(nai_dgram_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_dgram_nb_t* n;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_dgram_get_event(s) & NAI_EV_WRITE)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    /* check load stat */
    n = (nai_dgram_nb_t*)s->st.ctx;
    switch (n->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)n);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)n, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_sendto(fd, buf, len, flags, name, namelen);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_dgram_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_dgram_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};


intptr_t nai_dgram_nb_sendm(nai_dgram_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_dgram_nb_t* n;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_dgram_get_event(s) & NAI_EV_WRITE)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    /* check load stat */
    n = (nai_dgram_nb_t*)s->st.ctx;
    switch (n->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)n);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)n, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_sendm(fd, 
            v, count, flags, name, namelen, ctrl, ctrllen);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_dgram_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_dgram_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};


intptr_t nai_dgram_nb_sendmm(nai_dgram_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_dgram_nb_t* n;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };
    if (!(nai_dgram_get_event(s) & NAI_EV_WRITE)) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    /* check load stat */
    n = (nai_dgram_nb_t*)s->st.ctx;
    switch (n->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)n);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)n, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_sendmm(fd, 
            v, count, flags, name, namelen);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_dgram_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_dgram_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
            r = -1;
        };
    };
    goto _end;
};



