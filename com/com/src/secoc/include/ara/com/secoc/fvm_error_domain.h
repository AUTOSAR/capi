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
/// @file       fvm_error_domain.h
/// @brief      Vehicle safety communication freshness value management error header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_11340] -- Definition general ara::com::secoc errors
///
/// ================================================================

#ifndef ARA_COM_SECOC_FVM_ERROR_DOMAIN_H
#define ARA_COM_SECOC_FVM_ERROR_DOMAIN_H

#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace ara {
namespace com {
namespace secoc {
/// @brief Vehicle safety communication freshness value management error enumeration
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
/// @ref [SWS_CM_11342] -- The enumeration class defines the error codes for the SecOcFvmErrorDomain
enum class SecOcFvmErrc : ara::core::ErrorDomain::CodeType
{
    kUnknownError       = -1,  ///< Unknown/Other Error
    kFVNotAvailable     = 1,   ///< Recoverable Error meaning the Freshness Value not available
    kFVInitializeFailed = 2,   ///< Unrecoverable Error meaning the Freshness Value Manager could not be used
};

/// @brief Vehicle safety communication freshness value management exception type
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
class SecOcFvmException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @param[in] code Error code
    explicit SecOcFvmException(ara::core::ErrorCode code) noexcept : ara::core::Exception(code) {}
};

/// @brief Vehicle safety communication freshness value management error domain type
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
class SecOcFvmErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Error domain identifier
    /// @ref [SWS_CM_11341] -- SecOcFvm errors domain
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000001271};

public:
    /// @brief Type alias -- error type
    using Errc = SecOcFvmErrc;
    /// @brief Type alias -- exception type
    using Exception = SecOcFvmException;
    /// @brief Constructor
    constexpr SecOcFvmErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}
    /// @brief Get error domain name
    /// @return char const* SecOcFvmError
    char const* Name() const noexcept override { return "SecOcFvmError"; }
    /// @brief Get error code related message
    /// @param[in] code Error enumeration
    /// @return char const* Error code related message
    char const* Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        switch (static_cast< Errc >(code)) {
            case Errc::kUnknownError:
                return "Unknown/Other Error";
            case Errc::kFVNotAvailable:
                return "Recoverable Error meaning the Freshness Value not available";
            case Errc::kFVInitializeFailed:
                return "Unrecoverable Error meaning the Freshness Value Manager could not be used";
            default:
                return "Unknown error";
        }
    }
    /// @brief Throw exception
    /// @param[in] code Error code
    void ThrowAsException(ara::core::ErrorCode const& code) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(code);
    }
};

namespace internal {
/// @brief Vehicle safety communication freshness value management error domain
constexpr SecOcFvmErrorDomain kSecOcFvmErrorDomain;
}  // namespace internal

/// @brief Get vehicle safety communication freshness value management error domain
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
/// @return constexpr const ara::core::ErrorDomain&
inline constexpr const ara::core::ErrorDomain& GetSecOcFvmErrorDomain() noexcept
{
    return internal::kSecOcFvmErrorDomain;
}

/// @brief Generate vehicle safety communication freshness value management error code
/// @code{.isoft}
/// export_level=/COM/Security/SecOC
/// @endcode
/// @param[in] code Vehicle safety communication freshness value management error enumeration
/// @param[in] data Custom error data
/// @return constexpr ara::core::ErrorCode: An ErrorCode object
inline constexpr ara::core::ErrorCode MakeErrorCode(SecOcFvmErrc code,
                                                    ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetSecOcFvmErrorDomain(), data};
}
}  // namespace secoc
}  // namespace com
}  // namespace ara
#endif