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
/// @file       crypto_context.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Mutable encryption context interface base class
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
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=Crypto Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_CONTEXT_H_
#define ARA_CRYPTO_CRYP_CRYPTO_CONTEXT_H_

#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
/// @brief Crypto Provider
class CryptoProvider;
//********************************/
/// @brief Common interface for mutable crypto contexts, i.e., not bound to a single crypto object.
/// @brief A common interface of a mutable cryptographic context, i.e. that is not binded to a single crypto object.
/// @interface CryptoContext
/// @AUTOSAR_SWS {SWS_CRYPT_20400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02008}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02393
/// @trace_id_dd=DD_CRYPTO_05173
/// @needwork = ad
/// @endcode
class CryptoContext
{
public:
    /// @brief Type definition for vendor-specific binary cryptographic primitive ID.
    /// @brief Type definition of vendor specific binary Crypto Primitive ID.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03205
    /// @trace_id_dd=DD_CRYPTO_06417
    /// @needwork = ad
    /// @endcode
    using AlgId = CryptoAlgId;

public:
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02394
    /// @trace_id_dd=DD_CRYPTO_05174
    /// @needwork = ad
    /// @endcode
    virtual ~CryptoContext() noexcept = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02395
    /// @trace_id_dd=DD_CRYPTO_05175
    /// @needwork = ad
    /// @endcode
    CryptoContext() noexcept = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoContext to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05176
    /// @needwork = dda
    /// @endcode
    CryptoContext &operator=(CryptoContext const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoContext to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05177
    /// @needwork = dda
    /// @endcode
    CryptoContext &operator=(CryptoContext &&other) = delete;

public:
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05178
    /// @needwork = dda
    /// @endcode
    CryptoContext(CryptoContext const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05179
    /// @needwork = dda
    /// @endcode
    CryptoContext(CryptoContext &&other) noexcept = delete;

public:
    /// @brief Return the CryptoPrimitiveId instance containing the instance identifier. This is the COUID.
    /// @brief Return CryptoPrimitivId instance containing instance identification.
    /// @name GetCryptoPrimitiveId
    /// @return CryptoPrimitiveId::Uptr
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02396
    /// @trace_id_dd=DD_CRYPTO_05180
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept = 0;
    /// @brief Check whether the crypto context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
    /// @brief Check if the crypto context is already initialized and ready to use.
    ///           It checks all required values, including: key value, IV/seed, etc.
    /// @return true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02397
    /// @trace_id_dd=DD_CRYPTO_05181
    /// @needwork = ad
    /// @endcode
    virtual bool IsInitialized() const noexcept = 0;
    /// @brief Get the reference to the Crypto Provider for this context.
    /// @brief Get a reference to Crypto Provider of this context.
    /// @return a reference to Crypto Provider instance that provides this context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20654}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02398
    /// @trace_id_dd=DD_CRYPTO_05182
    /// @needwork = ad
    /// @endcode
    virtual CryptoProvider &MyProvider() const noexcept = 0;

public:
};
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_CRYPTO_CONTEXT_H_
