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
/// @file       isoft_service_digest_mac.cpp
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
/// @unit_name=PDigestServiceMacBase
/// @unit_description=Base class for Extended Meta-Information Service for generating Message Authentication Code context digests
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_digest_mac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/ //Interface: ExtensionService
/// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
/// @returns actual length of a key (now set to the algorithm context) in bits
std::size_t PDigestServiceMacBase::GetActualKeyBitLength() const noexcept { return 0U; }
/// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
/// @brief Get the COUID of the key deployed to the context this extension service is attached to.
/// If no key was set to the context yet then an empty COUID (Nil) is returned.
/// @returns the COUID of the CryptoObject
/// @trace_id_sws={SWS_CRYPT_29047}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
CryptoObjectUid PDigestServiceMacBase::GetActualKeyCOUID() const noexcept
{
    CryptoObjectUid objectUid;
    return objectUid;
}
/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @return a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PDigestServiceMacBase::GetAllowedUsage() const noexcept { return kAllowPrototypedOnly; }
/// @brief Get the maximum supported key length in bits.
/// @returns maximal supported length of the key in bits
std::size_t PDigestServiceMacBase::GetMaxKeyBitLength() const noexcept { return 0U; }
/// @brief Get the minimum supported key length in bits.
/// @returns minimal supported length of the key in bits
std::size_t PDigestServiceMacBase::GetMinKeyBitLength() const noexcept { return 0U; }
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PDigestServiceMacBase::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    std::ignore = keyBitLength;
    return true;
}
/// @brief Check if a key is set for this context.
/// @return true if a key has been set to this context false otherwise
bool PDigestServiceMacBase::IsKeyAvailable() const noexcept { return true; }
//********************************/ //Interface: BlockService
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// @returns size of the block in bytes
std::size_t PDigestServiceMacBase::GetBlockSize() const noexcept { return 0U; }
/// @brief Get the default expected size of the Initialization Vector (IV) or nonce.
/// @returns default expected size of IV in bytes
std::size_t PDigestServiceMacBase::GetIvSize() const noexcept { return 0U; }
/// @brief Verify the validity of a specific Initialization Vector (IV) length.
/// @param ivSize Initialization vector length
/// @returns  @c true if provided IV length is supported by the algorithm and @c false otherwise
bool PDigestServiceMacBase::IsValidIvSize(std::size_t ivSize) const noexcept
{
    std::ignore = ivSize;
    return false;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
