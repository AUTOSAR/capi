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
/// @file       isoft_ctx_ae_stream_des.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Authenticated encryption based on DES stream encryption method.
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
/// @trace_id_sr=SR_CRYPTO_01012
/// @unit_name=PCtxAeStreamDes_Base
/// @unit_description=Authenticated encryption based on DES stream encryption base class
/// @endcode
///
/// ================================================================
///
/// AE AuthCipherCtx abbreviation
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_DES_H_

#include "ara/crypto/cryp/ae/isoft_ctx_ae_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Authenticated encryption based on DES stream encryption method.
/// @code{.isoft}
/// @tparam T_Mac
/// @tparam T_CtxSymmetricStream
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00527
/// @trace_id_dd=DD_CRYPTO_01973
/// @needwork = ad
/// @endcode
template < typename T_Mac, typename T_CtxSymmetricStream >
class PCtxAeStreamDes_Base : public PCtxAe_Base< T_Mac, T_CtxSymmetricStream >
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01974
    /// @needwork = dda
    /// @endcode
    explicit PCtxAeStreamDes_Base(PCryptoProvider& cryptoProvider) noexcept(noexcept(std::make_unique< T_Mac >(
        cryptoProvider)) && noexcept(std::make_unique< T_CtxSymmetricStream >(cryptoProvider)));
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01975
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Base() noexcept = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01976
    /// @needwork = dda
    /// @endcode
    ~PCtxAeStreamDes_Base() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01977
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Base(PCtxAeStreamDes_Base&& other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01978
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Base(PCtxAeStreamDes_Base const& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01979
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Base& operator=(PCtxAeStreamDes_Base&& other) noexcept = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01980
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Base& operator=(PCtxAeStreamDes_Base const& other) noexcept = delete;
};
/// @brief Authenticated encryption based on des_cbc stream encryption.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00528
/// @trace_id_dd=DD_CRYPTO_01981
/// @unit_name=PCtxAeStreamDes_Cbc
/// @unit_description=Authenticated encryption based on descbc and des-cfb
/// @needwork = ad
/// @endcode
class PCtxAeStreamDes_Cbc : public PCtxAeStreamDes_Base< PCtxMacDesCbc, PCtxSymmetricStream_Des_Cfb >
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06305
    /// @needwork = dda
    /// @endcode
    using PCtxAeStreamDes_Base< PCtxMacDesCbc, PCtxSymmetricStream_Des_Cfb >::PCtxAeStreamDes_Base;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01982
    /// @needwork = dda
    /// @endcode
    ~PCtxAeStreamDes_Cbc() override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01983
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Cbc(PCtxAeStreamDes_Cbc&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01984
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Cbc(PCtxAeStreamDes_Cbc const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01985
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Cbc& operator=(PCtxAeStreamDes_Cbc&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01986
    /// @needwork = dda
    /// @endcode
    PCtxAeStreamDes_Cbc& operator=(PCtxAeStreamDes_Cbc const& other) = delete;

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
    /// @trace_id_ad=AD_CRYPTO_00529
    /// @trace_id_dd=DD_CRYPTO_01987
    /// @needwork = ad
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Get the corresponding algorithm ID.
    /// @return Returns the corresponding algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01988
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_AE_STREAM_DES_H_