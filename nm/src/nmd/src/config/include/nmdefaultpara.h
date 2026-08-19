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
/// @file       nmdefaultpara.h
/// @brief      Machine basic configuration
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/config
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmDefaultPara
/// @unit_description=Machine basic configuration
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIG_NMDEDAULTPARA_H_
#define _ARA_NM_CONFIG_NMDEDAULTPARA_H_
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

namespace ara {
namespace nm {
namespace internal {

/// @brief NmDefaultPara, default parameter struct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100054
/// @trace_id_dd=DD_NM_00793
/// @needwork = ad
/// @endcode
struct NmDefaultPara final
{
public:
    /// @brief defaultNmTimeoutTime
    /// default Network Timeout for NM packets.
    double defaultNmTimeoutTime{0};

    /// @brief defaultNmMsgCycleTime
    /// default Period of a NM-message
    double defaultNmMsgCycleTime{0};

    /// @brief defaultNmRepeatMessageTime
    /// default Timeout for Repeat Message State
    double defaultNmRepeatMessageTime{0};

    /// @brief defaultNmWaitBusSleepTime
    /// default Timeout for bus calm down phase
    double defaultNmWaitBusSleepTime{0};

    /// @brief defaultNmMsgCycleOffset
    /// default Time offset in the periodic transmission node. It determines the
    /// start delay of the transmission.
    double defaultNmMsgCycleOffset{0};

    /// @brief defaultNmPduNidPosition
    /// default position, the position of the source node identifier within the NM
    /// message
    std::int32_t defaultNmPduNidPosition{-1};

    /// @brief defaultNmPduCbvPosition
    /// default position, the position of the control bit vector within the NM
    /// message
    std::int32_t defaultNmPduCbvPosition{-1};

    /// @brief defaultNmPnInfoLength
    /// default length (in bytes) of the PN request information in the NM message
    std::int32_t defaultNmPnInfoLength{-1};

    /// @brief defaultNmPnInfoOffset
    /// default  offset (in bytes) of the PN request information in the NM message
    std::int32_t defaultNmPnInfoOffset{-1};

    /// @brief defaultNmUserDataLength
    /// default  Specifies the length (in bytes) of the user data information in
    /// the NM message. User data excludes the PN information.
    std::uint32_t defaultNmUserDataLength{0};

    /// @brief defaultNmPnEnabled
    /// default value. Enables or disables support of partial networking. true:
    /// Partial networking supported
    bool defaultNmPnEnabled{false};

    /// @brief defaultNmPnResetTime
    /// default value. Specifies the runtime of the reset timer in seconds
    double defaultNmPnResetTime{0};

    /// @brief defaultPncPrepareSleepTime
    /// Time in seconds the PNC state machine shall wait in PNC_PREPARE_SLEEP
    double defaultPncPrepareSleepTime{0};

    /// @brief defaultNmNodeId
    /// default value. Specifies the Node Identification of this NM node
    std::int16_t defaultNmNodeId{0};

    /// @brief defaultNmUserDataEnabled
    /// default value. Enables or disables support of user data information
    bool defaultNmUserDataEnabled{false};

    /// @brief skiptimerCheck
    /// Whether to skip timer check, required by AUTOSAR, some customers' timers do not meet AUTOSAR requirements
    bool skiptimerCheck{false};

    /// @brief didInstance
    /// default value. Enables or disables support of user data information
    ara::core::String didInstance{""};

    /// @brief diagMonitorId
    /// default value. Enables or disables support of user data information
    ara::core::String diagMonitorId{""};

    /// @brief diagOperationCycleInstance
    /// default value. Enables or disables support of user data information
    ara::core::String diagOperationCycleInstance{""};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIG_NMDEDAULTPARA_H_ */
