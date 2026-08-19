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
/// @file       partialnetworkoper.h
/// @brief      Partial network management class
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
/// @trace_id_sr=SRS_NM_00003
/// @unit_name=PartialNetworkOper
/// @unit_description=Partial network management class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_PARTIALNETWORKOPER_H_
#define _ARA_NM_PARTIALNETWORKOPER_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include "nmetherstatemachine.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Partial network communication mode change callback function, called when a registered event occurs
/// @param comStateType -  Partial network communication mode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00899
/// @needwork = ad
/// @endcode
using PnComModeHandler = std::function< void(NetworkStateType const &comStateType) >;

/// @brief partial network class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100058
/// @trace_id_dd=DD_NM_00802
/// @needwork = ad
/// @endcode
class PartialNetworkOper final
{
public:
    /// @brief constructor of PartialNetworkOper.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00399
    /// @needwork = dda
    /// @endcode
    PartialNetworkOper() = default;

    /// @brief copy constructor is forbidden.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00400
    /// @needwork = dda
    /// @endcode
    PartialNetworkOper(PartialNetworkOper const &other) = delete;

    /// @brief copy operator is forbidden.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00401
    /// @needwork = dda
    /// @endcode
    PartialNetworkOper &operator=(PartialNetworkOper const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00402
    /// @needwork = dda
    /// @endcode
    PartialNetworkOper(PartialNetworkOper &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00403
    /// @needwork = dda
    /// @endcode
    PartialNetworkOper &operator=(PartialNetworkOper &&other) = default;

    /// @brief Destructor of PartialNetworkOper.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00404
    /// @needwork = dda
    /// @endcode
    ~PartialNetworkOper() noexcept;

    /// @brief set request state of partial network.
    /// @param requestState network request state of partial network.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00405
    /// @needwork = dda
    /// @endcode
    void SetPartialNetworkRequestedState(NetworkStateType const requestState) noexcept;

    /// @brief add mode observer.
    /// @param modeChangeHandler mode change callback
    /// @param lnWatchEtherStateHandler mode change callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00406
    /// @needwork = dda
    /// @endcode
    void RegistModeObserverHandler(PnComModeHandler const &modeChangeHandler,
                                   EtherStateChangeHandler const &lnWatchEtherStateHandler) noexcept
    {
        pnModObservGroup_.emplace_back(modeChangeHandler);
        lnWatchEtherStateGroup_.emplace_back(lnWatchEtherStateHandler);
    };

    /// @brief PN request notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00407
    /// @needwork = dda
    /// @endcode
    void NotifyPassivePnRequest(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept;

    /// @brief PN release notification.
    /// @param etherIp NIC IP.
    /// @param pnList PN set.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00408
    /// @needwork = dda
    /// @endcode
    void NotifyPassivePnRelease(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept;

    /// @brief add the pointer of ethernet state machine object to partial
    /// network.
    /// @param ethernetPtr pointer of ethernet state machine.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00409
    /// @needwork = dda
    /// @endcode
    void AddNMEtherStateMachinePtr(std::shared_ptr< NMEtherStateMachine > const &ethernetPtr) noexcept;

    /// @brief content initialization.
    /// @param pncId      Partial network ID
    /// @param handleIndex handle index of partial network.
    /// @param pnResetTime PncReadySleep state timeout duration.
    /// @param pncPrepareSleepTime pncPrepareSleep state timeout duration.
    /// @returns               0 ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00410
    /// @needwork = dda
    /// @endcode
    std::int32_t Init(std::uint16_t const pncId,
                      ara::core::String const &handleIndex,
                      double const pnResetTime,
                      double const pncPrepareSleepTime) noexcept;

    /// @brief get the count of loigc networks of partial network.
    /// @returns counts of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00411
    /// @needwork = dda
    /// @endcode
    size_t GetLogicNetworkCount() const noexcept { return pnModObservGroup_.size(); };

    /// @brief Node detection.
    /// @param etherIp NIC IP
    /// @param callBack Callback method
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00412
    /// @needwork = dda
    /// @endcode
    void RequestDetectNode(ara::core::String const &etherIp, DetectNodeCallBack const &callBack) noexcept;

    /// @brief Passive wakeup.
    /// @param etherIp NIC IP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00413
    /// @needwork = dda
    /// @endcode
    void NotifyWakeup(ara::core::String const &etherIp) noexcept;

    /// @brief Whether the specified NIC is included.
    /// @param etherIp NIC IP
    /// @returns true if the specified NIC is included
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00414
    /// @needwork = dda
    /// @endcode
    bool HaveTheEther(ara::core::String const &etherIp) const noexcept { return 0U != etherSMPtrVec_.count(etherIp); };

    /// @brief Set NIC state machine state.
    /// @param etherIp NIC IP
    /// @param newState New state machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00415
    /// @needwork = dda
    /// @endcode
    void SetEtherStateMachineSate(ara::core::String const &etherIp,
                                  EthernetStateMachineStateType const newState) noexcept;

    /// @brief Get NIC state machine state.
    /// @param etherIp NIC IP
    /// @returns State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00416
    /// @needwork = dda
    /// @endcode
    EthernetStateMachineStateType GetEtherStateMachineState(ara::core::String const &etherIp) const noexcept;

    /// @brief Set NM message send/receive control
    /// @param etherIp NIC IP
    /// @param ctrType Send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00417
    /// @needwork = dda
    /// @endcode
    void SetMessageCtrlType(ara::core::String const &etherIp, ara::nm::MessageCtrlType const &ctrType) noexcept;

    /// @brief Get NM message send/receive control status
    /// @param etherIp NIC IP
    /// @returns Send/receive control status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00418
    /// @needwork = dda
    /// @endcode
    ara::nm::MessageCtrlType GetMessageCtrlType(ara::core::String const &etherIp) noexcept;

    /// @brief Get initialization status
    /// @returns Initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00419
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetInitState() const noexcept;

    /// @brief add pasive pn observer.
    /// @param requestHandler pn observer
    /// @param releaseHandler pn observer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00420
    /// @needwork = dda
    /// @endcode
    void RegistPassivePnObserHandler(PnInformHandler const &requestHandler,
                                     PnInformHandler const &releaseHandler) noexcept
    {
        pnRequestObservGroup_.emplace_back(requestHandler);
        pnReleaseObservGroup_.emplace_back(releaseHandler);
    };

    /// @brief Get passive PN list
    /// @param etherIp NIC IP
    /// @returns Passive PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00421
    /// @needwork = dda
    /// @endcode
    ara::core::String GetPassivePnRequestList(ara::core::String const &etherIp) noexcept;

private:
    /// @brief update pn is active.
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00086}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00422
    /// @needwork = dda
    /// @endcode
    void _updatePnMsg() noexcept;

    /// @brief process of change of ethernet communicator mode.
    /// @param currentState ethernet communicator mode.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00423
    /// @needwork = dda
    /// @endcode
    void _notifyEtherModeChanged(NetworkStateType const currentState) noexcept;

    /// @brief pn reset timer process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00424
    /// @needwork = dda
    /// @endcode
    void _pnResetTimerCalled() noexcept;

    /// @brief pn prepare-sleep timer process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00425
    /// @needwork = dda
    /// @endcode
    void _pnPrepareSleepTimerCalled() noexcept;

    /// @brief pnc state machine from other state To PncRequested
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00426
    /// @needwork = dda
    /// @endcode
    void _otherStateToPncRequested() noexcept;

    /// @brief pnc state machine from PncRequested To PncReadySleep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00427
    /// @needwork = dda
    /// @endcode
    void _pncRequestedToPncReadySleep() noexcept;

    /// @brief pnc state machine from PncReadySleep To PncPrepareSleep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00428
    /// @needwork = dda
    /// @endcode
    void _pncReadySleepToPncPrepareSleep() noexcept;

    /// @brief prepare sleep timer process to No Communication
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00429
    /// @needwork = dda
    /// @endcode
    void _pncParepareSleepToNoCommunication() noexcept;

    /// @brief switch to pnc ready sleep, from no-communication or prepare-sleep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00430
    /// @needwork = dda
    /// @endcode
    void _switchToPncReadySleep() noexcept;

    /// @brief report current state of to logic network.
    /// @param currentState network request state.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00431
    /// @needwork = dda
    /// @endcode
    void _notifyPartialNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept;

    /// @brief whether or not the pnc has been request(internal or external) AAAAA
    /// @returns true requested
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00432
    /// @needwork = dda
    /// @endcode
    bool _hasBeenRequested() const noexcept
    {
        return ((etherSMPtrVec_.size() == fullComEther_)
                && (((PNCSMStateType::kNmPncRequest == pnSMStateType_) || (etherSMPtrVec_.size() == pnMsgCount_))));
    }

private:
    /// @brief pnComState_
    /// current communication state of partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00433
    /// @needwork = dda
    /// @endcode
    NetworkStateType pnComState_{NetworkStateType::kNoCom};

    /// @brief pnSMStateType_
    /// current state type of partial network state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00434
    /// @needwork = dda
    /// @endcode
    PNCSMStateType pnSMStateType_{PNCSMStateType::kNmPncNoCommunication};

    /// @brief pnMsgCount_
    /// receive PnMsgCount in pnResetTimer period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00435
    /// @needwork = dda
    /// @endcode
    std::uint32_t pnMsgCount_{0};

    /// @brief networkRequestedCount_
    /// request count of partial network, partial network may belong to several
    /// logic networks
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00436
    /// @needwork = dda
    /// @endcode
    std::uint8_t networkRequestedCount_{0};

    /// @brief etherSMPtrVec_
    /// collection of ethernet communicator of this partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00437
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > > etherSMPtrVec_{};

    /// @brief fullComEther_
    /// count of full-com ethernet communicator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00438
    /// @needwork = dda
    /// @endcode
    std::uint32_t fullComEther_{0};

    /// @brief handleIndex_
    /// index of partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00439
    /// @needwork = dda
    /// @endcode
    ara::core::String handleIndex_{};

    /// @brief pnId_
    /// id of partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00440
    /// @needwork = dda
    /// @endcode
    std::uint16_t pnId_{};

    /// @brief mainLoop_
    /// pointer of event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00441
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{};

    /// @brief pnResetTime_
    /// PncReadySleep state timeout duration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00442
    /// @needwork = dda
    /// @endcode
    double pnResetTime_{};

    /// @brief pncPrepareSleepTime_
    /// pncPrepareSleep state timeout duration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00443
    /// @needwork = dda
    /// @endcode
    double pncPrepareSleepTime_{};

    /// @brief pnResetTimerRef_
    /// Specifies the runtime of the reset timer in seconds. This reset time is
    /// valid for the reset of PN requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00444
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr pnResetTimerRef_{nullptr};

    /// @brief pncPrepareSleepTimerRef_
    /// timer to count Time in seconds the PNC state machine shall wait in
    /// PNC_PREPARE_SLEEP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00445
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr pncPrepareSleepTimerRef_{nullptr};

    /// @brief pnModObservGroup_
    /// Set of NIC mode changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00446
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnComModeHandler > pnModObservGroup_{};

    /// @brief lnWatchEtherStateGroup_
    /// Set of NIC mode changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00447
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherStateChangeHandler > lnWatchEtherStateGroup_{};

    /// @brief pnRequestObservGroup_
    /// Set of NIC mode changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00448
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnInformHandler > pnRequestObservGroup_;

    /// @brief pnReleaseObservGroup_
    /// Set of NIC mode changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00449
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnInformHandler > pnReleaseObservGroup_;

    /// @brief EtherIterator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00901
    /// @needwork = dda
    /// @endcode
    using EtherIterator = ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > >::iterator;

    /// @brief ConstEthIterator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00902
    /// @needwork = dda
    /// @endcode
    using ConstEthIterator
        = ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > >::const_iterator;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_PARTIALNETWORKOPER_H_