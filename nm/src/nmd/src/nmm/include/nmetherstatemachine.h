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
/// @file       nmetherstatemachine.h
/// @brief      Ethernet state machine management class
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
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002,SRS_NM_00003,SRS_NM_00006,SRS_NM_00007,SRS_NM_00008,SRS_NM_00009,SRS_NM_00010,SRS_NM_00011,SRS_NM_00012,SRS_NM_00013,SRS_NM_00014,SRS_NM_00015,SRS_NM_00018,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=NMEtherStateMachine
/// @unit_description=Ethernet state machine management class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_NMETHERSTATEMACHINE_H_
#define _ARA_NM_NMETHERSTATEMACHINE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <memory>

#include "config/include/configure.h"
#include "protcl/include/nmmsg.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Ethernet NIC communication mode change callback function, called when a registered event occurs
/// @param comStateType -  Ethernet NIC communication mode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00890
/// @needwork = ad
/// @endcode
using EtherComModeHandler = std::function< void(NetworkStateType const &comStateType) >;

/// @brief PN callback for NIC affiliation
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00891
/// @needwork = ad
/// @endcode
using PnActiveHandler = std::function< void(void) >;

/// @brief Node detection result notification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00892
/// @needwork = ad
/// @endcode
using DetectNodeCallBack = std::function< void(ara::core::Vector< std::uint8_t > const &nodeList) >;

/// @brief Ethernet NIC state machine state change callback function, called when a registered event occurs
/// @param stateType -  Ethernet NIC communication mode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100003
/// @trace_id_dd=DD_NM_00893
/// @needwork = ad
/// @endcode
using EtherStateChangeHandler
    = std::function< void(ara::core::String const &etherIp, EthernetStateMachineStateType const &stateType) >;

/// @brief Passive PN notification
/// @param etherIp -  Ethernet NIC IP
/// @param pnList -  PN list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100004
/// @trace_id_dd=DD_NM_00894
/// @needwork = ad
/// @endcode
using PnInformHandler = std::function< void(ara::core::String const &etherIp, ara::core::String const &pnList) >;

/// @brief ethernet communicator state machine
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100060
/// @trace_id_dd=DD_NM_00804
/// @needwork = ad
/// @endcode
class NMEtherStateMachine final
{
public:
    /// @brief ForceInterfaceCallback declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00900
    /// @needwork = dda
    /// @endcode
    using ForceInterfaceCallback = std::function< void() >;

    /// @brief constructor of NMEtherStateMachine.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00494
    /// @needwork = dda
    /// @endcode
    NMEtherStateMachine() = default;

    /// @brief copy constructor is forbidden.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00495
    /// @needwork = dda
    /// @endcode
    NMEtherStateMachine(NMEtherStateMachine const &other) = delete;

    /// @brief copy operator is forbidden.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00496
    /// @needwork = dda
    /// @endcode
    NMEtherStateMachine &operator=(NMEtherStateMachine const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00497
    /// @needwork = dda
    /// @endcode
    NMEtherStateMachine(NMEtherStateMachine &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00498
    /// @needwork = dda
    /// @endcode
    NMEtherStateMachine &operator=(NMEtherStateMachine &&other) = default;

    /// @brief destructor of NMEtherStateMachine.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00499
    /// @needwork = dda
    /// @endcode
    ~NMEtherStateMachine() noexcept;

    /// @brief request network.
    /// @param requestEther true logic network request network otherwise false
    /// @param pnRequestInex partial network request network
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00022 SWS_ANM_00085}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00500
    /// @needwork = dda
    /// @endcode
    NmOperCode RequestNetwork(bool const requestEther = false, std::uint16_t const pnRequestInex = 0U) noexcept;

    /// @brief release network.
    /// @param requestEther true logic network request network, otherwise false
    /// @param pnRequestInex partial network request network
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00501
    /// @needwork = dda
    /// @endcode
    NmOperCode ReleaseNetwork(bool const requestEther = false, std::uint16_t const pnRequestInex = 0U) noexcept;

    /// @brief process of received nm message.
    /// @param nodeId message buffer
    /// @param repeatMessageBitIndication buffer length
    /// @param passiveValidPnGroup buffer length
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00008 SWS_ANM_00019 SWS_ANM_00025 SWS_ANM_00051
    /// SWS_ANM_00081 SWS_ANM_00089 SWS_ANM_00040 SWS_ANM_00055 SWS_ANM_00085}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00502
    /// @needwork = dda
    /// @endcode
    NmOperCode RecvNmMessage(std::uint8_t const nodeId,
                             bool const repeatMessageBitIndication,
                             ara::core::Vector< std::uint16_t > const &passiveValidPnGroup) noexcept;

    /// @brief add mode observer.
    /// @param modeChangeHandler mode change callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00503
    /// @needwork = dda
    /// @endcode
    void RegistModeObserverHandler(EtherComModeHandler const &modeChangeHandler) noexcept
    {
        etherModObservGroup_.emplace_back(modeChangeHandler);
    };

    /// @brief add pn observer.
    /// @param pnId pn id
    /// @param pnActHandler callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00504
    /// @needwork = dda
    /// @endcode
    void RegistPnObserverHandler(std::uint16_t const pnId, PnActiveHandler const &pnActHandler) noexcept
    {
        pnObservGroup_[pnId] = pnActHandler;
    };

    /// @brief content initialization.
    /// @param connectorName  ethernet communicator name
    /// @param pConfigRefTmp      json configure reference
    /// @returns               0 ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00505
    /// @needwork = dda
    /// @endcode
    std::int32_t Init(ara::core::String const &connectorName,
                      std::shared_ptr< Configure > const &pConfigRefTmp) noexcept;

    /// @brief Node detection. Node detection returns to repeat message, after detection, returns to original state
    /// @param lnName LN name
    /// @param pnId partial network id
    /// @param callBack Result callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00506
    /// @needwork = dda
    /// @endcode
    void RequestDetectNode(ara::core::String const &lnName,
                           std::uint16_t const pnId,
                           DetectNodeCallBack const &callBack) noexcept;

    /// @brief Low-level wakeup callback, passive wakeup
    /// Need to accelerate sending the first frame, i.e., enter repeat message state upon packet reception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00507
    /// @needwork = dda
    /// @endcode
    void NotifyWakeup() noexcept;

    /// @brief  Interface set state machine state (not supposed to be set normally)
    /// @param desType State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00508
    /// @needwork = dda
    /// @endcode
    void SetEtherStateMachineSate(EthernetStateMachineStateType const desType) noexcept;

    /// @brief  Get IP address configuration
    /// @returns IP address configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00509
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetIpAddress() const noexcept { return pEtherUdpNmconfig_->GetiPv4Address(); };

    /// @brief  Query state machine state
    /// @returns State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00510
    /// @needwork = dda
    /// @endcode
    EthernetStateMachineStateType const &GetEtherStateMachineState() const noexcept { return stateType_; };

    /// @brief
    /// Notify adapter promptly of interface state machine changes (adapter may need to interact with driver to respond to bus_sleep)
    /// @param modeChangeHandler mode change callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00511
    /// @needwork = dda
    /// @endcode
    void RegistStateObserverHandler(EtherStateChangeHandler const &modeChangeHandler) noexcept
    {
        etherStateObservGroup_.emplace_back(modeChangeHandler);
    };

    /// @brief Set NM message send/receive control
    /// @param ctrType Send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00512
    /// @needwork = dda
    /// @endcode
    void SetMessageCtrlType(ara::nm::MessageCtrlType const &ctrType) noexcept;

    /// @brief Get NM message send/receive control
    /// @returns Send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00513
    /// @needwork = dda
    /// @endcode
    ara::nm::MessageCtrlType GetMessageCtrlType() const noexcept { return ctrType_; };

    /// @brief Whether there is a DTC error
    /// @returns Whether there is a DTC error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00514
    /// @needwork = dda
    /// @endcode
    bool HasDtcError() const noexcept;

    /// @brief add pn request observer.
    /// @param obserHandler pn request callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00515
    /// @needwork = dda
    /// @endcode
    void RegistPnRequestObserverHandler(PnInformHandler const &obserHandler) noexcept
    {
        pnRequestObserver_.emplace_back(obserHandler);
    };

    /// @brief add pn release observer.
    /// @param obserHandler pn release callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00516
    /// @needwork = dda
    /// @endcode
    void RegistPnReleaseObserverHandler(PnInformHandler const &obserHandler) noexcept
    {
        pnReleaseObserver_.emplace_back(obserHandler);
    };

    /// @brief Get NM message send/receive control
    /// @returns Send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00517
    /// @needwork = dda
    /// @endcode
    ara::core::String GetPassivePnRequestList() const noexcept;

private:
    /// @brief Check if the NIC exists and initialize the socket,
    /// If the NIC is link-down, try to initialize the socket again on the next active request.
    /// @returns  kNmOperOK ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00009 SWS_ANM_00033 SWS_ANM_00034 SWS_ANM_00035
    /// SWS_ANM_00037 SWS_ANM_00038 SWS_ANM_00071 SWS_ANM_00091 SWS_ANM_00040}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00518
    /// @needwork = dda
    /// @endcode
    std::int32_t _checkCardAndInitSocket() noexcept;

    /// @brief send nm message.
    /// @returns  kNmOperOK ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00009 SWS_ANM_00033 SWS_ANM_00034 SWS_ANM_00035
    /// SWS_ANM_00037 SWS_ANM_00038 SWS_ANM_00071 SWS_ANM_00091 SWS_ANM_00040}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00519
    /// @needwork = dda
    /// @endcode
    NmOperCode _sendNmMessage() noexcept;

    /// @brief multi-network requests, force to enter repeate message in order to
    /// send nm message immediately
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00011 SWS_ANM_00013 SWS_ANM_00092 SWS_ANM_00044
    /// SWS_ANM_00094}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00520
    /// @needwork = dda
    /// @endcode
    void _multipleNetworkRequestsForceToRepeateMessage() noexcept;

    /// @brief from bus-sleep mode to repeate message state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00006 SWS_ANM_00007 SWS_ANM_00011 SWS_ANM_00013
    /// SWS_ANM_00070 SWS_ANM_00044 SWS_ANM_00094}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00521
    /// @needwork = dda
    /// @endcode
    void _busSleepToRepeateMessage() noexcept;

    /// @brief from prepare-bus-sleep mode to repeate message state in network
    /// mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00007 SWS_ANM_00011 SWS_ANM_00013 SWS_ANM_00070
    /// SWS_ANM_00044 SWS_ANM_00094}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00522
    /// @needwork = dda
    /// @endcode
    void _prepareBusSleepToRepeateMessage() noexcept;

    /// @brief from bus-sleep mode to repeate message state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00006 SWS_ANM_00007 SWS_ANM_00011 SWS_ANM_00013
    /// SWS_ANM_00070 SWS_ANM_00044 SWS_ANM_00094}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00523
    /// @needwork = dda
    /// @endcode
    void _wakeupCallToRepeateMessage() noexcept;

    /// @brief request network: from ready-sleep state in network mode to normal
    /// operation state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00016}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00524
    /// @needwork = dda
    /// @endcode
    void _readySleepToNormalOperation() noexcept;

    /// @brief request release: from normal operation in network mode to
    /// ready-sleep state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00018}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00525
    /// @needwork = dda
    /// @endcode
    void _normalOperationToReadySleep() noexcept;

    /// @brief process of repeate message timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00526
    /// @needwork = dda
    /// @endcode
    void _repeateMessageTimerCalled() noexcept;

    /// @brief process of perodic nm-message transmission
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00046 SWS_ANM_00047 SWS_ANM_00020}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00527
    /// @needwork = dda
    /// @endcode
    void _msgCycleTimerCalled() noexcept;

    /// @brief process of nm-message offset
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00044}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00528
    /// @needwork = dda
    /// @endcode
    void _msgCycleOffsetTimerCalled() noexcept;

    /// @brief process of nm-message timeout
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00012 SWS_ANM_00017 SWS_ANM_00021}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00529
    /// @needwork = dda
    /// @endcode
    void _networkTimerCalled() noexcept;

    /// @brief process of wait bus sleep timer in prepare-bus-sleep mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00530
    /// @needwork = dda
    /// @endcode
    void _waitBusSleepTimerCalled() noexcept;

    /// @brief process of immediate nm transmissions timer in repeate message
    /// state in network mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00531
    /// @needwork = dda
    /// @endcode
    void _immediateNmTransmissionsTimerCalled() noexcept;

    /// @brief from prepare bus sleep mode to bus sleep mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00028}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00532
    /// @needwork = dda
    /// @endcode
    void _prepareBusSleepToBusSleep() noexcept;

    /// @brief repeate message timer expires, if request network, from repeate
    /// message state in network mode to normal operation state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00014}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00533
    /// @needwork = dda
    /// @endcode
    void _repeateMessageToNormalOperation() noexcept;

    /// @brief repeate message timer expires, if request release, from repeate
    /// message state in network mode to normal operation state in network mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00015}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00534
    /// @needwork = dda
    /// @endcode
    void _repeateMessageToReadySleep() noexcept;

    /// @brief nm-message timeout, from ready sleep state in network mode to
    /// prepare bus sleep mode
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00024}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00535
    /// @needwork = dda
    /// @endcode
    void _readySleepToPrepareBusSleep() noexcept;

    /// @brief from normal operation or ready sleep to repeate message
    /// state(repeatMessageRequest)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00536
    /// @needwork = dda
    /// @endcode
    void _reptMSgReqToRepeateMessage() noexcept;

    /// @brief Notify upper layer of node detection result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00537
    /// @needwork = dda
    /// @endcode
    void _notifyDetectNodeList() noexcept;

    /// @brief IPC registration callback initialization.
    /// @returns  0 ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00538
    /// @needwork = dda
    /// @endcode
    std::int32_t _initIpcRegister() noexcept;

    /// @brief Start ImmediateNmTransmissionsTimer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00539
    /// @needwork = dda
    /// @endcode
    void _makeImmediateNmTransmissionsTimer() noexcept;

    /// @brief Start MsgCycleTimer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00540
    /// @needwork = dda
    /// @endcode
    void _makeMsgCycleTimer() noexcept;

    /// @brief Start MsgCycleOffsetTimer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00541
    /// @needwork = dda
    /// @endcode
    void _makeMsgCycleOffsetTimer() noexcept;

    /// @brief Start RepeateMessageTimer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00542
    /// @needwork = dda
    /// @endcode
    void _makeRepeateMessageTimer() noexcept;

    /// @brief Start NetworkTimer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00543
    /// @needwork = dda
    /// @endcode
    void _makeNetworkTimer() noexcept;

    /// @brief Start WaitBusSleepTimer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00544
    /// @needwork = dda
    /// @endcode
    void _makeWaitBusSleepTimer() noexcept;

private:
    /// @brief check the partial network has request network or not.
    /// @param pnId partial network id
    /// @returns true requested
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00545
    /// @needwork = dda
    /// @endcode
    bool _hasQuestPn(std::uint16_t const pnId) noexcept;

    /// @brief check the partial network has request network or not.
    /// @param pnId partial network id
    /// @returns true requested
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00546
    /// @needwork = dda
    /// @endcode
    void _eraseQuestPn(std::uint16_t const pnId) noexcept;

    /// @brief check the partial network has configed the ethernet communicator or
    /// not.
    /// @param pnId partial network id
    /// @returns true configed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00547
    /// @needwork = dda
    /// @endcode
    bool _hasConfigPn(std::uint16_t const pnId) noexcept;

    /// @brief check the ethernet communicator has been reqeust network
    /// proactively by ln or pn.
    /// @returns true reqeusted proactively
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00548
    /// @needwork = dda
    /// @endcode
    bool _checkActiveRequested() const noexcept;

    /// @brief print state information for debug.
    ///
    /// @param destMode mode type
    /// @param destStateType state type
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00549
    /// @needwork = dda
    /// @endcode
    void _printDebugInfo(ESMModeType const destMode, EthernetStateMachineStateType const destStateType) const noexcept;

    /// @brief in order to notify pn or ln the change of ethernet communication
    /// type full-com no-com.
    /// @param modeType mode type
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00004}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00550
    /// @needwork = dda
    /// @endcode
    void _setMode(ESMModeType const modeType) noexcept;

    /// @brief set state machine state type.
    /// @param stateType state type
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00005}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00551
    /// @needwork = dda
    /// @endcode
    void _setNetworkStateType(EthernetStateMachineStateType const stateType) noexcept;

    /// @brief notify pn is active.
    /// @param validPns pn group.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00552
    /// @needwork = dda
    /// @endcode
    void _updatePnMsg(ara::core::Vector< std::uint16_t > const &validPns) noexcept;

    /// @brief state information for debug.
    /// @returns debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00553
    /// @needwork = dda
    /// @endcode
    ara::core::String _getCurrentModeStateInfo() const noexcept;

    /// @brief  Interface set state machine state: repeat message state (not supposed to be set normally).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00554
    /// @needwork = dda
    /// @endcode
    void _forceRepeatMessageState() noexcept;

    /// @brief  Interface set state machine state: normal operation (not supposed to be set normally).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00555
    /// @needwork = dda
    /// @endcode
    void _forceNormalOperState() noexcept;

    /// @brief  Interface set state machine state: sleep ready (not supposed to be set normally).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00556
    /// @needwork = dda
    /// @endcode
    void _forceReadySleep() noexcept;

    /// @brief  Interface set state machine state: prepare bus sleep mode (not supposed to be set normally).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00557
    /// @needwork = dda
    /// @endcode
    void _forcePrepareSleepMode() noexcept;

    /// @brief  Interface set state machine state: bus sleep mode (not supposed to be set normally).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00558
    /// @needwork = dda
    /// @endcode
    void _forceBusSleepMode() noexcept;

    /// @brief Set NMState in user data.
    /// @param destType  Target state to set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00559
    /// @needwork = dda
    /// @endcode
    void _setuserDataNMState(EthernetStateMachineStateType const destType) noexcept;

    /// @brief Check first PN notification
    /// @param pnGroup PN set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00560
    /// @needwork = dda
    /// @endcode
    void _checkPassivePnRequest(ara::core::Vector< std::uint16_t > const &pnGroup) noexcept;

    /// @brief process of passive pn check timeout
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00561
    /// @needwork = dda
    /// @endcode
    void _passivePnCheckTimerCalled() noexcept;

private:
    /// @brief connectorName_
    /// ethernet communication name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00562
    /// @needwork = dda
    /// @endcode
    ara::core::String connectorName_{};

    /// @brief comState_
    /// ethernet state machine communication state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00563
    /// @needwork = dda
    /// @endcode
    NetworkStateType comState_{NetworkStateType::kNoCom};

    /// @brief modeType_
    /// ethernet state machine mode type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00564
    /// @needwork = dda
    /// @endcode
    ESMModeType modeType_{ESMModeType::kModeBusSleep};

    /// @brief stateType_
    /// ethernet state machine state type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00565
    /// @needwork = dda
    /// @endcode
    EthernetStateMachineStateType stateType_{EthernetStateMachineStateType::kStateBusSleep};

    /// @brief nmImmediateNmTransmissionsLeft_
    /// count left of sending nm immediate message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00566
    /// @needwork = dda
    /// @endcode
    std::uint32_t nmImmediateNmTransmissionsLeft_{0U};

    /// @brief configPnGroup_
    /// collection of configured pn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00567
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint16_t > configPnGroup_{};

    /// @brief requestPnGroup_
    /// collection of requested pn
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00568
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint16_t > requestPnGroup_{};

    /// @brief requestEtherCount_
    /// count of requested ln
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00569
    /// @needwork = dda
    /// @endcode
    std::uint32_t requestEtherCount_{0U};

    /// @brief pEtherUdpNmconfig_
    /// pointer of configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00570
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig const *pEtherUdpNmconfig_{nullptr};

    /// @brief initConfig_
    /// flag of initialization
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00571
    /// @needwork = dda
    /// @endcode
    bool initConfig_{false};

    /// @brief mainLoop_
    /// pointer of event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00572
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief networkTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00573
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr networkTimerRef_{nullptr};

    /// @brief nmMsgCycleOffsetTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00574
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr nmMsgCycleOffsetTimerRef_{nullptr};

    /// @brief msgCycleTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00575
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr msgCycleTimerRef_{nullptr};

    /// @brief repeateMessageTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00576
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr repeateMessageTimerRef_{nullptr};

    /// @brief immediateNmTransmissionsTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00577
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr immediateNmTransmissionsTimerRef_{nullptr};

    /// @brief waitBusSleepTimerRef_
    /// pointer of event timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00578
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr waitBusSleepTimerRef_{nullptr};

    /// @brief etherModObservGroup_
    /// Set of NIC mode changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00579
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherComModeHandler > etherModObservGroup_{};

    /// @brief pnObservGroup_
    /// PN callback set for NIC affiliation, notifies PN when PN messages are received or sent
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00580
    /// @needwork = dda
    /// @endcode
    ara::core::Map< std::uint16_t, PnActiveHandler > pnObservGroup_{};

    /// @brief pNmMsg_
    /// pointer of ethernet socket handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00581
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< NmMsg > pNmMsg_{nullptr};

    /// @brief detectNode_
    /// Whether to start node detection. Once started, node information is collected and reported to the upper layer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00582
    /// @needwork = dda
    /// @endcode
    bool detectNode_{false};

    /// @brief detectNodePn_
    /// When there is a PN, collect node information. Which PN ID requested, result returned the same way
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00583
    /// @needwork = dda
    /// @endcode
    std::uint16_t detectNodePn_{kInvalidPnID};

    /// @brief detectNodeLn_
    /// When LN directly contains the NIC, the name of the LN. detectNodePn_ and detectNodeLn_ are mutually exclusive
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00584
    /// @needwork = dda
    /// @endcode
    ara::core::String detectNodeLn_{};

    /// @brief nodeList_
    /// Whether to start node detection. Once started, node information is collected and reported to the upper layer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00585
    /// @needwork = dda
    /// @endcode
    ara::core::Map< std::uint8_t, bool > nodeList_{};

    /// @brief wakeUp_
    /// Whether it has already woken up, to prevent notifying ether multiple times when there is a PN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00586
    /// @needwork = dda
    /// @endcode
    bool wakeUp_{false};

    /// @brief detctNodCall_
    /// Node detection result notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00587
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< DetectNodeCallBack > detctNodCall_{};

    /// @brief etherStateObservGroup_
    /// Set of NIC state machine state changes that are of interest, callback set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00588
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherStateChangeHandler > etherStateObservGroup_{};

    /// @brief pConfigRef_
    /// Configuration pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00589
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< Configure > pConfigRef_{nullptr};

    /// @brief activeWakeUpNet_
    /// Whether to actively wake up the network. When prepare/bus_sleep to
    /// repeat, this is an active request for the network, set this bit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00590
    /// @needwork = dda
    /// @endcode
    bool activeWakeUpNet_{false};

    /// @brief userDataNMState_
    /// When there is a PN, fill NMstate in user data. Specific values are defined in requirements
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00591
    /// @needwork = dda
    /// @endcode
    std::uint8_t userDataNMState_{0};

    /// @brief supportSleepMode_
    /// Whether the NIC supports sleep mode. If supported, when entering sleep mode, the related interface in the SO will be called
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00592
    /// @needwork = dda
    /// @endcode
    bool supportSleepMode_{false};

    /// @brief ctrType_
    /// Control NM message sending and receiving, related to UDS 0x28 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00593
    /// @needwork = dda
    /// @endcode
    ara::nm::MessageCtrlType ctrType_{ara::nm::MessageCtrlType::kEnableAll};

    /// @brief dtcError_
    /// DTC error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00594
    /// @needwork = dda
    /// @endcode
    bool dtcError_{false};

    /// @brief passivePnGroup_
    /// Passive request PN set. When first received or timeout without reception, it will notify the user
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00595
    /// @needwork = dda
    /// @endcode
    ara::core::Map< std::uint16_t, std::uint32_t > passivePnGroup_;

    /// @brief passivePnCheckRef_
    /// Timer, for detecting passive PN timeout
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00596
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr passivePnCheckRef_;

    /// @brief pnRequestObserver_
    /// PN request
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00597
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnInformHandler > pnRequestObserver_{};

    /// @brief pnReleaseObserver_
    /// PN release
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00598
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnInformHandler > pnReleaseObserver_{};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_NMETHERSTATEMACHINE_H_