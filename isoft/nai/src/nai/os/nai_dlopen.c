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
/// @file       nai_dlopen.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_dlopen.h"


#if defined(_WIN32)


#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_file.h"
#include "win/nai_windows.h"
#include <libloaderapi.h>


nai_so_t nai_dlopen(const char* name)
{
    nai_int_t ec;
    nai_wcs_t ws;
    HMODULE h;


    if (nai_wcs_from_path(&ws, name) < 0) {
        ec = nai_errno;
        h = 0;
        goto _fail;
    };

    h = LoadLibraryW(ws.str);
    if (h == 0) {
        ec = nai_errno_from_oserr(GetLastError());
    };

_fail:
    nai_wcs_release(&ws, 0);

    if (h == 0) {
        nai_errno = ec;
    };
    return h;
};


void* nai_dlsym(nai_so_t h, const char* sym)
{
    void* s;

    s = GetProcAddress((HMODULE)h, sym);
    if (s == 0) {
        nai_errno = nai_errno_from_oserr(GetLastError());
    };

    return s;
};


nai_int_t nai_dlclose(nai_so_t h)
{
    if (h) {
        FreeLibrary((HMODULE)h);
    };
    return 0;
};


#elif (NAI_HAVE_DLFCN_H)


#include <dlfcn.h>


nai_so_t nai_dlopen(const char* name)
{
    return dlopen(name, RTLD_NOW);
};


void* nai_dlsym(nai_so_t h, const char* sym)
{
    return dlsym(h, sym);
};


nai_int_t nai_dlclose(nai_so_t h)
{
    if (h) {
        dlclose(h);
    };
    return 0;
};


#else

#error "nai_dlopen is not implemented, unsupported platform!"

#endif

