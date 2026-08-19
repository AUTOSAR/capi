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
/// @brief      AutoSar-AP data persistence storage module
/// @details    AP-Per initialization
/// @date       2021-04-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-28  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/per/internal/initialize.h"

#include <functional>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/manifest/manifest_instance.h"

namespace {
/// @brief Whether Per module is initialized
bool g_IsHasInit{false};
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Whether initialized before
/// @return
bool IsPerInitialize() noexcept { return g_IsHasInit; }
/// @brief Initialize AP-Per system, mainly determining manifest.json configuration file
/// @param stJsonConfig
/// @param pLogger
/// @return
/// @traceid {SWS_PER_00408}
ara::core::Result< void > Initialize(ara::core::StringView const &stJsonConfig,
                                     ara::log::Logger *const pLogger) noexcept
{
    ara::core::StringView stVersion{"2023-12-18"};
    isoftkv::LogInfo() << "---- AP_Per.Initialize Success, Versino = <" << stVersion.data() << "> ----";
    // When called from ara::core::Initialize, stJsonConfig is empty
    if (false == stJsonConfig.empty()) {
        ara::per::manifest::PManifestInstance::Initialize(stJsonConfig);
        isoftkv::LogDebug() << "---- AP_Per.Initialize ConfigFile = " << stJsonConfig.data();
    }
    if (nullptr != pLogger) {
        isoftkv::SetWorkLogger(pLogger);
    }
    // Logger cannot be null, so create default
    if (nullptr == isoftkv::GetWorkLogger()) {
        isoftkv::SetWorkLogger(isoftkv::CreateDefLog());
    }
    g_IsHasInit = true;
    return ara::core::Result< void >::FromValue();
}
/// @brief Deinitialize AP-Per system
/// @return
/// @traceid {SWS_PER_00409}
ara::core::Result< void > Deinitialize() noexcept
{
    ara::per::manifest::PManifestInstance::DeInitialize();
    g_IsHasInit = false;
    return ara::core::Result< void >::FromValue();
}
///****************/
/// @brief Get redundant file name
/// @param[in] stFileName
/// @param[in] nIndex
/// @return
ara::core::String MakeReddFileName(ara::core::StringView const &stFileName, int32_t const nIndex) noexcept
{
    ara::core::String stNewFileName{stFileName.data()};
    if (nIndex >= 1) {
        stNewFileName += ara::per::isoftkv::kFileExt_Redd;
        stNewFileName += std::to_string(nIndex).c_str();
    }
    return stNewFileName;
}
/// @brief Traverse each redundant file
/// @param stFileName
/// @param nReddCount
/// @param pfun
/// @return
int32_t ScanAllReddFile(ara::core::StringView const &stFileName,
                        int32_t const nReddCount,
                        CB_EachReddFile const &pfun) noexcept
{
    int32_t nScanCount{0};
    for (int32_t i{0}; i < nReddCount; i++) {
        ara::core::String stNewFileName{MakeReddFileName(stFileName, i)};
        if (pfun(T_StringView(stNewFileName))) {
            nScanCount += 1;
        }
    }
    return nScanCount;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
