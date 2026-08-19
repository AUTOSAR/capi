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
/// @file       initialize.cpp
/// @brief      AutoSar-AP Data Persistent Storage Module
/// @details
/// @date       2022-09-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @endcode
///
/// ================================================================

#include "ara/crypto/internal/initialize.h"

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/manifest/manifest_instance.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "isoft/ara_fsh/platform.h"

namespace {
/// @brief Flag indicating whether the module is initialized
bool g_IsInit{false};
#if !PUHUA_OPENSSL_IS_11X
struct ProviderDeleter
{
    void operator()(OSSL_PROVIDER *prov) const
    {
        if (prov != nullptr) {
            OSSL_PROVIDER_unload(prov);
        }
    }
};
using ProviderPtr = std::unique_ptr< OSSL_PROVIDER, ProviderDeleter >;
#endif

}  // namespace

namespace ara {
namespace crypto {
namespace internal {
///********************************/
/// @brief Whether initialized
/// @returns  true if has already Initialized false otherwise
bool IsCryptoInitialize() noexcept { return g_IsInit; }
/// @brief Set environment variable
/// @param path Path
/// @return Whether successful
bool Set_openssl_modules_env(const ara::core::String &path)
{
    // Check if the path exists
    if (access(path.c_str(), F_OK) != 0) {
        return false;
    }

    if (setenv("OPENSSL_MODULES", path.c_str(), 0) != 0) {  // NOLINT
        return false;
    }

    return true;
}
/// @brief Initialize the AP-CRYPTO system
/// @returns  has value if Initialize sucess false otherwise
ara::core::Result< void > Initialize() noexcept { return Initialize(false); }
/// @brief Initialize the AP-CRYPTO system
/// @param bNoLog Do not output call log for this function
/// @return has value if Initialize sucess false otherwise
ara::core::Result< void > Initialize(bool const bNoLog) noexcept
{
#if !PUHUA_OPENSSL_IS_11X
    isoft::ara_fsh::Platform const platForm;
    std::string const &platFormLibDir = platForm.GetPlatformLibDir();
    ///Set environment variable
    if (platFormLibDir.empty()) {
        if (getenv("OPENSSL_MODULES") == nullptr) {  // NOLINT
            ara::crypto::isoft_def::LogError()
                << "The environment variable OPENSSL_MODULES or ISOFT_ARA_FSH_SYSROOT is not found";
        }
    } else {
        char const *osslModulesPathLocal{"../../../../usr/lib/ossl-modules"};
        char const *osslModulesPathBoard{"/ossl-modules"};

        ara::core::String valueInfo;
        valueInfo += platFormLibDir.c_str();
        valueInfo += osslModulesPathLocal;
        if (Set_openssl_modules_env(valueInfo) == false) {
            valueInfo.clear();
            valueInfo += platFormLibDir.c_str();
            valueInfo += osslModulesPathBoard;
            if (Set_openssl_modules_env(valueInfo) == false) {
                ara::crypto::isoft_def::LogError() << "Set environment variable OPENSSL_MODULES failed";
            }
        }
    }
    /// Load provider
    static ProviderPtr s_Default_Prov{nullptr};
    static ProviderPtr s_Legacy_Prov{nullptr};
    if (!s_Default_Prov) {
        s_Default_Prov.reset(OSSL_PROVIDER_load(nullptr, "default"));
        if (!s_Default_Prov) {
            ara::crypto::isoft_def::LogError() << "Failed to load 'default' provider";
        }
    }
    if (!s_Legacy_Prov) {
        s_Legacy_Prov.reset(OSSL_PROVIDER_load(nullptr, "legacy"));
        if (!s_Legacy_Prov) {
            ara::crypto::isoft_def::LogError() << "Failed to load 'legacy' provider";
        }
    }
#endif
    g_IsInit = true;
    if (false == bNoLog) {
        ara::core::StringView const stVersion{"2026-01-30"};
        ara::crypto::isoft_def::LogInfo()
            << "---- AP_Crypto.Libs.Initialize Success : Version = <" << stVersion.data() << "> ----";
    }
    ara::crypto::g_SecureCounterPtr = std::make_unique< ara::crypto::SecureCounter >();
    return ara::core::Result< void >::FromValue();
}
/// @brief De-initialize the AP-CRYPTO system
/// @return has value if Deinitialize sucess false otherwise
ara::core::Result< void > Deinitialize() noexcept
{
    // PIpcClient is created when first used, so we need to recycle the possibly used PIpcClient here
    std::ignore = keys::isoft_def::PIpcClient::Deinitialize();
    manifest::PManifestInstance::Deinitialize();
    ara::crypto::g_SecureCounterPtr.reset();
    g_IsInit = false;
    return ara::core::Result< void >::FromValue();
}

//********************************/
}  // namespace internal
}  // namespace crypto
}  // namespace ara
