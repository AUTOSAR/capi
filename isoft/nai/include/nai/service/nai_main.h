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
/// @file       nai_main.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MAIN_H_NAI
#define _MAIN_H_NAI

#pragma once

#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_string.h"
#include "nai_module.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * @anchor  NAI_MAIN_CLOC
 * @name    NAI_MAIN_CLOC       the options of main location
 * @{
 */

#define NAI_CLOC_MAIN  0x01
#define NAI_CLOC_EVENT 0x02
#define NAI_CLOC_DNS   0x04
#define NAI_CLOC_AGENT 0x08

/** @} */

/**
 * @name    NAI_MAIN_TAG
 * @{
 */
#define NAI_TAG1(x)     ((uint8_t)(x))
#define NAI_TAG2(x)     ((uint8_t)(x) << 8)
#define NAI_MODULE_MAIN (NAI_TAG1('m') + NAI_TAG2('a'))

    /** @} */

#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_MAIN_CONF_T
    #define _NAI_TYPEDEF_MAIN_CONF_T
    typedef struct nai_main_conf_s nai_main_conf_t;
#endif

    struct nai_main_s
    {
        nai_pool_t pool;          /**< the memory pool of main */
        nai_str_t root_path;      /**< the path of work directory */
        nai_str_t conf_path;      /**< the path of configuration directory */
        nai_local_t* local;       /**< the local data of main */
        nai_main_conf_t* conf;    /**< the configuration context */
        nai_module_map_t modules; /**< the loaded modules */
        uint32_t inited : 1;      /**< mark of initialize */
        uint32_t prepare : 2;     /**< mark of prepare stage */
        uint32_t start : 1;       /**< mark of start stage */
    };

/**
 * @name    nai_main_defines    the functions of main
 * @{
 */

/**
 * get the array of slot
 * @param   m       pointer to the main
 * @return  the array of slot which is used for construct local
 */
#define nai_main_slot(m) ((m)->local->slot[0])

/**
 * get the data reference of the module from main local
 * @param   m       pointer to the main
 * @param   mod     the module
 * @return  the data reference of the module
 */
#define nai_main_local_at(m, mod) nai_global_at((m)->local, mod)

    /** @} */

    /**
 * initialize the main
 * @param   m       pointer to the main
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_main_init(nai_main_t* m);

    /**
 * set work and conf directory
 * @param   m       pointer to the main
 * @param   root    pointer to the path of work directory
 * @param   conf    pointer to the path of conf directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_root(nai_main_t* m, const char* root, const char* conf);

    /**
 * set option without any argument
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_optv(nai_main_t* m, const char* name);

    /**
 * set option with an integer argument
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @param   v       an integer value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_opti(nai_main_t* m, const char* name, intptr_t v);

    /**
 * set option with a float argument
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @param   v       a float value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_optf(nai_main_t* m, const char* name, float v);

    /**
 * set option with a double argument
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @param   v       a double value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_optd(nai_main_t* m, const char* name, double v);

    /**
 * set option with a string argument
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @param   str     a string value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_opts(nai_main_t* m, const char* name, const char* str);

    /**
 * set option with a string of multi-arguments
 * @param   m       pointer to the main
 * @param   name    pointer to the string of option name
 * @param   str     a string of multi-arguments which separated by spaces
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_optm(nai_main_t* m, const char* name, const char* str);

    /**
 * set option with a command line
 * @param   m       pointer to the main
 * @param   str     pointer to a command line which separated by spaces
 * @param   size    the length of the command line
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_line(nai_main_t* m, const char* str, size_t size);

    /**
 * set option with a format line
 * @param   m       pointer to the main
 * @param   fmt     pointer to the format string
 * @param   ...     the arguments of format
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_fmts(nai_main_t* m, const char* fmt, ...) nai_fmt_attribute(2, 3);

    /**
 * set option with multi command lines
 * @param   m       pointer to the main
 * @param   str     pointer to the string of multi command lines
 * @param   size    the length of the string
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_main_set_block(nai_main_t* m, const char* str, size_t size);

    /**
 * add a module to the main
 * @param   m       pointer to the main
 * @param   mod     pointer to the module
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_main_add_module(nai_main_t* m, nai_module_t* mod);

    /**
 * start main
 * @param   m       pointer to the main
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    main does not create io threads, 
 *          users need to run the event loop of main after startup
 */
    NAI_EXTERN
    nai_int_t nai_main_open(nai_main_t* m);

    /**
 * reuse the main, close but keep all setting
 * @param   m       pointer to the main
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    users need to stop the event loop of main before reuse
 */
    NAI_EXTERN
    nai_int_t nai_main_reuse(nai_main_t* m);

    /**
 * close the main
 * @param   m       pointer to the main
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    users need to stop the event loop of main before close
 */
    NAI_EXTERN
    nai_int_t nai_main_close(nai_main_t* m);

    /**
 * get the full path based on the work directory
 * @param   m       pointer to the main
 * @param   s       pointer to the string view to get path
 * @param   path    pointer to the relative path
 * @param   len     the length of the relative path, 
 *                  < 0 means path is a null-terminated string
 * @param   pool    pointer to the memory pool, can be null
 * @retval  >=0     the number of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_main_get_full_path(nai_main_t* m, nai_str_t* s, const char* path, size_t len, nai_pool_t* pool);

    /**
 * get the full path based on the work directory
 * @param   m       pointer to the main
 * @param   buf     pointer to the buffer
 * @param   buflen  the length of the buffer
 * @param   path    pointer to the relative path
 * @param   len     the length of the relative path, 
 *                  < 0 means path is a null-terminated string
 * @retval  >=0     the number of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'len' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'len' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_main_copy_full_path(nai_main_t* m, char* buf, size_t buflen, const char* path, size_t len);

    /**
 * get the temporary memory pool of configuration
 * @param   m       pointer to the main
 * @return  the address of the temporary pool on success, 
 *          null is returned on failure, see #nai_errno
 * @note    this pool is only used during configuration, 
 *          in other words, between the first option setting and start.
 */
    NAI_EXTERN
    nai_pool_t* nai_main_get_temp_pool(nai_main_t* m);

    /**
 * allocate a new local data
 * @param   m       pointer to the main
 * @param   modules number of module slots for local data
 * @param   pool    pointer to a memory pool
 * @return  the address of the new local on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_local_t* nai_main_local_alloc(nai_main_t* m, nai_int_t modules, nai_pool_t* pool);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
