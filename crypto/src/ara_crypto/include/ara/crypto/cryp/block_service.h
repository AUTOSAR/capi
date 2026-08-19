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
/// @file       block_service.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Extended metadata service for block cipher contexts.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Service Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=BlockService
/// @unit_description=Extended metadata service for block cipher contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_BLOCK_SERVICE_H_
#define ARA_CRYPTO_CRYP_BLOCK_SERVICE_H_

#include "ara/core/optional.h"
#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Extended metadata service for block cipher contexts.
/// @brief Extension meta-information service for block cipher contexts.
/// @interface BlockService
/// @AUTOSAR_SWS {SWS_CRYPT_29030}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02271
/// @trace_id_dd=DD_CRYPTO_05047
/// @needwork = ad
/// @endcode
class BlockService : public ExtensionService
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29031}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02271
    /// @trace_id_dd=DD_CRYPTO_06407
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< BlockService >;

public:
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @brief Get actual bit-length of an IV loaded to the context.
    /// @param ivUid  optional pointer to a buffer for saving an @a COUID of a IV object now loaded to the context.
    /// If the context was initialized by a @c SecretSeed object then the output buffer @c *ivUid must be filled
    ///       by @a COUID of this loaded IV object, in other cases @c *ivUid must be filled by all zeros.
    /// @return actual length of the IV (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29035}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02272
    /// @trace_id_dd=DD_CRYPTO_05048
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept = 0;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @brief Get block (or internal buffer) size of the base algorithm.
    /// @return size of the block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29033}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02273
    /// @trace_id_dd=DD_CRYPTO_05049
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept = 0;
    /// @brief Get the default expected size of the initialization vector (IV) or nonce.
    /// @brief Get default expected size of the Initialization Vector (IV) or nonce.
    /// @return default expected size of IV in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29032}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02274
    /// @trace_id_dd=DD_CRYPTO_05050
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetIvSize() const noexcept = 0;
    /// @brief Verify the validity of a specific initialization vector (IV) length.
    /// @brief Verify validity of specific Initialization Vector (IV) length.
    /// @param ivSize  the length of the IV in bytes
    /// @return @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29034}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02275
    /// @trace_id_dd=DD_CRYPTO_05051
    /// @needwork = ad
    /// @endcode
    virtual bool IsValidIvSize(std::size_t ivSize) const noexcept = 0;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02276
    /// @trace_id_dd=DD_CRYPTO_05052
    /// @needwork = ad
    /// @endcode
    BlockService() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02277
    /// @trace_id_dd=DD_CRYPTO_05053
    /// @needwork = ad
    /// @endcode
    ~BlockService() override = default;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws Code logic exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02278
    /// @trace_id_dd=DD_CRYPTO_05054
    /// @needwork = ad
    /// @endcode
    BlockService &operator=(BlockService const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws Code logic exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02279
    /// @trace_id_dd=DD_CRYPTO_05055
    /// @needwork = ad
    /// @endcode
    BlockService &operator=(BlockService &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02280
    /// @trace_id_dd=DD_CRYPTO_05056
    /// @needwork = ad
    /// @endcode
    BlockService(BlockService &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02281
    /// @trace_id_dd=DD_CRYPTO_05057
    /// @needwork = ad
    /// @endcode
    BlockService(BlockService const &other) noexcept = delete;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_BLOCK_SERVICE_H_
