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
/// @file       daemon.cpp
/// @brief      configuration module management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#if !ARA_TSYNC_DEBUG_WITHOUT_EM && ARA_TSYNC_WITH_EXEC
    #include <ara/exec/execution_client.h>
#endif

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/daemon.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief PTP delay measurement completion event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::PtpPdelayFinishedCb(internal::TimeDomainId const &domainId) noexcept
{
    std::ignore = domainId;
    /// do nothing
}

/// @brief PTP time synchronization completion event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpTimeSyncFinishedCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }

    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeSyncFinished);
}

/// @brief PTP time synchronization timeout event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpTimeSyncTimeoutCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }
    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeSyncTimeout);
}

/// @brief PTP message (Sync) send event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpSyncMesgSendCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }
    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeValidationSetMasterTimingData);
}

/// @brief PTP message (Followup) receive event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpFollowupMesgRecvCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }
    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeValidationSetSlaveTimingData);
}

/// @brief PTP message (PdelayRespFollowup) send event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpPdelayRespFlpMesgSendCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }
    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeValidationSetPdelayResponderData);
}

/// @brief PTP message (PdelayRespFollowup) receive event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_ptpPdelayRespFlpMesgRecvCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == timeBaseManager_) {
        return;
    }
    timeBaseManager_->Notify(domainId, internal::timebase::EventType::kTimeValidationSetPdelayInitiatorData);
}

/// @brief TimeBase class SetTime event callback function
/// @param domainId - time domain ID where the event occurred
void Daemon::_timeBaseSetTimeCb(internal::TimeDomainId const &domainId) const noexcept
{
    if (nullptr == ptpManager_) {
        return;
    }
    static_cast< void >(ptpManager_->SetTime(domainId));
}

/// @brief initialize
/// @param bAllowDisPach - whether to allow time domain master to distribute time by default
/// @return 0 - success
/// @return <0 - failure
std::int32_t Daemon::Init(bool const bAllowDisPach) noexcept
{
    std::int32_t const retCode{0};
    bAllowDisPach_ = bAllowDisPach;
    LOG().Warn() << "hello";
    LOG().Info() << "world";

    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (nullptr == mainLoop_) {
        LOG().Warn() << "nullptr == mainLoop_";
        return kRET_E1;
    }

    /// Create configuration manager
    configManager_ = config::ConfigManager::CreateManager();
    if (nullptr == configManager_) {
        LOG().Fatal() << "config::ConfigManager::CreateManager()";
        return kRET_E2;
    }

    /// Create time domain manager
    timeDomainManager_ = timedomain::TDManager::CreateManager(configManager_->GetTimeDomainSet());
    if (nullptr == timeDomainManager_) {
        LOG().Fatal() << "timedomain::TDManager::CreateManager()";
        return kRET_E3;
    }

    /// Create network manager
    netManager_ = ptp::NetworkManager::CreateManager(timeDomainManager_);
    if (nullptr == netManager_) {
        LOG().Fatal() << "ptp::NetworkManager::CreateManager()";
        return kRET_E4;
    }
    /// Create PTP manager
    ptpManager_ = ptp::PtpManager::CreateManager(timeDomainManager_, netManager_, configManager_, bAllowDisPach_);
    if (nullptr == ptpManager_) {
        LOG().Fatal() << "ptp::PtpManager::CreateManager()";
        return kRET_E6;
    }
    ptpManager_->OnTimeEvent(
        ptp::TimeEventType::kPdelayFinished,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { PtpPdelayFinishedCb(domainId); });
    ptpManager_->OnTimeEvent(
        ptp::TimeEventType::kTimeSyncFinished,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpTimeSyncFinishedCb(domainId); });
    ptpManager_->OnTimeEvent(
        ptp::TimeEventType::kTimeSyncTimeout,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpTimeSyncTimeoutCb(domainId); });

    ptpManager_->OnMessageSend(
        ptp::MessageEventType::kSync,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpSyncMesgSendCb(domainId); });
    ptpManager_->OnMessageRecv(
        ptp::MessageEventType::kFollowup,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpFollowupMesgRecvCb(domainId); });
    ptpManager_->OnMessageSend(
        ptp::MessageEventType::kPdelayRespFollowup,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpPdelayRespFlpMesgSendCb(domainId); });
    ptpManager_->OnMessageRecv(
        ptp::MessageEventType::kPdelayRespFollowup,
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _ptpPdelayRespFlpMesgRecvCb(domainId); });

    /// Create time base manager
    storageManager_ = storage::StorageManager::CreateManager();
    if (nullptr == storageManager_) {
        LOG().Fatal() << "storage::StorageManager::CreateManager()";
        return kRET_E6;
    }

    /// Create time base manager
    timeBaseManager_ = timebase::TBManager::CreateManager(configManager_, timeDomainManager_);
    if (nullptr == timeBaseManager_) {
        LOG().Fatal() << "timebase::TBManager::CreateManager()";
        return kRET_E7;
    }
    timeBaseManager_->InitStorage(
        [this](ara::core::String const &timebaseName, ara::core::StringView kvSpecifier,
               ara::core::StringView perInstanceSpecifier, ara::core::StringView fileSpecifier,
               ara::core::String &userData, double &rateDeviation, std::uint64_t &lastGlobalNano) noexcept -> bool {
            return this->storageManager_->GetStorageTime(timebaseName, kvSpecifier, perInstanceSpecifier, fileSpecifier,
                                                         userData, rateDeviation, lastGlobalNano);
        },
        [this](StorageType type, ara::core::String const &timebaseName, ara::core::StringView keyStrView,
               ara::core::String const &userData, double const rateDeviation,
               std::uint64_t const lastGlobalNano) noexcept -> void {
            this->storageManager_->SetStorageTime(type, timebaseName, keyStrView, userData, rateDeviation,
                                                  lastGlobalNano);
        });

    timeBaseManager_->OnSetTime(
        [this](internal::TimeDomainId const &domainId) noexcept -> void { _timeBaseSetTimeCb(domainId); });

    return retCode;
}

/// @brief destroy
void Daemon::Destroy() noexcept
{
    if (nullptr != timeBaseManager_) {
        std::ignore      = timeBaseManager_->StoreTimeToStorage();
        timeBaseManager_ = nullptr;
    }
    storageManager_    = nullptr;
    ptpManager_        = nullptr;
    timeDomainManager_ = nullptr;
    configManager_     = nullptr;
    netManager_        = nullptr;
    mainLoop_          = nullptr;
}

/// @brief run, start service routine
void Daemon::Run() noexcept
{
#if !ARA_TSYNC_DEBUG_WITHOUT_EM && ARA_TSYNC_WITH_EXEC
    // LOGINFO() << "Report Running State to EMD"
    /// Report running status to execution management
    {
        ara::exec::ExecutionClient const ec;
        static_cast< void >(ec.ReportExecutionState(ara::exec::ExecutionState::kRunning));
    }
#endif

    static_cast< void >(mainLoop_->Run(true));
}

}  // namespace internal
}  // namespace tsync
}  // namespace ara
