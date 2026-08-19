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
/// @file       globaltimecorrectionprops.h
/// @brief      time domain time correction configuration management class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMECORRECTIONPROPS_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMECORRECTIONPROPS_H_

#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>

#include "ara/tsync/internal/config/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief GlobalTimeCorrectionProps
class GlobalTimeCorrectionProps final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->offsetCorrectionAdaptionInterval_     = node.GetValue(_getKeyOffsetCorrectionAdaptionInterval(), 0.0);
        this->offsetCorrectionJumpThreshold_        = node.GetValue(_getKeyOffsetCorrectionJumpThreshold(), 0.0);
        this->rateCorrectionMeasurementDuration_    = node.GetValue(_getKeyRateCorrectionMeasurementDuration(), 0.0);
        this->rateCorrectionPerMeasurementDuration_ = node.GetValue(_getKeyRateCorrectionPerMeasurementDuration(), 0U);
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "offsetCorrectionAdaptionInterval: " << offsetCorrectionAdaptionInterval_ << std::endl;
        std::cout << "offsetCorrectionJumpThreshold: " << offsetCorrectionJumpThreshold_ << std::endl;
        std::cout << "rateCorrectionMeasurementDuration: " << rateCorrectionMeasurementDuration_ << std::endl;
        std::cout << "rateCorrectionPerMeasurementDuration: " << rateCorrectionPerMeasurementDuration_ << std::endl;
#endif
    };

    /// @brief get offset correction adaptation interval
    /// @traceid {}
    /// @return offset correction adaptation interval
    double GetOffsetCorrectionAdaptionInterval() const noexcept { return offsetCorrectionAdaptionInterval_; }

    /// @brief get offset correction jump threshold
    /// @traceid {}
    /// @return offset correction jump threshold
    double GetOffsetCorrectionJumpThreshold() const noexcept { return offsetCorrectionJumpThreshold_; }

    /// @brief get rate correction measurement period
    /// @traceid {}
    /// @return rate correction measurement period
    double GetRateCorrectionMeasurementDuration() const noexcept { return rateCorrectionMeasurementDuration_; }

    /// @brief get the number of parallel rate corrections per measurement period
    /// @traceid {}
    /// @return the number of parallel rate corrections per measurement period
    std::uint32_t GetRateCorrectionPerMeasurementDuration() const noexcept
    {
        return rateCorrectionPerMeasurementDuration_;
    }

private:
    inline ara::core::StringView _getKeyOffsetCorrectionAdaptionInterval() const noexcept
    {
        return ara::core::StringView{"offsetCorrectionAdaptionInterval"};
    }
    inline ara::core::StringView _getKeyOffsetCorrectionJumpThreshold() const noexcept
    {
        return ara::core::StringView{"offsetCorrectionJumpThreshold"};
    }
    inline ara::core::StringView _getKeyRateCorrectionMeasurementDuration() const noexcept
    {
        return ara::core::StringView{"rateCorrectionMeasurementDuration"};
    }
    inline ara::core::StringView _getKeyRateCorrectionPerMeasurementDuration() const noexcept
    {
        return ara::core::StringView{"rateCorrectionPerMeasurementDuration"};
    }

private:
    // /// @name kOffsetCorrectionAdaptionInterval
    // static constexpr ara::core::StringView kOffsetCorrectionAdaptionInterval{"offsetCorrectionAdaptionInterval"};
    // /// @name kOffsetCorrectionJumpThreshold
    // static constexpr ara::core::StringView kOffsetCorrectionJumpThreshold{"offsetCorrectionJumpThreshold"};
    // /// @name kRateCorrectionMeasurementDuration
    // static constexpr ara::core::StringView kRateCorrectionMeasurementDuration{"rateCorrectionMeasurementDuration"};
    // /// @name kRateCorrectionPerMeasurementDuration
    // static constexpr ara::core::StringView kRateCorrectionPerMeasurementDuration{
    //     "rateCorrectionPerMeasurementDuration"};
    /// @name offsetCorrectionAdaptionInterval
    double offsetCorrectionAdaptionInterval_{0.0};
    /// @name offsetCorrectionJumpThreshold
    double offsetCorrectionJumpThreshold_{0.0};
    /// @name rateCorrectionMeasurementDuration
    double rateCorrectionMeasurementDuration_{0.0};
    /// @name rateCorrectionPerMeasurementDuration
    std::uint32_t rateCorrectionPerMeasurementDuration_{0U};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMECORRECTIONPROPS_H_
