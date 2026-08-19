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
/// @file       nai_buf.h
/// @brief      the i/o buffer
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details support complex content buffer, \n
 *          support custom memory management.
 *
 * @details the code example is as follows:
 *
 * @par     create a buffer pool
 * @code
 *          nai_int_t r;
 *          nai_bufpool_t pool;
 *
 *          nai_bufpool_init(&pool);
 *
 *          r = nai_bufpool_open(&pool, 0, 0);
 *          if (r < 0) {
 *              goto _end;
 *          };
 * @endcode
 *
 * @par     create an empty buffer from the buffer pool
 * @code
 *          nai_buf_t* b;
 *          nai_bufpool_t* p;
 *
 *          b = nai_buf_alloc(p, 1024);
 *          if (b == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create an empty buffer from the user buffer
 * @code
 *          nai_int_t len;
 *          void* buf;
 *          nai_buf_t* b;
 *          nai_bufpool_t* p;
 *
 *          b = nai_buf_from_wmemory(p, buf, len, 0);
 *          if (b == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create a buffer from the user buffer with the content
 * @code
 *          nai_int_t len;
 *          const void* buf;
 *          nai_buf_t* b;
 *          nai_bufpool_t* p;
 *
 *          // 
 *          b = nai_buf_from_rmemory(p, buf, len, 0);
 *          if (b == 0) {
 *              goto _fail
 *          };
 * @endcode
 *
 * @par     read data from the memory buffer
 * @code
 *          nai_int_t len;
 *          void* buf;
 *          nai_buf_t* b;
 *
 *          if (len > (intptr_t)nai_buf_size(b)) {
 *              len = (nai_int_t)nai_buf_size(b);
 *          };
 *
 *          // read and commit
 *          nai_memcpy(buf, nai_buf_ptr(b), len);
 *          nai_buf_rcommit(b, len);
 * @endcode
 *
 * @par     write data to the memory buffer
 * @code
 *          nai_int_t len;
 *          void* buf;
 *          nai_buf_t* b;
 *
 *          if (len > (intptr_t)nai_buf_space(b)) {
 *              len = (nai_int_t)nai_buf_space(b);
 *          };
 *
 *          // write and commit
 *          nai_memcpy(nai_buf_ptr(b) + nai_buf_size(b), buf, len);
 *          nai_buf_wcommit(b, len);
 * @endcode
 *
 * @par     initial a buffer list
 * @code
 *          nai_buflist_t list;
 *          nai_buflist_init(&list, 0);
 * @endcode
 *
 * @par     append a buffer into the buffer list
 * @code
 *          nai_buf_t* b;
 *          nai_buflist_t* l;
 *
 *          nai_buflist_add_tail(l, b);
 * @endcode
 *
 *
 * @par     remove a buffer from the buffer list
 * @code
 *          nai_buf_t* b;
 *          nai_buf_entry_remove(b);
 * @endcode
 *
 */

#ifndef _BUF_H_NAI
#define _BUF_H_NAI

#pragma once

#include <assert.h>

#include "nai/runtime/nai_list.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_BUF_TYPE
 * @name    NAI_BUF_TYPE        buffer types
 * @{
 */
#define NAI_BUF_NONE   0 /**< no type */
#define NAI_BUF_REF    1 /**< a buffer references other buffers */
#define NAI_BUF_MEMORY 2 /**< a buffer of memory */
#define NAI_BUF_FILE   3 /**< a buffer of file */
#define NAI_BUF_OBJECT 4 /**< a buffer of user object */
    /** @} */

#ifndef _NAI_TYPEDEF_BUF_T
    #define _NAI_TYPEDEF_BUF_T
    typedef struct nai_buf_s nai_buf_t;
#endif
#ifndef _NAI_TYPEDEF_BUF_EXTRA_T
    #define _NAI_TYPEDEF_BUF_EXTRA_T
    typedef struct nai_buf_extra_s nai_buf_extra_t;
#endif
#ifndef _NAI_TYPEDEF_BUF_OPS_T
    #define _NAI_TYPEDEF_BUF_OPS_T
    typedef struct nai_buf_ops_s nai_buf_ops_t;
#endif
#ifndef _NAI_TYPEDEF_BUFPOOL_T
    #define _NAI_TYPEDEF_BUFPOOL_T
    typedef struct nai_bufpool_s nai_bufpool_t;
#endif
#ifndef _NAI_TYPEDEF_BUFPOOL_OPS_T
    #define _NAI_TYPEDEF_BUFPOOL_OPS_T
    typedef struct nai_bufpool_ops_s nai_bufpool_ops_t;
#endif
#ifndef _NAI_TYPEDEF_BUFLIST_T
    #define _NAI_TYPEDEF_BUFLIST_T
    typedef struct nai_buflist_s nai_buflist_t;
#endif

    //////////////////////////////////////////////////////////////////////////////
    // bufpool

    /**
 * the structure of the operations of the buffer pool
 */
    struct nai_bufpool_ops_s
    {
        /** the name of the operations */
        const char* name;

        /**
     * allocate memory, internal save allocation size
     * @param   p       pointer to the user pool
     * @param   size    the allocated size
     * @return  the address of allocated memory on success, 
     *          NULL is retruned on fails, see #nai_errno.
     */
        void* (*alloc)(void* p, size_t size);

        /**
     * free the memory which allocated by alloc
     * @param   p       pointer to the user pool
     * @param   m       pointer to the memory to free
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*free)(void* p, void* m);

        /**
     * allocate memory, internal dont save allocation size
     * @param   p       pointer to the user pool
     * @param   size    the allocated size
     * @return  the address of allocated memory on success, 
     *          NULL is retruned on fails, see #nai_errno.
     */
        void* (*xalloc)(void* p, size_t size);

        /**
     * free the memory which allocated by xalloc
     * @param   p       pointer to the user pool
     * @param   m       pointer to the memory to free
     * @param   size    the allocated size
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*xfree)(void* p, void* m, size_t size);

        /**
     * allocate metadata, internal dont save allocation size
     * @param   p       pointer to the user pool
     * @param   size    the allocated size
     * @return  the address of allocated memory on success, 
     *          NULL is retruned on fails, see #nai_errno.
     */
        void* (*malloc)(void* p, size_t size);

        /**
     * free the metadata which allocated by malloc
     * @param   p       pointer to the user pool
     * @param   m       pointer to the memory to free
     * @param   size    the allocated size
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*mfree)(void* p, void* m, size_t size);

        /**
     * close the buffer pool
     * @param   p       pointer to the user pool
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*close)(void* p);
    };

    /**
 * the structure of the buffer pool
 */
    struct nai_bufpool_s
    {
        void* pool;             /**< pointer to the user pool */
        nai_bufpool_ops_t* ops; /**< pointer to the operations */
    };

/**
 * @name    nai_bufpool_defines buffer pool operations
 * @{
 */

/**
 * initial the bufpool
 * @param   p       pointer to the bufpool
 * @return  void
 */
#define nai_bufpool_init(p)                                                                                            \
    {                                                                                                                  \
        (p)->pool = 0;                                                                                                 \
        (p)->ops  = 0;                                                                                                 \
    }

/** 
 * allocate memory from a bufpool, 
 * @param   p       pointer to the bufpool
 * @param   s       the size to allocate
 * @return  the address of allocated memory on success, 
 *          NULL is retruned on fails, see #nai_errno.
 * @note    the bufpool will save the size for release
 */
#define nai_bufpool_alloc(p, s) ((p)->ops->alloc((p)->pool, (s)))

/** 
 * free memory to a bufpool, 
 * @param   p       pointer to the bufpool
 * @param   m       pointer to the memory to free
 * @return  void
 * @note    the memory must be allocated by nai_bufpool_alloc
 */
#define nai_bufpool_free(p, m) ((p)->ops->free((p)->pool, (m)))

/**
 * allocate memory from a bufpool
 * @param   p       pointer to the bufpool
 * @param   s       the size to allocate
 * @return  the address of allocated memory on success, 
 *          NULL is retruned on fails, see #nai_errno.
 * @note    the bufpool will not save size, 
 *          the user should provide size when released.
 */
#define nai_bufpool_xalloc(p, s) ((p)->ops->xalloc((p)->pool, (s)))

/** 
 * free memory to a bufpool
 * @param   p       pointer to the bufpool
 * @param   m       pointer to the memory to free
 * @param   s       the allocated size
 * @return  void
 * @note    the memory must be allocated by nai_bufpool_xalloc
 */
#define nai_bufpool_xfree(p, m, s) ((p)->ops->xfree((p)->pool, (m), (s)))

/**
 * allocate metadata from a bufpool
 * @param   p       pointer to the bufpool
 * @param   s       the size to allocate
 * @return  the address of allocated memory on success, 
 *          NULL is retruned on fails, see #nai_errno.
 * @note    the bufpool will not save size, 
 *          the user should provide size when released.
 */
#define nai_bufpool_malloc(p, s) ((p)->ops->malloc((p)->pool, (s)))

/** 
 * free metadata to a bufpool
 * @param   p       pointer to the bufpool
 * @param   m       pointer to the memory to free
 * @param   s       the allocated size
 * @return  void
 * @note    the memory must be allocated by nai_bufpool_malloc
 */
#define nai_bufpool_mfree(p, m, s) ((p)->ops->mfree((p)->pool, (m), (s)))

/** @} */

/**
 * deprecated, see #nai_bufpool_open
 */
#define nai_bufpool_create nai_bufpool_open

    /**
 * create a bufpool
 * @param   p       pointer to a initialized bufpool
 * @param   size    the size of the memory block allocated by the backend
 * @param   mt      indicates whether it is used in multi-threads
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_bufpool_open(nai_bufpool_t* p, size_t size, nai_int_t mt);

    /**
 * create a bufpool from pool
 * @param   p       pointer to a initialized bufpool
 * @param   pool    pointer to a pool used for backend allocator, can be null
 * @param   mt      indicates whether it is used in multi-threads
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_bufpool_from(nai_bufpool_t* p, nai_pool_t* pool, nai_int_t mt);

    /**
 * close a bufpool
 * @param   p       pointer to a bufpool will be closed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_bufpool_close(nai_bufpool_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // bufops

    /**
 * the structure of the operation of the buffer
 */
    struct nai_buf_ops_s
    {
        /** the name of the operation */
        const char* name;

        /**
     * mapping the object at the specified offset to the memory
     * @param   p       pointer to the user object
     * @param   dst     pointer to the memory pointer
     * @param   size    the size to mapping, this is a recommended value
     * @param   offset  the specified offset of the user object
     * @retval  >=0     the number of bytes mapped is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*mmap)(void* p, void** dst, size_t size, nai_off64_t offset);

        /**
     * read data at the specified offset from the object
     * @param   p       pointer to the user object
     * @param   dst     pointer to the memory
     * @param   size    the size of the memory
     * @param   offset  the specified offset of the user object
     * @retval  >=0     the number of bytes read is returned on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        intptr_t (*read)(void* p, void* dst, size_t size, nai_off64_t offset);

        /**
     * free the user object
     * @param   p       pointer to the user object
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*free)(void* p);
    };

/**
 * @name    nai_bufops_defines  user defined buffer operations
 * @{
 */

/**
 * mapping the object at the specified offset to the memory
 * @param   b       pointer to the operation of the buffer
 * @param   p       pointer to the user object
 * @param   d       pointer to the memory pointer
 * @param   s       the size to mapping, this is a recommended value
 * @param   o       the specified offset of the user object
 * @retval  >=0     the number of bytes mapped is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_bufops_mmap(b, p, d, s, o) ((b)->mmap((p), (d), (s), (o)))

/**
 * read data at the specified offset from the object
 * @param   b       pointer to the operation of the buffer
 * @param   p       pointer to the user object
 * @param   d       pointer to the memory
 * @param   s       the size of the memory
 * @param   o       the specified offset of the user object
 * @retval  >=0     the number of bytes read is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_bufops_read(b, p, d, s, o) ((b)->read((p), (d), (s), (o)))

/**
 * free the user object
 * @param   b       pointer to the operation of the buffer
 * @param   p       pointer to the user object
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_bufops_free(b, p) ((b)->free((p)))

    /** @} */

    //////////////////////////////////////////////////////////////////////////////
    // buf

    /**
 * the structure of the buffer
 */
    struct nai_buf_s
    {
        /** the entry of the buffer list */
        nai_list_entry_t ent;

        /** start and size */
        union
        {
#if NAI_SIZEOF_VOID_P == 4 && (NAI_HAVE_BIG_ENDIAN)
            struct
            {
                uint32_t reserve; /**< placeholder */
                uint8_t* start;   /**< memory start point */
            };
#elif NAI_SIZEOF_VOID_P <= 8
        uint8_t* start; /**< memory start point */
#else
    #error "unknown pointer size cannot correct memory layout"
#endif
            nai_off64_t offset; /**< file offset */
        };
        size_t size;  /**< the content length of buffer */
        size_t total; /**< the space size of buffer */

        /** buffer stat, don't modify it */
        union
        {
            struct
            {
                uint32_t type : 4;      /**< type of buffer */
                uint32_t typeref : 4;   /**< type of reference object */
                uint32_t linked : 1;    /**< the buffer linked in list */
                uint32_t temporary : 1; /**< temporary memory */
                uint32_t flush : 1;     /**< flush */
                uint32_t eos : 1;       /**< is end of stream/message */
                uint32_t threading : 1; /**< is share by other threads */
                uint32_t asyncio : 1;   /**< file opened with async flag */
                uint32_t directio : 1;  /**< file opened with direct flag */
                uint32_t mempool : 1;   /**< memory allocate from pool */
                uint32_t extra : 1;     /**< has buf extra part */
            };
            uint32_t flags;
        };

        /** count of reference by other bufs */
        nai_atomic32_t refcount;

        /* reference object's ptr */
        union
        {
            void* obj;      /**< reference memory or object */
            nai_buf_t* buf; /**< reference buffer */
        } ref;

        /** allocate pool */
        nai_bufpool_t* pool;
    };

    struct nai_buf_extra_s
    {
        /** reference object's ops */
        nai_buf_ops_t* ops;

        /** reference file handle */
        nai_fd_t fd;
    };

/**
 * @name    nai_buf_defines     buffer operations
 * @{
 */

/**
 * test the buffer is a memory
 * @param   b       pointer to the buffer
 * @return  if it is a memory, return 1, otherwise return 0
 */
#define nai_buf_in_memory(b) ((b)->type == NAI_BUF_MEMORY)

/**
 * test the buffer is a file
 * @param   b       pointer to the buffer
 * @return  if it is a file, return 1, otherwise return 0
 */
#define nai_buf_in_file(b) ((b)->type == NAI_BUF_FILE)

/**
 * test the buffer is an object
 * @param   b       pointer to the buffer
 * @return  if it is an object, return 1, otherwise return 0
 */
#define nai_buf_in_object(b) ((b)->type == NAI_BUF_OBJECT)

/**
 * get the memory address of the buffer
 * @param   b       pointer to the buffer
 * @return  the address of the memory
 */
#define nai_buf_ptr(b) (&*(b)->start)

/**
 * get the size of the buffer
 * @param   b       pointer to the buffer
 * @return  the size of the buffer in bytes
 */
#define nai_buf_size(b) ((size_t)(b)->size)

/**
 * get the space size of the buffer which is writable
 * @param   b       pointer to the buffer
 * @return  the space size of the buffer in bytes
 */
#define nai_buf_space(b) ((b)->total - (b)->size)

/**
 * get the total size of the buffer
 * @param   b       pointer to the buffer
 * @return  the total size of the buffer in bytes
 */
#define nai_buf_total(b) ((size_t)(b)->total)

/**
 * get the offset of the file or the object in the buffer
 * @param   b       pointer to the buffer
 * @return  the offset of the file or the object
 */
#define nai_buf_offset(b) ((nai_off64_t)(b)->offset)

/**
 * commit the number of bytes be read from buffer
 * @param   b       pointer to the buffer
 * @param   s       the bytes read
 * @return  void
 */
#define nai_buf_rcommit(b, s)                                                                                          \
    {                                                                                                                  \
        intptr_t _size = (s);                                                                                          \
        assert(_size >= 0);                                                                                            \
        assert(_size <= (intptr_t)nai_buf_size(b));                                                                    \
        (b)->start += _size;                                                                                           \
        (b)->size -= _size;                                                                                            \
        (b)->total -= _size;                                                                                           \
    }

/**
 * commit the number of byes be written to buffer
 * @param   b       pointer to the buffer
 * @param   s       the bytes written
 * @return  void
 */
#define nai_buf_wcommit(b, s)                                                                                          \
    {                                                                                                                  \
        intptr_t _size = (s);                                                                                          \
        assert(_size >= 0);                                                                                            \
        assert(_size <= (intptr_t)nai_buf_space(b));                                                                   \
        (b)->size += _size;                                                                                            \
    }

/**
 * mark the buffer will be threading
 * @param   b       pointer to the buffer
 * @return  void
 * @note    this function always sets the threading flag of buffer, 
 *          regardless of the reference count.
 * @note    when the buffer is marked as threaded, it will use atomic 
 *          operations to access the reference count.
 * @note    the user needs to ensure that the pool of buffer is 
 *          thread-safe, otherwise the buffer is still thread-unsafe 
 *          when it is released
 */
#define nai_buf_set_threading(b)                                                                                       \
    {                                                                                                                  \
        nai_buf_t* _b = (b);                                                                                           \
        if (_b->typeref == NAI_BUF_REF) {                                                                              \
            _b            = _b->ref.buf;                                                                               \
            _b->threading = 1;                                                                                         \
        } else {                                                                                                       \
            _b->threading = 1;                                                                                         \
        };                                                                                                             \
    }

/**
 * mark the buffer will be used in other thread
 * @param   b       pointer to the buffer
 * @return  void
 * @note    this function sets the threading flag of buffer
 *          on multiple references
 * @note    when the buffer is marked as threaded, it will use atomic 
 *          operations to access the reference count.
 * @note    the user needs to ensure that the pool of buffer is 
 *          thread-safe, otherwise the buffer is still thread-unsafe 
 *          when it is released
 */
#define nai_buf_set_threadsafe(b)                                                                                      \
    {                                                                                                                  \
        nai_buf_t* _b = (b);                                                                                           \
        if (_b->typeref == NAI_BUF_REF) {                                                                              \
            _b            = _b->ref.buf;                                                                               \
            _b->threading = 1;                                                                                         \
        } else if (_b->refcount > 1) {                                                                                 \
            _b->threading = 1;                                                                                         \
        } else {                                                                                                       \
            /* nothing */                                                                                              \
            ;                                                                                                          \
        };                                                                                                             \
    }

/**
 * insert a buffer before a reference buffer
 * @param   b       pointer to the buffer
 * @param   e       pointer to a buffer 
 *                  which will be inserted before the buffer 'b'
 * @return  void
 */
#define nai_buf_insert_before(b, e)                                                                                    \
    {                                                                                                                  \
        assert(!(e)->linked);                                                                                          \
        assert((b)->linked);                                                                                           \
        (e)->linked = 1;                                                                                               \
        nai_list_insert_before(&(b)->ent, &(e)->ent);                                                                  \
    }

/**
 * insert a buffer after a reference buffer
 * @param   b       pointer to the buffer
 * @param   e       pointer to a buffer 
 *                  which will be inserted after the buffer 'b'
 * @return  void
 */
#define nai_buf_insert_after(b, e)                                                                                     \
    {                                                                                                                  \
        assert(!(e)->linked);                                                                                          \
        assert((b)->linked);                                                                                           \
        (e)->linked = 1;                                                                                               \
        nai_list_insert_after(&(b)->ent, &(e)->ent);                                                                   \
    }

/**
 * remove a buffer from list
 * @param   b       pointer to the buffer which will be removed
 * @return  void
 */
#define nai_buf_entry_remove(b)                                                                                        \
    {                                                                                                                  \
        assert((b)->linked);                                                                                           \
        (b)->linked = 0;                                                                                               \
        nai_list_entry_remove(&(b)->ent);                                                                              \
    }

    /** @} */

    /**
 * get file descriptor in the buffer
 * @param   b       pointer to the file buffer
 * @return  if it is a file buffer, it returns the file descriptor, 
 *          otherwise it returns #NAI_FD_INVALID.
 */
    NAI_EXTERN
    nai_fd_t nai_buf_fd(nai_buf_t* b);

    /**
 * create a file-type buffer
 * @param   pool    pointer to the buffer pool
 * @param   fd      the file descriptor
 * @param   start   the file offset
 * @param   size    the file size
 * @param   oflags  file open flags, see NAI_OPEN
 * @param   ops     user defined file operations
 * @param   obj     pointer to a user object
 * @return  if success a new buffer is retruned, otherwise null is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_from_file(nai_bufpool_t* pool,
                                 nai_fd_t fd,
                                 nai_off64_t start,
                                 size_t size,
                                 nai_int_t oflags,
                                 nai_buf_ops_t* ops,
                                 void* obj);

    /**
 * create a object-type buffer
 * @param   pool    pointer to the buffer pool
 * @param   p       pointer to a user memory, can be null
 * @param   size    user object size
 * @param   ops     user defined object operations
 * @param   obj     pointer to a user object
 * @return  if success a new buffer is retruned, otherwise null is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_from_object(nai_bufpool_t* pool, void* p, size_t size, nai_buf_ops_t* ops, void* obj);

    /**
 * create a memory-type buffer for reading
 * @param   pool    pointer to the buffer pool
 * @param   p       pointer to a user memory
 * @param   size    user memory size
 * @param   ops     set for free memory, 
 *                  if null be seted the memory will not be free
 * @return  if success a new buffer is retruned, otherwise null is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_from_rmemory(nai_bufpool_t* pool, const void* p, size_t size, nai_buf_ops_t* ops);

    /**
 * create a memory-type buffer for writing
 * @param   pool    pointer to the buffer pool
 * @param   p       pointer to a user memory
 * @param   size    user memory size
 * @param   ops     set for free memory, 
 *                  if null be seted the memory will not be free
 * @return  if success a new buffer is retruned, otherwise null is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_from_wmemory(nai_bufpool_t* pool, void* p, size_t size, nai_buf_ops_t* ops);

    /**
 * alloc a memory-type buffer
 * @param   pool    pointer to the buffer pool
 * @param   size    the size to allocate
 * @return  if success a new buffer is retruned, otherwise 0 is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_alloc(nai_bufpool_t* pool, size_t size);

    /**
 * duplicate a buffer
 * @param   pool    pointer to the buffer pool
 * @param   b       pointer to a source buffer
 * @param   memory  indicates construct a memory buffer
 * @return  if success a new buffer is retruned, otherwise 0 is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_dup(nai_bufpool_t* pool, nai_buf_t* b, nai_int_t memory);

    /**
 * split buffer
 * @param   pool    pointer to the buffer pool
 * @param   b       pointer to a source buffer
 * @param   size    the number of bytes to split
 * @param   memory  indicates construct a memory buffer
 * @return  if success a new buffer is retruned, otherwise 0 is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_cut(nai_bufpool_t* pool, nai_buf_t* b, size_t size, nai_int_t memory);

    /**
 * extracts parts of a buffer
 * @param   pool    pointer to the buffer pool
 * @param   b       pointer to a source buffer
 * @param   start   the position where to start the extraction
 * @param   size    the number of bytes to extract
 * @param   memory  indicates construct a memory buffer
 * @return  if success a new buffer is retruned, otherwise 0 is retruned
 */
    NAI_EXTERN
    nai_buf_t* nai_buf_sub(nai_bufpool_t* pool, nai_buf_t* b, nai_off64_t start, size_t size, nai_int_t memory);

    /**
 * close a buffer
 * @param   b       pointer to the buffer which will be closed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buf_close(nai_buf_t* b);

    //////////////////////////////////////////////////////////////////////////////
    // buflist

#ifndef _NAI_TYPEDEF_BUFLIST_INFO_T
    #define _NAI_TYPEDEF_BUFLIST_INFO_T
    typedef struct nai_buflist_info_s nai_buflist_info_t;
#endif

    /**
 * the structure of the information of the buffer list
 */
    struct nai_buflist_info_s
    {
        union
        {
            struct
            {
                uint32_t flush : 1;    /**< has flush */
                uint32_t eos : 1;      /**< has end of stream */
                uint32_t object : 1;   /**< has object */
                uint32_t file : 1;     /**< has file */
                uint32_t asyncio : 1;  /**< has asyncio file */
                uint32_t directio : 1; /**< has directio file */
            };
            uint32_t flags;
        };
    };

    /**
 * @name    nai_buflist_defines buffer list operations
 * @{
 */

    /**
 * the structure of the buffer list
 */
    struct nai_buflist_s
    {
        nai_bufpool_t* pool;  /**< pointer to the buffer pool */
        nai_list_entry_t ent; /**< the buffer list */
    };

/**
 * initial the buffer list
 * @param   l       pointer to the buffer list
 * @param   p       pointer to a buffer pool 
 *                  which will associated with the buffer list
 * @return  void
 */
#define nai_buflist_init(l, p)                                                                                         \
    {                                                                                                                  \
        nai_list_init (&(l)->ent)(l)->pool = (p);                                                                      \
    }

/**
 * is an empty buffer list
 * @param   l       pointer to the buffer list
 * @retval  1       means an empty buffer list
 * @retval  0       means an non-empty buffer list
 */
#define nai_buflist_is_empty(l) nai_list_is_empty(&(l)->ent)

/**
 * insert a buffer to the head of list
 * @param   l       pointer to the buffer list
 * @param   b       pointer to a buffer to insert
 *                  which will be inserted to the head of list
 * @return  void
 */
#define nai_buflist_insert_head(l, b)                                                                                  \
    {                                                                                                                  \
        assert(!(b)->linked);                                                                                          \
        (b)->linked = 1;                                                                                               \
        nai_list_insert_head(&(l)->ent, &(b)->ent);                                                                    \
    }

/**
 * insert a buffer to the tail of list
 * @param   l       pointer to the buffer list
 * @param   b       pointer to a buffer to insert
 *                  which will be inserted to the tail of list
 * @return  void
 */
#define nai_buflist_insert_tail(l, b)                                                                                  \
    {                                                                                                                  \
        assert(!(b)->linked);                                                                                          \
        (b)->linked = 1;                                                                                               \
        nai_list_insert_tail(&(l)->ent, &(b)->ent);                                                                    \
    }

    /** @} */

    /**
 * move a list to the head of specilist list
 * @param   l       pointer to the buffer list
 * @param   a       pointer to a buffer list to insert 
 *                  which will be moved to the head of list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_add_head(nai_buflist_t* l, nai_buflist_t* a);

    /**
 * move a list to the tail of specilist list
 * @param   l       pointer to the buffer list
 * @param   a       pointer to a buffer list to insert 
 *                  which will be moved to the tail of list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_add_tail(nai_buflist_t* l, nai_buflist_t* a);

    /**
 * mark the list will be threading
 * @param   l       pointer to the buffer list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function always sets the threading flag of buffer, 
 *          regardless of the reference count.
 * @note    when the buffer is marked as threaded, it will use atomic 
 *          operations to access the reference count.
 * @note    the user needs to ensure that the pool of buffer is 
 *          thread-safe, otherwise the buffer is still thread-unsafe 
 *          when it is released
 */
    NAI_EXTERN
    nai_int_t nai_buflist_set_threading(nai_buflist_t* l);

    /**
 * mark the list will be used in other thread
 * @param   l       pointer to the buffer list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function sets the threading flag of buffer
 *          on multiple references
 * @note    when the buffer is marked as threaded, it will use atomic 
 *          operations to access the reference count.
 * @note    the user needs to ensure that the pool of buffer is 
 *          thread-safe, otherwise the buffer is still thread-unsafe 
 *          when it is released
 */
    NAI_EXTERN
    nai_int_t nai_buflist_set_threadsafe(nai_buflist_t* l);

    /**
 * close buffer list and cloes all buffer
 * @param   l       pointer to the buffer list
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_close(nai_buflist_t* l);

    /**
 * commit the number of bytes read from buffer list
 * @param   l       pointer to the buffer list
 * @param   bytes   the number of bytes read
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_rcommit(nai_buflist_t* l, size_t bytes);

    /**
 * commit the number of bytes read from last of buffer list
 * @param   l       pointer to the buffer list
 * @param   bytes   the number of bytes read
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_rcommit_last(nai_buflist_t* l, size_t bytes);

    /**
 * commit the number of bytes written to buffer list
 * @param   l       pointer to the buffer list
 * @param   bytes   the number of bytes written
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_wcommit(nai_buflist_t* l, size_t bytes);

    /**
 * commit written bytes and move fulled buf from list 'l' to list 'f'
 * @param   l       pointer to the buffer list
 * @param   f       pointer to the fulled buffer list
 * @param   bytes   the number of bytes written
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_buflist_fulled(nai_buflist_t* l, nai_buflist_t* f, size_t bytes);

    /**
 * count the space size of buffer list
 * @param   l       pointer to the buffer list
 * @param   check   only check if there has space for writing
 * @retval  >=0     the number of space bytes in the buffer list is returned
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    int64_t nai_buflist_space(nai_buflist_t* l, nai_int_t check);

    /**
 * count the size of buffer list
 * @param   l       pointer to the buffer list
 * @param   eos     whether to stop when the end flag is found
 * @param   i       pointer to the information of the buffer list, can be null
 * @retval  >=0     the number of bytes in the buffer list is returned
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    int64_t nai_buflist_size(nai_buflist_t* l, nai_int_t eos, nai_buflist_info_t* i);

    /**
 * move the specified number of bytes from list 's' to list 'l'
 * @param   l       pointer to the destination buffer list
 * @param   s       pointer to the source buffer list
 * @param   bytes   the number of bytes to move
 * @param   eos     whether to stop when the end flag is found
 * @param   i       pointer to the information of the buffer list, can be null
 * @retval  >=0     the number of moved bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_move(nai_buflist_t* l, nai_buflist_t* s, size_t bytes, nai_int_t eos, nai_buflist_info_t* i);

    /**
 * move the specified number of last bytes from list 's' to list 'l'
 * @param   l       pointer to the destination buffer list
 * @param   s       pointer to the source buffer list
 * @param   bytes   the number of bytes to move
 * @param   eos     whether to stop when the end flag is found
 * @param   i       pointer to the information of the buffer list, can be null
 * @retval  >=0     the number of moved bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_move_last(
        nai_buflist_t* l, nai_buflist_t* s, size_t bytes, nai_int_t eos, nai_buflist_info_t* i);

    /**
 * read from the beginning of buffer list
 * @param   l       pointer to the buffer list
 * @param   buf     pointer to the buffer to read
 * @param   size    the length of the buffer
 * @retval  >=0     the number of bytes read is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_read(nai_buflist_t* l, void* buf, size_t size);

    /**
 * write to the end of buffer list
 * @param   l       pointer to the buffer list
 * @param   buf     pointer to the buffer to write
 * @param   size    the length of the buffer
 * @param   grow    the flag of auto grown buffer list when no space
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_write(nai_buflist_t* l, const void* buf, size_t size, nai_int_t grow);

    //////////////////////////////////////////////////////////////////////////////
    // buflist operations

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
#ifndef _NAI_TYPEDEF_BUFVEC_T
    #define _NAI_TYPEDEF_BUFVEC_T
    typedef struct nai_bufvec_s nai_bufvec_t;
#endif
#ifndef _NAI_TYPEDEF_BUFARRAY_T
    #define _NAI_TYPEDEF_BUFARRAY_T
    typedef struct nai_bufarray_s nai_bufarray_t;
#endif

    /**
 * fill the content of buffer list into a buffer vector
 * @param   l       pointer to the buffer list
 * @param   v       pointer to a buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from file or object to memory
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_rbufvec(nai_buflist_t* l, nai_bufvec_t* v, size_t limit, nai_int_t message);

    /**
 * fill the content of buffer list into a buffer array
 * @param   l       pointer to the buffer list
 * @param   a       pointer to a buffer array which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from file or object to memory
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_rbufarray(
        nai_buflist_t* l, nai_bufarray_t* a, nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message);

    /**
 * fill the content of buffer list into a file chunk
 * @param   l       pointer to the buffer list
 * @param   c       pointer to a file chunk which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   max_ht  maximun the count of head or tail buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from object to memory
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_filechunk(nai_buflist_t* l,
                                      nai_filechunk_t* c,
                                      nai_bufvec_t* v,
                                      nai_int_t count,
                                      nai_int_t max_ht,
                                      size_t limit,
                                      nai_int_t message);

    /**
 * fill the content of buffer list into a file array
 * @param   l       pointer to the buffer list
 * @param   a       pointer to a file array which is saved fill result
 * @param   v       pointer to an array of file vector
 * @param   count   the count of file vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from object to memory
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_filearray(
        nai_buflist_t* l, nai_filearray_t* a, nai_filevec_t* v, nai_int_t count, size_t limit, nai_int_t message);

    /**
 * fill the space of buffer list into a buffer vector
 * @param   l       pointer to the buffer list
 * @param   v       pointer to a buffer vector
 * @param   limit   maximum the number of space bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of space bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_wbufvec(nai_buflist_t* l, nai_bufvec_t* v, size_t limit, nai_int_t message);

    /**
 * fill the space of buffer list into a buffer array
 * @param   l       pointer to the buffer list
 * @param   a       pointer to a buffer array which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   limit   maximum the number of space bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of space bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_buflist_to_wbufarray(
        nai_buflist_t* l, nai_bufarray_t* a, nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message);

    //////////////////////////////////////////////////////////////////////////////
    // bufwalk operations

#ifndef _NAI_TYPEDEF_BUFWALK_T
    #define _NAI_TYPEDEF_BUFWALK_T
    typedef struct nai_bufwalk_s nai_bufwalk_t;
#endif

    /**
 * the structure of the buffer iterator
 */
    struct nai_bufwalk_s
    {
        nai_buflist_t* list;
        nai_list_entry_t* buf;
        size_t off;
    };

/**
 * initial the buffer iterator
 * @param   i       pointer to the buffer iterator
 * @param   l       pointer to the buffer list
 * @return  void
 */
#define nai_bufwalk_init(i, l)                                                                                         \
    {                                                                                                                  \
        (i)->list = (l);                                                                                               \
        (i)->buf  = (l)->ent.next;                                                                                     \
        (i)->off  = 0;                                                                                                 \
    }

    /**
 * fill the content of buffer list into a buffer vector
 * @param   i       pointer to the buffer iterator
 * @param   v       pointer to a buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from file or object to memory
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_rbufvec(nai_bufwalk_t* i, nai_bufvec_t* v, size_t limit, nai_int_t message);

    /**
 * fill the content of buffer list into a buffer array
 * @param   i       pointer to the buffer iterator
 * @param   a       pointer to a buffer array which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from file or object to memory
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_rbufarray(
        nai_bufwalk_t* i, nai_bufarray_t* a, nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message);

    /**
 * fill the content of buffer list into a file chunk
 * @param   i       pointer to the buffer iterator
 * @param   c       pointer to a file chunk which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   max_ht  maximun the count of head or tail buffer vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from object to memory
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_filechunk(nai_bufwalk_t* i,
                                      nai_filechunk_t* c,
                                      nai_bufvec_t* v,
                                      nai_int_t count,
                                      nai_int_t max_ht,
                                      size_t limit,
                                      nai_int_t message);

    /**
 * fill the content of buffer list into a file array
 * @param   i       pointer to the buffer iterator
 * @param   a       pointer to a file array which is saved fill result
 * @param   v       pointer to an array of file vector
 * @param   count   the count of file vector
 * @param   limit   maximum the number of content bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of content bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will auto read data from object to memory
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_filearray(
        nai_bufwalk_t* i, nai_filearray_t* a, nai_filevec_t* v, nai_int_t count, size_t limit, nai_int_t message);

    /**
 * fill the space of buffer list into a buffer vector
 * @param   i       pointer to the buffer iterator
 * @param   v       pointer to a buffer vector
 * @param   limit   maximum the number of space bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of space bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_wbufvec(nai_bufwalk_t* i, nai_bufvec_t* v, size_t limit, nai_int_t message);

    /**
 * fill the space of buffer list into a buffer array
 * @param   i       pointer to the buffer iterator
 * @param   a       pointer to a buffer array which is saved fill result
 * @param   v       pointer to an array of buffer vector
 * @param   count   the count of buffer vector
 * @param   limit   maximum the number of space bytes
 * @param   message indicates that it is a message type
 * @retval  >=0     the number of space bytes is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_bufwalk_to_wbufarray(
        nai_bufwalk_t* i, nai_bufarray_t* a, nai_bufvec_t* v, nai_int_t count, size_t limit, nai_int_t message);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
