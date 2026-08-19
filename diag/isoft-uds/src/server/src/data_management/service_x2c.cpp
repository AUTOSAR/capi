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
/// @file       service_x2c.cpp
/// @brief      This file implements service_x2c functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=80a7b0a0-b003-481c-8ae4-a5fc20c0af92
/// @unit_name=service_x2c.h
/// @unit_description=This file provides service_x2c functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x2c.h"

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x2c {

}  // namespace service_x2c
ServiceX2C::ServiceX2C() noexcept
    : sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint16_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint16_t > >()}
{
}

std::int32_t ServiceX2C::Initialize(ServiceX2CModel& model,
                                    std::shared_ptr< ServiceX22 >& serviceX22Ptr,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager >& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                                    std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    if (model.table.empty()) {
        LogWarn() << "ServiceX2CModel.table is empty!";
        return -1;
    }
    if (serviceX22Ptr.get() == nullptr) {
        LogWarn() << "serviceX22Ptr is nullptr";
        return -1;
    }
    serviceX22Ptr_ = serviceX22Ptr;
    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "sessionManagerPtr is nullptr";
        return -1;
    }
    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "securityLevelManagerPtr is nullptr";
        return -1;
    }

    if (diagnosticDataManagerPtr.get() == nullptr) {
        LogWarn() << "diagnosticDataManager is nullptr";
        return -1;
    }
    checkPerSourceId_ = model.checkPerSourceId;
    stored_           = model.stored;

    for (auto&& instanceConfig : model.table) {
        bool hasDataIdentifier{true};
        hasDataIdentifier = diagnosticDataManagerPtr->HasDataIdentifier(instanceConfig.id);
        if (!hasDataIdentifier) {
            LogWarn() << "diagnostic data manager has not data identifier:" << instanceConfig.id;
            return -1;
        }
        std::shared_ptr< Instance > instancePtr{nullptr};
        instancePtr = std::make_shared< Instance >();
        if (instancePtr.get() == nullptr) {
            LogWarn() << "allow memory fails in new struct Instance";
            return -1;
        }
        instancePtr->id = instanceConfig.id;
        if (instanceConfig.maxSourceElement == 0U) {
            LogWarn() << "DynamicDefinedDiagnosticData.maxSourceElement must not equal 0!";
            return -1;
        }
        instancePtr->maxSourceElement = instanceConfig.maxSourceElement;
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint16_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            std::int32_t result{};
            result = static_cast< int32_t >(authValidator->Initialize(
                instanceConfig.id, instanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint16_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(instanceConfig.id, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check ReadDiagnosticDataByIdentifier.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint16_t > >();
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }
        std::int32_t result{0};
        result = static_cast< int32_t >(instancePtr->sessionValidator->Initialize(
            instanceConfig.id, instanceConfig.accessPermissionSession, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        result = sessionValidator_->RegisterSubfunctionLevel(instanceConfig.id, instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn()
                << "session id repeatability! Please check ReadDiagnosticDataByIdentifier.accessPermissionSession";
            return -1;
        }
        if (model.checkPerSourceId) {
            NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSessionId, SessionId newSessionId) {
                _clearWhenSessionChange(oldSessionId, newSessionId);
            }};
            std::ignore = sessionManagerPtr->AddNotifyOfSessionChange(sessionChangeCallback);
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint16_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            instanceConfig.id, instanceConfig.accessPermissionSecurityLevel, securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result
            = securityLevelValidator_->RegisterSubfunctionLevel(instanceConfig.id, instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(instanceConfig.id);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ReadDiagnosticDataByIdentifier.id:" << instanceConfig.id << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({instanceConfig.id, instancePtr});
    }
    subfunctionTable_      = model.subfunction;
    dataIdentifierManager_ = diagnosticDataManagerPtr;

    p4ServerMax_ = model.p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX2C::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    if (authValidator_.get() != nullptr) {
        validatorTable.push_back(authValidator_);
    }
    if (sessionValidator_.get() != nullptr) {
        validatorTable.push_back(sessionValidator_);
    }
    if (securityLevelValidator_.get() != nullptr) {
        validatorTable.push_back(securityLevelValidator_);
    }
    return validatorTable;
}


void ServiceX2C::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::Process| context is nullptr";
    }
    isoft::uds::server::MessagePtr request{context->GetRequest()};
    if (request.get() == nullptr) {
        LogWarn() << "ServiceX2C::Process| context->Request is nullptr";
    }
    LogDebug() << "ServiceX2C::Process| request:" << request->GetBody();

    ///  [SWS_DM_00098]
    bool checkMinimumSizeResult = _checkMinimumSize(request);
    if (!checkMinimumSizeResult) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }

    bool checkSubfunctionSupportedResult = _checkSubfunctionSupported(request);
    if (!checkSubfunctionSupportedResult) {
        LogWarn() << "ServiceX2C::Process| subfunction not support in current seesion";
        context->Respond(NrcErrc::kSubfunctionNotSupported);
        return;
    }

    NrcErrc checkSubfunctionSessionResult = _checkSubfunctionSession(context);
    if (checkSubfunctionSessionResult != NrcErrc::kSuccess) {
        LogWarn() << "ServiceX2C::Process| subfunction not support in current seesion";
        context->Respond(checkSubfunctionSessionResult);
        return;
    }
    bool supportSecurityLevel{false};
    supportSecurityLevel = _supportSecurityLevel();
    if (supportSecurityLevel) {
        bool checkSubfunctionSecurityLevelResult = _checkSubfunctionSecurityLevel(context);
        if (!checkSubfunctionSecurityLevelResult) {
            LogWarn() << "ServiceX2C::Process| subfunction not support in current security level";
            context->Respond(NrcErrc::kSecurityAccessDenied);
            return;
        }
    }


    return _parseAndResponse(context);
}

void ServiceX2C::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    LogVerbose() << "ServiceX2C::Confirm| RequestHandle:" << request.GetMetaInfoMap()["kRequestHandle"]
                 << "response status:" << EnumToString(status);
}

bool ServiceX2C::_checkMinimumSize(MessagePtr const& reuqestPtr) noexcept
{
    bool result{true};
    Serialize serialize{reuqestPtr->GetBody()};
    serialize.Seek(1);
    std::uint8_t subfunction = serialize.ReadUint8();
    LogInfo() << "ServiceX2C::_checkMinimumSize| subfunction:" << subfunction;
    if (subfunction
        == static_cast< std::uint8_t >(DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier)) {
        std::size_t const minBody{kInt32_8U};
        if (minBody > reuqestPtr->GetBody().size()) {
            result = false;
        }
    } else if (subfunction
               == static_cast< std::uint8_t >(
                   DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier)) {
        std::size_t const minBody{2U};
        if (minBody > reuqestPtr->GetBody().size()) {
            result = false;
        }
    }
    return result;
}

bool ServiceX2C::_checkSubfunctionSupported(MessagePtr const& reuqestPtr) noexcept
{
    if (reuqestPtr.get() != nullptr) {
        Serialize serialize{reuqestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction = serialize.ReadUint8();
        LogInfo() << "ServiceX2C::_checkSubfunctionSupported| subfunction:" << subfunction;
        for (auto&& supportSubfunction : subfunctionTable_) {
            LogVerbose() << "ServiceX2C::_checkSubfunctionSupported| config subfunction:"
                         << static_cast< std::uint8_t >(supportSubfunction);
            using UnderlyingType
                = std::underlying_type< DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum >::type;
            if (static_cast< UnderlyingType >(supportSubfunction) == subfunction) {
                return true;
            }
        }
    }

    LogWarn() << "ServiceX2C::_checkSubfunctionSupported| not support current type, only support "
                 "kDefineByIdentifier,kClearDynamicallyDefineDataIdentifier";
    return false;
}

NrcErrc ServiceX2C::_checkSubfunctionSession(std::shared_ptr< RequestContext > const& context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::_checkSubfunctionSession| context is nullptr";
        return NrcErrc::kGeneralReject;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        Serialize serialize{requestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction = serialize.ReadUint8();
        if (subfunction
            == static_cast< std::uint8_t >(
                DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier)) {
            return NrcErrc::kSuccess;
        }
        if (subfunction
            == static_cast< std::uint8_t >(
                DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier)) {
            std::uint16_t dataIdentifier = serialize.ReadUint16();
            LogInfo() << "ServiceX2C::_checkSubfunctionSession| dataIdentifier:" << dataIdentifier;
            auto findInstanceResult = instanceTable_.find(dataIdentifier);
            if (findInstanceResult == instanceTable_.end()) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSession| dataIdentifier:" << dataIdentifier
                          << "not find dataIdentifier in ServiceX2CModel config!";
                return NrcErrc::kRequestOutOfRange;
            }
            std::shared_ptr< Instance > instancePtr{};
            instancePtr = findInstanceResult->second;
            if (instancePtr.get() == nullptr) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSession| dataIdentifier:" << dataIdentifier
                          << " instacePtr is nullptr";
                return NrcErrc::kRequestOutOfRange;
            }
            if (instancePtr->sessionValidator.get() == nullptr) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSession| dataIdentifier:" << dataIdentifier
                          << " instacePtr->sessionValidator is nullptr";
                return NrcErrc::kRequestOutOfRange;
            }
            if (checkPerSourceId_) {
                std::vector< std::uint8_t > rawData{};
                DiagnosticDynamicDataModel dynamicModel;
                serialize.Seek(2U);
                rawData = serialize.ReadAll();
                _parse(rawData, dynamicModel);
                for (auto&& soureData : dynamicModel.record) {
                    bool sourceCheckSession{};
                    sourceCheckSession = serviceX22Ptr_->CheckDataIdentifierSession(soureData.id);
                    if (!sourceCheckSession) {
                        LogWarn() << "ServiceX2C::_checkSubfunctionSession| source data Identifier:" << soureData.id
                                  << " check session fails!";
                        return NrcErrc::kRequestOutOfRange;
                    }
                }
            }
            if (instancePtr->sessionValidator->Validate(context)) {
                return NrcErrc::kSuccess;
            }
            return NrcErrc::kRequestOutOfRange;
        }
    }
    LogWarn() << "ServiceX2C::_checkSubfunctionSession| request message is nullptr ";
    return NrcErrc::kRequestOutOfRange;
}

bool ServiceX2C::_supportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

bool ServiceX2C::_checkSubfunctionSecurityLevel(std::shared_ptr< RequestContext > const& context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| context is nullptr";
        return false;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        Serialize serialize{requestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction = serialize.ReadUint8();
        if (subfunction
            == static_cast< std::uint8_t >(
                DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier)) {
            return true;
        }
        if (subfunction
            == static_cast< std::uint8_t >(
                DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier)) {
            std::uint16_t dataIdentifier = serialize.ReadUint16();
            LogInfo() << "ServiceX2C::_checkSubfunctionSecurityLevel| dataIdentifier:" << dataIdentifier;
            auto findInstanceResult = instanceTable_.find(dataIdentifier);
            if (findInstanceResult == instanceTable_.end()) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| dataIdentifier:" << dataIdentifier
                          << "not find dataIdentifier in ServiceX2CModel config!";
                return false;
            }
            std::shared_ptr< Instance > instancePtr{};
            instancePtr = findInstanceResult->second;
            if (instancePtr.get() == nullptr) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| dataIdentifier:" << dataIdentifier
                          << " instacePtr is nullptr";
                return false;
            }
            if (instancePtr->securityLevelValidator.get() == nullptr) {
                LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| dataIdentifier:" << dataIdentifier
                          << " instacePtr->securityLevelValidator is nullptr";
                return false;
            }
            if (checkPerSourceId_) {
                std::vector< std::uint8_t > rawData{};
                DiagnosticDynamicDataModel dynamicModel;
                serialize.Seek(2U);
                rawData = serialize.ReadAll();
                _parse(rawData, dynamicModel);
                for (auto&& soureData : dynamicModel.record) {
                    bool sourceCheckSession{};
                    sourceCheckSession = serviceX22Ptr_->CheckDataIdentifierSecurityLevel(soureData.id);
                    if (!sourceCheckSession) {
                        LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| source data Identifier:"
                                  << soureData.id << " check session fails!";
                        return sourceCheckSession;
                    }
                }
            }
            return instancePtr->securityLevelValidator->Validate(context);
        }
    }
    LogWarn() << "ServiceX2C::_checkSubfunctionSecurityLevel| request message is nullptr ";
    return false;
}

void ServiceX2C::_parseAndResponse(std::shared_ptr< RequestContext > const& context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::_parseAndResponse| context is nullptr";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        Serialize serialize{requestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction{0};
        subfunction = serialize.ReadUint8();
        for (auto&& supportSubfunction : subfunctionTable_) {
            LogVerbose() << "ServiceX2C::_parseAndResponse| config subfunction:"
                         << static_cast< std::uint8_t >(supportSubfunction);
            using UnderlyingType
                = std::underlying_type< DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum >::type;
            if (static_cast< UnderlyingType >(supportSubfunction) == subfunction) {
                if (supportSubfunction
                    == DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier) {
                    return _defindAndResponse(context);
                }
                if (supportSubfunction
                    == DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::
                        kClearDynamicallyDefineDataIdentifier) {
                    return _clearAndResponse(context);
                }
            }
        }
        return;
    }
    LogWarn() << "ServiceX2C::_parseAndResponse| request message is nullptr ";
    return;
}

void ServiceX2C::_clearAndResponse(std::shared_ptr< RequestContext > const& context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::_clearAndResponse| context is nullptr";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        std::size_t bodySize{0U};
        std::size_t const notDidSize{2U};
        std::vector< std::uint8_t >& body{requestPtr->GetBody()};
        bodySize = body.size();
        if (bodySize == notDidSize) {
            std::ignore = dataIdentifierManager_->ClearAllDefined();
        } else {
            Serialize serialize{requestPtr->GetBody()};
            serialize.Seek(notDidSize);
            if (bodySize % sizeof(std::uint16_t) != 0) {
                LogWarn() << "ServiceX2C::_clearAndResponse| request size % 2 != 0! Please check request size!";
                return context->Respond(NrcErrc::kRequestOutOfRange);
            }
            std::uint16_t did{0U};
            std::size_t countOfDid{0U};
            countOfDid = bodySize / sizeof(std::uint16_t) - 1;
            std::size_t countOfFails{0U};
            while (serialize.Read(did)) {
                std::uint8_t clearResult{};
                clearResult = dataIdentifierManager_->ClearDefined(did);
                if (clearResult == 0U) {
                    LogVerbose() << "ServiceX2C::_clearAndResponse|did:" << did << "clear success!";
                } else {
                    LogWarn() << "ServiceX2C::_clearAndResponse|did:" << did << "clear fails!";
                    countOfFails++;
                }
            }
            if (countOfDid == countOfFails) {
                LogWarn() << "ServiceX2C::_clearAndResponse| all did in request is fails!";
                return context->Respond(NrcErrc::kRequestOutOfRange);
            }
        }
        MessagePtr response{};
        response = context->CreateResponse();

        Serialize deserialize(response->GetBody());
        deserialize.Write(static_cast< std::uint8_t >(
            DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kClearDynamicallyDefineDataIdentifier));
        context->Respond(std::move(response));
        return;
    }
    LogWarn() << "ServiceX2C::_clearAndResponse| request message is nullptr ";
    return;
}

void ServiceX2C::_defindAndResponse(std::shared_ptr< RequestContext > const& context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX2C::_defindAndResponse| context is nullptr";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        std::vector< std::uint8_t >& body{requestPtr->GetBody()};
        std::vector< std::uint8_t > rawData{};
        DiagnosticDynamicDataModel dynamicModel;
        Serialize serialize{body};
        serialize.Seek(2U);
        rawData             = serialize.ReadAll();
        NrcErrc parseResult = _parse(rawData, dynamicModel);
        if (parseResult != NrcErrc::kSuccess) {
            LogWarn() << "ServiceX2C::_defindAndResponse| parse source did fails!";
            return context->Respond(parseResult);
        }
        auto findInstanceResult = instanceTable_.find(dynamicModel.id);
        if (findInstanceResult == instanceTable_.end()) {
            LogWarn() << "ServiceX2C::_defindAndResponse| dataIdentifier:" << dynamicModel.id
                      << "not find dataIdentifier in ServiceX2CModel config!";
            return context->Respond(NrcErrc::kRequestOutOfRange);
        }
        std::shared_ptr< Instance > instancePtr{};
        instancePtr = findInstanceResult->second;
        if (instancePtr.get() == nullptr) {
            LogWarn() << "ServiceX2C::_defindAndResponse| dataIdentifier:" << dynamicModel.id
                      << " instacePtr is nullptr";
            return context->Respond(NrcErrc::kRequestOutOfRange);
        }
        std::size_t numOfRequestSourceElement{};
        numOfRequestSourceElement = dynamicModel.record.size();
        std::size_t numOfDefinedSourceElement{};
        numOfDefinedSourceElement = dataIdentifierManager_->GetCountOfDiagnosticDynamicDataModelSource(dynamicModel.id);
        if (instancePtr->maxSourceElement < numOfDefinedSourceElement + numOfRequestSourceElement) {
            LogWarn() << "ServiceX2C::_defindAndResponse| dataIdentifier:" << dynamicModel.id
                      << "source element too long!max source Element:" << instancePtr->maxSourceElement
                      << " defined source element num:" << numOfDefinedSourceElement;
            return context->Respond(NrcErrc::kRequestOutOfRange);
        }
        std::size_t dataSize{};
        dataSize = dataIdentifierManager_->GetDataSize(dataSize);
        for (auto&& sourceModel : dynamicModel.record) {
            dataSize += sourceModel.nSize;
        }
        if (dataSize > context->GetMaxPayloadLength()) {
            LogWarn() << "ServiceX2C::_defindAndResponse|The total length of a dynamically defined "
                         "periodicDataIdentifier exceeds the maximum length that fits into a single frame of the data "
                         "link used for transmission of the periodic response messag";
            return context->Respond(NrcErrc::kRequestOutOfRange);
        }

        std::uint8_t definedResult{};
        definedResult = dataIdentifierManager_->Defined(dynamicModel, stored_);
        if (0 != definedResult) {
            LogWarn() << "ServiceX2C::_defindAndResponse| dataIdentifier:" << dynamicModel.id << "Defined fails";
            return context->Respond(NrcErrc::kRequestOutOfRange);
        }
        MessagePtr response{};
        response = context->CreateResponse();

        Serialize deserialize(response->GetBody());
        deserialize.Write(
            static_cast< std::uint8_t >(DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier));
        deserialize.Write(dynamicModel.id);
        context->Respond(std::move(response));
        return;
    }
    LogWarn() << "ServiceX2C::_defindAndResponse| request message is nullptr ";
    return;
}

NrcErrc ServiceX2C::_parse(std::vector< std::uint8_t >& rawData, DiagnosticDynamicDataModel& dynamicDataModel) noexcept
{
    std::size_t const minSize{6U};
    if (rawData.size() < minSize) {
        LogWarn() << "ServiceX2C::_parse| rawData size must > 6 byte!";
        return NrcErrc::kRequestOutOfRange;
    }
    std::size_t const sourceDataSize{4U};
    if ((rawData.size() - sizeof(std::uint16_t)) % sourceDataSize != 0) {
        LogWarn() << "ServiceX2C::_parse| message size is invalid! message size:" << rawData.size() + 2;
        return NrcErrc::kRequestOutOfRange;
    }

    Serialize deserialize(rawData);
    dynamicDataModel.id = deserialize.ReadUint16();
    SourceData sourceData{};
    std::uint8_t pos{};
    while (deserialize.Read(sourceData.id)) {
        pos = deserialize.ReadUint8();
        if (pos == 0) {

            LogWarn() << "ServiceX2C::_parse|The positionInSourceDataRecord was incorrect (less than 1);";
            return NrcErrc::kRequestOutOfRange;
        }
        sourceData.nPos  = pos - 1U;
        sourceData.nSize = deserialize.ReadUint8();
        dynamicDataModel.record.push_back(sourceData);
    }
    return NrcErrc::kSuccess;
}

void ServiceX2C::_clearWhenSessionChange(SessionId, SessionId) noexcept
{
    auto dynamicDataModelTable = dataIdentifierManager_->GetAllDynamicDataModel();
    for (auto&& dynamicDataModelRef : dynamicDataModelTable) {
        for (auto&& soureData : dynamicDataModelRef.get().record) {
            bool sourceCheck{};
            sourceCheck = serviceX22Ptr_->CheckDataIdentifierSession(soureData.id);
            if (!sourceCheck) {
                LogWarn() << "ServiceX2C::_clearWhenSessionChange| source data Identifier:" << soureData.id
                          << " check session fails!";
                std::uint8_t clearResult{0U};
                clearResult = dataIdentifierManager_->ClearDefined(dynamicDataModelRef.get().id);
                LogInfo() << "ServiceX2C::_clearWhenSessionChange| clear dynamic id:" << dynamicDataModelRef.get().id;
                if (clearResult != 0) {
                    LogWarn() << "ServiceX2C::_clearWhenSessionChange| clear dynamic id:"
                              << dynamicDataModelRef.get().id << " fails";
                }
                return;
            }

            sourceCheck = serviceX22Ptr_->CheckDataIdentifierSecurityLevel(soureData.id);
            if (!sourceCheck) {
                LogWarn() << "ServiceX2C::_clearWhenSessionChange| source data Identifier:" << soureData.id
                          << " check session fails!";
                std::uint8_t clearResult{0U};
                clearResult = dataIdentifierManager_->ClearDefined(dynamicDataModelRef.get().id);
                LogInfo() << "ServiceX2C::_clearWhenSessionChange| clear dynamic id:" << dynamicDataModelRef.get().id;
                if (clearResult != 0) {
                    LogWarn() << "ServiceX2C::_clearWhenSessionChange| clear dynamic id:"
                              << dynamicDataModelRef.get().id << " fails";
                }
                return;
            }
        }
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
