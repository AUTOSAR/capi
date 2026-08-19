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
/// @file       nai_sendfile.h
/// @brief      file transfer interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is to optimize the sending of file data 
 *          to the network, avoid the process of moving the data from 
 *          the file to the user mode and then sending it to the network, 
 *          and reduce multiple copies of data and multiple switching 
 *          between the user mode and the kernel mode.
 *
 * @details the code example is as follows:
 *
 * @par     construct a file chunk
 * @code
 *          nai_int_t nhead;
 *          nai_int_t ntail;
 *          size_t hsize;
 *          size_t tsize;
 *          size_t fsize;
 *          nai_off64_t foffset;
 *          nai_fd_t fd;
 *          nai_bufvec_t* head;
 *          nai_bufvec_t* tail;
 *          nai_filechunk_t c;
 *
 *          c.fd = fd;
 *          c.size = fsize;
 *          c.offset = foffset;
 *          c.hsize = hsize;
 *          c.tsize = tsize;
 *          c.hcnt = nhead;
 *          c.tcnt = ntail;
 *          c.hvec = head;
 *          c.tvec = tail;
 * @endcode
 *
 * @par     send a file chunk
 * @code
 *          nai_int_t ec;
 *          intptr_t r;
 *          nai_fd_t s;
 *          nai_filechunk_t* c;
 *
 *          r = nai_sendfile(s, c, 60000);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              // blokced
 *              goto _wait;
 *          };
 *
 *          // success
 *          printf("transmited %d bytes\n", (nai_int_t)r);
 * @endcode
 *
 * @par     send an array of the file vector
 * @code
 *          nai_int_t ec;
 *          nai_int_t count;
 *          intptr_t r;
 *          nai_fd_t s;
 *          nai_filecvec_t* v;
 *
 *          r = nai_sendfilev(s, v, count, 60000);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              // blokced
 *              goto _wait;
 *          };
 *
 *          // success
 *          printf("transmited %d bytes\n", (nai_int_t)r);
 * @endcode
 */

#ifndef _SENDFILE_H_NAI
#define _SENDFILE_H_NAI

#pragma once

#include "nai_file.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_FILEVEC_T
    #define _NAI_TYPEDEF_FILEVEC_T
    typedef struct nai_filevec_s nai_filevec_t;
#endif
#ifndef _NAI_TYPEDEF_FILECHUNK_T
    #define _NAI_TYPEDEF_FILECHUNK_T
    typedef struct nai_filechunk_s nai_filechunk_t;
#endif
#ifndef _NAI_TYPEDEF_FILEARRAY_T
    #define _NAI_TYPEDEF_FILEARRAY_T
    typedef struct nai_filearray_s nai_filearray_t;
#endif

    /**
 * the structure of the transfer chunk, 
 * which can be file or memory.
 * set fd to NAI_FD_INVALID to indicate a memory chunk
 */
    struct nai_filevec_s
    {
        nai_fd_t fd;      /**< the file descriptor to be transfered */
        nai_int_t oflags; /**< the open flags of the file descriptor */
        size_t size;      /**< the length of chunk */
        union
        {
            void* ptr;       /**< pointer to the memory chunk */
            nai_off64_t off; /**< the offset of the file */
        };
    };

    /**
 * the sturcture of the array of the transfer chunk
 */
    struct nai_filearray_s
    {
        nai_filevec_t* v; /**< pointer to the array of the transfer chunk */
        nai_int_t count;  /**< the length of the array */
    };

    /**
 * the structure of the file chunk
 */
    struct nai_filechunk_s
    {
        nai_fd_t fd;        /**< the file descriptor to be transfered */
        nai_int_t oflags;   /**< the open flags of the file descriptor */
        size_t size;        /**< the bytes of whole chunk */
        size_t hsize;       /**< the bytes of the head buffers */
        size_t tsize;       /**< the bytes of the tail buffers */
        nai_off64_t off;    /**< the offset of the file */
        nai_int_t hcnt;     /**< the count of the head buffers */
        nai_int_t tcnt;     /**< the count of the tail buffers */
        nai_bufvec_t* hvec; /**< pointer to the head buffers before the file */
        nai_bufvec_t* tvec; /**< pointer to the tail buffers after the file */
    };

    /**
 * send a file chunk to the socket
 * @param   s       the file descriptor of socket
 * @param   c       pointer to a file chunk
 * @param   timeout the max waiting time in milli-seconds
 * @retval  >=0     the number of bytes sent is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function always works in blocking mode on win32.
 * @note    the parameter 'timeout' is only used on win32.
 */
    NAI_EXTERN
    intptr_t nai_sendfile(nai_fd_t s, const nai_filechunk_t* c, uint32_t timeout);

    /**
 * send an array of the transfer chunk to the socket
 * @param   s       the file descriptor of socket
 * @param   v       pointer to the array of the transfer chunk
 * @param   count   the length of the array
 * @param   timeout the max waiting time in milli-seconds
 * @retval  >=0     the number of bytes sent is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function always works in blocking mode on win32.
 * @note    the parameter 'timeout' is only used on win32.
 */
    NAI_EXTERN
    intptr_t nai_sendfilev(nai_fd_t s, const nai_filevec_t* v, nai_int_t count, uint32_t timeout);

    /**
 * move the chunks from the array to the file chunk
 * @param   a       pointer the array of the transfer chunk
 * @param   c       pointer the file chunk
 * @param   v       pointer the buffer of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   max_ht  the maximum number of filling head or tail buffers, 
 *                  the suitable value depends on the system support.
 * @return  the number of bytes moved
 */
    NAI_EXTERN
    size_t nai_filearray_to_filechunk(
        nai_filearray_t* a, nai_filechunk_t* c, nai_bufvec_t* v, nai_int_t count, nai_int_t max_ht);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
