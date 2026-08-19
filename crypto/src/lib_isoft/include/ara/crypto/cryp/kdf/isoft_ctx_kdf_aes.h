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
/// @file       isoft_ctx_kdf_aes.h
/// @brief      AutoSar-Crypto encryption and decryption module
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
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Key Derivation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKdf_Aes
/// @unit_description=Key derivation context based on AES
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_AES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_AES_H_

#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Key derivation function interface: OpenSSL wrapper.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00189
/// @trace_id_dd=DD_CRYPTO_01032
/// @needwork = ad
/// @endcode
class PCtxKdf_Aes : public PCtxKdf_Base
{
private:
    /// @brief Bit length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01033
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyBitLength_;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param nKeyBitLength key length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01034
    /// @needwork = dda
    /// @endcode
    explicit PCtxKdf_Aes(PCryptoProvider& cryptoProvider, uint32_t const nKeyBitLength) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01035
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @name   GetExtensionService
    /// @returns ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01036
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;
    /// @brief Get the fixed size of the application-specific "filler" required by this context instance.
    /// If this instance of the key derivation context does not support filler values, 0 should be returned.
    ///          Get the fixed size of the target key ID required by the diversification algorithm. The return value is constant for each interface instance, i.e., independent of configuration.
    /// @name   GetKeyIdSize
    /// @returns size of the application specific filler in bytes Returned value is constant for this instance of the
    /// key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01037
    /// @needwork = dda
    /// @endcode
    std::size_t GetKeyIdSize() const noexcept override;
    /// @brief Get the bit length of the target (diversified) key. The return value is configured by the context factory method, i.e., independent of configuration.
    /// @name   GetTargetKeyBitLength
    /// @returns bit length of the key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01038
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetTargetKeyBitLength() const noexcept override
    {
        return static_cast< std::size_t >(nKeyBitLength_);
    }
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @name GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01039
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @name GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01040
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;

protected:
    /// @brief: Check whether the given key material meets the requirements
    /// @name   ChecRestrictedUseObject
    /// @param sourceKM source key material
    /// @returns true if stafiy false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01041
    /// @needwork = dda
    /// @endcode
    bool ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept override;
    /// @brief Check whether the length of the input raw key meets the requirements
    /// @name   CheckSourceKeyLen
    /// @param nKeyLen key length
    /// @returns true if keylen is support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01042
    /// @needwork = dda
    /// @endcode
    bool CheckSourceKeyLen(uint32_t nKeyLen) const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_AES_H_
