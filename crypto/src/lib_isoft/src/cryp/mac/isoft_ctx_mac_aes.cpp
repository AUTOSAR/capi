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
/// @file       isoft_ctx_mac_aes.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-04-15
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=PCtxMacAes
/// @unit_description=AES-based Message Authentication Code Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/mac/isoft_ctx_mac_aes.h"

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_md5.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_digest_mac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

///****************/// PCtxMacCbc
/// @brief Constructor with parameters
/// @name  PCtxMacAes
/// @param cryptoProvider Crypto provider
PCtxMacAes::PCtxMacAes(PCryptoProvider &cryptoProvider) noexcept : PCtxMac_Base{cryptoProvider} {}

/// @brief Get a DigestService instance. // Multiple calls, each time a new object is created
/// @returns DigestService instance
DigestService::Uptr PCtxMacAes::GetDigestService() const noexcept
{
    return {std::make_unique< PDigestServiceMac< PCtxMacAes > >(*this)};
}
/// @brief Perform the Init operation locally
/// @param piv Pointer to initialization vector
/// @return true if DoInitLocal sucess false otherwise
bool PCtxMacAes::DoInitLocal(uint8_t const *piv) noexcept
{
    uint8_t const *pdata{nullptr};
    size_t len{0U};
    PKeySymmetric_Aes const *const pSymmetricKeyAes{dynamic_cast< PKeySymmetric_Aes const * >(GetKey())};
    if (pSymmetricKeyAes == nullptr) {
        return false;
    }
    pdata = pSymmetricKeyAes->GetKeySymmetric();
    if (pdata == nullptr) {
        return false;
    }
    len = pSymmetricKeyAes->GetPayloadSize();
    if ((pdata == nullptr) || (len == 0U)) {
        return false;
    }

    if (CMAC_Init(cmacCtx_, pdata, len, GetEvpCipher(), nullptr, piv) == 0) {
        return false;
    }
    InitMacResult();
    return true;
}
/// @brief Perform the Finish operation locally
/// @returns  true if DoFinishLocal sucess false otherwise
bool PCtxMacAes::DoFinishLocal() noexcept
{
    ara::core::Vector< u_char > vecMd;
    vecMd.resize(static_cast< size_t >(EVP_MAX_MD_SIZE));
    u_char *const pMd{vecMd.data()};

    size_t len{0U};
    if (CMAC_Final(cmacCtx_, static_cast< uint8_t * >(pMd), &len) == 0) {
        return false;
    }
    std::ignore = std::memcpy(static_cast< void * >(macAesData_), static_cast< void const * >(pMd), len);
    return true;
}
/// @brief Perform the Reset operation locally
/// @returns  true if DoResetLocal sucess false otherwise
bool PCtxMacAes::DoResetLocal() noexcept
{
    if (CMAC_resume(cmacCtx_) == 0) {
        return false;
    }
    return true;
}
/// @brief Perform the Update operation locally
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @returns  true if DoUpdateLocal sucess false otherwise
bool PCtxMacAes::DoUpdateLocal(void const *pVoidData, uint32_t nDataLen) noexcept
{
    if (CMAC_Update(cmacCtx_, pVoidData, static_cast< std::size_t >(nDataLen)) == 0) {
        return false;
    }
    return true;
}
/// @brief Get the maximum length of the corresponding IV according to the specific algorithm
/// @name   GetIvMaxLength()
/// @returns Maximum length of the corresponding IV
uint32_t PCtxMacAes::GetIvMaxLength() const noexcept
{
    EVP_CIPHER const *const cipher{GetEvpCipher()};
    uint32_t const ivLen{static_cast< uint32_t >(EVP_CIPHER_iv_length(cipher))};
    return ivLen;
}

/// @brief Get the MAC result length: in bytes
/// @returns MAC result length
uint32_t PCtxMacAes::GetMacLength() const noexcept { return static_cast< uint32_t >(sizeof(macAesData_)); }

/// @brief Check whether the key meets the requirements
/// @param key Symmetric key
/// @returns  true if check key sucess false otherwise
bool PCtxMacAes::CheckKey(SymmetricKey const &key) const noexcept
{
    PAlgId_Symmetric_AesKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief [SWS_CRYPT_01209] If the signature object is generated by a keyed MAC/HMAC/AE/AEAD algorithm, the hash algorithm ID field of the signature is set to unknown
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxMacAes::GetHashAlgId() const noexcept
{
    return kAlgIdUndefined;  // aes has nothing to do with hash, return kAlgIdUndefined
}

/// @brief Check whether IV operations are supported
/// @name   SupportIv()
/// @returns true if support iv false otherwise
bool PCtxMacAes::SupportIv() const noexcept { return true; }
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
