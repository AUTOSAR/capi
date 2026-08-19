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
/// @file       isoft_ctx_hash_function_crc.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/HashFunction
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01020
/// @unit_name=PCtxHashFunctionCrc
/// @unit_description=CRC-based hash context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_crc.h"

#include <ara/core/optional.h>
#include <isoft/crc/crc.h>

#include <memory>

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_digest_hash_crc.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
template class T_PCtxHashFunctionCrc< uint8_t >;
template class T_PCtxHashFunctionCrc< uint16_t >;
template class T_PCtxHashFunctionCrc< uint32_t >;
template class T_PCtxHashFunctionCrc< uint64_t >;
//********************************/
/// @brief Get DigestService instance. // Multiple calls will create a new object each time.
/// @name GetDigestService
/// @returns DigestService instance
DigestService::Uptr PCtxHashFunctionCrc::GetDigestService() const noexcept
{
    return {std::make_unique< PDigestServiceHashCrc >(*this)};
}
//***************/
/// @brief Check whether the specific hash function supports iv; CRC series do not need iv
/// @name   SupportIv
/// @returns true if support iv false otherwise
bool PCtxHashFunctionCrc::SupportIv() const noexcept { return false; }
/// @brief Get the maximum length of IV corresponding to the specific algorithm. CRC series do not need iv, so the length is directly set to -1
/// @name   GetIvMaxLength
/// @returns maximum length of corresponding IV
uint32_t PCtxHashFunctionCrc::GetIvMaxLength() const noexcept
{
    uint32_t const ivLen{UINT32_MAX};
    return ivLen;
}
/// @brief Perform initialization logic via initialization vector
/// @name   DoInitByIV
/// @param piv initialization vector pointer
void PCtxHashFunctionCrc::DoInitByIV(uint8_t const *piv) noexcept { std::ignore = piv; }
//********************************/
/// @brief Return the length of the hash result
/// @name GetHashLength
/// @returns length of hash result
template < typename T_Data >
inline uint32_t T_PCtxHashFunctionCrc< T_Data >::GetHashLength() const noexcept
{
    return sizeof(nCrcResult_);
}
//***************/
/// @brief Return CRC calculation result
/// @name GetHashResult
/// @returns hash result pointer
template < typename T_Data >
inline uint8_t const *T_PCtxHashFunctionCrc< T_Data >::GetHashResult() const noexcept
{
    return static_cast< uint8_t const * >(static_cast< void const * >(&nCrcResult_));
}
//***************/
/// @brief Perform initialization logic
/// @name DoInit
template < typename T_Data >
inline void T_PCtxHashFunctionCrc< T_Data >::DoInit() noexcept
{
    nCrcResult_ = 0U;
}
/// @brief Perform update logic
/// @name DoUpdate
/// @param pVoidData data start address
/// @param nDataLen data length
template < typename T_Data >
inline void T_PCtxHashFunctionCrc< T_Data >::DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept
{
    ara::core::Optional< isoft::crc::BufferView > maybeBuffer;
    try {
        std::ignore = maybeBuffer.emplace(T_TransBytes(pVoidData), static_cast< std::size_t >(nDataLen));
    } catch (...) {
        // Construction failed, cannot calculate CRC, return initial value or error code
        nCrcResult_ = 0U;
        return;
    }
    std::ignore = _DoCalculateCRC(*maybeBuffer);
}
//***************/
/// @brief Perform CRC calculation logic
/// @name _DoCalculateCRC
/// @param bufferView data to be calculated
/// @returns CRC result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06655
/// @needwork = dd
/// @endcode
template <>
uint8_t T_PCtxHashFunctionCrc< uint8_t >::_DoCalculateCRC(isoft::crc::BufferView const &bufferView) noexcept
{
    nCrcResult_ = isoft::crc::CRC::CalculateCRC8(bufferView, nCrcResult_ == 0U, nCrcResult_);
    return nCrcResult_;
}
/// @brief Perform CRC calculation logic
/// @name _DoCalculateCRC
/// @param bufferView data to be calculated
/// @returns CRC result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06656
/// @needwork = dd
/// @endcode
template <>
uint16_t T_PCtxHashFunctionCrc< uint16_t >::_DoCalculateCRC(isoft::crc::BufferView const &bufferView) noexcept
{
    nCrcResult_ = isoft::crc::CRC::CalculateCRC16(bufferView, nCrcResult_ == 0U, nCrcResult_);
    return nCrcResult_;
}
/// @brief Perform CRC calculation logic
/// @name _DoCalculateCRC
/// @param bufferView data to be calculated
/// @returns CRC result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06657
/// @needwork = dd
/// @endcode
template <>
uint32_t T_PCtxHashFunctionCrc< uint32_t >::_DoCalculateCRC(isoft::crc::BufferView const &bufferView) noexcept
{
    nCrcResult_ = isoft::crc::CRC::CalculateCRC32(bufferView, nCrcResult_ == 0U, nCrcResult_);
    return nCrcResult_;
}
/// @brief Perform CRC calculation logic
/// @name _DoCalculateCRC
/// @param bufferView data to be calculated
/// @returns CRC result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06658
/// @needwork = dd
/// @endcode
template <>
uint64_t T_PCtxHashFunctionCrc< uint64_t >::_DoCalculateCRC(isoft::crc::BufferView const &bufferView) noexcept
{
    nCrcResult_ = isoft::crc::CRC::CalculateCRC64(bufferView, nCrcResult_ == 0U, nCrcResult_);
    return nCrcResult_;
}
//********************************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionCrc8::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Crc8 >()};
}
//***************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionCrc16::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Crc16 >()};
}
//***************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionCrc32::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Crc32 >()};
}
//***************/
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxHashFunctionCrc64::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Crc64 >()};
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
