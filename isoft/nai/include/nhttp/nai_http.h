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
/// @file       nai_http.h
/// @brief      
/// @details
/// @date       2021-02-15
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _HTTP_H_NHTTP
#define _HTTP_H_NHTTP

#pragma once

#include "nai_config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 
 * @anchor  NAI_HTTP_VERSIONS
 * @name    NAI_HTTP_VERSIONS   the versions of http
 * @{
 */

#define NAI_HTTP_VERSION(major, minor) (uint16_t)(((major) << 8) + (minor))

#define NAI_HTTP_09 NAI_HTTP_VERSION(0, 9)
#define NAI_HTTP_10 NAI_HTTP_VERSION(1, 0)
#define NAI_HTTP_11 NAI_HTTP_VERSION(1, 1)

/** @} */

/** 
 * @anchor  NAI_HTTP_METHODS
 * @name    NAI_HTTP_METHODS    the methods of http
 * @{
 */

/* http methods */
#define NAI_HTTP_UNKNOWN   0
#define NAI_HTTP_GET       1
#define NAI_HTTP_HEAD      2
#define NAI_HTTP_POST      3
#define NAI_HTTP_PUT       4
#define NAI_HTTP_DELETE    5
#define NAI_HTTP_MKCOL     6
#define NAI_HTTP_COPY      7
#define NAI_HTTP_MOVE      8
#define NAI_HTTP_OPTIONS   9
#define NAI_HTTP_PROPFIND  10
#define NAI_HTTP_PROPPATCH 11
#define NAI_HTTP_LOCK      12
#define NAI_HTTP_UNLOCK    13
#define NAI_HTTP_PATCH     14
#define NAI_HTTP_TRACE     15

/** @} */

/** 
 * @anchor  NAI_HTTP_STATUS
 * @name    NAI_HTTP_STATUS     the status code of http
 * @{
 */

/* http status code */
#define NAI_HTTP_CONTINUE            100
#define NAI_HTTP_SWITCHING_PROTOCOLS 101
#define NAI_HTTP_PROCESSING          102

#define NAI_HTTP_OK              200
#define NAI_HTTP_CREATED         201
#define NAI_HTTP_ACCEPTED        202
#define NAI_HTTP_NO_CONTENT      204
#define NAI_HTTP_PARTIAL_CONTENT 206

#define NAI_HTTP_SPECIAL_RESPONSE   300
#define NAI_HTTP_MOVED_PERMANENTLY  301
#define NAI_HTTP_MOVED_TEMPORARILY  302
#define NAI_HTTP_SEE_OTHER          303
#define NAI_HTTP_NOT_MODIFIED       304
#define NAI_HTTP_TEMPORARY_REDIRECT 307
#define NAI_HTTP_PERMANENT_REDIRECT 308

#define NAI_HTTP_BAD_REQUEST              400
#define NAI_HTTP_UNAUTHORIZED             401
#define NAI_HTTP_FORBIDDEN                403
#define NAI_HTTP_NOT_FOUND                404
#define NAI_HTTP_NOT_ALLOWED              405
#define NAI_HTTP_REQUEST_TIME_OUT         408
#define NAI_HTTP_CONFLICT                 409
#define NAI_HTTP_LENGTH_REQUIRED          411
#define NAI_HTTP_PRECONDITION_FAILED      412
#define NAI_HTTP_REQUEST_ENTITY_TOO_LARGE 413
#define NAI_HTTP_REQUEST_URI_TOO_LARGE    414
#define NAI_HTTP_UNSUPPORTED_MEDIA_TYPE   415
#define NAI_HTTP_RANGE_NOT_SATISFIABLE    416
#define NAI_HTTP_MISDIRECTED_REQUEST      421
#define NAI_HTTP_TOO_MANY_REQUESTS        429

#define NAI_HTTP_INTERNAL_SERVER_ERROR 500
#define NAI_HTTP_NOT_IMPLEMENTED       501
#define NAI_HTTP_BAD_GATEWAY           502
#define NAI_HTTP_SERVICE_UNAVAILABLE   503
#define NAI_HTTP_GATEWAY_TIME_OUT      504
#define NAI_HTTP_VERSION_NOT_SUPPORTED 505
#define NAI_HTTP_INSUFFICIENT_STORAGE  507

/** @} */

/** 
 * @anchor  NAI_HTTP_CONNECTION
 * @name    NAI_HTTP_CONNECTION     the connection type of http
 * @{
 */

/* http connection */
#define NAI_HTTP_CLOSE     1
#define NAI_HTTP_KEEPALIVE 2

/** @} */

/** 
 * @anchor  NAI_HTTP_SET
 * @name    NAI_HTTP_SET        the set/add flags of http
 * @{
 */

/* http set/add flags */
#define NAI_HTTP_DUP        0x01
#define NAI_HTTP_DUP_HEADER 0x06
#define NAI_HTTP_DUP_KEY    0x02
#define NAI_HTTP_DUP_VALUE  0x04
#define NAI_HTTP_SET        0x08

/** @} */

/** 
 * @anchor  NAI_HTTP_ERROR
 * @name    NAI_HTTP_ERROR      the error code range of http
 * @{
 */

/* http filter error sult code */
#define NAI_HTTP_ESTART 52000
#define NAI_HTTP_ELAST  52600

    /** @} */

    /** 
 * @name    NAI_HTTP_DEFINES    the functions of http
 * @{
 */

#define nai_errno_is_http_status(s) ((s) >= NAI_HTTP_ESTART && (s) < NAI_HTTP_ELAST)

#define nai_errno_to_http_status(s) ((s)-NAI_HTTP_ESTART)

#define nai_errno_from_http_status(s) (NAI_HTTP_ESTART + (s))

#define nai_sult_is_http_status(s) (nai_errno_is_http_status(-(s)))

#define nai_sult_to_http_status(s) (nai_errno_to_http_status(-(s)))

#define nai_sult_from_http_status(s) (-nai_errno_from_http_status(s))

    /** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
