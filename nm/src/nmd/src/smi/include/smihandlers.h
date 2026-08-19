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
/// @file       smihandlers.h
/// @brief      COM communication interface
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
/// @unit_name=SMIHandlers
/// @unit_description=COM communication interface
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_SMIHANDLERS_H_
#define _ARA_NM_SMIHANDLERS_H_

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include "ara/nm/networkstate_skeleton.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief  Callback interface type for setting default IP, requesting node detection start, and NIC wakeup notification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00848
/// @needwork = ad
/// @endcode
using SetEtherHandler = std::function< void(ara::core::String const &ethIp, ara::core::Promise< void > &promise) >;

/// @brief Callback interface type for setting NIC state machine state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100003
/// @trace_id_dd=DD_NM_00849
/// @needwork = ad
/// @endcode
using SetEtherSMSHandler = std::function< void(
    ara::core::String const &ethIp, ara::nm::NetworkStateType const &ethState, ara::core::Promise< void > &promise) >;

/// @brief  Callback interface type for getting NIC state machine state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100004
/// @trace_id_dd=DD_NM_00850
/// @needwork = ad
/// @endcode
using GetEtherSMSHandler = std::function< void(ara::core::String const &ethIp, ara::nm::NetworkStateType &type) >;

/// @brief  Callback interface type for setting logical network request state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100005
/// @trace_id_dd=DD_NM_00851
/// @needwork = ad
/// @endcode
using SetRequestStateHandler
    = std::function< ara::core::Future< ara::nm::NetworkStateType >(ara::nm::NetworkStateType const requestState) >;

/// @brief Callback interface type for setting NIC NM message send/receive control
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100006
/// @trace_id_dd=DD_NM_00852
/// @needwork = ad
/// @endcode
using SetNmMsgCtrlTypeHandler = std::function< void(
    ara::core::String const &ethIp, ara::nm::MessageCtrlType const &ctrType, ara::core::Promise< void > &promise) >;

/// @brief Callback interface type for getting NIC NM message send/receive control
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100007
/// @trace_id_dd=DD_NM_00853
/// @needwork = ad
/// @endcode
using GetNmMsgCtrlTypeHandler
    = std::function< void(ara::core::String const &ethIp, ara::nm::MessageCtrlType &etherCtrlType) >;

/// @brief Callback interface type for getting initialization status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100008
/// @trace_id_dd=DD_NM_00854
/// @needwork = ad
/// @endcode
using GetInitStateHandler = std::function< void(std::uint32_t &lnInitCode) >;

/// @brief Callback interface type for getting NIC passive PN list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100009
/// @trace_id_dd=DD_NM_00855
/// @needwork = ad
/// @endcode
using GetPassivePnListHandler = std::function< void(ara::core::String const &ethIp, ara::core::String &etherPnList) >;

/// @brief SMIHandlers callback function registration set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100097
/// @trace_id_dd=DD_NM_00841
/// @needwork = ad
/// @endcode
struct SMIHandlers
{
    /// @brief Set default IP callback method
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100010
    /// @trace_id_dd=DD_NM_00856
    /// @needwork = ad
    /// @endcode
    SetEtherHandler setDefaultEtherIpHandle{nullptr};

    /// @brief requestDetectNodeHandle
    /// @brief Request node detection callback method
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100011
    /// @trace_id_dd=DD_NM_00857
    /// @needwork = ad
    /// @endcode
    SetEtherHandler requestDetectNodeHandle{nullptr};

    /// @brief Wakeup notification
    /// @brief notifyWakeupHandle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100012
    /// @trace_id_dd=DD_NM_00858
    /// @needwork = ad
    /// @endcode
    SetEtherHandler notifyWakeupHandle{nullptr};

    /// @brief setEtherStateMachineSateHandle
    /// Callback method for setting state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100013
    /// @trace_id_dd=DD_NM_00859
    /// @needwork = ad
    /// @endcode
    SetEtherSMSHandler setEtherStateMachineSateHandle{nullptr};

    /// @brief getEtherStateMachineStateHandler
    /// Callback method for getting state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100014
    /// @trace_id_dd=DD_NM_00860
    /// @needwork = ad
    /// @endcode
    GetEtherSMSHandler getEtherStateMachineStateHandler{nullptr};

    /// @brief setLNRequestStateHandler
    /// Callback method for setting logical network RequestState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100015
    /// @trace_id_dd=DD_NM_00861
    /// @needwork = ad
    /// @endcode
    SetRequestStateHandler setLNRequestStateHandler{nullptr};

    /// @brief setEtherNmMsgCtrlTypeHandler
    /// Callback interface type for setting NIC NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100016
    /// @trace_id_dd=DD_NM_00862
    /// @needwork = ad
    /// @endcode
    SetNmMsgCtrlTypeHandler setEtherNmMsgCtrlTypeHandler{nullptr};

    /// @brief getEtherNmMsgCtrlTypeHandler
    /// Callback interface type for getting NIC NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100017
    /// @trace_id_dd=DD_NM_00863
    /// @needwork = ad
    /// @endcode
    GetNmMsgCtrlTypeHandler getEtherNmMsgCtrlTypeHandler{nullptr};

    /// @brief getInitStateHandler
    /// Callback interface type for getting initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100018
    /// @trace_id_dd=DD_NM_00864
    /// @needwork = ad
    /// @endcode
    GetInitStateHandler getInitStateHandler{nullptr};

    /// @brief getPassivePnListHandler
    /// Callback interface type for getting NIC passive PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100019
    /// @trace_id_dd=DD_NM_00865
    /// @needwork = ad
    /// @endcode
    GetPassivePnListHandler getPassivePnListHandler{nullptr};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_SMIHANDLERS_H_