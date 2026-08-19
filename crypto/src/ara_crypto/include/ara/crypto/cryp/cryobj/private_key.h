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
/// @file       private_key.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Generic asymmetric private key interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Asymmetric Private Key
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PrivateKey
/// @unit_description=Private Key Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PRIVATE_KEY_H_
#define ARA_CRYPTO_CRYP_PRIVATE_KEY_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/cryobj/restricted_use_object.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Generic asymmetric private key interface.
/// @brief Generalized Asymmetric Private Key interface.
/// @interface PrivateKey
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02540
/// @trace_id_dd=DD_CRYPTO_05337
/// @needwork = ad
/// @endcode
class PrivateKey : public RestrictedUseObject
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03221
    /// @trace_id_dd=DD_CRYPTO_06433
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< PrivateKey const >;

private:
    /// @brief This interface maps to a static specific value of the CryptoObjectType enumeration.
    /// @brief Static mapping of this interface to specific value of @c CryptoObjectType enumeration.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22503}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05338
    /// @needwork = dda
    /// @endcode
    CryptoObjectType const kObjectType{CryptoObjectType::kPrivateKey};

public:
    /// @brief Get the type of cryptographic material.
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02541
    /// @trace_id_dd=DD_CRYPTO_05339
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetkObjectType() const noexcept { return kObjectType; }

public:
    /// @brief Get the public key corresponding to this private key.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22511}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02542
    /// @trace_id_dd=DD_CRYPTO_05340
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< PublicKey::Uptrc >
    /// @brief Get the public key correspondent to this private key.
    /// @returns unique smart pointer to the public key correspondent to this private key
    GetPublicKey() const noexcept = 0;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PRIVATE_KEY_H_
