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
/// @file       isoft_ctx_mac_des.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code implementation based on DES encryption.
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
/// <tr><td>2023-8-10   <td>1.0.0    <td>Che Jinzhao  <td>Improve functional requirements
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01006
/// @unit_name=PCtxMacDes
/// @unit_description=Message Authentication Code Context Based on DES
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_H_

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"
#include "ara/crypto/openssl/isoft_openssl_cmac.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00248
/// @trace_id_dd=DD_CRYPTO_01159
/// @needwork = dd
/// @endcode
constexpr int32_t kMacDesDataLength{kInt_8};
/// @brief Message authentication code implementation based on DES encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00248
/// @trace_id_dd=DD_CRYPTO_01160
/// @needwork = ad
/// @endcode
class PCtxMacDes : public PCtxMac_Base
{
public:
    /// @brief Parameterized constructor
    /// @name PCtxMacDes
    /// @param cryptoProvider Encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01161
    /// @needwork = dda
    /// @endcode
    explicit PCtxMacDes(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01162
    /// @needwork = dda
    /// @endcode
    PCtxMacDes() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01163
    /// @needwork = dda
    /// @endcode
    ~PCtxMacDes() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01164
    /// @needwork = dda
    /// @endcode
    PCtxMacDes(PCtxMacDes&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01165
    /// @needwork = dda
    /// @endcode
    PCtxMacDes& operator=(PCtxMacDes&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01166
    /// @needwork = dda
    /// @endcode
    PCtxMacDes& operator=(PCtxMacDes const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01167
    /// @needwork = dda
    /// @endcode
    PCtxMacDes(PCtxMacDes const& other) = delete;

private:
    /// @brief MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01169
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macDesData_[kMacDesDataLength]{kInt8_0U};

public:
    /// @brief Gets the DigestService instance.
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01170
    /// @needwork = dda
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;
    /// @brief Gets the maximum IV length corresponding to the specific algorithm
    /// @returns Maximum IV length corresponding to the algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01171
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
    /// @brief Get MAC result length: in bytes
    /// @returns MAC result length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01172
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief MAC init local operation, callable externally
    /// @param piv Pointer to initialization vector
    /// @returns true if DoInitLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01173
    /// @needwork = dda
    /// @endcode
    bool DoInitLocal(uint8_t const* piv) noexcept override;
    /// @brief Execute Finish operation locally
    /// @returns true if DoFinishLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01174
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
    /// @trace_id_dd=DD_CRYPTO_01175
    /// @needwork = dda
    /// @endcode
    bool DoUpdateLocal(void const* pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief MAC reset local operation, callable externally
    /// @returns true if DoResetLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01176
    /// @needwork = dda
    /// @endcode
    bool DoResetLocal() noexcept override;
    /// @brief Get MAC calculation result
    /// @returns MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01177
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macDesData_; }

protected:
    /// @brief Initialize member variables
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01178
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macDesData_), 0, sizeof(macDesData_));
    }
    /// @brief Get EVP_CIPHER structure pointer
    /// @returns EVP_CIPHER structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01179
    /// @needwork = dda
    /// @endcode
    virtual EVP_CIPHER const* GetEvpCipher() const noexcept = 0;
    /// @brief Get crypto primitive ID
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01180
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
    /// @brief Check if IV operation is supported
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01181
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_DES_H_
