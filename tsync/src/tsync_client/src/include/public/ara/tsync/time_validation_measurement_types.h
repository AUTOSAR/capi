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
/// @file       time_validation_measurement_types.h
/// @brief      time validation measurement type
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

#ifndef ARA_TSYNC_MEASUREMENT_TYPES_H_
#define ARA_TSYNC_MEASUREMENT_TYPES_H_

#include <chrono>
#include <cstdint>

#include "ara/tsync/timestamp.h"

namespace ara {
namespace tsync {

/// @traceid{SWS_TS_00414}@tracestatus{draft}
/// @traceid{RS_TS_00034}
/// @brief Structure with detailed data for validation of the Time Master
struct TimeMasterMeasurementType final
{
    /// @traceid{SWS_TS_14140}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief egress timestamp of Sync frame in Global Time
    ara::tsync::Timestamp preciseOriginTimestamp{};

    /// @traceid{SWS_TS_14141}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief egress timestamp of Sync frame
    std::uint64_t syncEgressTimestamp{0U};

    /// @traceid{SWS_TS_14142}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief sequence Id of sent Ethernet frame
    std::uint16_t sequenceId{0U};
};

/// @traceid{SWS_TS_00415}@tracestatus{draft}
/// @traceid{RS_TS_00034}
/// @brief Structure with detailed data for validation of the Time Slave
struct TimeSlaveMeasurementType final
{
    /// @traceid{SWS_TS_14150}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief preciseOriginTimestamp taken from the received Follow_Up frame
    ara::tsync::Timestamp preciseOriginTimestamp{};

    /// @traceid{SWS_TS_14151}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief SyncLocal Time Tuple (Global Time part)
    ara::tsync::Timestamp referenceGlobalTimestamp{};

    /// @traceid{SWS_TS_14152}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief ingress timestamp of Sync frame converted to Virtual Local Time
    std::uint64_t syncIngressTimestamp{0U};

    /// @traceid{SWS_TS_14153}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief correctionField taken from the received Follow_Up frame
    std::int64_t correctionField{0};

    /// @traceid{SWS_TS_14154}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief SyncLocal Time Tuple (Virtual Local Time part)
    std::uint64_t referenceLocalTimestamp{0U};

    /// @traceid{SWS_TS_14155}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief currently valid pDelay value
    std::uint32_t pDelay{0U};

    /// @traceid{SWS_TS_14156}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief sequence Id of received Sync frame
    std::uint16_t sequenceId{0U};
};

/// @traceid{SWS_TS_00416}@tracestatus{draft}
/// @traceid{RS_TS_00034}
/// @brief Structure with detailed timing data for the pDelay Initiator
struct PdelayInitiatorMeasurementType final
{
    /// @traceid{SWS_TS_14160}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief egress timestamp of Pdelay_Req in Virtual Local Time
    std::uint64_t requestOriginTimestamp{0U};

    /// @traceid{SWS_TS_14161}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief ingress timestamp of Pdelay_Resp in Virtual Local Time
    std::uint64_t responseReceiptTimestamp{0U};

    /// @traceid{SWS_TS_14162}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief ingress timestamp of Pdelay_Req in Global Time taken from the received Pdelay_Resp
    ara::tsync::Timestamp requestReceiptTimestamp;

    /// @traceid{SWS_TS_14163}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief egress timestamp of Pdelay_Resp in Global Time taken from the received Pdelay_Resp_Follow_Up
    ara::tsync::Timestamp responseOriginTimestamp;

    /// @traceid{SWS_TS_14164}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief value of the Virtual Local Time of the reference Global Time Tuple
    std::uint64_t referenceLocalTimestamp{0U};

    /// @traceid{SWS_TS_14165}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief value of the local instance of the Global Time of the reference Global Time Tuple
    ara::tsync::Timestamp referenceGlobalTimestamp;

    /// @traceid{SWS_TS_14166}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief currently valid pDelay value
    std::uint32_t pDelay{0U};

    /// @traceid{SWS_TS_14167}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief sequence Id of sent Pdelay_Req frame
    std::uint16_t sequenceId{0U};
};

/// @traceid{SWS_TS_00417}@tracestatus{draft}
/// @traceid{RS_TS_00034}
/// @brief Structure with detailed timing data for the pDelay Responder
struct PdelayResponderMeasurementType final
{
    /// @traceid{SWS_TS_14170}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief ingress timestamp of Pdelay_Req converted to Virtual Local Time
    std::uint64_t requestReceiptTimestamp{0U};

    /// @traceid{SWS_TS_14171}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief egress timestamp of Pdelay_Resp converted to Virtual Local Time
    std::uint64_t responseOriginTimestamp{0U};

    /// requestReceiptTimestamp and responseOriginTimestamp into Global Time
    /// @traceid{SWS_TS_14172}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief value of the Virtual Local Time of the reference Global Time Tuple used to convert
    std::uint64_t referenceLocalTimestamp{0U};

    /// requestReceiptTimestamp and responseOriginTimestamp into Global Time
    /// @traceid{SWS_TS_14173}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief value of the local instance of the Global Time of the reference Global Time Tuple used to convert
    ara::tsync::Timestamp referenceGlobalTimestamp{};

    /// @traceid{SWS_TS_14174}@tracestatus{draft}
    /// @traceid{RS_TS_00034}
    /// @brief sequence Id of received Pdelay_Req frame
    std::uint16_t sequenceId{0U};
};

}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_MEASUREMENT_TYPES_H_
