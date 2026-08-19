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
/// @file       nai_shmem.c
/// @brief      
/// @details
/// @date       2021-11-04
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_shmem.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"
#include <stdarg.h>


#if defined(_WIN32)
#include "win/nai_windows.h"
#include <memoryapi.h>
#endif


static nai_int_t nai_shm_open_anonyous(
    nai_shm_t* p, void* addr, size_t size, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t access;
#if defined(_WIN32)
    HANDLE m;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };
#endif


    switch (flags & (NAI_O_CREAT|NAI_O_EXCL)) {
    case 0:
    case NAI_O_EXCL:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    default:
        break;
    };

    switch (flags & (NAI_O_RDONLY|NAI_O_WRONLY|NAI_O_RDWR)) {
    case NAI_O_RDONLY:
        access = NAI_MPROT_READ;
        break;
    case NAI_O_WRONLY:
        access = NAI_MPROT_WRITE;
        break;
    case NAI_O_RDWR:
        access = NAI_MPROT_READ|NAI_MPROT_WRITE;
        break;
    default:
        ec = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & NAI_O_EXECUTE) {
        access |= NAI_MPROT_EXECUTE;
    };

    nai_mmap_init(&p->map);

#if defined(_WIN32)
    m = CreateFileMappingW(NAI_FD_INVALID, 
        &sa, PAGE_EXECUTE_READWRITE, 
        (uint32_t)((uint64_t)size >> 32), (uint32_t)size, 0);
    if (m == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    p->map.map = m;
#endif

    p->addr = (uint8_t*)nai_mmap(
        &p->map, addr, size, 0, access | NAI_M_SHARED);
    if (p->addr == 0) {
        goto _fail;
    };

    nai_mmap_detach(&p->map);

    /* success */
    p->size = size;
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_mmap_close(&p->map);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_shm_open(nai_shm_t* p, 
    const char* path, void* addr, size_t size, nai_int_t flags, ...)
{
    int64_t r;
    nai_int_t ec;
    nai_int_t mode;
    nai_int_t open;
    nai_int_t create;
    nai_int_t access;
    uint64_t init;
    va_list va;
    nai_fd_t fd;


    if (path == 0) {
        r = nai_shm_open_anonyous(p, addr, size, flags);
        goto _end;
    };

    if (flags & NAI_O_TRUNC) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & NAI_O_TMPFILE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    open = flags & (NAI_O_CREAT | NAI_O_EXCL);
    if ((open & NAI_O_CREAT) && size == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    switch (flags & (NAI_O_RDONLY|NAI_O_WRONLY|NAI_O_RDWR)) {
    case NAI_O_RDONLY:
        if (open & NAI_O_CREAT) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        access = NAI_MPROT_READ;
        break;
    case NAI_O_WRONLY:
        access = NAI_MPROT_WRITE;
        break;
    case NAI_O_RDWR:
        access = NAI_MPROT_READ|NAI_MPROT_WRITE;
        break;
    default:
        ec = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & NAI_O_EXECUTE) {
        access |= NAI_MPROT_EXECUTE;
    };


    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create == 0) {
        fd = nai_file_open(path, flags);
    } else {
        va_start(va, flags);
        mode = va_arg(va, nai_int_t);
        va_end(va);

        fd = nai_file_open(path, flags, mode);
    };
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    init = -1;
    r = nai_file_lock(fd, 
        (access & NAI_MPROT_WRITE) ? NAI_FLOCK_WRITE : NAI_FLOCK_READ);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    init = nai_file_get_size(fd);
    if (init == (uint64_t)-1) {
        ec = nai_errno;
        goto _fail;
    };

    if (!(open & NAI_O_CREAT)) {    /* open only */
        if (init == 0) {
            ec = ENOENT;
            goto _fail;
        };
    } else {                        /* open and create */
        if (init != 0 && (open & NAI_O_EXCL)) {
            ec = EEXIST;
            goto _fail;
        };
        if (init == 0) {
            r = nai_file_set_size(fd, size);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };
        };
    };

    r = nai_file_unlock(fd);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };


    r = nai_mmap_open(&p->map, fd);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    p->addr = (uint8_t*)nai_mmap(
        &p->map, addr, size, 0, access | NAI_M_SHARED);
    if (p->addr == 0) {
        ec = nai_errno;
        nai_mmap_close(&p->map);
        goto _fail;
    };

    /* close the file and detach from file mapping */
    nai_mmap_detach(&p->map);
    nai_file_close(fd);

    /* success */
    p->size = size;
    r = 0;

_end:
    return (nai_int_t)r;

_fail:
    nai_file_close(fd);

    if (open & NAI_O_CREAT) {
        if ((open & NAI_O_EXCL) ||      /* case create new */
            (init == 0)) {              /* case open always */
            nai_file_unlink(path);
        };
    };

    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_shm_close(nai_shm_t* p)
{
    nai_int_t r;


    if (p->addr) {
        r = nai_munmap(&p->map, p->addr, p->size);
        if (r < 0) {
            goto _end;
        };

        p->addr = 0;
        p->size = 0;
    };

    nai_mmap_close(&p->map);
    r = 0;

_end:
    return r;
};


nai_int_t nai_shm_unlink(const char* path)
{
    return nai_file_unlink(path);
};


