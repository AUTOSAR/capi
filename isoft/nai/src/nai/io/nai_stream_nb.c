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
/// @file       nai_stream_nb.c
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


static nai_int_t nai_stream_nb_open(nai_stream_t* s, nai_evloop_t* l);
static nai_int_t nai_stream_nb_close(nai_stream_t* s);
static nai_int_t nai_stream_nb_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_stream_nb_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value);

static nai_int_t nai_stream_nb_connect(nai_stream_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);


static intptr_t nai_stream_nb_file_read(
    nai_stream_t* s, void* buf, size_t len);
static intptr_t nai_stream_nb_file_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_nb_sock_read(
    nai_stream_t* s, void* buf, size_t len);
static intptr_t nai_stream_nb_sock_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_nb_file_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_nb_sock_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);

static intptr_t nai_stream_nb_file_pread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_preadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_qread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_qreadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qrecv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qrecvv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


static intptr_t nai_stream_nb_file_write(
    nai_stream_t* s, const void* buf, size_t len);
static intptr_t nai_stream_nb_file_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_nb_sock_write(
    nai_stream_t* s, const void* buf, size_t len);
static intptr_t nai_stream_nb_sock_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_nb_file_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_nb_sock_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);

static intptr_t nai_stream_nb_file_pwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_pwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_qwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_file_qwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qsend(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qsendv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qsendfile(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_nb_sock_qsendfilev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


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


#define NAI_NB_FEAT (NAI_EV_FEAT_EDGE|NAI_EV_FEAT_LEVEL|NAI_EV_FEAT_BLOCK)


nai_stream_ops_t nai_stream_file_b = {
    "file-stream-b", 
    NAI_EV_FEAT_BLOCK|NAI_EV_FEAT_FILE,
    nai_stream_nb_open, 
    nai_stream_nb_setopt,
    nai_stream_nb_getopt, 
    nai_iobase_nb_shutdown, 
    nai_stream_nb_close, 
    nai_stream_no_bind, 
    nai_stream_no_connect, 
    nai_stream_nb_file_read, 
    nai_stream_nb_file_readv, 
    nai_stream_nb_file_readq, 
    nai_stream_nb_file_write, 
    nai_stream_nb_file_writev, 
    nai_stream_nb_file_writeq, 
    nai_stream_no_recv, 
    nai_stream_no_recvm, 
    nai_stream_no_send, 
    nai_stream_no_sendm, 
    nai_stream_no_sendmm
};

nai_stream_ops_t nai_stream_pipe_nb = {
    "pipe-stream-nb", 
    NAI_NB_FEAT|NAI_EV_FEAT_PIPE|NAI_EV_FEAT_DEVC,
    nai_stream_nb_open, 
    nai_stream_nb_setopt,
    nai_stream_nb_getopt, 
    nai_iobase_nb_shutdown, 
    nai_stream_nb_close, 
    nai_stream_no_bind, 
    nai_stream_no_connect, 
    nai_stream_nb_file_read, 
    nai_stream_nb_file_readv, 
    nai_stream_nb_file_readq, 
    nai_stream_nb_file_write, 
    nai_stream_nb_file_writev, 
    nai_stream_nb_file_writeq, 
    nai_stream_no_recv, 
    nai_stream_no_recvm, 
    nai_stream_no_send, 
    nai_stream_no_sendm, 
    nai_stream_no_sendmm
};

nai_stream_ops_t nai_stream_sock_nb = {
    "sock-stream-nb", 
    NAI_NB_FEAT|NAI_EV_FEAT_SOCK,
    nai_stream_nb_open, 
    nai_stream_nb_setopt,
    nai_stream_nb_getopt, 
    nai_iobase_nb_shutdown, 
    nai_stream_nb_close, 
    nai_stream_no_bind, 
    nai_stream_nb_connect, 
    nai_stream_nb_sock_read, 
    nai_stream_nb_sock_readv, 
    nai_stream_nb_sock_readq, 
    nai_stream_nb_sock_write, 
    nai_stream_nb_sock_writev, 
    nai_stream_nb_sock_writeq, 
    nai_dgram_nb_recv, 
    nai_dgram_nb_recvm, 
    nai_dgram_nb_send, 
    nai_dgram_nb_sendm, 
    nai_dgram_nb_sendmm
};


//////////////////////////////////////////////////////////////////////////////
// open, connect, close, setopt and getopt 


#define nai_stream_nb_init(s)                           \
    nai_iobase_nb_init((nai_iobase_t*)s)                \


#define nai_stream_nb_add_evloop(s, l)                  \
    nai_iobase_nb_add_evloop((nai_iobase_t*)s, l)       \


#undef  nai_stream_blocked
#define nai_stream_blocked      nai_iobase_nb_blocked



typedef struct nai_iobase_nb_s nai_stream_nb_t;
typedef intptr_t (*nai_stream_nb_read_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
typedef intptr_t (*nai_stream_nb_send_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


static nai_int_t nai_stream_nb_init;
static uint8_t nai_stream_nb_is_writev[4] = { 0 };
static uint8_t nai_stream_nb_is_sendfile[4] = { 0 };
static nai_stream_nb_read_f nai_stream_nb_readfn[4][2] = { { 0 } };
static nai_stream_nb_send_f nai_stream_nb_sendfn[4][2] = { { 0 } };


static nai_int_t nai_stream_nb_iofn_init()
{
    nai_int_t r;
    uint8_t* is_writev;
    uint8_t* is_sendfile;
    nai_stream_nb_read_f* fn;
    nai_stream_nb_read_f* fs;


    if (nai_stream_nb_init) {
        goto _end;
    };


    /* setup read functions */
    fn = nai_stream_nb_readfn[NAI_IOBASE_IOFN_FILE];
    if (nai_iofeat.preadv) {
        fn[0] = nai_stream_nb_file_preadv;
    } else {
        fn[0] = nai_stream_nb_file_pread;
    };
    if (nai_iofeat.readv) {
        fn[1] = nai_stream_nb_file_qreadv;
    } else {
        fn[1] = nai_stream_nb_file_qread;
    };

    fn = nai_stream_nb_readfn[NAI_IOBASE_IOFN_PIPE];
    if (nai_iofeat.readv) {
        fn[0] = nai_stream_nb_file_qreadv;
        fn[1] = nai_stream_nb_file_qreadv;
    } else {
        fn[0] = nai_stream_nb_file_qread;
        fn[1] = nai_stream_nb_file_qread;
    };

    fn = nai_stream_nb_readfn[NAI_IOBASE_IOFN_SOCK];
    if (nai_iofeat.recvv) {
        fn[0] = nai_stream_nb_sock_qrecvv;
        fn[1] = nai_stream_nb_sock_qrecvv;
    } else if (nai_iofeat.readv) {
        fn[0] = nai_stream_nb_file_qreadv;
        fn[1] = nai_stream_nb_file_qreadv;
    } else {
        fn[0] = nai_stream_nb_sock_qrecv;
        fn[1] = nai_stream_nb_sock_qrecv;
    };

    fn = nai_stream_nb_readfn[NAI_IOBASE_IOFN_SNDF];
    fs = nai_stream_nb_readfn[NAI_IOBASE_IOFN_SOCK];
    fn[0] = fs[0];
    fn[1] = fs[1];


    /* setup send functions */
    fn = nai_stream_nb_sendfn[NAI_IOBASE_IOFN_FILE];
    if (nai_iofeat.pwritev) {
        fn[0] = nai_stream_nb_file_pwritev;
    } else {
        fn[0] = nai_stream_nb_file_pwrite;
    };
    if (nai_iofeat.writev) {
        fn[1] = nai_stream_nb_file_qwritev;
    } else {
        fn[1] = nai_stream_nb_file_qwrite;
    };

    fn = nai_stream_nb_sendfn[NAI_IOBASE_IOFN_PIPE];
    if (nai_iofeat.writev) {
        fn[0] = nai_stream_nb_file_qwritev;
        fn[1] = nai_stream_nb_file_qwritev;
    } else {
        fn[0] = nai_stream_nb_file_qwrite;
        fn[1] = nai_stream_nb_file_qwrite;
    };

    fn = nai_stream_nb_sendfn[NAI_IOBASE_IOFN_SOCK];
    if (nai_iofeat.sendv) {
        fn[0] = nai_stream_nb_sock_qsendv;
        fn[1] = nai_stream_nb_sock_qsendv;
    } else if (nai_iofeat.writev) {
        fn[0] = nai_stream_nb_file_qwritev;
        fn[1] = nai_stream_nb_file_qwritev;
    } else {
        fn[0] = nai_stream_nb_sock_qsend;
        fn[1] = nai_stream_nb_sock_qsend;
    };

    fn = nai_stream_nb_sendfn[NAI_IOBASE_IOFN_SNDF];
    fs = nai_stream_nb_sendfn[NAI_IOBASE_IOFN_SOCK];
    is_sendfile = nai_stream_nb_is_sendfile;
#if !defined(_WIN32)
    if (nai_iofeat.sendfilev) {
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 1;
        fn[0] = nai_stream_nb_sock_qsendfilev;
        fn[1] = nai_stream_nb_sock_qsendfilev;
    } else if (nai_iofeat.sendfile) {
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 1;
        fn[0] = nai_stream_nb_sock_qsendfile;
        fn[1] = nai_stream_nb_sock_qsendfile;
    } else {
#endif
        is_sendfile[NAI_IOBASE_IOFN_SNDF] = 0;
        fn[0] = fs[0];
        fn[1] = fs[1];
#if !defined(_WIN32)
    };
#endif

    /* setup suppoted writev */
    is_writev = nai_stream_nb_is_writev;
    if (nai_iofeat.pwritev || nai_iofeat.writev) {
        is_writev[NAI_IOBASE_IOFN_FILE] = 1;
        is_writev[NAI_IOBASE_IOFN_PIPE] = 1;
    };
    if (nai_iofeat.sendv || nai_iofeat.writev) {
        is_writev[NAI_IOBASE_IOFN_SOCK] = 1;
        is_writev[NAI_IOBASE_IOFN_SNDF] = 1;
    };


    /* setup done */
    nai_memory_barrier();
    nai_stream_nb_init = 1;

_end:
    r = 0;
    return r;
};


nai_int_t nai_stream_nb_sel_iofn(nai_stream_t* s)
{
    nai_int_t index;
    nai_stream_nb_t* n;


    nai_stream_nb_iofn_init();

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

    n = (nai_stream_nb_t*)s->st.ctx;
    n->iofn = index;

    return 0;
};


static nai_int_t nai_stream_nb_open(nai_stream_t* s, nai_evloop_t* l)
{
    return nai_iobase_nb_open(s, l, 0);
};


static nai_int_t nai_stream_nb_connect(nai_stream_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_iobase_nb_connect(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_STREAM);
};


static nai_int_t nai_stream_nb_close(nai_stream_t* s)
{
    return nai_iobase_nb_close(s);
};


static nai_int_t nai_stream_nb_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;

    r = nai_iobase_nb_getopt(s, opt, value);

    return r;
};


static nai_int_t nai_stream_nb_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;


    switch (opt) {
    case NAI_IO_SENDFILE:
        if (s->st.sendfile != !!value) {
            s->st.sendfile = !!value;
            nai_stream_nb_sel_iofn(s);
        };
        r = 0;
        break;

    default:
        r = nai_iobase_nb_setopt(s, opt, value);
        break;
    };

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// read


static intptr_t nai_stream_nb_file_read(
    nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;


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


    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_file_pread(fd, buf, len, s->offset);
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
        r = nai_stream_file_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };
    if (r > 0) {
        /* seek file pointer */
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
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


static intptr_t nai_stream_nb_file_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t message;
    nai_fd_t fd;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    if (!nai_iofeat.preadv && (
        !nai_iofeat.readv || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1))) {

        message = nai_stream_is_message(s);
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_nb_file_read(s, v[0].buf, v[0].len);
        };
        goto _end;
    };

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


    fd = nai_evnode_get_fd(&s->ev);


    if (nai_iofeat.preadv) {
        while (1) {
            r = nai_file_preadv(fd, v, count, s->offset);
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
            r = nai_stream_file_wait(s, 0);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    } else {
        while (1) {
            r = nai_file_readv(fd, v, count);
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
            r = nai_stream_file_wait(s, 0);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    };
    if (r > 0) {
        /* seek file pointer */
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
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


static intptr_t nai_stream_nb_sock_read(
    nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;


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


    flags = 0;
#if defined(MSG_WAITALL)
    if (nai_stream_is_message(s)) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_recv(fd, buf, len, flags);
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


static intptr_t nai_stream_nb_sock_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_int_t message;
    nai_fd_t fd;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    if (!nai_iofeat.recvv) {
        message = nai_stream_is_message(s);
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_nb_sock_read(s, v[0].buf, v[0].len);
        };
        goto _end;
    };

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


    flags = 0;
    message = nai_stream_is_message(s);
#if defined(MSG_WAITALL)
    if (message) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        r = nai_sock_recvv(fd, v, count, flags);
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


static intptr_t nai_stream_nb_file_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t ec;
    nai_int_t message;
    size_t max;
    size_t read;
    nai_buflist_t full;
    nai_stream_nb_t* n;
    nai_stream_nb_read_f* fns;
    nai_stream_nb_read_f readfn;


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


    max = NAI_IO_READSIZE;
    message = nai_stream_is_message(s);
    if (message) {
        max = limit;
    };


    /* get read function */
    n = (nai_stream_nb_t*)s->st.ctx;
    fns = nai_stream_nb_readfn[n->iofn];
    readfn = fns[s->offset == (nai_off64_t)-1];


    nai_list_init(&full.ent);

    /* start read */
    while (!nai_buflist_is_empty(l)) {
        read = max;
        if (read > limit) {
            read = limit;
        };

        r = readfn(s, l, read, message);
        if (r < 0) {
            ec = (nai_int_t)-r;
            r = -1;
            goto _fail;
        };
        if (r == 0) {
            break;
        };


        total += r;
        nai_buflist_fulled(l, &full, r);


        if (message) {
            break;
        };

        limit -= r;
        if (limit <= 0) {
            break;
        };
    };

    nai_buflist_add_head(l, &full);
    r = total;


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            if (total > 0) {
                r = total;
            } else {
                nai_errno = NAI_EAGAIN;
                r = -1;
            };
        };
    };

    nai_buflist_add_head(l, &full);
    goto _end;
};


static intptr_t nai_stream_nb_sock_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t ec;
    nai_int_t message;
    size_t max;
    size_t read;
    nai_buflist_t full;
    nai_stream_nb_t* n;
    nai_stream_nb_read_f* fns;
    nai_stream_nb_read_f readfn;


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


    max = NAI_IO_READSIZE;
    message = nai_stream_is_message(s);
    if (message) {
        max = limit;
    };


    /* get read function */
    n = (nai_stream_nb_t*)s->st.ctx;
    fns = nai_stream_nb_readfn[n->iofn];
    readfn = fns[0];


    nai_list_init(&full.ent);

    /* start read */
    while (!nai_buflist_is_empty(l)) {
        read = max;
        if (read > limit) {
            read = limit;
        };

        r = readfn(s, l, read, message);
        if (r < 0) {
            ec = (nai_int_t)-r;
            r = -1;
            goto _fail;
        };
        if (r == 0) {
            break;
        };


        total += r;
        nai_buflist_fulled(l, &full, r);


        if (message) {
            break;
        };

        limit -= r;
        if (limit <= 0) {
            break;
        };
    };

    nai_buflist_add_head(l, &full);
    r = total;


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            if (total > 0) {
                r = total;
            } else {
                nai_errno = NAI_EAGAIN;
                r = -1;
            };
        };
    };

    nai_buflist_add_head(l, &full);
    goto _end;
};


static intptr_t nai_stream_nb_file_pread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_wbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    fd = nai_evnode_get_fd(&s->ev);
    while (1) {
        r = nai_file_pread(fd, v.buf, v.len, s->offset);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_preadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_wbufarray(
        l, &ba, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    fd = nai_evnode_get_fd(&s->ev);
    while (1) {
        r = nai_file_preadv(fd, ba.v, ba.count, s->offset);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_qread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_wbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    fd = nai_evnode_get_fd(&s->ev);
    while (1) {
        r = nai_file_read(fd, v.buf, v.len);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_qreadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_wbufarray(
        l, &ba, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    fd = nai_evnode_get_fd(&s->ev);
    while (1) {
        r = nai_file_readv(fd, ba.v, ba.count);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qrecv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_wbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    flags = 0;
#if defined(MSG_WAITALL)
    if (message) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);
    while (1) {
        r = nai_sock_recv(fd, v.buf, v.len, flags);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
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
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qrecvv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_wbufarray(
        l, &ba, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        goto _end;
    };

    flags = 0;
#if defined(MSG_WAITALL)
    if (message) {
        flags = MSG_WAITALL;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);
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
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
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
    r = -ec;
    goto _end;
};



//////////////////////////////////////////////////////////////////////////////
// write


#define nai_stream_nb_load_and_flush                    \
    nai_iobase_load_and_flush                           \


static intptr_t nai_stream_nb_file_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_stream_nb_t* n;


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


    /* check load stat */
    n = (nai_stream_nb_t*)s->st.ctx;
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


    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    while (1) {
        r = nai_file_pwrite(fd, buf, len, s->offset);
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
        r = nai_stream_file_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


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


static intptr_t nai_stream_nb_file_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t message;
    nai_fd_t fd;
    nai_stream_nb_t* n;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    if (!nai_iofeat.pwritev && (
        !nai_iofeat.writev || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1))) {

        message = nai_stream_is_message(s);
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_nb_file_write(s, v[0].buf, v[0].len);
        };
        goto _end;
    };

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


    /* check load stat */
    n = (nai_stream_nb_t*)s->st.ctx;
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


    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    if (nai_iofeat.pwritev) {
        while (1) {
            r = nai_file_pwritev(fd, v, count, s->offset);
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
            r = nai_stream_file_wait(s, 1);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    } else {
        while (1) {
            r = nai_file_writev(fd, v, count);
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
            r = nai_stream_file_wait(s, 1);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    };

    /* seek file pointer */
    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };


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


static intptr_t nai_stream_nb_sock_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_stream_nb_t* n;


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


    /* check load stat */
    n = (nai_stream_nb_t*)s->st.ctx;
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


    flags = 0;
#if defined(MSG_EOR)
    if (nai_stream_is_message(s)) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    while (1) {
        r = nai_sock_send(fd, buf, len, flags);
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
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


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


static intptr_t nai_stream_nb_sock_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_int_t message;
    nai_fd_t fd;
    nai_stream_nb_t* n;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    if (!nai_iofeat.sendv) {
        message = nai_stream_is_message(s);
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_nb_sock_write(s, v[0].buf, v[0].len);
        };
        goto _end;
    };

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


    /* check load stat */
    n = (nai_stream_nb_t*)s->st.ctx;
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


    flags = 0;
    message = nai_stream_is_message(s);
#if defined(MSG_EOR)
    if (message) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_evnode_get_fd(&s->ev);


    /* start write */
    while (1) {
        r = nai_sock_sendv(fd, v, count, flags);
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
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


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


static intptr_t nai_stream_nb_file_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t ec;
    nai_int_t message;
    size_t max;
    size_t write;
    nai_stream_nb_t* n;
    nai_stream_nb_send_f* fns;
    nai_stream_nb_send_f sendfn;


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


    n = (nai_stream_nb_t*)s->st.ctx;

    /* try load files */
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
        r = nai_stream_nb_load_and_flush(s, l, max, 
            nai_stream_nb_sock_writev, nai_stream_nb_is_writev[n->iofn]);
        goto _end;
    };


    /* get write function */
    fns = nai_stream_nb_sendfn[n->iofn];
    sendfn = fns[s->offset == (nai_off64_t)-1];


    /* start write */
    while (!nai_buflist_is_empty(l)) {
        write = max;
        if (write > limit) {
            write = limit;
        };

        r = sendfn(s, l, write, message);
        if (r < 0) {
            ec = (nai_int_t)-r;
            r = -1;
            goto _fail;
        };


        total += r;
        nai_buflist_rcommit(l, r);


        if (message) {
            break;
        };

        limit -= r;
        if (limit <= 0) {
            break;
        };
    };

    r = total;


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            if (total > 0) {
                r = total;
            } else {
                nai_errno = NAI_EAGAIN;
                r = -1;
            };
        };
    };
    goto _end;
};


static intptr_t nai_stream_nb_sock_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    intptr_t total = 0;
    nai_int_t ec;
    nai_int_t message;
    nai_int_t sendfile;
    size_t max;
    size_t write;
    nai_stream_nb_t* n;
    nai_stream_nb_send_f* fns;
    nai_stream_nb_send_f sendfn;


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


    n = (nai_stream_nb_t*)s->st.ctx;
    sendfile = nai_stream_nb_is_sendfile[n->iofn];

    /* try load files */
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
        /* if (sendfile) { */
        /*     break; */
        /* }; */

        /* fallthrough */

    default:
        r = nai_stream_nb_load_and_flush(s, l, max, 
            nai_stream_nb_sock_writev, nai_stream_nb_is_writev[n->iofn]);
        goto _end;
    };


    /* get write function */
    fns = nai_stream_nb_sendfn[n->iofn];
    sendfn = fns[0];


    /* start write */
    if (!sendfile) {
        while (!nai_buflist_is_empty(l)) {
            write = max;
            if (write > limit) {
                write = limit;
            };

            r = sendfn(s, l, write, message);
            if (r < 0) {
                ec = (nai_int_t)-r;
                r = -1;
                goto _fail;
            };


            total += r;
            nai_buflist_rcommit(l, r);


            if (message) {
                break;
            };

            limit -= r;
            if (limit <= 0) {
                break;
            };
        };
    } else {
        while (!nai_buflist_is_empty(l)) {
            write = max;
            if (write > limit) {
                write = limit;
            };


            r = sendfn(s, l, write, message);
            if (r < 0) {
                ec = (nai_int_t)-r;
                r = -1;
                goto _fail;
            };


            ec = nai_errno;

            total += r;
            nai_buflist_rcommit(l, r);

            if (ec == NAI_EAGAIN) {
                /* socket may be blocked when sendfile returns successfully, 
                 * stop sending if blocked */
                goto _fail;
            };


            if (message) {
                break;
            };

            limit -= r;
            if (limit <= 0) {
                break;
            };
        };
    };

    r = total;


_end:
    return r;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_stream_blocked(s, NAI_EV_WRITE);
        if (r >= 0) {
            if (total > 0) {
                r = total;
            } else {
                nai_errno = NAI_EAGAIN;
                r = -1;
            };
        };
    };
    goto _end;
};



static intptr_t nai_stream_nb_file_pwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_rbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_file_pwrite(fd, v.buf, v.len, s->offset);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_pwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    r = nai_buflist_to_rbufarray(
        l, &ba, b, nai_countof(b), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_file_pwritev(fd, ba.v, ba.count, s->offset);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
        s->offset += r;
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_qwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_rbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_file_write(fd, v.buf, v.len);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_file_qwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_rbufarray(
        l, &ba, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_file_writev(fd, ba.v, ba.count);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_file_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qsend(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufvec_t v;


    r = nai_buflist_to_rbufvec(l, &v, limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    flags = 0;
#if defined(MSG_EOR)
    if (message) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_sock_send(fd, v.buf, v.len, flags);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qsendv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_fd_t fd;
    nai_bufarray_t ba;
    nai_bufvec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_rbufarray(
        l, &ba, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_buflist_rcommit(l, 0);
        goto _end;
    };

    flags = 0;
#if defined(MSG_EOR)
    if (message) {
        flags = MSG_EOR;
    };
#endif

    fd = nai_stream_get_fd(s);
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
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qsendfile(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_filechunk_t fc;
    nai_bufvec_t v[NAI_BUFV_MAX];


#if defined(_WIN32)
    const nai_int_t max_ht = 1; /* TransmitFile can't accept bufvec array */
#else
    const nai_int_t max_ht = message ? 0 : NAI_BUFV_MAX;
#endif


    r = nai_buflist_to_filechunk(
        l, &fc, v, nai_countof(v), max_ht, limit, message);
    if (r < 0) {
        ec = nai_errno;
        if (message && ec == EOVERFLOW) {
            r = nai_stream_nb_sock_qsendv(s, l, limit, message);
            if (r >= 0) {
                nai_errno = 0;
            };
            goto _end;
        };
        goto _fail;
    };
    if (r == 0) {
        nai_errno = 0;
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_sendfile(fd, &fc, s->st.timeout[1]);
        if (r > 0) {
            break;
        };
        if (r == 0) {
            /* file eof */
            ec = ENODATA;
            goto _fail;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


static intptr_t nai_stream_nb_sock_qsendfilev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_filearray_t fa;
    nai_filevec_t v[NAI_BUFV_MAX];


    r = nai_buflist_to_filearray(
        l, &fa, v, nai_countof(v), limit, message);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };
    if (r == 0) {
        nai_errno = 0;
        goto _end;
    };

    fd = nai_stream_get_fd(s);
    while (1) {
        r = nai_sendfilev(fd, fa.v, fa.count, s->st.timeout[1]);
        if (r > 0) {
            break;
        };
        if (r == 0) {
            /* file eof */
            ec = ENODATA;
            goto _fail;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ec = ETIMEDOUT;
            goto _fail;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_stream_sock_wait(s, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

_end:
    return r;

_fail:
    r = -ec;
    goto _end;
};


