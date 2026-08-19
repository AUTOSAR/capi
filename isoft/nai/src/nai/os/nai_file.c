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
/// @file       nai_file.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"
#include <stdarg.h>


#if defined(_WIN32)


#include "nai/runtime/nai_util.h"
#include "win/nai_windows.h"
#include <windef.h>
#include <winbase.h>
#include <winioctl.h>
#include <fileapi.h>
#include <ioapiset.h>
#include <handleapi.h>
#include <processthreadsapi.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>


nai_io_feature_t nai_iofeat = {
    .directio = 1, 
    .readv = 0, 
    .readv_sim = 1, 
    .writev = 0, 
    .writev_sim = 1, 
    .preadv = 0, 
    .preadv_sim = 1, 
    .pwritev = 0, 
    .pwritev_sim = 1, 
    .recvv = 1, 
    .recvv_sim = 1, 
    .recvmsg = 1, 
    .recvmsg_sim = 1, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 1, 
    .sendv = 1, 
    .sendv_sim = 1, 
    .sendmsg = 1, 
    .sendmsg_sim = 1, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 1, 
    .sendfile = 1, 
    .sendfile_sim = 1, 
    .sendfilev = 1, 
    .sendfilev_sim = 0
};


nai_fd_t nai_file_dup(nai_fd_t f, nai_int_t flags)
{
    nai_fd_t d;


    if (DuplicateHandle(
        GetCurrentProcess(), f, 
        GetCurrentProcess(), &d, 
        0, !!(flags&NAI_O_NOCLOEXEC), DUPLICATE_SAME_ACCESS) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        d = NAI_FD_INVALID;
    };

    return d;
};


nai_fd_t nai_get_stdio(nai_int_t n)
{
    nai_fd_t fd;


    if (n < 0 || n > 2) {
        nai_errno = EINVAL;
        fd = NAI_FD_INVALID;
        goto _end;
    };

    fd = (nai_fd_t)_get_osfhandle(n);

_end:
    return fd;
};


nai_int_t nai_set_stdio(nai_int_t n, nai_fd_t fd)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t fc;
    FILE* fp;
    nai_fd_t fn;


    switch(n) {
    case 0:
        fp = stdin;
        break;
    case 1:
        fp = stdout;
        break;
    case 2:
        fp = stderr;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    fn = nai_file_dup(fd, NAI_O_NOCLOEXEC);
    if (fn == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    fflush(fp);
    setvbuf(fp, NULL, _IONBF, 0);
    if (n != 0) {
        if (!_isatty(n)) {
            _commit(n /* stderr */);
        }
    };

    if (n == 0) {
        fc = _open_osfhandle((INT_PTR)fn, _O_RDONLY | _O_BINARY);
    } else {
        fc = _open_osfhandle((INT_PTR)fn, _O_WRONLY | _O_BINARY);
    };
    if (fc < 0) {
        ec = nai_errno;
        nai_file_close(fn);
        nai_errno = ec;
        r = -1;
        goto _end;
    };
    _dup2(fc, n); /* */
    _close(fc);
    _setmode(n, _O_BINARY);

    r = 0;

_end:
    return r;
};


nai_fd_t nai_file_open(const char* path, nai_int_t flags, ...)
{
    nai_int_t mode;
    nai_int_t create;
    va_list va;
    nai_fd_t fd = NAI_FD_INVALID;
    nai_wcs_t ws = { .buf = 0 };
    uint32_t fileaccess;
    uint32_t filecreate;
    uint32_t fileattrib;
    uint32_t fileshare;
    uint32_t fileflags;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };


    if (flags & NAI_O_NONBLOCK) {
        nai_errno = ENOTSUP;
        goto _end;
    };
    if (flags & NAI_O_TMPFILE) {
        nai_errno = ENOTSUP;
        goto _end;
    };

    switch (flags & (NAI_O_RDONLY | NAI_O_WRONLY | NAI_O_RDWR) ) {
    case NAI_O_RDONLY:          /* read access */
        fileaccess = GENERIC_READ;
        break;
    case NAI_O_WRONLY:          /* write access */
        fileaccess = GENERIC_WRITE;
        break;
    case NAI_O_RDWR:            /* read and write access */
        fileaccess = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        nai_errno = EINVAL;
        goto _end;
    };

    if (flags & NAI_O_APPEND) {
        fileaccess &= ~GENERIC_WRITE;
        fileaccess |= FILE_APPEND_DATA | 
            FILE_WRITE_ATTRIBUTES | STANDARD_RIGHTS_WRITE;
    };
    if (flags & NAI_O_EXECUTE) {
        fileaccess |= GENERIC_EXECUTE;
    };

    switch (flags & (NAI_O_CREAT | NAI_O_EXCL | NAI_O_TRUNC) ) {
    case 0:
    case NAI_O_EXCL:            /* ignore EXCL w/o CREAT */
        filecreate = OPEN_EXISTING;
        break;
    case NAI_O_CREAT:
        filecreate = OPEN_ALWAYS;
        break;
    case NAI_O_CREAT | NAI_O_EXCL:
    case NAI_O_CREAT | NAI_O_TRUNC | NAI_O_EXCL:
        filecreate = CREATE_NEW;
        break;
    case NAI_O_TRUNC:
    case NAI_O_TRUNC | NAI_O_EXCL:   /* ignore EXCL w/o CREAT */
        filecreate = TRUNCATE_EXISTING;
        break;
    case NAI_O_CREAT | NAI_O_TRUNC:
        filecreate = CREATE_ALWAYS;
        break;
    default:
        nai_errno = EINVAL;
        goto _end;
    };

    fileflags = 0;
    if (flags & NAI_O_DIRECT) {
        fileflags |= FILE_FLAG_NO_BUFFERING;
    };
    if (flags & NAI_O_ASYNCIO) {
        fileflags |= FILE_FLAG_OVERLAPPED;
    };
    if (flags & NAI_O_LARGEFILE) {
        ;
    };
    if (flags & NAI_O_DIRECTORY) {
        fileflags |= FILE_FLAG_BACKUP_SEMANTICS;
    };
    if (flags & NAI_O_SEARCH) {
        fileflags |= FILE_FLAG_BACKUP_SEMANTICS;
    };
    if (flags & NAI_O_NOFOLLOW) {
        if (!(flags & NAI_O_PATH)) {
            nai_errno = ENOTSUP;
            goto _end;
        };
        fileflags |= FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT;
    };

    if (flags & NAI_O_NOCLOEXEC) {
        sa.bInheritHandle = TRUE;
    }

    fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    fileattrib = FILE_ATTRIBUTE_NORMAL; /* default */

    /* to wchar_t* */
    if (nai_wcs_from_path(&ws, path) < 0) {
        goto _end;
    };

    /* get mode */
    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create) {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);

        /* setup security attributes */
    };

    /* open file */
    fd = CreateFileW(ws.str, 
        fileaccess, fileshare, &sa, 
        filecreate, fileattrib | fileflags, NULL);
    if (fd == NAI_FD_INVALID) {
        nai_errno = nai_errno_from_oserr(GetLastError());
    };

_end:
    nai_wcs_release(&ws, fd == NAI_FD_INVALID);
    return fd;
};


intptr_t nai_file_read(nai_fd_t fd, void* lp, size_t len)
{
    DWORD n;
    DWORD ec;


    if (ReadFile(fd, lp, (DWORD)len, &n, 0) == 0) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_HANDLE_EOF:
        case ERROR_BROKEN_PIPE:
            break;
        case ERROR_NO_DATA:
            nai_errno = NAI_EAGAIN;
            n = -1;
            goto _end;
        default:
            nai_errno = nai_errno_from_oserr(GetLastError());
            n = -1;
            goto _end;
        }

        n = 0;
    };

_end:
    return n;
};


intptr_t nai_file_readv(nai_fd_t fd, nai_bufvec_t* v, size_t count)
{
    intptr_t r;
    intptr_t n;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        r = nai_file_read(fd, v[i].buf, v[i].len);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };
        if (r == 0) {
            break;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };
    };

_end:
    return n;
};


intptr_t nai_file_pread(nai_fd_t fd, void* lp, size_t len, nai_off64_t offset)
{
    DWORD n;
    DWORD ec;
    OVERLAPPED  op;
    OVERLAPPED* pop = 0;


    if (offset != -1) {
        op.Internal = 0;
        op.InternalHigh = 0;
        op.Offset = (uint32_t)offset;
        op.OffsetHigh = (uint32_t)(offset >> 32);
        op.hEvent = NULL;
        pop = &op;
    };
    if (ReadFile(fd, lp, (DWORD)len, &n, pop) == 0) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_HANDLE_EOF:
        case ERROR_BROKEN_PIPE:
            break;
        case ERROR_NO_DATA:
            nai_errno = NAI_EAGAIN;
            n = -1;
            goto _end;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            n = -1;
            goto _end;
        };

        n = 0;
    };

_end:
    return n;
};


intptr_t nai_file_preadv(
    nai_fd_t fd, nai_bufvec_t* v, size_t count, nai_off64_t offset)
{
    intptr_t r;
    intptr_t n;
    nai_int_t i;
    nai_int_t ec;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_readv(fd, v, count);
        goto _end;
    };

    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        r = nai_file_pread(fd, v[i].buf, v[i].len, offset);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };
        if (r == 0) {
            break;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };

        offset += r;
    };

_end:
    return n;
};


intptr_t nai_file_write(nai_fd_t fd, const void* lp, size_t len)
{
    DWORD n;


    if (WriteFile(fd, lp, (DWORD)len, &n, 0) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        n = -1;
        goto _end;
    };
#if 0
    if (n == 0 && len) { /* when pipe blocked */
        nai_errno = NAI_EAGAIN;
        return -1;
    };
#endif

_end:
    return n;
};


intptr_t nai_file_writev(nai_fd_t fd, const nai_bufvec_t* v, size_t count)
{
    intptr_t r;
    intptr_t n;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        r = nai_file_write(fd, v[i].buf, v[i].len);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };
    };

_end:
    return n;
};


intptr_t nai_file_pwrite(
    nai_fd_t fd, const void* lp, size_t len, nai_off64_t offset)
{
    DWORD n;
    OVERLAPPED  op;
    OVERLAPPED* pop = 0;


    if (offset != -1) {
        op.Internal = 0;
        op.InternalHigh = 0;
        op.Offset = (uint32_t)offset;
        op.OffsetHigh = (uint32_t)(offset >> 32);
        op.hEvent = NULL;
        pop = &op;
    };
    if (WriteFile(fd, lp, (DWORD)len, &n, pop) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        n = -1;
        goto _end;
    };
#if 0
    if (n == 0 && len) { /* when pipe blocked */
        nai_errno = NAI_EAGAIN;
        return -1;
    };
#endif

_end:
    return n;
};


intptr_t nai_file_pwritev(
    nai_fd_t fd, const nai_bufvec_t* v, size_t count, nai_off64_t offset)
{
    intptr_t r;
    intptr_t n;
    nai_int_t i;
    nai_int_t ec;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_writev(fd, v, count);
        goto _end;
    };

    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        r = nai_file_pwrite(fd, v[i].buf, v[i].len, offset);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };

        offset += r;
    };

_end:
    return n;
};


nai_off64_t nai_file_tell(nai_fd_t fd)
{
    return nai_file_seek(fd, 0, 1);
};


nai_off64_t nai_file_seek(nai_fd_t fd, nai_off64_t offset, nai_int_t mode)
{
    nai_off64_t r;
    DWORD high = (uint32_t)(offset >> 32);
    DWORD low;


    low = SetFilePointer(fd, (uint32_t)offset, &high, mode);
    if (low == INVALID_SET_FILE_POINTER) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = (uint64_t)low | ((int64_t)high << 32);

_end:
    return r;
};


nai_off64_t nai_file_get_size(nai_fd_t fd)
{
    nai_off64_t r;
    DWORD high;
    DWORD low;


    low = GetFileSize(fd, &high);
    if (low == INVALID_FILE_SIZE) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = (uint64_t)low | ((int64_t)high << 32);

_end:
    return r;
};


nai_off64_t nai_file_set_size(nai_fd_t fd, nai_off64_t newsize)
{
    nai_off64_t r;
    nai_off64_t pos;
    nai_int_t ec;


    pos = nai_file_tell(fd);
    if (pos == -1) {
        r = -1;
        goto _end;
    };

    if (nai_file_seek(fd, newsize, 0) == -1) {
        goto _fail;
    };

    if (!SetEndOfFile(fd)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        goto _fail;
    };

    if (pos > newsize) {
        pos = newsize;
    };

    nai_file_seek(fd, pos, 0);
    r = newsize;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_file_seek(fd, pos, 0);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_file_set_sparse(nai_fd_t fd)
{
    nai_int_t r;
    DWORD ret;
    FILE_SET_SPARSE_BUFFER sparse = { TRUE };


    if (!DeviceIoControl(fd, 
        FSCTL_SET_SPARSE, &sparse, sizeof(sparse), 0, 0, &ret, 0)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_file_set_cloexec(nai_fd_t fd, nai_int_t on)
{
    nai_int_t r;


    if (!SetHandleInformation(fd, 
        HANDLE_FLAG_INHERIT, on ? 0 : HANDLE_FLAG_INHERIT)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_file_set_blocking(nai_fd_t fd, nai_int_t on)
{
    nai_int_t r;
    DWORD mode;

    /* only for pipes !!! */

    if (!GetNamedPipeHandleState(fd, &mode, NULL, NULL, NULL, NULL, 0)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    if (on) {
        if ((mode & PIPE_NOWAIT) == 0) {
            r = 0;
            goto _end;
        };
    } else {
        if (mode & PIPE_NOWAIT) {
            r = 0;
            goto _end;
        };
    };

    mode &= (PIPE_READMODE_BYTE | PIPE_READMODE_MESSAGE);
    mode |= on ? PIPE_WAIT : PIPE_NOWAIT;
    if (!SetNamedPipeHandleState(fd, &mode, NULL, NULL)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_file_poll(nai_fd_t fd, nai_int_t events, uint32_t msec)
{
    nai_int_t r;
    DWORD stat;


    if (!events) {
        r = 0;
        goto _end;
    };

    stat = WaitForSingleObject(fd, msec);
    switch (stat) {
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

_end:
    return r;
};


nai_int_t nai_file_lock(nai_fd_t fd, nai_int_t mode)
{
#ifdef _WIN32_WCE
    /* The File locking is unsuported on WCE */
    nai_errno = EINVAL;
    return -1;
#else

    nai_int_t r;
    DWORD ec;
    DWORD bytes;
    DWORD len = 0xffffffff;
    DWORD flags; 
    OVERLAPPED ol;


    flags = (mode & NAI_FLOCK_TRY) ? LOCKFILE_FAIL_IMMEDIATELY : 0;
    flags |= (mode & NAI_FLOCK_READ) ? 0 : LOCKFILE_EXCLUSIVE_LOCK;

    nai_memset(&ol, 0, sizeof(ol));
    ol.hEvent = nai_win_ioevent();
    if (ol.hEvent == 0) {
        r = -1;
        goto _end;
    };

    if (!LockFileEx(fd, flags, 0, len, len, &ol)) {
        ec = GetLastError();
        if (ec != ERROR_IO_PENDING) {
            if (ec == ERROR_LOCK_VIOLATION) {
                nai_errno = EAGAIN;
            } else {
                nai_errno = nai_errno_from_oserr(ec);
            };
            r = -1;
            goto _end;
        };
    };
    if (!GetOverlappedResult(fd, &ol, &bytes, 1)) {
        ec = GetLastError();
        if (ec == ERROR_LOCK_VIOLATION) {
            nai_errno = EAGAIN;
        } else {
            nai_errno = nai_errno_from_oserr(ec);
        };
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
#endif
};


nai_int_t nai_file_unlock(nai_fd_t fd)
{
#ifdef _WIN32_WCE
    /* The File locking is unsuported on WCE */
    nai_errno = EINVAL;
    return -1;
#else

    nai_int_t r;
    DWORD ec;
    DWORD bytes;
    DWORD len = 0xffffffff;
    OVERLAPPED ol;

    nai_memset(&ol, 0, sizeof(ol));
    ol.hEvent = nai_win_ioevent();
    if (ol.hEvent == 0) {
        r = -1;
        goto _end;
    };

    if (!UnlockFileEx(fd, 0, len, len, &ol)) {
        ec = GetLastError();
        if (ec != ERROR_IO_PENDING) {
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };
    if (!GetOverlappedResult(fd, &ol, &bytes, 1)) {
        ec = GetLastError();
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
#endif
};


nai_int_t nai_file_flush(nai_fd_t fd)
{
    nai_int_t r;


    if (!FlushFileBuffers(fd)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_file_close(nai_fd_t fd)
{
    nai_int_t r;


    if (!CloseHandle(fd)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_file_link(const char* path, const char* to)
{
    nai_int_t r;
    nai_wcs_t ws;


    r = nai_wcs_from_2str(&ws, path, to);
    if (r < 0) {
        goto _end;
    };

    if (CreateHardLinkW(ws.s[1], ws.s[0], 0) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_file_unlink(const char* path)
{
    nai_int_t r;
    nai_wcs_t ws;


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    if (DeleteFileW(ws.str) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_path_rename(const char* from, const char* to)
{
    nai_int_t r;
    nai_wcs_t ws;


    r = nai_wcs_from_2str(&ws, from, to);
    if (r < 0) {
        goto _end;
    };

    if (MoveFileW(ws.s[0], ws.s[1]) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_path_symlink(const char* path, const char* to)
{
    nai_int_t r;


    (void)path;
    (void)to;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


intptr_t nai_path_readlink(const char* path, char* buf, size_t size)
{
    intptr_t r;


    (void)path;
    (void)buf;
    (void)size;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};



#else


#include <stdio.h>

#if (NAI_HAVE_SYS_FILE_H)
#include <sys/file.h>
#endif
#if (NAI_HAVE_SYS_UIO_H)
#include <sys/uio.h>
#endif
#if (NAI_HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if (NAI_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if (NAI_HAVE_POLL)
#include <poll.h>
#endif
#if (NAI_HAVE_SELECT)
#include <sys/select.h>
#endif

#include "nai/os/nai_aio.h"
#include "nai/os/nai_tlocal.h"


nai_io_feature_t nai_iofeat = {
#if defined(O_DIRECT)
    .directio = 1, 
#else
    .directio = 0, 
#endif

#if (NAI_HAVE_READV)
    .readv = 1, 
#else
    .readv = 0, 
#endif
    .readv_sim = 1, 

#if (NAI_HAVE_WRITEV)
    .writev = 1, 
#else
    .writev = 0, 
#endif
    .writev_sim = 1, 


#if (NAI_HAVE_PREADV)
    .preadv = 1, 
#else
    .preadv = 0, 
#endif
    .preadv_sim = 1, 

#if (NAI_HAVE_PWRITEV)
    .pwritev = 1, 
#else
    .pwritev = 0, 
#endif
    .pwritev_sim = 1, 


#if (NAI_HAVE_READV) || (NAI_HAVE_RECVMSG)
    .recvv = 1, 
    .recvmsg = 1, 
    .recvmsg_sim = 1, 
#else
    .recvv = 0, 
    .recvmsg = 0, 
    .recvmsg_sim = 0, 
#endif
    .recvv_sim = 1, 

#if (NAI_HAVE_RECVMMSG)
    .recvmmsg = 1, 
    .recvmmsg_sim = 1, 
#else
    .recvmmsg = 0, 
    .recvmmsg_sim = 1, 
#endif

#if (NAI_HAVE_WRITEV) || (NAI_HAVE_SENDMSG)
    .sendv = 1, 
    .sendmsg = 1, 
    .sendmsg_sim = 1, 
#else
    .sendv = 0, 
    .sendmsg = 0, 
    .sendmsg_sim = 0, 
#endif
    .sendv_sim = 1, 

#if (NAI_HAVE_SENDMMSG)
    .sendmmsg = 1, 
    .sendmmsg_sim = 1, 
#else
    .sendmmsg = 0, 
    .sendmmsg_sim = 1, 
#endif


#if (NAI_HAVE_SENDFILE_FREEBSD) ||   \
    (NAI_HAVE_SENDFILE_LINUX) ||     \
    (NAI_HAVE_SENDFILEV) 

    .sendfile = 1, 
    .sendfile_sim = 1, 
#if (NAI_HAVE_SENDFILEV)
    .sendfilev = 1, 
#else
    .sendfilev = 0, 
#endif
    .sendfilev_sim = 1, 

#else
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0, 
#endif
};


nai_fd_t nai_file_dup(nai_fd_t f, nai_int_t flags)
{
    nai_int_t ec;
    nai_fd_t r;


    r = dup(f);
    if (r < 0) {
        goto _end;
    };

    if (!(flags & NAI_O_NOCLOEXEC)) {
        if (nai_file_set_cloexec(r, 1) < 0) {
            ec = nai_errno;
            nai_file_close(r);
            nai_errno = ec;
            r = -1;
        };
    };

_end:
    return r;
};


nai_fd_t nai_get_stdio(nai_int_t n)
{
    nai_fd_t r;


    if (n < 0 || n > 2) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = n;

_end:
    return r;
};


nai_int_t nai_set_stdio(nai_int_t n, nai_fd_t fd)
{
    nai_int_t r;


    if (n < 0 || n > 2) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    if (fd == n) {
        r = nai_file_set_cloexec(n, 0);
    } else {
        r = dup2(fd, n);
    };

_end:
    return r;
};


nai_fd_t nai_file_open(const char* path, nai_int_t flags, ...)
{
    nai_int_t ec;
    nai_int_t nc;
    nai_int_t mode;
    nai_int_t create;
    va_list va;
    nai_fd_t r;


    if (flags & NAI_O_ASYNCIO && !nai_aiofeat.support) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

#if !defined(O_DIRECT)
    if (flags & NAI_O_DIRECT) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

#if !defined(O_TMPFILE)
    if (flags & NAI_O_TMPFILE) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

#if !defined(O_NOFOLLOW)
    if (flags & NAI_O_NOFOLLOW) {
#if (NAI_HAVE_SYMLINK)
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
#else
        flags &= ~NAI_O_NOFOLLOW;
#endif
    };
#endif

#if defined(O_PATH) && !defined(O_SEARCH) && !defined(O_EXEC)
    if (flags & NAI_O_SEARCH && !(flags & NAI_O_NOFOLLOW)) {
        flags |= NAI_O_PATH;
    };
#endif

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

    flags &= ~0xff000000;


    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create == 0) {
        do {
            r = open(path, flags);
        } while (r == -1 && (ec = nai_thread_io_errno()) == EINTR);
    } else {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);

        do {
            r = open(path, flags, mode);
        } while (r == -1 && (ec = nai_thread_io_errno()) == EINTR);
    };
#if defined(O_NOFOLLOW) && !(__linux__)
    if (r < 0 && ec == EMLINK && (flags & NAI_O_NOFOLLOW)) {
        nai_errno = ELOOP;
    };
#endif

#if !defined(O_CLOEXEC)
    if (r >= 0 && !nc) {
        if (nai_file_set_cloexec(r, 1) < 0) {
            ec = nai_errno;
            nai_file_close(r);
            nai_errno = ec;
            r = -1;
        };
    };
#endif

_end:
    return r;
};


#if (NAI_HAVE_OPENAT)


nai_fd_t nai_file_openat(
    nai_fd_t at, const char* path, nai_int_t flags, ...)
{
    nai_int_t ec;
    nai_int_t nc;
    nai_int_t mode;
    nai_int_t create;
    va_list va;
    nai_fd_t r;


    if (flags & NAI_O_ASYNCIO && !nai_aiofeat.support) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

#if !defined(O_DIRECT)
    if (flags & NAI_O_DIRECT) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

#if !defined(O_TMPFILE)
    if (flags & NAI_O_TMPFILE) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
#endif

#if !defined(O_NOFOLLOW)
    if (flags & NAI_O_NOFOLLOW) {
#if (NAI_HAVE_SYMLINK)
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
#else
        flags &= ~NAI_O_NOFOLLOW;
#endif
    };
#endif

#if defined(O_PATH) && !defined(O_SEARCH) && !defined(O_EXEC)
    if (flags & NAI_O_SEARCH && !(flags & NAI_O_NOFOLLOW)) {
        flags |= NAI_O_PATH;
    };
#endif

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

    flags &= ~0xff000000;


    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create == 0) {
        do {
            r = openat(at, path, flags);
        } while (r == -1 && (ec = nai_thread_io_errno()) == EINTR);
    } else {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);

        do {
            r = openat(at, path, flags, mode);
        } while (r == -1 && (ec = nai_thread_io_errno()) == EINTR);
    };
#if defined(O_NOFOLLOW) && !(__linux__)
    if (r < 0 && ec == EMLINK && (flags & NAI_O_NOFOLLOW)) {
        nai_errno = ELOOP;
    };
#endif

#if !defined(O_CLOEXEC)
    if (r >= 0 && !nc) {
        if (nai_file_set_cloexec(r, 1) < 0) {
            ec = nai_errno;
            nai_file_close(r);
            nai_errno = ec;
            r = -1;
        };
    };
#endif

_end:
    return r;
};


#endif


intptr_t nai_file_read(nai_fd_t fd, void* lp, size_t len)
{
    intptr_t n;


    do {
        n = read(fd, lp, len);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

    return n;
};


intptr_t nai_file_readv(nai_fd_t fd, nai_bufvec_t* v, size_t count)
{
    intptr_t n;

#if (NAI_HAVE_READV)

    do {
        n = readv(fd, (struct iovec*)v, count);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#else

    intptr_t r;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        r = nai_file_read(fd, v[i].buf, v[i].len);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };
        if (r == 0) {
            break;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };
    };

_end:

#endif

    return n;
};


intptr_t nai_file_pread(
    nai_fd_t fd, void* lp, size_t len, nai_off64_t offset)
{
    intptr_t n;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_read(fd, lp, len);
        goto _end;
    };

#if (NAI_HAVE_PREAD)

    do {
        n = pread(fd, lp, len, offset);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#else
#error "nai_file_pread is not implemented, require pread!"
#endif

_end:
    return n;
};


intptr_t nai_file_preadv(
    nai_fd_t fd, nai_bufvec_t* v, size_t count, nai_off64_t offset)
{
    intptr_t n;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_readv(fd, v, count);
        goto _end;
    };

#if (NAI_HAVE_PREADV)

    do {
        n = preadv(fd, (struct iovec*)v, count, offset);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#elif (NAI_HAVE_PREAD)

    intptr_t r;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    for (i = 0; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        do {
            r = pread(fd, v[i].buf, v[i].len, offset);
        } while (r == -1 && nai_thread_io_continue(&nai_errno));

        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };
        if (r == 0) {
            break;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };

        offset += r;
    };

#else
#error "nai_file_preadv is not implemented, require pread or preadv!"
#endif

_end:
    return n;
};


intptr_t nai_file_write(nai_fd_t fd, const void* lp, size_t len)
{
    intptr_t n;


    do {
        n = write(fd, lp, len);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

    return n;
};


intptr_t nai_file_writev(nai_fd_t fd, const nai_bufvec_t* v, size_t count)
{
    intptr_t n;

#if (NAI_HAVE_WRITEV)

    do {
        n = writev(fd, (struct iovec*)v, count);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#else

    intptr_t r;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    i = 0;
    for ( ; i < (intptr_t)count; i ++) {
        r = nai_file_write(fd, v[i].buf, v[i].len);
        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };
    };

_end:

#endif

    return n;
};


intptr_t nai_file_pwrite(
    nai_fd_t fd, const void* lp, size_t len, nai_off64_t offset)
{
    intptr_t n;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_write(fd, lp, len);
        goto _end;
    };

#if (NAI_HAVE_PWRITE)

    do {
        n = pwrite(fd, lp, len, offset);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#else
#error "nai_file_pwrite is not implemented, require pwrite!"
#endif

_end:
    return n;
};


intptr_t nai_file_pwritev(
    nai_fd_t fd, const nai_bufvec_t* v, size_t count, nai_off64_t offset)
{
    intptr_t n;


    if (offset == (nai_off64_t)-1) {
        n = nai_file_writev(fd, v, count);
        goto _end;
    };

#if (NAI_HAVE_PWRITEV)

    do {
        n = pwritev(fd, (struct iovec*)v, count, offset);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

#elif (NAI_HAVE_PWRITE)

    intptr_t r;
    nai_int_t i;
    nai_int_t ec;


    n = 0;
    for (i = 0; i < (intptr_t)count; i ++) {
        if (v[i].len == 0) {
            continue;
        };

        do {
            r = pwrite(fd, v[i].buf, v[i].len, offset);
        } while (r == -1 && nai_thread_io_continue(&nai_errno));

        if (r < 0) {
            if (n > 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == ECANCELED) {
                    break;
                };
            };

            n = -1;
            goto _end;
        };

        n += r;
        if (r < (intptr_t)v[i].len) {
            break;
        };

        offset += r;
    };

#else
#error "nai_file_pwritev is not implemented, require pwrite or pwritev!"
#endif

_end:
    return n;
};


nai_off64_t nai_file_tell(nai_fd_t fd)
{
    nai_off64_t n;


    do {
        n = nai_file_seek(fd, 0, 1);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

    return n;
};


nai_off64_t nai_file_seek(nai_fd_t fd, nai_off64_t offset, nai_int_t mode)
{
    nai_off64_t n;


    do {
#if (NAI_HAVE_LSEEK64)
        n = lseek64(fd, offset, mode);
#else
        n = lseek(fd, offset, mode);
#endif
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

    return n;
};


nai_off64_t nai_file_get_size(nai_fd_t fd)
{
    nai_int_t n;
    nai_off64_t r;


#if (NAI_HAVE_FSTAT64)
    struct stat64 st;
    do {
        n = fstat64(fd, &st);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));
#else
    struct stat st;
    do {
        n = fstat(fd, &st);
    } while (n == -1 && nai_thread_io_continue(&nai_errno));
#endif

    if (n < 0) {
        r = -1;
        goto _end;
    };

    r = st.st_size;

_end:
    return r;
};


nai_off64_t nai_file_set_size(nai_fd_t fd, nai_off64_t newsize)
{
    nai_int_t n;
    nai_off64_t r;


    do {
#if (NAI_HAVE_FTRUNCATE64)
        n = ftruncate64(fd, newsize);
#else
        n = ftruncate(fd, newsize);
#endif
    } while (n == -1 && nai_thread_io_continue(&nai_errno));

    if (n < 0) {
        r = -1;
        goto _end;
    };

    r = newsize;

_end:
    return r;
}


nai_int_t nai_file_set_sparse(nai_fd_t fd)
{
    (void)fd;

    return 0;
};


nai_int_t nai_file_set_cloexec(nai_fd_t fd, nai_int_t on)
{
    nai_int_t r;

#if (NAI_HAVE_FCNTL_H)
    nai_int_t v;
    nai_int_t f;


    f = FD_CLOEXEC;

    r = fcntl(fd, F_GETFD);
    if (r == -1) {
        goto _end;
    };

    v = r;
    if (on) {
        if (v & f) {
            r = 0;
            goto _end;
        };
        v |= f;
    } else {
        if ((v & f) == 0) {
            r = 0;
            goto _end;
        };
        v &= ~f;
    };

    r = fcntl(fd, F_SETFD, v);
    if (r == -1) {
        goto _end;
    };

#else
#error "no file control mechanism is available."

    (void)fd;
    (void)on;

#endif

_end:
    return r;
};


nai_int_t nai_file_set_blocking(nai_fd_t fd, nai_int_t on)
{
    nai_int_t r;

#if (NAI_HAVE_FCNTL_H)

    nai_int_t v;
    nai_int_t f;


#if defined(O_NONBLOCK)
    f = O_NONBLOCK;
#elif defined(O_NDELAY)
    f = O_NDELAY;
#elif defined(O_FNDELAY)
    f = O_FNDELAY;
#else
#error "no file blocking flag is available."
#endif

    r = fcntl(fd, F_GETFL);
    if (r == -1) {
        goto _end;
    };

    v = r;
    if (on) {
        if ((v & f) == 0) {
            r = 0;
            goto _end;
        };
        v &= ~f;
    } else {
        if (v & f) {
            r = 0;
            goto _end;
        };
        v |= f;
    };

    r = fcntl(fd, F_SETFL, v);
    if (r == -1) {
        goto _end;
    };

#else
#error "no file blocking mechanism is available."

    (void)fd;
    (void)on;

#endif

_end:
    return r;
};


nai_int_t nai_file_poll(nai_fd_t fd, nai_int_t events, uint32_t msec)
{
    nai_int_t r;

#if (NAI_HAVE_POLL)

    nai_int_t ev = 0;
    struct pollfd set;


    if (events & NAI_POLL_READ) {
        ev |= POLLIN;
    };
    if (events & NAI_POLL_WRITE) {
        ev |= POLLOUT;
    };

    set.fd = fd;
    set.events = ev;

#if !defined(_WIN32)
    do {
#endif
        r = poll(&set, 1, msec);
#if !defined(_WIN32)
    } while (r == -1 && nai_thread_io_continue(&nai_errno));
#endif

    if (r == 0) {
        nai_errno = ETIMEDOUT;
        r = -1;
    };

#elif (NAI_HAVE_SELECT)

    struct timeval tv, *tp;
    fd_set* read = 0;
    fd_set* write = 0;
    fd_set set[2];


    if (fd >= FD_SETSIZE) {
        nai_errno = EBADF;
        r = -1;
        goto _end;
    };
    if (msec == -1) {
        tp = 0;
    } else {
        tv.tv_sec = (long) (msec / 1000);
        tv.tv_usec = (long) ((msec % 1000) * 1000);
        tp = &tv;
    };

    if (events & NAI_POLL_READ) {
        read = &set[0];
        FD_ZERO(read);
        FD_SET(fd, read);
    };
    if (events & NAI_POLL_WRITE) {
        write = &set[1];
        FD_ZERO(write);
        FD_SET(fd, write);
    };

#if !defined(_WIN32)
    do {
#endif
        r = select(fd+1, read, write, 0, tp);
#if !defined(_WIN32)
    } while (r == -1 && nai_thread_io_continue(&nai_errno));
#endif

    if (r == 0) {
        nai_errno = ETIMEDOUT;
        r = -1;
    };

_end:

#else

#error "no file poll mechanism is available."

    (void)fd;
    (void)events;
    (void)msec;

#endif

    return r;
};


nai_int_t nai_file_lock(nai_fd_t fd, nai_int_t mode)
{
    nai_int_t r;

#if (NAI_HAVE_FCNTL_H)

    nai_int_t fc;
    struct flock l = { 0 };


    l.l_whence = SEEK_SET;  /* count l_start from start of file */
    l.l_start = 0;          /* lock from start of file */
    l.l_len = 0;            /* lock to end of file */
    if (mode & NAI_FLOCK_READ) {
        l.l_type = F_RDLCK;
    } else {
        l.l_type = F_WRLCK;
    };

    fc = (mode & NAI_FLOCK_TRY) ? F_SETLK : F_SETLKW;

    /* keep trying if fcntl() gets interrupted (by a signal) */
    do {
        r = fcntl(fd, fc, &l);
    } while (r == -1 && nai_thread_io_continue(&nai_errno));

#elif (NAI_HAVE_SYS_FILE_H)

    nai_int_t lock;


    if (mode & NAI_FLOCK_READ) {
        lock = LOCK_SH;
    } else {
        lock = LOCK_EX;
    };
    if (mode & NAI_FLOCK_TRY) {
        lock |= LOCK_NB;
    };

    /* keep trying if flock() gets interrupted (by a signal) */
    do {
        r = flock(fd, lock);
    } while (r == -1 && nai_thread_io_continue(&nai_errno));

#else

#error "no file locking mechanism is available."

    (void)fd;
    (void)mode;

#endif

    return r;
}


nai_int_t nai_file_unlock(nai_fd_t fd)
{
    nai_int_t r;

#if (NAI_HAVE_FCNTL_H)

    struct flock l = { 0 };


    l.l_whence = SEEK_SET;  /* count l_start from start of file */
    l.l_start = 0;          /* lock from start of file */
    l.l_len = 0;            /* lock to end of file */
    l.l_type = F_UNLCK;

    /* keep trying if fcntl() gets interrupted (by a signal) */
    do {
        r = fcntl(fd, F_SETLKW, &l);
    } while (r == -1 && nai_errno == EINTR);

#elif (NAI_HAVE_SYS_FILE_H)

    /* keep trying if flock() gets interrupted (by a signal) */
    do {
        r = flock(fd, LOCK_UN);
    } while (r == -1 && nai_errno == EINTR);

#else
#error "no file locking mechanism is available."

    (void)fd;
#endif

    return r;
}


nai_int_t nai_file_flush(nai_fd_t fd)
{
    nai_int_t r;


    do {
        r = fsync(fd);
    } while (r == -1 && nai_thread_io_continue(&nai_errno));

    return r;
};


nai_int_t nai_file_close(nai_fd_t fd)
{
    nai_int_t r;


#if 0 /* hp-ux */
    nai_int_t ec;

    for (;;) {
        r = close(fd);
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec == EINTR) {
            continue;
        };
        if (ec != EBADF) {
            r = 0;
        };

        break;
    };
#endif

    /* retrying the close() after a failure return is the wrong thing 
     * to do, expect the EINTR error on hp-ux
     */
    r = close(fd);
    if (r < 0 && nai_errno != EBADF) {
        r = 0;
    };

    return r;
};


nai_int_t nai_file_link(const char* path, const char* to)
{
    nai_int_t r;

    r = link(path, to);
    return r;
};


nai_int_t nai_file_unlink(const char* path)
{
    nai_int_t r;

    r = unlink(path);
    return r;
};


nai_int_t nai_path_rename(const char* from, const char* to)
{
    return rename(from, to);
};


nai_int_t nai_path_symlink(const char* path, const char* to)
{
    nai_int_t r;

    r = symlink(path, to);
    return r;
};


intptr_t nai_path_readlink(const char* path, char* buf, size_t size)
{
    intptr_t r;
    size_t len;
    struct stat st;


    r = lstat(path, &st);
    if (r < 0) {
        goto _end;
    };
    if (!S_ISLNK(st.st_mode)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    len = (nai_int_t)st.st_size;
    if (len <= size && buf) {
        r = readlink(path, buf, size);
        if (r < 0) {
            goto _end;
        };

        len = r;
        if (len + 1 <= size) {
            buf[len] = 0;
        };
    };


    r = (nai_int_t)len;

_end:
    return r;
};



#endif



//////////////////////////////////////////////////////////////////////////////
// deprecated apis


nai_int_t nai_file_cloexec(nai_fd_t fd, nai_int_t on)
{
    return nai_file_set_cloexec(fd, on);
};


nai_int_t nai_file_blocking(nai_fd_t fd, nai_int_t on)
{
    return nai_file_set_blocking(fd, on);
};



