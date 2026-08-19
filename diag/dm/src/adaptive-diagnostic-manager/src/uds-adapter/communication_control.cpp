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
/// @file       communication_control.cpp
/// @brief
/// @details
/// @date       2024-12-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "communication_control.h"

#include "serialization/serialization/common_data_type.h"
namespace ara {
namespace diag {
namespace dmd {
CommunicationControl::CommunicationControl(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::CommunicationControlAgent >(instanceId, serviceInstanceId)}
{
}

/// @brief Called for CommunicationControl (x028) with any subfunction as subfunction value is part of argument
///        list. Typically provider of this interface is considered as part of the state management.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] controlType All UDS request parameters packed into a structure since it holds optional elements
/// @param[in] metaInfo contains additional meta information
/// @param[in] cancellationHandler informs if the current conversation is canceled
/// @return ara::core::Future<void> error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
/// kConditionsNotCorrect, kRequestOutOfRange
/// @throws on overflow
///
/// @traceid{SWS_DM_00808}@tracestatus{draft}
isoft::uds::Result< void > CommunicationControl::CommCtrlRequest(ComCtrlRequestParamsType controlType,
                                                                 MetaInfoMap& metaInfo,
                                                                 CancellationHandler cancellationHandler) noexcept
{
    isoft::dm::ComCtrlRequestParamsType comControlType{};
    comControlType.communicationType        = controlType.communicationType;
    comControlType.controlType              = controlType.controlType;
    comControlType.nodeIdentificationNumber = controlType.nodeIdentificationNumber;
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< void > requestResult
        = agentPtr_->CommCtrlRequest(comControlType, comMetaInfo, cancellationEventPtr);
    ara::core::Result< void > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara
