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
/// @file       nm_network_state_control_server.h
/// @brief      A implementation of ara::sm::nm_comm::NMNetworkStateControlServer.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/NMComm
/// @unit_name=NMNetworkStateControlServer
/// @interface_level=uint
/// @unit_description=A implementation of ara::sm::nm_comm::NMNetworkStateControlServer.
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @endcode
///
/// ================================================================

#ifndef NM_NETWORK_STATE_CONTROL_SERVER_H_
#define NM_NETWORK_STATE_CONTROL_SERVER_H_

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#ifdef ARA_ENABLE_NM_USING_COM
    #include <ara/nm/networkstate_proxy.h>
#else
    #include <ara/nm/network_handle.h>
#endif  // DEBUG

#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "event.h"

namespace ara {
namespace sm {
namespace nm_comm {
/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#ifdef ARA_ENABLE_NM_USING_COM
using NetworkStateProxy = nm::proxy::NetworkStateProxy;
#else
using NetworkStateProxy = nm::NetworkHandle;
#endif

/// @brief a implementation of IMsControlServer for NM network control
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_08026
/// @trace_id_dd=DD_SM_08034
/// @needwork = ad
/// @endcode
class NMNetworkStateControlServer
{
public:
    /// @brief Constructor
    /// @param nmHandleName Network handle
    /// @param instanceId Instance identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00001
    /// @trace_id_dd=DD_SM_00001
    /// @needwork = ad
    /// @endcode
    NMNetworkStateControlServer(core::String nmHandleName, core::String instanceId) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00002
    /// @trace_id_dd=DD_SM_00002
    /// @needwork = ad
    /// @endcode
    ~NMNetworkStateControlServer() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The NMNetworkStateControlServer instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00003
    /// @trace_id_dd=DD_SM_00003
    /// @needwork = ad
    /// @endcode
    NMNetworkStateControlServer(NMNetworkStateControlServer const &other) = delete;

    /// @brief Move constructor function
    /// @param other The NMNetworkStateControlServer instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00004
    /// @trace_id_dd=DD_SM_00004
    /// @needwork = ad
    /// @endcode
    NMNetworkStateControlServer(NMNetworkStateControlServer &&other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The NMNetworkStateControlServer instance to be copyed
    /// @return the assigned NMNetworkStateControlServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00005
    /// @trace_id_dd=DD_SM_00005
    /// @needwork = ad
    /// @endcode
    NMNetworkStateControlServer &operator=(NMNetworkStateControlServer const &other) = delete;

    /// @brief Move assignment function
    /// @param other The NMNetworkStateControlServer instance to be moved
    /// @return the assigned NMNetworkStateControlServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00006
    /// @trace_id_dd=DD_SM_00006
    /// @needwork = ad
    /// @endcode
    NMNetworkStateControlServer &operator=(NMNetworkStateControlServer &&other) = delete;

    /// @brief Start accepting UCM requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00008
    /// @trace_id_dd=DD_SM_00008
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept;

    /// @brief Stop accepting UCM requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00009
    /// @trace_id_dd=DD_SM_00009
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

    /// @brief Set NetworkRequestedState to the specified state
    /// @param requestedState
    /// @return Setting result, indicating that the setting request has been received
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00010
    /// @trace_id_dd=DD_SM_00010
    /// @needwork = ad
    /// @endcode
    core::Future< void > AsyncSetNetworkRequestedState(common::NetworkStateInternalType const &requestedState) noexcept;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00011
    /// @trace_id_dd=DD_SM_00011
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

private:
    /// @brief Subscribe to field: NetworkCurrentState, and update its value to networkCurrentState_
    /// @param networkCurrentStateChangeHandler Network state change callback function
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08035
    /// @needwork = dda
    /// @endcode
    bool _subscribeNetworkCurrentState() noexcept;

    /// @brief Called when network state is updated
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08036
    /// @needwork = dda
    /// @endcode
    void _onNetworkCurrentStateChange() noexcept;

    void _onNetworkCurrentStateChange(nm::NetworkStateType const &currentState) noexcept;

    /// @brief  Forward NetworkCurrentState change request
    /// @param nmHandleName Network name
    /// @param newState Network state
    /// @param promise Request promise
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08037
    /// @needwork = dda
    /// @endcode
    void _deliverNetworkCurrentStateChangeRequest(core::String const &nmHandleName,
                                                  common::NetworkStateInternalType const &newState,
                                                  core::Promise< void > &&promise) const noexcept;

    /// @brief Wait for network current state
    /// @param state Requested network state
    /// @param promise Used to set the result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08038
    /// @needwork = dda
    /// @endcode
    void _waitForNetworkCurrentState(common::NetworkStateInternalType const &state,
                                     std::shared_ptr< core::Promise< void > > const &promise);

    void _createWaitDoneThreads(common::NetworkStateInternalType const &state,
                                std::shared_ptr< core::Promise< void > > promise);

    /// @brief Convert SM internal network state to NM network state
    /// @param state SM internal network state
    /// @return NM network state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08039
    /// @needwork = dda
    /// @endcode
    nm::NetworkStateType _toNmType(common::NetworkStateInternalType const &state) const noexcept;

    /// @brief Convert NM network state to SM internal network state
    /// @param state NM network state
    /// @return SM internal network state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08040
    /// @needwork = dda
    /// @endcode
    common::NetworkStateInternalType _toInternalType(nm::NetworkStateType const &state) const noexcept;

    bool _isServerReady() const noexcept;

    /// @brief Name of nmHandle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08041
    /// @needwork = dda
    /// @endcode
    core::String nmHandleName_;

    /// @brief id identifier
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08042
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceId_;

#ifdef ARA_ENABLE_NM_USING_COM
    /// @brief Handle for finding NetworkState service
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08043
    /// @needwork = dda
    /// @endcode
    ara::com::FindServiceHandle findHandle_;
#endif
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08044
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;

    /// @brief Whether NetworkState service has been found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08045
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > foundService_{false};

    /// @brief State proxy
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08046
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< NetworkStateProxy > networkStateProxy_{nullptr};

    /// @brief Current network state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08047
    /// @needwork = dda
    /// @endcode
    common::NetworkStateInternalType currentState_{common::NetworkStateInternalType::kNoCom};

    /// @brief Network state condition variable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08048
    /// @needwork = dda
    /// @endcode
    std::condition_variable stateCond_;

    /// @brief Mutex to protect currentState_ from race conditions.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08049
    /// @needwork = dda
    /// @endcode
    std::mutex stateMutex_;

    /// @brief Mapping of threads listening to NM network state changes, key is thread ID, value is thread object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08050
    /// @needwork = dda
    /// @endcode
    core::Map< std::thread::id, std::thread > waitChangeWorkingThreads_;

    /// @brief Mutex to protect waitChangeDoneThreadIDs_ from race conditions.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08051
    /// @needwork = dda
    /// @endcode
    std::mutex waitChangeDoneThreadIDsMutex_;

    /// @brief If the network state monitored by this thread has changed, store the thread number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08052
    /// @needwork = dda
    /// @endcode
    core::Vector< std::thread::id > waitChangeDoneThreadIDs_;

    /// @brief This lock is used to prevent the proxy from being created repeatedly
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08053
    /// @needwork = dda
    /// @endcode
    std::mutex serviceMutex_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08054
    /// @needwork = dda
    /// @endcode
    ara::log::Logger const &log_{
        ara::log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"NMNetworkStateControlServer"}))};

    // When using ipc communication, to ensure that the call does not block, use multi-threading for switching network states
    core::Map< std::thread::id, std::thread > doChangeWorkingThreads_;

    std::mutex doChangeDoneThreadIDsMutex_;

    core::Vector< std::thread::id > doChangeDoneThreadIDs_;

    std::atomic< bool > subscribedOK_{false};

    std::thread subscribedThread_;
};

}  // namespace nm_comm
}  // namespace sm
}  // namespace ara

#endif  // NM_NETWORK_STATE_CONTROL_SERVER_H_
