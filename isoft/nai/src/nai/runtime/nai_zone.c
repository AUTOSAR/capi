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
/// @file       nai_zone.c
/// @brief      
/// @details
/// @date       2022-06-21
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_zone.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_thread.h"


#define NAI_PAGESIZE            (4*1024)
#define NAI_HALFSIZE            (2*1024)
#define NAI_HUGESIZE            (64*1024)


#define NAI_ZONE_TYPE_PAGE      0
#define NAI_ZONE_TYPE_SMALL     1
#define NAI_ZONE_TYPE_EXACT     2
#define NAI_ZONE_TYPE_BIG       3
#define NAI_ZONE_TYPE_MASK      3


#ifndef _NAI_TYPEDEF_ZONE_ELEM_T
#define _NAI_TYPEDEF_ZONE_ELEM_T
typedef struct nai_zone_elem_s nai_zone_elem_t;
#endif
#ifndef _NAI_TYPEDEF_ZONE_SEGM_T
#define _NAI_TYPEDEF_ZONE_SEGM_T
typedef struct nai_zone_segm_s nai_zone_segm_t;
#endif
#ifndef _NAI_TYPEDEF_ZONE_PAGE_T
#define _NAI_TYPEDEF_ZONE_PAGE_T
typedef struct nai_zone_page_s nai_zone_page_t;
#endif


struct nai_zone_elem_s {
    uint8_t* nptr;              /**< pointer to the unused element */
    union {
        nai_zone_elem_t* list;  /**< pointer to the free list */
        nai_zone_elem_t* next;  /**< pointer to the next element */
    };
};


struct nai_zone_segm_s {

    union {
        nai_rbnode_t node;      /**< the node of segment map */
        nai_list_entry_t ent;   /**< the entry of free segment list */
    };

    uint8_t* start;             /**< pointer to the start of memory */
    uint8_t* end;               /**< pointer to the end of memory */

    size_t pused;               /**< the count of allocated pages */
    size_t ptotal;              /**< the total pages */

    nai_zone_page_t* pstart;    /**< pointer to the start of pages */
    nai_zone_page_t* pend;      /**< pointer to the end of pages */

    int16_t segs;               /**< the count of allocated segments */
    uint16_t master:1;          /**< is master segment which include the zone */
    size_t size;                /**< the size of memory */
    void* orig;                 /**< pointer to the original memory address */
};


struct nai_zone_page_s {
    nai_list_entry_t ent;       /**< the entry of page list */
    nai_zone_segm_t* segm;      /**< pointer to the segment */
    union {
        uint64_t mask;          /**< exact info, 
                                     the bits of allocated elements */
        struct {                /**< small info */
            uint64_t freed:59;  /**< the count of free elements */
            /* uint64_t index:5; */
        };
        struct {                /**< big or large info */
            uint64_t maskl:59;  /**< the bits of allocated elements */
            uint64_t index:5;   /**< the index of slot table */
        };
        struct {                /**< page info */
            uint64_t count:63;  /**< the count of allocated pages */
            uint64_t alloc:1;   /**< is allocated */
        };
    };
};


#define nai_zone_page_mem(s, p)                         \
    ((s)->start + (((p) - (s)->pstart)) * NAI_PAGESIZE) \


#define nai_zone_is_page(p)                             \
    (nai_zone_get_type(p) == NAI_ZONE_TYPE_PAGE)        \


#define nai_zone_get_type(p)                            \
    ((intptr_t)(p)->prev & NAI_ZONE_TYPE_MASK)          \


#define nai_zone_get_prev(p)                            \
    ((nai_list_entry_t*)(                               \
        (intptr_t)(p)->prev & ~NAI_ZONE_TYPE_MASK))     \


#define nai_zone_set_prev_ptr(p, v, t)                  \
    ((p)->prev = (nai_list_entry_t*)                    \
        (((intptr_t)(v) & ~NAI_ZONE_TYPE_MASK) | (t)))  \


#define nai_zone_set_type(p, t)                         \
    nai_zone_set_prev_ptr(p, (p)->prev, t)              \


#define nai_zone_set_prev(p, v)                         \
    nai_zone_set_prev_ptr(p,                            \
        (v), (intptr_t)(p)->prev & NAI_ZONE_TYPE_MASK)  \



static nai_rbnode_t** nai_zone_find_segment(
    nai_zone_t* p, uint8_t* ptr, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->smap);
    nai_rbnode_t* parent = nai_rbtree_end(&p->smap);
    nai_zone_segm_t* c;


    while (*n) {
        parent = *n;
        c = nai_containof(parent, nai_zone_segm_t, node);
        if (c->start > ptr) {
            n = &parent->rb_left;
        } else if (c->end <= ptr) {
            n = &parent->rb_right;
        } else {
            break;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_int_t nai_zone_init_segment_pages(nai_zone_segm_t* s)
{
    nai_int_t r;
    intptr_t n;
    intptr_t count;
    nai_zone_page_t* start;


    count = s->ptotal;
    start = s->pstart;
    start->count = count;
    start->alloc = 0;
    start->segm = s;
    for (n = 1; n < count; n ++) {
        start[n].segm = s;
        start[n].mask = 0;
        start[n].ent.next = 0;
        start[n].ent.prev = 0;
    };

    r = 0;

    return r;
};


static nai_int_t nai_zone_plist_index(size_t len)
{
    uint32_t index;


#if 1
    /* 
     * entries
     * 0.  1    --  1
     * 1.  4
     * 2.  8
     * 3.  12   --  3
     * 4.  16
     * 5.  32
     * 6.  48   --  3
     * 7.  64
     * 8.  128
     * 9.  192  --  3
     * 10. 256
     * 11. 512
     * 12. 768  --  3
     * 13. 1024
     * 14. 2048 --  2
     * 15. ----
     */

    if (len <= 1) {
        index = 0;
    } else if (len <= ((4 - 1) * 4)) {      //  3 slot,   4 - 12
        index = (uint32_t)((len +  3) >> 2) - 1 + 1;
    } else if (len <= ((4 - 1) * 16)) {     //  3 slot,  16 - 48
        index = (uint32_t)((len + 15) >> 4) - 1 + 4;
    } else if (len <= ((4 - 1) * 64)) {     //  3 slot,  64 - 192
        index = (uint32_t)((len + 63) >> 6) - 1 + 7;
    } else if (len <= ((4 - 1) * 256)) {    //  3 slot, 256 - 768
        index = (uint32_t)((len + 255) >> 8) - 1 + 10;
    } else if (len <= ((3 - 1) * 1024)) {   //  2 slot,  1k - 2k
        index = (uint32_t)((len + 1023) >> 10) - 1 + 13;
    } else {
        index = 15;
    };
#else
    size_t m;

    m = 1;
    for (index = 0; index < 15; index ++) {
        if (len <= m) {
            break;
        };
        m <<= 1;
    };
#endif

    return index;
};


static nai_int_t nai_zone_insert_pages(nai_zone_t* p, nai_zone_page_t* page)
{
    nai_int_t r;
    nai_int_t n;


    n = nai_zone_plist_index((size_t)page->count);
    nai_list_insert_tail(&p->plist[n], &page->ent);
    r = 0;

    return r;
};


static nai_zone_page_t* nai_zone_alloc_page(nai_zone_t* p, size_t count)
{
    nai_int_t n;
    nai_int_t l;
    intptr_t left;
    nai_list_t* list;
    nai_list_entry_t* e;
    nai_zone_segm_t* s;
    nai_zone_page_t* r;
    nai_zone_page_t* page;


    n = nai_zone_plist_index(count);
    for ( ; n < (nai_int_t)nai_countof(p->plist); n ++) {

        list = &p->plist[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            page = nai_containof(e, nai_zone_page_t, ent);

            /* test space is enough */
            left = (size_t)page->count - count;
            if (left < 0) {
                continue;
            };

            /* pick pages */
            if (left == 0) {
                nai_list_entry_remove(&page->ent);
                r = page;
            } else {
                if (left > 1) {
                    page[left-1].ent.prev = &page->ent;
                };
                page->count = left;
                r = page + left;

                /* adjust the position of the remaining pages */
                l = nai_zone_plist_index((size_t)page->count);
                if (l != n) {
                    nai_list_entry_remove(&page->ent);
                    nai_list_insert_tail(&p->plist[l], &page->ent);
                };
            };

            /* fill stats of pages */
            r->alloc = 1;
            r->count = count;
            if (count > 1) {
                r[count-1].mask = -1;
            };

            s = r->segm;
            if (s->pused <= 0 && !s->master) {
                p->sempty --;
            };
            s->pused += count;
            goto _end;
        };
    };

    nai_errno = ENOMEM;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_zone_free_page(
    nai_zone_t* p, nai_zone_page_t* ptr, size_t count)
{
    nai_int_t r;
    nai_int_t n, l;
    nai_int_t ncon;
    nai_int_t pcon;
    nai_zone_segm_t* s;
    nai_zone_page_t* page;
    nai_zone_page_t* next;
    nai_zone_page_t* prev;


    page = ptr;
    page->alloc = 0;
    page->count = count;

    if (count > 1) {
        page[count-1].mask = 0;
        page[count-1].ent.prev = 0;
    };

    s = page->segm;
    next = page + page->count;
    prev = page - 1;
    ncon = (next < s->pend && nai_zone_is_page(&next->ent) && !next->alloc);
    pcon = (prev >= s->pstart && nai_zone_is_page(&prev->ent) && !prev->alloc);
    if (pcon) {

        if (ncon) {
            nai_list_entry_remove(&next->ent);
            page->count += next->count;
            next->count = 0;
        };

        page->ent.prev = 0;

        next = page;
        if (prev->count) {
            page = prev;
        } else {
            page = nai_containof(prev->ent.prev, nai_zone_page_t, ent);
        };

        n = nai_zone_plist_index((size_t)page->count);
        page->count += next->count;
        page[page->count-1].ent.prev = &page->ent;
        next->count = 0;

    } else if (ncon) {

        n = nai_zone_plist_index((size_t)next->count);
        nai_list_insert_before(&next->ent, &page->ent);
        nai_list_entry_remove(&next->ent);
        page->count += next->count;
        page[page->count-1].ent.prev = &page->ent;
        next->count = 0;

    } else {

        n = -1;
        if (page->count > 1) {
            page[page->count-1].ent.prev = &page->ent;
        };
    };

    l = nai_zone_plist_index((size_t)page->count);
    if (l != n) {
        if (n >= 0) {
            nai_list_entry_remove(&page->ent);
        };

        /* bigger after merging, move to tail */
        nai_list_insert_tail(&p->plist[l], &page->ent);
    };

    s->pused -= count;
    if (s->pused <= 0 && !s->master) {
        p->sempty ++;
    };

    r = 0;

    return r;
};


#define k(x)                ((x)*1024)


static nai_int_t nai_zone_page_index;
static nai_int_t nai_zone_exact_index;
static size_t nai_zone_exact_size;


static nai_int_t nai_zone_size[27] = {
       16,    32,    64,    96,   128,   160,   192,   224, 
      256,   512,   768,  1024,  2048,  3072,  4096,  k(6), 
     k(8), k(10), k(12), k(14), k(16), k(24), k(32), k(40), 
    k(48), k(56), k(64), 
};
static nai_int_t nai_zone_shift[27] = {
        4,     5,     6,     0,     7,     0,     0,     0, 
        8,     9,     0,    10,    11,     0,    12,     0, 
       13,     0,     0,     0,    14,     0,    15,     0, 
        0,     0,    16, 
};
static nai_int_t nai_zone_count[nai_countof(nai_zone_size)] = { 0 };


static nai_int_t nai_zone_slot_index(size_t len, size_t* alloc)
{
    uint32_t index;

    /* 
     * entries
     * 0.  16    --  1
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
     * 14. 4k
     * 15. 6k
     * 16. 8k
     * 17. 10k
     * 18. 12k
     * 19. 14k   --  6
     * 20. 16k
     * 21. 24k
     * 22. 32k
     * 23. 40k
     * 24. 48k
     * 25. 56k
     * 26. 64k   --  7
     */

    if (len <= 16) {
        index = 0;
        alloc[0] = 16;

    } else if (len <= (8 - 1) * 32) {        //  7 slot,  32 - 224
        index = (uint32_t)((len +  31) >> 5) - 1 + 1;
        alloc[0] = ((index + 1 - 1) << 5);

    } else if (len <= ((4 - 1) * 256)) {     //  3 slot, 256 - 768
        index = (uint32_t)((len + 255) >> 8) - 1 + 8;
        alloc[0] = ((index + 1 - 8) << 8);

    } else if (len <= ((4 - 1) * 1024)) {    //  3 slot,  1k - 3k
        index = (uint32_t)((len + 1023) >> 10) - 1 + 11;
        alloc[0] = ((index + 1 - 11) << 10);

    } else if (len <= ((8 - 1) * 2048)) {    //  6 slot,  4k - 14k
        index = (uint32_t)((len + 2047) >> 11) - 2 + 14;
        if (index == 19) {
            index = 20;     /* loss is same: 64/14 == 64/16 */
        };
        alloc[0] = ((index + 2 - 14) << 11);

    } else if (len <= ((8 - 0) * 8192)) {    //  7 slot, 16k - 64k
        index = (uint32_t)((len + 8191) >> 13) - 2 + 20;
        if (index == 25) {
            index = 26;     /* loss is same: 256/56 == 256/64 */
        };
        alloc[0] = ((index + 2 - 20) << 13);

    } else {
        assert(0);
        index = -1;
        alloc[0] = 0;
    };

    return index;
};


static nai_int_t nai_zone_init_sizes()
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t m;


    for (n = 0; n < (intptr_t)nai_countof(nai_zone_size); n ++) {
        if ((NAI_PAGESIZE / nai_zone_size[n]) == (sizeof(uint64_t) * 8)) {
            break;
        };
    };
    assert(n < (intptr_t)nai_countof(nai_zone_size));

    for (m = 0; m < (intptr_t)nai_countof(nai_zone_size); m ++) {
        if (m < n) {
            nai_zone_count[m] = (NAI_PAGESIZE - 
                sizeof(nai_zone_elem_t)) / nai_zone_size[m];
        } else if (nai_zone_size[m] <= NAI_HALFSIZE) {
            nai_zone_count[m] = (NAI_PAGESIZE) / nai_zone_size[m];
        } else if (nai_zone_size[m] <= NAI_HUGESIZE / 4) {
            nai_zone_count[m] = (NAI_HUGESIZE) / nai_zone_size[m];
        } else {
            nai_zone_count[m] = (NAI_HUGESIZE * 4) / nai_zone_size[m];
        };

        if (nai_zone_size[m] == NAI_PAGESIZE) {
            nai_zone_page_index = m;
        };
    };

    nai_zone_exact_size = nai_zone_size[n];
    nai_memory_barrier();
    nai_zone_exact_index = n;
    r = 0;

    return r;
};


nai_zone_t* nai_zone_from(
    void* ptr, size_t psize, nai_int_t type, size_t fsize)
{
    nai_int_t r;
    intptr_t n;
    intptr_t nslot;
    size_t used;
    nai_zone_t* p;
    nai_zone_segm_t* s;
    nai_zone_slot_t* slots;


    if (nai_zone_exact_index == 0) {
        nai_zone_init_sizes();
    };


    switch (type) {
    case NAI_ZONE_MEMORY:
    case NAI_ZONE_DEVICE:
        break;
    default:
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };

    if ((intptr_t)ptr & (sizeof(void*)-1)) {
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };

    if (fsize <= 0) {
        fsize = NAI_HUGESIZE / 4;
    } else if (fsize > NAI_HUGESIZE) {
        fsize = NAI_HUGESIZE;
    } else {
        /* nothing */
        ;
    };

    p = (nai_zone_t*)ptr;

    /* checking */
    used = sizeof(*p);
    if (psize < used) {
        nai_errno = ENOMEM;
        p = 0;
        goto _end;
    };

    /* initialize */
    p->type = type;
    p->used = 0;
    p->page = 0;
    p->fsize = fsize;
    p->sempty = 0;
    p->segs = 0;
    p->sptr = 0;
    p->start = 0;
    p->end = 0;
    nai_list_init(&p->slist);

    /* initialize stat */
    if (p->type == NAI_ZONE_MEMORY) {
        p->loss = used;
        p->total = used;
    } else {
        p->loss = 0;
        p->total = 0;
    };

    /* initialize page lists */
    for (n = 0; n < (intptr_t)nai_countof(p->plist); n ++) {
        nai_list_init(&p->plist[n]);
    };

    /* initialize slots */
    nai_static_assert(nai_countof(p->slots) == nai_countof(nai_zone_size));
    slots = p->slots;
    nslot = nai_countof(p->slots);
    for (n = 0; n < nslot; n ++) {
        nai_list_init(&slots[n].part);
        nai_list_init(&slots[n].full);
        slots[n].npart = 0;
        slots[n].nfull = 0;
        slots[n].loss = 0;
        slots[n].used = 0;
    };

    /* add first segment */
    if (p->type == NAI_ZONE_MEMORY) {
        r = nai_zone_offer(p, (uint8_t*)p + used, psize - used);
        if (r < 0) {
            if (nai_errno != ENOMEM) {
                p = 0;
                goto _end;
            };
        };
    };

    /* reset orig pointer */
    if (p->sptr) {
        p->sempty --;
        s = (nai_zone_segm_t*)p->sptr;
        s->orig = ptr;
        s->size = psize;
        s->master = 1;
    };

_end:
    return p;
};


nai_int_t nai_zone_init(nai_zone_t* p)
{
    nai_int_t r;
    nai_zone_t* n;


    n = nai_zone_from(p, sizeof(*p), NAI_ZONE_MEMORY, 0);
    assert(n != 0);
    (void)n;

    r = 0;

    return r;
};


nai_int_t nai_zone_open(nai_zone_t* p, nai_int_t type, size_t fsize)
{
    nai_int_t r;


    switch (type) {
    case NAI_ZONE_MEMORY:
    case NAI_ZONE_DEVICE:
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (fsize <= 0) {
        fsize = NAI_HUGESIZE / 4;
    } else if (fsize > NAI_HUGESIZE) {
        fsize = NAI_HUGESIZE;
    } else {
        /* nothing */
        ;
    };

    if (p->segs > 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    p->type = type;
    p->fsize = fsize;
    r = 0;

_end:
    return r;
};


nai_int_t nai_zone_reset(nai_zone_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_rbnode_t* e;
    nai_zone_segm_t* s;


    /* reset slots */
    for (n = 0; n < (intptr_t)nai_countof(nai_zone_size); n ++) {
        p->loss -= p->slots[n].loss;
        p->slots[n].loss = 0;
        p->slots[n].used = 0;
        nai_list_init(&p->slots[n].part);
        nai_list_init(&p->slots[n].full);
    };

    /* reset page list */
    for (n = 0; n < (intptr_t)nai_countof(p->plist); n ++) {
        nai_list_init(&p->plist[n]);
    };

    /* reset segments */
    if (p->segs == 1) {
        s = (nai_zone_segm_t*)p->sptr;
        s->pused = 0;
        nai_zone_init_segment_pages(s);
        nai_zone_insert_pages(p, s->pstart);
    } else if (p->segs > 1) {
        e = nai_rbtree_begin(&p->smap);
        for ( ; e != nai_rbtree_end(&p->smap); ) {
            s = (nai_zone_segm_t*)e;
            e = nai_rbtree_next(e);
            s->pused = 0;
            nai_zone_init_segment_pages(s);
            nai_zone_insert_pages(p, s->pstart);
        };
    } else {
        /* nothing */
        ;
    };

    /* reset counts */
    p->used = 0;
    r = 0;

    return r;
};


nai_int_t nai_zone_clean(nai_zone_t* p, nai_zone_clean_f cb, void* ud)
{
    nai_int_t r;
    nai_int_t ec;
    void* ptr;
    size_t size;


    while (1) {
        ptr = nai_zone_eject(p, &size);
        if (ptr != 0) {
            if (cb) {
                r = cb(ud, ptr, size);
                if (r < 0) {
                    goto _end;
                };
            };
            continue;
        };

        ec = nai_errno;
        if (ec != ENOENT) {
            r = -1;
            goto _end;
        };
        break;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_zone_offer(nai_zone_t* p, void* ptr, size_t size)
{
    nai_int_t r;
    nai_int_t m;
    int16_t segs;
    size_t count;
    size_t used;
    size_t align;
    void* segp;
    uint8_t* start;
    uint8_t* end;
    nai_zone_segm_t* l;
    nai_zone_segm_t* s;
    nai_zone_page_t* pages;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    if (p->type == NAI_ZONE_MEMORY) {
        /* align with 32 */
        align = (0-(intptr_t)ptr) & (31);

        /* check size */
        used = align;
        if (nai_list_is_empty(&p->slist)) {
            if (size < used + sizeof(*s) * 1) {
                nai_errno = ENOMEM;
                r = -1;
                goto _end;
            };

            used += sizeof(*s) * 1;
            segp = (uint8_t*)ptr + align;
            segs = 1;
        } else {
            segp = 0;
            segs = 0;
        };

        /* align with 32 */
        used += (0-((intptr_t)ptr+used)) & (31);

        /* get space of page meta */
        pages = (nai_zone_page_t*)((uint8_t*)ptr + used);
        count = (size - used) / (sizeof(*pages) + NAI_PAGESIZE);
        used += sizeof(*pages) * count;

        /* do align */
        start = (uint8_t*)ptr + used;
        start = (uint8_t*)nai_align((intptr_t)start, NAI_PAGESIZE);
        end = (uint8_t*)ptr + size;
        if (start + NAI_PAGESIZE > end) {
            nai_errno = ENOMEM;
            r = -1;
            goto _end;
        };

        /* count pages */
        count = (end - start) / NAI_PAGESIZE;
        end = start + count * NAI_PAGESIZE;

    } else {

        /* do align */
        start = (uint8_t*)ptr;
        start = (uint8_t*)nai_align((intptr_t)start, NAI_PAGESIZE);
        end = (uint8_t*)ptr + size;
        if (start + NAI_PAGESIZE > end) {
            nai_errno = ENOMEM;
            r = -1;
            goto _end;
        };

        /* count pages */
        count = (end - start) / NAI_PAGESIZE;
        end = start + count * NAI_PAGESIZE;

        /* allocate */
        used = sizeof(*s) + sizeof(*pages) * count + 32;
        segp = nai_malloc(used);
        if (segp == 0) {
            r = -1;
            goto _end;
        };

        /* align with 32 */
        used = sizeof(*s);
        used += (0-((intptr_t)segp+used)) & (31);

        pages = (nai_zone_page_t*)((uint8_t*)segp + used);
        segs = 1;
    };


    /* find exists */
    if (p->segs <= 0) {
        /* to disable warning for gcc */
        n = 0;
        parent = 0;
    } else if (p->segs > 1) {
        n = nai_zone_find_segment(p, start, &parent);
        if (n[0] != 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    } else {
        if ((start <= p->start && end >= p->end) || 
            (start >= p->start && end <= p->end) || 
            (start <= p->start && end > p->start) || 
            (start < p->end && end >= p->end)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        n = 0;
        parent = 0;
    };

    /* get segment */
    if (!nai_list_is_empty(&p->slist)) {
        s = (nai_zone_segm_t*)p->slist.next;
        nai_list_entry_remove(&s->ent);
    } else {
        s = (nai_zone_segm_t*)segp;
        for (m = 1; m < segs; m ++) {
            nai_list_insert_tail(&p->slist, &s[m].ent);
        };
    };

    /* initialize segment */
    s->master = 0;
    s->segs = segs;
    s->size = size;
    s->orig = ptr;
    s->start = start;
    s->end = end;
    s->pused = 0;
    s->ptotal = count;
    s->pstart = pages;
    s->pend = pages + count;

    /* add pages */
    nai_zone_init_segment_pages(s);
    nai_zone_insert_pages(p, s->pstart);

    /* add segment */
    if (p->segs <= 0) {
        /* only one segment, do not use rbtree */
        p->sptr = s;
        p->start = start;
        p->end = end;
    } else {
        if (p->segs == 1) {
            l = (nai_zone_segm_t*)p->sptr;
            nai_rbtree_init(&p->smap);

            /* insert exist segment */
            n = nai_zone_find_segment(p, l->start, &parent);
            assert(n[0] == 0);
            nai_rbtree_link(&p->smap, &l->node, parent, n);
            nai_rbtree_color(&p->smap, &l->node);

            n = nai_zone_find_segment(p, start, &parent);
            assert(n[0] == 0);
        };

        /* insert segment */
        nai_rbtree_link(&p->smap, &s->node, parent, n);
        nai_rbtree_color(&p->smap, &s->node);
    };

    p->sempty ++;
    p->segs ++;
    p->loss += size - count * NAI_PAGESIZE;
    p->total += size;
    r = 0;

_end:
    return r;
};


void* nai_zone_eject(nai_zone_t* p, size_t* psize)
{
    void* r;
    nai_int_t unused;
    nai_rbnode_t* e;
    nai_list_entry_t* n;
    nai_zone_segm_t* s;
    nai_zone_segm_t* m;


    if (p->segs <= 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };
    if (p->segs == 1) {
        s = (nai_zone_segm_t*)p->sptr;
        if (s->master) {
            nai_errno = ENOENT;
            r = 0;
            goto _end;
        };
        if (s->pused) {
            nai_errno = EAGAIN;
            r = 0;
            goto _end;
        };

        goto _detach;
    };

    if (p->sempty <= 0) {
        nai_errno = EAGAIN;
        r = 0;
        goto _end;
    };

    /* find an unused segment */
    e = nai_rbtree_begin(&p->smap);
    for ( ; e != nai_rbtree_end(&p->smap); ) {
        s = (nai_zone_segm_t*)e;
        e = nai_rbtree_next(e);
        if (s->pused) {
            continue;
        };
        if (s->master) {
            continue;
        };

        assert(s->pstart->alloc == 0);
        assert(s->pstart->count == s->ptotal);

        if (s->segs > 1) {
            unused = 1;

            /* check segments are inused */
            n = p->slist.next;
            for ( ; n != &p->slist; ) {
                m = (nai_zone_segm_t*)n;
                n = n->next;
                if ((uint8_t*)m >= (uint8_t*)s->orig && 
                    (uint8_t*)m < (uint8_t*)s->end) {
                    unused ++;
                };
            };

            if (unused < s->segs) {
                /* some node in use */
                continue;
            };

            n = p->slist.next;
            for ( ; n != &p->slist; ) {
                m = (nai_zone_segm_t*)n;
                n = n->next;
                if ((uint8_t*)m >= (uint8_t*)s->orig && 
                    (uint8_t*)m < (uint8_t*)s->end) {
                    nai_list_entry_remove(&m->ent);
                };
            };
        };

        goto _detach;
    };

    nai_errno = EAGAIN;
    r = 0;

_end:
    return r;

_detach:
    p->sempty --;
    p->segs --;
    p->loss -= s->size - s->ptotal * NAI_PAGESIZE;
    p->total -= s->size;
    nai_list_entry_remove(&s->pstart->ent);

    /**
     * rbtree is enabled when there are multiple segments, 
     * and does not need to be removed when the last segment
     */
    if (p->segs > 0) {
        nai_rbtree_erase(&p->smap, &s->node);
    };

    r = s->orig;
    if (psize) {
        psize[0] = s->size;
    };

    /* free entry */
    if (p->type == NAI_ZONE_DEVICE) {
        assert(s->segs == 1);
        nai_free(s);
    } else if (s->segs <= 0) {
        /* the segment is allocated from other memory */
        nai_list_insert_tail(&p->slist, &s->ent);
    } else {
        /* nothing */
        ;
    };

    /* less than two, don't use rbtree */
    if (p->segs == 0) {
        p->sptr = 0;
        p->start = 0;
        p->end = 0;
    } else if (p->segs == 1) {
        s = (nai_zone_segm_t*)nai_rbtree_root(&p->smap);
        p->sptr = s;
        p->start = s->start;
        p->end = s->end;
    } else {
        /* nothing */
        ;
    };

    goto _end;
};


void* nai_zone_map_segment(nai_zone_t* p, const void* ptr)
{
    void* r;
    nai_rbnode_t** n;
    nai_zone_segm_t* s;


    if (p->segs > 1) {
        n = nai_zone_find_segment(p, (void*)ptr, 0);
        if (n[0] == 0) {
            nai_errno = EINVAL;
            r = 0;
            goto _end;
        };

        s = (nai_zone_segm_t*)n[0];
    } else {
        if ((uint8_t*)ptr < p->start || 
            (uint8_t*)ptr >= p->end) {
            nai_errno = EINVAL;
            r = 0;
            goto _end;
        };

        s = (nai_zone_segm_t*)p->sptr;
    };

    r = s->start;

_end:
    return r;
};


static const uint8_t nai_msb_lut[256] = {
    0, 0, 1, 1, 2, 2, 2, 2, // 0000_0000 - 0000_0111
    3, 3, 3, 3, 3, 3, 3, 3, // 0000_1000 - 0000_1111
    4, 4, 4, 4, 4, 4, 4, 4, // 0001_0000 - 0001_0111
    4, 4, 4, 4, 4, 4, 4, 4, // 0001_1000 - 0001_1111
    5, 5, 5, 5, 5, 5, 5, 5, // 0010_0000 - 0010_0111
    5, 5, 5, 5, 5, 5, 5, 5, // 0010_1000 - 0010_1111
    5, 5, 5, 5, 5, 5, 5, 5, // 0011_0000 - 0011_0111
    5, 5, 5, 5, 5, 5, 5, 5, // 0011_1000 - 0011_1111

    6, 6, 6, 6, 6, 6, 6, 6, // 0100_0000 - 0100_0111
    6, 6, 6, 6, 6, 6, 6, 6, // 0100_1000 - 0100_1111
    6, 6, 6, 6, 6, 6, 6, 6, // 0101_0000 - 0101_0111
    6, 6, 6, 6, 6, 6, 6, 6, // 0101_1000 - 0101_1111
    6, 6, 6, 6, 6, 6, 6, 6, // 0110_0000 - 0110_0111
    6, 6, 6, 6, 6, 6, 6, 6, // 0110_1000 - 0110_1111
    6, 6, 6, 6, 6, 6, 6, 6, // 0111_0000 - 0111_0111
    6, 6, 6, 6, 6, 6, 6, 6, // 0111_1000 - 0111_1111

    7, 7, 7, 7, 7, 7, 7, 7, // 1000_0000 - 1000_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1000_1000 - 1000_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1001_0000 - 1001_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1001_1000 - 1001_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1010_0000 - 1010_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1010_1000 - 1010_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1011_0000 - 1011_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1011_1000 - 1011_1111

    7, 7, 7, 7, 7, 7, 7, 7, // 1100_0000 - 1100_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1100_1000 - 1100_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1101_0000 - 1101_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1101_1000 - 1101_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1110_0000 - 1110_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1110_1000 - 1110_1111
    7, 7, 7, 7, 7, 7, 7, 7, // 1111_0000 - 1111_0111
    7, 7, 7, 7, 7, 7, 7, 7, // 1111_1000 - 1111_1111
};


static void* nai_zalloc_large(nai_zone_t* p, size_t size)
{
    void* r;
    intptr_t n;
    nai_zone_segm_t* s;
    nai_zone_page_t* page;


    n = nai_align(size, NAI_PAGESIZE) / NAI_PAGESIZE;
    page = nai_zone_alloc_page(p, n);
    if (page == 0) {
        r = 0;
    } else {
        s = page->segm;
        r = nai_zone_page_mem(s, page);
        p->used += n * NAI_PAGESIZE;
        p->page += n * NAI_PAGESIZE;
    };

    return r;
};


void* nai_zalloc(nai_zone_t* p, size_t size)
{
    void* r;
    nai_int_t full;
    nai_int_t index;
    intptr_t n, b;
    intptr_t count;
    uint64_t m;
    uint64_t mask;
    uint64_t bits;
    uint16_t value;
    size_t used;
    size_t alloc;
    nai_list_entry_t* prev;
    nai_list_entry_t* next;
    nai_zone_segm_t* s;
    nai_zone_slot_t* t;
    nai_zone_page_t* page;
    nai_zone_elem_t* elem;
    nai_zone_elem_t* free;


    assert(p->fsize <= NAI_HUGESIZE);
    if (size > p->fsize || size > NAI_HUGESIZE) {
        r = nai_zalloc_large(p, size);
        goto _end;
    };

    if (p->type == NAI_ZONE_DEVICE && size <= nai_zone_exact_size) {
        index = nai_zone_exact_index;
        alloc = nai_zone_exact_size;
    } else {
        index = nai_zone_slot_index(size, &alloc);
        assert(index >= 0);
    };

    t = &p->slots[index];
    if (!nai_list_is_empty(&t->part)) {
        page = (nai_zone_page_t*)t->part.next;
        s = page->segm;
    } else {
        if (size <= NAI_HALFSIZE) {
            used = NAI_PAGESIZE;
        } else if (size <= NAI_HUGESIZE / 4) {
            used = NAI_HUGESIZE;
        } else {
            used = NAI_HUGESIZE * 4;
        };

        page = nai_zone_alloc_page(p, used / NAI_PAGESIZE);
        if (page == 0) {
            if (used > NAI_PAGESIZE) {
                r = nai_zalloc_large(p, size);
            } else {
                r = 0;
            };
            goto _end;
        };

        used -= nai_zone_size[index] * nai_zone_count[index];
        p->loss += used;
        t->loss += used;
        t->npart ++;

        s = page->segm;
        if (index < nai_zone_exact_index) {

            nai_list_insert_head(&t->part, &page->ent);
            nai_zone_set_type(&page->ent, NAI_ZONE_TYPE_SMALL);
            page->freed = nai_zone_count[index];
            page->index = index;
            elem = (nai_zone_elem_t*)nai_zone_page_mem(s, page);
            elem->nptr = (uint8_t*)elem + (NAI_PAGESIZE - page->freed * alloc);
            elem->list = 0;

        } else if (index == nai_zone_exact_index) {

            nai_list_insert_head(&t->part, &page->ent);
            nai_zone_set_type(&page->ent, NAI_ZONE_TYPE_EXACT);
            page->mask = 0;

        } else if (alloc <= NAI_HALFSIZE) {

            nai_list_insert_head(&t->part, &page->ent);
            nai_zone_set_type(&page->ent, NAI_ZONE_TYPE_BIG);
            page->maskl = 0;
            page->index = index;

        } else {

            nai_list_insert_head(&t->part, &page->ent);
            nai_zone_set_type(&page->ent, NAI_ZONE_TYPE_BIG);
            count = (size_t)page->count;
            page->maskl = 0;
            page->index = index;
            for (n = 1; n < count; n ++) {
                page[n].mask = 0;
                page[n].ent.prev = &page->ent;
                nai_zone_set_type(&page[n].ent, NAI_ZONE_TYPE_BIG);
            };
        };
    };

    switch (nai_zone_get_type(&page->ent)) {
    case NAI_ZONE_TYPE_SMALL:
        assert(p->type == NAI_ZONE_MEMORY);
        elem = (nai_zone_elem_t*)nai_zone_page_mem(s, page);
        assert(page->freed > 0);

        if (elem->list) {
            free = elem->list;
            r = (uint8_t*)free - (alloc - sizeof(*elem));
            if (alloc & (alloc-1)) {
                if (((0-(uintptr_t)r) & (NAI_PAGESIZE-1)) % alloc) {
                    /* wrong */
                    nai_log_crit(NAI_LOG_CORE, EFAULT, 
                        "the elem list of the page(%d, 0x%p) is corrupted, "
                        "next=%p is unaligned", 
                        (nai_int_t)(page - s->pstart), page, r);
                    nai_errno = EFAULT;
                    r = 0;
                    goto _end;
                };
            } else {
                if ((uintptr_t)r & (alloc-1)) {
                    /* wrong */
                    nai_log_crit(NAI_LOG_CORE, EFAULT, 
                        "the elem list of the page(%d, 0x%p) is corrupted, "
                        "next=%p is unaligned", 
                        (nai_int_t)(page - s->pstart), page, r);
                    nai_errno = EFAULT;
                    r = 0;
                    goto _end;
                };
            };

            elem->list = free->next;
        } else {
            assert(elem->nptr + alloc <= (uint8_t*)elem + NAI_PAGESIZE);
            r = elem->nptr;
            elem->nptr += alloc;
        };

        page->freed --;
        full = page->freed <= 0;
        break;

    case NAI_ZONE_TYPE_EXACT:
        count = (intptr_t)sizeof(page->mask) * 8;
        n = count - 1;
        b = nai_aligndown(n, 16);
        bits = ~page->mask;
        if (bits == 0) {
            nai_log_crit(NAI_LOG_CORE, EFAULT, 
                "the metadata of the page(%d, 0x%p) is corrupted, "
                "mask=%" NAI_INT64_FMT "x", 
                (nai_int_t)(page - s->pstart), page, page->mask);
            nai_errno = EFAULT;
            r = 0;
            goto _end;
        };

        if ((size_t)(bits >> 32) == 0) {
            b -= 32;
        };

        mask = (uint64_t)0xffff << b;
        for (;;) {
            if (mask & bits) {
                break;
            };

            b -= 16;
            mask >>= 16;
        };

        value = (uint16_t)(bits >> b);
        assert(value != 0);

        /* test high byte */
        if (value >> 8) {
            value >>= 8;
            b += 8;
        };
        n = nai_msb_lut[value] + b;
        m = (uint64_t)1 << n;

        assert(n >= 0);
        r = nai_zone_page_mem(s, page) + n * alloc;

        page->mask |= m;
        full = page->mask == (uint64_t)-1;
        break;

    case NAI_ZONE_TYPE_BIG:
        count = nai_zone_count[page->index];
        n = count - 1;
        b = nai_aligndown(n, 16);
        bits = ~page->maskl;
        bits &= ((uint64_t)2 << n) - 1;
        if (bits == 0) {
            nai_log_crit(NAI_LOG_CORE, EFAULT, 
                "the metadata of the page(%d, 0x%p) is corrupted, "
                "maskl=%" NAI_INT64_FMT "x", 
                (nai_int_t)(page - s->pstart), page, (uint64_t)page->maskl);
            nai_errno = EFAULT;
            r = 0;
            goto _end;
        };

        if (b > 0) {
            mask = (uint64_t)0xffff << b;
            for (;;) {
                if (mask & bits) {
                    break;
                };

                b -= 16;
                mask >>= 16;
            };

            value = (uint16_t)(bits >> b);
        } else {
            value = (uint16_t)(bits);
        };
        assert(value != 0);

        /* test high byte */
        if (value >> 8) {
            value >>= 8;
            b += 8;
        };
        n = nai_msb_lut[value] + b;
        m = (uint64_t)1 << n;

        assert(n >= 0);
        r = nai_zone_page_mem(s, page) + n * alloc;

        page->maskl |= m;
        m -= 1;
        full = (page->maskl & m) == m;
        break;

    case NAI_ZONE_TYPE_PAGE:
        /* wrong page */
        nai_log_crit(NAI_LOG_CORE, EFAULT, 
            "the metadata of the page(%d, 0x%p) is corrupted, "
            "mask=%" NAI_INT64_FMT "x", 
            (nai_int_t)(page - s->pstart), page, page->mask);
        nai_errno = EFAULT;
        r = 0;
        goto _end;

    default:
        assert(0);
        nai_errno = ENOMEM;
        r = 0;
        goto _end;
    };

    t->used += alloc;
    p->used += alloc;

    /* become to full page */
    if (full) {
        /* move from part */
        prev = nai_zone_get_prev(&page->ent);
        prev->next = page->ent.next;
        next = page->ent.next;
        nai_zone_set_prev(next, page->ent.prev);

        /* move to tail of full */
        prev = t->full.prev;
        prev->next = &page->ent;
        t->full.prev = &page->ent;
        page->ent.next = &t->full;
        nai_zone_set_prev(&page->ent, prev);

        /* update statistics */
        t->npart --;
        t->nfull ++;
    };


_end:
    return r;
};


nai_int_t nai_zfree(nai_zone_t* p, void* ptr)
{
    nai_int_t r;
    nai_int_t part;
    nai_int_t none;
    nai_int_t shift;
    nai_int_t index;
    intptr_t i;
    intptr_t m;
    intptr_t count;
    size_t max;
    size_t offs;
    size_t size;
    size_t used;
    nai_rbnode_t** n;
    nai_list_entry_t* prev;
    nai_list_entry_t* next;
    nai_zone_segm_t* s;
    nai_zone_slot_t* t;
    nai_zone_page_t* page;
    nai_zone_elem_t* elem;
    nai_zone_elem_t* free;


    if (p->segs > 1) {
        n = nai_zone_find_segment(p, ptr, 0);
        if (n[0] == 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        s = (nai_zone_segm_t*)n[0];
    } else {
        if ((uint8_t*)ptr < p->start || 
            (uint8_t*)ptr >= p->end) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        s = (nai_zone_segm_t*)p->sptr;
    };

    i = ((uint8_t*)ptr - s->start) / NAI_PAGESIZE;
    page = &s->pstart[i];

    switch (nai_zone_get_type(&page->ent)) {
    case NAI_ZONE_TYPE_SMALL:
        assert(p->type == NAI_ZONE_MEMORY);
        index = (nai_int_t)page->index;
        size = nai_zone_size[index];
        if (size & (size-1)) {
            if (((0-(uintptr_t)ptr) & (NAI_PAGESIZE-1)) % size) {
                /* bad pointer */
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        } else {
            if ((uintptr_t)ptr & (size-1)) {
                /* bad pointer */
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        };

        page->freed ++;
        part = page->freed == 1;
        none = page->freed == (size_t)nai_zone_count[index];

        elem = (nai_zone_elem_t*)nai_zone_page_mem(s, page);
        free = (nai_zone_elem_t*)((uint8_t*)ptr + (size - sizeof(*elem)));
        free->next = elem->list;
        elem->list = free;
        break;

    case NAI_ZONE_TYPE_EXACT:
        index = nai_zone_exact_index;
        size = nai_zone_size[index];
        offs = (uintptr_t)ptr & (NAI_PAGESIZE-1);
        if (offs & (size-1)) {
            /* bad pointer */
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        shift = nai_zone_shift[nai_zone_exact_index];
        m = offs >> shift;
        if (!(page->mask & ((uint64_t)1 << m))) {
            /* double free */
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        part = page->mask == (uint64_t)-1;
        page->mask &= ~((uint64_t)1 << m);
        none = page->mask == 0;
        break;

    case NAI_ZONE_TYPE_BIG:
        if (page->index != 0 && page->index <= nai_zone_page_index) {
            index = (nai_int_t)page->index;
            size = nai_zone_size[index];
            offs = (uintptr_t)ptr & (NAI_PAGESIZE-1);
        } else {
            if (page->index == 0) {
                page = (nai_zone_page_t*)nai_zone_get_prev(&page->ent);
            };

            index = (nai_int_t)page->index;
            size = nai_zone_size[index];
            offs = ((uint8_t*)ptr - nai_zone_page_mem(s, page));
            if (size <= NAI_HUGESIZE / 4) {
                offs = offs & (NAI_HUGESIZE - 1);
            } else {
                offs = offs & (NAI_HUGESIZE * 4 - 1);
            };
        };

        if (size & (size-1)) {
            if (offs % size) {
                /* bad pointer */
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            m = offs / size;
        } else {
            if (offs & (size-1)) {
                /* bad pointer */
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            shift = nai_zone_shift[index];
            m = offs >> shift;
        };

        if (!(page->maskl & ((uint64_t)1<<m))) {
            /* double free */
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        max = nai_zone_count[index];
        part = page->maskl == (((uint64_t)1 << max) - 1);
        page->maskl &= ~((uint64_t)1 << m);
        none = page->maskl == 0;
        break;

    case NAI_ZONE_TYPE_PAGE:
        if (!page->alloc) {
            /* double free */
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        count = (size_t)page->count;
        if (page + count > s->pend || 
            page + count < page) {
            if (page->mask == (uint64_t)-1) {
                /* bad pointer */
                nai_errno = EINVAL;
                r = -1;
            } else {
                /* wrong */
                nai_log_crit(NAI_LOG_CORE, EFAULT, 
                    "the metadata of the page(%d, 0x%p) is corrupted, "
                    "count=%" NAI_INT64_FMT "d", 
                    (nai_int_t)(page - s->pstart), page, (uint64_t)count);
                nai_errno = EFAULT;
                r = -1;
            };
            goto _end;
        };

#if defined(_DEBUG)
        /* checking */
        for (m = 1; m < (intptr_t)count; m ++) {
            if (!nai_zone_is_page(&page[m].ent)) {
                /* wrong */
                nai_log_crit(NAI_LOG_CORE, EFAULT, 
                    "the metadata of the page(%d, 0x%p) is corrupted, "
                    "type=%" NAI_INT64_FMT "d", 
                    (nai_int_t)(page - s->pstart) + m, page + m, 
                    (nai_int_t)nai_zone_get_type(&page[m].ent));
                nai_errno = EFAULT;
                r = -1;
                goto _end;
            };

            if (!((page[m].mask == (uint64_t)0 && m != (intptr_t)count-1)) && 
                !((page[m].mask == (uint64_t)-1 && m == (intptr_t)count-1))) {
                /* wrong */
                nai_log_crit(NAI_LOG_CORE, EFAULT, 
                    "the metadata of the page(%d, 0x%p) is corrupted, "
                    "mask=0x%" NAI_INT64_FMT "x", 
                    (nai_int_t)(page - s->pstart) + m, page + m, page[m].mask);
                nai_errno = EFAULT;
                r = -1;
                goto _end;
            };
        };
#endif

        r = nai_zone_free_page(p, page, count);
        if (r < 0) {
            nai_log_crit(NAI_LOG_CORE, nai_errno, 
                "nai_zone_free_page(0x%p, %d) failed", 
                page, (nai_int_t)page->count);
            goto _end;
        };

        p->used -= count * NAI_PAGESIZE;
        p->page -= count * NAI_PAGESIZE;
        r = 0;
        goto _end;

    default:
        assert(0);
        nai_errno = EFAULT;
        r = -1;
        goto _end;
    };

    t = &p->slots[index];
    t->used -= size;
    p->used -= size;


    /* become to none page */
    if (none) {
        /* move from part or full */
        prev = nai_zone_get_prev(&page->ent);
        prev->next = page->ent.next;
        next = page->ent.next;
        nai_zone_set_prev(next, page->ent.prev);

        if (size <= NAI_HALFSIZE) {
            used = NAI_PAGESIZE;
        } else if (size <= NAI_HUGESIZE / 4) {
            used = NAI_HUGESIZE;
        } else {
            used = NAI_HUGESIZE * 4;
        };
        count = used / NAI_PAGESIZE;
        used -= nai_zone_size[index] * nai_zone_count[index];

        /* update statistics */
        p->loss -= used;
        t->loss -= used;
        t->npart --;


#if defined(_DEBUG)
        /* checking */
        for (m = 1; m < (intptr_t)count; m ++) {
            if (nai_zone_get_type(&page[m].ent) != NAI_ZONE_TYPE_BIG) {
                /* wrong */
                nai_log_crit(NAI_LOG_CORE, EFAULT, 
                    "the metadata of the page(%d, 0x%p) is corrupted, "
                    "type=%" NAI_INT64_FMT "d", 
                    (nai_int_t)(page - s->pstart) + m, page + m, 
                    (nai_int_t)nai_zone_get_type(&page[m].ent));
                nai_errno = EFAULT;
                r = -1;
                goto _end;
            };
        };
#endif

        /* clear mask of pages */
        for (m = 1; m < count; m ++) {
            page[m].mask = 0;
            page[m].ent.prev = 0;
        };

        /* free page */
        r = nai_zone_free_page(p, page, count);
        if (r < 0) {
            nai_log_crit(NAI_LOG_CORE, nai_errno, 
                "nai_zone_free_page(0x%p, %d) failed", 
                page, (nai_int_t)count);
            goto _end;
        };

    /* become to part page */
    } else if (part) {
        /* move from full */
        prev = nai_zone_get_prev(&page->ent);
        prev->next = page->ent.next;
        next = page->ent.next;
        nai_zone_set_prev(next, page->ent.prev);

        /* move to tail of part */
        prev = t->part.prev;
        prev->next = &page->ent;
        t->part.prev = &page->ent;
        page->ent.next = &t->part;
        nai_zone_set_prev(&page->ent, prev);

        /* update statistics */
        t->nfull --;
        t->npart ++;

    } else {
        /* nothing */
        ;
    };

    r = 0;

_end:
    return r;
};


