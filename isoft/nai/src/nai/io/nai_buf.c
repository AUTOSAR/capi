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
/// @file       nai_buf.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_buf.h"
#include "nai/os/nai_thread.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_aio.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"



#ifndef NAI_BUF_PAGESIZE
#define NAI_BUF_PAGESIZE        (4096)
#endif
#ifndef NAI_BUF_READSIZE
#define NAI_BUF_READSIZE        (4096*2)
#endif


//////////////////////////////////////////////////////////////////////////////
// default bufpool


static void* nai_bufmem_alloc(void* c, size_t size)
{
    (void)c;

    return nai_malloc(size);
};

static nai_int_t nai_bufmem_free(void* c, void* p)
{
    (void)c;

    nai_free(p);
    return 0;
};

static nai_int_t nai_bufmem_xfree(void* c, void* p, size_t size)
{
    (void)c;
    (void)size;

    nai_free(p);
    return 0;
};

static nai_bufpool_ops_t nai_bufpool_sys_ops = {
    "syspool", 
    nai_bufmem_alloc, 
    nai_bufmem_free, 
    nai_bufmem_alloc, 
    nai_bufmem_xfree, 
    nai_bufmem_alloc, 
    nai_bufmem_xfree, 
    0
};
static nai_bufpool_t nai_bufpool_sys = {
    0,
    &nai_bufpool_sys_ops
};

static nai_bufpool_t* nai_bufpool_check(nai_bufpool_t* p)
{
    if (p == 0) {
        p = &nai_bufpool_sys;
    };
    return p;
};



//////////////////////////////////////////////////////////////////////////////
// buf


static nai_buf_t* nai_buf_new(nai_bufpool_t* p, nai_int_t extra)
{
    nai_buf_t* b;

    b  = (nai_buf_t*)nai_bufpool_malloc(p, 
        sizeof(nai_buf_t) + (extra ? sizeof(nai_buf_extra_t) : 0));

    return b;
};

static nai_int_t nai_buf_free(nai_buf_t* b)
{
    if (b->pool) {
        nai_bufpool_mfree(b->pool, b, 
            sizeof(*b) + (b->extra ? sizeof(nai_buf_extra_t) : 0));
    };
    return 0;
};

static nai_int_t nai_buf_ref(nai_buf_t* b)
{
    nai_int_t r;


    if (b->threading) {
        r = nai_atomic32_inc(&b->refcount);
        goto _end;
    };

    b->refcount ++;
    r = b->refcount;

_end:
    return r;
};

static nai_int_t nai_buf_unref(nai_buf_t* b)
{
    nai_int_t r;


    if (b->threading) {
        r = nai_atomic32_dec(&b->refcount);
        goto _end;
    };

    b->refcount --;
    r = b->refcount;

_end:
    return r;
};

static nai_int_t nai_buf_close_impl(nai_buf_t* b)
{
    nai_int_t r;
    size_t size;
    nai_buf_extra_t* x;


    if (nai_buf_unref(b) > 0) {
        r = 0;
        goto _end;
    };

    x = (nai_buf_extra_t*)(b + 1);
    if (b->extra && x->ops) {
        if (x->ops->free) {
            r = nai_bufops_free(x->ops, b->ref.obj);
            if (r < 0) {
                goto _end;
            };
        };
    } else if (b->typeref == NAI_BUF_MEMORY) {
        if (b->mempool) {
            size = b->total + (b->start - (uint8_t*)b->ref.obj);
            r = nai_bufpool_xfree(b->pool, b->ref.obj, size);
            if (r < 0) {
                goto _end;
            };
        };
    } else if (b->typeref == NAI_BUF_REF) {
        r = nai_buf_close_impl(b->ref.buf);
        if (r < 0) {
            goto _end;
        };
    };

    nai_buf_free(b);
    r = 0;

_end:
    return r;
};


nai_fd_t nai_buf_fd(nai_buf_t* b)
{
    nai_fd_t fd;
    nai_buf_t* d;
    nai_buf_extra_t* x;


    if (b->type != NAI_BUF_FILE) {
        fd = NAI_FD_INVALID;
    } else {
        /* get buffer which own file handle */
        d = b;
        if (b->typeref == NAI_BUF_REF) {
            d = b->ref.buf;
        };

        assert(d->extra);
        x = (nai_buf_extra_t*)(d + 1);

        fd = x->fd;
    };

    return fd;
};


nai_buf_t* nai_buf_from_file(
    nai_bufpool_t* pool, 
    nai_fd_t fd, nai_off64_t start, size_t size, nai_int_t oflags, 
    nai_buf_ops_t* ops, void* obj)
{
    nai_buf_t* b;
    nai_buf_extra_t* x;
    nai_bufpool_t* m;


    m = nai_bufpool_check(pool);
    b = nai_buf_new(m, 1);
    if (b == 0) {
        goto _end;
    };

    b->pool = m;
    b->refcount = 1;
    b->ref.obj = obj ? obj : 0;
    b->offset = start;
    b->size = size;
    b->total = size;
    b->flags = 0;
    b->type = NAI_BUF_FILE;
    b->typeref = NAI_BUF_FILE;
    b->asyncio = !!(oflags & NAI_O_ASYNCIO);
    b->directio = !!(oflags & NAI_O_DIRECT);
    b->extra = 1;

    x = (nai_buf_extra_t*)(b + 1);
    x->ops = ops;
    x->fd = fd;

_end:
    return b;
};


nai_buf_t* nai_buf_from_object(
    nai_bufpool_t* pool, void* p, size_t size, 
    nai_buf_ops_t* ops, void* obj)
{
    nai_buf_t* b;
    nai_buf_extra_t* x;
    nai_bufpool_t* m;


    if (ops == 0) {
        nai_errno = EINVAL;
        b = 0;
        goto _end;
    };

    m = nai_bufpool_check(pool);
    b = nai_buf_new(m, ops != 0);
    if (b == 0) {
        goto _end;
    };

    b->pool = m;
    b->refcount = 1;
    b->ref.obj = obj ? obj : 0;
    b->offset = (nai_off64_t)p;
    b->size = size;
    b->total = size;
    b->flags = 0;
    b->type = p ? NAI_BUF_MEMORY : NAI_BUF_OBJECT;
    b->typeref = obj ? NAI_BUF_OBJECT : NAI_BUF_MEMORY;
    b->extra = ops != 0;

    if (b->extra) {
        x = (nai_buf_extra_t*)(b + 1);
        x->ops = ops;
    };

_end:
    return b;
};


nai_buf_t* nai_buf_from_rmemory(
    nai_bufpool_t* pool, 
    const void* p, size_t size, nai_buf_ops_t* ops)
{
    nai_buf_t* b;
    nai_buf_extra_t* x;
    nai_bufpool_t* m;


    m = nai_bufpool_check(pool);
    b = nai_buf_new(m, ops != 0);
    if (b == 0) {
        goto _end;
    };

    b->pool = m;
    b->refcount = 1;
    b->ref.obj = (void*)p;
    b->offset = (nai_off64_t)p;
    b->size = size;
    b->total = size;
    b->flags = 0;
    b->type = NAI_BUF_MEMORY;
    b->typeref = NAI_BUF_MEMORY;
    b->extra = ops != 0;

    if (b->extra) {
        x = (nai_buf_extra_t*)(b + 1);
        x->ops = ops;
    };

_end:
    return b;
};


nai_buf_t* nai_buf_from_wmemory(
    nai_bufpool_t* pool, 
    void* p, size_t size, nai_buf_ops_t* ops)
{
    nai_buf_t* b;
    nai_buf_extra_t* x;
    nai_bufpool_t* m;


    m = nai_bufpool_check(pool);
    b = nai_buf_new(m, ops != 0);
    if (b == 0) {
        goto _end;
    };

    b->pool = m;
    b->refcount = 1;
    b->ref.obj = p;
    b->offset = (nai_off64_t)p;
    b->size = 0;
    b->total = size;
    b->flags = 0;
    b->type = NAI_BUF_MEMORY;
    b->typeref = NAI_BUF_MEMORY;
    b->extra = ops != 0;

    if (b->extra) {
        x = (nai_buf_extra_t*)(b + 1);
        x->ops = ops;
    };

_end:
    return b;
};


nai_buf_t* nai_buf_alloc(nai_bufpool_t* pool, size_t size)
{
    nai_int_t ec;
    void* p;
    nai_buf_t* b;
    nai_bufpool_t* m;


    m = nai_bufpool_check(pool);
    p = nai_bufpool_xalloc(m, size);
    if (p == 0) {
        b = 0;
        goto _end;
    };

    b = nai_buf_new(m, 0);
    if (b == 0) {
        ec = nai_errno;
        nai_bufpool_xfree(m, p, size);
        nai_errno = ec;
        goto _end;
    };

    b->pool = m;
    b->refcount = 1;
    b->ref.obj = p;
    b->offset = (nai_off64_t)p;
    b->size = 0;
    b->total = size;
    b->flags = 0;
    b->type = NAI_BUF_MEMORY;
    b->typeref = NAI_BUF_MEMORY;
    b->mempool = 1;

_end:
    return b;
};


nai_buf_t* nai_buf_dup(
    nai_bufpool_t* pool, nai_buf_t* b, nai_int_t memory)
{
    return nai_buf_sub(pool, b, 0, b->size, memory);
};


nai_buf_t* nai_buf_cut(
    nai_bufpool_t* pool, nai_buf_t* b, size_t size, nai_int_t memory)
{
    nai_buf_t* d;


    d = nai_buf_sub(pool, b, 0, size, memory);
    if (d == 0) {
        goto _end;
    };

    nai_buf_rcommit(b, d->size);
    if (b->linked) {
        d->linked = 1;
        nai_list_insert_before(&b->ent, &d->ent);

        if (nai_buf_size(b) <= 0) {
            d->eos = b->eos;
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        };
    };

_end:
    return d;
};


nai_buf_t* nai_buf_sub(
    nai_bufpool_t* pool, 
    nai_buf_t* b, nai_off64_t start, size_t size, nai_int_t memory)
{
    intptr_t r;
    nai_int_t ec = 0;
    nai_int_t directio;
    size_t align;
    size_t sector_size;
    size_t roff;
    size_t rsize;
    nai_off64_t offset;
    nai_buf_t* d = 0;
    nai_buf_t* o;
    nai_buf_extra_t* x;
    nai_bufpool_t* m;
#if defined(_WIN32)
    nai_aio_t aio;
#endif


    // check start and size
    if (start < 0) {
        start = b->size - start;
    };
    if (start > (nai_off64_t)b->size) {
        nai_errno = EINVAL;
        goto _end;
    };
    if (size > b->size || start + size > b->size) {
        nai_errno = EINVAL;
        goto _end;
    };


    // alloc a new buffer
    m = nai_bufpool_check(pool);
    d = nai_buf_new(m, 0);
    if (d == 0) {
        goto _end;
    };

    d->pool = m;
    d->refcount = 1;
    d->flags = 0;


    // get real buffer
    o = b;
    if (o->typeref == NAI_BUF_REF) {
        o = o->ref.buf;
    };

    // 
    if (b->type == NAI_BUF_MEMORY || !memory) {
        d->offset = (nai_off64_t)b->start + start;
        d->size = size;
        d->total = size;
        d->type = b->type;
        d->typeref = NAI_BUF_REF;
        d->ref.buf = o;
        nai_buf_ref(o);

    } else {

        d->size = size;
        d->total = size;

        if (o->extra) {
            x = (nai_buf_extra_t*)(o + 1);
        } else {
            x = 0;
        };

        offset = b->offset + start;
        if (x && x->ops && x->ops->mmap) {

            if (nai_bufops_mmap(x->ops, 
                o->ref.obj, (void**)&d->start, size, offset) < 0) {
                goto _fail;
            };

            d->type = NAI_BUF_MEMORY;
            d->typeref = NAI_BUF_REF;
            d->ref.buf = o;
            nai_buf_ref(o);

        } else {

            d->type = NAI_BUF_MEMORY;
            d->typeref = NAI_BUF_MEMORY;
            d->mempool = 1;

            /* check whether we shuold perform direct read */
            directio = 0;
            if (o->directio && o->typeref == NAI_BUF_FILE) {
                assert(x != 0);
                if (x->ops == 0 || x->ops->read == 0) {
                    directio = 1;
                };
            };

            /* align offset and count required memory size */
            if (directio) {
                sector_size = nai_sector_size;

                /* align offset */
                roff = (size_t)(
                    offset - nai_aligndown(offset, sector_size));
                offset -= roff;

                /* count required memory size */
                rsize = size;
                size += roff;
                size = (size_t)(nai_align(
                    offset + size, sector_size) - offset);

                size += sector_size;
                d->total = size;
            } else {
                roff = 0;
                rsize = 0;
            };

            /* alloc memory */
            d->ref.obj = nai_bufpool_xalloc(m, size);
            d->offset = (intptr_t)d->ref.obj;
            if (d->offset == 0) {
                ec = nai_errno;
                goto _fail;
            };

            /* align memory  */
            if (directio) {
                size -= sector_size;
                align = (size_t)d->start & (sector_size-1);
                if (align) {
                    d->total -= align;
                    d->start += align;
                };
            };

            /* read data */
            if (x && x->ops && x->ops->read) {
                r = nai_bufops_read(
                    x->ops, o->ref.obj, d->start, size, offset);

            } else if (o->typeref == NAI_BUF_FILE) {
                assert(x);

#if defined(_WIN32)
                if (o->asyncio) {
                    nai_aio_init(&aio);
                    nai_aio_set_waitable(&aio, 1);
                    r = nai_aio_read(&aio, x->fd, d->start, size, offset);
                    if (r < 0) {
                        ec = nai_errno;
                        if (ec != EINPROGRESS) {
                            goto _fail;
                        };
                    };
                    r = nai_aio_result(&aio, 1);
                } else {
#endif
                    r = nai_file_pread(x->fd, d->start, size, offset);
#if defined(_WIN32)
                };
#endif

            } else {
                ec = EINVAL;
                goto _fail;
            };
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            if (directio) {
                d->start += roff;
                d->total -= roff;

                r -= roff;
                if (r < 0) {
                    r = 0;
                };

                size = rsize;
            };
            if (size > (size_t)r) {
                /* warning: no more data ??? */
                if (r > 0) {
                    d->size = r;
                } else {
                    /* eof, fill zero */
                    nai_memset(d->start, 0, size);
                };
            };
        };
    };


_end:
    return d;

_fail:
    if (d) {
        nai_buf_close(d);
        nai_errno = ec;
        d = 0;
    };
    return d;
};


nai_int_t nai_buf_close(nai_buf_t* b)
{
    nai_int_t r;


    if (b == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (b->linked) {
        b->linked = 0;
        nai_list_entry_remove(&b->ent);
    };

    r = nai_buf_close_impl(b);

_end:
    return r;
};




//////////////////////////////////////////////////////////////////////////////
// buflist


nai_int_t nai_buflist_add_head(nai_buflist_t* l, nai_buflist_t* a)
{
    nai_list_add_head(&l->ent, &a->ent);
    return 0;
};


nai_int_t nai_buflist_add_tail(nai_buflist_t* l, nai_buflist_t* a)
{
    nai_list_add_tail(&l->ent, &a->ent);
    return 0;
};


nai_int_t nai_buflist_set_threading(nai_buflist_t* l)
{
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = l->ent.next;
    for ( ; e != &l->ent; e = e->next) {
        b = (nai_buf_t*)e;
        nai_buf_set_threading(b);
    };

    return 0;
};


nai_int_t nai_buflist_set_threadsafe(nai_buflist_t* l)
{
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = l->ent.next;
    for ( ; e != &l->ent; e = e->next) {
        b = (nai_buf_t*)e;
        nai_buf_set_threadsafe(b);
    };

    return 0;
};


nai_int_t nai_buflist_close(nai_buflist_t* l)
{
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = l->ent.next;
    for ( ; e != &l->ent; ) {
        b = (nai_buf_t*)e;
        e = e->next;
        nai_buf_close(b);
    };

    nai_list_init(&l->ent);
    return 0;
};


nai_int_t nai_buflist_rcommit(nai_buflist_t* l, size_t bytes)
{
    size_t size;
    nai_buf_t* b;


    for (;;) {
        if (nai_buflist_is_empty(l)) {
            /* the left bytes should be zero */
            assert(bytes == 0);
            break;
        };

        b = (nai_buf_t*)l->ent.next;
        size = nai_buf_size(b);
        if (bytes >= size) {
            bytes -= size;
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        } else {
            nai_buf_rcommit(b, bytes);
            break;
        };
    };

    return 0;
};


nai_int_t nai_buflist_rcommit_last(nai_buflist_t* l, size_t bytes)
{
    size_t size;
    nai_buf_t* b;


    for (;;) {
        if (nai_buflist_is_empty(l)) {
            /* the left bytes should be zero */
            assert(bytes == 0);
            break;
        };

        b = (nai_buf_t*)l->ent.prev;
        size = nai_buf_size(b);
        if (bytes >= size) {
            bytes -= size;
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        } else {
            b->size -= bytes;
            break;
        };
    };

    return 0;
};


nai_int_t nai_buflist_wcommit(nai_buflist_t* l, size_t bytes)
{
    size_t space;
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = l->ent.next;
    for ( ; e != &l->ent; ) {
        b = (nai_buf_t*)e;
        e = e->next;

        space = nai_buf_space(b);
        if (bytes > space) {
            bytes -= space;
            b->size = b->total;
        } else {
            b->size += bytes;
            bytes = 0;
            break;
        };
    };

    assert(bytes == 0);

    return 0;
};


static nai_int_t nai_buf_info(nai_buf_t* b, nai_buflist_info_t* i)
{
    if (b->eos) {
        i->eos = 1;
    };
    if (b->flush) {
        i->flush = 1;
    };

    switch (b->type) {
    case NAI_BUF_FILE:
        i->file = 1;
        if (b->typeref == NAI_BUF_REF) {
            b = b->ref.buf;
        };
        if (b->asyncio) {
            i->asyncio = 1;
        };
        if (b->directio) {
            i->directio = 1;
        };
        break;

    case NAI_BUF_OBJECT:
        i->object = 1;
        break;

    default:
        break;
    };

    return 0;
};


static nai_int_t nai_buf_typeinfo(nai_buf_t* b, nai_buflist_info_t* i)
{
    switch (b->type) {
    case NAI_BUF_FILE:
        i->file = 1;
        if (b->typeref == NAI_BUF_REF) {
            b = b->ref.buf;
        };
        if (b->asyncio) {
            i->asyncio = 1;
        };
        if (b->directio) {
            i->directio = 1;
        };
        break;

    case NAI_BUF_OBJECT:
        i->object = 1;
        break;

    default:
        break;
    };

    return 0;
};


nai_int_t nai_buflist_fulled(nai_buflist_t* l, nai_buflist_t* f, size_t bytes)
{
    size_t space;
    nai_buf_t* b;


    for ( ; bytes > 0; ) {
        if (nai_buflist_is_empty(l)) {
            /* buflist should be not empty */
            assert(0);
            break;
        };

        b = (nai_buf_t*)l->ent.next;
        space = nai_buf_space(b);
        if (space <= bytes) {
            bytes -= space;
            b->size = b->total;
            nai_buf_entry_remove(b);
            nai_buflist_insert_tail(f, b);
        } else {
            b->size += bytes;
            break;
        };
    };

    return 0;
};


int64_t nai_buflist_space(nai_buflist_t* l, nai_int_t check)
{
    int64_t r;
    nai_buf_t* b;
    nai_list_entry_t* e;


    r  = 0;
    e = l->ent.next;

    if (check == 0) {
        for ( ; e != &l->ent; e = e->next) {
            b = (nai_buf_t*)e;
            r += nai_buf_space(b);
        };
    } else {
        for ( ; e != &l->ent; e = e->next) {
            b = (nai_buf_t*)e;
            r += nai_buf_space(b);
            if (r > 0) {
                break;
            };
        };
    };

    return r;
};


int64_t nai_buflist_size(
    nai_buflist_t* l, nai_int_t eos, nai_buflist_info_t* i)
{
    int64_t r;
    nai_buf_t* b;
    nai_list_entry_t* e;


    r  = 0;
    e = l->ent.next;

    if (eos == 0) {
        if (i == 0) {
            for ( ; e != &l->ent; e = e->next) {
                b = (nai_buf_t*)e;
                r += nai_buf_size(b);
            };
        } else {
            i->flags = 0;
            for ( ; e != &l->ent; e = e->next) {
                b = (nai_buf_t*)e;
                r += nai_buf_size(b);
                nai_buf_info(b, i);
            };
        };
    } else {
        if (i == 0) {
            for ( ; e != &l->ent; e = e->next) {
                b = (nai_buf_t*)e;
                r += nai_buf_size(b);

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };
            };
        } else {
            i->flags = 0;
            for ( ; e != &l->ent; e = e->next) {
                b = (nai_buf_t*)e;
                r += nai_buf_size(b);
                nai_buf_info(b, i);

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };
            };
        };
    };

    return r;
};


intptr_t nai_buflist_move(nai_buflist_t* l, 
    nai_buflist_t* s, size_t bytes, nai_int_t eos, nai_buflist_info_t* i)
{
    intptr_t r;
    size_t size;
    nai_buf_t* b;
    nai_list_entry_t* e;


    r  = 0;
    e = s->ent.next;

    if (i == 0) {
        for ( ; e != &s->ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= bytes) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(l, b);
                bytes -= size;
                r += size;

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };

            } else {
                if (bytes <= 0) {
                    break;
                };

                size = bytes;
                b = nai_buf_sub(l->pool, b, 0, size, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                nai_buflist_insert_tail(l, b);
                bytes = 0;
                r += size;
                break;
            };
        };
    } else {
        i->flags = 0;
        for ( ; e != &s->ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= bytes) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(l, b);
                nai_buf_info(b, i);
                bytes -= size;
                r += size;

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };

            } else {
                if (bytes <= 0) {
                    break;
                };

                size = bytes;
                b = nai_buf_sub(l->pool, b, 0, size, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                nai_buflist_insert_tail(l, b);
                nai_buf_typeinfo(b, i);
                bytes = 0;
                r += size;
                break;
            };
        };
    };

_end:
    return r;
};


intptr_t nai_buflist_move_last(nai_buflist_t* l, 
    nai_buflist_t* s, size_t bytes, nai_int_t eos, nai_buflist_info_t* i)
{
    intptr_t r;
    size_t size;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_list_entry_t* e;


    r  = 0;
    e = s->ent.prev;

    if (i == 0) {
        for ( ; e != &s->ent; ) {
            b = (nai_buf_t*)e;
            e = e->prev;

            size = nai_buf_size(b);
            if (size <= bytes) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(l, b);
                bytes -= size;
                r += size;

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };

            } else {
                if (bytes <= 0) {
                    break;
                };

                d = nai_buf_sub(l->pool, b, size - bytes, bytes, 0);
                if (d == 0) {
                    r = -1;
                    goto _end;
                };

                nai_buflist_insert_tail(l, d);
                size = bytes;
                b->size -= size;
                b->total = b->size;
                bytes = 0;
                r += size;
                break;
            };
        };
    } else {
        i->flags = 0;
        for ( ; e != &s->ent; ) {
            b = (nai_buf_t*)e;
            e = e->prev;

            size = nai_buf_size(b);
            if (size <= bytes) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(l, b);
                nai_buf_info(b, i);
                bytes -= size;
                r += size;

                /* is last buffer of message */
                if (eos && b->eos) {
                    break;
                };

            } else {
                if (bytes <= 0) {
                    break;
                };

                d = nai_buf_sub(l->pool, b, size - bytes, bytes, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                nai_buflist_insert_tail(l, d);
                nai_buf_typeinfo(d, i);
                size = bytes;
                b->size -= size;
                b->total = b->size;
                bytes = 0;
                r += size;
                break;
            };
        };
    };

_end:
    return r;
};


intptr_t nai_buflist_read(nai_buflist_t* l, void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t left;
    nai_off64_t offs;
    nai_buf_t* b;
    nai_list_entry_t* e;
    uint8_t* str;


    str = (uint8_t*)buf;

    r = 0;
    e = l->ent.next;
    for ( ; ; e = e->next) {
        if (e == &l->ent) {
            /* fill all */
            break;
        };

        b = (nai_buf_t*)e;
        size = nai_buf_size(b);
        if (size <= 0) {
            continue;
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
            b = nai_buf_cut(l->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            };
            e = (nai_list_entry_t*)b;
        };

        left = size;
        if (left) {
            if (left >= len) {
                nai_memcpy(str, nai_buf_ptr(b), len);
                nai_buf_rcommit(b, len);
                r += len;
                str += len;
                break;
            } else {
                nai_memcpy(str, nai_buf_ptr(b), left);
                nai_buf_rcommit(b, left);
                r += left;
                str += left;
                len -= left;
            };
        };
    };


_end:
    return r;
};


intptr_t nai_buflist_write(
    nai_buflist_t* l, const void* buf, size_t len, nai_int_t grow)
{
    intptr_t r;
    /* nai_int_t eos = 0; */
    size_t size;
    size_t left;
    nai_buf_t* b;
    nai_list_entry_t* e;
    uint8_t* str;


    str = (uint8_t*)buf;

    r = 0;
    e = l->ent.next;
    for ( ; ; e = e->next) {
        if (e == &l->ent) {
            if (!grow) {
                break;
            };

            /* grow buffer list */
            size = len;
            size = nai_align(size, NAI_BUF_PAGESIZE);
            b = nai_buf_alloc(l->pool, size);
            if (b == 0) {
                r = -1;
                goto _end;
            };
            nai_buflist_insert_tail(l, b);
            e = (nai_list_entry_t*)b;
            /* eos = 1; */

        } else {

            b = (nai_buf_t*)e;
            if (!nai_buf_in_memory(b)) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        };

        size = nai_buf_size(b);
        left = nai_buf_space(b);
        if (left) {
            /* if (eos == 0 && e->next != &l->ent) { */
            /*     e = e->next; */
            /*     for (;;) { */
            /*         if (nai_buf_size((nai_buf_t*)e) > 0) { */
            /*             break; */
            /*         }; */
            /*         e = e->next; */
            /*         if (e == &l->ent) { */
            /*             eos = 1; */
            /*             break; */
            /*         }; */
            /*     }; */
            /*     if (eos == 0) { */
            /*         continue; */
            /*     }; */
            /*  */
            /*     e = (nai_list_entry_t*)b; */
            /* }; */

            if (left >= len) {
                nai_memcpy(nai_buf_ptr(b)+size, str, len);
                nai_buf_wcommit(b, len);
                r += len;
                str += len;
                len = 0;
                break;
            } else {
                nai_memcpy(nai_buf_ptr(b)+size, str, left);
                nai_buf_wcommit(b, left);
                r += left;
                str += left;
                len -= left;
            };
        };
    };


_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// buflist operations


intptr_t nai_buflist_to_rbufvec(
    nai_buflist_t* l, nai_bufvec_t* v, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_rbufvec(&i, v, limit, message);

    return r;
};


intptr_t nai_buflist_to_rbufarray(
    nai_buflist_t* l, nai_bufarray_t* a, 
    nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_rbufarray(&i, a, v, count, limit, message);

    return r;
};


intptr_t nai_buflist_to_filechunk(
    nai_buflist_t* l, nai_filechunk_t* c, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t max_ht, size_t limit, 
    nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_filechunk(&i, c, v, count, max_ht, limit, message);

    return r;
};


intptr_t nai_buflist_to_filearray(
    nai_buflist_t* l, nai_filearray_t* a, 
    nai_filevec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_filearray(&i, a, v, count, limit, message);

    return r;
};


intptr_t nai_buflist_to_wbufvec(
    nai_buflist_t* l, nai_bufvec_t* v, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_wbufvec(&i, v, limit, message);

    return r;
};


intptr_t nai_buflist_to_wbufarray(
    nai_buflist_t* l, nai_bufarray_t* a, 
    nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    nai_bufwalk_t i;


    nai_bufwalk_init(&i, l);
    r = nai_bufwalk_to_wbufarray(&i, a, v, count, limit, message);

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// bufwalk operations


intptr_t nai_bufwalk_to_rbufvec(
    nai_bufwalk_t* i, nai_bufvec_t* v, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t off;
    size_t size;
    size_t total = 0;
    nai_off64_t offs;
    nai_buf_t* b;
    nai_buflist_t* l;
    nai_list_entry_t* e;


    if (limit <= 0) {
        v->buf = 0;
        v->len = 0;
        r = 0;
        goto _end;
    };

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };

            continue;
        };

        if (size > limit) {
            size = limit;
        };

        if (!nai_buf_in_memory(b)) {
            if (off != 0) {
                b = nai_buf_cut(l->pool, b, off, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                e = (nai_list_entry_t*)b;
                continue;
            };

            if (size > NAI_BUF_READSIZE) {
                if (!nai_buf_in_file(b) || !b->directio) {
                    size = NAI_BUF_READSIZE;
                } else {
                    offs = nai_buf_offset(b) + NAI_BUF_READSIZE;
                    offs = nai_aligndown(offs, nai_sector_size);
                    size = (size_t)(offs - nai_buf_offset(b));
                };
            };
            b = nai_buf_cut(l->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            } else {
                /* nothing */
                ;
            };
            e = (nai_list_entry_t*)b;
        };

        if (message) {
            if (total) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };

            v->buf = b->start + off;
            v->len = size;
            total += size;
            limit -= size;
            if (limit <= 0) {
                off += size;
                break;
            };

            /* is last buffer of message */
            if (b->eos) {
                off += size;
                break;
            };

        } else {
            v->buf = b->start + off;
            v->len = size;
            total += size;
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_size(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    r = total;

_end:
    return r;
};


intptr_t nai_bufwalk_to_rbufarray(
    nai_bufwalk_t* i, nai_bufarray_t* a, 
    nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t off;
    size_t size;
    size_t total = 0;
    nai_off64_t offs;
    nai_buf_t* b;
    nai_buflist_t* l;
    nai_list_entry_t* e;


    a->v = v;
    a->count = 0;
    if (limit <= 0) {
        r = 0;
        goto _end;
    };
    if (count <= 0 && !message) {
        r = 0;
        goto _end;
    };

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };

            continue;
        };

        if (size > limit) {
            size = limit;
        };

        if (!nai_buf_in_memory(b)) {
            if (off != 0) {
                b = nai_buf_cut(l->pool, b, off, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                e = (nai_list_entry_t*)b;
                continue;
            };

            if (size > NAI_BUF_READSIZE) {
                if (!nai_buf_in_file(b) || !b->directio) {
                    size = NAI_BUF_READSIZE;
                } else {
                    offs = nai_buf_offset(b) + NAI_BUF_READSIZE;
                    offs = nai_aligndown(offs, nai_sector_size);
                    size = (size_t)(offs - nai_buf_offset(b));
                };
            };
            b = nai_buf_cut(l->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            } else {
                /* nothing */
                ;
            };
            e = (nai_list_entry_t*)b;
        };

        if (message) {
            if (a->count >= count) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            a->count ++;

            v->buf = b->start + off;
            v->len = size;
            v ++;
            total += size;

            /* is last buffer of message */
            if (b->eos) {
                off += size;
                break;
            };

        } else {
            v->buf = b->start + off;
            v->len = size;
            v ++;
            total += size;

            a->count ++;
            if (a->count >= count) {
                off += size;
                break;
            };
        };

        limit -= size;
        if (limit <= 0) {
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_size(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    r = total;

_end:
    return r;
};


intptr_t nai_bufwalk_to_filechunk(
    nai_bufwalk_t* i, nai_filechunk_t* c, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t max_ht, size_t limit, 
    nai_int_t message)
{
    intptr_t r;
    nai_int_t file = 0;
    nai_int_t head = 0;
    nai_int_t tail = 0;
    size_t off;
    size_t size;
    size_t hsize = 0;
    size_t tsize = 0;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_buf_extra_t* x;
    nai_buflist_t* l;
    nai_bufvec_t* vstart = v;
    nai_bufvec_t* vend = v + count;
    nai_list_entry_t* e;


    c->fd = NAI_FD_INVALID;
    c->oflags = 0;
    c->off = 0;
    c->size = 0;

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };

            continue;
        };

        if (size > limit) {
            size = limit;
        };

        if (!nai_buf_in_file(b)) {

            if (v >= vend) {
                if (message) {
                    goto _over;
                };
                break;
            };

            if (nai_buf_in_object(b)) {
                if (off != 0) {
                    b = nai_buf_cut(l->pool, b, off, 0);
                    if (b == 0) {
                        r = -1;
                        goto _end;
                    };

                    e = (nai_list_entry_t*)b;
                    continue;
                };

                if (size > NAI_BUF_READSIZE) {
                    size = NAI_BUF_READSIZE;
                };
                b = nai_buf_cut(l->pool, b, size, 1);
                if (b == 0) {
                    r = -1;
                    goto _end;
                } else if (nai_buf_size(b) < size) {
                    size = nai_buf_size(b);
                } else {
                    /* nothing */
                    ;
                };
                e = (nai_list_entry_t*)b;
            };

            v->buf = b->start;
            v->len = size;
            v ++;

            /* add head if the element before file, otherwise add tail */
            if (file) {
                /* break if too many tailers */
                if (tail >= max_ht && max_ht >= 0) {
                    if (message) {
                        goto _over;
                    };
                    break;
                };

                tail ++;
                tsize += size;
            } else {
                head ++;
                hsize += size;
            };

        } else {
            /* already found a file, filechunk accept only one file */
            if (file) {
                if (message) {
                    goto _over;
                };
                break;
            };
            /* break if too many headers */
            if (head > max_ht && max_ht >= 0) {
                if (message) {
                    goto _over;
                };
                break;
            };

            /* get the buffer which own file handle */
            d = b;
            if (b->typeref == NAI_BUF_REF) {
                d = b->ref.buf;
            };

            assert(d->extra);
            x = (nai_buf_extra_t*)(d + 1);

            /* get file information */
            c->fd = x->fd;
            c->off = b->offset;
            c->size = size;
            c->oflags |= b->asyncio ? NAI_O_ASYNCIO : 0;
            c->oflags |= b->directio ? NAI_O_DIRECT : 0;
            file = 1;
        };

        limit -= size;
        if (limit <= 0) {
            off += size;
            break;
        };

        /* is last buffer of message */
        if (message && b->eos) {
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_size(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    c->hvec = vstart;
    c->hcnt = head;
    c->hsize = hsize;
    c->tvec = vstart + head;
    c->tcnt = tail;
    c->tsize = tsize;

    r = c->size + c->hsize + c->tsize;

_end:
    return r;

_over:
    nai_errno = EOVERFLOW;
    r = -1;
    goto _end;
};


intptr_t nai_bufwalk_to_filearray(
    nai_bufwalk_t* i, nai_filearray_t* a, 
    nai_filevec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t off;
    size_t size;
    size_t total = 0;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_buf_extra_t* x;
    nai_buflist_t* l;
    nai_list_entry_t* e;


    a->v = v;
    a->count = 0;
    if (limit <= 0) {
        r = 0;
        goto _end;
    };
    if (count <= 0 && !message) {
        r = 0;
        goto _end;
    };

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            /* is last buffer of message */
            if (message && b->eos) {
                break;
            };

            continue;
        };

        if (size > limit) {
            size = limit;
        };

        if (nai_buf_in_object(b)) {
            if (off != 0) {
                b = nai_buf_cut(l->pool, b, off, 0);
                if (b == 0) {
                    r = -1;
                    goto _end;
                };

                e = (nai_list_entry_t*)b;
                continue;
            };

            if (size > NAI_BUF_READSIZE) {
                size = NAI_BUF_READSIZE;
            };
            b = nai_buf_cut(l->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            };
            e = (nai_list_entry_t*)b;
        };

        /* get the buffer which own file handle */
        d = b;
        if (nai_buf_in_file(d)) {
            if (b->typeref == NAI_BUF_REF) {
                d = b->ref.buf;
            };
            assert(d->extra);
            x = (nai_buf_extra_t*)(d + 1);
            v->fd = x->fd;
            v->oflags = 0;
            v->oflags |= d->asyncio ? NAI_O_ASYNCIO : 0;
            v->oflags |= d->directio ? NAI_O_DIRECT : 0;
        } else {
            v->fd = NAI_FD_INVALID;
            v->oflags = 0;
        };

        if (message) {
            if (a->count >= count) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            a->count ++;

            v->off = b->offset;
            v->size = size;
            v ++;
            total += size;

            /* is last buffer of message */
            if (b->eos) {
                off += size;
                break;
            };

        } else {

            v->off = b->offset;
            v->size = size;
            v ++;
            total += size;

            a->count ++;
            if (a->count >= count) {
                off += size;
                break;
            };
        };

        limit -= size;
        if (limit <= 0) {
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_size(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    r = total;

_end:
    return r;
};


intptr_t nai_bufwalk_to_wbufvec(
    nai_bufwalk_t* i, nai_bufvec_t* v, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t off;
    size_t size;
    size_t total = 0;
    nai_buf_t* b;
    nai_buflist_t* l;
    nai_list_entry_t* e;


    if (limit <= 0) {
        v->buf = 0;
        v->len = 0;
        r = 0;
        goto _end;
    };

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_space(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            continue;
        };

        if (!nai_buf_in_memory(b)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if (size > limit) {
            size = limit;
        };

        if (message) {
            if (total) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };

            v->buf = b->start + b->size + off;
            v->len = size;
            total += size;
            limit -= size;
            if (limit <= 0) {
                off += size;
                break;
            };
        } else {
            v->buf = b->start + b->size + off;
            v->len = size;
            total += size;
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_space(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    r = total;

_end:
    return r;
};


intptr_t nai_bufwalk_to_wbufarray(
    nai_bufwalk_t* i, nai_bufarray_t* a, 
    nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message)
{
    intptr_t r;
    size_t off;
    size_t size;
    size_t total = 0;
    nai_buf_t* b;
    nai_buflist_t* l;
    nai_list_entry_t* e;


    a->v = v;
    a->count = 0;
    if (limit <= 0) {
        r = 0;
        goto _end;
    };
    if (count <= 0 && !message) {
        r = 0;
        goto _end;
    };

    l = i->list;
    e = i->buf;
    off = i->off;
    for ( ; e != &l->ent; e = e->next, off = 0) {
        b = (nai_buf_t*)e;
        size = nai_buf_space(b) - off;
        if ((intptr_t)size <= 0) {
            if ((intptr_t)size < 0) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            continue;
        };

        if (!nai_buf_in_memory(b)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if (size > limit) {
            size = limit;
        };
        if (message) {
            if (a->count >= count) {
                nai_errno = EOVERFLOW;
                r = -1;
                goto _end;
            };
            a->count ++;

            v->buf = b->start + b->size;
            v->len = size;
            v ++;
            total += size;
        } else {
            v->buf = b->start + b->size;
            v->len = size;
            v ++;
            total += size;

            a->count ++;
            if (a->count >= count) {
                off += size;
                break;
            };
        };

        limit -= size;
        if (limit <= 0) {
            off += size;
            break;
        };
    };

    if (e != &l->ent) {
        b = (nai_buf_t*)e;
        if (off >= nai_buf_space(b)) {
            e = e->next;
            off = 0;
        };
    };

    i->buf = e;
    i->off = off;

    r = total;

_end:
    return r;
};


