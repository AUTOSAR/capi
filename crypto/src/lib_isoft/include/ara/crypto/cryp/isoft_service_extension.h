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
/// @file       isoft_service_extension.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Base meta-information service class for all contexts.
/// @date       2022-03-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-03-11  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Service module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceExtension
/// @unit_description=Base meta-information service class for all contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SERVICE_EXTENSION_H_
#define ARA_CRYPTO_CRYP_PUHUA_SERVICE_EXTENSION_H_

#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Base meta-information service class for all contexts.
/// @code{.isoft}
/// @tparam T_CtxCrypto
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00113
/// @trace_id_dd=DD_CRYPTO_00720
/// @needwork = ad
/// @endcode
template < typename T_CtxCrypto >
class PServiceExtension : public ExtensionService
{
private:
    /// @brief Crypto context interface used by the extended service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00721
    /// @needwork = dda
    /// @endcode
    T_CtxCrypto const& ctxCrypto_;

public:
    /// @brief the constrctor with parameter
    /// @param ctxCrypto Crypto context template class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00722
    /// @needwork = dda
    /// @endcode
    explicit PServiceExtension(T_CtxCrypto const& ctxCrypto) noexcept : ExtensionService{}, ctxCrypto_{ctxCrypto} {}
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00723
    /// @needwork = dda
    /// @endcode
    ~PServiceExtension() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00724
    /// @needwork = dda
    /// @endcode
    PServiceExtension& operator=(PServiceExtension const& other) noexcept = delete;
    /// @brief default copy move operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00725
    /// @needwork = dda
    /// @endcode
    PServiceExtension& operator=(PServiceExtension&& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00726
    /// @needwork = dda
    /// @endcode
    PServiceExtension(PServiceExtension&& other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00727
    /// @needwork = dda
    /// @endcode
    PServiceExtension(PServiceExtension const& other) noexcept = delete;

public:
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00728
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetActualKeyBitLength() const noexcept final { return ctxCrypto_.GetActualKeyBitLength(); }
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00729
    /// @needwork = dda
    /// @endcode
    inline CryptoObjectUid GetActualKeyCOUID() const noexcept final
    {
        return std::move(ctxCrypto_.GetActualKeyCOUID());
    }
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00730
    /// @needwork = dda
    /// @endcode
    inline AllowedUsageFlags GetAllowedUsage() const noexcept final { return ctxCrypto_.GetAllowedUsage(); }
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00731
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetMaxKeyBitLength() const noexcept final { return ctxCrypto_.GetMaxKeyBitLength(); }
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00732
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetMinKeyBitLength() const noexcept final { return ctxCrypto_.GetMinKeyBitLength(); }
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00733
    /// @needwork = dda
    /// @endcode
    inline bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept final
    {
        return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
    }
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00734
    /// @needwork = dda
    /// @endcode
    inline bool IsKeyAvailable() const noexcept final { return ctxCrypto_.IsKeyAvailable(); }

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SERVICE_EXTENSION_H_
