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
/// @file       nai_procattr.c
/// @brief      
/// @details
/// @date       2021-02-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_proc.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"


#if defined(_WIN32)


#include "nai/runtime/nai_util.h"
#include "win/nai_windows.h"
#include <windef.h>
#include <winbase.h>
#include <handleapi.h>


#endif


nai_int_t nai_proc_attr_init(nai_proc_attr_t* a)
{
    nai_int_t r;


    a->cwd = 0;
    a->log = 0;
#if defined(_WIN32)
    a->token = 0;
#elif (NAI_HAVE_UID)
    a->uid = 0;
    a->gid = 0;
#endif
    a->sched = 0;
    a->priority = 0;
    a->cpu.set = 0;
    a->cpu.size = 0;
    a->valid.bits = 0;
    r = 0;

    return r;
};


nai_int_t nai_proc_attr_open(nai_proc_attr_t* a)
{
    nai_proc_attr_init(a);
    return 0;
};


nai_int_t nai_proc_attr_close(nai_proc_attr_t* a)
{
    nai_int_t r;


#if defined(_WIN32)
    if (a->token) {
        CloseHandle(a->token);
        a->token = 0;
    };
#endif

    r = nai_proc_attr_init(a);
    return r;
};


nai_int_t nai_proc_attr_set_cwd(nai_proc_attr_t* a, const char* cwd)
{
    nai_int_t r;


    a->cwd = cwd;
    r = 0;

    return r;
};


nai_int_t nai_proc_attr_set_daemon(nai_proc_attr_t* a, nai_int_t daemon)
{
    nai_int_t r;


    a->valid.daemon = !!daemon;
    r = 0;

    return r;
};


nai_int_t nai_proc_attr_set_suspend(nai_proc_attr_t* a, nai_int_t suspend)
{
    nai_int_t r;


    a->valid.suspend = !!suspend;
    r = 0;

    return r;
};


nai_int_t nai_proc_attr_set_stdio(
    nai_proc_attr_t* a, nai_int_t no, nai_fd_t fd)
{
    nai_int_t r;


    if (no < 0 || no >= (intptr_t)nai_countof(a->stdio)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    a->stdio[no] = fd;
    a->valid.stdio |= 1 << no;
    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_attr_set_stdio_inherit(nai_proc_attr_t* a, nai_int_t no)
{
    nai_int_t r;


    if (no < 0 || no >= (intptr_t)nai_countof(a->stdio)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    a->valid.stdio &= ~(1 << no);
    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_attr_set_sched(
    nai_proc_attr_t* a, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;


    if (sched == NAI_SCHED_UNSET) {
        a->valid.sched = 0;
        r = 0;
        goto _end;
    };

    switch (sched) {
    case NAI_SCHED_NORMAL:
        break;
    case NAI_SCHED_FIFO:
    case NAI_SCHED_RR:
#if (NAI_HAVE_SCHED_SETSCHEDULER)
        break;
#else
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
#endif
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (priority > NAI_PRIORITY_HIGHEST) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    a->sched = sched;
    a->priority = priority;
    a->valid.sched = 1;
    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_attr_set_affinity(
    nai_proc_attr_t* a, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;


    a->cpu.set = set;
    a->cpu.size = size;
    r = 0;

    return r;
};


nai_int_t nai_proc_attr_set_limit(
    nai_proc_attr_t* a, nai_int_t res, const nai_rlimit_t* limit)
{
    nai_int_t r;


#if (NAI_HAVE_RLIMIT)
    if (res < 0 || res >= (intptr_t)nai_countof(a->rlimit)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (limit) {
        a->rlimit[res] = *limit;
        a->valid.rlimit |= 1 << res;
    } else {
        a->valid.rlimit &= ~(1 << res);
    };
    r = 0;

_end:
#else
    (void)a;
    (void)res;
    (void)limit;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


#if defined(_WIN32)


nai_int_t nai_proc_attr_set_user(
    nai_proc_attr_t* a, const char* name, const char* passwd)
{
    nai_int_t r;
    HANDLE user;
    nai_wcs_t ws = {0};


    if (a->token) {
        CloseHandle(a->token);
        a->token = 0;
    };

    if (name) {
        if (passwd == 0) {
            r = nai_wcs_from_1str(&ws, name);
        } else {
            r = nai_wcs_from_2str(&ws, name, passwd);
        };
        if (r < 0) {
            goto _end;
        };

        if (!LogonUserW(
            ws.str, 0, passwd ? ws.s[1] : L"", 
            LOGON32_LOGON_NETWORK,
            LOGON32_PROVIDER_DEFAULT,
            &user)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        if (!DuplicateTokenEx(user, 
            TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, 
            NULL,
            SecurityImpersonation,
            TokenPrimary,
            &(a->token))) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            CloseHandle(user);
            r = -1;
            goto _end;
        };

        CloseHandle(user);
    };

    r = 0;

_end:
    if (ws.s[1]) {
        /* clear password */
        nai_memset(ws.s[1], 0, sizeof(wchar_t)*wcslen(ws.s[1]));
    };
    nai_wcs_release(&ws, r);
    return r;
};


nai_int_t nai_proc_attr_set_group(nai_proc_attr_t* a, const char* name)
{
    return 0;
};


#elif (NAI_HAVE_UID)


#include <pwd.h>
#include <grp.h>


#define NAI_PWBUF_SIZE      8192


static nai_int_t nai_lookup_uid(
    const char* name, uint32_t* uid, uint32_t* gid)
{
    nai_int_t r;
    struct passwd* p;

#if (NAI_HAVE_GETGRNAM_R)
    struct passwd pwd;
    char buf[NAI_PWBUF_SIZE];


    r = getpwnam_r(name, &pwd, buf, sizeof(buf), &p);
    if (r < 0) {
        goto _end;
    };


#else

    p = getpwnam(name);
    if (p == 0) {
        r = -1;
        goto _end;
    };

    r = 0;

#endif

    if (uid) {
        uid[0] = p->pw_uid;
    };
    if (gid) {
        gid[0] = p->pw_gid;
    };

_end:
    return r;
};


static nai_int_t nai_lookup_gid(const char* name, uint32_t* gid)
{
    nai_int_t r;
    struct group* g;

#if (NAI_HAVE_GETGRNAM_R)
    struct group grp;
    char buf[NAI_PWBUF_SIZE];


    r = getgrnam_r(name, &grp, buf, sizeof(buf), &g);
    if (r < 0) {
        goto _end;
    };


#else

    g = getgrnam(name);
    if (g == 0) {
        r = -1;
        goto _end;
    };

    r = 0;

#endif

    if (gid) {
        gid[0] = g->gr_gid;
    };

_end:
    return r;
};


nai_int_t nai_proc_attr_set_user(
    nai_proc_attr_t* a, const char* name, const char* passwd)
{
    nai_int_t r;
    uint32_t uid;


    (void)passwd;

    if (name == 0) {
        a->valid.uid = 0;
        r = 0;
    } else {
        r = nai_lookup_uid(name, &uid, 0);
        if (r < 0) {
            goto _end;
        };

        a->uid = uid;
        a->valid.uid = 1;
    };

_end:
    return r;
};


nai_int_t nai_proc_attr_set_group(nai_proc_attr_t* a, const char* name)
{
    nai_int_t r;
    uint32_t gid;


    if (name == 0) {
        a->valid.gid = 0;
        r = 0;
    } else {
        r = nai_lookup_gid(name, &gid);
        if (r < 0) {
            goto _end;
        };

        a->gid = gid;
        a->valid.gid = 1;
    };

_end:
    return r;
};


#else


nai_int_t nai_proc_attr_set_user(
    nai_proc_attr_t* a, const char* name, const char* passwd)
{
    (void)a;
    (void)name;
    (void)passwd;

    return 0;
};


nai_int_t nai_proc_attr_set_group(nai_proc_attr_t* a, const char* name)
{
    (void)a;
    (void)name;

    return 0;
};


#endif


nai_int_t nai_proc_attr_set_log(nai_proc_attr_t* a, nai_log_t* log)
{
    nai_int_t r;


    a->log = log;
    r = 0;

    return r;
};


