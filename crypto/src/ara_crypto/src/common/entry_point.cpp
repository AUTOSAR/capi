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
/// @file       entry_point.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06002
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/entry_point.h"

#include "ara/core/instance_specifier.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_load_provider.h"
#include "ara/crypto/common/keys/ipc_key_provider.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/internal/initialize.h"
#include "ara/crypto/keys/isoft_ipc_key_provider.h"
#include "ara/crypto/x509/isoft_x509_provider.h"

namespace ara {
namespace crypto {
/// @brief Counter object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02139
/// @trace_id_dd=DD_CRYPTO_06653
/// @trace_id_sr=SR_CRYPTO_06002
/// @unit_name=CryptoProvider
/// @needwork = dda
/// @endcode
std::unique_ptr< SecureCounter > g_SecureCounterPtr{nullptr};  // NOLINT
//********************************/
/// @brief Create or return an existing singleton instance of a specific crypto provider. If (providerUid == nullptr), the platform default provider should be loaded.
/// @param iSpecify Target certificate instance specifier
/// @returns  Corresponding type of crypto provider instance
ara::crypto::cryp::CryptoProvider::Uptr LoadCryptoProvider(ara::core::InstanceSpecifier const &iSpecify) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    std::ignore = iSpecify;
    ara::core::StringView const nSpecify{iSpecify.ToString()};

    ara::crypto::cryp::CryptoProvider::Uptr pProviderPuhua{keys::LoadCryptoProviderPuhua(nSpecify)};
    if (nullptr != pProviderPuhua) {
        return pProviderPuhua;
    }

    ara::crypto::cryp::CryptoProvider::Uptr pProviderBst{keys::LoadCryptoProviderHsmBst(nSpecify)};
    if (nullptr != pProviderBst) {
        return pProviderBst;
    }

    ara::crypto::cryp::CryptoProvider::Uptr pProviderNxp{keys::LoadCryptoProviderHsmNxp(nSpecify)};
    if (nullptr != pProviderNxp) {
        return pProviderNxp;
    }

    // Print log: Warning
    return {std::make_unique< cryp::isoft_def::PCryptoProvider >()};
}
/// @brief Create or return an existing singleton instance of a key storage provider.
/// @returns  Key provider instance
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProvider() noexcept
{
    PH_CheckInit_RetValue(nullptr);
    return {std::make_unique< keys::IpcKeyProvider< ara::crypto::keys::isoft_def::PIpcKeyProvider,
                                                    ara::crypto::cryp::isoft_def::PCryptoProvider > >()};
}
/// @brief Create or return an existing singleton instance of a key storage provider.
/// @param iSpecify Target certificate instance specifier
/// @returns  Key provider instance
ara::crypto::keys::KeyStorageProvider::Uptr LoadKeyStorageProvider(
    ara::core::InstanceSpecifier const &iSpecify) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    std::ignore = iSpecify;
    ara::core::StringView const nSpecify{iSpecify.ToString()};
    if (nSpecify.find("puhua", 0U) != ara::core::StringView::npos) {
        return {std::make_unique< keys::IpcKeyProvider< ara::crypto::keys::isoft_def::PIpcKeyProvider,
                                                        ara::crypto::cryp::isoft_def::PCryptoProvider > >()};
    }
    if (nSpecify.find("isoft", 0U) != ara::core::StringView::npos) {
        return {std::make_unique< keys::IpcKeyProvider< ara::crypto::keys::isoft_def::PIpcKeyProvider,
                                                        ara::crypto::cryp::isoft_def::PCryptoProvider > >()};
    }
    if (nSpecify.find("hsmbst", 0U) != ara::core::StringView::npos) {
        return keys::LoadKeyStorageProviderHsmBst();
    }
    if (nSpecify.find("hsm", 0U) != ara::core::StringView::npos) {
        return keys::LoadKeyStorageProviderHsmNxp();
    }
    return {nullptr};
}

void IN1() {}
/// @brief Create or return an existing singleton instance of an X.509 provider.
///			The X.509 provider should use the default crypto provider for hashing and signature verification! Therefore, when you load the X.509 provider, it will also load the default crypto provider in the background.
/// @returns  Certificate provider instance
ara::crypto::x509::X509Provider::Uptr LoadX509Provider() noexcept
{
    PH_CheckInit_RetValue(nullptr);
    return {std::make_unique< x509::isoft_def::PX509Provider >()};
}
/// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
/// @param count Number of random bytes to generate
/// @returns  @c a buffer filled with the generated random sequence
ara::core::Result< ara::core::Vector< ara::core::Byte > > GenerateRandomData(std::uint32_t const count) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< ara::core::Byte > >);
    ara::core::InstanceSpecifier const iSpecify{ara::core::StringView("isoft")};
    ara::crypto::cryp::CryptoProvider::Uptr const pCryptoProvider{LoadCryptoProvider(iSpecify)};
    if (pCryptoProvider.get() == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    ara::core::Result< cryp::RandomGeneratorCtx::Uptr > const resRandomGeneratorCtx{
        pCryptoProvider->CreateRandomGeneratorCtx(
            static_cast< cryp::CryptoProvider::AlgId >(cryp::isoft_def::EPhCtxTypeID::kRandomGeneratorglobal), true)};
    if (!resRandomGeneratorCtx.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    cryp::RandomGeneratorCtx::Uptr const &pRandomGeneratorCtx{resRandomGeneratorCtx.Value()};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resByte{pRandomGeneratorCtx->Generate(count)};
    if (!resByte.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    return resByte;
}
/// @brief Get the current value of the 128-bit secure counter supported by the crypto stack.
/// @returns  Secure counter
ara::core::Result< SecureCounter > GetSecureCounter() noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SecureCounter >);
    if (nullptr == g_SecureCounterPtr.get()) {
        return ara::core::Result< SecureCounter >::FromError(SecurityErrorDomain::Errc::kNotInitialized);
    }
    SecureCounter secureCounter{*g_SecureCounterPtr};
    if ((secureCounter.mLSQW == 0U) && (secureCounter.mMSQW == 0U)) {
        /// Fill the counter default value using the uuid method
        secureCounter.mLSQW = GetUuidByTime().mQwordMs;
        secureCounter.mMSQW = GetUuidByTime().mQwordLs;
    }

    if (secureCounter.mLSQW == UINT64_MAX) {
        secureCounter.mMSQW = secureCounter.mMSQW + 1U;
    } else {
        secureCounter.mLSQW = secureCounter.mLSQW + 1U;
    }
    return ara::core::Result< SecureCounter >::FromValue(secureCounter);
}
}  // namespace crypto
}  // namespace ara
