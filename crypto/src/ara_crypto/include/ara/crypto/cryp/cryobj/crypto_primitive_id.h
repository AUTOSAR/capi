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
/// @file       crypto_primitive_id.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Generic interface for identifying all cryptographic primitives, their keys, and parameters.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Cryptographic Primitive
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=CryptoPrimitiveId
/// @unit_description=Generic interface for identifying all cryptographic primitives, their keys, and parameters
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_PRIMITIVE_ID_H_
#define ARA_CRYPTO_CRYP_CRYPTO_PRIMITIVE_ID_H_

#include <memory>

#include "ara/core/string_view.h"
#include "ara/crypto/common/base_id_types.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Generic interface for identifying all cryptographic primitives, their keys, and parameters.
/// @brief Common interface for identification of all Crypto Primitives and their keys & parameters.
/// @interface CryptoPrimitiveId
/// @AUTOSAR_SWS {SWS_CRYPT_20600}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_05341
/// @needwork = ad
/// @endcode
class CryptoPrimitiveId
{
public:
    /// @brief Type definition for vendor-specific binary cryptographic primitive ID.
    /// @brief Type definition of vendor specific binary Crypto Primitive ID.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20641}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03222
    /// @trace_id_dd=DD_CRYPTO_06434
    /// @needwork = ad
    /// @endcode
    using AlgId = CryptoAlgId;
    /// @brief Unique smart pointer for the constant interface.
    /// @brief type definition pointer
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20644}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03223
    /// @trace_id_dd=DD_CRYPTO_06435
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< CryptoPrimitiveId const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief type definition pointer to const
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20643}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03224
    /// @trace_id_dd=DD_CRYPTO_06436
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< CryptoPrimitiveId >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02544
    /// @trace_id_dd=DD_CRYPTO_05342
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId() noexcept = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10808}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02545
    /// @trace_id_dd=DD_CRYPTO_05343
    /// @needwork = ad
    /// @endcode
    virtual ~CryptoPrimitiveId() noexcept = default;
    /// @brief Get the vendor-specific ID of the primitive.
    /// @brief Get vendor specific ID of the primitive.
    /// @return the binary Crypto Primitive ID
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20652}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02546
    /// @trace_id_dd=DD_CRYPTO_05344
    /// @needwork = ad
    /// @endcode
    virtual AlgId GetPrimitiveId() const noexcept = 0;
    /// @brief Get the unified name of the primitive. The cryptographic primitive name can be fully specified or partially specified (refer to "Cryptographic Primitive Naming Convention" for details).
    ///         The lifetime of the returned StringView instance should not exceed the lifetime of this CryptoPrimitiveId instance!
    /// @brief Get a unified name of the primitive.
    ///         The crypto primitive name can be fully or partially specified (see "Crypto Primitives Naming Convention"
    ///         for more details). The life-time of the returned @c StringView instance should not exceed the life-time
    ///         of this @c CryptoPrimitiveId instance!
    /// @return the unified name of the crypto primitive
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20651}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02547
    /// @trace_id_dd=DD_CRYPTO_05345
    /// @needwork = ad
    /// @endcode
    virtual ara::core::StringView const GetPrimitiveName() const noexcept = 0;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoPrimitiveId to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30212}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05346
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId &operator=(CryptoPrimitiveId const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoPrimitiveId to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30213}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05347
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId &operator=(CryptoPrimitiveId &&other) = delete;

public:
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05348
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId(CryptoPrimitiveId &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05349
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId(CryptoPrimitiveId const &other) noexcept = delete;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_CRYPTO_PRIMITIVE_ID_H_
