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
/// @file       icc_const.h
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONST_H_ICC
#define _CONST_H_ICC

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  ICC_EVENT
 * @name    ICC_EVENT       the type of event
 * @{
 */
#define ICC_ET_UNKNOWN 0x00 /**< unknown, follow the configure */
#define ICC_ET_EVENT   0x01 /**< a normail event */
#define ICC_ET_FIELD   0x02 /**< a field */
#define ICC_ET_SIGNAL  0x03 /**< a singal */
/** @} */

/**
 * @anchor  ICC_RELIABLE
 * @name    ICC_RELIABLE    the type of event
 * @{
 */
#define ICC_RT_UNKNOWN    0x00 /**< unknown, follow the configure */
#define ICC_RT_RELIABLE   0x01 /**< is reliable */
#define ICC_RT_UNRELIABLE 0x02 /**< is unreliable */
#define ICC_RT_BOTH       0x03 /**< both */
/** @} */

/* PRS_SOMEIPSD_00516 */
#define ICC_EVENT_ANY    0x00       /**< any event */
#define ICC_METHOD_ANY   0x00       /**< any method */
#define ICC_SERVICE_ANY  0xffff     /**< any service */
#define ICC_INSTANCE_ANY 0xffff     /**< any instance */
#define ICC_MAJOR_ANY    0xff       /**< any major version */
#define ICC_MINOR_ANY    0xffffffff /**< any minor version */

/**
 * @anchor  ICC_AVAILABLE
 * @name    ICC_AVAILABLE   the stat of service
 * @{
 */
#define ICC_AVAIL_OFFLINED 0
#define ICC_AVAIL_ONLINED  1
#define ICC_AVAIL_CHANGED  2
    /** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
