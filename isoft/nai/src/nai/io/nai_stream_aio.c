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
/// @file       nai_stream_aio.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// aio stream


static nai_int_t nai_stream_aio_open(nai_stream_t* s, nai_evloop_t* l);
static nai_int_t nai_stream_aio_close(nai_stream_t* s);
static nai_int_t nai_stream_aio_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_stream_aio_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value);

static nai_int_t nai_stream_aio_connect(nai_stream_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

static intptr_t nai_stream_aio_read(
    nai_stream_t* s, void* buf, size_t len);
static intptr_t nai_stream_aio_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_aio_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_aio_file_qread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_file_qreadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qrecv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qrecvv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);

static intptr_t nai_stream_aio_write(
    nai_stream_t* s, const void* buf, size_t len);
static intptr_t nai_stream_aio_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_aio_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_aio_file_qwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_file_qwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qsend(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qsendv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qsendfile(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_aio_sock_qsendfilev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


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


#define NAI_AIO_FEAT (NAI_EV_FEAT_ASYNC|NAI_EV_FEAT_BLOCK)


nai_stream_ops_t nai_stream_file_aio = {
    "file-stream-aio", 
    NAI_AIO_FEAT|NAI_EV_FEAT_FILE|NAI_EV_FEAT_PIPE|NAI_EV_FEAT_DEVC,
    nai_stream_aio_open, 
    nai_stream_aio_setopt,
    nai_stream_aio_getopt, 
    nai_iobase_aio_shutdown, 
    nai_stream_aio_close, 
    nai_stream_no_bind, 
    nai_stream_no_connect, 
    nai_stream_aio_read, 
    nai_stream_aio_readv, 
    nai_stream_aio_readq, 
    nai_stream_aio_write, 
    nai_stream_aio_writev, 
    nai_stream_aio_writeq, 
    nai_stream_no_recv, 
    nai_stream_no_recvm, 
    nai_stream_no_send, 
    nai_stream_no_sendm, 
    nai_stream_no_sendmm
};

nai_stream_ops_t nai_stream_sock_aio = {
    "sock-stream-aio", 
    NAI_AIO_FEAT|NAI_EV_FEAT_SOCK,
    nai_stream_aio_open, 
    nai_stream_aio_setopt,
    nai_stream_aio_getopt, 
    nai_iobase_aio_shutdown, 
    nai_stream_aio_close, 
    nai_stream_no_bind, 
    nai_stream_aio_connect, 
    nai_stream_aio_read, 
    nai_stream_aio_readv, 
    nai_stream_aio_readq, 
    nai_stream_aio_write, 
    nai_stream_aio_writev, 
    nai_stream_aio_writeq, 
    nai_dgram_aio_recv, 
    nai_dgram_aio_recvm, 
    nai_dgram_aio_send, 
    nai_dgram_aio_sendm, 
    nai_dgram_aio_sendmm
};


extern nai_stream_ops_t nai_stream_file_b;
extern nai_stream_ops_t nai_stream_sock_nb;


#if defined(_WIN32)

#define nai_stream_nb_ops(s)                            \
    nai_stream_sock_nb                                  \

#else

#define nai_stream_nb_ops(s)                            \
    (nai_stream_is_sock(s) ?                            \
        nai_stream_sock_nb : nai_stream_file_b)         \

#endif


//////////////////////////////////////////////////////////////////////////////
// open, connect, close, setopt and getopt 


#define NAI_STREAM_AIO_DONE         NAI_IOBASE_AIO_DONE
#define NAI_STREAM_AIO_PENDING      NAI_IOBASE_AIO_PENDING
#define NAI_STREAM_AIO_COMPLETED    NAI_IOBASE_AIO_COMPLETED
#define NAI_STREAM_AIO_ERROR        NAI_IOBASE_AIO_ERROR


typedef struct nai_iobase_aio_s nai_stream_aio_t;
typedef intptr_t (*nai_stream_aio_read_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
typedef intptr_t (*nai_stream_aio_send_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


#define nai_stream_aio_init(s)                          \
    nai_iobase_aio_init(s)                              \


#define nai_stream_aio_add_evloop(s, l, a)              \
    nai_iobase_aio_add_evloop(s, l, a)                  \


#define nai_stream_aio_start_connect(s, n, l)           \
    nai_iobase_aio_start_connect(s, n, l)               \


#define nai_stream_aio_wait(a, w, t)                    \
    nai_iobase_aio_wait(a, w, t)                        \



static nai_int_t nai_stream_aio_init;
static uint8_t nai_stream_aio_is_writev[4] = { 0 };
static uint8_t nai_stream_aio_is_sendfile[4] = { 0 };
static nai_stream_aio_read_f nai_stream_aio_readfn[4] = { 0 };
static nai_stream_aio_send_f nai_stream_aio_sendfn[4] = { 0 };


static nai_int_t nai_stream_aio_iofn_init()
{
    nai_int_t r;
    uint8_t* is_writev;
    uint8_t* is_sendfile;
    nai_stream_aio_read_f* fn;


    if (nai_stream_aio_init) {
        goto _end;
    };


    /* setup read functions */
    fn = nai_stream_aio_readfn;
    if (nai_aiofeat.readv) {
        fn[NAI_IOBASE_IOFN_FILE] = nai_stream_aio_file_qreadv;
        fn[NAI_IOBASE_IOFN_PIPE] = nai_stream_aio_file_qreadv;
    } else {
        fn[NAI_IOBASE_IOFN_FILE] = nai_stream_aio_file_qread;
        fn[NAI_IOBASE_IOFN_PIPE] = nai_stream_aio_file_qread;
    };
    if (nai_aiofeat.recvv) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_sock_qrecvv;
    } else if (nai_aiofeat.readv) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_file_qreadv;
    } else if (nai_aiofeat.recv) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_sock_qrecv;
    } else {
        fn[NAI_IOBASE_IOFN_SOCK] = fn[NAI_IOBASE_IOFN_FILE];
    };
    fn[NAI_IOBASE_IOFN_SNDF] = fn[NAI_IOBASE_IOFN_SOCK];


    /* setup send functions */
    fn = nai_stream_aio_sendfn;
    is_sendfile = nai_stream_aio_is_sendfile;
    if (nai_aiofeat.writev) {
        fn[NAI_IOBASE_IOFN_FILE] = nai_stream_aio_file_qwritev;
        fn[NAI_IOBASE_IOFN_PIPE] = nai_stream_aio_file_qwritev;
    } else {
        fn[NAI_IOBASE_IOFN_FILE] = nai_stream_aio_file_qwrite;
        fn[NAI_IOBASE_IOFN_PIPE] = nai_stream_aio_file_qwrite;
    };
    if (nai_aiofeat.sendv) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_sock_qsendv;
    } else if (nai_aiofeat.writev) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_file_qwritev;
    } else if (nai_aiofeat.send) {
        fn[NAI_IOBASE_IOFN_SOCK] = nai_stream_aio_sock_qsend;
    } else {
        fn[NAI_IOBASE_IOFN_SOCK] = fn[NAI_IOBASE_IOFN_FILE];
    };;
    if (nai_aiofeat.sendfilev) {
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 1;
        fn[NAI_IOBASE_IOFN_SNDF] = nai_stream_aio_sock_qsendfilev;
    } else if (nai_aiofeat.sendfile) {
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 1;
        fn[NAI_IOBASE_IOFN_SNDF] = nai_stream_aio_sock_qsendfile;
    } else {
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 0;
        fn[NAI_IOBASE_IOFN_SNDF] = fn[NAI_IOBASE_IOFN_SOCK];
    };

    /* setup suppoted writev */
    is_writev = nai_stream_aio_is_writev;
    if (nai_aiofeat.writev) {
        is_writev[NAI_IOBASE_IOFN_FILE] = 1;
        is_writev[NAI_IOBASE_IOFN_PIPE] = 1;
    };
    if (nai_aiofeat.sendv || nai_aiofeat.writev) {
        is_writev[NAI_IOBASE_IOFN_SOCK] = 1;
        is_writev[NAI_IOBASE_IOFN_SNDF] = 1;
    };


    /* setup done */
    nai_memory_barrier();
    nai_stream_aio_init = 1;

_end:
    r = 0;
    return r;
};


nai_int_t nai_stream_aio_sel_iofn(nai_stream_t* s)
{
    nai_int_t index;
    nai_stream_aio_t* a;


    nai_stream_aio_iofn_init();

    if (nai_stream_is_seekable(s)) {
        index = NAI_IOBASE_IOFN_FILE;
    } else if (!nai_stream_is_sock(s)) {
        index = NAI_IOBASE_IOFN_PIPE;
    } else {
        if (!s->st.sendfile) {
            index = NAI_IOBASE_IOFN_SOCK;
        } else {
            index = NAI_IOBASE_IOFN_SNDF;
        };
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    a->iofn = index;

    return 0;
};


static nai_int_t nai_stream_aio_open(nai_stream_t* s, nai_evloop_t* l)
{
    return nai_iobase_aio_open(s, l, 0);
};


static nai_int_t nai_stream_aio_connect(nai_stream_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_iobase_aio_connect(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_STREAM);
};


static nai_int_t nai_stream_aio_close(nai_stream_t* s)
{
    return nai_iobase_aio_close(s);
};


static nai_int_t nai_stream_aio_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;

    r = nai_iobase_aio_getopt(s, opt, value);

    return r;
};


static nai_int_t nai_stream_aio_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;

    switch (opt) {
    case NAI_IO_SENDFILE:
        if (s->st.sendfile != !!value) {
            s->st.sendfile = !!value;
            nai_stream_aio_sel_iofn(s);
        };
        r = 0;
        break;

    default:
        r = nai_iobase_aio_setopt(s, opt, value);
        break;
    };

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// read


static intptr_t nai_stream_aio_read_sult(
    nai_stream_t* s, nai_stream_aio_t* a, nai_buflist_t* l)
{
    intptr_t r;


    switch (a->readstat){
    case NAI_STREAM_AIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_stream_aio_wait(a, 0, s->st.timeout[0]);
        if (r < 0) {
            goto _end;
        };
        break;
    default:
        break;
    };

    switch (a->readstat){
    case NAI_STREAM_AIO_COMPLETED:
        a->readstat = NAI_STREAM_AIO_DONE;
        r = a->readsult;
        if (r == -1) {
            r = nai_aio_result(&a->readop, 0);
            if (r < 0) {
                break;
            };
        };

        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
        };
        if (l != 0) {
            nai_buflist_wcommit(l, r);
        };
        break;

    case NAI_STREAM_AIO_ERROR:
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


static intptr_t nai_stream_aio_read_wait(
    nai_stream_t* s, nai_stream_aio_t* a, intptr_t bytes)
{
    intptr_t r;
    nai_int_t ec;


    /* check result */
    if (bytes < 0) {
        ec = nai_errno;
        if (ec == 0) {
            /* no buffer */
            r = 0;
            goto _end;
        } else if (ec != EINPROGRESS) {
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
        a->readstat = NAI_STREAM_AIO_PENDING;
        a->refs ++;

        /* set timer */
        r = nai_stream_blocked(s, NAI_EV_READ);
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


static intptr_t nai_stream_aio_read(nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_stream_aio_t* a;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_read_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s)) {
#endif

            r = nai_stream_nb_ops(s).read(s, buf, len);
            goto _end;

#if defined(_WIN32)
        };

        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    if (!nai_stream_is_sock(s)) {
        r = nai_aio_read(&a->readop, fd, buf, len, s->offset);
    } else {

        flags = 0;
#if defined(MSG_WAITALL)
        if (nai_stream_is_message(s)) {
            flags = MSG_WAITALL;
        };
#endif
        r = nai_aio_recv(&a->readop, fd, buf, len, flags);
    };


    /* waiting */
    r = nai_stream_aio_read_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t flags;
    nai_int_t message;
    nai_fd_t fd;
    nai_stream_aio_t* a;


    if (count <= 0) {
        r = 0;
        goto _end;
    };
    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_read_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s)) {
#endif

            r = nai_stream_nb_ops(s).readv(s, v, count);
            goto _end;

#if defined(_WIN32)
        };

        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
#endif
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    message = nai_stream_is_message(s);
    if (!nai_stream_is_sock(s)) {
        if (nai_aiofeat.readv) {
            r = nai_aio_readv(&a->readop, fd, v, count, s->offset);
        } else {
            if (message && count > 1) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            r = nai_aio_read(
                &a->readop, fd, v[0].buf, v[0].len, s->offset);
        };
    } else {
        flags = 0;
#if defined(MSG_WAITALL)
        if (message) {
            flags = MSG_WAITALL;
        };
#endif
        if (nai_aiofeat.recvv) {
            r = nai_aio_recvv(&a->readop, fd, v, count, flags);
        } else if (nai_aiofeat.readv) {
            r = nai_aio_readv(&a->readop, fd, v, count, 0);
        } else if (!message || count <= 1) {
            r = nai_aio_recv(
                &a->readop, fd, v[0].buf, v[0].len, flags);
        } else {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        };
    };


    /* waiting */
    r = nai_stream_aio_read_wait(s, a, r);
    if (r < 0) {
        goto _end;
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t message;
    size_t max;
    size_t read;
    nai_stream_aio_t* a;
    nai_stream_aio_read_f readfn;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_read_sult(s, a, l);
        goto _end;
    };


    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s)) {
#endif

            r = nai_stream_nb_ops(s).readq(s, l, limit);
            goto _end;

#if defined(_WIN32)
        };
#endif
    };


    /* check subtype */
    message = nai_stream_is_message(s);
    max = NAI_IO_READSIZE;
    if (message) {
        max = limit;
    };

    /* initialize for read */
    readfn = nai_stream_aio_readfn[a->iofn];


    /* start read loop */
    r = 0;
    while (!nai_buflist_is_empty(l)) {
        read = max;
        if (read > limit) {
            read = limit;
        };


        /* start read */
        r = readfn(s, l, read, message);


        /* waiting */
        r = nai_stream_aio_read_wait(s, a, r);
        if (r < 0) {
            goto _end;
        };
        if (r == 0) {
            break;
        };


        /* submit read bytes */
        nai_buflist_wcommit(l, r);

        /* seek file pointer */
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
        };

        break;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_file_qread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_fd_t fd;
    nai_bufvec_t v;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;


    r = nai_buflist_to_wbufvec(l, &v, limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    r = nai_aio_read(&a->readop, fd, v.buf, v.len, s->offset);


_end:
    return r;
};


static intptr_t nai_stream_aio_file_qreadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    r = nai_buflist_to_wbufarray(
        l, &ba, b, nai_countof(b), limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    r = nai_aio_readv(&a->readop, fd, ba.v, ba.count, s->offset);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qrecv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufvec_t v;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;


    r = nai_buflist_to_wbufvec(l, &v, limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    flags = 0;
#if defined(MSG_WAITALL)
    if (message) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    r = nai_aio_recv(&a->readop, fd, v.buf, v.len, flags);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qrecvv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    r = nai_buflist_to_wbufarray(
        l, &ba, b, nai_countof(b), limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->readop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    flags = 0;
#if defined(MSG_WAITALL)
    if (message) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start read */
    r = nai_aio_recvv(&a->readop, fd, ba.v, ba.count, flags);


_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// write


#define nai_stream_aio_load_and_flush                   \
    nai_iobase_load_and_flush                           \


static intptr_t nai_stream_aio_write_sult(
    nai_stream_t* s, nai_stream_aio_t* a, nai_buflist_t* l)
{
    intptr_t r;


    switch (a->sendstat){
    case NAI_STREAM_AIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_stream_aio_wait(a, 1, s->st.timeout[1]);
        if (r < 0) {
            goto _end;
        };
        break;
    default:
        break;
    };

    switch (a->sendstat){
    case NAI_STREAM_AIO_COMPLETED:

        /* check loader */
        switch (a->loadstat) {
        case NAI_ASYNC_LOAD_COMPLETED:
            r = nai_stream_aio_load_and_flush(s, l, -1, 
                nai_stream_aio_writev, nai_stream_aio_is_writev[a->iofn]);
            goto _end;

        case NAI_ASYNC_LOAD_PENDING:
            assert(0);
            break;

        default:
            break;
        };

        a->sendstat = NAI_STREAM_AIO_DONE;
        r = a->sendsult;
        if (r == -1) {
            r = nai_aio_result(&a->sendop, 0);
            if (r < 0) {
                break;
            };
        };

        if (r > 0) {
            if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
                s->offset += r;
            };
            if (l != 0) {
                nai_buflist_rcommit(l, r);
            };
        } else {
            if (a->sendfile) {
                /* file eof */
                nai_errno = ENODATA;
                r = -1;
            };
        };
        break;

    case NAI_STREAM_AIO_ERROR:
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


static intptr_t nai_stream_aio_write_wait(
    nai_stream_t* s, nai_stream_aio_t* a, intptr_t bytes, nai_int_t sendfile)
{
    intptr_t r;
    nai_int_t ec;


    /* check result */
    if (bytes < 0) {
        ec = nai_errno;
        if (ec == 0) {
            /* no data */
            r = 0;
            goto _end;
        } else if (ec != EINPROGRESS) {
            r = -1;
            goto _end;
        };
    };

    /* already finished */
    if (bytes >= 0 && a->finishskip) {
        r = bytes;
        goto _done;
    };


    /* non-blocking return immediately */
    if (!s->st.blocking) {

        /* mark async io is pending */
        a->sendstat = NAI_STREAM_AIO_PENDING;
        a->refs ++;
        a->sendfile = !!sendfile;


        /* set timer */
        r = nai_stream_blocked(s, NAI_EV_WRITE);
        if (r < 0) {
            /* do nothing */
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

        /* wait until result */
        r = nai_aio_result(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


_done:
    if (r == 0 && sendfile) {
        /* file eof */
        nai_errno = ENODATA;
        r = -1;
    };

_end:
    return r;
};


static intptr_t nai_stream_aio_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_stream_aio_t* a;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_write_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s)) {
#endif

            r = nai_stream_nb_ops(s).write(s, buf, len);
            goto _end;

#if defined(_WIN32)
        };

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


    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    if (!nai_stream_is_sock(s)) {
        r = nai_aio_write(&a->sendop, fd, buf, len, s->offset);
    } else {

        flags = 0;
#if defined(MSG_EOR)
        if (nai_stream_is_message(s)) {
            flags = MSG_EOR;
        };
#endif
        r = nai_aio_send(&a->sendop, fd, buf, len, flags);
    };


    /* waiting */
    r = nai_stream_aio_write_wait(s, a, r, 0);
    if (r < 0) {
        goto _end;
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t flags;
    nai_int_t message;
    nai_fd_t fd;
    nai_stream_aio_t* a;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_write_sult(s, a, 0);
        goto _end;
    };


    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s)) {
#endif

            r = nai_stream_nb_ops(s).writev(s, v, count);
            goto _end;

#if defined(_WIN32)
        };

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


    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    message = nai_stream_is_message(s);
    if (!nai_stream_is_sock(s)) {
        if (nai_aiofeat.writev) {
            r = nai_aio_writev(&a->sendop, fd, v, count, s->offset);
        } else {
            if (message && count > 1) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            r = nai_aio_write(
                &a->sendop, fd, v[0].buf, v[0].len, s->offset);
        };
    } else {
        flags = 0;
#if defined(MSG_EOR)
        if (message) {
            flags = MSG_EOR;
        };
#endif
        if (nai_aiofeat.sendv) {
            r = nai_aio_sendv(&a->sendop, fd, v, count, flags);
        } else if (nai_aiofeat.writev) {
            r = nai_aio_writev(&a->sendop, fd, v, count, 0);
        } else if (!message || count <= 1) {
            r = nai_aio_send(
                &a->sendop, fd, v[0].buf, v[0].len, flags);
        } else {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        };
    };


    /* waiting */
    r = nai_stream_aio_write_wait(s, a, r, 0);
    if (r < 0) {
        goto _end;
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t message;
    nai_int_t sendfile;
    size_t max;
    size_t write;
    nai_stream_aio_t* a;
    nai_stream_aio_send_f sendfn;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_aio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_AIO_DONE) {
        r = nai_stream_aio_write_sult(s, a, l);
        goto _end;
    };


    sendfile = nai_stream_aio_is_sendfile[a->iofn];

    if (s->st.blocking) {
#if defined(_WIN32)
        /* overlapped file cannot use non-overlapped api on win32, 
         * but socket supported */
        if (nai_stream_is_sock(s) && sendfile == 0) {
#endif

            r = nai_stream_nb_ops(s).writeq(s, l, limit);
            goto _end;

#if defined(_WIN32)
        };
#endif
    };


#if NAI_SIZEOF_SIZE_T == 4
    /* prevent overflow */
    if (limit >= NAI_INT_T_MAX) {
        limit = NAI_INT_T_MAX;
    };
#endif

    /* check subtype */
    message = nai_stream_is_message(s);
    max = NAI_IO_WRITESIZE;
    if (message) {
        max = limit;
    };


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
        if (sendfile) {
            break;
        };

        /* fallthrough */

    default:
        r = nai_stream_aio_load_and_flush(s, l, max, 
            nai_stream_aio_writev, nai_stream_aio_is_writev[a->iofn]);
        goto _end;
    };


    /* initialize for write */
    sendfn = nai_stream_aio_sendfn[a->iofn];


    /* start write loop */
    r = 0;
    while (!nai_buflist_is_empty(l)) {

        write = max;
        if (write > limit) {
            write = limit;
        };


        /* start send */
        r = sendfn(s, l, write, message);


        /* waiting */
        r = nai_stream_aio_write_wait(s, a, r, sendfile);
        if (r < 0) {
            goto _end;
        };

        /* submit wroten bytes */
        nai_buflist_rcommit(l, r);

        /* seek file pointer */
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
        };

        break;
    };


_end:
    return r;
};


static intptr_t nai_stream_aio_file_qwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_fd_t fd;
    nai_bufvec_t v;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;


    r = nai_buflist_to_rbufvec(l, &v, limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    r = nai_aio_write(&a->sendop, fd, v.buf, v.len, s->offset);


_end:
    return r;
};


static intptr_t nai_stream_aio_file_qwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    r = nai_buflist_to_rbufarray(
        l, &ba, b, nai_countof(b), limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    r = nai_aio_writev(&a->sendop, fd, ba.v, ba.count, s->offset);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qsend(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufvec_t v;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;


    r = nai_buflist_to_rbufvec(l, &v, limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    flags = 0;
#if defined(MSG_EOR)
    if (message) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_send(&a->sendop, fd, v.buf, v.len, flags);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qsendv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t flags;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    r = nai_buflist_to_rbufarray(
        l, &ba, b, nai_countof(b), limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    flags = 0;
#if defined(MSG_EOR)
    if (message) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_sendv(&a->sendop, fd, ba.v, ba.count, flags);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qsendfile(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t submit;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_filechunk_t fc;
    nai_bufvec_t b[NAI_BUFV_MAX];

#if defined(_WIN32)
    const nai_int_t max_ht = 1; /* TransmitFile can't accept bufvec array */
#else
    const nai_int_t max_ht = message ? 0 : NAI_BUFV_MAX;
#endif


    r = nai_buflist_to_filechunk(
        l, &fc, b, nai_countof(b), max_ht, limit, message);
    if (r < 0) {
        if (message && nai_errno == EOVERFLOW) {
            r = nai_stream_aio_sock_qsendv(s, l, limit, message);
        };
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_sendfile(&a->sendop, fd, &fc, &submit);


_end:
    return r;
};


static intptr_t nai_stream_aio_sock_qsendfilev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t submit;
    nai_fd_t fd;
    nai_stream_aio_t* a = (nai_stream_aio_t*)s->st.ctx;
    nai_filearray_t fa;
    nai_filevec_t b[NAI_BUFV_MAX];


    (void)message;

    r = nai_buflist_to_filearray(
        l, &fa, b, nai_countof(b), limit, message);
    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = 0;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_aio_set_waitable(&a->sendop, 1);
        if (r < 0) {
            goto _end;
        };
    };


    fd = nai_evnode_get_fd(&s->ev);


    /* start send */
    r = nai_aio_sendfilev(&a->sendop, fd, fa.v, fa.count, &submit);


_end:
    return r;
};


