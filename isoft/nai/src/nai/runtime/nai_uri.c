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
/// @file       nai_uri.c
/// @brief      
/// @details
/// @date       2021-01-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_thread.h"
#include <ctype.h>
#include <string.h>


enum {
    oct =           0x01,
    hex =           0x02, 
    dot =           0x04, 
};
enum {
    dotp =          0x01,
    slash =         0x02,
    question =      0x04,
    hashnil =       0x08,
    escapeScheme =  0x10,
    escapeHost =    0x20,
    escapePath =    0x40,
    escapeArgu =    0x80
};


typedef struct nai_uri_data_s {
    nai_int_t inited;
    nai_once_t once;
    uint8_t delims[256];
    uint8_t iplite[256];
} nai_uri_data_t;


static nai_uri_data_t nai_uri;


static void nai_uri_data_init()
{
    nai_int_t ch;
    uint8_t* delims;
    uint8_t* iplite;


    delims = nai_uri.delims;
    delims[0] = hashnil;
    delims['#'] = hashnil;
    delims['?'] = question;
    delims['.'] = dotp;
    delims['/'] = slash;
    delims['\\'] = slash;

    /* unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
     * reserved      = gen-delims / sub-delims
     * gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
     * sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
     *               / "*" / "+" / "," / ";" / "="
     * userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
     * host          = IP-literal / IPv4address / reg-name
     * port          = *DIGIT
     * reg-name      = *( unreserved / pct-encoded / sub-delims )
     * pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
     * query         = *( pchar / "/" / "?" )
     * fragment      = *( pchar / "/" / "?" )
     */

    for (ch = 0; ch < (nai_int_t)sizeof(nai_uri.delims); ch ++) {
        /* alpha, num and +-. */
        if (!isalnum(ch) && !strchr("+-.", ch)) {
            delims[ch] |= escapeScheme;
        };
        /* alpha, num and -._~!$&'()*+,;= */
        if (!isalnum(ch) &&
            !strchr("-._~!$&'()*+,;=", ch)) {
            delims[ch] |= escapeHost;
        };
        /* alpha, num and -._~/?#[]@!$&'()*+,;=% */
        if (!isalnum(ch) &&
            !strchr("-._~/?#[]@!$&'()*+,;=%", ch)) {
            delims[ch] |= escapePath;
        };
        if (ch < 32 || ch > 126 || strchr("", ch)) {
            delims[ch] |= escapeArgu;
        };
        /* if (ch < 32 || ch > 126 || */
        /*     strchr(";\\?@&=+$, <>#%\"{}|^[]`", ch)) { */
        /*     delims[ch] |= escapeArg;    [> uri args case, from atl <] */
        /* }; */
    };

    iplite = nai_uri.iplite;
    iplite['.'] = dot;

    for (ch = 0; ch < (nai_int_t)sizeof(nai_uri.iplite); ch ++) {
        if (ch >= '0' && ch <= '9') {
            iplite[ch] |= oct | hex;
        };
        if (ch >= 'a' && ch <= 'f') {
            iplite[ch] |= hex;
        };
        if (ch >= 'A' && ch <= 'F') {
            iplite[ch] |= hex;
        };
    };

    nai_memory_barrier();
    nai_uri.inited = 1;
};


void nai_uri_init(nai_uri_t* u)
{
    u->flags = 0;
    nai_str_setn(&u->scheme);
    nai_str_setn(&u->user);
    nai_str_setn(&u->password);
    nai_str_setn(&u->host);
    nai_str_setn(&u->hostname);
    nai_str_setn(&u->portname);
    nai_str_setn(&u->path);
    nai_str_setn(&u->query);
    nai_str_setn(&u->fragment);

    return;
};


nai_int_t nai_uri_parse(nai_uri_t* u, 
    const char* str, size_t len, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ipv6;
    nai_int_t user;
    nai_int_t port_invalid;
    nai_int_t strict;
    uint8_t ch;
    char* p;
    char* pend;
    char* start;
    char* colon;
    const uint8_t* delims;
    const uint8_t* iplite;


    if (nai_uri.inited == 0) {
        nai_once(&nai_uri.once, nai_uri_data_init);
    };
    delims = nai_uri.delims;


    p = (char*)str;
    pend = (intptr_t)len >= 0 ? p + len : 0;

    strict = flags & NAI_URI_STRICT;
    if (flags & NAI_URI_HOSTONLY) {
        goto _host;
    };

    if (pend && p >= pend) {
        ch = 0;
    } else {
        ch = p[0];
        if (ch) {
            ch = nai_tolower(ch);
        };
    };
    if (flags & NAI_URI_PATHONLY) {
        goto _path;
    };

    if (ch >= 'a' && ch <= 'z') {
        /* match 'scheme://' */
        start = p;
        p ++;
        for ( ; ; p ++) {
            if (pend && p >= pend) {
                break;
            };
            ch = *p;
            if (delims[ch] & escapeScheme) {
                break;
            };
        };
        if ((pend && (p + 1) > pend) || ch != ':') {
            p = start;
            ch = p[0];
            nai_str_setm(&u->scheme, "", 0);

            if (flags & NAI_URI_HOSTPATH) {
                goto _host;
            } else {
                goto _nohost;
            };
        };

        /* fill scheme */
        nai_str_setm(&u->scheme, start, p - start);

        /* double slash */
        ch = 0;
        if ((pend && (p + 3) > pend) || 
            (ch = p[1]) != '/' || p[2] != '/') {
            if (flags & NAI_URI_HOSTPATH) {
                p = start;
                nai_str_setm(&u->scheme, "", 0);
                goto _host;
            } else {
                p ++;
                goto _nohost;
            };
        };

        p += 3;

    } else if (ch == '/' && (!pend || p + 1 < pend) && p[1] == '/') {
        nai_str_setm(&u->scheme, "", 0);
        p += 2;
    } else {
        nai_str_setm(&u->scheme, "", 0);

        if (!(flags & NAI_URI_HOSTPATH)) {
            goto _nohost;
        };
    };

_host:
    /* match 'user:pass@hostname:port' */
    iplite = nai_uri.iplite;
    ipv6 = 0;
    user = 0;
    port_invalid = 0;
    start = p;
    colon = 0;
    for ( ; ; p ++) {
        if (pend && p >= pend) {
            ch = 0;
        } else {
            ch = *p;
        };
        switch (ch) {
        case ':':
            if (ipv6 == 1) {
                break;
            } else if (ipv6 == 2) {
                ipv6 = 0;
            };
            if (colon) {
                /* find ':' again, malformed format */
                if (strict) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
                break;
            };
            colon = p;
            break;

        case '@':
            if (user) {
                /* find '@' again, malformed format */
                if (strict) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
                break;
            };

            /* get user and password */
            if (colon == 0) {
                nai_str_setm(&u->user, start, p - start);
                nai_str_setm(&u->password, "", 0);
            } else {
                nai_str_setm(&u->user, start, colon - start);
                nai_str_setm(&u->password, colon + 1, p - colon - 1);
            };
            start = p + 1;
            colon = 0;
            user = 1;
            port_invalid = 0;
            break;

        case '[':
            if (p != start) {
                /* a ipv6 address, '[' must be the lead char */
                if (strict) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
                break;
            };
            ipv6 = 1;
            user = 1;
            break;

        case ']':
            if (ipv6 != 1) {
                if (strict) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
                break;
            };
            ipv6 = 2;
            break;

        default:
            if (!(delims[ch] & (slash|question|hashnil))) {
                if (strict) {
                    /* invalid character */
                    if (delims[ch] & escapeHost) {
                        nai_errno = EINVAL;
                        r = -1;
                        goto _fail;
                    };
                    /* ipv6 address missing colon */
                    if (ipv6 == 2) {
                        nai_errno = EINVAL;
                        r = -1;
                        goto _fail;
                    } else if (ipv6 == 1) {
                        if (!(iplite[ch] & (hex|dot))) {
                            nai_errno = EINVAL;
                            r = -1;
                            goto _fail;
                        };
                    } else if (colon) {
                        if (!(iplite[ch] & oct)) {
                            port_invalid = 1;
                        };
                    };
                };
                continue;
            };
            if (strict) {
                /* ipv6 address missing ']' */
                if (ipv6 == 1) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
                /* port only allow '0'-'9' */
                if (port_invalid) {
                    nai_errno = EINVAL;
                    r = -1;
                    goto _fail;
                };
            };

            if (!user) {
                nai_str_setm(&u->user, "", 0);
                nai_str_setm(&u->password, "", 0);
            };

            /* get hostname and port */
            nai_str_setm(&u->host, start, p - start);
            if (colon == 0) {
                nai_str_setm(&u->hostname, start, p - start);
                nai_str_setm(&u->portname, "", 0);
            } else {
                nai_str_setm(&u->hostname, start, colon - start);
                nai_str_setm(&u->portname, colon + 1, p - colon - 1);
            };

            if (flags & NAI_URI_HOSTONLY) {
                goto _end;
            } else {
                goto _path;
            };
        };
    };

#if 0
    if (0) {
#endif
_nohost:
        nai_str_setm(&u->user, "", 0);
        nai_str_setm(&u->password, "", 0);
        nai_str_setm(&u->host, "", 0);
        nai_str_setm(&u->hostname, "", 0);
        nai_str_setm(&u->portname, "", 0);
#if 0
    };
#endif


_path:
    u->flags = 0;
    /* check path */
    if (delims[ch] & (question|hashnil)) {
        nai_str_setm(&u->path, "", 0);
        u->path_empty = 1;
    } else {
        if (strict && (delims[ch] & escapePath)) {
            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };
        if (ch == '%') {
            u->path_escape = 1;
        };

        start = p;
        p ++;
        for ( ; ; p ++) {
            if (pend && p >= pend) {
                break;
            };
            ch = *p;
            if (delims[ch] & (question|hashnil)) {
                break;
            };
            if (strict && (delims[ch] & escapePath)) {
                nai_errno = EINVAL;
                r = -1;
                goto _fail;
            };
            if (ch == '%') {
                u->path_escape = 1;
            } else if (delims[ch] & (dotp|slash)) {
                u->path_complex = 1;
            };
        };
        if (p == start) {
            nai_str_setm(&u->path, "", 0);
        } else {
            nai_str_setm(&u->path, start, p - start);
        };
    };

    /* match '?xxxxxx' */
    if (ch != '?') {
        nai_str_setm(&u->query, "", 0);
    } else {
        p ++;
        start = p;
        for ( ; ; p ++) {
            if (pend && p >= pend) {
                break;
            };
            ch = *p;
            if (delims[ch] & (hashnil)) {
                break;
            };
            if (strict && (delims[ch] & escapeArgu)) {
                nai_errno = EINVAL;
                r = -1;
                goto _fail;
            };
            if (ch == '%') {
                u->query_escape = 1;
            };
        };
        if (p == start) {
            nai_str_setm(&u->query, "", 0);
        } else {
            nai_str_setm(&u->query, start, p - start);
        };
    };

    /* match '#xxxxxx' */
    if (ch != '#') {
        nai_str_setm(&u->fragment, "", 0);
    } else if (pend) {
        nai_str_setm(&u->fragment, p + 1, pend - p - 1);
    } else {
        nai_str_sets(&u->fragment, p + 1);
    };

_end:
    r = 0;

_fail:
    return r;
};


nai_int_t nai_uri_parse_value(
    const nai_mem_t* key, nai_mem_t* val, const char* str, size_t len)
{
    nai_int_t r;
    nai_int_t klen;
    char ch;
    char lead;
    char* end;
    char* p;
    char* pend;
    char* kstr;


    kstr = nai_str(key);
    klen = nai_str_len(key);
    if (klen <= 0) {
        goto _fail;
    };

    lead = *kstr;
    p = (char*)str;
    pend = (intptr_t)len >= 0 ? p + len : 0;
    for (;;) {
        if (pend) {
            if (pend - p < klen) {
                goto _fail;
            };
            ch = *p ++;
        } else {
            ch = *p ++;
            if (ch == 0) {
                goto _fail;
            };
        };


        while (ch == lead) {
            if (nai_strncasecmp(p, kstr+1, klen-1) != 0) {
                break;
            };

            p += klen-1;
            if (pend && p >= pend) {
                goto _fail;
            };

            ch = *p ++;

            if (ch != '=') {
                break;
            };

            /* found */
            if (pend) {
                end = nai_memchr(p, '&', pend - p);
                if (end == 0) {
                    end = pend;
                };
            } else {
                end = nai_strchr(p, '&');
                if (end == 0) {
                    end = p + nai_strlen(p);
                };
            };

            nai_str_setm(val, p, end - p);
            r = 0;
            goto _end;
        };


        /* skip */
        if (pend) {
            p = nai_memchr(p, '&', pend - p);
        } else {
            p = nai_strchr(p, '&');
        };
        if (p == 0) {
            goto _fail;
        };

        p ++;
    };

_end:
    return r;

_fail:
    nai_errno = ENOENT;
    r = -1;
    goto _end;
};


const char* nai_uri_parse_query(
    nai_mem_t* key, nai_mem_t* val, const char* str, size_t len)
{
    nai_int_t k;
    char ch;
    char* start;
    char* p;
    char* pend;


    k = 0;
    p = (char*)str;
    pend = (intptr_t)len >= 0 ? p + len : 0;

    start = p;
    for ( ; ; p ++) {
        if (pend && p >= pend) {
            ch = 0;
        } else {
            ch = *p;
        };

        switch (ch) {
        default:
            continue;
        case '=':
            if (k == 0) {
                k = 1;
                nai_str_setm(key, start, p-start);
                start = p + 1;
            } else {
                /* already has a key, as a part of value */
                ;
            };
            continue;
        case '&':
        case '\0':
            if (k == 0) {
                if (p - start <= 0) {
                    if (ch == '\0') {
                        break;
                    };
                    start = p + 1;
                    continue;
                };

                nai_str_setm(key, start, p-start);
                nai_str_setm(val, p, 0);
            } else {
                nai_str_setm(val, start, p-start);
            };
        };


        if (ch == '&') {
            p ++;
        };
        break;
    };

    return k ? p : 0;
};


/* from ngx_string.c */

static uint32_t nai_escape_uri[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x80000029, /* 1000 0000 0000 0000  0000 0000 0010 1001 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

                /* " ", "#", "%", "&", "+", "?", %00-%1F, %7F-%FF */
static uint32_t nai_escape_args[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x88000869, /* 1000 1000 0000 0000  0000 1000 0110 1001 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

                /* not ALPHA, DIGIT, "-", ".", "_", "~" */
static uint32_t nai_escape_uri_component[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0xfc009fff, /* 1111 1100 0000 0000  1001 1111 1111 1111 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x78000001, /* 0111 1000 0000 0000  0000 0000 0000 0001 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0xb8000001, /* 1011 1000 0000 0000  0000 0000 0000 0001 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

                /* " ", "#", """, "%", "'", %00-%1F, %7F-%FF */
static uint32_t nai_escape_html[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x000000ad, /* 0000 0000 0000 0000  0000 0000 1010 1101 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

                /* " ", """, "%", "'", %00-%1F, %7F-%FF */
static uint32_t nai_escape_refresh[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x00000085, /* 0000 0000 0000 0000  0000 0000 1000 0101 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

                /* " ", "%", %00-%1F */
static uint32_t nai_escape_memcached[] = {
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x00000021, /* 0000 0000 0000 0000  0000 0000 0010 0001 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
    0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
};

                /* mail_auth is the same as memcached */

static char nai_escape_hex[] = "0123456789abcdef";
static uint32_t* nai_escape_map[] = { 
    nai_escape_uri, 
    nai_escape_args, 
    nai_escape_uri_component, 
    nai_escape_html, 
    nai_escape_refresh, 
    nai_escape_memcached, 
    nai_escape_memcached 
};


intptr_t nai_uri_escape(
    char* buf, size_t buflen, 
    const char* str, size_t len, nai_int_t type)
{
    intptr_t r;
    intptr_t n;
    uint8_t ch;
    char* start;
    char* src;
    char* srcend;
    char* dst;
    char* dstend;
    uint32_t* map;


    if (type < 0 || type >= NAI_ESCAPE_MAIL_AUTH) {
        nai_errno = EINVAL;
        r = -1;
        goto _fail;
    };
    map = nai_escape_map[type];


    if (buf == 0) {
        buflen = 0;
    };


    src = (char*)str;
    srcend = (intptr_t)len >= 0 ? src + len : 0;
    dst = buf;
    dstend = dst + buflen;


    start = src;
    for ( ; ; src ++) {
        if (srcend && src >= srcend) {
            ch = 0;
        } else {
            ch = *src;
        };

        if (ch == 0) {
            goto _end;
        };
        if (!(map[ch >> 5] & (1 << (ch&0x1f)))) {
            continue;
        };

        /* check space */
        n = src - start;
        if (n + 3 > dstend - dst) {
            break;
        };

        nai_memcpy(dst, start, n);
        dst += n;
        dst[0] = '%';
        dst[1] = nai_escape_hex[ch >> 4];
        dst[2] = nai_escape_hex[ch & 0xf];
        dst += 3;
        start = src + 1;
    };

    /* buffer is not enough, count space only */
    for ( ; ; src ++) {
        if (srcend && src >= srcend) {
            ch = 0;
        } else {
            ch = *src;
        };

        if (ch == 0) {
            goto _end;
        };
        if (!(map[ch >> 5] & (1 << (ch&0x1f)))) {
            continue;
        };

        dst += src - start;
        dst += 3;
        start = src + 1;
    };


_end:
    n = src - start;
    if (n > 0) {
        if (n <= dstend - dst) {
            nai_memcpy(dst, start, n);
        };
        dst += n;
    };

    /* if src with null-terminated, put it */
    if (!srcend || src < srcend) {
        if (dst < dstend) {
            dst[0] = 0;
        };
        /* dst ++; */
    };
    r = dst - buf;

_fail:
    return r;
};


intptr_t nai_uri_unescape(
    char* buf, size_t buflen, 
    const char* str, size_t len)
{
    intptr_t r;
    intptr_t n;
    uint8_t ch;
    uint8_t unc;
    char* start;
    char* src;
    char* srcend;
    char* dst;
    char* dstend;


    if (buf == 0) {
        buflen = 0;
    };


    src = (char*)str;
    srcend = (intptr_t)len >= 0 ? src + len : 0;
    dst = buf;
    dstend = dst + buflen;


    start = src;
    for (;;) {
        if (srcend && src >= srcend) {
            ch = 0;
        } else {
            ch = *src;
        };

        if (ch == 0) {
            goto _end;
        };
        if (ch != '%') {
            src ++;
            continue;
        };

        /* check space */
        n = src - start;
        if (n + 1 > dstend - dst) {
            break;
        };


        nai_memcpy(dst, start, n);
        dst += n;


        /* check first char */
        if (srcend && src + 1 >= srcend) {
            start = src;
            ch = 0;
            goto _end;
        };

        ch = src[1];
        switch (ch) {
        case '%':
            /* % */
            unc = '%';
            src += 2;
            break;

        default:
            /* 0-9 a-f */
            if (ch >= '0' && ch <= '9') {
                unc = ch - '0';
            } else {
                ch |= 0x20;
                if (ch < 'a' || ch > 'f') {
                    unc = '%';
                    src += 1;
                    break;
                };
                unc = ch - 'a' + 10;
            };

            /* check second char */
            if (srcend && src + 2 >= srcend) {
                start = src;
                dst[0] = unc;
                dst ++;
                ch = 0;
                goto _end;
            };

            /* 0-9 a-f */
            ch = src[2];
            if (ch >= '0' && ch <= '9') {
                unc <<= 4;
                unc |= ch - '0';
            } else {
                ch |= 0x20;
                if (ch < 'a' || ch > 'f') {
                    src += 2;
                    break;
                };
                unc <<= 4;
                unc |= ch - 'a' + 10;
            };

            src += 3;
        };

        dst[0] = unc;
        dst ++;
        start = src;
    };

    for (;;) {
        if (srcend && src >= srcend) {
            ch = 0;
        } else {
            ch = *src;
        };

        if (ch == 0) {
            goto _end;
        };
        if (ch != '%') {
            src ++;
            continue;
        };


        dst += src - start;


        /* check first char */
        if (srcend && src + 1 >= srcend) {
            start = src;
            ch = 0;
            goto _end;
        };

        ch = src[1];
        switch (ch) {
        case '%':
            /* % */
            src += 2;
            break;

        default:
            /* 0-9 a-f */
            if (ch < '0' || ch > '9') {
                ch |= 0x20;
                if (ch < 'a' || ch > 'f') {
                    src += 1;
                    break;
                };
            };

            /* check second char */
            if (srcend && src + 2 >= srcend) {
                start = src;
                dst ++;
                ch = 0;
                goto _end;
            };

            /* 0-9 a-f */
            ch = src[2];
            if (ch < '0' || ch > '9') {
                ch |= 0x20;
                if (ch < 'a' || ch > 'f') {
                    src += 2;
                    break;
                };
            };

            src += 3;
        };

        dst ++;
        start = src;
    };


_end:
    (void)ch;

    n = src - start;
    if (n > 0) {
        if (n <= dstend - dst) {
            nai_memcpy(dst, start, n);
        };
        dst += n;
    };

    /* if src with null-terminated, put it */
    if (!srcend || src < srcend) {
        if (dst < dstend) {
            dst[0] = 0;
        };
        /* dst ++; */
    };
    r = dst - buf;

    return r;
};


