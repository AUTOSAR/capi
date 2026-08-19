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
/// @file       general_service.cpp
/// @brief      This file implements general_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=common
/// @trace_id_sr=939abe32-1901-40cf-8013-e8b5074131c5
/// @unit_name=GeneralService
/// @unit_description=This file provides general_service functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "common/general_service.h"

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/uds_nrc_error_domain.h"
namespace isoft {
namespace uds {
namespace server {
void GeneralService::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    LogVerbose() << "GeneralService::Confirm| RequestHandle:" << request.GetMetaInfoMap()["kRequestHandle"]
                 << "response status:" << EnumToString(status);
}


void GeneralService::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "GeneralService::Process| context is nullptr";
    }
    decltype(auto) request{context->GetRequest()};
    if (request.get() == nullptr) {
        LogWarn() << "GeneralService::Process| context->Request is nullptr";
    }

    ///  [SWS_DM_00098]
    Serialize deserialize{request->GetBody()};
    if (2 > request->GetBody().size()) {
        LogWarn() << "GeneralService::Process| message length must > 2 bytes";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::uint8_t sid{deserialize.ReadUint8()};
    std::ignore = sid;
    std::uint8_t subfunction{deserialize.ReadUint8()};
    std::uint8_t const subfunctionMask{0x7FU};
    subfunction             = subfunction & subfunctionMask;
    std::size_t mininumSize = GetMinimumSize(subfunction);
    if (mininumSize > request->GetBody().size()) {
        LogWarn() << "GeneralService::Process| message length too short,sid:" << LogHex16{sid};
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }

    bool checkSubfunctionSupportedResult = IsSupport(subfunction);
    if (!checkSubfunctionSupportedResult) {
        LogWarn() << "GeneralService::Process| subfunction not support,sid:" << LogHex16{sid}
                  << " subfunction:" << subfunction;
        context->Respond(NrcErrc::kSubfunctionNotSupported);
        return;
    }
    std::shared_ptr< Validator > subfunctionAuthValidator = GetSubfunctionAuth(subfunction);
    if (subfunctionAuthValidator.get() != nullptr && !subfunctionAuthValidator->Validate(context)) {
        LogWarn() << "GeneralService::Process| subfunction not support in auth,sid:" << LogHex16{sid}
                  << " subfunction:" << subfunction;
        context->Respond(NrcErrc::kAuthenticationRequired);
        return;
    }
    std::shared_ptr< Validator > subfunctionSessionValidator = GetSubfunctionSession(subfunction);
    if (subfunctionSessionValidator.get() == nullptr || !subfunctionSessionValidator->Validate(context)) {
        LogWarn() << "GeneralService::Process| subfunction not support in current session,sid:" << LogHex16{sid}
                  << " subfunction:" << subfunction;
        context->Respond(NrcErrc::kSubFunctionNotSupportedInActiveSession);
        return;
    }
    bool supportSecurityLevel{false};
    supportSecurityLevel = SupportSecurityLevel();
    if (supportSecurityLevel) {
        std::shared_ptr< Validator > subfunctionSecurityLevelValidator = GetSubfunctionSecurityLevel(subfunction);
        if (subfunctionSecurityLevelValidator.get() != nullptr
            && !subfunctionSecurityLevelValidator->Validate(context)) {
            LogWarn() << "GeneralService::Process| subfunction not support in current security level,sid:"
                      << HexFormat(sid) << " subfunction:" << subfunction;
            context->Respond(NrcErrc::kSecurityAccessDenied);
            return;
        }
    }


    return ParseAndResponse(context);
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
