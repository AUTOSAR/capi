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
/// @file       security_level_manager.cpp
/// @brief      This file provides DiagnosticSessionControl (0x10) service
/// @details
/// @date       2024-11-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=security_access_management
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=service_x31.h
/// @unit_description=This file provides service_x31 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "security_level_manager.h"

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {

SecurityLevel::SecurityLevel(SecurityLevelId& currentId) noexcept : currentId_{currentId}, config_{} {}

std::int32_t SecurityLevel::Initialize(SecurityLevelConfig const& config) noexcept
{
    /// According to SWS_DM_00863 GetSeed can be without securityAccessDataRecord
    // if (config.accessDataRecordSize == 0) {
    //     LogError() << "SecurityLevel::Initialize| accessDataRecordSize must greater then 0!";
    //     return -1;
    // }
    if (config.keySize == 0) {
        LogError() << "SecurityLevel::Initialize| keySize must greater then 0!";
        return -1;
    }
    if (config.seedSize == 0) {
        LogError() << "SecurityLevel::Initialize| seedSize must greater then 0!";
        return -1;
    }
    if (config.numFailedSecurityAccess == 0) {
        LogError() << "SecurityLevel::Initialize| numFailedSecurityAccess must greater then 0!";
        return -1;
    }
    if (config.securityDelayTime == 0) {
        LogError() << "SecurityLevel::Initialize| securityDelayTime must greater then 0!";
        return -1;
    }
    config_ = config;
    return 0;
}

std::string SecurityLevel::GetShortName() const noexcept { return config_.shortName; }

/// @brief accessDataRecordSize
std::size_t SecurityLevel::GetAccessDataRecordSize() const noexcept { return config_.accessDataRecordSize; }

/// @brief keySize
std::size_t SecurityLevel::GetKeySize() const noexcept { return config_.keySize; }

/// @brief numFailedSecurityAccess
std::size_t SecurityLevel::GetNumFailedSecurityAccess() const noexcept { return config_.numFailedSecurityAccess; }

/// @brief securityDelayTime
///   Unit: seconds
std::uint32_t SecurityLevel::GetSecurityDelayTime() const noexcept { return config_.securityDelayTime; }

/// @brief seedSize
std::size_t SecurityLevel::GetSeedSize() const noexcept { return config_.seedSize; }

/// @brief id
SecurityLevelId SecurityLevel::GetId() const noexcept { return config_.id; }

bool SecurityLevel::Check() const noexcept
{
    LogDebug() << "current security level:" << currentId_ << "config security level:" << config_.id;
    return config_.id == currentId_;
}

std::int32_t SecurityLevelManager::Initialize(std::set< SecurityLevelConfig > const& securityLevelTable,
                                              std::shared_ptr< SessionManager >& sessionManager) noexcept
{
    if (securityLevelTable.empty()) {
        LogError() << "SecurityLevelManager::Initialize| securityLevelTable is empty!";
        return -1;
    }
    if (sessionManager.get() == nullptr) {
        LogError() << "SecurityLevelManager::Initialize| sessionManager is nullptr!";
    }
    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSession, SessionId newSession) {
        if (oldSession != newSession || newSession == 0x01) {
            LogDebug() << "SecurityLevelManager::Initialize| security level lock!";
            SetSecurityLevel(0);
        }
    }};
    sessionManager->AddNotifyOfSessionChange(sessionChangeCallback);
    std::int32_t result{};
    for (auto&& securityLevel : securityLevelTable) {
        auto findSecurityLevelResult = table_.find(securityLevel.id);
        if (findSecurityLevelResult == table_.end()) {
            std::shared_ptr< SecurityLevel > securityLevelPtr{nullptr};
            securityLevelPtr = std::make_shared< SecurityLevel >(currentSecurityLevelType_);
            result           = securityLevelPtr->Initialize(securityLevel);
            if (result != 0) {
                LogError() << "SecurityLevelManager::Initialize| init security level fails! security level id:"
                           << securityLevel.id << " error:" << result;
                return result;
            }
            std::ignore = table_.insert({securityLevel.id, securityLevelPtr});
            continue;
        }
        LogError() << "SecurityLevelManager::Initialize| SecurityLevel initialized repeatedly!";
        return -1;
    }
    return result;
}

SecurityLevelId SecurityLevelManager::GetCurrentSecurityLevel() const noexcept { return currentSecurityLevelType_; }

std::shared_ptr< SecurityLevel > SecurityLevelManager::GetSecurityLevel(SecurityLevelId id) noexcept
{
    auto findSecurityLevelResult = table_.find(id);
    if (findSecurityLevelResult == table_.end()) {
        LogWarn() << "SecurityLevelManager::GetSecurityLevel| get security level fails! id:" << id;
        return nullptr;
    }
    return findSecurityLevelResult->second;
}

NotifyOfSecurityLevelChangeCallbackId SecurityLevelManager::AddNotifyOfSecurityLevelChange(
    NotifyOfSecurityLevelChangeCallback& callback) noexcept
{
    std::unique_lock< std::recursive_mutex > lock{lock_};
    NotifyOfSecurityLevelChangeCallbackId const max{0xFFFFFFFF};
    NotifyOfSecurityLevelChangeCallbackId index{0U};
    while (max > index) {
        auto findResult = notifyOfSecurityLevelChangeCallbackTable_.find(index);
        if (findResult == notifyOfSecurityLevelChangeCallbackTable_.end()) {
            std::ignore = notifyOfSecurityLevelChangeCallbackTable_.insert({index, callback});
            return index;
        }
        ++index;
    }
    return max;
}

void SecurityLevelManager::RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId id) noexcept
{
    std::unique_lock< std::recursive_mutex > lock{lock_};
    auto findResult = notifyOfSecurityLevelChangeCallbackTable_.find(id);
    if (findResult != notifyOfSecurityLevelChangeCallbackTable_.end()) {
        std::ignore = notifyOfSecurityLevelChangeCallbackTable_.erase(id);
    }
}

void SecurityLevelManager::SetSecurityLevel(SecurityLevelId id) noexcept
{
    currentSecurityLevelType_ = id;
    _notify();
}

bool SecurityLevelManager::SecurityTimerElapsed() const noexcept { return securityTimerElapsed_; }

void SecurityLevelManager::SetSecurityTimerElapsed(bool elapsed) noexcept { securityTimerElapsed_ = elapsed; }

void SecurityLevelManager::_notify() noexcept
{
    std::unique_lock< std::recursive_mutex > lock{lock_};
    for (auto&& notify : notifyOfSecurityLevelChangeCallbackTable_) {
        notify.second(currentSecurityLevelType_);
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft