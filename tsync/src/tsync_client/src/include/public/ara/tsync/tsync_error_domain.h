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
/// @file       tsync_error_domain.h
/// @brief      time synchronization error domain
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef _ARA_TSYNC_ERROR_DOAMIN_H_
#define _ARA_TSYNC_ERROR_DOAMIN_H_

#include "ara/core/exception.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace ara {
namespace tsync {

/// @brief  time synchronization error code enumeration
enum class TsyncErrc : ara::core::ErrorDomain::CodeType
{
    /// @brief unable to set.
    kTimeCannotSet = 1,
    /// @brief action cannot be performed because the connection to the time synchronization service is lost.
    kDaemonConnectionLost,
    /// @brief severely out of limits
    kLimitsExceeded,
};

/// @brief  time synchronization exception definition
class TsyncException : public ara::core::Exception
{
public:
    /// @brief constructor
    /// @param errCode - error code
    explicit TsyncException(ara::core::ErrorCode errCode) noexcept : ara::core::Exception{std::move(errCode)} {}
};  // class TsyncException

/// @brief TsyncErrorDomain
class TsyncErrorDomain final : public ara::core::ErrorDomain
{
public:
    /// @brief : Alias for the error code value enumeration.
    using Errc = TsyncErrc;

    /// @brief : Alias for the error code value enumeration.
    using Exception = TsyncException;

    /// @brief : Creates a TsyncErrorDomain instance.
    TsyncErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief : Returns the name of the error domain.
    /// @returns : The name of the error domain.
    const char* Name() const noexcept final { return "Tsync"; }

    /// @brief : Returns the message associated with the error code.
    /// @param errorCode : The error code number.
    /// @returns : The message associated with the error code.
    const char* Message(CodeType errorCode) const noexcept final
    {
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
#endif
        char const* msg{nullptr};
        Errc const code{static_cast< Errc >(errorCode)};
        switch (code) {
            case TsyncErrc::kTimeCannotSet: {
                msg = "the action cannot be executed, because the connection to time sync daemon is currently lost.";
                break;
            }
            case TsyncErrc::kDaemonConnectionLost: {
                msg = "the connection to time sync daemon is currently lost.";
                break;
            }
            case TsyncErrc::kLimitsExceeded: {
                msg = "the value is out of limits exceedingly.";
                break;
            }
            default: {
                msg = "Unknown error";
                break;
            }
        }
        return msg;
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
    }

    /// @brief : Throws the exception associated with the error code.
    /// @param errorCode : The error to throw.
    /// @returns : None
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }

private:
    /// @brief Key ID for tsync error domain.
    static ErrorDomain::IdType const kId{0x8000000000000200};
};
//********************************/
namespace internal {
/// @brief error domain declaration.
static TsyncErrorDomain const kTsyncErrorDomain;  // NOLINT
}  // namespace internal

/// @brief : Returns the global TsyncErrorDomain object.
/// @returns : The global TsyncErrorDomain object.
constexpr ara::core::ErrorDomain const& GetTsyncDomain() noexcept { return internal::kTsyncErrorDomain; }

/// @brief : Creates an error code.
/// @param code : Error code number.
/// @param data : Vendor defined data associated with the error.
/// @returns :  An ErrorCode object.
constexpr ara::core::ErrorCode MakeErrorCode(TsyncErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetTsyncDomain(), data);
}

}  // namespace tsync
}  // namespace ara

#endif  //_ARA_TSYNC_ERROR_DOAMIN_H_
