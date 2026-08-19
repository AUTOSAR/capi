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
/// @file       sm_service.cpp
/// @brief      sm service implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UpdateRequestService
/// @unit_description=sm service implementation
/// @endcode
///
/// ================================================================

#include "sm_service.h"

#include <thread>

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"  // generated
#include "common/log.h"
#include "util/future_ext.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

// helpers
/// @brief FutureWithPrepareUpdateFailed
/// @return future with result
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10369, AD_UCM_10371, AD_UCM_10372, AD_UCM_10373, AD_UCM_10374
/// @trace_id_dd=DD_UCM_10873, DD_UCM_10875, DD_UCM_10876, DD_UCM_10877, DD_UCM_10878
/// @needwork = ad
/// @endcode
static AraFutureVoid FutureWithPrepareUpdateFailed()
{
    return SyncCallVoid([]() { return AraResultVoid(UCMErrorDomainErrc::kPreActivationFailed); });
}

///UpdateRequestService::UpdateRequestService() noexcept :
///    service_{nullptr},
///    serviceLock_{},
///    maxWaitTime_{2000},
///    retryCount_{10},
///    retryInterval_{500},
///    businessCallback_{nullptr}, {}

/// @brief SetMaxWaitTime
/// @param maxWaitTime
/// @throws no
void UpdateRequestService::SetMaxWaitTime(std::int32_t const maxWaitTime) noexcept { maxWaitTime_ = maxWaitTime; }

/// @brief init sm adapter
/// @return bool
/// @throws no
bool UpdateRequestService::Init() noexcept { return (nullptr != service_); }
/// @brief AsyncInit
/// @param cb
/// @throws no
void UpdateRequestService::AsyncInit(FunctionVoid&& cb) noexcept
{
    LOGI << "call start find service...";
    businessCallback_ = std::move(cb);
    std::ignore       = UpdateRequestProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer< UpdateRequestProxy::HandleType > handles,
               ara::com::FindServiceHandle const& handler) {
            _genConfiguredUpdateRequestService(std::move(handles), handler);
        },
        ara::core::InstanceSpecifier(std::move(AraStringView("ucmd/package_manager_root/UpdateRequestRPort"))));
}

/// @brief start update session with sm
/// @return future with result
/// @throws no
AraFutureVoid UpdateRequestService::StartUpdateSession() noexcept
{
    LOGI << "call...";
    if (Init()) {
        return service_->StartUpdateSession();
    }

    return FutureWithPrepareUpdateFailed();
}
/// @brief stop update session with sm
/// @throws no
void UpdateRequestService::StopUpdateSession() noexcept
{
    LOGI << "call...";
    if (Init()) {
        service_->StopUpdateSession();
    }
}
/// @brief reset machine with sm
/// @return future with result
/// @throws no
AraFutureVoid UpdateRequestService::ResetMachine() noexcept
{
    LOGI << "call...";
    if (Init()) {
        return service_->ResetMachine();
    }

    return FutureWithPrepareUpdateFailed();
}

/// @brief prepare update for function group list with sm
/// @param functionGroups
/// @return future with result
/// @throws no
AraFutureVoid UpdateRequestService::PrepareUpdate(AraVectorString const& functionGroups) noexcept
{
    LOGI << "call..." << strutil::JoinVector(functionGroups).c_str();
    if (Init()) {
        AraVector< AraString > const functionGroupVec{functionGroups.begin(), functionGroups.end()};
        return service_->PrepareUpdate(functionGroupVec);
    }

    return FutureWithPrepareUpdateFailed();
}
/// @brief verify update for function group list with sm
/// @param functionGroups
/// @return future with result
/// @throws no
AraFutureVoid UpdateRequestService::VerifyUpdate(AraVectorString const& functionGroups) noexcept
{
    LOGI << "call..." << strutil::JoinVector(functionGroups).c_str();
    if (Init()) {
        AraVector< AraString > const functionGroupVec{functionGroups.begin(), functionGroups.end()};
        return service_->VerifyUpdate(functionGroupVec);
    }

    return FutureWithPrepareUpdateFailed();
}
/// @brief prepare rollback for function group list with sm
/// @param functionGroups
/// @return future with result
/// @throws no
AraFutureVoid UpdateRequestService::PrepareRollback(AraVectorString const& functionGroups) noexcept
{
    LOGI << "call..." << strutil::JoinVector(functionGroups).c_str();
    if (Init()) {
        AraVector< AraString > const functionGroupVec{functionGroups.begin(), functionGroups.end()};
        return service_->PrepareRollback(functionGroupVec);
    }

    return FutureWithPrepareUpdateFailed();
}

/// @brief RetryResetMachine
/// @return bool
/// @throws no
bool UpdateRequestService::RetryResetMachine() noexcept
{
    return _retryUpdateMethod([this]() noexcept -> AraFutureVoid { return ResetMachine(); });
}
/// @brief RetryPrepareUpdate
/// @param functionGroups
/// @return bool
/// @throws no
bool UpdateRequestService::RetryPrepareUpdate(AraVectorString const& functionGroups) noexcept
{
    return _retryUpdateMethod(
        [this, &functionGroups]() noexcept -> AraFutureVoid { return PrepareUpdate(functionGroups); });
}
/// @brief RetryVerifyUpdate
/// @param functionGroups
/// @return bool
/// @throws no
bool UpdateRequestService::RetryVerifyUpdate(AraVectorString const& functionGroups) noexcept
{
    return _retryUpdateMethod(
        [this, &functionGroups]() noexcept -> AraFutureVoid { return VerifyUpdate(functionGroups); });
}
/// @brief RetryPrepareRollback
/// @param functionGroups
/// @return bool
/// @throws no
bool UpdateRequestService::RetryPrepareRollback(AraVectorString const& functionGroups) noexcept
{
    return _retryUpdateMethod(
        [this, &functionGroups]() noexcept -> AraFutureVoid { return PrepareRollback(functionGroups); });
}

/// @brief retry update method
/// @param method
/// @return bool
/// @throws no
bool UpdateRequestService::_retryUpdateMethod(std::function< AraFutureVoid() >&& method) const noexcept
{
    bool success{false};
    std::int32_t count{0};

    do {
        count++;

        // COM currently does not have a timeout setting, but the call can be cancelled in the future (just delete the returned future)
        std::chrono::system_clock::time_point const waitUntilTime{
            std::chrono::system_clock::now() + std::chrono::milliseconds(maxWaitTime_)};  // TODO////mytodo
        std::time_t const t{std::chrono::system_clock::to_time_t(waitUntilTime)};
        LOGI << "count:" << count
             << ", waitUntilTime:" << std::ctime(&t);  // NOLINT : [std::ctime]function is not thread safe

        AraFutureVoid fu{method()};
        AraFutureStatus const status{fu.wait_until(waitUntilTime)};
        if (AraFutureStatus::ready == status) {
            AraResultVoid const ret{fu.GetResult()};
            if (ret.HasValue()) {
                LOGI << "ready and success";
                success = true;
                break;
            }

            if (ara::sm::SMErrc::kRejected == static_cast< ara::sm::SMErrc >(ret.Error().Value())) {
                LOGE << "ready but failed, receive err kRejected";
                ///std::this_thread::sleep_for(std::chrono::milliseconds(retryInterval_));.
                ///continue;
            } else {
                LOGE << "ready but failed, other err:" << ret.Error() << ret.Error().Message().data();
                ///break;
            }
        } else if (AraFutureStatus::timeout == status) {
            LOGE << "timeout";
            ///break;
        } else {
            ;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(retryInterval_));
    } while (count < retryCount_);

    return success;
}

/// @brief callback after async find sm service
/// @param handles
/// @param handler
/// @throws no
void UpdateRequestService::_genConfiguredUpdateRequestService(
    ara::com::ServiceHandleContainer< UpdateRequestProxy::HandleType > handles,
    ara::com::FindServiceHandle const& handler) noexcept
{
    LOGI << "after start find service...";

    if (handles.empty()) {
        LOGE << "no instance is available";
        return;
    }

    std::ignore = std::for_each(
        handles.begin(),
        handles.begin()
            + static_cast< ara::com::ServiceHandleContainer< UpdateRequestProxy::HandleType >::difference_type >(
                handles.size()),
        [](UpdateRequestProxy::HandleType const& it) {
            LOGD << "instance " << it.GetInstanceId().ToString().data() << " is available";
        });

    std::lock_guard< std::mutex > const lock{serviceLock_};
    if (service_) {
        LOGI << "already init service, do nothing";
        return;
    }

    service_ = std::make_shared< UpdateRequestProxy >(UpdateRequestProxy::Create(handles[0U]).Value());
    LOGI << "use instance:" << service_->GetHandle().GetInstanceId().ToString().data();

    LOGI << "stop find service...";
    UpdateRequestProxy::StopFindService(handler);

    LOGI << "handle business callback...";
    businessCallback_();
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
