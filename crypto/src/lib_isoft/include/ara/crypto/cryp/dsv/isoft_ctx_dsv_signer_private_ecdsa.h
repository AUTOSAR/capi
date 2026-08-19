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
/// @file       isoft_ctx_dsv_signer_private_ecdsa.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Signature private key context interface.
/// @date       2022-03-23
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
/// @trace_id_sr=SR_CRYPTO_01010
/// @unit_name=PCtxDsv_SignerPrivate_Ecdsa
/// @unit_description=Signature ECC private key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_ECDSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_ECDSA_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature private key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00596
/// @trace_id_dd=DD_CRYPTO_02163
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Ecdsa : public PCtxDsv_SignerPrivate
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06310
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_SignerPrivate::PCtxDsv_SignerPrivate;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02164
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Ecdsa() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02165
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Ecdsa(PCtxDsv_SignerPrivate_Ecdsa&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02166
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Ecdsa& operator=(PCtxDsv_SignerPrivate_Ecdsa&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02167
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Ecdsa& operator=(PCtxDsv_SignerPrivate_Ecdsa const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02168
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Ecdsa(PCtxDsv_SignerPrivate_Ecdsa const& other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02169
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // SignerPrivateCtx interface
    /// @brief Get the SignatureService instance.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02170
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;

public:  // PServiceSignature interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return  maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02171
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02172
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verify support for a specific key length according to the context.
    /// @param keyBitLength key length in bits
    /// @return  @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02173
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value produced and required by the current algorithm.
    /// @return size of the signature value produced and required
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02174
    /// @needwork = dda
    /// @endcode
    std::size_t GetSignatureSize() const noexcept override;
    /// @brief Check whether the key meets the requirements
    /// @param key private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02175
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PrivateKey const& key) const noexcept override;
    /// @brief Get the cryptographic primitive ID
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02176
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
    /// @brief Perform signature encryption logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02177
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExecuteSignLogic(uint8_t const* const pInputData,
                                                                               uint32_t const nDataLen) const noexcept;

protected:
    /// @brief Perform signature encryption logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02178
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoEncrypto(uint8_t const* pInputData,
                                                                         uint32_t nDataLen,
                                                                         bool suppressPadding) const noexcept override;
    /// @brief Perform ECC IPC signature logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nInDataLen length of input data for algorithm operation
    /// @param slotId key slot ID
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02179
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< ara::core::Vector< ara::core::Byte > > DoEncrypto_Ipc(uint8_t const* const pInputData,
                                                                                    uint32_t const nInDataLen,
                                                                                    uint32_t const slotId) noexcept;

public:
};

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_ECDSA_H_
