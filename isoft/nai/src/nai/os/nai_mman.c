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
/// @file       nai_mman.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_mman.h"
#include "nai/runtime/nai_errno.h"



#if defined(_WIN32)


#include "win/nai_windows.h"
#include <memoryapi.h>
#include <handleapi.h>
#include <processthreadsapi.h>


nai_int_t nai_mmap_init(nai_mmap_t* p)
{
    p->map = 0;
#if defined(_DEBUG)
    p->detached = 0;
#endif
    return 0;
};


nai_int_t nai_mmap_open(nai_mmap_t* p, nai_fd_t file)
{
    nai_int_t r;
    nai_int_t ec;
    DWORD access;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), 0, FALSE };
    PUBLIC_OBJECT_BASIC_INFORMATION info;


    if (file == NAI_FD_INVALID) {
        p->map = 0;
    } else {
        if (nai_wapi.inited == 0) {
            nai_wapi_init();
        };

        ec = nai_wapi.NtQueryObject(file, 
            ObjectBasicInformation, &info, sizeof(info), 0);
        if (!NT_SUCCESS(ec)) {
            ec = nai_wapi.RtlNtStatusToDosError(ec);
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };

        switch (info.GrantedAccess & 
            (FILE_READ_DATA|FILE_WRITE_DATA|FILE_EXECUTE)) {
        case 0:
            access = PAGE_NOACCESS;
            break;
        case FILE_READ_DATA:
            access = PAGE_READONLY;
            break;
        case FILE_WRITE_DATA:
        case FILE_READ_DATA|FILE_WRITE_DATA:
            access = PAGE_READWRITE;
            break;
        case FILE_EXECUTE:
        case FILE_EXECUTE|FILE_READ_DATA:
            access = PAGE_EXECUTE_READ;
            break;
        case FILE_EXECUTE|FILE_WRITE_DATA:
        case FILE_EXECUTE|FILE_READ_DATA|FILE_WRITE_DATA:
            access = PAGE_EXECUTE_READWRITE;
            break;

        default:
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        p->map = CreateFileMappingW(file, &sa, access, 0, 0, 0);
        if (p->map == NULL) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };

#if (NAI_COMPAT_POSIX_MPROTECT)
        if (flags & NAI_MPROT_WRITE) {
            p->map = (void*)((intptr_t)p->map | 0x1);
        };
#endif
    };

#if defined(_DEBUG)
    p->detached = 0;
#endif
    r = 0;

_end:
    return r;
};


void* nai_mmap(nai_mmap_t* p, 
    void* addr, size_t len, nai_off64_t offset, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t ec;
    void* map;
    void* lp;
    DWORD access;
    MEMORY_BASIC_INFORMATION info;

#if (NAI_COMPAT_POSIX_MPROTECT)
    DWORD real;
#endif


    if (p && p->map != 0) {

        switch (flags & (NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE)) {
        case NAI_MPROT_NONE:
            access = 0;
            break;
        case NAI_MPROT_READ:
            access = FILE_MAP_READ;
            break;
        case NAI_MPROT_WRITE:
        case NAI_MPROT_READ|NAI_MPROT_WRITE:
            access = FILE_MAP_READ|FILE_MAP_WRITE;
            break;
        case NAI_MPROT_EXECUTE:
        case NAI_MPROT_EXECUTE|NAI_MPROT_READ:
            access = FILE_MAP_READ|FILE_MAP_EXECUTE;
            break;
        case NAI_MPROT_EXECUTE|NAI_MPROT_WRITE:
        case NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE:
            access = FILE_MAP_READ|FILE_MAP_WRITE|FILE_MAP_EXECUTE;
            break;

        default:
            nai_errno = EINVAL;
            lp = 0;
            goto _end;
        };
        if ((flags & NAI_M_SHARED) == 0) {
            access |= FILE_MAP_COPY;
        };

#if (NAI_COMPAT_POSIX_MPROTECT)
        map = (void*)((intptr_t)p->map & ~0x1);
        real = access;
        if ((intptr_t)p->map & 0x1) {
            access |= FILE_MAP_WRITE;
        };
#else
        map = p->map;
#endif

        while (1) {
            lp = MapViewOfFileEx(map, 
                access, (uint32_t)(offset >> 32), (uint32_t)offset, len, addr);
            if (lp != 0) {
                break;
            };

            ec = GetLastError();
            if (ec != ERROR_INVALID_ADDRESS || !(flags & NAI_M_FIXED)) {
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            /* query for remapping */
            r = (nai_int_t)VirtualQuery(addr, &info, sizeof(info));
            if (r == 0) {
                nai_errno = nai_errno_from_oserr(GetLastError());
                goto _end;
            };

            /* check memory is reserved */ 
            if (info.AllocationBase != addr && 
                info.RegionSize != len) {
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            if (info.Type & MEM_IMAGE) {
                ec = ERROR_INVALID_ADDRESS;
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            /* free */
            if (info.Type & MEM_MAPPED) {
                r = UnmapViewOfFile(addr);
            } else {
                r = VirtualFree(addr, 0, MEM_RELEASE);
            };
            if (r == 0) {
                nai_errno = nai_errno_from_oserr(GetLastError());
                goto _end;
            };

            flags &= ~NAI_M_FIXED;
        };

#if (NAI_COMPAT_POSIX_MPROTECT)
        if (access != real) {
            r = nai_mprotect(lp, len, 
                flags & (NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE));
            if (r < 0) {
                ec = nai_errno;
                UnmapViewOfFile(lp);
                nai_errno = ec;
                lp = 0;
            };
        };
#endif

    } else {

        if (flags & NAI_M_SHARED) {
            nai_errno = EINVAL;
            lp = 0;
            goto _end;
        };

#if defined(_DEBUG)
        if (p && p->detached) {
            nai_errno = EINVAL;
            lp = 0;
            goto _end;
        };
#endif

        switch (flags & (NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE)) {
        case NAI_MPROT_NONE:
            access = PAGE_NOACCESS;
            break;
        case NAI_MPROT_READ:
            access = PAGE_READONLY;
            break;
        case NAI_MPROT_WRITE:
        case NAI_MPROT_READ|NAI_MPROT_WRITE:
            access = PAGE_READWRITE;
            break;
        case NAI_MPROT_EXECUTE:
        case NAI_MPROT_EXECUTE|NAI_MPROT_READ:
            access = PAGE_EXECUTE_READ;
            break;
        case NAI_MPROT_EXECUTE|NAI_MPROT_WRITE:
        case NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE:
            access = PAGE_EXECUTE_READWRITE;
            break;

        default:
            nai_errno = EINVAL;
            lp = 0;
            goto _end;
        };

        while (1) {
            lp = VirtualAllocEx(
                GetCurrentProcess(), 
                addr, len, MEM_COMMIT|MEM_RESERVE, access);
            if (lp != 0) {
                break;
            };

            ec = GetLastError();
            if (ec != ERROR_INVALID_ADDRESS || !(flags & NAI_M_FIXED)) {
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            /* query for remapping */
            r = (nai_int_t)VirtualQuery(lp, &info, sizeof(info));
            if (r == 0) {
                nai_errno = nai_errno_from_oserr(GetLastError());
                goto _end;
            };

            /* check memory is reserved */ 
            if (info.AllocationBase != lp && 
                info.RegionSize != len) {
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            if (info.Type & MEM_IMAGE) {
                ec = ERROR_INVALID_ADDRESS;
                nai_errno = nai_errno_from_oserr(ec);
                goto _end;
            };

            if (info.Type & MEM_MAPPED) {
                r = UnmapViewOfFile(addr);
                if (r == 0) {
                    ec = GetLastError();
                    nai_errno = nai_errno_from_oserr(ec);
                    goto _end;
                };
            } else {
                lp = VirtualAllocEx(
                    GetCurrentProcess(), 
                    addr, len, MEM_COMMIT, access);
                if (lp != 0) {
                    goto _end;
                };

                ec = GetLastError();
                if (ec != ERROR_INVALID_ADDRESS) {
                    nai_errno = nai_errno_from_oserr(ec);
                    goto _end;
                };
            };

            flags &= ~NAI_M_FIXED;
        };
    };

_end:
    return lp;
};


nai_int_t nai_mprotect(void* addr, size_t len, nai_int_t protect)
{
    nai_int_t r;
    DWORD access = 0;
    DWORD prev;


    switch (protect) {
    case NAI_MPROT_NONE:
        access = PAGE_NOACCESS;
        break;
    case NAI_MPROT_READ:
        access = PAGE_READONLY;
        break;
    case NAI_MPROT_WRITE:
    case NAI_MPROT_READ|NAI_MPROT_WRITE:
        access = PAGE_READWRITE;
        break;
    case NAI_MPROT_EXECUTE:
    case NAI_MPROT_EXECUTE|NAI_MPROT_READ:
        access = PAGE_EXECUTE_READ;
        break;
    case NAI_MPROT_EXECUTE|NAI_MPROT_READ|NAI_MPROT_WRITE:
        access = PAGE_EXECUTE_READWRITE;
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (!VirtualProtect(addr, len, access, &prev)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_madvise(void* addr, size_t len, nai_int_t advise)
{
    nai_int_t r;


    switch (advise) {
    case NAI_MADV_NORMAL:
    case NAI_MADV_RANDOM:
    case NAI_MADV_SEQUENTIAL:
    case NAI_MADV_WILLNEED:
        break;

    case NAI_MADV_DONTNEED:
        if (!VirtualAlloc(addr, len, MEM_RESET, PAGE_NOACCESS)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_msync(void* addr, size_t len, nai_int_t flags)
{
    nai_int_t r;


    (void)flags;

    if (!FlushViewOfFile(addr, len)) {
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_munmap(nai_mmap_t* p, void* lp, size_t len)
{
    nai_int_t r;


    if (p && p->map) {
        if (!UnmapViewOfFile(lp)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
    } else {
        if (!VirtualFree(lp, 0, MEM_RELEASE)) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_mmap_detach(nai_mmap_t* p)
{
#if defined(_DEBUG)
    if (p->map != NULL) {
        p->detached = 1;
    };
#endif
    return 0;
};


nai_int_t nai_mmap_close(nai_mmap_t* p)
{
    if (p->map != NULL) {
#if (NAI_COMPAT_POSIX_MPROTECT)
        CloseHandle((void*)((intptr_t)p->map & ~0x1));
#else
        CloseHandle(p->map);
#endif
        p->map = 0;
#if defined(_DEBUG)
        p->detached = 0;
#endif
    };

    return 0;
};


#else



#include <sys/mman.h>


#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS        MAP_ANON
#endif /* MAP_ANON */


#if !defined(MAP_ANONYMOUS)


#include "nai/os/nai_thread.h"


typedef struct nai_dev_zero_s {
    nai_int_t inited;
    nai_int_t error;
    nai_once_t once;
    nai_fd_t fd;
} nai_dev_zero_t;


static nai_dev_zero_t nai_dev_zero = {
    0, 0, NAI_ONCE_INIT, -1
};

static void nai_dev_zero_init()
{
    nai_dev_zero.fd = nai_file_open("/dev/zero", O_RDWR);
    if (nai_dev_zero.fd < 0) {
        nai_dev_zero.error = nai_errno;
    };

    nai_memory_barrier();
    nai_dev_zero.inited = 1;
    return;
};


static void nai_dev_zero_term()
{
    if (nai_dev_zero.fd != NAI_FD_INVALID) {
        nai_file_close(nai_dev_zero.fd);
        nai_dev.zero.fd = NAI_FD_INVALID;
    };

    return;
};

#endif


nai_int_t nai_mmap_init(nai_mmap_t* p)
{
    p->fd = NAI_FD_INVALID;
#if defined(_DEBUG)
    p->detach = 0;
#endif

    return 0;
};


nai_int_t nai_mmap_open(nai_mmap_t* p, nai_fd_t file)
{
    nai_int_t r;


    if (file == NAI_FD_INVALID) {
#if !defined(MAP_ANONYMOUS)
        if (nai_dev_zero.inited == 0) {
            nai_once(&nai_dev_zero.once, nai_dev_zero_init);
        };
        if (nai_dev_zero.fd < 0) {
            nai_errno = nai_dev_zero.error;
            r = -1;
            goto _end;
        };
#endif
    };

    p->fd = file;
#if defined(_DEBUG)
    p->detached = 0;
#endif
    r = 0;

#if !defined(MAP_ANONYMOUS)
_end:
#endif
    return r;
};


void* nai_mmap(nai_mmap_t* p, 
    void* addr, size_t len, nai_off64_t offset, nai_int_t flags)
{
    void* r;
    nai_int_t access = 0;
    nai_int_t type;

#if (__qnx__) && (NAI_COMPAT_POSIX_MPROTECT)
    nai_int_t ec;
#endif

    if (flags & NAI_MPROT_READ) {
        access |= PROT_READ;
    };
    if (flags & NAI_MPROT_WRITE) {
        access |= PROT_WRITE;
    };
    if (flags & NAI_MPROT_EXECUTE) {
        access |= PROT_EXEC;
    };

    if (flags & NAI_M_SHARED) {
        type = MAP_SHARED;
#if (__qnx__) && (NAI_COMPAT_POSIX_MPROTECT)
        /* add write permission */
        if (!(flags & NAI_MPROT_WRITE)) {
            access |= PROT_WRITE;
        };
#endif
    } else {
        type = MAP_PRIVATE;
    };
    if (flags & NAI_M_FIXED) {
        type |= MAP_FIXED;
    };

    if (!p || p->fd == NAI_FD_INVALID) {
#if !defined(MAP_ANONYMOUS)
        if (nai_dev_zero.inited == 0) {
            nai_once(&nai_dev_zero.once, nai_dev_zero_init);
        };
        if (nai_dev_zero.fd < 0) {
            nai_errno = nai_dev_zero.error;
            return 0;
        };
        r = mmap(addr, len, access, type, nai_dev_zero.fd, 0);
#else
        r = mmap(addr, len, access, type|MAP_ANONYMOUS, -1, 0);
#endif
    } else {
#if defined(_DEBUG)
        if (p && p->detached) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
#endif

        r = mmap(addr, len, access, type, p->fd, offset);
    };
    if (r == MAP_FAILED) {
        r = 0;
        goto _end;
    };

#if (__qnx__) && (NAI_COMPAT_POSIX_MPROTECT)
    /* remove write permission */
    if (flags & NAI_M_SHARED && !(flags & NAI_MPROT_WRITE)) {
        access &= ~PROT_WRITE;
        r = mprotect(r, len, backup);
        if (r < 0) {
            ec = nai_errno;
            munmap(r, len);
            nai_errno = ec;
            r = 0;
            goto _end;
        };
    };
#endif

_end:
    return r;
};


nai_int_t nai_mprotect(void* addr, size_t len, nai_int_t protect)
{
    nai_int_t r;
    nai_int_t access = 0;


    if (protect & NAI_MPROT_READ) {
        access |= PROT_READ;
    };
    if (protect & NAI_MPROT_WRITE) {
        access |= PROT_WRITE;
    };
    if (protect & NAI_MPROT_EXECUTE) {
        access |= PROT_EXEC;
    };

    r = mprotect(addr, len, access);

    return r;
};


nai_int_t nai_madvise(void* addr, size_t len, nai_int_t advise)
{
    nai_int_t r;
    nai_int_t adv;


    switch (advise) {
    case NAI_MADV_NORMAL: adv = POSIX_MADV_NORMAL; break;
    case NAI_MADV_RANDOM: adv = POSIX_MADV_RANDOM; break;
    case NAI_MADV_SEQUENTIAL: adv = POSIX_MADV_SEQUENTIAL; break;
    case NAI_MADV_WILLNEED: adv = POSIX_MADV_WILLNEED; break;
    case NAI_MADV_DONTNEED: adv = POSIX_MADV_DONTNEED; break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = posix_madvise(addr, len, adv);

_end:
    return r;
};


nai_int_t nai_msync(void* addr, size_t len, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t flush;


    switch (flags) {
    case NAI_MSYNC_SYNC:
        flush = MS_SYNC;
        break;
    case NAI_MSYNC_ASYNC:
        flush = MS_ASYNC;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = msync(addr, len, flush);

_end:
    return r;
};


nai_int_t nai_munmap(nai_mmap_t* p, void* lp, size_t len)
{
    (void)p;

    return munmap(lp, len);
};


nai_int_t nai_mmap_detach(nai_mmap_t* p)
{
    if (p->fd != NAI_FD_INVALID) {
        p->fd = NAI_FD_INVALID;
#if defined(_DEBUG)
        p->detached = 1;
#endif
    };
    return 0;
};


nai_int_t nai_mmap_close(nai_mmap_t* p)
{
    p->fd = NAI_FD_INVALID;
#if defined(_DEBUG)
    p->detached = 0;
#endif
    return 0;
}


#endif


void nai_mmap_term()
{
#if !defined(_WIN32)
#if !defined(MAP_ANONYMOUS)
    nai_dev_zero_term();
#endif
#endif

    return;
};


