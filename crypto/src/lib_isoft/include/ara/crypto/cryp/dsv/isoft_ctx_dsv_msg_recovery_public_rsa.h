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
/// @file       isoft_ctx_dsv_msg_recovery_public_rsa.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Public key context for asymmetric recovery of short messages and their signature verification (similar to RSA).
/// @date       2022-03-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-29  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_MsgRecoveryPublic_Rsa
/// @unit_description=Public key context for asymmetric recovery of short messages and their signature verification (similar to RSA)
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_RSA_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_msg_recovery_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Public key context for asymmetric recovery of short messages and their signature verification (similar to RSA).
///     A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided the public keys are properly generated and kept confidential.
///     If (0 == BlockCryptor::ProcessBlock(…)), the input message block is contaminated.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00597
/// @trace_id_dd=DD_CRYPTO_02180
/// @needwork = ad
/// @endcode
class PCtxDsv_MsgRecoveryPublic_Rsa : public PCtxDsv_MsgRecoveryPublic
{
protected:
public:
    /// @brief Buffer length enumeration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00598
    /// @trace_id_dd=DD_CRYPTO_02181
    /// @needwork = ad
    /// @endcode
    enum class EBuffLen : std::int32_t
    {
        kMaxInputBuffLen  = 1024 * 4,
        kMaxOutputBuffLen = 1024,
    };

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06311
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_MsgRecoveryPublic::PCtxDsv_MsgRecoveryPublic;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02182
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_MsgRecoveryPublic_Rsa() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02183
    /// @needwork = dda
    /// @endcode
    PCtxDsv_MsgRecoveryPublic_Rsa& operator=(PCtxDsv_MsgRecoveryPublic_Rsa const& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02184
    /// @needwork = dda
    /// @endcode
    PCtxDsv_MsgRecoveryPublic_Rsa& operator=(PCtxDsv_MsgRecoveryPublic_Rsa&& other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02185
    /// @needwork = dda
    /// @endcode
    PCtxDsv_MsgRecoveryPublic_Rsa(PCtxDsv_MsgRecoveryPublic_Rsa const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02186
    /// @needwork = dda
    /// @endcode
    PCtxDsv_MsgRecoveryPublic_Rsa(PCtxDsv_MsgRecoveryPublic_Rsa&& other) noexcept = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02187
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // MsgRecoveryPublicCtx interface
    /// @brief Get the ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02188
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;

public:  // ExtensionService interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return Maximum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02189
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return Minimum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02190
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @param keyBitLength key length in bits
    /// @return true if key len is support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02191
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;

protected:
    /// @brief Get the length of the encrypted block
    /// @return length of the encrypted block
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02192
    /// @needwork = dda
    /// @endcode
    uint32_t GetBlockSize() const noexcept override;
    /// @brief Check whether the key meets the requirements
    /// @param key public key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02193
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PublicKey const& key) const noexcept override;
    /// @brief Perform signature decryption logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02194
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoDecrypto(uint8_t const* pInputData,
                                                                         uint32_t nDataLen,
                                                                         bool suppressPadding) const noexcept override;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_RSA_H_
