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
/// @file       ipchandlers.h
/// @brief      IPC communication interface class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=/NetworkManager/smi
/// @module_path=/NetworkManager/smi
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00004,SRS_NM_00005,SRS_NM_00014,SRS_NM_00015,SRS_NM_00016,SRS_NM_00019,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=IpcHandlers
/// @unit_description=IPC communication interface class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_IPCHANDLERS_H_
#define _ARA_NM_IPCHANDLERS_H_

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#ifdef ARA_NM_WITH_COM
    #include "ara/nm/networkstate_skeleton.h"
#endif
#include "common/common.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief  Callback interface type for setting default IP, requesting node detection start, and wakeup notification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100075
/// @trace_id_dd=DD_NM_00819
/// @needwork = ad
/// @endcode
using IpcSetEtherHandler = std::function< void() >;

/// @brief Callback interface type for setting NIC state machine state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100076
/// @trace_id_dd=DD_NM_00820
/// @needwork = ad
/// @endcode
using IpcSetEtherSMSHandler = std::function< void(ara::nm::NetworkStateType const &ethState) >;

/// @brief  Callback interface type for getting NIC state machine state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100077
/// @trace_id_dd=DD_NM_00821
/// @needwork = ad
/// @endcode
using IpcGetEtherSMSHandler = std::function< ara::nm::NetworkStateType() >;

/// @brief  Callback interface type for setting logical network request state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100078
/// @trace_id_dd=DD_NM_00822
/// @needwork = ad
/// @endcode
using IpcSetRequestStateHandler
    = std::function< ara::nm::NetworkStateType(ara::nm::NetworkStateType const requestState) >;

/// @brief  Callback interface type for getting logical network request state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100079
/// @trace_id_dd=DD_NM_00823
/// @needwork = ad
/// @endcode
using IpcGetRequestStateHandler = std::function< ara::nm::NetworkStateType() >;

/// @brief  Callback interface type for getting logical network current state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100080
/// @trace_id_dd=DD_NM_00824
/// @needwork = ad
/// @endcode
using IpcGetNetworkCurrentStateHandler = std::function< ara::nm::NetworkStateType() >;

/// @brief Callback interface type for setting NIC NM message send/receive control
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100081
/// @trace_id_dd=DD_NM_00825
/// @needwork = ad
/// @endcode
using IpcSetNmMsgCtrlTypeHandler = std::function< void(ara::nm::MessageCtrlType const &ctrType) >;

/// @brief Callback interface type for getting NIC NM message send/receive control
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100082
/// @trace_id_dd=DD_NM_00826
/// @needwork = ad
/// @endcode
using IpcGetNmMsgCtrlTypeHandler = std::function< ara::nm::MessageCtrlType() >;

/// @brief Callback interface type for getting initialization status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100083
/// @trace_id_dd=DD_NM_00827
/// @needwork = ad
/// @endcode
using IpcGetInitStateHandler = std::function< std::uint32_t() >;

/// @brief Callback interface type for getting NIC passive PN list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100084
/// @trace_id_dd=DD_NM_00828
/// @needwork = ad
/// @endcode
using IpcGetExternalPnListHandler = std::function< ara::core::String() >;

/// @brief IpcHandlers callback function registration set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100085
/// @trace_id_dd=DD_NM_00829
/// @needwork = ad
/// @endcode
struct IpcHandlers
{
    /// @brief requestDetectNodeHandle
    /// @brief Request node detection callback method
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100086
    /// @trace_id_dd=DD_NM_00830
    /// @needwork = ad
    /// @endcode
    IpcSetEtherHandler requestDetectNodeHandle{nullptr};

    /// @brief Wakeup notification
    /// @brief notifyWakeupHandle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100087
    /// @trace_id_dd=DD_NM_00831
    /// @needwork = ad
    /// @endcode
    IpcSetEtherHandler notifyWakeupHandle{nullptr};

    /// @brief setEtherStateMachineSateHandle
    /// Callback method for setting state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100088
    /// @trace_id_dd=DD_NM_00832
    /// @needwork = ad
    /// @endcode
    IpcSetEtherSMSHandler setEtherStateMachineSateHandle{nullptr};

    /// @brief getEtherStateMachineStateHandler
    /// Callback method for getting state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100089
    /// @trace_id_dd=DD_NM_00833
    /// @needwork = ad
    /// @endcode
    IpcGetEtherSMSHandler getEtherStateMachineStateHandler{nullptr};

    /// @brief setLNRequestStateHandler
    /// Callback method for setting logical network RequestState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100090
    /// @trace_id_dd=DD_NM_00834
    /// @needwork = ad
    /// @endcode
    IpcSetRequestStateHandler setLNRequestStateHandler{nullptr};

    /// @brief getLNRequestStateHandler
    /// Callback method for setting logical network RequestState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100091
    /// @trace_id_dd=DD_NM_00835
    /// @needwork = ad
    /// @endcode
    IpcGetRequestStateHandler getLNRequestStateHandler{nullptr};

    /// @brief getNetowrkCurrentStateHandler
    /// Callback method for setting logical network RequestState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100092
    /// @trace_id_dd=DD_NM_00836
    /// @needwork = ad
    /// @endcode
    IpcGetNetworkCurrentStateHandler getNetowrkCurrentStateHandler{nullptr};

    /// @brief setEtherNmMsgCtrlTypeHandler
    /// Callback interface type for setting NIC NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100093
    /// @trace_id_dd=DD_NM_00837
    /// @needwork = ad
    /// @endcode
    IpcSetNmMsgCtrlTypeHandler setEtherNmMsgCtrlTypeHandler{nullptr};

    /// @brief getEtherNmMsgCtrlTypeHandler
    /// Callback interface type for getting NIC NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100094
    /// @trace_id_dd=DD_NM_00838
    /// @needwork = ad
    /// @endcode
    IpcGetNmMsgCtrlTypeHandler getEtherNmMsgCtrlTypeHandler{nullptr};

    /// @brief getInitStateHandler
    /// Callback interface type for getting initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100095
    /// @trace_id_dd=DD_NM_00839
    /// @needwork = ad
    /// @endcode
    IpcGetInitStateHandler getInitStateHandler{nullptr};

    /// @brief getExternalPnListHandler
    /// Callback interface type for getting NIC passive PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100096
    /// @trace_id_dd=DD_NM_00840
    /// @needwork = ad
    /// @endcode
    IpcGetExternalPnListHandler getExternalPnListHandler{nullptr};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_IPCHANDLERS_H_