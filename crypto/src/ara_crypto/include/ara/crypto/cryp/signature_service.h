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
/// @file       signature_service.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Extended metadata service for signature contexts.
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
/// @unit_name=SignatureService
/// @unit_description=Extended metadata service for signature contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SIGNATURE_SERVICE_H_
#define ARA_CRYPTO_CRYP_SIGNATURE_SERVICE_H_

#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Extended metadata service for signature contexts.
/// @brief Extension meta-information service for signature contexts.
/// @interface SignatureService
/// @AUTOSAR_SWS {SWS_CRYPT_29000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02418
/// @trace_id_dd=DD_CRYPTO_05202
/// @needwork = ad
/// @endcode
class SignatureService : public ExtensionService
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29001}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03207
    /// @trace_id_dd=DD_CRYPTO_06419
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< SignatureService >;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02419
    /// @trace_id_dd=DD_CRYPTO_05203
    /// @needwork = ad
    /// @endcode
    SignatureService() = default;
    /// @brief Default destructor
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02420
    /// @trace_id_dd=DD_CRYPTO_05204
    /// @needwork = ad
    /// @endcode
    ~SignatureService() override = default;

public:
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05205
    /// @needwork = dda
    /// @endcode
    SignatureService &operator=(SignatureService const &other) = delete;
    /// @brief Default copy move operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05206
    /// @needwork = dda
    /// @endcode
    SignatureService &operator=(SignatureService &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05207
    /// @needwork = dda
    /// @endcode
    SignatureService(SignatureService const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05208
    /// @needwork = dda
    /// @endcode
    SignatureService(SignatureService &&other) noexcept = delete;

public:
    /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
    /// @brief Get an ID of hash algorithm required by current signature algorithm.
    /// @return required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
    /// concrete hash function
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29003}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02421
    /// @trace_id_dd=DD_CRYPTO_05209
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::AlgId GetRequiredHashAlgId() const noexcept = 0;
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @return required hash size in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02422
    /// @trace_id_dd=DD_CRYPTO_05210
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetRequiredHashSize() const noexcept = 0;
    /// @brief Get the size of the signature value produced and required by the current algorithm.
    /// @brief Get size of the signature value produced and required by the current algorithm.
    /// @return size of the signature value in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29004}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02423
    /// @trace_id_dd=DD_CRYPTO_05211
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetSignatureSize() const noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SIGNATURE_SERVICE_H_
