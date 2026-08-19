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
/// @file       service_x14.cpp
/// @brief      This file implements service_x14 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=d5025337-8827-4698-9619-8ff7730742d2
/// @unit_name=service_x14.h
/// @unit_description=This file provides service_x14 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/service_x14.h"

#include <functional>

#include "common/log.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x14 {
std::uint8_t const kRequestPrimaryMemSize{4U};
std::uint8_t const kRequestUserDefinedMemSize{5U};
std::uint8_t const kTwo{2U};
std::uint8_t const kThree{3U};
}  // namespace service_x14

std::int32_t ServiceX14::Initialize(ClearDiagnosticInformationInstanceConfig& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    if (config.accessPermissionSession.empty()) {
        LogWarn() << "ServiceX14::Initialize|config is empty!";
        return -1;
    }

    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX14::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX14::Initialize|securityLevelManagerPtr is nullptr!";
        return -1;
    }

    std::int32_t result{0};
    if (config.accessPermissionAuth.enable) {
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
            = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
        if (authValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(
            authValidator->Initialize(0U, config.accessPermissionAuth.authRoleList, authManagerPtr));
        if (result != 0) {
            LogWarn() << "auth config initialize fails!";
            return -1;
        }
        if (authValidator_.get() == nullptr) {
            authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
        }
        result = authValidator_->RegisterSubfunctionLevel(0U, authValidator);
        if (result != 0) {
            LogWarn()
                << "auth id repeatability! Please check ClearDiagnosticInformationInstanceConfig.accessPermissionAuth";
            return -1;
        }
    }

    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator
        = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
    if (sessionValidator.get() == nullptr) {
        LogWarn() << "ServiceX14::Initialize|allow memory fails in new SubfunctionLevelSessionValidator instance";
        return -1;
    }

    result = static_cast< int32_t >(sessionValidator->Initialize(0, config.accessPermissionSession, sessionManagerPtr));
    if (result != 0) {
        LogWarn() << "ServiceX14::Initialize|session config initialize fails result =" << result;
        return -1;
    }

    sessionValidator_ = std::make_shared< ServerLevelSessionValidator< std::uint8_t > >();
    result            = sessionValidator_->RegisterSubfunctionLevel(0, sessionValidator);
    if (result != 0) {
        LogWarn() << "ServiceX14::Initialize|session id repeatability Please check accessPermissionSession";
        return -1;
    }

    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator
        = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
    if (securityLevelValidator.get() == nullptr) {
        LogWarn() << "ServiceX14::Initialize|allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
        return -1;
    }
    result = static_cast< int32_t >(
        securityLevelValidator->Initialize(0, config.accessPermissionSecurityLevel, securityLevelManagerPtr));
    if (result != 0) {
        LogWarn() << "ServiceX14::Initialize|security level config initialize fails result =" << result;
        return -1;
    }

    securityLevelValidator_ = std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >();
    result                  = securityLevelValidator_->RegisterSubfunctionLevel(0, securityLevelValidator);
    if (result != 0) {
        LogWarn() << "ServiceX14::Initialize|security level id repeatability! Please check "
                     "accessPermissionSecurityLevel result ="
                  << result;
        return -1;
    }
    p4ServerMax_ = config.p4ServerMax;
    LogVerbose() << "ServiceX14::Initialize|success";
    return 0;
}

std::vector< ValidatorPtr > ServiceX14::GetSidValidator() noexcept
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

void ServiceX14::Process(std::shared_ptr< RequestContext > context) noexcept
{
    decltype(auto) request{context->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const requestSize{requestBody.size()};

    ///  [SWS_DM_00098]
    if ((requestSize != service_x14::kRequestPrimaryMemSize)
        && (requestSize != service_x14::kRequestUserDefinedMemSize)) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX14::Process|kIncorrectMessageLengthOrInvalidFormat";
        return;
    }

    std::uint32_t dtc{requestBody[1U]};
    dtc = (dtc << CHAR_BIT) | static_cast< std::uint32_t >(requestBody[service_x14::kTwo]);
    dtc = std::move((dtc << CHAR_BIT) | static_cast< std::uint32_t >(requestBody[service_x14::kThree]));

    bool expected{false};
    if (!inProgress_.compare_exchange_strong(expected, true)) {
        ///  [SWS_DM_00114]
        context->Respond(NrcErrc::kConditionsNotCorrect);
        LogWarn() << "ServiceX14::Process|0x14 is running";
        return;
    }

    int32_t const result = faultManager_.ClearDTC(dtc, requestSize == 5U, *requestBody.rbegin());
    if (result == 0) {
        ///  [SWS_DM_00113]
        ///  [SWS_DM_00162]
        std::shared_ptr< Message > response{context->CreateResponse()};
        context->Respond(std::move(response));
    } else {
        context->Respond(static_cast< NrcErrc >(result));
        LogWarn() << "ServiceX14::Process|FaultManager::ClearDTC|" << result;
    }
    inProgress_ = false;
    std::ignore = expected;
    LogDebug() << "ServiceX14::Process|process success dtc =" << dtc;
}

void ServiceX14::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    std::ignore = request;
    std::ignore = status;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
