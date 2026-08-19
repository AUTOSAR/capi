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
/// @file       transfer_manager.cpp
/// @brief      This file implements transfer_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=transfer_managment
/// @trace_id_sr=413f148d-404c-457c-80d6-b49758149bc8
/// @unit_name=TransferManager
/// @unit_description=This file provides transfer_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "transfer_manager.h"

#include "common/serialize.h"
namespace isoft {
namespace uds {
namespace server {

namespace {
std::uint8_t const kDownload{0x34};
std::uint8_t const kUpload{0x35};
std::uint8_t const kTransfer{0x36};
std::uint8_t const kTransferExit{0x37};
std::uint8_t const kFileTransfer{0x38};
}  // namespace

namespace transfer_manager {
/// @brief Length function
///  ⎯ bit 7 - 4: Length (number of bytes) of the memorySize parameter
///  ⎯ bit 3 - 0: Length (number of bytes) of the memoryAddress parameter
/// @param formatIdentifier
/// @return std::pair<std::uint8_t,std::uint8_t> {Size,Address}

std::pair< std::uint8_t, std::uint8_t > ReadSizeAndAddressFormatIdentifier(std::uint8_t formatIdentifier) noexcept
{
    //std::uint8_t const SizeMask{0xF0};
    std::uint8_t const addressMask{0x0F};
    std::uint8_t const sizePos{4};
    return {formatIdentifier >> sizePos, formatIdentifier & addressMask};
}
std::uint8_t const kBlockSequenceCounterStartValue{1U};
}  // namespace transfer_manager



std::int32_t TransferManager::Initialize(
    TransferManagment& config,
    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
    std::shared_ptr< SessionManager >& sessionManagerPtr,
    std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
    std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr) noexcept
{
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "TransferManager::Initialize| sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogError() << "TransferManager::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }
    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSessionId, SessionId newSessionId) {
        if (oldSessionId != newSessionId) {
            _clearTransferInfo();
        }
    }};
    std::ignore = sessionManagerPtr->AddNotifyOfSessionChange(sessionChangeCallback);
    NotifyOfSecurityLevelChangeCallback securityLevelCallback{
        [this](isoft::uds::server::SecurityLevelId) { _clearTransferInfo(); }};
    std::ignore = securityLevelManagerPtr->AddNotifyOfSecurityLevelChange(securityLevelCallback);
    if (envirConditionManagerPtr.get() == nullptr) {
        LogWarn() << "TransferManager::Initialize| envirConditionManagerPtr is nullptr!";
    }
    if (config.requestDownload.get() != nullptr) {
        std::int32_t result{};
        result = _initialize(downloadInstance_, config.requestDownload->accessPermissionAuth,
                             config.requestDownload->accessPermissionSession,
                             config.requestDownload->accessPermissionSecurityLevel,
                             config.requestDownload->accessPermissionEnvCondition, authManagerPtr, sessionManagerPtr,
                             securityLevelManagerPtr, envirConditionManagerPtr);
        if (result != 0) {
            LogError() << "TransferManager::Initialize| init download service fails!";
            return -1;
        }
        downloadInstance_.maxNumberOfBlockLength = config.requestDownload->maxNumberOfBlockLength;
        downloadPtr_                             = config.requestDownload->interfacePtr;
    }
    if (config.requestUpload.get() != nullptr) {
        std::int32_t result{};
        result = _initialize(
            uploadInstance_, config.requestUpload->accessPermissionAuth, config.requestUpload->accessPermissionSession,
            config.requestUpload->accessPermissionSecurityLevel, config.requestUpload->accessPermissionEnvCondition,
            authManagerPtr, sessionManagerPtr, securityLevelManagerPtr, envirConditionManagerPtr);
        if (result != 0) {
            LogError() << "TransferManager::Initialize| init upload service fails!";
            return -1;
        }
        uploadInstance_.maxNumberOfBlockLength = config.requestUpload->maxNumberOfBlockLength;
        uploadPtr_                             = config.requestUpload->interfacePtr;
    }
    if (config.requestFileTransfer.get() != nullptr) {
        std::int32_t result{};
        result = _initialize(fileTransferInstance_, config.requestFileTransfer->accessPermissionAuth,
                             config.requestFileTransfer->accessPermissionSession,
                             config.requestFileTransfer->accessPermissionSecurityLevel,
                             config.requestFileTransfer->accessPermissionEnvCondition, authManagerPtr,
                             sessionManagerPtr, securityLevelManagerPtr, envirConditionManagerPtr);
        if (result != 0) {
            LogError() << "TransferManager::Initialize| init file transfer service fails!";
            return -1;
        }
        fileTransferInstance_.maxNumberOfBlockLength = config.requestFileTransfer->maxNumberOfBlockLength;
        fileTransferPtr_                             = config.requestFileTransfer->interfacePtr;
    }

    return 0;
}

std::int32_t TransferManager::_initialize(
    Instance& serverInstance,
    EnableAuth const& enableAuth,
    std::set< SessionId > const& accessPermissionSession,
    std::set< SecurityLevelId > const& accessPermissionSecurityLevel,
    EnvironmentalConditionId accessPermissionEnvCondition,
    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
    std::shared_ptr< SessionManager >& sessionManagerPtr,
    std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
    std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr) noexcept
{
    std::int32_t result{0};
    if (enableAuth.enable) {
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
            = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
        if (authValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(authValidator->Initialize(0, enableAuth.authRoleList, authManagerPtr));
        if (result != 0) {
            LogWarn() << "auth config initialize fails!";
            return -1;
        }
        if (serverInstance.authValidator.get() == nullptr) {
            serverInstance.authValidator = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
        }
        result = serverInstance.authValidator->RegisterSubfunctionLevel(0, authValidator);
        if (result != 0) {
            LogWarn() << "auth id repeatability! Please check Tranfer.accessPermissionAuth";
            return -1;
        }
    }
    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator
        = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
    if (sessionValidator.get() == nullptr) {
        LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
        return -1;
    }

    result = static_cast< int32_t >(sessionValidator->Initialize(0, accessPermissionSession, sessionManagerPtr));
    if (result != 0) {
        LogWarn() << "session config initialize fails!";
        return -1;
    }
    if (serverInstance.sessionValidator.get() == nullptr) {
        serverInstance.sessionValidator = std::make_shared< ServerLevelSessionValidator< std::uint8_t > >();
    }
    result = serverInstance.sessionValidator->RegisterSubfunctionLevel(0, sessionValidator);
    if (result != 0) {
        LogWarn() << "session id repeatability! Please check accessPermissionSession";
        return -1;
    }

    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator
        = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
    if (securityLevelValidator.get() == nullptr) {
        LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
        return -1;
    }
    result = static_cast< int32_t >(
        securityLevelValidator->Initialize(0, accessPermissionSecurityLevel, securityLevelManagerPtr));
    if (result != 0) {
        LogWarn() << "security level config initialize fails!";
        return -1;
    }

    if (serverInstance.securityLevelValidator.get() == nullptr) {
        serverInstance.securityLevelValidator = std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >();
    }
    result = serverInstance.securityLevelValidator->RegisterSubfunctionLevel(0, securityLevelValidator);
    if (result != 0) {
        LogWarn() << "security level id repeatability! Please check accessPermissionSecurityLevel";
        return -1;
    }

    if (accessPermissionEnvCondition >= 0) {
        serverInstance.condition = envirConditionManagerPtr->Get(accessPermissionEnvCondition);
        if (serverInstance.condition.get() == nullptr) {
            LogError() << "TransferManager::Initialize| condition id is invalid! id:" << accessPermissionEnvCondition;
            return -1;
        }
    }
    return 0;
}

void TransferManager::Process(std::shared_ptr< RequestContext > requestContext) noexcept
{
    if (requestContext.get() == nullptr) {
        LogWarn() << "TransferManager::Process| requestContext is nullptr!";
        return;
    }
    std::uint8_t const sid = requestContext->GetSid();
    switch (sid) {
        case kDownload:
            _requestDownload(requestContext);
            break;
        case kUpload:
            _requestUpload(requestContext);
            break;
        case kTransfer:
            _transfer(requestContext);
            break;
        case kTransferExit:
            _transferExit(requestContext);
            break;
        case kFileTransfer:
            _requestFileTransfer(requestContext);
            break;
        default:
            LogWarn() << "TransferManager::Process| invalid request message! sid:" << sid;
            break;
    }
}

std::vector< ValidatorPtr > TransferManager::GetSidValidator(std::uint8_t sid) noexcept
{
    std::vector< ValidatorPtr > validatorTable;
    if (sid == kDownload || sid == kUpload || sid == kTransfer || sid == kTransferExit || sid == kFileTransfer) {
        if ((sid == kDownload) || (requestServerType_ == RequestServerType::kDownload)) {
            if (downloadInstance_.authValidator.get() != nullptr) {
                validatorTable.push_back(downloadInstance_.authValidator);
            }
            if (downloadInstance_.sessionValidator.get() != nullptr) {
                validatorTable.push_back(downloadInstance_.sessionValidator);
            }
            if (downloadInstance_.securityLevelValidator.get() != nullptr) {
                validatorTable.push_back(downloadInstance_.sessionValidator);
            }
        }
        if (sid == kUpload || (requestServerType_ == RequestServerType::kUpload)) {
            if (uploadInstance_.authValidator.get() != nullptr) {
                validatorTable.push_back(uploadInstance_.authValidator);
            }
            if (uploadInstance_.sessionValidator.get() != nullptr) {
                validatorTable.push_back(uploadInstance_.sessionValidator);
            }
            if (uploadInstance_.securityLevelValidator.get() != nullptr) {
                validatorTable.push_back(uploadInstance_.sessionValidator);
            }
        }
        if (sid == kFileTransfer || (requestServerType_ == RequestServerType::kFileTransfer)) {
            if (fileTransferInstance_.authValidator.get() != nullptr) {
                validatorTable.push_back(fileTransferInstance_.authValidator);
            }
            if (fileTransferInstance_.sessionValidator.get() != nullptr) {
                validatorTable.push_back(fileTransferInstance_.sessionValidator);
            }
            if (fileTransferInstance_.securityLevelValidator.get() != nullptr) {
                validatorTable.push_back(fileTransferInstance_.sessionValidator);
            }
        }
    }
    return validatorTable;
}

void TransferManager::_requestDownload(std::shared_ptr< RequestContext >& requestContext) noexcept
{
    decltype(auto) request{requestContext->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const minSize{5U};
    ///  [SWS_DM_00098]
    if (requestBody.size() < minSize) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_requestDownload| request message size < 5!";
        return;
    }
    Serialize deserialize{requestBody};
    /// sid
    std::ignore = deserialize.ReadUint8();

    /// dataFormatIdentifier
    std::uint8_t const dataFormatIdentifier = deserialize.ReadUint8();

    /// addressAndLengthFormatIdentifier
    std::uint8_t const addressAndLengthFormatIdentifier = deserialize.ReadUint8();

    ///  [SWS_DM_00129]
    ///  [SWS_DM_00130]
    std::pair< std::uint8_t, std::uint8_t > sizeAndAddress{
        transfer_manager::ReadSizeAndAddressFormatIdentifier(addressAndLengthFormatIdentifier)};
    std::uint8_t const kMinMemorySize{1U};
    std::uint8_t const kMinMemoryAddress{1U};
    std::uint8_t const kMaxMemorySize{8U};
    std::uint8_t const kMaxMemoryAddress{8U};
    if ((sizeAndAddress.first < kMinMemorySize) || (sizeAndAddress.first > kMaxMemorySize)
        || (sizeAndAddress.second < kMinMemoryAddress) || (sizeAndAddress.second > kMaxMemoryAddress)) {
        requestContext->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_requestDownload| addressAndLengthFormatIdentifier is invalid!";
        return;
    }

    /// Full length check
    std::uint8_t const kMemoryAddressParameterPos{3U};
    if (static_cast< std::uint32_t >(kMemoryAddressParameterPos + sizeAndAddress.second + sizeAndAddress.first)
        != (requestBody.size())) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_requestDownload|  request message size is invalid! please check "
                     "addressAndLengthFormatIdentifier parameter!";
        return;
    }

    std::uint64_t memoryAddress = deserialize.ReadUint(sizeAndAddress.second);
    std::uint64_t memorySize{0U};
    memorySize = deserialize.ReadUint(sizeAndAddress.first);
    LogVerbose() << "TransferManager::_requestDownload| memoryAddress:" << memoryAddress
                 << " memorySize:" << memorySize;
    decltype(auto) metaInfo{requestContext->GetMetaInfoMap()};
    if (downloadInstance_.condition.get() != nullptr) {
        std::uint8_t const nrcValue{
            downloadInstance_.condition->Check(metaInfo, requestContext->GetCancellationState())};
        if (nrcValue != 0U) {
            requestContext->Respond(nrcValue);
            LogWarn() << "TransferManager::_requestDownload| condition check fails! error:" << nrcValue;
            return;
        }
    }
    deserialize.Seek(kMemoryAddressParameterPos);
    std::vector< std::uint8_t > data{};
    data = deserialize.ReadAll();
    Result< void > result{downloadPtr_->RequestDownload(dataFormatIdentifier, addressAndLengthFormatIdentifier, data,
                                                        metaInfo,
                                                        CancellationHandler(requestContext->GetCancellationState()))};
    if (result.HasValue()) {
        std::shared_ptr< Message > response{requestContext->CreateResponse()};
        std::uint8_t const lengthFormatIdentifier{0x40U};
        Serialize serialize{response->GetBody()};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(downloadInstance_.maxNumberOfBlockLength);
        requestContext->Respond(std::move(response));
        transferInfo_.memorySize           = memorySize;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kDownload;
        state_                             = State::kRequest;
    } else {
        std::int32_t nrc = result.Error();
        requestContext->Respond(nrc);
    }
}

void TransferManager::_requestUpload(std::shared_ptr< RequestContext >& requestContext) noexcept
{
    decltype(auto) request{requestContext->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const minSize{5U};
    ///  [SWS_DM_00098]
    if (requestBody.size() < minSize) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_requestUpload| request message size < 5!";
        return;
    }
    Serialize deserialize{requestBody};
    /// sid
    std::ignore = deserialize.ReadUint8();

    /// dataFormatIdentifier
    std::uint8_t const dataFormatIdentifier = deserialize.ReadUint8();

    /// addressAndLengthFormatIdentifier
    std::uint8_t const addressAndLengthFormatIdentifier = deserialize.ReadUint8();

    ///  [SWS_DM_00129]
    ///  [SWS_DM_00130]
    std::pair< std::uint8_t, std::uint8_t > sizeAndAddress{
        transfer_manager::ReadSizeAndAddressFormatIdentifier(addressAndLengthFormatIdentifier)};
    std::uint8_t const kMinMemorySize{1U};
    std::uint8_t const kMinMemoryAddress{1U};
    std::uint8_t const kMaxMemorySize{8U};
    std::uint8_t const kMaxMemoryAddress{8U};
    if ((sizeAndAddress.first < kMinMemorySize) || (sizeAndAddress.first > kMaxMemorySize)
        || (sizeAndAddress.second < kMinMemoryAddress) || (sizeAndAddress.second > kMaxMemoryAddress)) {
        requestContext->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_requestUpload| addressAndLengthFormatIdentifier is invalid!";
        return;
    }

    /// Full length check
    std::uint8_t const kMemoryAddressParameterPos{3U};
    if (static_cast< std::uint32_t >(kMemoryAddressParameterPos + sizeAndAddress.second + sizeAndAddress.first)
        != requestBody.size()) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_requestUpload|  request message size is invalid! please check "
                     "addressAndLengthFormatIdentifier parameter!";
        return;
    }

    std::uint64_t memoryAddress = deserialize.ReadUint(sizeAndAddress.second);

    std::uint64_t memorySize{0U};
    memorySize = deserialize.ReadUint(sizeAndAddress.first);
    LogVerbose() << "TransferManager::_requestDownload| memoryAddress:" << memoryAddress
                 << " memorySize:" << memorySize;
    decltype(auto) metaInfo{requestContext->GetMetaInfoMap()};
    if (uploadInstance_.condition.get() != nullptr) {
        std::uint8_t const nrcValue{uploadInstance_.condition->Check(metaInfo, requestContext->GetCancellationState())};
        if (nrcValue != 0U) {
            requestContext->Respond(nrcValue);
            LogWarn() << "TransferManager::_requestUpload| condition check fails! error:" << nrcValue;
            return;
        }
    }
    if (uploadPtr_.get() == nullptr) {
        requestContext->Respond(NrcErrc::kServiceNotSupported);
        LogWarn() << "TransferManager::_requestUpload| upload service not start";
        return;
    }
    deserialize.Seek(kMemoryAddressParameterPos);
    std::vector< std::uint8_t > data{};
    data = deserialize.ReadAll();
    Result< void > result{uploadPtr_->RequestUpload(dataFormatIdentifier, addressAndLengthFormatIdentifier, data,
                                                    metaInfo,
                                                    CancellationHandler(requestContext->GetCancellationState()))};
    if (result.HasValue()) {
        std::shared_ptr< Message > response{requestContext->CreateResponse()};
        std::uint8_t const lengthFormatIdentifier{0x40U};
        Serialize serialize{response->GetBody()};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(uploadInstance_.maxNumberOfBlockLength);
        requestContext->Respond(std::move(response));
        transferInfo_.memorySize           = memorySize;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kUpload;
        state_                             = State::kRequest;
    } else {
        std::int32_t nrc = result.Error();
        requestContext->Respond(nrc);
    }
}

void TransferManager::_transfer(std::shared_ptr< RequestContext >& requestContext) noexcept
{
    decltype(auto) request{requestContext->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const minSize{2U};
    ///  [SWS_DM_00098]
    if (requestBody.size() < minSize) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_transfer| request message size < 5!";
        return;
    }

    /// sequence is respected for SID
    if ((requestServerType_ == RequestServerType::kUnknow)
        || ((state_ != State::kRequest) && (state_ != State::kTransfer))) {
        requestContext->Respond(NrcErrc::kRequestSequenceError);
        LogWarn() << "TransferManager::_transfer| sequence is respected for SID";
        return;
    }

    Serialize deserialize{requestBody};
    /// sid
    std::ignore = deserialize.ReadUint8();

    /// blockSequenceCounter
    std::uint8_t const blockSequenceCounter = deserialize.ReadUint8();
    if ((transferInfo_.blockSequenceCounter != blockSequenceCounter)
        && (transferInfo_.previousblockSequenceCounter != blockSequenceCounter)) {
        requestContext->Respond(NrcErrc::kWrongBlockSequenceCounter);
        LogWarn() << "TransferManager::_transfer| sequence is respected for SID";
        return;
    }
    decltype(auto) metaInfo{requestContext->GetMetaInfoMap()};
    if (requestServerType_ == RequestServerType::kDownload) {
        if (transferInfo_.previousblockSequenceCounter == blockSequenceCounter) {
            std::shared_ptr< Message > response{requestContext->CreateResponse()};
            Serialize serialize{response->GetBody()};
            serialize.Write(blockSequenceCounter);
            requestContext->Respond(std::move(response));
            LogInfo() << "TransferManager::_transfer| request message replace!";
            return;
        }
        std::vector< std::uint8_t > transferRequestParameterRecord{};
        transferRequestParameterRecord = deserialize.ReadAll();
        Result< void > downloadResult
            = downloadPtr_->DownloadData(blockSequenceCounter, transferRequestParameterRecord, metaInfo,
                                         CancellationHandler(requestContext->GetCancellationState()));
        if (downloadResult.HasValue()) {
            transferInfo_.previousblockSequenceCounter = transferInfo_.blockSequenceCounter;
            transferInfo_.blockSequenceCounter += 1U;
            transferInfo_.transferSize += transferRequestParameterRecord.size();
            std::shared_ptr< Message > response{requestContext->CreateResponse()};
            Serialize serialize{response->GetBody()};
            serialize.Write(blockSequenceCounter);
            requestContext->Respond(std::move(response));
        } else {
            std::uint8_t const nrc{static_cast< uint8_t >(downloadResult.Error())};
            requestContext->Respond(nrc);
            LogWarn() << "TransferManager::_transfer| DownloadData fails! error:" << nrc;
            return;
        }
    } else if (requestServerType_ == RequestServerType::kUpload) {
        if (transferInfo_.previousblockSequenceCounter == blockSequenceCounter) {
            std::shared_ptr< Message > response{requestContext->CreateResponse()};
            Serialize serialize{response->GetBody()};
            serialize.Write(blockSequenceCounter);
            serialize.Write(transferInfo_.transferRequestParameterRecord);
            requestContext->Respond(std::move(response));
            LogInfo() << "TransferManager::_transfer| request message replace!";
            return;
        }
        std::vector< std::uint8_t > transferRequestParameterRecord{};
        transferRequestParameterRecord = deserialize.ReadAll();
        Result< std::vector< std::uint8_t > > uploadResult
            = uploadPtr_->UploadData(blockSequenceCounter, uploadInstance_.maxNumberOfBlockLength, metaInfo,
                                     CancellationHandler(requestContext->GetCancellationState()));
        if (uploadResult.HasValue()) {
            transferInfo_.previousblockSequenceCounter = transferInfo_.blockSequenceCounter;
            transferInfo_.blockSequenceCounter += 1U;
            transferInfo_.transferRequestParameterRecord = uploadResult.Value();
            std::shared_ptr< Message > response{requestContext->CreateResponse()};
            Serialize serialize{response->GetBody()};
            serialize.Write(blockSequenceCounter);
            serialize.Write(uploadResult.Value());
            requestContext->Respond(std::move(response));
        } else {
            std::uint8_t const nrc{static_cast< uint8_t >(uploadResult.Error())};
            requestContext->Respond(nrc);
            LogWarn() << "TransferManager::_transfer| Upload data fails! error:" << nrc;
            return;
        }
    } else if (requestServerType_ == RequestServerType::kFileTransfer) {
        if (modeOfOperation_ == ModeOfOperation::kReadFile || modeOfOperation_ == ModeOfOperation::kReadDir) {
            if (transferInfo_.previousblockSequenceCounter == blockSequenceCounter) {
                std::shared_ptr< Message > response{requestContext->CreateResponse()};
                Serialize serialize{response->GetBody()};
                serialize.Write(blockSequenceCounter);
                serialize.Write(transferInfo_.transferRequestParameterRecord);
                requestContext->Respond(std::move(response));
                LogInfo() << "TransferManager::_transfer| request message replace!";
                return;
            }
            Result< std::vector< std::uint8_t > > fileResult
                = fileTransferPtr_->Read(blockSequenceCounter, fileTransferInstance_.maxNumberOfBlockLength, metaInfo,
                                         CancellationHandler(requestContext->GetCancellationState()));
            if (fileResult.HasValue()) {
                transferInfo_.previousblockSequenceCounter = transferInfo_.blockSequenceCounter;
                transferInfo_.blockSequenceCounter += 1U;
                for (auto& element : fileResult.Value()) {
                    transferInfo_.transferRequestParameterRecord.emplace_back(element);
                }
                transferInfo_.transferSize += fileResult.Value().size();
                std::shared_ptr< Message > response{requestContext->CreateResponse()};
                Serialize serialize{response->GetBody()};
                serialize.Write(blockSequenceCounter);
                serialize.Write(fileResult.Value());
                requestContext->Respond(std::move(response));
            } else {
                std::uint8_t const nrc{static_cast< uint8_t >(fileResult.Error())};
                requestContext->Respond(nrc);
                LogWarn() << "TransferManager::_transfer| file read data fails! error:" << nrc;
                return;
            }
        } else if (modeOfOperation_ == ModeOfOperation::kAddFile || modeOfOperation_ == ModeOfOperation::kReplaceFile
                   || modeOfOperation_ == ModeOfOperation::kResumeFile) {
            if (transferInfo_.previousblockSequenceCounter == blockSequenceCounter) {
                std::shared_ptr< Message > response{requestContext->CreateResponse()};
                Serialize serialize{response->GetBody()};
                serialize.Write(blockSequenceCounter);
                requestContext->Respond(std::move(response));
                LogInfo() << "TransferManager::_transfer| request message replace!";
                return;
            }
            std::vector< std::uint8_t > transferRequestParameterRecord{};
            transferRequestParameterRecord = deserialize.ReadAll();
            Result< void > fileResult
                = fileTransferPtr_->Write(blockSequenceCounter, transferRequestParameterRecord, metaInfo,
                                          CancellationHandler(requestContext->GetCancellationState()));
            if (fileResult.HasValue()) {
                transferInfo_.previousblockSequenceCounter = transferInfo_.blockSequenceCounter;
                transferInfo_.blockSequenceCounter += 1U;
                transferInfo_.transferSize += transferRequestParameterRecord.size();
                std::shared_ptr< Message > response{requestContext->CreateResponse()};
                Serialize serialize{response->GetBody()};
                serialize.Write(blockSequenceCounter);
                requestContext->Respond(std::move(response));
            } else {
                std::uint8_t const nrc{static_cast< uint8_t >(fileResult.Error())};
                requestContext->Respond(nrc);
                LogWarn() << "TransferManager::_transfer| file write data fails! error:" << nrc;
                return;
            }
        }
    }
}

void TransferManager::_transferExit(std::shared_ptr< RequestContext >& requestContext) noexcept
{
    decltype(auto) request{requestContext->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const minSize{1U};
    ///  [SWS_DM_00098]
    if (requestBody.size() < minSize) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_transferExit| request message size < 5!";
        return;
    }

    /// sequence is respected for SID
    if ((requestServerType_ == RequestServerType::kUnknow)
        || ((state_ != State::kRequest) && (state_ != State::kTransfer))) {
        requestContext->Respond(NrcErrc::kRequestSequenceError);
        LogWarn() << "TransferManager::_transferExit| sequence is respected for SID";
        return;
    }

    Serialize deserialize{requestBody};
    /// sid
    std::ignore = deserialize.ReadUint8();
    std::vector< std::uint8_t > transferRequestParameterRecord{};
    transferRequestParameterRecord = deserialize.ReadAll();
    decltype(auto) metaInfo{requestContext->GetMetaInfoMap()};
    Result< std::vector< std::uint8_t > > exitResult;
    if (requestServerType_ == RequestServerType::kDownload) {
        exitResult = downloadPtr_->RequestDownloadExit(transferRequestParameterRecord, metaInfo,
                                                       CancellationHandler(requestContext->GetCancellationState()));
    } else if (requestServerType_ == RequestServerType::kUpload) {
        exitResult = uploadPtr_->RequestUploadExit(transferRequestParameterRecord, metaInfo,
                                                   CancellationHandler(requestContext->GetCancellationState()));
    } else if (requestServerType_ == RequestServerType::kFileTransfer) {
        DataTransferExitType exitType{DataTransferExitType::kAcknowledge};
        if (transferInfo_.fileSize == transferInfo_.transferSize) {
            exitType = DataTransferExitType::kAbort;
        }

        if (modeOfOperation_ == ModeOfOperation::kReadFile || modeOfOperation_ == ModeOfOperation::kReadDir) {
            exitResult = fileTransferPtr_->ExitRead(exitType, transferRequestParameterRecord, metaInfo,
                                                    CancellationHandler(requestContext->GetCancellationState()));
        } else if (modeOfOperation_ == ModeOfOperation::kAddFile || modeOfOperation_ == ModeOfOperation::kReplaceFile
                   || modeOfOperation_ == ModeOfOperation::kResumeFile) {
            exitResult = fileTransferPtr_->ExitWrite(exitType, transferRequestParameterRecord, metaInfo,
                                                     CancellationHandler(requestContext->GetCancellationState()));
        }
    }
    if (exitResult.HasValue()) {
        _clearTransferInfo();
        state_ = State::kExit;
        std::shared_ptr< Message > response{requestContext->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(exitResult.Value());
        requestContext->Respond(std::move(response));
    } else {
        std::uint8_t const nrc{static_cast< uint8_t >(exitResult.Error())};
        requestContext->Respond(nrc);
        LogWarn() << "TransferManager::_transferExit| exit fails! error:" << nrc;
        return;
    }
}

void TransferManager::_requestFileTransfer(std::shared_ptr< RequestContext >& requestContext) noexcept
{
    decltype(auto) request{requestContext->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const kMinRequestSize{5U};
    ///  [SWS_DM_00098]
    if (requestBody.size() < kMinRequestSize) {
        requestContext->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_requestFileTransfer|minimum length: 5 byte.request message size too short!";
        return;
    }
    Serialize deserialize{requestBody};
    std::ignore = deserialize.ReadUint8();
    std::uint8_t const modeOfOperation{deserialize.ReadUint8()};
    if (modeOfOperation < static_cast< std::uint8_t >(ModeOfOperation::kAddFile)
        || modeOfOperation > static_cast< std::uint8_t >(ModeOfOperation::kResumeFile)) {
        requestContext->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_requestFileTransfer|modeOfOperation valid range [1,5],current modeOfOperation "
                     "invalid:"
                  << modeOfOperation;
        return;
    }
    ModeOfOperation const requestType{static_cast< ModeOfOperation >(modeOfOperation)};
    switch (requestType) {
        case ModeOfOperation::kAddFile:
            _addFile(requestContext, deserialize);
            break;
        case ModeOfOperation::kDeleteFile:
            _deleteFile(requestContext, deserialize);
            break;
        case ModeOfOperation::kReplaceFile:
            _replaceFile(requestContext, deserialize);
            break;
        case ModeOfOperation::kReadFile:
            _readFile(requestContext, deserialize);
            break;
        case ModeOfOperation::kReadDir:
            _readDir(requestContext, deserialize);
            break;
        case ModeOfOperation::kResumeFile:
            _resumeFile(requestContext, deserialize);
            break;
        default:
            break;
    }
}

void TransferManager::_clearTransferInfo() noexcept
{
    transferInfo_.blockSequenceCounter         = transfer_manager::kBlockSequenceCounterStartValue;
    transferInfo_.fileSize                     = 0U;
    transferInfo_.memorySize                   = 0U;
    transferInfo_.previousblockSequenceCounter = 0U;
    transferInfo_.transferRequestParameterRecord.clear();
    transferInfo_.transferSize = 0U;
    state_                     = State::kIdle;
    modeOfOperation_           = ModeOfOperation::kReserved;
    requestServerType_         = RequestServerType::kUnknow;
}




void TransferManager::_addFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    std::size_t const minLength{6};
    if ((minLength + filePathAndNameLength) >= requestDeserialize.Size()) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_addFile| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    std::uint8_t dataFormatIdentifier{0x00};
    dataFormatIdentifier = requestDeserialize.ReadUint8();
    std::uint8_t const fileSizeParameterLength{requestDeserialize.ReadUint8()};
    if (filePathAndNameLength == 0U) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_addFile| fileSizeParameterLength is invalid,fileSizeParameterLength:"
                  << filePathAndNameLength;
        return;
    }
    if (((requestDeserialize.Tell() + fileSizeParameterLength * 2) != requestDeserialize.Size())) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_addFile|request message length too short,not deserialize fileSizeUnCompressed";
        return;
    }
    std::uint64_t const fileSizeUnCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    std::uint64_t const fileSizeCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_addFile|condition check fails! error:" << nrc;
            return;
        }
    }
    FileTransferInterface::FileSizes fileSizes{fileSizeUnCompressed, fileSizeCompressed, {}};
    FileTransferInterface::WriteFileMode mode{FileTransferInterface::WriteFileMode::kAdd};
    Result< std::vector< uint8_t > > requestResult
        = fileTransferPtr_->RequestWriteFile(filePathAndName, dataFormatIdentifier, fileSizes, mode, metaInfo,
                                             CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kAddFile));
        std::uint8_t const lengthFormatIdentifier{4U};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(fileTransferInstance_.maxNumberOfBlockLength);
        serialize.Write(dataFormatIdentifier);
        /// If there is MD5
        _fillMD5(requestResult.Value(), serialize);
        context->Respond(std::move(response));
        transferInfo_.fileSize             = fileSizeUnCompressed;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kFileTransfer;
        state_                             = State::kRequest;
        modeOfOperation_                   = ModeOfOperation::kAddFile;
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_addFile| RequestWriteFile fails! error:" << nrc;
        return;
    }
}




void TransferManager::_deleteFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    if ((requestDeserialize.Tell() + filePathAndNameLength) != requestDeserialize.Size()) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_deleteFile| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    if (filePathAndNameLength == 0U) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_deleteFile| fileSizeParameterLength is invalid,fileSizeParameterLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_deleteFile|condition check fails! error:" << nrc;
            return;
        }
    }
    Result< std::vector< uint8_t > > requestResult
        = fileTransferPtr_->DeleteFile(filePathAndName, metaInfo, CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kDeleteFile));
        _fillMD5(requestResult.Value(), serialize);
        context->Respond(std::move(response));
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_deleteFile| RequestWriteFile fails! error:" << nrc;
        return;
    }
}




void TransferManager::_replaceFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    std::size_t const minLength{6};
    if ((minLength + filePathAndNameLength) >= requestDeserialize.Size()) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_replaceFile| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    std::uint8_t dataFormatIdentifier{0x00};
    dataFormatIdentifier = requestDeserialize.ReadUint8();
    std::uint8_t const fileSizeParameterLength{requestDeserialize.ReadUint8()};
    if (filePathAndNameLength == 0U) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_replaceFile| fileSizeParameterLength is invalid,fileSizeParameterLength:"
                  << filePathAndNameLength;
        return;
    }
    if (((requestDeserialize.Tell() + fileSizeParameterLength * 2) != requestDeserialize.Size())) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn()
            << "TransferManager::_replaceFile|request message length too short,not deserialize fileSizeUnCompressed";
        return;
    }
    std::uint64_t const fileSizeUnCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    std::uint64_t const fileSizeCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_replaceFile|condition check fails! error:" << nrc;
            return;
        }
    }
    FileTransferInterface::FileSizes fileSizes{fileSizeUnCompressed, fileSizeCompressed, {}};
    FileTransferInterface::WriteFileMode mode{FileTransferInterface::WriteFileMode::kReplace};
    Result< std::vector< uint8_t > > requestResult
        = fileTransferPtr_->RequestWriteFile(filePathAndName, dataFormatIdentifier, fileSizes, mode, metaInfo,
                                             CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kReplaceFile));
        std::uint8_t const lengthFormatIdentifier{4U};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(fileTransferInstance_.maxNumberOfBlockLength);
        serialize.Write(dataFormatIdentifier);
        _fillMD5(requestResult.Value(), serialize);
        context->Respond(std::move(response));
        transferInfo_.fileSize             = fileSizeUnCompressed;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kFileTransfer;
        state_                             = State::kRequest;
        modeOfOperation_                   = ModeOfOperation::kReplaceFile;
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_replaceFile| RequestWriteFile fails! error:" << nrc;
        return;
    }
}




void TransferManager::_readFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    std::size_t const minLength{5};
    if ((filePathAndNameLength == 0U) || ((minLength + filePathAndNameLength) != requestDeserialize.Size())) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_readFile| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    std::uint8_t dataFormatIdentifier{0x00};
    dataFormatIdentifier = requestDeserialize.ReadUint8();
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_readFile|condition check fails! error:" << nrc;
            return;
        }
    }
    Result< FileTransferInterface::FileSizes > requestResult = fileTransferPtr_->RequestReadFile(
        filePathAndName, dataFormatIdentifier, metaInfo, CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kReadFile));
        std::uint8_t const lengthFormatIdentifier{4U};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(fileTransferInstance_.maxNumberOfBlockLength);
        serialize.Write(dataFormatIdentifier);
        std::uint16_t const fileSizeOrDirInfoParameterLength{8};
        serialize.Write(fileSizeOrDirInfoParameterLength);
        serialize.Write(requestResult.Value().uncompressedSize);
        serialize.Write(requestResult.Value().compressedSize);
        _fillMD5(requestResult.Value().vecMD5, serialize);
        context->Respond(std::move(response));
        transferInfo_.fileSize             = requestResult.Value().compressedSize;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kFileTransfer;
        state_                             = State::kRequest;
        modeOfOperation_                   = ModeOfOperation::kReadFile;
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_readFile| RequestReadFile fails! error:" << nrc;
        return;
    }
}




void TransferManager::_readDir(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    std::size_t const minLength{4};
    if ((filePathAndNameLength == 0U) || ((minLength + filePathAndNameLength) != requestDeserialize.Size())) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_readDir| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_readDir|condition check fails! error:" << nrc;
            return;
        }
    }
    Result< FileTransferInterface::RequestFileResponse > requestResult = fileTransferPtr_->RequestReadDirectory(
        filePathAndName, metaInfo, CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kReadDir));
        std::uint8_t const lengthFormatIdentifier{4U};
        std::uint8_t const dataFormatIdentifier{0U};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(fileTransferInstance_.maxNumberOfBlockLength);
        serialize.Write(dataFormatIdentifier);
        std::uint16_t const fileSizeOrDirInfoParameterLength{8};
        serialize.Write(fileSizeOrDirInfoParameterLength);
        serialize.Write(requestResult.Value().fileSize);
        _fillMD5(requestResult.Value().vecMD5, serialize);
        context->Respond(std::move(response));
        transferInfo_.fileSize             = requestResult.Value().fileSize;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        requestServerType_                 = RequestServerType::kFileTransfer;
        state_                             = State::kRequest;
        modeOfOperation_                   = ModeOfOperation::kReadDir;
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_readDir| RequestReadDirectory fails! error:" << nrc;
        return;
    }
}




void TransferManager::_resumeFile(std::shared_ptr< RequestContext >& context, Serialize& requestDeserialize) noexcept
{
    /// full length check
    std::uint16_t const filePathAndNameLength{requestDeserialize.ReadUint16()};
    std::size_t const minLength{6};
    if ((minLength + filePathAndNameLength) >= requestDeserialize.Size()) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "TransferManager::_resumeFile| filePathAndNameLength is invalid,filePathAndNameLength:"
                  << filePathAndNameLength;
        return;
    }
    std::string const filePathAndName{requestDeserialize.ReadString(filePathAndNameLength)};
    std::uint8_t dataFormatIdentifier{0x00};
    dataFormatIdentifier = requestDeserialize.ReadUint8();
    std::uint8_t const fileSizeParameterLength{requestDeserialize.ReadUint8()};
    if (filePathAndNameLength == 0U) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "TransferManager::_resumeFile| fileSizeParameterLength is invalid,fileSizeParameterLength:"
                  << filePathAndNameLength;
        return;
    }
    if (((requestDeserialize.Tell() + fileSizeParameterLength * 2) != requestDeserialize.Size())) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn()
            << "TransferManager::_resumeFile|request message length too short,not deserialize fileSizeUnCompressed";
        return;
    }
    std::uint64_t const fileSizeUnCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    std::uint64_t const fileSizeCompressed{requestDeserialize.ReadUint(fileSizeParameterLength)};
    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    if (fileTransferInstance_.condition.get() != nullptr) {
        std::uint8_t nrc = fileTransferInstance_.condition->Check(metaInfo, context->GetCancellationState());
        if (nrc != 0) {
            context->Respond(nrc);
            LogWarn() << "TransferManager::_resumeFile|condition check fails! error:" << nrc;
            return;
        }
    }
    FileTransferInterface::FileSizes fileSizes{fileSizeUnCompressed, fileSizeCompressed, {}};
    Result< FileTransferInterface::RequestFileResponse > requestResult
        = fileTransferPtr_->RequestResumeWriteFile(filePathAndName, dataFormatIdentifier, fileSizes, metaInfo,
                                                   CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        Serialize serialize{response->GetBody()};
        serialize.Write(static_cast< std::uint8_t >(ModeOfOperation::kResumeFile));
        std::uint8_t const lengthFormatIdentifier{4U};
        serialize.Write(lengthFormatIdentifier);
        serialize.Write(fileTransferInstance_.maxNumberOfBlockLength);
        serialize.Write(dataFormatIdentifier);
        serialize.Write(requestResult.Value().fileSize);
        _fillMD5(requestResult.Value().vecMD5, serialize);
        context->Respond(std::move(response));
        transferInfo_.fileSize             = fileSizeCompressed;
        transferInfo_.blockSequenceCounter = transfer_manager::kBlockSequenceCounterStartValue;
        transferInfo_.transferSize         = requestResult.Value().fileSize;
        requestServerType_                 = RequestServerType::kFileTransfer;
        state_                             = State::kRequest;
        modeOfOperation_                   = ModeOfOperation::kResumeFile;
    } else {
        std::uint8_t nrc = static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(nrc);
        LogWarn() << "TransferManager::_resumeFile| RequestResumeWriteFile fails! error:" << nrc;
        return;
    }
}

void TransferManager::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    std::ignore = request;
    if (status == ResponseStatusType::kCanceled || status == ResponseStatusType::kResNegOk
        || status == ResponseStatusType::kResNegNotOk || status == ResponseStatusType::kResPosNotOk) {
        _clearTransferInfo();
    }
}

void TransferManager::_fillMD5(std::vector< uint8_t > const& vecMD5, Serialize& serialize)
{
    if (vecMD5.empty()) {
        return;
    }

    serialize.Write(vecMD5);
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
