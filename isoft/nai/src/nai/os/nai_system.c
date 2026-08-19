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
/// @file       nai_system.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_system.h"
#include "nai/runtime/nai_errno.h"


#if defined(_WIN32)


#include "win/nai_windows.h"
#include <sysinfoapi.h>
#include <synchapi.h>
#include <timezoneapi.h>
#include <profileapi.h>
#include <processthreadsapi.h>
#include <malloc.h>


typedef BOOL (WINAPI *GetLogicalProcessorInformation_f)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);

typedef ULONGLONG (WINAPI *GetTickCount64_f)();


static LARGE_INTEGER frequency;


uint64_t nai_time()
{
    FILETIME fTime;
#if defined(_WINCE)
    GetSystemTimeAsFileTime(&fTime);
#else
    SYSTEMTIME sTime;
    GetSystemTime(&sTime);
    SystemTimeToFileTime(&sTime, &fTime);
#endif
    return (*(uint64_t*)(&fTime) - NAI_DELTA_EPOCH_USEC) / 10i64;
};


uint64_t nai_tick(nai_int_t units)
{
    uint64_t r;
    uint32_t scale;
    LARGE_INTEGER time;


#if 0
    if (nai_wapi.inited == 0) {
        nai_wapi_init();
    };

    if (nai_wapi.GetTickCount64) {
        switch (units) {
        case NAI_TIME_MSEC:
            r = nai_wapi.GetTickCount64();
            break;
        case NAI_TIME_USEC:
            r = nai_wapi.GetTickCount64() * 1000;
            break;
        default:
            nai_errno = EINVAL;
            r = -1;
            break;
        };

        goto _end;
    };
#endif

    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    };


    switch (units) {
    case NAI_TIME_MSEC:
        scale = 1000;
        break;
    case NAI_TIME_USEC:
        scale = 1000*1000;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    QueryPerformanceCounter(&time);
    time.QuadPart *= scale;
    time.QuadPart /= frequency.QuadPart;
    r = time.QuadPart;

_end:
    return r;
};


uint32_t nai_tick32(nai_int_t units)
{
    uint32_t r;


#if 0
    switch (units) {
    case NAI_TIME_MSEC:
        r = GetTickCount();
        break;
    case NAI_TIME_USEC:
        r = GetTickCount() * 1000;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        break;
    };
#endif

    r = (uint32_t)nai_tick(units);

    return r;
};


nai_int_t nai_sleep(uint64_t usec)
{
    uint32_t msec;
    msec = (uint32_t)(usec / 1000);
    msec += !!(usec % 1000);
    Sleep(msec);
    return 0;
};


nai_int_t nai_yield()
{
    SwitchToThread();
    return 0;
};


nai_int_t nai_nprocs(nai_int_t logical)
{
    nai_int_t r = -1;
    nai_int_t n;
    nai_int_t count;
    SYSTEM_INFO si;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buf = 0;
    DWORD buflen = 0;


    if (logical == 0 && !nai_wapi.inited) {
        nai_wapi_init();
    };

    if (logical || nai_wapi.GetLogicalProcessorInformation == 0) {
        GetSystemInfo(&si);
        r = si.dwNumberOfProcessors;
        goto _end;
    };


    while (!nai_wapi.GetLogicalProcessorInformation(buf, &buflen)) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            nai_errno = nai_errno_from_oserr(GetLastError());
            goto _fail;
        };

        if (buf) {
            free(buf);
        };

        buf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(buflen);
        if (buf == 0) {
            nai_errno = ENOMEM;
            goto _fail;
        };
    };


    r = 0;
    count = buflen/sizeof(*buf);
    for (n = 0; n < count; n ++) {
        if (buf[n].Relationship == RelationProcessorCore) {
            r ++;
        };
    };

_fail:
    if (buf) {
        free(buf);
    };

_end:
    return r;
};



/* copy from msvcrt and wepoll */

/* The following two constants must be the minimum and maximum
   values in the (contiguous) range of Exec Failure errors. */
#define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
#define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN

/* These are the low and high value in the range of errors that are
   access violations */
#define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
#define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED


#define OSERR_2_ERRNO_MAPPINGS(X)                                   \
    X(ERROR_ACCESS_DENIED,          EACCES          )  /* 5 */      \
    X(ERROR_ALREADY_EXISTS,         EEXIST          )  /* 183 */    \
    X(ERROR_ARENA_TRASHED,          ENOMEM          )  /* 7 */      \
    X(ERROR_BAD_COMMAND,            EACCES          )               \
    X(ERROR_BAD_ENVIRONMENT,        E2BIG           )  /* 10 */     \
    X(ERROR_BAD_EXE_FORMAT,         ENOEXEC         )               \
    X(ERROR_BAD_FORMAT,             ENOEXEC         )  /* 11 */     \
    X(ERROR_BAD_LENGTH,             EACCES          )               \
    X(ERROR_BAD_NETPATH,            ENOENT          )  /* 53 */     \
    X(ERROR_BAD_NET_NAME,           ENOENT          )  /* 67 */     \
    X(ERROR_BAD_NET_RESP,           ENETDOWN        )               \
    X(ERROR_BAD_PATHNAME,           ENOENT          )  /* 161 */    \
    X(ERROR_BROKEN_PIPE,            EPIPE           )  /* 109 */    \
    X(ERROR_CANNOT_MAKE,            EACCES          )  /* 82 */     \
    X(ERROR_CHILD_NOT_COMPLETE,     ECHILD          )  /* 129 */    \
    X(ERROR_COMMITMENT_LIMIT,       ENOMEM          )               \
    X(ERROR_CONNECTION_ABORTED,     ECONNABORTED    )               \
    X(ERROR_CONNECTION_ACTIVE,      EISCONN         )               \
    X(ERROR_CONNECTION_REFUSED,     ECONNREFUSED    )               \
    X(ERROR_CRC,                    EACCES          )               \
    X(ERROR_CURRENT_DIRECTORY,      EACCES          )  /* 16 */     \
    X(ERROR_DIRECT_ACCESS_HANDLE,   EBADF           )  /* 130 */    \
    X(ERROR_DIR_NOT_EMPTY,          ENOTEMPTY       )  /* 145 */    \
    X(ERROR_DISK_FULL,              ENOSPC          )  /* 112 */    \
    X(ERROR_DRIVE_LOCKED,           EACCES          )  /* 108 */    \
    X(ERROR_DUP_NAME,               EADDRINUSE      )               \
    X(ERROR_FAIL_I24,               EACCES          )  /* 83 */     \
    X(ERROR_FILENAME_EXCED_RANGE,   ENOENT          )  /* 206 */    \
    X(ERROR_FILE_EXISTS,            EEXIST          )  /* 80 */     \
    X(ERROR_FILE_NOT_FOUND,         ENOENT          )  /* 2 */      \
    X(ERROR_GEN_FAILURE,            EACCES          )               \
    X(ERROR_GRACEFUL_DISCONNECT,    EPIPE           )               \
    X(ERROR_HOST_DOWN,              EHOSTUNREACH    )               \
    X(ERROR_HOST_UNREACHABLE,       EHOSTUNREACH    )               \
    X(ERROR_INSUFFICIENT_BUFFER,    EFAULT          )               \
    X(ERROR_INVALID_ACCESS,         EINVAL          )  /* 12 */     \
    X(ERROR_INVALID_ADDRESS,        EADDRNOTAVAIL   )               \
    X(ERROR_INVALID_BLOCK,          ENOMEM          )  /* 9 */      \
    X(ERROR_INVALID_DATA,           EINVAL          )  /* 13 */     \
    X(ERROR_INVALID_DRIVE,          ENOENT          )  /* 15 */     \
    X(ERROR_INVALID_FUNCTION,       EINVAL          )  /* 1 */      \
    X(ERROR_INVALID_HANDLE,         EBADF           )  /* 6 */      \
    X(ERROR_INVALID_NETNAME,        EADDRNOTAVAIL   )               \
    X(ERROR_INVALID_PARAMETER,      EINVAL          )  /* 87 */     \
    X(ERROR_INVALID_TARGET_HANDLE,  EBADF           )  /* 114 */    \
    X(ERROR_INVALID_USER_BUFFER,    EMSGSIZE        )               \
    X(ERROR_IO_PENDING,             EINPROGRESS     )  /* 997 */    \
    X(ERROR_LOCK_FAILED,            EACCES          )  /* 167 */    \
    X(ERROR_LOCK_VIOLATION,         EACCES          )  /* 33 */     \
    X(ERROR_MAX_THRDS_REACHED,      ENOMEM          )  /* 164 */    \
    X(ERROR_MORE_DATA,              EMSGSIZE        )               \
    X(ERROR_NETNAME_DELETED,        ECONNABORTED    )               \
    X(ERROR_NETWORK_ACCESS_DENIED,  EACCES          )  /* 65 */     \
    X(ERROR_NETWORK_BUSY,           ENETDOWN        )               \
    X(ERROR_NETWORK_UNREACHABLE,    ENETUNREACH     )               \
    X(ERROR_NEGATIVE_SEEK,          EINVAL          )  /* 131 */    \
    X(ERROR_NESTING_NOT_ALLOWED,    EACCES          )  /* 215 */    \
    X(ERROR_NOACCESS,               EFAULT          )               \
    X(ERROR_NONPAGED_SYSTEM_RESOURCES, ENOMEM       )               \
    X(ERROR_NO_DATA,                ENODATA         )               \
    X(ERROR_NO_PROC_SLOTS,          ENOENT          )  /* 89 */     \
    X(ERROR_NOT_ENOUGH_MEMORY,      ENOMEM          )  /* 8 */      \
    X(ERROR_NOT_ENOUGH_QUOTA,       ENOMEM          )  /* 1816 */   \
    X(ERROR_NOT_FOUND,              ENOENT          )               \
    X(ERROR_NOT_LOCKED,             EACCES          )  /* 158 */    \
    X(ERROR_NOT_READY,              EACCES          )               \
    X(ERROR_NOT_SAME_DEVICE,        EXDEV           )  /* 17 */     \
    X(ERROR_NOT_SUPPORTED,          ENOTSUP         )               \
    X(ERROR_NO_MORE_FILES,          ENOENT          )  /* 18 */     \
    X(ERROR_NO_SYSTEM_RESOURCES,    ENOMEM          )               \
    X(ERROR_OPERATION_ABORTED,      ECANCELED       )  /* 995 */    \
    X(ERROR_OUT_OF_PAPER,           EACCES          )               \
    X(ERROR_PAGED_SYSTEM_RESOURCES, ENOMEM          )               \
    X(ERROR_PAGEFILE_QUOTA,         ENOMEM          )               \
    X(ERROR_PATH_NOT_FOUND,         ENOENT          )  /* 3 */      \
    X(ERROR_PIPE_NOT_CONNECTED,     EPIPE           )               \
    X(ERROR_PORT_UNREACHABLE,       ECONNRESET      )               \
    X(ERROR_PROTOCOL_UNREACHABLE,   ENETUNREACH     )               \
    X(ERROR_REM_NOT_LIST,           ECONNREFUSED    )               \
    X(ERROR_REQUEST_ABORTED,        EINTR           )               \
    X(ERROR_REQ_NOT_ACCEP,          EWOULDBLOCK     )               \
    X(ERROR_SECTOR_NOT_FOUND,       EACCES          )               \
    X(ERROR_SEEK_ON_DEVICE,         EACCES          )  /* 132 */    \
    X(ERROR_SEM_TIMEOUT,            ETIMEDOUT       )               \
    X(ERROR_SHARING_VIOLATION,      EACCES          )               \
    X(ERROR_TOO_MANY_NAMES,         ENOMEM          )               \
    X(ERROR_TOO_MANY_OPEN_FILES,    EMFILE          )  /* 4 */      \
    X(ERROR_UNEXP_NET_ERR,          ECONNABORTED    )               \
    X(ERROR_WAIT_NO_CHILDREN,       ECHILD          )  /* 128 */    \
    X(ERROR_WORKING_SET_QUOTA,      ENOMEM          )               \
    X(ERROR_WRITE_PROTECT,          EACCES          )               \
    X(ERROR_WRONG_DISK,             EACCES          )               \
    X(WSAEACCES,                    EACCES          )  /* 10013 */  \
    X(WSAEALREADY,                  EALREADY        )  /* 10037 */  \
    X(WSAEADDRINUSE,                EADDRINUSE      )  /* 10047 */  \
    X(WSAEADDRNOTAVAIL,             EADDRNOTAVAIL   )  /* 10049 */  \
    X(WSAEAFNOSUPPORT,              EAFNOSUPPORT    )  /* 10047 */  \
    X(WSAEBADF,                     EBADF           )  /* 10009 */  \
    X(WSAECANCELLED,                ECANCELED       )  /* 10103 */  \
    X(WSAECONNABORTED,              ECONNABORTED    )  /* 10053 */  \
    X(WSAECONNREFUSED,              ECONNREFUSED    )  /* 10061 */  \
    X(WSAECONNRESET,                ECONNRESET      )  /* 10054 */  \
    X(WSAEDESTADDRREQ,              EDESTADDRREQ    )  /* 10039 */  \
    X(WSAEDISCON,                   EPIPE           )               \
    X(WSAEFAULT,                    EFAULT          )  /* 10014 */  \
    X(WSAEHOSTDOWN,                 EHOSTUNREACH    )               \
    X(WSAEHOSTUNREACH,              EHOSTUNREACH    )  /* 10065 */  \
    X(WSAEINPROGRESS,               EINPROGRESS     )  /* 10036 */  \
    X(WSAEINTR,                     EINTR           )  /* 10004 */  \
    X(WSAEINVAL,                    EINVAL          )  /* 10022 */  \
    X(WSAEISCONN,                   EISCONN         )  /* 10056 */  \
    X(WSAELOOP,                     ELOOP           )  /* 10062 */  \
    X(WSAEMFILE,                    EMFILE          )  /* 10024 */  \
    X(WSAEMSGSIZE,                  EMSGSIZE        )  /* 10040 */  \
    X(WSAENAMETOOLONG,              ENAMETOOLONG    )  /* 10063 */  \
    X(WSAENETDOWN,                  ENETDOWN        )  /* 10050 */  \
    X(WSAENETRESET,                 ENETRESET       )  /* 10052 */  \
    X(WSAENETUNREACH,               ENETUNREACH     )  /* 10051 */  \
    X(WSAENOBUFS,                   ENOMEM          )  /* 10055 */  \
    X(WSAENOPROTOOPT,               ENOPROTOOPT     )  /* 10042 */  \
    X(WSAENOTCONN,                  ENOTCONN        )  /* 10057 */  \
    X(WSAENOTEMPTY,                 ENOTEMPTY       )  /* 10066 */  \
    X(WSAENOTSOCK,                  ENOTSOCK        )  /* 10038 */  \
    X(WSAEOPNOTSUPP,                EOPNOTSUPP      )  /* 10045 */  \
    X(WSAEPROCLIM,                  ENOMEM          )               \
    X(WSAEPROTONOSUPPORT,           EPROTONOSUPPORT )  /* 10043 */  \
    X(WSAEPROTOTYPE,                EPROTOTYPE      )  /* 10041 */  \
    X(WSAESHUTDOWN,                 EPIPE           )               \
    X(WSAETIMEDOUT,                 ETIMEDOUT       )  /* 10060 */  \
    X(WSAEWOULDBLOCK,               EAGAIN          )  /* 10035 */  \
    X(WSANOTINITIALISED,            ENETDOWN        )               \
    X(WSASYSNOTREADY,               ENETDOWN        )               \
    X(WSAVERNOTSUPPORTED,           ENOTSUP         )               \


nai_int_t nai_errno_from_oserr(unsigned long oserrno)
{
    nai_int_t r;

#define CASE(oserr_sym, errno_sym)                      \
    case oserr_sym:                                     \
        r = errno_sym;                                  \
        goto _end;                                      \

    switch (oserrno) {
    OSERR_2_ERRNO_MAPPINGS(CASE)
    };

#undef CASE

    /* The error code wasn't in the table.  We check for a range of */
    /* EACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
    /* EINVAL is returned.                                          */

    if (oserrno >= MIN_EACCES_RANGE && oserrno <= MAX_EACCES_RANGE)
        r = EACCES;
    else if (oserrno >= MIN_EXEC_ERROR && oserrno <= MAX_EXEC_ERROR)
        r = ENOEXEC;
    else if (oserrno >= 10000)
        r = oserrno;
    else
        r = EINVAL;

_end:
    return r;
};


#if !defined(NAI_STATIC)

nai_int_t* __nai_errno()
{
    return &errno;
};

#endif


#if defined(NAI_DLL)


static void nai_term();


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        nai_term();
        break;
    }
    return TRUE;
}

#endif


#else


#if (NAI_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if (NAI_HAVE_SYS_SYSINFO_H)
#include <sys/sysinfo.h>
#endif

#if (NAI_HAVE_TIME_H)
#include <time.h>
#endif
#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if (NAI_HAVE_SCHED_YIELD)
#include <sched.h>
#elif (NAI_HAVE_PTHREAD_YIELD)
#include <pthread.h>
#endif



uint64_t nai_time()
{
    uint64_t r;

#if (NAI_HAVE_CLOCK_GETTIME)

    struct timespec now;


    if (clock_gettime(CLOCK_REALTIME, &now) < 0) {
        r = -1;
        goto _end;
    };

    r = nai_timespec_to_usec(&now);

#elif (NAI_HAVE_GETTIMEOFDAY)

    struct timeval now;


    if (gettimeofday(&now, 0) < 0) {
        r = -1;
        goto _end;
    };

    r = nai_timeval_to_usec(&now);

#else

#error "nai_time is not implemented!"

#endif

_end:
    return r;
};


uint64_t nai_tick(nai_int_t units)
{
    uint64_t r;

#if (NAI_HAVE_CLOCK_GETTIME)

    struct timespec now;


    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) {
        r = -1;
        goto _end;
    };

    switch (units) {
    default:
        nai_errno = EINVAL;
        r = -1;
        break;
    case NAI_TIME_MSEC:
        r = nai_timespec_to_msec(&now);
        break;
    case NAI_TIME_USEC:
        r = nai_timespec_to_usec(&now);
        break;
    };

#elif (NAI_HAVE_GETTIMEOFDAY)

    struct timeval now;


    if (gettimeofday(&now, 0) < 0) {
        r = -1;
        goto _end;
    };

    switch (units) {
    default:
        nai_errno = EINVAL;
        r = -1;
        break;
    case NAI_TIME_MSEC:
        r = nai_timeval_to_msec(&now);
        break;
    case NAI_TIME_USEC:
        r = nai_timeval_to_usec(&now);
        break;
    };

#else

#error "nai_tick is not implemented!"

    (void)units;

#endif

_end:
    return r;
}


uint32_t nai_tick32(nai_int_t units)
{
    uint32_t r;

#if (NAI_HAVE_CLOCK_GETTIME)

    struct timespec now;


    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) {
        r = -1;
        goto _end;
    };

    switch (units) {
    default:
        nai_errno = EINVAL;
        r = -1;
        break;
    case NAI_TIME_MSEC:
        r = (uint32_t)nai_timespec_to_msec(&now);
        break;
    case NAI_TIME_USEC:
        r = (uint32_t)nai_timespec_to_usec(&now);
        break;
    };

#elif (NAI_HAVE_GETTIMEOFDAY)

    struct timeval now;


    if (gettimeofday(&now, 0) < 0) {
        r = -1;
        goto _end;
    };

    switch (units) {
    default:
        nai_errno = EINVAL;
        r = -1;
        break;
    case NAI_TIME_MSEC:
        r = (uint32_t)nai_timeval_to_msec(&now);
        break;
    case NAI_TIME_USEC:
        r = (uint32_t)nai_timeval_to_usec(&now);
        break;
    };

#else

#error "nai_tick32 is not implemented!"

    (void)units;

#endif

_end:
    return r;
}


nai_int_t nai_sleep(uint64_t usec)
{
    nai_int_t r;

#if (NAI_HAVE_NANOSLEEP)

    struct timespec tm;
    struct timespec tv;


    tv.tv_sec = usec / 1000000;
    tv.tv_nsec = usec % 1000000 * 1000;
    for (;;) {
        r = nanosleep(&tv, &tm);
        if (r >= 0 || nai_errno != EINTR) {
            break;
        };
        tv = tm;
    };

#elif (NAI_HAVE_SELECT)

    struct timeval tv;


    tv.tv_sec = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    r = select(0, NULL, NULL, NULL, &tv);
    /* fixme: unhandle EINTR */

#else

#error "nai_sleep is not implemented!"

    (void)usec;

#endif

    return r;
};


nai_int_t nai_yield()
{
    nai_int_t r;


#if (NAI_HAVE_SCHED_YIELD)
    r = sched_yield();
#elif (NAI_HAVE_PTHREAD_YIELD) && defined(__USE_GNU)

    r = pthread_yield();
    if (r) {
        nai_errno = r;
        r = -1;
    };
#else
    r = 0;
#endif

    return r;
};


nai_int_t nai_nprocs(nai_int_t logical)
{
    nai_int_t cores;
#if defined(__i386__) || defined(__x86_64__)
    nai_int_t ht;
    nai_int_t regs[4];
#endif


#if (NAI_HAVE_GET_NPROCS)
    cores = get_nprocs();
#elif defined(_SC_NPROCESSORS_ONLN)
    cores = sysconf(_SC_NPROCESSORS_ONLN);
#else
#error "nai_nprocs is not implemented!"
#endif

    if (logical == 0 && cores > 1) {

#if defined(__i386__) || defined(__x86_64__)
        __asm__ __volatile__ ("cpuid " :
               "=a" (regs[0]),
               "=b" (regs[1]),
               "=c" (regs[2]),
               "=d" (regs[3])
               : "a" (1), "c" (0));

        ht = regs[3] & (1 << 28);
        if (ht) {
            cores /= 2;
        };
#endif
    };

    return cores;
};



#endif


uint32_t nai_pagesize = 4096;
uint32_t nai_pagesize_shift = 12;
uint32_t nai_cacheline_size = 64;
uint32_t nai_sector_size = 512;



#if __GNUC__ >= 4
#define NAI_DETOR __attribute__((destructor))
#else
#define NAI_DETOR /* nothing */
#endif


extern void nai_signal_term();
extern void nai_fs_watch_term();
extern void nai_ssl_term();
extern void nai_bufpool_term();
extern void nai_aio_term();
extern void nai_sem_term();
extern void nai_mmap_term();
extern void nai_proc_term();
extern void nai_thread_term();
extern void nai_os_term();
extern void nai_log_term();


NAI_DETOR
static void nai_term()
{
    nai_signal_term();
    nai_fs_watch_term();
    nai_ssl_term();
    nai_bufpool_term();
    nai_aio_term();
    nai_sem_term();
    nai_mmap_term();
    nai_proc_term();
    nai_thread_term();
    nai_os_term();
    nai_log_term();

    return;
};


