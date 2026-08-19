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
/// @file       raw_data_element_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-06
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "raw_data_element_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "cancellation/cancellation_handler_factory.h"
#include "netProxy/net_proxy.h"
#include "serialization/serialization.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDRead{0x3U};
constexpr uint8_t kFuncIDWrite{0x4U};

RawDataElementProxy::RawDataElementProxy(uint32_t const &serviceId) : BussinessServiceProxy{serviceId} {}

void RawDataElementProxy::RegisterService(ara::diag::internal::RawDataElement *const &gdi) { serviceObj_ = gdi; }

void RawDataElementProxy::CallHandle(CallRequest &&callData)
{
    switch (callData.funcData.funcId) {
        case kFuncIDRead: {
            _read(std::move(callData));
            break;
        }
        default: {
        } break;
    }
}

void RawDataElementProxy::_read(CallRequest &&callData)
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

}  // namespace dis
}  // namespace dm
}  // namespace isoft