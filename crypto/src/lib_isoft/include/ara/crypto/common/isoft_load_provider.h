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
/// @file       isoft_load_provider.h
/// @brief      AutoSar-Crypto encryption and decryption module provider factory
/// @details    Factory for crypto primitives (crypto provider)
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Crypto Provider
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=CryptoProvider
/// @unit_description=Crypto Provider Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_LOAD_PROVIDER_H_
#define ARA_CRYPTO_CRYP_CRYPTO_LOAD_PROVIDER_H_
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/keys/key_storage_provider.h"

namespace ara {
namespace crypto {
namespace keys {

ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProviderHsmNxp();
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProviderHsmBst();

ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderPuhua(ara::core::StringView const &nSpecify);
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderHsmNxp(ara::core::StringView const &nSpecify);
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderHsmBst(ara::core::StringView const &nSpecify);

/// @brief Check the format of the Provider's FQN
/// @param nSpecify FQN string
/// @param UppercasePrefix Uppercase prefix character to check
/// @param lowercasePrefix Lowercase prefix character to check
/// @return
inline bool CheckNSpecifyPrefix(ara::core::StringView const &nSpecify,
                                ara::core::StringView const &uppercasePrefix,
                                ara::core::StringView const &lowercasePrefix)
{
    if (nSpecify.size() < uppercasePrefix.size()) {
        return false;
    }

    if (nSpecify.find(uppercasePrefix, 0U) == 0) {
        return true;
    }

    if (nSpecify.find(lowercasePrefix, 0U) == 0) {
        return true;
    }

    return false;
}

}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif
