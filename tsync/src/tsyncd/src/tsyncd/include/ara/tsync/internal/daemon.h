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
/// @file       daemon.h
/// @brief      time synchronization service process background management class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_DAEMON_H_
#define ARA_TSYNC_INTERNAL_DAEMON_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/naicpp/evloop.h>

#include <cstddef>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/ptp/networkmanager.h"
#include "ara/tsync/internal/ptp/ptpmanager.h"
#include "ara/tsync/internal/storage/storagemanager.h"
#include "ara/tsync/internal/timebase/tbmanager.h"
#include "ara/tsync/internal/timedomain/manager.h"

namespace ara {
namespace tsync {
namespace internal {

/// @brief time synchronization background service class
class Daemon final
{
public:
    /// @brief default constructor
    explicit Daemon() = default;

    /// @brief default destructor
    ~Daemon() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    /// TODO(zhoubo): copy assignment operator is explicitly defaulted but
    /// implicitly deleted
    Daemon &operator=(Daemon const &other) = delete;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    Daemon &operator=(Daemon &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    /// TODO(zhoubo): copy constructor is explicitly defaulted but implicitly
    /// deleted
    Daemon(Daemon const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Daemon(Daemon &&) noexcept = default;

    /// @brief initialize
    /// @param bAllowDisPach - whether to allow time domain master to distribute time by default
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Init(bool const bAllowDisPach) noexcept;

    /// @brief run, start service routine
    void Run() noexcept;

    /// @brief destroy
    void Destroy() noexcept;

private:
    /// @brief PTP delay measurement completion event callback function
    /// @param domainId - time domain ID where the event occurred
    static void PtpPdelayFinishedCb(internal::TimeDomainId const &domainId) noexcept;

    /// @brief PTP time synchronization completion event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpTimeSyncFinishedCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief PTP time synchronization timeout event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpTimeSyncTimeoutCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief PTP message (Sync) send event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpSyncMesgSendCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief PTP message (Followup) receive event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpFollowupMesgRecvCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief PTP message (PdelayRespFollowup) send event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpPdelayRespFlpMesgSendCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief PTP message (PdelayRespFollowup) receive event callback function
    /// @param domainId - time domain ID where the event occurred
    void _ptpPdelayRespFlpMesgRecvCb(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief TimeBase class SetTime event callback function
    /// @param domainId - time domain ID where the event occurred
    void _timeBaseSetTimeCb(internal::TimeDomainId const &domainId) const noexcept;

private:
    /// @name mainLoop_ - event loop handle
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @name configManager_ - configuration manager handle
    std::shared_ptr< config::ConfigManager > configManager_{nullptr};

    /// @name timeDomainManager_ - time domain manager handle
    std::shared_ptr< timedomain::TDManager > timeDomainManager_{nullptr};

    /// @name netManager_ - network manager handle
    std::shared_ptr< ptp::NetworkManager > netManager_{nullptr};

    /// @name ptpManager_ - PTP manager handle
    std::unique_ptr< ptp::PtpManager > ptpManager_{nullptr};

    /// @name storageManager_ - storage manager handle
    std::unique_ptr< storage::StorageManager > storageManager_{nullptr};

    /// @name timeBaseManager_ - time base manager handle
    std::unique_ptr< timebase::TBManager > timeBaseManager_{nullptr};

    /// @name bAllowDisPach_ - whether to allow provider to send sync/followup messages without setting time
    bool bAllowDisPach_{false};
};  /// class Daemon

}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_DAEMON_H_
