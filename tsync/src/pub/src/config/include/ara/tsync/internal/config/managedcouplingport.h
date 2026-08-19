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
/// @file       managedcouplingport.h
/// @brief      global time domain managed coupling port management class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_MANAGEDCOUPLINGPORT_H_
#define ARA_TSYNC_INTERNAL_CONFIG_MANAGEDCOUPLINGPORT_H_

#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief ManagedCouplingPort
class ManagedCouplingPort final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kCouplingPort
        static constexpr ara::core::StringView kCouplingPort{"couplingPort"};
        /// @name kPdelayLatencyThreshold
        static constexpr ara::core::StringView kPdelayLatencyThreshold{"pdelayLatencyThreshold"};
        /// @name kPdelayRequestPeriod
        static constexpr ara::core::StringView kPdelayRequestPeriod{"pdelayRequestPeriod"};
        /// @name kPdelayRespAndRespFollowupTimeout
        static constexpr ara::core::StringView kPdelayRespAndRespFollowupTimeout{"pdelayRespAndRespFollowUpTimeout"};
        /// @name kPdelayResponseEnabled
        static constexpr ara::core::StringView kPdelayResponseEnabled{"pdelayResponseEnabled"};
        this->couplingPort_                     = node.GetValue(kCouplingPort, ara::core::String());
        this->pdelayLatencyThreshold_           = node.GetValue(kPdelayLatencyThreshold, 0.0);
        this->pdelayRequestPeriod_              = node.GetValue(kPdelayRequestPeriod, 0.0);
        this->pdelayRespAndRespFollowupTimeout_ = node.GetValue(kPdelayRespAndRespFollowupTimeout, 0.0);
        this->pdelayResponseEnabled_            = node.GetValue(kPdelayResponseEnabled, false);
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "couplingPort: " << couplingPort_ << std::endl;
        std::cout << "pdelayLatencyThreshold: " << pdelayLatencyThreshold_ << std::endl;
        std::cout << "pdelayRequestPeriod: " << pdelayRequestPeriod_ << std::endl;
        std::cout << "pdelayRespAndRespFollowupTimeout: " << pdelayRespAndRespFollowupTimeout_ << std::endl;
        std::cout << "pdelayResponseEnabled: " << pdelayResponseEnabled_ << std::endl;
#endif
    };

    /// @brief get pdelay latency threshold
    /// @traceid {}
    /// @return pdelay latency threshold
    double GetPdelayLatencyThreshold() const noexcept { return pdelayLatencyThreshold_; }

    /// @brief get pdelay request sending period
    /// @traceid {}
    /// @return pdelay request sending period
    double GetPdelayRequestPeriod() const noexcept { return pdelayRequestPeriod_; }

    /// @brief get PdelayRespAndRespFollowup timeout configuration
    /// @traceid {}
    /// @return PdelayRespAndRespFollowup timeout configuration
    double GetPdelayRespAndRespFollowupTimeout() const noexcept { return pdelayRespAndRespFollowupTimeout_; }

    /// @brief get whether to support sending pdelay response
    /// @traceid {}
    /// @return whether to support sending pdelay response
    bool GetPdelayResponseEnabled() const noexcept { return pdelayResponseEnabled_; }

private:
    /// @name couplingPort  temporarily unused
    ara::core::String couplingPort_;
    /// @name pdelayLatencyThreshold
    double pdelayLatencyThreshold_{0.0};
    /// @name pdelayRequestPeriod
    double pdelayRequestPeriod_{0.0};
    /// @name pdelayRespAndRespFollowupTimeout
    double pdelayRespAndRespFollowupTimeout_{0.0};
    /// @name pdelayResponseEnabled
    bool pdelayResponseEnabled_{false};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_MANAGEDCOUPLINGPORT_H_
