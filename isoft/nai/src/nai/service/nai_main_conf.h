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
/// @file       nai_main_conf.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MAIN_CONF_H_NAI
#define _MAIN_CONF_H_NAI

#pragma once

#include "nai/runtime/nai_value.h"
#include "nai/service/nai_main.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_CONF_MAX_ARG   8
#define NAI_CONF_FILE_READ 1024

#ifndef _NAI_TYPEDEF_CONF_SEG_T
    #define _NAI_TYPEDEF_CONF_SEG_T
    typedef struct nai_conf_seg_s nai_conf_seg_t;
#endif
#ifndef _NAI_TYPEDEF_CONF_FILE_T
    #define _NAI_TYPEDEF_CONF_FILE_T
    typedef struct nai_conf_file_s nai_conf_file_t;
#endif
#ifndef _NAI_TYPEDEF_CONF_T
    #define _NAI_TYPEDEF_CONF_T
    typedef struct nai_main_conf_s nai_conf_t;
#endif
#ifndef _NAI_TYPEDEF_MAIN_CONF_T
    #define _NAI_TYPEDEF_MAIN_CONF_T
    typedef struct nai_main_conf_s nai_main_conf_t;
#endif

    struct nai_conf_seg_s
    {
        const char* name;
        nai_pool_t* pool;
        nai_pool_t* pool_tmp;
        nai_local_t** clocal;
        nai_int_t stack;
        nai_int_t grp;
        uint32_t grp_mask : 16;
        uint32_t push : 1;
        uint32_t lock : 1;
    };

    struct nai_conf_file_s
    {
        nai_conf_file_t* next;
        nai_str_t path;
    };

    struct nai_main_conf_s
    {
        nai_pool_t pool;
        nai_str_t path;
        char* buf;
        nai_int_t bufalloc;
        nai_int_t stack;
        nai_int_t alloc;
        nai_int_t count;
        nai_conf_seg_t** segs;
        nai_conf_file_t* list;
    };

    extern nai_int_t nai_main_module_init(nai_main_t* m);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
