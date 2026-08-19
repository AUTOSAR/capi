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
/// @file       nai_stream_eio.c
/// @brief      
/// @details
/// @date       2022-05-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"
#include "nai/os/nai_tlocal.h"


//////////////////////////////////////////////////////////////////////////////
// eio stream


static nai_int_t nai_stream_eio_open(nai_stream_t* s, nai_evloop_t* l);
static nai_int_t nai_stream_eio_close(nai_stream_t* s);
static nai_int_t nai_stream_eio_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_stream_eio_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value);


static intptr_t nai_stream_eio_read(
    nai_stream_t* s, void* buf, size_t len);
static intptr_t nai_stream_eio_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_eio_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_eio_file_pread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_eio_file_preadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


static intptr_t nai_stream_eio_write(
    nai_stream_t* s, const void* buf, size_t len);
static intptr_t nai_stream_eio_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count);
static intptr_t nai_stream_eio_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit);
static intptr_t nai_stream_eio_file_pwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
static intptr_t nai_stream_eio_file_pwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);




#define NAI_EIO_FEAT (NAI_EV_FEAT_ASYNC|NAI_EV_FEAT_BLOCK)


nai_stream_ops_t nai_stream_file_eio = {
    "file-stream-eio", 
    NAI_EIO_FEAT|NAI_EV_FEAT_FILE,
    nai_stream_eio_open, 
    nai_stream_eio_setopt,
    nai_stream_eio_getopt, 
    nai_iobase_eio_shutdown, 
    nai_stream_eio_close, 
    nai_stream_no_bind, 
    nai_stream_no_connect, 
    nai_stream_eio_read, 
    nai_stream_eio_readv, 
    nai_stream_eio_readq, 
    nai_stream_eio_write, 
    nai_stream_eio_writev, 
    nai_stream_eio_writeq, 
    nai_stream_no_recv, 
    nai_stream_no_recvm, 
    nai_stream_no_send, 
    nai_stream_no_sendm, 
    nai_stream_no_sendmm
};


extern nai_stream_ops_t nai_stream_file_b;


#define nai_stream_nb_ops(s)                            \
    nai_stream_file_b                                   \


//////////////////////////////////////////////////////////////////////////////
// open, connect, close, setopt and getopt 


#define NAI_STREAM_EIO_DONE         NAI_IOBASE_EIO_DONE
#define NAI_STREAM_EIO_PENDING      NAI_IOBASE_EIO_PENDING
#define NAI_STREAM_EIO_COMPLETED    NAI_IOBASE_EIO_COMPLETED
#define NAI_STREAM_EIO_ERROR        NAI_IOBASE_EIO_ERROR


typedef struct nai_iobase_eio_s nai_stream_eio_t;
typedef intptr_t (*nai_stream_eio_read_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);
typedef intptr_t (*nai_stream_eio_send_f)(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message);


#define nai_stream_eio_init(s)                          \
    nai_iobase_eio_init(s)                              \


#define nai_stream_eio_add_evloop(s, l, a)              \
    nai_iobase_eio_add_evloop(s, l, a)                  \


#define nai_stream_eio_wait(a, w, t)                    \
    nai_iobase_eio_wait(a, w, t)                        \



static nai_int_t nai_stream_eio_open(nai_stream_t* s, nai_evloop_t* l)
{
    return nai_iobase_eio_open(s, l, 0);
};


static nai_int_t nai_stream_eio_close(nai_stream_t* s)
{
    return nai_iobase_eio_close(s);
};


static nai_int_t nai_stream_eio_getopt(
    nai_stream_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;

    r = nai_iobase_aio_getopt(s, opt, value);

    return r;
};


static nai_int_t nai_stream_eio_setopt(
    nai_stream_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;

    r = nai_iobase_aio_setopt(s, opt, value);

    return r;
};


static nai_int_t nai_stream_eio_poll(nai_iobase_eio_t* a, nai_int_t ev)
{
    nai_int_t r;
    nai_int_t ec;
    int64_t now;
    int64_t start;
    int64_t timeo;
    nai_eio_w_t* o;


    start = -1;
    o = ev == NAI_POLL_READ ? (nai_eio_w_t*)&a->readop : &a->sendop;
    for (;;) {
        if (nai_task_is_canceling(&o->op)) {
            r = ECANCELED;
            break;
        };

        now = nai_tick_to_msec();
        if (start == -1) {
            start = now;
        };

        if (o->timeout == (uint32_t)-1) {
            timeo = 1000;
        } else {
            timeo = now - (start + o->timeout);
            if (timeo < 0) {
                r = ETIMEDOUT;
                break;
            };
            if (timeo > 1000) {
                timeo = 1000;
            };
        };

        r = nai_file_poll(a->fd, ev, (uint32_t)timeo);
        if (r >= 0) {
            r = 0;
            break;
        };

        ec = nai_errno;
        if (ec != ETIMEDOUT || timeo == 0) {
            r = ec;
            break;
        };
    };

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// read


static nai_int_t nai_stream_eio_read_task(void* p)
{
    intptr_t r;
    nai_int_t ec;
    nai_eio_r_t* o;
    nai_stream_eio_t* a;
    nai_tlocal_t* t;


    a = (nai_stream_eio_t*)p;
    o = &a->readop;
    assert(o->count < (nai_int_t)nai_countof(o->ba));


    t = nai_tlocal_get();
    if (t == 0) {
        o->err = nai_errno;
        goto _end;
    };

    r = nai_tlocal_io_start(t, &o->op);
    if (r < 0) {
        o->err = nai_errno;
        goto _end;
    };

    for (;;) {
        if (nai_task_is_canceling(&o->op)) {
            o->err = ECANCELED;
            goto _end;
        };

        if (o->offset == (nai_off64_t)-1) {
            r = nai_file_readv(
                a->fd, o->ba, o->count);
        } else {
            r = nai_file_preadv(
                a->fd, o->ba, o->count, o->offset);
        };
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != EAGAIN) {
            o->err = ec;
            goto _end;
        };

        ec = nai_stream_eio_poll(a, NAI_POLL_READ);
        if (ec != 0) {
            o->err = ec;
            goto _end;
        };
    };

    o->err = 0;
    o->bytes = r;


_end:
    if (t != 0) {
        nai_tlocal_io_end(t);
    };

    nai_evloop_queue(a->loop, 
        (nai_task_f)nai_iobase_eio_read_complete, a);
    return 0;
};


static nai_int_t nai_stream_eio_read_start(
    nai_stream_t* s, nai_stream_eio_t* a)
{
    nai_int_t r;
    nai_eio_r_t* o;
    nai_task_pool_t* p;


    /* mark async io is pending */
    a->readstat = NAI_STREAM_EIO_PENDING;
    a->refs ++;

    p = nai_evloop_get_io_threads(a->loop);
    o = &a->readop;
    o->timeout = s->st.timeout[0];
    o->offset = nai_stream_is_seekable(s) ? s->offset : (nai_off64_t)-1;
    r = nai_task_pool_queue(p, 
        nai_stream_eio_read_task, a, 0, &o->op);

    if (r < 0) {
        a->readstat = NAI_STREAM_EIO_DONE;
        a->refs --;
    };


    return r;
};


static intptr_t nai_stream_eio_read_sult(
    nai_stream_t* s, nai_stream_eio_t* a, nai_buflist_t* l)
{
    intptr_t r;


    switch (a->readstat){
    case NAI_STREAM_EIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_stream_eio_wait(a, 0, s->st.timeout[0]);
        if (r < 0) {
            goto _end;
        };
        break;
    default:
        break;
    };

    switch (a->readstat){
    case NAI_STREAM_EIO_COMPLETED:
        a->readstat = NAI_STREAM_EIO_DONE;
        r = a->readop.bytes;
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
        };
        if (l != 0) {
            nai_buflist_wcommit(l, r);
        };
        break;

    case NAI_STREAM_EIO_ERROR:
        nai_errno = (nai_int_t)a->readop.err;
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


static intptr_t nai_stream_eio_read_wait(
    nai_stream_t* s, nai_stream_eio_t* a)
{
    intptr_t r;


    (void)a;

    assert(!s->st.blocking);

    /* set timer */
    r = nai_stream_blocked(s, NAI_EV_READ);
    if (r < 0) {
        /* do nothing */
        ;
    };

    nai_errno = EINPROGRESS;
    r = -1;

    return r;
};


static intptr_t nai_stream_eio_read(nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_stream_eio_t* a;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_read_sult(s, a, 0);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).read(s, buf, len);
        goto _end;
    };


    /* setup read */
    a->readop.count = 1;
    a->readop.ba[0].buf = buf;
    a->readop.ba[0].len = len;

    /* start read */
    r = nai_stream_eio_read_start(s, a);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_read_wait(s, a);
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


static intptr_t nai_stream_eio_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t message;
    nai_stream_eio_t* a;


    if (count <= 0) {
        r = 0;
        goto _end;
    };
    if (!nai_iofeat.preadv && (
        !nai_iofeat.readv || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1))) {

        a = (nai_stream_eio_t*)s->st.ctx;
        message = a->message;
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_eio_read(s, v[0].buf, v[0].len);
        };
        goto _end;
    };


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_read_sult(s, a, 0);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).readv(s, v, count);
        goto _end;
    };


    /* setup read */
    a->readop.count = count;
    if (a->readop.count > (nai_int_t)nai_countof(a->readop.ba)) {
        a->readop.count = nai_countof(a->readop.ba);
    };
    nai_memcpy(a->readop.ba, v, a->readop.count * sizeof(*v));

    /* start read */
    r = nai_stream_eio_read_start(s, a);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_read_wait(s, a);
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


static intptr_t nai_stream_eio_readq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t message;
    size_t max;
    size_t read;
    nai_stream_eio_t* a;
    nai_stream_eio_read_f readfn;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->readstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_read_sult(s, a, l);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).readq(s, l, limit);
        goto _end;
    };


    /* check subtype */
    message = a->message;
    max = NAI_IO_READSIZE;
    if (message) {
        max = limit;
    };

    read = max;
    if (read > limit) {
        read = limit;
    };

    if (nai_iofeat.preadv) {
        readfn = nai_stream_eio_file_preadv;
    } else if (!nai_iofeat.readv || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1)) {
        readfn = nai_stream_eio_file_pread;
    } else {
        readfn = nai_stream_eio_file_preadv;
    };


    /* start read */
    r = readfn(s, l, read, message);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_read_wait(s, a);
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


static intptr_t nai_stream_eio_file_pread(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufarray_t ba;
    nai_stream_eio_t* a = (nai_stream_eio_t*)s->st.ctx;


    if (message) {
        r = nai_buflist_to_wbufarray(
            l, &ba, a->readop.ba, 1, limit, message);
    } else {
        r = nai_buflist_to_wbufarray(
            l, &ba, a->readop.ba, nai_countof(a->readop.ba), limit, message);
    };
    if (r < 0) {
        goto _end;
    };


    /* setup read */
    a->readop.count = ba.count;

    /* start read */
    r = nai_stream_eio_read_start(s, a);


_end:
    return r;
};


static intptr_t nai_stream_eio_file_preadv(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufarray_t ba;
    nai_stream_eio_t* a = (nai_stream_eio_t*)s->st.ctx;


    r = nai_buflist_to_wbufarray(
        l, &ba, a->readop.ba, nai_countof(a->readop.ba), limit, message);
    if (r < 0) {
        goto _end;
    };


    /* setup read */
    a->readop.count = ba.count;

    /* start read */
    r = nai_stream_eio_read_start(s, a);


_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// write


static nai_int_t nai_stream_eio_write_bufs(void* p)
{
    intptr_t r;
    nai_int_t ec;
    nai_eio_w_t* o;
    nai_stream_eio_t* a;
    nai_tlocal_t* t;


    a = (nai_stream_eio_t*)p;
    o = &a->sendop;
    assert(o->count < (nai_int_t)nai_countof(o->ba));


    t = nai_tlocal_get();
    if (t == 0) {
        o->err = nai_errno;
        goto _end;
    };

    r = nai_tlocal_io_start(t, &o->op);
    if (r < 0) {
        o->err = nai_errno;
        goto _end;
    };

    for (;;) {
        if (nai_task_is_canceling(&o->op)) {
            o->err = ECANCELED;
            goto _end;
        };

        if (o->offset == (nai_off64_t)-1) {
            r = nai_file_writev(
                a->fd, o->ba, o->count);
        } else {
            r = nai_file_pwritev(
                a->fd, o->ba, o->count, o->offset);
        };
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec != EAGAIN) {
            o->err = ec;
            goto _end;
        };

        ec = nai_stream_eio_poll(a, NAI_POLL_WRITE);
        if (ec != 0) {
            o->err = ec;
            goto _end;
        };
    };

    o->err = 0;
    o->bytes = r;


_end:
    if (t != 0) {
        nai_tlocal_io_end(t);
    };

    nai_evloop_queue(a->loop, 
        (nai_task_f)nai_iobase_eio_send_complete, a);
    return 0;
};


static nai_int_t nai_stream_eio_write_copy(void* p)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t count;
    nai_int_t message;
    nai_int_t directio;
    uint8_t* buf;
    size_t bsize;
    size_t read;
    size_t left;
    size_t total;
    size_t align;
    size_t sector_size;
    size_t offr;
    size_t size;
    nai_off64_t offs;
#if defined(_WIN32)
    nai_aio_t aio;
#endif
    nai_eio_w_t* o;
    nai_stream_eio_t* a;
    nai_tlocal_t* t;
    nai_filevec_t* f;
    nai_bufvec_t* v;
    nai_bufvec_t ba[NAI_BUFV_MAX];
    uint8_t bufs[64*1024];


    a = (nai_stream_eio_t*)p;
    o = &a->sendop;
    assert(o->count < (nai_int_t)nai_countof(ba));
    assert(o->count < (nai_int_t)nai_countof(o->fa));


    t = nai_tlocal_get();
    if (t == 0) {
        o->err = nai_errno;
        goto _end;
    };

    r = nai_tlocal_io_start(t, &o->op);
    if (r < 0) {
        o->err = nai_errno;
        goto _end;
    };

    message = a->message;
    sector_size = nai_sector_size;

    n = 0;
    total = 0;
    for ( ; n < o->count; ) {

        buf = bufs;
        bsize = sizeof(bufs);
        left = 0;
        count = 0;

        /* read loop */
        for ( ; n < o->count; n ++) {
            f = o->fa + n;
            if (f->fd == NAI_FD_INVALID) {
                v = ba + count;
                v->buf = f->ptr;
                v->len = f->size;
                left += f->size;
                count ++;
                continue;
            };

            offs = f->off;
            size = f->size;
            directio = f->oflags & NAI_O_DIRECT;


            if (directio) {
                /* align offset */
                offr = (size_t)(offs - nai_aligndown(offs, sector_size));
                offs -= offr;

                /* count required memory size */
                size += offr;
                size = (size_t)(nai_align(
                    offs + size, sector_size) - offs);

                align = (size_t)buf & (sector_size-1);
                if (align) {
                    align = sector_size - align;
                    buf += align;
                    bsize -= align;
                };
            } else {
                offr = 0;
            };

            if ((ssize_t)bsize < (ssize_t)size) {
                if (message) {
                    /* too large */
                    ec = EOVERFLOW;
                    goto _fail;
                };

                size = bsize;
                if (directio) {
                    size = nai_aligndown(size, sector_size);
                };
                if ((ssize_t)size <= 0) {
                    /* no buffer */
                    break;
                };
            };

            /* read file */
            read = 0;
            for (;;) {
                if (nai_task_is_canceling(&o->op)) {
                    ec = ECANCELED;
                    goto _fail;
                };

#if defined(_WIN32)
                if (f->oflags & NAI_O_ASYNCIO) {
                    nai_aio_init(&aio);
                    nai_aio_set_waitable(&aio, 1);
                    r = nai_aio_read(&aio, 
                        f->fd, buf + read, size - read, offs + read);
                    if (r < 0) {
                        ec = nai_errno;
                        if (ec != EINPROGRESS) {
                            goto _fail;
                        };
                    };
                    r = nai_aio_result(&aio, 1);
                } else {
#endif
                    r = nai_file_pread(
                        f->fd, buf + read, size - read, offs + read);
#if defined(_WIN32)
                };
#endif
                if (r < 0) {
                    ec = nai_errno;
                    goto _fail;
                };
                if (r == 0) {
                    /* file eof, fill zero */
                    r = size - read;
                    nai_memset(buf + read, 0, r);
                };

                read += r;
                if (read >= size) {
                    break;
                };
            };

            buf += offr;
            size -= offr;

            v = ba + count;
            v->buf = buf;
            v->len = size;
            left += size;
            count ++;

            if (size < f->size) {
                f->off += size;
                f->size -= size;
                break;
            };

            buf += size;
            bsize -= offr + size;
        };


        /* write loop */
        v = ba;
        for (;;) {
            for (;;) {
                if (nai_task_is_canceling(&o->op)) {
                    ec = ECANCELED;
                    goto _fail;
                };

                if (o->offset == (nai_off64_t)-1) {
                    r = nai_file_writev(a->fd, v, count);
                } else {
                    r = nai_file_pwritev(a->fd, v, count, o->offset);
                };
                if (r >= 0) {
                    break;
                };

                ec = nai_errno;
                if (ec != EAGAIN) {
                    goto _fail;
                };

                ec = nai_stream_eio_poll(a, NAI_POLL_WRITE);
                if (ec != 0) {
                    goto _fail;
                };
            };

            total += r;
            if (o->offset != (nai_off64_t)-1) {
                o->offset += r;
            };
            if (r >= (intptr_t)left) {
                break;
            };

            left -= r;
            for ( ; count > 0; count --) {
                if (r < (intptr_t)v->len) {
                    v->len -= r;
                    v->buf += r;
                    break;
                };

                r -= v->len;
                v ++;
            };
        };
    };

    o->err = 0;
    o->bytes = total;


_end:
    if (t != 0) {
        nai_tlocal_io_end(t);
    };

    nai_evloop_queue(a->loop, 
        (nai_task_f)nai_iobase_eio_send_complete, a);
    return 0;

_fail:
    if ((ec != ETIMEDOUT && ec != ECANCELED) || total <= 0) {
        o->err = ec;
    } else {
        o->err = 0;
        o->bytes = total;
    };
    goto _end;
};


static nai_int_t nai_stream_eio_write_file(void* p)
{

#if !(NAI_HAVE_SENDFILE_LINUX)

    nai_stream_eio_write_copy(p);
    return 0;

#else

    intptr_t r;
    nai_int_t ec;
    nai_int_t count;
    nai_int_t message;
    size_t total;
    nai_eio_w_t* o;
    nai_stream_eio_t* a;
    nai_tlocal_t* t;
    nai_filevec_t* v;


    a = (nai_stream_eio_t*)p;
    o = &a->sendop;
    assert(o->count < (nai_int_t)nai_countof(o->fa));

    message = a->message;
    if ((message && o->count > 1) || o->offset != (uint32_t)-1) {
        r = nai_stream_eio_write_copy(p);
        goto _ret;
    };


    t = nai_tlocal_get();
    if (t == 0) {
        o->err = nai_errno;
        goto _end;
    };

    r = nai_tlocal_io_start(t, &o->op);
    if (r < 0) {
        o->err = nai_errno;
        goto _end;
    };

    v = o->fa;
    count = o->count;
    total = 0;
    for ( ; count > 0; ) {
        for (;;) {
            if (nai_task_is_canceling(&o->op)) {
                ec = ECANCELED;
                goto _fail;
            };

            r = nai_sendfilev(a->fd, v, count, 0);
            if (r >= 0) {
                break;
            };

            ec = nai_errno;
            if (ec != EAGAIN) {
                goto _fail;
            };

            ec = nai_stream_eio_poll(a, NAI_POLL_WRITE);
            if (ec != 0) {
                goto _fail;
            };
        };

        total += r;
        for ( ; count > 0; count --) {
            if (r < (intptr_t)v->size) {
                v->off += r;
                v->size -= r;
                break;
            };

            r -= v->size;
            v ++;
        };
    };

    o->err = 0;
    o->bytes = total;


_end:
    if (t != 0) {
        nai_tlocal_io_end(t);
    };

    nai_evloop_queue(a->loop, 
        (nai_task_f)nai_iobase_eio_send_complete, a);

_ret:
    return 0;

_fail:
    if ((ec != ETIMEDOUT && ec != ECANCELED) || total <= 0) {
        o->err = ec;
    } else {
        o->err = 0;
        o->bytes = total;
    };
    goto _end;

#endif
};


static nai_int_t nai_stream_eio_write_start(
    nai_stream_t* s, nai_stream_eio_t* a, nai_int_t farray)
{
    nai_int_t r;
    nai_eio_w_t* o;
    nai_task_pool_t* p;


    /* mark async io is pending */
    a->sendstat = NAI_STREAM_EIO_PENDING;
    a->refs ++;


    p = nai_evloop_get_io_threads(a->loop);
    o = &a->sendop;
    o->timeout = s->st.timeout[1];
    o->offset = nai_stream_is_seekable(s) ? s->offset : (nai_off64_t)-1;
    if (farray) {
        r = nai_task_pool_queue(p, 
            nai_stream_eio_write_file, a, 0, &o->op);
    } else {
        r = nai_task_pool_queue(p, 
            nai_stream_eio_write_bufs, a, 0, &o->op);
    };

    if (r < 0) {
        a->sendstat = NAI_STREAM_EIO_DONE;
        a->refs --;
    };


    return r;
};


static intptr_t nai_stream_eio_write_sult(
    nai_stream_t* s, nai_stream_eio_t* a, nai_buflist_t* l)
{
    intptr_t r;


    switch (a->sendstat){
    case NAI_STREAM_EIO_PENDING:
        if (!s->st.blocking) {
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        };
        r = nai_stream_eio_wait(a, 1, s->st.timeout[1]);
        if (r < 0) {
            goto _end;
        };
        break;
    default:
        break;
    };

    switch (a->sendstat){
    case NAI_STREAM_EIO_COMPLETED:
        a->sendstat = NAI_STREAM_EIO_DONE;
        r = a->sendop.bytes;
        if (nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1) {
            s->offset += r;
        };
        if (l != 0) {
            nai_buflist_rcommit(l, r);
        };
        break;

    case NAI_STREAM_EIO_ERROR:
        nai_errno = (nai_int_t)a->sendop.err;
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


static intptr_t nai_stream_eio_write_wait(
    nai_stream_t* s, nai_stream_eio_t* a)
{
    intptr_t r;


    (void)a;

    assert(!s->st.blocking);

    /* set timer */
    r = nai_stream_blocked(s, NAI_EV_WRITE);
    if (r < 0) {
        /* do nothing */
    };

    nai_errno = EINPROGRESS;
    r = -1;

    return r;
};


static intptr_t nai_stream_eio_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_stream_eio_t* a;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_write_sult(s, a, 0);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).write(s, buf, len);
        goto _end;
    };


    /* setup write */
    a->sendop.count = 1;
    a->sendop.ba[0].buf = (void*)buf;
    a->sendop.ba[0].len = len;

    /* start write */
    r = nai_stream_eio_write_start(s, a, 0);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_write_wait(s, a);
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


static intptr_t nai_stream_eio_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_int_t message;
    nai_stream_eio_t* a;


    if (count <= 0) {
        r = 0;
        goto _end;
    };
    if (!nai_iofeat.pwritev && (
        !nai_iofeat.writev || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1))) {

        a = (nai_stream_eio_t*)s->st.ctx;
        message = a->message;
        if (message && count > 1) {
            nai_errno = EOVERFLOW;
            r = -1;
        } else {
            r = nai_stream_eio_write(s, v[0].buf, v[0].len);
        };
        goto _end;
    };


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_write_sult(s, a, 0);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).writev(s, v, count);
        goto _end;
    };


    /* setup write */
    a->sendop.count = count;
    if (a->sendop.count > (nai_int_t)nai_countof(a->sendop.ba)) {
        a->sendop.count = nai_countof(a->sendop.ba);
    };
    nai_memcpy(a->sendop.ba, v, a->sendop.count * sizeof(*v));

    /* start write */
    r = nai_stream_eio_write_start(s, a, 0);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_write_wait(s, a);
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


static intptr_t nai_stream_eio_writeq(
    nai_stream_t* s, nai_buflist_t* l, size_t limit)
{
    intptr_t r;
    nai_int_t message;
    size_t max;
    size_t send;
    nai_stream_eio_t* a;
    nai_stream_eio_send_f sendfn;


    if (!(s->st.mode & NAI_EV_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    a = (nai_stream_eio_t*)s->st.ctx;
    if (a->sendstat != NAI_STREAM_EIO_DONE) {
        r = nai_stream_eio_write_sult(s, a, l);
        goto _end;
    };

    if (s->st.blocking) {
        r = nai_stream_nb_ops(s).writeq(s, l, limit);
        goto _end;
    };


    /* check subtype */
    message = a->message;
    max = NAI_IO_WRITESIZE;
    if (message) {
        max = limit;
    };

    send = max;
    if (send > limit) {
        send = limit;
    };

    if (nai_iofeat.pwritev) {
        sendfn = nai_stream_eio_file_pwritev;
    } else if (!nai_iofeat.writev || (
        nai_stream_is_seekable(s) && s->offset != (nai_off64_t)-1)) {
        sendfn = nai_stream_eio_file_pwrite;
    } else {
        sendfn = nai_stream_eio_file_pwritev;
    };


    /* start write */
    r = sendfn(s, l, send, message);
    if (r < 0) {
        goto _end;
    };


    /* waiting */
    r = nai_stream_eio_write_wait(s, a);
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


static intptr_t nai_stream_eio_file_pwrite(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_filearray_t fa;
    nai_stream_eio_t* a = (nai_stream_eio_t*)s->st.ctx;


    if (message) {
        r = nai_buflist_to_filearray(
            l, &fa, a->sendop.fa, 1, limit, message);
    } else {
        r = nai_buflist_to_filearray(
            l, &fa, a->sendop.fa, nai_countof(a->sendop.ba), limit, message);
    };
    if (r < 0) {
        goto _end;
    };


    /* setup write */
    a->sendop.count = fa.count;

    /* start write */
    r = nai_stream_eio_write_start(s, a, 1);


_end:
    return r;
};


static intptr_t nai_stream_eio_file_pwritev(
    nai_stream_t* s, nai_buflist_t* l, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_filearray_t fa;
    nai_stream_eio_t* a = (nai_stream_eio_t*)s->st.ctx;


    r = nai_buflist_to_filearray(
        l, &fa, a->sendop.fa, nai_countof(a->sendop.fa), limit, message);
    if (r < 0) {
        goto _end;
    };


    /* setup write */
    a->sendop.count = fa.count;

    /* start write */
    r = nai_stream_eio_write_start(s, a, 1);


_end:
    return r;
};


