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
/// @file       nai_httpc_script.h
/// @brief      
/// @details
/// @date       2021-09-17
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SCRIPT_H_NHTTPC
#define _SCRIPT_H_NHTTPC

#pragma once

#include "nai/runtime/nai_regex.h"
#include "nai/service/nai_script.h"
#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define nai_httpc_expn_compile(h, e, s) nai_script_expn_compile(e, &(h)->vars, s)

    NHTTP_EXTERN
    nai_sult_t nai_httpc_script_exec(nai_httpc_request_t* r, const void* code, nai_script_value_t* sp);

    NHTTP_EXTERN
    nai_sult_t nai_httpc_expn_value(nai_httpc_request_t* r,
                                    const nai_script_expn_t* e,
                                    nai_str_t* v,
                                    const nai_reserved_t* rev);

    NHTTP_EXTERN
    nai_sult_t nai_httpc_command_expn(nai_command_t* c, nai_command_args_t* args);

#if (NAI_HAVE_REGEX)

    /**
 * the structure of map the regex variable to the script variable
 */
    typedef struct nai_httpc_regex_var_s
    {
        nai_int_t index;   /**< the index of the script variable */
        nai_int_t capture; /**< the index of the regex variable */
    } nai_httpc_regex_var_t;

    /**
 * the structure of the regex of http client
 */
    typedef struct nai_httpc_regex_s
    {
        nai_str_t name;             /**< the string view of the pattern */
        nai_regex_t re;             /**< the regex */
        nai_httpc_regex_var_t* var; /**< the array of variable mapping */
        nai_int_t count;            /**< the count of the array */
    } nai_httpc_regex_t;

    /**
 * compile the regular expression
 * @param   h       pointer to the service of http client
 * @param   re      pointer to the regex of http client
 * @param   pattern pointer to the string view of the pattern
 * @param   flags   the flags of compile, see @ref NAI_REGEX_FLAG
 * @param   ei      pointer to the error information of the regex
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_regex_compile(
        nai_httpc_t* h, nai_httpc_regex_t* re, const nai_str_t* pattern, nai_int_t flags, nai_regex_errinfo_t* ei);

    /**
 * perform regular expression matching
 * @param   r       pointer to the http request
 * @param   re      pointer to the regex of http client
 * @param   str     pointer to the memory view of matching stirng
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_regex_exec(nai_httpc_request_t* r, nai_httpc_regex_t* re, const nai_mem_t* str);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
