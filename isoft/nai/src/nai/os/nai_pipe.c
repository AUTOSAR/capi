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
/// @file       nai_pipe.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_pipe.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


#if defined(_WIN32)


#include "nai/os/nai_thread.h"
#include "win/nai_windows.h"
#include <windef.h>
#include <winbase.h>


static nai_atomic32_t pipe_id = 0;


nai_int_t nai_pipe(nai_fd_t fds[2], nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    DWORD nPipeMode;
    DWORD nOpenMode;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };
    wchar_t name[128];


    if (flags & NAI_O_NOCLOEXEC) {
        sa.bInheritHandle = TRUE;
    };
    if (flags & NAI_O_ASYNCIO) {
        swprintf(name, nai_countof(name), L"\\\\.\\pipe\\nio-pipe-%u.%lu", 
            GetCurrentProcessId(), nai_atomic32_inc(&pipe_id));

        nPipeMode = 0;
        nOpenMode = PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED;
        fds[0] = CreateNamedPipeW(name, 
            nOpenMode, nPipeMode, 1, 0, 0, 1, &sa);
        if (fds[0] == NAI_FD_INVALID) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

        nOpenMode &= ~PIPE_ACCESS_INBOUND;
        fds[1] = CreateFileW(name, 
            GENERIC_WRITE, 0, &sa, OPEN_EXISTING, nOpenMode, NULL);
        if (fds[1] == NAI_FD_INVALID) {
            ec = nai_errno_from_oserr(GetLastError());
            CloseHandle(fds[0]);
            fds[0] = NAI_FD_INVALID;
            nai_errno = ec;
            r = -1;
            goto _end;
        };

    } else {
        if (!CreatePipe(fds+0, fds+1, &sa, 0)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
    };

    if (flags & NAI_O_NONBLOCK) {
        for (n = 0; n < 2; n ++) {
            r = nai_file_set_blocking(fds[n], 1);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            }
        };
    };

    r = 0;

_end:
    return r;

_fail:
    nai_file_close(fds[0]);
    nai_file_close(fds[1]);
    nai_errno = ec;
    goto _end;
};


#else


#if (NAI_HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if (NAI_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif


nai_int_t nai_pipe(nai_fd_t fds[2], nai_int_t flags)
{
    nai_int_t r;
    nai_int_t nc;

#if !(NAI_HAVE_PIPE2)
    nai_int_t ec;
    nai_int_t n, f;
#endif


    if (flags & NAI_O_ASYNCIO) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    nc = flags & NAI_O_NOCLOEXEC;
#if defined(O_CLOEXEC)
    if (nc) {
        flags &= ~(O_CLOEXEC|NAI_O_NOCLOEXEC);
    } else {
        flags |= O_CLOEXEC;
    };
#else
    flags &= ~NAI_O_NOCLOEXEC;
#endif


#if (NAI_HAVE_PIPE2)

    r = pipe2(fds, flags);
    if (r < 0) {
        goto _end;
    };

#else
    r = pipe(fds);
    if (r < 0) {
        goto _end;
    };

    if (flags) {
        for (n = 0; n < 2; n ++) {
            r = fcntl(fds[n], F_GETFL);
            if (r == -1) {
                ec = nai_errno;
                goto _fail;
            };

            f = r;
            f |= flags;
            r = fcntl(fds[n], F_SETFL, f);
            if (r == -1) {
                ec = nai_errno;
                goto _fail;
            };
        };
    };
#endif


#if !defined(O_CLOEXEC) || !(NAI_HAVE_PIPE2)
    if (!nc) {
        for (n = 0; n < 2; n ++) {
            r = nai_file_set_cloexec(fds[n], 1);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    };
#endif


_end:
    return r;

#if !(NAI_HAVE_PIPE2)
_fail:
    close(fds[0]);
    close(fds[1]);
    nai_errno = ec;
    goto _end;
#endif
};

#endif

