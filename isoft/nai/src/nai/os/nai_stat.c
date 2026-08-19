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
/// @file       nai_stat.c
/// @brief      
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_stat.h"


#if defined(_WIN32)


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "win/nai_windows.h"
#include <windef.h>
#include <winbase.h>
#include <fileapi.h>
#include <wchar.h>


static struct timespec nai_filetime_to_timespec(FILETIME* ft)
{
    uint64_t usec;
    struct timespec tv;


    usec = ft->dwLowDateTime + ((uint64_t)ft->dwLowDateTime << 32);
    usec += NAI_DELTA_EPOCH_USEC;

    tv.tv_sec = usec / (1000*1000);
    tv.tv_nsec = usec % (1000*1000) * 1000;
    return tv;
};


static nai_fd_t nai_path_open(
    const char* path, nai_int_t read, nai_int_t link)
{
    nai_int_t r;
    nai_fd_t fd;
    nai_wcs_t ws;
    DWORD fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD filecreate = OPEN_EXISTING;
    DWORD fileaccess = read ? (
        GENERIC_READ & ~FILE_READ_DATA) : FILE_WRITE_ATTRIBUTES;
    DWORD fileattri = link ? 
        FILE_FLAG_OPEN_REPARSE_POINT :FILE_ATTRIBUTE_NORMAL;


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        fd = NAI_FD_INVALID;
        goto _end;
    };

    fd = CreateFileW(ws.str, 
        fileaccess, fileshare, 0, filecreate, fileattri, 0);
    if (fd == NAI_FD_INVALID) {
        nai_errno = nai_errno_from_oserr(GetLastError());
    };

_end:
    nai_wcs_release(&ws, fd == NAI_FD_INVALID);
    return fd;
};


nai_int_t nai_stat(const char* path, nai_stat_t* st, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t fd;


    fd = nai_path_open(path, 1, (flags & NAI_STAT_SYMLINK));
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_file_stat(fd, st, flags);

_end:
    if (fd != NAI_FD_INVALID) {
        if (r < 0) {
            ec = nai_errno;
        };
        nai_file_close(fd);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return r;
};


nai_int_t nai_file_stat(nai_fd_t fd, nai_stat_t* st, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t perm;
    BY_HANDLE_FILE_INFORMATION info;


    if (GetFileInformationByHandle(fd, &info) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    st->st_dev = info.dwVolumeSerialNumber;
    st->st_ino = info.nFileIndexLow + ((uint64_t)info.nFileIndexHigh << 32);
    st->st_nlink = info.nNumberOfLinks;
    st->st_rdev = st->st_dev;
    st->st_size = info.nFileSizeLow + ((uint64_t)info.nFileSizeHigh << 32);
    st->st_atim = nai_filetime_to_timespec(&info.ftLastAccessTime);
    st->st_mtim = nai_filetime_to_timespec(&info.ftLastWriteTime);
    st->st_ctim = nai_filetime_to_timespec(&info.ftCreationTime);

    st->st_mode = 0;
    if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        st->st_mode = NAI_S_IFLNK;
    } else if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        st->st_mode = NAI_S_IFDIR;
    } else if (info.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) {
        st->st_mode = NAI_S_IFCHR;
    } else {
        st->st_mode = NAI_S_IFREG;
        if (GetFileType(fd) == FILE_TYPE_PIPE) {
            st->st_mode = NAI_S_IFIFO;
        }
    };
    if (info.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
        perm = 005;
    } else {
        perm = 007;
    };
    st->st_mode |= perm;
    st->st_mode |= perm << 3;
    st->st_mode |= perm << 6;
    st->st_uid = 0;
    st->st_gid = 0;

    r = 0;

_end:
    return r;
};


nai_int_t nai_path_set_mode(const char* path, nai_int_t mode)
{
    nai_int_t r;
    nai_wcs_t ws;


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    r = _wchmod(ws.str, mode);

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_path_set_mtime(const char* path, uint64_t usec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t fd;
    FILETIME mtime;


    fd = nai_path_open(path, 0, 0);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    usec -= NAI_DELTA_EPOCH_USEC;
    mtime.dwLowDateTime = (uint32_t)usec;
    mtime.dwHighDateTime = (uint32_t)(usec >> 32);

    if (SetFileTime(fd, 0, 0, &mtime) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    if (fd != NAI_FD_INVALID) {
        if (r < 0) {
            ec = nai_errno;
        };
        nai_file_close(fd);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return r;
};



#else

#if (NAI_HAVE_UTIMES)
#include <sys/time.h>
#endif
#if (NAI_HAVE_UTIME)
#include <utime.h>
#endif


static nai_int_t nai_stat_from_os(nai_stat_t* st, const struct stat* os)
{
    st->st_dev = os->st_dev;
    st->st_ino = os->st_ino;
    st->st_mode = os->st_mode;
    st->st_uid = os->st_uid;
    st->st_gid = os->st_gid;
    st->st_nlink = os->st_nlink;
    st->st_rdev = os->st_rdev;
    st->st_size = os->st_size;
    st->st_blksize = os->st_blksize;
    st->st_blocks = os->st_blocks;
#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE) ||    \
    _POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700
#if (__darwin__)
#define nai_stat_time(x)    x##espec
#else
#define nai_stat_time(x)    x
#endif
    st->st_atim = os->nai_stat_time(st_atim);
    st->st_mtim = os->nai_stat_time(st_mtim);
    st->st_ctim = os->nai_stat_time(st_ctim);
#else
    st->st_atim.tv_sec = os->st_atime;
    st->st_mtim.tv_sec = os->st_mtime;
    st->st_ctim.tv_sec = os->st_ctime;
    st->st_atim.tv_nsec = 0;
    st->st_mtim.tv_nsec = 0;
    st->st_ctim.tv_nsec = 0;
#endif
    return 0;
};


nai_int_t nai_stat(const char* path, nai_stat_t* st, nai_int_t flags)
{
    nai_int_t r;
    struct stat s;


    if (!(flags & NAI_STAT_SYMLINK)) {
        r = stat(path, &s);
    } else {
        r = lstat(path, &s);
    };
    if (r >= 0) {
        nai_stat_from_os(st, &s);
    };

    return r;
};


nai_int_t nai_file_stat(nai_fd_t fd, nai_stat_t* st, nai_int_t flags)
{
    nai_int_t r;
    struct stat s;


    (void)flags;

    r = fstat(fd, &s);
    if (r >= 0) {
        nai_stat_from_os(st, &s);
    };

    return r;
};


#if (NAI_HAVE_FSTATAT)


nai_int_t nai_stat_at(nai_fd_t fd, 
    const char* path, nai_stat_t* st, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t f;
    struct stat s;


    f = 0;
#if defined(AT_EMPTY_PATH)
    if (path[0] == 0) {
        f |= AT_EMPTY_PATH;
    };
#endif
    if (flags & NAI_STAT_SYMLINK) {
        f |= AT_SYMLINK_NOFOLLOW;
    };

    if (fd == NAI_FD_INVALID) {
        fd = AT_FDCWD;
    };

    r = fstatat(fd, path, &s, f);
    if (r >= 0) {
        nai_stat_from_os(st, &s);
    };

    return r;
};


#endif


nai_int_t nai_path_set_mode(const char* path, nai_int_t mode)
{
    return chmod(path, mode);
};


nai_int_t nai_path_set_mtime(const char* path, uint64_t usec)
{
    nai_int_t r;
    nai_stat_t st;


    r = nai_stat(path, &st, 0);
    if (r < 0) {
        goto _end;
    };

#if (NAI_HAVE_UTIMES)
    struct timeval tv[2];

    tv[0].tv_sec = st.st_atim.tv_sec;
    tv[0].tv_usec = st.st_atim.tv_nsec / 1000;
    tv[1].tv_sec = usec / (1000 * 1000);
    tv[1].tv_usec = usec % (1000 * 1000);

    r = utimes(path, tv);

#elif (NAI_HAVE_UTIME)

    struct utimbuf tv;

    tv.atime = (time_t)(st.st_atime);
    tv.modtime = (time_t)(msec / (1000 * 1000));

    r = utime(path, &tv);

#else

#error "nai_path_set_mtime is not implemented!"

    (void)usec;

#endif

_end:
    return r;
};


#endif

