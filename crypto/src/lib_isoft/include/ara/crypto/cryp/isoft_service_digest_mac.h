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
/// @file       isoft_service_digest_mac.h
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
/// @unit_name=PDigestServiceMacBase
/// @unit_description=Base class for extended meta-information service for generating message authentication code context digests
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_MAC_H_
#define ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_MAC_H_

#include "ara/core/result.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/digest_service.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Extended meta-information service for generating context digests. Puhua version: provides a default value for interfaces that do not need to be implemented.
/// @brief Extension meta-information service for digest producing contexts.
/// @code{.isoft}
/// @interface PDigestServiceMacBase
/// @AUTOSAR_SWS {SWS_CRYPT_29010}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00036
/// @trace_id_dd=DD_CRYPTO_00526
/// @needwork = ad
/// @endcode
class PDigestServiceMacBase : public DigestService
{
public:  // Interface : ExtensionService
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
    /// @trace_id_dd=DD_CRYPTO_00527
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualKeyBitLength() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00528
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid GetActualKeyCOUID() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00529
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00530
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00531
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00532
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00533
    /// @needwork = dda
    /// @endcode
    bool IsKeyAvailable() const noexcept override;

public:  // Interface : BlockService
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
    /// @trace_id_dd=DD_CRYPTO_00534
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept override = 0;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @brief Get block (or internal buffer) size of the base algorithm.
    /// @returns size of the block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29033}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00535
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00536
    /// @needwork = dda
    /// @endcode
    std::size_t GetIvSize() const noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_00537
    /// @needwork = dda
    /// @endcode
    bool IsValidIvSize(std::size_t ivSize) const noexcept override;

public:  // Interface : DigestService  // No need to rewrite specifically here, kept only for code readability
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
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
    ///                by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    ///                context, which was initialized by a key without @c kAllowSignature permission, but actual
    ///                size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
    /// @trace_id_sws={SWS_CRYPT_29013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00538
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Compare(ReadOnlyMemRegion const &expected,
                                      std::size_t offset = 0U) const noexcept override = 0;
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
    /// @trace_id_dd=DD_CRYPTO_00539
    /// @needwork = dda
    /// @endcode
    std::size_t GetDigestSize() const noexcept override = 0;
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
    /// @trace_id_dd=DD_CRYPTO_00540
    /// @needwork = dda
    /// @endcode
    bool IsFinished() const noexcept override = 0;
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
    /// @trace_id_dd=DD_CRYPTO_00541
    /// @needwork = dda
    /// @endcode
    bool IsStarted() const noexcept override = 0;

public:
};
//********************************/
/// @brief Digest service: MAC
/// @code{.isoft}
/// @tparam T_CtxMac
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00037
/// @trace_id_dd=DD_CRYPTO_00542
/// @unit_name=PDigestServiceMac
/// @unit_description=Extended meta-information service for generating message authentication code context digests
/// @needwork = ad
/// @endcode
template < typename T_CtxMac >
class PDigestServiceMac : public PDigestServiceMacBase
{
private:
    /// @brief Encryption context used by MAC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00543
    /// @needwork = dda
    /// @endcode
    T_CtxMac const &ctxMac_;

public:
    /// @brief the constrctor with parameter
    /// @param ctxMac the T_CtxMac instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00544
    /// @needwork = dda
    /// @endcode
    explicit PDigestServiceMac(T_CtxMac const &ctxMac) noexcept : ctxMac_(ctxMac) {}

public:  // Interface : ExtensionService
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
    /// @trace_id_dd=DD_CRYPTO_00545
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualKeyBitLength() const noexcept override { return ctxMac_.GetActualKeyBitLength(); }
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
    /// @trace_id_dd=DD_CRYPTO_00546
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid GetActualKeyCOUID() const noexcept override { return std::move(ctxMac_.GetActualKeyCOUID()); }
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
    /// @trace_id_dd=DD_CRYPTO_00547
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override { return ctxMac_.GetAllowedUsage(); }
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
    /// @trace_id_dd=DD_CRYPTO_00548
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override { return ctxMac_.GetMaxKeyBitLength(); }
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
    /// @trace_id_dd=DD_CRYPTO_00549
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override { return ctxMac_.GetMinKeyBitLength(); }
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
    /// @trace_id_dd=DD_CRYPTO_00550
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override
    {
        return ctxMac_.IsKeyBitLengthSupported(keyBitLength);
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
    /// @trace_id_dd=DD_CRYPTO_00551
    /// @needwork = dda
    /// @endcode
    bool IsKeyAvailable() const noexcept override { return ctxMac_.IsKeyAvailable(); }

public:  // Interface : DigestService
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
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
    ///                by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    ///                context, which was initialized by a key without @c kAllowSignature permission, but actual
    ///                size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
    /// @trace_id_sws={SWS_CRYPT_29013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00552
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Compare(ReadOnlyMemRegion const &expected, std::size_t offset) const noexcept override
    {
        /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
        /// by a call of the @c Finish() method
        if (false == ctxMac_.IsFinished()) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
        }
        /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
        /// context, which was initialized by a key without @c kAllowSignature permission, but actual
        /// size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
        if ((ctxMac_.GetAllowedUsage() & kAllowSignature) != kAllowSignature) {
            if (ctxMac_.GetMacLength() < kInt_8U) {
                return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kBruteForceRisk);
            }
        }
        return ara::core::Result< bool >::FromValue(ctxMac_.Compare(expected, offset));
    }
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
    /// @trace_id_dd=DD_CRYPTO_00553
    /// @needwork = dda
    /// @endcode
    std::size_t GetDigestSize() const noexcept override { return static_cast< std::size_t >(ctxMac_.GetMacLength()); }
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
    /// @trace_id_dd=DD_CRYPTO_00554
    /// @needwork = dda
    /// @endcode
    bool IsFinished() const noexcept override { return ctxMac_.IsFinished(); }
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
    /// @trace_id_dd=DD_CRYPTO_00555
    /// @needwork = dda
    /// @endcode
    bool IsStarted() const noexcept override { return ctxMac_.IsStarted(); }
    /// @brief Get Actual Iv Bit Length
    /// @param ivUid UID of the initialization vector (IV)
    /// @return  Actual Iv Bit Length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00556
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept override
    {
        if (ivUid.has_value()) {
        }
        ara::core::Optional< CryptoObjectUid > const rhs{ctxMac_.GetSecretSeedCryptoObjectUid()};
        ivUid = rhs;
        uint32_t const nIvLen{ctxMac_.GetIvMaxLength()};
        //// GetIvMaxLength returns 0xFFFFFFFF indicating that IV is not supported
        if (nIvLen == kInt_0xFFFFFFFFU) {
            return 0U;
        }
        return static_cast< size_t >(nIvLen);
    }
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
    /// @trace_id_dd=DD_CRYPTO_00557
    /// @needwork = dda
    /// @endcode
    std::size_t GetIvSize() const noexcept override { return static_cast< size_t >(ctxMac_.GetIvMaxLength()); }
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
    /// @trace_id_dd=DD_CRYPTO_00558
    /// @needwork = dda
    /// @endcode
    bool IsValidIvSize(std::size_t ivSize) const noexcept override
    {
        std::size_t const nSize{ctxMac_.GetIvMaxLength()};
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

#endif  // ARA_CRYPTO_CRYP_PUHUA_SERVICE_DIGEST_MAC_H_
