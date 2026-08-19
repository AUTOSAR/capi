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
/// @file       nai_util.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_thread.h"


//////////////////////////////////////////////////////////////////////////////
//


#if defined(_WIN32)


void* nai_malloc(size_t size)
{
    return malloc(size);
};


void nai_free(void* ptr)
{
    free(ptr);
};


#endif



//////////////////////////////////////////////////////////////////////////////
// rand


typedef struct nai_r64_s {
    uint64_t s;
} nai_r64_t;


typedef struct nai_r1024_s {
    uint64_t s[16];
    nai_int_t index;
    nai_int_t reserved;
} nai_r1024_t;



static void nai_r64_seed(nai_r64_t* p, uint64_t seed)
{
    p->s = seed;
};

static uint64_t nai_r64_next(nai_r64_t* p)
{
    uint64_t r;


    p->s ^= p->s >> 12;
    p->s ^= p->s << 25;
    p->s ^= p->s >> 27;
    r = p->s * 2685821657736338717LL;

    return r;
};


static void nai_r1024_seed(nai_r1024_t* p, uint64_t seed)
{
    nai_int_t n;
    nai_r64_t r64;


    nai_r64_seed(&r64, seed);

    for (n = 0; n < 16; n ++) {
        p->s[n] = nai_r64_next(&r64);
    };

    p->index = 0;

    return;
};

static uint64_t nai_r1024_next(nai_r1024_t* p)
{
    nai_int_t p0, p1;
    uint64_t r;
    uint64_t s0, s1;


    p0 = p->index;
    p1 = p->index = (p0 + 1) & 15;
    s0 = p->s[p0];
    s1 = p->s[p1];
    s1 ^= s1 << 31; // a
    s1 ^= s1 >> 11; // b
    s0 ^= s0 >> 30; // c
    r = (p->s[p1] = s0 ^ s1) * 1181783497276652981LL;

    return r;
};



#ifndef NAI_DEFAULT_SEED
#define NAI_DEFAULT_SEED        (uint32_t)nai_time()
#endif


typedef struct nai_rstate_s {
    nai_int_t inited;
    nai_once_t once;
    nai_atomic32_t lock;
    nai_r1024_t data;
} nai_rstate_t;


static nai_rstate_t nai_rstate = { 
    0, NAI_ONCE_INIT
};



#if (NAI_HAVE_THREADS)
#define nai_rstate_lock(s)      nai_atomic32_lock(&(s)->lock)
#define nai_rstate_unlock(s)    nai_atomic32_unlock(&(s)->lock)
#else
#define nai_rstate_lock(s)      (void)0
#define nai_rstate_unlock(s)    (void)0
#endif


static void nai_rstate_init()
{
    nai_srand(NAI_DEFAULT_SEED);
};


void nai_srand(nai_int_t seed)
{
    nai_rstate_t* s;


    s = &nai_rstate;
    nai_rstate_lock(s);

    nai_r1024_seed(&s->data, (uint32_t)seed);
    nai_memory_barrier();
    s->inited = 1;

    nai_rstate_unlock(s);

    return;
};


float nai_random()
{
    uint32_t r;
    nai_rstate_t* s;


    s = &nai_rstate;
    if (s->inited == 0) {
        nai_once(&s->once, nai_rstate_init);
    };

    nai_rstate_lock(s);

    r = (uint32_t)nai_r1024_next(&s->data);

    nai_rstate_unlock(s);

    /* float format: sign 1bit, exp 8bit, num 23bit
     * we need 23bit only, and use 8bit:30bit
     */
    return ((r & NAI_INT32_T_MAX) >> 8) * (1.0f / (NAI_INT32_T_MAX >> 8));
};


uint32_t nai_rand32()
{
    uint32_t r;
    nai_rstate_t* s;


    s = &nai_rstate;
    if (s->inited == 0) {
        nai_once(&s->once, nai_rstate_init);
    };

    nai_rstate_lock(s);

    r = (uint32_t)nai_r1024_next(&s->data);

    nai_rstate_unlock(s);

    return r & NAI_INT32_T_MAX;
};


uint64_t nai_rand64()
{
    uint64_t r;
    nai_rstate_t* s;


    s = &nai_rstate;
    if (s->inited == 0) {
        nai_once(&s->once, nai_rstate_init);
    };

    nai_rstate_lock(s);

    r = (uint64_t)nai_r1024_next(&s->data);

    nai_rstate_unlock(s);

    return r & NAI_INT64_T_MAX;
};


//////////////////////////////////////////////////////////////////////////////
// fast math


float nai_absf(float x)
{
    void* p = (void*)&x;
    nai_int_t* n = (nai_int_t*)p;
    n[0] &= 0x7FFFFFFF;
    return *(float*)n;
};


float nai_floorf(float x)
{
    nai_int_t n = (nai_int_t)x;   // maybe error
    n = n - (n > x);
    return (float)n;
};


float nai_sinf(float x)
{
    const float tp = 1.0f / (2.0f * (float)M_PI);
    x *= tp;
    x -= 0.5f + (nai_int_t)nai_floorf(x);
    x *= 16.0f * (nai_absf(x) - 0.5f);
    x += .225f * x * (nai_absf(x) - 1.0f);
    return x;
};


float nai_cosf(float x) 
{
    const float tp = 1.0f / (2.0f * (float)M_PI);
    x *= tp;
    x -= 0.25f + (nai_int_t)nai_floorf(x + 0.25f);
    x *= 16.0f * (nai_absf(x) - 0.5f);
    x += .225f * x * (nai_absf(x) - 1.0f);
    return x;
};


float nai_exp2f(float e)
{
    float offset = (e < 0) ? 1.0f : 0.0f;
    float clipp = (e < -126) ? -126.0f : e;
    nai_int_t w = (nai_int_t)clipp;
    float z = clipp - w + offset;
    union { uint32_t i; float f; } v = { 
        (uint32_t)(
        (1 << 23) * (clipp + 121.2740575f + 27.7280233f / 
        (4.84252568f - z) - 1.49012907f * z)) 
    };
    return v.f;
};


float nai_log2f(float x)
{
    union { float f; uint32_t i; } vx = { x };
    union { uint32_t i; float f; } mx = { 
        (vx.i & 0x007FFFFF) | 0x3f000000
    };
    float y = (float)vx.i;
    y *= 1.1920928955078125e-7f;
    return y - 124.22551499f 
        - 1.498030302f * mx.f 
        - 1.72587999f / (0.3520887068f + mx.f);
};


float nai_powf(float x, float e)
{
    return nai_exp2f(e * nai_log2f(x));
};


//////////////////////////////////////////////////////////////////////////////
// 


nai_sult_t nai_sult(nai_int_t r)
{
    nai_sult_t rc;

    rc = r;
    if (rc < 0) {
        rc = nai_sult_from_errno();
    };

    return rc;
};


void nai_pzero(void** p, size_t count)
{
    size_t n = 0;
    for ( ; n < count; n ++) {
        p[n] = NULL;
    };
};


