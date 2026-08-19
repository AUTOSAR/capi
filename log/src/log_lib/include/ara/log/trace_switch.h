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
/// @file       trace_switch.h
/// @brief      Encapsulation of internal log parameters
/// @details
/// @date       2025-09-15
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001
/// @unit_name = logger_impl
/// @unit_description=Lib interface side of Dlt, encapsulation of internal log parameters
/// @endcode
///
/// ================================================================

#ifndef ARA_LOG_TRACE_SWITCH_H
#define ARA_LOG_TRACE_SWITCH_H

#include <iostream>
#include <tuple>

#include "dlt_logger.h"
#include "trace_arti.h"

namespace ara {
namespace log {

template < typename MsgId, typename... Params >
void TraceSwitchNone(DltLogger& logger, const MsgId& messageId, const Params&... args) noexcept
{
    // route to nothing
    std::ignore = logger;
    std::ignore = messageId;
    std::initializer_list< void >{(void)args...};

    //    (std::ignore = args, ...); c++17
}

template < typename MsgId, typename... Params >
void TraceSwitchArti(DltLogger& logger, const MsgId& messageId, const Params&... args) noexcept
{
    std::ignore = logger;
    // route to arti only
    TraceArti(messageId, args...);
}

template < typename MsgId, typename... Params >
void TraceSwitchLog(DltLogger& logger, const MsgId& messageId, const Params&... args) noexcept
{
    // route to logger only
    logger.Log(messageId, args...);
}

template < typename MsgId, typename... Params >
void TraceSwitchBoth(DltLogger& logger, const MsgId& messageId, const Params&... args) noexcept
{
    // route to arti and log
    TraceArti(messageId, args...);
    logger.Log(messageId, args...);
}

template < typename MsgId, typename... Params >
void TraceSwitch(DltLogger& logger, const MsgId& messageId, const Params&... args) noexcept
{
    // the default routes to log
    // if a different route is wanted then specify specializations of TraceSwitch
    TraceSwitchLog(logger, messageId, args...);
}

}  // namespace log
}  // namespace ara

#endif /* ARA_LOG_TRACE_SWITCH_H */
