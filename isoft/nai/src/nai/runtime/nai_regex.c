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
/// @file       nai_regex.c
/// @brief      
/// @details
/// @date       2021-01-15
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_regex.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_tlocal.h"



#if (NAI_HAVE_REGEX)


#if (NAI_HAVE_PCRE)
#if defined(_WIN32)
#define PCRE_STATIC 1
#endif
#define PCREX(x) PCRE_ ## x
#include "pcre.h"
/* #elif (NAI_HAVE_PCRE2) */
/* #define PCRE2_CODE_UNIT_WIDTH 8 */
/* #define PCREX(x) PCRE2_ ## x */
/* #include "pcre2.h" */
#else
#error "nai_regex is not implemented!"
#endif



typedef struct nai_regex_state_s {
    nai_int_t inited;
    nai_int_t error;
    nai_int_t jit;
    nai_once_t once;
} nai_regex_state_t;


static nai_regex_state_t nai_rstate = {
    0, 0, 0, NAI_ONCE_INIT, 
};


static void* nai_regex_malloc(size_t size)
{
    void* m;
    nai_pool_t* p;
    nai_tlocal_t* t;


    t = nai_tlocal_get();
    if (t == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_tlocal_get() failed");

        m = 0;
        goto _end;
    };

    p = (nai_pool_t*)t->pcre;
    if (p) {
        m = nai_palloc(p, size);
    } else {
        m = nai_malloc(size);
    };

_end:
    return m;
};


static void nai_regex_free(void* p)
{
    nai_free(p);
};


static void nai_regex_jit_free(void* p)
{
    pcre_free_study((pcre_extra*)p);
};


static void nai_regex_state_init()
{
    pcre_config(PCRE_CONFIG_JIT, &nai_rstate.jit);
    pcre_malloc = nai_regex_malloc;
    pcre_free = nai_regex_free;

    nai_memory_barrier();
    nai_rstate.inited = 1;
    return;
};


nai_int_t nai_regex_compile(nai_regex_t* re, 
    const char* pattern, nai_int_t flags, 
    nai_regex_errinfo_t* e, nai_pool_t* pool)
{
    nai_int_t r;
    nai_int_t jit;
    nai_int_t options = PCREX(DUPNAMES);
    nai_int_t erroff;
    const char* errstr;
    nai_tlocal_t* t;


    if (flags & NAI_REGEX_CASELESS)
        options |= PCREX(CASELESS);
    if (flags & NAI_REGEX_NEWLINE)
        options |= PCREX(MULTILINE);
    if (flags & NAI_REGEX_DOTALL)
        options |= PCREX(DOTALL);
    if (flags & NAI_REGEX_DOLLAR_ENDONLY)
        options |= PCREX(DOLLAR_ENDONLY);

    if (nai_rstate.inited == 0) {
        nai_once(&nai_rstate.once, nai_regex_state_init);
    };
    if (nai_rstate.error) {
        nai_errno = nai_rstate.error;
        errstr = "nai_regex global initialize failed";
        erroff = -1;
        r = PCRE_ERROR_NOMATCH; /* don't change errno */
        goto _fail;
    };

    /* setup current memory pool */
    t = nai_tlocal_get();
    if (t == 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "nai_tlocal_get() failed");
        errstr = "nai_tlocal_get() failed";
        erroff = -1;
        r = PCRE_ERROR_NOMATCH;
        goto _fail;
    };
    t->pcre = pool;

    /* compile string of pattern */
    re->sub = 0;
    re->reg = pcre_compile2(
        pattern, options, &r, &errstr, &erroff, 0);
    if (re->reg == 0) {
        re->pool = 0;
        re->extra = 0;
        goto _fail;
    };

    r = pcre_fullinfo((pcre*)re->reg, 0, PCRE_INFO_CAPTURECOUNT, &re->sub);
    if (r < 0) {
        if (re->reg) {
            pcre_free(re->reg);
            re->reg = 0;
        };
        errstr = "pcre_fullinfo(PCRE_INFO_CAPTURECOUNT) failed";
        erroff = -1;
        goto _fail;
    };

    jit = 0;
    if ((flags & NAI_REGEX_JIT) && nai_rstate.jit) {
        jit = PCRE_STUDY_JIT_COMPILE;
    };

    errstr = 0;
    re->pool = pool;
    re->extra = pcre_study((pcre*)re->reg, jit, &errstr);
    if (errstr == 0) {
        if (pool && re->extra) {
            nai_pool_add_cleanup(pool, nai_regex_jit_free, re->extra);
        };
    } else {
        if (re->reg) {
            pcre_free(re->reg);
            re->reg = 0;
            re->pool = 0;
        };
        r = PCRE_ERROR_NOMEMORY;
        erroff = -1;
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    if (e) {
        e->str = errstr;
        e->offset = erroff;
    };
    switch (r) {
    case PCRE_ERROR_NOMATCH:
        break;
    case PCRE_ERROR_NULL:
        nai_errno = EINVAL;
        break;
    case PCRE_ERROR_NOMEMORY:
        nai_errno = ENOMEM;
        break;
    default:
        nai_errno = NAI_EFAILED;
        break;
    };

    r = -1;
    goto _end;
};


nai_int_t nai_regex_exec(nai_regex_t* re, 
    const char* string, size_t len, nai_int_t* mbuf, nai_int_t msize)
{
    nai_int_t r;


    r = pcre_exec(re->reg, 
        re->extra, string, (nai_int_t)len, 0, 0, mbuf, msize);
    if (r < 0) {
        switch (r) {
        case PCRE_ERROR_NOMATCH:
            break;
        case PCRE_ERROR_NULL:
            nai_errno = EINVAL;
            break;
        case PCRE_ERROR_NOMEMORY:
            nai_errno = ENOMEM;
            break;
        default:
            nai_errno = NAI_EFAILED;
            break;
        };
    };

    return r;
};


nai_int_t nai_regex_get_named(nai_regex_t* re, nai_regex_named_t* named)
{
    nai_int_t r;


    r = pcre_fullinfo((pcre*)re->reg, 0, PCRE_INFO_NAMECOUNT, &named->count);
    if (r < 0) {
        goto _fail;
    };
    r = pcre_fullinfo((pcre*)re->reg, 
        0, PCRE_INFO_NAMEENTRYSIZE, &named->eltsize);
    if (r < 0) {
        goto _fail;
    };

    r = pcre_fullinfo((pcre*)re->reg, 
        0, PCRE_INFO_NAMETABLE, &named->data);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    switch (r) {
    case PCRE_ERROR_NOMATCH:
        break;
    case PCRE_ERROR_NULL:
        nai_errno = EINVAL;
        break;
    case PCRE_ERROR_NOMEMORY:
        nai_errno = ENOMEM;
        break;
    default:
        nai_errno = NAI_EFAILED;
        break;
    };
    r = -1;
    goto _end;
};


nai_int_t nai_regex_close(nai_regex_t* re)
{
    if (re->pool == 0) {
        if (re->reg) {
            nai_free((pcre*)re->reg);
            re->reg = 0;
        };
        if (re->extra) {
            pcre_free_study((pcre_extra*)re->extra);
            re->extra = 0;
        };
    };

    return 0;
};


#endif


