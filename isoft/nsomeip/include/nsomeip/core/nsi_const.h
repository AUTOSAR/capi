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
/// @file       nsi_const.h
/// @brief
/// @details
/// @date       2021-06-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONST_H_NSI
#define _CONST_H_NSI

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NSI_EVENT
 * @name    NSI_EVENT       the type of event
 * @{
 */
#define NSI_ET_UNKNOWN 0x00 /**< unknown, follow the configure */
#define NSI_ET_EVENT   0x01 /**< a normail event */
#define NSI_ET_FIELD   0x02 /**< a field */
#define NSI_ET_SIGNAL  0x03 /**< a singal */
/** @} */

/**
 * @anchor  NSI_RELIABLE
 * @name    NSI_RELIABLE    the type of event
 * @{
 */
#define NSI_RT_UNKNOWN    0x00 /**< unknown, follow the configure */
#define NSI_RT_RELIABLE   0x01 /**< is reliable */
#define NSI_RT_UNRELIABLE 0x02 /**< is unreliable */
#define NSI_RT_BOTH       0x03 /**< both */
/** @} */

/* PRS_SOMEIPSD_00516 */
#define NSI_EVENT_ANY    0x00       /**< any event */
#define NSI_METHOD_ANY   0x00       /**< any method */
#define NSI_SERVICE_ANY  0xffff     /**< any service */
#define NSI_INSTANCE_ANY 0xffff     /**< any instance */
#define NSI_MAJOR_ANY    0xff       /**< any major version */
#define NSI_MINOR_ANY    0xffffffff /**< any minor version */

/**
 * @anchor  NSI_AVAILABLE
 * @name    NSI_AVAILABLE   the stat of service
 * @{
 */
#define NSI_AVAIL_OFFLINED 0
#define NSI_AVAIL_ONLINED  1
#define NSI_AVAIL_CHANGED  2
    /** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
