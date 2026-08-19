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
/// @file       nai_macos.h
/// @brief      
/// @details
/// @date       2023-10-30
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MACOS_H_NAI
#define _MACOS_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if (__darwin__)

    #include "nai/os/nai_dlopen.h"
    #include "nai/os/nai_thread.h"

    typedef double CFAbsoluteTime;
    typedef double CFTimeInterval;
    typedef int FSEventStreamEventFlags;
    typedef int OSStatus;
    typedef long CFIndex;
    typedef struct CFArrayCallBacks CFArrayCallBacks;
    typedef struct CFRunLoopSourceContext CFRunLoopSourceContext;
    typedef struct FSEventStreamContext FSEventStreamContext;
    typedef uint32_t FSEventStreamCreateFlags;
    typedef uint64_t FSEventStreamEventId;
    typedef unsigned CFStringEncoding;
    typedef void* CFAllocatorRef;
    typedef void* CFArrayRef;
    typedef void* CFBundleRef;
    typedef void* CFDictionaryRef;
    typedef void* CFRunLoopRef;
    typedef void* CFRunLoopSourceRef;
    typedef void* CFStringRef;
    typedef void* CFTypeRef;
    typedef void* FSEventStreamRef;

    struct CFArrayCallBacks;
    struct CFRunLoopSourceContext;
    struct FSEventStreamContext;

    struct CFRunLoopSourceContext
    {
        CFIndex version;
        void* info;
        void* pad[7];
        void (*perform)(void*);
    };

    struct FSEventStreamContext
    {
        CFIndex version;
        void* info;
        void* pad[3];
    };

    typedef CFArrayRef (*CFArrayCreate_f)(CFAllocatorRef, const void**, CFIndex, const CFArrayCallBacks*);

    typedef void (*CFRelease_f)(CFTypeRef);

    typedef CFStringRef(*kCFRunLoopDefaultMode);
    typedef CFRunLoopRef (*CFRunLoopGetCurrent_f)(void);
    typedef void (*CFRunLoopRun_f)(void);
    typedef void (*CFRunLoopStop_f)(CFRunLoopRef);
    typedef void (*CFRunLoopWakeUp_f)(CFRunLoopRef);
    typedef void (*CFRunLoopAddSource_f)(CFRunLoopRef, CFRunLoopSourceRef, CFStringRef);
    typedef void (*CFRunLoopRemoveSource_f)(CFRunLoopRef, CFRunLoopSourceRef, CFStringRef);

    typedef CFRunLoopSourceRef (*CFRunLoopSourceCreate_f)(CFAllocatorRef, CFIndex, CFRunLoopSourceContext*);
    typedef void (*CFRunLoopSourceSignal_f)(CFRunLoopSourceRef);

    typedef void (*FSEventStreamCallback)(
        const FSEventStreamRef, void*, size_t, void*, const FSEventStreamEventFlags*, const FSEventStreamEventId*);
    typedef CFStringRef (*CFStringCreateWithFileSystemRepresentation_f)(CFAllocatorRef, const char*);
    typedef FSEventStreamRef (*FSEventStreamCreate_f)(CFAllocatorRef,
                                                      FSEventStreamCallback,
                                                      FSEventStreamContext*,
                                                      CFArrayRef,
                                                      FSEventStreamEventId,
                                                      CFTimeInterval,
                                                      FSEventStreamCreateFlags);
    typedef void (*FSEventStreamScheduleWithRunLoop_f)(FSEventStreamRef, CFRunLoopRef, CFStringRef);
    typedef int (*FSEventStreamStart_f)(FSEventStreamRef);
    typedef void (*FSEventStreamStop_f)(FSEventStreamRef);
    typedef void (*FSEventStreamInvalidate_f)(FSEventStreamRef);
    typedef void (*FSEventStreamRelease_f)(FSEventStreamRef);

    typedef struct nai_mapi_s
    {
        nai_int_t inited;
        nai_int_t error;
        nai_once_t once;
        nai_so_t so[2];

        CFStringRef* kCFRunLoopDefaultMode;

        CFArrayCreate_f CFArrayCreate;
        CFRelease_f CFRelease;
        CFRunLoopGetCurrent_f CFRunLoopGetCurrent;
        CFRunLoopRun_f CFRunLoopRun;
        CFRunLoopStop_f CFRunLoopStop;
        CFRunLoopWakeUp_f CFRunLoopWakeUp;
        CFRunLoopAddSource_f CFRunLoopAddSource;
        CFRunLoopRemoveSource_f CFRunLoopRemoveSource;
        CFRunLoopSourceCreate_f CFRunLoopSourceCreate;
        CFRunLoopSourceSignal_f CFRunLoopSourceSignal;
        CFStringCreateWithFileSystemRepresentation_f CFStringCreateWithFileSystemRepresentation;

        FSEventStreamCreate_f FSEventStreamCreate;
        FSEventStreamScheduleWithRunLoop_f FSEventStreamScheduleWithRunLoop;
        FSEventStreamStart_f FSEventStreamStart;
        FSEventStreamStop_f FSEventStreamStop;
        FSEventStreamInvalidate_f FSEventStreamInvalidate;
        FSEventStreamRelease_f FSEventStreamRelease;

    } nai_mapi_t;

    extern nai_mapi_t nai_mapi;
    extern nai_int_t nai_mapi_init();

        /*
static const CFStringEncoding kCFStringEncodingUTF8 = 0x8000100;
static const OSStatus noErr = 0;

static const FSEventStreamEventId kFSEventStreamEventIdSinceNow = -1;

static const int kFSEventStreamCreateFlagNoDefer = 2;
static const int kFSEventStreamCreateFlagFileEvents = 16;

static const int kFSEventStreamEventFlagEventIdsWrapped = 8;
static const int kFSEventStreamEventFlagHistoryDone = 16;
static const int kFSEventStreamEventFlagItemChangeOwner = 0x4000;
static const int kFSEventStreamEventFlagItemCreated = 0x100;
static const int kFSEventStreamEventFlagItemFinderInfoMod = 0x2000;
static const int kFSEventStreamEventFlagItemInodeMetaMod = 0x400;
static const int kFSEventStreamEventFlagItemIsDir = 0x20000;
static const int kFSEventStreamEventFlagItemModified = 0x1000;
static const int kFSEventStreamEventFlagItemRemoved = 0x200;
static const int kFSEventStreamEventFlagItemRenamed = 0x800;
static const int kFSEventStreamEventFlagItemXattrMod = 0x8000;
static const int kFSEventStreamEventFlagKernelDropped = 4;
static const int kFSEventStreamEventFlagMount = 64;
static const int kFSEventStreamEventFlagRootChanged = 32;
static const int kFSEventStreamEventFlagUnmount = 128;
static const int kFSEventStreamEventFlagUserDropped = 2;
*/

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
