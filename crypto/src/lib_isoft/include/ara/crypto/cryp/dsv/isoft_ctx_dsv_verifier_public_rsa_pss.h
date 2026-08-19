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
/// @file       isoft_ctx_dsv_verifier_public_rsa_pss.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Signature verification public key context interface — PSS.
/// @date       2023-10-30
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-10-30  </td>1.0.0    </td>Che Jinzhao      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_VerifierPublic_Rsa_Pss
/// @unit_description=Signature verification public key RSA-PSS context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_PSS_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_PSS_H_

// DSV == Digital Signatures and Verifier
// PSS == RSA-PSS (Probabilistic Signature Scheme) is an improved scheme for the RSA signature algorithm, mainly used to enhance the security of RSA signatures.

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public.h"
#include "openssl/rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00634
/// @trace_id_dd=DD_CRYPTO_02248
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss : public PCtxDsv_VerifierPublic
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02249
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02250
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02251
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02252
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss(PCtxDsv_VerifierPublic_Rsa_Pss const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02253
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss(PCtxDsv_VerifierPublic_Rsa_Pss &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02254
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss &operator=(PCtxDsv_VerifierPublic_Rsa_Pss const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02255
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss &operator=(PCtxDsv_VerifierPublic_Rsa_Pss &&other) = delete;

private:
    /// @brief Pass salt parameter.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02256
    /// @needwork = dda
    /// @endcode
    int32_t nSaltLen_{RSA_PSS_SALTLEN_MAX};

public:  // PServiceSignature interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02257
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02258
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @param keyBitLength Key length in bits
    /// @return @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02259
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @return Size of the signature value generated and required
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02260
    /// @needwork = dda
    /// @endcode
    std::size_t GetSignatureSize() const noexcept override;

protected:
    /// @brief Check whether the key meets requirements.
    /// @param key Public key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02261
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PublicKey const &key) const noexcept override;
    /// @brief Perform signature encryption logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @param pDigestData Digest data
    /// @param nDigestDataLen Digest data length
    /// @return Encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02262
    /// @needwork = dda
    /// @endcode
    bool DoDecrypto(uint8_t const *pInputData,
                    uint32_t nDataLen,
                    bool suppressPadding,
                    uint8_t const *const pDigestData,
                    uint32_t nDigestDataLen) const noexcept override;
    /// @brief Set the salt length.
    /// @param slen Salt length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02263
    /// @needwork = dda
    /// @endcode
    void _SetSaltLen(int32_t const slen) noexcept;

public:
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00635
/// @trace_id_dd=DD_CRYPTO_02264
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Md5 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02265
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Md5(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02266
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Md5() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02267
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Md5() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02268
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Md5(PCtxDsv_VerifierPublic_Rsa_Pss_Md5 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02269
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Md5(PCtxDsv_VerifierPublic_Rsa_Pss_Md5 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02270
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Md5 &operator=(PCtxDsv_VerifierPublic_Rsa_Pss_Md5 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02271
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Md5 &operator=(PCtxDsv_VerifierPublic_Rsa_Pss_Md5 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02272
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02273
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macth sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02274
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00636
/// @trace_id_dd=DD_CRYPTO_02275
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02276
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Sha1(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02277
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha1() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02278
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Sha1() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02279
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha1(PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02280
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha1(PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02281
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 &operator=(PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02282
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 &operator=(PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02283
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02284
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Checks if equal
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02285
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00637
/// @trace_id_dd=DD_CRYPTO_02286
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02287
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02288
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02289
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02290
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02291
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 &&other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02292
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 &operator             =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 &&other) noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02293
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 &operator                  =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02294
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02295
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Checks if equal
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02296
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00638
/// @trace_id_dd=DD_CRYPTO_02297
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02298
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02299
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02300
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02301
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02302
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 &&other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02303
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 &operator             =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 &&other) noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02304
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 &operator                  =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02305
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02306
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Checks if equal
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02307
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00639
/// @trace_id_dd=DD_CRYPTO_02308
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02309
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02310
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02311
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02312
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02313
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 &&other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02314
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 &operator             =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 &&other) noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02315
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 &operator                  =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02316
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02317
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Checks if equal
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02318
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//********************************/
/// @brief Signature verification public key context interface: Rsa_Pss_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00640
/// @trace_id_dd=DD_CRYPTO_02319
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 : public PCtxDsv_VerifierPublic_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @name PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02320
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02321
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02322
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512() noexcept override = default;
    /// @brief Default copy constructor
    /// @name PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02323
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @name PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02324
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512(PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 &&other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02325
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 &operator             =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 &&other) noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02326
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 &operator                  =(
        PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02327
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02328
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Checks if equal
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02329
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_PSS_H_
