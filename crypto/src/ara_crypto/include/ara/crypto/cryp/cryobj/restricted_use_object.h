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
/// @file       restricted_use_object.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Common interface for all objects that support usage restrictions.
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
/// @unit_name=RestrictedUseObject
/// @unit_description=Common interface for all objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_RESTRICTED_USE_OBJECT_H_
#define ARA_CRYPTO_CRYP_RESTRICTED_USE_OBJECT_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/crypto_object.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Common interface for all objects that support usage restrictions.
/// @brief A common interface for all objects supporting the usage restriction.
/// @interface RestrictedUseObject
/// @AUTOSAR_SWS {SWS_CRYPT_24800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02008}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02500
/// @trace_id_dd=DD_CRYPTO_05288
/// @needwork = ad
/// @endcode
class RestrictedUseObject : public CryptoObject
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02403}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03212
    /// @trace_id_dd=DD_CRYPTO_06424
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< RestrictedUseObject const >;
    /// @brief Alias for the container type for bit-flags of allowed usages of the object.
    /// @brief Alias to the container type for bit-flags of allowed usages of the object.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03213
    /// @trace_id_dd=DD_CRYPTO_06425
    /// @needwork = ad
    /// @endcode
    using Usage = AllowedUsageFlags;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02501
    /// @trace_id_dd=DD_CRYPTO_05289
    /// @needwork = ad
    /// @endcode
    RestrictedUseObject() = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02502
    /// @trace_id_dd=DD_CRYPTO_05290
    /// @needwork = ad
    /// @endcode
    RestrictedUseObject(RestrictedUseObject const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02503
    /// @trace_id_dd=DD_CRYPTO_05291
    /// @needwork = ad
    /// @endcode
    RestrictedUseObject(RestrictedUseObject &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02504
    /// @trace_id_dd=DD_CRYPTO_05292
    /// @needwork = ad
    /// @endcode
    RestrictedUseObject &operator=(RestrictedUseObject const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02505
    /// @trace_id_dd=DD_CRYPTO_05293
    /// @needwork = ad
    /// @endcode
    RestrictedUseObject &operator=(RestrictedUseObject &&other) = delete;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02506
    /// @trace_id_dd=DD_CRYPTO_05294
    /// @needwork = ad
    /// @endcode
    ~RestrictedUseObject() override = default;

public:
    /// @brief Get the allowed usages of this object.
    /// @brief Get allowed usages of this object.
    /// @return a combination of bit-flags that specifies allowed applications of the object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24811}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02507
    /// @trace_id_dd=DD_CRYPTO_05295
    /// @needwork = ad
    /// @endcode
    virtual Usage GetAllowedUsage() const noexcept = 0;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_RESTRICTED_USE_OBJECT_H_
