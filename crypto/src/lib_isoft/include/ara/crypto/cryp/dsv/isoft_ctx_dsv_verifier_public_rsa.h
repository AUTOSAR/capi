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
/// @file       isoft_ctx_dsv_verifier_public_rsa.h
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
/// <tr><td>2022-03-23  </tr>1.0.0    <tr>hanjingjing      <tr>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_VerifierPublic_Rsa
/// @unit_description=Signature verification RSA public key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature verification public key context interface: RSA
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00705
/// @trace_id_dd=DD_CRYPTO_02545
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa : public PCtxDsv_VerifierPublic
{
public:
    /// @brief Use base class construction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06315
    /// @needwork = dda
    /// @endcode
    using PCtxDsv_VerifierPublic::PCtxDsv_VerifierPublic;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02546
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02547
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02548
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa(PCtxDsv_VerifierPublic_Rsa const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02549
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa(PCtxDsv_VerifierPublic_Rsa &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02550
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa &operator=(PCtxDsv_VerifierPublic_Rsa const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02551
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa &operator=(PCtxDsv_VerifierPublic_Rsa &&other) = delete;

public:  // PServiceSignature interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return  maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02552
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return minimal supported key length in bits.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02553
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @param keyBitLength Key length in bits
    /// @return  @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02554
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @return Size of the signature value generated and required
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02555
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
    /// @trace_id_dd=DD_CRYPTO_02556
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
    /// @trace_id_dd=DD_CRYPTO_02557
    /// @needwork = dda
    /// @endcode
    bool DoDecrypto(uint8_t const *pInputData,
                    uint32_t nDataLen,
                    bool suppressPadding,
                    uint8_t const *const pDigestData,
                    uint32_t nDigestDataLen) const noexcept override;

public:
};
//********************************/
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00706
/// @trace_id_dd=DD_CRYPTO_02558
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Md5 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02559
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Md5(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02560
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Md5() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02561
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Md5() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02562
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Md5(PCtxDsv_VerifierPublic_Rsa_Md5 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02563
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Md5(PCtxDsv_VerifierPublic_Rsa_Md5 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02564
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Md5 &operator=(PCtxDsv_VerifierPublic_Rsa_Md5 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02565
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Md5 &operator=(PCtxDsv_VerifierPublic_Rsa_Md5 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02566
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02567
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02568
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00707
/// @trace_id_dd=DD_CRYPTO_02569
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Sha1 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02570
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Sha1(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02571
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha1() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02572
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Sha1() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02573
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha1(PCtxDsv_VerifierPublic_Rsa_Sha1 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02574
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha1(PCtxDsv_VerifierPublic_Rsa_Sha1 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02575
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha1 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha1 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02576
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha1 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha1 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02577
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02578
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02579
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00708
/// @trace_id_dd=DD_CRYPTO_02580
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Sha2_224 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02581
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Sha2_224(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02582
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_224() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02583
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Sha2_224() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02584
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_224(PCtxDsv_VerifierPublic_Rsa_Sha2_224 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02585
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_224(PCtxDsv_VerifierPublic_Rsa_Sha2_224 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02586
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_224 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_224 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02587
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_224 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_224 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02588
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02589
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns  true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02590
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00709
/// @trace_id_dd=DD_CRYPTO_02591
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Sha2_256 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02592
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Sha2_256(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02593
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_256() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02594
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Sha2_256() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02595
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_256(PCtxDsv_VerifierPublic_Rsa_Sha2_256 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02596
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_256(PCtxDsv_VerifierPublic_Rsa_Sha2_256 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02597
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_256 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_256 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02598
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_256 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_256 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02599
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02600
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02601
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00710
/// @trace_id_dd=DD_CRYPTO_02602
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Sha2_384 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02603
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Sha2_384(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02604
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_384() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02605
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Sha2_384() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02606
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_384(PCtxDsv_VerifierPublic_Rsa_Sha2_384 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02607
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_384(PCtxDsv_VerifierPublic_Rsa_Sha2_384 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02608
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_384 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_384 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02609
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_384 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_384 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02610
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02611
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02612
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Signature verification public key context interface: Rsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00711
/// @trace_id_dd=DD_CRYPTO_02613
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic_Rsa_Sha2_512 : public PCtxDsv_VerifierPublic_Rsa
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02614
    /// @needwork = dda
    /// @endcode
    explicit PCtxDsv_VerifierPublic_Rsa_Sha2_512(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02615
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_512() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02616
    /// @needwork = dda
    /// @endcode
    ~PCtxDsv_VerifierPublic_Rsa_Sha2_512() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02617
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_512(PCtxDsv_VerifierPublic_Rsa_Sha2_512 const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02618
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_512(PCtxDsv_VerifierPublic_Rsa_Sha2_512 &&other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02619
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_512 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_512 &&other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02620
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic_Rsa_Sha2_512 &operator=(PCtxDsv_VerifierPublic_Rsa_Sha2_512 const &other) noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02621
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Extended service member class.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02622
    /// @needwork = dda
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override;
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02623
    /// @needwork = dda
    /// @endcode
    bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_RSA_H_
