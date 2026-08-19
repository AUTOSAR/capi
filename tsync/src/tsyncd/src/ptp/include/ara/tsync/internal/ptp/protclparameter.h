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
/// @file       protclparameter.h
/// @brief      PTP management class configuration
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_PROTCLPARAMETER_H_
#define ARA_TSYNC_INTERNAL_PTP_PROTCLPARAMETER_H_

#include <cstdint>

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/config/common.h"
#include "isoft/define.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief protocol parameters, the protocol parameters during PTP communication. The PTP layer only cares about these parameters, not the TimeDomain configuration.
struct ProtclParameter final
{
public:
    /// @brief verification strategy enumeration class

    /// @upstrace PRS_TS_00002 Use default configuration

    ///////////////////////////// PTP protocol specification ///////////////////////////////////
    /// @name crcSupport - indicates that CRC verification is provided.
    bool crcSupport{false};

    /// @name rxResidenceTime - indicates the default value of residence time.
    /// TODO(zhoubo): magic number should be place a common file
    double rxResidenceTime{kTS_NUM_0_POINT_001};

    /// @name txResidenceTime - indicates the default value of residence time.
    /// TODO(zhoubo): magic number should be place a common file
    double txResidenceTime{kTS_NUM_0_POINT_001};

    /// @name globalTimeTxPdelayReqPeriod - Pdelay_Req message sending period, 0 means Pdelay measurement is disabled.
    /// TODO(zhoubo): magic number should be place a common file
    double globalTimeTxPdelayReqPeriod{kTS_NUM_DOUBLE_2};

    /// @name pdelayLatencyThreshold - Pdelay fixed value; if the measured Pdelay value exceeds this fixed value, discard the new value and use the old value.
    /// TODO(zhoubo): magic number should be place a common file
    double pdelayLatencyThreshold{kTS_NUM_0_POINT_001};

    /// @name pdelayRespAndRespFollowupTimeout -
    /// Timeout value for Pdelay_Resp and Pdelay_resp_Follow_Up after sending Pdelay_Req. A value of 0 means timeout observation is disabled.
    /// TODO(zhoubo): magic number should be place a common file
    double pdelayRespAndRespFollowupTimeout{kTS_NUM_DOUBLE_3};

    /// @name globalTimePropagationDelay -
    /// If path delay measurement is enabled, this parameter represents the default propagation delay until the first actual measured value is available. If cyclic propagation delay measurement is disabled, the side uses this parameter as a fixed value.
    /// TODO(zhoubo): magic number should be place a common file
    double globalTimePropagationDelay{kTS_NUM_0_POINT_001};

    /// @name globalTimePdelayRespEnable -
    /// If you do not expect to receive Pdelay_Req messages, you can set it to False to disable responses to Pdelay_Resp/Pdelay_Resp_Follow_Up messages.
    bool globalTimePdelayRespEnable{false};

    /// @name globalTimeTxPeriod - Sync message sending period.
    /// TODO(zhoubo): magic number should be place a common file
    double globalTimeTxPeriod{kTS_NUM_DOUBLE_5};

    /// @name globalTimeFollowupTimeout - Follow_Up message timeout after Sync message, 0 means timeout detection is disabled.
    /// TODO(zhoubo): magic number should be place a common file
    double globalTimeFollowupTimeout{kTS_NUM_DOUBLE_3};

    /// @name masterSlaveConflictDetection - Enable Master/Slave conflict detection and notification function, false means disabled.
    bool masterSlaveConflictDetection{true};
    /// @name messageCompliance - true, means using IEEE 802.1AS message format; false, means using AUTOSAR extension.
    bool messageCompliance{true};

    /////////////// CRC validate ////////////////
    /// @name rxCrcValidated - determines the strategy for CRC verification when messages are received, see [PRS_TS_00107]
    config::GlobalTimeCrcValidation rxCrcValidated{config::GlobalTimeCrcValidation::kCrcOptional};

    /// @name crcFlagsRxValidated - determines which parameters need CRC verification
    std::uint8_t crcFlagsRxValidated{0};
    /// @name crcMessageLength
    bool crcMessageLength{false};
    /// @name crcDomainNumber
    bool crcDomainNumber{false};
    /// @name crcCorrectionField
    bool crcCorrectionField{false};
    /// @name crcSourcePortIdentity
    bool crcSourcePortIdentity{false};
    /// @name crcSequenceId
    bool crcSequenceId{false};
    /// @name crcPreciseOriginTimestamp
    bool crcPreciseOriginTimestamp{false};
    /// @name globalTimeUplinkToTxSwitchResidenceTime
    double globalTimeUplinkToTxSwitchResidenceTime{kTS_NUM_0_POINT_001};
    /// @name tlvFollowupTimeSubTLV
    bool tlvFollowupTimeSubTLV{false};
    /// @name tlvFollowupStatusSubTLV
    bool tlvFollowupStatusSubTLV{false};
    /// @name tlvFollowupUserDataSubTLV
    bool tlvFollowupUserDataSubTLV{false};
    /// @name tsynTLVFollowupOFSSubTLV
    bool tsynTLVFollowupOFSSubTLV{false};

    /// @name globalTimeTxCrcSecured - controls whether the SubTlv type is Secured when the sender assembles the message, see [PRS_TS_00093].
    bool globalTimeTxCrcSecured{false};

    /// @name crcTimeFlagsTxSecured - controls which elements in the Followup message need to participate in CRC calculation when sending,
    /// see [PRS_TS_00098]. Controlled by the globalTimeTxCrcSecured master switch.
    std::uint8_t crcTimeFlagsTxSecured{kTS_NUM_63};

    /// @name globalTimeSequenceCounterJumpWidth - specifies the maximum allowed jump in the sequence counter between two consecutive synchronization messages.
    std::uint16_t globalTimeSequenceCounterJumpWidth{kTS_NUM_10};
};  /// class ProtclParameter

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_PTP_PROTCLPARAMETER_H_
