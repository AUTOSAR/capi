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
/// @file       service_x10.cpp
/// @brief      This file implements service_x10 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=session_management
/// @trace_id_sr=e92eaaab-66c1-458b-9027-28efb734f324
/// @unit_name=service_x10.h
/// @unit_description=This file provides service_x10 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x10.h"

#include <functional>

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/type_int.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {



ServiceX10::ServiceX10() noexcept
    : serverLevelSessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , serverLevelSecurityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
{
}

std::int32_t ServiceX10::Initialize(std::set< SessionControlInstanceConfig >& table,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr,
                                    std::uint32_t const p4ServerMax) noexcept
{
    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX10|Please Init Session Manager Instance!";
        return -1;
    }
    diagnosticSessionManagerPtr_ = sessionManagerPtr;
    for (auto&& config : table) {
        std::shared_ptr< Instance > instancePtr{std::make_shared< Instance >()};
        instancePtr->id = sessionManagerPtr->GetSession(config.sesssionIdConfig);
        if (instancePtr->id == nullptr) {
            LogWarn() << "ServiceX10| SessionControlInstanceConfig->sesssionIdConfig:" << config.sesssionIdConfig
                      << " is invalid! Please check SessionControlInstanceConfig->sesssionIdConfig config!";
            return -1;
        }
        std::int32_t result{0U};
        if (config.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(authValidator->Initialize(
                config.sesssionIdConfig, config.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(config.sesssionIdConfig, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check SecurityAccessInstanceConfig.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }

        std::shared_ptr< SubfunctionLevelSessionValidator< SessionId > > subfunctionSession{
            std::make_shared< SubfunctionLevelSessionValidator< SessionId > >()};
        result = static_cast< int32_t >(
            subfunctionSession->Initialize(config.sesssionIdConfig, config.sessionPermission, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        instancePtr->sessionValidator = subfunctionSession;
        result = serverLevelSessionValidator_->RegisterSubfunctionLevel(config.sesssionIdConfig, subfunctionSession);
        if (result != 0) {
            LogWarn() << "session control instance duplication";
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            config.sesssionIdConfig, config.securityLevelPermission, securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result = serverLevelSecurityLevelValidator_->RegisterSubfunctionLevel(config.sesssionIdConfig,
                                                                              instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "RoutineControlInstanceConfig.securityLevelPermission";
            return -1;
        }

        std::ignore = instanceTable_.insert({config.sesssionIdConfig, instancePtr});
    }
    p4ServerMax_ = p4ServerMax;
    return 0;
}

std::size_t ServiceX10::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    std::size_t const minSize{2U};
    std::ignore = subfunction;
    return minSize;
}

bool ServiceX10::IsSupport(std::uint8_t subfunction) noexcept
{
    auto findInstanceResult = instanceTable_.find(subfunction);
    if (findInstanceResult == instanceTable_.end()) {
        return false;
    }
    return true;
}

std::shared_ptr< Validator > ServiceX10::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX10::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX10::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    auto findInstanceResult = instanceTable_.find(subfunction);
    if (findInstanceResult == instanceTable_.end()) {
        return nullptr;
    }
    return findInstanceResult->second->sessionValidator;
}

bool ServiceX10::SupportSecurityLevel() noexcept { return false; }

std::shared_ptr< Validator > ServiceX10::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    std::ignore = subfunction;
    return nullptr;
}

void ServiceX10::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    std::shared_ptr< Message > request{context->GetRequest()};
    if (request->GetBody().size() != 2) {
        LogWarn() << "ServiceX10::ParseAndResponse| message size must equal 2 bytes";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    /// @brief serialize function
    
    Serialize serialize(request->GetBody());
    serialize.Seek(1U);
    std::uint8_t subfunction = serialize.ReadUint8();
    std::uint8_t const kSubfunctionMask{0x7FU};
    SessionId newSession    = subfunction & kSubfunctionMask;
    auto findInstanceResult = instanceTable_.find(newSession);
    if ((findInstanceResult == instanceTable_.end()) || findInstanceResult->second.get() == nullptr) {
        LogWarn() << "ServiceX10::ParseAndResponse| not found instance,instance id:" << newSession;
        context->Respond(NrcErrc::kRequestOutOfRange);
        return;
    }
    if (findInstanceResult->second->id == nullptr) {
        LogWarn() << "ServiceX10::ParseAndResponse| not found instance config,instance id:" << newSession;
        context->Respond(NrcErrc::kRequestOutOfRange);
        return;
    }
    newSessionId_ = newSession;
    std::shared_ptr< Message > response{context->CreateResponse()};
    /// @brief deserialize function
    
    Serialize deserialize(response->GetBody());
    deserialize.Write(newSession);
    deserialize.Write(findInstanceResult->second->id->GetP2ServerMax());
    deserialize.Write(findInstanceResult->second->id->GetP2StarServerMax());
    context->Respond(std::move(response));
}

void ServiceX10::Confirm(RequestContext& requestContext, ResponseStatusType status) noexcept
{
    if (status == ResponseStatusType::kResPosOk || status == ResponseStatusType::kResPosNotOk) {
        Address client{requestContext.GetRequest()->GetSA()};
        LogInfo() << "ServiceX10::Confirm| change session:" << newSessionId_;
        diagnosticSessionManagerPtr_->SetSession(newSessionId_, client);
    }
}

/// @brief Get session validator pointer
/// @return Session validator pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_00888
/// @trace_id_dd=DD_UDS_01091
/// @needwork = dd
/// @endcode
std::vector< ValidatorPtr > ServiceX10::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    if (authValidator_.get() != nullptr) {
        validatorTable.emplace_back(authValidator_);
    }
    if (serverLevelSessionValidator_.get() != nullptr) {
        validatorTable.emplace_back(serverLevelSessionValidator_);
    }
    if (serverLevelSecurityLevelValidator_.get() != nullptr) {
        validatorTable.emplace_back(serverLevelSecurityLevelValidator_);
    }
    return validatorTable;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
