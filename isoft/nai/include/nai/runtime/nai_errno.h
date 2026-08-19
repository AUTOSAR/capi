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
/// @file       nai_errno.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _ERRNO_H_NAI
#define _ERRNO_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <errno.h>

#if defined(NAI_STATIC) || !defined(_WIN32)

    #define nai_errno errno

#else

    #if defined(NAI_DLL)
        #define nai_errno errno
    #else
        #define nai_errno *__nai_errno()
    #endif

NAI_EXTERN
int* __nai_errno();

#endif

#if defined(__freebsd__)
    #if !defined(ENODATA)
        #define ENODATA ENOATTR
    #endif
#endif

    //////////////////////////////////////////////////////////////////////////////
    // error code

    /**
 * @anchor  NAI_ERROR
 * @name    NAI_ERROR           the error codes
 * @{
 */

#define NAI_EAGAIN      EAGAIN
#define NAI_EINPROGRESS EINPROGRESS
#define NAI_ETIMEDOUT   ETIMEDOUT

#define NAI_ECUSTOM_START      50000
#define NAI_ECUSTOM(x)         (NAI_ECUSTOM_START + (x))
#define NAI_EFAILED            NAI_ECUSTOM(1)
#define NAI_ETYPE              NAI_ECUSTOM(4)
#define NAI_EOPERATION         NAI_ECUSTOM(5)
#define NAI_ESETTING           NAI_ECUSTOM(6)
#define NAI_ECLOSED            NAI_ECUSTOM(10)
#define NAI_EINTERNAL          NAI_ECUSTOM(11)
#define NAI_ECORRUPTION        NAI_ECUSTOM(14)
#define NAI_ESINGLETON         NAI_ECUSTOM(17)
#define NAI_ENODRI             NAI_ECUSTOM(31)
#define NAI_EDEVICE_FUNCTION   NAI_ECUSTOM(32)
#define NAI_EDRIVER_INTERNAL   NAI_ECUSTOM(33)
#define NAI_EDRIVER_INITIALIZE NAI_ECUSTOM(34)
#define NAI_EARCHIVE           NAI_ECUSTOM(41)
#define NAI_EVERSION           NAI_ECUSTOM(42)

#if defined(ESHUTDOWN)
    #define NAI_ESHUTDOWN ESHUTDOWN
#else
    #define NAI_ESHUTDOWN NAI_ECUSTOM(101)
#endif

    /** @} */

    //////////////////////////////////////////////////////////////////////////////
    // result code

#define nai_errno_to_sult(e) (-(e))

#define nai_sult_to_errno(s)  ((int)-(s))
#define nai_sult_from_errno() (-nai_errno)

/**
 * @anchor  NAI_SULT
 * @name    NAI_SULT            the result codes
 * @{
 */

/* success sult code */
#define NAI_OK       0
#define NAI_DECLINED 1
#define NAI_DONE     2

/* error sult code */
#define NAI_E_OUT_OF_MEMORY   -ENOMEM
#define NAI_E_TOO_MANY_OBJECT -EMFILE
#define NAI_E_INVALID_ARG     -EINVAL
#define NAI_E_DENY_ACCESS     -EACCES
#define NAI_E_NOT_SUPPORTED   -ENOTSUP
#define NAI_E_NOT_EXISIS      -ENOENT
#define NAI_E_NOT_ENOUGH      -ERANGE
#define NAI_E_TOO_LARGE       -EOVERFLOW
#define NAI_E_PENDING         -EINPROGRESS
#define NAI_E_INPROGRESS      -EINPROGRESS
#define NAI_E_FILE_ERROR      -EIO
#define NAI_E_FILE_NOENT      -ENOENT
#define NAI_E_FILE_EXIST      -EEXIST
#define NAI_E_FILE_ACCESS     -EACCES
#define NAI_E_FILE_NOSPACE    -ENOSPC
#define NAI_E_INVALID_DEVICE  -ENODEV

#define NAI_E_AGAIN -EAGAIN
    //#define NAI_E_WOULDBLOCK        -EWOULDBLOCK

#define NAI_E_FAILED            -NAI_EFAILED
#define NAI_E_TYPE              -NAI_ETYPE
#define NAI_E_OPERATION         -NAI_EOPERATION
#define NAI_E_SETTING           -NAI_ESETTING
#define NAI_E_CLOSED            -NAI_ECLOSED
#define NAI_E_INTERNAL          -NAI_EINTERNAL
#define NAI_E_CORRUPTION        -NAI_ECORRUPTION
#define NAI_E_SINGLETON         -NAI_ESINGLETON
#define NAI_E_INVALID_DRIVER    -NAI_ENODRI
#define NAI_E_DEVICE_FUNCTION   -NAI_EDEVICE_FUNCTION
#define NAI_E_DRIVER_INTERNAL   -NAI_EDRIVER_INTERNAL
#define NAI_E_DRIVER_INITIALIZE -NAI_EDRIVER_INITIALIZE
#define NAI_E_ARCHIVE           -NAI_EARCHIVE
#define NAI_E_VERSION           -NAI_EVERSION

    /** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
