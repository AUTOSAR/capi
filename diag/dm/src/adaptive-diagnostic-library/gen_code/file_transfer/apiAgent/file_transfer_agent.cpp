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
/// @file       file_transfer_agent.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "file_transfer_agent.h"

#include <ara/diag/diag_uds_nrc_error_domain.h>

#include <tuple>

#include "release_handler_factory.h"
namespace isoft {
namespace dm {
namespace dis {

FileTransferServiceAgent::FileTransferServiceAgent(uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< FileTransferServiceProxy >(serviceInstanceId)}
{
}

void FileTransferServiceAgent::RegisterService(ara::diag::FileTransferService* const& gdi)
{
    serviceObj_ = gdi;
    proxy_->RegisterService(this);
}

void FileTransferServiceAgent::SetReentrancyType(ara::diag::ReentrancyType const& reentrancyType) noexcept
{
    proxy_->SetReentrancyType(reentrancyType);
}

auto FileTransferServiceAgent::RequestReadFile(ara::core::String fileName,
                                               ara::core::Byte dataFormatIdentifier,
                                               ara::diag::MetaInfo const& metaInfo,
                                               ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< ara::diag::FileTransferService::FileSizes >
{
    ara::core::Future< std::tuple< ara::diag::DataTransferReadSession, ara::diag::FileTransferService::FileSizes > >
        requestFuture = serviceObj_->RequestReadFile(std::move(fileName), dataFormatIdentifier, metaInfo,
                                                     std::move(cancellationHandler));
    ara::core::Result< std::tuple< ara::diag::DataTransferReadSession, ara::diag::FileTransferService::FileSizes > >
        result{requestFuture.GetResult()};
    if (result.HasValue()) {
        ara::diag::DataTransferReadSession readSession{
            std::move(std::get< ara::diag::DataTransferReadSession >(std::move(result).Value()))};
        readSession_ = std::make_shared< ara::diag::DataTransferReadSession >(std::move(readSession));
        auto fileSizes{std::get< ara::diag::FileTransferService::FileSizes >(std::move(result).Value())};
        return ara::core::Result< ara::diag::FileTransferService::FileSizes >::FromValue(fileSizes);
    }
    return ara::core::Result< ara::diag::FileTransferService::FileSizes >::FromError(result.Error());
}

auto FileTransferServiceAgent::RequestReadDirectory(ara::core::String directoryName,
                                                    ara::diag::MetaInfo const& metaInfo,
                                                    ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< std::uint64_t >
{
    ara::core::Future< std::tuple< ara::diag::DataTransferReadSession, std::uint64_t > > requestFuture
        = serviceObj_->RequestReadDirectory(std::move(directoryName), metaInfo, std::move(cancellationHandler));
    ara::core::Result< std::tuple< ara::diag::DataTransferReadSession, std::uint64_t > > result{
        requestFuture.GetResult()};
    if (result.HasValue()) {
        ara::diag::DataTransferReadSession readSession{
            std::move(std::get< ara::diag::DataTransferReadSession >(std::move(result).Value()))};
        readSession_ = std::make_shared< ara::diag::DataTransferReadSession >(std::move(readSession));
        auto readSessionNum{std::get< std::uint64_t >(std::move(result).Value())};
        return ara::core::Result< std::uint64_t >::FromValue(readSessionNum);
    }
    return ara::core::Result< std::uint64_t >::FromError(result.Error());
}

auto FileTransferServiceAgent::RequestWriteFile(ara::core::String fileName,
                                                ara::core::Byte dataFormatIdentifier,
                                                ara::diag::FileTransferService::FileSizes fileSizes,
                                                ara::diag::FileTransferService::WriteFileMode mode,
                                                ara::diag::MetaInfo const& metaInfo,
                                                ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< void >
{
    ara::core::Future< ara::diag::DataTransferWriteSession > requestFuture = serviceObj_->RequestWriteFile(
        std::move(fileName), dataFormatIdentifier, fileSizes, mode, metaInfo, std::move(cancellationHandler));
    ara::core::Result< ara::diag::DataTransferWriteSession > result{requestFuture.GetResult()};
    if (result.HasValue()) {
        ara::diag::DataTransferWriteSession writeSession{std::move(std::move(result).Value())};
        writeSession_ = std::make_shared< ara::diag::DataTransferWriteSession >(std::move(writeSession));
        return {};
    }
    return ara::core::Result< void >::FromError(result.Error());
}

auto FileTransferServiceAgent::RequestResumeWriteFile(ara::core::String fileName,
                                                      ara::core::Byte dataFormatIdentifier,
                                                      ara::diag::FileTransferService::FileSizes fileSizes,
                                                      ara::diag::MetaInfo const& metaInfo,
                                                      ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< std::uint64_t >
{
    ara::core::Future< std::tuple< ara::diag::DataTransferWriteSession, std::uint64_t > > requestFuture
        = serviceObj_->RequestResumeWriteFile(std::move(fileName), dataFormatIdentifier, fileSizes, metaInfo,
                                              std::move(cancellationHandler));
    ara::core::Result< std::tuple< ara::diag::DataTransferWriteSession, std::uint64_t > > result{
        requestFuture.GetResult()};
    if (result.HasValue()) {
        ara::diag::DataTransferWriteSession writeSession{
            std::move(std::get< ara::diag::DataTransferWriteSession >(std::move(result).Value()))};
        writeSession_ = std::make_shared< ara::diag::DataTransferWriteSession >(std::move(writeSession));
        auto writeSessionNum{std::get< std::uint64_t >(std::move(result).Value())};
        return ara::core::Result< std::uint64_t >::FromValue(writeSessionNum);
    }
    return ara::core::Result< std::uint64_t >::FromError(result.Error());
}

auto FileTransferServiceAgent::DeleteFile(ara::core::String fileName,
                                          ara::diag::MetaInfo const& metaInfo,
                                          ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< void >
{
    ara::core::Future< void > requestFuture
        = serviceObj_->DeleteFile(std::move(fileName), metaInfo, std::move(cancellationHandler));
    return requestFuture.GetResult();
}

auto FileTransferServiceAgent::Read(std::size_t numBytesToReturn,
                                    ara::diag::MetaInfo const& metaInfo,
                                    ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< ara::core::Vector< ara::core::Byte > >
{
    if (readSession_->byPullHandler_.get() != nullptr) {
        ara::core::Vector< ara::core::Byte > readData{};
        readData.resize(numBytesToReturn);
        ara::core::Span< ara::core::Byte > responseData{readData.data(), numBytesToReturn};
        ara::core::Future< std::uint32_t > readResult{
            readSession_->byPullHandler_->Read(responseData, metaInfo, std::move(cancellationHandler))};
        ara::core::Result< std::uint32_t > result{readResult.GetResult()};
        if (result.HasValue()) {
            readData.resize(result.Value());
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(readData);
        }
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(result.Error());
    }
    if (readSession_->byPushHandler_.get() != nullptr) {
        ara::core::Future< ara::core::Vector< ara::core::Byte > > readResult{
            readSession_->byPushHandler_->Read(numBytesToReturn, metaInfo, std::move(cancellationHandler))};
        return readResult.GetResult();
    }
    if (readSession_->sharedDataHandler_.get() != nullptr) {
        if (sharedData_.empty()) {
            releaseHandlerImpl_ = std::make_shared< ReleaseHandlerImpl >(sharedData_);
            ara::diag::ReleaseHandler releaseHandler{ReleaseHandlerFactory::Create(releaseHandlerImpl_)};
            ara::core::Future< ara::core::Span< ara::core::Byte > > readResult{readSession_->sharedDataHandler_->Read(
                std::move(releaseHandler), metaInfo, std::move(cancellationHandler))};
            ara::core::Result< ara::core::Span< ara::core::Byte > > result{readResult.GetResult()};
            if (result.HasValue()) {
                sharedData_ = result.Value();
                ara::core::Vector< ara::core::Byte > data{};
                if (sharedData_.size() > numBytesToReturn) {
                    data.assign(sharedData_.begin(), sharedData_.begin() + numBytesToReturn);
                    sharedData_ = sharedData_.subspan(numBytesToReturn);
                } else {
                    data.assign(sharedData_.begin(), sharedData_.end());
                    sharedData_ = ara::core::Span< ara::core::Byte >();
                    releaseHandlerImpl_->Notifier();
                }
                return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(data);
            }
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(result.Error());
        }
        ara::core::Vector< ara::core::Byte > data{};
        if (sharedData_.size() > numBytesToReturn) {
            data.assign(sharedData_.begin(), sharedData_.begin() + numBytesToReturn);
            sharedData_ = sharedData_.subspan(numBytesToReturn);
        } else {
            data.assign(sharedData_.begin(), sharedData_.end());
            sharedData_ = ara::core::Span< ara::core::Byte >();
            releaseHandlerImpl_->Notifier();
        }
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(data);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
        ara::diag::DiagUdsNrcErrc::kGeneralReject);
}

auto FileTransferServiceAgent::ExitRead(ara::diag::DataTransferExitType exitType,
                                        ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                                        ara::diag::MetaInfo const& metaInfo,
                                        ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< ara::core::Vector< ara::core::Byte > >
{
    if (readSession_->byPullHandler_.get() != nullptr) {
        ara::core::Future< ara::core::Vector< ara::core::Byte > > readResult{readSession_->byPullHandler_->ExitRead(
            exitType, transferRequestParameterRecord, metaInfo, std::move(cancellationHandler))};
        return readResult.GetResult();
    }
    if (readSession_->byPushHandler_.get() != nullptr) {
        ara::core::Future< ara::core::Vector< ara::core::Byte > > readResult{readSession_->byPushHandler_->ExitRead(
            exitType, transferRequestParameterRecord, metaInfo, std::move(cancellationHandler))};
        return readResult.GetResult();
    }
    if (readSession_->sharedDataHandler_.get() != nullptr) {
        if (releaseHandlerImpl_.get() != nullptr) {
            releaseHandlerImpl_->Notifier();
        }
        ara::core::Future< ara::core::Vector< ara::core::Byte > > readResult{readSession_->sharedDataHandler_->ExitRead(
            exitType, transferRequestParameterRecord, metaInfo, std::move(cancellationHandler))};
        return readResult.GetResult();
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
        ara::diag::DiagUdsNrcErrc::kRequestSequenceError);
}

auto FileTransferServiceAgent::Write(ara::core::Span< ara::core::Byte > requestData,
                                     ara::diag::MetaInfo const& metaInfo,
                                     ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< void >
{
    if (writeSession_->handler_.get() != nullptr) {
        ara::core::Future< void > writeResult{
            writeSession_->handler_->Write(requestData, metaInfo, std::move(cancellationHandler))};
        return writeResult.GetResult();
    }
    return ara::core::Result< void >::FromError(ara::diag::DiagUdsNrcErrc::kGeneralReject);
}

auto FileTransferServiceAgent::ExitWrite(ara::diag::DataTransferExitType exitType,
                                         ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                                         ara::diag::MetaInfo const& metaInfo,
                                         ara::diag::CancellationHandler cancellationHandler) noexcept
    -> ara::core::Result< ara::core::Vector< ara::core::Byte > >
{
    if (writeSession_->handler_.get() != nullptr) {
        ara::core::Future< ara::core::Vector< ara::core::Byte > > writeResult{writeSession_->handler_->ExitWrite(
            exitType, transferRequestParameterRecord, metaInfo, std::move(cancellationHandler))};
        return writeResult.GetResult();
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
        ara::diag::DiagUdsNrcErrc::kRequestSequenceError);
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft