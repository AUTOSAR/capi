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
/// @file       crypto_error_domain.h
/// @brief      The definition and implementation of ara::ucm::pkgmgr::CryptoErrorDomain type that derives from ara::core::ErrorDomain and contains the errors that can originate from within the crypto lib in the ucm.
/// @details
/// @date       2023-11-02
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=CryptoErrorDomain
/// @unit_description=CryptoErrorDomain definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_INTERNAL_CRYPTO_CRYPTO_ERROR_DOMAIN_H_
#define ARA_UCM_INTERNAL_CRYPTO_CRYPTO_ERROR_DOMAIN_H_

#include <cerrno>
#include <tuple>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief An enumeration with errors that can occur within this lib
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00154
/// @needwork = ad
/// @endcode
enum class CryptoErrc : ara::core::ErrorDomain::CodeType
{
    kGeneralError = 10,  ///< General error.
};

/// @brief An error domain for ara::ucm::pkgmgr::Crypto errors.
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00155
/// @needwork = ad
/// @endcode
class CryptoErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00156
    /// @needwork = dd
    /// @endcode
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000000703U};

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @uptrace={SWS_CORE_05231, cbc97baeb641245a26e2116268ec79e91adad3a3}
    /// @uptrace={SWS_CORE_10933, 9831758d60afd125a57a3e1f234b0c99950cbacc}
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Errc = CryptoErrc;

    /// @brief Alias for the exception base class
    ///
    /// @uptrace={SWS_CORE_05232, 975eef28bfc3232ffc21051c99647c512e83069a}
    /// @uptrace={SWS_CORE_10934, 1ad643bb41b8713cbf8d8509f7c6b2f8d7c332ea}
    ///using Exception = CryptoException;

    /// @brief Default constructor
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_CORE_05241, d58f187ed597e27e3db3d490a28bdbf50cc5a9f4}
    /// @uptrace={SWS_CORE_00014, 4448596ec421fc7980b549e3473b417cff14324a}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00157
    /// @needwork = dda
    /// @endcode
    constexpr CryptoErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    /// @returns "Crypto"
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_CORE_05242, c50e3baf0a82dfca2ef68558eab69705d03eb630}
    /// @uptrace={SWS_CORE_00014, 4448596ec421fc7980b549e3473b417cff14324a}
    /// @uptrace={SWS_CORE_10951, 15768dd685697ff3573bca7fb11fc5d5aa5890c6}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00158
    /// @needwork = dda
    /// @endcode
    char const* Name() const noexcept final { return "UCMCryptoErrorDomain"; }

    /// @brief Translate an error code value into a text message.
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_CORE_05243, 4e6aa85d836086353c9273796103eec7a2c59e14}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00159
    /// @needwork = dda
    /// @endcode
    char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept final
    {
        Errc const code{static_cast< Errc >(errorCode)};
        if (Errc::kGeneralError == code) {
            return "General error";
        }
        return "Unknown error";
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @param errorCode  the ErrorCode instance
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_CORE_05244, 04e6694764e2cad009f923154e5516fad9549986}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00160
    /// @needwork = dda
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) final
    {
        /// @uptrace={SWS_CORE_10953, 36d8ef828b0244ac573e536abc463ebab5890f2d}
        std::ignore = errorCode;
    }
};

namespace internal {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00161
/// @needwork = ad
/// @endcode
constexpr CryptoErrorDomain kCoreErrorDomain;
}  // namespace internal

/// @brief Return a reference to the global CryptoErrorDomain.
/// @returns the CryptoErrorDomain
/// @throws no
///
/// @code{.isoft}
/// @uptrace={SWS_CORE_05280, 4c8e15250f7089acb4952d258e89fc6a2dccea39}
/// @uptrace={SWS_CORE_10980, 7b3527e548da813f22785e8be0ecdf00830ad9cb}
/// @uptrace={SWS_CORE_10981, a22ca5477e277b1e1da68c5b8f641ce52d6518b2}
/// @uptrace={SWS_CORE_10982, 81fb283c392bb9e68639bd90ff71373109230314}
/// @uptrace={SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00162
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const& GetCryptoErrorDomain() noexcept { return internal::kCoreErrorDomain; }

/// @brief Create a new ErrorCode within CryptoErrorDomain.
///
/// This function is used internally by constructors of ErrorCode. It is usually not
/// used directly by users.
///
/// @param code  the CryptoErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @returns a new ErrorCode instance
/// @throws no
///
/// @code{.isoft}
/// @uptrace={SWS_CORE_05290, f2b33f86557b9f9abd1d47b26dab2fcc41adedbb}
/// @uptrace={SWS_CORE_10990, 20da35eb88503606d9b0a645c0ae8b59090bbfe7}
/// @uptrace={SWS_CORE_10991, df4e2b40a1a0f62208151663589cda3d8b4f2c3f}
/// @uptrace={SWS_CORE_10999, 59d020b68015c3dd68ba3aabd01365a5a4c00768}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00163
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(CryptoErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetCryptoErrorDomain(), data);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_INTERNAL_CRYPTO_CRYPTO_ERROR_DOMAIN_H_
