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
/// @file       nai_sockaddr.c
/// @brief      
/// @details
/// @date       2020-12-03
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_socket.h"
#include "nai/os/nai_netif.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include <stdlib.h>



intptr_t nai_wtodec(char* dst, intptr_t len, uint16_t i)
{
    nai_int_t n;
    nai_int_t m;
    nai_int_t u;
    char* b;


    u = i;
    if (u >= 10000) {
        n = 4;
    } else if (u >= 1000) {
        n = 3;
    } else if (u >= 100) {
        n = 2;
    } else if (u >= 10) {
        n = 1;
    } else {
        n = 0;
    };

    b = dst;
    if (n + 1 > len) {
        goto _end;
    };

    m = n;
    b = b + n;
    for ( ; m > 0; m --) {
        b[0] = '0' + u % 10;
        b --;
        u /= 10;
    };

    b[0] = '0' + u;

    /* null terminated */
    if (n + 1 < len) {
        b += n + 1;
        b[0] = 0;
    };

_end:
    return n + 1;
};


intptr_t nai_wtohex(char* dst, intptr_t len, uint16_t i)
{
    nai_int_t n;
    nai_int_t c;
    char* b;
    const char* hex = "0123456789abcdef";


    b = dst;
    n = sizeof(i)*2 - 1;
    for ( ; n >= 0; n --) {
        c = (i >> (n*4)) & 0xf;
        if (c == 0) {
            continue;
        };

        if (n + 1 > len) {
            goto _end;
        };

        b[0] = hex[c];
        b ++;
        n --;
        break;
    };
    for ( ; n >= 0; n --) {
        c = (i >> (n*4)) & 0xf;
        b[0] = hex[c];
        b ++;
    };

    /* null terminated */
    if (b - dst < len) {
        b[0] = 0;
    };

_end:
    return b - dst;
};



//////////////////////////////////////////////////////////////////////////////
// socket address


#if (NAI_HAVE_SOCKADDR_IN6)
#if !defined(IN6_IS_ADDR_V4MAPPED)

#ifndef s6_word
#define s6_word s6_words
#endif

#define IN6_IS_ADDR_V4MAPPED(a) (                       \
    (((a)->s6_word[0]) == 0) &&                         \
    (((a)->s6_word[1]) == 0) &&                         \
    (((a)->s6_word[2]) == 0) &&                         \
    (((a)->s6_word[3]) == 0) &&                         \
    (((a)->s6_word[4]) == 0) &&                         \
    (((a)->s6_word[5]) == 0xFFFF)                       \
)                                                       \

#endif
#endif



static intptr_t nai_inet_ntop4(const uint8_t* name, char* buf, size_t len)
{
    intptr_t r;
    nai_int_t n;
    char tmp[16]; /* max space of inet */
    char* dst;
    uint8_t u;


    if (len < sizeof(tmp)) {
        dst = tmp;
    } else {
        dst = buf;
    };

    for (n = 0; n < 4; n ++) {
        u = name[n];
        if (u > 99) {
            dst[0] = '0' + u/100;
            dst ++;
            u %= 100;
            dst[0] = '0' + u/10;
            dst ++;
            u %= 10;
        } else if (u > 9) {
            dst[0] = '0' + u/10;
            dst ++;
            u %= 10;
        };

        dst[0] = '0' + u;
        dst ++;
        if (n < 3) {
            dst[0] = '.';
            dst ++;
        };
    };

    dst[0] = '\0';
    /* dst ++; */
    if (len < sizeof(tmp)) {
        r = dst - tmp;
        if (r <= (intptr_t)len) {
            nai_memcpy(buf, tmp, r + (r < (intptr_t)len));
        } else {
            nai_errno = ERANGE;
        };
    } else {
        r = dst - buf;
    };

    return r;
};


static intptr_t nai_inet_ntop6(const uint8_t* name, char* buf, size_t len)
{
    intptr_t r;
    nai_int_t n;
    char tmp[5*6+4*4]; /* max space: 'ffff:' * 6 + '255.' * 4 */
    char* dst;
    uint32_t max;
    uint32_t count;
    uint32_t zero;
    uint32_t last;


    if (len < sizeof(tmp)) {
        dst = tmp;
    } else {
        dst = buf;
    };

    zero = -1;
    last = -1;
    max = 1;
    count = 0;

    for (n = 0; n < 16; n += 2) {

        if (name[n] || name[n + 1]) {
            if (max < count) {
                max = count;
                zero = last;
            };

            count = 0;
            continue;
        };

        if (count ++ == 0) {
            last = n;
        };
    };


    if (max < count) {
        max = count;
        zero = last;
    };

    count = 16;

    if (zero == 0) {
        if ((max == 5 && name[10] == 0xff && name[11] == 0xff) || 
            (max == 6) || 
            (max == 7 && name[14] != 0 && name[15] != 1)) {
            count = 12;
        };

        dst[0] = ':';
        dst ++;
    };

    for (n = 0; n < (nai_int_t)count; n += 2) {

        if (n == (nai_int_t)zero) {
            dst[0] = ':';
            dst ++;
            n += (max - 1) * 2;
            continue;
        };

        dst += nai_wtohex(dst, 4, name[n] * 256 + name[n + 1]);

        if (n < 14) {
            dst[0] = ':';
            dst ++;
        };
    };

    if (count == 12) {
        dst += nai_inet_ntop4(name+12, dst, 16);
    } else {
        dst[0] = '\0';
        /* dst ++; */
    };

    if (len < sizeof(tmp)) {
        r = dst - tmp;
        if (r <= (intptr_t)len) {
            nai_memcpy(buf, tmp, r + (r < (intptr_t)len));
        } else {
            nai_errno = ERANGE;
        };
    } else {
        r = dst - buf;
    };

    return r;
};


const char* nai_inet_ntop(
    nai_int_t family, const void* name, char* buf, size_t buflen)
{
    intptr_t r;
    char* dst;


    switch (family) {
    case AF_INET:
        r = nai_inet_ntop4(name, buf, buflen);
        break;

#if defined(AF_INET6)
    case AF_INET6:
        r = nai_inet_ntop6(name, buf, buflen);
        break;
#endif

    default:
        nai_errno = EAFNOSUPPORT;
        return 0;
    };

    if (r >= (intptr_t)buflen) {
        dst = 0;
    } else {
        dst = buf;
    };
    return dst;
};



static nai_int_t nai_inet_pton4(const char* buf, size_t len, void* name)
{
    nai_int_t r;
    nai_int_t word;
    nai_int_t wordlen;
    nai_int_t count;
    uint8_t ch;
    uint8_t* dst;
    const char* p;
    const char* pend;


    p = buf;
    pend = len != (size_t)-1 ? (p + len) : 0;
    word = 0;
    wordlen = 0;
    count = 0;
    dst = name;

    for (;;) {
        if (pend) {
            if (p >= pend) {
                break;
            };
            ch = *p ++;
        } else {
            ch = *p ++;
            if (ch == 0) {
                p --;
                break;
            };
        };

        if (ch >= '0' && ch <= '9') {
            word *= 10;
            word += ch - '0';
            wordlen ++;
            if (wordlen > 3) {
                r = 0;
                goto _end;
            };
        } else if (ch == '.') {
            if (wordlen <= 0) {
                r = 0;
                goto _end;
            };
            dst[0] = (uint8_t)word;
            dst ++;
            word = 0;
            wordlen = 0;
            count ++;
            if (count > 3) {
                r = 0;
                goto _end;
            };
        } else {
            r = 0;
            goto _end;
        };
    };
    if (wordlen <= 0) {
        r = 0;
        goto _end;
    };

    dst[0] = (uint8_t)word;
    dst ++;
    r = (nai_int_t)(p - buf);

_end:
    return r;
};


static nai_int_t nai_inet_pton6(const char* buf, size_t len, void* name)
{
    nai_int_t r;
    nai_int_t word;
    nai_int_t wordlen;
    nai_int_t move;
    nai_int_t count;
    uint8_t ch;
    uint8_t* src;
    uint8_t* dst;
    uint8_t* zero;
    const char* p;
    const char* pend;
    const char* digit;


    p = buf;
    pend = len != (size_t)-1 ? (p + len) : 0;
    word = 0;
    wordlen = 0;
    count = 0;
    dst = name;
    zero = 0;
    digit = 0;


    if (p[0] == ':') {
        p ++;
    };

    for (;;) {
        if (pend) {
            if (p >= pend) {
                break;
            };
            ch = *p ++;
        } else {
            ch = *p ++;
            if (ch == 0) {
                p --;
                break;
            };
        };

        if (ch == ':') {
            if (wordlen) {
                count ++;
                if (count < 8) {
                    digit = p;
                    dst[0] = (uint8_t)(word >> 8);
                    dst[1] = (uint8_t)(word & 0xff);
                    dst += 2;

                    word = 0;
                    wordlen = 0;
                    continue;
                };

            } else {
                /* rfc2373: The "::" can only appear once in an address */
                if (zero == 0) {
                    zero = dst;
                    digit = p;
                    continue;
                };
            };

            r = 0;
            goto _end;
        };

        if (ch == '.') {
            if (wordlen <= 0) {
                r = 0;
                goto _end;
            };
            if (count > 6 || digit == 0) {
                r = 0;
                goto _end;
            }

            r = nai_inet_pton4(digit, 
                (nai_int_t)(pend ? pend-digit : -1), dst);
            if (r == 0) {
                goto _end;
            }

            dst += 4;
            count += 2;
            goto _move;
        };

        if (ch == '%') {
            p --;
            break;
        };

        wordlen ++;
        if (wordlen > 4) {
            r = 0;
            goto _end;
        };

        if (ch >= '0' && ch <= '9') {
            word = word * 16 + (ch - '0');
            continue;
        };

        ch |= 0x20;

        if (ch >= 'a' && ch <= 'f') {
            word = word * 16 + (ch - 'a') + 10;
            continue;
        };

        r = 0;
        goto _end;
    };

    if (wordlen == 0 && zero == 0) {
        r = 0;
        goto _end;
    };

    if (word) {
        dst[0] = (uint8_t)(word >> 8);
        dst[1] = (uint8_t)(word & 0xff);
        dst += 2;
        count ++;
    };

_move:
    if (count < 8) {
        if (zero == 0) {
            r = 0;
            goto _end;
        };

        /* move bytes */
        move = 8 - count;
        move *= 2;

        /* move to end */
        src = dst - 1;
        dst = src + move;
        while (src >= zero) {
            *dst-- = *src--;
        };
        nai_memset(zero, 0, move);

    } else {
        if (zero) {
            r = 0;
            goto _end;
        };
    };

    r = (nai_int_t)(p - buf);

_end:
    return r;
};


nai_int_t nai_inet_pton(
    nai_int_t family, const char* buf, size_t buflen, void* name)
{
    nai_int_t r;


    switch (family) {
    case AF_INET:
        r = nai_inet_pton4(buf, buflen, name) != 0;
        break;

#if defined(AF_INET6)
    case AF_INET6:
        r = nai_inet_pton6(buf, buflen, name) != 0;
        break;
#endif

    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        break;
    };

    return r;
};


intptr_t nai_sockaddr_ntop(
    const nai_sockaddr_t* name, nai_int_t namelen, 
    char* buf, size_t buflen, nai_int_t with_opt)
{
    intptr_t r;
    intptr_t n;
    intptr_t len;
#if (NAI_HAVE_SOCKADDR_UN)
    nai_sockaddr_un_t* un;
#endif
#if (NAI_HAVE_SOCKADDR_IN)
    nai_sockaddr_in4_t* in4;
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
    uint32_t scope_id;
#endif


    len = buflen;
    switch (name->sa_family) {
    case AF_INET:
        in4 = (nai_sockaddr_in4_t*)name;
        if (namelen < (nai_int_t)sizeof(*in4)) {
            nai_errno = EINVAL;
            r = -1;
            break;
        };
        r = nai_inet_ntop4((uint8_t*)&in4->sin_addr, buf, len);
        if (r < 0) {
            nai_errno = EINVAL;
            r = -1;
            break;
        };
        if (in4->sin_port == 0) {
            with_opt &= ~NAI_ADDR_PORT;
        };
        if (with_opt & NAI_ADDR_PORT) {

            n = r;
            buf += n;
            len -= n;

            if (len > 0) {
                buf[0] = ':';
                buf ++;
                len --;
            };
            n ++;

            r = nai_wtodec(buf, 
                len > 0 ? len : 0, nai_ntohs(in4->sin_port));
            if (r < 0) {
                break;
            };

            r += n;
        };
        if (r >= (intptr_t)buflen) {
            nai_errno = ERANGE;
        };
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        in6 = (nai_sockaddr_in6_t*)name;
        if (namelen < (nai_int_t)sizeof(*in6)) {
            nai_errno = EINVAL;
            r = -1;
            break;
        };

        n = 0;
        if (with_opt & NAI_ADDR_PORT) {
            if (len > 0) {
                buf[0] = '[';
                buf ++;
                len --;
            };
            n ++;
        };

        r = nai_inet_ntop6((uint8_t*)&in6->sin6_addr, buf, len);
        if (r < 0) {
            nai_errno = EINVAL;
            r = -1;
            break;
        };

        buf += r;
        len -= r;
        r += n;

        if (in6->sin6_scope_id && (
            with_opt & (NAI_ADDR_ZONE|NAI_ADDR_ZINDEX))) {

            n = r;

            if (len > 0) {
                buf[0] = '%';
                buf ++;
                len --;
            };
            n ++;

            /* scope_id = nai_ntohl(in6->sin6_scope_id); */
            scope_id = in6->sin6_scope_id;

            if (with_opt & NAI_ADDR_ZONE) {
                r = nai_if_indextoname(scope_id, 
                    buf, len, !(with_opt & NAI_ADDR_ZINDEX));
            } else if (scope_id < 0x10000) {
                r = nai_wtodec(buf, 
                    len > 0 ? len : 0, (uint16_t)scope_id);
            } else {
                r = nai_snprintf(buf, 
                    len > 0 ? len : 0, "%u", scope_id);
            };
            if (r < 0) {
                break;
            };

            buf += r;
            len -= r;
            r += n;
        };

        if (with_opt & NAI_ADDR_PORT) {
            n = r;

            if (len > 0) {
                buf[0] = ']';
                buf ++;
                len --;
            };
            n ++;

            if (in6->sin6_port != 0) {
                if (len > 0) {
                    buf[0] = ':';
                    buf ++;
                    len --;
                };
                n ++;

                r = nai_wtodec(buf, 
                    len > 0 ? len : 0, nai_ntohs(in6->sin6_port));
                if (r < 0) {
                    break;
                };

                n += r;

            } else if (len > 0) {
                buf[0] = '\0';
                buf ++;
                len --;
            };

            r = n;
        };

        if (r >= (intptr_t)buflen) {
            nai_errno = ERANGE;
        };
        break;
#endif


#if (NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        un = (nai_sockaddr_un_t*)name;

        if (len < 0) {
            len = 0;
        };
        if (namelen < (nai_int_t)nai_offsetof(nai_sockaddr_un_t, sun_path)) {
            r = nai_snprintf(buf, len, "unix:");
        } else {
            n = nai_strnlen(un->sun_path, 
                namelen - nai_offsetof(nai_sockaddr_un_t, sun_path));

            r = nai_snprintf(buf, len, 
                "unix:%.*s", (nai_int_t)n, un->sun_path);
        };
        if (r < 0) {
            break;
        };

        if (r >= (intptr_t)buflen) {
            nai_errno = ERANGE;
        };
        break;
#endif

    default:

        nai_errno = EAFNOSUPPORT;
        r = -1;
        break;
    };

    return r;
};


nai_int_t nai_inet_ptoc(const char* buf, size_t buflen, nai_cidr_t* p)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t bits;
    size_t addrlen;
    size_t masklen;
    const char* addr;
    const char* mask;
    uint8_t* data;


    if ((intptr_t)buflen < 0) {
        buflen = nai_strlen(buf);
    };

    mask = nai_strnchr(buf, buflen, '/');
    if (mask) {
        addr = buf;
        addrlen = mask - buf;
        mask += 1;
        masklen = buflen - addrlen - 1;
    } else {
        addr = buf;
        addrlen = buflen;
        masklen = 0;
    };

    r = nai_inet_pton(AF_INET, addr, addrlen, &p->in4.addr);
    if (r > 0) {
        p->af = AF_INET;
        goto _mask;
    };

#if (NAI_HAVE_SOCKADDR_IN6)
    if (addr[0] == '[' && addr[addrlen-1] == ']') {
        addr ++;
        addrlen -= 2;
    };
    r = nai_inet_pton(AF_INET6, addr, addrlen, &p->in6.addr);
    if (r > 0) {
        p->af = AF_INET6;
        goto _mask;
    };
#endif

    nai_errno = EINVAL;
    r = -1;
    goto _end;


_mask:
    if (mask != 0) {
        r = nai_atoi(&bits, mask, masklen, 0);
        if (r < 0) {
            goto _maskaddr;
        };
        if (bits < 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    switch (p->af) {
    case AF_INET:
        if (mask == 0) {
            nai_memset(&p->in4.mask, -1, sizeof(p->in4.mask));
            break;
        };
        if (bits > 32) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        data = (uint8_t*)&p->in4.mask;
        for (n = 0; n < (nai_int_t)sizeof(p->in4.mask); n ++) {
            if (bits >= 8) {
                bits -= 8;
                data[n] = 0xff;
            } else {
                data[n] = 0xff << (8-bits);
                bits = 0;
            };
        };
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        if (mask == 0) {
            nai_memset(&p->in6.mask, -1, sizeof(p->in6.mask));
            break;
        };
        if (bits > 128) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        data = (uint8_t*)&p->in6.mask;
        for (n = 0; n < (nai_int_t)sizeof(p->in6.mask); n ++) {
            if (bits >= 8) {
                bits -= 8;
                data[n] = 0xff;
            } else {
                data[n] = 0xff << (8-bits);
                bits = 0;
            };
        };
        break;
#endif
    default:
        break;
    };

    r = 0;
    goto _prep;


_maskaddr:
    switch (p->af) {
    case AF_INET:
        r = nai_inet_pton(AF_INET, mask, masklen, &p->in4.mask);
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        r = nai_inet_pton(AF_INET6, mask, masklen, &p->in6.mask);
        break;
#endif
    default:
        break;
    };

    if (r < 0) {
        goto _end;
    };

_prep:
    switch (p->af) {
    case AF_INET:
        p->in4.addr.s_addr &= p->in4.mask.s_addr;
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        for (n = 0; n < (nai_int_t)nai_countof(p->in6.addr.s6_addr); n ++) {
            p->in6.addr.s6_addr[n] &= p->in6.mask.s6_addr[n];
        };
        break;
#endif
    default:
        break;
    };


_end:
    return r;
};


nai_int_t nai_sockaddr_pton(const char* buf, 
    size_t blen, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t v;
    nai_int_t len;
    intptr_t buflen = blen;
    char* p;
    char* token;
    uint8_t addr[16];
    uint16_t* port = 0;
#if (NAI_HAVE_SOCKADDR_UN)
    nai_sockaddr_un_t* un;
#endif
#if (NAI_HAVE_SOCKADDR_IN)
    nai_sockaddr_in4_t* in4;
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
    uint32_t scope_id;
#endif


    len = *namelen;

#if (NAI_HAVE_SOCKADDR_UN)
    if ((buflen < 0 || buflen >= 5) && 
        strncmp(buf, "unix:", 5) == 0) {

        n = buflen < 0 ? ((nai_int_t)nai_strlen(buf+5)) : (buflen-5);
        r = nai_offsetof(nai_sockaddr_un_t, sun_path);
        r += n + 1;
        if (r > len || r > (nai_int_t)sizeof(*un)) {
            nai_errno = ERANGE;
            goto _end;
        };

        un = (nai_sockaddr_un_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
        un->sun_len = r;
#endif
        un->sun_family = AF_UNIX;
        nai_memcpy(un->sun_path, buf+5, n);
        un->sun_path[n] = 0; /* force to end with a terminator */

        namelen[0] = r;
        goto _end;
    };
#endif

#if (NAI_HAVE_SOCKADDR_IN6)
    if (buf[0] == '[') {

        if (buflen < 0) {
            token = nai_strchr(buf+1, ']');
        } else {
            token = nai_strnchr(buf+1, buflen-1, ']');
        };
        if (token == 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = sizeof(*in6);
        if (len < r) {
            nai_errno = ERANGE;
            goto _end;
        };

        p = (char*)buf + 1;
        in6 = (nai_sockaddr_in6_t*)name;
        r = nai_inet_pton6(p, (nai_int_t)(token - p), &in6->sin6_addr);
        if (r == 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if (r < (nai_int_t)(token - p)) {
            p += r + 1;
            n = nai_if_nametoindex(p, (nai_int_t)(token - p), &scope_id);
            if (n < 0) {
                r = n;
                goto _end;
            };

            /* scope_id = nai_htonl(scope_id); */
        } else {
            scope_id = 0;
        };

        r = sizeof(*in6);

#if (NAI_HAVE_SOCKADDR_LEN)
        in6->sin6_len = r;
#endif
        in6->sin6_family = AF_INET6;
        in6->sin6_port = 0;
        in6->sin6_scope_id = scope_id;
        in6->sin6_flowinfo = 0;

        if (token[1] == ':') {
            token += 2;
            port = &in6->sin6_port;
        };
        r = sizeof(*in6);

    } else {

        r = nai_inet_pton6(buf, buflen, addr);
        if (r > 0) {
            if (buflen < 0) {
                n = buf[r] == '%';
            } else {
                n = buflen > r;
            };
            if (n) {
                /* n = nai_atoi(&scope_id, buf + r + 1, buflen - r - 1, 0); */
                n = nai_if_nametoindex(buf + r + 1, buflen - r - 1, &scope_id);
                if (n < 0) {
                    r = n;
                    goto _end;
                };

                /* scope_id = nai_htonl(scope_id); */
            } else {
                scope_id = 0;
            };

            r = sizeof(*in6);
            if (len < r) {
                nai_errno = ERANGE;
                goto _end;
            };

            in6 = (nai_sockaddr_in6_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
            in6->sin6_len = r;
#endif
            in6->sin6_family = AF_INET6;
            in6->sin6_port = 0;
            in6->sin6_scope_id = scope_id;
            in6->sin6_flowinfo = 0;
            nai_memcpy(&in6->sin6_addr, addr, sizeof(in6->sin6_addr));
            namelen[0] = r;
            goto _end;
        };

#else
    {
#endif

        if (buflen < 0) {
            token = nai_strchr(buf, ':');
        } else {
            token = nai_strnchr(buf, buflen, ':');
        };
        if (token == 0) {
            r = nai_inet_pton4(buf, buflen, addr);
        } else {
            r = nai_inet_pton4(buf, (nai_int_t)(token-buf), addr);
        };
        if (r == 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = sizeof(*in4);
        if (len < r) {
            nai_errno = ERANGE;
            goto _end;
        };

        in4 = (nai_sockaddr_in4_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
        in4->sin_len = r;
#endif
        in4->sin_family = AF_INET;
        in4->sin_port = 0;
        nai_memcpy(&in4->sin_addr, addr, sizeof(in4->sin_addr));
        nai_memset(in4->sin_zero, 0, sizeof(in4->sin_zero));

        if (token) {
            token += 1;
            port = &in4->sin_port;
        };
    };

    namelen[0] = r;

    if (port) {
        if (buf[0] == 0) {
            v = 0;
        } else {
            n = nai_atoi(&v, token, 
                buflen < 0 ? buflen : buflen - (token-buf), 0);
            if (n < 0) {
                r = n;
                goto _end;
            };
        };

        port[0] = nai_htons(v);
    };


_end:
    return r;
};


#if defined(_WIN32)


#include "win/nai_windows.h"
#include "win/nai_wsock.h"
#include <ws2tcpip.h>


#ifndef NAI_HAVE_GETADDRINFO
#define NAI_HAVE_GETADDRINFO    1
#endif


uint16_t nai_ntohs(uint16_t s)
{
    return ntohs(s);
}


uint32_t nai_ntohl(uint32_t l)
{
    return ntohl(l);
}


uint16_t nai_htons(uint16_t s)
{
    return htons(s);
}


uint32_t nai_htonl(uint32_t l)
{
    return htonl(l);
}


static nai_int_t nai_errno_from_neterr(nai_int_t n)
{
    nai_int_t e;


    switch (n) {
    case WSATRY_AGAIN:
        e = EAGAIN;
        break;
    case WSAEINVAL:
        e = EINVAL;
        break;
    case WSANO_RECOVERY:
        e = NAI_EFAILED;
        break;
    case WSAEAFNOSUPPORT:
        e = EPROTONOSUPPORT;
        break;
    case WSA_NOT_ENOUGH_MEMORY:
        e = ENOMEM;
        break;
    case WSANO_DATA:
        e = ENOENT;
        break;
    case WSAHOST_NOT_FOUND:
        e = ENOENT;
        break;
    case WSATYPE_NOT_FOUND:
        e = ENOENT;
        break;
    case WSAESOCKTNOSUPPORT:
        e = ENOTSUP;
        break;
    default:
        e = nai_errno_from_oserr(n);
        break;
    };

    return e;
};


#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


#if (NAI_HAVE_GETADDRINFO)


static nai_int_t nai_errno_from_neterr(nai_int_t n)
{
    nai_int_t e;


    switch (n) {
    case EAI_AGAIN:
        e = EAGAIN;
        break;
    case EAI_BADFLAGS:
        e = EINVAL;
        break;
    case EAI_MEMORY:
        e = ENOMEM;
        break;
#if defined(EAI_NODATA)
    case EAI_NODATA:
#endif
    case EAI_NONAME:
        e = ENOENT;
        break;
    case EAI_FAMILY:
        e = EAFNOSUPPORT;
        break;
#if defined(EAI_ADDRFAMILY)
    case EAI_ADDRFAMILY:
        e = EADDRNOTAVAIL;
        break;
#endif
    case EAI_SYSTEM:
        e = nai_errno;
        break;
    default:
        e = NAI_EFAILED;
        break;
    };

    return e;
};


#endif


#endif


#if (NAI_HAVE_GETADDRINFO)


intptr_t nai_sockaddr_list(const char* host, 
    const char* serv, void* buf, size_t buflen)
{
    intptr_t r;
    intptr_t n;
    struct addrinfo hints;
    struct addrinfo* sult;
    struct addrinfo* next;
    nai_socknbuf_t* list;


    nai_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


    /* lookup host and port */
    r = getaddrinfo(host, serv, &hints, &sult);
    if (r != 0) {
#if defined(_WIN32)
        nai_errno = nai_errno_from_neterr(GetLastError());
#else
        nai_errno = nai_errno_from_neterr(r);
#endif
        r = -1;
        goto _end;
    };

    /* fill address list */
    n = 0;
    list = (nai_socknbuf_t*)buf;
    next = sult;
    for ( ; next; next = next->ai_next) {
        n += nai_offsetof(nai_socknbuf_t, 
            addr) + nai_align(next->ai_addrlen, sizeof(intptr_t));
        if (n > (intptr_t)(buflen - sizeof(nai_int_t))) {
            continue;
        };

        list->len = (nai_int_t)next->ai_addrlen;
        nai_memcpy(&list->addr, next->ai_addr, next->ai_addrlen);
        list = (nai_socknbuf_t*)((uint8_t*)buf + n);
    };

    freeaddrinfo(sult);


    if (buflen >= sizeof(list->len)) {
        list->len = 0;
    };

    r = n + sizeof(list->len);
    if (r == 0) {
        nai_errno = ENOENT;
        r = -1;
    } else {
        if (r > (intptr_t)buflen) {
            nai_errno = ERANGE;
        };
    };

_end:
    return r;
};


#else


intptr_t nai_sockaddr_list(const char* host, 
    const char* serv, void* buf, size_t buflen)
{
    nai_int_t i;
    intptr_t r;
    intptr_t n;
    uint16_t port;
    nai_socknbuf_t* list;
    nai_sockaddr_in4_t* in;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* in6;
#endif

    struct hostent* he;
    struct servent* se;

#if !defined(_WIN32)
    nai_int_t error;
#endif
#if (NAI_HAVE_GETHOSTBYNAME_R)
    struct servent ss;
    struct hostent hs;
    char tmp[512];
#endif


    /* lookup serv */
    if (serv == 0) {
        port = 0;
    } else {
#if (NAI_HAVE_GETHOSTBYNAME_R)
        se = 0;
        error = getservbyname_r(serv, 0, &ss, tmp, sizeof(tmp)-1, &se);
#else
        se = getservbyname(serv, 0);
#endif

        if (se == 0) {
#if defined(_WIN32)
            nai_errno = nai_errno_from_neterr(GetLastError());
#else
            nai_errno = EINVAL;
#endif
            r = -1;
            goto _end;
        };

        port = se->s_port;
    };


    /* lookup host */
#if (NAI_HAVE_GETHOSTBYNAME_R)
    he = 0;
    gethostbyname_r(host, &hs, tmp, sizeof(tmp)-1, &he, &error);
#else
    he = gethostbyname(host);
#endif

    if (he == 0) {
#if defined(_WIN32)
        nai_errno = nai_errno_from_neterr(GetLastError());
#else

#if !(NAI_HAVE_GETHOSTBYNAME_R)
        error = h_errno;
#endif
        switch (error) {
        case HOST_NOT_FOUND:
            nai_errno = ENOENT;
            break;
        case NO_DATA:
            nai_errno = ENOENT;
            break;
        case TRY_AGAIN:
            nai_errno = EAGAIN;
            break;
        case NO_RECOVERY:
        default:
            nai_errno = NAI_EFAILED;
            break;
        };
#endif
        r = -1;
        goto _end;
    };


    /* fill address list */
    i = 0;
    n = 0;
    list = (nai_socknbuf_t*)buf;
    switch (he->h_addrtype) {
    case AF_INET:
        for ( ; he->h_addr_list[i]; i ++) {
            n += nai_offsetof(nai_socknbuf_t, 
                addr) + nai_align(sizeof(*in), sizeof(intptr_t));
            if (n > (intptr_t)(buflen - sizeof(nai_int_t))) {
                continue;
            };

            list->len = sizeof(*in);
            in = (nai_sockaddr_in4_t*)&list->addr;
#if (NAI_HAVE_SOCKADDR_LEN)
            in->sin_len = list->len;
#endif
            in->sin_family = AF_INET;
            in->sin_port = port;
            nai_memset(&in->sin_zero, 0, sizeof(in->sin_zero));
            nai_memcpy(&in->sin_addr, he->h_addr_list[i], sizeof(in->sin_addr));

            list = (nai_socknbuf_t*)((uint8_t*)buf + n);
        };
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        for ( ; he->h_addr_list[i]; i ++) {
            n += nai_offsetof(nai_socknbuf_t, 
                addr) + nai_align(sizeof(*in6), sizeof(intptr_t));
            if (n > (intptr_t)(buflen - sizeof(nai_int_t))) {
                continue;
            };

            list->len = sizeof(*in6);
            in6 = (nai_sockaddr_in6_t*)&list->addr;
#if (NAI_HAVE_SOCKADDR_LEN)
            in6->sin6_len = list->len;
#endif
            in6->sin6_family = AF_INET6;
            in6->sin6_port = port;
            in6->sin6_scope_id = 0;
            in6->sin6_flowinfo = 0;
            nai_memcpy(&in6->sin6_addr, 
                he->h_addr_list[i], sizeof(in6->sin6_addr));

            list = (nai_socknbuf_t*)((uint8_t*)buf + n);
        };
        break;
#endif
    default:
        break;
    };


    if (n == 0) {
        nai_errno = ENOENT;
        r = -1;
    } else {
        r = n + sizeof(list->len);
        if (r + sizeof(list->len) <= buflen) {
            list->len = 0;
        };
        if (r > (intptr_t)buflen) {
            nai_errno = ERANGE;
        };
    };


_end:
    return r;
};


#endif


nai_int_t nai_sockaddr_compare(
    const nai_sockaddr_t* sa1, nai_int_t len1, 
    const nai_sockaddr_t* sa2, nai_int_t len2)
{
    nai_int_t r;


    r = sa1->sa_family - sa2->sa_family;
    if (r != 0) {
        goto _end;
    };

    r = len1 - len2;
    if (r != 0) {
        goto _end;
    };

    switch (sa1->sa_family) {
    case AF_INET:
        r = ((nai_sockaddr_in4_t*)sa1)->sin_port - 
            ((nai_sockaddr_in4_t*)sa2)->sin_port;
        if (r != 0) {
            break;
        };

        r = nai_memcmp(
            &((nai_sockaddr_in4_t*)sa1)->sin_addr, 
            &((nai_sockaddr_in4_t*)sa2)->sin_addr, 
            sizeof(nai_addr_in4_t));

        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        r = ((nai_sockaddr_in6_t*)sa1)->sin6_port - 
            ((nai_sockaddr_in6_t*)sa2)->sin6_port;
        if (r != 0) {
            break;
        };

        r = ((nai_sockaddr_in6_t*)sa1)->sin6_scope_id - 
            ((nai_sockaddr_in6_t*)sa2)->sin6_scope_id;
        if (r != 0) {
            break;
        };

        r = ((nai_sockaddr_in6_t*)sa1)->sin6_flowinfo - 
            ((nai_sockaddr_in6_t*)sa2)->sin6_flowinfo;
        if (r != 0) {
            break;
        };

        r = nai_memcmp(
            &((nai_sockaddr_in6_t*)sa1)->sin6_addr, 
            &((nai_sockaddr_in6_t*)sa2)->sin6_addr, 
            sizeof(nai_addr_in6_t));
        break;
#endif
#if (NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        r = nai_memcmp(
            &((nai_sockaddr_un_t*)sa1)->sun_path, 
            &((nai_sockaddr_un_t*)sa2)->sun_path, 
            len1 - nai_offsetof(nai_sockaddr_un_t, sun_path));
        break;
#endif
    default:
        r = nai_memcmp(&sa1->sa_data, &sa2->sa_data, 
            len1-nai_offsetof(nai_sockaddr_t, sa_data));
        break;
    };

_end:
    return r;
};


nai_int_t nai_sockaddr_match(
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const nai_cidr_t* cidr)
{
    nai_int_t r;
    nai_addr_in4_t* in4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_int_t l;
    nai_addr_in6_t* in6;
#endif


    (void)namelen;

    switch (cidr->af) {
    case AF_INET:
        if (name->sa_family == AF_INET) {
            in4 = &((nai_sockaddr_in4_t*)name)->sin_addr;
#if (NAI_HAVE_SOCKADDR_IN6)
        } else if (name->sa_family == AF_INET6) {
            in6 = &((nai_sockaddr_in6_t*)name)->sin6_addr;
            if (!IN6_IS_ADDR_V4MAPPED(in6)) {
                r = 0;
                break;
            };

            in4 = (nai_addr_in4_t*)&in6->s6_addr[12];
#endif
        } else {
            r = 0;
            break;
        };
        if ((cidr->in4.mask.s_addr & cidr->in4.addr.s_addr) != 
            (cidr->in4.mask.s_addr & in4->s_addr)) {
            r = 0;
        } else {
            r = 1;
        };
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        if (name->sa_family != AF_INET6) {
            r = 0;
            break;
        };

        in6 = &((nai_sockaddr_in6_t*)name)->sin6_addr;
        for (l = 0; l < (nai_int_t)nai_countof(in6->s6_addr); l ++) {
            if ((cidr->in6.mask.s6_addr[l] & cidr->in6.addr.s6_addr[l]) != 
                (cidr->in6.mask.s6_addr[l] & in6->s6_addr[l])) {
                break;
            };
        };
        if (l < (nai_int_t)nai_countof(in6->s6_addr)) {
            r = 0;
        } else {
            r = 1;
        };
        break;
#endif
    default:
        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_sockaddr_mk_inet(nai_int_t af, 
    const void* addr, nai_int_t port, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_sockaddr_in4_t* sa4;
#if (NAI_HAVE_SOCKADDR_IN6)
    nai_sockaddr_in6_t* sa6;
#endif


    if (namelen == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (af) {
    case AF_INET:
        r = sizeof(nai_sockaddr_in4_t);
        if (r > namelen[0]) {
            nai_errno = ERANGE;
            goto _end;
        };

        sa4 = (nai_sockaddr_in4_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
        sa4->sin_len = r;
#endif
        sa4->sin_family = af;
        sa4->sin_port = nai_htons(port);
        nai_memset(&sa4->sin_zero, 0, sizeof(sa4->sin_zero));
        if (addr) {
            nai_memcpy(&sa4->sin_addr, addr, sizeof(sa4->sin_addr));
        } else {
            nai_memset(&sa4->sin_addr, 0, sizeof(sa4->sin_addr));
        };
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        r = sizeof(nai_sockaddr_in6_t);
        if (r > namelen[0]) {
            nai_errno = ERANGE;
            goto _end;
        };

        sa6 = (nai_sockaddr_in6_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
        sa6->sin6_len = r;
#endif
        sa6->sin6_family = af;
        sa6->sin6_port = nai_htons(port);
        sa6->sin6_scope_id = 0;
        sa6->sin6_flowinfo = 0;
        if (addr) {
            nai_memcpy(&sa6->sin6_addr, addr, sizeof(sa6->sin6_addr));
        } else {
            nai_memset(&sa6->sin6_addr, 0, sizeof(sa6->sin6_addr));
        };
        break;
#endif

    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        goto _end;
    };

    namelen[0] = r;

_end:
    return r;
};


nai_int_t nai_sockaddr_mk_unix(
    const char* path, nai_int_t len, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;

#if (NAI_HAVE_SOCKADDR_UN)
    nai_int_t n;
    nai_int_t term;
    nai_sockaddr_un_t* un;


    if (namelen == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    n = len < 0 ? ((nai_int_t)nai_strlen(path)+1) : (len+1);
    r = nai_offsetof(nai_sockaddr_un_t, sun_path);
    r += n;
    term = 1;
    if (r == sizeof(*un) + 1) {
        r -= 1;
        term = 0;
    };
    if (r > namelen[0]) {
        nai_errno = ERANGE;
        goto _end;
    };

    un = (nai_sockaddr_un_t*)name;
#if (NAI_HAVE_SOCKADDR_LEN)
    un->sun_len = r;
#endif
    un->sun_family = AF_UNIX;
    nai_memcpy(un->sun_path, path, n-1);
    if (term) {
        un->sun_path[n] = 0;
    };

    namelen[0] = r;

_end:
#else
    (void)path;
    (void)len;
    (void)name;
    (void)namelen;

    nai_errno = EAFNOSUPPORT;
    r = -1;
#endif

    return r;
};



#if (NAI_HAVE_SOCKADDR_UN)
static nai_sockaddr_info_t nai_unix = {
    sizeof(nai_sockaddr_un_t), 
    nai_offsetof(nai_sockaddr_un_t, sun_path), 
    sizeof(((nai_sockaddr_un_t*)0)->sun_path), 
    0, 
    0, 
};
#endif

#if (NAI_HAVE_SOCKADDR_IN)
static nai_sockaddr_info_t nai_inet = {
    sizeof(nai_sockaddr_in4_t), 
    nai_offsetof(nai_sockaddr_in4_t, sin_addr), 
    sizeof(nai_addr_in4_t), 
    nai_offsetof(nai_sockaddr_in4_t, sin_port), 
    0, 
};
#endif

#if (NAI_HAVE_SOCKADDR_IN6)
static nai_sockaddr_info_t nai_inet6 = {
    sizeof(nai_sockaddr_in6_t), 
    nai_offsetof(nai_sockaddr_in6_t, sin6_addr), 
    sizeof(nai_addr_in6_t), 
    nai_offsetof(nai_sockaddr_in6_t, sin6_port), 
    nai_offsetof(nai_sockaddr_in6_t, sin6_scope_id), 
};
#endif

const nai_sockaddr_info_t* nai_sockaddr_info(nai_int_t af)
{
    nai_sockaddr_info_t* r;


    switch (af) {
#if (NAI_HAVE_SOCKADDR_IN)
    case AF_INET:
        r = &nai_inet;
        break;
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        r = &nai_inet6;
        break;
#endif
#if (NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        r = &nai_unix;
        break;
#endif
    default:
        nai_errno = EAFNOSUPPORT;
        r = 0;
        break;
    };

    return r;
};



