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
/// @file       isoft_ctx_mac_hash.cpp
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
/// @trace_id_sr=SR_CRYPTO_01007
/// @unit_name=PCtxMacHash
/// @unit_description=Hash-based Message Authentication Code Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/mac/isoft_ctx_mac_hash.h"

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_digest_mac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
// PCtxMacHash
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
PCtxMacHash::PCtxMacHash(PCryptoProvider &cryptoProvider) noexcept : PCtxMac_Base{cryptoProvider}
{
    evpMdHmacCtx_ = EVP_MD_CTX_new();
}
/// @brief Destructor
PCtxMacHash::~PCtxMacHash() noexcept
{
    if (evpMdHmacCtx_ != nullptr) {
        EVP_MD_CTX_free(evpMdHmacCtx_);
        evpMdHmacCtx_ = nullptr;
    }
}
/// @brief Get a DigestService instance. // Multiple calls, each time a new object is created
/// @returns DigestService instance
DigestService::Uptr PCtxMacHash::GetDigestService() const noexcept
{
    return {std::make_unique< PDigestServiceMac< PCtxMacHash > >(*this)};
}
/// @brief MAC init local operation, can be called externally
/// @param piv Pointer to initialization vector
/// @return  true if DoInitLocal sucess false otherwise
bool PCtxMacHash::DoInitLocal(uint8_t const *piv) noexcept
{
    std::ignore = piv;
    uint8_t const *pdata{nullptr};
    size_t len{0U};
    PKeySymmetric_Base const *const pSymmetricKey{dynamic_cast< PKeySymmetric_Base const * >(GetKey())};
    if (pSymmetricKey == nullptr) {
        return false;
    }
    pdata = pSymmetricKey->GetKeySymmetric();
    if (pdata == nullptr) {
        return false;
    }
    len = pSymmetricKey->GetPayloadSize();
    if ((pdata == nullptr) || (len == 0U)) {
        return false;
    }
    if (EVP_DigestInit_mac(PCtxHashFunction::GetEVP_MD(GetHashAlgId()), pdata, static_cast< int32_t >(len)) != 1) {
        return false;
    }
    InitMacResult();
    return true;
}
/// @brief Perform local finalization logic
/// @returns  true if DoFinishLocal sucess false otherwise
bool PCtxMacHash::DoFinishLocal() noexcept
{
    ara::core::Vector< u_char > vecMd;
    size_t len{static_cast< size_t >(EVP_MAX_MD_SIZE)};
    vecMd.resize(len);
    u_char *const pMd{vecMd.data()};

    if (EVP_DigestFinal_mac(static_cast< uint8_t * >(pMd), &len) != 1) {
        return false;
    }
    std::ignore = std::memcpy(static_cast< void * >(GetMacResult()), static_cast< void const * >(pMd),
                              static_cast< std::size_t >(len));
    return true;
}

/// @brief Perform local reset operation
/// @returns  true if DoResetLocal sucess false otherwise
bool PCtxMacHash::DoResetLocal() noexcept
{
    if (EVP_MD_CTX_reset(evpMdHmacCtx_) == 0) {
        return false;
    }
    return true;
}
/// @brief SWS_CRYPT_01204: The Update function will perform the configured hash algorithm calculation
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @returns  true if DoUpdateLocal sucess false otherwise
bool PCtxMacHash::DoUpdateLocal(void const *pVoidData, uint32_t nDataLen) noexcept
{
    if (EVP_DigestUpdate_mac(T_TransBytes(pVoidData), static_cast< std::size_t >(nDataLen)) != 1) {
        return false;
    }
    return true;
}

/// @brief Check whether the key meets the requirements
/// @param key Symmetric key
/// @returns  true if check key sucess false otherwiseool
bool PCtxMacHash::CheckKey(SymmetricKey const &key) const noexcept
{
    std::ignore = key;
    return true;
}

/// @brief Hash does not require IV, return -1 to make the judgment always false
/// @name   GetIvMaxLength()
/// @returns Maximum IV length
uint32_t PCtxMacHash::GetIvMaxLength() const noexcept
{
    uint32_t const ivLen{0U};
    return ivLen;
}

/// @brief Check whether IV operations are supported
/// @name   SupportIv()
/// @returns true if support iv false otherwise
bool PCtxMacHash::SupportIv() const noexcept { return false; }
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara