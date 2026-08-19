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
/// @file       nai_sample_http.h
/// @brief      
/// @details
/// @date       2022-06-13
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SAMPLE_HTTP_H_NAI
#define _SAMPLE_HTTP_H_NAI

#pragma once

#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_util.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_SAMPLE_PARSE_STATE_T
    #define _NAI_TYPEDEF_SAMPLE_PARSE_STATE_T
    typedef struct nai_sample_parse_state_s nai_sample_parse_state_t;
#endif
#ifndef _NAI_TYPEDEF_SAMPLE_REQUEST_T
    #define _NAI_TYPEDEF_SAMPLE_REQUEST_T
    typedef struct nai_sample_request_s nai_sample_request_t;
#endif

    /**
 * the structure of the parse stat of sample http
 */
    struct nai_sample_parse_state_s
    {
        uint32_t state; /**< the parse state */

        union
        {
            struct
            {
                nai_mem_t cur;   /**< the current parsing word */
                nai_mem_t stash; /**< the saved word */
            };
        };
    };

    /**
 * the structure of sample http request
 */
    struct nai_sample_request_s
    {
        union
        {
            uint16_t version; /**< the full version */
            struct
            {
                uint16_t minor : 8; /**< the minor version */
                uint16_t major : 8; /**< the major version */
            };
        };

        uint32_t status; /**< the http status code */

        char* start; /**< pointer to the start of data */
        char* cur;   /**< pointer to the current point */
        char* cend;  /**< pointer to the end of data */

        /** the parse state of sample http */
        nai_sample_parse_state_t parse;
    };

    /**
 * parse the status line of http response
 * @param   q       pointer to the sample http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    nai_sult_t nai_sample_parse_status_line(nai_sample_request_t* q);

    /**
 * parse the header line of http request/response
 * @param   q       pointer to the sample http request
 * @param   k       pointer to the memory view to get the header key
 * @param   v       pointer to the memory view to get the header value
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    nai_sult_t nai_sample_parse_header_line(nai_sample_request_t* q, nai_mem_t* k, nai_mem_t* v);

    /**
 * skip the header line of http request/response
 * @param   q       pointer to the sample http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    nai_sult_t nai_sample_parse_header_skip(nai_sample_request_t* q);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
