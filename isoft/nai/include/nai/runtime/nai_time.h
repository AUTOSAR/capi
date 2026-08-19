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
/// @file       nai_time.h
/// @brief      the functions of the c time
/// @details
/// @date       2021-02-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 */

#ifndef _TIME_H_NAI
#define _TIME_H_NAI

#pragma once

#include "nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_TIME_STR_LEN    24 /**< the bytes without null-terminated */
#define NAI_TIME_RFC822_LEN 29 /**< the bytes without null-terminated */

    /**
 * convert time to standard c time format
 * @param   sec     the value of the time
 * @param   buf     pointer to the buffer
 * @param   len     the length of the buffer, see #NAI_TIME_STR_LEN
 * @return  the number of filled bytes without null-terminated
 */
    NAI_EXTERN
    nai_int_t nai_time_to_str(time_t sec, char* buf, size_t len);

    /**
 * convert time to rfc822 format
 * @param   sec     the value of the time
 * @param   buf     pointer to the buffer
 * @param   len     the length of the buffer, see #NAI_TIME_RFC822_LEN
 * @return  the number of filled bytes without null-terminated
 */
    NAI_EXTERN
    nai_int_t nai_time_to_rfc822(time_t sec, char* buf, size_t len);

    /**
 * parse the time string of the standard c format to the time
 * @param   sec     pointer to the time
 * @param   str     pointer to the time string
 * @param   len     the length of the string, can be -1
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_time_parse(time_t* sec, const char* str, size_t len);

    /**
 * parse the time string of the rfc format to the time
 * @param   sec     pointer to the time
 * @param   str     pointer to the time string
 * @param   len     the length of the string, can be -1
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_time_parse_rfc(time_t* sec, const char* str, size_t len);

    /**
 * get the time zone
 * @param   isdst   whether to use daylight saving time
 * @return  the value of the time zone
 */
    NAI_EXTERN
    nai_int_t nai_time_get_zone(nai_int_t isdst);

    /**
 * update the time zone
 * @return  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is a no-op in most systems, 
 *          only some systems need to actively trigger the time zone update.
 */
    NAI_EXTERN
    nai_int_t nai_time_update_zone();

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
