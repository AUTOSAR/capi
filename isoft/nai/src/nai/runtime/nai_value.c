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
/// @file       nai_value.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_value.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include <stdlib.h>


nai_sult_t nai_value_geti(const nai_value_t* p, int32_t* v)
{
    const char* end;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_INT32:
        *v = *p->i;
        break;

    case NAI_VALUE_INT64:
        *v = (nai_int_t)*p->l;
        break;

    case NAI_VALUE_FLOAT:
        *v = (nai_int_t)*p->f;
        break;

    case NAI_VALUE_DOUBLE:
        *v = (nai_int_t)*p->d;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        r = nai_atoi(v, nai_str(&p->s), nai_str_len(&p->s), &end);
        if (r < 0) {
            r = nai_sult_from_errno();
        } else if (*end != 0 && !nai_isspace(*end)) {
            r = NAI_E_INVALID_ARG;
            nai_errno = nai_sult_to_errno(r);
        }
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_geti64(const nai_value_t* p, int64_t* v)
{
    const char* end;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_INT32:
        *v = *p->i;
        break;

    case NAI_VALUE_INT64:
        *v = *p->l;
        break;

    case NAI_VALUE_FLOAT:
        *v = (int64_t)*p->f;
        break;

    case NAI_VALUE_DOUBLE:
        *v = (int64_t)*p->d;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        r = nai_atol(v, nai_str(&p->s), nai_str_len(&p->s), &end);
        if (r < 0) {
            r = nai_sult_from_errno();
        } else if (*end != 0 && !nai_isspace(*end)) {
            r = NAI_E_INVALID_ARG;
            nai_errno = nai_sult_to_errno(r);
        }
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


#if (__darwin__)


nai_sult_t nai_value_getiptr(const nai_value_t* p, intptr_t* v)
{
#if NAI_SIZEOF_VOID_P == 4
    return nai_value_geti32(p, (int32_t*)v);
#else
    return nai_value_geti64(p, (int64_t*)v);
#endif
};


#endif


nai_sult_t nai_value_getf(const nai_value_t* p, float* v)
{
    nai_int_t len;
    char* s;
    char* end;
    char buf[128];
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_INT32:
        *v = (float)*p->i;
        break;

    case NAI_VALUE_INT64:
        *v = (float)*p->l;
        break;

    case NAI_VALUE_FLOAT:
        *v = *p->f;
        break;

    case NAI_VALUE_DOUBLE:
        *v = (float)*p->d;
        break;

    case NAI_VALUE_MEMORY:
    case NAI_VALUE_STRING:
        if (p->type == NAI_VALUE_STRING) {
            s = p->s.ptr;
        } else {
            s = buf;
            if (p->s.len < nai_countof(buf)) {
                len = p->s.len;
            } else {
                len = nai_countof(buf)-1;
            };
            nai_memcpy(s, p->s.ptr, len);
            s[len] = 0;
        };

        nai_errno = 0;
        *v = strtof(s, &end);
        if (nai_errno) {
            r = nai_sult_from_errno();
        } else if (*end != 0 && !nai_isspace(*end)) {
            r = NAI_E_INVALID_ARG;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_getd(const nai_value_t* p, double* v)
{
    nai_int_t len;
    char* s;
    char* end;
    char buf[128];
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_INT32:
        *v = (double)*p->i;
        break;

    case NAI_VALUE_INT64:
        *v = (double)*p->l;
        break;

    case NAI_VALUE_FLOAT:
        *v = (double)*p->f;
        break;

    case NAI_VALUE_DOUBLE:
        *v = *p->d;
        break;

    case NAI_VALUE_MEMORY:
    case NAI_VALUE_STRING:
        if (p->type == NAI_VALUE_STRING) {
            s = p->s.ptr;
        } else {
            s = buf;
            if (p->s.len < nai_countof(buf)) {
                len = p->s.len;
            } else {
                len = nai_countof(buf)-1;
            };
            nai_memcpy(s, p->s.ptr, len);
            s[len] = 0;
        };

        nai_errno = 0;
        *v = strtod(s, &end);
        if (nai_errno) {
            r = nai_sult_from_errno();
        } else if (*end != 0 && !nai_isspace(*end)) {
            r = NAI_E_INVALID_ARG;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sultp_t nai_value_gets(const nai_value_t* p, char* v, size_t len)
{
    nai_sultp_t r;


    switch (p->type) {
    case NAI_VALUE_INT32:
        r = nai_snprintf(v, len, "%d", *p->i);
        break;

    case NAI_VALUE_INT64:
        r = nai_snprintf(v, len, "%" NAI_INT64_FMT "d", *p->l);
        break;

    case NAI_VALUE_FLOAT:
        r = nai_snprintf(v, len, "%f", (double)*p->f);
        break;

    case NAI_VALUE_DOUBLE:
        r = nai_snprintf(v, len, "%lf", *p->d);
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        if (len > p->s.len) {
            len = p->s.len;
            v[len] = '\0';
        };
        nai_memcpy(v, p->s.ptr, len);
        r = p->s.len;
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno((nai_sult_t)r);
        break;
    };

    return r;
};


nai_sult_t nai_value_getm(const nai_value_t* p, nai_mem_t* m)
{
    nai_sult_t r;


    switch (p->type) {
    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        m[0] = p->s;
        r = 0;
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_seti32(nai_value_t* p, int32_t v)
{
    intptr_t len;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_VOID:
        break;

    case NAI_VALUE_INT32:
        *p->i = v;
        break;

    case NAI_VALUE_INT64:
        *p->l = v;
        break;

    case NAI_VALUE_FLOAT:
        *p->f = (float)v;
        break;

    case NAI_VALUE_DOUBLE:
        *p->d = (double)v;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        len = nai_snprintf(p->s.ptr, p->s.len, "%d", v);
        if (len < 0) {
            r = nai_sult_from_errno();
        } else if (len+1 > (intptr_t)p->s.len) {
            r = NAI_E_NOT_ENOUGH;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_seti64(nai_value_t* p, int64_t v)
{
    intptr_t len;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_VOID:
        break;

    case NAI_VALUE_INT32:
        *p->i = (int32_t)v;
        break;

    case NAI_VALUE_INT64:
        *p->l = v;
        break;

    case NAI_VALUE_FLOAT:
        *p->f = (float)v;
        break;

    case NAI_VALUE_DOUBLE:
        *p->d = (double)v;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        len = nai_snprintf(p->s.ptr, p->s.len, "%" NAI_INT64_FMT "d", v);
        if (len < 0) {
            r = nai_sult_from_errno();
        } else if (len+1 > (intptr_t)p->s.len) {
            r = NAI_E_NOT_ENOUGH;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_setf(nai_value_t* p, float v)
{
    intptr_t len;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_VOID:
        break;

    case NAI_VALUE_INT32:
        *p->i = (int32_t)v;
        break;

    case NAI_VALUE_INT64:
        *p->l = (int64_t)v;
        break;

    case NAI_VALUE_FLOAT:
        *p->f = v;
        break;

    case NAI_VALUE_DOUBLE:
        *p->d = (double)v;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        len = nai_snprintf(p->s.ptr, p->s.len, "%f", (double)v);
        if (len < 0) {
            r = nai_sult_from_errno();
        } else if (len+1 > (intptr_t)p->s.len) {
            r = NAI_E_NOT_ENOUGH;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_setd(nai_value_t* p, double v)
{
    intptr_t len;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_VOID:
        break;

    case NAI_VALUE_INT32:
        *p->i = (int32_t)v;
        break;

    case NAI_VALUE_INT64:
        *p->l = (int64_t)v;
        break;

    case NAI_VALUE_FLOAT:
        *p->f = (float)v;
        break;

    case NAI_VALUE_DOUBLE:
        *p->d = v;
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        len = nai_snprintf(p->s.ptr, p->s.len, "%lf", v);
        if (len < 0) {
            r = nai_sult_from_errno();
        } else if (len+1 > (intptr_t)p->s.len) {
            r = NAI_E_NOT_ENOUGH;
            nai_errno = nai_sult_to_errno(r);
        };
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_sets(nai_value_t* p, const char* v)
{
    intptr_t len;
    char* end;
    nai_sult_t r = 0;


    switch (p->type) {
    case NAI_VALUE_VOID:
        break;

    case NAI_VALUE_INT32:
        nai_errno = 0;
        *p->i = strtol(v, &end, 0);
        if (*end != 0 || nai_isspace(*end)) {
            r = nai_sult_from_errno();
        };
        break;

    case NAI_VALUE_FLOAT:
        nai_errno = 0;
        *p->f = strtof(v, &end);
        if (*end != 0 || nai_isspace(*end)) {
            r = nai_sult_from_errno();
        };
        break;

    case NAI_VALUE_DOUBLE:
        nai_errno = 0;
        *p->d = strtod(v, &end);
        if (*end != 0 || nai_isspace(*end)) {
            r = nai_sult_from_errno();
        };
        break;

    case NAI_VALUE_STRING:
    case NAI_VALUE_MEMORY:
        len = nai_strlen(v) + 1;
        if (len < (intptr_t)p->s.len) {
            nai_memcpy(p->s.ptr, v, len);
        } else {
            nai_memcpy(p->s.ptr, v, p->s.len);
        };
        r = (nai_int_t)len;
        break;

    default:
        r = NAI_E_TYPE;
        nai_errno = nai_sult_to_errno(r);
        break;
    };

    return r;
};


nai_sult_t nai_value_dups(const nai_value_t* v, nai_str_t* s, nai_pool_t* p)
{
    nai_sultp_t r;
    char* m;

    r = nai_value_gets(v, 0, 0);
    if (r < 0) {
        goto _end;
    };

    r += 1;
    m = (char*)(p ? nai_palloc(p, r) : nai_malloc(r));
    if (m == 0) {
        r = nai_sult_from_errno();
        goto _end;
    };

    r = nai_value_gets(v, m, r);
    if (r < 0) {
        if (p == 0) {
            nai_free(m);
        };
        goto _end;
    };

    nai_str_setm(s, m, r);
    r = 0;

_end:
    return (nai_sult_t)r;
};


