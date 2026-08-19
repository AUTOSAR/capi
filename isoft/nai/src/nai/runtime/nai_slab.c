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
/// @file       nai_slab.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_slab.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_mman.h"



//////////////////////////////////////////////////////////////////////////////
// the default backend of allocator


static void* nai_valloc(void* ud, size_t len)
{
    (void)ud;

    return nai_mmap(0, 0, len, 0, NAI_MPROT_READ|NAI_MPROT_WRITE);
};

static nai_int_t nai_vadvise(void* ud, void* lp, size_t len, nai_int_t advise)
{
    (void)ud;

    return nai_madvise(lp, len, advise);
};

static nai_int_t nai_vfree(void* ud, void* lp, size_t len)
{
    (void)ud;

    return nai_munmap(0, lp, len);
};


static nai_slab_ops_t nai_valloc_ops = {
    nai_valloc, 
    nai_vadvise, 
    nai_vfree, 
};


#define nai_slab_alloc(p, size)                     \
    (p)->ops->alloc((p)->ud, (size))                \

#define nai_slab_free(p, lp, size)                  \
    (p)->ops->free((p)->ud, (lp), (size))           \

#define nai_slab_advise(p, lp, size, flag)          \
    (p)->ops->advise((p)->ud, (lp), (size), (flag)) \



//////////////////////////////////////////////////////////////////////////////
// the simple allocator


#define NAI_PAGESIZE        4096
#define NAI_GROWSIZE        4096*1024


static nai_int_t nai_slab_clean(void* u, void* ptr, size_t size)
{
    nai_int_t r;
    nai_slab_t* p;


    p = (nai_slab_t*)u;
    r = nai_slab_free(p, ptr, size);

    return r;
};


nai_int_t nai_slab_init(nai_slab_t* p)
{
    nai_int_t r;


    nai_zone_init(&p->zone);
    p->ops = 0;
    p->ud = 0;
    p->lock = 0;
    p->gsize = 0;
    r = 0;

    return r;
};


nai_int_t nai_slab_open(nai_slab_t* p, 
    nai_int_t type, size_t gsize, const nai_slab_ops_t* ops, void* ud)
{
    nai_int_t r;


    if (p->ops) {
        r = 0;
        goto _end;
    };

    if (gsize <= 0) {
        gsize = NAI_GROWSIZE;
    };
    gsize = nai_align(gsize, NAI_PAGESIZE);


    if (ops == 0) {
        ops = &nai_valloc_ops;
    };

    p->ops = (nai_slab_ops_t*)ops;
    p->ud = ud;
    p->gsize = gsize;
    r = nai_zone_open(&p->zone, type, 0);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_slab_reset(nai_slab_t* p)
{
    nai_int_t r;


    if (p->ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_zone_reset(&p->zone);
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_slab_close(nai_slab_t* p)
{
    nai_int_t r;


    if (p->ops == 0) {
        r = 0;
        goto _end;
    };

    r = nai_zone_reset(&p->zone);
    if (r < 0) {
        goto _end;
    };

    r = nai_zone_clean(&p->zone, nai_slab_clean, p);
    if (r < 0) {
        goto _end;
    };

    p->ud = 0;
    p->ops = 0;
    r = 0;

_end:
    return r;
};


void* nai_salloc(nai_slab_t* p, size_t size)
{
    void* r;
    nai_int_t n;
    nai_int_t ec;
    size_t alloc;
    size_t align;
    void* ptr;


    /* lock */
    if (p->lock) {
        nai_spin_lock(p->lock);
    };

    /* do alloc */
    r = nai_zalloc(&p->zone, size);
    while (r == 0) {
        ec = nai_errno;
        if (ec != ENOMEM) {
            break;
        };

        if (p->ops == 0) {
            ec = EINVAL;
            break;
        };

        if (p->zone.type != NAI_ZONE_MEMORY) {
            alloc = 0;          /* device: meta data is on host memory */
        } else {
            alloc = size / 128; /* add loss size of meta data */
        };
        alloc += NAI_PAGESIZE;  /* add loss size of unaligned page */
        alloc += size;
        align = p->gsize;
        for ( ; align < alloc; align <<= 1) {
            /* nothing */
            ;
        };

        ptr = nai_slab_alloc(p, align);
        if (ptr == 0) {
            ec = nai_errno;
            break;
        };

        n = nai_zone_offer(&p->zone, ptr, align);
        if (n < 0) {
            ec = nai_errno;
            nai_slab_free(p, ptr, align);
            break;
        };

        r = nai_zalloc(&p->zone, size);
        assert(r != 0);
        if (r == 0) {
            ec = nai_errno;
        };

        break;
    };

    /* unlock */
    if (p->lock) {
        nai_spin_unlock(p->lock);
    };

    if (r == 0) {
        nai_errno = ec;
    };

    return r;
};


nai_int_t nai_sfree(nai_slab_t* p, void* ptr)
{
    nai_int_t r;
    nai_int_t ec;
    void* mem;
    size_t size;


    /* lock */
    if (p->lock) {
        nai_spin_lock(p->lock);
    };

    /* do free */
    r = nai_zfree(&p->zone, ptr);
    if (r < 0) {
        ec = nai_errno;
    } else {
        if (nai_zone_empty_segs(&p->zone) > 1) {
            mem = nai_zone_eject(&p->zone, &size);
            if (mem) {
                nai_slab_clean(p, mem, size);
            };
        };
    };

    /* unlock */
    if (p->lock) {
        nai_spin_unlock(p->lock);
    };

    if (r < 0) {
        nai_errno = ec;
    };

    return r;
};

