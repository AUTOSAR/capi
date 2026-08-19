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
/// @file       proxyeventtype.h
/// @brief      time base proxy Event definition
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_PROXYEVENTTYPE_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_PROXYEVENTTYPE_H_

#include <ara/core/string_view.h>

#include <cstdint>
#include <functional>
#include <memory>

#include "ara/tsync/internal/timebase/resource/tbcontext.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief event type definition
enum class ProxyEventType : std::uint32_t
{
    kSetTime                     = 0x01,
    kUpdateTime                  = 0x02,
    kSetUserData                 = 0x04,
    kSetRateDeviation            = 0x08,
    kStatusChanged               = 0x10,
    kSynchronizationStateChanged = 0x20,
    kLeapJump                    = 0x40,
    kPrecisionMeasurement        = 0x80,
    kValidationMeasurement       = 0x100,
};

/// @brief event callback function, called when a registered event occurs.
/// @param tbId - time base ID when the event occurs;
/// @param event - event type
using ProxyEventHandler = std::function< void(timebase::resource::TimeBaseId const tbId, ProxyEventType const event) >;

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_PROXYEVENTTYPE_H_