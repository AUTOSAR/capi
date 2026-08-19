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
/// @file       timedomainset.cpp
/// @brief      configuration module time domain set class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/config/timedomainset.h"

#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief load time domain configuration from file
/// @param manifestPath - manifest file path
/// @return 0 - success
/// @return <0 - error code
std::int32_t TimeDomainSet::Load(ara::core::String const &manifestPath) noexcept
{
    /// @name kName
    static constexpr ara::core::StringView kName{"name"};
    /// @name kGlobalTimeDomain
    static constexpr ara::core::StringView kGlobalTimeDomain{"globalTimeDomain"};
    ara::core::StringView const svManifestPath{manifestPath.c_str(), manifestPath.length()};
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(svManifestPath)};
    if (!manifestRes.HasValue()) {
        LOG().Fatal() << "isoft::manifestreader::OpenManifest(" << manifestPath << "): " << GetErrString();
        return kRET_E1;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    return manifest->Load(kGlobalTimeDomain, timeDomainSet_);
}

/// @brief print debug information
void TimeDomainSet::Debug() const noexcept
{
#if ARA_TSYNC_DEBUG
    std::cout << "+++ TimeDomainSet::Debug() +++" << std::endl;
    for (auto const &timeDomain : timeDomainSet_) {
        timeDomain.Debug();
    }
    std::cout << "--- TimeDomainSet::Debug() ---" << std::endl;
#endif  // ARA_TSYNC_DEBUG
}

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
