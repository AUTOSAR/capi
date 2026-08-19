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
/// @file       statuschecker.h
/// @brief      time base management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeBase
/// module_path=/TimeSync/TimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_STATUSCHECKER_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_STATUSCHECKER_H_

#include <ara/core/map.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/timebase/eventtype.h"  // NOLINT
#include "ara/tsync/internal/timebase/proxy/skeleton.h"
#include "ara/tsync/internal/timebase/resource/tbresmanager.h"
#include "ara/tsync/internal/timedomain/manager.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {

/// @brief time base status checker
class StatusChecker final
{
public:
    /// @brief constructor
    StatusChecker() noexcept : status{}, flag_{}
    {
        SetLeapJumpChanged(false);
        SetSynchronizationStatusChanged(false);
        SetUserDataChanged(false);
    }

    /// void SetTimeStatus(const internal::TimeStatus &st) noexcept {status_ = st;}
    /// internal::TimeStatus &GetTimeStatus() noexcept {return status_;}.

    /// @brief whether a time jump change has occurred
    /// @return whether a time jump change has occurred
    bool IsLeapJumpChanged() const noexcept { return static_cast< std::uint8_t >(1U) == flag_.lj; }

    /// @brief set whether a time jump change has occurred
    /// @param is whether a time jump change has occurred
    void SetLeapJumpChanged(bool const is) noexcept { flag_.lj = is; }

    /// @brief check whether a synchronization status change has occurred
    /// @return whether a synchronization status change has occurred
    bool IsSynchronizationStatusChanged() const noexcept { return static_cast< std::uint8_t >(1U) == flag_.sc; }

    /// @brief set whether a synchronization status change has occurred
    /// @param is whether a synchronization status change has occurred
    void SetSynchronizationStatusChanged(bool const is) noexcept { flag_.sc = is; }

    /// @brief check whether user data has changed
    /// @return whether user data has changed
    bool IsUserDataChanged() const noexcept { return static_cast< std::uint8_t >(1U) == flag_.uc; }

    /// @brief set whether user data has changed
    /// @param is whether user data has changed
    void SetUserDataChanged(bool const is) noexcept { flag_.uc = is; }

    /// @brief check whether status has changed
    /// @return whether status has changed
    bool IsStatusChanged() const noexcept
    {
        if (IsLeapJumpChanged() || IsSynchronizationStatusChanged() || IsUserDataChanged()) {
            return true;
        }
        return false;
    }
    /// @name status
    /// TODO(zhoubo): clang-tidy does not recommend public class members
    internal::TimeStatus status;  // NOLINT

private:
    /// @brief type declaration
    struct FlagStrut final
    {
        std::uint8_t lj : 1;  /// time jump change occurred
        std::uint8_t sc : 1;  /// synchronization status change occurred
        std::uint8_t uc : 1;  /// user data change occurred
    };
    /// @brief flag_
    struct FlagStrut flag_;
};

}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  //ARA_TSYNC_INTERNAL_TIMEBASE_STATUSCHECKER_H_
