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
/// @file       isoft_ctx_hash_function_sha.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-01-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/HashFunction
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01022
/// @unit_name=PCtxHashFunctionSha
/// @unit_description=SHA-based hash context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_sha.h"

#include <memory>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_digest_hash.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Get DigestService instance. // Multiple calls will create a new object each time.
/// @name GetDigestService
/// @returns DigestService instance
DigestService::Uptr PCtxHashFunctionSha::GetDigestService() const noexcept
{
    return {std::make_unique< PDigestServiceHash< PCtxHashFunctionSha > >(*this)};
}
//***************/
/// @brief Check whether the specific hash function supports iv; SHA series all need iv
/// @name   SupportIv
/// @returns true if support iv false otherwise
bool PCtxHashFunctionSha::SupportIv() const noexcept { return true; }
/// @brief Perform initialization operation
/// @name   DoInit
/// @returns
void PCtxHashFunctionSha::DoInit() noexcept {}
/// @brief Check initialization vector
/// @param piv initialization vector pointer
/// @return true if support IV false otherwise
bool PCtxHashFunctionSha::CheckIV(uint8_t const* const piv) noexcept
{
    char8_t const end{'\0'};
    char8_t const nullChar{' '};
    uint8_t const* p{piv};
    char8_t current{static_cast< char8_t >(*p)};
    while (true) {
        if (current == end) {
            break;
        }
        char8_t const pData{static_cast< char8_t >(*p)};
        if (pData != nullChar) {
            return false;
        }
        p++;
        current = static_cast< char8_t >(*p);
    }
    return true;
}
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @name GetCryptoPrimitiveId
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionSha1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Sha1 >()};
}
/// @brief Get hash result length
/// @name GetHashLength
/// @returns hash result length
uint32_t PCtxHashFunctionSha1::GetHashLength() const noexcept
{
    return (sizeof(hashDataSha_)) - static_cast< uint32_t >(kPhHashTailLength);
}
/// @brief Perform initialization operation: via initialization vector
/// @param piv initialization vector pointer
/// @name DoInitByIV
void PCtxHashFunctionSha1::DoInitByIV(uint8_t const* piv) noexcept
{
    std::ignore = memset(&hashDataSha_, 0, sizeof(hashDataSha_));
    if (EVP_DigestInit_ph(EVP_sha1()) != 1) {
        return;
    }

    if (CheckIV(piv)) {
        return;
    }
    // Call update to pass IV
    DoUpdate(piv, kInt_64U);
}
/// @brief Perform update operation
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionSha1::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform finalization operation
/// @name DoFinish
void PCtxHashFunctionSha1::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataSha_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Get the maximum length of IV corresponding to the specific algorithm
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionSha1::GetIvMaxLength() const noexcept { return kInt_64U; }
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionSha2_224::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Sha2_224 >()};
}
/// @brief Get hash calculation result length
/// @returns hash calculation result length
uint32_t PCtxHashFunctionSha2_224::GetHashLength() const noexcept
{
    return (sizeof(hashDataSha_)) - static_cast< uint32_t >(kPhHashTailLength);
}
/// @brief Perform Init operation: via initialization vector
/// @name DoInitByIV
/// @param piv initialization vector pointer
void PCtxHashFunctionSha2_224::DoInitByIV(uint8_t const* piv) noexcept
{
    std::ignore = memset(&hashDataSha_, 0, sizeof(hashDataSha_));
    if (EVP_DigestInit_ph(EVP_sha224()) != 1) {
        return;
    }

    if (CheckIV(piv)) {
        return;
    }
    // Call update to pass IV
    DoUpdate(piv, kInt_64U);
}
/// @brief Perform Update operation
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionSha2_224::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform Finish operation
/// @name DoFinish
void PCtxHashFunctionSha2_224::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataSha_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Get the maximum length of IV corresponding to the specific algorithm
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionSha2_224::GetIvMaxLength() const noexcept { return kInt_64U; }
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @name GetCryptoPrimitiveId
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionSha2_256::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Sha2_256 >()};
}
/// @brief Get hash result length
/// @name GetHashLength
/// @returns hash result length
uint32_t PCtxHashFunctionSha2_256::GetHashLength() const noexcept
{
    return (sizeof(hashDataSha_)) - static_cast< uint32_t >(kPhHashTailLength);
}
/// @brief Perform initialization operation: via initialization vector
/// @param piv initialization vector pointer
/// @name DoInitByIV
void PCtxHashFunctionSha2_256::DoInitByIV(uint8_t const* piv) noexcept
{
    std::ignore = memset(&hashDataSha_, 0, sizeof(hashDataSha_));
    if (EVP_DigestInit_ph(EVP_sha256()) != 1) {
        return;
    }

    if (CheckIV(piv)) {
        return;
    }
    // Call update to pass IV
    DoUpdate(piv, kInt_64U);
}
/// @brief Perform Update operation
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionSha2_256::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform finalization operation
/// @name DoFinish
void PCtxHashFunctionSha2_256::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataSha_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Get the maximum length of IV corresponding to the specific algorithm
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionSha2_256::GetIvMaxLength() const noexcept { return kInt_64U; }
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionSha2_384::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Sha2_384 >()};
}
/// @brief Get hash result length
/// @name GetHashLength
/// @returns hash result length
uint32_t PCtxHashFunctionSha2_384::GetHashLength() const noexcept
{
    return (sizeof(hashDataSha_)) - static_cast< uint32_t >(kPhHashTailLength);
}
/// @brief Perform initialization operation: via initialization vector
/// @param piv initialization vector pointer
/// @name DoInitByIV
void PCtxHashFunctionSha2_384::DoInitByIV(uint8_t const* piv) noexcept
{
    std::ignore = memset(&hashDataSha_, 0, sizeof(hashDataSha_));
    if (EVP_DigestInit_ph(EVP_sha384()) != 1) {
        return;
    }
    if (CheckIV(piv)) {
        return;
    }
    // Call update to pass IV
    DoUpdate(piv, kInt_128U);
}
/// @brief Perform Update operation
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionSha2_384::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform finalization operation
/// @name DoFinish
void PCtxHashFunctionSha2_384::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataSha_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Get the maximum length of IV corresponding to the specific algorithm
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionSha2_384::GetIvMaxLength() const noexcept
{
    return kInt_128U;  // SHA384_DIGEST_LENGTH;
}
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionSha2_512::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Sha2_512 >()};
}
/// @brief Get hash result length
/// @name GetHashLength
/// @returns hash result length
uint32_t PCtxHashFunctionSha2_512::GetHashLength() const noexcept
{
    return (sizeof(hashDataSha_)) - static_cast< uint32_t >(kPhHashTailLength);
}
/// @brief Perform initialization operation: via initialization vector
/// @param piv initialization vector pointer
/// @name DoInitByIV
void PCtxHashFunctionSha2_512::DoInitByIV(uint8_t const* piv) noexcept
{
    std::ignore = memset(&hashDataSha_, 0, sizeof(hashDataSha_));
    if (EVP_DigestInit_ph(EVP_sha512()) != 1) {
        return;
    }

    if (CheckIV(piv)) {
        return;
    }
    // Call update to pass IV
    DoUpdate(piv, kInt_128U);
}
/// @brief Perform Update operation
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionSha2_512::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform finalization operation
/// @name DoFinish
void PCtxHashFunctionSha2_512::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataSha_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Get the maximum length of IV corresponding to the specific algorithm
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionSha2_512::GetIvMaxLength() const noexcept
{
    return kInt_128U;  // SHA512_DIGEST_LENGTH;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara