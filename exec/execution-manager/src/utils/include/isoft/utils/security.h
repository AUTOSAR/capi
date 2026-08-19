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
/// @file       security.h
/// @brief      Security related algorithm set
/// @details
/// @date       2023-11-15
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTILS_SECURITY_H_
#define ISOFT_UTILS_SECURITY_H_
#include <openssl/evp.h>

#include <cstdint>
#include <string>

namespace isoft {
namespace utils {
namespace security {

// PRQA S 2024,2025 ++

/// @brief Redefine character type
using Char8_t = char;

/// @brief md5 algorithm name
/// @return md5 algorithm name
inline constexpr Char8_t const *GetMd5DgstAlgo() noexcept { return "md5"; }

/// @brief sha1 algorithm name
/// @return sha1 algorithm name
inline constexpr Char8_t const *GetSha1DgstAlgo() noexcept { return "sha1"; }

/// @brief sha256 algorithm name
/// @return sha256 algorithm name
inline constexpr Char8_t const *GetSha256DgstAlgo() noexcept { return "sha256"; }

/// @brief Read public key from string and convert to EVP_PKEY
/// @param pubKeyString Public key string
/// @return nullptr failure; !=nullptr success
EVP_PKEY *CreateEvpPubKey(std::string const &pubKeyString) noexcept;

/// @brief Release public key resource
/// @param pEvpPubKey Public key resource to release
void DeleteEvpPubKey(EVP_PKEY *const pEvpPubKey) noexcept;

/// @brief Get MD5 value of a piece of data
/// @param data Data
/// @return MD5 value of data
std::string GetMd5OfData(std::string const &data) noexcept;

/// @brief Verify signature using public key
/// @param pPubKey Public key EVP_PKEY pointer
/// @param digestAlgo Digest algorithm (currently only supports MD5, SHA1 and SHA256)
/// @param unsignedFile Unsigned file
/// @param signedFile Signed file
/// @return 0 signature verification success; <0 signature verification failure
int32_t VerifySignature(EVP_PKEY *const pPubKey,
                        std::string const &digestAlgo,
                        std::string const &unsignedFile,
                        std::string const &signedFile) noexcept;

/// @brief Get file digest using specified digest algorithm
/// @param fileName File name
/// @param digestAlgo Digest algorithm
/// @return Digest data
std::string GetDigestOfFile(std::string const &fileName, std::string const &digestAlgo) noexcept;

// PRQA S 2024,2025 --

}  // namespace security
}  // namespace utils
}  // namespace isoft

#endif  ///< ISOFT_UTILS_SECURITY_H_
