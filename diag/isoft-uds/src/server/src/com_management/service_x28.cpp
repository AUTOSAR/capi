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
/// @file       service_x28.cpp
/// @brief      This file implements service_x28 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=com_management
/// @trace_id_sr=d37ae3a4-9404-475d-9b9a-447a5bf689b8
/// @unit_name=service_x28.h
/// @unit_description=This file provides service_x28 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x28.h"

#include "common/log.h"
#include "common/serialize.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x28 {
std::uint8_t const kMinRequestSize{3U};
std::uint8_t const kTwo{2U};
}  // namespace service_x28

ServiceX28::ServiceX28() noexcept
    : instanceTable_{}
    , authValidator_{nullptr}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
    , interface_{nullptr}
{
}

std::int32_t ServiceX28::Initialize(ComManagementConfig& instanceTableConfig,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    assert(!instanceTableConfig.instanceTable.empty());
    assert(instanceTableConfig.interface.get() != nullptr);
    assert(sessionManagerPtr.get() != nullptr);
    assert(securityLevelManagerPtr.get() != nullptr);
    if (instanceTableConfig.instanceTable.empty()) {
        LogError() << "ServiceX28::Initialize|instanceTableConfig.instanceTable_ is empty!";
        return -1;
    }
    if (instanceTableConfig.interface.get() == nullptr) {
        LogError() << "ServiceX28::Initialize|instanceTableConfig.interface_ is empty!";
        return -1;
    }
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "ServiceX28::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogError() << "ServiceX28::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }
    interface_ = instanceTableConfig.interface;
    for (auto&& instanceConfig : instanceTableConfig.instanceTable) {
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
                LogWarn() << "auth id repeatability! Please check ComControlInstanceConfig.accessPermissionAuth";
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
            LogWarn() << "session id repeatability! Please check ComControlInstanceConfig.accessPermissionSession";
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
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(instanceConfig.subfunctionNumber);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ComControlInstanceConfig.subfunctionNumber:" << instanceConfig.subfunctionNumber
                      << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({instanceConfig.subfunctionNumber, instancePtr});
    }

    p4ServerMax_ = instanceTableConfig.p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX28::GetSidValidator() noexcept
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

std::size_t ServiceX28::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    if (subfunction == kEnableRxAndDisableTxWithEnhancedAddressInformation
        || subfunction == kEnableRxAndTxWithEnhancedAddressInformation) {
        std::size_t const minBody{5U};
        return minBody;
    }
    std::size_t const minBody{3U};
    return minBody;
}

bool ServiceX28::IsSupport(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX28::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogWarn() << "ServiceX28::GetSubfunctionAuth|Please config auth in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX28::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX28::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX28::SupportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX28::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX28::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX28::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        return;
    }
    MessagePtr requestMessagePtr{context->GetRequest()};
    if (requestMessagePtr.get() == nullptr) {
        return;
    }
    Serialize deserialize{requestMessagePtr->GetBody()};
    deserialize.Seek(1);
    std::uint8_t subfunction{deserialize.ReadUint8()};
    /// Validate legal length
    if (subfunction == 4 || subfunction == kInt8_5U) {
        if (requestMessagePtr->GetBody().size() != kInt8_5U) {
            LogWarn() << "ServiceX28::ParseAndResponse| message size must equal 5 byte";
            context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
            return;
        }
    } else {
        if (requestMessagePtr->GetBody().size() != 3) {
            LogWarn() << "ServiceX28::ParseAndResponse| message size must equal 3 byte";
            context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
            return;
        }
    }
    CommunicationControlInterface::ComCtrlRequestParamsType param{};
    param.controlType       = subfunction;
    param.communicationType = deserialize.ReadUint8();
    if (subfunction == kEnableRxAndDisableTxWithEnhancedAddressInformation
        || subfunction == kEnableRxAndTxWithEnhancedAddressInformation) {
        param.nodeIdentificationNumber = deserialize.ReadUint16();
    }
    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    Result< void > requestResult
        = interface_->CommCtrlRequest(param, metaInfo, CancellationHandler(context->GetCancellationState()));
    if (requestResult.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        response->GetBody().push_back(subfunction);
        context->Respond(std::move(response));
    } else {
        LogWarn() << "ServiceX28::ParseAndResponse| call CommCtrlRequest fails! error:"
                  << static_cast< std::uint8_t >(requestResult.Error());
        context->Respond(requestResult.Error());
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
