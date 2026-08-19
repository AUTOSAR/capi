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
/// @file       update_request_impl.cpp
/// @brief      A implementation of ara::sm::skeleton::UpdateRequestSkeleton.
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/UCMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @unit_name=UpdateRequestImpl
/// @unit_description=A implementation of ara::sm::skeleton::UpdateRequestSkeleton.
/// @endcode
///
/// ================================================================

#include "update_request_impl.h"

#include "define.h"
#include "event.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace ucm_comm {

/// @brief Destructor
UpdateRequestImpl::~UpdateRequestImpl() noexcept { log_.LogInfo() << "UpdateRequestImpl::~UpdateRequestImpl()"; }

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void UpdateRequestImpl::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "UpdateRequestImpl::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Method called, when a ResetMachine request is issued.
/// @return ara::com::Future  Holds no payload.
ara::core::Future< void > UpdateRequestImpl::ResetMachine() noexcept
{
    log_.LogInfo() << "UpdateRequestImpl::ResetMachine()";
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInUCMResetMachine;
        request.data           = nullptr;
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::ResetMachine(), appendEventHandler_ is nullptr";
    }

    return future;
}

/// @brief  Method called, when a StartUpdateSession request is issued.
/// @return ara::com::Future  Holds no payload.
ara::core::Future< void > UpdateRequestImpl::StartUpdateSession() noexcept
{
    log_.LogInfo() << "UpdateRequestImpl::StartUpdateSession()";
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInUCMStartUpdateSession;
        request.data           = nullptr;
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::StartUpdateSession(), appendEventHandler_ is nullptr";
    }

    return future;
}

/// @brief Method called, when a StopUpdateSession request is issued.
/// @return ara::com::Future  Holds no payload.
void UpdateRequestImpl::StopUpdateSession() noexcept
{
    log_.LogInfo() << "UpdateRequestImpl::StopUpdateSession()";
    core::Promise< void > promise;

    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInUCMStopUpdateSession;
        request.data           = nullptr;
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::StopUpdateSession(), appendEventHandler_ is nullptr";
    }
}

/// @brief Method called, when a PrepareUpdate request is issued.
/// @param[in] functionGroupList  The list of FunctionGroups within the SoftwareCluster to be prepared to be updated.
/// @return ara::com::Future  Holds no payload.
ara::core::Future< void > UpdateRequestImpl::PrepareUpdate(
    ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept

{
    log_.LogInfo() << "UpdateRequestImpl::PrepareUpdate(), begin with fgList:"
                   << common::ConcatenateStrings(functionGroupList).c_str();
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type = common::EventType::kInUCMPrepareUpdate;

        common::FGListInfo *const data{new common::FGListInfo()};
        data->fgList = functionGroupList;
        request.data = data;

        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::PrepareUpdate(), appendEventHandler_ is nullptr";
    }

    return future;
}

/// @brief Method called, when a VerifyUpdate request is issued.
/// @param[in] functionGroupList The list of FunctionGroups within the SoftwareCluster to be verified.
/// @return ara::com::Future  Holds no payload.
ara::core::Future< void > UpdateRequestImpl::VerifyUpdate(
    ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept

{
    log_.LogInfo() << "UpdateRequestImpl::VerifyUpdate(), begin with fgList:"
                   << common::ConcatenateStrings(functionGroupList).c_str();
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type = common::EventType::kInUCMVerifyUpdate;

        common::FGListInfo *const data{new common::FGListInfo()};
        data->fgList = functionGroupList;
        request.data = data;

        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::VerifyUpdate(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief Method called, when a PrepareRollback request is issued.
/// @param[in] functionGroupList  The list of FunctionGroups within the SoftwareCluster to be prepared to roll back.
/// @return ara::com::Future  Holds no payload.
ara::core::Future< void > UpdateRequestImpl::PrepareRollback(
    ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept
{
    log_.LogInfo() << "UpdateRequestImpl::PrepareRollback(), begin with fgList:"
                   << common::ConcatenateStrings(functionGroupList).c_str();
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type = common::EventType::kInUCMPrepareRollback;

        common::FGListInfo *const data{new common::FGListInfo()};
        data->fgList = functionGroupList;
        request.data = data;

        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "UpdateRequestImpl::VerifyUpdate(), appendEventHandler_ is nullptr";
    }

    return future;
}

}  // namespace ucm_comm
}  // namespace sm
}  // namespace ara
