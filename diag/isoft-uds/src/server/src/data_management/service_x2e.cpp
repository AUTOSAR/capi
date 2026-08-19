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
/// @file       service_x2e.cpp
/// @brief      This file implements service_x2e functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=ccc6be0c-a528-4c55-927d-c71b320fac2f
/// @unit_name=service_x2e.h
/// @unit_description=This file provides service_x2e functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x2e.h"

#include "common/log.h"
#include "common/serialize.h"
namespace isoft {
namespace uds {
namespace server {
namespace service_x2e {
std::uint8_t const kRequestBodyHead{3U};

}  // namespace service_x2e

ServiceX2E::ServiceX2E() noexcept
    : instanceTable_{}
    , dataIdentifierManager_{nullptr}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint16_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint16_t > >()}

{
}



std::int32_t ServiceX2E::Initialize(ServiceX2EModel& model,
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
        LogWarn() << "diagnosticDataManager is nullptr";
        return -1;
    }
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
        if (instanceConfig.dataSize == 0U) {
            LogWarn() << "WriteDiagnosticDataByIdentifier.dataSIze_ must not equal 0!";
            return -1;
        }
        instancePtr->dataSize = instanceConfig.dataSize;
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
                LogWarn() << "auth id repeatability! Please check WriteDiagnosticDataByIdentifier.accessPermissionAuth";
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


void ServiceX2E::Process(std::shared_ptr< RequestContext > context) noexcept
{
    MessagePtr& request{context->GetRequest()};

    ///  [SWS_DM_00098]
    std::size_t const bodySize{request->GetBody().size()};
    std::size_t const minBodySize{4U};
    if (bodySize < minBodySize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
        LogWarn() << "request data size < 4 byte!";
        return;
    }
    DiagnosticData data;
    Serialize deserialize{request->GetBody()};
    std::ignore             = deserialize.ReadUint8();
    data.id                 = deserialize.ReadUint16();
    data.record             = deserialize.ReadAll();
    auto findInstanceResult = instanceTable_.find(data.id);
    if (findInstanceResult == instanceTable_.end()) {
        LogWarn() << "dataIdentifier:" << data.id << " not find in ServiceX2E";
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        return;
    }
    std::shared_ptr< Instance > instancePtr{nullptr};
    instancePtr = findInstanceResult->second;
    if (instancePtr.get() == nullptr) {
        LogWarn() << "dataIdentifier:" << data.id << ", instace not created!";
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        return;
    }
    if (instancePtr->dataSize + sizeof(std::uint8_t) + sizeof(std::uint16_t) != bodySize) {

        LogWarn() << "dataIdentifier:" << data.id << ", total length is 3 byte (SI + DID) + nth byte DREC";
        return context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
    }

    bool validateResult{false};
    if (instancePtr->sessionValidator.get() == nullptr) {
        LogWarn() << "dataIdentifier:" << data.id << ", not config session id!";
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        return;
    }

    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
    sessionValidator = instancePtr->sessionValidator;
    validateResult   = sessionValidator->Validate(context);
    if (!validateResult) {
        LogWarn() << "dataIdentifier:" << data.id << ", not support in current session!";
        context->Respond(NrcErrc::kRequestOutOfRange, true);
        return;
    }
    if (instancePtr->authValidator.get() != nullptr) {
        validateResult = instancePtr->authValidator->Validate(context);
        if (!validateResult) {
            LogWarn() << "dataIdentifier:" << data.id << ", not support in auth!";
            context->Respond(NrcErrc::kAuthenticationRequired, true);
            return;
        }
    }
    if (instancePtr->securityLevelValidator.get() != nullptr) {
        validateResult = instancePtr->securityLevelValidator->Validate(context);
        if (!validateResult) {
            LogWarn() << "dataIdentifier:" << data.id << ", not support in current security level!";
            context->Respond(NrcErrc::kSecurityAccessDenied, true);
            return;
        }
    }
    if (instancePtr->envCondition.get() != nullptr) {
        std::uint8_t nrc{0U};
        MetaInfoMap metaInfo{context->GetMetaInfoMap()};
        std::shared_ptr< CancellationState > cancellationState{};
        cancellationState = context->GetCancellationState();
        nrc               = instancePtr->envCondition->Check(metaInfo, cancellationState);
        if (nrc != 0U) {
            LogWarn() << "dataIdentifier:" << data.id << ", not support in enviroment condition!";
            context->Respond(nrc, true);
            return;
        }
    }

    Result< void > readResult{0U};
    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    std::shared_ptr< CancellationState > cancellationState{};
    cancellationState = context->GetCancellationState();
    readResult        = dataIdentifierManager_->Write(data, metaInfo, cancellationState);
    if (!readResult.HasValue()) {
        LogWarn() << " write diagnostic data fails! error = " << readResult.Error();
        context->Respond(readResult.Error(), true);
        return;
    }
    MessagePtr responsePtr{nullptr};
    responsePtr          = context->CreateResponse();
    Buffer& responseBody = responsePtr->GetBody();

    Serialize serialize(responseBody);
    serialize.Write(data.id);
    context->Respond(std::move(responsePtr));
    return;
}

std::vector< ValidatorPtr > ServiceX2E::GetSidValidator() noexcept
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

void ServiceX2E::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    LogVerbose() << "ServiceX2E::Confirm| RequestHandle:" << request.GetMetaInfoMap()["kRequestHandle"]
                 << "response status:" << EnumToString(status);
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
