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
/// @file       isoft_crl.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Certificate Revocation Lists(CRL)
/// @date       2023-11-20
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-11-20  <td>1.0.0    <td>Che Jinzhao      <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/Certificate Revocation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CRL
/// @unit_description=Certificate Revocation List
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_PUHUA_CRL_H_
#define ARA_CRYPTO_X509_PUHUA_CRL_H_

#include "ara/core/result.h"
#include "ara/core/utility.h"
#include "ara/core/vector.h"
#include "ara/crypto/x509/x509_object.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {

/// @brief Certificate Revocation class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_03646
/// @needwork = ad
/// @endcode
class CRL : public X509Object
{
private:
    /// @brief Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03647
    /// @needwork = dda
    /// @endcode
    X509Provider& x509Provider_;

public:
    /// @brief Default constructor
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03648
    /// @needwork = dda
    /// @endcode
    explicit CRL(X509Provider& x509Provider) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03649
    /// @needwork = dda
    /// @endcode
    ~CRL() noexcept override = default;
    /// @brief Move constructor
    /// @param other Another object instance of this class (the other crl)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03650
    /// @needwork = dda
    /// @endcode
    CRL(CRL&& other) noexcept = delete;
    /// @brief Copy constructor
    /// @param other Another object instance of this class (the other crl)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03651
    /// @needwork = dda
    /// @endcode
    CRL(CRL const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other Another object instance of this class (the other crl)
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03652
    /// @needwork = dda
    /// @endcode
    CRL& operator=(CRL&& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param other Another object instance of this class (the other crl)
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03653
    /// @needwork = dda
    /// @endcode
    CRL& operator=(CRL const& other) noexcept = delete;

public
    :  // Serializable interface
       /// @brief Publicly serialize itself.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03654
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

public:
    /// @brief Check if certificate is valid
    /// @name  CheckCert
    /// @returns true cert valid false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03655
    /// @needwork = dda
    /// @endcode
    static bool CheckCert() noexcept { return false; }

protected:
    /// @brief Used for clang-tidy hint that x509Provider_ is unused
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    inline void _UseX509Provider() const noexcept { std::ignore = x509Provider_; }
};

}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_X509_PUHUA_CRL_H_