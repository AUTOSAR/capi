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
/// @file       isoft_ctx_mac_aes.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Message authentication code implemented based on AES encryption function interface.
/// @date       2022-04-15
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr>}<2021-12-21  </td>1.0.0    </td>Chang Zheng    </td>Create initial version</td>
/// <tr>}<2023-8-10   </td>1.0.0    </td>Che Jinzhao  </td>Improve function functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Message authentication code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=PCtxMacAes
/// @unit_description=AES-based message authentication code context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_H_

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"
#include "ara/crypto/openssl/isoft_openssl_cmac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_01221
/// @needwork = dd
/// @endcode
constexpr int32_t kMacAesDataLength{kInt_16};
/// @brief Message authentication code implemented based on AES encryption function interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00253
/// @trace_id_dd=DD_CRYPTO_01222
/// @needwork = ad
/// @endcode
class PCtxMacAes : public PCtxMac_Base
{
public:
    /// @brief Parameterized constructor
    /// @name PCtxMacAes
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01223
    /// @needwork = dda
    /// @endcode
    explicit PCtxMacAes(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01224
    /// @needwork = dda
    /// @endcode
    PCtxMacAes() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01225
    /// @needwork = dda
    /// @endcode
    ~PCtxMacAes() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01226
    /// @needwork = dda
    /// @endcode
    PCtxMacAes(PCtxMacAes&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01227
    /// @needwork = dda
    /// @endcode
    PCtxMacAes& operator=(PCtxMacAes&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01228
    /// @needwork = dda
    /// @endcode
    PCtxMacAes& operator=(PCtxMacAes const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01229
    /// @needwork = dda
    /// @endcode
    PCtxMacAes(PCtxMacAes const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01231
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macAesData_[kMacAesDataLength]{kInt8_0U};

public:
    /// @brief Gets the DigestService instance.
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01232
    /// @needwork = dda
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;

    /// @brief Gets the maximum IV length corresponding to the specific algorithm
    /// @returns Corresponding maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01233
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
    /// @brief Get MAC result length: in bytes
    /// @returns MAC result length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01234
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Execute Init operation locally
    /// @param piv Pointer to initialization vector
    /// @returns true if DoInitLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01235
    /// @needwork = dda
    /// @endcode
    bool DoInitLocal(uint8_t const* piv) noexcept override;
    /// @brief Execute Finish operation locally
    /// @returns true if DoFinishLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01236
    /// @needwork = dda
    /// @endcode
    bool DoFinishLocal() noexcept override;
    /// @brief Execute Update operation locally
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @returns true if DoUpdateLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01237
    /// @needwork = dda
    /// @endcode
    bool DoUpdateLocal(void const* pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Execute Reset operation locally
    /// @returns true if DoResetLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01238
    /// @needwork = dda
    /// @endcode
    bool DoResetLocal() noexcept override;
    /// @brief Get MAC calculation result
    /// @returns Pointer to MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01239
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macAesData_; }

protected:
    /// @brief Initialize member variables
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01240
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macAesData_), 0, sizeof(macAesData_));
    }
    /// @brief Check key
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01241
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Get EVP_CIPHER pointer
    /// @returns EVP_CIPHER pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01242
    /// @needwork = dda
    /// @endcode
    virtual EVP_CIPHER const* GetEvpCipher() const noexcept = 0;
    /// @brief Get crypto primitive ID
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01243
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;

    /// @brief Check if IV operation is supported
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01244
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_H_
