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
/// @file       npc_bufpool.c
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "npc/shm/npc_bufpool.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



#define NPC_BUFZONE_ENTRIES             (18)
#define NPC_BUFZONE_MAXSIZE             (16*1024)


#if !defined(NPC_BUFCHECK_ENTRY)
#if !defined(_DEBUG)
#define NPC_BUFCHECK_ENTRY              0
#else
#define NPC_BUFCHECK_ENTRY              1
#endif
#endif

#if !defined(NPC_BUFCHECK_PADDING)
#if !defined(_DEBUG)
#define NPC_BUFCHECK_PADDING            0
#else
#define NPC_BUFCHECK_PADDING            1
#endif
#endif



typedef struct npc_bufzone_entry_s npc_bufzone_entry_t;
typedef struct npc_bufzone_large_s npc_bufzone_large_t;
typedef struct npc_bufzone_s npc_bufzone_t;


struct npc_bufzone_entry_s {
    npc_bufzone_entry_t* next;
#if NPC_BUFCHECK_ENTRY
    npc_bufzone_entry_t* copy;
#endif
};


struct npc_bufzone_large_s {
    nai_list_entry_t ent;
    npc_bufzone_entry_t data;
};


struct npc_bufzone_s {
    nai_pool_t* pool;
    npc_zone_t* zone;
    size_t size;
    size_t used;
    size_t total;
    nai_list_entry_t large;
    npc_bufzone_entry_t* ents[NPC_BUFZONE_ENTRIES];
};


static int npc_bufzone_index(size_t len, size_t* alloc)
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
        index = NPC_BUFZONE_ENTRIES;
        alloc[0] = len;
    };

    return index;
};


#if NPC_BUFCHECK_PADDING

static char npc_buf_padding[8] = { 
    0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad
};

#endif


static void* npc_bufzone_malloc(void* u, size_t size)
{
    size_t alloc;
    uint32_t index;
    npc_bufzone_t* p = (npc_bufzone_t*)u;
    npc_bufzone_entry_t* e;
    npc_bufzone_large_t* l;


#if NPC_BUFCHECK_PADDING
    size += sizeof(nai_buf_padding); /* add padding space */
#endif

    if (size > p->size) {
        size += nai_offsetof(npc_bufzone_large_t, data);
        l = (npc_bufzone_large_t*)nai_malloc(size);
        if (l == 0) {
            e = 0;
            goto _end;
        } else {
            nai_list_insert_tail(&p->large, &l->ent);
            p->used += size;
            p->total += size;
            e = &l->data;
            size -= nai_offsetof(npc_bufzone_large_t, data);
            goto _ok;
        };
    };


    index = npc_bufzone_index(size, &alloc);

    if (p->ents[index]) {
        e = (npc_bufzone_entry_t*)p->ents[index];
#if NPC_BUFCHECK_ENTRY
        if (e->next != e->copy) {
            assert(0);
            nai_log_error(NPC_LOG_CORE, 0, 
                "the buffer %p of entry %d is overflow", e, index);
        };
#endif
        p->ents[index] = e->next;
    } else {
        e = (npc_bufzone_entry_t*)nai_palloc(p->pool, alloc);
        if (e == 0) {
            goto _end;
        };

        p->total += alloc;
    };

    p->used += alloc;

_ok:
#if NPC_BUFCHECK_PADDING
    /* fill padding */
    nai_memcpy((char*)e + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding));
#endif

_end:
    return e;
};


static int npc_bufzone_mfree(void* u, void* m, size_t size)
{
    size_t alloc;
    uint32_t index;
    npc_bufzone_t* p = (npc_bufzone_t*)u;
    npc_bufzone_entry_t* e;
    npc_bufzone_large_t* l;


#if NPC_BUFCHECK_PADDING
    size += sizeof(nai_buf_padding); /* add padding space */

    if (nai_memcmp((char*)m + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding)) != 0) {
        assert(0);
        nai_log_error(NPC_LOG_CORE, 0, 
            "the padding of buffer %p is overflow", m);
    };
#endif

    if (size > p->size) {
        size += nai_offsetof(npc_bufzone_large_t, data);
        l = nai_containof(m, npc_bufzone_large_t, data);

        /* remove from list */
        nai_list_entry_remove(&l->ent);
        p->used -= size;
        p->total -= size;

        /* free large memory */
        nai_free(l);
        goto _end;
    };

    /* get zone index */
    index = npc_bufzone_index(size, &alloc);

    /* put into zone */
    e = (npc_bufzone_entry_t*)m;
    e->next = p->ents[index];
#if NPC_BUFCHECK_ENTRY
    e->copy = e->next; /* for check overflow */
#endif
    p->ents[index] = e;
    p->used -= alloc;

_end:
    return 0;
};


static void* npc_bufzone_xalloc(void* u, size_t size)
{
    void* r;
    npc_bufzone_t* p = (npc_bufzone_t*)u;


#if NPC_BUFCHECK_PADDING
    size += sizeof(nai_buf_padding); /* add padding space */
#endif

    r = npc_zalloc(p->zone, size);
    if (r == 0) {
        goto _end;
    };

#if NPC_BUFCHECK_PADDING
    /* fill padding */
    nai_memcpy((char*)e + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding));
#endif

_end:
    return r;
};


static int npc_bufzone_xfree(void* u, void* m, size_t size)
{
    int r;
    npc_bufzone_t* p = (npc_bufzone_t*)u;


#if NPC_BUFCHECK_PADDING
    size += sizeof(nai_buf_padding); /* add padding space */

    if (nai_memcmp((char*)m + size - 
        sizeof(nai_buf_padding), nai_buf_padding, 
        sizeof(nai_buf_padding)) != 0) {
        assert(0);
        nai_log_error(NPC_LOG_CORE, 0, 
            "the padding of buffer %p is overflow", m);
    };
#else
    (void)size;
#endif

    r = npc_zfree(p->zone, m);

    return r;
};


static void* npc_bufzone_alloc(void* u, size_t size)
{
    void* r;


#if NPC_BUFCHECK_PADDING

    size_t* c;

    c = npc_bufzone_xalloc(u, size + sizeof(*c));
    if (c == 0) {
        r = 0;
        goto _end;
    };

    c[0] = size;
    r = c + 1;

_end:

#else
    r = npc_bufzone_xalloc(u, size);
#endif

    return r;
};


static int npc_bufzone_free(void* u, void* m)
{
    int r;


#if NPC_BUFCHECK_PADDING

    size_t* c;

    c = (size_t*)m - 1;
    r = npc_bufzone_xfree(u, c, c[0] + sizeof(*c));

#else
    r = npc_bufzone_xfree(u, m, 0);
#endif

    return r;
};


static int npc_bufzone_close(void* u)
{
    nai_pool_t stack;
    nai_list_entry_t* e;
    npc_bufzone_t* b = (npc_bufzone_t*)u;
    npc_bufzone_large_t* l;


    e = b->large.next;
    for ( ; e != &b->large; ) {
        l = (npc_bufzone_large_t*)e;
        e = e->next;
        nai_free(l);
    };

    nai_list_init(&b->large);

    nai_pool_init(&stack, 0);
    nai_pool_exchange(&stack, b->pool);
    nai_pool_close(&stack);

    return 0;
};


static void npc_bufzone_cleanup(void* u)
{
    npc_bufzone_close(u);
};



static nai_bufpool_ops_t npc_zoneops = {
    "bufzone.shm", 
    npc_bufzone_alloc, 
    npc_bufzone_free, 
    npc_bufzone_xalloc, 
    npc_bufzone_xfree,
    npc_bufzone_malloc, 
    npc_bufzone_mfree,
    npc_bufzone_close
};


int npc_bufpool_open(
    nai_bufpool_t* p, npc_zone_t* zone)
{
    int r;
    int e;
    nai_pool_t stack;
    nai_pool_t* pool;
    npc_bufzone_t* b;


    if (zone == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nai_pool_init(&stack, 1024);

    /* allocate own pool */
    pool = (nai_pool_t*)nai_palloc(&stack, sizeof(*pool));
    if (pool == 0) {
        r = -1;
        goto _end;
    };

    nai_pool_init(pool, 0);
    nai_pool_exchange(pool, &stack);


    /* alloc bufzone */
    b = (npc_bufzone_t*)nai_palloc(pool, sizeof(*b));
    if (b == 0) {
        r = -1;
        goto _fail;
    };


    /* initialize bufzone */
    nai_list_init(&b->large);
    nai_pzero((void*)b->ents, nai_countof(b->ents));
    b->pool = pool;
    b->zone = zone;
    b->total = 0;
    b->used = 0;
    b->size = pool->size - sizeof(nai_list_entry_t) - sizeof(size_t) * 2;
    if (b->size > NPC_BUFZONE_MAXSIZE) {
        b->size = NPC_BUFZONE_MAXSIZE;
    };


    /* add bufzone cleanup */
    r = nai_pool_add_cleanup(pool, npc_bufzone_cleanup, b);
    if (r < 0) {
        goto _fail;
    };


    /* setup bufpool */
    p->ops = &npc_zoneops;
    p->pool = b;
    r = 0;

_end:
    return r;

_fail:
    if (pool) {
        e = nai_errno;
        nai_pool_exchange(&stack, pool);
        nai_pool_close(&stack);
        nai_errno = e;
    };
    goto _end;
};


