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
/// @file       synchronized_time_base_status_private_impl.h
/// @brief      time base status private implementation class
/// @details
/// @date       2023-02-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/SyncTimeBase
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_PRIVATE_IMPL_H_
#define ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_PRIVATE_IMPL_H_

#include <ara/core/vector.h>

#include "ara/tsync/internal/userdata.h"
#include "ara/tsync/synchronized_time_base_status.h"

namespace ara {
namespace tsync {

/// @brief time point snapshot and status private implementation class
class SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatusPrivateImpl final
{
public:
    /// @traceid  {SWS_TS_00007}
    /// @traceid  {SWS_TS_00026}
    /// @name timeLeapJump - time jump status,
    LeapJump timeLeapJump{LeapJump::kTimeLeapNone};
    /// @name status - time synchronization status,
    SynchronizationStatus status{SynchronizationStatus::kNotSynchronizedUntilStartup};
    /// @name userData - user data,
    internal::UserData userData;
    /// @name creationTime,
    ara::tsync::Timestamp creationTime;
};

}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_PRIVATE_IMPL_H_
