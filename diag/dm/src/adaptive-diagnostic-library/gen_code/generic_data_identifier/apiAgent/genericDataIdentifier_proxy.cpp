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
/// @file       genericDataIdentifier_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-06
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "genericDataIdentifier_proxy.h"

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

GenericDataIdentifierProxy::GenericDataIdentifierProxy(uint32_t const &serviceId) : BussinessServiceProxy{serviceId} {}

void GenericDataIdentifierProxy::RegisterService(ara::diag::GenericDataIdentifier *const &gdi) { serviceObj_ = gdi; }

void GenericDataIdentifierProxy::CallHandle(CallRequest &&callData)
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
            {
                std::unique_lock< std::mutex > lock{reentrancyLock_};
                processRead_ = false;
            }
            break;
        }
        case kFuncIDWrite: {
            _write(std::move(callData));
            {
                std::unique_lock< std::mutex > lock{reentrancyLock_};
                processWrite_ = false;
            }
            break;
        }
        default: {
        } break;
    }
}

void GenericDataIdentifierProxy::_read(CallRequest &&callData)
{
    std::uint16_t dataIdentifier{};
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;

    RetData retData;
    std::vector< uint8_t > buf;
    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, dataIdentifier, metaInfoData) > 0) {
            // build obj of param
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

            ara::core::Future< ara::diag::GenericDataIdentifier::OperationOutput > futureResult{
                serviceObj_->Read(dataIdentifier, metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::diag::GenericDataIdentifier::OperationOutput > result{futureResult.GetResult()};
            UnregisterOnCancel(callData.funcData.funcId, callData.funcData.callId);
            if (result.HasValue()) {
                if (serialize::Serialize(buf, result.Value().responseData) > 0) {
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

void GenericDataIdentifierProxy::_write(CallRequest &&callData)
{
    std::uint16_t dataIdentifier{};
    ara::core::Vector< std::uint8_t > requestData;
    ara::core::Map< ara::core::String, ara::core::String > metaInfoData;
    RetData retData;
    std::vector< uint8_t > buf;
    if (serviceObj_ != nullptr) {
        if (serialize::Deserialize(callData.funcData.data, dataIdentifier, requestData, metaInfoData) > 0) {
            ara::diag::MetaInfo metaInfo{ara::diag::api::CreateMetaInfo(metaInfoData)};
            ara::diag::CancellationHandler cancellationHandler
                = isoft::dm::CancellationHandlerFactory::Create(cancellationEventManager_, metaInfo);
            std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr
                = cancellationEventManager_.Get(metaInfo);
            RegisterOnCancel(callData.funcData.funcId, callData.funcData.callId,
                             [cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
            ara::core::Future< void > futureResult{serviceObj_->Write(
                dataIdentifier, ara::core::Span< uint8_t >(requestData), metaInfo, std::move(cancellationHandler))};
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