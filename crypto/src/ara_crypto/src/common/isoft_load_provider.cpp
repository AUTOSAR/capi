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
/// @file       isoft_load_provider.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module provider factory
/// @details    Factory for cryptographic primitives (Crypto Provider)
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================

#include "ara/crypto/common/isoft_load_provider.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/keys/key_storage_provider.h"

namespace ara {
namespace crypto {
namespace keys {
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderPuhua(ara::core::StringView const &nSpecify)
{
    if (nSpecify.find("puhua", 0U) != ara::core::StringView::npos) {
        return {std::make_unique< ara::crypto::cryp::isoft_def::PCryptoProvider >()};
    }
    if (nSpecify.find("isoft", 0U) != ara::core::StringView::npos) {
        return {std::make_unique< ara::crypto::cryp::isoft_def::PCryptoProvider >()};
    }
    if (CheckNSpecifyPrefix(nSpecify, "PUHUA_", "puhua_") == true) {
        return {std::make_unique< ara::crypto::cryp::isoft_def::PCryptoProvider >()};
    }

    return {nullptr};
}

#ifndef ARA_WITH_CRYPTO_HSM_NXP
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProviderHsmNxp() { return {nullptr}; }
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderHsmNxp(ara::core::StringView const &nSpecify)
{
    std::ignore = nSpecify;
    return {nullptr};
}
#endif

#ifndef ARA_WITH_CRYPTO_HSM_BST
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProviderHsmBst() { return {nullptr}; }
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProviderHsmBst(ara::core::StringView const &nSpecify)
{
    std::ignore = nSpecify;
    return {nullptr};
}
#endif
}  // namespace keys
}  // namespace crypto
}  // namespace ara