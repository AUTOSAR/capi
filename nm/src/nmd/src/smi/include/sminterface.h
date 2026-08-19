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
/// @file       sminterface.h
/// @brief      COM communication management class
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
/// @unit_name=SMInterface
/// @unit_description=COM communication management class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_SMINTERFACE_H_
#define _ARA_NM_SMINTERFACE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include "ara/nm/networkstate_skeleton.h"
#include "smihandlers.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Interface class for state management
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100074
/// @trace_id_dd=DD_NM_00818
/// @needwork = ad
/// @endcode
class SMInterface : public ara::nm::skeleton::NetworkStateSkeleton
{
public:
    /// @brief NetworkStateSkeleton declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100000
    /// @trace_id_dd=DD_NM_00846
    /// @needwork = ad
    /// @endcode
    using NetworkStateSkeleton = ara::nm::skeleton::NetworkStateSkeleton;

    /// @brief Declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_100001
    /// @trace_id_dd=DD_NM_00847
    /// @needwork = ad
    /// @endcode

    using NetworkStateSkeleton::NetworkStateSkeleton;

    /// @brief constructor of SMInterface.
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00613
    /// @needwork = dda
    /// @endcode
    SMInterface() = delete;

    /// @brief copy constructor is forbidden.
    ///
    /// @param other class instance.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00614
    /// @needwork = dda
    /// @endcode
    SMInterface(SMInterface const &other) = delete;

    /// @brief copy operator is forbidden.
    ///
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00615
    /// @needwork = dda
    /// @endcode
    SMInterface &operator=(SMInterface const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00616
    /// @needwork = dda
    /// @endcode
    SMInterface(SMInterface &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00617
    /// @needwork = dda
    /// @endcode
    SMInterface &operator=(SMInterface &&other) = default;

    /// @brief destructor of SMInterface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00618
    /// @needwork = dda
    /// @endcode
    ~SMInterface() final = default;

    /// @brief Set default IP. If the IP passed to the following interfaces is empty, the default IP will be used.
    /// @param ethIp
    /// @returns Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00619
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > SetDefaultEtherIp(ara::core::String const &ethIp) noexcept final;

    /// @brief Set NIC state machine state
    /// @param ethIp  NIC IP
    /// @param ethState NIC state
    /// @returns Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00620
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > SetEtherStateMachineSate(ara::core::String const &ethIp,
                                                       ara::nm::NetworkStateType const &ethState) noexcept final;

    /// @brief Get NIC state machine state
    /// @param ethIp NIC IP
    /// @returns NIC state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00621
    /// @needwork = dda
    /// @endcode
    ara::core::Future< NetworkState::GetEtherStateMachineStateOutput > GetEtherStateMachineState(
        ara::core::String const &ethIp) noexcept final;

    /// @brief Request to start node detection
    /// @param ethIp NIC IP
    /// @returns Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00622
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > RequestDetectNode(ara::core::String const &ethIp) noexcept final;

    /// @brief NIC wakeup notification, accelerate sending the first NM message
    /// @param ethIp NIC IP
    /// @returns Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00623
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > NotifyWakeup(ara::core::String const &ethIp) noexcept final;

    /// @brief Initialize, register callback
    /// @param handleIndex  Logical network name
    /// @param handle    Callback method set
    /// @returns true init ok, false fail.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00624
    /// @needwork = dda
    /// @endcode
    bool InitAndOffer(ara::core::StringView const &handleIndex, SMIHandlers const &handle) noexcept;

    /// @brief Node detection result notification
    /// @param nodeList  Node set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00625
    /// @needwork = dda
    /// @endcode
    void DetctNodesCallbak(ara::core::Vector< std::uint8_t > const &nodeList) noexcept;

    /// @brief NIC state machine state change notification
    /// @param etherIp  NIC IP
    /// @param stateType  State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00626
    /// @needwork = dda
    /// @endcode
    void SendEtherStateChanged(ara::core::String const &etherIp, ara::nm::NetworkStateType const stateType) noexcept;

    /// @brief Update NetworkCurrentState
    /// @param stateType Current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00627
    /// @needwork = dda
    /// @endcode
    void UpdateNetworkCurrentState(ara::nm::NetworkStateType const stateType) noexcept;

    /// @brief When the service stops, update the NetworkCurrentState field of the logical network to NoCom
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00628
    /// @needwork = dda
    /// @endcode
    void NotifyNMStop() noexcept;

    /// @brief Set NM message send/receive control
    /// @param ethIp  NIC IP
    /// @param ctrType  Send/receive control
    /// @returns Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00629
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > SetMessageCtrlType(ara::core::String const &ethIp,
                                                 ara::nm::MessageCtrlType const &ctrType) noexcept final;

    /// @brief Get NM message send/receive control
    /// @param ethIp  NIC IP
    /// @returns NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00630
    /// @needwork = dda
    /// @endcode
    ara::core::Future< NetworkState::GetMessageCtrlTypeOutput > GetMessageCtrlType(
        ara::core::String const &ethIp) noexcept final;

    /// @brief Get initialization status
    /// @returns Initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00631
    /// @needwork = dda
    /// @endcode
    ara::core::Future< NetworkState::GetInitStateOutput > GetInitState() noexcept final;

    /// @brief PN request notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00632
    /// @needwork = dda
    /// @endcode
    void PassivePnRequest(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept;

    /// @brief PN release notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00633
    /// @needwork = dda
    /// @endcode
    void PassivePnRelease(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept;

    /// @brief Get passive PN list
    /// @param ethIp  NIC IP
    /// @returns Passive PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00634
    /// @needwork = dda
    /// @endcode
    ara::core::Future< NetworkState::GetPassivePnRequestListOutput > GetPassivePnRequestList(
        ara::core::String const &ethIp) noexcept final;

private:
    /// @brief handler_
    /// current callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00635
    /// @needwork = dda
    /// @endcode
    SMIHandlers handler_{};

    /// @brief logicNetworkCurrentState_
    /// current state of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00636
    /// @needwork = dda
    /// @endcode
    NetworkStateType logicNetworkCurrentState_{NetworkStateType::kNoCom};

    /// @brief logicNetworkRequestedState_
    /// request state of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00637
    /// @needwork = dda
    /// @endcode
    NetworkStateType logicNetworkRequestedState_{NetworkStateType::kNoCom};

    /// @brief terminate_
    /// Whether it is currently terminating
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00638
    /// @needwork = dda
    /// @endcode
    bool terminate_{false};

    /// @brief handleIndex_
    /// logic network handle index
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00639
    /// @needwork = dda
    /// @endcode
    ara::core::StringView handleIndex_{};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_SMINTERFACE_H_