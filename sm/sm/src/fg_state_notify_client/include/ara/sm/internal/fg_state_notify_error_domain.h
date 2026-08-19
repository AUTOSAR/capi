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
/// @file       fg_state_notify_error_domain.h
/// @brief      Define communication error types
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyClient
/// @unit_name=FGStateNotifyError
/// @interface_level=software
/// @unit_description=Define communication error types
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_FG_STATE_NOTIFY_ERROR_DOMAIN_H_
#define ARA_SM_INTERNAL_FG_STATE_NOTIFY_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include <cerrno>

namespace ara {
namespace sm {
namespace fg_state_notify_client {

/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using ErrorDomain = core::ErrorDomain;
/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using Exception = core::Exception;
/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using ErrorCode = core::ErrorCode;

/// @brief An enumeration with errors that can occur within this Functional Cluster
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08021
/// @trace_id_dd=DD_SM_08021
/// @needwork = ad
/// @endcode
enum class FGStateNotifyErrc : ErrorDomain::CodeType
{
    kConnectionRefused     = -1,    ///< connection refused
    kTimeout               = -2,    ///< time out
    kNotInited             = -3,    ///< not inited
    kCanNotCreateMSG       = -4,    ///< can not create msg
    kCanNotAppendBuffer    = -5,    ///< can not append buffer
    kAlreadyInited         = -11,   ///< already inited
    kCanNotGetEVLoop       = -12,   ///< can not get ev loop
    kCanNotInitNai         = -13,   ///< can not init nai socket
    kCanNotCreateIPCClient = -14,   ///< can not create IPC client
    kCanNotStartIPCClient  = -15,   ///< can not start IPC client
    kAlreadySubscribed     = -21,   ///< already subscribed
    kNotAlreadySubscribed  = -31,   ///< not already subscribed
    kCanNotFindServer      = -32,   ///< can not find server
    kCanNotMalloc          = -33,   ///< can not malloc
    kOthers                = -100,  ///< other errors
};

/// @brief Exception type thrown for CORE errors.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08022
/// @trace_id_dd=DD_SM_08022
/// @needwork = ad
/// @endcode
class FGStateNotifyException : public Exception
{
public:
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @needwork = no
    /// @endcode
    using Exception::Exception;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08002
    /// @trace_id_dd=DD_SM_08002
    /// @needwork = ad
    /// @endcode
    ~FGStateNotifyException() final = default;
    /// @brief Copy constructor
    /// @param other The FGInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08003
    /// @trace_id_dd=DD_SM_08003
    /// @needwork = ad
    /// @endcode
    FGStateNotifyException(FGStateNotifyException const& other) = default;
    /// @brief Move copy
    /// @param other The FGInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08004
    /// @trace_id_dd=DD_SM_08004
    /// @needwork = ad
    /// @endcode
    FGStateNotifyException(FGStateNotifyException&& other) = default;
    /// @brief Assignment function
    /// @param other The FGInfoConfig instance to be moved
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08005
    /// @trace_id_dd=DD_SM_08005
    /// @needwork = ad
    /// @endcode
    FGStateNotifyException& operator=(FGStateNotifyException const& other) = delete;
    /// @brief Move assignment function
    /// @param other The FGInfoConfig instance to be moved
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08006
    /// @trace_id_dd=DD_SM_08006
    /// @needwork = ad
    /// @endcode
    FGStateNotifyException& operator=(FGStateNotifyException&& other) = delete;
};

/// @brief An error domain for ara::core errors.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08023
/// @trace_id_dd=DD_SM_08023
/// @needwork = ad
/// @endcode
class FGStateNotifyErrorDomain final : public ErrorDomain
{
    /// @brief Pre-declared ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @needwork = no
    /// @endcode
    constexpr static ErrorDomain::IdType kId{0x8000000000000014UL};

public:
    /// @brief Alias for the error code value enumeration
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @needwork = no
    /// @endcode
    using Errc = FGStateNotifyErrc;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08007
    /// @trace_id_dd=DD_SM_08007
    /// @needwork = ad
    /// @endcode
    constexpr FGStateNotifyErrorDomain() noexcept : ErrorDomain{kId} {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    /// @returns "FGStateNotify"
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08008
    /// @trace_id_dd=DD_SM_08008
    /// @needwork = ad
    /// @endcode
    char const* Name() const noexcept final { return "FGStateNotify"; }

    /// @brief Translate an error code value into a text message.
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08009
    /// @trace_id_dd=DD_SM_08009
    /// @needwork = ad
    /// @endcode
    char const* Message(ErrorDomain::CodeType errorCode) const noexcept final
    {
        char const* str{nullptr};
        Errc const code{static_cast< Errc >(errorCode)};
        switch (code) {
            case Errc::kConnectionRefused: {
                str = "Connection refused";
            } break;
            case Errc::kTimeout: {
                str = "Time out";
            } break;
            case Errc::kNotInited: {
                str = "Not inited";
            } break;
            case Errc::kCanNotCreateMSG: {
                str = "Can not create msg";
            } break;
            case Errc::kCanNotAppendBuffer: {
                str = "Time out";
            } break;
            case Errc::kAlreadyInited: {
                str = "Already inited";
            } break;
            case Errc::kCanNotGetEVLoop: {
                str = "Can not get ev loop";
            } break;
            case Errc::kCanNotInitNai: {
                str = "Can not init nai socket";
            } break;
            case Errc::kCanNotCreateIPCClient: {
                str = "Can not create IPC client";
            } break;
            case Errc::kCanNotStartIPCClient: {
                str = "Time out";
            } break;
            case Errc::kAlreadySubscribed: {
                str = "Already subscribed";
            } break;
            case Errc::kNotAlreadySubscribed: {
                str = "Not already subscribed";
            } break;
            case Errc::kCanNotFindServer: {
                str = "Can not find server";
            } break;
            case Errc::kCanNotMalloc: {
                str = "Can not malloc";
            } break;
            case Errc::kOthers: {
                str = "Other errors";
            } break;
            default: {
                str = "Unknown error";
            } break;
        }
        return str;
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @param errorCode  the ErrorCode instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08010
    /// @trace_id_dd=DD_SM_08010
    /// @needwork = ad
    /// @endcode
    void ThrowAsException(ErrorCode const& errorCode) const noexcept(false) final
    {
        core::internal::ThrowOrTerminate< FGStateNotifyException >(errorCode);
    }
};

namespace internal {

/// @brief Core error domain
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08011
/// @trace_id_dd=DD_SM_08024
/// @needwork = dd
/// @endcode
constexpr FGStateNotifyErrorDomain kCoreErrorDomain;
}  // namespace internal

/// @brief Return a reference to the global FGStateNotifyErrorDomain.
/// @returns the FGStateNotifyErrorDomain
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08011
/// @trace_id_dd=DD_SM_08011
/// @needwork = ad
/// @endcode
constexpr ErrorDomain const& GetFGStateNotifyErrorDomain() noexcept { return internal::kCoreErrorDomain; }

/// @brief Create a new ErrorCode within FGStateNotifyErrorDomain.
/// This function is used internally by constructors of ErrorCode. It is usually not
/// used directly by users.
/// @param code  the FGStateNotifyErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @returns a new ErrorCode instance
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08012
/// @trace_id_dd=DD_SM_08012
/// @needwork = ad
/// @endcode
constexpr ErrorCode MakeErrorCode(FGStateNotifyErrc const code, ErrorDomain::SupportDataType const data) noexcept
{
    return ErrorCode(static_cast< ErrorDomain::CodeType >(code), GetFGStateNotifyErrorDomain(), data);
}

}  // namespace fg_state_notify_client
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_INTERNAL_FG_STATE_NOTIFY_ERROR_DOMAIN_H_
