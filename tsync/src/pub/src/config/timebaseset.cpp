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
/// @file       timebaseset.cpp
/// @brief      configuration module time base set class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/config/timebaseset.h"

#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief load configuration
/// @param manifestPath - configuration file path
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseSet::Load(ara::core::String const &manifestPath) noexcept
{
    /// @name kTimeBase
    static constexpr ara::core::StringView kTimeBase{"timeBase"};
    ara::core::StringView const svManifestPath{manifestPath.c_str(), manifestPath.length()};
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(svManifestPath)};
    if (!manifestRes.HasValue()) {
        LOG().Error() << "isoft::manifestreader::OpenManifest(" << manifestPath << "): " << GetErrString();
        return kRET_E1;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    return manifest->Load(kTimeBase, timeBaseSet_);
}

/// @brief print debug information
void TimeBaseSet::Debug() const noexcept
{
#if ARA_TSYNC_DEBUG
    std::cout << "+++" << __PRETTY_FUNCTION__ << "+++" << std::endl;
    for (TimeBase const &tb : GetAll()) {
        tb.Debug();
    }
    std::cout << "---" << __PRETTY_FUNCTION__ << "---" << std::endl;
#endif
}

/// @brief get time base pointer based on time base name
/// @param name - time base name
/// @return time base configuration pointer
TimeBase const *TimeBaseSet::GetTimeBase(ara::core::String const &name) const noexcept
{
    TimeBase const *tbc{nullptr};
    for (ara::core::Vector< TimeBase >::const_iterator itm{timeBaseSet_.cbegin()}; itm != timeBaseSet_.cend(); ++itm) {
        if (name == itm->GetName()) {
            tbc = &(*itm);
            break;
        }
    }
    return tbc;
}

/// @brief get the names of time domains bound to all time base providers
/// @param domainNameList - list of time domain names
void TimeBaseSet::GetTimeDomainNameListWithProvider(
    ara::core::Vector< ara::core::String > &domainNameList) const noexcept
{
    for (ara::core::Vector< TimeBase >::const_iterator itm{timeBaseSet_.cbegin()}; itm != timeBaseSet_.cend(); ++itm) {
        if (true != itm->GetNetworkTimeProvider().empty()) {
            ara::core::String const name{itm->GetTimeDomainName().data(), itm->GetTimeDomainName().length()};
            if (true != name.empty()) {
                domainNameList.push_back(name);
            }
        }
    }
}

/// @brief set time domain configuration for time base
/// @param pTimeDomainSet - time domain set
/// @return time base configuration pointer
void TimeBaseSet::SetTimeDomainId(std::shared_ptr< TimeDomainSet > const &pTimeDomainSet) noexcept
{
    /// Set time domain ID for time base based on time domain configuration
    for (ara::core::Vector< TimeBase >::iterator itm{timeBaseSet_.begin()}; itm != timeBaseSet_.end(); ++itm) {
        ara::core::String const domainName(itm->GetTimeDomainName().data(), itm->GetTimeDomainName().length());
        TimeDomainId const domainId{pTimeDomainSet->GetIdbyName(domainName)};
        itm->SetTimeDomainId(domainId);
    }
}

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara