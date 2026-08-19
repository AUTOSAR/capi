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
/// @file       isoft_service_digest_hash_crc.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PDigestServiceHashCrc
/// @unit_description=Extended Meta-Information Service for Generating Hash-Crc Context Digests
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_digest_hash_crc.h"

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_crc.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/ //Interface: DigestService
/// @brief Parameterized constructor
/// @param ctxHashFunction Hash algorithm crypto context
PDigestServiceHashCrc::PDigestServiceHashCrc(PCtxHashFunctionCrc const& ctxHashFunction) noexcept
    : PDigestServiceHashBase{}  // NOLINT
    , ctxHashFunction_{ctxHashFunction}
{
}
/// @brief Compare the calculated digest with the expected value.
///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be verified or extracted again.
///         if (full_digest_size <= offset) || (expected.size() == 0) then return false; Else comparison_size =
///         min(expected.size(), (full_digest_size - offset)) bytes.
///         This method can be implemented "inline" after the ara::core::memcmp() function is standardized.
/// @param expected Expected value
/// @param offset Offset
/// @returns  @c true if the expected bytes sequence is identical to first bytes of calculated digest
ara::core::Result< bool > PDigestServiceHashCrc::Compare(ReadOnlyMemRegion const& expected,
                                                         std::size_t offset = 0U) const noexcept
{
    /// @returns @c true if the expected bytes sequence is identical to first bytes of calculated digest
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished
    ///                by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kBruteForceRisk  if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    ///                context, which was initialized by a key without @c kAllowSignature permission, but actual
    ///                size of requested digest is less than 8 bytes (it is a protection from the brute-force attack)
    if (false == ctxHashFunction_.IsFinished()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    return ara::core::Result< bool >::FromValue(ctxHashFunction_.Compare(expected, offset));
}
/// @brief Get output digest size.
/// @returns  size of the full output from this digest-function in bytes
std::size_t PDigestServiceHashCrc::GetDigestSize() const noexcept
{
    return static_cast< std::size_t >(ctxHashFunction_.GetHashLength());
}
/// @brief Check current state of stream processing: whether finished.
/// @returns  @c true if a previously started stream processing was finished by a call of the @c Finish() or @c
bool PDigestServiceHashCrc::IsFinished() const noexcept { return ctxHashFunction_.IsFinished(); }
/// @brief View current state of stream processing: whether started.
/// @returns  @c true if the processing was start by a call of the @c Start() methods and was not finished yet
bool PDigestServiceHashCrc::IsStarted() const noexcept { return ctxHashFunction_.IsStarted(); }
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
