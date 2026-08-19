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
/// @file       entry_point.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Level 1 factory class interface for the crypto module
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06002
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_ENTRY_POINT_H_
#define ARA_CRYPTO_ENTRY_POINT_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief 128-bit secure counter consisting of most significant and least significant quad words.
/// @brief 128 bit secure counter made up of most significant and least significant quad-word of the hardware counter.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30001}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02401}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02139
/// @trace_id_dd=DD_CRYPTO_04909
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
struct SecureCounter
{
public:
    /// @brief Least significant 64 bits
    /// @brief least significant 64 bits
    /// @trace_id_sws={SWS_CRYPT_30002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    std::uint64_t mLSQW{0U};  // NOLINT
    /// @brief Most significant 64 bits
    /// @brief most significant 64 bits
    /// @trace_id_sws={SWS_CRYPT_30003}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    std::uint64_t mMSQW{0U};  // NOLINT
};
/// @brief Global counter
extern std::unique_ptr< SecureCounter > g_SecureCounterPtr;
//********************************/
/// @brief Create or return an existing singleton instance of a specific crypto provider. If (providerUid == nullptr), the platform default provider should be loaded.
/// @brief Factory that creates or return existing single instance of specific Crypto Provider.
///			If (providerUid == nullptr) then platform default provider should be loaded.
/// @param iSpecify  the globally unique identifier of required Crypto Provider
/// @return unique smart pointer to loaded Crypto Provider
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_20099}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02401}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02140
/// @trace_id_dd=DD_CRYPTO_04911
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProvider(ara::core::InstanceSpecifier const& iSpecify) noexcept;
/// @brief Create or return an existing singleton instance of a key storage provider.
/// @brief Factory that creates or return existing single instance of the Key Storage Provider.
/// @returns unique smart pointer to loaded Key Storage Provider
/// @code{.isoft}
/// @error: SecurityErrorDomain::kRuntimeFault  if the Key Storage Provider instance cannot be created
/// @trace_id_sws={SWS_CRYPT_30099}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02109}
/// @uptrace={RS_CRYPTO_02401}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02141
/// @trace_id_dd=DD_CRYPTO_04912
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProvider() noexcept;
/// @brief 2311 standard upgrade, overload this method
/// @brief Create or return an existing singleton instance of a key storage provider.
/// @param iSpecify Target certificate instance specifier
/// @returns  Key provider instance
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProvider(
    ara::core::InstanceSpecifier const& iSpecify) noexcept;
/// @brief Create or return an existing singleton instance of an X.509 provider.
///            The X.509 provider should use the default crypto provider for hashing and signature verification! Therefore, when you load the X.509 provider, it will also load the default crypto provider in the background.
/// @brief Factory that creates or return existing single instance of the X.509 Provider.
///     X.509 Provider should use the default Crypto Provider for hashing and signature verification!
///       Therefore when you load the X.509 Provider, in background it loads the default Crypto Provider too.
/// @returns unique smart pointer to loaded X.509 Provider
/// @code{.isoft}
/// @error: SecurityErrorDomain::kRuntimeFault  if the X.509 Provider cannot be loaded
/// @trace_id_sws={SWS_CRYPT_40099}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02142
/// @trace_id_dd=DD_CRYPTO_04913
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
ara::crypto::x509::X509Provider::Uptr LoadX509Provider() noexcept;
/// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
/// @brief Return an allocated buffer with a generated random sequence of the requested size.
/// @param count number of random bytes to generate
/// @returns @c a buffer filled with the generated random sequence
/// @code{.isoft}
/// @error: SecurityErrorDomain::kBusyResource  if the used RNG is currently out-of-entropy
///                 and therefore cannot provide the requested number of random bytes
/// @trace_id_sws={SWS_CRYPT_30098}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02143
/// @trace_id_dd=DD_CRYPTO_04914
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
ara::core::Result< ara::core::Vector< ara::core::Byte > > GenerateRandomData(std::uint32_t const count) noexcept;
/// @brief Get the current value of the 128-bit secure counter supported by the crypto stack.
/// @return a SecureCounter struct made up of the two unsigned 64 bit values (LSQW and MSQW)
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02144
/// @trace_id_dd=DD_CRYPTO_04915
/// @trace_id_sr=SR_CRYPTO_06002
/// @needwork = ad
/// @endcode
ara::core::Result< SecureCounter >
/// @error: SecurityErrorDomain::kUnsupported		if the Secure Counter is unsupported by the Crypto Stack
/// implementation on this Platform
/// @error: SecurityErrorDomain::kAccessViolation	if current Actor has no permission to call this routine
/// @trace_id_sws={SWS_CRYPT_20098}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02401}
/// @threadsafety={Thread-safe}
/// @brief Get current value of 128 bit Secure Counter supported by the Crypto Stack.
/// @return a SecureCounter struct made up of the two unsigned 64 bit values (LSQW and MSQW)
GetSecureCounter() noexcept;
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_ENTRY_POINT_H_
