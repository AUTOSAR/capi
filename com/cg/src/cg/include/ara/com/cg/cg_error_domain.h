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
/// @file       cg_error_domain.h
/// @brief      Communication group error header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_99023] -- Definition general ara::com errors
///
/// ================================================================

#ifndef ARA_COM_CG_CG_ERROR_DOMAIN_H
#define ARA_COM_CG_CG_ERROR_DOMAIN_H

#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace ara {
namespace com {
namespace cg {
/// @brief Communication group error enumeration
/// @code{.isoft}
/// export_level=/COM/SoC/CG
/// @endcode
/// @ref [SWS_CM_99024] -- The CgErrc enumeration defines the error codes for the CgErrorDomain
enum class CgErrc : ara::core::ErrorDomain::CodeType
{
    kUnknownError                = -1,  ///< Unknown/Other Error
    kCommunicationGroupNotActive = 1,   ///< Commincation Group not active/connected by a Server
    kNoClients                   = 2,   ///< No communication group clients
    kWrongClientAddress          = 3,   ///< Wrong client address
    kBindingError                = 4,   ///< Error at technology binding
    kMemoryError                 = 5,   ///< Memory Error
    kServerExists                = 6,   ///< Other server already connected to communication group
};

/// @brief Communication group exception type
/// @code{.isoft}
/// export_level=/COM/SoC/CG
/// @endcode
class CgException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @param[in] code Error code
    explicit CgException(ara::core::ErrorCode code) noexcept : ara::core::Exception(code) {}
};

/// @brief Communication group error domain type
/// @code{.isoft}
/// export_level=/COM/SoC/CG
/// @endcode
class CgErrorDomain final : public ara::core::ErrorDomain
{
    /// @ref [SWS_CM_99027] -- Cg errors domain
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000001270};

public:
    /// @brief Type alias -- Error type
    using Errc = CgErrc;
    /// @brief Type alias -- Exception type
    using Exception = CgException;
    /// @brief Constructor
    constexpr CgErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}
    /// @brief Get error domain name
    /// @return char const* CgError
    char const* Name() const noexcept override { return "CgError"; }
    /// @brief Get error message associated with error code
    /// @param[in] code Error enumeration
    /// @return char const* Error message associated with error code
    char const* Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        switch (static_cast< Errc >(code)) {
            case Errc::kUnknownError:
                return "Unknown/Other Error";
            case Errc::kCommunicationGroupNotActive:
                return "Commincation Group not active/connected by a Server Values";
            case Errc::kNoClients:
                return "No communication group clients";
            case Errc::kWrongClientAddress:
                return "Wrong client address";
            case Errc::kBindingError:
                return "Error at technology binding";
            case Errc::kMemoryError:
                return "Memory Error";
            case Errc::kServerExists:
                return "Other server already connected to communication group";
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
/// @brief Communication group error domain
constexpr CgErrorDomain kCgErrorDomain;
}  // namespace internal

/// @brief Get the communication group error domain
/// @code{.isoft}
/// export_level=/COM/SoC/CG
/// @endcode
/// @return constexpr ara::core::ErrorDomain const&
inline constexpr ara::core::ErrorDomain const& GetCgErrorDomain() noexcept { return internal::kCgErrorDomain; }

/// @brief Generate communication group error code
/// @code{.isoft}
/// export_level=/COM/SoC/CG
/// @endcode
/// @param[in] code Communication group error enumeration
/// @param[in] data Custom error data
/// @return constexpr ara::core::ErrorCode: An ErrorCode object
inline constexpr ara::core::ErrorCode MakeErrorCode(CgErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetCgErrorDomain(), data};
}
}  // namespace cg
}  // namespace com
}  // namespace ara
#endif