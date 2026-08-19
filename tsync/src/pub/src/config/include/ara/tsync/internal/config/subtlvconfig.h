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
/// @file       subtlvconfig.h
/// @brief      global time domain sub-tlv configuration management class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_SUBTLVCONFIG_H_
#define ARA_TSYNC_INTERNAL_CONFIG_SUBTLVCONFIG_H_

#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief SubTlvConfig
class SubTlvConfig final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kOfsSubTlv
        static constexpr ara::core::StringView kOfsSubTlv{"ofsSubTlv"};
        /// @name kStatusSubTlv
        static constexpr ara::core::StringView kStatusSubTlv{"statusSubTlv"};
        /// @name kTimeSubTlv
        static constexpr ara::core::StringView kTimeSubTlv{"timeSubTlv"};
        /// @name kUserDataSubTlv
        static constexpr ara::core::StringView kUserDataSubTlv{"userDataSubTlv"};
        ofsSubTlv_      = node.GetValue(kOfsSubTlv, false);
        statusSubTlv_   = node.GetValue(kStatusSubTlv, false);
        timeSubTlv_     = node.GetValue(kTimeSubTlv, false);
        userDataSubTlv_ = node.GetValue(kUserDataSubTlv, false);
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "ofsSubTlv: " << ofsSubTlv_ << std::endl;
        std::cout << "statusSubTlv: " << statusSubTlv_ << std::endl;
        std::cout << "timeSubTlv" << timeSubTlv_ << std::endl;
        std::cout << "userDataSubTlv: " << userDataSubTlv_ << std::endl;
#endif
    };

    /// @brief get whether the time domain master supports OfsSubTlv
    /// @traceid {}
    /// @return whether OfsSubTlv is supported
    bool OfsSubTlv() const noexcept { return ofsSubTlv_; }

    /// @brief get whether the time domain master supports statusSubTlv
    /// @traceid {}
    /// @return whether statusSubTlv is supported
    bool StatusSubTlv() const noexcept { return statusSubTlv_; }

    /// @brief get whether the time domain master supports timeSubTlv
    /// @traceid {}
    /// @return whether timeSubTlv is supported
    bool TimeSubTlv() const noexcept { return timeSubTlv_; }

    /// @brief get whether the time domain master supports userDataSubTlv
    /// @traceid {}
    /// @return whether userDataSubTlv is supported
    bool UserDataSubTlv() const noexcept { return userDataSubTlv_; }

private:
    /// @name ofsSubTlv
    bool ofsSubTlv_{false};
    /// @name statusSubTlv
    bool statusSubTlv_{false};
    /// @name timeSubTlv
    bool timeSubTlv_{false};
    /// @name userDataSubTlv
    bool userDataSubTlv_{false};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_SUBTLVCONFIG_H_
