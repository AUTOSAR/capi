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
/// @file       meta_info.cpp
/// @brief      This file provides the implementation of MetaInfo.
/// @details
/// @date       2022-07-25
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/meta_info.h"

#include <cstdlib>

#include "utility.h"

namespace ara {
namespace diag {

/// @brief Create MetaInfo object
/// @param metaInfo
/// @return
/// @throws on overflow
MetaInfo api::CreateMetaInfo(ara::core::Map< ara::core::String, ara::core::String > const &mapping)
{
    return MetaInfo{mapping};
}

/// @brief Get the metainfo value for a given key
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] key identification of value to be returned
/// @return Returns value for the given key.
///
///
/// @traceid{SWS_DM_00978}@tracestatus{draft}
ara::core::Optional< ara::core::StringView > MetaInfo::GetValue(ara::core::StringView key) const noexcept
{
    decltype(data_.find(ara::core::String{key})) const it{data_.find(ara::core::String{key})};
    if (it != data_.end()) {
        return {it->second};
    }
    return {};
}

/// @brief Get the context of the invocation
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return Returns the context.
///
///
/// @traceid{SWS_DM_00979}@tracestatus{draft}
MetaInfo::Context MetaInfo::GetContext() const noexcept
{
    decltype(data_.find(ara::core::String{"kContext"})) const it{data_.find(ara::core::String{"kContext"})};
    if (it != data_.end()) {
        constexpr int base10 = 10;  /// NOLINT
        return static_cast< MetaInfo::Context >(std::strtol(it->second.c_str(), nullptr, base10));
    }
    internal::LogWarn() << "MetaInfo::GetContext|kContext not found, defaults to "
                           "kDiagnosticCommunication";
    return MetaInfo::Context::kDiagnosticCommunication;
}

}  // namespace diag
}  // namespace ara
