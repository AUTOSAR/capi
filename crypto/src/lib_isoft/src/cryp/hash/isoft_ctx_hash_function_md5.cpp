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
/// @file       isoft_ctx_hash_function_md5.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-01-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/HashFunction
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01021
/// @unit_name=PCtxHashFunctionMd5
/// @unit_description=MD5-based hash context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_md5.h"

#include <memory>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_digest_hash.h"
#include "openssl/md5.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionMd5::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Md5 >()};
}
/// @brief Get DigestService instance. // Multiple calls will create a new object each time
/// @return DigestService instance
DigestService::Uptr PCtxHashFunctionMd5::GetDigestService() const noexcept
{
    return {std::make_unique< PDigestServiceHash< PCtxHashFunctionMd5 > >(*this)};
}
//***************/
/// @brief Get hash result length: byte length
/// @return hash result length
uint32_t PCtxHashFunctionMd5::GetHashLength() const noexcept
{
    return (sizeof(hashDataMD5_)) - static_cast< uint32_t >(kPhHashTailLength);
}
//***************/
/// @brief Perform Init logic
void PCtxHashFunctionMd5::DoInit() noexcept
{
    std::ignore = memset(&hashDataMD5_, 0, sizeof(hashDataMD5_));
    if (EVP_DigestInit_ph(EVP_md5()) != 1) {
        return;
    }
}
/// @brief Perform Update logic
/// @param pVoidData data start address
/// @param nDataLen data length
void PCtxHashFunctionMd5::DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_ph(pVoidData, nDataLen) != 1) {
        return;
    }
}
/// @brief Perform Finish logic
void PCtxHashFunctionMd5::DoFinish() noexcept
{
    uint32_t nDataLen{0U};
    if (EVP_DigestFinal_ph(hashDataMD5_, &nDataLen) != 1) {
        return;
    }
}
/// @brief Check whether the specific hash function supports iv; MD5 series do not need iv
/// @name   SupportIv
/// @returns true if support iv false otherwise
bool PCtxHashFunctionMd5::SupportIv() const noexcept { return false; }
/// @brief Get the maximum length of IV corresponding to the specific algorithm. MD5 series do not need iv, so the length is directly set to -1
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionMd5::GetIvMaxLength() const noexcept
{
    uint32_t const ivLen{0U};
    return ivLen;
}
/// @brief Initialize MD5 hash context via initialization vector
/// @name   DoInitByIV
/// @param piv initialization vector pointer
void PCtxHashFunctionMd5::DoInitByIV(uint8_t const* piv) noexcept { std::ignore = piv; }
//********************************/
/// @brief Get hash result length: byte length
/// @return MD5 data length
uint32_t PCtxHashFunctionMd5_16::GetHashLength() const noexcept { return kInt_8U; }
/// @brief Get hash result
/// @return MD5 data pointer
uint8_t const* PCtxHashFunctionMd5_16::GetHashResult() const noexcept { return _GetHashDataMD5() + kInt_4U; }
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara