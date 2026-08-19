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
/// @file       ecu_reset_request_impl.cpp
/// @brief      A implementation of EcuResetRequest.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/DMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_03002
/// @unit_name=EcuResetRequestImpl
/// @unit_description=A implementation of EcuResetRequest.
/// @endcode
///
/// ================================================================

#include "ecu_reset_request_impl.h"

#include <ara/core/error_code.h>
#include <ara/sm/error_domain_sm.h>

#include <cstdint>
#include <functional>

#include "define.h"
#include "event.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace dm_comm {

/// @brief Constructor
EcuResetRequestImpl::EcuResetRequestImpl(core::InstanceSpecifier const &specifier) noexcept
    : EcuResetRequest{specifier}
    , appendEventHandler_{nullptr}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
    , kHex{SIXTEEN_L}
{
    log_.LogInfo() << "EcuResetRequestImpl::EcuResetRequestImpl()";
}

/// @brief Destructor
EcuResetRequestImpl::~EcuResetRequestImpl() noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::~EcuResetRequestImpl()";
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler
void EcuResetRequestImpl::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Get the last reset cause
/// @return The type of the last machine reset. error code: kRequestFailed
core::Result< diag::LastResetType > EcuResetRequestImpl::GetLastResetCause() noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::GetLastResetCause()";
    return core::Result< diag::LastResetType >::FromValue(diag::LastResetType::kRegular);
}

/// @brief Enable rapid shutdown
/// @param enable
/// @param metaInfo MetaInfo of the request.
/// @param cancellationHandler Set if the current conversation is canceled.
/// @return core::Future<void> error code: kRejected, kResetTypeNotSupported
core::Future< void > EcuResetRequestImpl::EnableRapidShutdown(bool enable,
                                                              diag::MetaInfo const &metaInfo,
                                                              diag::CancellationHandler cancellationHandler) noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::EnableRapidShutdown(), enable:" << enable;
    std::ignore = metaInfo;
    std::ignore = cancellationHandler;
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};
    promise.set_value();
    return future;
}

/// @brief Called for any EcuRest subFunction, except En-/DisableRapidShutdown.
/// StateManagement needs to evalute carefully if the request to restart parts or the whole machine. Once the request to
/// reset is accepted, the StateManagement has to rely on this decision for the ExecuteReset() trigger.
/// @param resetType Type of the requested reset.
/// @param id id of the custom reset type. Will only be evaluated when resetType is "custom"
/// @param metaInfo MetaInfo of the request.
/// @param cancellationHandler Set if the current conversation is canceled.
/// @return core::Future<void> error code : kRejected, kRequestFailed, kCustomResetTypeNotSupported,
/// kResetTypeNotSupported
core::Future< void > EcuResetRequestImpl::RequestReset(diag::ResetRequestType resetType,
                                                       core::Optional< std::uint8_t > id,
                                                       diag::MetaInfo const &metaInfo,
                                                       diag::CancellationHandler cancellationHandler) noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::RequestReset(), resetType:"
                   << common::ResetRequestTypeToString(_toInternalType(resetType)).c_str() << "id:" << id.value_or(0);
    std::ignore = cancellationHandler;
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};
    if (appendEventHandler_) {
        core::StringView kTA{common::GetkTA()};
        ara::core::Optional< ara::core::StringView > targetAddressRes{metaInfo.GetValue(kTA)};
        core::String targetAddressTypeStr{""};
        int32_t targetAddress{-1};
        if (targetAddressRes.has_value()) {
            core::String const targetAddressStr{core::String(*targetAddressRes)};
            targetAddress = stoi(targetAddressStr, nullptr, kHex);
            core::StringView kTAType{common::GetkTAType()};
            ara::core::Optional< ara::core::StringView > targetAddressTypeRes{metaInfo.GetValue(kTAType)};
            if (targetAddressTypeRes.has_value()) {
                targetAddressTypeStr = core::String(*targetAddressTypeRes);
            }
        }
        // RequestReset request
        common::Event event;
        event.type = common::EventType::kInDiagRequestReset;

        // RequestReset data
        common::DiagRequestResetInfo *const data{
            new common::DiagRequestResetInfo{_toInternalType(resetType), targetAddressTypeStr, targetAddress}};
        event.data           = data;
        event.requestPromise = std::move(promise);
        appendEventHandler_(std::move(event));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "EcuResetRequestImpl::ExecuteReset(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief Execute reset
/// @param metaInfo
void EcuResetRequestImpl::ExecuteReset(diag::MetaInfo metaInfo) noexcept
{
    log_.LogInfo() << "EcuResetRequestImpl::ExecuteReset()";
    std::ignore = metaInfo;
}

/// @brief Convert diag reset type to SM internal reset type
/// @param resetType Diag reset type
/// @return SM internal reset type
common::DiagResetRequestInternalType EcuResetRequestImpl::_toInternalType(
    diag::ResetRequestType const resetType) const noexcept
{
    std::ignore = appendEventHandler_;
    common::DiagResetRequestInternalType diagResetRequestInternalType{common::DiagResetRequestInternalType::kSoftReset};
    switch (resetType) {
        case diag::ResetRequestType::kSoftReset: {
            diagResetRequestInternalType = common::DiagResetRequestInternalType::kSoftReset;
        } break;
        case diag::ResetRequestType::kHardReset: {
            diagResetRequestInternalType = common::DiagResetRequestInternalType::kHardReset;
        } break;
        case diag::ResetRequestType::kKeyOffOnReset: {
            diagResetRequestInternalType = common::DiagResetRequestInternalType::kKeyOffOnReset;
        } break;
        case diag::ResetRequestType::kCustomReset: {
            diagResetRequestInternalType = common::DiagResetRequestInternalType::kCustomReset;
        } break;
        default: {
            diagResetRequestInternalType = common::DiagResetRequestInternalType::kSoftReset;
        } break;
    }
    return diagResetRequestInternalType;
}

/// @brief Convert SM internal reset type to diag reset type
/// @param resetType SM internal reset type
/// @return Diag reset type
diag::ResetRequestType EcuResetRequestImpl::_toDiagType(
    common::DiagResetRequestInternalType const resetType) const noexcept
{
    std::ignore = appendEventHandler_;
    diag::ResetRequestType diagResetRequestInternalType{diag::ResetRequestType::kSoftReset};
    switch (resetType) {
        case common::DiagResetRequestInternalType::kSoftReset: {
            diagResetRequestInternalType = diag::ResetRequestType::kSoftReset;
        } break;
        case common::DiagResetRequestInternalType::kHardReset: {
            diagResetRequestInternalType = diag::ResetRequestType::kHardReset;
        } break;
        case common::DiagResetRequestInternalType::kKeyOffOnReset: {
            diagResetRequestInternalType = diag::ResetRequestType::kKeyOffOnReset;
        } break;
        case common::DiagResetRequestInternalType::kCustomReset: {
            diagResetRequestInternalType = diag::ResetRequestType::kCustomReset;
        } break;
        default: {
            diagResetRequestInternalType = diag::ResetRequestType::kSoftReset;
        } break;
    }
    return diagResetRequestInternalType;
}
}  // namespace dm_comm
}  // namespace sm
}  // namespace ara
