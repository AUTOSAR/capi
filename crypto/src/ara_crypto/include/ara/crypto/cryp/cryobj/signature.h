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
/// @file       signature.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Signature container interface for storing digital signatures, hash digests, and (Hash-based) Message Authentication Codes (MAC/HMAC).
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature Storage
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Signature
/// @unit_description=Digital Signature
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SIGNATURE_H_
#define ARA_CRYPTO_CRYP_SIGNATURE_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/cryobj/crypto_object.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Signature container interface for storing digital signatures, hash digests, and (Hash-based) Message Authentication Codes (MAC/HMAC).
///     In the case of key signatures (digital signatures or MAC/HMAC), the COUID of the signature verification key can be obtained by calling CryptoObject::HasDependency()!
/// @brief Signature container interface This interface is applicable for keeping the Digital Signature, Hash Digest,
/// (Hash-based) Message Authentication Code (MAC/HMAC).
///     In case of a keyed signature (Digital Signature or MAC/HMAC) a COUID of the signature verification key can be
///     obtained by a call of CryptoObject::HasDependence()!
/// @interface Signature
/// @AUTOSAR_SWS {SWS_CRYPT_23300}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02203}
/// @uptrace={RS_CRYPTO_02204}
/// @uptrace={RS_CRYPTO_02205}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02534
/// @trace_id_dd=DD_CRYPTO_05330
/// @needwork = ad
/// @endcode
class Signature : public CryptoObject
{
public:
    /// @brief Unique pointer for Signature
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03328
    /// @trace_id_dd=DD_CRYPTO_06659
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< Signature >;
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the interface
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23301}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02205}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03329
    /// @trace_id_dd=DD_CRYPTO_06660
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< Signature const >;

private:
    /// @brief Signature object initialization.
    /// @brief Signature object initialized
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23302}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02205}
    /// static CryptoObjectType const  kObjectType = CryptoObjectType::kSignature;
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05331
    /// @needwork = dda
    /// @endcode
    CryptoObjectType const kObjectType{CryptoObjectType::kSignature};

public:
    /// @brief Get the type of cryptographic material.
    /// @return Key object type
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02535
    /// @trace_id_dd=DD_CRYPTO_05332
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetkObjectType() const noexcept { return kObjectType; }

public:
    /// @brief Get the ID of the hash algorithm used for generating this signature object.
    /// @brief Get an ID of hash algorithm used for this signature object production.
    /// @return ID of used hash algorithm only (without signature algorithm specification)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02536
    /// @trace_id_dd=DD_CRYPTO_05333
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept = 0;
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @return required hash size in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23312}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02537
    /// @trace_id_dd=DD_CRYPTO_05334
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetRequiredHashSize() const noexcept = 0;

    /// @brief Set the public key couid that the signature depends on
    /// @param keyId IO interface ID corresponding to the key material
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02538
    /// @trace_id_dd=DD_CRYPTO_05335
    /// @needwork = ad
    /// @endcode
    virtual void SetDependence(COIdentifier const& keyId) noexcept = 0;
    /// @brief During signature verification, retrieve the COUID of the public key (if error) to compare with the public key COUID used in the verification context.
    /// @brief This step is located in the VerifyPrehashed() function in ph_ctx_dsv_verifier_public.cpp
    /// @return Dependent object UUID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02539
    /// @trace_id_dd=DD_CRYPTO_05336
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetDependence() const noexcept = 0;

public
    :  // Custom virtual functions: Derived from Serializable interface, new virtual functions created because multiple inheritance is not allowed.
    /// @brief Serialize Public itself.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08783
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08784
    /// @needwork = dda
    /// @endcode
    ExportPublicly(Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept = 0;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SIGNATURE_H_
