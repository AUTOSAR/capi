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
/// @file       nai_macos.c
/// @brief      
/// @details
/// @date       2023-10-30
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_macos.h"


#if (__darwin__)


#include "nai/runtime/nai_errno.h"


nai_mapi_t nai_mapi = { 
    0, 0, NAI_ONCE_INIT
};


static void nai_mapi_init_once()
{
    nai_int_t ec;
    nai_so_t core = 0;
    nai_so_t serv = 0;


#define nai_get_sym(so, sym, type, fail) {                  \
    if ((nai_mapi.sym =                                     \
        (type)nai_dlsym((so), #sym)) == 0) {                \
        goto fail;                                          \
    }                                                       \
}                                                           \

#define nai_get_api(so, sym, fail)                          \
    nai_get_sym(so, sym, sym##_f, fail)                     \


    core = nai_dlopen("/System/Library/Frameworks/"
        "CoreFoundation.framework/"
        "Versions/A/CoreFoundation");

    if (core == 0) {
        goto _fail;
    };

    serv = nai_dlopen("/System/Library/Frameworks/"
        "CoreServices.framework/"
        "Versions/A/CoreServices");

    if (serv == 0) {
        goto _fail;
    };


    nai_get_sym(core, kCFRunLoopDefaultMode, CFStringRef, _fail);
    nai_get_api(core, CFArrayCreate, _fail);
    nai_get_api(core, CFRelease, _fail);
    nai_get_api(core, CFRunLoopGetCurrent, _fail);
    nai_get_api(core, CFRunLoopRun, _fail);
    nai_get_api(core, CFRunLoopStop, _fail);
    nai_get_api(core, CFRunLoopWakeUp, _fail);
    nai_get_api(core, CFRunLoopAddSource, _fail);
    nai_get_api(core, CFRunLoopRemoveSource, _fail);
    nai_get_api(core, CFRunLoopSourceCreate, _fail);
    nai_get_api(core, CFRunLoopSourceSignal, _fail);
    nai_get_api(core, CFStringCreateWithFileSystemRepresentation, _fail);
    nai_get_api(serv, FSEventStreamCreate, _fail);
    nai_get_api(serv, FSEventStreamScheduleWithRunLoop, _fail);
    nai_get_api(serv, FSEventStreamStart, _fail);
    nai_get_api(serv, FSEventStreamStop, _fail);
    nai_get_api(serv, FSEventStreamInvalidate, _fail);
    nai_get_api(serv, FSEventStreamRelease, _fail);

    nai_mapi.so[0] = core;
    nai_mapi.so[1] = serv;


_end:
    nai_memory_barrier();
    nai_mapi.inited = 1;
    return;

_fail:
    ec = nai_errno;
    if (core) {
        nai_dlclose(core);
    };
    if (serv) {
        nai_dlclose(serv);
    };
    nai_errno = ec;
    nai_mapi.error = ec;
    goto _end;
};


nai_int_t nai_mapi_init()
{
    nai_int_t r;


    if (nai_mapi.inited == 0) {
        nai_once(&nai_mapi.once, nai_mapi_init_once);
    };
    if (nai_mapi.error) {
        nai_errno = nai_mapi.error;
        r = 0;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


void nai_mapi_term()
{
    nai_int_t n;
    nai_so_t so;


    for (n = 0; n < (nai_int_t)nai_countof(nai_mapi.so); n ++) {
        so = nai_mapi.so[n];
        if (so != 0) {
            nai_dlclose(so);
            nai_mapi.so[n] = 0;
        };
    };

    return;
};


#endif

