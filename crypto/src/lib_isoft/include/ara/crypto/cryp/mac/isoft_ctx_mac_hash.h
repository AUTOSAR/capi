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
/// @file       isoft_ctx_mac_hash.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code implementation based on hash.
/// @date       2022-04-15
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>Chang Zheng         <td>Initial version creation
/// <tr><td>2023-8-10   <td>1.0.0    <td>Che Jinzhao  <td>Add functions
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01007
/// @unit_name=PCtxMacHash
/// @unit_description=Message Authentication Code Base Class Based on HASH
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_H_

#include <openssl/hmac.h>

#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief MAC class using HASH
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00254
/// @trace_id_dd=DD_CRYPTO_01245
/// @needwork = ad
/// @endcode
class PCtxMacHash : public PCtxMac_Base
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01246
    /// @needwork = dda
    /// @endcode
    explicit PCtxMacHash(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01247
    /// @needwork = dda
    /// @endcode
    PCtxMacHash() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01248
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHash() noexcept override;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01249
    /// @needwork = dda
    /// @endcode
    PCtxMacHash(PCtxMacHash&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01250
    /// @needwork = dda
    /// @endcode
    PCtxMacHash& operator=(PCtxMacHash&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01251
    /// @needwork = dda
    /// @endcode
    PCtxMacHash& operator=(PCtxMacHash const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01252
    /// @needwork = dda
    /// @endcode
    PCtxMacHash(PCtxMacHash const& other) = delete;

private:
    /// @brief EVP_MD_CTX MAC context
    EVP_MD_CTX* evpMdHmacCtx_;

public:
    /// @brief Initialize
    /// @param type
    /// @return 1 success, others failure
    int EVP_DigestInit_mac(const EVP_MD* type, const u_char* key, int keylen)
    {
        // 1. Create EVP_PKEY encapsulation for HMAC key
        EVP_PKEY* pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, nullptr, key, keylen);
        if (pkey == nullptr) {
            return 0;
        }

        // Only proceed to set HMAC key if the previous step succeeded
        if (EVP_DigestSignInit(evpMdHmacCtx_, nullptr, type, nullptr, pkey) != 1) {
            return 0;
        }
        return 1;
    }
    /// @brief Update
    /// @param d Data to update
    /// @param cnt Length of data to update
    /// @return 1 success, others failure
    int EVP_DigestUpdate_mac(const void* data, size_t dataLen)
    {
        return EVP_DigestUpdate(evpMdHmacCtx_, data, dataLen);
    }
    /// @brief Finalize
    /// @param md MAC result
    /// @param s MAC result length
    /// @return 1 success, others failure
    int EVP_DigestFinal_mac(u_char* md, size_t* s) { return EVP_DigestSignFinal(evpMdHmacCtx_, md, s); }

public:
    /// @brief Get digest service
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01254
    /// @needwork = dda
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;
    /// @brief Hash does not require IV, return -1 to make the check always false
    /// @returns Maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01255
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
    /// @brief MAC init local operation, callable externally
    /// @param piv Pointer to initialization vector
    /// @returns true if DoInitLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01256
    /// @needwork = dda
    /// @endcode
    bool DoInitLocal(uint8_t const* piv) noexcept override;
    /// @brief Execute finalization locally
    /// @returns true if DoFinishLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01257
    /// @needwork = dda
    /// @endcode
    bool DoFinishLocal() noexcept override;
    /// @brief MAC update local operation, callable externally
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @returns true if DoUpdateLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01258
    /// @needwork = dda
    /// @endcode
    bool DoUpdateLocal(void const* pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief MAC reset local operation, callable externally
    /// @returns true if DoResetLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01259
    /// @needwork = dda
    /// @endcode
    bool DoResetLocal() noexcept override;

protected:
    /// @brief Check key
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01260
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Check if IV operation is supported
    /// @returns  true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01262
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_H_
