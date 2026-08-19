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
/// @file       serialization_error_domain.h
/// @brief      grant error code.
/// @details
/// @date       2022-08-18
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td> <td> <td> <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM-COMMON
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00501
/// @unit_name=IAM_SerializationError
/// @unit_description=Provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_SERIALIZATION_ERROR_DOMAIN_H_
#define ARA_IAM_COMMON_SERIALIZATION_ERROR_DOMAIN_H_

#include <ara/core/string.h>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace iam {
namespace internal {
namespace grant {
/// @brief Use char8_t as an alias for char to eliminate qac 2428: Direct use of
/// character type.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = no
/// @endcode
using char8_t = char;  // NOLINT

/// @brief Error codes of grant serialization
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00331
/// @trace_id_dd=DD_IAM_00402
/// @needwork = ad
/// @endcode
enum class GrantSerializationErrc : ara::core::ErrorDomain::CodeType
{
    kInitCode            = 0,   ///< Initial error: Success
    kInvalidArgument     = 1,   ///< Invalid argument
    kFileNotFound        = 2,   ///< File not found for binary serialization
    kJsonLoadFailed      = 3,   ///< load json data failed
    kGrantTypeError      = 4,   ///< get Grant Type error.
    kGrantNotFound       = 5,   ///< not found query grant.
    kInvalidInstanceID   = 6,   ///< Invalid InstanceID
    kSingleGrantCheckPtr = 7,   ///< Invalid Single GrantCheckPtr
    kError               = 255  ///< Any other error
};

/// @brief Specialization of ara::core::Exception
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00332
/// @trace_id_dd=DD_IAM_00403
/// @needwork = ad
/// @endcode
class GrantSerializationException : public ara::core::Exception
{
public:
    /// @brief See ara::core::Exception
    /// @param grantErr

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00404
    /// @needwork = dda
    /// @endcode
    explicit GrantSerializationException(ara::core::ErrorCode &&grantErr) noexcept : ara::core::Exception{grantErr} {}
};

/// @brief Specialization of ara::core::ErrorDomain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00333
/// @trace_id_dd=DD_IAM_00405
/// @needwork = ad
/// @endcode
class GrantSerializationErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId idtype.

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00406
    /// @needwork = dda
    /// @endcode
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000000501U};

public:
    /// @brief Errc for this ErrorDomain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Errc = GrantSerializationErrc;

    /// @brief Constructor
    ///

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00407
    /// @needwork = dda
    /// @endcode
    constexpr GrantSerializationErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief See base class ara::core::ErrorDomain
    ///
    /// @returns GrantSerialization

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00408
    /// @needwork = dda
    /// @endcode
    char8_t const *Name() const noexcept final { return "GrantSerialization"; }

    /// @brief error message
    /// @param errCode CodeType
    /// @returns String

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00409
    /// @needwork = dda
    /// @endcode
    char8_t const *Message(ara::core::ErrorDomain::CodeType errCode) const noexcept final
    {
        ara::core::String strCode;
        switch (errCode) {
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kInitCode): {
                strCode = "NoError";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kInvalidArgument): {
                strCode = "InvalidArgument";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kFileNotFound): {
                strCode = "FileNotFound";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kJsonLoadFailed): {
                strCode = "kJsonLoadFailed";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kGrantTypeError): {
                strCode = "kGrantTypeError";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kGrantNotFound): {
                strCode = "kGrantNotFound";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kInvalidInstanceID): {
                strCode = "kInvalidInstanceID";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kSingleGrantCheckPtr): {
                strCode = "kSingleGrantCheckPtr";
            } break;
            case static_cast< ara::core::ErrorDomain::CodeType >(GrantSerializationErrc::kError): {
                strCode = "Error";
            } break;
            default: {
                strCode = "Unknown error";
            } break;
        }
        return strCode.c_str();
    }

    /// @brief  eror throw
    /// @param twErrorCode
    /// @exception error  code throw

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00410
    /// @needwork = dda
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const &twErrorCode) const noexcept(false) final
    {
        ara::core::internal::ThrowOrTerminate< GrantSerializationException >(twErrorCode);
    }
};

#if defined(__GNUC__)
    #pragma GCC diagnostic pop  // NOLINT
#endif

/// @brief Static instance of GrantSerializationErrorDomain

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00334
/// @trace_id_dd=DD_IAM_00411
/// @needwork = ad
/// @endcode
constexpr GrantSerializationErrorDomain kGrantSerializationErrorDomain;

/// @brief Get the Grant Serialization Error Domain object
/// @return GrantSerializationErrorDomain

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00335
/// @trace_id_dd=DD_IAM_00412
/// @needwork = ad
/// @endcode
inline constexpr ara::core::ErrorDomain const &GetGrantSerializationErrorDomain() noexcept
{
    return kGrantSerializationErrorDomain;
}
/// @brief Specialization of MakeErrorCode with GrantSerializationErrc
/// @param code GrantSerializationErrc
/// @param data SupportDataType
/// @return constexpr ara::core::ErrorCode

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00336
/// @trace_id_dd=DD_IAM_00413
/// @needwork = ad
/// @endcode
inline constexpr ara::core::ErrorCode MakeErrorCode(GrantSerializationErrc const &code,
                                                    ara::core::ErrorDomain::SupportDataType const &data) noexcept
{
    return ara::core::ErrorCode{static_cast< ara::core::ErrorDomain::CodeType >(code),
                                GetGrantSerializationErrorDomain(), data};
}

}  // namespace grant
}  // namespace internal
}  // namespace iam
}  //  namespace ara
#endif  // ARA_IAM_COMMON_SERIALIZATION_ERROR_DOMAIN_H_
