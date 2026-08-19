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
/// @file       isoft_ctx_mac_aes_cbc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code implementation based on aes_cbc encryption.
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
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=PCtxMacAesCbc_128
/// @unit_description=Message Authentication Code Context Based on AES-CBC-128
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_CBC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_CBC_H_

#include "ara/crypto/cryp/mac/isoft_ctx_mac_aes.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Message authentication code implementation based on aes_cbc_128 encryption
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00249
/// @trace_id_dd=DD_CRYPTO_01182
/// @needwork = ad
/// @endcode
class PCtxMacAesCbc_128 : public PCtxMacAes
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06266
    /// @needwork = dda
    /// @endcode
    using PCtxMacAes::PCtxMacAes;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacAesCbc_128
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01183
    /// @needwork = dda
    /// @endcode
    ~PCtxMacAesCbc_128() override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01184
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_128(PCtxMacAesCbc_128&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01185
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_128& operator=(PCtxMacAesCbc_128&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01186
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_128& operator=(PCtxMacAesCbc_128 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01187
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_128(PCtxMacAesCbc_128 const& other) = delete;

public:
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01188
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01189
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Check key
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01190
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
};

/// @brief Message authentication code implementation based on aes_cbc_192 encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00250
/// @trace_id_dd=DD_CRYPTO_01191
/// @unit_name=PCtxMacAesCbc_192
/// @unit_description=Message Authentication Code Context Based on AES-CBC-192
/// @needwork = ad
/// @endcode
class PCtxMacAesCbc_192 : public PCtxMacAes
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06267
    /// @needwork = dda
    /// @endcode
    using PCtxMacAes::PCtxMacAes;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacAesCbc_192
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01192
    /// @needwork = dda
    /// @endcode
    ~PCtxMacAesCbc_192() override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01193
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_192(PCtxMacAesCbc_192&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01194
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_192& operator=(PCtxMacAesCbc_192&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01195
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_192& operator=(PCtxMacAesCbc_192 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01196
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_192(PCtxMacAesCbc_192 const& other) = delete;

public:
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01197
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01198
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Check key
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01199
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
};

/// @brief Message authentication code implementation based on aes_cbc_256 encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00251
/// @trace_id_dd=DD_CRYPTO_01200
/// @unit_name=PCtxMacAesCbc_256
/// @unit_description=Message Authentication Code Context Based on AES-CBC-256
/// @needwork = ad
/// @endcode
class PCtxMacAesCbc_256 : public PCtxMacAes
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06268
    /// @needwork = dda
    /// @endcode
    using PCtxMacAes::PCtxMacAes;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacAesCbc_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01201
    /// @needwork = dda
    /// @endcode
    ~PCtxMacAesCbc_256() override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01202
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_256(PCtxMacAesCbc_256&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01203
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_256& operator=(PCtxMacAesCbc_256&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01204
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_256& operator=(PCtxMacAesCbc_256 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01205
    /// @needwork = dda
    /// @endcode
    PCtxMacAesCbc_256(PCtxMacAesCbc_256 const& other) = delete;

public:
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01206
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01207
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Check key
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01208
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_AES_CBC_H_