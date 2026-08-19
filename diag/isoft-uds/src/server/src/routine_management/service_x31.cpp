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
/// @file       service_x31.cpp
/// @brief      This file implements service_x31 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=routine_management
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=service_x31.h
/// @unit_description=This file provides service_x31 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x31.h"

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x31 {
std::uint8_t const kMinRequestSize{4U};

}  // namespace service_x31
ServiceX31::ServiceX31() noexcept
    : instanceTable_{}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint16_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint16_t > >()}
    , interface_{nullptr}
{
}

std::int32_t ServiceX31::Initialize(RoutineManagement& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    if (config.routineInstanceTable.empty()) {
        LogError() << "ServiceX31::Initialize| RoutineManagement.routineInstanceTable is empty !";
        return -1;
    }
    if (config.interfacePtr.get() == nullptr) {
        LogError() << "ServiceX31::Initialize|RoutineManagement.interfacePtr is empty!";
        return -1;
    }
    interface_ = config.interfacePtr;
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "ServiceX31::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogError() << "ServiceX31::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }

    for (auto&& routineInstanceConfig : config.routineInstanceTable) {
        std::shared_ptr< Instance > instancePtr{std::make_shared< Instance >()};
        if (instancePtr.get() == nullptr) {
            LogError() << "allow memory fails in new struct Instance";
            return -1;
        }

        instancePtr->id               = routineInstanceConfig.routine.id;
        instancePtr->routineInfo      = routineInstanceConfig.routine.routineInfo;
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint16_t > >();
        if (routineInstanceConfig.routine.startAccessPermissionEnvCondition == -1) {
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kStarRoutine, nullptr));
        } else if (routineInstanceConfig.routine.startAccessPermissionEnvCondition >= 0) {
            std::shared_ptr< EnvironmentalCondition > enCondition = std::make_shared< EnvironmentalCondition >();
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kStarRoutine, enCondition));
        }

        if (routineInstanceConfig.routine.stopAccessPermissionEnvCondition == -1) {
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kStopRoutine, nullptr));
        } else if (routineInstanceConfig.routine.stopAccessPermissionEnvCondition >= 0) {
            std::shared_ptr< EnvironmentalCondition > enCondition = std::make_shared< EnvironmentalCondition >();
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kStarRoutine, enCondition));
        }

        if (routineInstanceConfig.routine.getResultAccessPermissionEnvCondition == -1) {
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kRequestRoutineResults, nullptr));
        } else if (routineInstanceConfig.routine.getResultAccessPermissionEnvCondition >= 0) {
            std::shared_ptr< EnvironmentalCondition > enCondition = std::make_shared< EnvironmentalCondition >();
            instancePtr->subfunctionConditionMap.insert(std::make_pair(Subfunction::kStarRoutine, enCondition));
        }

        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }
        std::int32_t result{0};
        if (routineInstanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint16_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(authValidator->Initialize(
                instancePtr->id, routineInstanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint16_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(instancePtr->id, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check RoutineControlInstanceConfig.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        result = static_cast< int32_t >(instancePtr->sessionValidator->Initialize(
            instancePtr->id, routineInstanceConfig.sessionPermission, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        result = sessionValidator_->RegisterSubfunctionLevel(instancePtr->id, instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn() << "session id repeatability! Please check RoutineControlInstanceConfig.sessionPermission";
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint16_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            instancePtr->id, routineInstanceConfig.securityLevelPermission, securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result
            = securityLevelValidator_->RegisterSubfunctionLevel(instancePtr->id, instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "RoutineControlInstanceConfig.securityLevelPermission";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(instancePtr->id);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "RoutineControlInstanceConfig.id:" << instancePtr->id << " alreay exist";
            return -1;
        }

        instancePtr->p4ServerMax       = routineInstanceConfig.routine.p4ServerMax;
        instancePtr->startP4ServerMax  = routineInstanceConfig.routine.startP4ServerMax;
        instancePtr->stopP4ServerMax   = routineInstanceConfig.routine.stopP4ServerMax;
        instancePtr->resultP4ServerMax = routineInstanceConfig.routine.resultP4ServerMax;

        std::ignore = instanceTable_.insert({instancePtr->id, instancePtr});
    }

    return 0;
}

void ServiceX31::Process(std::shared_ptr< RequestContext > context) noexcept
{
    decltype(auto) request{context->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};

    std::size_t const bodySize{requestBody.size()};
    ///  [SWS_DM_00098]
    if (bodySize < service_x31::kMinRequestSize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX31::Process|kIncorrectMessageLengthOrInvalidFormat";
        return;
    }

    Serialize deserialize{requestBody};
    std::ignore                        = deserialize.ReadUint8();
    std::uint8_t subfunction           = deserialize.ReadUint8();
    std::uint16_t id                   = deserialize.ReadUint16();
    std::vector< std::uint8_t > record = deserialize.ReadAll();
    ///  [SWS_DM_00202]
    ara::core::Map< std::uint16_t, std::shared_ptr< Instance > >::iterator const it{instanceTable_.find(id)};
    if (it == instanceTable_.end()) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "ServiceX31::Process| not find routine instance in instanceTable!,id:" << id;
        return;
    }

    decltype(auto) instance{it->second};

    if (instance->authValidator.get() != nullptr) {
        ///  [SWS_DM_00448]
        if (!instance->authValidator->Validate(context)) {
            context->Respond(NrcErrc::kAuthenticationRequired);
            LogWarn() << "ServiceX31::Process| auth validator fails!,id:" << id;
            return;
        }
    }

    ///  [SWS_DM_00448]
    if (!instance->sessionValidator->Validate(context)) {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "ServiceX31::Process| session validator fails!,id:" << id;
        return;
    }

    ///  [SWS_DM_00437]

    if (instance->securityLevelValidator.get() != nullptr && !instance->securityLevelValidator->Validate(context)) {
        context->Respond(NrcErrc::kSecurityAccessDenied);
        LogWarn() << "ServiceX31::Process| security level validator fails!,id:" << id;
        return;
    }
    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    auto findSubfunctionResult = instance->subfunctionConditionMap.find(static_cast< Subfunction >(subfunction));
    if (findSubfunctionResult != instance->subfunctionConditionMap.end()) {
        if (findSubfunctionResult->second.get() != nullptr) {
            std::uint8_t const nrcValue{
                findSubfunctionResult->second->Check(metaInfo, context->GetCancellationState())};
            if (nrcValue != 0U) {
                context->Respond(nrcValue);
                LogWarn() << "ServiceX31::Process| condition check fails!,id:" << id;
                return;
            }
            LogWarn() << "ServiceX31::Process|1 subfuntion not support!,id:" << id << " subfunction:" << subfunction;
            return;
        }
    } else {
        context->Respond(NrcErrc::kSubfunctionNotSupported);
        LogWarn() << "ServiceX31::Process|2 subfuntion not support!,id:" << id << " subfunction:" << subfunction;
        return;
    }

    Result< GenericRoutineInterface::OperationOutput > requestResult;
    switch (static_cast< Subfunction >(subfunction)) {
        case Subfunction::kStarRoutine: {
            requestResult
                = interface_->Start(id, record, metaInfo, CancellationHandler(context->GetCancellationState()));
        } break;
        case Subfunction::kStopRoutine: {
            requestResult
                = interface_->Stop(id, record, metaInfo, CancellationHandler(context->GetCancellationState()));
        } break;

        case Subfunction::kRequestRoutineResults: {
            requestResult = interface_->RequestResults(id, record, metaInfo,
                                                       CancellationHandler(context->GetCancellationState()));
        } break;

        default:
            break;
    }
    if (requestResult.HasValue()) {
        MessagePtr responsePtr{context->CreateResponse()};
        Serialize serialize{responsePtr->GetBody()};
        serialize.Write(subfunction);
        serialize.Write(id);
        if (instance->routineInfo != -1) {
            std::uint8_t const routineInfo{static_cast< std::uint8_t >(instance->routineInfo)};
            serialize.Write(routineInfo);
        }
        serialize.Write(requestResult.Value().responseData);
        context->Respond(std::move(responsePtr));
        LogInfo() << "ServiceX31::Process| positive response send end!";
    } else {
        context->Respond(requestResult.Error());
        LogWarn() << "ServiceX31::Process| nrc response send end! nrc:" << requestResult.Error();
    }
}

std::vector< ValidatorPtr > ServiceX31::GetSidValidator() noexcept
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

void ServiceX31::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    std::ignore = request;
    std::ignore = status;
}

std::uint32_t ServiceX31::GetP4ServerMax(std::int16_t const subfunction, std::int32_t const assistParam) noexcept
{
    if (assistParam == -1) {
        LogWarn() << "ServiceX31::GetP4ServerMax|invalid routine id";
        return 0U;
    }

    ara::core::Map< std::uint16_t, std::shared_ptr< Instance > >::iterator it
        = instanceTable_.find(static_cast< uint16_t >(assistParam));
    if (it == instanceTable_.end()) {
        LogWarn() << "ServiceX31::GetP4ServerMax|invalid routine id =" << assistParam;
        return 0U;
    }

    if (it->second.get() == nullptr) {
        LogWarn() << "ServiceX31::GetP4ServerMax|routine instance is NULL, routine id =" << assistParam;
        return 0U;
    }

    std::uint32_t p4 = it->second->p4ServerMax;

    Subfunction const sf = static_cast< Subfunction >(subfunction);
    switch (sf) {
        case Subfunction::kStarRoutine:
            if (it->second->startP4ServerMax > 0U) {
                p4 = it->second->startP4ServerMax;
            }
            break;
        case Subfunction::kStopRoutine:
            if (it->second->stopP4ServerMax > 0U) {
                p4 = it->second->stopP4ServerMax;
            }
            break;
        case Subfunction::kRequestRoutineResults:
            if (it->second->resultP4ServerMax > 0U) {
                p4 = it->second->resultP4ServerMax;
            }
            break;
        default:
            break;
    }

    return p4;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
