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
/// @file       time_precision_measurement_type.h
/// @brief      time precision measurement type
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

#ifndef _ARA_TSYNC_TIME_PRECISION_MEASUREMENT_TYPE_H_
#define _ARA_TSYNC_TIME_PRECISION_MEASUREMENT_TYPE_H_

#include <cstdint>

namespace ara {
namespace tsync {

/// @brief    Detailed data for precision measurement of TimeSlave.
///
/// @traceid  {SWS_TS_01400, 20-11}
struct TimePrecisionMeasurement final
{
    /// @traceid  {SWS_TS_01401, 20-11}
    /// @brief    Seconds of local time base directly after synchronization with the global time base.
    ///           Range: 0 - 4294967295
    std::uint32_t glbSeconds{0U};

    /// @traceid  {SWS_TS_01402, 20-11}
    /// @brief    Nanoseconds of local time base directly after synchronization with the global time base.
    ///           Range: 0 - 999999999
    std::uint32_t glbNanoSeconds{0U};

    /// @traceid  {SWS_TS_01403, 20-11}
    /// @brief    Local time base status that appears directly after synchronization with the global time base.
    ///           This variable indicates whether and how the local time base is synchronized to the global master time base. This variable is a bit field of a single state, and not every combination is possible.
    ///           For example, only when the GLOBAL_TIME_BASE bit is set, TIMEOUT/TIMELEAP_FUTURE/TIMELEAP_PAST/SYNC_TO_GATEWAY can be set.
    ///           In the following bitmap, 0 is the least significant bit (LSB).
    ///           The meaning of the value corresponds to the whole byte, for example TIMELEAP_PAST may be 0x00/0x20.
    ///   0x20 binary expansion is 00100000B, the valid bit 1 corresponds to Bit5.
    /// ============================================================================================================
    ///   Field name          Byte value   BIT bit   Value    Meaning
    /// ------------------------------------------------------------------------------------------------------------
    ///   TIMEOUT             0x01        0       0x00    No timeout when receiving sync message;
    ///                                           0x01    Timeout occurred when receiving sync message.
    ///   SYNC_TO_GATEWAY     0x04        2       0x00    Local time base synchronized with global master time base;
    ///                                           0x04    Update of local time base is based on gateway under master time base.
    ///   GLOBAL_TIME_BASE    0x08        3       0x00    Local time base is based only on local time base's reference clock (never synchronized with global time base).
    ///                                           0x08    Local time base has been synchronized with global time base at least once.
    ///   TIMELEAP_FUTURE     0x10        4       0x00    No jump to the future when receiving time base.
    ///                                           0x10    A jump to the future occurred when receiving time base and exceeded the configured threshold.
    ///   TIMELEAP_PAST       0x20        5       0x00    No jump to the past when receiving time base.
    ///                                           0x20    A jump to the past occurred when receiving time base and exceeded the configured threshold.
    ///   RESERVED                        167     0x00    Bits 1,6,7 reserved, fixed to 0.
    /// =============================================================================================================
    std::uint8_t timeBaseStatus{0U};

    /// @traceid  {SWS_TS_01404, 20-11}
    /// @brief    Lower 32 bits of virtual local time after synchronization with the global time base.
    ///           Range: 0 - 4294967295
    std::uint32_t virtualLocalTimeLow{0U};

    /// @traceid  {SWS_TS_01405, 20-11}
    /// @brief    Rate deviation value calculated after rate deviation measurement.
    ///           Range: 0 - +-32000
    std::int16_t rateDeviation{0};

    /// @traceid  {SWS_TS_01406, 20-11}
    /// @brief    Seconds of local time base before synchronization with the global time base.
    ///           Range: 0 - 4294967295
    std::uint32_t locSeconds{0U};

    /// @traceid  {SWS_TS_01407, 20-11}
    /// @brief    Nanoseconds of local time base before synchronization with the global time base.
    ///           Range: 0 - 999999999
    std::uint32_t locNanoSeconds{0U};

    /// @traceid  {SWS_TS_01408, 20-11}
    /// @brief    Current propagation delay, in nanoseconds.
    ///           Range: 0 - 4294967295
    std::uint32_t pathDelay{0U};
};

}  // namespace tsync
}  // namespace ara

#endif  /// _ARA_TSYNC_TIME_PRECISION_MEASUREMENT_TYPE_H_
