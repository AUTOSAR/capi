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
/// @file       tcpoint.h
/// @brief      parallel calculation time structure.
/// @details
/// @date       2022-05-25
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_TIME_TCPOINT_H_
#define _ARA_TSYNC_TIME_TCPOINT_H_

#include <chrono>
#include <cmath>

#include "ara/tsync/clock.h"
namespace ara {
namespace tsync {
namespace internal {
namespace timebase {

/// @brief  TCPoint parallel measurement, perform parallel measurement only when TimeSyncCorrection.rateCorrectionPerMeasurementDuration & rateDeviationMeasurementDuration are configured
struct TCPoint
{
    Clock::time_point
        /// @name beginTime  theoretical start value
        beginTime{};

    Clock::time_point
        /// @name realBeginTime  actual start value
        realBeginTime{};

    Clock::time_point
        /// @name tVStart  VirtualLocalTime measurement start time snapshot
        tVStart{};

    bool
        /// @name isConf  whether configured
        isConf{false};
};

}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // _ARA_TSYNC_TIME_TCPOINT_H_
