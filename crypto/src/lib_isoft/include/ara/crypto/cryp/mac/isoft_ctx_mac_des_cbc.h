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
/// @file       isoft_ctx_mac_des_cbc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code implementation based on des_cbc encryption.
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
/// @trace_id_sr=SR_CRYPTO_01006
/// @unit_name=PCtxMacDesCbc
/// @unit_description=Message Authentication Code Context Based on DES-CBC
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_CBC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_CBC_H_

#include "ara/crypto/cryp/mac/isoft_ctx_mac_des.h"
#include "ara/crypto/openssl/isoft_openssl_cmac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Message authentication code implementation based on des_cbc encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00260
/// @trace_id_dd=DD_CRYPTO_01323
/// @needwork = ad
/// @endcode
class PCtxMacDesCbc : public PCtxMacDes
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06275
    /// @needwork = dda
    /// @endcode
    using PCtxMacDes::PCtxMacDes;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01324
    /// @needwork = dda
    /// @endcode
    ~PCtxMacDesCbc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01325
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc(PCtxMacDesCbc&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01326
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc& operator=(PCtxMacDesCbc&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01327
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc& operator=(PCtxMacDesCbc const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01328
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc(PCtxMacDesCbc const& other) = delete;

public:
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01329
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
    /// @trace_id_dd=DD_CRYPTO_01330
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01331
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
};
/// @brief Message authentication code implementation based on des_cbc_ede2 encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00261
/// @trace_id_dd=DD_CRYPTO_01332
/// @unit_name=PCtxMacDesCbc_Ede2
/// @unit_description=Message Authentication Code Context Based on 2DES-CBC
/// @needwork = ad
/// @endcode
class PCtxMacDesCbc_Ede2 : public PCtxMacDes
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06276
    /// @needwork = dda
    /// @endcode
    using PCtxMacDes::PCtxMacDes;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacDesCbc_Ede2
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01333
    /// @needwork = dda
    /// @endcode
    ~PCtxMacDesCbc_Ede2() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01334
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede2(PCtxMacDesCbc_Ede2&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01335
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede2& operator=(PCtxMacDesCbc_Ede2&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01336
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede2& operator=(PCtxMacDesCbc_Ede2 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01337
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede2(PCtxMacDesCbc_Ede2 const& other) = delete;

public:
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01338
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
    /// @trace_id_dd=DD_CRYPTO_01339
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01340
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
};
/// @brief Message authentication code implementation based on des_cbc_ede3 encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00262
/// @trace_id_dd=DD_CRYPTO_01341
/// @unit_name=PCtxMacDesCbc_Ede3
/// @unit_description=Message Authentication Code Context Based on 3DES-CBC
/// @needwork = ad
/// @endcode
class PCtxMacDesCbc_Ede3 : public PCtxMacDes
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06277
    /// @needwork = dda
    /// @endcode
    using PCtxMacDes::PCtxMacDes;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01342
    /// @needwork = dda
    /// @endcode
    ~PCtxMacDesCbc_Ede3() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01343
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede3(PCtxMacDesCbc_Ede3&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01344
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede3& operator=(PCtxMacDesCbc_Ede3&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01345
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede3& operator=(PCtxMacDesCbc_Ede3 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01346
    /// @needwork = dda
    /// @endcode
    PCtxMacDesCbc_Ede3(PCtxMacDesCbc_Ede3 const& other) = delete;

public:
    /// @brief Get crypto primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01347
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Check key
    /// @name CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01348
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Get EVP_CIPHER structure pointer
    /// @name   GetEvpCipher
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01349
    /// @needwork = dda
    /// @endcode
    EVP_CIPHER const* GetEvpCipher() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_CBC_H_