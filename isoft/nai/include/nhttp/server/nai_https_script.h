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
/// @file       nai_https_script.h
/// @brief      
/// @details
/// @date       2021-09-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SCRIPT_H_NHTTPS
#define _SCRIPT_H_NHTTPS

#pragma once

#include "nai/runtime/nai_regex.h"
#include "nai/service/nai_script.h"
#include "nai_https.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define nai_https_expn_compile(h, e, s) nai_script_expn_compile(e, &(h)->vars, s)

    NHTTP_EXTERN
    nai_sult_t nai_https_script_exec(nai_https_request_t* r, const void* code, nai_script_value_t* sp);

    NHTTP_EXTERN
    nai_sult_t nai_https_expn_value(nai_https_request_t* r,
                                    const nai_script_expn_t* e,
                                    nai_str_t* v,
                                    const nai_reserved_t* rev);

    NHTTP_EXTERN
    nai_sult_t nai_https_command_expn(nai_command_t* c, nai_command_args_t* args);

#if (NAI_HAVE_REGEX)

    typedef struct nai_https_regex_var_s
    {
        nai_int_t index;
        nai_int_t capture;
    } nai_https_regex_var_t;

    typedef struct nai_https_regex_s
    {
        nai_str_t name;
        nai_regex_t re;
        nai_https_regex_var_t* var;
        nai_int_t count;
    } nai_https_regex_t;

    NHTTP_EXTERN
    nai_sult_t nai_https_regex_compile(
        nai_https_t* h, nai_https_regex_t* re, const nai_str_t* pattern, nai_int_t flags, nai_regex_errinfo_t* ei);

    NHTTP_EXTERN
    nai_sult_t nai_https_regex_exec(nai_https_request_t* r, nai_https_regex_t* re, const nai_mem_t* str);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
