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
/// @file       nai_dirent.h
/// @brief      the directory of the filesystem access
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is to read, create and delete directories.
 *
 * @details the code example is as follows:
 *
 * @par     read the content of the directory
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          const char* path;
 *          nai_dir_t d;
 *
 *          nai_dir_init(&d);
 *
 *          r = nai_dir_open(&d, path);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          for (;;) {
 *              r = nai_dir_read(&d);
 *              if (r < 0) {
 *                  ec = nai_errno;
 *                  if (ec != ENOENT) {
 *                      goto _fail;
 *                  };
 *
 *                  break;
 *              };
 *
 *              printf("%.*s\n", d.name);
 *          };
 *
 *          // finish
 *          nai_dir_close(&d);
 *
 * @endcode
 *
 * @par     create a new directory
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          const char* path;
 *
 *          r = nai_dir_create(path, 0755, 0);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != EEXIST) {
 *                  goto _fail;
 *              };
 *          };
 * @endcode
 */

#ifndef _DIRENT_H_NAI
#define _DIRENT_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_DIR_T
    #define _NAI_TYPEDEF_DIR_T
    typedef struct nai_dir_s nai_dir_t;
#endif

    /**
 * the structure of the directory
 */
    struct nai_dir_s
    {
        void* dir;        /**< the handle of the directory */
        const char* name; /**< file name currently enumerated */
        nai_int_t type;   /**< file type currently enumerated, 
                                     see @ref NAI_MODE */
    };

/**
 * @name    nai_dir_defines
 * @{
 */

/**
 * initial the structure of the directory
 * @param   d       pointer to the directory to be initialized
 * @return  void
 */
#define nai_dir_init(d)                                                                                                \
    {                                                                                                                  \
        (d)->dir  = 0;                                                                                                 \
        (d)->name = 0;                                                                                                 \
        (d)->type = 0;                                                                                                 \
    }

    /** @} */

    /**
 * open the specified directory
 * @param   d       pointer to the directory
 * @param   path    pointer to the specified path
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dir_open(nai_dir_t* d, const char* path);

    /**
 * read the next directory entry
 * @param   d       pointer to the directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    you can get file name and type 
 *          from the structure of the directory after read success.
 */
    NAI_EXTERN
    nai_int_t nai_dir_read(nai_dir_t* d);

    /**
 * close the directory
 * @param   d       pointer to the directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dir_close(nai_dir_t* d);

    /**
 * create a directory on the specified path
 * @param   path    pointer to the specified path
 * @param   mode    the initial permissions, ie. 0755, 0644
 * @param   recursive automatically create parent directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dir_create(const char* path, nai_int_t mode, nai_int_t recursive);

    /**
 * remove a empty directory
 * @param   path    pointer to the specified path
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when the directory is not empty, the deletion will fail.
 */
    NAI_EXTERN
    nai_int_t nai_dir_remove(const char* path);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
