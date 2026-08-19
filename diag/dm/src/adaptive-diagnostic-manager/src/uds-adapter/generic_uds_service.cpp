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
/// @file       generic_uds_service.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_uds_service.h"
namespace ara {
namespace diag {
namespace dmd {

void GenericUDSService::RegisterUdsServiceInstance(std::uint8_t sid,
                                                   uint16_t const& instanceId,
                                                   uint32_t const& serviceInstanceId) noexcept
{
    ComInfo comInfo{instanceId, serviceInstanceId};
    decltype(auto) findComInstance = comInfoToInstance_.find(comInfo);
    if (findComInstance == comInfoToInstance_.end()) {
        GenericUdsServiceAgentPtr instancePtr
            = std::make_unique< isoft::dm::dic::GenericUdsServiceAgent >(instanceId, serviceInstanceId);
        comInfoToInstance_.insert({comInfo, std::move(instancePtr)});
    }
    sidToComInfo_.insert({sid, comInfo});
}

isoft::uds::Result< GenericUDSService::OperationOutput > GenericUDSService::HandleMessage(
    std::uint8_t sid,
    std::vector< std::uint8_t > requestData,
    MetaInfoMap& metaInfo,
    CancellationHandler cancellationHandler) noexcept
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    GenericUdsServiceAgentPtr instancePtr{_getInstance(sid)};
    if (instancePtr.get() != nullptr) {
        ara::core::Future< ara::core::Vector< uint8_t > > requestResult
            = instancePtr->HandleMessage(sid, requestData, comMetaInfo, cancellationEventPtr);
        ara::core::Result< ara::core::Vector< uint8_t > > result{requestResult.GetResult()};
        if (result.HasValue()) {
            GenericUDSService::OperationOutput output{};
            output.responseData.assign(result.Value().begin(), result.Value().end());
            return isoft::uds::Result< GenericUDSService::OperationOutput >::FromValue(output);
        }
        if (result.Error().Value() < 0) {
            return isoft::uds::Result< GenericUDSService::OperationOutput >::FromError(isoft::dm::kGeneralReject);
        }
        return isoft::uds::Result< GenericUDSService::OperationOutput >::FromError(result.Error().Value());
    }
    return isoft::uds::Result< GenericUDSService::OperationOutput >::FromError(isoft::dm::kGeneralReject);
}

GenericUDSService::GenericUdsServiceAgentPtr GenericUDSService::_getInstance(std::uint8_t sid) noexcept
{
    decltype(auto) findComInfo = sidToComInfo_.find(sid);
    if (findComInfo != sidToComInfo_.end()) {
        decltype(auto) findComInstance = comInfoToInstance_.find(findComInfo->second);
        if (findComInstance != comInfoToInstance_.end()) {
            return findComInstance->second;
        }
    }
    return nullptr;
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
