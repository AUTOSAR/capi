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
/// @file       isoft_ctx_dsv_signer_private_rsa.h
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
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SignerPrivate_Rsa
/// @unit_description=Signature RSA private key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_H_

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
/// @trace_id_ad=AD_CRYPTO_00666
/// @trace_id_dd=DD_CRYPTO_02361
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa : public PCtxDsv_SignerPrivate
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06313
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_SignerPrivate::PCtxDsv_SignerPrivate;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02362
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa() noexcept override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02363
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa() = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02364
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa(PCtxDsv_SignerPrivate_Rsa const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02365
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa(PCtxDsv_SignerPrivate_Rsa &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02366
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa &operator=(PCtxDsv_SignerPrivate_Rsa const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02367
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa &operator=(PCtxDsv_SignerPrivate_Rsa &&other) = delete;

public:  // PServiceSignature interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02368
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02369
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verify support for a specific key length according to the context.
    /// @param keyBitLength key length in bits
    /// @return @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02370
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value produced and required by the current algorithm.
    /// @return size of the signature value produced and required by the algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02371
    /// @needwork = dda
    /// @endcode
    std::size_t GetSignatureSize() const noexcept override;
    /// @brief Check whether the key meets the requirements
    /// @param key private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02372
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PrivateKey const &key) const noexcept override;

protected:
    /// @brief Perform signature encryption logic
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02373
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoEncrypto(uint8_t const *pInputData,
                                                                         uint32_t nDataLen,
                                                                         bool suppressPadding) const noexcept override;

public:
};
//********************************/
/// @brief Signature private key context interface: Rsa_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00667
/// @trace_id_dd=DD_CRYPTO_02374
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Md5 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02375
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Md5(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02376
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Md5() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02377
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Md5() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02378
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Md5(PCtxDsv_SignerPrivate_Rsa_Md5 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02379
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Md5(PCtxDsv_SignerPrivate_Rsa_Md5 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02380
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Md5 &operator=(PCtxDsv_SignerPrivate_Rsa_Md5 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02381
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Md5 &operator=(PCtxDsv_SignerPrivate_Rsa_Md5 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02382
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02383
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02384
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00668
/// @trace_id_dd=DD_CRYPTO_02385
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Sha1 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02386
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Sha1(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02387
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha1() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02388
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Sha1() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02389
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha1(PCtxDsv_SignerPrivate_Rsa_Sha1 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02390
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha1(PCtxDsv_SignerPrivate_Rsa_Sha1 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02391
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha1 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha1 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02392
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha1 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha1 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02393
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02394
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02395
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00669
/// @trace_id_dd=DD_CRYPTO_02396
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Sha2_224 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02397
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Sha2_224(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02398
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_224() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02399
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Sha2_224() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02400
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_224(PCtxDsv_SignerPrivate_Rsa_Sha2_224 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02401
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_224(PCtxDsv_SignerPrivate_Rsa_Sha2_224 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02402
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_224 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_224 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02403
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_224 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_224 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02404
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02405
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02406
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00670
/// @trace_id_dd=DD_CRYPTO_02407
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Sha2_256 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02408
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Sha2_256(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02409
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_256() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02410
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Sha2_256() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02411
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_256(PCtxDsv_SignerPrivate_Rsa_Sha2_256 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02412
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_256(PCtxDsv_SignerPrivate_Rsa_Sha2_256 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02413
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_256 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_256 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02414
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_256 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_256 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02415
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02416
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02417
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00671
/// @trace_id_dd=DD_CRYPTO_02418
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Sha2_384 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02419
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Sha2_384(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02420
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_384() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02421
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Sha2_384() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02422
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_384(PCtxDsv_SignerPrivate_Rsa_Sha2_384 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02423
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_384(PCtxDsv_SignerPrivate_Rsa_Sha2_384 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02424
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_384 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_384 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02425
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_384 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_384 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02426
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02427
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02428
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00672
/// @trace_id_dd=DD_CRYPTO_02429
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Sha2_512 : public PCtxDsv_SignerPrivate_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02430
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Sha2_512(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02431
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_512() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02432
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Sha2_512() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02433
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_512(PCtxDsv_SignerPrivate_Rsa_Sha2_512 const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02434
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_512(PCtxDsv_SignerPrivate_Rsa_Sha2_512 &&other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02435
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_512 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_512 &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02436
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Sha2_512 &operator=(PCtxDsv_SignerPrivate_Rsa_Sha2_512 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02437
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extension service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02438
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the encryption algorithm
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02439
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_H_
