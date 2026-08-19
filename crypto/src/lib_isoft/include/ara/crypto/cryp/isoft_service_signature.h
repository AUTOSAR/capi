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
/// @file       isoft_service_signature.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Extended meta-information service for signature contexts.
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-03-23  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Service module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceSignature
/// @unit_description=Extended meta-information service for signature contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SERVICE_SIGNATURE_H_
#define ARA_CRYPTO_CRYP_PUHUA_SERVICE_SIGNATURE_H_

#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/signature_service.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Extended meta-information service for signature contexts.
/// @brief Extension meta-information service for signature contexts.
/// @code{.isoft}
/// @interface PServiceSignature
/// @AUTOSAR_SWS {SWS_CRYPT_29000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @tparam T_CtxCrypto
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00035
/// @trace_id_dd=DD_CRYPTO_00508
/// @needwork = ad
/// @endcode
template < typename T_CtxCrypto >
class PServiceSignature : public SignatureService
{
private:
    /// @brief Crypto context object used by the signature context information service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00509
    /// @needwork = dda
    /// @endcode
    T_CtxCrypto const &ctxCrypto_;

public:
    /// @brief the constrctor with parameter
    /// @param ctxCrypto Crypto context template class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00510
    /// @needwork = dda
    /// @endcode
    explicit PServiceSignature(T_CtxCrypto const &ctxCrypto) noexcept : SignatureService{}, ctxCrypto_{ctxCrypto} {}
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00511
    /// @needwork = dda
    /// @endcode
    ~PServiceSignature() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00512
    /// @needwork = dda
    /// @endcode
    PServiceSignature &operator=(PServiceSignature const &other) = delete;
    /// @brief default copy move operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00513
    /// @needwork = dda
    /// @endcode
    PServiceSignature &operator=(PServiceSignature &&other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00514
    /// @needwork = dda
    /// @endcode
    PServiceSignature(PServiceSignature const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00515
    /// @needwork = dda
    /// @endcode
    PServiceSignature(PServiceSignature &&other) noexcept = delete;

public:  // ExtensionService interface
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
    /// @trace_id_dd=DD_CRYPTO_00516
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
    /// @trace_id_dd=DD_CRYPTO_00517
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
    /// @trace_id_dd=DD_CRYPTO_00518
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
    /// @trace_id_dd=DD_CRYPTO_00519
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
    /// @trace_id_dd=DD_CRYPTO_00520
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
    /// @trace_id_dd=DD_CRYPTO_00521
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
    /// @trace_id_dd=DD_CRYPTO_00522
    /// @needwork = dda
    /// @endcode
    inline bool IsKeyAvailable() const noexcept final { return ctxCrypto_.IsKeyAvailable(); }

public:  // SignatureService interface
    /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
    /// @brief Get an ID of hash algorithm required by current signature algorithm.
    /// @returns required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
    /// concrete hash function
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29003}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00523
    /// @needwork = dda
    /// @endcode
    inline CryptoPrimitiveId::AlgId GetRequiredHashAlgId() const noexcept final
    {
        return ctxCrypto_.GetRequiredHashAlgId();
    }
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @returns required hash size in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00524
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetRequiredHashSize() const noexcept final { return ctxCrypto_.GetRequiredHashSize(); }
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @brief Get size of the signature value produced and required by the current algorithm.
    /// @returns size of the signature value in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29004}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00525
    /// @needwork = dda
    /// @endcode
    inline std::size_t GetSignatureSize() const noexcept final { return ctxCrypto_.GetSignatureSize(); }

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SERVICE_SIGNATURE_H_
