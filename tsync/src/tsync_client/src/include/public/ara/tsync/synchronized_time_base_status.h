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
/// @file       synchronized_time_base_status.h
/// @brief      time base synchronization status
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_H_
#define _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_H_

#include <memory>

#include "ara/core/span.h"
#include "ara/tsync/timestamp.h"

namespace ara {
namespace tsync {

/// @traceid  {SWS_TS_01051, 20-11}
/// @brief time jump status enumeration
enum class LeapJump : std::uint32_t
{
    /// @brief no jump occurred
    kTimeLeapNone = 0,
    /// @brief forward jump occurred
    kTimeLeapFuture = 1,
    /// @brief backward jump occurred
    kTimeLeapPast = 2
};

/// @traceid  {SWS_TS_01050, 20-11}
/// @brief time synchronization status enumeration
/// @note
/// Synchronization status is divided into four types: never synchronized / synchronization timeout / synchronized to global time / synchronized to time gateway.
///  1. Never synchronized: Never synchronized with time gateway or time master; it is the initial state of the time base.
///  2. Synchronization timeout: Synchronized at least once, but a synchronization overdue has occurred recently.
///  3. Synchronized to global time: The received synchronization time is aligned with the time master (whether directly synchronized or indirectly via a time gateway), meaning the time is trustworthy.
///  4. Synchronized to time gateway: Can only prove that the current time base is synchronized with the superior time gateway, but this time is not synchronized with the time master.
///
enum class SynchronizationStatus : std::uint32_t
{
    /// @brief never synchronized
    kNotSynchronizedUntilStartup = 0,
    /// @brief synchronization timeout
    kTimeOut = 0x1,
    /// @brief synchronized to global time
    kSynchronized = 0x2,
    /// @brief synchronized to time gateway
    kSynchToGateway = 0x3
};

/// @traceid  {SWS_TS_01052,20-11}
/// @brief    This class represents a time point snapshot and status.
class SynchronizedTimeBaseStatus final
{
public:
    /// @brief constructor
    SynchronizedTimeBaseStatus() noexcept;

    /// @brief destructor
    ~SynchronizedTimeBaseStatus() noexcept;

    /// @traceid  {SWS_TS_01057, 20-11}
    /// @brief    move constructor
    /// @param    tbs object reference
    SynchronizedTimeBaseStatus(SynchronizedTimeBaseStatus &&tbs) noexcept;

    /// @traceid  {SWS_TS_01058, 20-11}
    /// @brief    copy constructor
    /// @param    tbs object reference
    SynchronizedTimeBaseStatus(SynchronizedTimeBaseStatus const &tbs) noexcept;

    /// @traceid  {SWS_TS_01059, 20-11}
    /// @brief    move assignment operator overload
    /// @param    tbs object reference
    /// @returns  object reference
    SynchronizedTimeBaseStatus &operator=(SynchronizedTimeBaseStatus &&tbs) &noexcept;

    /// @traceid  {SWS_TS_01060, 20-11}
    /// @brief    copy assignment operator overload
    /// @param    tbs object reference
    /// @returns  object reference
    SynchronizedTimeBaseStatus &operator=(SynchronizedTimeBaseStatus const &tbs) noexcept;

    /// @traceid  {SWS_TS_01055, 20-11}
    /// @brief    get the creation time of the current snapshot
    /// @returns  timestamp
    ara::tsync::Timestamp GetCreationTime() const noexcept;

    /// @traceid  {SWS_TS_01053, 20-11}
    /// @brief    get synchronization status
    /// @returns  synchronization status enumeration value.
    SynchronizationStatus GetSynchronizationStatus() const noexcept;

    /// @traceid  {SWS_TS_01054, 20-11}
    /// @brief    get time jump status.
    /// @returns  time jump status enumeration value.
    LeapJump GetLeapJump() const noexcept;

    /// @traceid  {SWS_TS_010056, 20-11}
    /// @traceid  {SWS_TS_00120, 20-11}
    /// @brief    get user data
    /// @returns  user data; if none, the function should return an empty Span.
    /// @note User data is a string used to describe the characteristics of the time source, up to three characters, e.g., "RTC", "GPS".
    ara::core::Span< ara::core::Byte const > GetUserData() const noexcept;

public:
    /// @brief SynchronizedTimeBaseStatusPrivateImpl
    class SynchronizedTimeBaseStatusPrivateImpl;

    std::unique_ptr< SynchronizedTimeBaseStatusPrivateImpl >
        /// @name privateImpl - pointer to private implementation class
        privateImpl;
};  ///  class SynchronizedTimeBaseStatus

}  // namespace tsync
}  // namespace ara

#endif  /// __ARA_TSYNC_SYNCHRONIZED_TIME_BASE_STATUS_H__
