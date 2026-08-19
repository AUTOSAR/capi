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
/// @file       nai_module.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MODULE_H_NAI
#define _MODULE_H_NAI

#pragma once

#include "nai/runtime/nai_array.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 
 * @anchor  NAI_MODULE_STAGE
 * @name    NAI_MODULE_STAGE    the stage of module
 * @{
 */

#define NAI_MODULE_INIT    0
#define NAI_MODULE_PREPARE 1
#define NAI_MODULE_START   2
#define NAI_MODULE_STOP    3
#define NAI_MODULE_CLEANUP 4

    /** @} */

    /** 
 * @anchor  NAI_MODULE_HEADER
 * @name    NAI_MODULE_HEADER   the header of module
 * @{
 */

#define NAI_MODULE_HEADER 0

    /** @} */

#ifndef _NAI_TYPEDEF_SLOT_T
    #define _NAI_TYPEDEF_SLOT_T
    typedef uint16_t nai_slot_t;
#endif
#ifndef _NAI_TYPEDEF_LOCAL_T
    #define _NAI_TYPEDEF_LOCAL_T
    typedef struct nai_local_s nai_local_t;
#endif

    struct nai_local_s
    {
        union
        {
            void* data[1];       /**< the array of local data */
            nai_slot_t* slot[1]; /**< the array of module slot */
        };
    };

/** 
 * @name    nai_local_defines   the functions of local
 * @{
 */

/**
 * get the slot of the module
 * @param   l       pointer to the local
 * @param   m       the module
 * @return  the value of slot
 */
#define nai_local_slot(l, m) ((l)->slot[0][(m).index])

/**
 * get local data reference of the module
 * @param   l       pointer to the local
 * @param   m       the module
 * @return  the data reference of the module
 */
#define nai_local_at(l, m) ((l)->data[nai_local_slot(l, m)])

/**
 * get global data reference of the module
 * @param   l       pointer to the local
 * @param   m       the module
 * @return  the data reference of the module
 */
#define nai_global_at(l, m) ((l)->data[(m).index])

    /** @} */

#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_MODULE_T
    #define _NAI_TYPEDEF_MODULE_T
    typedef struct nai_module_s nai_module_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_T
    #define _NAI_TYPEDEF_COMMAND_T
    typedef struct nai_command_s nai_command_t;
#endif

    /**
 * the structure of the module
 */
    struct nai_module_s
    {
        nai_int_t index;         /**< the global index of module */
        nai_int_t grp;           /**< the group id of module */
        nai_command_t* commands; /**< the commands of module */
        const char* name;        /**< the module name */
        void* ctx;               /**< the module context */

        /* module functions */
        struct
        {
            nai_sult_t (*init)(nai_main_t*);    /**< init handler */
            nai_sult_t (*prepare)(nai_main_t*); /**< prepare handler */
            nai_sult_t (*start)(nai_main_t*);   /**< start handler */
            nai_sult_t (*stop)(nai_main_t*);    /**< stop handler */
            nai_sult_t (*cleanup)(nai_main_t*); /**< cleanup handler */
        };
    };

#ifndef _NAI_TYPEDEF_MODULE_MAP_T
    #define _NAI_TYPEDEF_MODULE_MAP_T
    typedef struct nai_module_map_s nai_module_map_t;
#endif

    /**
 * the structure of the module map
 */
    struct nai_module_map_s
    {
        nai_array_t a;        /**< the array of the module map */
        nai_main_t* main;     /**< the main */
        nai_int_t last_grp;   /**< the last group id */
        nai_int_t last_index; /**< the last index */
    };

/** 
 * @name    nai_module_map_defines  the functions of module map
 * @{
 */

/**
 * get the count of module map
 * @param   l       pointer to the module map
 * @return  the number of the modules
 */
#define nai_module_map_count(l) ((size_t)(l)->a.count)

/**
 * get the module by index
 * @param   l       pointer to the module map
 * @param   n       the index of module map array
 * @return  the address of the module
 */
#define nai_module_map_at(l, n) (((nai_module_t**)(l)->a.elts)[n])

    /** @} */

    /**
 * initialize the module map
 * @param   l       pointer to the module map
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_module_map_init(nai_module_map_t* l);

    /**
 * open the module map
 * @param   l       pointer to the module map
 * @param   m       pointer to the main
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_module_map_open(nai_module_map_t* l, nai_main_t* m);

    /**
 * add a module to the module map
 * @param   l       pointer to the module map
 * @param   m       pointer to the module
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_module_map_push(nai_module_map_t* l, nai_module_t* m);

    /**
 * call the module map's handler
 * @param   l       pointer to the module map
 * @param   index   the index of handler, see @ref NAI_MODULE_STAGE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_module_map_on(nai_module_map_t* l, nai_int_t index);

    /**
 * count the number of modules in the group
 * @param   l       pointer to the module map
 * @param   grp     the group id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_module_map_count_grp(nai_module_map_t* l, nai_int_t grp);

    /**
 * find module by name
 * @param   l       pointer to the module map
 * @param   name    the name of module
 * @return  the address of the module on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_module_t* nai_module_map_find(nai_module_map_t* l, const char* name);

    /**
 * find command by name
 * @param   l       pointer to the module map
 * @param   grp     the group id
 * @param   grp_mask the mask of location in group
 * @param   name    the name of command
 * @param   m       to recevice the module of command on success
 * @return  the address of the command on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_command_t* nai_module_map_command(
        nai_module_map_t* l, nai_int_t grp, uint32_t grp_mask, const char* name, nai_module_t** m);

    /**
 * close the module map
 * @param   l       pointer to the module map
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_module_map_close(nai_module_map_t* l);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
