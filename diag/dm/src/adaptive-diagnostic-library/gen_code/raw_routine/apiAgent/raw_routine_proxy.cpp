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
/// @file       raw_routine_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "raw_routine_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "cancellation/cancellation_handler_factory.h"
#include "netProxy/net_proxy.h"
#include "serialization/serialization.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDStart{1U};
constexpr uint8_t kFuncIDStop{2U};
constexpr uint8_t kFuncIDRequestResults{3U};

RawRoutineProxy::RawRoutineProxy(uint32_t const &serviceId) : BussinessServiceProxy{serviceId} {}

void RawRoutineProxy::RegisterService(ara::diag::internal::RawRoutine *const &gdi) { serviceObj_ = gdi; }

void RawRoutineProxy::CallHandle(CallRequest &&callData)
{
    if (callData.funcData.funcId == kFuncIDStart) {
        _start(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDStop) {
        _stop(std::move(callData));
    } else if (callData.funcData.funcId == kFuncIDRequestResults) {
        _requestResults(std::move(callData));
    }
}

void RawRoutineProxy::_start(CallRequest &&callData)
{
    ara::core::Vector< uint8_t > requestData;
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;

    RetData retData;
    std::vector< uint8_t > buf;

    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, requestData, metaInfoData) > 0) {
            // build obj of param
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< ara::diag::internal::Chunk > futureResult{
                serviceObj_->RawStart(requestData, metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::diag::internal::Chunk > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                serialize::Serialize(retData.retData, result.Value());
            } else {
                retData.errorCode = static_cast< int32_t >(result.Error().Value());
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void RawRoutineProxy::_stop(CallRequest &&callData)
{
    ara::core::Vector< uint8_t > requestData;
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;

    RetData retData;
    std::vector< uint8_t > buf;

    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, requestData, metaInfoData) > 0) {
            // build obj of param
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< ara::diag::internal::Chunk > futureResult{
                serviceObj_->RawStop(requestData, metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::diag::internal::Chunk > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                serialize::Serialize(retData.retData, result.Value());
            } else {
                retData.errorCode = static_cast< int32_t >(result.Error().Value());
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void RawRoutineProxy::_requestResults(CallRequest &&callData)
{
    ara::core::Vector< uint8_t > requestData;
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;

    RetData retData;
    std::vector< uint8_t > buf;

    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, requestData, metaInfoData) > 0) {
            // build obj of param
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< ara::diag::internal::Chunk > futureResult{
                serviceObj_->RawRequestResults(requestData, metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::diag::internal::Chunk > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                serialize::Serialize(retData.retData, result.Value());
            } else {
                retData.errorCode = static_cast< int32_t >(result.Error().Value());
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft