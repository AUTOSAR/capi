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
/// @file       provider_time_base_validation_notification.h
/// @brief      provider-side time base validation notification
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_PROVIDER_TIME_BASE_VALIDATION_NOTIFICATION_H_
#define ARA_TSYNC_PROVIDER_TIME_BASE_VALIDATION_NOTIFICATION_H_

#include "ara/tsync/time_validation_measurement_types.h"

namespace ara {
namespace tsync {

/// @brief Callback interface to notify (Validator) Application about the availability of a new data block recorded for
/// the Time Base.
///
/// gfddfgfdg
/// @traceid{SWS_TS_00419}@tracestatus{draft}
/// @traceid{RS_TS_00034}
/// @traceid{RS_TS_00029}
class ProviderTimeBaseValidationNotification
{
public:
    /// @brief Destructor
    ///
    /// @traceid{SWS_TS_01301}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @traceid{RS_TS_00030}
    virtual ~ProviderTimeBaseValidationNotification()                                         = default;
    ProviderTimeBaseValidationNotification()                                                  = default;
    ProviderTimeBaseValidationNotification(ProviderTimeBaseValidationNotification const& obj) = default;
    ProviderTimeBaseValidationNotification(ProviderTimeBaseValidationNotification&& obj)      = default;
    ProviderTimeBaseValidationNotification& operator=(ProviderTimeBaseValidationNotification const& obj) = default;
    ProviderTimeBaseValidationNotification& operator=(ProviderTimeBaseValidationNotification&& obj) = default;

    /// @brief Provide the recorded data block for the pPelay Responder of the Time Base.
    /// @param measurementData Detailed timing data for the pDelay Responder
    ///
    /// @traceid{SWS_TS_00423}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @traceid{RS_TS_00029}
    virtual void SetPdelayResponderData(PdelayResponderMeasurementType const& measurementData) noexcept = 0;

    /// @brief Provide the recorded data block for the Time Master of the Time Base.
    /// @param measurementData Detailed data for validation of the Time Master
    ///
    /// @traceid{SWS_TS_00421}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @traceid{RS_TS_00029}
    virtual void SetMasterTimingData(TimeMasterMeasurementType const& measurementData) noexcept = 0;
};

}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_PROVIDER_TIME_BASE_VALIDATION_NOTIFICATION_H_
