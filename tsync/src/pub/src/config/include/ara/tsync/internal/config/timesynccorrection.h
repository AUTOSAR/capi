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
/// @file       timesynccorrection.h
/// @brief      configuration module time base set class
/// @details
/// @date       2023-01-11
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_TIMESYNCCORRECTION_H_
#define ARA_TSYNC_INTERNAL_CONFIG_TIMESYNCCORRECTION_H_

#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

#include "ara/tsync/internal/config/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time correction class definition
class TimeSyncCorrection final
{
public:
    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kAllowProviderRateCorrection
        static constexpr ara::core::StringView kAllowProviderRateCorrection{"allowProviderRateCorrection"};
        /// @name kOffsetCorrectionAdaptionInterval
        static constexpr ara::core::StringView kOffsetCorrectionAdaptionInterval{"offsetCorrectionAdaptionInterval"};
        /// @name kOffsetCorrectionJumpThreshold
        static constexpr ara::core::StringView kOffsetCorrectionJumpThreshold{"offsetCorrectionJumpThreshold"};
        /// @name kRateCorrectionPerMeasurementDuration
        static constexpr ara::core::StringView kRateCorrectionPerMeasurementDuration{
            "rateCorrectionsPerMeasurementDuration"};
        /// @name kRateDeviationMeasurementDuration
        static constexpr ara::core::StringView kRateDeviationMeasurementDuration{"rateDeviationMeasurementDuration"};
        initConfig_                                 = true;
        this->allowProviderRateCorrection_          = node.GetValue(kAllowProviderRateCorrection, false);
        this->offsetCorrectionJumpThreshold_        = node.GetValue(kOffsetCorrectionJumpThreshold, 0.0);
        this->offsetCorrectionAdaptionInterval_     = node.GetValue(kOffsetCorrectionAdaptionInterval, 0.0);
        this->rateCorrectionPerMeasurementDuration_ = node.GetValue(kRateCorrectionPerMeasurementDuration, 0U);
        this->rateDeviationMeasurementDuration_     = node.GetValue(kRateDeviationMeasurementDuration, 0.0);
        return isoft::kSuccess;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "allowProviderRateCorrection: " << allowProviderRateCorrection_ << std::endl;
        std::cout << "offsetCorrectionJumpThreshold: " << offsetCorrectionJumpThreshold_ << std::endl;
        std::cout << "offsetCorrectionAdaptionInterval: " << offsetCorrectionAdaptionInterval_ << std::endl;
        std::cout << "rateCorrectionPerMeasurementDuration: " << rateCorrectionPerMeasurementDuration_ << std::endl;
        std::cout << "rateDeviationMeasurementDuration: " << rateDeviationMeasurementDuration_ << std::endl;
#endif
    };

    /// @brief get whether rate correction is allowed
    /// @return whether rate correction is allowed
    bool GetAllowProviderRateCorrection() const noexcept { return allowProviderRateCorrection_; }

    /// @brief get the rate adaptive elimination interval in offset correction
    /// @return the rate adaptive elimination interval in offset correction
    double GetOffsetCorrectionAdaptionInterval() const noexcept { return offsetCorrectionAdaptionInterval_; }

    /// @brief get the jump correction threshold in offset correction.
    /// @return the jump correction threshold in offset correction.
    double GetOffsetCorrectionJumpThreshold() const noexcept { return offsetCorrectionJumpThreshold_; }

    /// @brief get the maximum concurrency number for rate deviation measurement.
    /// @return the maximum concurrency number for rate deviation measurement
    std::uint32_t GetRateCorrectionPerMeasurementDuration() const noexcept
    {
        return rateCorrectionPerMeasurementDuration_;
    }

    /// @brief get the rate deviation measurement period.
    /// @return the rate deviation measurement period
    double GetRateDeviationMeasurementDuration() const noexcept { return rateDeviationMeasurementDuration_; }

    /// @brief get whether rate correction is allowed
    /// @return whether rate correction is allowed
    bool GetInitConfig() const noexcept { return initConfig_; }

private:
    /// @name allowProviderRateCorrection - whether rate correction is allowed
    bool allowProviderRateCorrection_{false};
    /// @name offsetCorrectionAdaptionInterval - rate adaptive elimination interval in offset correction
    double offsetCorrectionAdaptionInterval_{0.0};  // seconds
    /// @name offsetCorrectionJumpThreshold - jump correction threshold in offset correction.
    double offsetCorrectionJumpThreshold_{0.0};  // seconds
    /// @name rateCorrectionPerMeasurementDuration - maximum concurrency number for rate deviation measurement
    uint32_t rateCorrectionPerMeasurementDuration_{0};  // positive integer
    /// @name rateDeviationMeasurementDuration - rate deviation measurement period
    double rateDeviationMeasurementDuration_{0.0};  // seconds

    /// @name initConfig - whether TimeSyncCorrection is configured; if not, use the time domain's
    bool initConfig_{false};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_TIMESYNCCORRECTION_H_
