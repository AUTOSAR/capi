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
/// @file       isoft_ctx_kdf_des.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Key derivation function interface.
/// @date       2022-03-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-15  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key derivation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKdf_Des
/// @unit_description=DES-based key derivation context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_DES_H_

#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Key derivation function interface: DES encapsulation
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00190
/// @trace_id_dd=DD_CRYPTO_01043
/// @needwork = ad
/// @endcode
class PCtxKdf_Des : public PCtxKdf_Base
{
private:
    /// @brief Key length: length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01044
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyBitLength_{0U};

protected:
    /// @brief Set the key length in bits.
    /// @param bitLen Key length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01045
    /// @needwork = dda
    /// @endcode
    void _SetKeyBitLen(uint32_t const bitLen) noexcept { nKeyBitLength_ = bitLen; }

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01046
    /// @needwork = dda
    /// @endcode
    explicit PCtxKdf_Des(PCryptoProvider& cryptoProvider) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01047
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @name   GetExtensionService
    /// @returns ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01048
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;
    /// @brief Get the fixed size of the application-specific "filler" required by this context instance.
    ///         If this instance of the key derivation context does not support a filler value, it should return 0.
    ///			 Get the fixed size of the target key ID required for the diversification algorithm. The return value is constant for each interface instance, i.e., independent of configuration.
    /// @name   GetKeyIdSize
    /// @returns size of the application specific filler in bytes Returned value is constant for this instance of the
    /// key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01049
    /// @needwork = dda
    /// @endcode
    std::size_t GetKeyIdSize() const noexcept override;
    /// @brief Get the bit length of the target (diversified) key. The return value is configured by the context factory method, i.e., independent of configuration.
    /// @name   GetTargetKeyBitLength
    /// @returns Key bit length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01050
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetTargetKeyBitLength() const noexcept override
    {
        return static_cast< std::size_t >(nKeyBitLength_);
    }

protected:
    /// @brief: Check whether the given key material meets requirements.
    /// @name   ChecRestrictedUseObject
    /// @param sourceKM Source key material
    /// @returns true if stafiy false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01051
    /// @needwork = dda
    /// @endcode
    bool ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept override;
    /// @brief Check whether the length of the input raw key is satisfied.
    /// @name   CheckSourceKeyLen
    /// @param nKeyLen Key length
    /// @returns true if keylen is support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01052
    /// @needwork = dda
    /// @endcode
    bool CheckSourceKeyLen(uint32_t nKeyLen) const noexcept override;
};
//********************************/
/// @brief Key derivation function interface: 3DES encapsulation
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00191
/// @trace_id_dd=DD_CRYPTO_01053
/// @needwork = ad
/// @endcode
class PCtxKdf_3Des : public PCtxKdf_Des
{
public:
    /// @brief Parameterized constructor
    /// @name   PCtxKdf_3Des
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01054
    /// @needwork = dda
    /// @endcode
    explicit PCtxKdf_3Des(PCryptoProvider& cryptoProvider) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01055
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @name   GetExtensionService
    /// @returns ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01056
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;

protected:
    /// @brief: Check whether the given key material meets requirements.
    /// @name   ChecRestrictedUseObject
    /// @param sourceKM Source key material
    /// @returns true if stafiy false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01057
    /// @needwork = dda
    /// @endcode
    bool ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_DES_H_
