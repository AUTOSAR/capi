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
/// @file       nai_windows.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _WINDOWS_H_NAI
#define _WINDOWS_H_NAI

#pragma once

#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_WIN32)

    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif

    #if !defined(_WIN32_WINNT)
        #define _WIN32_WINNT 0x0501
    #endif

    #define NOATOM
    #define NOGDI
    #define NOGDICAPMASKS
    #define NOMETAFILE
    #define NOMINMAX
    #define NOMSG
    #define NOOPENFILE
    #define NORASTEROPS
    #define NOSCROLL
    #define NOSOUND
    #define NOSYSMETRICS
    #define NOTEXTMETRIC
    #define NOWH
    #define NOCOMM
    #define NOKANJI
    #define NOCRYPT
    #define NOMCX
    #define NOIMM
    #define NOUSER

    #include <errhandlingapi.h>
    #include <minwinbase.h>
    #include <minwindef.h>
    #include <winerror.h>

    #ifndef FILE_SKIP_COMPLETION_PORT_ON_SUCCESS
        #define FILE_SKIP_COMPLETION_PORT_ON_SUCCESS 0x1
    #endif
    #ifndef FILE_SKIP_SET_EVENT_ON_HANDLE
        #define FILE_SKIP_SET_EVENT_ON_HANDLE 0x2
    #endif

    /* filetime delta */
    #define NAI_DELTA_EPOCH_USEC 11644473600000000LL

        //////////////////////////////////////////////////////////////////////////////
        // wapis

    #define NT_SUCCESS(status) ((status) >= 0)

    typedef LONG NTSTATUS;

    #ifndef _WIN32_WINNT_WIN7

    typedef struct _GROUP_AFFINITY
    {
        ULONG_PTR Mask;
        WORD Group;
        WORD Reserved[3];
    } GROUP_AFFINITY, *PGROUP_AFFINITY;

    #endif

    typedef ULONGLONG(WINAPI* GetTickCount64_f)();

    typedef BOOL(WINAPI* GetLogicalProcessorInformation_f)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

    typedef BOOL(WINAPI* GetProcessGroupAffinity_f)(HANDLE hProcess, PUSHORT GroupCount, PUSHORT GroupArray);

    typedef BOOL(WINAPI* GetThreadGroupAffinity_f)(HANDLE hThread, PGROUP_AFFINITY GroupAffinity);

    typedef BOOL(WINAPI* SetThreadGroupAffinity_f)(HANDLE hThread,
                                                   const GROUP_AFFINITY* GroupAffinity,
                                                   PGROUP_AFFINITY PreviousGroupAffinity);

    typedef BOOL(WINAPI* GetQueuedCompletionStatusEx_f)(HANDLE CompletionPort,
                                                        LPOVERLAPPED_ENTRY lpCompletionPortEntries,
                                                        ULONG ulCount,
                                                        PULONG ulNumEntriesRemoved,
                                                        DWORD dwMilliseconds,
                                                        BOOL fAlertable);

    typedef BOOL(WINAPI* SetFileCompletionNotificationModes_f)(HANDLE FileHandle, UCHAR Flags);

    typedef BOOL(WINAPI* CancelSynchronousIo_f)(HANDLE hThread);

    typedef DWORD(WINAPI* if_nametoindex_f)(PCSTR name);

    typedef PCHAR(WINAPI* if_indextoname_f)(DWORD index, PCHAR name);

    typedef struct _RTL_SRWLOCK* PSRWLOCK;

    typedef void(WINAPI* InitializeSRWLock_f)(PSRWLOCK);

    typedef BOOL(WINAPI* TryAcquireSRWLockExclusive_f)(PSRWLOCK);

    typedef BOOL(WINAPI* TryAcquireSRWLockShared_f)(PSRWLOCK);

    typedef void(WINAPI* AcquireSRWLockExclusive_f)(PSRWLOCK);

    typedef void(WINAPI* ReleaseSRWLockExclusive_f)(PSRWLOCK);

    typedef void(WINAPI* AcquireSRWLockShared_f)(PSRWLOCK);

    typedef void(WINAPI* ReleaseSRWLockShared_f)(PSRWLOCK);

    #define RTL_CONSTANT_STRING(s)                                                                                     \
        {                                                                                                              \
            sizeof(s) - sizeof((s)[0]), sizeof(s), s                                                                   \
        }

    #define RTL_CONSTANT_OBJECT_ATTRIBUTES(ObjectName, Attributes)                                                     \
        {                                                                                                              \
            sizeof(OBJECT_ATTRIBUTES), NULL, ObjectName, Attributes, NULL, NULL                                        \
        }

    typedef struct _IO_STATUS_BLOCK
    {
        NTSTATUS Status;
        ULONG_PTR Information;
    } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

    typedef VOID(NTAPI* PIO_APC_ROUTINE)(PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, ULONG Reserved);

    typedef struct _UNICODE_STRING
    {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR Buffer;
    } UNICODE_STRING, *PUNICODE_STRING;

    typedef struct _OBJECT_ATTRIBUTES
    {
        ULONG Length;
        HANDLE RootDirectory;
        PUNICODE_STRING ObjectName;
        ULONG Attributes;
        PVOID SecurityDescriptor;
        PVOID SecurityQualityOfService;
    } OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

    typedef enum _OBJECT_INFORMATION_CLASS
    {
        ObjectBasicInformation,
        ObjectTypeInformation
    } OBJECT_INFORMATION_CLASS;

    typedef struct _PUBLIC_OBJECT_BASIC_INFORMATION
    {
        ULONG Attributes;
        ACCESS_MASK GrantedAccess;
        ULONG HandleCount;
        ULONG PointerCount;
        ULONG Reserved[10];
    } PUBLIC_OBJECT_BASIC_INFORMATION, *PPUBLIC_OBJECT_BASIC_INFORMATION;

    typedef ULONG(NTAPI* RtlNtStatusToDosError_f)(NTSTATUS Status);

    typedef NTSTATUS(NTAPI* NtResumeProcess_f)(HANDLE ProcessHandle);

    typedef NTSTATUS(NTAPI* NtSuspendProcess_f)(HANDLE ProcessHandle);

    typedef NTSTATUS(NTAPI* NtQueryObject_f)(HANDLE Handle,
                                             OBJECT_INFORMATION_CLASS ObjectInformationClass,
                                             PVOID ObjectInformation,
                                             ULONG ObjectInformationLength,
                                             PULONG ReturnLength);

    typedef NTSTATUS(NTAPI* NtCreateFile_f)(PHANDLE FileHandle,
                                            ACCESS_MASK DesiredAccess,
                                            POBJECT_ATTRIBUTES ObjectAttributes,
                                            PIO_STATUS_BLOCK IoStatusBlock,
                                            PLARGE_INTEGER AllocationSize,
                                            ULONG FileAttributes,
                                            ULONG ShareAccess,
                                            ULONG CreateDisposition,
                                            ULONG CreateOptions,
                                            PVOID EaBuffer,
                                            ULONG EaLength);

    typedef NTSTATUS(NTAPI* NtCancelIoFileEx_f)(HANDLE FileHandle,
                                                PIO_STATUS_BLOCK IoRequestToCancel,
                                                PIO_STATUS_BLOCK IoStatusBlock);

    typedef NTSTATUS(NTAPI* NtDeviceIoControlFile_f)(HANDLE FileHandle,
                                                     HANDLE Event,
                                                     PIO_APC_ROUTINE ApcRoutine,
                                                     PVOID ApcContext,
                                                     PIO_STATUS_BLOCK IoStatusBlock,
                                                     ULONG IoControlCode,
                                                     PVOID InputBuffer,
                                                     ULONG InputBufferLength,
                                                     PVOID OutputBuffer,
                                                     ULONG OutputBufferLength);

    typedef struct nai_wapi_s
    {
        nai_int_t inited;
        nai_int_t error;
        nai_once_t once;
        HMODULE modules[1];

        GetTickCount64_f GetTickCount64;
        GetLogicalProcessorInformation_f GetLogicalProcessorInformation;
        GetProcessGroupAffinity_f GetProcessGroupAffinity;
        GetThreadGroupAffinity_f GetThreadGroupAffinity;
        SetThreadGroupAffinity_f SetThreadGroupAffinity;
        GetQueuedCompletionStatusEx_f GetQueuedCompletionStatusEx;
        SetFileCompletionNotificationModes_f SetFileCompletionNotificationModes;
        CancelSynchronousIo_f CancelSynchronousIo;

        InitializeSRWLock_f InitializeSRWLock;
        TryAcquireSRWLockExclusive_f TryAcquireSRWLockExclusive;
        TryAcquireSRWLockShared_f TryAcquireSRWLockShared;
        AcquireSRWLockExclusive_f AcquireSRWLockExclusive;
        ReleaseSRWLockExclusive_f ReleaseSRWLockExclusive;
        AcquireSRWLockShared_f AcquireSRWLockShared;
        ReleaseSRWLockShared_f ReleaseSRWLockShared;

        RtlNtStatusToDosError_f RtlNtStatusToDosError;
        NtResumeProcess_f NtResumeProcess;
        NtSuspendProcess_f NtSuspendProcess;
        NtQueryObject_f NtQueryObject;
        NtCreateFile_f NtCreateFile;
        NtCancelIoFileEx_f NtCancelIoFileEx;
        NtDeviceIoControlFile_f NtDeviceIoControlFile;

        FARPROC GetAdaptersAddresses;
        FARPROC GetNetworkParams;
        if_nametoindex_f if_nametoindex;
        if_indextoname_f if_indextoname;

    } nai_wapi_t;

    extern nai_wapi_t nai_wapi;
    extern nai_int_t nai_wapi_init();

        //////////////////////////////////////////////////////////////////////////////
        //

    #ifndef _NAI_TYPEDEF_AIO_T
        #define _NAI_TYPEDEF_AIO_T
    typedef struct nai_aio_s nai_aio_t;
    #endif

    #define nai_aio_mark_event(e) ((HANDLE)((intptr_t)(e) | !!(e)))

    #define nai_aio_set_event(a)                                                                                       \
        ((a)->waitable ? ((a)->cb.hEvent = nai_win_ioevent(), (a)->cb.hEvent = nai_aio_mark_event((a)->cb.hEvent),     \
                          (a)->cb.hEvent ? 0 : -1)                                                                     \
                       : ((a)->cb.hEvent = 0, 0))

    #define nai_aio_set_offset(a, off)                                                                                 \
        {                                                                                                              \
            (a)->cb.Offset     = (DWORD)(off);                                                                         \
            (a)->cb.OffsetHigh = (DWORD)((off) >> 32);                                                                 \
        }

    /**
 * update the context of a connected socket
 * @param   a       pointer to the completed aio opeartions
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used on win32, 
 *          on other platform it is empty function.
 */
    nai_int_t nai_aio_postpro(nai_aio_t* a);

    /**
 * get thread local event for waiting overlapped io
 */
    HANDLE nai_win_ioevent();

    /**
 * convert os error code to posix error code
 */
    nai_int_t nai_errno_from_oserr(unsigned long oserrno);

    //////////////////////////////////////////////////////////////////////////////
    // wcs thread local buffer and convert

    typedef struct nai_wcs_s
    {
        wchar_t* buf;
        wchar_t* tbuf;
        size_t tbuflen;
        union
        {
            wchar_t* str;
            wchar_t* s[2];
        };
    } nai_wcs_t;

    #define nai_wcs_release(ws, r)                                                                                     \
        if ((ws)->buf) {                                                                                               \
            nai_int_t ec;                                                                                              \
            if ((r)) {                                                                                                 \
                ec = nai_errno;                                                                                        \
            };                                                                                                         \
            nai_free((ws)->buf);                                                                                       \
            if ((r)) {                                                                                                 \
                nai_errno = ec;                                                                                        \
            };                                                                                                         \
        };

    nai_int_t nai_wcs_buffer(nai_wcs_t* ws);

    nai_int_t nai_wcs_from_path(nai_wcs_t* ws, const char* path);

    nai_int_t nai_wcs_from_1str(nai_wcs_t* ws, const char* s0);

    nai_int_t nai_wcs_from_1str_ext(nai_wcs_t* ws, const char* s0, size_t extlen);

    nai_int_t nai_wcs_from_2str(nai_wcs_t* ws, const char* s0, const char* s1);

    intptr_t nai_wcs_to_utf8(char* buf, size_t buflen, const wchar_t* str, size_t len);

    intptr_t nai_wcs_from_utf8(wchar_t* buf, size_t buflen, const char* str, size_t len);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
