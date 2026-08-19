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
/// @file       logicnetworkoper.h
/// @brief      Logical network management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00004,SRS_NM_00005,SRS_NM_00014,SRS_NM_00015,SRS_NM_00016,SRS_NM_00019,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=LogicNetworkOper
/// @unit_description=Logical network management class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_LOGICNETWORKOPER_H_
#define _ARA_NM_LOGICNETWORKOPER_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include "common/common.h"
#include "partialnetworkoper.h"
#include "smi/include/nmipcproc.h"
#ifdef ARA_NM_WITH_COM
    #include "smi/include/sminterface.h"
#endif
namespace ara {
namespace nm {
namespace internal {

/// @brief logic network class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100059
/// @trace_id_dd=DD_NM_00803
/// @needwork = ad
/// @endcode
class LogicNetworkOper final
{
public:
    /// @brief constructor of LogicNetworkOper.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00451
    /// @needwork = dda
    /// @endcode
    LogicNetworkOper() = default;

    /// @brief copy constructor is forbidden.
    ///
    /// @param other class instance.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00452
    /// @needwork = dda
    /// @endcode
    LogicNetworkOper(LogicNetworkOper const &other) = delete;

    /// @brief copy operator is forbidden.
    ///
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00453
    /// @needwork = dda
    /// @endcode
    LogicNetworkOper &operator=(LogicNetworkOper const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00454
    /// @needwork = dda
    /// @endcode
    LogicNetworkOper(LogicNetworkOper &&other) = delete;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00455
    /// @needwork = dda
    /// @endcode
    LogicNetworkOper &operator=(LogicNetworkOper &&other) = delete;

    /// @brief destructor of UdpNm.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00456
    /// @needwork = dda
    /// @endcode
    ~LogicNetworkOper();

    /// @brief initialize the class.
    /// @param instanceSpec Communication descriptor.
    /// @param handleIndex Logical network name.
    /// @returns true init ok, false fail.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00063}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00457
    /// @needwork = dda
    /// @endcode
    bool Init(ara::core::String const &instanceSpec, ara::core::String const &handleIndex) noexcept;

    /// @brief initialize the class.
    /// @param handleIndex Logical network name.
    /// @returns true init ok, false fail.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00063}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00458
    /// @needwork = dda
    /// @endcode
    bool InitIpc(ara::core::String const &handleIndex) noexcept;

    /// @brief process of change of partial network communicator mode.
    /// @param currentState partial network communicator mode.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00083}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00459
    /// @needwork = dda
    /// @endcode
    void NotifyPartialNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept;

    /// @brief process of change of ethernet communicator mode.
    /// @param etherIp connect ip.
    /// @param stateType Current state.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00083}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00460
    /// @needwork = dda
    /// @endcode
    void NotifyEtherStateChanged(ara::core::String const &etherIp,
                                 EthernetStateMachineStateType const &stateType) noexcept;

    /// @brief add the pointer of ethernet state machine object to logic network.
    /// @param ethernetPtr pointer of ethernet state machine object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00461
    /// @needwork = dda
    /// @endcode
    void AddNMEtherStateMachinePtr(std::shared_ptr< NMEtherStateMachine > const &ethernetPtr) noexcept
    {
        if (nullptr != ethernetPtr) {
            etherSMPtrVec_[ethernetPtr->GetIpAddress()] = ethernetPtr;
            ethernetPtr->RegistModeObserverHandler(
                [this](NetworkStateType const currentMode) noexcept -> void { _notifyEtherModeChanged(currentMode); });
            ethernetPtr->RegistStateObserverHandler(
                [this](ara::core::String const &etherIp,
                       EthernetStateMachineStateType const &stateType) noexcept -> void {
                    NotifyEtherStateChanged(etherIp, stateType);
                });
        }
    };

    /// @brief add the pointer of partial network object to logic network.
    /// @param pnPtr pointer of partial network object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00462
    /// @needwork = dda
    /// @endcode
    void AddPartialNetworkStatusPtr(std::shared_ptr< PartialNetworkOper > const &pnPtr) noexcept
    {
        if (nullptr != pnPtr) {
            pnPtrVec_.push_back(pnPtr);
            pnPtr->RegistModeObserverHandler(
                [this](NetworkStateType const currentMode) noexcept -> void {
                    NotifyPartialNetworkCurrentStateChanged(currentMode);
                },
                [this](ara::core::String const &etherIp, EthernetStateMachineStateType const &stateType) noexcept
                -> void { NotifyEtherStateChanged(etherIp, stateType); });

            pnPtr->RegistPassivePnObserHandler(
                [this](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
                    NotifyPassivePnRequest(etherIp, pnList);
                },
                [this](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
                    NotifyPassivePnRelease(etherIp, pnList);
                });
        }
    };

    /// @brief notify the active logic network change to no-com when terminating
    /// nm service;
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00090}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00463
    /// @needwork = dda
    /// @endcode
    void NotifyNMStop() noexcept;

    /// @brief Node detection.
    /// @param etherIp NIC IP
    /// @param promise promise object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00464
    /// @needwork = dda
    /// @endcode
    void RequestDetectNode(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept;

    /// @brief  Set default IP. If the IP passed to the following interfaces is empty, the default IP will be used.
    /// @param etherIp NIC IP
    /// @param promise promise object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00465
    /// @needwork = dda
    /// @endcode
    void SetDefaultEtherIp(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept;

    /// @brief Set NIC state machine state (not supposed to be set normally)
    /// @param etherIp NIC IP
    /// @param ethState NIC IP state
    /// @param promise promise object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00466
    /// @needwork = dda
    /// @endcode
    void SetEtherStateMachineSate(ara::core::String const &etherIp,
                                  ara::nm::NetworkStateType const &ethState,
                                  ara::core::Promise< void > &promise) noexcept;

    /// @brief Get NIC state machine state (not supposed to be obtained normally)
    /// @param etherIp NIC IP
    /// @param innerState Object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00467
    /// @needwork = dda
    /// @endcode
    void GetEtherStateMachineState(ara::core::String const &etherIp, ara::nm::NetworkStateType &innerState) noexcept;

    /// @brief Node wakeup.
    /// @param etherIp NIC IP
    /// @param promise promise object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00468
    /// @needwork = dda
    /// @endcode
    void NotifyWakeup(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept;

    /// @brief Set NM message send/receive control
    /// @param etherIp NIC IP
    /// @param ctrType Send/receive control
    /// @param promise promise object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00469
    /// @needwork = dda
    /// @endcode
    void SetMessageCtrlType(ara::core::String const &etherIp,
                            ara::nm::MessageCtrlType const &ctrType,
                            ara::core::Promise< void > &promise) noexcept;

    /// @brief Get NM message send/receive control status
    /// @param etherIp NIC IP
    /// @param etherCtrlType Status return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00470
    /// @needwork = dda
    /// @endcode
    void GetMessageCtrlType(ara::core::String const &etherIp, ara::nm::MessageCtrlType &etherCtrlType) noexcept;

    /// @brief Get initialization status
    /// @param lnInitCode Status return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00471
    /// @needwork = dda
    /// @endcode
    void GetInitState(std::uint32_t &lnInitCode) noexcept;

    /// @brief inner process of setting request.
    /// @param requestState set value.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00084}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00472
    /// @needwork = dda
    /// @endcode
    void SetLogicNetworkRequestedState(NetworkStateType const requestState) noexcept;

    /// @brief PN request notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @uptrace=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00473
    /// @needwork = dda
    /// @endcode
    void NotifyPassivePnRequest(ara::core::String const &etherIp, ara::core::String const &pnList) const noexcept;

    /// @brief PN release notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @uptrace=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00474
    /// @needwork = dda
    /// @endcode
    void NotifyPassivePnRelease(ara::core::String const &etherIp, ara::core::String const &pnList) const noexcept;

    /// @brief Get passive PN list
    /// @param etherIp NIC IP
    /// @param etherPnList Passive PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00475
    /// @needwork = dda
    /// @endcode
    void GetPassivePnRequestList(ara::core::String const &etherIp, ara::core::String &etherPnList) noexcept;

private:
    /// @brief report network current state of logic network to sm.
    /// @param currentState network current state of logic network.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00476
    /// @needwork = dda
    /// @endcode
    void _notifyNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept;

    /// @brief Node detection result notification
    /// @param nodeList Node detection result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00477
    /// @needwork = dda
    /// @endcode
    void _detctNodesCallbak(ara::core::Vector< std::uint8_t > const &nodeList) const noexcept;

    /// @brief process of change of ethernet communicator mode.
    /// @param currentState ethernet communicator mode.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00083}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00478
    /// @needwork = dda
    /// @endcode
    void _notifyEtherModeChanged(NetworkStateType const currentState) noexcept;

private:
    /// @brief instanceSpec_
    /// instance specifier of logic network service skeleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00479
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceSpec_{};

    /// @brief logicNetworkCurrentState_
    /// current state of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00480
    /// @needwork = dda
    /// @endcode
    NetworkStateType logicNetworkCurrentState_{NetworkStateType::kNoCom};

    /// @brief logicNetworkRequestedState_
    /// request state of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00481
    /// @needwork = dda
    /// @endcode
    NetworkStateType logicNetworkRequestedState_{NetworkStateType::kNoCom};

    /// @brief pnPtrVec_
    /// collection of partial network pointer of logic network, dont need to
    /// release
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00482
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::shared_ptr< PartialNetworkOper > > pnPtrVec_{};

    /// @brief etherSMPtrVec_
    /// collection of ethernet state machine pointer of logic network, dont need
    /// to release
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00483
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > > etherSMPtrVec_{};

    /// @brief fullComPn_
    /// count of of full-com partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00484
    /// @needwork = dda
    /// @endcode
    std::uint32_t fullComPn_{0U};

    /// @brief fullComEther_
    /// count of of full-com ethernet communicator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00485
    /// @needwork = dda
    /// @endcode
    std::uint32_t fullComEther_{0U};

    /// @brief handleIndex_
    /// logic network handle index
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00486
    /// @needwork = dda
    /// @endcode
    ara::core::String handleIndex_{};
#ifdef ARA_NM_WITH_COM
    /// @brief nmSmi_
    /// service skeleton pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00487
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SMInterface > nmSmi_{nullptr};
#endif

    /// @brief terminate_
    /// indicates, that nmd is terminating
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00488
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > terminate_{false};

    /// @brief defaultEtherIp_
    /// Default NIC IP for method operations
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00489
    /// @needwork = dda
    /// @endcode
    ara::core::String defaultEtherIp_{};

    /// @brief bSupportCom_
    /// Whether to support 20 COM communication
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00490
    /// @needwork = dda
    /// @endcode
    bool bSupportCom_{false};

    /// @brief bSupportIpc_
    /// Whether to support 23 IPC communication
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00491
    /// @needwork = dda
    /// @endcode
    bool bSupportIpc_{false};

    /// @brief etherStateMap_
    ///  Save NIC status to prevent duplicate notifications to adapter
    /// If LN1's PN1 and PN2 both contain eth1,
    /// then eth1's status will be notified twice, the logical network will filter it, only notifying when status changes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00492
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, EthernetStateMachineStateType > etherStateMap_{};

    /// @brief EtherIterator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00887
    /// @needwork = dda
    /// @endcode
    using EtherIterator = ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > >::iterator;

    /// @brief EthStateIter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00888
    /// @needwork = dda
    /// @endcode
    using EthStateIter = ara::core::Map< ara::core::String, EthernetStateMachineStateType >::iterator;

    /// @brief ParttIter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00889
    /// @needwork = dda
    /// @endcode
    using ParttIter = ara::core::Vector< std::shared_ptr< PartialNetworkOper > >::iterator;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_LOGICNETWORKOPER_H_