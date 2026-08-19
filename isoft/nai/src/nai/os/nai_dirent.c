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
/// @file       nai_dirent.c
/// @brief      
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_dirent.h"
#include "nai/os/nai_stat.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


#if defined(_WIN32)


#include "win/nai_windows.h"
#include <fileapi.h>
#include <stringapiset.h>


typedef struct nai_filefind_s {
    HANDLE find;
    WIN32_FIND_DATAW data;
    nai_int_t first;
    char name[NAI_PATH_MAX*2];
} nai_filefind_t;


nai_int_t nai_dir_open(nai_dir_t* d, const char* path)
{
    nai_int_t r;
    nai_int_t ec;
    nai_wcs_t ws = { .buf = 0 };
    nai_filefind_t* f;


    r = nai_wcs_from_1str_ext(&ws, path, 4);
    if (r < 0) {
        f = 0;
        goto _end;
    };

    f = nai_malloc(sizeof(nai_filefind_t));
    if (f == 0) {
        r = -1;
        goto _end;
    };

    wcscat(ws.str, L"\\*.*");


    f->find = FindFirstFileW(ws.str, &f->data);
    if (f->find == NAI_FD_INVALID) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    f->first = 1;
    d->dir = f;
    d->name = 0;
    d->type = 0;
    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);

    if (r < 0 && f) {
        ec = nai_errno;
        nai_free(f);
        nai_errno = ec;
    };
    return r;
};


nai_int_t nai_dir_read(nai_dir_t* d)
{
    nai_int_t r;
    nai_int_t ec;
    nai_filefind_t* f;


    f = (nai_filefind_t*)d->dir;
    if (f == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (f->first) {
        f->first = 0;
        d->name = f->name;
    } else {
        if (FindNextFileW(f->find, &f->data) == 0) {
            ec = GetLastError();
            switch (ec) {
            case ERROR_FILE_NOT_FOUND:
                nai_errno = ENOENT;
                break;
            default:
                nai_errno = nai_errno_from_oserr(ec);
                break;
            };
            r = -1;
            goto _end;
        };
    };

    if (WideCharToMultiByte(CP_UTF8, 0, 
        f->data.cFileName, -1, f->name, nai_countof(f->name), 0, 0) == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };
    if (f->data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        d->type = NAI_S_IFLNK;
    } else if (f->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        d->type = NAI_S_IFDIR;
    } else if (f->data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) {
        d->type = NAI_S_IFCHR;
    } else {
        d->type = NAI_S_IFREG;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_dir_close(nai_dir_t* d)
{
    nai_int_t r;
    nai_filefind_t* f;


    f = (nai_filefind_t*)d->dir;
    if (f) {
        FindClose(f->find);
        nai_free(f);
        d->dir = 0;
        d->name = 0;
        d->type = 0;
    };

    r = 0;

    return r;
};


static nai_int_t nai_dir_create_recursive(
    wchar_t* path, SECURITY_ATTRIBUTES* sa)
{
    nai_int_t r;
    nai_int_t ec;
    wchar_t* slash;


    slash = wcsrchr(path, L'\\');
    if (slash == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* create parent */
    slash[0] = 0;
    if (CreateDirectoryW(path, sa) == 0) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_ALREADY_EXISTS:
            break;
        case ERROR_PATH_NOT_FOUND:
            r = nai_dir_create_recursive(path, sa);
            if (r < 0) {
                goto _end;
            };
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    /* create directory */
    slash[0] = L'\\';
    if (CreateDirectoryW(path, sa) == 0) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_ALREADY_EXISTS:
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_dir_create(
    const char* path, nai_int_t mode, nai_int_t recursive)
{
    nai_int_t r;
    nai_int_t ec;
    nai_wcs_t ws;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };
    wchar_t pathfull[NAI_PATH_MAX];


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    if (CreateDirectoryW(ws.str, &sa) == 0) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_ALREADY_EXISTS:
            nai_errno = EEXIST;
            r = -1;
            goto _end;
        case ERROR_PATH_NOT_FOUND:
            if (recursive) {
                if (GetFullPathNameW(ws.str, 
                    nai_countof(pathfull), pathfull, 0) == 0) {
                    nai_errno = nai_errno_from_oserr(GetLastError());
                    r = -1;
                    goto _end;
                };
                r = nai_dir_create_recursive(pathfull, &sa);
            };
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


nai_int_t nai_dir_remove(const char* path)
{
    nai_int_t r;
    nai_int_t ec;
    nai_wcs_t ws;


    r = nai_wcs_from_path(&ws, path);
    if (r < 0) {
        goto _end;
    };

    if (RemoveDirectoryW(ws.str) == 0) {
        ec = GetLastError();
        nai_errno = nai_errno_from_oserr(ec);
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    nai_wcs_release(&ws, r < 0);
    return r;
};


#else


#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif


nai_int_t nai_dir_open(nai_dir_t* d, const char* path)
{
    nai_int_t r;
#if (__qnx__)
    nai_int_t ec;
#endif


    d->type = 0;
    d->name = 0;
    d->dir = opendir(path);
    if (d->dir == 0) {
        r = -1;
        goto _end;
    };


#if (__qnx__)
    r = dircntl(d->dir, D_SETFLAG, D_FLAG_STAT|D_FLAG_FILTER);
    if (r < 0) {
        ec = nai_errno;
        closedir(d->dir);
        nai_errno = ec;
        r = -1;
        goto _end;
    };
#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_dir_read(nai_dir_t* d)
{
    nai_int_t r;
    struct dirent* ent;


    ent = readdir((DIR*)d->dir);
    if (ent == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    d->name = ent->d_name;

#if !(__qnx__)

    switch (ent->d_type) {
    case DT_BLK:
        d->type = NAI_S_IFBLK;
        break;
    case DT_CHR:
        d->type = NAI_S_IFCHR;
        break;
    case DT_DIR:
        d->type = NAI_S_IFDIR;
        break;
    case DT_LNK:
        d->type = NAI_S_IFLNK;
        break;
    case DT_REG:
        d->type = NAI_S_IFREG;
        break;
    case DT_FIFO:
        d->type = NAI_S_IFIFO;
        break;
    case DT_SOCK:
        d->type = NAI_S_IFSOCK;
        break;
    default:
        d->type = 0;
        break;
    };

#else

    struct stat* stat = 0;
    struct dirent_extra* ext;
    struct dirent_extra_stat* extstat;

    ext = _DEXTRA_FIRST(ent);
    for ( ; _DEXTRA_VALID(ext, ent); ext = _DEXTRA_NEXT(ext)) {
        switch (ext->d_type) {
        case _DTYPE_STAT:
        case _DTYPE_LSTAT:
            break;
        default:
            continue;
        };

        extstat = (struct dirent_extra_stat*)ext;
        stat = &extstat->d_stat;
        break;
    };

    if (stat) {
        d->type = stat->st_mode & NAI_S_IFMT;
    };

#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_dir_close(nai_dir_t* d)
{
    nai_int_t r;


    if (d->dir) {
        r = closedir((DIR*)d->dir);
        if (r < 0) {
            goto _end;
        };

        d->dir = 0;
        d->name = 0;
        d->type = 0;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_dir_create_recursive(const char* path, nai_int_t mode)
{
    nai_int_t r;
    nai_int_t ec;
    char* slash;


    slash = strrchr(path, '/');
    if (slash == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* create parent */
    slash[0] = 0;
    r = mkdir(path, mode);
    if (r < 0) {
        ec = nai_errno;
        switch (ec) {
        case EEXIST:
            break;
        case ENOENT:
            r = nai_dir_create_recursive(path, mode);
            if (r < 0) {
                goto _end;
            };
            break;
        default:
            r = -1;
            goto _end;
        };
    };

    /* create directory */
    slash[0] = '/';
    r = mkdir(path, mode);
    if (r < 0) {
        ec = nai_errno;
        switch (ec) {
        case EEXIST:
            break;
        default:
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_dir_create(
    const char* path, nai_int_t mode, nai_int_t recursive)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t outlen;
    char* out;
    char buf[512];


    r = mkdir(path, mode);
    if (r < 0) {
        if (recursive && nai_errno == ENOENT) {

            out = buf;
            outlen = sizeof(buf);
            for (;;) {
                r = nai_path_get_full(out, outlen, path);
                if (r < 0) {
                    ec = nai_errno;
                    break;
                };

                if (r < outlen) {
                    break;
                };

                if (out != buf) {
                    nai_free(out);
                };

                outlen = r + 1;
                out = nai_malloc(outlen);
                if (out == 0) {
                    goto _end;
                };
            };

            if (r >= 0) {
                r = nai_dir_create_recursive(out, mode);
                if (r < 0) {
                    ec = nai_errno;
                };
            };

            if (out != buf) {
                nai_free(out);
                if (r < 0) {
                    nai_errno = ec;
                };
            };
        };
    };

_end:
    return r;
};


nai_int_t nai_dir_remove(const char* path)
{
    nai_int_t r;

    r = rmdir(path);

    return r;
};



#endif

