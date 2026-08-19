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
/// @file       upload_agent.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "upload_agent.h"

namespace isoft {
namespace dm {
namespace dic {

UploadServiceAgent::UploadServiceAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< UploadServiceProxy >(instanceId, serviceInstanceId);
}

ara::core::Future< void > UploadServiceAgent::RequestUpload(
    std::uint8_t dataFormatIdentifier,
    std::uint8_t addressAndLengthFormatIdentifier,
    std::vector< std::uint8_t > memoryAddressAndSize,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->RequestUpload(dataFormatIdentifier, addressAndLengthFormatIdentifier,
                                 std::move(memoryAddressAndSize), metaInfo, cancellationEventPtr);
}

ara::core::Future< std::vector< std::uint8_t > > UploadServiceAgent::UploadData(
    std::size_t numBytesToReturn,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->UploadData(numBytesToReturn, metaInfo, cancellationEventPtr);
}

ara::core::Future< std::vector< std::uint8_t > > UploadServiceAgent::RequestUploadExit(
    std::vector< std::uint8_t > transferRequestParameterRecord,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->RequestUploadExit(std::move(transferRequestParameterRecord), metaInfo, cancellationEventPtr);
}
}  // namespace dic
}  // namespace dm
}  // namespace isoft