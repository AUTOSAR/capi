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
/// @file       nai_string.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include <string.h>


intptr_t nai_str_dup(nai_str_t* d, 
    const char* s, size_t len, nai_pool_t* p)
{
    return nai_str_dup_r(d, s, len, 0, p);
};


intptr_t nai_str_dup_r(nai_str_t* d, 
    const char* s, size_t len, 
    const nai_reserved_t* rev, nai_pool_t* p)
{
    char* m;
    intptr_t r;
    size_t alloc;
    size_t pre;


    if (s == 0) {
        nai_str_setn(d);
        r = 0;
        goto _end;
    };

    if ((intptr_t)len < 0) {
        len = nai_strlen(s);
    };

    if (rev == 0) {
        pre = 0;
        alloc = len + 1;
    } else {
        pre = rev->pre;
        alloc = len + 1 + pre + rev->post;
    };
    m = p ? nai_palloc(p, alloc) : nai_malloc(alloc);
    if (m == 0) {
        r = -1;
        goto _end;
    };

    nai_memcpy(m + pre, s, len);

    len += pre;
    m[len] = 0;
    nai_str_setm(d, m, len);
    r = len;

_end:
    return r;
};


size_t nai_strnlen(const char* s, size_t size)
{
    char* d;
    size_t r;


    d = (char*)memchr(s, 0, size);
    if (d) {
        r = (d - s);
    } else {
        r = size;
    };

    return r;
};


nai_int_t nai_strn2cmp(const char* s0, size_t n0, const char* s1, size_t n1)
{
    nai_int_t c;
    intptr_t n;


    n = n0 - n1;
    c = nai_strncmp(s0, s1, n <= 0 ? n0 : n1);
    if (c == 0) {
        c = (nai_int_t)n;
    };
    return c;
};


nai_int_t nai_strcasecmp(const char* s0, const char* s1)
{
    nai_int_t r;
    uint8_t c0;
    uint8_t c1;


    for (;;) {
        c0 = *s0 ++;
        c1 = *s1 ++;

        r = c0 - c1;
        if (r) {
            c0 = nai_tolower(c0);
            c1 = nai_tolower(c1);

            r = c0 - c1;
            if (r) {
                break;
            };
        };
        if (c0 == 0) {
            break;
        };
    };

    return r;
};


nai_int_t nai_strncasecmp(const char* s0, const char* s1, size_t len)
{
    nai_int_t r = 0;
    intptr_t n = len;
    uint8_t c0;
    uint8_t c1;


    for ( ; n > 0; n --) {
        c0 = *s0 ++;
        c1 = *s1 ++;

        r = c0 - c1;
        if (r) {
            c0 = nai_tolower(c0);
            c1 = nai_tolower(c1);

            r = c0 - c1;
            if (r) {
                break;
            };
        };
        if (c0 == 0) {
            break;
        };
    };

    return r;
};


nai_int_t nai_strn2casecmp(
    const char* s0, size_t n0, const char* s1, size_t n1)
{
    nai_int_t c;
    intptr_t n;

    n = n0 - n1;
    c = nai_strncasecmp(s0, s1, n <= 0 ? n0 : n1);
    if (c == 0) {
        c = (nai_int_t)n;
    };

    return c;
};


uint32_t nai_strhash(const char* s, nai_int_t cases)
{
    uint32_t hash;
    const char* p;
    const char* pend;


    p = s;
    pend = p + 16;
    hash = 0;

    if (cases) {
        for ( ; p < pend; p ++) {
            if (*p == 0) {
                break;
            };
            hash = (hash << 5) + hash + (uint8_t)nai_tolower(*p);
        };
    } else {
        for ( ; p < pend; p ++) {
            if (*p == 0) {
                break;
            };
            hash = (hash << 5) + hash + (uint8_t)*p;
        };
    };

    return hash;
};

uint32_t nai_strnhash(const char* s, size_t len, nai_int_t cases)
{
    uint32_t hash;
    const char* p;
    const char* pend;


    if (len > 16) {
        len = 16;
    };

    p = s;
    pend = p + len;
    hash = 0;

    if (cases) {
        for ( ; p < pend; p ++) {
            hash = (hash << 5) + hash + (uint8_t)nai_tolower(*p);
        };
    } else {
        for ( ; p < pend; p ++) {
            hash = (hash << 5) + hash + (uint8_t)*p;
        };
    };

    return hash;
};


#if (__darwin__)


nai_int_t nai_atoof(off_t* pv, 
    const char* s, size_t len, const char** pend)
{
#if NAI_SIZEOF_SIZE_T == 4
    return nai_atoi((int32_t*)pv, s, len, pend);
#else
    return nai_atol((int64_t*)pv, s, len, pend);
#endif
};


nai_int_t nai_atosz(ssize_t* pv, 
    const char* s, size_t len, const char** pend)
{
#if NAI_SIZEOF_SIZE_T == 4
    return nai_atoi((int32_t*)pv, s, len, pend);
#else
    return nai_atol((int64_t*)pv, s, len, pend);
#endif
};


#endif


nai_int_t nai_atoi(int32_t* pv, 
    const char* s, size_t len, const char** pend)
{
    nai_int_t r;
    nai_int_t sign = 0;
    nai_int_t v;
    nai_int_t vnew;


    if ((intptr_t)len < 0) {
        len = strlen(s);
    };

    if (len > 0) {
        switch (s[0]) {
        case '+':
            sign = 0;
            s ++;
            len --;
            break;
        case '-':
            sign = 1;
            s ++;
            len --;
            break;
        default:
            break;
        };
    };

    v = 0;
    for ( ; len > 0; s ++, len --) {
        if (!nai_isdigit(s[0])) {
            if (pend) {
                break;
            };

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        vnew = v * 10 + (s[0] - '0');
        if (vnew < v) {
            nai_errno = ERANGE;
            r = -1;
            goto _end;
        };

        v = vnew;
    };

    if (sign) {
        v = -v;
    };

    pv[0] = v;
    r = 0;

_end:
    if (pend) {
        pend[0] = s;
    };
    return r;
};


nai_int_t nai_atoui(uint32_t* pv, 
    const char* s, size_t len, const char** pend)
{
    return nai_atoi((int32_t*)pv, s, len, pend);
};


nai_int_t nai_atol(int64_t* pv, 
    const char* s, size_t len, const char** pend)
{
    nai_int_t r;
    nai_int_t sign = 0;
    int64_t v;
    int64_t vnew;


    if ((intptr_t)len < 0) {
        len = strlen(s);
    };

    if (len > 0) {
        switch (s[0]) {
        case '+':
            sign = 0;
            s ++;
            len --;
            break;
        case '-':
            sign = 1;
            s ++;
            len --;
            break;
        default:
            break;
        };
    };

    v = 0;
    for ( ; len > 0; s ++, len --) {
        if (!nai_isdigit(s[0])) {
            if (pend) {
                break;
            };

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        vnew = v * 10 + (s[0] - '0');
        if (vnew < v) {
            nai_errno = ERANGE;
            r = -1;
            goto _end;
        };

        v = vnew;
    };

    if (sign) {
        v = -v;
    };

    pv[0] = v;
    r = 0;

_end:
    if (pend) {
        pend[0] = s;
    };
    return r;
};


nai_int_t nai_atoul(uint64_t* pv, 
    const char* s, size_t len, const char** pend)
{
    return nai_atol((int64_t*)pv, s, len, pend);
};



#if (NAI_HAVE_THIRDPARTY_PRINTF)



extern nai_int_t vsprintf_(char* s, const char* fmt, va_list va);


extern nai_int_t vsnprintf_(char* s, size_t size, const char* fmt, va_list va);


nai_int_t _putchar(char ch)
{
    return 0;
};


nai_int_t nai_sprintf(char* s, const char* fmt, ...)
{
    nai_int_t r;
    va_list va;

    va_start(va, fmt);
    r = vsnprintf_(s, -1, fmt, va);
    va_end(va);

    return r;
};


nai_int_t nai_vsprintf(char* s, const char* fmt, va_list va)
{
    return vsnprintf_(s, -1, fmt, va);
};


nai_int_t nai_snprintf(char* s, size_t size, const char* fmt, ...)
{
    nai_int_t r;
    va_list va;

    va_start(va, fmt);
    r = vsnprintf_(s, size, fmt, va);
    va_end(va);

    return r;
};


nai_int_t nai_vsnprintf(char* s, size_t size, const char* fmt, va_list va)
{
    return vsnprintf_(s, size, fmt, va);
};


#else


#if defined(_MSC_VER) && _MSC_VER < 1900


nai_int_t nai_snprintf(char* s, size_t size, const char* fmt, ...)
{
    nai_int_t r;
    va_list va;

    va_start(va, fmt);
    r = nai_vsnprintf(s, size, fmt, va);
    va_end(va);

    return r;
};

nai_int_t nai_vsnprintf(char* s, size_t size, const char* fmt, va_list va)
{
    nai_int_t r;

    r = _vsnprintf(s, size, fmt, va);
    if (r < 0) {
        if (size > 0) {
            s[size-1] = 0;
        };
        r = _vscprintf(fmt, va);
    };

    return r;
};


#endif



//////////////////////////////////////////////////////////////////////////////
// base64 encode, copy from nginx


static intptr_t nai_encode_base64_impl(
    char* dst, size_t dstlen, 
    const char* src, size_t srclen, const char* tbl, nai_int_t padding)
{
    intptr_t r;
    intptr_t len;
    uint8_t* d;
    uint8_t* s;


    (void)dstlen;

    d = (uint8_t*)dst;
    s = (uint8_t*)src;
    len = srclen;
    if (len < 0) {
        len = nai_strlen(src);
    };

    if (dstlen < (size_t)nai_encode_base64_len(len)) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    while (len >= 3) {
        *d++ = tbl[((s[0]) >> 2) & 0x3f];
        *d++ = tbl[((s[0] & 0x03) << 4) | (s[1] >> 4)];
        *d++ = tbl[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = tbl[((s[2] & 0x3f))];

        s += 3;
        len -= 3;
    };

    if (len > 0) {
        *d++ = tbl[((s[0]) >> 2) & 0x3f];

        if (len == 1) {
            *d++ = tbl[((s[0] & 3) << 4)];
            if (padding) {
                *d++ = '=';
            };

        } else {
            *d++ = tbl[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = tbl[((s[1] & 0x0f) << 2)];
        };

        if (padding) {
            *d++ = '=';
        };
    };

    r = (intptr_t)(d - (uint8_t*)dst);

_end:
    return r;
};


intptr_t nai_encode_base64(
    char* d, size_t dstlen, const char* s, size_t srclen)
{
    static char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    return nai_encode_base64_impl(d, dstlen, s, srclen, tbl, 1);
};


intptr_t nai_encode_base64uri(
    char* d, size_t dstlen, const char* s, size_t srclen)
{
    static char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    return nai_encode_base64_impl(d, dstlen, s, srclen, tbl, 0);
};


static intptr_t nai_decode_base64_impl(
    char* dst, size_t dstlen, 
    const char* src, size_t srclen, const uint8_t* tbl)
{
    intptr_t r;
    intptr_t n;
    intptr_t len;
    uint8_t ch;
    uint8_t* d;
    uint8_t* s;


    s = (uint8_t*)src;
    d = (uint8_t*)dst;
    len = srclen;
    if (len < 0) {
        len = nai_strlen(src);
    };

    if (dstlen < (size_t)nai_decode_base64_len(len)) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    n = 0;
    for ( ; n < len; n ++) {
        ch = s[n];
        if (ch == '=') {
            break;
        };

        if (tbl[ch] == 77) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    if (len % 4 == 1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    while (len >= 4) {
        *d++ = (tbl[s[0]] << 2 | tbl[s[1]] >> 4);
        *d++ = (tbl[s[1]] << 4 | tbl[s[2]] >> 2);
        *d++ = (tbl[s[2]] << 6 | tbl[s[3]]);

        s += 4;
        len -= 4;
    };

    if (len > 1) {
        *d++ = (tbl[s[0]] << 2 | tbl[s[1]] >> 4);
    }

    if (len > 2) {
        *d++ = (tbl[s[1]] << 4 | tbl[s[2]] >> 2);
    }

    r = (intptr_t)(d - (uint8_t*)dst);

_end:
    return r;
}


intptr_t nai_decode_base64(
    char* d, size_t dstlen, const char* s, size_t srclen)
{
    static uint8_t tbl[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return nai_decode_base64_impl(d, dstlen, s, srclen, tbl);
};


intptr_t nai_decode_base64uri(
    char* d, size_t dstlen, const char* s, size_t srclen)
{
    static uint8_t tbl[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 63,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return nai_decode_base64_impl(d, dstlen, s, srclen, tbl);
};



#endif


