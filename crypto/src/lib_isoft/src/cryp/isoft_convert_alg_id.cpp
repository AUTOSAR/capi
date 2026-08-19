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
/// @file       isoft_convert_alg_id.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-01-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Crypto Primitives
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PConvertAlgID
/// @unit_description=Crypto primitive conversion to string
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_convert_alg_id.h"

#include <algorithm>
#include <string>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"

namespace {
/// @brief Initialize the crypto primitive name mapping list: shared
/// @param vecAlgName Vector of crypto primitive names
/// @throw ???
void InitVecAlgName(ara::crypto::cryp::isoft_def::PConvertAlgID::VecAlgName &vecAlgName)
{
    if (false == vecAlgName.empty()) {
        return;
    }
    uint32_t const nNum100{100U};
    vecAlgName.reserve(nNum100);
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_CertSlot >());
    // Hash algorithms: Crc + Md5 + SHA
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc8 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc16 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc32 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc64 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_512 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha1_File >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_224_File >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_256_File >());
    // Symmetric encryption: DES
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_DesKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Ecb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Cbc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3Des_Ecb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3Des_Cbc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3Des_Cfb1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3Des_Cfb64 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3Des_Ofb64 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3DesKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_2DesKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Cfb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Cfb64 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Ofb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Des_Ofb64 >());

    // Symmetric encryption: AES
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKey128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKey192 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKey256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKeyHMAC >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ecb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ecb_128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ecb_192 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ecb_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cbc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cbc_128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cbc_192 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cbc_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ctr >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Gcm >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cfb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cfb1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cfb8 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Cfb128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ofb >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Symmetric_Aes_Ofb128 >());

    // Asymmetric encryption: RSA
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_Rsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey512 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey1024 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey2048 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey4096 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_RsaKey8192 >());

    /// hsm
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_Rsa_Oaep >());

    // Random number generator
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_RandomGenerator >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_RandomGenerator_Global >());
    // KDF
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_Des >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_3Des >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_Aes >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_Aes128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_Aes192 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kdf_Aes256 >());
    // DSV Digital signature
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SigEncodePrivateRsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_MsgRecoveryPublicRsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss >());
    /// hsm
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Sha2_512 >());

    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512 >());

    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_Sha2_512 >());
    // hardware ecc
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_SignerPrivateEcdsa_R5_Sha2_256 >());

    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Sha2_512 >());

    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512 >());

    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_Sha2_512 >());
    // hardware ecc
    vecAlgName.push_back(
        std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Dsv_VerifierPublicEcdsa_R5_Sha2_256 >());

    // mac Message authentication code
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacAesCbc_128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacAesCbc_192 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacAesCbc_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacDesCbc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacDesCbc_ede2 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacDesCbc_ede3 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashMd5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashSha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashSha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashSha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashSha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_MacHashSha2_512 >());
    // wrap
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_WrapAesPad >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_WrapAesUnPad >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_WrapDes >());
    // kem
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kem_private_rsa >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Kem_public_rsa >());
    // dh
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Key_Agreement_Private >());
    // ae
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Ae_Stream_Aes_Cbc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Ae_Stream_Des_Cbc >());
    // ecc
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_Ecc >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_EccKey >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_EccKey128 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_EccKey256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Asymmetric_EccKey192 >());
}

/// @brief Initialize the crypto primitive name mapping list: Hash
/// @param vecAlgName Vector of crypto primitive names
void InitVecHashAlgName(ara::crypto::cryp::isoft_def::PConvertAlgID::VecAlgName &vecAlgName) noexcept
{
    if (false == vecAlgName.empty()) {
        return;
    }
    uint32_t const nNum100{100U};
    vecAlgName.reserve(nNum100);
    // Hash algorithms: Crc + Md5 + SHA
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc8 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc16 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc32 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Crc64 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Md5 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha1 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_224 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_256 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_384 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_512 >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha1_File >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_224_File >());
    vecAlgName.push_back(std::make_unique< ara::crypto::cryp::isoft_def::PAlgId_Sha2_256_File >());
}
}  // namespace

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Constructor
PConvertAlgID::PConvertAlgID() noexcept
{
    InitVecAlgName(vecPuhuaAlgName_);
    InitVecHashAlgName(vecHashAlgName_);
}
/// @brief Destructor
PConvertAlgID::~PConvertAlgID() noexcept
{
    vecPuhuaAlgName_.clear();
    vecHashAlgName_.clear();
}
/// @brief Convert string to crypto primitive ID
/// @param stAlgName Crypto primitive string
/// @returns Crypto primitive ID
uint64_t PConvertAlgID::ConvertToAlgId(ara::core::StringView const &stAlgName) const noexcept
{
    ara::core::String stNewAlgName{stAlgName.data()};
    ara::core::internal::basic_string< char8_t >::iterator const itBegin{stNewAlgName.begin()};
    ara::core::internal::basic_string< char8_t >::iterator const ifEnd{stNewAlgName.end()};
    std::ignore = std::transform(itBegin, ifEnd, stNewAlgName.begin(), &::tolower);
    // The requirement is case-insensitive
    for (auto const &itData : vecPuhuaAlgName_) {
        if (itData->GetPrimitiveName().data() == stNewAlgName) {
            return itData->GetPrimitiveId();
        }
    }
    /// @returns vendor specific binary algorithm ID or @c kAlgIdUndefined if a primitive with provided name is not
    /// supported
    return static_cast< uint64_t >(EPhCtxTypeID::kUnDefine);
}
/// @brief Convert crypto primitive ID to string
/// @param nTypeID Crypto primitive ID
/// @returns Crypto primitive string
ara::core::StringView PConvertAlgID::ConvertToAlgName(uint64_t const nTypeID) const noexcept
{
    for (auto const &itData : vecPuhuaAlgName_) {
        if (itData->GetPrimitiveId() == nTypeID) {
            return itData->GetPrimitiveName();
        }
    }
    return {""};
}
/// @brief Check whether the crypto primitive is a valid crypto primitive
/// @param nTypeID Crypto primitive ID
/// @returns  true if valid false otherwise
bool PConvertAlgID::IsValidAlgID(uint64_t const nTypeID) const noexcept
{
    for (auto const &itData : vecPuhuaAlgName_) {
        if (itData->GetPrimitiveId() == nTypeID) {
            return true;
        }
    }
    return false;
}
/// @brief Check whether the crypto primitive is of Hash type
/// @param nTypeID Crypto primitive ID
/// @returns  true if valid false otherwise
bool PConvertAlgID::IsValidHashAlgID(uint64_t const nTypeID) const noexcept
{
    for (auto const &itData : vecHashAlgName_) {
        if (itData->GetPrimitiveId() == nTypeID) {
            return true;
        }
    }
    return false;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
