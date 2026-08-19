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
/// @file       isoft_ctx_dsv_sig_encode_private_rsa.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Private key context for asymmetric signature calculation and short message encoding (similar to RSA).
/// @date       2022-03-24
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SigEncodePrivate_Rsa
/// @unit_description=Private key context for asymmetric signature calculation and short message encoding (similar to RSA)
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_RSA_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_sig_encode_private.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Private key context for asymmetric signature calculation and short message encoding (similar to RSA).
///     A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided the public key is properly generated and kept confidential.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00630
/// @trace_id_dd=DD_CRYPTO_02234
/// @needwork = ad
/// @endcode
class PCtxDsv_SigEncodePrivate_Rsa : public PCtxDsv_SigEncodePrivate
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06312
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_SigEncodePrivate::PCtxDsv_SigEncodePrivate;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02235
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SigEncodePrivate_Rsa() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02236
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SigEncodePrivate_Rsa(PCtxDsv_SigEncodePrivate_Rsa&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02237
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SigEncodePrivate_Rsa& operator=(PCtxDsv_SigEncodePrivate_Rsa&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02238
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SigEncodePrivate_Rsa& operator=(PCtxDsv_SigEncodePrivate_Rsa const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02239
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SigEncodePrivate_Rsa(PCtxDsv_SigEncodePrivate_Rsa const& other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02240
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // SigEncodePrivateCtx interface
    /// @brief Extension service member class.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02241
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;

public:  // ExtensionService interface
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00631
    /// @trace_id_dd=DD_CRYPTO_02242
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00632
    /// @trace_id_dd=DD_CRYPTO_02243
    /// @needwork = ad
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verify support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00633
    /// @trace_id_dd=DD_CRYPTO_02244
    /// @needwork = ad
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;

protected:
    /// @brief Get the length of the encrypted block
    /// @return length of the encrypted block
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02245
    /// @needwork = dda
    /// @endcode
    uint32_t GetBlockSize() const noexcept override;
    /// @brief Check whether the key meets the requirements
    /// @param key private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02246
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PrivateKey const& key) const noexcept override;
    /// @brief Perform signature encryption logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02247
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoEncrypto(uint8_t const* pInputData,
                                                                         uint32_t nDataLen,
                                                                         bool suppressPadding) const noexcept override;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_RSA_H_
