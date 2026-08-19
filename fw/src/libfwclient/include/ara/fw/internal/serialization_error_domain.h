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
/// @brief      fw  error code.
/// @details    fw  error code.
/// @date       2025-03-19
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/error handling
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Serialization_Error_Domain
/// @unit_description=fw error code processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_SERIALIZATION_ERROR_DOMAIN_H_
#define ARA_FW_SERIALIZATION_ERROR_DOMAIN_H_
#include "ara/core/error_code.h"
#include "ara/core/exception.h"
#include "ara/core/string.h"
#include "ara/fw/common/common.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief Error codes of grant serialization
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00165
/// @trace_id_dd=DD_FW_00246
/// @needwork = ad
/// @endcode
enum class FwErrc : ara::core::ErrorDomain::CodeType
{
    kOk = 0,                         //< process ok.
    kServiceNotAvailable,            //< Communication to Firewall daemon is broken, i.e.
                                     // state is not switched
    kInvalidStateDependentFirewall,  ///< This firewallState is not used by any
                                     ///< StateDependentFirewall rule-sets
    kError                           ///< Any other error
};

/// @brief Specialization of ara::core::Exception
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00166
/// @trace_id_dd=DD_FW_00247
/// @needwork = ad
/// @endcode
class FWSerializationException : public ara::core::Exception
{
public:
    /// @brief See ara::core::Exception
    /// @param fwErr fw error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00167
    /// @trace_id_dd=DD_FW_00248
    /// @needwork = ad
    /// @endcode
    explicit FWSerializationException(ara::core::ErrorCode &&fwErr) noexcept : ara::core::Exception{std::move(fwErr)} {}
};

/// @brief Specialization of ara::core::ErrorDomain
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00168
/// @trace_id_dd=DD_FW_00249
/// @needwork = ad
/// @endcode
class FirewallErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId idtype.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00250
    /// @needwork = dda
    /// @endcode
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000000501U};

public:
    /// @brief Errc for this ErrorDomain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00168
    /// @trace_id_dd=DD_FW_00249
    /// @needwork = ad
    /// @endcode
    using Errc = FwErrc;

    /// @brief Constructor
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00169
    /// @trace_id_dd=DD_FW_00251
    /// @needwork = ad
    /// @endcode
    constexpr FirewallErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief See base class ara::core::ErrorDomain
    ///
    /// @returns GrantSerialization
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00170
    /// @trace_id_dd=DD_FW_00252
    /// @needwork = ad
    /// @endcode
    char8_t const *Name() const noexcept final { return "FWSerialization"; }

    /// @brief error message
    /// @param errCode CodeType
    /// @returns String
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00171
    /// @trace_id_dd=DD_FW_00253
    /// @needwork = ad
    /// @endcode
    char8_t const *Message(ara::core::ErrorDomain::CodeType errCode) const noexcept final
    {
        ara::core::String strCode;
        switch (errCode) {
            case static_cast< ara::core::ErrorDomain::CodeType >(FwErrc::kOk): {
                strCode = "firewall switch  status success!";
                break;
            }

            case static_cast< ara::core::ErrorDomain::CodeType >(FwErrc::kServiceNotAvailable): {
                strCode = "Servive not available";
                break;
            }

            case static_cast< ara::core::ErrorDomain::CodeType >(FwErrc::kInvalidStateDependentFirewall): {
                strCode = "invaild firewall status";
                break;
            }

            case static_cast< ara::core::ErrorDomain::CodeType >(FwErrc::kError): {
                strCode = "Error";
                break;
            }

            default: {
                strCode = "Unknown error";
                break;
            }
        }
        return strCode.c_str();
    }

    /// @brief  eror throw
    /// @param twErrorCode fw error code
    /// @exception error  code throw
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00172
    /// @trace_id_dd=DD_FW_00254
    /// @needwork = ad
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const &twErrorCode) const noexcept(false) final
    {
        ara::core::internal::ThrowOrTerminate< FWSerializationException >(twErrorCode);
    }
};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00255
/// @needwork = dda
/// @endcode
constexpr FirewallErrorDomain kFirewallErrorDomain;  ///< Static instance of FirewallErrorDomain

/// @brief Get the Grant Serialization Error Domain object
/// @return FirewallErrorDomain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00173
/// @trace_id_dd=DD_FW_00256
/// @needwork = ad
/// @endcode
inline constexpr ara::core::ErrorDomain const &GetFirewallErrorDomain() noexcept { return kFirewallErrorDomain; }
/// @brief Specialization of MakeErrorCode with FwErrc
/// @param code FwErrc
/// @param data SupportDataType
/// @return constexpr ara::core::ErrorCode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00174
/// @trace_id_dd=DD_FW_00257
/// @needwork = ad
/// @endcode
inline constexpr ara::core::ErrorCode MakeErrorCode(FwErrc const &code,
                                                    ara::core::ErrorDomain::SupportDataType const &data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetFirewallErrorDomain(), data);
}

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // ARA_IAM_IFC_GRANT_GRANT_SERIALIZATION_ERROR_DOMAIN_H_
