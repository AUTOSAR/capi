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
/// @file       nai_array.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"



nai_int_t nai_array_init(nai_array_t* p, size_t eltsize, nai_pool_t* pool)
{
    nai_int_t r;


    if (eltsize <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    p->pool = pool;
    p->elts = 0;
    p->alloc = 0;
    p->count = 0;
    p->eltsize = eltsize;

    r = 0;

_end:
    return r;
};


nai_int_t nai_array_reserve(nai_array_t* p, size_t size)
{
    nai_int_t r;
    void* elts;
    size_t alloc;


    if (size == p->alloc) {
        r = 0;
        goto _end;
    };
    if (size < p->count) {
        r = 0;
        goto _end;
    };

    alloc = size;
    elts = p->pool ? 
        nai_palloc(p->pool, p->eltsize * alloc) : 
        nai_malloc(p->eltsize * alloc);
    if (elts == 0) {
        r = -1;
        goto _end;
    };

    if (p->elts) {
        nai_memcpy(elts, p->elts, p->eltsize * p->count);

        if (p->pool == 0) {
            nai_free(p->elts);
        };
    };

    p->elts = elts;
    r = 0;

_end:
    return r;
};


nai_int_t nai_array_close(nai_array_t* p)
{
    if (p->elts) {
        if (p->pool == 0) {
            nai_free(p->elts);
        };
        p->elts = 0;
    };
    p->count = 0;
    p->alloc = 0;

    return 0;
};


void* nai_array_push(nai_array_t* p)
{
    return nai_array_push_n(p, 1);
};


void* nai_array_push_n(nai_array_t* p, size_t n)
{
    void* elts;
    size_t alloc;


    if (p->count + n >= p->alloc) {

        alloc = 2 * (n > p->alloc ? n : p->alloc);
        if (alloc < 8) {
            alloc = 8;
        };

        elts = p->pool ? 
            nai_palloc(p->pool, p->eltsize * alloc) : 
            nai_malloc(p->eltsize * alloc);
        if (elts == 0) {
            goto _end;
        };

        nai_memcpy(elts, p->elts, p->count * p->eltsize);

        if (p->elts && !p->pool) {
            nai_free(p->elts);
        };
        p->elts = elts;
        p->alloc = alloc;
    };

    elts = (uint8_t*)p->elts + p->count * p->eltsize;
    p->count += n;

_end:
    return elts;
};

