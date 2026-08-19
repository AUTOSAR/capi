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
/// @file       idsm_error_domain.h
/// @brief      IDSM error description class
/// @details
/// @date       2022-07-24
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Common
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0001
/// @unit_description=IDSM error description class
/// @endcode
///
/// ================================================================

#ifndef _ARA_CORE_IDSM_ERROR_DOMAIN_H_
#define _ARA_CORE_IDSM_ERROR_DOMAIN_H_

#include <cerrno>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"
#include "ara/core/string.h"

namespace ara {
namespace idsm {
//********************************/
/// @brief Defines the errors for Idsm.
/// The enumeration values 0 - 255 are reserved for AUTOSAR assigned errors,
/// the stack provider is free to define additional errors starting from 256.
/// @code{.isoft}
/// @Function: Intrusion detection error code enumeration
/// @unit_name=IdsmErrorCode
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00052
/// @trace_id_dd=DD_IDSM_00099
/// @needwork = ad
/// @endcode
enum class IdsmErrorCode : ara::core::ErrorDomain::CodeType
{
    kManifestParseError        = 1,
    kManifestKeyRepeat         = 2,
    kManifestKeyNotFound       = 3,
    kIdsmIpcInitError          = 4,
    kIdsmIpcCreateMsgError     = 5,
    kIdsmIpcRequestError       = 6,
    kIdsmIpcNotifyError        = 7,
    kIdsmDidNotFind            = 8,
    kIdsmDidReportModeErr      = 9,
    kIdsmDtcServerWriteNotCall = 10,
    kIdsmTrafficLimitErr       = 11,
    kIdsmCommunicationErr      = 12,
    kIdsmInvalidEventId        = 13,
    kIdsmDidServerOfferErr     = 14,
    kIdsmDtcServerOfferErr     = 15,
    // kIdsmEventToMultiFilterChainErr,
    kIdsmConfigParseErr           = 16,
    kIdsmPortNotEventErr          = 17,
    kIdsmIAMErr                   = 18,
    kIdsmEventMemErr              = 19,
    kIdsmContextMemErr            = 20,
    kIdsmEventNotProperErr        = 21,
    kIdsmEventNotFilterChainErr   = 22,
    kIdsmEventMultiFilterChainErr = 23,
    kIdsmThreadExitSign           = 24,
    kIdsmSuccess                  = 0xff
};

//********************************/
/// @brief : Exception type thrown by Idsm classes.
/// @code{.isoft}
/// @Function: Intrusion detection exception handling class.
/// @unit_name=IdsmException
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00053
/// @trace_id_dd=DD_IDSM_00100
/// @needwork = ad
/// @endcode
class IdsmException : public ara::core::Exception
{
public:
    /// @brief
    /// @param errorCode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00054
    /// @trace_id_dd=DD_IDSM_00101
    /// @needwork = ad
    /// @endcode
    explicit IdsmException(ara::core::ErrorCode errorCode) noexcept : ara::core::Exception{errorCode} {}
};

//********************************/
/// @brief : Defines the error domain for Idsm.
/// @brief : Intrusion detection error handling class.
/// @code{.isoft}
/// @unit_name=IdsmErrorDomain
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00055
/// @trace_id_dd=DD_IDSM_00102
/// @needwork = ad
/// @endcode
class IdsmErrorDomain final : public ara::core::ErrorDomain
{
public:
    /// @brief Redefine char
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00103
    /// @needwork = dda
    /// @endcode
    using char_8 = char;  // NOLINT
    /// @brief : Alias for the error code value enumeration.
    /// @code{.isoft}
    /// @Function: Alias for error code value enumeration.
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00104
    /// @needwork = dda
    /// @endcode
    using Errc = IdsmErrorCode;

    /// @brief : Alias for the error code value enumeration.
    /// @code{.isoft}
    /// @Function: Alias for exception handling class).
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00105
    /// @needwork = dda
    /// @endcode
    using Exception = IdsmException;

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00056
    /// @trace_id_dd=DD_IDSM_00106
    /// @needwork = ad
    /// @endcode
    constexpr IdsmErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief : Returns the name of the error domain.
    /// @returns : The name of the error domain.
    /// @code{.isoft}
    /// @AUTOSAR_SWS [SWS_Idsm_?]
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00057
    /// @trace_id_dd=DD_IDSM_00107
    /// @needwork = ad
    /// @endcode
    char_8 const* Name() const noexcept final { return "Idsm"; }

    /// @brief : Returns the message associated with the error code.
    /// @param errorCode : The error code number.
    /// @returns : The message associated with the error code.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00058
    /// @trace_id_dd=DD_IDSM_00108
    /// @needwork = ad
    /// @endcode
    char_8 const* Message(CodeType errorCode) const noexcept final
    {
        ara::core::String str{""};
        Errc const code{static_cast< Errc >(errorCode)};
        switch (code) {
            case IdsmErrorCode::kIdsmSuccess: {
                str = "Idsm Success.";
                break;
            }
            case IdsmErrorCode::kManifestParseError: {
                str = "manifest json parse error";
                break;
            }
            case IdsmErrorCode::kManifestKeyRepeat: {
                str = "Manifest node key repeat";
                break;
            }
            case IdsmErrorCode::kManifestKeyNotFound: {
                str = "manifest key not found";
                break;
            }
            case IdsmErrorCode::kIdsmIpcCreateMsgError: {
                str = "IPC create message object error";
                break;
            }
            case IdsmErrorCode::kIdsmIpcRequestError: {
                str = "IPC Request error! please check network";
                break;
            }
            case IdsmErrorCode::kIdsmIpcNotifyError: {
                str = "IPC Notify error! please check network";
                break;
            }
            case IdsmErrorCode::kIdsmIpcInitError: {
                str = "IPC init error";
                break;
            }
            case IdsmErrorCode::kIdsmDidNotFind: {
                str = "Idsm DiagDidServer can't find specified did";
                break;
            }
            case IdsmErrorCode::kIdsmDtcServerWriteNotCall: {
                str = "Idsm DiagDtcServer the Method should not be called";
                break;
            }
            case IdsmErrorCode::kIdsmDidReportModeErr: {
                str = "Idsm DiagDidServer set reporting mode invalid";
                break;
            }
            case IdsmErrorCode::kIdsmTrafficLimitErr: {
                str = "Idsm transmit event traffic exceeds limitation";
                break;
            }
            case IdsmErrorCode::kIdsmCommunicationErr: {
                str = "Idsm transmit event communication error";
                break;
            }
            case IdsmErrorCode::kIdsmInvalidEventId: {
                str = "Idsm custom event id is invalid.";
                break;
            }
            case IdsmErrorCode::kIdsmDidServerOfferErr: {
                str = "Idsm did server call diag offer fail";
                break;
            }
            case IdsmErrorCode::kIdsmDtcServerOfferErr: {
                str = "Idsm dtc server call diag offer fail";
                break;
            }
            case IdsmErrorCode::kIdsmConfigParseErr: {
                str = "Idsm parse config fail";
                break;
            }
            case IdsmErrorCode::kIdsmPortNotEventErr: {
                str = "Idsm port not found event definition";
                break;
            }
            case IdsmErrorCode::kIdsmIAMErr: {
                str = "Idsm iam check fail";
                break;
            }
            case IdsmErrorCode::kIdsmEventMemErr: {
                str = "Idsm event memory exceed limitation";
                break;
            }
            case IdsmErrorCode::kIdsmContextMemErr: {
                str = "Idsm context data memory exceed limitation";
                break;
            }
            case IdsmErrorCode::kIdsmEventNotProperErr: {
                str = "Idsm event not found  custom config property";
                break;
            }
            case IdsmErrorCode::kIdsmEventNotFilterChainErr: {
                str = "Idsm event not found custom filter chain";
                break;
            }
            case IdsmErrorCode::kIdsmEventMultiFilterChainErr: {
                str = "Idsm event has one more custom filter chain";
                break;
            }
            case IdsmErrorCode::kIdsmThreadExitSign: {
                str = "Idsm child thread exit sign";
                break;
            }
            default: {
                str = "Unknown error";
                break;
            }
        }
        return str.c_str();
    }

    /// @brief : Throws the exception associated with the error code.
    /// @param errorCode : The error to throw.
    /// @returns : None
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00059
    /// @trace_id_dd=DD_IDSM_00109
    /// @needwork = ad
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }

private:
    /// @brief Key ID for Idsm error domain.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00110
    /// @needwork = dda
    /// @endcode
    static ErrorDomain::IdType const kId{0x8000000000000102U};
};

//********************************/
namespace internal {
/// @code{.isoft}
/// @unit_name=kIdsmErrorDomain
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00060
/// @trace_id_dd=DD_IDSM_00111
/// @needwork = dd
/// @endcode
static IdsmErrorDomain const kIdsmErrorDomain{};
}  // namespace internal

/// @brief : Returns the global IdsmErrorDomain object.
/// @returns : The global IdsmErrorDomain object.
/// @code{.isoft}
/// @AUTOSAR_SWS [SWS_Idsm_?]
/// @threadsafety={re-entrant}
/// @unit_name=GetIdsmDomain
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00060
/// @trace_id_dd=DD_IDSM_00112
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const& GetIdsmDomain() noexcept { return internal::kIdsmErrorDomain; }
/// @brief : Creates an error code.
/// @param code : Error code number.
/// @param data : Vendor defined data associated with the error.
/// @returns :  An ErrorCode object.
/// @code{.isoft}
/// @AUTOSAR_SWS [SWS_Idsm_?]
/// @threadsafety={re-entrant}
/// @unit_name=MakeErrorCode
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00061
/// @trace_id_dd=DD_IDSM_00113
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(IdsmErrorCode const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetIdsmDomain(), data};
}

}  // namespace idsm
}  // namespace ara
#endif  // _ARA_CORE_IDSM_ERROR_DOMAIN_H_