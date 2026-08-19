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
/// @file       core_error_domain.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_CORE_CORE_ERROR_DOMAIN_H_
#define ISOFT_CORE_CORE_ERROR_DOMAIN_H_

#include <cerrno>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace isoft {
namespace core {

/// @brief An enumeration with errors that can occur within this Functional Cluster
///

enum class CoreErrc : ara::core::ErrorDomain::CodeType
{
    kFailure = -1, ///< Operation failed, error_data shall not be used to distinguish error types
    ///< If error types need to be distinguished, the correct way is to define a new error code. If there are more than one error codes in the same submodule, a new error domain should be customized)
};

/// @brief Exception type thrown for CORE errors.
///

class CoreException : public ara::core::Exception
{
public:
    /**
     * @brief Construct a new CoreException from an ErrorCode.
     * @param err  the ErrorCode
     *
     * @uptrace{SWS_CORE_05212, f31d7abbdf343ee1ae8a11ec3a3d385cbbeb36da}
     */
    explicit CoreException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief An error domain for isoft::core errors.
///

class CoreErrorDomain final : public ara::core::ErrorDomain
{
    constexpr static ara::core::ErrorDomain::IdType kId = 0xC000000000000001;

public:
    /// @brief Alias for the error code value enumeration
    ///

    using Errc = CoreErrc;

    /// @brief Alias for the exception base class
    ///

    using Exception = CoreException;

    /// @brief Default constructor
    ///

    constexpr CoreErrorDomain() noexcept : ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    /// @returns "Core"
    ///

    char const* Name() const noexcept override { return "Core"; }

    /// @brief Translate an error code value into a text message.
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    ///

    char const* Message(ErrorDomain::CodeType errorCode) const noexcept override
    {
        Errc const code = static_cast< Errc >(errorCode);
        if (Errc::kFailure == code) {
            return "Operation failed";
        }
        return "Unknown error";
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @param errorCode  the ErrorCode instance
    ///

    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace internal {
constexpr CoreErrorDomain kCoreErrorDomain;
}  // namespace internal

/// @brief Return a reference to the global CoreErrorDomain.
/// @returns the CoreErrorDomain
///

constexpr ara::core::ErrorDomain const& GetCoreErrorDomain() noexcept { return internal::kCoreErrorDomain; }

/// @brief Create a new ErrorCode within CoreErrorDomain.
///
/// This function is used internally by constructors of ErrorCode. It is usually not
/// used directly by users.
///
/// @param code  the CoreErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @returns a new ErrorCode instance
///

constexpr ara::core::ErrorCode MakeErrorCode(CoreErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetCoreErrorDomain(), data);
}

}  // namespace core
}  // namespace isoft

#endif  // ISOFT_CORE_CORE_ERROR_DOMAIN_H_
