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
/// @file       isoft_ctx_ae_stream_aes.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Authenticated encryption based on AES stream encryption method.
/// @date       2022-05-10
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Authenticated Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01011
/// @unit_name=PCtxAeStreamAes_Base
/// @unit_description=Authenticated encryption based on AES stream encryption base class
/// @endcode
///
/// ================================================================
///
/// AE AuthCipherCtx abbreviation
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_AES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_AES_H_

#include "ara/crypto/cryp/ae/isoft_ctx_ae_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Authenticated encryption based on AES stream encryption method.
/// @code{.isoft}
/// @tparam T_Mac
/// @tparam T_CtxSymmetricStream
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00524
/// @trace_id_dd=DD_CRYPTO_01958
/// @needwork = ad
/// @endcode
template < typename T_Mac, typename T_CtxSymmetricStream >
class PCtxAeStreamAes_Base : public PCtxAe_Base< T_Mac, T_CtxSymmetricStream >
{
public:
    /// @brief Constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01959
    /// @needwork = dda
    /// @endcode
    explicit PCtxAeStreamAes_Base(PCryptoProvider& cryptoProvider) noexcept(noexcept(std::make_unique< T_Mac >(
        cryptoProvider)) && noexcept(std::make_unique< T_CtxSymmetricStream >(cryptoProvider)));
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01960
    /// @needwork = dda
    /// @endcode
    ~PCtxAeStreamAes_Base() override = default;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01961
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Base& operator=(PCtxAeStreamAes_Base const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01962
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Base& operator=(PCtxAeStreamAes_Base&& other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01963
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Base(PCtxAeStreamAes_Base&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01964
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Base(PCtxAeStreamAes_Base const& other) noexcept = delete;
};
//********************************/
/// @brief Authenticated encryption based on AES-CBC stream encryption method.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00525
/// @trace_id_dd=DD_CRYPTO_01965
/// @unit_name=PCtxAeStreamAes_Cbc
/// @unit_description=Authenticated encryption based on MAC-AES-CBC and AES-CFB128
/// @needwork = ad
/// @endcode
class PCtxAeStreamAes_Cbc : public PCtxAeStreamAes_Base< PCtxMacAesCbc_128, PCtxSymmetricStream_Aes_Cfb128 >
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06304
    /// @needwork = dda
    /// @endcode
    using PCtxAeStreamAes_Base< PCtxMacAesCbc_128, PCtxSymmetricStream_Aes_Cfb128 >::PCtxAeStreamAes_Base;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01966
    /// @needwork = dda
    /// @endcode
    ~PCtxAeStreamAes_Cbc() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01967
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Cbc(PCtxAeStreamAes_Cbc&& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01968
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Cbc(PCtxAeStreamAes_Cbc const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01969
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Cbc& operator=(PCtxAeStreamAes_Cbc&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01970
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamAes_Cbc& operator=(PCtxAeStreamAes_Cbc const& other) = delete;

public:
    /// @brief Get the BlockService instance.
    /// @brief Get BlockService instance.
    /// @return BlockService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20102}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00526
    /// @trace_id_dd=DD_CRYPTO_01971
    /// @needwork = ad
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Return the CryptoPrimtivId instance containing the instance identifier.
    /// @return encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01972
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_AES_H_