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
/// @file       nai_config.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CONFIG_H_NAI
#define _CONFIG_H_NAI

#if defined(__LP64__) || defined(_WIN64)
    #define _NAI64 1
#else
    #define _NAI32 1
#endif

#if defined(_NAI64)
    #include "config/nai_config.64.h"
#else
    #include "config/nai_config.32.h"
#endif

#if defined(_WIN32)
    #include "config/nai_config.win.h"
#endif

#if defined(_MSC_VER)

    #if defined(NAI_DLL)
        #define NAI_EXTERN __declspec(dllexport)
    #elif !defined(NAI_STATIC)
        #define NAI_EXTERN __declspec(dllimport)
    #else
        #define NAI_EXTERN /* nothing */
    #endif

#elif __GNUC__ >= 4

    #if defined(NAI_DLL)
        #define NAI_EXTERN __attribute__((visibility("default")))
    #else
        #define NAI_EXTERN /* nothing */
    #endif

#else
    #define NAI_EXTERN /* nothing */
#endif

#if defined(__GNUC__)
    #define NAI_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define NAI_DEPRECATED __declspec(deprecated)
#else
    #define NAI_DEPRECATED
#endif

#if defined(__GNUC__)
    #define nai_fmt_attribute(x, y) __attribute__((format(printf, x, y)))
#else
    #define nai_fmt_attribute(x, y)
#endif

#endif
