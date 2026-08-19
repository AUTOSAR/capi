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
/// @file       event.h
/// @brief      PTP event definition
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_EVENT_H_
#define ARA_TSYNC_INTERNAL_PTP_EVENT_H_

#include <chrono>
#include <functional>

#include "ara/tsync/internal/ptp/message/header.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief message type
using MessageEventType = message::Type;

/// @brief message event callback function, called when a registered event occurs.
/// @param domainId - time domain ID when the event occurs;
using MessageEventHandler = std::function< void(internal::TimeDomainId const& domainId) >;

/// @brief time event type
enum class TimeEventType : std::uint8_t
{
    kTimeSyncFinished,
    kTimeSyncTimeout,
    kPdelayFinished
};

/// @brief time event callback function, called when a registered event occurs. Time events include: time synchronization completion, path delay calculation completion, etc.
/// @param domainId - time domain ID when the event occurs;
using TimeEventHandler = std::function< void(internal::TimeDomainId const& domainId) >;

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_PTP_EVENT_H_
