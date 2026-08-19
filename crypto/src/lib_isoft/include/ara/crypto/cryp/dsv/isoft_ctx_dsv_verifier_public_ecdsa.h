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
/// @file       isoft_ctx_dsv_verifier_public_ecdsa.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Signature verification public key context interface.
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-23  <tr>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01010
/// @unit_name=PCtxDsv_VerifierPublic_Ecdsa
/// @unit_description=Signature verification ECC public key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_ECDSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_ECDSA_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature verification public key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00562
/// @trace_id_dd=DD_CRYPTO_02108
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Ecdsa : public PCtxDsv_VerifierPublic
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06308
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_VerifierPublic::PCtxDsv_VerifierPublic;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02109
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Ecdsa() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02110
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Ecdsa(PCtxDsv_VerifierPublic_Ecdsa&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02111
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Ecdsa& operator=(PCtxDsv_VerifierPublic_Ecdsa&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02112
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Ecdsa& operator=(PCtxDsv_VerifierPublic_Ecdsa const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02113
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Ecdsa(PCtxDsv_VerifierPublic_Ecdsa const& other) = delete;

public:  // PServiceSignature interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02114
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02115
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @param keyBitLength Key length in bits
    /// @return @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02116
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @return Size of the signature value generated and required
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02117
    /// @needwork = dda
    /// @endcode
    std::size_t GetSignatureSize() const noexcept override;
    /// @brief Get the crypto primitive ID.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02118
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @name   GetSignatureService
    /// @returns SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02119
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Perform signature verification logic (external interface, used in key manager).
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param pDigestData Digest data
    /// @param nDigestDataLen Digest data length
    /// @return Encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02120
    /// @needwork = dda
    /// @endcode
    bool ExecuteVerifyLogic(uint8_t const* const pInputData,
                            uint32_t const nDataLen,
                            uint8_t const* const pDigestData,
                            uint32_t const nDigestDataLen) const noexcept;

protected:
    /// @brief Check whether the key meets requirements.
    /// @param key Public key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02121
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PublicKey const& key) const noexcept override;
    /// @brief Perform signature encryption logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @param pDigestData Digest data
    /// @param nDigestDataLen Digest data length
    /// @return true if DoDecrypto sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02122
    /// @needwork = dda
    /// @endcode
    bool DoDecrypto(uint8_t const* pInputData,
                    uint32_t nDataLen,
                    bool suppressPadding,
                    uint8_t const* const pDigestData,
                    uint32_t nDigestDataLen) const noexcept override;
    /// @brief Perform signature encryption logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Length of input data for algorithm operation
    /// @param pInDigestData Digest data
    /// @param nInDigestDataLen Digest data length
    /// @param slotId Key slot ID
    /// @return true if DoDecrypto_Ipc sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02123
    /// @needwork = dda
    /// @endcode
    static bool DoDecrypto_Ipc(uint8_t const* const pInputData,
                               uint32_t const nInDataLen,
                               uint8_t const* const pInDigestData,
                               uint32_t const nInDigestDataLen,
                               uint32_t const slotId) noexcept;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macth sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02124
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_H_
