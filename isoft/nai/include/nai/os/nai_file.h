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
/// @file       nai_file.h
/// @brief      the posix-like file api
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details there are a few differences in file-related interfaces under 
 *          different systems, mainly in three points.
 *
 * @details some systems do not support reading and writing on multiple 
 *          buffers and need to be implemented in an analog way. firstly, 
 *          performance will be lost in scattered small buffers, 
 *          and secondly, the behavior on message-type devices is wrong.
 *
 * @details on some systems, the two subsystems of file and socket are 
 *          not compatible, so that the file interface should not be used to 
 *          operate sockets on these systems.
 *
 * @details on some systems, blocking mode and asynchronous mode cannot 
 *          be mixed, see nai_aio.h for details.
 *
 * @details the code example is as follows:
 *
 * @par     open a read only file
 * @code
 *          nai_int_t r;
 *          const char* path;
 *          nai_fd_t fd;
 *
 *          fd = nai_file_open(path, NAI_O_RDONLY, 0);
 *          if (fd == NAI_FD_INVALID) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     set the file to be inherited by the child process
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *
 *          r = nai_file_set_cloexec(fd, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     set the file to non-blocking mode
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *
 *          r = nai_file_set_blocking(fd, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     lock and unlock file
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_fd_t fd;
 *
 *          r = nai_file_lock(fd, NAI_FLOCK_TRY);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != EACCES && ec != EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              // locked by others
 *              goto _wait;
 *          };
 *
 *          // process
 *          ...
 *
 *          r = nai_file_unlock(fd);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     get the real path
 * @code
 *          nai_int_t r;
 *          nai_int_t alloc = 0;
 *          char* buf = 0;
 *          const char* path;
 *
 *          for (;;) {
 *              r = nai_path_get_real(buf, alloc, path);
 *              if (r < 0) {
 *                  goto _fail;
 *              };
 *
 *              if (r + 1 <= alloc) {
 *                  break;
 *              };
 *
 *              if (buf) {
 *                  nai_free(buf);
 *              };
 *
 *              alloc = r + 1;
 *              buf = (char*)nai_malloc(alloc);
 *              if (buf == 0) {
 *                  goto _fail;
 *              };
 *          };
 * @endcode
 *
 * @par     normalized processing file path
 * @code
 *          nai_int_t r;
 *          nai_int_t len;
 *          nai_int_t alloc = 0;
 *          char* buf = 0;
 *          const char* path;
 *
 *          len = nai_strlen(path);
 *          for (;;) {
 *              r = nai_path_canonicalize(
 *                  buf, alloc, path, len, nai_path_sep);
 *              if (r < 0) {
 *                  goto _fail;
 *              };
 *
 *              if (r + 1 <= alloc) {
 *                  break;
 *              };
 *
 *              if (buf) {
 *                  nai_free(buf);
 *              };
 *
 *              alloc = r + 1;
 *              buf = (char*)nai_malloc(alloc);
 *              if (buf == 0) {
 *                  goto _fail;
 *              };
 *          };
 * @endcode
 */

#ifndef _FILE_H_NAI
#define _FILE_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // file descriptor

#ifndef _NAI_DEFINED_FD_T
    #define _NAI_DEFINED_FD_T
    #if defined(_WIN32)
    typedef void* HANDLE;
    typedef HANDLE nai_fd_t;
    #else
    typedef int nai_fd_t;
    #endif
#endif

#ifndef NAI_FD_INVALID
    #define NAI_FD_INVALID ((nai_fd_t)-1)
#endif

/**
 * @anchor  NAI_FD_TYPE
 * @name    NAI_FD_TYPE         the type of file descriptor
 * @{
 */
#define NAI_FD_TYPE_NONE 0 /**< invalid type */
#define NAI_FD_TYPE_FILE 1 /**< a regular file */
#define NAI_FD_TYPE_PIPE 2 /**< a pipe */
#define NAI_FD_TYPE_DEVC 3 /**< a device or a mqueue or others */
#define NAI_FD_TYPE_SOCK 4 /**< a socket */
    /** @} */

    /**
 * dupilcate the specified file descriptor
 * @param   fd      the old file descriptor to dupilcate
 * @param   flags   the flags of dupilcate, see #NAI_O_NOCLOEXEC
 * @return  the new file descriptor is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_file_dup(nai_fd_t fd, nai_int_t flags);

    /**
 * get the file descriptor of specified stdio
 * @param   n       the no of stdio, ie. 0, 1, 2
 * @return  the file descriptor of stdio is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_get_stdio(nai_int_t n);

    /**
 * set the specified file descriptor to stdio
 * @param   n       the no of stdio, ie 0, 1, 2
 * @param   fd      the file descriptor to be setted
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    set the value of fd to NAI_FD_INVALID can close stdio
 */
    NAI_EXTERN
    nai_int_t nai_set_stdio(nai_int_t n, nai_fd_t fd);

    //////////////////////////////////////////////////////////////////////////////
    // file

#ifndef _NAI_DEFINED_BUFVEC_T
    #define _NAI_DEFINED_BUFVEC_T
    #ifndef _NAI_TYPEDEF_BUFVEC_T
        #define _NAI_TYPEDEF_BUFVEC_T
    typedef struct nai_bufvec_s nai_bufvec_t;
    #endif
    #ifndef _NAI_TYPEDEF_BUFARRAY_T
        #define _NAI_TYPEDEF_BUFARRAY_T
    typedef struct nai_bufarray_s nai_bufarray_t;
    #endif

    /**
 * the structure of the buffer vector
 */
    #if defined(_WIN32)
    struct nai_bufvec_s
    {
        size_t len;
        uint8_t* buf;
    };
    #else
    struct nai_bufvec_s
    {
        uint8_t* buf;
        size_t len;
    };
    #endif

#endif

#if (NAI_HAVE_LIMITS_H)
    #include <limits.h>
#endif
#ifndef IOV_MAX
    #define IOV_MAX 64
#endif

/**
 * the max count of the buffer vector
 */
#if (IOV_MAX > 64)
    #define NAI_BUFV_MAX 64
#else
    #define NAI_BUFV_MAX IOV_MAX
#endif

    /**
 * the structure of the bffer array
 */
    struct nai_bufarray_s
    {
        nai_bufvec_t* v; /**< pointer to the buffer vectors */
        nai_int_t count; /**< the count of the vectors */
    };

/**
 * the max count of the buffer array
 */
#define NAI_BUFA_MAX 16

/**
 * @anchor  NAI_FOPEN
 * @name    NAI_FOPEN           the flags of file open
 * @{
 */
#if (NAI_HAVE_FCNTL_H) && !(defined(_WIN32) || defined(DOXYGEN))

    #include <fcntl.h>
    #define NAI_O_RDONLY    O_RDONLY
    #define NAI_O_WRONLY    O_WRONLY
    #define NAI_O_RDWR      O_RDWR
    #define NAI_O_CREAT     O_CREAT
    #define NAI_O_TRUNC     O_TRUNC
    #define NAI_O_EXCL      O_EXCL
    #define NAI_O_APPEND    O_APPEND
    #define NAI_O_NONBLOCK  O_NONBLOCK
    #define NAI_O_LARGEFILE O_LARGEFILE

    #if defined(O_DIRECT)
        #define NAI_O_DIRECT O_DIRECT
    #else
        #define NAI_O_DIRECT 0
    #endif
    #if defined(O_DIRECTORY)
        #define NAI_O_DIRECTORY O_DIRECTORY
    #else
        #define NAI_O_DIRECTORY 0
    #endif

#else

    #define NAI_O_RDONLY    0x0000  /**< open for reading only */
    #define NAI_O_WRONLY    0x0001  /**< open for writing only */
    #define NAI_O_RDWR      0x0002  /**< open for reading and writing */
    #define NAI_O_CREAT     0x0010  /**< create and open file */
    #define NAI_O_TRUNC     0x0020  /**< open and truncate */
    #define NAI_O_EXCL      0x0040  /**< open only if file doesn't exist */
    #define NAI_O_APPEND    0x0100  /**< open with append mode */
    #define NAI_O_NONBLOCK  0x0800  /**< to enable non-blocking */
    #define NAI_O_LARGEFILE 0x8000  /**< to enable largefile supported */
    #define NAI_O_DIRECT    0x4000  /**< to enable direct io */
    #define NAI_O_DIRECTORY 0x10000 /**< prevent open non-directories */

#endif

/**
 * the flag for preevent reparse symlink, 
 * if path is a symlink then the open fails with errno 'ELOOP'
 */
#if defined(O_NOFOLLOW)
    #define NAI_O_NOFOLLOW O_NOFOLLOW
#else
    #define NAI_O_NOFOLLOW 0x20000
#endif

/**
 * the flag for open the file reference only
 */
#if defined(O_PATH)
    #define NAI_O_PATH O_PATH
#else
    #define NAI_O_PATH 0x200000
#endif

/**
 * the flag for create a temprary file
 */
#if defined(O_TMPFILE)
    #define NAI_O_TMPFILE O_TMPFILE
#else
    #define NAI_O_TMPFILE 0x400000
#endif

/**
 * the flag for open symlink itself
 */
#if defined(O_SYMLINK)
    #define NAI_O_SYMLINK O_SYMLINK
#else
    #define NAI_O_SYMLINK (NAI_O_PATH | NAI_O_NOFOLLOW)
#endif

/**
 * the flag for fast open a lookup file descriptor
 */
#if defined(O_SEARCH)
    #define NAI_O_SEARCH O_SEARCH
#elif defined(O_EXEC)
    #define NAI_O_SEARCH O_EXEC
#else
    #define NAI_O_SEARCH 0x1000000
#endif

/**
 * open for executable
 */
#define NAI_O_EXECUTE 0x02000000

/**
 * the flag for disable close on exec, 
 */
#define NAI_O_NOCLOEXEC 0x04000000

/**
 * the flag for open asynchronous file, 
 * must use nai_aio_read or nai_aio_write on asynchronous file or pipe
 */
#define NAI_O_ASYNCIO 0x08000000
/** @} */

/**
 * @anchor  NAI_FLOCK
 * @name    NAI_FLOCK           the flags of file lock
 * @{
 */
#define NAI_FLOCK_READ  0x01 /**< a read (or shared) lock */
#define NAI_FLOCK_WRITE 0x00 /**< a write (or exclusive) lock */
#define NAI_FLOCK_TRY   0x02 /**< try to lock file */
/** @} */

/**
 * @anchor  NAI_PROTECT
 * @name    NAI_PROTECT         options of protect
 * @{
 */
#define NAI_PROT_NONE    0x00 /**< used to reserved address */
#define NAI_PROT_READ    0x01 /**< readable permission */
#define NAI_PROT_WRITE   0x02 /**< writable permission */
#define NAI_PROT_EXECUTE 0x04 /**< executable permission */
/** @} */

/**
 * @anchor  NAI_POLL
 * @name    NAI_POLL            the flags of poll
 * @{
 */
#define NAI_POLL_READ      0x01 /**< wait for readable */
#define NAI_POLL_WRITE     0x02 /**< wait for writable */
#define NAI_POLL_READWRITE 0x03 /**< wait for readable or writable */
/** @} */

/**
 * @anchor  NAI_SEEK
 * @name    NAI_SEEK            the flags of seek
 * @{
 */
#define NAI_SEEK_SET 0 /**< set the file cursor */
#define NAI_SEEK_CUR 1 /**< current */
#define NAI_SEEK_END 2 /**< go to end of file */
    /** @} */

#ifndef _NAI_TYPEDEF_IO_FEATURE_T
    #define _NAI_TYPEDEF_IO_FEATURE_T
    typedef struct nai_io_feature_s nai_io_feature_t;
#endif

    /**
 * the structure of io features
 * @note    the simulation version only guarantees the basic behavior, 
 *          some advanced features are not supported, 
 *          and the performance is relatively poor.
 */
    struct nai_io_feature_s
    {
        uint32_t directio : 1;      /**< native support for direct io */
        uint32_t readv : 1;         /**< native support for readv */
        uint32_t readv_sim : 1;     /**< simulation support for readv */
        uint32_t writev : 1;        /**< native support for writev */
        uint32_t writev_sim : 1;    /**< simulation support for writev */
        uint32_t preadv : 1;        /**< native support for preadv */
        uint32_t preadv_sim : 1;    /**< simulation support for preadv */
        uint32_t pwritev : 1;       /**< native support for pwritev */
        uint32_t pwritev_sim : 1;   /**< simulation support for pwritev */
        uint32_t recvv : 1;         /**< native support for recvv */
        uint32_t recvv_sim : 1;     /**< simulation support for recvv */
        uint32_t recvmsg : 1;       /**< native support for recvmsg */
        uint32_t recvmsg_sim : 1;   /**< simulation support for recvmsg */
        uint32_t recvmmsg : 1;      /**< native support for recvmmsg */
        uint32_t recvmmsg_sim : 1;  /**< simulation support for recvmmsg */
        uint32_t sendv : 1;         /**< native support for sendv */
        uint32_t sendv_sim : 1;     /**< simulation support for sendv */
        uint32_t sendmsg : 1;       /**< native support for sendmsg */
        uint32_t sendmsg_sim : 1;   /**< simulation support for sendmsg */
        uint32_t sendmmsg : 1;      /**< native support for sendmmsg */
        uint32_t sendmmsg_sim : 1;  /**< simulation support for sendmmsg */
        uint32_t sendfile : 1;      /**< native support for sendfile */
        uint32_t sendfile_sim : 1;  /**< simulation support for sendfile */
        uint32_t sendfilev : 1;     /**< native support for sendfilev */
        uint32_t sendfilev_sim : 1; /**< simulation support for sendfilev */
    };

    NAI_EXTERN
    extern nai_io_feature_t nai_iofeat;

    /**
 * open a file which specified by path
 * @param   path    pointer to the string of path
 * @param   flags   the flags of open file, see @ref NAI_FOPEN
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @return  the file descriptor is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_file_open(const char* path, nai_int_t flags, ... /* nai_int_t mode */);

#if (NAI_HAVE_OPENAT)

    #ifndef NAI_FD_CWD
        #define NAI_FD_CWD AT_FDCWD
    #endif

    /**
 * open a file under the specified directory
 * @param   fd      the specified directory of file descriptor
 * @param   path    the path relative to the specified directory
 * @param   flags   the flags of open file, see @ref NAI_FOPEN
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @return  the file descriptor is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_file_openat(nai_fd_t fd, const char* path, nai_int_t flags, ... /* nai_int_t mode */);

#endif

    /**
 * read data from the specified file
 * @param   fd      the file descriptor
 * @param   lp      pointer to the buffer to receive data
 * @param   len     the length of buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened with #NAI_O_ASYNCIO, 
 *          this function will fail directly.
 */
    NAI_EXTERN
    intptr_t nai_file_read(nai_fd_t fd, void* lp, size_t len);

    /**
 * read data from the specified file into multiple buffers
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer array
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened with #NAI_O_ASYNCIO, 
 *          this function will fail directly.
 */
    NAI_EXTERN
    intptr_t nai_file_readv(nai_fd_t fd, nai_bufvec_t* v, size_t count);

    /**
 * write data to the specified file
 * @param   fd      the file descriptor
 * @param   lp      pointer to the buffer to write
 * @param   len     the length of buffer
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened with #NAI_O_ASYNCIO, 
 *          this function will fail directly.
 */
    NAI_EXTERN
    intptr_t nai_file_write(nai_fd_t fd, const void* lp, size_t len);

    /**
 * write data to the specified file from multiple buffers
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer array
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened with #NAI_O_ASYNCIO, 
 *          this function will fail directly.
 */
    NAI_EXTERN
    intptr_t nai_file_writev(nai_fd_t fd, const nai_bufvec_t* v, size_t count);

    /**
 * read data from the specified offset of the file
 * @param   fd      the file descriptor
 * @param   lp      pointer to the buffer to receive data
 * @param   len     the length of buffer
 * @param   offset  the specified offset of the file
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened without NAI_O_ASYNC, 
 *          this function will update the file cursor on win32, 
 *          we assume that read and pread will not be mixed, 
 *          so no fixes will be made
 */
    NAI_EXTERN
    intptr_t nai_file_pread(nai_fd_t fd, void* lp, size_t len, nai_off64_t offset);

    /**
 * read data from the specified offset of the file into multiple buffers
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer array
 * @param   offset  the specified offset of the file
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened without NAI_O_ASYNC, 
 *          this function will update the file cursor on win32, 
 *          we assume that read and pread will not be mixed, 
 *          so no fixes will be made
 */
    NAI_EXTERN
    intptr_t nai_file_preadv(nai_fd_t fd, nai_bufvec_t* v, size_t count, nai_off64_t offset);

    /**
 * write data to the specified offset of the file
 * @param   fd      the file descriptor
 * @param   lp      pointer to the buffer to write
 * @param   len     the length of buffer
 * @param   offset  the specified offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened without NAI_O_ASYNC, 
 *          this function will update the file cursor on win32, 
 *          we assume that write and pwrite will not be mixed, 
 *          so no fixes will be made
 */
    NAI_EXTERN
    intptr_t nai_file_pwrite(nai_fd_t fd, const void* lp, size_t len, nai_off64_t offset);

    /**
 * write data to the specified offset of the file from multiple buffers
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer array
 * @param   offset  the specified offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the file descriptor is opened without NAI_O_ASYNC, 
 *          this function will update the file cursor on win32, 
 *          we assume that write and pwrite will not be mixed, 
 *          so no fixes will be made
 */
    NAI_EXTERN
    intptr_t nai_file_pwritev(nai_fd_t fd, const nai_bufvec_t* v, size_t count, nai_off64_t offset);

    /**
 * get the currect position of the specified file
 * @param   fd      the file descriptor
 * @retval  >=0     the current position of file
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_off64_t nai_file_tell(nai_fd_t fd);

    /**
 * seek the file cursor to the specified position
 * @param   fd      the file descriptor
 * @param   offset  the relative offset of mode
 * @param   mode    how to move the file cursor, see @ref NAI_SEEK
 * @retval  >=0     the current position of file
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_off64_t nai_file_seek(nai_fd_t fd, nai_off64_t offset, nai_int_t mode);

    /**
 * get the size of the specified file
 * @param   fd      the file descriptor
 * @retval  >=0     the size of file
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_off64_t nai_file_get_size(nai_fd_t fd);

    /**
 * set the size of the specified file
 * @param   fd      the file descriptor
 * @param   newsize the new size to be setted
 * @retval  >=0     the new size of file
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_off64_t nai_file_set_size(nai_fd_t fd, nai_off64_t newsize);

    /**
 * to enable sparse file supported
 * @param   fd      the file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_set_sparse(nai_fd_t fd);

    /**
 * to enable close on exec
 * @param   fd      the file descriptor
 * @param   on      indicates whether to enable close on exec
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_set_cloexec(nai_fd_t fd, nai_int_t on);

    /**
 * to enable blocking mode
 * @param   fd      the file descriptor
 * @param   on      indicates whether it is to enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_set_blocking(nai_fd_t fd, nai_int_t on);

    /**
 * wait the file descriptor to become ready to perform I/O
 * @param   fd      the file descriptor
 * @param   events  which events to wait, see @ref NAI_POLL
 * @param   msec    the milliseconds to wait
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if timedout, the nai_errno is setted to #NAI_ETIMEDOUT
 */
    NAI_EXTERN
    nai_int_t nai_file_poll(nai_fd_t fd, nai_int_t events, uint32_t msec);

    /**
 * lock the entire file
 * @param   fd      the file descriptor
 * @param   mode    the mode of lock, see @ref NAI_FLOCK
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is #NAI_EAGAIN or EACCES, 
 *          means the file is locked by others.
 */
    NAI_EXTERN
    nai_int_t nai_file_lock(nai_fd_t fd, nai_int_t mode);

    /**
 * unlock the entire file
 * @param   fd      the file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_unlock(nai_fd_t fd);

    /**
 * flush the file to make sure data has been written to disk
 * @param   fd      the file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_flush(nai_fd_t fd);

    /**
 * close the specified file
 * @param   fd      the file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_close(nai_fd_t fd);

    /**
 * make a hard link to the path
 * @param   path    pointer to the stirng of target path
 * @param   to      pointer to the string of new path
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_link(const char* path, const char* to);

    /**
 * delete the file in the specified path
 * @param   path    pointer to the string of path to delete
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_file_unlink(const char* path);

    /**
 * rename the specified file from old path to new path
 * @param   from    pointer to the string of old path
 * @param   to      pointer to the string of new path
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_path_rename(const char* from, const char* to);

    /**
 * make a symbolic link to the path
 * @param   path    pointer to the string of target path
 * @param   to      pointer to the string of symlink path
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_path_symlink(const char* path, const char* to);

    /**
 * read the symbolic link
 * @param   path    pointer to the string of symlink path
 * @param   buf     pointer to the buffer
 * @param   size    the size of the buffer
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'size' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'size' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_path_readlink(const char* path, char* buf, size_t size);

    /**
 * set the mode of the file in the specified path
 * @param   path    pointer to the string of path to set
 * @param   mode    the new mode of file, ie. 0755, 0666
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_path_set_mode(const char* path, nai_int_t mode);

    /**
 * set the last modifed time of the file in the specified path
 * @param   path    pointer to the string of path to set
 * @param   usec    the new time in micro-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_path_set_mtime(const char* path, uint64_t usec);

    //////////////////////////////////////////////////////////////////////////////
    // deprecated apis

    /**
 * to enable close on exec
 * @param   fd      the file descriptor
 * @param   on      indicates whether to enable close on exec
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_file_set_cloexec instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_file_cloexec(nai_fd_t fd, nai_int_t on);

    /**
 * to enable blocking mode
 * @param   fd      the file descriptor
 * @param   on      indicates whether it is blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_file_set_blocking instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_file_blocking(nai_fd_t fd, nai_int_t on);

    //////////////////////////////////////////////////////////////////////////////
    // path

#if defined(_WIN32)

    /**
 * on win32, the path is not case sensitive
 */
    #define NAI_PATH_CASE_SENSITIVE 0

    /**
 * the max size of the path
 */
    #define NAI_PATH_MAX (260 + 64)

    /**
 * the default path separator
 */
    #define nai_path_sep '\\'

    /**
 * test whether the character is a path separator
 * @param   c       the value of a charater
 * @return  if it is a path separator, return 1, otherwise return 0
 */
    #define nai_path_is_sep(c) ((c) == '\\' || (c) == '/')

#else

    /**
 * on unix like, the path is case sensitive
 */
    #define NAI_PATH_CASE_SENSITIVE 1

    /**
 * the max size of the path
 */
    #define NAI_PATH_MAX            (PATH_MAX)

    /**
 * the default path separator
 */
    #define nai_path_sep            '/'

    /**
 * test whether the character is a path separator
 * @param   c       the value of a charater
 * @return  if it is a path separator, return 1, otherwise return 0
 */
    #define nai_path_is_sep(c)      ((c) == '/')

#endif

    /**
 * check if it is an absolute path
 * @param   path    pointer to the path string
 * @param   len     the length of the path string, 
 *                  < 0 means path is a null-terminated string
 * @retval  1       indication is an absolute path
 * @retval  0       indication is a relative path
 */
    NAI_EXTERN
    nai_int_t nai_path_is_absolute(const char* path, size_t len);

    /**
 * get the number chars of root
 * @param   path    pointer to the path string
 * @param   len     the length of the path string, 
 *                  < 0 means path is a null-terminated string
 * @retval  >=0     the number chars indication is an absolute path
 * @retval  0       indication is a relative path
 */
    NAI_EXTERN
    nai_int_t nai_path_get_root_len(const char* path, size_t len);

    /**
 * set the current work directory
 * @param   path    pointer to the path of the new work directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_path_set_cwd(const char* path);

    /**
 * get the current work directory
 * @param   buf     pointer to the buffer to receive data
 * @param   buflen  the length of the buffer
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_path_get_cwd(char* buf, size_t buflen);

    /**
 * get the real path of the specified path
 * @param   buf     pointer to the buffer to receive data
 * @param   buflen  the length of the buffer
 * @param   path    pointer to the specified path string
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_path_get_real(char* buf, size_t buflen, const char* path);

    /**
 * get the full path of the specified path
 * @param   buf     pointer to the buffer to receive data
 * @param   buflen  the length of the buffer
 * @param   path    pointer to the specified path string
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_path_get_full(char* buf, size_t buflen, const char* path);

    /**
 * canonicalize the specified path
 * @param   buf     pointer to the buffer to receive data
 * @param   buflen  the length of the buffer
 * @param   path    pointer the specified path to canonicalize
 * @param   len     the length of the path string, 
 *                  < 0 means path is a null-terminated string
 * @param   dirsep  the specified path separator
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_path_canonicalize(char* buf, size_t buflen, const char* path, size_t len, char dirsep);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
