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
/// @file       nai_path.c
/// @brief      
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_file.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_string.h"



nai_int_t nai_path_is_absolute(const char* path, size_t len)
{
    nai_int_t r;
    const char* src;
    const char* srcend;


    src = path;
    srcend = (intptr_t)len >= 0 ? src + len : 0;


    if (!srcend || src + 1 <= srcend) {
        if (nai_path_is_sep(src[0])) {
            r = 1;
            goto _end;
        };
    };

#if defined(_WIN32)
    if (!srcend || src + 2 <= srcend) {
        if (src[0] == '\\' && src[1] == '\\') {
            r = 1;
            goto _end;
        };

        if (nai_isalpha(src[0]) && src[1] == ':') {
            r = 1;
            goto _end;
        };
    };
#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_path_get_root_len(const  char* path, size_t len)
{
    intptr_t r;
    const char* src;
    const char* srcend;

#if defined(_WIN32)
    char ch;
    const char* start;
#endif


    src = path;
    srcend = (intptr_t)len >= 0 ? src + len : 0;


    if (!srcend || src + 1 <= srcend) {
        if (nai_path_is_sep(src[0])) {
            r = 1;
            goto _end;
        };
    };

#if defined(_WIN32)
    /* test form '\\server\' */
    if ((!srcend || src + 2 <= srcend) && 
        (src[0] == '\\' && src[1] == '\\')) {
        start = src;
        src += 2;
        for ( ; ; src ++) {
            if (srcend && src >= srcend) {
                ch = 0;
                break;
            } else {
                ch = *src;
                if (ch == 0) {
                    break;
                };
            };

            if (nai_path_is_sep(ch)) {
                break;
            };
        };

        r = src - start + nai_path_is_sep(ch);
        goto _end;
    };

    /* test form 'x:\' */
    if ((!srcend || src + 2 <= srcend) && 
        (nai_isalpha(src[0]) && src[1] == ':')) {
        start = src;
        src += 2;
        if (srcend && src >= srcend) {
            ch = 0;
        } else {
            ch = *src;
        };

        r = src - start + nai_path_is_sep(ch);
        goto _end;
    };
#endif

    r = 0;

_end:
    return (nai_int_t)r;
};


intptr_t nai_path_canonicalize(
    char* buf, size_t buflen, const char* path, size_t len, char sep)
{
    intptr_t r;
    nai_int_t ec;
    char ch;
    char lastc;
    char* dst;
    char* dstend;
    const char* start;
    const char* src;
    const char* srcend;
    nai_int_t* tmp;
    nai_int_t* seg;
    nai_int_t segc;
    nai_int_t segalloc;
    nai_int_t segments[256];


    if (buf == 0) {
        buflen = 0;
    };

    src = path;
    srcend = (intptr_t)len >= 0 ? src + len : 0;
    dst = buf;
    dstend = dst + buflen;
    lastc = 0;


#if defined(_WIN32)
    /* test form '\\server\' */
    if ((!srcend || src + 2 <= srcend) && 
        (src[0] == '\\' && src[1] == '\\')) {
        start = src;
        src += 2;
        for ( ; ; src ++) {
            if (srcend && src >= srcend) {
                ch = 0;
                break;
            } else {
                ch = *src;
                if (ch == 0) {
                    break;
                };
            };

            if (nai_path_is_sep(ch)) {
                break;
            };
        };

        if (ch == 0) {
            r = src - start + 1;
            if (r <= dstend - dst) {
                nai_memcpy(dst, start, r);
            };
            seg = segments;
            goto _end;
        };

        r = src - start;
        if (r + 1 <= dstend - dst) {
            nai_memcpy(dst, start, r);
            dst[r] = sep;
        };
        dst += r + 1;
        src ++;
        lastc = sep;
    };

    /* test form 'x:\' */
    if ((!srcend || src + 2 <= srcend) && 
        (nai_isalpha(src[0]) && src[1] == ':')) {
        if (3 <= dstend -dst) {
            dst[0] = src[0];
            dst[1] = ':';
            dst[2] = sep;
        };
        dst += 3;
        lastc = nai_path_sep;

        src += 2;
        for ( ; ; src ++) {
            if (srcend && src >= srcend) {
                break;
            } else {
                ch = *src;
                if (ch == 0) {
                    break;
                };
            };

            if (!nai_path_is_sep(ch)) {
                break;
            };
        };
    };
#endif


    seg = segments;
    segc = 0;
    segalloc = nai_countof(segments);

    for (;;) {
        start = src;
        for ( ; ; src ++) {
            if (srcend && src >= srcend) {
                ch = 0;
                break;
            } else {
                ch = *src;
                if (ch == 0) {
                    break;
                };
            };

            if (nai_path_is_sep(ch)) {
                break;
            };
        };
        r = src - start;
        if (r == 0) {
            ;
        } else if (r == 1 && start[0] == '.') {
            lastc = ch;
        } else if (r == 2 && start[0] == '.' && start[1] == '.') {
            if (segc > 0) {
                segc --;
                dst = buf + seg[segc];
            };
            lastc = ch;
        } else {
            if (segc >= segalloc) {
                segalloc += segalloc;
                tmp = (nai_int_t*)nai_malloc(segalloc*sizeof(nai_int_t));
                if (tmp == 0) {
                    r = -1;
                    goto _end;
                };
                nai_memcpy(tmp, seg, segc*sizeof(nai_int_t));
                if (seg != segments) {
                    nai_free(seg);
                };
                seg = tmp;
            };
            seg[segc] = (nai_int_t)(dst-buf);
            segc ++;

            if (r <= dstend - dst) {
                nai_memcpy(dst, start, r);
            };
            dst += r;
            lastc = 0;
        };
        if (ch == 0) {
            break;
        };
        if (!nai_path_is_sep(ch)) {
            continue;
        };
        if (!nai_path_is_sep(lastc)) {
            if (dst < dstend) {
                dst[0] = sep;
            };
            dst ++;
            lastc = sep;
        };
        for ( ; ; src ++) {
            if (srcend && src >= srcend) {
                break;
            } else {
                ch = *src;
                if (ch == 0) {
                    break;
                };
            };

            if (!nai_path_is_sep(ch)) {
                break;
            };
        };
    };

    /* if src with null-terminated, put it */
    if (!srcend || src < srcend) {
        if (dst < dstend) {
            dst[0] = 0;
        };
        /* dst ++; */
    };
    if (dst > dstend) {
        nai_errno = ERANGE;
    };
    r = dst - buf;


_end:
    if (seg != segments) {
        if (r < 0) {
            ec = nai_errno;
        };
        nai_free(seg);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return r;
};


#if defined(_WIN32)


#include "win/nai_windows.h"
#include <fileapi.h>
#include <processenv.h>


nai_int_t nai_path_set_cwd(const char* path)
{
    nai_int_t r;
    nai_wcs_t ws;


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    if (SetCurrentDirectoryW(ws.str) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


intptr_t nai_path_get_cwd(char* buf, size_t buflen)
{
    intptr_t r;
    intptr_t len;
    nai_wcs_t ws;


    r = nai_wcs_buffer(&ws);
    if (r < 0) {
        goto _end;
    };

    r = GetCurrentDirectoryW((nai_int_t)ws.tbuflen, ws.tbuf);
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    /* try convert */
    len = r + 1;
    r = nai_wcs_to_utf8(buf, buflen, ws.tbuf, len);
    if (r > 0) {
        r --;
    };

_end:
    return r;
};


intptr_t nai_path_get_real(char* buf, size_t buflen, const char* path)
{
    intptr_t r;
    intptr_t len;
    nai_wcs_t ws;
    size_t fulllen;
    wchar_t* full;
    wchar_t stack[NAI_PATH_MAX];


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    if (ws.tbuflen < nai_countof(stack)) {
        full = stack;
        fulllen = nai_countof(stack);
    } else {
        full = ws.tbuf;
        fulllen = ws.tbuflen;
    };

    r = GetFullPathNameW(ws.str, (nai_int_t)fulllen, full, 0);
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    /* try convert */
    len = r + 1;
    r = nai_wcs_to_utf8(buf, buflen, full, len);
    if (r > 0) {
        r --;
    };

_end:
    return r;
};


intptr_t nai_path_get_full(char* buf, size_t buflen, const char* path)
{
    return nai_path_get_real(buf, buflen, path);
};


#else


#include "nai/os/nai_tlocal.h"
#include <unistd.h>
#include <stdlib.h>


nai_int_t nai_path_set_cwd(const char* path)
{
    nai_int_t r;

    r = chdir(path);

    return r;
};


intptr_t nai_path_get_cwd(char* buf, size_t buflen)
{
    nai_int_t r;
    char* path;


    if (buf && buflen > 0) {
        if (getcwd(buf, buflen) != 0) {
            r = nai_strlen(buf);
            goto _end;
        };

        switch (nai_errno) {
        case ERANGE:
            break;
        default:
            r = -1;
            goto _end;
        };
    };

    path = (char*)nai_thread_local_mem(NAI_PATH_MAX);
    if (path == 0) {
        r = -1;
        goto _end;
    };

    if (getcwd(path, NAI_PATH_MAX) == 0) {
        r = -1;
        goto _end;
    };

    r = nai_strlen(path);
    nai_errno = ERANGE;

_end:
    return r;
};


intptr_t nai_path_get_real(char* buf, size_t buflen, const char* path)
{
    intptr_t r;
    char* real;


    if (buf && buflen >= NAI_PATH_MAX) {
        if (realpath(path, buf) == 0) {
            r = -1;
        } else {
            r = nai_strlen(buf);
        };
        goto _end;
    };


    real = (char*)nai_thread_local_mem(NAI_PATH_MAX);
    if (real == 0) {
        r = -1;
        goto _end;
    };

    if (realpath(path, real) == 0) {
        r = -1;
        goto _end;
    };

    r = nai_strlen(real);
    if (r <= (intptr_t)buflen) {
        nai_memcpy(buf, real, r + (r < (intptr_t)buflen));
    } else {
        nai_errno = ERANGE;
    };

_end:
    return r;
};


intptr_t nai_path_get_full(char* buf, size_t buflen, const char* path)
{
    intptr_t r;
    intptr_t pathlen;
    intptr_t fulllen;
    intptr_t rootlen;
    char* full;


    pathlen = nai_strlen(path);
    r = nai_path_is_absolute(path, pathlen);
    if (r) {
        r = nai_path_canonicalize(
            buf, buflen, path, pathlen + 1, nai_path_sep);
        goto _fix;
    };

    if (path[0] == '.' && path[1] == 0) {
        r = nai_path_get_cwd(buf, buflen);
        goto _end;
    };

    r = NAI_PATH_MAX - pathlen - 2;
    if (r < 0) {
        r = 0;
    };
    for (;;) {
        fulllen = r;
        full = (char*)nai_thread_local_mem(fulllen + pathlen + 2);
        if (full == 0) {
            goto _end;
        };

        r = nai_path_get_cwd(full, fulllen);
        if (r < 0) {
            goto _end;
        };

        if (r <= fulllen) {
            break;
        };
    };

    fulllen = r;
    nai_memcpy(full + fulllen, "/", 1);
    fulllen ++;
    nai_memcpy(full + fulllen, path, pathlen + 1);
    fulllen += pathlen + 1;

    r = nai_path_canonicalize(
        buf, buflen, full, fulllen, nai_path_sep);

_fix:
    if (buf && r > 0 && r <= (intptr_t)buflen) {

        rootlen = nai_path_get_root_len(buf, r);
        if (rootlen < r && buf[r - 1] == nai_path_sep) {
            r -= 1;
            buf[r] = 0;
        };
    };

_end:
    return r;
};


#endif


