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
/// @file       nai_bufpool.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_buf.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



#define NAI_BUFZONE_LOCKS               (17)
#define NAI_BUFZONE_ENTRIES             (18)
#define NAI_BUFZONE_MAXSIZE             (16*1024)


#if !defined(NAI_BUFCHECK_ENTRY)
#if !defined(_DEBUG)
#define NAI_BUFCHECK_ENTRY              0
#else
#define NAI_BUFCHECK_ENTRY              1
#endif
#endif

#if !defined(NAI_BUFCHECK_PADDING)
#if !defined(_DEBUG)
#define NAI_BUFCHECK_PADDING            0
#else
#define NAI_BUFCHECK_PADDING            1
#endif
#endif



typedef struct nai_bufzone_entry_s nai_bufzone_entry_t;
typedef struct nai_bufzone_large_s nai_bufzone_large_t;
typedef struct nai_bufzone_s nai_bufzone_t;
typedef struct nai_buflock_s nai_buflock_t;


struct nai_bufzone_entry_s {
    nai_bufzone_entry_t* next;
#if (NAI_BUFCHECK_ENTRY)
    nai_bufzone_entry_t* copy;
#endif
};


struct nai_bufzone_large_s {
    nai_list_entry_t ent;
    nai_bufzone_entry_t data;
};


struct nai_bufzone_s {
    nai_pool_t* pool;
    nai_spin_t* lock;
    uint32_t own:1;
    size_t size;
    size_t used;
    size_t total;
    nai_list_entry_t large;
    nai_bufzone_entry_t* ents[NAI_BUFZONE_ENTRIES];
};


struct nai_buflock_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_spin_t lock;
    uint8_t stat[NAI_BUFZONE_LOCKS];
    nai_spin_t subl[NAI_BUFZONE_LOCKS];
};


static nai_int_t nai_bufzone_index(size_t len, size_t* alloc)
{
    uint32_t index;

    /* 
     * entries
     * 0.  0     --  1
     * 1.  32
     * 2.  64
     * 3.  96
     * 4.  128
     * 5.  160
     * 6.  192
     * 7.  224   --  7
     * 8.  256
     * 9.  512
     * 10. 768   --  3
     * 11. 1024
     * 12. 2048
     * 13. 3072  --  3
     * 14. 4096
     * 15. 8192
     * 16. 12288
     * 17. 16384 --  4
     */
    len += !len;
    if (len <= (8 - 1) * 32) {               //  7 slot,  32 - 224
        index = (uint32_t)((len +  31) >> 5) - 1 + 1;
        alloc[0] = ((index + 1 - 1) << 5);

    } else if (len <= ((4 - 1) * 256)) {     //  3 slot, 256 - 768
        index = (uint32_t)((len + 255) >> 8) - 1 + 8;
        alloc[0] = ((index + 1 - 8) << 8);

    } else if (len <= ((4 - 1) * 1024)) {    //  3 slot,  1k -  3k
        index = (uint32_t)((len + 1023) >> 10) - 1 + 11;
        alloc[0] = ((index + 1 - 11) << 10);

    } else if (len <= ((4 - 0) * 4096)) {    //  4 slot,  4k - 16k
        index = (uint32_t)((len + 4095) >> 12) - 1 + 14;
        alloc[0] = ((index + 1 - 14) << 12);
    } else {
        index = NAI_BUFZONE_ENTRIES;
        alloc[0] = len;
    };

    return index;
};


#if (NAI_BUFCHECK_PADDING)

static uint8_t nai_buf_padding[8] = { 
    0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad
};

#endif


static void* nai_bufzone_xalloc(void* u, size_t size)
{
    size_t alloc;
    uint32_t index;
    nai_bufzone_t* p = (nai_bufzone_t*)u;
    nai_bufzone_entry_t* e;
    nai_bufzone_large_t* l;


#if (NAI_BUFCHECK_PADDING)
    size += sizeof(nai_buf_padding); /* add padding space */
#endif

    if (size > p->size) {
        size += nai_offsetof(nai_bufzone_large_t, data);
        l = (nai_bufzone_large_t*)nai_malloc(size);
        if (l == 0) {
            e = 0;
            goto _end;
        } else {
            nai_list_insert_tail(&p->large, &l->ent);
            p->used += size;
            p->total += size;
            e = &l->data;
            size -= nai_offsetof(nai_bufzone_large_t, data);
            goto _ok;
        };
    };


    index = nai_bufzone_index(size, &alloc);

    if (p->ents[index]) {
        e = (nai_bufzone_entry_t*)p->ents[index];
#if (NAI_BUFCHECK_ENTRY)
        if (e->next != e->copy) {
            assert(0);
            nai_log_error(NAI_LOG_CORE, 0, 
                "the buffer %p of entry %d is overflow", e, index);
        };
#endif
        p->ents[index] = e->next;
    } else {
        e = (nai_bufzone_entry_t*)nai_palloc(p->pool, alloc);
        if (e == 0) {
            goto _end;
        };

        p->total += alloc;
    };

    p->used += alloc;

_ok:

#if (NAI_BUFCHECK_PADDING)
    /* fill padding */
    nai_memcpy((uint8_t*)e + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding));
#endif

_end:
    return e;
};


static void* nai_bufzone_mt_xalloc(void* u, size_t size)
{
    nai_int_t ec;
    size_t alloc;
    uint32_t index;
    nai_bufzone_t* p = (nai_bufzone_t*)u;
    nai_bufzone_entry_t* e;
    nai_bufzone_large_t* l;


    assert(p->lock);

#if (NAI_BUFCHECK_PADDING)
    size += sizeof(nai_buf_padding); /* add padding space */
#endif

    if (size > p->size) {
        size += nai_offsetof(nai_bufzone_large_t, data);
        l = (nai_bufzone_large_t*)nai_malloc(size);
        if (l == 0) {
            e = 0;
            goto _end;
        } else {
            nai_spin_lock(p->lock);
            nai_list_insert_tail(&p->large, &l->ent);
            p->used += size;
            p->total += size;
            e = &l->data;
            size -= nai_offsetof(nai_bufzone_large_t, data);
            goto _ok;
        };
    };


    index = nai_bufzone_index(size, &alloc);

    /* lock */
    nai_spin_lock(p->lock);

    /* find and alloc entry */
    if (p->ents[index]) {
        e = (nai_bufzone_entry_t*)p->ents[index];
#if (NAI_BUFCHECK_ENTRY)
        if (e->next != e->copy) {
            assert(0);
            nai_log_error(NAI_LOG_CORE, 0, 
                "the buffer %p of entry %d is overflow", e, index);
        };
#endif
        p->ents[index] = e->next;
    } else {
        e = (nai_bufzone_entry_t*)nai_palloc(p->pool, alloc);
        if (e == 0) {
            ec = nai_errno;
            nai_spin_unlock(p->lock);
            nai_errno = ec;
            goto _end;
        };

        p->total += alloc;
    };

    p->used += alloc;

_ok:
    /* unlock */
    nai_spin_unlock(p->lock);

#if (NAI_BUFCHECK_PADDING)
    /* fill padding */
    nai_memcpy((uint8_t*)e + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding));
#endif

_end:
    return e;
};


static nai_int_t nai_bufzone_xfree(void* u, void* m, size_t size)
{
    size_t alloc;
    uint32_t index;
    nai_bufzone_t* p = (nai_bufzone_t*)u;
    nai_bufzone_entry_t* e;
    nai_bufzone_large_t* l;


#if (NAI_BUFCHECK_PADDING)
    size += sizeof(nai_buf_padding); /* add padding space */

    if (nai_memcmp((uint8_t*)m + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding)) != 0) {
        assert(0);
        nai_log_error(NAI_LOG_CORE, 0, 
            "the padding of buffer %p is overflow", m);
    };
#endif

    if (size > p->size) {
        size += nai_offsetof(nai_bufzone_large_t, data);
        l = nai_containof(m, nai_bufzone_large_t, data);

        /* remove from list */
        nai_list_entry_remove(&l->ent);
        p->used -= size;
        p->total -= size;

        /* free large memory */
        nai_free(l);
        goto _end;
    };

    /* get zone index */
    index = nai_bufzone_index(size, &alloc);

    /* put into zone */
    e = (nai_bufzone_entry_t*)m;
    e->next = p->ents[index];
#if (NAI_BUFCHECK_ENTRY)
    e->copy = e->next; /* for check overflow */
#endif
    p->ents[index] = e;
    p->used -= alloc;

_end:
    return 0;
};


static nai_int_t nai_bufzone_mt_xfree(void* u, void* m, size_t size)
{
    size_t alloc;
    uint32_t index;
    nai_bufzone_t* p = (nai_bufzone_t*)u;
    nai_bufzone_entry_t* e;
    nai_bufzone_large_t* l;


    assert(p->lock);

#if (NAI_BUFCHECK_PADDING)
    size += sizeof(nai_buf_padding); /* add padding space */

    if (nai_memcmp((uint8_t*)m + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding)) != 0) {
        assert(0);
        nai_log_error(NAI_LOG_CORE, 0, 
            "the padding of buffer %p is overflow", m);
    };
#endif

    if (size > p->size) {
        size += nai_offsetof(nai_bufzone_large_t, data);
        l = nai_containof(m, nai_bufzone_large_t, data);

        /* lock */
        nai_spin_lock(p->lock);

        /* remove from list */
        nai_list_entry_remove(&l->ent);
        p->used -= size;
        p->total -= size;

        /* unlock */
        nai_spin_unlock(p->lock);

        /* free large memory */
        nai_free(l);
        goto _end;
    };

    /* get zone index */
    index = nai_bufzone_index(size, &alloc);

    /* lock */
    nai_spin_lock(p->lock);

    /* put into zone */
    e = (nai_bufzone_entry_t*)m;
    e->next = p->ents[index];
#if (NAI_BUFCHECK_ENTRY)
    e->copy = e->next; /* for check overflow */
#endif
    p->ents[index] = e;
    p->used -= alloc;

    /* unlock */
    nai_spin_unlock(p->lock);

_end:
    return 0;
};


static void* nai_bufzone_alloc(void* u, size_t size)
{
    void* r;
    size_t* c;


    c = nai_bufzone_xalloc(u, size + sizeof(*c));
    if (c == 0) {
        r = 0;
        goto _end;
    };

    c[0] = size;
    r = c + 1;

_end:
    return r;
};


static void* nai_bufzone_mt_alloc(void* u, size_t size)
{
    void* r;
    size_t* c;


    c = nai_bufzone_mt_xalloc(u, size + sizeof(*c));
    if (c == 0) {
        r = 0;
        goto _end;
    };

    c[0] = size;
    r = c + 1;

_end:
    return r;
};


static nai_int_t nai_bufzone_free(void* u, void* m)
{
    nai_int_t r;
    size_t* c;


    c = (size_t*)m - 1;
    r = nai_bufzone_xfree(u, c, c[0] + sizeof(*c));

    return r;
};


static nai_int_t nai_bufzone_mt_free(void* u, void* m)
{
    nai_int_t r;
    size_t* c;


    c = (size_t*)m - 1;
    r = nai_bufzone_mt_xfree(u, c, c[0] + sizeof(*c));

    return r;
};


static nai_int_t nai_bufzone_close(void* u)
{
    nai_pool_t stack;
    nai_list_entry_t* e;
    nai_bufzone_t* b = (nai_bufzone_t*)u;
    nai_bufzone_large_t* l;


    e = b->large.next;
    for ( ; e != &b->large; ) {
        l = (nai_bufzone_large_t*)e;
        e = e->next;
        nai_free(l);
    };

    nai_list_init(&b->large);

    if (b->own) {
        nai_pool_init(&stack, 0);
        nai_pool_exchange(&stack, b->pool);
        nai_pool_close(&stack);
    };

    return 0;
};


static void nai_bufzone_cleanup(void* u)
{
    nai_bufzone_t* b = (nai_bufzone_t*)u;
    b->own = 0;
    nai_bufzone_close(u);
};



static nai_bufpool_ops_t nai_zoneops = {
    "bufzone", 
    nai_bufzone_alloc, 
    nai_bufzone_free, 
    nai_bufzone_xalloc, 
    nai_bufzone_xfree,
    nai_bufzone_xalloc, 
    nai_bufzone_xfree,
    nai_bufzone_close
};

static nai_bufpool_ops_t nai_zoneops_mt = {
    "bufzone-mt", 
    nai_bufzone_mt_alloc, 
    nai_bufzone_mt_free, 
    nai_bufzone_mt_xalloc, 
    nai_bufzone_mt_xfree,
    nai_bufzone_mt_xalloc, 
    nai_bufzone_mt_xfree,
    nai_bufzone_close
};


static nai_buflock_t nai_buflock = { 0, 0, NAI_ONCE_INIT };


static void nai_buflock_init()
{
    nai_int_t r;

    r = nai_spin_open(&nai_buflock.lock, 0);
    if (r < 0) {
        nai_buflock.error = nai_errno;
    };

    nai_memory_barrier();
    nai_buflock.inited = 1;
    return;
};


static void nai_buflock_term()
{
    nai_int_t n;

    if (nai_buflock.inited) {
        for (n = 0; n < (nai_int_t)nai_countof(nai_buflock.subl); n ++) {
            if (nai_buflock.stat[n]) {
                nai_spin_close(&nai_buflock.subl[n]);
                nai_buflock.stat[n] = 0;
            };
        };

        nai_spin_close(&nai_buflock.lock);
    };

    return;
};


static nai_spin_t* nai_buflock_at(void* k)
{
    nai_int_t r;
    nai_int_t ec;
    intptr_t index;
    nai_spin_t* l;
    nai_buflock_t* m;


    m = &nai_buflock;
    if (m->inited == 0) {
        nai_once(&m->once, nai_buflock_init);
    };
    if (m->error) {
        nai_errno = m->error;
        l = 0;
        goto _end;
    };


    r = 0;
    index = (intptr_t)k;
    index >>= 4;
    index %= nai_countof(m->stat);

    if (m->stat[index] == 0) {

        /* lock manage */
        nai_spin_lock(&m->lock);

        /* initialize lock */
        if (m->stat[index] == 0) {
            r = nai_spin_open(&m->subl[index], 0);
            if (r >= 0) {
                m->stat[index] = 1;
            } else {
                ec = nai_errno;
            };
        };

        /* unlock manage */
        nai_spin_unlock(&m->lock);
    };



    if (r >= 0) {
        l = &m->subl[index];
    } else {
        nai_errno = ec;
        l = 0;
    };


_end:
    return l;
};


static nai_int_t nai_bufpool_open_impl(
    nai_bufpool_t* p, nai_pool_t* share, size_t size, nai_int_t mt)
{
    nai_int_t r;
    nai_int_t ec;
    nai_pool_t stack;
    nai_pool_t* pool;
    nai_spin_t* lock;
    nai_bufzone_t* b;


    pool = share;
    if (pool == 0) {
        nai_pool_init(&stack, size);

        /* allocate own pool */
        pool = (nai_pool_t*)nai_palloc(&stack, sizeof(*pool));
        if (pool == 0) {
            r = -1;
            goto _end;
        };

        nai_pool_init(pool, 0);
        nai_pool_exchange(pool, &stack);
    };


    /* get buffer lock */
    if (mt == 0) {
        lock = 0;
    } else {
        lock = nai_buflock_at(pool);
        if (lock == 0) {
            r = -1;
            goto _end;
        };
    };


    /* alloc bufzone */
    b = (nai_bufzone_t*)nai_palloc(pool, sizeof(*b));
    if (b == 0) {
        r = -1;
        goto _end;
    };


    /* initialize bufzone */
    nai_list_init(&b->large);
    nai_pzero((void*)b->ents, nai_countof(b->ents));
    b->pool = pool;
    b->lock = lock;
    b->own = pool != share;
    b->total = 0;
    b->used = 0;
    b->size = pool->size - sizeof(nai_list_entry_t) - sizeof(size_t) * 2;
    if (b->size > NAI_BUFZONE_MAXSIZE) {
        b->size = NAI_BUFZONE_MAXSIZE;
    };


    /* add bufzone cleanup */
    r = nai_pool_add_cleanup(pool, nai_bufzone_cleanup, b);
    if (r < 0) {
        goto _end;
    };


    /* setup bufpool */
    if (lock == 0) {
        p->ops = &nai_zoneops;
    } else {
        p->ops = &nai_zoneops_mt;
    };
    p->pool = b;
    r = 0;

_end:
    if (r < 0) {
        if (pool && pool != share) {
            ec = nai_errno;
            nai_pool_exchange(&stack, pool);
            nai_pool_close(&stack);
            nai_errno = ec;
        };
    };
    return r;
};


nai_int_t nai_bufpool_open(
    nai_bufpool_t* p, size_t size, nai_int_t mt)
{
    return nai_bufpool_open_impl(p, 0, size, mt);
};


nai_int_t nai_bufpool_from(
    nai_bufpool_t* p, nai_pool_t* share, nai_int_t mt)
{
    return nai_bufpool_open_impl(p, share, 0, mt);
};


nai_int_t nai_bufpool_close(nai_bufpool_t* p)
{
    nai_int_t r;

    if (p->pool) {
        if (p->ops) {
            r = p->ops->close(p->pool);
            if (r < 0) {
                goto _end;
            };
        };
        p->pool = 0;
    };

    r = 0;

_end:
    return r;
};


void nai_bufpool_term()
{
    nai_buflock_term();
    return;
};


