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
/// @file       nai_pool.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_pool.h"

#include "nai/os/nai_system.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"

//////////////////////////////////////////////////////////////////////////////
// Memory pool

#define NAI_POOL_PAGE_SIZE    (nai_pagesize)
#define NAI_POOL_DEFAULT_SIZE (16 * 1024)
#define NAI_POOL_MAX_SIZE     (64 * 1024)
#define NAI_POOL_MAX_ALLOC    (4 * 1024 - sizeof(nai_pool_entry_t))
#define NAI_POOL_ALIGNMENT    (sizeof(void*) * 2)

#ifndef _NAI_TYPEDEF_POOL_ENTRY_T
    #define _NAI_TYPEDEF_POOL_ENTRY_T
typedef struct nai_pool_entry_s nai_pool_entry_t;
#endif
#ifndef _NAI_TYPEDEF_POOL_CLEANUP_T
    #define _NAI_TYPEDEF_POOL_CLEANUP_T
typedef struct nai_pool_cleanup_s nai_pool_cleanup_t;
#endif

struct nai_pool_entry_s
{
    nai_list_entry_t ent;
    size_t size;
    size_t used;
};

struct nai_pool_cleanup_s
{
    nai_pool_cleanup_t* next;
    nai_pool_cleanup_f cb;
    void* data;
};

#define nai_pool_align(x, a) (((x) + (a)-1) / (a) * (a))

nai_int_t nai_pool_init(nai_pool_t* p, size_t size)
{
    if (size <= 0) {
        size = NAI_POOL_DEFAULT_SIZE;
    } else if (size > NAI_POOL_MAX_SIZE) {
        size = NAI_POOL_MAX_SIZE;
    };

    nai_list_init(&p->mem);
    p->clean  = 0;
    p->parent = 0;
    p->used   = 0;
    p->total  = 0;
    p->size   = size;
    return 0;
};

nai_int_t nai_pool_from(nai_pool_t* p, nai_pool_t* q)
{
    if (q && q->parent) {
        q = q->parent;
    };

    nai_list_init(&p->mem);
    p->clean  = 0;
    p->parent = q;
    p->used   = 0;
    p->total  = 0;
    return 0;
};

nai_int_t nai_pool_free_last(nai_pool_t* p, void* m, size_t len)
{
    nai_pool_entry_t* e;

    e = (nai_pool_entry_t*)p->mem.next;
    if (e != (nai_pool_entry_t*)&p->mem) {
        len = nai_pool_align(len, NAI_POOL_ALIGNMENT);
        if (e->used >= sizeof(*e) + len && (uint8_t*)(e) + e->used == (uint8_t*)(m) + len) {
            e->used -= len;
        };
    };

    return 0;
};

void* nai_palloc(nai_pool_t* p, size_t len)
{
    nai_int_t ec;
    void* m;
    size_t left;
    size_t alloc;
    nai_pool_t* q;
    nai_pool_entry_t* e;
    nai_pool_entry_t* n;

    /* large alloc */
    len = nai_pool_align(len, NAI_POOL_ALIGNMENT);
    if (len > p->size - sizeof(*e)) {
        m = nai_malloc(len);
        if (m != 0) {
            if (nai_pool_add_cleanup(p, nai_free, m) < 0) {
                ec = nai_errno;
                nai_free(m);
                nai_errno = ec;
                m         = 0;
            };
        };

        goto _end;
    };

    /* small alloc
     * the first half of the linked list is the entry which is used
     * the second half of the linked list is the entry which is empty
     */
    e = (nai_pool_entry_t*)p->mem.next;
    while (e != (nai_pool_entry_t*)&p->mem) {
        /* check if there is enough space */
        left = e->size - e->used;
        if (left >= len) {
            goto _ok;
        };

        /* try find a larger one */
        /* if (e->ent.next == &p->mem) { */
        /*     break; */
        /* }; */
        /* n = (nai_pool_entry_t*)e->ent.next; */
        /* if (left >= (n->size - n->used)) { */
        /*     break; */
        /* }; */

        /* sort by left in reverse order */
        n = e;
        do {
            n = (nai_pool_entry_t*)n->ent.next;
            if (n == (nai_pool_entry_t*)&p->mem) {
                break;
            };
            if (n->used == sizeof(*n)) {
                /* a empty entry, don't sort with used one */
                break;
            };

        } while (left < (n->size - n->used));

        if (n != (nai_pool_entry_t*)e->ent.next) {
            nai_list_entry_remove(&e->ent);
            nai_list_insert_before(&n->ent, &e->ent);

            /* we try find a larger entry, check again */
            e    = (nai_pool_entry_t*)p->mem.next;
            left = e->size - e->used;
            if (left >= len) {
                goto _ok;
            };
        };

        /* we try find a empty one from the last of linked list */
        e    = (nai_pool_entry_t*)p->mem.prev;
        left = e->size - e->used;
        if (left >= len) {
            nai_list_entry_remove(&e->ent);
            nai_list_insert_head(&p->mem, &e->ent);
            goto _ok;
        };

        /* no enough space */
        break;
    };

    /* count alloc size */
    alloc = len + sizeof(*e);
    if (alloc <= p->size) {
        alloc = p->size;
    } else if (alloc < NAI_POOL_MAX_SIZE) { /* old code, never reach here */
        alloc = nai_pool_align(alloc, NAI_POOL_PAGE_SIZE);
    };

    /* is child pool, try lookup parent's pages */
    if (p->parent) {
        q = p->parent;
        e = (nai_pool_entry_t*)q->mem.prev;
        for (;;) {
            if (e == (nai_pool_entry_t*)&q->mem) {
                break;
            };
            if (e->used > 0) {
                break;
            };
            if (e->size < alloc) {
                break;
            };

            /* remove from parent */
            q->used -= sizeof(*e);
            q->total -= e->size;
            nai_list_entry_remove(&e->ent);

            /* insert into child */
            p->used += sizeof(*e);
            p->total += e->size;
            nai_list_insert_head(&p->mem, &e->ent);
            goto _ok;
        };
    };

    /* alloc new entry */
    e = (nai_pool_entry_t*)nai_malloc(alloc);
    if (e == 0) {
        m = 0;
        goto _end;
    };

    e->size = alloc;
    e->used = sizeof(*e);
    p->used += sizeof(*e);
    p->total += alloc;

    /* add head */
    nai_list_insert_head(&p->mem, &e->ent);

_ok:
    m = (uint8_t*)(e) + e->used;
    e->used += len;
    p->used += len;

_end:
    return m;
};

nai_int_t nai_pool_close_impl(nai_pool_t* p, nai_int_t reset)
{
    nai_pool_t* q;
    nai_pool_entry_t* m;
    nai_pool_cleanup_t* c;
    nai_list_entry_t* e;
    nai_list_entry_t* r;
    nai_list_entry_t list;

    c = p->clean;
    while (c != 0) {
        if (c->cb) {
            c->cb(c->data);
        };
        c = c->next;
    };

    e = p->mem.next;
    if (reset) {
        p->clean = 0;
        p->used  = 0;
        while (e != &p->mem) {
            m = (nai_pool_entry_t*)e;
            e = e->next;
            if ((uint8_t*)p >= (uint8_t*)m && (uint8_t*)p < (uint8_t*)m + m->size) {
                m->used = (uint8_t*)p - (uint8_t*)m + sizeof(*p);
            } else {
                m->used = sizeof(*m);
            };
            p->used += m->used;
        };
    } else if (p->parent == 0) {
        /* the pool may allocated itself, 
         * excahnge mem list from pool to stack before free 
         **/
        if (e != &p->mem) {
            list.next       = p->mem.next;
            list.prev       = p->mem.prev;
            list.next->prev = &list;
            list.prev->next = &list;
            nai_pool_init(p, 0);

            /* free mem list */
            while (e != &list) {
                r = e;
                e = e->next;
                nai_free(r);
            };
        };
    } else {
        q = p->parent;
        while (e != &p->mem) {
            m       = (nai_pool_entry_t*)e;
            e       = e->next;
            m->used = sizeof(*m);
            q->used += m->used;
            q->total += m->size;
            nai_list_entry_remove(&m->ent);
            nai_list_insert_tail(&q->mem, &m->ent);
        };
        nai_pool_init(p, 0);
    };

    return 0;
};

nai_int_t nai_pool_close(nai_pool_t* p) { return nai_pool_close_impl(p, 0); };

nai_int_t nai_pool_reset(nai_pool_t* p) { return nai_pool_close_impl(p, 1); };

nai_int_t nai_pool_exchange(nai_pool_t* p, nai_pool_t* s)
{
    size_t tmp;
    nai_list_entry_t* ptr;
    nai_pool_cleanup_t* tmpc;

    tmp      = p->total;
    p->total = s->total;
    s->total = tmp;

    tmp     = p->used;
    p->used = s->used;
    s->used = tmp;

    tmp     = p->size;
    p->size = s->size;
    s->size = tmp;

    tmpc     = p->clean;
    p->clean = s->clean;
    s->clean = tmpc;

    if (nai_list_is_empty(&p->mem) && nai_list_is_empty(&s->mem)) {
        ;  // do nothing
    } else if (nai_list_is_empty(&p->mem)) {
        p->mem.next       = s->mem.next;
        p->mem.prev       = s->mem.prev;
        p->mem.next->prev = &p->mem;
        p->mem.prev->next = &p->mem;
        nai_list_init(&s->mem);
    } else if (nai_list_is_empty(&s->mem)) {
        s->mem.next       = p->mem.next;
        s->mem.prev       = p->mem.prev;
        s->mem.next->prev = &s->mem;
        s->mem.prev->next = &s->mem;
        nai_list_init(&p->mem);
    } else {
        ptr         = p->mem.next;
        p->mem.next = s->mem.next;
        s->mem.next = ptr;

        ptr         = p->mem.prev;
        p->mem.prev = s->mem.prev;
        s->mem.prev = ptr;

        p->mem.next->prev = &p->mem;
        p->mem.prev->next = &p->mem;

        s->mem.next->prev = &s->mem;
        s->mem.prev->next = &s->mem;
    };

    return 0;
};

nai_int_t nai_pool_add_cleanup(nai_pool_t* p, nai_pool_cleanup_f cb, void* data)
{
    nai_int_t r;
    nai_pool_cleanup_t* c;

    c = (nai_pool_cleanup_t*)nai_palloc(p, sizeof(*c));
    if (c == 0) {
        r = -1;
        goto _end;
    };

    c->cb    = cb;
    c->data  = data;
    c->next  = p->clean;
    p->clean = c;
    r        = 0;

_end:
    return r;
};

    //////////////////////////////////////////////////////////////////////////////
    // Fixed-size memory pool

#define NAI_ALLOC_ALIGNMENT (sizeof(void*) * 2)

typedef struct nai_fixedchunk_t nai_fixedchunk_t;

struct nai_fixedchunk_t
{
    nai_list_entry_t ent;
    uint8_t* cur;
    uint8_t* end;
};

nai_int_t nai_fixedpool_init(nai_fixedpool_t* p, size_t size, size_t alignment, nai_pool_t* pool)
{
    nai_int_t r;

    if (alignment < NAI_ALLOC_ALIGNMENT) {
        alignment = NAI_ALLOC_ALIGNMENT;
    };

    nai_list_init(&p->chunks);
    p->freed     = 0;
    p->pool      = pool;
    p->alignment = alignment;
    p->size      = nai_align(size, p->alignment);
    r            = 0;

    return r;
};

nai_int_t nai_fixedpool_close(nai_fixedpool_t* p)
{
    nai_int_t r;
    nai_list_entry_t* e;
    nai_fixedchunk_t* c;

    if (p->pool == 0) {
        e = p->chunks.next;
        for (; e != &p->chunks;) {
            c = (nai_fixedchunk_t*)e;
            e = e->next;
            nai_list_entry_remove(&c->ent);
            nai_free(c);
        };
    };

    nai_list_init(&p->chunks);
    p->freed = 0;
    r        = 0;

    return r;
};

nai_int_t nai_fixedpool_reset(nai_fixedpool_t* p)
{
    nai_int_t r;
    uint8_t* start;
    nai_list_entry_t* e;
    nai_fixedchunk_t* c;

    // reclaim entry
    e = p->chunks.next;
    for (; e != &p->chunks;) {
        c      = (nai_fixedchunk_t*)e;
        e      = e->next;
        start  = (uint8_t*)(c + 1);
        start  = (uint8_t*)nai_align((uintptr_t)start, p->alignment);
        c->cur = start;
        c->end = start + p->size * 16;
    };

    // init freed list
    p->freed = 0;
    r        = 0;

    return r;
};

void* nai_fixedpool_alloc(nai_fixedpool_t* p)
{
    intptr_t alloc;
    uint8_t* start;
    nai_list_entry_t* e;
    nai_fixedchunk_t* c;

    if (p->freed != 0) {
        e        = p->freed;
        p->freed = e->next;
    } else {
        /* find a non-empty chunk */
        e = p->chunks.next;
        c = (nai_fixedchunk_t*)e;
        if (e == &p->chunks || c->cur >= c->end) {
            alloc = (nai_align(sizeof(*c), p->alignment) + p->size * 16);
            if (p->alignment > NAI_ALLOC_ALIGNMENT) {
                alloc += p->alignment - NAI_ALLOC_ALIGNMENT;
            };

            c = (nai_fixedchunk_t*)(p->pool ? nai_palloc(p->pool, alloc) : nai_malloc(alloc));
            if (c == 0) {
                e = 0;
                goto _end;
            };

            start  = (uint8_t*)(c + 1);
            start  = (uint8_t*)nai_align((uintptr_t)start, p->alignment);
            c->cur = start;
            c->end = start + p->size * 16;
            nai_list_insert_head(&p->chunks, &c->ent);
        };

        /* alloc */
        e = (nai_list_entry_t*)c->cur;
        c->cur += p->size;
        if (c->cur >= c->end) {
            nai_list_entry_remove(&c->ent);
            nai_list_insert_tail(&p->chunks, &c->ent);
        };
    };

_end:
    return e;
};

nai_int_t nai_fixedpool_free(nai_fixedpool_t* p, void* ptr)
{
    nai_list_entry_t* e = (nai_list_entry_t*)ptr;
    e->next             = p->freed;
    p->freed            = e->next;
    return 0;
};
