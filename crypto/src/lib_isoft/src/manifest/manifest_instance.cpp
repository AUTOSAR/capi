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
/// @file       manifest_instance.cpp
/// @brief      AutoSar-Crypto Configuration
/// @details
/// @date       2021-09-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Interpret Configuration File
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestInstance
/// @unit_description=Singleton class for configuration file interpreter
/// @endcode
///
/// ================================================================

#include "ara/crypto/manifest/manifest_instance.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <memory>

#include "ara/crypto/common/isoft_common_api.h"

namespace ara {
namespace crypto {
namespace manifest {
namespace {
/// @brief Unique object for configuration file parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01571
/// @trace_id_dd=DD_CRYPTO_04037
/// @needwork = dd
/// @endcode
std::unique_ptr< PManifestMapConfig > g_InstancePtr{};  // NOLINT
/// @brief Get configuration file
/// @name  GetConfigFileName
/// @return Configuration file name
ara::core::String GetConfigFileName() noexcept
{
    ara::core::String stConfigFileName;
    isoft::ara_fsh::Process const fsh;
    stConfigFileName = fsh.GetCrypto();
    if (false == stConfigFileName.empty()) {
        return stConfigFileName;
    }

    stConfigFileName = manifest::GetkCryptoManifestFileName();
    return stConfigFileName;
}
}  // namespace
/// @brief Initialize
/// @return Pointer to PManifestMapConfig
PManifestMapConfig *PManifestInstance::Get() noexcept
{
    if (g_InstancePtr == nullptr) {
        g_InstancePtr = std::make_unique< PManifestMapConfig >();
        ara::core::String const stConfigFile(GetConfigFileName());
        bool const ret{g_InstancePtr->InitManifest(T_StringView(stConfigFile))};
        std::ignore = ret;
    }
    return g_InstancePtr.get();
}
/// @brief Uninitialize
void PManifestInstance::Deinitialize() noexcept
{
    if (g_InstancePtr != nullptr) {
        g_InstancePtr.reset();
    }
}
//********************************/
}  // namespace manifest
}  // namespace crypto
}  // namespace ara