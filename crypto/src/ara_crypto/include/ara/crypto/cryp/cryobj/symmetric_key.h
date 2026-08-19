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
/// @file       symmetric_key.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Symmetric key interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Symmetric Key
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SymmetricKey
/// @unit_description=Symmetric Key Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SYMMETRIC_KEY_H_
#define ARA_CRYPTO_CRYP_SYMMETRIC_KEY_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/restricted_use_object.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Symmetric key interface.
/// @brief Symmetric Key interface.
/// @interface SymmetricKey
/// @AUTOSAR_SWS {SWS_CRYPT_23800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02001}
/// @uptrace={RS_CRYPTO_02403}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02532
/// @trace_id_dd=DD_CRYPTO_05327
/// @needwork = ad
/// @endcode
class SymmetricKey : public RestrictedUseObject
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03220
    /// @trace_id_dd=DD_CRYPTO_06432
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< SymmetricKey const >;

private:
    /// @brief Constant object type
    /// @brief const object type
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05328
    /// @needwork = dda
    /// @endcode
    CryptoObjectType const kObjectType{CryptoObjectType::kSymmetricKey};

public:
    /// @brief Get the type of cryptographic material object.
    /// @return Key object type
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02533
    /// @trace_id_dd=DD_CRYPTO_05329
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetkObjectType() const noexcept { return kObjectType; }

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SYMMETRIC_KEY_H_
