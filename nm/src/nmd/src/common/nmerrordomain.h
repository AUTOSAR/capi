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
/// @file       nmerrordomain.h
/// @brief      Network management error domain
/// @details
/// @date       2024-05-21
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/utils
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=utils
/// @unit_description=Network management error domain
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_ERRORDOAMIN_H_
#define _ARA_NM_ERRORDOAMIN_H_
#include <ara/core/error_domain.h>

#include "common.h"
#include "nmexception.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief  Network management error code enumeration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00739
/// @needwork = ad
/// @endcode
enum class NmErrc : ara::core::ErrorDomain::CodeType
{
    kDtcServerCantWrite = 1,  /// DTC does not support write.
    kCantFindLNPointer,       /// Unable to find LN pointer
};                            // enum class NmErrc

/// @brief NmErrorDomain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00740
/// @needwork = ad
/// @endcode
class NmErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief Pre-declare ID.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00866
    /// @needwork = dda
    /// @endcode
    constexpr static ErrorDomain::IdType kId{0x8000000000000200UL};

public:
    /// @brief : Alias for the error code value enumeration.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00968
    /// @needwork = dda
    /// @endcode
    using Errc = NmErrc;

    /// @brief : Creates a NmErrorDomain instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00124
    /// @needwork = dda
    /// @endcode
    constexpr NmErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief : Returns the name of the error domain.
    /// @returns : The name of the error domain.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00125
    /// @needwork = dda
    /// @endcode
    NmChar const *Name() const noexcept final { return "NmException"; }

    /// @brief : Returns the message associated with the error code.
    /// @param errorCode : The error code number.
    /// @returns : The message associated with the error code.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00126
    /// @needwork = dda
    /// @endcode
    NmChar const *Message(CodeType errorCode) const noexcept final
    {
        Errc const code{static_cast< Errc >(errorCode)};
        NmChar const *msg{nullptr};
        switch (code) {
            case NmErrc::kDtcServerCantWrite: {
                msg = "nm dtc server cant write.";
                break;
            }
            case NmErrc::kCantFindLNPointer: {
                msg = "cant find logical network pointer.";
                break;
            }
            default: {
                msg = "Unknown error";
                break;
            }
        }
        return msg;
    }

    /// @brief : Throws the exception associated with the error code.
    /// @param errorCode : The error to throw.
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00127
    /// @needwork = dda
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const &errorCode) const noexcept(false) final
    {
        ara::core::internal::ThrowOrTerminate< NmException >(errorCode);
    }
};
namespace errinternal {
/// @brief Core error domain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100003
/// @trace_id_dd=DD_NM_00742
/// @needwork = ad
/// @endcode
constexpr NmErrorDomain kNmErrorDomain;
}  // namespace errinternal

/// @brief : Returns the global NmErrorDomain object.
/// @returns : The global NmErrorDomain object.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100004
/// @trace_id_dd=DD_NM_00743
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const &GetNmDomain() noexcept { return errinternal::kNmErrorDomain; }

/// @brief : Creates an error code.
/// @param code : Error code number.
/// @param data : Vendor defined data associated with the error.
/// @returns :  An ErrorCode object.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100005
/// @trace_id_dd=DD_NM_00744
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(NmErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetNmDomain(), data);
}

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_ERRORDOAMIN_H_
