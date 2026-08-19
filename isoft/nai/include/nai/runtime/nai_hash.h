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
/// @file       nai_hash.h
/// @brief      the hash map
/// @details
/// @date       2021-01-27
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is a hash map, 
 *          the user is responsible for memory management.
 * @details this implementation supports memory pool or malloc 
 *          for memory allocation, depending on whether 
 *          it is passed to the memory pool during construction.
 *
 * @details the code example is as follows:
 * 
 * @par     declare an user node
 * @code
 *          typedef struct my_node_s {
 *              nai_hash_str_t ent;
 *              nai_str_t value;
 *          } my_node_t;
 * @endcode
 *
 * @par     initial a string hash map
 * @code
 *          nai_int_t r;
 *          nai_hashmap_t t;
 *          nai_hashmap_init(&t);
 *
 *          r = nai_hashmap_open(&t, 17, NAI_HASH_STRING, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     find a node in the hash map
 * @code
 *          nai_str_t key;
 *          nai_hashmap_t* h;
 *          nai_hash_elt_t* n;
 *          my_node_t* e;
 *
 *          n = nai_hashmap_str_find(h, &key, 0);
 *          if (n == 0) {
 *              // not found
 *              goto _done;
 *          };
 *
 *          e = nai_containof(n, my_node_t, ent);
 *          printf("found: %.*s\n", 
 *              nai_str_len(&e->value), nai_str(&e->value));
 * @endcode
 *
 * @par     insert or remove a node into the hash map
 * @code
 *          nai_str_t key;
 *          nai_str_t val;
 *          nai_hashmap_t* h;
 *          my_node_t* e;
 *
 *          // create a new node
 *          e = (my_node_t*)nai_malloc(sizeof(*e));
 *          if (e == 0) {
 *              goto _fail;
 *          };
 *
 *          e->key = key;
 *          e->value = val;
 *
 *          // insert
 *          nai_str_hash(&e->key, 0);
 *          nai_hashmap_insert(h, (nai_hash_elt_t*)&e->ent);
 *
 *          // remove
 *          nai_hashmap_remove(h, (nai_hash_elt_t*)&e->ent);
 * @endcode
 *
 * @par     enumerate nodes in the hash map
 * @code
 *          my_node_t* n;
 *          nai_hashmap_t* h;
 *          nai_hash_elt_t* e = nai_hashmap_next(h, 0);
 *
 *          for ( ; e; ) {
 *              n = nai_containof(e, my_node_t, ent);
 *              e = nai_hashmap_next(h, e);
 *              // process data here
 *          };
 * @endcode
 *
 * @par     build a hashsnap
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_int_t hashsize;
 *          nai_int_t bucket;
 *          nai_int_t count;
 *          nai_hash_value_t* values;
 *          nai_hashsnap_t snap;
 *
 *          nai_hashsnap_init(&snap, 0);
 *
 *          r = nai_hashsnap_build(&snap, values, count, hashsize, bucket);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != EOVERFLOW) {
 *                  // other errors
 *                  goto _fail;
 *              };
 *
 *              // the hashsize or bucket is not big enough
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     find a node in the hashsnap
 * @code
 *          void* value;
 *          nai_str_t key;
 *          nai_hashsnap_t* snap;
 *
 *          value = nai_hashsnap_find(snap, &key, 0);
 *          if (value == 0) {
 *              // not found
 *              goto _done;
 *          };
 * @endcode
 */

#ifndef _HASH_H_NAI
#define _HASH_H_NAI

#pragma once

#include "nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// hash map

/**
 * @anchor  NAI_HASH_TYPE
 * @name    NAI_HASH_TYPE       the type of the hash
 * @{
 */
#define NAI_HASH_T32 0 /**< 32bits hash key */
#define NAI_HASH_T64 1 /**< 64bits hash key */
/** @} */

/**
 * @anchor  NAI_HASH_KEY
 * @name    NAI_HASH_KEY        the type of the hash key
 * @{
 */
#define NAI_HASH_STRING 0 /**< string type hash element */
#define NAI_HASH_INT32  1 /**< int32 type hash element */
#define NAI_HASH_INT64  2 /**< int64 type hash element */
#if NAI_SIZEOF_VOID_P == 4
    #define NAI_HASH_INTPTR NAI_HASH_INT32
#elif NAI_SIZEOF_VOID_P == 8
    #define NAI_HASH_INTPTR NAI_HASH_INT64
#else
    #error "unknown size of void*"
#endif
    /** @} */

#ifndef _NAI_TYPEDEF_HASH_ELT_T
    #define _NAI_TYPEDEF_HASH_ELT_T
    typedef struct nai_hash_elt_s nai_hash_elt_t;
#endif
#ifndef _NAI_TYPEDEF_HASH_STR_T
    #define _NAI_TYPEDEF_HASH_STR_T
    typedef struct nai_hash_str_s nai_hash_str_t;
#endif
#ifndef _NAI_TYPEDEF_HASH_INT32_T
    #define _NAI_TYPEDEF_HASH_INT32_T
    typedef struct nai_hash_int32_s nai_hash_int32_t;
#endif
#ifndef _NAI_TYPEDEF_HASH_INT64_T
    #define _NAI_TYPEDEF_HASH_INT64_T
    typedef struct nai_hash_int64_s nai_hash_int64_t;
#endif

    /**
 * the structure of the hash element
 */
    struct nai_hash_elt_s
    {
        nai_hash_elt_t* next; /**< pointer to the next hash element */
    };

    /**
 * the structure of the hash element with string key
 */
    struct nai_hash_str_s
    {
        nai_hash_str_t* next; /**< pointer to the next hash element */
        nai_str_t key;        /**< the string key */
    };

    /**
 * the structure of the hash element with int32 key
 */
    struct nai_hash_int32_s
    {
        nai_hash_int32_t* next; /**< pointer to the next hash element */
        union
        {
#if NAI_SIZEOF_VOID_P == 4
            void* p; /**< the pointer key */
#endif
            int32_t i;  /**< the int32 key */
            uint32_t u; /**< the uint32 key */
        } key;
    };

    /**
 * the structure of the hash element with int64 key
 */
    struct nai_hash_int64_s
    {
        nai_hash_int64_t* next; /**< pointer to the next hash element */
        union
        {
#if NAI_SIZEOF_VOID_P == 8
            void* p; /**< the pointer key */
#endif
            int64_t i;  /**< the int64 key */
            uint64_t u; /**< the uint64 key */
        } key;
    };

#ifndef _NAI_TYPEDEF_HASH_INTPTR_T
    #define _NAI_TYPEDEF_HASH_INTPTR_T
    #if NAI_SIZEOF_VOID_P == 4
    typedef struct nai_hash_int32_s nai_hash_intptr_t;
    #elif NAI_SIZEOF_VOID_P == 8
    typedef struct nai_hash_int64_s nai_hash_intptr_t;
    #else
        #error "unknown size of void*"
    #endif
#endif

#ifndef _NAI_TYPEDEF_HASHMAP_T
    #define _NAI_TYPEDEF_HASHMAP_T
    typedef struct nai_hashmap_s nai_hashmap_t;
#endif

    /**
 * the structure of the hash map
 */
    struct nai_hashmap_s
    {
        nai_pool_t* pool;      /**< pointer to the memory pool */
        nai_hash_elt_t** hash; /**< pointer to the hash array */
        uint32_t hashsize;     /**< the size of the hash array */
        uint16_t hash_voff;    /**< the offset of the hash */
        uint16_t hash_vtype;   /**< the type of the hash, see @ref 
                                     NAI_HASH_TYPE */
    };

    /**
 * initial the hash map
 * @param   p       pointer to the hash map
 * @param   pool    pointer to the memory pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no pool is given, the map will use nai_malloc to allocate.
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_init(nai_hashmap_t* p, nai_pool_t* pool);

    /**
 * open the hash map
 * @param   p       pointer to the hash map
 * @param   hsize   the size of the hash map
 * @param   type    the type of the hash key, see @ref NAI_HASH_KEY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_open(nai_hashmap_t* p, size_t hsize, nai_int_t type);

    /**
 * open the hash map with custom type
 * @param   p       pointer to the hash map
 * @param   hsize   the size of the hash map
 * @param   hoff    the offset of the hash
 * @param   htype   the type of the hash, see @ref NAI_HASH_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_open_user(nai_hashmap_t* p, size_t hsize, uint16_t hoff, uint16_t htype);

    /**
 * close the hash map
 * @param   p       pointer to the hash map
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_close(nai_hashmap_t* p);

    /**
 * insert a new hash element to the hash map
 * @param   p       pointer to the hash map
 * @param   e       pointer to the hash element
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_insert(nai_hashmap_t* p, nai_hash_elt_t* e);

    /**
 * remove a hash element from the hash map
 * @param   p       pointer to the hash map
 * @param   e       pointer to the hash element
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashmap_remove(nai_hashmap_t* p, nai_hash_elt_t* e);

    /**
 * get the next element of the specified element
 * @param   p       pointer to the hash map
 * @param   e       pointer to the hash element
 * @return  the address of the next element, 
 *          if the next element is not exists, null is returned.
 * @note    if e is null, returned the first element in the hash map
 */
    NAI_EXTERN
    nai_hash_elt_t* nai_hashmap_next(nai_hashmap_t* p, nai_hash_elt_t* e);

    /**
 * find a hash element from the string hash map
 * @param   p       pointer to the hash map
 * @param   s       pointer to the string key
 * @param   cases   character case sensitivity
 * @return  the address of the found element, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    nai_hash_elt_t* nai_hashmap_str_find(nai_hashmap_t* p, const nai_str_t* s, nai_int_t cases);

    /**
 * find a hash element from the pointer hash map
 * @param   p       pointer to the hash map
 * @param   key     the pointer key
 * @return  the address of the found element, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    nai_hash_elt_t* nai_hashmap_ptr_find(nai_hashmap_t* p, const void* key);

    /**
 * find a hash element from the pointer hash map
 * @param   p       pointer to the hash map
 * @param   key     the int32 key
 * @return  the address of the found element, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    nai_hash_elt_t* nai_hashmap_int32_find(nai_hashmap_t* p, uint32_t key);

    /**
 * find a hash element from the pointer hash map
 * @param   p       pointer to the hash map
 * @param   key     the uint32 key
 * @return  the address of the found element, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    nai_hash_elt_t* nai_hashmap_int64_find(nai_hashmap_t* p, uint64_t key);

#if NAI_SIZEOF_VOID_P == 4
    #define nai_hashmap_intptr_find nai_hash_int32_find;
#elif NAI_SIZEOF_VOID_P == 8
    #define nai_hashmap_intptr_find nai_hash_int64_find;
#else
    #error "unknown size of void*"
#endif

    //////////////////////////////////////////////////////////////////////////////
    // hash snap

#ifndef _NAI_TYPEDEF_HASH_VALUE_T
    #define _NAI_TYPEDEF_HASH_VALUE_T
    typedef struct nai_hash_value_s nai_hash_value_t;
#endif
#ifndef _NAI_TYPEDEF_HASHSNAP_T
    #define _NAI_TYPEDEF_HASHSNAP_T
    typedef struct nai_hashsnap_s nai_hashsnap_t;
#endif

    struct nai_hash_value_s
    {
        nai_str_t key;
        void* value;
    };

    /**
 * the structure of the hash snapshot
 */
    struct nai_hashsnap_s
    {
        nai_pool_t* pool;  /**< pointer to the memory pool */
        uint32_t* snap;    /**< pointer to the snapshot of the hash 
                                     array */
        uint32_t hashsize; /**< the size of the hash array */
    };

    /**
 * initial the hash snapshot
 * @param   p       pointer to the hash snapshot
 * @param   pool    pointer to the memory pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no pool is given, the snapshot will use nai_malloc to allocate.
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_hashsnap_init(nai_hashsnap_t* p, nai_pool_t* pool);

    /**
 * close the hash snapshot
 * @param   p       pointer to the hash snapshot
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_hashsnap_close(nai_hashsnap_t* p);

    /**
 * build the hash snapshot
 * @param   p       pointer to the hash snapshot
 * @param   elts    pointer to the array of the keys and values
 * @param   count   the length of the array
 * @param   hsize_max the max size of the hash array
 * @param   bucket  the size of the hash bucket
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the caller needs to set the hash of the key value before calling
 */
    NAI_EXTERN
    nai_int_t nai_hashsnap_build(
        nai_hashsnap_t* p, const nai_hash_value_t* elts, nai_int_t count, nai_int_t hsize_max, size_t bucket);

    /**
 * find a value from the hash snapshot
 * @param   p       pointer to the hash snapshot
 * @param   str     pointer to the string key
 * @param   cases   character case sensitivity
 * @return  the address of the value, 
 *          if the key is not exists, null is returned.
 */
    NAI_EXTERN
    void* nai_hashsnap_find(nai_hashsnap_t* p, const nai_str_t* str, nai_int_t cases);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
