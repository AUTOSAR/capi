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
/// @file       security.cpp
/// @brief      Security related algorithm set
/// @details
/// @date       2023-11-15
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include <isoft/utils/security.h>
#include <openssl/bio.h>
#include <openssl/decoder.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace isoft {
namespace utils {
namespace security {

namespace {

/// @brief Maximum block size read each time when calculating file hash
constexpr uint16_t const kMaxBlockSize{4096U};

/// @brief Signature file size, generally 256 bytes, defined as 1024 to prevent future changes
constexpr uint16_t const kMaxSizeOfSignedFile{1024U};

/// @brief Convert decimal number to hexadecimal string
/// @param pHashNum Hash value
/// @param len Length
/// @return Hexadecimal string
std::string Num2HexString(uint8_t const *const pHashNum, uint32_t const len) noexcept
{
    uint32_t const kTwoBytes{2U};
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i{0U}; i < len; ++i) {
        oss << std::setw(kTwoBytes) << static_cast< int >(pHashNum[i]);
    }
    return oss.str();
}

/// @brief Read signature file content
/// @param fileName File name
/// @return Success returns file content; failure returns empty vector
std::vector< uint8_t > ReadSignedFile(std::string const &fileName) noexcept
{
    if (fileName.empty()) {
        std::cerr << "ReadSignedFile(): empty file name !!!" << std::endl;
        return {};
    }

    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "ReadSignedFile(): open file (" << fileName << ") failed !!!" << std::endl;
        return {};
    }

    ifs.seekg(0, std::ios::end);
    std::streampos const fileSize = ifs.tellg();
    if (fileSize <= 0) {
        return {};
    }

    if (static_cast< size_t >(fileSize) > kMaxSizeOfSignedFile) {
        std::cerr << "ReadSignedFile(): signed file (" << fileName << ") is larger than expected" << std::endl;
        return {};
    }

    ifs.seekg(0, std::ios::beg);
    ifs.unsetf(std::ios::skipws);

    std::vector< uint8_t > result;
    result.assign(std::istreambuf_iterator< char >(ifs), std::istreambuf_iterator< char >());

    if (result.size() != static_cast< size_t >(fileSize)) {
        std::cerr << "ReadSignedFile(): read file (" << fileName << ") failed !!!" << std::endl;
        return {};
    }

    return result;
}

/// @brief Get file digest using specified digest algorithm
/// @param fileName File name
/// @param pEvpMd Digest algorithm
/// @return Digest data
std::vector< uint8_t > GetDigestOfFile(std::string const &fileName, EVP_MD const *const pEvpMd) noexcept
{
    if (fileName.empty() || (pEvpMd == nullptr)) {
        std::cerr << "GetDigestOfFile(): invalid file name or digest algorithm !!!" << std::endl;
        return {};
    }

    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "GetDigestOfFile(): open file (" << fileName << ") failed !!!" << std::endl;
        return {};
    }

    EVP_MD_CTX *const pMdCtx{EVP_MD_CTX_new()};
    if (pMdCtx == nullptr) {
        return {};
    }

    if (EVP_DigestInit_ex(pMdCtx, pEvpMd, nullptr) != 1) {
        EVP_MD_CTX_free(pMdCtx);
        std::cerr << "GetDigestOfFile(): create digest context failed !!!" << std::endl;
        return {};
    }

    std::vector< Char8_t > vecTempData(kMaxBlockSize);
    while (ifs.read(reinterpret_cast< char * >(vecTempData.data()), vecTempData.size()) || ifs.gcount() > 0) {
        if (EVP_DigestUpdate(pMdCtx, vecTempData.data(), static_cast< size_t >(ifs.gcount())) != 1) {
            EVP_MD_CTX_free(pMdCtx);
            std::cerr << "GetDigestOfFile(): update digest failed !!!" << std::endl;
            return {};
        }
    }

    std::vector< uint8_t > vecDigest(EVP_MD_get_size(pEvpMd));
    uint32_t digestLen{0};
    if (EVP_DigestFinal_ex(pMdCtx, vecDigest.data(), &digestLen) != 1) {
        EVP_MD_CTX_free(pMdCtx);
        return {};
    }

    return vecDigest;
}

/// @brief Verify digest signature
/// @param pPubKey Public key EVP_PKEY
/// @param pEvpMd Digest algorithm
/// @param digest Digest data
/// @param digestLen Digest length
/// @param signature Signature data
/// @param signatureLen Signature length
/// @return 0 success; <0 failure
int32_t VerifyDigest(EVP_PKEY *const pPubKey,
                     EVP_MD const *const pEvpMd,
                     uint8_t const *const digest,
                     std::uint32_t const digestLen,
                     uint8_t const *const signature,
                     std::uint32_t const signatureLen) noexcept
{
    if ((pPubKey == nullptr) || (pEvpMd == nullptr) || (digest == nullptr) || (signature == nullptr)) {
        return -1;
    }

    EVP_PKEY_CTX *const pCtx{EVP_PKEY_CTX_new(pPubKey, nullptr)};
    if (pCtx == nullptr) {
        return -1;
    }

    if (EVP_PKEY_verify_init(pCtx) <= 0) {
        EVP_PKEY_CTX_free(pCtx);
        return -1;
    }

    if (EVP_PKEY_CTX_set_signature_md(pCtx, pEvpMd) <= 0) {
        EVP_PKEY_CTX_free(pCtx);
        return -1;
    }

    if (EVP_PKEY_base_id(pPubKey) == EVP_PKEY_RSA) {
        if (EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_PADDING) <= 0) {
            EVP_PKEY_CTX_free(pCtx);
            return -1;
        }
    }

    int const rc{EVP_PKEY_verify(pCtx, signature, signatureLen, digest, digestLen)};
    EVP_PKEY_CTX_free(pCtx);
    return rc == 1 ? 0 : -1;
}

EVP_PKEY *DecodePubKeyBio(BIO *const bio) noexcept
{
    if (bio == nullptr) {
        return nullptr;
    }

    EVP_PKEY *pPubKey{nullptr};
    OSSL_DECODER_CTX *const decoderCtx{
        OSSL_DECODER_CTX_new_for_pkey(&pPubKey, nullptr, nullptr, nullptr, 0, nullptr, nullptr)};
    if (decoderCtx == nullptr) {
        return nullptr;
    }

    if (OSSL_DECODER_from_bio(decoderCtx, bio) != 1) {
        EVP_PKEY_free(pPubKey);
        pPubKey = nullptr;
    }

    OSSL_DECODER_CTX_free(decoderCtx);
    return pPubKey;
}

}  // namespace

/// @brief Read public key from string and convert to RSA
/// @param pubKeyString Public key string
/// @return nullptr failure; !=nullptr success
EVP_PKEY *CreateEvpPubKey(std::string const &pubKeyString) noexcept
{
    if (pubKeyString.empty()) {
        std::cerr << "CreateEvpPubKey(): empty pubkey string !!!" << std::endl;
        return nullptr;
    }

    BIO *const pPubKeyBio{BIO_new_mem_buf(pubKeyString.data(), static_cast< int >(pubKeyString.size()))};
    if (nullptr == pPubKeyBio) {
        std::cerr << "CreateEvpPubKey(): Allocate pubkey bio failed" << std::endl;
        return nullptr;
    }

    EVP_PKEY *const pPubKey{DecodePubKeyBio(pPubKeyBio)};
    BIO_free(pPubKeyBio);
    if (pPubKey == nullptr) {
        std::cerr << "CreateEvpPubKey(): failed to decode public key" << std::endl;
        return nullptr;
    }

    return pPubKey;
}

/// @brief Release public key resource
/// @param pEvpPubKey Public key resource to release
void DeleteEvpPubKey(EVP_PKEY *const pEvpPubKey) noexcept
{
    if (nullptr != pEvpPubKey) {
        EVP_PKEY_free(pEvpPubKey);
    }
}

/// @brief Get MD5 value of a piece of data
/// @param dataString Data to get MD5 value of
/// @return MD5 value of data
std::string GetMd5OfData(std::string const &dataString) noexcept
{
    EVP_MD_CTX *const pCtx{EVP_MD_CTX_new()};
    if (pCtx == nullptr) {
        return std::string{};
    }

    if (EVP_DigestInit_ex(pCtx, EVP_md5(), nullptr) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    if (EVP_DigestUpdate(pCtx, dataString.data(), dataString.size()) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    std::vector< uint8_t > digest(static_cast< size_t >(EVP_MD_get_size(EVP_md5())));
    unsigned int outLen{0};
    if (EVP_DigestFinal_ex(pCtx, digest.data(), &outLen) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    EVP_MD_CTX_free(pCtx);
    return Num2HexString(digest.data(), outLen);
}

/// @brief Verify signature using public key
/// @param pRsa RSA structure pointer
/// @param digestAlgo Digest algorithm (currently only supports MD5, SHA1 and SHA256)
/// @param unsignedFile Unsigned file
/// @param signedFile Signed file
/// @return 0 signature verification success; <0 signature verification failure
int32_t VerifySignature(EVP_PKEY *const pPubKey,
                        std::string const &digestAlgo,
                        std::string const &unsignedFile,
                        std::string const &signedFile) noexcept
{
    if (nullptr == pPubKey) {
        std::cerr << "VerifySignature(): empty public key !!!" << std::endl;
        return -1;
    }
    if (digestAlgo.empty()) {
        std::cerr << "VerifySignature(): degest algorithm !!!" << std::endl;
        return -1;
    }
    if (unsignedFile.empty()) {
        std::cerr << "VerifySignature(): empty unsigned file path !!!" << std::endl;
        return -1;
    }
    if (signedFile.empty()) {
        std::cerr << "VerifySignature(): empty signed file path !!!" << std::endl;
        return -1;
    }

    std::string tmpAlgo{digestAlgo};
    Char8_t const kCharSapce{' '};
    std::ignore = tmpAlgo.erase(0U, tmpAlgo.find_first_not_of(kCharSapce));
    std::ignore = tmpAlgo.erase(tmpAlgo.find_last_not_of(kCharSapce) + 1U);

    const EVP_MD *pEvpMd{nullptr};
    if (0 == strcasecmp(tmpAlgo.c_str(), GetMd5DgstAlgo())) {
        pEvpMd = EVP_md5();
    } else if (0 == strcasecmp(tmpAlgo.c_str(), GetSha1DgstAlgo())) {
        pEvpMd = EVP_sha1();
    } else if (0 == strcasecmp(tmpAlgo.c_str(), GetSha256DgstAlgo())) {
        pEvpMd = EVP_sha256();
    } else {
        std::cerr << "VerifySignature(): unsupported digest algorithm !!!" << std::endl;
        return -1;
    }

    std::vector< uint8_t > unsignedData{GetDigestOfFile(unsignedFile, pEvpMd)};
    if (unsignedData.empty()) {
        std::cerr << "VerifySignature(): failed to compute unsigned file digest !!!" << std::endl;
        return -1;
    }

    std::vector< uint8_t > signedData{ReadSignedFile(signedFile)};
    if (signedData.empty()) {
        std::cerr << "VerifySignature(): empty signed file data !!!" << std::endl;
        return -1;
    }

    std::int32_t const ret{VerifyDigest(pPubKey, pEvpMd, unsignedData.data(),
                                        static_cast< uint32_t >(unsignedData.size()), signedData.data(),
                                        static_cast< uint32_t >(signedData.size()))};

    if (ret != 0) {
        std::cerr << "VerifySignature(): " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return -1;
    }

    return 0;
}

/// @brief Get file digest using specified digest algorithm
/// @param fileName File name
/// @param digestAlgo Digest algorithm
/// @return Digest data
std::string GetDigestOfFile(std::string const &fileName, std::string const &digestAlgo) noexcept
{
    if (fileName.empty()) {
        std::cerr << "GetDigestOfFile(): empty file name !!!" << std::endl;
        return {};
    }
    if (digestAlgo.empty()) {
        std::cerr << "GetDigestOfFile(): empty digest algorithm !!!" << std::endl;
        return {};
    }

    std::string tmpAlgo{digestAlgo};
    Char8_t const kCharSapce{' '};
    std::ignore = tmpAlgo.erase(0U, tmpAlgo.find_first_not_of(kCharSapce));
    std::ignore = tmpAlgo.erase(tmpAlgo.find_last_not_of(kCharSapce) + 1U);

    const EVP_MD *pEvpMd{nullptr};
    if (0 == strcasecmp(tmpAlgo.c_str(), GetMd5DgstAlgo())) {
        pEvpMd = EVP_md5();
    } else if (0 == strcasecmp(tmpAlgo.c_str(), GetSha1DgstAlgo())) {
        pEvpMd = EVP_sha1();
    } else if (0 == strcasecmp(tmpAlgo.c_str(), GetSha256DgstAlgo())) {
        pEvpMd = EVP_sha256();
    } else {
        std::cerr << "GetDigestOfFile(): unsupported digest algorithm !!!" << std::endl;
        return {};
    }

    return Num2HexString(GetDigestOfFile(fileName, pEvpMd).data(), static_cast< uint32_t >(EVP_MD_get_size(pEvpMd)));
}

}  // namespace security
}  // namespace utils
}  // namespace isoft
