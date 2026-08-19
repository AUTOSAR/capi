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
/// @file       service_custom.cpp
/// @brief      This file implements service_custom functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=custom_management
/// @trace_id_sr=cbe021ad-8b8d-41d3-9c0d-5faf0ec843ce
/// @unit_name=ServiceCustom
/// @unit_description=This file provides service_custom functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_custom.h"

#include "common/log.h"
#include "common/serialize.h"

namespace isoft {
namespace uds {
namespace server {

namespace service_x3e {
std::uint8_t const kBodySize{2U};
}  // namespace service_x3e

ServiceCustom::ServiceCustom() noexcept
    : authValidator_{std::make_shared< ServerLevelAuthValidator< std::uint8_t > >()}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
    , interface_{nullptr}
{
}

std::int32_t ServiceCustom::Initialize(CustomInstance const& config,
                                       std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                       std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                       std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    std::uint8_t const sessionControl{0x10};
    std::uint8_t const ecuReset{0x11};
    std::uint8_t const clearDtc{0x14};
    std::uint8_t const readDtc{0x19};
    std::uint8_t const readDid{0x22};
    std::uint8_t const writeDid{0x2E};
    std::uint8_t const securityAccess{0x27};
    std::uint8_t const comControl{0x28};
    std::uint8_t const definedDDid{0x2C};
    std::uint8_t const readPDid{0x2A};
    std::uint8_t const routineControl{0x31};
    std::uint8_t const download{0x34};
    std::uint8_t const upload{0x35};
    std::uint8_t const transfer{0x36};
    std::uint8_t const transferExit{0x37};
    std::uint8_t const fileTransfer{0x38};
    std::uint8_t const testerPresent{0x3E};
    std::uint8_t const controlDtcSetting{0x85};
    std::uint8_t const responseOnEvent{0x86};

    std::set< std::uint8_t > const invalidSidTable{
        sessionControl, ecuReset,     clearDtc,      readDtc,           readDid,        writeDid, securityAccess,
        comControl,     definedDDid,  readPDid,      routineControl,    download,       upload,   transfer,
        transferExit,   fileTransfer, testerPresent, controlDtcSetting, responseOnEvent};
    auto findSidResult = invalidSidTable.find(config.nSid);
    if (findSidResult != invalidSidTable.end()) {
        LogWarn() << "ServiceCustom::Initialize| The sid is a built-in service and cannot be used! sid:" << config.nSid;
    }
    std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
        = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
    if (authValidator.get() == nullptr) {
        LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
        return -1;
    }
    std::int32_t result{0};
    if (config.accessPermissionAuth.enable) {
        result = static_cast< int32_t >(
            authValidator->Initialize(config.nSid, config.accessPermissionAuth.authRoleList, authManagerPtr));
        if (result != 0) {
            LogWarn() << "auth config initialize fails!";
            return -1;
        }
        result = authValidator_->RegisterSubfunctionLevel(config.nSid, authValidator);
        if (result != 0) {
            LogWarn() << "auth id repeatability! Please check ComControlInstanceConfig.accessPermissionAuth";
            return -1;
        }
    } else {
        authValidator_.reset();
    }

    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator
        = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
    if (sessionValidator.get() == nullptr) {
        LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
        return -1;
    }
    result = static_cast< int32_t >(
        sessionValidator->Initialize(config.nSid, config.accessPermissionSession, sessionManagerPtr));
    if (result != 0) {
        LogWarn() << "session config initialize fails!";
        return -1;
    }
    result = sessionValidator_->RegisterSubfunctionLevel(config.nSid, sessionValidator);
    if (result != 0) {
        LogWarn() << "session id repeatability! Please check ComControlInstanceConfig.accessPermissionSession";
        return -1;
    }

    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator
        = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
    if (securityLevelValidator.get() == nullptr) {
        LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
        return -1;
    }
    result = static_cast< int32_t >(
        securityLevelValidator->Initialize(config.nSid, config.accessPermissionSecurityLevel, securityLevelManagerPtr));
    if (result != 0) {
        LogWarn() << "security level config initialize fails!";
        return -1;
    }
    result = securityLevelValidator_->RegisterSubfunctionLevel(config.nSid, securityLevelValidator);
    if (result != 0) {
        LogWarn() << "security level id repeatability! Please check "
                     "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
        return -1;
    }

    interface_   = config.interfacePtr;
    p4ServerMax_ = config.p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceCustom::GetSidValidator() noexcept
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

void ServiceCustom::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogError() << "ServiceCustom::Process|context is nullptr";
        return;
    }
    std::uint8_t const sid{context->GetSid()};
    if (sid == kTesterPresentId) {
        _process3EService(context);
    } else {
        _processOtherService(context);
    }
}

void ServiceCustom::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    std::ignore = request;
    std::ignore = status;
}


void ServiceCustom::_process3EService(std::shared_ptr< RequestContext > const& context)
{
    decltype(auto) request{context->GetRequest()};
    Serialize deserialize{request->GetBody()};

    ///  [SWS_DM_00098]
    size_t udsSize = deserialize.Size();
    if (udsSize != service_x3e::kBodySize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogError() << "ServiceCustom::_process3EService|kIncorrectMessageLengthOrInvalidFormat udsSize =" << udsSize;
        return;
    }

    std::ignore = deserialize.ReadUint8();
    std::uint8_t const subfuction{deserialize.ReadUint8()};
    std::uint8_t const kSubfunctionMask{0x7FU};
    if ((subfuction & kSubfunctionMask) != 0U) {
        context->Respond(NrcErrc::kSubfunctionNotSupported);
        LogError() << "ServiceCustom::_process3EService|kSubfunctionNotSupported";
        return;
    }

    std::shared_ptr< Message > response{context->CreateResponse()};
    std::ignore = response->GetBody().insert(response->GetBody().cend(), subfuction);
    context->Respond(std::move(response));
    LogError() << "ServiceCustom::_process3EService|process success subfunction =" << subfuction;
}


void ServiceCustom::_processOtherService(std::shared_ptr< RequestContext > const& context)
{
    decltype(auto) request{context->GetRequest()};
    Serialize deserialize{request->GetBody()};
    std::uint8_t const sid{deserialize.ReadUint8()};
    std::vector< std::uint8_t > const record{deserialize.ReadAll()};

    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    Result< GenericUDSServiceInterface::OperationOutput > result
        = interface_->HandleMessage(sid, record, metaInfo, CancellationHandler(context->GetCancellationState()));
    if (result.HasValue()) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        decltype(auto) responseData{result.Value().responseData};
        std::ignore = response->GetBody().insert(response->GetBody().cend(), responseData.begin(), responseData.end());
        context->Respond(std::move(response));
    } else {
        context->Respond(result.Error());
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
