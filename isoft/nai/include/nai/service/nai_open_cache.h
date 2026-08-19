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
/// @file       nai_open_cache.h
/// @brief      
/// @details
/// @date       2022-04-12
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _OPEN_CACHE_H_NAI
#define _OPEN_CACHE_H_NAI

#pragma once

#include "nai/os/nai_file.h"
#include "nai/os/nai_stat.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 
 * @anchor  NAI_OPEN_VERIFY
 * @name    NAI_OPEN_VERIFY     the flags of open verify
 * @{
 */

#define NAI_OPEN_VERIFY_OFF   0x0 /**< disable verify */
#define NAI_OPEN_VERIFY_LINK  0x1 /**< verify to deny the links */
#define NAI_OPEN_VERIFY_OWNER 0x2 /**< verify to deny the different owner */

    /** @} */

#ifndef _NAI_TYPEDEF_OPEN_CACHE_FILE_T
    #define _NAI_TYPEDEF_OPEN_CACHE_FILE_T
    typedef struct nai_open_cache_file_s nai_open_cache_file_t;
#endif
#ifndef _NAI_TYPEDEF_OPEN_CACHE_T
    #define _NAI_TYPEDEF_OPEN_CACHE_T
    typedef struct nai_open_cache_s nai_open_cache_t;
#endif
#ifndef _NAI_TYPEDEF_OPEN_FILE_T
    #define _NAI_TYPEDEF_OPEN_FILE_T
    typedef struct nai_open_file_s nai_open_file_t;
#endif
#ifndef _NAI_TYPEDEF_OPEN_ATTR_T
    #define _NAI_TYPEDEF_OPEN_ATTR_T
    typedef struct nai_open_attr_s nai_open_attr_t;
#endif

    /**
 * the structure of the open attribute
 */
    struct nai_open_attr_s
    {
        nai_stat_t st;       /**< the file stat */
        nai_open_file_t* of; /**< pointer to the opened file */

        union
        {
            struct
            {
                uint32_t verify : 2; /**< the flags of verify, 
                                     see @ref NAI_OPEN_VERIFY */
            };
            uint32_t flags; /**< the open flags */
        };

        size_t root_length; /**< the specified root length to skip verify */
        size_t valid_timeo; /**< the specified expire time */
        uint64_t directio;  /**< the min size to enable directio */
    };

/** 
 * @name    nai_open_attr_defines   the functions of the open attribute
 * @{
 */

/**
 * initial the open attribute
 * @param   p       pointer to the open attribute
 * @param   o       pointer to the opened file
 * @return  void
 */
#define nai_open_attr_init(p, o)                                                                                       \
    {                                                                                                                  \
        (p)->of          = (o);                                                                                        \
        (p)->flags       = 0;                                                                                          \
        (p)->root_length = -1;                                                                                         \
        (p)->valid_timeo = -1;                                                                                         \
        (p)->directio    = -1;                                                                                         \
    }

/**
 * set the expire time
 * @param   p       pointer to the open attribute
 * @param   t       the value of expire time
 * @return  void
 */
#define nai_open_attr_set_valid(p, t)                                                                                  \
    {                                                                                                                  \
        (p)->valid_timeo = (t);                                                                                        \
    }

/**
 * set the verify options
 * @param   p       pointer to the open attribute
 * @param   v       the verify flags
 * @param   l       the specified root length to skip verify
 * @return  void
 */
#define nai_open_attr_set_verify(p, v, l)                                                                              \
    {                                                                                                                  \
        (p)->verify      = (v);                                                                                        \
        (p)->root_length = (l);                                                                                        \
    }

/**
 * to disable verify
 * @param   p       pointer to the open attribute
 * @return  void
 */
#define nai_open_attr_set_verify_off(p)                                                                                \
    {                                                                                                                  \
        (p)->verify      = NAI_OPEN_VERIFY_OFF;                                                                        \
        (p)->root_length = -1;                                                                                         \
    }

/**
 * to enalbe verfiy link only
 * @param   p       pointer to the open attribute
 * @param   l       the specified root length to skip verify
 * @return  void
 */
#define nai_open_attr_set_verify_link(p, l)                                                                            \
    {                                                                                                                  \
        (p)->verify      = NAI_OPEN_VERIFY_LINK;                                                                       \
        (p)->root_length = (l);                                                                                        \
    }

/**
 * to enalbe verfiy owner only
 * @param   p       pointer to the open attribute
 * @param   l       the specified root length to skip verify
 * @return  void
 */
#define nai_open_attr_set_verify_owner(p, l)                                                                           \
    {                                                                                                                  \
        (p)->verify      = NAI_OPEN_VERIFY_OWNER;                                                                      \
        (p)->root_length = (l);                                                                                        \
    }

    /** @} */

    /**
 * the sturcture of the opened file
 */
    struct nai_open_file_s
    {
        nai_int_t oflags;          /**< the open flags */
        nai_fd_t fd;               /**< the file discriptor */
        nai_open_cache_file_t* cf; /**< pointer to the file cache */
    };

/** 
 * @name    nai_open_file_defines   the functions of the opened file
 * @{
 */

/**
 * initial the opened file
 * @param   p       pointer to the opened file
 * @return  void
 */
#define nai_open_file_init(p)                                                                                          \
    {                                                                                                                  \
        (p)->oflags = 0;                                                                                               \
        (p)->fd     = NAI_FD_INVALID;                                                                                  \
        (p)->cf     = 0;                                                                                               \
    }

    /** @} */

    /**
 * close the opened file
 * @param   of      pointer to the opened file
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_open_file_close(nai_open_file_t* of);

    /**
 * add the opened file into the list of cleanup
 * @param   of      pointer to the opened file
 * @param   pool    pointer to the pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_open_file_add_cleanup(nai_open_file_t* of, nai_pool_t* pool);

    /**
 * the structure of the opened cache
 */
    struct nai_open_cache_s
    {
        nai_rbtree_t cmap; /**< the cache map */
        nai_list_t queue;  /**< the expire queue */

        size_t count;        /**< the count of cache */
        size_t max_count;    /**< the max count of cache */
        size_t min_use;      /**< the min use times before close fd */
        size_t expire_timeo; /**< the cache expire time */
        uint32_t oflags;     /**< the default open flags */
    };

/** 
 * @name    nai_open_cache_defines  the functions of the opened cache
 * @{
 */

/**
 * set the default open flags
 * @param   p       pointer to the opened cache
 * @param   f       the open flags
 * @return  void
 */
#define nai_open_cache_set_oflags(p, f)                                                                                \
    {                                                                                                                  \
        (p)->oflags = (f);                                                                                             \
    }

/**
 * set the cache expire time
 * @param   p       pointer to the opened cache
 * @param   t       the value of cache expire time
 * @return  void
 */
#define nai_open_cache_set_expire(p, t)                                                                                \
    {                                                                                                                  \
        (p)->expire_timeo = (t);                                                                                       \
    }

/**
 * set the min use times
 * @param   p       pointer to the opened cache
 * @param   m       the value of min use times
 * @return  void
 */
#define nai_open_cache_set_min_use(p, m)                                                                               \
    {                                                                                                                  \
        (p)->min_use = (m);                                                                                            \
    }

/**
 * set the max count of cache
 * @param   p       pointer to the opened cache
 * @param   m       the value of max cache
 * @return  void
 */
#define nai_open_cache_set_max_count(p, m)                                                                             \
    {                                                                                                                  \
        (p)->max_count = (m);                                                                                          \
    }

    /** @} */

    /**
 * initial the opened cache
 * @param   p       pointer to the opened cache
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_open_cache_init(nai_open_cache_t* p);

    /**
 * open the opened cache
 * @param   p       pointer to the opened cache
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_open_cache_open(nai_open_cache_t* p);

    /**
 * close the opened cache
 * @param   p       pointer to the opened cache
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_open_cache_close(nai_open_cache_t* p);

    /**
 * open and stat the specified path
 * @param   p       pointer to the opened cache
 * @param   path    pointer to the string view of specifed path
 * @param   opt     pointer to the open attribute
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the result is store in the member 'st' and 'of' of 
 *          the open attribute
 */
    NAI_EXTERN
    nai_int_t nai_open_and_stat(nai_open_cache_t* p, const nai_str_t* path, nai_open_attr_t* opt);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
