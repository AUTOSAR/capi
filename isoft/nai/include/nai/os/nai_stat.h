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
/// @file       nai_stat.h
/// @brief      file status query interface
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation provides a shallow encapsulation of file 
 *          status query, and solves the problem of inconsistencies in 
 *          the interface and structure of different platforms.
 *
 * @details the code example is as follows:
 *
 * @par     get file status from a path
 * @code
 *          nai_int_t r;
 *          const char* path;
 *          nai_stat_t s;
 *
 *          r = nai_stat(&s, path, NAI_STAT_BASIC|NAI_STAT_PERM);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     get file status from a opened file
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *          nai_stat_t s;
 *
 *          r = nai_file_stat(&s, fd, NAI_STAT_BASIC|NAI_STAT_PERM);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _STAT_H_NAI
#define _STAT_H_NAI

#pragma once

#include "nai/os/nai_file.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * @anchor  NAI_MODE
 * @name    NAI_MODE            the flags of mode
 * @{
 */

#if defined(_WIN32) || defined(DOXYGEN)

    #include <time.h>

    #define NAI_S_IFMT   0170000 /**< the mask of type */
    #define NAI_S_IFDIR  0040000 /**< is a directory */
    #define NAI_S_IFCHR  0020000 /**< is a char device */
    #define NAI_S_IFBLK  0060000 /**< is a block device */
    #define NAI_S_IFREG  0100000 /**< is a file */
    #define NAI_S_IFIFO  0010000 /**< is a fifo */
    #define NAI_S_IFLNK  0120000 /**< is a symlink */
    #define NAI_S_IFSOCK 0140000 /**< is a socket */

    #define NAI_S_ISUID 0004000 /**< set uid on execute */
    #define NAI_S_ISGID 0002000 /**< set gid on execute */
    #define NAI_S_IRUSR 0000400 /**< read permission of user */
    #define NAI_S_IWUSR 0000200 /**< write permission of user */
    #define NAI_S_IXUSR 0000100 /**< exec permission of user */
    #define NAI_S_IRWXU 0000700 /**< all permission of ussr */
    #define NAI_S_IRGRP 0000040 /**< read permission of group */
    #define NAI_S_IWGRP 0000020 /**< write permission of group */
    #define NAI_S_IXGRP 0000010 /**< exec permission of group */
    #define NAI_S_IRWXG 0000070 /**< all permission of group */
    #define NAI_S_IROTH 0000004 /**< read permission of eveyone */
    #define NAI_S_IWOTH 0000002 /**< write permission of eveyone */
    #define NAI_S_IXOTH 0000001 /**< exec permission of eveyone */
    #define NAI_S_IRWXO 0000007 /**< all permission of eveyone */

#else

    #include <sys/stat.h>

    #define NAI_S_IFMT   S_IFMT
    #define NAI_S_IFDIR  S_IFDIR
    #define NAI_S_IFCHR  S_IFCHR
    #define NAI_S_IFBLK  S_IFBLK
    #define NAI_S_IFREG  S_IFREG
    #define NAI_S_IFIFO  S_IFIFO
    #define NAI_S_IFLNK  S_IFLNK
    #define NAI_S_IFSOCK S_IFSOCK

    #define NAI_S_ISUID S_ISUID
    #define NAI_S_ISGID S_ISGID
    #define NAI_S_IRUSR S_IRUSR
    #define NAI_S_IWUSR S_IWUSR
    #define NAI_S_IXUSR S_IXUSR
    #define NAI_S_IRWXU S_IRWXU
    #define NAI_S_IRGRP S_IRGRP
    #define NAI_S_IWGRP S_IWGRP
    #define NAI_S_IXGRP S_IXGRP
    #define NAI_S_IRWXG S_IRWXG
    #define NAI_S_IROTH S_IROTH
    #define NAI_S_IWOTH S_IWOTH
    #define NAI_S_IXOTH S_IXOTH
    #define NAI_S_IRWXO S_IRWXO

#endif

    /** @} */

#ifndef _NAI_TYPEDEF_STAT_T
    #define _NAI_TYPEDEF_STAT_T
    typedef struct nai_stat_s nai_stat_t;
#endif

    /**
 * the structure of the file stat
 */
    struct nai_stat_s
    {
        uint64_t st_dev;         /**< ID of device containing file */
        uint64_t st_rdev;        /**< device ID (if special file) */
        uint64_t st_ino;         /**< inode number */
        uint32_t st_nlink;       /**< number of hard links */
        uint32_t st_mode;        /**< file type and mode, see @ref NAI_MODE */
        uint32_t st_uid;         /**< user ID of owner */
        uint32_t st_gid;         /**< group ID of owner */
        uint64_t st_size;        /**< total size, in bytes */
        uint32_t st_blksize;     /**< block size for filesystem I/O */
        uint64_t st_blocks;      /**< number of 512B blocks allocated */
        struct timespec st_atim; /**< time of last access */
        struct timespec st_mtim; /**< time of last modification */
        struct timespec st_ctim; /**< time of last status change */
    };

/**
 * @anchor  NAI_STAT
 * @name    NAI_STAT            the flags of stat
 * @{
 */
#define NAI_STAT_BASIC   0x00 /**< get the basic information */
#define NAI_STAT_PERM    0x01 /**< get the permission information */
#define NAI_STAT_SYMLINK 0x02 /**< get the symbolic link itself */
    /** @} */

    /**
 * get file status of the specified path
 * @param   path    pointer to the specified path
 * @param   st      pointer to the structure of stat
 * @param   flags   the flags of stat, see @ref NAI_STAT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_stat(const char* path, nai_stat_t* st, nai_int_t flags);

#if (NAI_HAVE_FSTATAT)

    /**
 * get file status of the relatively path under the specified directory
 * @param   fd      the specified directory of file descriptor
 * @param   path    the path relative to the specified directory
 * @param   st      pointer to the structure of stat
 * @param   flags   the flags of stat, see @ref NAI_STAT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_stat_at(nai_fd_t fd, const char* path, nai_stat_t* st, nai_int_t flags);

#endif

    /**
 * get file status of the specified file descriptor
 * @param   fd      the specified file descriptor
 * @param   st      pointer to the structure of stat
 * @param   flags   the flags of stat, see @ref NAI_STAT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_stat(nai_fd_t fd, nai_stat_t* st, nai_int_t flags);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
