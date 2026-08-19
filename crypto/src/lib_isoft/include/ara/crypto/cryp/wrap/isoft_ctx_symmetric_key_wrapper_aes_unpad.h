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
/// @file       isoft_ctx_symmetric_key_wrapper_aes_unpad.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Symmetric key wrapping based on AES.
/// @date       2022-04-18
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-04-18  </td>1.0.0    <td>Chang Zheng     </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PCtxSymmetricKeyWrapperAesUnPad
/// @unit_description=AES-UNPAD-based key wrapping and unwrapping context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_AES_UNPAD_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_AES_UNPAD_H_

#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Symmetric key wrapping based on AES.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00561
/// @trace_id_dd=DD_CRYPTO_02095
/// @needwork = ad
/// @endcode
class PCtxSymmetricKeyWrapperAesUnPad : public PCtxSymmetricKeyWrapperAes
{
public:
    /// @code{.isoft}
    ///@brief brief description
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06307
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricKeyWrapperAes::PCtxSymmetricKeyWrapperAes;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02096
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricKeyWrapperAesUnPad() override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02097
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperAesUnPad(PCtxSymmetricKeyWrapperAesUnPad&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02098
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperAesUnPad& operator=(PCtxSymmetricKeyWrapperAesUnPad&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02099
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperAesUnPad& operator=(PCtxSymmetricKeyWrapperAesUnPad const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02100
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperAesUnPad(PCtxSymmetricKeyWrapperAesUnPad const& other) = delete;

public:
    /// @brief Get the target key granularity.
    /// @return Key granularity size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02101
    /// @needwork = dda
    /// @endcode
    std::size_t GetTargetKeyGranularity() const noexcept override;
    /// @brief Get the exclusive pointer to the extended service.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02102
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;
    /// @brief Get the crypto primitive ID.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02103
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Perform wrapping logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Wrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02104
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrap(uint8_t const* pInputData,
                                                                     uint32_t nDataLen) const noexcept override;
    /// @brief Perform local unwrapping logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Unwrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02105
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnWrap(uint8_t const* pInputData,
                                                                       uint32_t nDataLen) const noexcept override;

protected:
    /// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
    /// @param wrappedSeed Wrapped seed
    /// @return Wrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02106
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnwrap(
        ReadOnlyMemRegion const& wrappedSeed) const noexcept override;
    /// @brief Perform a "key wrapping" operation on the provided key material.
    /// @param key Key material
    /// @return Unwrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02107
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrapKeyMaterial(
        RestrictedUseObject const& key) const noexcept override;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_AES_UNPAD_H_