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
/// @file       nai_proc.c
/// @brief      
/// @details
/// @date       2020-12-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_proc.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_string.h"


//////////////////////////////////////////////////////////////////////////////
// cpuset


nai_int_t nai_cpuset_equal_s(size_t size, 
    const nai_cpuset_t* s1, const nai_cpuset_t* s2)
{
    nai_int_t r;
    intptr_t n;
    intptr_t max = size / sizeof(nai_cpumask_t);


    for (n = 0; n < max; n ++) {
        if (s1->bits[n] != s2->bits[n]) {
            r = 0;
            goto _end;
        };
    };

    r = 1;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// proc


#if defined(_WIN32)


#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"
#include "win/nai_windows.h"
#include <windef.h>
#include <winbase.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <process.h>
#include <consoleapi.h>
#include <consoleapi2.h>


#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )
#define WAIT_ABANDONED      ((STATUS_ABANDONED_WAIT_0 ) + 0 )
#endif


static nai_int_t nai_psult(nai_int_t n)
{
    if (!n) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        return -1;
    };

    return 0;
};

static nai_int_t nai_wsult(nai_int_t n)
{
    nai_int_t r;


    switch (n) {
    case WAIT_OBJECT_0:
        r = 0;
        break;
    case WAIT_ABANDONED:
        r = -1;
        break;
    case WAIT_TIMEOUT:
        nai_errno = ETIMEDOUT;
        r = -1;
        break;
    default:
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        break;
    };

    return r;
};


static nai_int_t nai_win_proc_priority(nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;


    if (sched == NAI_SCHED_UNSET) {
        r = 0;
        goto _end;
    };

    if (sched != NAI_SCHED_NORMAL) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (priority < 0) {
        priority = -priority;
    };
    if (priority < NAI_PRIORITY_LOWEST || 
        priority > NAI_PRIORITY_HIGHEST) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (priority < NAI_PRIORITY_LOWER) {
        r = IDLE_PRIORITY_CLASS;
    } else if (priority < NAI_PRIORITY_NORMAL) {
        r = BELOW_NORMAL_PRIORITY_CLASS;
    } else if (priority < NAI_PRIORITY_HIGHER) {
        r = NORMAL_PRIORITY_CLASS;
    } else if (priority < NAI_PRIORITY_HIGHEST) {
        r = ABOVE_NORMAL_PRIORITY_CLASS;
    } else {
        r = HIGH_PRIORITY_CLASS;
    };

_end:
    return r;
};



typedef struct nai_proc_wait_s {
    nai_rbnode_t node;
    nai_list_t ent;
    nai_pid_t pid;
    nai_int_t exited;
    HANDLE handle;
    HANDLE wait;
} nai_proc_wait_t;


typedef struct nai_proc_map_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_cond_t cond;
    nai_mutex_t lock;
    nai_rbtree_t map;
    nai_list_t exited;
    nai_int_t waiting;
} nai_proc_map_t;


static nai_proc_map_t nai_procmap = {
    0, 0, NAI_ONCE_INIT
};


static nai_sighandle_f nai_sigchld = SIG_DFL;


static void nai_proc_map_init()
{
    nai_int_t r;
    nai_proc_map_t* s;


    s = &nai_procmap;
    nai_cond_init(&s->cond);
    nai_mutex_init(&s->lock);
    nai_rbtree_init(&s->map);
    nai_list_init(&s->exited);

    r = nai_cond_open(&s->cond, 0);
    if (r < 0) {
        s->error = nai_errno;
        goto _end;
    };

    r = nai_mutex_open(&s->lock, 0);
    if (r < 0) {
        s->error = nai_errno;
        goto _end;
    };

_end:
    nai_memory_barrier();
    nai_procmap.inited = 1;
    return;
};


static void nai_proc_map_term()
{
    if (nai_procmap.inited) {
        nai_cond_close(&nai_procmap.cond);
        nai_mutex_close(&nai_procmap.lock);
    };

    return;
};


static nai_proc_map_t* nai_proc_map_get()
{
    nai_proc_map_t* r;


    if (nai_procmap.inited == 0) {
        nai_once(&nai_procmap.once, nai_proc_map_init);
    };
    if (nai_procmap.error) {
        nai_errno = nai_procmap.error;
        r = 0;
        goto _end;
    };

    r = &nai_procmap;

_end:
    return r;
};


static nai_rbnode_t** nai_proc_find(
    nai_rbtree_t* t, nai_pid_t pid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_proc_wait_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_proc_wait_t, node);
        if (e->pid == pid) {
            break;
        } else if (e->pid >= pid) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static void WINAPI nai_proc_exited(void* data, BOOLEAN timedout)
{
    nai_int_t r;
    nai_int_t signal;
    nai_pid_t pid;
    nai_rbnode_t** n;
    nai_proc_map_t* s;
    nai_proc_wait_t* w;
    nai_sighandle_f chld;
    HANDLE wait;


    (void)timedout;
    pid = (nai_pid_t)(intptr_t)data;

    s = nai_proc_map_get();
    if (s == 0) {
        r = -1;
        goto _end;
    };


    nai_mutex_lock(&s->lock);

    n = nai_proc_find(&s->map, pid, 0);
    if (n[0] == 0) {
        signal = 0;
        wait = 0;
    } else {
        signal = s->waiting;

        w = (nai_proc_wait_t*)n[0];
        w->exited = 1;
        nai_list_entry_remove(&w->ent);
        nai_list_insert_tail(&s->exited, &w->ent);

        wait = w->wait;
        w->wait = 0;
    };

    nai_mutex_unlock(&s->lock);


    if (signal) {
        nai_cond_signal(&s->cond);
    };
    if (wait) {
        UnregisterWaitEx(wait, INVALID_HANDLE_VALUE);
    };


    chld = nai_sigchld;
    if (chld != 0 && 
        chld != SIG_IGN && chld != SIG_DFL) {
        chld(SIGCHLD);
    };

_end:
    return;
};


static nai_int_t nai_proc_add(nai_pid_t pid, HANDLE handle)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_proc_map_t* s;
    nai_proc_wait_t* w;


    s = nai_proc_map_get();
    if (s == 0) {
        r = -1;
        goto _end;
    };

    nai_mutex_lock(&s->lock);

    n = nai_proc_find(&s->map, pid, &parent);
    if (n[0] != 0) {
        ec = EEXIST;
        w = 0;
        goto _fail;
    };

    w = (nai_proc_wait_t*)nai_malloc(sizeof(*w));
    if (w == 0) {
        ec = nai_errno;
        goto _fail;
    };

    w->exited = 0;
    w->pid = pid;
    w->handle = handle;

    if (!RegisterWaitForSingleObject(
        &w->wait, w->handle, nai_proc_exited, (void*)(intptr_t)pid, -1,
        WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE)) {
        ec = nai_errno_from_oserr(GetLastError());
        goto _fail;
    };

    nai_rbtree_link(&s->map, &w->node, parent, n);
    nai_rbtree_color(&s->map, &w->node);
    nai_list_init(&w->ent);

    nai_mutex_unlock(&s->lock);
    r = 0;

_end:
    return r;

_fail:
    nai_mutex_unlock(&s->lock);

    if (w != 0) {
        nai_free(w);
    };

    nai_errno = ec;
    r = -1;
    goto _end;
};


static HANDLE nai_proc_handle(nai_proc_t* p)
{
    nai_int_t ec;
    nai_rbnode_t** n;
    nai_proc_map_t* s;
    nai_proc_wait_t* w;
    HANDLE h;


    if (p == 0 || 
        p->pid == (nai_pid_t)GetCurrentProcessId()) {
        h = GetCurrentProcess();
    } else {
        s = nai_proc_map_get();
        if (s == 0) {
            h = 0;
            goto _end;
        };

        /* find process */
        nai_mutex_lock(&s->lock);

        n = nai_proc_find(&s->map, p->pid, 0);
        if (n[0] == 0) {
            ec = ENOENT;
            goto _fail;
        };

        w = (nai_proc_wait_t*)n[0];
        h = w->handle;

        nai_mutex_unlock(&s->lock);
    };

_end:
    return h;

_fail:
    nai_mutex_unlock(&s->lock);
    nai_errno = ec;
    h = 0;
    goto _end;
};



nai_proc_t nai_proc_self()
{
    nai_proc_t r;

    r.pid = (nai_pid_t)GetCurrentProcessId();
    r.stat = 0;
    return r;
};


nai_pid_t nai_get_pid()
{
    return (nai_pid_t)GetCurrentProcessId();
};


nai_pid_t nai_proc_get_pid(nai_proc_t* p)
{
    nai_int_t r;


    if (p == 0 || 
        p->pid == 0) {
        r = (nai_pid_t)GetCurrentProcessId();
    } else if (p->pid > 0) {
        r = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
    };

    return r;
};


nai_pid_t nai_proc_spawn(nai_proc_t* p, 
    const char* path, const nai_proc_attr_t* a, 
    const char* args[], const char* env[])
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t space;
    size_t len;
    size_t buflen;
    wchar_t* buf;
    wchar_t* wpath = 0;
    wchar_t* wcwd = 0;
    wchar_t* wcmd = 0;
    wchar_t* envblock = 0;
    nai_wcs_t ws;
    nai_proc_attr_t attr;
    DWORD createflags = CREATE_UNICODE_ENVIRONMENT;
    DWORD reset[6];
    DWORD e;
    HANDLE h;
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR* sd;
    char sdbuf[SECURITY_DESCRIPTOR_MIN_LENGTH];


    len = 0;
    if (path) {
        len += nai_strlen(path) + 1;
    };
    if (args) {
        for (n = 0; args[n]; n ++) {
            len += nai_strlen(args[n]) + 1;
            if (nai_strchr(args[n], ' ') || nai_strchr(args[n], '\t')) {
                len += 2;
            };
        }
    };
    if (env) {
        for (n = 0; env[n]; n ++) {
            len += nai_strlen(env[n]) + 1;
        };
        len ++;
    };

    if (a == 0) {
        a = &attr;
        nai_proc_attr_init(&attr);
    };

    if (a->cwd) {
        len += nai_strlen(a->cwd) + 1;
    }

    r = nai_wcs_buffer(&ws);
    if (r < 0) {
        goto _end;
    };

    if (ws.tbuflen > len) {
        buflen = ws.tbuflen;
        buf = ws.tbuf;
    } else {
        buflen = len;
        buf = ws.buf = (wchar_t*)nai_malloc(len * sizeof(wchar_t));
        if (buf == 0) {
            r = -1;
            goto _end;
        };
    };

    if (path) {
        wpath = buf;
        r = nai_wcs_from_utf8(buf, buflen, path, -1);
        if (r < 0) {
            goto _end;
        } else if (r > (intptr_t)buflen) {
            goto _error;
        };
        buf += r;
        buflen -= r;
    };
    if (args) {
        wcmd = buf;
        for (n = 0; args[n]; n ++) {
            space = 0;
            if (nai_strchr(args[n], ' ') || nai_strchr(args[n], '\t')) {
                space = 1;
            };
            if (space) {
                if (buflen < 1) {
                    goto _error;
                };
                buf[0] = '"';
                buf ++;
                buflen --;
            };

            r = nai_wcs_from_utf8(buf, buflen, args[n], -1);
            if (r < 0) {
                goto _end;
            } else if (r > (intptr_t)buflen) {
                goto _error;
            };
            r --;
            buf += r;
            buflen -= r;

            if (space) {
                if (buflen < 2) {
                    goto _error;
                };
                buf[0] = '"';
                buf ++;
                buflen --;
            };

            buf[0] = ' ';
            buf ++;
            buflen --;
        };

        if (buf > wcmd) {
            buf[-1] = 0;
        };
    };
    if (env) {
        envblock = buf;
        for (n = 0; env[n]; n ++) {
            r = nai_wcs_from_utf8(buf, buflen, env[n], -1);
            if (r < 0) {
                goto _end;
            } else if (r > (intptr_t)buflen) {
                goto _error;
            };
            buf += r;
            buflen -= r;
        };

        if (buflen < 1) {
            goto _error;
        };
        buf[0] = 0;
        buf ++;
        buflen --;
    };
    if (a->cwd) {
        wcwd = buf;
        r = nai_wcs_from_utf8(buf, buflen, a->cwd, -1);
        if (r < 0) {
            goto _end;
        } else if (r > (intptr_t)buflen) {
            goto _error;
        };
        buf += r;
        buflen -= r;
    };


    nai_memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (a->valid.daemon) {
        si.dwFlags |= STARTF_USESHOWWINDOW;
        createflags |= DETACHED_PROCESS;
    };
    if (a->valid.suspend) {
        createflags |= CREATE_SUSPENDED;

    };

    if (a->valid.sched) {
        r = nai_win_proc_priority(a->sched, a->priority);
        if (r == -1) {
            goto _end;
        };
        createflags |= r;
    };

    if (a->valid.stdio) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        nai_memset(reset, 0, sizeof(reset));
        for (n = 0; n < (intptr_t)nai_countof(a->stdio); n ++) {
            h = nai_get_stdio(n);
            if (a->valid.stdio & (1 << n)) {
                if (h != NAI_FD_INVALID) {
                    if (!GetHandleInformation(h, &reset[n])) {
                        e = GetLastError();
                        r = 0;
                        goto _fail;
                    };
                    if (reset[n] & HANDLE_FLAG_INHERIT) {
                        if (!SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0)) {
                            e = GetLastError();
                            r = 0;
                            goto _fail;
                        };
                    };
                };
                h = a->stdio[n];
                if (h != NAI_FD_INVALID) {
                    if (!GetHandleInformation(h, &reset[n+3])) {
                        e = GetLastError();
                        r = 0;
                        goto _fail;
                    };
                    if (!(reset[n+3] & HANDLE_FLAG_INHERIT)) {
                        if (!SetHandleInformation(h, 
                            HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
                            e = GetLastError();
                            r = 0;
                            goto _fail;
                        };
                    };
                };
            };

            switch (n) {
            case 0:
                si.hStdInput = h;
                break;
            case 1:
                si.hStdOutput = h;
                break;
            case 2:
                si.hStdError = h;
                break;
            };
        };
    };

    if (a->cpu.set) {
        createflags |= CREATE_SUSPENDED;
    };

    sd = (SECURITY_DESCRIPTOR*)sdbuf;
    InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(sd, -1, 0, 0);
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = sd;
    sa.bInheritHandle = FALSE;
    if (a->token) {

        r = ImpersonateLoggedOnUser(a->token);
        if (!r) {
            e = GetLastError();
            goto _fail;
        };

        r = CreateProcessAsUserW(a->token, wpath, 
            wcmd, &sa, 0, TRUE, createflags, envblock, wcwd, &si, &pi);
        if (!r) {
            e = GetLastError();
        };

        RevertToSelf();

    } else {
        r = CreateProcessW(wpath, 
            wcmd, &sa, 0, TRUE, createflags, envblock, wcwd, &si, &pi);
        if (!r) {
            e = GetLastError();
        };
    }

_fail:
    if (a->valid.stdio) {
        for (n = 0; n < (intptr_t)nai_countof(a->stdio); n ++) {
            if (a->valid.stdio & (1 << n)) {
                if (reset[n] & HANDLE_FLAG_INHERIT) {
                    h = nai_get_stdio(n);
                    SetHandleInformation(h, 
                        HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
                };
                h = a->stdio[n];
                if (h != NAI_FD_INVALID) {
                    if (!(reset[n+3] & HANDLE_FLAG_INHERIT)) {
                        SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);
                    };
                };
            };
        };
    };

    if (!r) {
        nai_errno = nai_errno_from_oserr(e);
        r = -1;
        goto _end;
    };

    p->pid = (nai_pid_t)pi.dwProcessId;
    p->stat = 0;
    p->detach = a->valid.daemon;
    p->suspend = a->valid.suspend;
    if (a->cpu.set) {
        r = nai_proc_set_affinity(p, a->cpu.size, a->cpu.set);
        if (r >= 0 && !a->valid.suspend) {
            if (!ResumeThread(pi.hThread)) {
                nai_errno = nai_errno_from_oserr(GetLastError());
                r = -1;
            };
        };
        if (r < 0) {
            goto _clean;
        };
    };

    r = nai_proc_add(pi.dwProcessId, pi.hProcess);
    if (r < 0) {
        goto _clean;
    };

    CloseHandle(pi.hThread);
    r = p->pid;

_end:
    nai_wcs_release(&ws, r);
    return (nai_pid_t)r;

_error:
    nai_errno = NAI_EFAILED;
    r = -1;
    goto _end;

_clean:
    ec = nai_errno;
    TerminateProcess(pi.hProcess, -1);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    p->pid = 0;
    p->stat = 0;
    nai_errno = ec;
    goto _end;
}


nai_pid_t nai_proc_fork(nai_proc_t* p, nai_int_t suspend)
{
    nai_errno = ENOTSUP;
    return -1;
};


nai_pid_t nai_proc_wait(nai_proc_t* p, nai_int_t* code, nai_int_t nowait)
{
    nai_int_t r;
    nai_int_t ec;
    nai_pid_t pid;
    nai_list_entry_t* e;
    nai_rbnode_t** n;
    nai_proc_map_t* s;
    nai_proc_wait_t* w;
    DWORD status;
    HANDLE h;


    if (p == 0) {
        pid = -1;
    } else {
        pid = p->pid;
        if (pid < -1) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };
        if (pid == (nai_pid_t)GetCurrentProcessId()) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    s = nai_proc_map_get();
    if (s == 0) {
        r = -1;
        goto _end;
    };

    if (pid <= 0) {

        nai_mutex_lock(&s->lock);

        /* wait exited process */
        while (nai_list_is_empty(&s->exited)) {
            if (nowait) {
                ec = EINPROGRESS;
                goto _fail;
            };

            s->waiting ++;
            nai_cond_wait(&s->cond, &s->lock);
            s->waiting --;
        };

        /* remove from map */
        e = s->exited.next;
        w = nai_containof(e, nai_proc_wait_t, ent);
        nai_rbtree_erase(&s->map, &w->node);
        nai_list_entry_remove(&w->ent);


        /* wakeup others */
        if (s->waiting > 0) {
            nai_cond_signal(&s->cond);
        };

        nai_mutex_unlock(&s->lock);

    } else {

        /* find process */
        nai_mutex_lock(&s->lock);

        n = nai_proc_find(&s->map, pid, 0);
        if (n[0] == 0) {
            ec = ENOENT;
            goto _fail;
        };

        w = (nai_proc_wait_t*)n[0];
        if (w->exited) {
            nai_rbtree_erase(&s->map, &w->node);
            nai_list_entry_remove(&w->ent);
            nai_mutex_unlock(&s->lock);
            goto _exited;
        };

        h = w->handle;

        nai_mutex_unlock(&s->lock);


        /* waiting */
        status = WaitForSingleObject(h, nowait ? 0 : -1);
        switch (status) {
        case WAIT_OBJECT_0:
            r = 0;
            break;
        case WAIT_TIMEOUT:
            nai_errno = EINPROGRESS;
            r = -1;
            break;
        default:
            r = nai_wsult(status);
            break;
        };
        if (r < 0) {
            goto _end;
        };


        /* exited, remove from proc map */
        nai_mutex_lock(&s->lock);

        n = nai_proc_find(&s->map, pid, 0);
        if (n[0] == 0) {
            ec = ENOENT;
            goto _fail;
        };

        w = (nai_proc_wait_t*)n[0];
        nai_rbtree_erase(&s->map, &w->node);
        nai_list_entry_remove(&w->ent);

        nai_mutex_unlock(&s->lock);
    };

_exited:
    /* get exit code */
    if (GetExitCodeProcess(w->handle, code)) {
        r = 0;
    } else {
        ec = nai_errno_from_oserr(GetLastError());
        r = -1;
    };

    /* close wait handle */
    if (w->wait) {
        UnregisterWaitEx(w->wait, INVALID_HANDLE_VALUE);
    };

    /* return pid */
    if (r < 0) {
        nai_errno = ec;
    } else {

        r = w->pid;

        /* close process handle */
        CloseHandle(w->handle);
        nai_free(w);
    };

_end:
    return r;

_fail:
    nai_mutex_unlock(&s->lock);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_proc_kill(nai_proc_t* p, nai_int_t signum)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t code;
    HANDLE h;


    if (p == 0 || 
        p->pid < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (signum < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    h = nai_proc_handle(p);
    if (h == 0) {
        r = -1;
        goto _end;
    };

    switch (signum) {
    case SIGTERM:
    case SIGKILL:
    case SIGINT:

        r = TerminateProcess(h, 1);
        if (r) {
            r = 0;
            break;
        };

        ec = GetLastError();
        if (ec == ERROR_ACCESS_DENIED && 
            GetExitCodeProcess(h, &code) && 
            code != STILL_ACTIVE) {
            nai_errno = ESRCH;
        } else {
            nai_errno = nai_errno_from_oserr(ec);
        };

        r = -1;
        break;

    case SIGBREAK:

        r = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, p->pid);
        if (r) {
            r = 0;
            break;
        };

        ec = GetLastError();
        if (ec == ERROR_ACCESS_DENIED && 
            GetExitCodeProcess(h, &code) && 
            code != STILL_ACTIVE) {
            nai_errno = ESRCH;
        } else {
            nai_errno = nai_errno_from_oserr(ec);
        };
        break;

    case SIGCONT:

        if (!p->suspend) {
            r = 0;
            break;
        };

        if (nai_wapi.inited == 0) {
            nai_wapi_init();
        };

        ec = nai_wapi.NtResumeProcess(h);
        if (NT_SUCCESS(ec)) {
            p->suspend = 0;
            r = 0;
        } else {
            ec = nai_wapi.RtlNtStatusToDosError(ec);
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
        };
        break;

    case SIGSTOP:

        if (p->suspend) {
            r = 0;
            break;
        };

        if (nai_wapi.inited == 0) {
            nai_wapi_init();
        };

        ec = nai_wapi.NtSuspendProcess(h);
        if (NT_SUCCESS(ec)) {
            p->suspend = p->pid != (nai_pid_t)GetCurrentProcessId();
            r = 0;
        } else {
            ec = nai_wapi.RtlNtStatusToDosError(ec);
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
        };
        break;

    case 0:

        r = GetExitCodeProcess(h, &code);
        if (!r) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
        } else if (code != STILL_ACTIVE) {
            nai_errno = ESRCH;
            r = -1;
        } else {
            r = 0;
        };
        break;

    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


nai_int_t nai_proc_set_sched(
    nai_proc_t* p, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_int_t sched_priority;
    HANDLE handle;


    r = nai_win_proc_priority(sched, priority);
    if (r <= 0) {
        goto _end;
    };

    handle = nai_proc_handle(p);
    if (handle == 0) {
        r = -1;
        goto _end;
    };

    sched_priority = r;
    if (!SetPriorityClass(handle, sched_priority)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_get_affinity(
    nai_proc_t* p, size_t size, nai_cpuset_t* set)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t units;
    HANDLE handle;
    USHORT groupc;
    USHORT groups[NAI_CPUSET_COUNT/NAI_CPUMASK_BITS];


    if (set == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    units = (nai_int_t)(size*8 + NAI_CPUMASK_BITS - 1) / NAI_CPUMASK_BITS;
    assert(units > 0);


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    handle = nai_proc_handle(p);
    if (handle == 0) {
        r = -1;
        goto _end;
    };

    if (nai_wapi.GetProcessGroupAffinity) {
        groupc = nai_countof(groups);
        if (!nai_wapi.GetProcessGroupAffinity(handle, &groupc, groups)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        nai_cpuset_zero_s(size, set);

        for (n = 0; n < (nai_int_t)groupc; n ++) {
            if (units <= (nai_int_t)groups[n]) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            set->bits[groups[n]] = -1;
        };

    } else {
        if (units <= 1) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if (!GetProcessAffinityMask(handle, &set->bits[0], 0)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        nai_memset(&set->bits[1], 0, (units-1) * sizeof(set->bits[0]));
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_set_affinity(
    nai_proc_t* p, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t units;
    nai_int_t group = 0;
    HANDLE handle;
    USHORT groupc;
    USHORT groups[NAI_CPUSET_COUNT/NAI_CPUMASK_BITS];


    if (set == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    units = (nai_int_t)(size*8 + NAI_CPUMASK_BITS - 1) / NAI_CPUMASK_BITS;
    assert(units > 0);

    /* find group, not support more than one group */
    group = -1;
    for (n = 0; n < units; n ++) {
        if (!set->bits[n]) {
            continue;
        };

        if (group >= 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        group = n;
    };


    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    handle = nai_proc_handle(p);
    if (handle == 0) {
        r = -1;
        goto _end;
    };

    if (nai_wapi.GetProcessGroupAffinity) {
        groupc = nai_countof(groups);
        if (!nai_wapi.GetProcessGroupAffinity(handle, &groupc, groups)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        /* not support more than one group */
        if (groupc > 1) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        /* mask of group should be valid */
        if (group != (nai_int_t)groups[0]) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    } else {
        /* mask of group should be valid */
        if (group != 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    if (!SetProcessAffinityMask(handle, set->bits[group])) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_exec(const char* path, 
    const nai_proc_attr_t* attr, 
    const char* args[], const char* env[])
{
    (void)path;
    (void)attr;
    (void)args;
    (void)env;

    nai_errno = ENOTSUP;
    return -1;
};


nai_exit(nai_int_t code)
{
    exit(code);
    return -1;
};


nai_int_t nai_detach(nai_int_t daemon)
{
    nai_int_t r;

    r = 0;
    return r;
};


nai_int_t nai_fork_at(nai_fork_cb_f pre, 
    nai_fork_cb_f parent, nai_fork_cb_f child)
{
    nai_int_t r;


    (void)pre;
    (void)parent;
    (void)child;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_set_env(const char* name, const char* value)
{
    nai_int_t r;
    nai_wcs_t ws;


    if (name == 0) {
        nai_errno = EINVAL;
        r = -1;
        return r;
    };

    if (value == 0) {
        r = nai_wcs_from_1str(&ws, name);
    } else {
        r = nai_wcs_from_2str(&ws, name, value);
    };
    if (r < 0) {
        goto _end;
    };

    /* setenv */
    r = SetEnvironmentVariableW(ws.s[0], ws.s[1]);
    if (r == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


intptr_t nai_get_env(const char* name, char* buf, size_t buflen)
{
    nai_int_t ec;
    intptr_t r;
    intptr_t datalen;
    wchar_t* data = 0;
    nai_wcs_t ws;


    if (name == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_wcs_from_1str(&ws, name);
    if (r < 0) {
        goto _end;
    };

    data = ws.tbuf;
    datalen = ws.tbuflen;
    for (;;) {
        r = GetEnvironmentVariableW(ws.str, data, (nai_int_t)datalen);
        if (r == 0) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _fail;
        };
        if (datalen >= r) {
            datalen = r;
            break;
        };

        if (data != ws.tbuf) {
            nai_free(data);
        }
        datalen = r;
        data = nai_malloc(datalen * sizeof(*data));
        if (data == 0) {
            r = -1;
            goto _fail;
        };
    };

    r = nai_wcs_to_utf8(buf, buflen, data, datalen+1);
    if (r > 0) {
        r --;
    };

_fail:
    if (r < 0) {
        ec = nai_errno;
    };
    if (data != ws.tbuf) {
        nai_free(data);
    };

    nai_wcs_release(&ws, 0);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


nai_int_t nai_get_rlimit(nai_int_t res, nai_rlimit_t* limit)
{
    nai_errno = ENOTSUP;
    return -1;
};


nai_int_t nai_set_rlimit(nai_int_t res, const nai_rlimit_t* limit)
{
    nai_errno = ENOTSUP;
    return -1;
};



#else


#include "unix/nai_unix.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#if (NAI_HAVE_PTHREAD_H)
#include <pthread.h>
#endif
#if (NAI_HAVE_SCHED_H)
#include <sched.h>
#endif
#if (NAI_HAVE_SYS_CPUSET_H)
#include <sys/param.h>
#include <sys/cpuset.h>
#endif
#if (NAI_HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif
#if (NAI_HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif


static nai_pid_t nai_fork(
    nai_int_t suspend, nai_int_t spawn, nai_log_t* l)
{
    nai_int_t r;

#if (NAI_HAVE_FORK)

    nai_int_t n;
    nai_int_t ec;
    nai_int_t sig;
    nai_int_t status;
    nai_pid_t pid;
    sigset_t signewset;
    sigset_t sigoldset;


    sigfillset(&signewset);
    sigdelset(&signewset, SIGKILL);
    sigdelset(&signewset, SIGSTOP);
    sigdelset(&signewset, SIGTRAP);
    sigdelset(&signewset, SIGSEGV);
    sigdelset(&signewset, SIGBUS);
    sigdelset(&signewset, SIGILL);
    sigdelset(&signewset, SIGSYS);
    sigdelset(&signewset, SIGABRT);

    r = nai_sigprocmask(SIG_BLOCK, &signewset, &sigoldset);
    if (r < 0) {
        goto _end;
    };


    r = fork();
    if (r < 0) {
        ec = nai_errno;
    } else if (r == 0) {

        /* child, is fork for spawn */
        if (spawn) {
            /* clear all signal handlers */
            for (n = 1; n < 32; n ++) {
                if (n == SIGKILL || n == SIGSTOP) {
                    continue;
                };

                if (nai_signal(n, SIG_DFL) == SIG_ERR) {
                    if (l != 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_ARGS(l), ec, 
                            "child reset signal handlers failed");
                    };

                    nai_exit(-1);
                };
            };

            /* clear blocked sigset which will be restore later */
            sigemptyset(&sigoldset);
        };

        /* child, is auto suspended after fork */
        if (suspend) {
            /* wait signal from parent */
            sigemptyset(&signewset);
            sigaddset(&signewset, SIGUSR1);
            r = sigwait(&signewset, &sig);
            if (r != 0) {
                if (l != 0) {
                    ec = r;
                    nai_log_error(NAI_LOG_ARGS(l), ec, 
                        "child wait signal of suspend failed");
                };
                nai_exit(-1);

                /* to disable warning for gcc */
                ec = 0;
            };
        };

    } else {
        /* parent, is auto suspended after fork */
        if (suspend) {
            pid = r;

            /* stop child */
            r = kill(pid, SIGSTOP);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            /* wakeup 'sigwait' in child */
            r = kill(pid, SIGUSR1);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            r = pid;

            if (0) {
_fail:
                /* kill child */
                n = kill(pid, SIGKILL);
                if (n >= 0) {
                    /* waiting */
                    do {
                        n = waitpid(pid, &status, 0);
                    } while (n < 0 && nai_errno == EINTR);
                };
            };
        };
    };


    n = nai_sigprocmask(SIG_SETMASK, &sigoldset, 0);
    if (n < 0) {
        /* fixme: ignored error */
        ;
    };

    if (r < 0) {
        nai_errno = ec;
    };

_end:

#else

    (void)suspend;
    (void)spawn;
    (void)l;

    nai_errno = ENOTSUP;
    r = -1;

#endif

    return r;
};


nai_proc_t nai_proc_self()
{
    nai_proc_t r;

    r.pid = getpid();
    r.stat = 0;
    return r;
}


nai_pid_t nai_get_pid()
{
    return getpid();
};


nai_pid_t nai_proc_get_pid(nai_proc_t* p)
{
    nai_int_t r;


    if (p == 0 || 
        p->pid == 0) {
        r = getpid();
    } else if (p->pid > 0) {
        r = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
    };

    return r;
};


nai_pid_t nai_proc_spawn(nai_proc_t* p, 
    const char* path, const nai_proc_attr_t* a, 
    const char* args[], const char* env[])
{
    nai_int_t r;
    nai_log_t* l;
    nai_proc_attr_t attr;


    if (p == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (a == 0) {
        a = &attr;
        nai_proc_attr_init(&attr);
    };

    if (a->cwd) {
        r = access(a->cwd, X_OK);
        if (r < 0) {
            goto _end;
        };
    };

    l = a->log;
    if (l == 0) {
        l = &nai_log_core;
    };

    r = nai_fork(a->valid.suspend, 1, l);
    if (r < 0) {
        goto _end;
    };


    /* in child process */
    if (r == 0) {

        r = nai_exec(path, a, args, env);
        if (r < 0) {
            nai_exit(nai_errno);
        } else {
            assert(0);
            nai_exit(-1);
        };
    };


    /* in parent process */
    p->pid = r;
    p->stat = 0;
    p->detach = a->valid.daemon;
    p->suspend = a->valid.suspend;


_end:
    return r;
};


nai_pid_t nai_proc_fork(nai_proc_t* p, nai_int_t suspend)
{
    nai_int_t r;


    if (p == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_fork(suspend, 0, 0);
    if (r < 0) {
        goto _end;
    };

    if (r > 0) {
        p->stat = 0;
        p->suspend = !!suspend;
        p->pid = r;
    } else {
        p->stat = 0;
        p->pid = getpid();
    };

_end:
    return r;
};


nai_pid_t nai_proc_wait(nai_proc_t* p, nai_int_t* code, nai_int_t nowait)
{
    nai_int_t r;
    nai_int_t flags;
    nai_int_t status;
    nai_pid_t pid;


    if (p == 0) {
        pid = -1;
    } else {
        if (p->pid == getpid()) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        pid = p->pid;
    };

    flags = WUNTRACED | (nowait ? WNOHANG : 0);
    do {
        r = waitpid(pid, &status, flags);
    } while (r < 0 && nai_errno == EINTR);

    if (r < 0) {
        goto _end;
    };
    if (r == 0) {
        nai_errno = EINPROGRESS;
        r = -1;
        goto _end;
    };

    if (WIFEXITED(status)) {
        if (code) {
            code[0] = WEXITSTATUS(status);
        };
    } else if (WIFSIGNALED(status)) {
        if (code) {
            code[0] = WTERMSIG(status) | __NAI_SIGNALED;
        };
    } else if (WIFSTOPPED(status)) {
        if (code) {
            code[0] = 0 | __NAI_STOPPED;
        };
    } else {
        /* unexcepted case */
        nai_errno = ECHILD;
        r = -1;
    };

_end:
    return r;
};


nai_int_t nai_proc_kill(nai_proc_t* p, nai_int_t signum)
{
    nai_int_t r;
#if 0
    nai_int_t status;
#endif


    if (p == 0 || p->pid == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (signum) {
    case SIGCONT:
        r = kill(p->pid, signum);
        if (r < 0) {
            goto _end;
        };

        /* mark already suspended, unneed block wait in next time */
        p->suspend = 0;

#if 0
        do {
            r = waitpid(p->pid, &status, WCONTINUED);
        } while (r < 0 && nai_errno == EINTR);

        if (r < 0) {
            goto _end;
        };

        if (!WIFCONTINUED(status)) {
            nai_errno = ECHILD;
            r = -1;
            goto _end;
        };
#endif

        break;

    case SIGSTOP:
        r = kill(p->pid, signum);
        if (r < 0) {
            goto _end;
        };

        p->suspend = 1;

        /* need waiting ? */
#if 0
        do {
            r = waitpid(p->pid, &status, WUNTRACED);
        } while (r < 0 && nai_errno == EINTR);

        if (r < 0) {
            goto _end;
        };

        if (!WIFSTOPPED(status) || 
            (WSTOPSIG(status) != SIGSTOP && WSTOPSIG(status) != SIGTSTP)) {
            nai_errno = ECHILD;
            r = -1;
            goto _end;
        };
#endif
        break;

    default:
        r = kill(p->pid, signum);
        break;
    };


_end:
    return r;
};


nai_int_t nai_proc_set_sched(
    nai_proc_t* p, nai_int_t sched, nai_int_t priority)
{
    nai_int_t r;
    nai_pid_t pid;
    nai_sched_priority_t si;
#if (NAI_HAVE_SCHED_SETSCHEDULER)
    struct sched_param sp;
#endif


    if (p == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid > 0) {
        pid = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = nai_sched_query_priority(&si, sched, priority);
    if (r <= 0) {
        goto _end;
    };


#if (NAI_HAVE_SCHED_SETSCHEDULER)
    if (si.sched == SCHED_OTHER) {
        sp.sched_priority = 0;
    } else {
        sp.sched_priority = si.priority;
    };

    r = sched_setscheduler(pid, si.sched, &sp);
    if (r < 0) {
        goto _end;
    };
#endif

    if (si.sched == SCHED_OTHER) {
        r = setpriority(PRIO_PROCESS, pid, si.priority);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_proc_get_affinity(
    nai_proc_t* p, size_t size, nai_cpuset_t* set)
{
    nai_int_t r;

#if (NAI_HAVE_SCHED_GETAFFINITY)
    nai_pid_t pid;


    if (p == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid > 0) {
        pid = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = sched_getaffinity(pid, size, (cpu_set_t*)set);

_end:

#elif (NAI_HAVE_CPUSET_GETAFFINITY)
    nai_pid_t pid;


    if (p == 0) {
        pid = -1; /* current process */
    } else if (p->pid == 0) {
        pid = -1; /* current process */
    } else if (p->pid > 0) {
        pid = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = cpuset_getaffinity(CPU_LEVEL_WHICH, 
        CPU_WHICH_PID, pid, size, (cpuset_t*)set);

_end:

#else
    (void)p;
    (void)size;
    (void)set;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_proc_set_affinity(
    nai_proc_t* p, size_t size, const nai_cpuset_t* set)
{
    nai_int_t r;

#if (NAI_HAVE_SCHED_SETAFFINITY)
    nai_pid_t pid;


    if (p == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid == 0) {
        pid = getpid(); /* main thread of current process on linux */
    } else if (p->pid > 0) {
        pid = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = sched_setaffinity(pid, size, (cpu_set_t*)set);

_end:

#elif (NAI_HAVE_CPUSET_SETAFFINITY)
    nai_pid_t pid;


    if (p == 0) {
        pid = -1; /* current process */
    } else if (p->pid == 0) {
        pid = -1; /* current process */
    } else if (p->pid > 0) {
        pid = p->pid;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = cpuset_setaffinity(CPU_LEVEL_WHICH, 
        CPU_WHICH_PID, pid, size, (cpuset_t*)set);

_end:

#else
    (void)p;
    (void)size;
    (void)set;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_exec(const char* path, 
    const nai_proc_attr_t* a, 
    const char* args[], const char* env[])
{
    nai_int_t r;

#if (NAI_HAVE_EXECV)

    nai_int_t n;
    nai_int_t ec;
    nai_log_t* l;
    nai_proc_attr_t attr;


    if (a == 0) {
        a = &attr;
        nai_proc_attr_init(&attr);
    };

    l = a->log;
    if (l == 0) {
        l = &nai_log_core;
    };

    if (a->cwd) {
        r = access(a->cwd, X_OK);
        if (r < 0) {
            goto _end;
        };

        r = chdir(a->cwd);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "set current directory of child failed");
            goto _end;
        };
    };

#if (NAI_HAVE_UID)
    if (a->valid.gid && !geteuid()) {
        r = setgid(a->gid);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "set group of child failed");
            goto _end;
        };
    };
    if (a->valid.uid && !geteuid()) {
        r = setuid(a->uid);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "set user of child failed");
            goto _end;
        };
    };
#endif

#if (NAI_HAVE_RLIMIT)
    for (n = 0; n < (intptr_t)nai_countof(a->rlimit); n ++) {
        if (a->valid.rlimit & (1 << n)) {
            r = nai_set_rlimit(n, &a->rlimit[n]);
            if (r < 0) {
                ec = nai_errno;
                if (ec != ENOTSUP) {
                    nai_log_error(NAI_LOG_ARGS(l), ec, 
                        "set rlimit(%d) of child failed", n);
                    goto _end;
                };
            };
        };
    };
#endif

    if (a->cpu.set) {
        r = nai_proc_set_affinity(0, a->cpu.size, a->cpu.set);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "set affinity of child failed");
            goto _end;
        };
    };

    if (a->valid.sched) {
        r = nai_proc_set_sched(0, a->sched, a->priority);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "set priority of child failed");
            goto _end;
        };
    };

    if (a->valid.daemon) {
        r = nai_detach(1);
        if (r < 0) {
            nai_log_error(NAI_LOG_ARGS(l), 
                nai_errno, "detach child failed");
            goto _end;
        };
    };

    for (n = 0; n < (intptr_t)nai_countof(a->stdio); n ++) {
        if (a->valid.stdio & (1 << n)) {
            if (a->stdio[n] == NAI_FD_INVALID) {
                r = close(n);
                if (r < 0 && 
                    nai_errno != EBADF) {
                    nai_log_error(NAI_LOG_ARGS(l), 
                        nai_errno, "close stdio(%d) of child failed", n);
                    goto _end;
                };
            } else {
                r = nai_set_stdio(n, a->stdio[n]);
                if (r < 0) {
                    nai_log_error(NAI_LOG_ARGS(l), 
                        nai_errno, "set stdio(%d) of child failed", n);
                    goto _end;
                };
            };
        };
    };

    if (env == 0) {
        r = execv(path, (char* const*)args);
    } else {
        r = execve(path, (char* const*)args, (char* const*)env);
    };
    if (r < 0) {
        nai_log_error(NAI_LOG_ARGS(l), 
            nai_errno, "execv(%s) failed", path);
        goto _end;
    };

_end:

#else

    (void)path;
    (void)a;
    (void)args;
    (void)env;

    nai_errno = ENOTSUP;
    r = -1;

#endif

    return r;
};


nai_int_t nai_exit(nai_int_t code)
{
    exit(code);
    return -1;
};


nai_int_t nai_detach(nai_int_t daemon)
{
    nai_int_t r;


#if 0
    r = chdir("/");
    if (r < 0) {
        goto _end;
    };
#endif

    if (daemon) {
        r = nai_fork(0, 0, 0);
        if (r < 0) {
            goto _end;
        };

        if (r > 0) {
            nai_exit(0);
        };
    };

#if (NAI_HAVE_SETSID)
    if (daemon) {
        r = setsid();
        if (r < 0) {
            goto _end;
        };
    };
#else
    r = setpgid(0, 0);
    if (r < 0) {
        goto _end;
    };
#endif

    if (freopen("/dev/null", "r", stdin) == 0) {
        r = -1;
        goto _end;
    };
    if (freopen("/dev/null", "w", stdout) == 0) {
        r = -1;
        goto _end;
    };
    if (freopen("/dev/null", "w", stderr) == 0) {
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_fork_at(nai_fork_cb_f pre, 
    nai_fork_cb_f parent, nai_fork_cb_f child)
{
    nai_int_t r;


#if (NAI_HAVE_FORK) && (NAI_HAVE_PTHREAD_H)
    r = pthread_atfork(pre, parent, child);
    if (r != 0) {
        nai_errno = r;
        r = -1;
    };
#else
    (void)pre;
    (void)parent;
    (void)child;

    nai_errno = ENOTSUP;
    r = -1;
#endif

    return r;
};


nai_int_t nai_set_env(const char* name, const char* value)
{
    nai_int_t r;


    if (value == 0) {
        r = unsetenv(name);
    } else {
        r = setenv(name, value, 1);
    };

    return r;
};


intptr_t nai_get_env(const char* name, char* buf, size_t buflen)
{
    intptr_t r;
    char* str;


    str = getenv(name);
    if (str == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    r = nai_strlen(str);
    if (r <= (intptr_t)buflen) {
        nai_memcpy(buf, str, r+(r<(intptr_t)buflen));
    } else {
        nai_errno = ERANGE;
    };

_end:
    return r;
};


nai_int_t nai_get_rlimit(nai_int_t res, nai_rlimit_t* limit)
{
    nai_int_t r;
    nai_int_t which = -1;
#if (NAI_HAVE_SETRLIMIT)
    struct rlimit tmp;
#endif


    if (limit == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (res) {
    case NAI_RLIMIT_CPU:
#if defined(RLIMIT_CPU)
        which = RLIMIT_CPU;
#endif
        break;

    case NAI_RLIMIT_MEM:
#if defined(RLIMIT_AS)
        which = RLIMIT_AS;
#elif defined(RLIMIT_DATA)
        which = RLIMIT_DATA;
#elif defined(RLIMIT_VMEM)
        which = RLIMIT_VMEM;
#endif
        break;

    case NAI_RLIMIT_NPROC:
#if defined(RLIMIT_NPROC)
        which = RLIMIT_NPROC;
#endif
        break;

    case NAI_RLIMIT_NOFILE:
#if defined(RLIMIT_NOFILE)
        which = RLIMIT_NOFILE;
#endif
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#if (NAI_HAVE_GETRLIMIT)
    if (which == -1) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = getrlimit(which, (struct rlimit*)&tmp);
    if (r < 0) {
        goto _end;
    };

    limit->rlim_cur = tmp.rlim_cur;
    limit->rlim_max = tmp.rlim_max;
#else
    nai_errno = ENOTSUP;
    r = -1;
#endif

_end:
    return r;
};


nai_int_t nai_set_rlimit(nai_int_t res, const nai_rlimit_t* limit)
{
    nai_int_t r;
    nai_int_t which = -1;
#if (NAI_HAVE_SETRLIMIT)
    struct rlimit tmp;
#endif


    if (limit == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (res) {
    case NAI_RLIMIT_CPU:
#if defined(RLIMIT_CPU)
        which = RLIMIT_CPU;
#endif
        break;

    case NAI_RLIMIT_MEM:
#if defined(RLIMIT_AS)
        which = RLIMIT_AS;
#elif defined(RLIMIT_DATA)
        which = RLIMIT_DATA;
#elif defined(RLIMIT_VMEM)
        which = RLIMIT_VMEM;
#endif
        break;

    case NAI_RLIMIT_NPROC:
#if defined(RLIMIT_NPROC)
        which = RLIMIT_NPROC;
#endif
        break;

    case NAI_RLIMIT_NOFILE:
#if defined(RLIMIT_NOFILE)
        which = RLIMIT_NOFILE;
#endif
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#if (NAI_HAVE_SETRLIMIT)
    if (which == -1) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    tmp.rlim_cur = limit->rlim_cur;
    tmp.rlim_max = limit->rlim_max;
    r = setrlimit(which, (struct rlimit*)&tmp);
    if (r < 0) {
        goto _end;
    };
#else
    nai_errno = ENOTSUP;
    r = -1;
#endif

_end:
    return r;
};


#endif


#if (NAI_HAVE_SIGNAL_H)


nai_sighandle_f nai_signal(nai_int_t signum, nai_sighandle_f handle)
{
    nai_sighandle_f r;

#if (NAI_HAVE_SIGACTION)

    nai_int_t n;
    struct sigaction sa, info;


    nai_memset(&sa, 0, sizeof(sa));
    sigfillset(&sa.sa_mask);
#if defined(SA_RESTART)
    sa.sa_flags = SA_RESTART;
#else
    sa.sa_flags = 0;
#endif
    sa.sa_handler = handle;


    n = sigaction(signum, &sa, &info);
    if (n < 0) {
        r = SIG_ERR;
    } else {
        r = info.sa_handler;
    };

#else

#if defined(_WIN32)
    if (signum == SIGCHLD) {
        nai_sigchld = handle;
        r = 0;
    } else
#endif

    r = signal(signum, handle);

#endif

    return r;
};


#else


nai_sighandle_f nai_signal(nai_int_t signum, nai_sighandle_f handle)
{
    nai_sighandle_f r;


    r = (nai_sighandle_f)-1;
    nai_errno = ENOTSUP;

    return r;
};


#endif


void nai_proc_term()
{
#if defined(_WIN32)
    nai_proc_map_term();
#endif

    return;
};


