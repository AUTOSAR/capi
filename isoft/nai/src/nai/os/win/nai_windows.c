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
/// @file       nai_windows.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_windows.h"


#if defined(_WIN32)


#include "nai/os/nai_dlopen.h"
#include "nai/os/nai_thread.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include <libloaderapi.h>
#include <handleapi.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <stringapiset.h>


nai_wapi_t nai_wapi = {
    0, 0, NAI_ONCE_INIT
};


static void nai_wapi_init_once()
{
    HMODULE ntdll;
    HMODULE kernel;
    HMODULE iphlp;


#define nai_get_sym(so, sym, type)                          \
    (nai_wapi.sym = (type)nai_dlsym((so), #sym))            \

#define nai_get_api(so, sym)                                \
    nai_get_sym(so, sym, sym##_f)                           \


    kernel = GetModuleHandleA("kernel32.dll");
    nai_get_api(kernel, GetTickCount64);
    nai_get_api(kernel, GetLogicalProcessorInformation);
    nai_get_api(kernel, GetProcessGroupAffinity);
    nai_get_api(kernel, GetThreadGroupAffinity);
    nai_get_api(kernel, SetThreadGroupAffinity);
    nai_get_api(kernel, GetQueuedCompletionStatusEx);
    nai_get_api(kernel, SetFileCompletionNotificationModes);
    nai_get_api(kernel, CancelSynchronousIo);

    nai_get_api(kernel, InitializeSRWLock);
    nai_get_api(kernel, TryAcquireSRWLockExclusive);
    nai_get_api(kernel, TryAcquireSRWLockShared);
    nai_get_api(kernel, AcquireSRWLockExclusive);
    nai_get_api(kernel, ReleaseSRWLockExclusive);
    nai_get_api(kernel, AcquireSRWLockShared);
    nai_get_api(kernel, ReleaseSRWLockShared);

    ntdll = GetModuleHandleA("ntdll.dll");
    nai_get_api(ntdll, RtlNtStatusToDosError);
    nai_get_api(ntdll, NtResumeProcess);
    nai_get_api(ntdll, NtSuspendProcess);
    nai_get_api(ntdll, NtQueryObject);
    nai_get_api(ntdll, NtCreateFile);
    nai_get_api(ntdll, NtCancelIoFileEx);
    nai_get_api(ntdll, NtDeviceIoControlFile);

    iphlp = GetModuleHandleA("iphlpapi.dll");
    if (iphlp == 0) {
        iphlp = LoadLibraryA("iphlpapi.dll");
        nai_wapi.modules[0] = iphlp;
    };
    if (iphlp) {
        nai_get_sym(iphlp, GetAdaptersAddresses, FARPROC);
        nai_get_sym(iphlp, GetNetworkParams, FARPROC);
        nai_get_api(iphlp, if_nametoindex);
        nai_get_api(iphlp, if_indextoname);
    };

    nai_memory_barrier();
    nai_wapi.inited = 1;
    return;
};


nai_int_t nai_wapi_init()
{
    nai_int_t r;


    if (nai_wapi.inited == 0) {
        nai_once(&nai_wapi.once, nai_wapi_init_once);
    };
    if (nai_wapi.error) {
        nai_errno = nai_wapi.error;
        r = 0;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


void nai_wapi_term()
{
    nai_int_t n;
    HMODULE module;


    for (n = 0; n < (nai_int_t)nai_countof(nai_wapi.modules); n ++) {
        module = nai_wapi.modules[n];
        if (module != 0) {
            FreeLibrary(module);
            nai_wapi.modules[n] = 0;
        };
    };

    return;
};



//////////////////////////////////////////////////////////////////////////////
// win init


typedef struct nai_win_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_thread_key_t tlocal;
} nai_win_t;


#define nai_win_tlocal_init(t) {                        \
    nai_tlocal_init(&(t)->base);                        \
    (t)->event = 0;                                     \
}                                                       \


static nai_win_t nai_win = {
    0, 0, NAI_ONCE_INIT, NAI_THREAD_KEY_NIL
};


extern nai_int_t nai_wsock_init();
static void nai_win_tlocal_release(void* h);


static void nai_win_init()
{
    nai_int_t r;
    uint32_t n;
    uint32_t size;
    uint32_t shift;
    SYSTEM_INFO si;


    GetSystemInfo(&si);
    size = si.dwPageSize;
    n = size;
    for (shift = 0; n >>= 1; shift ++) {
    };

    nai_pagesize = size;
    nai_pagesize_shift = shift;

    r = nai_thread_key_open(&nai_win.tlocal, nai_win_tlocal_release);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_thread_key_open() failed");
        goto _fail;
    };

    r = nai_wapi_init();
    if (r < 0) {
        goto _fail;
    };

    r = nai_wsock_init();
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_fail:
    if (r < 0) {
        nai_win.error = nai_errno;
    };

    nai_memory_barrier();
    nai_win.inited = 1;
    return;
};


nai_int_t nai_init()
{
    nai_int_t r;


    if (nai_win.inited == 0) {
        nai_once(&nai_win.once, nai_win_init);
    };
    if (nai_win.error) {
        nai_errno = nai_win.error;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


void nai_os_term()
{
    if (nai_win.inited) {
        nai_win_tlocal_release(nai_thread_key_get(&nai_win.tlocal));
        nai_thread_key_close(&nai_win.tlocal);
    };

    nai_wapi_term();

    return;
};



//////////////////////////////////////////////////////////////////////////////
// win thread local


typedef struct nai_win_tlocal_s {
    /* common */
    nai_tlocal_t base;

    /* windows private */
    HANDLE event;

} nai_win_tlocal_t;


#define nai_win_tlocal_get()                            \
    ((nai_win_tlocal_t*)nai_tlocal_get())               \


static void nai_win_tlocal_release(void* h)
{
    nai_int_t r;
    nai_int_t ec;
    nai_win_tlocal_t* t = (nai_win_tlocal_t*)h;


    if (t == 0) {
        return;
    };

    r = nai_tlocal_cleanup(&t->base);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_tlocal_cleanup() failed");
    };

    if (t->event) {
        if (!CloseHandle(t->event)) {
            ec = nai_errno_from_oserr(GetLastError());
            nai_log_crit(NAI_LOG_CORE, ec, "CloseHandle() failed");
        };
    };

    nai_free(t);
    return;
};


nai_tlocal_t* nai_tlocal_get()
{
    nai_win_tlocal_t* t;


    if (nai_win.inited == 0) {
        nai_once(&nai_win.once, nai_win_init);
    };
    if (nai_win.error) {
        nai_errno = nai_win.error;
        t = 0;
        goto _end;
    };

    t = (nai_win_tlocal_t*)nai_thread_key_get(&nai_win.tlocal);
    if (t == 0) {
        t = (nai_win_tlocal_t*)nai_malloc(sizeof(*t));
        if (t == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "malloc failed");
            goto _end;
        };

        nai_win_tlocal_init(t);
        nai_thread_key_set(&nai_win.tlocal, t);
    };

_end:
    return (nai_tlocal_t*)t;
};


HANDLE nai_win_ioevent()
{
    nai_int_t ec;
    HANDLE h;
    nai_win_tlocal_t* t;


    t = nai_win_tlocal_get();
    if (t == 0) {
        h = 0;
        goto _end;
    };

    if (t->event == 0) {
        t->event = CreateEventW(0, TRUE, 0, 0);
        if (t->event == 0) {
            nai_errno = ec = nai_errno_from_oserr(GetLastError());
            nai_log_alert(NAI_LOG_CORE, ec, "CreateEvent() failed");
        };
    };

    h = t->event;

_end:
    return h;
};



//////////////////////////////////////////////////////////////////////////////
// wcs from/to utf8


typedef struct nai_win_wbuf_s {
    wchar_t* wbuf;
    size_t walloc;
} nai_win_wbuf_t;


nai_int_t nai_wcs_buffer(nai_wcs_t* ws)
{
    nai_int_t r;
    nai_win_wbuf_t t;


    /* clear for caller safe free */
    ws->buf = 0;


    /* get thread local mem */
    t.walloc = 1024;
    t.wbuf = nai_thread_local_mem(sizeof(wchar_t)*t.walloc);
    if (t.wbuf == 0) {
        r = -1;
        goto _end;
    };


    ws->tbuf = t.wbuf;
    ws->tbuflen = t.walloc;
    r = 0;

_end:
    return r;
};


nai_int_t nai_wcs_from_path(nai_wcs_t* ws, const char* path)
{
    return nai_wcs_from_1str_ext(ws, path, 0);
};


nai_int_t nai_wcs_from_1str(nai_wcs_t* ws, const char* s0)
{
    return nai_wcs_from_1str_ext(ws, s0, 0);
};


nai_int_t nai_wcs_from_1str_ext(nai_wcs_t* ws, const char* s0, size_t extlen)
{
    intptr_t r;
    nai_int_t ec;
    nai_win_wbuf_t t;


    /* clear for caller safe free */
    ws->buf = 0;


    /* get thread local mem */
    t.walloc = 1024;
    t.wbuf = nai_thread_local_mem(sizeof(wchar_t)*t.walloc);
    if (t.wbuf == 0) {
        r = -1;
        goto _end;
    };


    /* try convert */
    r = MultiByteToWideChar(CP_UTF8, 0, s0, -1, t.wbuf, (nai_int_t)t.walloc);
    if (r > 0 && (r + extlen) <= t.walloc) {
        r += extlen;
        ws->s[0] = t.wbuf;
        ws->s[1] = 0;
        ws->buf = 0;
        ws->tbuf = t.wbuf + r;
        ws->tbuflen = t.walloc - r;
        r = 0;
        goto _end;
    } else if (r == 0) {
        ec = GetLastError();
        if (ec != ERROR_INSUFFICIENT_BUFFER) {
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    /* count required space */
    r = MultiByteToWideChar(CP_UTF8, 0, s0, -1, 0, 0);
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    /* alloc buffer */
    ws->buf = (wchar_t*)nai_malloc((r + extlen) * sizeof(wchar_t));
    if (ws->buf == 0) {
        r = -1;
        goto _end;
    };

    /* convert again */
    r = MultiByteToWideChar(CP_UTF8, 0, s0, -1, ws->buf, (nai_int_t)r);
    if (r == 0) {
        ec = nai_errno_from_oserr(GetLastError());
        nai_free(ws->buf);
        nai_errno = ec;
        r = -1;
        goto _end;
    };

    ws->s[0] = ws->buf;
    ws->s[1] = 0;
    ws->tbuf = t.wbuf;
    ws->tbuflen = t.walloc;
    r = 0;

_end:
    return (nai_int_t)r;
};


nai_int_t nai_wcs_from_2str(nai_wcs_t* ws, const char* s0, const char* s1)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t used;
    nai_int_t succ;
    nai_int_t alloc;
    const char* s[2];
    nai_win_wbuf_t t;


    /* clear for caller safe free */
    ws->buf = 0;


    /* get thread local mem */
    t.walloc = 1024;
    t.wbuf = nai_thread_local_mem(sizeof(wchar_t)*t.walloc);
    if (t.wbuf == 0) {
        r = -1;
        goto _end;
    };


    s[0] = s0;
    s[1] = s1;
    succ = 0;
    used = 0;

    /* convert string */
    for (n = 0; n < 2; n ++) {
        r = MultiByteToWideChar(
            CP_UTF8, 0, s[n], -1, t.wbuf + used, (nai_int_t)t.walloc - used);
        if (r == 0) {
            ec = GetLastError();
            if (ec != ERROR_INSUFFICIENT_BUFFER) {
                nai_errno = nai_errno_from_oserr(ec);
                r = -1;
                goto _end;
            };
        } else {
            ws->s[n] = t.wbuf + used;
            succ |= 1 << n;
            used += r;
        };
    };

    ws->tbuf = t.wbuf + used;
    ws->tbuflen = t.walloc - used;
    if (succ == 3) {
        /* all string has been converted */
        ws->buf = 0;
        r = 0;
        goto _end;
    };


    /* count required space */
    alloc = 0;
    for (n = 0; n < 2; n ++) {
        if (succ & (1<<n)) {
            continue;
        };

        r = MultiByteToWideChar(CP_UTF8, 0, s[n], -1, 0, 0);
        if (r == 0) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
        alloc += r;
    };

    /* alloc space */
    ws->buf = (wchar_t*)nai_malloc(r * sizeof(wchar_t));
    if (ws->buf == 0) {
        r = -1;
        goto _end;
    };

    /* convert again */
    used = 0;
    for (n = 0; n < 2; n ++) {
        if (succ & (1<<n)) {
            continue;
        };

        r = MultiByteToWideChar(
            CP_UTF8, 0, s[n], -1, ws->buf + used, alloc - used);
        if (r == 0) {
            ec = nai_errno_from_oserr(GetLastError());
            nai_free(ws->buf);
            nai_errno = ec;
            r = -1;
            goto _end;
        };

        ws->s[n] = ws->buf + used;
        used += r;
    };

    r = 0;

_end:
    return r;
};


intptr_t nai_wcs_to_utf8(
    char* buf, size_t buflen, const wchar_t* str, size_t len)
{
    nai_int_t r;
    nai_int_t ec;


    r = WideCharToMultiByte(
        CP_UTF8, 0, str, (nai_int_t)len, buf, (nai_int_t)buflen, 0, 0);
    if (r > 0) {
        if ((intptr_t)buflen < r || buf == 0) {
            nai_errno = ERANGE;
        };
        goto _end;
    };

    ec = GetLastError();
    if (ec != ERROR_INSUFFICIENT_BUFFER) {
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _end;
    };


    /* get require space */
    r = WideCharToMultiByte(CP_UTF8, 0, str, (nai_int_t)len, 0, 0, 0, 0);
    nai_errno = ERANGE;

_end:
    return r;
};


intptr_t nai_wcs_from_utf8(
    wchar_t* buf, size_t buflen, const char* str, size_t len)
{
    nai_int_t r;
    nai_int_t ec;


    r = MultiByteToWideChar(
        CP_UTF8, 0, str, (nai_int_t)len, buf, (nai_int_t)buflen);
    if (r > 0) {
        goto _end;
    };

    ec = GetLastError();
    if (ec != ERROR_INSUFFICIENT_BUFFER) {
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _end;
    };


    /* get require space */
    r = MultiByteToWideChar(CP_UTF8, 0, str, (nai_int_t)len, 0, 0);
    nai_errno = ERANGE;

_end:
    return r;
};


#endif

