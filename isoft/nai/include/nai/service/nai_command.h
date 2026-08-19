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
/// @file       nai_command.h
/// @brief      configuration command definition and general command implementation
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _COMMAND_H_NAI
#define _COMMAND_H_NAI

#pragma once

#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_value.h"
#include "nai_module.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 
 * @anchor  NAI_OPT_UNSET
 * @name    NAI_OPT_UNSET       the unset value of types
 * @{
 */

#define NAI_OPT_UNSET_VALUE  (-1)
#define NAI_OPT_UNSET_SIZE   ((size_t)-1)
#define NAI_OPT_UNSET_MSEC   ((uint64_t)-1)
#define NAI_OPT_UNSET_UINT32 ((uint32_t)-1)
#define NAI_OPT_UNSET_UINT64 ((uint64_t)-1)

/** @} */

/** 
 * @anchor  NAI_OPT_INT
 * @name    NAI_OPT_INT         the options of integer
 * @{
 */

/** verify function */
#define NAI_OPT_INT_ANY    (0)                /**< the value can be any */
#define NAI_OPT_INT_MIN(n) (void*)((n)*4 + 1) /**< the min value */
#define NAI_OPT_INT_MAX(n) (void*)((n)*4 + 2) /**< the max value */

/** @} */

/** 
 * @anchor  NAI_OPT_ARRAY
 * @name    NAI_OPT_ARRAY       the options of array
 * @{
 */

/* the type of array */
#define NAI_OPT_ARRAY_STR (0) /**< an array of string */

/** @} */

/** 
 * @anchor  NAI_COPT
 * @name    NAI_COPT            the options of command
 * @{
 */

/* command location */
#define NAI_CLOC_ANY 0x000000ff /**< any configuration location */

/* command options */
#define NAI_COPT_SEGEMENT 0x00000100 /**< is a segment instruction */

/* command args */
#define NAI_COPT_ARG0    0x00010000 /**< take 0 args */
#define NAI_COPT_ARG1    0x00020000 /**< take 1 args */
#define NAI_COPT_ARG2    0x00040000 /**< take 2 args */
#define NAI_COPT_ARG3    0x00080000 /**< take 3 args */
#define NAI_COPT_ARG4    0x00100000 /**< take 4 args */
#define NAI_COPT_ARG5    0x00200000 /**< take 5 args */
#define NAI_COPT_ARG6    0x00400000 /**< take 6 args */
#define NAI_COPT_ARG7    0x00800000 /**< take 7 args */
#define NAI_COPT_ARGMASK 0x00ff0000 /**< mask of args taken */

#define NAI_COPT_ARGANY   0x00ff0000 /**< accept 0-7 parameters */
#define NAI_COPT_ARG1MORE 0x00fe0000 /**< take at least 1 args */
#define NAI_COPT_ARG2MORE 0x00fc0000 /**< take at least 2 args */
#define NAI_COPT_ARG3MORE 0x00f80000 /**< take at least 3 args */

/* command local index */
#define NAI_COPT_LOCAL0    0x00000000 /**< local data at index 0 */
#define NAI_COPT_LOCAL1    0x01000000 /**< local data at index 1 */
#define NAI_COPT_LOCAL2    0x02000000 /**< local data at index 2 */
#define NAI_COPT_LOCAL3    0x03000000 /**< local data at index 3 */
#define NAI_COPT_LOCALMASK 0x03000000

    /** @} */

#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_T
    #define _NAI_TYPEDEF_COMMAND_T
    typedef struct nai_command_s nai_command_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_ARGS_T
    #define _NAI_TYPEDEF_COMMAND_ARGS_T
    typedef struct nai_command_args_s nai_command_args_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_HANDLE_F
    #define _NAI_TYPEDEF_COMMAND_HANDLE_F
    typedef nai_sult_t (*nai_command_handle_f)(nai_command_t*, nai_command_args_t*);
#endif

    /**
 * the structure of the command arguments
 */
    struct nai_command_args_s
    {
        nai_main_t* main;       /**< the main */
        nai_pool_t* pool;       /**< the memory pool */
        nai_pool_t* pool_tmp;   /**< the temporary pool in configuration */
        nai_local_t** clocal;   /**< the array of local */
        nai_value_t* argv;      /**< the arguments of command */
        nai_int_t argc;         /**< the number of the arguments */
        nai_int_t grp;          /**< the group of command */
        uint32_t grp_mask : 16; /**< the mask of group location of command */
        uint32_t endseg : 1;    /**< is end segment call */
        uint8_t* data;          /**< the data of command */
    };

    /**
 * the structure of the command
 */
    struct nai_command_s
    {
        const char* name;            /**< the name of command */
        nai_command_handle_f handle; /**< the handler of command */
        uint32_t flags;              /**< the flags of command, see @ref NAI_COPT */
        size_t offset;               /**< the value offset in struct */
        void* ctx;                   /**< the context of command */
    };

    /** 
 * @name    nai_command_defines the functions of command
 * @{
 */

#define nai_command(c, a) (c)->handle((c), (a))

    /** @} */

#ifndef _NAI_TYPEDEF_BUFSIZE_T
    #define _NAI_TYPEDEF_BUFSIZE_T
    typedef struct nai_bufsize_s nai_bufsize_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_OPTINFO_T
    #define _NAI_TYPEDEF_COMMAND_OPTINFO_T
    typedef struct nai_command_optinfo_s nai_command_optinfo_t;
#endif

    /**
 * the structure of the multi-buffer configuration
 */
    struct nai_bufsize_s
    {
        size_t size; /**< the size of buffer */
        size_t num;  /**< the number of buffer */
    };

    /**
 * the structure of the predefined options
 */
    struct nai_command_optinfo_s
    {
        nai_str_t name;  /**< the name of option */
        nai_int_t value; /**< the value of option */
    };

    /**
 * parse a offset from string
 * @param   p       pointer to a nai_off_t
 * @param   s       pointer to the string
 * @param   len     the length of string, <0 means null-terminated string
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    format examples:
 *          1. 1.5g     # 1.5 gb
 *          2. 500k     # 500 kb
 *          3. 1.25kb   # 1.25 kb
 *          4. 512      # 512 b
 */
    NAI_EXTERN
    nai_int_t nai_parse_off(nai_off_t* p, const char* s, size_t len);

    /**
 * parse a size from string
 * @param   p       pointer to a size_t
 * @param   s       pointer to the string
 * @param   len     the length of string, <0 means null-terminated string
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    format examples:
 *          1. 1.5g     # 1.5 gb
 *          2. 500k     # 500 kb
 *          3. 1.25kb   # 1.25 kb
 *          4. 512      # 512 b
 */
    NAI_EXTERN
    nai_int_t nai_parse_size(size_t* p, const char* s, size_t len);

    /**
 * parse a timeval in msec from string
 * @param   p       pointer to a uint64_t
 * @param   s       pointer to the string
 * @param   len     the length of string, <0 means null-terminated string
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    suffix: y, M, d, h, m, s, ms
 * @note    format examples:
 *          1. 1y       # 1 year
 *          2. 1y5d     # 1 year + 5 day
 *          3. 2M       # 2 month
 *          4. 500ms    # 500 msec
 *          5. 10       # 10 sec
 */
    NAI_EXTERN
    nai_int_t nai_parse_timeval(uint64_t* p, const char* s, size_t len);

    /** 
 * @name    nai_command_defines the functions of command
 * @{
 */

#define nai_command_set_int nai_command_set_int32

#if NAI_SIZEOF_VOID_P == 4
    #define nai_command_set_intptr nai_command_set_int32
#else
    #define nai_command_set_intptr nai_command_set_int64
#endif

    /** @} */

    /**
 * the command is used for set a string
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command 
 *          is nai_str_t or nai_mem_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_string(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set an array
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is nai_array.
 * @note    the ctx of the command can be set as one of @ref NAI_OPT_ARRAY
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_array(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a offset
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is nai_off_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_off(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a size
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is size_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_size(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a bufsize
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is nai_bufsize_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_bufsize(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a option
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is nai_int_t.
 * @note    the ctx of the command can be set as an array of 
 *          nai_command_optinfo_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_opts(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a int32
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is int32_t.
 * @note    the ctx of the command can be set as one of @ref NAI_OPT_INT.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_int32(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a int64
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is int64_t.
 * @note    the ctx of the command can be set as one of @ref NAI_OPT_INT.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_int64(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a timeval in sec
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is time_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_sec(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a timeval in msec
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is uint64_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_msec(nai_command_t* c, nai_command_args_t* args);

    /**
 * the command is used for set a timeval in msec
 * @param   c       pointer to the command
 * @param   args    pointer to the command args
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the type of value which's offset in the command is uint32_t.
 */
    NAI_EXTERN
    nai_sult_t nai_command_set_msec32(nai_command_t* c, nai_command_args_t* args);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
