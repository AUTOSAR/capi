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
/// @file       crypto_service.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Extended metadata service for crypto contexts.
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
/// @unit_name=CryptoService
/// @unit_description=Extended metadata service for crypto contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_SERVICE_H_
#define ARA_CRYPTO_CRYP_CRYPTO_SERVICE_H_

#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Extended metadata service for crypto contexts.
/// @brief Extension meta-information service for cryptographic contexts.
/// @interface CryptoService
/// @AUTOSAR_SWS {SWS_CRYPT_29020}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02261
/// @trace_id_dd=DD_CRYPTO_05037
/// @needwork = ad
/// @endcode
class CryptoService : public ExtensionService
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29024}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03195
    /// @trace_id_dd=DD_CRYPTO_06406
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< CryptoService >;

public:
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte-wise stream cipher, and RNG contexts, it is an informational method used only for optimizing interface usage.
    /// @brief Get block (or internal buffer) size of the base algorithm.
    ///         For digest, byte-wise stream cipher and RNG contexts it is an informative method, intended only for
    ///         optimization of the interface usage.
    /// @return  size of the block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29023}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02262
    /// @trace_id_dd=DD_CRYPTO_05038
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept = 0;
    /// @brief Get the maximum expected size of the input data block. suppressPadding parameter, it will equal the block size.
    /// @brief Get maximum expected size of the input data block. @c suppressPadding argument and it will be equal to
    /// the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @return  maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29021}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02263
    /// @trace_id_dd=DD_CRYPTO_05039
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept = 0;
    /// @brief Get the maximum possible size of the output data block.
    ///         If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// @brief Get maximum possible size of the output data block.
    ///         If (IsEncryption() == true) then a value returned by this method is independent from the @c
    ///         suppressPadding argument and will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @return maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29022}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02264
    /// @trace_id_dd=DD_CRYPTO_05040
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept = 0;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02265
    /// @trace_id_dd=DD_CRYPTO_05041
    /// @needwork = ad
    /// @endcode
    CryptoService() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02266
    /// @trace_id_dd=DD_CRYPTO_05042
    /// @needwork = ad
    /// @endcode
    ~CryptoService() override = default;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws Function execution exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02267
    /// @trace_id_dd=DD_CRYPTO_05043
    /// @needwork = ad
    /// @endcode
    CryptoService &operator=(CryptoService const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @throws Function execution exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02268
    /// @trace_id_dd=DD_CRYPTO_05044
    /// @needwork = ad
    /// @endcode
    CryptoService &operator=(CryptoService &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02269
    /// @trace_id_dd=DD_CRYPTO_05045
    /// @needwork = ad
    /// @endcode
    CryptoService(CryptoService &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02270
    /// @trace_id_dd=DD_CRYPTO_05046
    /// @needwork = ad
    /// @endcode
    CryptoService(CryptoService const &other) noexcept = delete;
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif /* ARA_CRYPTO_CRYP_CRYPTO_SERVICE_H_ */
