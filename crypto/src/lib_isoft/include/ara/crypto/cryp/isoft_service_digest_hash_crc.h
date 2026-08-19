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
/// @file       isoft_service_digest_hash_crc.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Extended meta-information service for generating context digests.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2021-12-21  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Service module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PDigestServiceHashCrc
/// @unit_description=Extended meta-information service for generating Hash-Crc context digests
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_HASH_CRC_H_
#define ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_HASH_CRC_H_

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_crc.h"
#include "ara/crypto/cryp/isoft_service_digest_hash.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief CRC hash algorithm interface context
class PCtxHashFunctionCrc;
//********************************/
/// @brief Extended meta-information service for generating context digests.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00114
/// @trace_id_dd=DD_CRYPTO_00735
/// @needwork = ad
/// @endcode
class PDigestServiceHashCrc : public PDigestServiceHashBase
{
private:
    /// @brief CRC hash algorithm interface context object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00736
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionCrc const& ctxHashFunction_;

public:
    /// @brief Parameterized constructor
    /// @param ctxHashFunction Hash algorithm encryption context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00737
    /// @needwork = dda
    /// @endcode
    explicit PDigestServiceHashCrc(PCtxHashFunctionCrc const& ctxHashFunction) noexcept;

public:  // Interface : DigestService
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @brief Get actual bit-length of an IV loaded to the context.
    /// @param ivUid  optional pointer to a buffer for saving an @a COUID of a IV object now loaded to the context.
    /// If the context was initialized by a @c SecretSeed object then the output buffer @c *ivUid must be filled
    ///       by @a COUID of this loaded IV object, in other cases @c *ivUid must be filled by all zeros.
    /// @returns actual length of the IV (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29035}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00738
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid >& ivUid) const noexcept override
    {
        if (ivUid.has_value()) {
        }
        ara::core::Optional< CryptoObjectUid > const rhs{ctxHashFunction_.GetSecretSeedCryptoObjectUid()};
        ivUid = rhs;
        uint32_t const nIvLen{ctxHashFunction_.GetIvMaxLength()};
        //// GetIvMaxLength returns 0xFFFFFFFF indicating that IV is not supported
        if (nIvLen == kInt_0xFFFFFFFFU) {
            return 0U;
        }
        return static_cast< size_t >(nIvLen);
    }
    /// @brief Compare the computed digest with the expected value.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be verified or extracted again.
    ///         if (full_digest_size <= offset) || (expected.size() == 0) then return false; Else comparison_size =
    ///         min(expected.size(), (full_digest_size - offset)) bytes.
    ///         This method can be implemented "inline" after the function ara::core::memcmp() is standardized.
    /// @brief Compare the calculated digest against an expected value.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///       verified again or extracted.
    ///       If <tt>(full_digest_size <= offset) || (expected.size() == 0)</tt> then return @c false;
    ///       else <tt>comparison_size = min(expected.size(), (full_digest_size - offset))</tt> bytes.
    ///       This method can be implemented as "inline" after standartization of function @c ara::core::memcmp().
    /// @param expected  the memory region containing an expected digest value
    /// @param offset  position of the first byte in calculated digest for the comparison starting
    /// @returns @c true if the expected bytes sequence is identical to first bytes of calculated digest
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
    ///                by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    ///                context, which was initialized by a key without @c kAllowSignature permission, but actual
    ///                size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00739
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Compare(ReadOnlyMemRegion const& expected, std::size_t offset) const noexcept override;
    /// @brief Get the output digest size.
    /// @brief Get the output digest size.
    /// @returns size of the full output from this digest-function in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00740
    /// @needwork = dda
    /// @endcode
    std::size_t GetDigestSize() const noexcept override;
    /// @brief Check the current state of stream processing: whether finished.
    /// @brief Check current status of the stream processing: finished or no.
    /// @returns @c true if a previously started stream processing was finished by a call of the @c Finish() or @c
    /// FinishBytes() methods
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29015}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00741
    /// @needwork = dda
    /// @endcode
    bool IsFinished() const noexcept override;
    /// @brief Check the current state of stream processing: whether started.
    /// @brief Check current status of the stream processing: started or no.
    /// @returns @c true if the processing was start by a call of the @c Start() methods and was not finished yet
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29014}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00742
    /// @needwork = dda
    /// @endcode
    bool IsStarted() const noexcept override;
    /// @brief Get the default expected size of the initialization vector (IV) or nonce.
    /// @brief Get default expected size of the Initialization Vector (IV) or nonce.
    /// @returns default expected size of IV in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29032}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00743
    /// @needwork = dda
    /// @endcode
    std::size_t GetIvSize() const noexcept override { return static_cast< size_t >(ctxHashFunction_.GetIvMaxLength()); }
    /// @brief Validate the validity of a specific initialization vector (IV) length.
    /// @brief Verify validity of specific Initialization Vector (IV) length.
    /// @param ivSize  the length of the IV in bytes
    /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29034}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00744
    /// @needwork = dda
    /// @endcode
    bool IsValidIvSize(std::size_t ivSize) const noexcept override
    {
        std::size_t const nSize{ctxHashFunction_.GetIvMaxLength()};
        if (ivSize < nSize) {
            return false;
        }
        return true;
    }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_HASH_CRC_H_
