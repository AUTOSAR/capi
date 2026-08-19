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
/// @file       isoft_crypto_primitive_id.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Hash function interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21</td><td>1.0.0</td><td>hanjingjing</td><td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Crypto primitives
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PAlgId_Base
/// @unit_description=Crypto primitive ID encapsulation
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PRIMITIVE_ID_H_
#define ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PRIMITIVE_ID_H_

#include "ara/core/string.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
// PRQA S 2502 QAC /// @qac: false positive
namespace isoft_def {
// PRQA L:QAC
//********************************/
/// @brief Initialize "initialization vector"
/// @param pInitData Initialization data
/// @param nDataLen Data length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00764
/// @trace_id_dd=DD_CRYPTO_02732
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=InitVector
/// @needwork = ad
/// @endcode
void InitVector(uint8_t *const &pInitData, int32_t const nDataLen) noexcept;
/// @brief Puhua crypto context type definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00765
/// @trace_id_dd=DD_CRYPTO_02733
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=EPhCtxTypeID
/// @needwork = ad
/// @endcode
enum class EPhCtxTypeID : std::uint64_t
{
    /// @brief Undefined
    kUnDefine = static_cast< std::uint64_t >(kAlgIdUndefined),
    /// @brief Certificate slot specific ID
    kCertSlot = 1,
    /// @brief Random number generator
    kRandomGenerator = 1000,
    /// @brief Random number generator
    kRandomGeneratorglobal = kRandomGenerator + 1U,
    /// @brief Hash start
    kHashBegin = 3000,
    /// @brief Md5
    kHashMd5 = kHashBegin + 1U,
    /// @brief CRC8
    kHashCrc8 = kHashBegin + 11U,
    /// @brief CRC16
    kHashCrc16 = kHashBegin + 12U,
    /// @brief CRC32
    kHashCrc32 = kHashBegin + 13U,
    /// @brief CRC64
    kHashCrc64 = kHashBegin + 14U,
    /// @brief SHA-1
    kHashSha1 = kHashBegin + 21U,
    /// @brief SHA-224
    kHashSha2_224 = kHashBegin + 22U,
    /// @brief SHA-256
    kHashSha2_256 = kHashBegin + 23U,
    /// @brief SHA-384
    kHashSha2_384 = kHashBegin + 24U,
    /// @brief SHA-512
    kHashSha2_512 = kHashBegin + 25U,
    /// @brief SHA-3 //2022-01-19 Not implemented yet
    kHashSha3 = kHashBegin + 31U,
    /// @brief SHA2-1 File Hash
    kHashSha1_File = kHashBegin + 32U,
    /// @brief SHA-224 File Hash
    kHashSha2_224_File = kHashBegin + 33U,
    /// @brief SHA-256 File Hash
    kHashSha2_256_File = kHashBegin + 34U,

    /// @brief Symmetric encryption start // block cipher and stream cipher use the same ID
    kSymmetricBegin = 10000,
    /// @brief Des.Key
    kSymmetricDesKey = kSymmetricBegin + 100U,
    /// @brief Des.Ecb
    kSymmetricDesEcb = kSymmetricBegin + 101U,
    /// @brief Des.Cbc
    kSymmetricDesCbc = kSymmetricBegin + 102U,
    /// @brief Des.Cfb
    kSymmetricDesCfb = kSymmetricBegin + 103U,
    /// @brief Des.Cfb64
    kSymmetricDesCfb64 = kSymmetricBegin + 104U,
    /// @brief Des.Ofb
    kSymmetricDesOfb = kSymmetricBegin + 105U,
    /// @brief Des.Ofb64
    kSymmetricDesOfb64 = kSymmetricBegin + 106U,
    /// @brief Des.Key
    kSymmetric2DesKey = kSymmetricBegin + 109U,
    /// @brief Des.Key
    kSymmetric3DesKey = kSymmetricBegin + 110U,
    /// @brief Des.Ecb
    kSymmetric3DesEcb = kSymmetricBegin + 111U,
    /// @brief Des.Cbc
    kSymmetric3DesCbc = kSymmetricBegin + 112U,
    /// @brief Des.Cbc
    kSymmetric3DesCfb1 = kSymmetricBegin + 113U,
    /// @brief Des.Cbc
    kSymmetric3DesCfb64 = kSymmetricBegin + 114U,
    /// @brief Des.Cbc
    kSymmetric3DesOfb64 = kSymmetricBegin + 115U,

    /// @brief Aes.Key
    kSymmetricAesKey = kSymmetricBegin + 200U,
    /// @brief Aes.Key
    kSymmetricAesKey128 = kSymmetricBegin + 201U,
    /// @brief Aes.Key
    kSymmetricAesKey192 = kSymmetricBegin + 202U,
    /// @brief Aes.Key
    kSymmetricAesKey256 = kSymmetricBegin + 203U,
    /// @brief AES.Ecb
    kSymmetricAesEcb = kSymmetricBegin + 211U,
    /// @brief AES.Ecb-128
    kSymmetricAesEcb128 = kSymmetricBegin + 212U,
    /// @brief AES.Ecb-192
    kSymmetricAesEcb192 = kSymmetricBegin + 213U,
    /// @brief AES.Ecb-256
    kSymmetricAesEcb256 = kSymmetricBegin + 214U,
    /// @brief AES.Cbc
    kSymmetricAesCbc = kSymmetricBegin + 221U,
    /// @brief AES.Cbc-128
    kSymmetricAesCbc128 = kSymmetricBegin + 222U,
    /// @brief AES.Cbc-192
    kSymmetricAesCbc192 = kSymmetricBegin + 223U,
    /// @brief AES.Cbc-256
    kSymmetricAesCbc256 = kSymmetricBegin + 224U,
    /// @brief AES.Cfb
    kSymmetricAesCfb = kSymmetricBegin + 231U,
    /// @brief AES.Cfb-1
    kSymmetricAesCfb1 = kSymmetricBegin + 232U,
    /// @brief AES.Cfb-8
    kSymmetricAesCfb8 = kSymmetricBegin + 233U,
    /// @brief AES.Cfb-128
    kSymmetricAesCfb128 = kSymmetricBegin + 234U,
    /// @brief AES.Ofb
    kSymmetricAesOfb = kSymmetricBegin + 241U,
    /// @brief AES.Ofb-128
    kSymmetricAesOfb128 = kSymmetricBegin + 242U,
    /// @brief AES.Ctr
    kSymmetricAesCtr = kSymmetricBegin + 251U,
    /// @brief AES.Gcm
    kSymmetricAesGcm = kSymmetricBegin + 261U,
    /// @brief AES.HMAC-128
    kSymmetricAesKeyHmac = kSymmetricBegin + 271U,

    /// @brief Asymmetric encryption start
    kAsymmetricBegin = 20000,
    /// @brief RSA encryption key/decryption key: default length 1024
    /// @brief 2023.9.7 Abandoned kAsymmetricRsaKey —— Che
    kAsymmetricRsaKey = kAsymmetricBegin + 100U,
    /// @brief RSA encryption key/decryption key: length 512
    kAsymmetricRsaKey512 = kAsymmetricBegin + 101U,
    /// @brief RSA encryption key/decryption key: length 1024
    kAsymmetricRsaKey1024 = kAsymmetricBegin + 102U,
    /// @brief RSA encryption key/decryption key: length 2048
    kAsymmetricRsaKey2048 = kAsymmetricBegin + 103U,
    /// @brief RSA encryption key/decryption key: length 4096
    kAsymmetricRsaKey4096 = kAsymmetricBegin + 104U,
    /// @brief RSA encryption key/decryption key: length 8192
    kAsymmetricRsaKey8192 = kAsymmetricBegin + 105U,
    /// @brief RSA decryption/decryption algorithm
    /// @brief 2023.9.7 kAsymmetricRsa will be used to generate encryption/decryption contexts, distinguishing from keys —— Che
    kAsymmetricRsa = kAsymmetricBegin + 201U,

    /// @brief Currently has no effect
    /// @brief ECC encryption key/decryption key: default length 128, currently also has no effect
    kAsymmetricEccKey = kAsymmetricBegin + 203U,
    /// @brief
    kAsymmetricEccKey64 = kAsymmetricBegin + 204U,
    /// @brief ECC encryption key/decryption key: length 128
    kAsymmetricEccKey128 = kAsymmetricBegin + 205U,
    /// @brief ECC encryption key/decryption key: length 192
    kAsymmetricEccKey192 = kAsymmetricBegin + 206U,
    /// @brief ECC encryption key/decryption key: length 256
    kAsymmetricEccKey256 = kAsymmetricBegin + 207U,
    /// @brief RSA decryption/decryption algorithm
    /// @brief kAsymmetricEcc will be used to generate encryption/decryption contexts, distinguishing from keys —— Che
    kAsymmetricEcc = kAsymmetricBegin + 208U,

    /// @brief RSA OAEP encryption key/decryption key: default length 1024
    kAsymmetricRsaOaep = kAsymmetricBegin + 209U,

    /// @brief KDF
    kKdf_Begin = 30000,
    /// @brief Kdf for DES key
    kKdf_Des = kKdf_Begin + 101U,
    /// @brief Kdf for 3DES key
    kKdf_3Des = kKdf_Begin + 102U,
    /// @brief Kdf for AES key
    kKdf_Aes = kKdf_Begin + 103U,
    /// @brief Kdf for AES key
    kKdf_Aes128 = kKdf_Begin + 104U,
    /// @brief Kdf for AES key
    kKdf_Aes192 = kKdf_Begin + 105U,
    /// @brief Kdf for AES key
    kKdf_Aes256 = kKdf_Begin + 106U,

    /// @brief DSV == Digital Signatures and Verifier
    kDsv_Begin = 40000,
    /// @brief Signature object
    kDsv_SignatureObj = kDsv_Begin + 101U,
    /// @brief Message signing
    kDsv_SigEncodeRsa = kDsv_Begin + 102U,
    /// @brief Message verification
    kDsv_MsgRecoveryRsa = kDsv_Begin + 103U,

    /// @brief rsa private
    kDsv_SignerPrivateRsa = kDsv_Begin + 104U,
    /// @brief rsa md5
    kDsv_SignerPrivateRsaMd5 = kDsv_Begin + 105U,
    /// @brief rsa sha1
    kDsv_SignerPrivateRsaSha1 = kDsv_Begin + 106U,
    /// @brief rsa sha2-224
    kDsv_SignerPrivateRsaSha2_224 = kDsv_Begin + 107U,
    /// @brief rsa sha2-256
    kDsv_SignerPrivateRsaSha2_256 = kDsv_Begin + 108U,
    /// @brief rsa sha2-384
    kDsv_SignerPrivateRsaSha2_384 = kDsv_Begin + 109U,
    /// @brief rsa sha2-512
    kDsv_SignerPrivateRsaSha2_512 = kDsv_Begin + 110U,

    /// @brief PSS rsa
    kDsv_SignerPrivateRsa_Pss = kDsv_Begin + 111U,
    /// @brief PSS rsa md5
    kDsv_SignerPrivateRsa_PssMd5 = kDsv_Begin + 112U,
    /// @brief PSS rsa sha1
    kDsv_SignerPrivateRsa_PssSha1 = kDsv_Begin + 113U,
    /// @brief PSS rsa sha2-224
    kDsv_SignerPrivateRsa_PssSha2_224 = kDsv_Begin + 114U,
    /// @brief PSS rsa sha2-256
    kDsv_SignerPrivateRsa_PssSha2_256 = kDsv_Begin + 115U,
    /// @brief PSS rsa sha2-384
    kDsv_SignerPrivateRsa_PssSha2_384 = kDsv_Begin + 116U,
    /// @brief PSS rsa sha2-512
    kDsv_SignerPrivateRsa_PssSha2_512 = kDsv_Begin + 117U,

    /// @brief ecc
    kDsv_SignerPrivateEcdsa = kDsv_Begin + 118U,
    /// @brief ecc sha1
    kDsv_SignerPrivateEcdsaSha1 = kDsv_Begin + 119U,
    /// @brief ecc sha2-224
    kDsv_SignerPrivateEcdsaSha2_224 = kDsv_Begin + 120U,
    /// @brief ecc sha2-256
    kDsv_SignerPrivateEcdsaSha2_256 = kDsv_Begin + 121U,
    /// @brief ecc sha2-384
    kDsv_SignerPrivateEcdsaSha2_384 = kDsv_Begin + 122U,
    /// @brief ecc sha2-512
    kDsv_SignerPrivateEcdsaSha2_512 = kDsv_Begin + 123U,

    /// @brief Verify
    kDsv_VerifierPublicRsa = kDsv_Begin + 124U,
    /// @brief rsa Md5
    kDsv_VerifierPublicRsaMd5 = kDsv_Begin + 125U,
    /// @brief rsa sha1
    kDsv_VerifierPublicRsaSha1 = kDsv_Begin + 126U,
    /// @brief rsa sha2-224
    kDsv_VerifierPublicRsaSha2_224 = kDsv_Begin + 127U,
    /// @brief rsa sha2-256
    kDsv_VerifierPublicRsaSha2_256 = kDsv_Begin + 128U,
    /// @brief rsa sha2-384
    kDsv_VerifierPublicRsaSha2_384 = kDsv_Begin + 129U,
    /// @brief rsa sha2-512
    kDsv_VerifierPublicRsaSha2_512 = kDsv_Begin + 130U,

    /// @brief PSS
    kDsv_VerifierPublicRsa_Pss = kDsv_Begin + 131U,
    /// @brief PSS - Md5
    kDsv_VerifierPublicRsa_PssMd5 = kDsv_Begin + 132U,
    /// @brief PSS - sha1
    kDsv_VerifierPublicRsa_PssSha1 = kDsv_Begin + 133U,
    /// @brief PSS - sha2-224
    kDsv_VerifierPublicRsa_PssSha2_224 = kDsv_Begin + 134U,
    /// @brief PSS - sha2-256
    kDsv_VerifierPublicRsa_PssSha2_256 = kDsv_Begin + 135U,
    /// @brief PSS - sha2-384
    kDsv_VerifierPublicRsa_PssSha2_384 = kDsv_Begin + 136U,
    /// @brief PSS - sha2-512
    kDsv_VerifierPublicRsa_PssSha2_512 = kDsv_Begin + 137U,

    /// @brief ecc pub
    kDsv_VerifierPublicEcdsa = kDsv_Begin + 138U,
    /// @brief ecc pub sha1
    kDsv_VerifierPublicEcdsaSha1 = kDsv_Begin + 139U,
    /// @brief ecc pub sha2-224
    kDsv_VerifierPublicEcdsaSha2_224 = kDsv_Begin + 140U,
    /// @brief ecc pub sha2-256
    kDsv_VerifierPublicEcdsaSha2_256 = kDsv_Begin + 141U,
    /// @brief ecc pub sha2-384
    kDsv_VerifierPublicEcdsaSha2_384 = kDsv_Begin + 142U,
    /// @brief ecc pub sha2-512
    kDsv_VerifierPublicEcdsaSha2_512 = kDsv_Begin + 143U,

    /// @brief ecc hardware sha2-256
    kDsv_SignerPrivateEcdsaR5Sha2_256 = kDsv_Begin + 144U,
    /// @brief ecc hardware sha2-512
    kDsv_VerifierPublicEcdsaR5Sha2_256 = kDsv_Begin + 145U,

    /// @brief mac
    kMac_Begin = 50000,
    /// @brief mac aes cbc 128
    kMac_Aes_Cbc_128 = kMac_Begin + 101U,
    /// @brief mac aes cbc 192
    kMac_Aes_Cbc_192 = kMac_Begin + 102U,
    /// @brief mac aes cbc 256
    kMac_Aes_Cbc_256 = kMac_Begin + 103U,
    /// @brief mac des cbc
    kMac_Des_Cbc = kMac_Begin + 104U,
    /// @brief mac des cbc ed2
    kMac_Des_Cbc_ede2 = kMac_Begin + 105U,
    /// @brief mac des cbc ed3
    kMac_Des_Cbc_ede3 = kMac_Begin + 106U,
    /// @brief mac des cbc md5
    kMac_Hash_Md5 = kMac_Begin + 107U,
    /// @brief mac des cbc sha1
    kMac_Hash_Sha1 = kMac_Begin + 108U,
    /// @brief mac des cbc sha2-224
    kMac_Hash_Sha2_224 = kMac_Begin + 109U,
    /// @brief mac des cbc sha2-256
    kMac_Hash_Sha2_256 = kMac_Begin + 110U,
    /// @brief mac des cbc sha2-384
    kMac_Hash_Sha2_384 = kMac_Begin + 111U,
    /// @brief mac des cbc sha2-512
    kMac_Hash_Sha2_512 = kMac_Begin + 112U,

    /// @brief warpping
    kWrap_Begin = 60000,
    /// @brief warpping aes pad
    kWrap_Aes_pad = kWrap_Begin + 101U,
    /// @brief warpping aes pad 128
    kWrap_Aes_pad_128 = kWrap_Begin + 102U,
    /// @brief warpping aes pad 192
    kWrap_Aes_pad_192 = kWrap_Begin + 103U,
    /// @brief warpping aes pad 256
    kWrap_Aes_pad_256 = kWrap_Begin + 104U,
    /// @brief warpping unpad
    kWrap_Aes_unpad = kWrap_Begin + 105U,
    /// @brief warpping aes unpad 128
    kWrap_Aes_unpad_128 = kWrap_Begin + 106U,
    /// @brief warpping aes unpad 192
    kWrap_Aes_unpad_192 = kWrap_Begin + 107U,
    /// @brief warpping aes unpad 256
    kWrap_Aes_unpad_256 = kWrap_Begin + 108U,
    /// @brief warpping des
    kWrap_Des = kWrap_Begin + 109U,

    /// @brief kem
    kKem_Begin = 70000,
    /// @brief Kem decapsulator
    kKem_decapsulator = kKem_Begin + 101U,
    /// @brief Kem decapsulator rsa
    kKem_decapsulator_rsa = kKem_Begin + 102U,
    /// @brief Kem encapsulator
    kKem_encapsulator = kKem_Begin + 103U,
    /// @brief Kem encapsulator rsa
    kKem_encapsulator_rsa = kKem_Begin + 104U,

    /// @brief Key agreement
    kKey_agreement_Begin = 80000,
    /// @brief Key agreement private
    kKey_agreement_private = kKey_agreement_Begin + 101U,

    /// @brief ae
    kAe_Begin = 90000,
    /// @brief ae aes
    kAe_Aes = kAe_Begin + 101U,
    /// @brief ae des
    kAe_Des = kAe_Begin + 102U,

    /// @brief Secret seed
    kSecret_seedObj = 100000,

    /// @brief Start of next definition
    kNextDefine = 200000,
};
//********************************/
/// @brief Wrapper class for AlgID, used to distinguish various Ctx classes.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00766
/// @trace_id_dd=DD_CRYPTO_02734
/// @needwork = ad
/// @endcode
class PAlgId_Base : public CryptoPrimitiveId
{
private:
    /// @brief Crypto primitive name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02735
    /// @needwork = dda
    /// @endcode
    ara::core::String stAlgName_;

public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02736
    /// @needwork = dda
    /// @endcode
    PAlgId_Base() = delete;
    /// @brief default copy assignment operator
    /// @brief Copy-assign another PAlgId_Base to this instance.
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02737
    /// @needwork = dda
    /// @endcode
    PAlgId_Base &operator=(PAlgId_Base const &other) = delete;
    /// @brief default move assignment operator
    /// @brief Move-assign another PAlgId_Base to this instance.
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02738
    /// @needwork = dda
    /// @endcode
    PAlgId_Base &operator=(PAlgId_Base &&other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02739
    /// @needwork = dda
    /// @endcode
    PAlgId_Base(PAlgId_Base &&other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02740
    /// @needwork = dda
    /// @endcode
    PAlgId_Base(PAlgId_Base const &other) noexcept = delete;

public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00767
    /// @trace_id_dd=DD_CRYPTO_02741
    /// @needwork = ad
    /// @endcode
    ~PAlgId_Base() override = default;
    /// @brief SetAlgNamePre
    /// @param stAlgName Crypto primitive string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00768
    /// @trace_id_dd=DD_CRYPTO_02742
    /// @needwork = ad
    /// @endcode
    void SetAlgNamePre(ara::core::String const &stAlgName) noexcept { stAlgName_ = stAlgName + stAlgName_; }
    /// @brief UpdateAlgName
    /// @param stAlgName Crypto primitive string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00769
    /// @trace_id_dd=DD_CRYPTO_02743
    /// @needwork = ad
    /// @endcode
    void UpdateAlgName(ara::core::String const &stAlgName) noexcept { stAlgName_ += stAlgName; }

public:
    /// @brief the constructor
    /// @param nInitLen Length of initialization storage buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00770
    /// @trace_id_dd=DD_CRYPTO_02744
    /// @needwork = ad
    /// @endcode
    explicit PAlgId_Base(uint32_t const nInitLen) noexcept;
    /// @brief Get vendor-specific crypto primitive ID
    /// @brief Get vendor-specific ID of the crypto primitive
    /// @brief Get vendor specific ID of the primitive.
    /// @returns the binary Crypto Primitive ID
    /// @name GetPrimitiveId
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20652}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00771
    /// @trace_id_dd=DD_CRYPTO_02745
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Get the unified name of the primitive. The crypto primitive name can be fully specified or partially specified (see "Crypto Primitive Naming Convention" for details).
    ///         The lifetime of the returned StringView instance must not exceed the lifetime of this CryptoPrimitiveId instance!
    /// @brief Get a unified name of the primitive.
    ///         The crypto primitive name can be fully or partially specified (see "Crypto Primitives Naming Convention"
    ///         for more details). The life-time of the returned @c StringView instance should not exceed the life-time
    ///         of this @c CryptoPrimitiveId instance!
    /// @returns the unified name of the crypto primitive
    /// @name GetPrimitiveName
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @threadsafety={Thread-safe}
    /// @trace_id_sws={SWS_CRYPT_20651}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00772
    /// @trace_id_dd=DD_CRYPTO_02746
    /// @needwork = ad
    /// @endcode
    inline ara::core::StringView const GetPrimitiveName() const noexcept override
    {
        return {stAlgName_.data(), stAlgName_.size()};
    }

public:
    /// @brief Check if IDs are the same.
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Crypto primitive ID
    /// @returns true if same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00773
    /// @trace_id_dd=DD_CRYPTO_02747
    /// @needwork = ad
    /// @endcode
    virtual bool IsMinePrimitiveId(AlgId nAlgId) const noexcept;
    /// @brief Add key
    /// @name   AddKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00774
    /// @trace_id_dd=DD_CRYPTO_02748
    /// @needwork = ad
    /// @endcode
    inline void AddKey() noexcept;
    /// @brief Add des
    /// @name   AddDes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00775
    /// @trace_id_dd=DD_CRYPTO_02749
    /// @needwork = ad
    /// @endcode
    inline void AddDes() noexcept;
    /// @brief Add aes
    /// @name   AddAes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00776
    /// @trace_id_dd=DD_CRYPTO_02750
    /// @needwork = ad
    /// @endcode
    inline void AddAes() noexcept;
    /// @brief Add ecb
    /// @name   AddEcb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00777
    /// @trace_id_dd=DD_CRYPTO_02751
    /// @needwork = ad
    /// @endcode
    inline void AddEcb() noexcept;
    /// @brief Add cbc
    /// @name   AddCbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00778
    /// @trace_id_dd=DD_CRYPTO_02752
    /// @needwork = ad
    /// @endcode
    inline void AddCbc() noexcept;
    /// @brief Add ctr
    /// @name   AddCtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00779
    /// @trace_id_dd=DD_CRYPTO_02753
    /// @needwork = ad
    /// @endcode
    inline void AddCtr() noexcept;
    /// @brief Add gcm
    /// @name   AddGcm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00780
    /// @trace_id_dd=DD_CRYPTO_02754
    /// @needwork = ad
    /// @endcode
    inline void AddGcm() noexcept;
    /// @brief Add HMAC
    /// @name   AddHmac
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00781
    /// @trace_id_dd=DD_CRYPTO_02755
    /// @needwork = ad
    /// @endcode
    inline void AddHmac() noexcept;
    /// @brief Add cfb
    /// @name   AddCfb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00782
    /// @trace_id_dd=DD_CRYPTO_02756
    /// @needwork = ad
    /// @endcode
    inline void AddCfb() noexcept;
    /// @brief Add ofb
    /// @name   AddOfb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00783
    /// @trace_id_dd=DD_CRYPTO_02757
    /// @needwork = ad
    /// @endcode
    inline void AddOfb() noexcept;
    /// @brief Add rsa
    /// @name   AddRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00784
    /// @trace_id_dd=DD_CRYPTO_02758
    /// @needwork = ad
    /// @endcode
    inline void AddRsa() noexcept;
    /// @brief Add Pss
    /// @name   AddPss
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00785
    /// @trace_id_dd=DD_CRYPTO_02759
    /// @needwork = ad
    /// @endcode
    inline void AddPss() noexcept;
    /// @brief Add Crc
    /// @name   AddCrc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00786
    /// @trace_id_dd=DD_CRYPTO_02760
    /// @needwork = ad
    /// @endcode
    inline void AddCrc() noexcept;
    /// @brief Add Mac
    /// @name   AddMac
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00787
    /// @trace_id_dd=DD_CRYPTO_02761
    /// @needwork = ad
    /// @endcode
    inline void AddMac() noexcept;
    /// @brief Add Hash
    /// @name   AddHash
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00788
    /// @trace_id_dd=DD_CRYPTO_02762
    /// @needwork = ad
    /// @endcode
    inline void AddHash() noexcept;
    /// @brief Add sha
    /// @name   AddSha
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00789
    /// @trace_id_dd=DD_CRYPTO_02763
    /// @needwork = ad
    /// @endcode
    inline void AddSha() noexcept;
    /// @brief Add Kdf
    /// @name   AddKdf
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00790
    /// @trace_id_dd=DD_CRYPTO_02764
    /// @needwork = ad
    /// @endcode
    inline void AddKdf() noexcept;
    /// @brief Add Md5
    /// @name   AddMd5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00791
    /// @trace_id_dd=DD_CRYPTO_02765
    /// @needwork = ad
    /// @endcode
    inline void AddMd5() noexcept;
    /// @brief Add Pad
    /// @name   AddPad
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00792
    /// @trace_id_dd=DD_CRYPTO_02766
    /// @needwork = ad
    /// @endcode
    inline void AddPad() noexcept;
    /// @brief Add Ipc
    /// @name   AddIpc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00793
    /// @trace_id_dd=DD_CRYPTO_02767
    /// @needwork = ad
    /// @endcode
    inline void AddIpc() noexcept;
    /// @brief Add Ran
    /// @name   AddRan
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00794
    /// @trace_id_dd=DD_CRYPTO_02768
    /// @needwork = ad
    /// @endcode
    inline void AddRan() noexcept;
    /// @brief Add Dom
    /// @name   AddDom
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00795
    /// @trace_id_dd=DD_CRYPTO_02769
    /// @needwork = ad
    /// @endcode
    inline void AddDom() noexcept;
    /// @brief Add Ede
    /// @name   AddEde
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00796
    /// @trace_id_dd=DD_CRYPTO_02770
    /// @needwork = ad
    /// @endcode
    inline void AddEde() noexcept;
    /// @brief Add 64
    /// @name   Add64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00797
    /// @trace_id_dd=DD_CRYPTO_02771
    /// @needwork = ad
    /// @endcode
    inline void Add64() noexcept;
    /// @brief Add 128
    /// @name   Add128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00798
    /// @trace_id_dd=DD_CRYPTO_02772
    /// @needwork = ad
    /// @endcode
    inline void Add128() noexcept;
    /// @brief Add 192
    /// @name   Add192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00799
    /// @trace_id_dd=DD_CRYPTO_02773
    /// @needwork = ad
    /// @endcode
    inline void Add192() noexcept;
    /// @brief Add 224
    /// @name   Add224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00800
    /// @trace_id_dd=DD_CRYPTO_02774
    /// @needwork = ad
    /// @endcode
    inline void Add224() noexcept;
    /// @brief Add 256
    /// @name   Add256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00801
    /// @trace_id_dd=DD_CRYPTO_02775
    /// @needwork = ad
    /// @endcode
    inline void Add256() noexcept;
    /// @brief Add 384
    /// @name   Add384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00802
    /// @trace_id_dd=DD_CRYPTO_02776
    /// @needwork = ad
    /// @endcode
    inline void Add384() noexcept;
    /// @brief Add 512
    /// @name   Add512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00803
    /// @trace_id_dd=DD_CRYPTO_02777
    /// @needwork = ad
    /// @endcode
    inline void Add512() noexcept;
    /// @brief Add 1024
    /// @name   Add1024
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00804
    /// @trace_id_dd=DD_CRYPTO_02778
    /// @needwork = ad
    /// @endcode
    inline void Add1024() noexcept;
    /// @brief Add 2048
    /// @name   Add2048
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00805
    /// @trace_id_dd=DD_CRYPTO_02779
    /// @needwork = ad
    /// @endcode
    inline void Add2048() noexcept;
    /// @brief Add 4096
    /// @name   Add4096
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00806
    /// @trace_id_dd=DD_CRYPTO_02780
    /// @needwork = ad
    /// @endcode
    inline void Add4096() noexcept;
    /// @brief Add 8192
    /// @name   Add8192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00807
    /// @trace_id_dd=DD_CRYPTO_02781
    /// @needwork = ad
    /// @endcode
    inline void Add8192() noexcept;
};
//********************************/
/// @brief Crypto primitive: CertSlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00808
/// @trace_id_dd=DD_CRYPTO_02782
/// @needwork = ad
/// @endcode
class PAlgId_CertSlot : public PAlgId_Base
{
public:
    /// @brief constructor
    /// @name      PAlgId_CertSlot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00809
    /// @trace_id_dd=DD_CRYPTO_02783
    /// @needwork = ad
    /// @endcode
    PAlgId_CertSlot() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00810
    /// @trace_id_dd=DD_CRYPTO_02784
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ // Hash algorithm: CRC
/// @brief Crypto primitive: CrcBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00811
/// @trace_id_dd=DD_CRYPTO_02785
/// @needwork = ad
/// @endcode
class PAlgId_CrcBase : public PAlgId_Base
{
public:
    /// @brief constructor
    /// @name      PAlgId_CrcBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00812
    /// @trace_id_dd=DD_CRYPTO_02786
    /// @needwork = ad
    /// @endcode
    PAlgId_CrcBase() noexcept;
};
//***************/
/// @brief Crypto primitive: Crc8
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00813
/// @trace_id_dd=DD_CRYPTO_02787
/// @needwork = ad
/// @endcode
class PAlgId_Crc8 : public PAlgId_CrcBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Crc8
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00814
    /// @trace_id_dd=DD_CRYPTO_02788
    /// @needwork = ad
    /// @endcode
    PAlgId_Crc8() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00815
    /// @trace_id_dd=DD_CRYPTO_02789
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Crypto primitive: Crc16
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00816
/// @trace_id_dd=DD_CRYPTO_02790
/// @needwork = ad
/// @endcode
class PAlgId_Crc16 : public PAlgId_CrcBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Crc16
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00817
    /// @trace_id_dd=DD_CRYPTO_02791
    /// @needwork = ad
    /// @endcode
    PAlgId_Crc16() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00818
    /// @trace_id_dd=DD_CRYPTO_02792
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Crypto primitive: Crc32
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00819
/// @trace_id_dd=DD_CRYPTO_02793
/// @needwork = ad
/// @endcode
class PAlgId_Crc32 : public PAlgId_CrcBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Crc32
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00820
    /// @trace_id_dd=DD_CRYPTO_02794
    /// @needwork = ad
    /// @endcode
    PAlgId_Crc32() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00821
    /// @trace_id_dd=DD_CRYPTO_02795
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Crypto primitive: Crc64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00822
/// @trace_id_dd=DD_CRYPTO_02796
/// @needwork = ad
/// @endcode
class PAlgId_Crc64 : public PAlgId_CrcBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Crc64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00823
    /// @trace_id_dd=DD_CRYPTO_02797
    /// @needwork = ad
    /// @endcode
    PAlgId_Crc64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00824
    /// @trace_id_dd=DD_CRYPTO_02798
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ // Hash algorithm: Md5
/// @brief Crypto primitive: Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00825
/// @trace_id_dd=DD_CRYPTO_02799
/// @needwork = ad
/// @endcode
class PAlgId_Md5 : public PAlgId_Base
{
public:
    /// @brief constructor
    /// @name      PAlgId_Md5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00826
    /// @trace_id_dd=DD_CRYPTO_02800
    /// @needwork = ad
    /// @endcode
    PAlgId_Md5() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00827
    /// @trace_id_dd=DD_CRYPTO_02801
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ // Hash algorithm: SHA
/// @brief Crypto primitive: ShaBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00828
/// @trace_id_dd=DD_CRYPTO_02802
/// @needwork = ad
/// @endcode
class PAlgId_ShaBase : public PAlgId_Base
{
public:
    /// @brief constructor
    /// @name      PAlgId_ShaBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00829
    /// @trace_id_dd=DD_CRYPTO_02803
    /// @needwork = ad
    /// @endcode
    PAlgId_ShaBase() noexcept;
};
//***************/
/// @brief Crypto primitive: Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00830
/// @trace_id_dd=DD_CRYPTO_02804
/// @needwork = ad
/// @endcode
class PAlgId_Sha1 : public PAlgId_ShaBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00831
    /// @trace_id_dd=DD_CRYPTO_02805
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00832
    /// @trace_id_dd=DD_CRYPTO_02806
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Crypto primitive: Sha2
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00833
/// @trace_id_dd=DD_CRYPTO_02807
/// @needwork = ad
/// @endcode
class PAlgId_Sha2 : public PAlgId_ShaBase
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00834
    /// @trace_id_dd=DD_CRYPTO_02808
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2() noexcept;
};
/// @brief Crypto primitive: Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00835
/// @trace_id_dd=DD_CRYPTO_02809
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_224 : public PAlgId_Sha2
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00836
    /// @trace_id_dd=DD_CRYPTO_02810
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_224() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00837
    /// @trace_id_dd=DD_CRYPTO_02811
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00838
/// @trace_id_dd=DD_CRYPTO_02812
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_256 : public PAlgId_Sha2
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00839
    /// @trace_id_dd=DD_CRYPTO_02813
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00840
    /// @trace_id_dd=DD_CRYPTO_02814
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00841
/// @trace_id_dd=DD_CRYPTO_02815
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_384 : public PAlgId_Sha2
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00842
    /// @trace_id_dd=DD_CRYPTO_02816
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00843
    /// @trace_id_dd=DD_CRYPTO_02817
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00844
/// @trace_id_dd=DD_CRYPTO_02818
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_512 : public PAlgId_Sha2
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00845
    /// @trace_id_dd=DD_CRYPTO_02819
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00846
    /// @trace_id_dd=DD_CRYPTO_02820
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha1_File
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00847
/// @trace_id_dd=DD_CRYPTO_02821
/// @needwork = ad
/// @endcode
class PAlgId_Sha1_File : public PAlgId_Sha1
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha1_File
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00848
    /// @trace_id_dd=DD_CRYPTO_02822
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha1_File() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00849
    /// @trace_id_dd=DD_CRYPTO_02823
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha2_224_File
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00850
/// @trace_id_dd=DD_CRYPTO_02824
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_224_File : public PAlgId_Sha2_224
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2_224_File
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00851
    /// @trace_id_dd=DD_CRYPTO_02825
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_224_File() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00852
    /// @trace_id_dd=DD_CRYPTO_02826
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Sha2_256_File
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00853
/// @trace_id_dd=DD_CRYPTO_02827
/// @needwork = ad
/// @endcode
class PAlgId_Sha2_256_File : public PAlgId_Sha2_256
{
public:
    /// @brief constructor
    /// @name      PAlgId_Sha2_256_File
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00854
    /// @trace_id_dd=DD_CRYPTO_02828
    /// @needwork = ad
    /// @endcode
    PAlgId_Sha2_256_File() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00855
    /// @trace_id_dd=DD_CRYPTO_02829
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ //Symmetric Encryption: DES
/// @brief Encryption Primitive: Symmetric_DesBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00856
/// @trace_id_dd=DD_CRYPTO_02830
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_DesBase : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_DesBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00857
    /// @trace_id_dd=DD_CRYPTO_02831
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_DesBase() noexcept;
};
/// @brief Encryption Primitive: Symmetric_DesKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00858
/// @trace_id_dd=DD_CRYPTO_02832
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_DesKey : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief the key length : bit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00859
    /// @trace_id_dd=DD_CRYPTO_02833
    /// @needwork = ad
    /// @endcode
    enum class EKeyLen : std::uint32_t
    {
        /// @brief the des block size : 8bit
        kDesBlockSize = kInt_8U,
        /// @brief the des key min length : 64bit
        kDesKeyMinBitLength = 64U,
        /// @brief the des key max length : 192bit
        kDesKeyMaxBitLength = 192U,
    };

public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_DesKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00860
    /// @trace_id_dd=DD_CRYPTO_02834
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_DesKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00861
    /// @trace_id_dd=DD_CRYPTO_02835
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_3DesKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00862
/// @trace_id_dd=DD_CRYPTO_02836
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3DesKey : public PAlgId_Symmetric_DesKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3DesKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00863
    /// @trace_id_dd=DD_CRYPTO_02837
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3DesKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00864
    /// @trace_id_dd=DD_CRYPTO_02838
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Symmetric_2DesKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00865
/// @trace_id_dd=DD_CRYPTO_02839
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_2DesKey : public PAlgId_Symmetric_DesKey
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00866
    /// @trace_id_dd=DD_CRYPTO_02840
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_2DesKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00867
    /// @trace_id_dd=DD_CRYPTO_02841
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm ECB Mode
/// @brief Encryption Primitive: Symmetric_Des_Ecb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00868
/// @trace_id_dd=DD_CRYPTO_02842
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Ecb : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00869
    /// @trace_id_dd=DD_CRYPTO_02843
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Ecb() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00870
    /// @trace_id_dd=DD_CRYPTO_02844
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm CBC Mode
/// @brief Encryption Primitive: Symmetric_Des_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00871
/// @trace_id_dd=DD_CRYPTO_02845
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Cbc : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00872
    /// @trace_id_dd=DD_CRYPTO_02846
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Cbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00873
    /// @trace_id_dd=DD_CRYPTO_02847
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm Cfb Mode
/// @brief Encryption Primitive: Symmetric_Des_Cfb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00874
/// @trace_id_dd=DD_CRYPTO_02848
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Cfb : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Cfb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00875
    /// @trace_id_dd=DD_CRYPTO_02849
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Cfb() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00876
    /// @trace_id_dd=DD_CRYPTO_02850
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm Cfb64 Mode
/// @brief Encryption Primitive: Symmetric_Des_Cfb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00877
/// @trace_id_dd=DD_CRYPTO_02851
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Cfb64 : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Cfb64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00878
    /// @trace_id_dd=DD_CRYPTO_02852
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Cfb64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00879
    /// @trace_id_dd=DD_CRYPTO_02853
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm Ofb Mode
/// @brief Encryption Primitive: Symmetric_Des_Ofb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00880
/// @trace_id_dd=DD_CRYPTO_02854
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Ofb : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Ofb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00881
    /// @trace_id_dd=DD_CRYPTO_02855
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Ofb() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00882
    /// @trace_id_dd=DD_CRYPTO_02856
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //DES Algorithm Ofb64 Mode
/// @brief Encryption Primitive: Symmetric_Des_Ofb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00883
/// @trace_id_dd=DD_CRYPTO_02857
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Des_Ofb64 : public PAlgId_Symmetric_DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Des_Ofb64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00884
    /// @trace_id_dd=DD_CRYPTO_02858
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Des_Ofb64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00885
    /// @trace_id_dd=DD_CRYPTO_02859
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: Symmetric_3DesBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00886
/// @trace_id_dd=DD_CRYPTO_02860
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3DesBase : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3DesBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00887
    /// @trace_id_dd=DD_CRYPTO_02861
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3DesBase() noexcept;
};
// 3DES Algorithm ECB Mode
/// @brief Encryption Primitive: Symmetric_3Des_Ecb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00888
/// @trace_id_dd=DD_CRYPTO_02862
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3Des_Ecb : public PAlgId_Symmetric_3DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00889
    /// @trace_id_dd=DD_CRYPTO_02863
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3Des_Ecb() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00890
    /// @trace_id_dd=DD_CRYPTO_02864
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //3DES Algorithm CBC Mode
/// @brief Encryption Primitive: Symmetric_3Des_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00891
/// @trace_id_dd=DD_CRYPTO_02865
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3Des_Cbc : public PAlgId_Symmetric_3DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3Des_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00892
    /// @trace_id_dd=DD_CRYPTO_02866
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3Des_Cbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00893
    /// @trace_id_dd=DD_CRYPTO_02867
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //3DES Algorithm CFB Mode
/// @brief Encryption Primitive: Symmetric_3Des_Cfb1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00894
/// @trace_id_dd=DD_CRYPTO_02868
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3Des_Cfb1 : public PAlgId_Symmetric_3DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3Des_Cfb1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00895
    /// @trace_id_dd=DD_CRYPTO_02869
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3Des_Cfb1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00896
    /// @trace_id_dd=DD_CRYPTO_02870
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_3Des_Cfb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00897
/// @trace_id_dd=DD_CRYPTO_02871
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3Des_Cfb64 : public PAlgId_Symmetric_3DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3Des_Cfb64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00898
    /// @trace_id_dd=DD_CRYPTO_02872
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3Des_Cfb64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00899
    /// @trace_id_dd=DD_CRYPTO_02873
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //3DES Algorithm CFB Mode
/// @brief Encryption Primitive: Symmetric_3Des_Ofb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00900
/// @trace_id_dd=DD_CRYPTO_02874
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_3Des_Ofb64 : public PAlgId_Symmetric_3DesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_3Des_Ofb64
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00901
    /// @trace_id_dd=DD_CRYPTO_02875
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_3Des_Ofb64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00902
    /// @trace_id_dd=DD_CRYPTO_02876
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ //Symmetric Encryption: AES Key
/// @brief Encryption Primitive: Symmetric_AesBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00903
/// @trace_id_dd=DD_CRYPTO_02877
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesBase : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00904
    /// @trace_id_dd=DD_CRYPTO_02878
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesBase() noexcept;
};
/// @brief Encryption Primitive: Symmetric_AesKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00905
/// @trace_id_dd=DD_CRYPTO_02879
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesKey : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief the key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00906
    /// @trace_id_dd=DD_CRYPTO_02880
    /// @needwork = ad
    /// @endcode
    enum class EKeyLen : std::int32_t
    {
        /// @brief block size : bit
        kAesBlockSize = 16,
        /// @brief no length
        kKeyBitLength_No = 0,
        /// @brief length: 128bit
        kKeyBitLength_128 = 128,
        /// @brief length: 192bit
        kKeyBitLength_192 = 192,
        /// @brief length: 256it
        kKeyBitLength_256 = 256,
        /// @brief default length: 128bit
        kKeyBitLength_Def = kKeyBitLength_128,
        /// @brief Supports any of 128, 192, 256 bits; here '|' expresses compatibility with three lengths, value is meaningless
        kKeyBitLength_Any = static_cast< std::uint32_t >(kKeyBitLength_128)
                            | static_cast< std::uint32_t >(kKeyBitLength_192)
                            | static_cast< std::uint32_t >(kKeyBitLength_256),
    };

public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00907
    /// @trace_id_dd=DD_CRYPTO_02881
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00908
    /// @trace_id_dd=DD_CRYPTO_02882
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00909
    /// @trace_id_dd=DD_CRYPTO_02883
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Symmetric_AesKeyHMAC
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00910
/// @trace_id_dd=DD_CRYPTO_02884
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesKeyHMAC : public PAlgId_Symmetric_AesKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesKeyHMAC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00911
    /// @trace_id_dd=DD_CRYPTO_02885
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesKeyHMAC() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00912
    /// @trace_id_dd=DD_CRYPTO_02886
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @param nAlgId Encryption Primitive ID
    /// @return true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00913
    /// @trace_id_dd=DD_CRYPTO_02887
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Symmetric_AesKey128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00914
/// @trace_id_dd=DD_CRYPTO_02888
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesKey128 : public PAlgId_Symmetric_AesKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesKey128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00915
    /// @trace_id_dd=DD_CRYPTO_02889
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesKey128() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00916
    /// @trace_id_dd=DD_CRYPTO_02890
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00917
    /// @trace_id_dd=DD_CRYPTO_02891
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_AesKey192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00918
/// @trace_id_dd=DD_CRYPTO_02892
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesKey192 : public PAlgId_Symmetric_AesKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesKey192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00919
    /// @trace_id_dd=DD_CRYPTO_02893
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesKey192() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00920
    /// @trace_id_dd=DD_CRYPTO_02894
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00921
    /// @trace_id_dd=DD_CRYPTO_02895
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_AesKey256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00922
/// @trace_id_dd=DD_CRYPTO_02896
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_AesKey256 : public PAlgId_Symmetric_AesKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_AesKey256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00923
    /// @trace_id_dd=DD_CRYPTO_02897
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_AesKey256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00924
    /// @trace_id_dd=DD_CRYPTO_02898
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00925
    /// @trace_id_dd=DD_CRYPTO_02899
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//***************/  //AES.Ecb-128/192/256
/// @brief Encryption Primitive: Symmetric_Aes_Ecb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00926
/// @trace_id_dd=DD_CRYPTO_02900
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ecb : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00927
    /// @trace_id_dd=DD_CRYPTO_02901
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ecb() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00928
    /// @trace_id_dd=DD_CRYPTO_02902
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Ecb_128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04313
/// @trace_id_dd=DD_CRYPTO_08664
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ecb_128 : public PAlgId_Symmetric_Aes_Ecb
{
public:
    /// @brief  Constructor
    /// @name   PAlgId_Symmetric_Aes_Ecb_128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04321
    /// @trace_id_dd=DD_CRYPTO_08672
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ecb_128() noexcept;
    /// @brief  Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04322
    /// @trace_id_dd=DD_CRYPTO_08673
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Ecb_192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04314
/// @trace_id_dd=DD_CRYPTO_08665
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ecb_192 : public PAlgId_Symmetric_Aes_Ecb
{
public:
    /// @brief  Constructor
    /// @name      PAlgId_Symmetric_Aes_Ecb_192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04323
    /// @trace_id_dd=DD_CRYPTO_08674
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ecb_192() noexcept;
    /// @brief  Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04324
    /// @trace_id_dd=DD_CRYPTO_08675
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Ecb_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04315
/// @trace_id_dd=DD_CRYPTO_08666
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ecb_256 : public PAlgId_Symmetric_Aes_Ecb
{
public:
    /// @brief  Constructor
    /// @name      PAlgId_Symmetric_Aes_Ecb_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04325
    /// @trace_id_dd=DD_CRYPTO_08676
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ecb_256() noexcept;
    /// @brief  Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns    Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04326
    /// @trace_id_dd=DD_CRYPTO_08677
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};

//***************/  //AES.Cbc-128/Cbc-192/Cbc-256
/// @brief Encryption Primitive: Symmetric_Aes_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00929
/// @trace_id_dd=DD_CRYPTO_02903
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cbc : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00930
    /// @trace_id_dd=DD_CRYPTO_02904
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00931
    /// @trace_id_dd=DD_CRYPTO_02905
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Cbc_128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04316
/// @trace_id_dd=DD_CRYPTO_08667
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cbc_128 : public PAlgId_Symmetric_Aes_Cbc
{
public:
    /// @brief Constructor
    /// @name  PAlgId_Symmetric_Aes_Cbc_128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04327
    /// @trace_id_dd=DD_CRYPTO_08678
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cbc_128() noexcept;
    /// @brief  Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04328
    /// @trace_id_dd=DD_CRYPTO_08679
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Cbc_192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04317
/// @trace_id_dd=DD_CRYPTO_08668
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cbc_192 : public PAlgId_Symmetric_Aes_Cbc
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cbc_192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04329
    /// @trace_id_dd=DD_CRYPTO_08680
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cbc_192() noexcept;
    /// @brief  Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04330
    /// @trace_id_dd=DD_CRYPTO_08681
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief PAlgId_Symmetric_Aes_Cbc_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04318
/// @trace_id_dd=DD_CRYPTO_08669
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cbc_256 : public PAlgId_Symmetric_Aes_Cbc
{
public:
    /// @brief  Constructor
    /// @name   PAlgId_Symmetric_Aes_Cbc_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04331
    /// @trace_id_dd=DD_CRYPTO_08682
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cbc_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04332
    /// @trace_id_dd=DD_CRYPTO_08683
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};

//***************/  //AES.Ctr-128/Ctr-192/Ctr-256
/// @brief Encryption Primitive: Symmetric_Aes_Ctr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00932
/// @trace_id_dd=DD_CRYPTO_02906
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ctr : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Ctr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00933
    /// @trace_id_dd=DD_CRYPTO_02907
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ctr() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00934
    /// @trace_id_dd=DD_CRYPTO_02908
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //AES.Gcm-128/Gcm-192/Gcm-256
/// @brief Encryption Primitive: Symmetric_Aes_Gcm
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00935
/// @trace_id_dd=DD_CRYPTO_02909
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Gcm : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Gcm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00936
    /// @trace_id_dd=DD_CRYPTO_02910
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Gcm() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00937
    /// @trace_id_dd=DD_CRYPTO_02911
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //AES.Cfb-1/Cfb-8/Cfb-128
/// @brief Encryption Primitive: Symmetric_Aes_Cfb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00938
/// @trace_id_dd=DD_CRYPTO_02912
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cfb : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cfb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00939
    /// @trace_id_dd=DD_CRYPTO_02913
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cfb() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00940
    /// @trace_id_dd=DD_CRYPTO_02914
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_Aes_Cfb1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00941
/// @trace_id_dd=DD_CRYPTO_02915
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cfb1 : public PAlgId_Symmetric_Aes_Cfb
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cfb1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00942
    /// @trace_id_dd=DD_CRYPTO_02916
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cfb1() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00943
    /// @trace_id_dd=DD_CRYPTO_02917
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_Aes_Cfb8
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00944
/// @trace_id_dd=DD_CRYPTO_02918
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cfb8 : public PAlgId_Symmetric_Aes_Cfb
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cfb8
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00945
    /// @trace_id_dd=DD_CRYPTO_02919
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cfb8() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00946
    /// @trace_id_dd=DD_CRYPTO_02920
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_Aes_Cfb128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00947
/// @trace_id_dd=DD_CRYPTO_02921
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Cfb128 : public PAlgId_Symmetric_Aes_Cfb
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Cfb128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00948
    /// @trace_id_dd=DD_CRYPTO_02922
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Cfb128() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00949
    /// @trace_id_dd=DD_CRYPTO_02923
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/  //AES.Ofb-128
/// @brief Encryption Primitive: Symmetric_Aes_Ofb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00950
/// @trace_id_dd=DD_CRYPTO_02924
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ofb : public PAlgId_Symmetric_AesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Ofb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00951
    /// @trace_id_dd=DD_CRYPTO_02925
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ofb() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00952
    /// @trace_id_dd=DD_CRYPTO_02926
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Symmetric_Aes_Ofb128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00953
/// @trace_id_dd=DD_CRYPTO_02927
/// @needwork = ad
/// @endcode
class PAlgId_Symmetric_Aes_Ofb128 : public PAlgId_Symmetric_Aes_Ofb
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Symmetric_Aes_Ofb128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00954
    /// @trace_id_dd=DD_CRYPTO_02928
    /// @needwork = ad
    /// @endcode
    PAlgId_Symmetric_Aes_Ofb128() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00955
    /// @trace_id_dd=DD_CRYPTO_02929
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Asymmetric_Rsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00956
/// @trace_id_dd=DD_CRYPTO_02930
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00957
    /// @trace_id_dd=DD_CRYPTO_02931
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns  specific ID of the primitive
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00958
    /// @trace_id_dd=DD_CRYPTO_02932
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Asymmetric_Rsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00959
/// @trace_id_dd=DD_CRYPTO_02933
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa_Sha1 : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Rsa_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00960
    /// @trace_id_dd=DD_CRYPTO_02934
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa_Sha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00961
    /// @trace_id_dd=DD_CRYPTO_02935
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Asymmetric_Rsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00962
/// @trace_id_dd=DD_CRYPTO_02936
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa_Sha2_256 : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Rsa_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00963
    /// @trace_id_dd=DD_CRYPTO_02937
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00964
    /// @trace_id_dd=DD_CRYPTO_02938
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Asymmetric_Rsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00965
/// @trace_id_dd=DD_CRYPTO_02939
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa_Sha2_384 : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Rsa_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00966
    /// @trace_id_dd=DD_CRYPTO_02940
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa_Sha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00967
    /// @trace_id_dd=DD_CRYPTO_02941
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Asymmetric_Rsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00968
/// @trace_id_dd=DD_CRYPTO_02942
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa_Sha2_512 : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Rsa_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00969
    /// @trace_id_dd=DD_CRYPTO_02943
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa_Sha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00970
    /// @trace_id_dd=DD_CRYPTO_02944
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};

//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00971
/// @trace_id_dd=DD_CRYPTO_02945
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief the key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00972
    /// @trace_id_dd=DD_CRYPTO_02946
    /// @needwork = ad
    /// @endcode
    enum class EKeyLen : std::int32_t
    {
        /// @brief the rsa block step
        kRsaBlockStep = 512,
        /// @brief the rsa key min length
        kRsaKeyMinBitLength = 512,
        /// @brief the rsa key max length
        kRsaKeyMaxBitLength = 10240,
    };
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00973
    /// @trace_id_dd=DD_CRYPTO_02947
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00974
    /// @trace_id_dd=DD_CRYPTO_02948
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00975
    /// @trace_id_dd=DD_CRYPTO_02949
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00976
/// @trace_id_dd=DD_CRYPTO_02950
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey512 : public PAlgId_Asymmetric_RsaKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00977
    /// @trace_id_dd=DD_CRYPTO_02951
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns nAlgID Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00978
    /// @trace_id_dd=DD_CRYPTO_02952
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey1024
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00979
/// @trace_id_dd=DD_CRYPTO_02953
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey1024 : public PAlgId_Asymmetric_RsaKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey1024
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00980
    /// @trace_id_dd=DD_CRYPTO_02954
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey1024() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns nAlgID Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00981
    /// @trace_id_dd=DD_CRYPTO_02955
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey2048
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00982
/// @trace_id_dd=DD_CRYPTO_02956
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey2048 : public PAlgId_Asymmetric_RsaKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey2048
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00983
    /// @trace_id_dd=DD_CRYPTO_02957
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey2048() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00984
    /// @trace_id_dd=DD_CRYPTO_02958
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey4096
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00985
/// @trace_id_dd=DD_CRYPTO_02959
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey4096 : public PAlgId_Asymmetric_RsaKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey4096
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00986
    /// @trace_id_dd=DD_CRYPTO_02960
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey4096() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00987
    /// @trace_id_dd=DD_CRYPTO_02961
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_RsaKey8192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00988
/// @trace_id_dd=DD_CRYPTO_02962
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_RsaKey8192 : public PAlgId_Asymmetric_RsaKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_RsaKey8192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00989
    /// @trace_id_dd=DD_CRYPTO_02963
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_RsaKey8192() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00990
    /// @trace_id_dd=DD_CRYPTO_02964
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Asymmetric_Rsa_Oaep
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00991
/// @trace_id_dd=DD_CRYPTO_02965
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Rsa_Oaep : public PAlgId_Asymmetric_Rsa
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00992
    /// @trace_id_dd=DD_CRYPTO_02966
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Rsa_Oaep() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00993
    /// @trace_id_dd=DD_CRYPTO_02967
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ ECC
/// @brief Encryption Primitive: Asymmetric_Ecc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00994
/// @trace_id_dd=DD_CRYPTO_02968
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_Ecc : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_Ecc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00995
    /// @trace_id_dd=DD_CRYPTO_02969
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_Ecc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00996
    /// @trace_id_dd=DD_CRYPTO_02970
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Asymmetric_EccKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00997
/// @trace_id_dd=DD_CRYPTO_02971
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_EccKey : public PAlgId_Asymmetric_Ecc
{
public:
    /// @brief the key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00998
    /// @trace_id_dd=DD_CRYPTO_02972
    /// @needwork = ad
    /// @endcode
    enum class EKeyLen : std::int32_t
    {
        /// @brief ecc key min length
        kEccKeyMinBitLength = 64,
        /// @brief ecc key max length
        kEccKeyMaxBitLength = 256,
        /// @brief the ecc block step
        kEccBlockStep = 64
    };
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00999
    /// @trace_id_dd=DD_CRYPTO_02973
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_EccKey() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01000
    /// @trace_id_dd=DD_CRYPTO_02974
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @param nAlgId Encryption Primitive ID
    /// @return true if algid is same false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01001
    /// @trace_id_dd=DD_CRYPTO_02975
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//********************************/ ECC64
/// @brief Encryption Primitive: Asymmetric_EccKey64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01002
/// @trace_id_dd=DD_CRYPTO_02976
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_EccKey64 : public PAlgId_Asymmetric_EccKey
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01003
    /// @trace_id_dd=DD_CRYPTO_02977
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_EccKey64() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01004
    /// @trace_id_dd=DD_CRYPTO_02978
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ ECC128
/// @brief Encryption Primitive: Asymmetric_EccKey128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01005
/// @trace_id_dd=DD_CRYPTO_02979
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_EccKey128 : public PAlgId_Asymmetric_EccKey
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Asymmetric_EccKey128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01006
    /// @trace_id_dd=DD_CRYPTO_02980
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_EccKey128() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01007
    /// @trace_id_dd=DD_CRYPTO_02981
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ ECC192
/// @brief Encryption Primitive: Asymmetric_EccKey192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01008
/// @trace_id_dd=DD_CRYPTO_02982
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_EccKey192 : public PAlgId_Asymmetric_EccKey
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01009
    /// @trace_id_dd=DD_CRYPTO_02983
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_EccKey192() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01010
    /// @trace_id_dd=DD_CRYPTO_02984
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ ECC256
/// @brief Encryption Primitive: Asymmetric_EccKey256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01011
/// @trace_id_dd=DD_CRYPTO_02985
/// @needwork = ad
/// @endcode
class PAlgId_Asymmetric_EccKey256 : public PAlgId_Asymmetric_EccKey
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01012
    /// @trace_id_dd=DD_CRYPTO_02986
    /// @needwork = ad
    /// @endcode
    PAlgId_Asymmetric_EccKey256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01013
    /// @trace_id_dd=DD_CRYPTO_02987
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: RandomGenerator
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01014
/// @trace_id_dd=DD_CRYPTO_02988
/// @needwork = ad
/// @endcode
class PAlgId_RandomGenerator : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_RandomGenerator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01015
    /// @trace_id_dd=DD_CRYPTO_02989
    /// @needwork = ad
    /// @endcode
    PAlgId_RandomGenerator() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01016
    /// @trace_id_dd=DD_CRYPTO_02990
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: RandomGenerator_Global
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01017
/// @trace_id_dd=DD_CRYPTO_02991
/// @needwork = ad
/// @endcode
class PAlgId_RandomGenerator_Global : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_RandomGenerator_Global
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01018
    /// @trace_id_dd=DD_CRYPTO_02992
    /// @needwork = ad
    /// @endcode
    PAlgId_RandomGenerator_Global() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01019
    /// @trace_id_dd=DD_CRYPTO_02993
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: Kdf_Base
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01020
/// @trace_id_dd=DD_CRYPTO_02994
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Base : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name   PAlgId_Kdf_Base
    /// @param nInitLen Initialize storage buffer length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01021
    /// @trace_id_dd=DD_CRYPTO_02995
    /// @needwork = ad
    /// @endcode
    explicit PAlgId_Kdf_Base(uint32_t const nInitLen) noexcept;
};
//***************/
/// @brief Encryption Primitive: Kdf_Des
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01022
/// @trace_id_dd=DD_CRYPTO_02996
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Des : public PAlgId_Kdf_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_Des
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01023
    /// @trace_id_dd=DD_CRYPTO_02997
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_Des() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01024
    /// @trace_id_dd=DD_CRYPTO_02998
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Kdf_3Des
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01025
/// @trace_id_dd=DD_CRYPTO_02999
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_3Des : public PAlgId_Kdf_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_3Des
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01026
    /// @trace_id_dd=DD_CRYPTO_03000
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_3Des() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01027
    /// @trace_id_dd=DD_CRYPTO_03001
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Kdf_Aes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01028
/// @trace_id_dd=DD_CRYPTO_03002
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Aes : public PAlgId_Kdf_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_Aes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01029
    /// @trace_id_dd=DD_CRYPTO_03003
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_Aes() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01030
    /// @trace_id_dd=DD_CRYPTO_03004
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kdf_Aes128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01031
/// @trace_id_dd=DD_CRYPTO_03005
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Aes128 : public PAlgId_Kdf_Aes
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_Aes128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01032
    /// @trace_id_dd=DD_CRYPTO_03006
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_Aes128() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01033
    /// @trace_id_dd=DD_CRYPTO_03007
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kdf_Aes192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01034
/// @trace_id_dd=DD_CRYPTO_03008
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Aes192 : public PAlgId_Kdf_Aes
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_Aes192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01035
    /// @trace_id_dd=DD_CRYPTO_03009
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_Aes192() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01036
    /// @trace_id_dd=DD_CRYPTO_03010
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kdf_Aes256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01037
/// @trace_id_dd=DD_CRYPTO_03011
/// @needwork = ad
/// @endcode
class PAlgId_Kdf_Aes256 : public PAlgId_Kdf_Aes
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kdf_Aes256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01038
    /// @trace_id_dd=DD_CRYPTO_03012
    /// @needwork = ad
    /// @endcode
    PAlgId_Kdf_Aes256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01039
    /// @trace_id_dd=DD_CRYPTO_03013
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: SignatureObj
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01040
/// @trace_id_dd=DD_CRYPTO_03014
/// @needwork = ad
/// @endcode
class PAlgId_SignatureObj : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_SignatureObj
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01041
    /// @trace_id_dd=DD_CRYPTO_03015
    /// @needwork = ad
    /// @endcode
    PAlgId_SignatureObj() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01042
    /// @trace_id_dd=DD_CRYPTO_03016
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SigEncodePrivateRsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01043
/// @trace_id_dd=DD_CRYPTO_03017
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SigEncodePrivateRsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SigEncodePrivateRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01044
    /// @trace_id_dd=DD_CRYPTO_03018
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SigEncodePrivateRsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01045
    /// @trace_id_dd=DD_CRYPTO_03019
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_MsgRecoveryPublicRsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01046
/// @trace_id_dd=DD_CRYPTO_03020
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_MsgRecoveryPublicRsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_MsgRecoveryPublicRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01047
    /// @trace_id_dd=DD_CRYPTO_03021
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_MsgRecoveryPublicRsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01048
    /// @trace_id_dd=DD_CRYPTO_03022
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01049
/// @trace_id_dd=DD_CRYPTO_03023
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01050
    /// @trace_id_dd=DD_CRYPTO_03024
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01051
    /// @trace_id_dd=DD_CRYPTO_03025
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01052
/// @trace_id_dd=DD_CRYPTO_03026
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Sha1 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01053
    /// @trace_id_dd=DD_CRYPTO_03027
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Sha1() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01054
    /// @trace_id_dd=DD_CRYPTO_03028
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01055
/// @trace_id_dd=DD_CRYPTO_03029
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Sha2_224 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01056
    /// @trace_id_dd=DD_CRYPTO_03030
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Sha2_224() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01057
    /// @trace_id_dd=DD_CRYPTO_03031
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01058
/// @trace_id_dd=DD_CRYPTO_03032
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Sha2_256 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01059
    /// @trace_id_dd=DD_CRYPTO_03033
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Sha2_256() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01060
    /// @trace_id_dd=DD_CRYPTO_03034
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01061
/// @trace_id_dd=DD_CRYPTO_03035
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Sha2_384 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01062
    /// @trace_id_dd=DD_CRYPTO_03036
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Sha2_384() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01063
    /// @trace_id_dd=DD_CRYPTO_03037
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01064
/// @trace_id_dd=DD_CRYPTO_03038
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Sha2_512 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01065
    /// @trace_id_dd=DD_CRYPTO_03039
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Sha2_512() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01066
    /// @trace_id_dd=DD_CRYPTO_03040
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01067
/// @trace_id_dd=DD_CRYPTO_03041
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01068
    /// @trace_id_dd=DD_CRYPTO_03042
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01069
    /// @trace_id_dd=DD_CRYPTO_03043
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_MD5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01070
/// @trace_id_dd=DD_CRYPTO_03044
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_MD5 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_MD5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01071
    /// @trace_id_dd=DD_CRYPTO_03045
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_MD5() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01072
    /// @trace_id_dd=DD_CRYPTO_03046
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01073
/// @trace_id_dd=DD_CRYPTO_03047
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01074
    /// @trace_id_dd=DD_CRYPTO_03048
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01075
    /// @trace_id_dd=DD_CRYPTO_03049
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01076
/// @trace_id_dd=DD_CRYPTO_03050
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01077
    /// @trace_id_dd=DD_CRYPTO_03051
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01078
    /// @trace_id_dd=DD_CRYPTO_03052
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01079
/// @trace_id_dd=DD_CRYPTO_03053
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01080
    /// @trace_id_dd=DD_CRYPTO_03054
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01081
    /// @trace_id_dd=DD_CRYPTO_03055
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01082
/// @trace_id_dd=DD_CRYPTO_03056
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01083
    /// @trace_id_dd=DD_CRYPTO_03057
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01084
    /// @trace_id_dd=DD_CRYPTO_03058
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01085
/// @trace_id_dd=DD_CRYPTO_03059
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01086
    /// @trace_id_dd=DD_CRYPTO_03060
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01087
    /// @trace_id_dd=DD_CRYPTO_03061
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Pss_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01088
/// @trace_id_dd=DD_CRYPTO_03062
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Pss_Md5 : public PAlgId_Dsv_SignerPrivateRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Pss_Md5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01089
    /// @trace_id_dd=DD_CRYPTO_03063
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Pss_Md5() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01090
    /// @trace_id_dd=DD_CRYPTO_03064
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ Ecdsa
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01091
/// @trace_id_dd=DD_CRYPTO_03065
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01092
    /// @trace_id_dd=DD_CRYPTO_03066
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01093
    /// @trace_id_dd=DD_CRYPTO_03067
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
    /// @brief Check if IDs are identical
    /// @name   IsMinePrimitiveId
    /// @param nAlgId Encryption Primitive ID
    /// @returns true if same flase otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01094
    /// @trace_id_dd=DD_CRYPTO_03068
    /// @needwork = ad
    /// @endcode
    bool IsMinePrimitiveId(AlgId nAlgId) const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01095
/// @trace_id_dd=DD_CRYPTO_03069
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_Sha1 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01096
    /// @trace_id_dd=DD_CRYPTO_03070
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_Sha1() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01097
    /// @trace_id_dd=DD_CRYPTO_03071
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01098
/// @trace_id_dd=DD_CRYPTO_03072
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01099
    /// @trace_id_dd=DD_CRYPTO_03073
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01100
    /// @trace_id_dd=DD_CRYPTO_03074
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01101
/// @trace_id_dd=DD_CRYPTO_03075
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01102
    /// @trace_id_dd=DD_CRYPTO_03076
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01103
    /// @trace_id_dd=DD_CRYPTO_03077
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01104
/// @trace_id_dd=DD_CRYPTO_03078
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01105
    /// @trace_id_dd=DD_CRYPTO_03079
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01106
    /// @trace_id_dd=DD_CRYPTO_03080
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ RSA
/// @brief Encryption Primitive: Dsv_SignerPrivateEcdsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01107
/// @trace_id_dd=DD_CRYPTO_03081
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01108
    /// @trace_id_dd=DD_CRYPTO_03082
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01109
    /// @trace_id_dd=DD_CRYPTO_03083
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ hardware
/// @brief PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04336
/// @trace_id_dd=DD_CRYPTO_08694
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256 : public PAlgId_Dsv_SignerPrivateEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04337
    /// @trace_id_dd=DD_CRYPTO_08695
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04338
    /// @trace_id_dd=DD_CRYPTO_08696
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_SignerPrivateRsa_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01110
/// @trace_id_dd=DD_CRYPTO_03084
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_SignerPrivateRsa_Md5 : public PAlgId_Dsv_SignerPrivateRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_SignerPrivateRsa_Md5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01111
    /// @trace_id_dd=DD_CRYPTO_03085
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_SignerPrivateRsa_Md5() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01112
    /// @trace_id_dd=DD_CRYPTO_03086
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01113
/// @trace_id_dd=DD_CRYPTO_03087
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01114
    /// @trace_id_dd=DD_CRYPTO_03088
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01115
    /// @trace_id_dd=DD_CRYPTO_03089
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01116
/// @trace_id_dd=DD_CRYPTO_03090
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Md5 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Md5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01117
    /// @trace_id_dd=DD_CRYPTO_03091
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Md5() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01118
    /// @trace_id_dd=DD_CRYPTO_03092
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};  //***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01119
/// @trace_id_dd=DD_CRYPTO_03093
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Sha1 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01120
    /// @trace_id_dd=DD_CRYPTO_03094
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Sha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01121
    /// @trace_id_dd=DD_CRYPTO_03095
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01122
/// @trace_id_dd=DD_CRYPTO_03096
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Sha2_224 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01123
    /// @trace_id_dd=DD_CRYPTO_03097
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Sha2_224() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01124
    /// @trace_id_dd=DD_CRYPTO_03098
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01125
/// @trace_id_dd=DD_CRYPTO_03099
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Sha2_256 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01126
    /// @trace_id_dd=DD_CRYPTO_03100
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01127
    /// @trace_id_dd=DD_CRYPTO_03101
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01128
/// @trace_id_dd=DD_CRYPTO_03102
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Sha2_384 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01129
    /// @trace_id_dd=DD_CRYPTO_03103
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Sha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01130
    /// @trace_id_dd=DD_CRYPTO_03104
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01131
/// @trace_id_dd=DD_CRYPTO_03105
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Sha2_512 : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01132
    /// @trace_id_dd=DD_CRYPTO_03106
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Sha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01133
    /// @trace_id_dd=DD_CRYPTO_03107
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01134
/// @trace_id_dd=DD_CRYPTO_03108
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss : public PAlgId_Dsv_VerifierPublicRsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01135
    /// @trace_id_dd=DD_CRYPTO_03109
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01136
    /// @trace_id_dd=DD_CRYPTO_03110
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01137
/// @trace_id_dd=DD_CRYPTO_03111
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Md5 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Md5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01138
    /// @trace_id_dd=DD_CRYPTO_03112
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Md5() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01139
    /// @trace_id_dd=DD_CRYPTO_03113
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01140
/// @trace_id_dd=DD_CRYPTO_03114
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01141
    /// @trace_id_dd=DD_CRYPTO_03115
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01142
    /// @trace_id_dd=DD_CRYPTO_03116
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};  //***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01143
/// @trace_id_dd=DD_CRYPTO_03117
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01144
    /// @trace_id_dd=DD_CRYPTO_03118
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01145
    /// @trace_id_dd=DD_CRYPTO_03119
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01146
/// @trace_id_dd=DD_CRYPTO_03120
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01147
    /// @trace_id_dd=DD_CRYPTO_03121
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01148
    /// @trace_id_dd=DD_CRYPTO_03122
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01149
/// @trace_id_dd=DD_CRYPTO_03123
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01150
    /// @trace_id_dd=DD_CRYPTO_03124
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01151
    /// @trace_id_dd=DD_CRYPTO_03125
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Dsv_VerifierPublicRsa_Pss_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01152
/// @trace_id_dd=DD_CRYPTO_03126
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512 : public PAlgId_Dsv_VerifierPublicRsa_Pss
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01153
    /// @trace_id_dd=DD_CRYPTO_03127
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01154
    /// @trace_id_dd=DD_CRYPTO_03128
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01155
/// @trace_id_dd=DD_CRYPTO_03129
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01156
    /// @trace_id_dd=DD_CRYPTO_03130
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01157
    /// @trace_id_dd=DD_CRYPTO_03131
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa_Sha1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01158
/// @trace_id_dd=DD_CRYPTO_03132
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_Sha1 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_Sha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01159
    /// @trace_id_dd=DD_CRYPTO_03133
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_Sha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01160
    /// @trace_id_dd=DD_CRYPTO_03134
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa_Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01161
/// @trace_id_dd=DD_CRYPTO_03135
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01162
    /// @trace_id_dd=DD_CRYPTO_03136
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01163
    /// @trace_id_dd=DD_CRYPTO_03137
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01164
/// @trace_id_dd=DD_CRYPTO_03138
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01165
    /// @trace_id_dd=DD_CRYPTO_03139
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01166
    /// @trace_id_dd=DD_CRYPTO_03140
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa_Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01167
/// @trace_id_dd=DD_CRYPTO_03141
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01168
    /// @trace_id_dd=DD_CRYPTO_03142
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01169
    /// @trace_id_dd=DD_CRYPTO_03143
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief Encryption Primitive: Dsv_VerifierPublicEcdsa_Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01170
/// @trace_id_dd=DD_CRYPTO_03144
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01171
    /// @trace_id_dd=DD_CRYPTO_03145
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01172
    /// @trace_id_dd=DD_CRYPTO_03146
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//***************/
/// @brief PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04339
/// @trace_id_dd=DD_CRYPTO_08697
/// @needwork = ad
/// @endcode
class PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256 : public PAlgId_Dsv_VerifierPublicEcdsa
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04340
    /// @trace_id_dd=DD_CRYPTO_08698
    /// @needwork = ad
    /// @endcode
    PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04341
    /// @trace_id_dd=DD_CRYPTO_08699
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ //cmac aes
/// @brief Encryption Primitive: MacAesBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01173
/// @trace_id_dd=DD_CRYPTO_03147
/// @needwork = ad
/// @endcode
class PAlgId_MacAesBase : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacAesBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01174
    /// @trace_id_dd=DD_CRYPTO_03148
    /// @needwork = ad
    /// @endcode
    PAlgId_MacAesBase() noexcept;
};
//********************************/
/// @brief Encryption Primitive: MacAesCbc_128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01175
/// @trace_id_dd=DD_CRYPTO_03149
/// @needwork = ad
/// @endcode
class PAlgId_MacAesCbc_128 : public PAlgId_MacAesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacAesCbc_128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01176
    /// @trace_id_dd=DD_CRYPTO_03150
    /// @needwork = ad
    /// @endcode
    PAlgId_MacAesCbc_128() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01177
    /// @trace_id_dd=DD_CRYPTO_03151
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: MacAesCbc_192
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01178
/// @trace_id_dd=DD_CRYPTO_03152
/// @needwork = ad
/// @endcode
class PAlgId_MacAesCbc_192 : public PAlgId_MacAesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacAesCbc_192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01179
    /// @trace_id_dd=DD_CRYPTO_03153
    /// @needwork = ad
    /// @endcode
    PAlgId_MacAesCbc_192() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01180
    /// @trace_id_dd=DD_CRYPTO_03154
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: MacAesCbc_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01181
/// @trace_id_dd=DD_CRYPTO_03155
/// @needwork = ad
/// @endcode
class PAlgId_MacAesCbc_256 : public PAlgId_MacAesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacAesCbc_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01182
    /// @trace_id_dd=DD_CRYPTO_03156
    /// @needwork = ad
    /// @endcode
    PAlgId_MacAesCbc_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01183
    /// @trace_id_dd=DD_CRYPTO_03157
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/ //cmac des
/// @brief Encryption Primitive: MacDesBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01184
/// @trace_id_dd=DD_CRYPTO_03158
/// @needwork = ad
/// @endcode
class PAlgId_MacDesBase : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacDesBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01185
    /// @trace_id_dd=DD_CRYPTO_03159
    /// @needwork = ad
    /// @endcode
    PAlgId_MacDesBase() noexcept;
};
//********************************/
/// @brief Encryption Primitive: MacDesCbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01186
/// @trace_id_dd=DD_CRYPTO_03160
/// @needwork = ad
/// @endcode
class PAlgId_MacDesCbc : public PAlgId_MacDesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacDesCbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01187
    /// @trace_id_dd=DD_CRYPTO_03161
    /// @needwork = ad
    /// @endcode
    PAlgId_MacDesCbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01188
    /// @trace_id_dd=DD_CRYPTO_03162
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: MacDesCbc_ede2
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01189
/// @trace_id_dd=DD_CRYPTO_03163
/// @needwork = ad
/// @endcode
class PAlgId_MacDesCbc_ede2 : public PAlgId_MacDesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacDesCbc_ede2
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01190
    /// @trace_id_dd=DD_CRYPTO_03164
    /// @needwork = ad
    /// @endcode
    PAlgId_MacDesCbc_ede2() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01191
    /// @trace_id_dd=DD_CRYPTO_03165
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: MacDesCbc_ede3
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01192
/// @trace_id_dd=DD_CRYPTO_03166
/// @needwork = ad
/// @endcode
class PAlgId_MacDesCbc_ede3 : public PAlgId_MacDesBase
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacDesCbc_ede3
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01193
    /// @trace_id_dd=DD_CRYPTO_03167
    /// @needwork = ad
    /// @endcode
    PAlgId_MacDesCbc_ede3() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01194
    /// @trace_id_dd=DD_CRYPTO_03168
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: Machash
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01195
/// @trace_id_dd=DD_CRYPTO_03169
/// @needwork = ad
/// @endcode
class PAlgId_Machash : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Machash
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01196
    /// @trace_id_dd=DD_CRYPTO_03170
    /// @needwork = ad
    /// @endcode
    PAlgId_Machash() noexcept;
};
//********************************/
/// @brief Encryption Primitive: MacHashMd5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01197
/// @trace_id_dd=DD_CRYPTO_03171
/// @needwork = ad
/// @endcode
class PAlgId_MacHashMd5 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashMd5
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01198
    /// @trace_id_dd=DD_CRYPTO_03172
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashMd5() noexcept;
    /// @brief Get Encryption Primitive ID
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01199
    /// @trace_id_dd=DD_CRYPTO_03173
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // hmac base
/// @brief Encryption Primitive:
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01200
/// @trace_id_dd=DD_CRYPTO_03174
/// @needwork = ad
/// @endcode
class PAlgId_MacHashSha1 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashSha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01201
    /// @trace_id_dd=DD_CRYPTO_03175
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashSha1() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01202
    /// @trace_id_dd=DD_CRYPTO_03176
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // hmac base
/// @brief Encryption Primitive: MacHashSha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01203
/// @trace_id_dd=DD_CRYPTO_03177
/// @needwork = ad
/// @endcode
class PAlgId_MacHashSha2_224 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashSha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01204
    /// @trace_id_dd=DD_CRYPTO_03178
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashSha2_224() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01205
    /// @trace_id_dd=DD_CRYPTO_03179
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // hmac base
/// @brief Encryption Primitive: MacHashSha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01206
/// @trace_id_dd=DD_CRYPTO_03180
/// @needwork = ad
/// @endcode
class PAlgId_MacHashSha2_256 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashSha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01207
    /// @trace_id_dd=DD_CRYPTO_03181
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashSha2_256() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01208
    /// @trace_id_dd=DD_CRYPTO_03182
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // hmac base
/// @brief Encryption Primitive: MacHashSha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01209
/// @trace_id_dd=DD_CRYPTO_03183
/// @needwork = ad
/// @endcode
class PAlgId_MacHashSha2_384 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashSha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01210
    /// @trace_id_dd=DD_CRYPTO_03184
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashSha2_384() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01211
    /// @trace_id_dd=DD_CRYPTO_03185
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // hmac base
/// @brief Encryption Primitive: MacHashSha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01212
/// @trace_id_dd=DD_CRYPTO_03186
/// @needwork = ad
/// @endcode
class PAlgId_MacHashSha2_512 : public PAlgId_Machash
{
public:
    /// @brief Constructor
    /// @name      PAlgId_MacHashSha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01213
    /// @trace_id_dd=DD_CRYPTO_03187
    /// @needwork = ad
    /// @endcode
    PAlgId_MacHashSha2_512() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01214
    /// @trace_id_dd=DD_CRYPTO_03188
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/  // wrap aes
/// @brief Encryption Primitive: WrapAes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01215
/// @trace_id_dd=DD_CRYPTO_03189
/// @needwork = ad
/// @endcode
class PAlgId_WrapAes : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_WrapAes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01216
    /// @trace_id_dd=DD_CRYPTO_03190
    /// @needwork = ad
    /// @endcode
    PAlgId_WrapAes() noexcept;
};
//********************************/  // wrap aes-pad
/// @brief Encryption Primitive: WrapAesPad
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01217
/// @trace_id_dd=DD_CRYPTO_03191
/// @needwork = ad
/// @endcode
class PAlgId_WrapAesPad : public PAlgId_WrapAes
{
public:
    /// @brief Constructor
    /// @name      PAlgId_WrapAesPad
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01218
    /// @trace_id_dd=DD_CRYPTO_03192
    /// @needwork = ad
    /// @endcode
    PAlgId_WrapAesPad() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01219
    /// @trace_id_dd=DD_CRYPTO_03193
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: WrapAesUnPad
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01220
/// @trace_id_dd=DD_CRYPTO_03194
/// @needwork = ad
/// @endcode
class PAlgId_WrapAesUnPad : public PAlgId_WrapAes
{
public:
    /// @brief Constructor
    /// @name      PAlgId_WrapAesUnPad
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01221
    /// @trace_id_dd=DD_CRYPTO_03195
    /// @needwork = ad
    /// @endcode
    PAlgId_WrapAesUnPad() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01222
    /// @trace_id_dd=DD_CRYPTO_03196
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: WrapDes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01223
/// @trace_id_dd=DD_CRYPTO_03197
/// @needwork = ad
/// @endcode
class PAlgId_WrapDes : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_WrapDes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01224
    /// @trace_id_dd=DD_CRYPTO_03198
    /// @needwork = ad
    /// @endcode
    PAlgId_WrapDes() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01225
    /// @trace_id_dd=DD_CRYPTO_03199
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kem_private
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01226
/// @trace_id_dd=DD_CRYPTO_03200
/// @needwork = ad
/// @endcode
class PAlgId_Kem_private : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kem_private
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01227
    /// @trace_id_dd=DD_CRYPTO_03201
    /// @needwork = ad
    /// @endcode
    PAlgId_Kem_private() noexcept;
};
/// @brief Encryption Primitive: Kem_private_rsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01228
/// @trace_id_dd=DD_CRYPTO_03202
/// @needwork = ad
/// @endcode
class PAlgId_Kem_private_rsa : public PAlgId_Kem_private
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kem_private_rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01229
    /// @trace_id_dd=DD_CRYPTO_03203
    /// @needwork = ad
    /// @endcode
    PAlgId_Kem_private_rsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01230
    /// @trace_id_dd=DD_CRYPTO_03204
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kem_public
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01231
/// @trace_id_dd=DD_CRYPTO_03205
/// @needwork = ad
/// @endcode
class PAlgId_Kem_public : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kem_public
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01232
    /// @trace_id_dd=DD_CRYPTO_03206
    /// @needwork = ad
    /// @endcode
    PAlgId_Kem_public() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01233
    /// @trace_id_dd=DD_CRYPTO_03207
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Kem_public_rsa
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01234
/// @trace_id_dd=DD_CRYPTO_03208
/// @needwork = ad
/// @endcode
class PAlgId_Kem_public_rsa : public PAlgId_Kem_public
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Kem_public_rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01235
    /// @trace_id_dd=DD_CRYPTO_03209
    /// @needwork = ad
    /// @endcode
    PAlgId_Kem_public_rsa() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01236
    /// @trace_id_dd=DD_CRYPTO_03210
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Key_Agreement_Private
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01237
/// @trace_id_dd=DD_CRYPTO_03211
/// @needwork = ad
/// @endcode
class PAlgId_Key_Agreement_Private : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Key_Agreement_Private
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01238
    /// @trace_id_dd=DD_CRYPTO_03212
    /// @needwork = ad
    /// @endcode
    PAlgId_Key_Agreement_Private() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01239
    /// @trace_id_dd=DD_CRYPTO_03213
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Ae_Stream
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01240
/// @trace_id_dd=DD_CRYPTO_03214
/// @needwork = ad
/// @endcode
class PAlgId_Ae_Stream : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Ae_Stream
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01241
    /// @trace_id_dd=DD_CRYPTO_03215
    /// @needwork = ad
    /// @endcode
    PAlgId_Ae_Stream() noexcept;
};
/// @brief Encryption Primitive: Ae_Stream_Aes_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01242
/// @trace_id_dd=DD_CRYPTO_03216
/// @needwork = ad
/// @endcode
class PAlgId_Ae_Stream_Aes_Cbc : public PAlgId_Ae_Stream
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Ae_Stream_Aes_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01243
    /// @trace_id_dd=DD_CRYPTO_03217
    /// @needwork = ad
    /// @endcode
    PAlgId_Ae_Stream_Aes_Cbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01244
    /// @trace_id_dd=DD_CRYPTO_03218
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
/// @brief Encryption Primitive: Ae_Stream_Des_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01245
/// @trace_id_dd=DD_CRYPTO_03219
/// @needwork = ad
/// @endcode
class PAlgId_Ae_Stream_Des_Cbc : public PAlgId_Ae_Stream
{
public:
    /// @brief Constructor
    /// @name      PAlgId_Ae_Stream_Des_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01246
    /// @trace_id_dd=DD_CRYPTO_03220
    /// @needwork = ad
    /// @endcode
    PAlgId_Ae_Stream_Des_Cbc() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01247
    /// @trace_id_dd=DD_CRYPTO_03221
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};
//********************************/
/// @brief Encryption Primitive: SecretseedObj
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01248
/// @trace_id_dd=DD_CRYPTO_03222
/// @needwork = ad
/// @endcode
class PAlgId_SecretseedObj : public PAlgId_Base
{
public:
    /// @brief Constructor
    /// @name      PAlgId_SecretseedObj
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01249
    /// @trace_id_dd=DD_CRYPTO_03223
    /// @needwork = ad
    /// @endcode
    PAlgId_SecretseedObj() noexcept;
    /// @brief Get vendor specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Encryption Primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01250
    /// @trace_id_dd=DD_CRYPTO_03224
    /// @needwork = ad
    /// @endcode
    AlgId GetPrimitiveId() const noexcept override;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PRIMITIVE_ID_H_
