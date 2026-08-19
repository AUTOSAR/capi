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
/// @file       isoft_ctx_dsv_signer_private_rsa_pss.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Signature private key context interface - PSS.
/// @date       2023-10-30
/// @author     Che Jinzhao
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
/// @unit_name=PCtxDsv_SignerPrivate_Rsa_Pss
/// @unit_description=Signature RSA-PSS private key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_PSS_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_PSS_H_

// DSV == Digital Signatures and Verifier
// PSS == RSA-PSS (Probabilistic Signature Scheme) is an improvement of the RSA signature algorithm, mainly used to enhance the security of RSA signatures.

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private.h"
#include "openssl/rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature private key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00673
/// @trace_id_dd=DD_CRYPTO_02440
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss : public PCtxDsv_SignerPrivate
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06314
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_SignerPrivate::PCtxDsv_SignerPrivate;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02441
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02442
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss(PCtxDsv_SignerPrivate_Rsa_Pss const& other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02443
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss(PCtxDsv_SignerPrivate_Rsa_Pss&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02444
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss& operator=(PCtxDsv_SignerPrivate_Rsa_Pss const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02445
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss& operator=(PCtxDsv_SignerPrivate_Rsa_Pss&& other) = delete;

private:
    /// @brief Pass salt parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02446
    /// @needwork = dda
    /// @endcode
    int32_t nSaltLen_{RSA_PSS_SALTLEN_MAX};

public:  //***************/
    /// @brief Get the maximum supported key length (in bits).
    /// @return maximal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02447
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02448
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verify support for a specific key length according to the context.
    /// @param keyBitLength Key length in bits
    /// @return @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02449
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @return Size of the signature value generated and required by the algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02450
    /// @needwork = dda
    /// @endcode
    std::size_t GetSignatureSize() const noexcept override;
    /// @brief Check whether the key meets requirements.
    /// @param key Private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02451
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PrivateKey const& key) const noexcept override;

protected:
    /// @brief Perform signature encryption logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @return Encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02452
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoEncrypto(uint8_t const* pInputData,
                                                                         uint32_t nDataLen,
                                                                         bool suppressPadding) const noexcept override;
    /// @brief Set the salt length.
    /// @param slen Salt length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02453
    /// @needwork = dda
    /// @endcode
    void _SetSaltLen(int32_t const slen) noexcept;

public:
};
//********************************/
/// @brief Signature private key context interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00674
/// @trace_id_dd=DD_CRYPTO_02454
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Md5 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Assignment constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02455
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Md5(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02456
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Md5() noexcept override = default;
    /// @brief Copy assignment constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02457
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Md5(PCtxDsv_SignerPrivate_Rsa_Pss_Md5 const& other) noexcept = delete;
    /// @brief Move assignment constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02458
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Md5(PCtxDsv_SignerPrivate_Rsa_Pss_Md5&& other) noexcept = delete;
    /// @brief Move assignment constructor
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02459
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Md5& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Md5&& other) noexcept = delete;
    /// @brief Copy constructor
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02460
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Md5& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Md5 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02461
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02462
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02463
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Pss_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00675
/// @trace_id_dd=DD_CRYPTO_02464
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02465
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Sha1(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02466
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Sha1() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02467
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha1(PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02468
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha1(PCtxDsv_SignerPrivate_Rsa_Pss_Sha1&& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02469
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha1& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha1&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02470
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha1& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02471
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02472
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02473
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Pss_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00676
/// @trace_id_dd=DD_CRYPTO_02474
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02475
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02476
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02477
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02478
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224&& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02479
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02480
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224& operator                  =(
        PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02481
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02482
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02483
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Pss_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00677
/// @trace_id_dd=DD_CRYPTO_02484
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02485
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02486
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02487
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02488
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256&& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02489
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02490
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256& operator                  =(
        PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02491
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02492
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02493
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Pss_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00678
/// @trace_id_dd=DD_CRYPTO_02494
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02495
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02496
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02497
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02498
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384&& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02499
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02500
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384& operator                  =(
        PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02501
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02502
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02503
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/
/// @brief Signature private key context interface: Rsa_Pss_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00679
/// @trace_id_dd=DD_CRYPTO_02504
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 : public PCtxDsv_SignerPrivate_Rsa_Pss
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02505
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02506
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02507
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02508
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512&& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02509
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512& operator=(PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02510
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512& operator                  =(
        PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 const& other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02511
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02512
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02513
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept override;
};
//********************************/

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_RSA_PSS_H_