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
/// @file       globaltimegateway.h
/// @brief      global time domain gateway configuration management class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEGATEWAY_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEGATEWAY_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time gateway  temporarily unused
class GlobalTimeGateWay final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kHost
        static constexpr ara::core::StringView kHost{"host"};
        /// @name kMaster
        static constexpr ara::core::StringView kMaster{"master"};
        /// @name kSlave
        static constexpr ara::core::StringView kSlave{"slave"};
        this->host_   = node.GetValue(kHost, ara::core::String());
        this->master_ = node.GetValue(kMaster, ara::core::String());
        this->slave_  = node.GetValue(kSlave, ara::core::String());
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "host: " << host_ << std::endl;
        std::cout << "master: " << master_ << std::endl;
        std::cout << "slave: " << slave_ << std::endl;
#endif
    }

private:
    /// @name host
    ara::core::String host_;
    /// @name master
    ara::core::String master_;
    /// @name slave
    ara::core::String slave_;
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEGATEWAY_H_
