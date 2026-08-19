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
/// @file       nai_dgram_aio.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// aio dgram


static nai_int_t nai_dgram_aio_open(nai_dgram_t* s, nai_evloop_t* l);
static nai_int_t nai_dgram_aio_close(nai_dgram_t* s);
static nai_int_t nai_dgram_aio_setopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_dgram_aio_getopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t* value);


static nai_int_t nai_dgram_aio_bind(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);
static nai_int_t nai_dgram_aio_connect(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);


static intptr_t nai_dgram_aio_read(
    nai_dgram_t* s, void* buf, size_t len);
static intptr_t nai_dgram_aio_readv(
    nai_dgram_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_dgram_aio_readq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit);

static intptr_t nai_dgram_aio_write(
    nai_dgram_t* s, const void* buf, size_t len);
static intptr_t nai_dgram_aio_writev(
    nai_dgram_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_dgram_aio_writeq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit);


extern intptr_t nai_dgram_aio_recv(nai_dgram_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen);
extern intptr_t nai_dgram_aio_recvm(nai_dgram_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen);

extern intptr_t nai_dgram_aio_send(nai_dgram_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen);
extern intptr_t nai_dgram_aio_sendm(nai_dgram_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen);
extern intptr_t nai_dgram_aio_sendmm(nai_dgram_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen);


#define nai_dgram_aio_shutdown      nai_iobase_aio_shutdown



#define NAI_AIO_FEAT (NAI_EV_FEAT_ASYNC|NAI_EV_FEAT_BLOCK)


nai_dgram_ops_t nai_dgram_sock_aio = {
    "sock-dgram-aio", 
    NAI_AIO_FEAT|NAI_EV_FEAT_SOCK,
    nai_dgram_aio_open, 
    nai_dgram_aio_setopt,
    nai_dgram_aio_getopt, 
    nai_dgram_aio_shutdown, 
    nai_dgram_aio_close, 
    nai_dgram_aio_bind, 
    nai_dgram_aio_connect, 
    nai_dgram_aio_read, 
    nai_dgram_aio_readv, 
    nai_dgram_aio_readq, 
    nai_dgram_aio_write, 
    nai_dgram_aio_writev, 
    nai_dgram_aio_writeq, 
    nai_dgram_aio_recv, 
    nai_dgram_aio_recvm, 
    nai_dgram_aio_send, 
    nai_dgram_aio_sendm, 
    nai_dgram_aio_sendmm, 
};


extern nai_dgram_ops_t nai_dgram_sock_nb;



//////////////////////////////////////////////////////////////////////////////
// open, connect, close, setopt and getopt 


#define NAI_DGRAM_AIO_DONE          NAI_IOBASE_AIO_DONE
#define NAI_DGRAM_AIO_PENDING       NAI_IOBASE_AIO_PENDING
#define NAI_DGRAM_AIO_COMPLETED     NAI_IOBASE_AIO_COMPLETED
#define NAI_DGRAM_AIO_ERROR         NAI_IOBASE_AIO_ERROR


typedef struct nai_iobase_aio_s nai_dgram_aio_t;


#define nai_dgram_aio_free(a)                   \
    nai_iobase_aio_free(a)                      \

#define nai_dgram_aio_init(s)                   \
    nai_iobase_aio_init(s)                      \

#define nai_dgram_aio_add_evloop(s, l, a)       \
    nai_iobase_aio_add_evloop(s, l, a)          \

#define nai_dgram_aio_start_connect(s, n, l)    \
    nai_iobase_aio_start_connect(s, n, l)       \


#define nai_dgram_aio_wait(a, w, t)             \
    nai_iobase_aio_wait(a, w, t)                \


static nai_int_t nai_dgram_aio_open(nai_dgram_t* s, nai_evloop_t* l)
{
    return nai_iobase_aio_open(s, l, 0);
};


static nai_int_t nai_dgram_aio_bind(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;


    if (s->st.subtype != NAI_IO_SUBT_DEFAULT) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = nai_iobase_aio_bind(s, l, name, namelen, SOCK_DGRAM);

_end:
    return r;
};


static nai_int_t nai_dgram_aio_connect(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_iobase_aio_connect(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_DGRAM);
};


static nai_int_t nai_dgram_aio_close(nai_dgram_t* s)
{
    return nai_iobase_aio_close((nai_iobase_t*)s);
};


static nai_int_t nai_dgram_aio_getopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t* value)
{
    return nai_iobase_aio_getopt((nai_iobase_t*)s, opt, value);
};


static nai_int_t nai_dgram_aio_setopt(
    nai_dgram_t* s, nai_int_t opt, intptr_t value)
{
    return nai_iobase_aio_setopt((nai_iobase_t*)s, opt, value);
};



//////////////////////////////////////////////////////////////////////////////
// read


static intptr_t nai_dgram_aio_read_sult(
    nai_dgram_t* s, nai_dgram_aio_t* a, 
    nai_buflist_t* l, nai_int_t* namelen, nai_int_t* ctrllen)
{
    intptr_t r;


    switch (a->readstat){
    case NAI_DGRAM_AIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_dgram_aio_wait(a, 0, s->st.timeout[0]);
        if (r < 0) {
            goto _end;
        };
        break;

    default:
        break;
    };

    switch (a->readstat){
    case NAI_DGRAM_AIO_COMPLETED:
        a->readstat = NAI_DGRAM_AIO_DONE;
        r = a->readsult;
        if (r == -1) {
            r = nai_aio_result(&a->readop, 0);
            if (r < 0) {
                goto _end;
            };
        };

        if (l != 0) {
            nai_buflist_wcommit(l, r);
        };
        if (namelen) {
            namelen[0] = a->namelen;
        };
        if (ctrllen) {
            ctrllen[0] = a->ctrllen;
        };
        break;

    case NAI_DGRAM_AIO_ERROR:
        nai_errno = (nai_int_t)a->readsult;
        r = -1;
        break;

    default:
        assert(0);
        nai_errno = NAI_EINTERNAL;
        r = -1;
        break;
    };


_end:
    return r;
};


static intptr_t nai_dgram_aio_read_wait(
    nai_dgram_t* s, nai_dgram_aio_t* a, intptr_t bytes)
{
    intptr_t r;
    nai_int_t ec;


    /* check result */
    if (bytes < 0) {
        ec = nai_errno;
        if (ec != EINPROGRESS) {
            r = -1;
            goto _end;
        };
    };

    /* already finished */
    if (bytes >= 0 && a->finishskip) {
        r = bytes;
        goto _end;
    };


    /* non-blocking return immediately */
    if (!s->st.blocking) {

        /* mark async io is pending */
        a->readstat = NAI_DGRAM_AIO_PENDING;
        a->refs ++;

        /* set timer */
        r = nai_dgram_blocked(s, NAI_EV_READ);
        if (r < 0) {
            /* do nothing */
            ;
        };

        nai_errno = EINPROGRESS;
        r = -1;
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


_end:
    return r;
};


static intptr_t nai_dgram_aio_read(
    nai_dgram_t* s, void* buf, size_t len)
{
    return nai_dgram_aio_recv(s, buf, len, 0, 0, 0);
};


static intptr_t nai_dgram_aio_readv(
    nai_dgram_t* s, nai_bufvec_t* v, nai_int_t count)
{
    return nai_dgram_aio_recvm(s, v, count, 0, 0, 0, 0, 0);
};


static intptr_t nai_dgram_aio_readq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_dgram_aio_t* a;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->readstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_read_sult(s, a, l, 0, 0);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.readq(s, l, limit);
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
    r = nai_aio_recvv(&a->readop, fd, ba.v, ba.count, flags);


    /* waiting */
    r = nai_dgram_aio_read_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


intptr_t nai_dgram_aio_recv(nai_dgram_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;
    nai_int_t* nlenptr;
    nai_fd_t fd;
    nai_dgram_aio_t* a;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->readstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_read_sult(s, a, 0, namelen, 0);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.recv(s, buf, len, flags, name, namelen);
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    nlenptr = 0;
    if (namelen) {
        nlenptr = &a->namelen;
        a->namelen = *namelen;
    };

#if defined(MSG_WAITALL)
    flags |= nai_dgram_is_message(s) ? MSG_WAITALL : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    r = nai_aio_recvfrom(&a->readop, fd, buf, len, flags, name, nlenptr);


    /* waiting */
    r = nai_dgram_aio_read_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };

    if (namelen) {
        namelen[0] = a->namelen;
    };


_end:
    return r;
};


intptr_t nai_dgram_aio_recvm(nai_dgram_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;
    nai_int_t* nlenptr;
    nai_int_t* clenptr;
    nai_fd_t fd;
    nai_dgram_aio_t* a;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->readstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_read_sult(s, a, 0, namelen, ctrllen);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.recvm(
            s, v, count, flags, name, namelen, ctrl, ctrllen);
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    nlenptr = 0;
    if (namelen) {
        nlenptr = &a->namelen;
        a->namelen = *namelen;
    };

    clenptr = 0;
    if (ctrllen) {
        clenptr = &a->ctrllen;
        a->ctrllen = *ctrllen;
    };


#if defined(MSG_WAITALL)
    flags |= nai_dgram_is_message(s) ? MSG_WAITALL : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    if (!nai_aiofeat.recvmsg_sim) {
        if (count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        } else if (ctrl) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        } else if (count <= 0) {
            r = 0;
            goto _end;
        } else {
            r = nai_aio_recvfrom(&a->sendop, 
                fd, v[0].buf, v[0].len, flags, name, nlenptr);
        };
    } else {
        r = nai_aio_recvm(&a->readop, 
            fd, v, count, flags, name, nlenptr, ctrl, clenptr);
    };


    /* waiting */
    r = nai_dgram_aio_read_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };


    if (namelen) {
        namelen[0] = a->namelen;
    };


_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// write


#define nai_dgram_aio_load_and_flush                    \
    nai_iobase_load_and_flush                           \


static intptr_t nai_dgram_aio_write_sult(
    nai_dgram_t* s, nai_dgram_aio_t* a, nai_buflist_t* l)
{
    intptr_t r;


    switch (a->sendstat){
    case NAI_DGRAM_AIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_dgram_aio_wait(a, 1, s->st.timeout[1]);
        if (r < 0) {
            goto _end;
        };
        break;

    default:
        break;
    };

    switch (a->sendstat){
    case NAI_DGRAM_AIO_COMPLETED:
        /* check loader */
        switch (a->loadstat) {
        case NAI_ASYNC_LOAD_COMPLETED:
            r = nai_dgram_aio_load_and_flush(
                s, l, -1, nai_dgram_aio_writev, nai_aiofeat.sendv);
            goto _end;

        case NAI_ASYNC_LOAD_PENDING:
            assert(0);
            break;

        default:
            break;
        };

        a->sendstat = NAI_DGRAM_AIO_DONE;
        r = a->sendsult;
        if (r == -1) {
            r = nai_aio_result(&a->sendop, 0);
            if (r < 0) {
                goto _end;
            };
        };

        if (l != 0) {
            nai_buflist_wcommit(l, r);
        };
        break;

    case NAI_DGRAM_AIO_ERROR:
        nai_errno = (nai_int_t)a->sendsult;
        r = -1;
        break;

    default:
        assert(0);
        nai_errno = NAI_EINTERNAL;
        r = -1;
        break;
    };


_end:
    return r;
};


static intptr_t nai_dgram_aio_write_wait(
    nai_dgram_t* s, nai_dgram_aio_t* a, intptr_t bytes)
{
    intptr_t r;
    nai_int_t ec;


    /* check result */
    if (bytes < 0) {
        ec = nai_errno;
        if (ec != EINPROGRESS) {
            r = -1;
            goto _end;
        };
    };

    /* already finished */
    if (bytes >= 0 && a->finishskip) {
        r = bytes;
        goto _end;
    };


    /* non-blocking return immediately */
    if (!s->st.blocking) {

        /* mark async io is pending */
        a->sendstat = NAI_DGRAM_AIO_PENDING;
        a->refs ++;

        /* set timer */
        r = nai_dgram_blocked(s, NAI_EV_WRITE);
        if (r < 0) {
            /* do nothing */
            ;
        };

        nai_errno = EINPROGRESS;
        r = -1;
        goto _end;
    };


    /* try get result to avoid getting stuck in the kernel */
    r = nai_aio_result(&a->sendop, 0);
    if (r < 0) {
        if (nai_errno != EINPROGRESS) {
            goto _end;
        };

        /* wait result */
        r = nai_aio_wait(&a->sendop, s->st.timeout[1]);
        if (r < 0) {
            /* whether is timedout or failed, aio_op should be canceled */
            nai_aio_cancel(&a->sendop);
        };

        /* wait until finish */
        r = nai_aio_result(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


_end:
    return r;
};


static intptr_t nai_dgram_aio_write(
    nai_dgram_t* s, const void* buf, size_t len)
{
    return nai_dgram_aio_send(s, buf, len, 0, 0, 0);
};


static intptr_t nai_dgram_aio_writev(
    nai_dgram_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    return nai_dgram_aio_sendm(s, v, count, 0, 0, 0, 0, 0);
};


static intptr_t nai_dgram_aio_writeq(
    nai_dgram_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t flags;
    nai_int_t gather;
    nai_fd_t fd;
    nai_dgram_aio_t* a;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_write_sult(s, a, l);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.writeq(s, l, limit);
        goto _end;
    };


    gather = nai_aiofeat.sendv;

    /* try load files */
    switch (a->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        if (!s->st.loadfile) {
            break;
        };
        if (!nai_evloop_back_available(s->ev.loop)) {
            break;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_DONE:
        /* fallthrough */

    default:
        r = nai_dgram_aio_load_and_flush(
            s, l, limit, nai_dgram_aio_writev, gather);
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
        goto _end;
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


    /* start send */
    r = nai_aio_sendv(&a->readop, fd, ba.v, ba.count, flags);


    /* waiting */
    r = nai_dgram_aio_write_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };

    nai_buflist_rcommit(l, r);


_end:
    return r;
};


intptr_t nai_dgram_aio_send(nai_dgram_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_fd_t fd;
    nai_dgram_aio_t* a;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_write_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.send(s, buf, len, flags, name, namelen);
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    /* check load stat */
    switch (a->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)a);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)a, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_sendto(&a->sendop, fd, buf, len, flags, name, namelen);


    /* waiting */
    r = nai_dgram_aio_write_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


intptr_t nai_dgram_aio_sendm(nai_dgram_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    nai_fd_t fd;
    nai_dgram_aio_t* a;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_write_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.sendm(
            s, v, count, flags, name, namelen, ctrl, ctrllen);
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    /* check load stat */
    switch (a->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)a);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)a, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    if (!nai_aiofeat.sendmsg_sim) {
        if (count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        } else if (ctrl) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        } else if (count <= 0) {
            r = 0;
            goto _end;
        } else {
            r = nai_aio_sendto(&a->sendop, 
                fd, v[0].buf, v[0].len, flags, name, namelen);
        };
    } else {
        r = nai_aio_sendm(&a->sendop, 
            fd, v, count, flags, name, namelen, ctrl, ctrllen);
    };


    /* waiting */
    r = nai_dgram_aio_write_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


intptr_t nai_dgram_aio_sendmm(nai_dgram_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_fd_t fd;
    nai_dgram_aio_t* a;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_dgram_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_DGRAM_AIO_DONE) {
        r = nai_dgram_aio_write_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
        r = nai_dgram_sock_nb.sendmm(
            s, v, count, flags, name, namelen);
        goto _end;
#if 0
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    /* check load stat */
    switch (a->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_iobase_load_unhits((nai_iobase_ctx_t*)a);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_iobase_load_discard((nai_iobase_ctx_t*)a, s->st.blocking);
        if (r < 0) {
            goto _end;
        };
        break;
    };


#if defined(MSG_EOR)
    flags |= nai_dgram_is_message(s) ? MSG_EOR : 0;
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_sendmm(&a->sendop, 
        fd, v, count, flags, name, namelen);


    /* waiting */
    r = nai_dgram_aio_write_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};



