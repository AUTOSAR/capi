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
/// @file       npc_const.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONST_H_NPC
#define _CONST_H_NPC

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NPC_EVENT
 * @name    NPC_EVENT       the type of event
 * @{
 */
#define NPC_ET_UNKNOWN 0x00 /**< unknown, follow the configure */
#define NPC_ET_EVENT   0x01 /**< a normail event */
#define NPC_ET_FIELD   0x02 /**< a field */
#define NPC_ET_SIGNAL  0x03 /**< a singal */
/** @} */

/**
 * @anchor  NPC_RELIABLE
 * @name    NPC_RELIABLE    the type of event
 * @{
 */
#define NPC_RT_UNKNOWN    0x00 /**< unknown, follow the configure */
#define NPC_RT_RELIABLE   0x01 /**< is reliable */
#define NPC_RT_UNRELIABLE 0x02 /**< is unreliable */
#define NPC_RT_BOTH       0x03 /**< both */
    /** @} */

#define NPC_EVENT_ANY    0x00       /**< any event */
#define NPC_METHOD_ANY   0x00       /**< any method */
#define NPC_SERVICE_ANY  0xffff     /**< any service */
#define NPC_INSTANCE_ANY 0xffff     /**< any instance */
#define NPC_MAJOR_ANY    0xff       /**< any major version */
#define NPC_MINOR_ANY    0xffffffff /**< any minor version */

/**
 * @anchor  NPC_AVAILABLE
 * @name    NPC_AVAILABLE   the stat of service
 * @{
 */
#define NPC_AVAIL_OFFLINED 0
#define NPC_AVAIL_ONLINED  1
#define NPC_AVAIL_CHANGED  2
    /** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
