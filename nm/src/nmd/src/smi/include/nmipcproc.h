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
/// @file       nmipcproc.h
/// @brief      IPC communication management
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
/// @unit_name=NMIpcProc
/// @unit_description=IPC communication management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_NMIPCPROC_H_
#define _ARA_NM_NMIPCPROC_H_

#include <ara/core/map.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>

#include <cstdint>
#include <memory>
#include <string>

#include "common/ipccommand.h"
#include "ipchandlers.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Command sending, received command is called back externally
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100098
/// @trace_id_dd=DD_NM_00842
/// @needwork = ad
/// @endcode
class NMIpcProc final
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00643
    /// @needwork = dda
    /// @endcode
    NMIpcProc() = default;

    /// @brief Copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00644
    /// @needwork = dda
    /// @endcode
    NMIpcProc(NMIpcProc const &other) = delete;

    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00645
    /// @needwork = dda
    /// @endcode
    NMIpcProc &operator=(NMIpcProc &other) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00646
    /// @needwork = dda
    /// @endcode
    ~NMIpcProc() noexcept;

    /// @brief Move constructor
    /// @param other Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00647
    /// @needwork = dda
    /// @endcode
    NMIpcProc(NMIpcProc &&other) noexcept = default;

    /// @brief Move assignment
    /// @param other
    /// @return Reference
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00648
    /// @needwork = dda
    /// @endcode
    NMIpcProc &operator=(NMIpcProc &&other) &noexcept = delete;

    /// @brief Get IPC pointer
    /// @return IPC pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00649
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< NMIpcProc > GetNmIpcProHandler() noexcept
    {
        if (nullptr == s_IpcServerHandler_) {
            s_IpcServerHandler_ = std::make_shared< NMIpcProc >();
            std::ignore         = s_IpcServerHandler_->_init();
        }
        return s_IpcServerHandler_;
    }

    /// @brief Close IPC pointer
    /// @return IPC pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00650
    /// @needwork = dda
    /// @endcode
    static void CloseNmIpcProHandler() noexcept
    {
        if (nullptr != s_IpcServerHandler_) {
            if (1 == s_IpcServerHandler_.use_count()) {
                s_IpcServerHandler_ = nullptr;
            }
        }
    }

    /// @brief Register LN instance
    /// @param lnName LN instance
    /// @param handle Callback structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00651
    /// @needwork = dda
    /// @endcode
    void RegLnProcHandle(ara::core::String const &lnName, IpcHandlers const &handle) noexcept
    {
        lnProcMap_[lnName] = handle;
    };

    /// @brief Register Ethernet instance
    /// @param ipAddress Ethernet instance address
    /// @param handle Callback structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00652
    /// @needwork = dda
    /// @endcode
    void RegEtherProcHandle(ara::core::String const &ipAddress, IpcHandlers const &handle) noexcept
    {
        etherProcMap_[ipAddress] = handle;
    };

    /// @brief Update network state
    /// @param lnName LN instance
    /// @param currentState Network state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00653
    /// @needwork = dda
    /// @endcode
    void UpdateNetworkCurrentState(ara::core::String const &lnName, std::uint32_t const currentState) noexcept;

    /// @brief Update network request state
    /// @param lnName LN instance
    /// @param currentState Network state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00654
    /// @needwork = dda
    /// @endcode
    void UpdateNetworkRequestState(ara::core::String const &lnName, std::uint32_t const currentState) noexcept;

    /// @brief Notify node detection result
    /// @param ipAddress Ethernet instance address
    /// @param nodeList Node set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00655
    /// @needwork = dda
    /// @endcode
    void NotifyPresentNodeList(ara::core::String const &ipAddress, ara::core::String const &nodeList) noexcept;

    /// @brief Notify state machine state change
    /// @param ipAddress Ethernet instance address
    /// @param currentState State
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00656
    /// @needwork = dda
    /// @endcode
    void NotifyEtherStateChanged(ara::core::String const &ipAddress, std::uint32_t const currentState) noexcept;

    /// @brief Notify receipt of external PN request
    /// @param ipAddress Ethernet instance address
    /// @param pnList PN set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00657
    /// @needwork = dda
    /// @endcode
    void NotifyExternalPnRequest(ara::core::String const &ipAddress, ara::core::String const &pnList) noexcept;

    /// @brief Notify receipt of external PN release
    /// @param ipAddress Ethernet instance address
    /// @param pnList PN set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00658
    /// @needwork = dda
    /// @endcode
    void NotifyExternalPnRelease(ara::core::String const &ipAddress, ara::core::String const &pnList) noexcept;

private:
    /// @brief Initialize
    /// @return 0 ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00659
    /// @needwork = dda
    /// @endcode
    std::int32_t _init() noexcept;

    /// @brief Handle client request
    /// @param jsonstr Request
    /// @param sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00660
    /// @needwork = dda
    /// @endcode
    void _handleLibCmd(ara::core::String const &jsonstr, std::uint64_t const &sessionId) noexcept;

    /// @brief IPC request callback function, called when a request is received from a client.
    /// @param  context - User parameter
    /// @param  htype - Handle type
    /// @param  reqPacket - Data packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00661
    /// @needwork = dda
    /// @endcode
    void _ipcServerRequestHandler(void *const context,
                                  isoft::ipc::IPCServerHandleType const &htype,
                                  isoft::ipc::IPCPacket *const reqPacket) noexcept;

    /// @brief IPC service connection callback function, called when a client establishes or disconnects.
    /// @param  context - User parameter
    /// @param  type - Type
    /// @param  packet - Data packet
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00662
    /// @needwork = dda
    /// @endcode
    void _ipcServerConnectionHandler(void *const context,
                                     isoft::ipc::IPCServerHandleType const &type,
                                     isoft::ipc::IPCPacket *const packet);

private:
    /// @brief Send data
    /// @param  sessionid  Session ID
    /// @param  dataBuffer  Data pointer
    /// @param  messageSize Data length
    /// @return 0 if send succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00663
    /// @needwork = dda
    /// @endcode
    std::int32_t _send(std::uint64_t const &sessionid,
                       char const *const dataBuffer,
                       std::size_t const &messageSize) const noexcept;

    /// @brief Client offline, delete client session
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00664
    /// @needwork = dda
    /// @endcode
    void _delAAClient(std::uint64_t const &sessionId) noexcept;

    /// @brief Client gets logical network request state
    /// @param  lnName Logical network instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00665
    /// @needwork = dda
    /// @endcode
    void _getNetworkRequestState(ara::core::String const &lnName, std::uint64_t const &sessionId) noexcept;

    /// @brief Client sets logical network request state
    /// @param  lnName Logical network instance
    /// @param  sessionId ipc session
    /// @param  requestState Request state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00666
    /// @needwork = dda
    /// @endcode
    void _setNetworkRequestState(ara::core::String const &lnName,
                                 std::uint64_t const &sessionId,
                                 ara::nm::NetworkStateType const requestState) noexcept;

    /// @brief Client gets logical network state
    /// @param  lnName Logical network instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00667
    /// @needwork = dda
    /// @endcode
    void _getNetworkCurrentState(ara::core::String const &lnName, std::uint64_t const &sessionId) noexcept;

    /// @brief Client gets Ethernet instance state machine state
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00668
    /// @needwork = dda
    /// @endcode
    void _getEtherState(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief Client sets Ethernet instance state machine state
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @param  requestState Request state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00669
    /// @needwork = dda
    /// @endcode
    void _setEtherState(ara::core::String const &ipAddr,
                        std::uint64_t const &sessionId,
                        ara::nm::NetworkStateType const requestState) noexcept;

    /// @brief Client requests node detection
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00670
    /// @needwork = dda
    /// @endcode
    void _requestDetectNode(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief Client notifies Ethernet instance wakeup
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00671
    /// @needwork = dda
    /// @endcode
    void _notifyWakeup(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief Client sets Ethernet instance NM message send/receive control
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @param  requestType Send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00672
    /// @needwork = dda
    /// @endcode
    void _setMessageCtrlType(ara::core::String const &ipAddr,
                             std::uint64_t const &sessionId,
                             ara::nm::MessageCtrlType const requestType) noexcept;

    /// @brief Client gets Ethernet instance NM message send/receive control
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00673
    /// @needwork = dda
    /// @endcode
    void _getMessageCtrlType(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief Client gets Ethernet instance initialization status
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00674
    /// @needwork = dda
    /// @endcode
    void _getInitState(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief Client sets Ethernet instance external PN request list
    /// @param  ipAddr Ethernet instance
    /// @param  sessionId ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00675
    /// @needwork = dda
    /// @endcode
    void _getExternalPnRequestList(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept;

    /// @brief  Delete session from set
    /// @param  vectorRef Set
    /// @param  session ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00676
    /// @needwork = dda
    /// @endcode
    static void DeleteSession(ara::core::Vector< std::uint64_t > &vectorRef, std::uint64_t const session) noexcept;

    /// @brief  Add session to set
    /// @param  vectorRef Set
    /// @param  session ipc session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00677
    /// @needwork = dda
    /// @endcode
    static void AddSession(ara::core::Vector< std::uint64_t > &vectorRef, std::uint64_t const session) noexcept;

private:
    /// @brief ipcServer_
    /// IPC server handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00678
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::ipc::IPCServer > ipcServer_{nullptr};

    /// @brief mainLoop_
    /// mainLoop_ handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00679
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief kDaemonName
    /// Service name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00680
    /// @needwork = dda
    /// @endcode
    ara::core::String const kDaemonName{"nmd"};

    /// @brief s_IpcServerHandler_
    /// Global static variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00843
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< NMIpcProc > s_IpcServerHandler_;

    /// @brief lnProcMap_
    /// LN set and callback registration for communication with clients
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00682
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, IpcHandlers > lnProcMap_{};

    /// @brief lnRegisterMap_
    /// Client session set interested in LN state changes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00683
    /// @needwork = dda
    /// @endcode
    ara::core::Map< IpcCommand, ara::core::Map< ara::core::String, ara::core::Vector< std::uint64_t > > >
        lnRegisterMap_{};

    /// @brief etherProcMap_
    /// Ethernet instance set and callback registration for communication with clients
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00684
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, IpcHandlers > etherProcMap_{};

    /// @brief ethRegisterMap_
    /// Client session set interested in Ethernet internal changes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00685
    /// @needwork = dda
    /// @endcode
    ara::core::Map< IpcCommand, ara::core::Map< ara::core::String, ara::core::Vector< std::uint64_t > > >
        ethRegisterMap_{};

    /// @brief LnMapIterator
    /// Logical network iterator declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00895
    /// @needwork = dda
    /// @endcode
    using LnMapIterator = ara::core::Map< ara::core::String, IpcHandlers >::iterator;

    /// @brief EtherMapIterator
    /// State machine instance iterator declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00896
    /// @needwork = dda
    /// @endcode
    using EtherMapIterator = ara::core::Map< ara::core::String, IpcHandlers >::iterator;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif