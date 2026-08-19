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
/// @file       configmanager.cpp
/// @brief      configuration module management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/config/configmanager.h"

#include <ara/core/string.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <cstdint>
#include <memory>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief construct configuration manager
/// @return configuration manager pointer; return nullptr on failure.
std::shared_ptr< ConfigManager > ConfigManager::CreateManager() noexcept
{
    std::shared_ptr< ConfigManager > man;
    man = std::make_shared< ConfigManager >();
    if (nullptr == man) {
        return man;
    }

    if (0 != man->_load()) {
        man = nullptr;
        return man;
    }

    if (0 != man->_parse()) {
        man = nullptr;
        return man;
    }
    return man;
}

/// @brief load configuration
/// @return 0 - success
/// @return <0 - failure
std::int32_t ConfigManager::_load() noexcept
{
#if ARA_TSYNC_DEBUG_WITHOUT_EM
    ara::core::String const confFile{isoft::ara_fsh::Platform().GetTimeSyncManifest().c_str()};
#else
    ara::core::String const confFile{isoft::ara_fsh::Platform().GetTimeSyncManifest()};
#endif

    std::int32_t r{0};

    /// Load time base configuration
    timeBaseSet_ = std::make_shared< TimeBaseSet >();
    if (nullptr == timeBaseSet_) {
        LOG().Fatal() << "nullptr == timeBaseSet_";
        return internal::kRET_E1;
    }

    r = timeBaseSet_->Load(confFile);
    if (0 != r) {
        LOG().Fatal() << "timeBaseSet_->Load(" << confFile << "): " << r;
        r = internal::kRET_E2;
        return r;
    }

    timeDomainSet_ = std::make_shared< TimeDomainSet >();
    if (nullptr == timeDomainSet_) {
        LOG().Fatal() << "nullptr == timeDomainSet_";
        return internal::kRET_E3;
    }
    r = timeDomainSet_->Load(confFile);
    if (0 != r) {
        LOG().Fatal() << "timeDomainSet_->Load(" << confFile << "): " << r;
        return internal::kRET_E4;
    }
    return 0;
}

/// @brief parse configuration data, analyze data that needs to be coordinated and shared within the manager
/// @return 0 - success
/// @return <0 - failure
std::int32_t ConfigManager::_parse() noexcept
{
    /// Determine which time domains on the current machine are Master based on time base configuration
    ara::core::Vector< ara::core::String > domainNames;

    timeBaseSet_->GetTimeDomainNameListWithProvider(domainNames);
    for (auto const& itm : domainNames) {
        timeDomainSet_->SetMasterOnThisMachine(itm);
    }

    /// Set time domain IDs for time bases based on time domain configuration
    timeBaseSet_->SetTimeDomainId(timeDomainSet_);
    return 0;
}

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
