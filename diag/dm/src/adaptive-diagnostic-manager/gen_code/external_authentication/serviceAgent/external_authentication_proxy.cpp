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
/// @file       external_authentication_proxy.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "external_authentication_proxy.h"

#include <thread>

#include "ara/diag/diag_error_domain.h"
#include "isoft/define.h"
#include "log/log.h"
#include "netProxy/net_proxy.h"
#include "serialization/serialization.h"
namespace isoft {
namespace dm {
namespace dis {

using isoft::dm::RetData;

constexpr uint8_t kFuncIDHasClientAddress{1U};
constexpr uint8_t kFuncIDSetAuthState{2U};
constexpr uint8_t kFuncIDSetNotify{3U};
constexpr uint8_t kFuncIDAddDynamicAccessList{4U};
constexpr uint8_t kFuncIDClearDynamicAccessList{5U};
constexpr uint8_t kFuncIDRevoke{6U};
constexpr uint8_t kFuncIDRefresh{7U};
constexpr uint8_t kFuncIDGetAllAddress{8U};
ExternalAuthenticationProxy::ExternalAuthenticationProxy(uint32_t const &serviceInstanceId)
    : BussinessServiceProxy{serviceInstanceId}
{
    // AccessClient::GetComProxy().RegisterNotificationCallBack(instanceId,
    //  std::move([this](FuncData const& funcData){Notify(funcData);}));
}

void ExternalAuthenticationProxy::RegisterHasClientAddressCallback(HasClientAddressCallback callback) noexcept
{
    hasClientAddressCallback_ = std::move(callback);
}
void ExternalAuthenticationProxy::RegisterSetAuthStateCallback(SetAuthStateCallback callback) noexcept
{
    setAuthStateCallback_ = std::move(callback);
}

void ExternalAuthenticationProxy::RegisterAddDynamicAccessListCallback(AddDynamicAccessListCallback callback) noexcept
{
    addDynamicAccessListCallback_ = std::move(callback);
}
void ExternalAuthenticationProxy::RegisterSetDynamicAccessListCallback(SetDynamicAccessListCallback callback) noexcept
{
    setDynamicAccessListCallback_ = std::move(callback);
}
void ExternalAuthenticationProxy::RegisterRevokeCallback(RevokeCallback callback) noexcept
{
    revokeCallback_ = std::move(callback);
}
void ExternalAuthenticationProxy::RegisterRefreshCallback(RefreshCallback callback) noexcept
{
    refreshCallback_ = std::move(callback);
}

void ExternalAuthenticationProxy::RegisterGetAllAddressCallback(GetAllAddressCallback callback) noexcept
{
    getAllAddressCallback_ = std::move(callback);
}

void ExternalAuthenticationProxy::UpdateAuthState(std::uint16_t address, bool isAuth) noexcept
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, address, isAuth) < 0) {
        return;
    }
    Notify(kFuncIDSetNotify, std::move(payLoad));
}

void ExternalAuthenticationProxy::CallHandle(CallRequest &&callData)
{
    if (callData.funcData.funcId == kFuncIDHasClientAddress) {
        _hasClientAddress(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDSetAuthState) {
        _setAuthState(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDSetNotify) {
        Subscribe(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDAddDynamicAccessList) {
        _addDynamicAccessList(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDClearDynamicAccessList) {
        _setDynamicAccessList(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDRevoke) {
        _revoke(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDRefresh) {
        _refresh(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDGetAllAddress) {
        _getAllAddress(std::move(callData));
    }
}

void ExternalAuthenticationProxy::_hasClientAddress(CallRequest &&callData) noexcept
{
    RetData retData;
    if (hasClientAddressCallback_) {
        std::uint16_t address{};
        if (serialize::Deserialize(callData.funcData.data, address) > isoft::kSuccess) {
            // build obj of param
            bool has = hasClientAddressCallback_(address);
            serialize::Serialize(retData.retData, has);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_"
                                            "hasClientAddress| deserialize fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_hasClientAddress| "
                                        "hasClientAddressCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_setAuthState(CallRequest &&callData) noexcept
{
    RetData retData;
    if (setAuthStateCallback_) {
        std::uint16_t address{};
        bool state{};
        std::vector< ara::core::String > roleList{};

        std::uint64_t msecond{};
        if (serialize::Deserialize(callData.funcData.data, address, state, roleList, msecond) > isoft::kSuccess) {
            // build obj of param
            std::vector< std::string > roleListStr{};
            for (auto &&role : roleList) {
                roleListStr.emplace_back(role.c_str());
            }

            setAuthStateCallback_(address, state, roleListStr, msecond);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_setAuthState| deserialize fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_setAuthState| setAuthStateCallback_ "
                                        "not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_addDynamicAccessList(CallRequest &&callData) noexcept
{
    RetData retData;
    if (addDynamicAccessListCallback_) {
        std::uint16_t address{};
        std::vector< std::uint8_t > dynamicAccessList;
        if (serialize::Deserialize(callData.funcData.data, address, dynamicAccessList) > isoft::kSuccess) {
            // build obj of param
            addDynamicAccessListCallback_(address, dynamicAccessList);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_addDynamicAccessList| deserialize "
                                            "fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_addDynamicAccessList| "
                                        "addDynamicAccessListCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_setDynamicAccessList(CallRequest &&callData) noexcept
{
    RetData retData;
    if (setDynamicAccessListCallback_) {
        std::uint16_t address{};
        std::vector< std::uint8_t > dynamicAccessList;
        if (serialize::Deserialize(callData.funcData.data, address, dynamicAccessList) > isoft::kSuccess) {
            // build obj of param
            setDynamicAccessListCallback_(address, dynamicAccessList);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_setDynamicAccessList| deserialize "
                                            "fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_setDynamicAccessList| "
                                        "SetDynamicAccessListCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_revoke(CallRequest &&callData) noexcept
{
    RetData retData;
    if (revokeCallback_) {
        std::uint16_t address{};
        if (serialize::Deserialize(callData.funcData.data, address) > isoft::kSuccess) {
            // build obj of param
            revokeCallback_(address);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_revoke| deserialize fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_revoke| "
                                        "revokeCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_refresh(CallRequest &&callData) noexcept
{
    RetData retData;
    if (refreshCallback_) {
        std::uint16_t address{};
        if (serialize::Deserialize(callData.funcData.data, address) > isoft::kSuccess) {
            // build obj of param
            refreshCallback_(address);
        } else {
            ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_refresh| deserialize fails!";
            retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
        }

    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_refresh| "
                                        "refreshCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void ExternalAuthenticationProxy::_getAllAddress(CallRequest &&callData) noexcept
{
    RetData retData;
    if (getAllAddressCallback_) {
        // build obj of param
        auto table  = getAllAddressCallback_();
        std::ignore = serialize::Serialize(retData.retData, table);
    } else {
        ara::diag::common::LogWarn() << "ExternalAuthenticationProxy::_refresh| "
                                        "refreshCallback_ not register!";
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft
