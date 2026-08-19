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
/// @file       service_x85.cpp
/// @brief      This file implements service_x85 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=4eb17183-0cdb-4110-a074-57423601826f
/// @unit_name=service_x85.h
/// @unit_description=This file provides service_x85 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x85.h"

#include <functional>

#include "common/log.h"
#include "common/serialize.h"
#include "fault_management/fault_manager.h"
#include "isoft/uds/service_validation.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x85 {
std::uint8_t const kMinBodySize{2U};
}  // namespace service_x85

std::int32_t ServiceX85::Initialize(ControlDTCSettingInstanceConfig& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    if (config.instances.empty()) {
        LogWarn() << "ServiceX85::Initialize|config is empty!";
        return -1;
    }

    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX85::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX85::Initialize|securityLevelManagerPtr is nullptr!";
        return -1;
    }
    sessionValidator_       = std::make_shared< ServerLevelSessionValidator< std::uint8_t > >();
    securityLevelValidator_ = std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >();
    for (auto const& instanceConfig : config.instances) {
        std::shared_ptr< Instance > instancePtr = std::make_shared< Instance >();
        if (instancePtr.get() == nullptr) {
            LogWarn() << "ServiceX85::Initialize|allow memory fails in new struct Instance";
            return -1;
        }

        instancePtr->subfunctionNumber = static_cast< std::uint8_t >(instanceConfig.subfunctionNumber);
        std::int32_t result{0};
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(
                authValidator->Initialize(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                          instanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                              authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check ControlDTCSettingSubfunction.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "ServiceX85::Initialize|allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }

        result = static_cast< int32_t >(
            instancePtr->sessionValidator->Initialize(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                      instanceConfig.accessPermissionSession, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "ServiceX85::Initialize|session config initialize fails result =" << result;
            return -1;
        }

        result = sessionValidator_->RegisterSubfunctionLevel(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                             instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn() << "ServiceX85::Initialize|session id repeatability! Please check "
                         "ControlDTCSettingInstanceConfig.accessPermissionSession result ="
                      << result;
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "ServiceX85::Initialize|allow memory fails in new SubfunctionLevelSecurityLevelValidator "
                         "instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            static_cast< uint8_t >(instanceConfig.subfunctionNumber), instanceConfig.accessPermissionSecurityLevel,
            securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "ServiceX85::Initialize|security level config initialize fails!";
            return -1;
        }

        result = securityLevelValidator_->RegisterSubfunctionLevel(
            static_cast< uint8_t >(instanceConfig.subfunctionNumber), instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "ServiceX85::Initialize|security level id repeatability! Please check "
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(static_cast< uint8_t >(instanceConfig.subfunctionNumber));
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ServiceX85::Initialize|ControlDTCSettingInstanceConfig.subfunctionNumber:"
                      << static_cast< uint8_t >(instanceConfig.subfunctionNumber) << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({static_cast< uint8_t >(instanceConfig.subfunctionNumber), instancePtr});
    }
    p4ServerMax_ = config.p4ServerMax;
    LogVerbose() << "ServiceX85::Initialize|success";
    return 0;
}

std::vector< ValidatorPtr > ServiceX85::GetSidValidator() noexcept
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

std::size_t ServiceX85::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    std::ignore = subfunction;
    return service_x85::kMinBodySize;
}

bool ServiceX85::IsSupport(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX85::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX85::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX85::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX85::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX85::SupportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX85::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX85::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX85::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX85::Process|context is nullptr";
        return;
    }

    decltype(auto) request{context->GetRequest()};
    if (request.get() == nullptr) {
        LogWarn() << "ServiceX85::Process|request is nullptr";
        return;
    }
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const requestSize{requestBody.size()};

    ///  [SWS_DM_00098]
    if (requestSize < service_x85::kMinBodySize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX85::Process|kIncorrectMessageLengthOrInvalidFormat";
        return;
    }

    ///  [SWS_DM_00231]
    for (std::size_t i{2U}; i < requestSize; ++i) {
        if (requestBody[i] != static_cast< uint8_t >(UINT8_MAX)) {
            context->Respond(NrcErrc::kRequestOutOfRange);
            LogWarn() << "ServiceX85::Process|kRequestOutOfRange";
            return;
        }
    }

    Serialize deserialize{requestBody};
    deserialize.Seek(1);
    std::uint8_t subfunction{deserialize.ReadUint8()};

    bool const on{subfunction == static_cast< std::uint8_t >(ControlDTCSettingSubfunction::kOn)};
    int32_t const result = faultManager_.ControlDTCSetting(on);
    if (result == 0) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        response->GetBody().push_back(subfunction);
        context->Respond(std::move(response));
    } else {
        context->Respond(NrcErrc::kRequestOutOfRange);
        LogWarn() << "ServiceX85::Process|EventManager::ControlDTCSetting" << result;
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
