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
/// @file       nai_async_load.c
/// @brief      
/// @details
/// @date       2022-10-25
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_async_load.h"
#include "nai/io/nai_iobase.h"
#include "nai/os/nai_tlocal.h"



//////////////////////////////////////////////////////////////////////////////
// common


static nai_int_t nai_async_load_set(nai_async_load_t* l, nai_int_t m)
{
    nai_int_t r;


    if (m == l->meth) {
        r = 0;
    } else if (m == NAI_ASYNC_LOAD_AIO) {
        l->meth = NAI_ASYNC_LOAD_AIO;
        nai_aio_init(&l->aio);
        nai_aio_set_in_loop(&l->aio, 1);
        nai_aio_set_port(&l->aio, nai_evloop_get_aio_port(l->loop));
        r = 0;
    } else {
        l->meth = NAI_ASYNC_LOAD_EIO;
        r = 0;
    };

    return r;
};


static nai_int_t nai_async_load_free(nai_async_load_t* l)
{
    nai_int_t r;


    if (l->buf) {
        nai_free(l->buf);
    };

    if (l->poolmem) {
        nai_evloop_ultra_free(l->loop, l, l->syncmem);
    } else {
        nai_free(l);
    };
    r = 0;

    return r;
};


static nai_int_t nai_async_load_call(nai_async_load_t* l, nai_int_t events)
{
    nai_int_t r;
    nai_async_load_cb_f cb;


    cb = l->cb;
    if (cb == 0) {
        /* already detached */
        nai_async_load_free(l);
        r = 0;
        goto _end;
    };

    l->stat = NAI_ASYNC_LOAD_COMPLETED;
    r = cb(l, events);

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// common read


#ifndef NAI_BUF_READSIZE
#define NAI_BUF_READSIZE        (4096*2)
#endif


#ifndef NAI_ASYNC_LOAD_LIMIT
#define NAI_ASYNC_LOAD_LIMIT    (4096)
#endif


typedef intptr_t (*nai_async_load_read_f)(
    nai_async_load_t* l, 
    nai_filevec_t* f, void* buf, size_t size, nai_off64_t off);


static nai_int_t nai_async_load_push(
    nai_async_load_t* l, nai_bufvec_t* b)
{
    nai_int_t r;
    nai_int_t n;
    nai_bufvec_t* ba;


    n = l->out;
    ba = (nai_bufvec_t*)l->buf;
    if (n <= 0 || ba[n-1].buf + ba[n-1].len != b->buf) {
        ba[n] = *b;
        l->out ++;
    } else {
        ba[n-1].len += b->len;
    };

    r = 0;

    return r;
};


static nai_int_t nai_async_load_read_loop(
    nai_async_load_t* l, nai_async_load_read_f readfn)
{
    intptr_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t directio;
    size_t sector_size;
    size_t align;
    size_t bsize;
    size_t size;
    size_t offr;
    nai_off64_t offs;
    uint8_t* buf;
    nai_bufvec_t b;
    nai_filevec_t* fa;


    sector_size = nai_sector_size;

    buf = l->buf + l->used;
    bsize = l->total - l->used;
    n = l->next;
    count = l->count;
    fa = (nai_filevec_t*)l->buf;
    for ( ; n < count; n ++) {
        if (fa[n].fd == NAI_FD_INVALID) {
            b.buf = fa[n].ptr;
            b.len = fa[n].size;
            nai_async_load_push(l, &b);
            continue;
        };

        if (l->canceling) {
            nai_errno = ECANCELED;
            r = -1;
            goto _end;
        };
        if (l->discarded) {
            break;
        };

        offs = fa[n].off;
        size = fa[n].size;
        directio = fa[n].oflags & NAI_O_DIRECT;

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

        (void)bsize;
        assert((ssize_t)bsize >= (ssize_t)size);

        r = readfn(l, &fa[n], buf, size, offs);
        if (r < 0) {
            if (l->discarded && nai_errno == ECANCELED) {
                break;
            };

            l->next = n;
            l->used = buf - l->buf;
            goto _end;
        };

        if (r != (intptr_t)size) {
            if (l->discarded || l->canceling) {
                break;
            };

            /* file eof ? */
            nai_errno = ENODATA;
            r = -1;
            goto _end;
        };

        b.buf = buf + offr;
        b.len = fa[n].size;
        nai_async_load_push(l, &b);

        buf += size;
        bsize -= size;
    };

    l->used = buf - l->buf;
    l->next = 0;
    r = 0;

_end:
    return (nai_int_t)r;
};



//////////////////////////////////////////////////////////////////////////////
// eio read


static nai_int_t nai_async_load_eio_read_finish(void* e)
{
    nai_async_load_t* l;


    l = (nai_async_load_t*)e;
    nai_async_load_call(l, NAI_IO_WRITE);
    return 0;
};


static intptr_t nai_async_load_eio_read(
    nai_async_load_t* l, 
    nai_filevec_t* v, void* buf, size_t size, nai_off64_t off)
{
    intptr_t r;
    size_t read;
    nai_fd_t fd;

#if defined(_WIN32)
    nai_int_t ec;
    nai_aio_t aio;
#endif


    (void)l;

    /* read file */
    read = 0;
    fd = v->fd;
    for (;;) {
#if defined(_WIN32)
        if (v->oflags & NAI_O_ASYNCIO) {
            nai_aio_init(&aio);
            nai_aio_set_waitable(&aio, 1);
            r = nai_aio_read(&aio, 
                fd, (uint8_t*)buf + read, size - read, off + read);
            if (r < 0) {
                ec = nai_errno;
                if (ec != EINPROGRESS) {
                    goto _end;
                };
            };
            r = nai_aio_result(&aio, 1);
        } else {
#endif
            r = nai_file_pread(
                fd, (uint8_t*)buf + read, size - read, off + read);
#if defined(_WIN32)
        };
#endif
        if (r < 0) {
            goto _end;
        };
        if (r == 0) {
            /* file eof, fill zero */
            r = size - read;
            nai_memset((uint8_t*)buf + read, 0, r);
        };

        read += r;
        if (read >= size) {
            break;
        };

        /* test */
        if (l->canceling) {
            nai_errno = ECANCELED;
            r = -1;
            goto _end;
        };
    };

    r = read;

_end:
    return r;
};


static nai_int_t nai_async_load_eio_load(void* e)
{
    nai_int_t r;
    nai_tlocal_t* t;
    nai_async_load_t* l;


    l = (nai_async_load_t*)e;

    do {
        t = nai_tlocal_get();
        if (t == 0) {
            l->error = nai_errno;
            break;
        };

        r = nai_tlocal_io_start(t, &l->op);
        if (r < 0) {
            l->error = nai_errno;
            break;
        };

        r = nai_async_load_read_loop(l, nai_async_load_eio_read);
        if (r < 0) {
            l->error = nai_errno;
        };

        nai_tlocal_io_end(t);

    } while (0);

    r = nai_evloop_queue(l->loop, nai_async_load_eio_read_finish, l);

    return r;
};


static nai_int_t nai_async_load_eio_start_load(nai_async_load_t* l)
{
    nai_int_t r;
    nai_task_pool_t* t;


    t = nai_evloop_get_io_threads(l->loop);
    r = nai_task_pool_queue(t, nai_async_load_eio_load, l, 0, &l->op);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        r = -1;
    };

    return r;
};


static nai_int_t nai_async_load_bio_start_load(nai_async_load_t* l)
{
    nai_int_t r;


    r = nai_async_load_read_loop(l, nai_async_load_eio_read);
    if (r < 0) {
        l->error = nai_errno;
    };

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// aio read


static intptr_t nai_async_load_aio_read(
    nai_async_load_t* l, nai_filevec_t* v, void* buf, size_t size, 
    nai_off64_t off)
{
    intptr_t r;
    nai_fd_t fd;


    fd = v->fd;
    r = nai_aio_read(&l->aio, fd, buf, size, off);

    return r;
};


static nai_int_t nai_async_load_aio_read_finish(
    nai_aio_t* readop, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t directio;
    size_t sector_size;
    size_t size;
    size_t offr;
    nai_off64_t offs;
    uint8_t* buf;
    nai_async_load_t* l;
    nai_bufvec_t b;
    nai_filevec_t* fa;


    l = nai_containof(readop, nai_async_load_t, aio);
    if (err) {
        l->error = err;
        goto _end;
    };


    sector_size = nai_sector_size;

    buf = l->buf + l->used;
    fa = (nai_filevec_t*)l->buf;
    n = l->next;
    offs = fa[n].off;
    size = fa[n].size;
    directio = fa[n].oflags & NAI_O_DIRECT;

    if (directio) {
        /* align offset */
        offr = (size_t)(offs - nai_aligndown(offs, sector_size));
        offs -= offr;

        /* count required memory size */
        size += offr;
        size = (size_t)(nai_align(
            offs + size, sector_size) - offs);

    } else {
        offr = 0;
    };

    if (bytes != size) {
        /* file eof ? */
        l->error = ENODATA;
        goto _end;
    };

    b.buf = buf + offr;
    b.len = fa[n].size;
    nai_async_load_push(l, &b);

    n ++;
    l->next = n;
    l->used += bytes;


    r = nai_async_load_read_loop(l, nai_async_load_aio_read);
    if (r < 0) {
        ec = nai_errno;
        if (ec == EINPROGRESS) {
            goto _wait;
        };

        l->error = ec;
    };


_end:
    nai_async_load_call(l, NAI_IO_WRITE);

_wait:
    return 0;
};


static nai_int_t nai_async_load_aio_start_load(nai_async_load_t* l)
{
    nai_int_t r;


    nai_aio_set_cb(&l->aio, nai_async_load_aio_read_finish);
    r = nai_async_load_read_loop(l, nai_async_load_aio_read);

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// common send


typedef struct nai_async_send_s nai_async_send_t;


struct nai_async_send_s {
    uint32_t chunk;
    union {
        struct {
            nai_filechunk_t c;
            nai_bufvec_t va[NAI_BUFV_MAX];
        };
        struct {
            nai_filearray_t a;
            nai_filevec_t fa[NAI_BUFV_MAX];
        };
    };
};



static nai_int_t nai_async_load_poll(nai_async_load_t* l)
{
    nai_int_t r;
    nai_int_t ec;
    int64_t now;
    int64_t start;
    int64_t timeo;


    start = -1;
    for (;;) {
        if (l->canceling) {
            nai_errno = ECANCELED;
            r = -1;
            break;
        };
        if (l->discarded) {
            r = 0;
            break;
        };

        now = nai_tick_to_msec();
        if (start == -1) {
            start = now;
        };

        if (l->timeout == (uint32_t)-1) {
            timeo = 1000;
        } else {
            timeo = now - (start + l->timeout);
            if (timeo < 0) {
                nai_errno = ETIMEDOUT;
                r = -1;
                break;
            };
            if (timeo > 1000) {
                timeo = 1000;
            };
        };

        r = nai_file_poll(l->fd, NAI_POLL_WRITE, (uint32_t)timeo);
        if (r >= 0) {
            r = 0;
            break;
        };

        ec = nai_errno;
        if (ec != ETIMEDOUT || timeo == 0) {
            r = -1;
            break;
        };
    };

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// eio send


static nai_int_t nai_async_load_send_loop(
    nai_async_load_t* l, nai_int_t blocking)
{
    intptr_t r;
    nai_int_t ec;
    nai_async_send_t* s;


    s = (nai_async_send_t*)l->buf;
    for (;;) {
        if (l->canceling) {
            nai_errno = ECANCELED;
            r = -1;
            break;
        };
        if (l->discarded) {
            r = 0;
            break;
        };

        if (s->chunk) {
            r = nai_sendfile(l->fd, &s->c, l->timeout);
        } else {
            r = nai_sendfilev(l->fd, s->a.v, s->a.count, l->timeout);
        };
        if (r >= 0) {
            l->sent = r;
            r = 0;
            break;
        };

        ec = nai_errno;
        if (ec == EAGAIN && blocking) {
            r = nai_async_load_poll(l);
            if (r >= 0) {
                continue;
            };
        };

        break;
    };

    return (nai_int_t)r;
};


static nai_int_t nai_async_load_eio_send_finish(void* e)
{
    nai_async_load_t* l;


    l = (nai_async_load_t*)e;
    nai_async_load_call(l, NAI_IO_WRITE);
    return 0;
};


static nai_int_t nai_async_load_eio_send(void* e)
{
    nai_int_t r;
    nai_int_t ec;
    nai_tlocal_t* t;
    nai_async_load_t* l;


    l = (nai_async_load_t*)e;

    do {
        t = nai_tlocal_get();
        if (t == 0) {
            l->error = nai_errno;
            break;
        };

        r = nai_tlocal_io_start(t, &l->op);
        if (r < 0) {
            l->error = nai_errno;
            break;
        };

        r = nai_async_load_send_loop(l, 1);
        if (r < 0) {
            ec = nai_errno;
            if (l->discarded && ec == ECANCELED) {
                ec = 0;
            };

            l->error = ec;
        };

        nai_tlocal_io_end(t);

    } while (0);

    r = nai_evloop_queue(l->loop, nai_async_load_eio_send_finish, l);

    return r;
};


static nai_int_t nai_async_load_eio_start_send(nai_async_load_t* l)
{
    nai_int_t r;
    nai_task_pool_t* t;


    t = nai_evloop_get_io_threads(l->loop);
    r = nai_task_pool_queue(t, nai_async_load_eio_send, l, 0, &l->op);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        r = -1;
    };

    return r;
};


static nai_int_t nai_async_load_bio_start_send(
    nai_async_load_t* l, nai_int_t blocking)
{
    nai_int_t r;


    r = nai_async_load_send_loop(l, blocking);

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// aio send


static nai_int_t nai_async_load_aio_send_finish(
    nai_aio_t* readop, nai_int_t err, size_t bytes)
{
    nai_async_load_t* l;


    l = nai_containof(readop, nai_async_load_t, aio);
    if (err) {
        l->error = err;
        goto _end;
    };


    l->sent = bytes;

_end:
    nai_async_load_call(l, NAI_IO_WRITE);
    return 0;
};


static nai_int_t nai_async_load_aio_start_send(nai_async_load_t* l)
{
    intptr_t r;
    nai_async_send_t* s;


    nai_aio_set_cb(&l->aio, nai_async_load_aio_send_finish);

    s = (nai_async_send_t*)l->buf;
    if (s->chunk) {
        r = nai_aio_sendfile(&l->aio, l->fd, &s->c, 0);
    } else {
        r = nai_aio_sendfilev(&l->aio, l->fd, s->a.v, s->a.count, 0);
    };
    if (r >= 0) {
        l->sent = r;
        r = 0;
    };

    return (nai_int_t)r;
};



//////////////////////////////////////////////////////////////////////////////
// send file


static intptr_t nai_async_load_sendable(
    nai_async_load_t* l, nai_int_t headers, nai_int_t message)
{
    intptr_t r;


#if defined(_WIN32)
    const nai_int_t max_ht = 1; /* TransmitFile can't accept bufvec array */
#else
    const nai_int_t max_ht = 0;
#endif


    do {
        if (l->aioport) {
            if (nai_aiofeat.sendfilev) {
                r = NAI_ASYNC_LOAD_AIO | NAI_ASYNC_LOAD_FARRAY;
                break;
            };
            if (nai_aiofeat.sendfile && headers <= max_ht) {
                r = NAI_ASYNC_LOAD_AIO | NAI_ASYNC_LOAD_FCHUNK;
                break;
            };

            if (!l->sendfile) {
                r = NAI_INTPTR_T_MAX;
                break;
            };
        };

#if defined(_WIN32)
        (void)message;
#else
        if (l->iothread || (
            l->loop && nai_evloop_ref_io_threads(l->loop) >= 0)) {
            l->iothread = 1;
            if (nai_iofeat.sendfilev) {
                r = NAI_ASYNC_LOAD_EIO | NAI_ASYNC_LOAD_FARRAY;
                break;
            };
            if (nai_iofeat.sendfile && headers <= max_ht) {
                r = NAI_ASYNC_LOAD_EIO | (
                    message ? NAI_ASYNC_LOAD_FCHUNK : NAI_ASYNC_LOAD_FARRAY);
                break;
            };
        };
#endif

        r = NAI_INTPTR_T_MAX;

    } while (0);

    return r;
};


static intptr_t nai_async_load_start_send(
    nai_async_load_t* l, 
    nai_buflist_t* list, size_t bytes, nai_int_t message, 
    nai_async_load_sendv_f sendfn, void* p, nai_int_t vmax, 
    nai_int_t blocking)
{
    intptr_t r;
    intptr_t n;
    nai_int_t ec;
    nai_int_t max;
    nai_int_t method;
    nai_int_t direct;
    nai_async_send_t* s;
    uint8_t* buf;


#if defined(_WIN32)
    const nai_int_t max_ht = 1; /* TransmitFile can't accept bufvec array */
#else
    const nai_int_t max_ht = 0;
#endif


    n = nai_async_load_sendable(l, 0, message);
    if (n == NAI_INTPTR_T_MAX) {
        r = n;
        goto _end;
    };

    max = NAI_BUFV_MAX;

    buf = l->buf;
    if (buf == 0 || l->total < sizeof(*s)) {
        buf = (uint8_t*)nai_malloc(sizeof(*s));
        if (buf == 0) {
            r = -1;
            goto _end;
        };

        if (l->buf) {
            nai_free(l->buf);
        };

        l->buf = buf;
        l->total = sizeof(*s);
    };

    s = (nai_async_send_t*)buf;
    switch (n) {
    case NAI_ASYNC_LOAD_AIO | NAI_ASYNC_LOAD_FARRAY:
    case NAI_ASYNC_LOAD_EIO | NAI_ASYNC_LOAD_FARRAY:
        /* sendfilev */
        s->chunk = 0;
        r = nai_buflist_to_filearray(
            list, &s->a, s->fa, max, bytes, message);
        break;

    case NAI_ASYNC_LOAD_AIO | NAI_ASYNC_LOAD_FCHUNK:
    case NAI_ASYNC_LOAD_EIO | NAI_ASYNC_LOAD_FCHUNK:
        /* sendfile */
        s->chunk = 1;
        r = nai_buflist_to_filechunk(
            list, &s->c, s->va, max, max_ht, bytes, message);

        if (r >= 0 && s->c.fd == NAI_FD_INVALID) {
            if (message && s->c.hcnt > vmax) {
                nai_errno = EOVERFLOW;
                r = -1;
            } else {
                l->stat = NAI_ASYNC_LOAD_UNINIT;
                r = sendfn(p, s->c.hvec, s->c.hcnt);
                l->stat = NAI_ASYNC_LOAD_DONE;
                if (r > 0) {
                    nai_buflist_rcommit(list, r);
                };
            };
            goto _end;
        };
        break;

    default:
        assert(n == NAI_INTPTR_T_MAX);
        r = n;
        goto _end;
    };
    if (r < 0) {
        ec = nai_errno;
        if (ec == EOVERFLOW) {
            r = NAI_INTPTR_T_MAX;
            goto _end;
        };
    };


    /* init send variables */
    l->hits = 10;
    l->used = sizeof(*s);
    l->count = 0;
    l->next = 0;
    l->out = 0;
    l->error = 0;
    l->sent = 0;


    /* mark sending */
    l->sending = 1;
    l->canceling = 0;
    l->discarded = 0;
    l->stat = NAI_ASYNC_LOAD_PENDING;


    /* prepare send */
    method = (n & NAI_ASYNC_LOAD_METHOD);
    direct = blocking;

    /* start sending */
    if (direct || l->loop == 0) {
        r = nai_async_load_bio_start_send(l, blocking);
    } else if (method == NAI_ASYNC_LOAD_EIO) {
        nai_async_load_set(l, NAI_ASYNC_LOAD_EIO);
        r = nai_async_load_eio_start_send(l);
    } else {
        nai_async_load_set(l, NAI_ASYNC_LOAD_AIO);
        r = nai_async_load_aio_start_send(l);
    };
    if (r < 0) {
        ec = nai_errno;
        if (ec != EINPROGRESS) {
            l->stat = NAI_ASYNC_LOAD_DONE;
            goto _end;
        };

    } else {

        /* completed */
        l->stat = NAI_ASYNC_LOAD_COMPLETED;

        /* output now */
        r = nai_async_load_flush(l, list, sendfn, p);
    };


_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// load file


nai_async_load_t* nai_async_load_create(
    nai_evloop_t* loop, nai_async_load_cb_f cb, void* ud)
{
    nai_int_t mt;
    nai_int_t pool;
    nai_async_load_t* l;


    if (loop) {
        if (!nai_evloop_back_available(loop)) {
            nai_errno = ENOTSUP;
            l = 0;
            goto _end;
        };

        pool = 1;
        l = (nai_async_load_t*)nai_evloop_ultra_alloc(loop, &mt);
    } else {
        pool = mt = 0;
        l = (nai_async_load_t*)nai_malloc(sizeof(*l));
    };
    if (l == 0) {
        goto _end;
    };

    l->cb = cb;
    l->ud = ud;
    l->loop = loop;
    l->wait = 0;
    l->stat = NAI_ASYNC_LOAD_DONE;
    l->meth = NAI_ASYNC_LOAD_EIO;
    l->reserved = 0;
    l->closed = 0;
    l->canceling = 0;
    l->discarded = 0;
    l->poolmem = !!pool;
    l->syncmem = !!mt;
    l->aioport = 0;
    l->iothread = 0;
    l->sending = 0;
    l->sendfile = 0;
    l->restore = 0;
    l->hits = 0;
    l->fd = NAI_FD_INVALID;
    l->timeout = 0;
    l->buf = 0;
    l->sent = 0;
    l->used = 0;
    l->total = 0;


    if (loop == 0) {
        /* blocked */
        ;
    } else if (nai_evloop_get_aio_port(loop)) {
        nai_async_load_set(l, NAI_ASYNC_LOAD_AIO);
        l->aioport = 1;
    } else if (nai_evloop_ref_io_threads(loop) >= 0) {
        nai_async_load_set(l, NAI_ASYNC_LOAD_EIO);
        l->iothread = 1;
    } else {
        /* unsupported */
        nai_async_load_free(l);
        assert(0);
        nai_errno = ENOTSUP;
        l = 0;
        goto _end;
    };

_end:
    return l;
};


intptr_t nai_async_load_tryout(
    nai_async_load_t* l, 
    nai_buflist_t* list, size_t bytes, nai_int_t message, 
    nai_async_load_sendv_f sendfn, void* s, nai_int_t vmax)
{
    intptr_t r;
    nai_int_t count;
    size_t size;
    size_t limit;
    nai_off64_t offs;
    nai_buf_t* b;
    nai_list_entry_t* e;
    nai_bufvec_t* v;
    nai_bufvec_t va[NAI_BUFV_MAX];


    /* check required load */
    v = va;
    count = 0;
    limit = bytes;
    e = list->ent.next;
    for ( ; e != &list->ent; ) {
        b = (nai_buf_t*)e;
        e = e->next;

        size = nai_buf_size(b);
        if (size <= 0) {
            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };
            continue;
        };

        if (nai_buf_in_file(b)) {
            if (count <= 0 || 
                l == 0 || l->fd == NAI_FD_INVALID) {
                r = NAI_INTPTR_T_MAX;
                goto _end;
            };

            r = nai_async_load_sendable(l, count, message);
            if (r == NAI_INTPTR_T_MAX) {
                goto _end;
            };

            if (message && !b->eos) {
                r = NAI_INTPTR_T_MAX;
                goto _end;
            };

            /* direct output */
            break;
        };

        if (size > limit) {
            size = limit;
        };

        if (!nai_buf_in_memory(b)) {
            if (size > NAI_BUF_READSIZE) {
                if (!nai_buf_in_file(b) || !b->directio) {
                    size = NAI_BUF_READSIZE;
                } else {
                    offs = nai_buf_offset(b) + NAI_BUF_READSIZE;
                    offs = nai_aligndown(offs, nai_sector_size);
                    size = (size_t)(offs - nai_buf_offset(b));
                };
            };
            b = nai_buf_cut(list->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            };

            e = b->ent.next;
        };

        if (message) {
            if (count >= vmax) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            count ++;

            v->buf = b->start;
            v->len = size;
            v ++;

            /* is last buffer of message */
            if (b->eos) {
                break;
            };

        } else {
            v->buf = b->start;
            v->len = size;
            v ++;

            count ++;
            if (count >= vmax) {
                break;
            };
        };

        limit -= size;
        if (limit <= 0) {
            break;
        };
    };


    if (count <= 0) {
        if (l != 0) {
            nai_async_load_unhits(l);
        };

        r = 0;
    } else {
        /* direct output */
        if (l != 0) {
            nai_async_load_unhits(l);
            l->stat = NAI_ASYNC_LOAD_UNINIT;
            r = sendfn(s, va, count);
            l->stat = NAI_ASYNC_LOAD_DONE;
        } else {
            r = sendfn(s, va, count);
        };
        if (r > 0) {
            nai_buflist_rcommit(list, r);
        };
    };

_end:
    return r;
};


intptr_t nai_async_load_start(
    nai_async_load_t* l, 
    nai_buflist_t* list, size_t bytes, nai_int_t message, 
    nai_async_load_sendv_f sendfn, void* s, nai_int_t vmax, 
    nai_int_t blocking)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t max;
    nai_int_t method;
    nai_int_t direct;
    size_t align;
    size_t sector_size;
    size_t used;
    size_t size;
    size_t limit;
    size_t readsize;
    uint8_t* buf;
    nai_buf_t* b;
    nai_list_entry_t* e;
    nai_filevec_t* v;
    nai_filevec_t fa[NAI_BUFV_MAX*8];


    assert(l->stat == NAI_ASYNC_LOAD_DONE || 
        l->stat == NAI_ASYNC_LOAD_UNINIT);


    if (l->fd != NAI_FD_INVALID) {
        r = nai_async_load_start_send(
            l, list, bytes, message, sendfn, s, vmax, blocking);
        if (r != NAI_INTPTR_T_MAX) {
            goto _end;
        };
    };

    /* setup file vector */
    buf = l->buf;
    if (buf) {
        v = (nai_filevec_t*)buf;
        max = (nai_int_t)(l->total / sizeof(*v));
    } else {
        v = fa;
        max = nai_countof(fa);
    };
    used = sizeof(*v) * NAI_BUFV_MAX;
    buf += used;


    /* get constant */
    sector_size = nai_sector_size;


    /* duplicate buflist */
    n = 0;
    limit = bytes;
    readsize = 0;
    e = list->ent.next;
    for ( ; e != &list->ent; e = e->next) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b);
        if (size <= 0) {
            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };
            continue;
        };

        if (n >= vmax) {
            if (message) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
        };
        if (n >= max) {
            break;
        };

        if (size > limit) {
            size = limit;
        };

        if (nai_buf_in_object(b)) {
            if (size > NAI_BUF_READSIZE) {
                size = NAI_BUF_READSIZE;
            };
            b = nai_buf_cut(list->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            };
            e = (nai_list_entry_t*)b;
        };

        if (!nai_buf_in_file(b)) {
            v[n].ptr = nai_buf_ptr(b);
            v[n].size = size;
            v[n].fd = NAI_FD_INVALID;
        } else {
            if (!b->directio) {
                buf += size;
                used += size;
            } else {
                align = (size_t)buf & (sector_size-1);
                if (align) {
                    align = sector_size - align;
                    buf += align;
                    used += align;
                };

                /* do alignment handling when reading large file in batches */
                align = (size_t)b->offset & (sector_size-1);
                align += size;
                if ((align & (sector_size-1)) &&    /* last page is not full */
                    (size < nai_buf_size(b)) &&     /* cut by 'limit' */
                    !message) {

                    assert(limit == size);

                    /* if no more page, read file next time */
                    if (!(align & ~(sector_size-1))) {
                        break;
                    };

                    /* remove last page and read it next time */
                    size -= align & ~(sector_size-1);
                    limit = size;
                    align &= ~(sector_size-1);
                };

                buf += align;
                used += align;
            };

            v[n].fd = nai_buf_fd(b);
            v[n].off = nai_buf_offset(b);
            v[n].size = size;
            v[n].oflags = b->directio ? NAI_O_DIRECT : 0;
            v[n].oflags |= b->asyncio ? NAI_O_ASYNCIO : 0;
            readsize += size;
        };

        n ++;

        limit -= size;
        if (limit <= 0) {
            break;
        };

        /* is last buffer of message */
        if (message && b->eos) {
            break;
        };
    };

    /* if buffering is not enough, at least one more page 
     * needs to be added to prevent out-of-bounds.
     */
    if (l->buf == 0 || (n - NAI_BUFV_MAX) > 0) {
        used += sector_size;
        if ((n - NAI_BUFV_MAX) > 0) {
            used += (n - NAI_BUFV_MAX) * sizeof(*v);
        };
    };

    /* allocate buffer */
    if (l->buf == 0 || l->total < used) {
        used = nai_align(used, 64*1024);
        buf = (uint8_t*)nai_malloc(used);
        if (buf == 0) {
            r = -1;
            goto _end;
        };

        if (l->buf == 0) {
            nai_memcpy(buf, fa, n * sizeof(*v));
        } else {
            nai_memcpy(buf, l->buf, n * sizeof(*v));
            nai_free(l->buf);
        };

        l->buf = buf;
        l->total = used;
    };


    /* init load variables */
    l->hits = 10;
    l->used = n * sizeof(*v);
    l->count = n;
    l->next = 0;
    l->out = 0;
    l->error = 0;
    l->sent = 0;


    /* mark loading */
    l->sending = 0;
    l->canceling = 0;
    l->discarded = 0;
    l->stat = NAI_ASYNC_LOAD_PENDING;


    /* prepare load */
    method = l->aioport ? NAI_ASYNC_LOAD_AIO : NAI_ASYNC_LOAD_EIO;
    direct = blocking;

#if (NAI_ASYNC_LOAD_LIMIT)
    /* check limit */
    if (readsize <= NAI_ASYNC_LOAD_LIMIT && method) {
        direct = 1;
    };
#endif

    /* start loading */
    if (direct || l->loop == 0) {
        r = nai_async_load_bio_start_load(l);
    } else if (method == NAI_ASYNC_LOAD_EIO) {
        nai_async_load_set(l, NAI_ASYNC_LOAD_EIO);
        r = nai_async_load_eio_start_load(l);
    } else {
        nai_async_load_set(l, NAI_ASYNC_LOAD_AIO);
        r = nai_async_load_aio_start_load(l);
    };
    if (r < 0) {
        ec = nai_errno;
        if (ec != EINPROGRESS) {
            l->stat = NAI_ASYNC_LOAD_DONE;
            goto _end;
        };

    } else {

        /* completed */
        l->stat = NAI_ASYNC_LOAD_COMPLETED;

        /* output now */
        r = nai_async_load_flush(l, list, sendfn, s);
    };

_end:
    return r;
};


intptr_t nai_async_load_flush(
    nai_async_load_t* l, nai_buflist_t* list, 
    nai_async_load_sendv_f sendfn, void* s)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t send;
    nai_int_t count;
    size_t total;
    nai_bufvec_t* v;


    assert(l->stat == NAI_ASYNC_LOAD_COMPLETED);

    /* let write functions ignore load */
    l->stat = NAI_ASYNC_LOAD_UNINIT;

    /* if it is sending then return bytes */
    if (l->sending) {
        r = l->sent;
        goto _end;
    };

    /* output loop */
    total = 0;
    count = l->out - l->next;
    v = (nai_bufvec_t*)l->buf + l->next;
    for ( ; count > 0; ) {
        send = count;
        if (send > NAI_BUFV_MAX) {
            send = NAI_BUFV_MAX;
        };

        r = sendfn(s, v, send);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };

        total += r;

        for ( ; count > 0; count --) {
            if (r < (intptr_t)v->len) {
                v->len -= r;
                v->buf += r;
                break;
            };

            r -= v->len;
            v ++;
            l->next ++;
        };
    };

    r = total;

_end:
    if (r > 0) {
        nai_buflist_rcommit(list, r);
    };
    if (l->stat == NAI_ASYNC_LOAD_UNINIT) {
        l->stat = NAI_ASYNC_LOAD_DONE;
    };
    return r;

_fail:
    if (ec == EAGAIN || ec == EINPROGRESS) {
        l->stat = NAI_ASYNC_LOAD_COMPLETED;
        if (total > 0) {
            r = total;
        };
    };
    goto _end;
};


nai_int_t nai_async_load_cancel(nai_async_load_t* l)
{
    nai_int_t r;


    switch (l->stat) {
    case NAI_ASYNC_LOAD_PENDING:
        l->canceling = 1;
        if (l->meth == NAI_ASYNC_LOAD_EIO) {
            r = nai_task_cancel(&l->op, nai_thread_io_interrupt);
        } else {
            r = nai_aio_cancel(&l->aio);
        };
        if (r >= 0) {
            l->stat = NAI_ASYNC_LOAD_COMPLETED;
            l->error = ECANCELED;
        };
        break;

    default:
        r = 0;
        break;
    };


    return r;
};


nai_int_t nai_async_load_discard(nai_async_load_t* l)
{
    nai_int_t r;


    switch (l->stat) {
    case NAI_ASYNC_LOAD_PENDING:
        l->discarded = 1;
        if (l->meth == NAI_ASYNC_LOAD_EIO) {
            r = nai_task_cancel(&l->op, 0);
            if (r < 0) {
                break;
            };
        } else {
            /* do nothing, nai_aio_cancel may cause ECANCELED, 
             * but we do not expect to cause any errors
             */
            nai_errno = EINPROGRESS;
            r = -1;
            break;
        };

        /* fallthrough */

    case NAI_ASYNC_LOAD_COMPLETED:
        if (l->sending) {
            assert(0);
            nai_errno = EPERM;
            r = -1;
            break;
        };

        l->stat = NAI_ASYNC_LOAD_DONE;
        nai_async_load_unhits(l);
        r = 0;
        break;

    default:
        r = 0;
        break;
    };


    return r;
};


nai_int_t nai_async_load_clear(nai_async_load_t* l)
{
    nai_int_t r;


    assert(l->stat == NAI_ASYNC_LOAD_DONE);

    if (l->buf) {
        nai_free(l->buf);
        l->buf = 0;
        l->used = 0;
        l->total = 0;
    };

    r = 0;

    return r;
};


nai_int_t nai_async_load_close(nai_async_load_t* l)
{
    nai_int_t r;
    nai_int_t ec;


    l->cb = 0;
    l->ud = 0;
    l->fd = NAI_FD_INVALID;
    l->closed = 1;
    l->canceling = 1;

    /* unref the io threads */
    if (l->loop && l->iothread) {
        nai_evloop_unref_io_threads(l->loop);
    };

    /* free load */
    switch (l->stat) {
    case NAI_ASYNC_LOAD_PENDING:
        assert(l->loop);

        if (l->meth == NAI_ASYNC_LOAD_EIO) {
            r = nai_task_cancel(&l->op, nai_thread_io_interrupt);
        } else {
            r = nai_aio_cancel(&l->aio);
        };
        if (r < 0) {
            ec = nai_errno;
            if (ec != ECANCELED) {
                break;
            };
        };

        /* fallthrough */

    default:
        nai_async_load_free(l);
        break;
    };

    r = 0;

    return r;
};


nai_int_t nai_async_load_wait(nai_async_load_t* l)
{
    nai_int_t r;
    nai_int_t ec;
    nai_mutex_t* m;


    m = nai_thread_mutex();
    if (m == 0) {
        r = -1;
        goto _end;
    };

    nai_mutex_lock(m);

    if (l->wait) {
        ec = EBUSY;
        r = -1;
        goto _exit;
    };

    if (l->stat != NAI_ASYNC_LOAD_PENDING) {
        r = 0;
        goto _exit;
    }

    /* get thread local condition */
    l->wait = nai_thread_local_cond();
    if (l->wait == 0) {
        ec = nai_errno;
        r = -1;
        goto _exit;
    };

    /* wait loop */
    for (;;) {
        if (l->stat != NAI_ASYNC_LOAD_PENDING) {
            r = 0;
            break;
        };

        r = nai_cond_wait(l->wait, m);
        if (r < 0) {
            break;
        };
    };

    /* detach */
    l->wait = 0;

    if (r < 0) {
        ec = nai_errno;
    };


_exit:
    nai_mutex_unlock(m);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


nai_int_t nai_async_load_signal(nai_async_load_t* l)
{
    nai_int_t r;
    nai_mutex_t* m;


    m = nai_thread_mutex();
    if (m == 0) {
        r = -1;
        goto _end;
    };

    nai_mutex_lock(m);

    if (l->wait) {
        nai_cond_signal(l->wait);
    };

    nai_mutex_unlock(m);

    r = 0;

_end:
    return r;
};


