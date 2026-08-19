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
/// @file       raw_data_identifier_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-06
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "raw_data_identifier_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "cancellation/cancellation_handler_factory.h"
#include "netProxy/net_proxy.h"
#include "serialization/serialization.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDRead{0x3U};
constexpr uint8_t kFuncIDWrite{0x4U};

RawDataIdentifierProxy::RawDataIdentifierProxy(uint32_t const &serviceId) : BussinessServiceProxy{serviceId} {}

void RawDataIdentifierProxy::RegisterService(ara::diag::internal::RawDataIdentifier *const &gdi) { serviceObj_ = gdi; }

void RawDataIdentifierProxy::CallHandle(CallRequest &&callData)
{
    std::uint8_t funcId{callData.funcData.funcId};
    if (reentrancyType_.read == ara::diag::ReentrancyType::kNot && kFuncIDRead == funcId) {
        std::unique_lock< std::mutex > lock{reentrancyLock_};
        if (processRead_) {
            RetData retData{};
            retData.errorCode = isoft::serialize::kInt32_0x10;
            lock.unlock();
            NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
            return;
        }
        processRead_ = true;
    }
    if (reentrancyType_.readWrite == ara::diag::ReentrancyType::kNot && kFuncIDRead == funcId) {
        std::unique_lock< std::mutex > lock{reentrancyLock_};
        if (processWrite_) {
            RetData retData{};
            retData.errorCode = isoft::serialize::kInt32_0x10;
            lock.unlock();
            NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
            return;
        }
        processRead_ = true;
    }
    if (reentrancyType_.write == ara::diag::ReentrancyType::kNot && kFuncIDWrite == funcId) {
        std::unique_lock< std::mutex > lock{reentrancyLock_};
        if (processWrite_) {
            RetData retData{};
            retData.errorCode = isoft::serialize::kInt32_0x10;
            lock.unlock();
            NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
            return;
        }
        processWrite_ = true;
    }
    if (reentrancyType_.readWrite == ara::diag::ReentrancyType::kNot && kFuncIDWrite == funcId) {
        std::unique_lock< std::mutex > lock{reentrancyLock_};
        if (processRead_) {
            RetData retData{};
            retData.errorCode = isoft::serialize::kInt32_0x10;
            lock.unlock();
            NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
            return;
        }
        processWrite_ = true;
    }
    switch (callData.funcData.funcId) {
        case kFuncIDRead: {
            _read(std::move(callData));
            break;
        }
        case kFuncIDWrite: {
            _write(std::move(callData));
            break;
        }
        default: {
        } break;
    }
}

void RawDataIdentifierProxy::_read(CallRequest &&callData)
{
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;

    RetData retData;
    std::vector< uint8_t > buf;
    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, metaInfoData) > 0) {
            // build obj of param
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< ara::core::Vector< std::uint8_t > > futureResult{
                serviceObj_->RawRead(metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::core::Vector< std::uint8_t > > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                if (serialize::Serialize(buf, result.Value()) > 0) {
                    retData.retData = std::move(buf);
                }
            } else {
                retData.errorCode = static_cast< int32_t >(result.Error().Value());
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    if (serialize::Serialize(buf, retData) > 0) {
        NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
    }
}

void RawDataIdentifierProxy::_write(CallRequest &&callData)
{
    ara::core::Vector< std::uint8_t > requestData;
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;
    RetData retData;
    std::vector< uint8_t > buf;
    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, requestData, metaInfoData) > 0) {
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< void > futureResult{
                serviceObj_->RawWrite(requestData, metaInfo, std::move(cancellationHandler))};
            ara::core::Result< void > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                retData.errorCode = 0;
            } else {
                retData.errorCode = static_cast< int32_t >(result.Error().Value());
            }
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kServiceNoFound);
    }
    if (serialize::Serialize(buf, retData) > 0) {
        NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
    }
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft