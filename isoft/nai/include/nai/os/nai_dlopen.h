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
/// @file       nai_dlopen.h
/// @brief      the dynamic link library loading and access
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details the code example is as follows:
 *
 * @par     load a dynamic link library
 * @code
 *          const char* path;
 *          nai_so_t h;
 *
 *          h = nai_dlopen(path);
 *          if (h == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     get the address of a symbol
 * @code
 *          void* sym;
 *          nai_so_t h;
 *
 *          sym = nai_dlsym(h, "nai_file_read");
 *          if (sym == 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _DL_H_NAI
#define _DL_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef NAI_TYPEDEF_SO_T
    #define NAI_TYPEDEF_SO_T
    typedef void* nai_so_t;
#endif

    /**
 * open a shared library
 * @param   name    pointer to the path of shared library
 * @return  the address of shared library is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_so_t nai_dlopen(const char* name);

    /**
 * get the symbols from a shared library
 * @param   h       pointer to the shared library
 * @param   sym     pointer to the name of symbol
 * @return  the address of symbol is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_dlsym(nai_so_t h, const char* sym);

    /**
 * close the shared library
 * @param   h       pointer to the shared library
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dlclose(nai_so_t h);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
