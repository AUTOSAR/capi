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
/// @file       phm_error_domain.h
/// @brief      Error domain of PHM.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Util
/// @unit_description=Error domain of PHM.
/// @trace_id_sr=SR_PHM_01035
/// @unit_name=PhmErrorDomain
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_PHM_ERROR_DOMAIN_H_
#define ARA_PHM_INTERNAL_PHM_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include <cerrno>

namespace ara {
namespace phm {

/// @brief Defines an enumeration class for the Platform Health Management error
/// codes.
/// @trace_id_sr=SR_PHM_01035
/// @needwork = ad
enum class PhmErrc : ara::core::ErrorDomain::CodeType
{
    /// @brief Service could not be offered due to failure of communication with Phm daemon
    kOfferFailed = 2

};

/// @brief Exception type thrown by Platform Health Management.
/// @trace_id_sr=SR_PHM_01035
/// @needwork = ad
class PhmException : public ara::core::Exception
{
public:
    /// @brief Construct a new PlatformHealthManagement exception object containing an error code.
    /// @param errorCode The error code.
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = ad
    explicit PhmException(ara::core::ErrorCode errorCode) noexcept : ara::core::Exception{std::move(errorCode)} {}
};

/// @brief Defines the error domain for Platform Health Management.
/// @trace_id_sr=SR_PHM_01035
/// @needwork = ad
class PhmErrorDomain final : public ara::core::ErrorDomain
{
public:
    /// @brief Alias for the error code value enumeration.
    /// @needwork = dda
    using Errc = PhmErrc;

    /// @brief Alias for the exception base class.
    /// @needwork = dda
    using Exception = PhmException;

    /// @brief Creates a PhmErrorDomain instance.
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = ad
    PhmErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief Returns the name of the error domain.
    /// @code{.isoft}
    /// @interface_level=software
    /// @endcode
    /// @return "Phm".
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = ad
    char const *Name() const noexcept final { return "Phm"; }

    /// @brief Returns the message associated with the error code.
    /// @code{.isoft}
    /// @interface_level=software
    /// @endcode
    /// @param  errorCode The error code number.
    /// @return The message associated with the error code.
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = ad
    char const *Message(CodeType errorCode) const noexcept final
    {
        Errc const code{static_cast< Errc >(errorCode)};
        ara::core::StringView errcStr;
        if (Errc::kOfferFailed == code) {
            errcStr = ara::core::StringView{
                "Service could not be offered due to failure "
                "of communication with Phm daemon."};

        } else {
            errcStr = ara::core::StringView{"Unknown error"};
        }
        return errcStr.data();
    }

    /// @brief Throws the exception associated with the error code.
    /// @param errorCode The error to throw.
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = ad
    void ThrowAsException(ara::core::ErrorCode const &errorCode) const noexcept(false) final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }

private:
    /// @brief Key ID for phm error domain.
    /// @trace_id_sr=SR_PHM_01035
    /// @needwork = dda
    static ErrorDomain::IdType const kId{0x8000000000000602U};
};  // namespace phm

namespace internal {
/// @brief the global PhmErrorDomain object
/// @trace_id_sr=SR_PHM_01035
/// @needwork = dd
static PhmErrorDomain const kPhmErrorDomain;  // NOLINT
}  // namespace internal

/// @brief Returns the global PhmErrorDomain object.
/// @code{.isoft}
/// @interface_level=module
/// @endcode
/// @return The global PhmErrorDomain object.
/// @trace_id_sr=SR_PHM_01035
/// @needwork = ad
constexpr ara::core::ErrorDomain const &GetPhmDomain() noexcept { return internal::kPhmErrorDomain; }

/// @brief Creates an error code.
/// @code{.isoft}
/// @interface_level=module
/// @endcode
/// @param code Error code number.
/// @param data Vendor defined data associated with the error.
/// @return An ErrorCode object.
/// @trace_id_sr=SR_PHM_01035
/// @needwork = ad
constexpr ara::core::ErrorCode MakeErrorCode(PhmErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetPhmDomain(), data);
}

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_PHM_ERROR_DOMAIN_H_