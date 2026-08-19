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
/// @file       isoft_generate_key_data.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Factory for cryptographic primitives (Crypto Provider)
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
/// @module_path=/crypto/Default Encryption/Decryption/Crypto Provider
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=CryptoProvider
/// @unit_description=Crypto Provider Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_PH_GENERATE_KEY_DATA_H
#define ARA_CRYPTO_CRYP_PUHUA_PH_GENERATE_KEY_DATA_H

#include "ara/core/vector.h"
#include "ara/crypto/common/serializable.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04355
/// @trace_id_dd=DD_CRYPTO_08790
/// @needwork = ad
/// @endcode
struct MKeyLength
{
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04356
    /// @trace_id_dd=DD_CRYPTO_08791
    /// @needwork = ad
    /// @endcode
    uint32_t nPrivateKeyLen{0};
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04357
    /// @trace_id_dd=DD_CRYPTO_08792
    /// @needwork = ad
    /// @endcode
    uint32_t nPublicKeyLen{0};
};

enum class EccForPlatform
{
    kPuhua = 1,
    kNxp   = 2,
    kBst   = 3
};

enum class KeyLen : uint32_t
{
    kLen128 = 128,
    kLen192 = 192,
    kLen256 = 256,
    kLen384 = 384,
    kLen512 = 512
};
class GenerateKeyData
{
public:
    /// @brief For generating RSA key data
    /// @param tarKeyDataLen Target key length
    /// @param privateKeyData Private key data
    /// @param publicKeyData Public key data
    /// @param formatId Key format: DER/PEM
    /// @param priFilePath Optional parameter, private key file path
    /// @param pubFilePath Optional parameter, public key file path
    /// @param needSaveToFile Optional parameter, whether to save the private key to a file, if needed please provide the file name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_04319
    /// @trace_id_dd=DD_CRYPTO_08670
    /// @needwork = ad
    /// @endcode
    static MKeyLength GenerateRsaKeyData(int32_t tarKeyDataLen,
                                         ara::core::Vector< uint8_t > &privateKeyData,
                                         ara::core::Vector< uint8_t > &publicKeyData,
                                         Serializable::FormatId formatId,
                                         ara::core::StringView priFilePath = nullptr,
                                         ara::core::StringView pubFilePath = nullptr,
                                         bool needSaveToFile               = false) noexcept;
    /// @brief For generating ECC key data
    /// @param tarKeyLen Target key length (bit length)
    /// @param privateKeyData Private key data
    /// @param publicKeyData Public key data
    /// @param formatId Key format: DER/PEM
    /// @param eccForPlatform Specify the platform corresponding to the key
    /// @param priFilePath Optional parameter, private key file path
    /// @param pubFilePath Optional parameter, public key file path
    /// @param needSaveToFile Optional parameter, whether to save the private key to a file, if needed please provide the file name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_04320
    /// @trace_id_dd=DD_CRYPTO_08671
    /// @needwork = ad
    /// @endcode
    static MKeyLength GenerateEccKeyData(uint32_t tarKeyLen,
                                         ara::core::Vector< uint8_t > &privateKeyData,
                                         ara::core::Vector< uint8_t > &publicKeyData,
                                         Serializable::FormatId formatId,
                                         EccForPlatform eccForPlatform,
                                         ara::core::StringView priFilePath = nullptr,
                                         ara::core::StringView pubFilePath = nullptr,
                                         bool needSaveToFile               = false) noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_CRYP_PUHUA_PH_GENERATE_KEY_DATA_H