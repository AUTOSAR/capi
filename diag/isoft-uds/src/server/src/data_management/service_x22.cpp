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
/// @file       service_x22.cpp
/// @brief      This file implements service_x22 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=f4cb17cc-0c50-4150-805c-6a5d901cdc3a
/// @unit_name=service_x22.h
/// @unit_description=This file provides service_x22 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x22.h"

#include "common/log.h"
#include "common/serialize.h"

namespace isoft {
namespace uds {
namespace server {

ServiceX22::ServiceX22() noexcept
    : instanceTable_{}
    , dataIdentifierManager_{nullptr}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint16_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint16_t > >()}
{
}

std::int32_t ServiceX22::Initialize(ServiceX22Model& model,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager >& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                                    std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr,
                                    std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    if (model.table.empty()) {
        LogWarn() << "ServiceX22Model.table is empty!";
        return -1;
    }
    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "sessionManagerPtr is nullptr";
        return -1;
    }
    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "securityLevelManagerPtr is nullptr";
        return -1;
    }

    if (diagnosticDataManagerPtr.get() == nullptr) {
        LogWarn() << "envirConditionManagerPtr is nullptr";
        return -1;
    }
    maxDidToRead_ = model.maxDidToRead;
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
        std::int32_t result{0};
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint16_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
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
        if (instanceConfig.accessPermissionEnvCondition >= 0) {
            instancePtr->envCondition = envirConditionManagerPtr->Get(instanceConfig.accessPermissionEnvCondition);
            if (instancePtr->envCondition.get() == nullptr) {
                LogWarn() << "environmental condition id:" << instanceConfig.accessPermissionEnvCondition
                          << " is invalid!";
                return -1;
            }
        }
        auto findInstanceResult = instanceTable_.find(instanceConfig.id);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ReadDiagnosticDataByIdentifier.id:" << instanceConfig.id << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({instanceConfig.id, instancePtr});
    }
    dataIdentifierManager_ = diagnosticDataManagerPtr;
    p4ServerMax_           = model.p4ServerMax;
    return 0;
}

void ServiceX22::Process(std::shared_ptr< RequestContext > context) noexcept
{
    MessagePtr& request{context->GetRequest()};

    ///  [SWS_DM_00098]
    std::set< std::uint16_t > dataIdentifiers;
    isoft::serialize::Buffer< std::vector< uint8_t > > buffer(request->GetBody());
    std::size_t const bodySize{request->GetBody().size()};
    std::size_t const minBodySize{3U};
    if (bodySize < minBodySize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
        LogWarn() << "request data size < 3 byte!";
        return;
    }

    if ((bodySize - 1U) % sizeof(std::uint16_t) != 0U) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
        LogWarn() << "length of every did must equal 2";
        return;
    }

    std::size_t const elementNum{bodySize / sizeof(std::uint16_t)};
    {
        std::size_t point{buffer.First() + 1U};
        std::uint16_t id{0U};
        for (size_t i = 0U; i < elementNum; i++) {
            std::ignore = isoft::serialize::DeserializePart(buffer, point, id);
            std::ignore = dataIdentifiers.insert(id);
        }
    }

    if (elementNum > maxDidToRead_) {
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        LogWarn() << "numberOfDid > maxDidToRead in uds message! numberOfDid:" << elementNum
                  << " maxDidToRead:" << maxDidToRead_;
        return;
    }

    std::vector< std::uint16_t > validDataIdentifierTable;
    NrcErrc filterResult{NrcErrc::kSuccess};
    filterResult = _filter(context, dataIdentifiers, validDataIdentifierTable);
    if (filterResult != NrcErrc::kSuccess) {
        context->Respond(filterResult, true);
        return;
    }

    Result< std::list< DiagnosticData > > readResult{};
    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    std::shared_ptr< CancellationState > cancellationState{};
    cancellationState = context->GetCancellationState();
    readResult        = dataIdentifierManager_->Read(validDataIdentifierTable, metaInfo, cancellationState);
    if (!readResult.HasValue()) {
        LogWarn() << " read diagnostic data fails!";
        context->Respond(readResult.Error(), true);
        return;
    }
    if (readResult.Value().empty()) {
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        LogWarn() << "Read Did Data Fails! result is empty!Please check network!";
        return;
    }
    MessagePtr responsePtr{nullptr};
    responsePtr          = context->CreateResponse();
    Buffer& responseBody = responsePtr->GetBody();

    Serialize deserialize(responseBody);
    std::uint32_t didCount{0};
    for (auto&& diagnosticData : readResult.Value()) {
        if (diagnosticData.record.empty()) {
            continue;
        }
        ++didCount;
        deserialize.Write(diagnosticData.id);
        deserialize.Write(diagnosticData.record);
    }
    if (didCount == 0U) {
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        LogWarn() << "Read Did Data Fails! result is empty!Please check network!";
        return;
    }
    context->Respond(std::move(responsePtr));
    return;
}

bool ServiceX22::CheckDataIdentifierSession(std::uint16_t dataIdentifier) noexcept
{
    auto findInstanceResult = instanceTable_.find(dataIdentifier);
    if (findInstanceResult == instanceTable_.end()) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << " not find in ServiceX22";
        return false;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", instace not created!";
        return false;
    }
    bool validateResult{false};
    if (instancePtr->sessionValidator.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", not config session id!";
        return false;
    }

    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
    sessionValidator = instancePtr->sessionValidator;
    validateResult   = sessionValidator->Validate(nullptr);
    return validateResult;
}

bool ServiceX22::CheckDataIdentifierSecurityLevel(std::uint16_t dataIdentifier) noexcept
{
    auto findInstanceResult = instanceTable_.find(dataIdentifier);
    if (findInstanceResult == instanceTable_.end()) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << " not find in ServiceX22";
        return false;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", instace not created!";
        return false;
    }
    bool validateResult{false};
    if (instancePtr->securityLevelValidator.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", not config session id!";
        return false;
    }

    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator{nullptr};
    securityLevelValidator = instancePtr->securityLevelValidator;
    validateResult         = securityLevelValidator->Validate(nullptr);
    return validateResult;
}

bool ServiceX22::CheckDataIdentifierCondition(
    std::uint16_t dataIdentifier,
    const isoft::uds::server::MetaInfoMap& metaInfo,
    std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept
{
    auto findInstanceResult = instanceTable_.find(dataIdentifier);
    if (findInstanceResult == instanceTable_.end()) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << " not find in ServiceX22";
        return false;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", instace not created!";
        return false;
    }
    std::uint8_t validateResult{0};
    if (instancePtr->envCondition.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << dataIdentifier << ", not config condition!";
        return true;
    }

    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator{nullptr};
    securityLevelValidator = instancePtr->securityLevelValidator;
    validateResult         = instancePtr->envCondition->Check(metaInfo, cancellationState);
    if (validateResult != 0) {
        return false;
    }
    return true;
}

std::vector< ValidatorPtr > ServiceX22::GetSidValidator() noexcept
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

std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > ServiceX22::GetSessionValidator(
    std::uint16_t id) noexcept
{
    auto findInstanceResult = instanceTable_.find(id);
    if (findInstanceResult == instanceTable_.end()) {
        LogInfo() << "dataIdentifier:" << id << " not find in ServiceX22";
        return nullptr;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << id << ", instace not created!";
        return nullptr;
    }
    return instancePtr->sessionValidator;
}

std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > ServiceX22::GetSecurityLevelValidator(
    std::uint16_t id) noexcept
{
    auto findInstanceResult = instanceTable_.find(id);
    if (findInstanceResult == instanceTable_.end()) {
        LogInfo() << "dataIdentifier:" << id << " not find in ServiceX22";
        return nullptr;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogInfo() << "dataIdentifier:" << id << ", instace not created!";
        return nullptr;
    }
    return instancePtr->securityLevelValidator;
}

void ServiceX22::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    LogVerbose() << "ServiceX22::Confirm| RequestHandle:" << request.GetMetaInfoMap()["kRequestHandle"]
                 << "response status:" << EnumToString(status);
}

NrcErrc ServiceX22::_filter(std::shared_ptr< RequestContext >& context,
                            std::set< std::uint16_t >& rawDataIdentifierTable,
                            std::vector< std::uint16_t >& validDataIdentifierTable) noexcept
{
    for (auto&& dataIdentifier : rawDataIdentifierTable) {
        auto findInstanceResult = instanceTable_.find(dataIdentifier);
        if (findInstanceResult == instanceTable_.end()) {
            LogInfo() << "dataIdentifier:" << dataIdentifier << " not find in ServiceX22";
            continue;
        }
        std::shared_ptr< Instance > instancePtr{nullptr};
        instancePtr = findInstanceResult->second;
        if (instancePtr.get() == nullptr) {
            LogInfo() << "dataIdentifier:" << dataIdentifier << ", instace not created!";
            continue;
        }
        bool validateResult{false};
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogInfo() << "dataIdentifier:" << dataIdentifier << ", not config session id!";
            continue;
        }

        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
        sessionValidator = instancePtr->sessionValidator;
        validateResult   = sessionValidator->Validate(context);
        if (!validateResult) {
            LogInfo() << "dataIdentifier:" << dataIdentifier << ", not support in current session!";
            continue;
        }
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator{nullptr};
        authValidator = instancePtr->authValidator;
        if (authValidator.get() != nullptr) {
            validateResult = authValidator->Validate(context);
            if (!validateResult) {
                LogInfo() << "dataIdentifier:" << dataIdentifier << ", not support in authentication!";
                return NrcErrc::kAuthenticationRequired;
            }
        }

        if (instancePtr->securityLevelValidator.get() != nullptr) {
            validateResult = instancePtr->securityLevelValidator->Validate(context);
            if (!validateResult) {
                LogInfo() << "dataIdentifier:" << dataIdentifier << ", not support in current security level!";
                return NrcErrc::kSecurityAccessDenied;
            }
        }
        if (instancePtr->envCondition.get() != nullptr) {
            std::uint8_t nrc{0U};
            MetaInfoMap metaInfo{context->GetMetaInfoMap()};
            std::shared_ptr< CancellationState > cancellationState{};
            cancellationState = context->GetCancellationState();
            nrc               = instancePtr->envCondition->Check(metaInfo, cancellationState);
            if (nrc != 0U) {
                LogInfo() << "dataIdentifier:" << dataIdentifier << ", not support in enviroment condition!";
                return static_cast< NrcErrc >(nrc);
            }
        }
        validDataIdentifierTable.push_back(dataIdentifier);
    }
    if (validDataIdentifierTable.empty()) {
        return NrcErrc::kRequestOutOfRange;
    }
    return NrcErrc::kSuccess;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
