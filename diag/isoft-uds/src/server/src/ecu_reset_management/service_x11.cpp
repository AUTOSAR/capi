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
/// @file       service_x11.cpp
/// @brief      This file implements service_x11 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=ecu_reset_management
/// @trace_id_sr=a58af830-c7ef-4bcf-968e-c96363d8cb28
/// @unit_name=service_x11.h
/// @unit_description=This file provides service_x11 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x11.h"

#include "common/log.h"
#include "common/serialize.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x11 {
std::uint8_t const kBodySize{2U};
}  // namespace service_x11
ServiceX11::ServiceX11(std::shared_ptr< PersistenceService >& persistenceService,
                       std::function< void(BlockState) > notify) noexcept
    : instanceTable_{}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
    , interface_{nullptr}
    , respondToReset_{}
    , persistenceService_{persistenceService}
    , blockNotify_{std::move(notify)}
{
}

std::int32_t ServiceX11::Initialize(EcuResetManagement& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    assert(!config.instanceTable.empty());
    assert(config.interfacePtr.get() != nullptr);
    assert(sessionManagerPtr.get() != nullptr);
    assert(securityLevelManagerPtr.get() != nullptr);
    if (config.instanceTable.empty()) {
        LogError() << "ServiceX11::Initialize|instanceTableConfig.instanceTable_ is empty!";
        return -1;
    }
    if (config.interfacePtr.get() == nullptr) {
        LogError() << "ServiceX11::Initialize|instanceTableConfig.interface_ is empty!";
        return -1;
    }
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "ServiceX11::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogError() << "ServiceX11::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }
    interface_      = config.interfacePtr;
    respondToReset_ = config.respondToReset;
    for (auto&& instanceConfig : config.instanceTable) {
        std::shared_ptr< Instance > instancePtr{nullptr};
        instancePtr = std::make_shared< Instance >();
        if (instancePtr.get() == nullptr) {
            LogWarn() << "allow memory fails in new struct Instance";
            return -1;
        }
        instancePtr->subfunctionNumber = instanceConfig.subfunctionNumber;
        std::int32_t result{0};
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(authValidator->Initialize(
                instanceConfig.subfunctionNumber, instanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(instanceConfig.subfunctionNumber, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check EcuResetInstanceConfig.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }

        result = static_cast< int32_t >(instancePtr->sessionValidator->Initialize(
            instanceConfig.subfunctionNumber, instanceConfig.accessPermissionSession, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        result = sessionValidator_->RegisterSubfunctionLevel(instanceConfig.subfunctionNumber,
                                                             instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn() << "session id repeatability! Please check EcuResetInstanceConfig.accessPermissionSession";
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            instanceConfig.subfunctionNumber, instanceConfig.accessPermissionSecurityLevel, securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result = securityLevelValidator_->RegisterSubfunctionLevel(instanceConfig.subfunctionNumber,
                                                                   instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "EcuResetInstanceConfig.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(instanceConfig.subfunctionNumber);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "EcuResetInstanceConfig.subfunctionNumber:" << instanceConfig.subfunctionNumber
                      << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({instanceConfig.subfunctionNumber, instancePtr});
    }

    p4ServerMax_ = config.p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX11::GetSidValidator() noexcept
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

std::size_t ServiceX11::GetMinimumSize(std::uint8_t) noexcept
{
    std::size_t const minBody{2U};
    return minBody;
}

bool ServiceX11::IsSupport(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX11::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX11::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX11::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX11::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX11::SupportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX11::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX11::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX11::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        return;
    }
    MessagePtr requestMessagePtr{context->GetRequest()};
    if (requestMessagePtr.get() == nullptr) {
        return;
    }
    if (requestMessagePtr->GetBody().size() != 2) {
        LogWarn() << "ServiceX11::ParseAndResponse| message size must equal 2 bytes";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize deserialize{requestMessagePtr->GetBody()};
    deserialize.Seek(1);
    std::uint8_t subfunction{deserialize.ReadUint8()};

    decltype(auto) metaInfo{context->GetMetaInfoMap()};
    Result< void > requestResult;
    if ((subfunction == kEnableRapidPowerShutDown) || (subfunction == kDisableRapidPowerShutDown)) {
        bool const enable{subfunction == kEnableRapidPowerShutDown};
        requestResult
            = interface_->EnableRapidShutdown(enable, metaInfo, CancellationHandler(context->GetCancellationState()));

    } else {
        ResetRequestType type{kCustomResetType};
        if (subfunction == kHardReset) {
            type = kHardResetType;
        } else if (subfunction == kKeyOffOnReset) {
            type = kKeyOffOnResetType;
        } else if (subfunction == kSoftReset) {
            type = kSoftResetType;
        }
        _blockNotify();
        requestResult = interface_->RequestReset(type, subfunction, metaInfo,
                                                 CancellationHandler(context->GetCancellationState()));
    }

    _unblockNotify();
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        response->GetBody().push_back(subfunction);
        if (subfunction == kEnableRapidPowerShutDown) {
            response->GetBody().push_back(static_cast< std::uint8_t >(UINT8_MAX));
        }
        if (respondToReset_ == DiagnosticResponseToEcuResetEnum::kRespondAfterReset) {
            std::ignore = context->ReestablishAfterRestarted(requestMessagePtr->GetTA());
            _save(response);
            interface_->ExecuteReset(metaInfo);
        } else {
            context->Respond(std::move(response));
        }

    } else {
        context->Respond(NrcErrc::kConditionsNotCorrect);
        LogWarn() << "ServiceX11::ParseAndResponse| call interface fails! error:" << requestResult.Error();
    }
}

void ServiceX11::_save(std::shared_ptr< Message > const& msgPtr) noexcept
{
    std::string const key{"ServiceX11|Save"};
    std::vector< std::uint8_t > dump{msgPtr->Dump()};
    bool saveResult = persistenceService_->SaveData(key, dump);
    if (saveResult) {
        LogVerbose() << "ServiceX11::Save| success!";
    } else {
        LogWarn() << "ServiceX11::Save| fails!";
    }
}

void ServiceX11::NotifyReestablishment(std::shared_ptr< Channel > const& channel) noexcept
{
    std::string const key{"ServiceX11|Save"};
    std::vector< std::uint8_t > dump{};
    dump = persistenceService_->LoadData(key);
    if (!dump.empty()) {
        Serialize deserialize{dump};
        Address sa{deserialize.ReadUint16()};
        Address ta{deserialize.ReadUint16()};
        MessagePtr respond{std::make_shared< Message >(sa, ta)};
        std::vector< std::uint8_t >& body = respond->GetBody();
        body                              = deserialize.ReadAll();
        std::ignore                       = channel->Respond(respond);
    }
}

void ServiceX11::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    if (status == ResponseStatusType::kCanceled || status == ResponseStatusType::kNoProcessingNoResponse
        || status == ResponseStatusType::kResNegNotOk || status == ResponseStatusType::kResNegOk
        || status == ResponseStatusType::kResNegSuppressed || status == ResponseStatusType::kResPosNotOk) {
        LogWarn() << "ServiceX11::Confirm| request fails! not call ExecuteReset";
        return;
    }

    MessagePtr requestMessagePtr{request.GetRequest()};
    if (requestMessagePtr.get() == nullptr) {
        return;
    }
    Serialize deserialize{requestMessagePtr->GetBody()};
    deserialize.Seek(1);
    std::uint8_t subfunction{deserialize.ReadUint8()};
    if (subfunction != kEnableRapidPowerShutDown || subfunction != kDisableRapidPowerShutDown) {
        decltype(auto) metaInfo{request.GetMetaInfoMap()};
        interface_->ExecuteReset(metaInfo);
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
