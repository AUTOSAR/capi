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
/// @file       e2e_error_domain.h
/// @brief      E2E error header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_11264] -- Definition general ara::com errors
///
/// ================================================================

#ifndef ARA_COM_E2E_E2E_ERROR_DOMAIN_H
#define ARA_COM_E2E_E2E_ERROR_DOMAIN_H

#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace ara {
namespace com {
namespace e2e {
/// @brief E2E error enumeration
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
/// @ref [SWS_CM_10474] -- The E2EErrc enumeration defines the error codes for the E2EErrorDomain
enum class E2EErrc : ara::core::ErrorDomain::CodeType
{
    kUnknownError  = -1,  ///< Unknown/Other Error
    kRepeated      = 1,   ///< Data has a repeated counter
    kWrongSequence = 2,   ///< The checks of the Data in this cycle were successful, with the exception of counter jump,
                          ///< which changed more than the allowed delta
    kError = 3,  ///< Error not related to counters occurred (e.g. wrong crc, wrong length, wrong Data ID) or the return
                 ///< of the check function was not OK
    kNotAvailable = 4,  ///< No value has been received yet (e.g. during initialization). This is used as the
                        ///< initialization value for the buffer, it is not returned by any E2E profile
    kNoNewData = 5      ///< No new data is available.
};

/// @brief E2E exception type
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode

class E2EException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @param[in] code Error code
    explicit E2EException(ara::core::ErrorCode code) noexcept : ara::core::Exception(code) {}
};

/// @brief E2E exception type
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
class E2EErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Error domain identifier
    /// @ref [SWS_CM_99026] -- E2E errors domain
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000001268};

public:
    /// @brief Type alias -- error type
    using Errc = E2EErrc;
    /// @brief Type alias -- exception type
    using Exception = E2EException;
    /// @brief Constructor
    constexpr E2EErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}
    /// @brief Get error domain name
    /// @return char const* E2EError
    char const* Name() const noexcept override { return "E2EErrorDomain"; }
    /// @brief Get error code related message
    /// @param[in] code Error enumeration
    /// @return char const* Error code related message
    char const* Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        switch (static_cast< Errc >(code)) {
            case Errc::kUnknownError:
                return "Unknown/Other Error";
            case Errc::kRepeated:
                return "Data has a repeated counter";
            case Errc::kWrongSequence:
                return "The checks of the Data in this cycle were successful, with the exception of counter jump, "
                       "which changed more than the allowed delta";
            case Errc::kError:
                return "Error not related to counters occurred (e.g. wrong crc, wrong length, wrong Data ID) or the "
                       "return of the check function was not OK";
            case Errc::kNotAvailable:
                return "No value has been received yet (e.g. during initialization). This is used as the "
                       "initialization value for the buffer, it is not returned by any E2E profile";
            case Errc::kNoNewData:
                return "No new data is available.";
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
/// @brief E2E error domain
constexpr E2EErrorDomain kE2EErrorDomain;
}  // namespace internal

/// @brief Get E2E error domain
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
/// @return constexpr ara::core::ErrorDomain const&
inline constexpr ara::core::ErrorDomain const& GetE2EErrorDomain() noexcept { return internal::kE2EErrorDomain; }

/// @brief Generate E2E error code
/// @code{.isoft}
/// export_level=/COM/Safety/E2E
/// @endcode
/// @param[in] code E2E error enumeration
/// @param[in] data Custom error data
/// @return constexpr ara::core::ErrorCode: An ErrorCode object
inline constexpr ara::core::ErrorCode MakeErrorCode(E2EErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetE2EErrorDomain(), data};
}
}  // namespace e2e
}  // namespace com
}  // namespace ara
#endif
