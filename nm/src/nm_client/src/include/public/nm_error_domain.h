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
/// @file       nm_error_domain.h
/// @brief      Network management error class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=software
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmErrorDomain
/// @unit_description=Network management error class
/// @endcode
///
/// ================================================================

#ifndef ARA_NM_ERROR_DOMAIN_NM_H_
#define ARA_NM_ERROR_DOMAIN_NM_H_

#include "ara/core/error_code.h"
#include "ara/core/exception.h"

namespace ara {
namespace nm {

/// @brief Defines the errors for NetworkManagement.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100006
/// @trace_id_dd=DD_NM_00878
/// @needwork = ad
/// @endcode
enum class NmErrc : ara::core::ErrorDomain::CodeType
{
    /// @brief The connection to the daemon is currently lost.
    kServiceNotAvailable = 1,
    /// @brief the process is not allowed to access api.
    kAccessWithoutPermission = 2,
};

/// @brief Exception type thrown by NetworkManagement.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100007
/// @trace_id_dd=DD_NM_00879
/// @needwork = ad
/// @endcode
class NmException : public ara::core::Exception
{
public:
    /// @brief Construct a new NetworkManagement exception object containing an
    /// error code
    /// @param code The error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00084
    /// @needwork = dda
    /// @endcode
    explicit NmException(ara::core::ErrorCode const code) noexcept : ara::core::Exception(code) {}
};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00923
/// @needwork = ad
/// @endcode
class NmErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief unique id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00886
    /// @needwork = dda
    /// @endcode
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000000042};

public:
    /// @brief Alias for the error code value enumeration.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00924
    /// @needwork = dda
    /// @endcode
    using Errc = NmErrc;

    /// @brief Alias for the exception base class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00925
    /// @needwork = dda
    /// @endcode
    using Exception = NmException;

    /// @brief Creates a NmErrorDomain instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00087
    /// @needwork = dda
    /// @endcode
    constexpr NmErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief Returns the name of the error domain.
    /// @return  the name of the error domain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00088
    /// @needwork = dda
    /// @endcode
    const char *Name() const noexcept override { return "NmError"; }
    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error
    /// domain.
    /// @param code Error code number.
    /// @return The message associated with the error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00089
    /// @needwork = dda
    /// @endcode
    const char *Message(ara::core::ErrorDomain::CodeType code) const noexcept override
    {
        switch (static_cast< Errc >(code)) {
            case Errc::kServiceNotAvailable:
                return "The connection to the daemon is currently lost.";
            case Errc::kAccessWithoutPermission:
                return "the process is not allowed to access api.";
            default:
                return "Unknown error";
        }
    }

    /// @brief Throws the exception associated with the error code.
    /// @param code The error to throw.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00090
    /// @needwork = dda
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const &code) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(code);
    }
};

namespace internal {

/// @brief declaration of the global NmErrorDomain object.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100008
/// @trace_id_dd=DD_NM_00880
/// @needwork = ad
/// @endcode
constexpr NmErrorDomain kNmErrorDomain;
}  // namespace internal

/// @brief get the global NmErrorDomain object.
/// @return the global NmErrorDomain object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100009
/// @trace_id_dd=DD_NM_00881
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const &GetNmDomain() noexcept { return internal::kNmErrorDomain; }

/// @brief Creates an error code.
/// @param code Error code number.
/// @param data Vendor defined data associated with the error.
/// @return error code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100010
/// @trace_id_dd=DD_NM_00882
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(NmErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code), GetNmDomain(), data};
}

}  // namespace nm
}  // namespace ara

#endif  // ARA_NM_ERROR_DOMAIN_NM_H_
