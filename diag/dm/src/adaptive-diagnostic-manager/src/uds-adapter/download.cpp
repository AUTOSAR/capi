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
/// @file       download.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "download.h"

#include "serialization/serialization/common_data_type.h"
namespace ara {
namespace diag {
namespace dmd {
Download::Download(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::DownloadServiceAgent >(instanceId, serviceInstanceId)}
{
}

isoft::uds::Result< void > Download::RequestDownload(std::uint8_t dataFormatIdentifier,
                                                     std::uint8_t addressAndLengthFormatIdentifier,
                                                     std::vector< std::uint8_t > memoryAddressAndSize,
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

    if (agentPtr_.get() != nullptr) {
        ara::core::Future< void > requestResult
            = agentPtr_->RequestDownload(dataFormatIdentifier, addressAndLengthFormatIdentifier, memoryAddressAndSize,
                                         comMetaInfo, cancellationEventPtr);
        ara::core::Result< void > result{requestResult.GetResult()};
        if (result.HasValue()) {
            return {};
        }
        if (result.Error().Value() < 0) {
            return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
        }
        return isoft::uds::Result< void >::FromError(result.Error().Value());
    }
    return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
}

isoft::uds::Result< void > Download::DownloadData(std::uint8_t blockSequenceCounter,
                                                  std::vector< std::uint8_t > transferRequestParameterRecord,
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

    if (agentPtr_.get() != nullptr) {
        std::ignore = blockSequenceCounter;
        ara::core::Future< std::vector< uint8_t > > requestResult
            = agentPtr_->DownloadData(transferRequestParameterRecord, comMetaInfo, cancellationEventPtr);
        ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
        if (result.HasValue()) {
            return {};
        }
        if (result.Error().Value() < 0) {
            return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
        }
        return isoft::uds::Result< void >::FromError(result.Error().Value());
    }
    return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
}

isoft::uds::Result< std::vector< std::uint8_t > > Download::RequestDownloadExit(
    std::vector< std::uint8_t > transferRequestParameterRecord,
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

    if (agentPtr_.get() != nullptr) {
        ara::core::Future< std::vector< uint8_t > > requestResult
            = agentPtr_->RequestDownloadExit(transferRequestParameterRecord, comMetaInfo, cancellationEventPtr);
        ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
        if (result.HasValue()) {
            return isoft::uds::Result< std::vector< std::uint8_t > >::FromValue(result.Value());
        }
        if (result.Error().Value() < 0) {
            return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(isoft::dm::kGeneralReject);
        }
        return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(result.Error().Value());
    }
    return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(isoft::dm::kGeneralReject);
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
