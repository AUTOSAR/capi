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
/// @file       conversation_manager_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-18
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "conversation_manager_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "netProxy/net_proxy.h"
#include "serialization/serialization.h"
#include "thread_pool/thread_pool.h"
namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDGetConversationNumber{0x1U};
constexpr uint8_t kFuncIDGetActiveStatus{0x2U};
constexpr uint8_t kFuncIDGetSessionId{0x3U};
constexpr uint8_t kFuncIDGetSecurityLevelId{0x4U};
constexpr uint8_t kFuncIDUpdateActiveState{0x5U};
constexpr uint8_t kFuncIDUpdateSessionId{0x6U};
constexpr uint8_t kFuncIDUpdateSecurityLevelId{0x7U};
constexpr uint8_t kFuncIDResetToDefaultSession{0x8U};
constexpr uint8_t kFuncIDGetSessionShortName{0x9U};
constexpr uint8_t kFuncIDGetSecurityLevelShortName{0xAU};
constexpr uint8_t kFuncIDGetIdentifier{0xBU};

ConversationManagerProxy::ConversationManagerProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { Notify(funcData); });
}

void ConversationManagerProxy::RegisterUpdateActiveStatusCallback(
    std::function< void(std::uint8_t, std::uint8_t) > callback) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDUpdateActiveState};
    static uint8_t s_CallId{0U};
    std::unique_lock< std::mutex > lock{lock_};
    ara::core::Result< void > result = RegisterCallBack(kFun_Id, s_CallId);
    if (result.HasValue()) {
        updateActiveStatus_ = std::move(callback);
    } else {
        ara::diag::common::LogWarn()
            << "ConversationManagerProxy::RegisterUpdateActiveStatusCallback| register callback fails!";
    }
}

void ConversationManagerProxy::RegisterUpdateSessionIdCallback(std::function< void(std::uint8_t) > callback) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDUpdateSessionId};
    static uint8_t s_CallId{0U};
    std::unique_lock< std::mutex > lock{lock_};
    ara::core::Result< void > result = RegisterCallBack(kFun_Id, s_CallId);
    if (result.HasValue()) {
        updateSessionId_ = std::move(callback);
    } else {
        ara::diag::common::LogWarn()
            << "ConversationManagerProxy::RegisterUpdateSessionIdCallback| register callback fails!";
    }
}

void ConversationManagerProxy::RegisterUpdateSecurityLevelIdCallback(
    std::function< void(std::uint8_t) > callback) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDUpdateSecurityLevelId};
    static uint8_t s_CallId{0U};
    std::unique_lock< std::mutex > lock{lock_};
    ara::core::Result< void > result = RegisterCallBack(kFun_Id, s_CallId);
    if (result.HasValue()) {
        updateSecurityLevelId_ = std::move(callback);
    } else {
        ara::diag::common::LogWarn()
            << "ConversationManagerProxy::RegisterUpdateSecurityLevelIdCallback| register callback fails!";
    }
}

ara::core::Result< std::uint8_t > ConversationManagerProxy::GetConversationNumber() noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetConversationNumber};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint8_t > result{SyncCallFunc< std::uint8_t >(kFun_Id, s_CallId)};

    return result;
}

ara::core::Result< std::uint8_t > ConversationManagerProxy::GetActiveStatus(std::uint8_t index) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetActiveStatus};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint8_t > result{SyncCallFunc< std::uint8_t >(kFun_Id, s_CallId, index)};

    return result;
}

ara::core::Result< std::uint8_t > ConversationManagerProxy::GetSessionId() noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetSessionId};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint8_t > result{SyncCallFunc< std::uint8_t >(kFun_Id, s_CallId)};

    return result;
}

ara::core::Result< std::uint8_t > ConversationManagerProxy::GetSecurityLevelId() noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetSecurityLevelId};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint8_t > result{SyncCallFunc< std::uint8_t >(kFun_Id, s_CallId)};

    return result;
}

ara::core::Result< std::uint64_t > ConversationManagerProxy::GetIdentifier(std::uint8_t const index) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetIdentifier};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint64_t > result{SyncCallFunc< std::uint64_t >(kFun_Id, s_CallId, index)};

    return result;
}

/// @brief Get diagnostic Session name
/// @param[in] index Conversation index value
/// @param[in] session Session ID
/// @return Session name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManagerProxy::GetDiagnosticSessionShortName(
    std::uint8_t const session) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetSessionShortName};
    static uint8_t s_CallId{0U};
    ara::core::Result< ara::core::String > result{SyncCallFunc< ara::core::String >(kFun_Id, s_CallId, session)};
    if (result.HasValue()) {
        ara::core::String shortName{result.Value().c_str()};
        ara::diag::common::LogInfo() << "ConversationManagerProxy::GetDiagnosticSessionShortName| session short name:"
                                     << shortName;
        return ara::core::Result< ara::core::String >::FromValue(std::move(shortName));
    }
    return ara::core::Result< ara::core::String >::FromError(result.Error());
}

/// @brief Get diagnostic SecurityLevel name
/// @param[in] index Conversation index value
/// @param[in] securityLevel Security level
/// @return SecurityLevel name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManagerProxy::GetDiagnosticSecurityLevelShortName(
    std::uint8_t const securityLevel) noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDGetSecurityLevelShortName};
    static uint8_t s_CallId{0U};
    ara::core::Result< ara::core::String > result{SyncCallFunc< ara::core::String >(kFun_Id, s_CallId, securityLevel)};
    if (result.HasValue()) {
        ara::diag::common::LogInfo()
            << "ConversationManagerProxy::GetDiagnosticSecurityLevelShortName| security level short name:"
            << result.Value().c_str();
        return ara::core::Result< ara::core::String >::FromValue(result.Value().c_str());
    }
    return ara::core::Result< ara::core::String >::FromError(result.Error());
}

/// @brief Set to default Session
/// @param[in] identifier Conversation identifier
/// @return Setting result
/// @throws on overflow
ara::core::Result< void > ConversationManagerProxy::ResetToDefaultSession() const noexcept
{
    constexpr uint8_t kFun_Id{kFuncIDResetToDefaultSession};
    static uint8_t s_CallId{0U};
    ara::core::Result< void > result{SyncCallFunc< void >(kFun_Id, s_CallId)};

    return result;
}

/// @brief Register the callback function when the network service is Ready, the callback will be called according to the current network status only after calling Start to start
/// @return callback Callback function when the network service is Ready
/// @throw unknown
void ConversationManagerProxy::RegisterOnReady(FuncOnReadyCallBack const& callback) noexcept
{
    if (IsServiceReady()) {
        callback(true);
    }

    RegisterOnServiceReady(callback);
}

void ConversationManagerProxy::Notify(FuncData const& funcData)
{
    std::unique_lock< std::mutex > lock{lock_};
    switch (funcData.funcId) {
        case kFuncIDUpdateActiveState: {
            UpdateActiveStatus(funcData);
        } break;
        case kFuncIDUpdateSessionId: {
            UpdateSessionId(funcData);
        } break;
        case kFuncIDUpdateSecurityLevelId: {
            UpdateSecurityLevelId(funcData);
        } break;
    }
}

void ConversationManagerProxy::UpdateActiveStatus(FuncData const& funcData)
{
    std::uint8_t index{};
    std::uint8_t status{};
    if (serialize::Deserialize(funcData.data, index, status) > 0) {
        isoft::ThreadPool::GetInstance().Submit([this, index, status]() {
            if (updateActiveStatus_) {
                updateActiveStatus_(index, status);
            }
        });

    } else {
        ara::diag::common::LogWarn() << "ConversationManagerProxy::UpdateActiveStatus| serialize::Deserialize fails!";
    }
}
void ConversationManagerProxy::UpdateSessionId(FuncData const& funcData)
{
    std::uint8_t sessionId{};
    if (serialize::Deserialize(funcData.data, sessionId) > 0) {
        isoft::ThreadPool::GetInstance().Submit([this, sessionId]() {
            if (updateSessionId_) {
                updateSessionId_(sessionId);
            }
        });

    } else {
        ara::diag::common::LogWarn() << "ConversationManagerProxy::UpdateSessionId| serialize::Deserialize fails!";
    }
}
void ConversationManagerProxy::UpdateSecurityLevelId(FuncData const& funcData)
{
    std::uint8_t securityLevel{};
    if (serialize::Deserialize(funcData.data, securityLevel) > 0) {
        isoft::ThreadPool::GetInstance().Submit([this, securityLevel]() {
            if (updateSecurityLevelId_) {
                updateSecurityLevelId_(securityLevel);
            }
        });

    } else {
        ara::diag::common::LogWarn()
            << "ConversationManagerProxy::UpdateSecurityLevelId| serialize::Deserialize fails!";
    }
}
}  // namespace dic
}  // namespace dm
}  // namespace isoft