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
/// @file       isoft_generate_key_data.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/crypto/Default Encryption/Decryption/Crypto Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=PCryptoProvider
/// @unit_description=Crypto Provider
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_generate_key_data.h"

#include <openssl/ec.h>
#include <openssl/pem.h>

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace {
/// @brief Save key to a file
/// @param filePath File path
/// @param keyLen   Key length
/// @param keyBuff  Key content
void L_SaveDataToFile(const char *const filePath, int32_t const keyLen, const u_char *const keyBuff) noexcept
{
    /// Open file for writing ("wb" stands for writing binary file)
    FILE *const file{fopen(filePath, "wb")};  // NOLINT
    if (nullptr == file) {
        perror("Can't Open File!");
    }

    // Write the array content to the file
    size_t const written{fwrite(keyBuff, sizeof(u_char), static_cast< std::size_t >(keyLen), file)};
    // Close the file
    std::ignore = fclose(file);  // NOLINT
    ara::crypto::isoft_def::LogInfo() << "KeyData Has Save to File: " << filePath
                                      << ", WriteLen = " << static_cast< int32_t >(written);
}
}  // namespace

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief For generating RSA key data
/// @param tarKeyDataLen Target key length
/// @param privateKeyData Private key data
/// @param publicKeyData Public key data
/// @param formatId Key format: DER/PEM
/// @param priFilePath Optional parameter, private key file path
/// @param pubFilePath Optional parameter, public key file path
/// @param needSaveToFile Optional parameter, whether to save the private key to a file, if needed please provide the file name
MKeyLength GenerateKeyData::GenerateRsaKeyData(int32_t tarKeyDataLen,
                                               ara::core::Vector< uint8_t > &privateKeyData,
                                               ara::core::Vector< uint8_t > &publicKeyData,
                                               Serializable::FormatId formatId,
                                               ara::core::StringView priFilePath,
                                               ara::core::StringView pubFilePath,
                                               bool needSaveToFile) noexcept
{
    MKeyLength keyLength{};
    EVP_PKEY *pRsaPair{Generate_rsa_evp_key(tarKeyDataLen)};
    if (pRsaPair == nullptr) {
        return keyLength;
    }

    // Private key
    int32_t privateLen{0};
    BIO *const rsaPriBio{BIO_new(BIO_s_mem())};
    if (formatId == Serializable::kFormatPemEncoded) {
        std::ignore = PEM_write_bio_PrivateKey(rsaPriBio, pRsaPair, nullptr, nullptr, 0, nullptr, nullptr);
    } else if (formatId == Serializable::kFormatDerEncoded) {
        std::ignore = i2d_PrivateKey_bio(rsaPriBio, pRsaPair);
    } else {
        ara::crypto::isoft_def::LogError() << "GENERATE PRIVATE KEY: ONLY SUPPORT PEM AND DER! ";
    }

    privateLen = BIO_pending(rsaPriBio);
    u_char *privateBuff{nullptr};
    if (privateLen > 0) {
        privateKeyData.resize(privateLen);
        privateBuff = privateKeyData.data();
        std::ignore = BIO_read(rsaPriBio, privateBuff, privateLen);
    }
    keyLength.nPrivateKeyLen = static_cast< uint32_t >(privateLen);

    // Public key
    int32_t publicLen{0};

    BIO *const rsaPubBio{BIO_new(BIO_s_mem())};
    if (formatId == Serializable::kFormatPemEncoded) {
        std::ignore = PEM_write_bio_PUBKEY(rsaPubBio, pRsaPair);
    } else if (formatId == Serializable::kFormatDerEncoded) {
        std::ignore = i2d_PUBKEY_bio(rsaPubBio, pRsaPair);
    } else {
        ara::crypto::isoft_def::LogError() << "GENERATE PUBLICK KEY: ONLY SUPPORT PEM AND DER! ";
    }
    publicLen = BIO_pending(rsaPubBio);
    u_char *publicBuff{nullptr};
    if (publicLen > 0) {
        publicKeyData.resize(publicLen);
        publicBuff  = publicKeyData.data();
        std::ignore = BIO_read(rsaPubBio, publicBuff, publicLen);
    }
    keyLength.nPublicKeyLen = static_cast< uint32_t >(publicLen);

    std::ignore = BIO_free(rsaPubBio);
    std::ignore = BIO_free(rsaPriBio);

    if (true == needSaveToFile) {
        L_SaveDataToFile(priFilePath.data(), privateLen, privateBuff);
        L_SaveDataToFile(pubFilePath.data(), publicLen, publicBuff);
    }

    EVP_PKEY_free(pRsaPair);
    return keyLength;
}
/// @brief For generating ECC key data
/// @param tarKeyLen Target key length (bit length)
/// @param privateKeyData Private key data
/// @param publicKeyData Public key data
/// @param formatId Key format: DER/PEM
/// @param eccForPlatform Specify the platform corresponding to the key
/// @param priFilePath Optional parameter, private key file path
/// @param pubFilePath Optional parameter, public key file path
/// @param needSaveToFile Optional parameter, whether to save the private key to a file, if needed please provide the file name
MKeyLength GenerateKeyData::GenerateEccKeyData(uint32_t tarKeyLen,
                                               ara::core::Vector< uint8_t > &privateKeyData,
                                               ara::core::Vector< uint8_t > &publicKeyData,
                                               Serializable::FormatId formatId,
                                               EccForPlatform eccForPlatform,
                                               ara::core::StringView priFilePath,
                                               ara::core::StringView pubFilePath,
                                               bool needSaveToFile) noexcept
{
    MKeyLength keyLength{};
    //Generate ECC key pair:
    EVP_PKEY *const pEccPair{Generate_ecc_key(static_cast< int32_t >(tarKeyLen), eccForPlatform)};
    if (nullptr == pEccPair) {
        ara::crypto::isoft_def::LogError() << "Failed to create EC key object.";
        return keyLength;
    }

    // Private key
    int32_t privateLen{0};
    BIO *const eccPriBio{BIO_new(BIO_s_mem())};
    // Read private key into BIO
    if (formatId == Serializable::kFormatPemEncoded) {
        std::ignore = PEM_write_bio_PrivateKey(eccPriBio, pEccPair, nullptr, nullptr, 0, nullptr, nullptr);
    } else if (formatId == Serializable::kFormatDerEncoded) {
        std::ignore = i2d_PrivateKey_bio(eccPriBio, pEccPair);
    } else {
        ara::crypto::isoft_def::LogError() << "GENERATE PUBLICK KEY: ONLY SUPPORT PEM AND DER! ";
    }

    privateLen = BIO_pending(eccPriBio);
    privateKeyData.resize(privateLen);
    u_char *const privateBuff{privateKeyData.data()};
    if (privateLen > 0) {
        std::ignore = BIO_read(eccPriBio, privateBuff, privateLen);
    }
    keyLength.nPrivateKeyLen = static_cast< uint32_t >(privateLen);

    // Public key
    int32_t publicLen{0};
    BIO *const eccPubBio{BIO_new(BIO_s_mem())};
    // Read public key into BIO
    if (formatId == Serializable::kFormatPemEncoded) {
        std::ignore = PEM_write_bio_PUBKEY(eccPubBio, pEccPair);
    } else if (formatId == Serializable::kFormatDerEncoded) {
        std::ignore = i2d_PUBKEY_bio(eccPubBio, pEccPair);
    } else {
        ara::crypto::isoft_def::LogError() << "GENERATE PUBLICK KEY: ONLY SUPPORT PEM AND DER! ";
    }

    publicLen = BIO_pending(eccPubBio);
    publicKeyData.resize(publicLen);
    u_char *const publicBuff{publicKeyData.data()};
    if (publicLen > 0) {
        std::ignore = BIO_read(eccPubBio, static_cast< void * >(publicBuff), publicLen);
    }
    keyLength.nPublicKeyLen = static_cast< uint32_t >(publicLen);

    std::ignore = BIO_free(eccPubBio);
    std::ignore = BIO_free(eccPriBio);

    // Whether to save to file
    if (true == needSaveToFile) {
        L_SaveDataToFile(priFilePath.data(), privateLen, privateBuff);
        L_SaveDataToFile(pubFilePath.data(), publicLen, publicBuff);
    }

    EVP_PKEY_free(pEccPair);
    return keyLength;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara