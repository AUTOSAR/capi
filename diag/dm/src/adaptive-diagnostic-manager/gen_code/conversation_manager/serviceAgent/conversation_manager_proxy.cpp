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
namespace dis {

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

ConversationManagerProxy::ConversationManagerProxy(uint32_t const &serviceInstanceId)
    : BussinessServiceProxy{serviceInstanceId}
{
}

void ConversationManagerProxy::RegisterGetConversationNumberCallback(std::function< std::uint8_t() > callback) noexcept
{
    getConversationNumberCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetActiveStatusCallback(
    std::function< std::uint8_t(std::uint8_t) > callback) noexcept
{
    getActiveStatusCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetSessionIdCallback(std::function< std::uint8_t() > callback) noexcept
{
    getSessionIdCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetSecurityLevelIdCallback(std::function< std::uint8_t() > callback) noexcept
{
    getSecurityLevelIdCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetIdentifierCallback(
    std::function< std::uint64_t(std::uint8_t) > callback) noexcept
{
    getIdentifierCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetDiagnosticSessionShortNameCallback(
    std::function< ara::core::StringView(std::uint8_t) > callback) noexcept
{
    getDiagnosticSessionShortNameCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterGetDiagnosticSecurityLevelShortNameCallback(
    std::function< ara::core::StringView(std::uint8_t) > callback) noexcept
{
    getDiagnosticSecurityLevelShortNameCallback_ = std::move(callback);
}

void ConversationManagerProxy::RegisterResetToDefaultSessionCallback(std::function< void() > callback) noexcept
{
    resetToDefaultSessionCallback_ = std::move(callback);
}

void ConversationManagerProxy::UpdateActiveState(std::uint8_t index, std::uint8_t activeStatus) noexcept
{
    ara::diag::common::LogInfo() << "ConversationManagerProxy::UpdateActiveState| index:" << index
                                 << " activeStatus:" << activeStatus << " inactive:1,active:0";
    std::vector< uint8_t > data;
    serialize::Serialize(data, index, activeStatus);
    Notify(kFuncIDUpdateActiveState, std::move(data));
}

void ConversationManagerProxy::UpdateSessionId(std::uint8_t sessionId) noexcept
{
    std::vector< uint8_t > data;
    serialize::Serialize(data, sessionId);
    Notify(kFuncIDUpdateSessionId, std::move(data));
}

void ConversationManagerProxy::UpdateSecurityLevelId(std::uint8_t securityLevelId) noexcept
{
    std::vector< uint8_t > data;
    serialize::Serialize(data, securityLevelId);
    Notify(kFuncIDUpdateSecurityLevelId, std::move(data));
}

void ConversationManagerProxy::CallHandle(CallRequest &&callData)
{
    if (callData.funcData.funcId == kFuncIDGetConversationNumber) {
        _getConversationNumber(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetActiveStatus) {
        _getActiveStatus(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetSessionId) {
        _getSessionId(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetSecurityLevelId) {
        _getSecurityLevelId(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetIdentifier) {
        _getIdentifier(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetSessionShortName) {
        _getDiagnosticSessionShortName(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetSecurityLevelShortName) {
        _getDiagnosticSecurityLevelShortName(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDResetToDefaultSession) {
        _resetToDefaultSession(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDUpdateActiveState) {
        _updateActiveState(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDUpdateSessionId) {
        _updateSessionId(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDUpdateSecurityLevelId) {
        _updateSecurityLevelId(std::move(callData));
    }
}

void ConversationManagerProxy::_getConversationNumber(CallRequest &&callData) noexcept
{
    RetData retData;
    if (getConversationNumberCallback_) {
        std::uint8_t number{1};
        number = getConversationNumberCallback_();
        serialize::Serialize(retData.retData, number);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getActiveStatus(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t index{};
    if (getActiveStatusCallback_) {
        if (serialize::Deserialize(callData.funcData.data, index) > 0) {
            std::uint8_t activeStatus{1U};
            activeStatus = getActiveStatusCallback_(index);
            serialize::Serialize(retData.retData, activeStatus);
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getSessionId(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t sessionId{};
    if (getSessionIdCallback_) {
        sessionId = getSessionIdCallback_();
        serialize::Serialize(retData.retData, sessionId);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getSecurityLevelId(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t securityLevelId{};
    if (getSecurityLevelIdCallback_) {
        securityLevelId = getSecurityLevelIdCallback_();
        serialize::Serialize(retData.retData, securityLevelId);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getIdentifier(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t index{};
    if (getIdentifierCallback_) {
        if (serialize::Deserialize(callData.funcData.data, index) > 0) {
            std::uint64_t identifier{1};
            identifier = getIdentifierCallback_(index);
            serialize::Serialize(retData.retData, identifier);
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getDiagnosticSessionShortName(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t sessionId{};
    if (getDiagnosticSessionShortNameCallback_) {
        if (serialize::Deserialize(callData.funcData.data, sessionId) > 0) {
            ara::core::String shortName;
            shortName = getDiagnosticSessionShortNameCallback_(sessionId);
            serialize::Serialize(retData.retData, shortName);
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_getDiagnosticSecurityLevelShortName(CallRequest &&callData)
{
    RetData retData;
    std::uint8_t securityLevelId{};
    if (getDiagnosticSecurityLevelShortNameCallback_) {
        if (serialize::Deserialize(callData.funcData.data, securityLevelId) > 0) {
            ara::core::String shortName;
            shortName = getDiagnosticSecurityLevelShortNameCallback_(securityLevelId);
            serialize::Serialize(retData.retData, shortName);
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_resetToDefaultSession(CallRequest &&callData)
{
    RetData retData;
    if (resetToDefaultSessionCallback_) {
        ara::core::String shortName;
        resetToDefaultSessionCallback_();

    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ConversationManagerProxy::_updateActiveState(CallRequest &&callData) { Subscribe(std::move(callData)); }

void ConversationManagerProxy::_updateSessionId(CallRequest &&callData) { Subscribe(std::move(callData)); }

void ConversationManagerProxy::_updateSecurityLevelId(CallRequest &&callData) { Subscribe(std::move(callData)); }

}  // namespace dis
}  // namespace dm
}  // namespace isoft