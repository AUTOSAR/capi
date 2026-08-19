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
/// @file       isoft_ctx_mac_hash_md5.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code implementation based on hash_md5.
/// @date       2022-04-15
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01007
/// @unit_name=PCtxMacHashMd5
/// @unit_description=Message Authentication Code Based on HASH MD5
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_MD5_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_MD5_H_

#include <openssl/hmac.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_hash.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Message authentication code implementation based on hash_md5.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00252
/// @trace_id_dd=DD_CRYPTO_01209
/// @needwork = ad
/// @endcode
class PCtxMacHashMd5 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06269
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01210
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashMd5() override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01211
    /// @needwork = dda
    /// @endcode
    PCtxMacHashMd5(PCtxMacHashMd5&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01212
    /// @needwork = dda
    /// @endcode
    PCtxMacHashMd5& operator=(PCtxMacHashMd5&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01213
    /// @needwork = dda
    /// @endcode
    PCtxMacHashMd5& operator=(PCtxMacHashMd5 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01214
    /// @needwork = dda
    /// @endcode
    PCtxMacHashMd5(PCtxMacHashMd5 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01215
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_16U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01216
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get MAC length
    /// @return MAC length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01217
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the starting address of the MAC calculation result
    /// @return starting address of the MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01218
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01219
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the hash algorithm ID of the digest information; by default, MD5 is used here
    /// @return hash algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01220
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_MD5_H_