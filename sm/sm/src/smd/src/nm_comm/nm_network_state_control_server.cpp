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
/// @file       nm_network_state_control_server.cpp
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
/// @interface_level=uint
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @unit_name=NMNetworkStateControlServer
/// @unit_description=A implementation of ara::sm::nm_comm::NMNetworkStateControlServer.
/// @endcode
///
/// ================================================================

#include "nm_network_state_control_server.h"

#include <ara/com/types.h>

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>

#include "ara/sm/error_domain_sm.h"
#include "define.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace nm_comm {
using ara::sm::SMErrc;
/// @brief Constructor
/// @param nmHandleName Network handle
/// @param instanceId Instance identifier
NMNetworkStateControlServer::NMNetworkStateControlServer(core::String nmHandleName, core::String instanceId) noexcept
    : nmHandleName_{std::move(nmHandleName)}, instanceId_{std::move(instanceId)}
{
    log_.LogInfo() << "NMNetworkStateControlServer::NMNetworkStateControlServer(), nmHandleName_:"
                   << nmHandleName_.c_str() << "instanceId_:" << instanceId_.c_str();
}

/// @brief Destructor
NMNetworkStateControlServer::~NMNetworkStateControlServer() noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::~NMNetworkStateControlServer()";
}

/// @brief Start accepting messages
/// @return
bool NMNetworkStateControlServer::Start() noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::Start(), nmHandleName_:" << nmHandleName_.c_str()
                   << "try to StartFindService.";
    bool const ret{true};
#ifdef ARA_ENABLE_NM_USING_COM
    // Find service
    core::Promise< NetworkStateProxy > promise;  // stack var!!
    core::Future< NetworkStateProxy > const future{promise.get_future()};
    std::ignore = future;
    ara::core::Result< ara::com::FindServiceHandle > findHandleRes{NetworkStateProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer< NetworkStateProxy::HandleType > handles,
               ara::com::FindServiceHandle const &handler) noexcept {
            log_.LogDebug() << "NMNetworkStateControlServer::Start(), StartFindService CB called";
            if (handles.empty()) {
                return;
            }
            {
                std::lock_guard< std::mutex > const lock{serviceMutex_};
                if (networkStateProxy_ != nullptr) {
                    return;
                }
                networkStateProxy_
                    = std::make_unique< NetworkStateProxy >(NetworkStateProxy::Create(handles[0U]).Value());
            }

            // Subscribe to NetworkCurrentState
            std::thread t1{[this]() noexcept {
                log_.LogDebug() << "NMNetworkStateControlServer::Start(), before _subscribeNetworkCurrentState.";
                std::ignore = _subscribeNetworkCurrentState();
            }};
            t1.detach();

            // Found
            foundService_ = true;

            // Stop searching
            log_.LogDebug() << "NMNetworkStateControlServer::Start(), before StopFindService.";
            NetworkStateProxy::StopFindService(handler);
        },
        ara::com::InstanceIdentifier(instanceId_))};
    findHandle_ = std::move(findHandleRes).Value();
#else
    if (networkStateProxy_ == nullptr) {
        networkStateProxy_ = std::make_unique< NetworkStateProxy >(ara::core::InstanceSpecifier(instanceId_));
        log_.LogDebug() << "NMNetworkStateControlServer::Start(), _subscribeNetworkCurrentState by ipc.";
        std::ignore = _subscribeNetworkCurrentState();
    }
#endif  // ARA_ENABLE_NM_USING_COM

    log_.LogInfo() << "NMNetworkStateControlServer::Start(), end with ret:" << ret;
    return ret;
}

/// @brief Current subscribed network state
/// @return
bool NMNetworkStateControlServer::_subscribeNetworkCurrentState() noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::_subscribeNetworkCurrentState(), begin with nmHandleName_:"
                   << nmHandleName_.c_str();
#ifdef ARA_ENABLE_NM_USING_COM
    if (networkStateProxy_->NetworkCurrentState.IsSubscribed()) {
        log_.LogWarn()
            << "NMNetworkStateControlServer::_subscribeNetworkCurrentState(), NetworkCurrentState already subscribed";
        return true;
    }

    // Register event receive callback
    std::ignore = networkStateProxy_->NetworkCurrentState.SetReceiveHandler(
        [this]() noexcept { _onNetworkCurrentStateChange(); });

    // Subscribe to field
    // modified by hanzhibo: networkStateProxy_->NetworkCurrentState.Subscribe(1)
    std::ignore = networkStateProxy_->NetworkCurrentState.Subscribe(static_cast< size_t >(TEN_UL));
#else
    std::thread th{[this] {
        ara::core::Result< nm::NetworkStateType > getRes{nm::NetworkStateType::kNoCom};
        do {
            getRes = networkStateProxy_->GetNetworkState();
            std::this_thread::sleep_for(std::chrono::milliseconds(TEN_L));
        } while (!getRes.HasValue());
        _onNetworkCurrentStateChange(getRes.Value());
        auto registeRes = networkStateProxy_->RegisterNetworkStateChangeNotifier(
            [this](nm::NetworkStateType const &state) noexcept { _onNetworkCurrentStateChange(state); });

        if (registeRes.HasValue()) {
            subscribedOK_ = true;
        } else {
            log_.LogWarn() << "NMNetworkStateControlServer::_subscribeNetworkCurrentState(), "
                              "RegisterNetworkStateChangeNotifier failed:"
                           << registeRes.Error().Message();
        }
    }};
    th.detach();
    subscribedThread_.swap(th);

#endif
    return true;
}

/// @brief Network change
void NMNetworkStateControlServer::_onNetworkCurrentStateChange() noexcept
{
    log_.LogDebug() << "NMNetworkStateControlServer::_onNetworkCurrentStateChange(), start for nmHandleName_:"
                    << nmHandleName_.c_str();
#ifdef ARA_ENABLE_NM_USING_COM
    std::ignore = networkStateProxy_->NetworkCurrentState.GetNewSamples(
        [this](auto const &sample) {
            ara::sm::common::NetworkStateInternalType const newState{_toInternalType(*sample)};
            log_.LogDebug() << "NMNetworkStateControlServer::_onNetworkCurrentStateChange(), get a sample:"
                            << common::NetworkStateTypeToString(newState).data()
                            << "for nmHandleName_:" << nmHandleName_.c_str();
            // Forward NetworkCurrentState change request
            core::Promise< void > promise;
            bool deliver{false};
            {
                std::unique_lock< std::mutex > const lck{stateMutex_};
                if (newState != currentState_) {
                    deliver       = true;
                    currentState_ = newState;
                }
            }
            stateCond_.notify_all();
            if (deliver) {
                _deliverNetworkCurrentStateChangeRequest(nmHandleName_, newState, std::move(promise));
            }
        },
        1U);
#endif
}

/// @brief Network change
void NMNetworkStateControlServer::_onNetworkCurrentStateChange(nm::NetworkStateType const &currentState) noexcept
{
    log_.LogDebug() << "NMNetworkStateControlServer::_onNetworkCurrentStateChange(), nmHandleName_:"
                    << nmHandleName_.c_str()
                    << "currentState:" << common::NetworkStateTypeToString(_toInternalType(currentState)).data();
#ifndef ARA_ENABLE_NM_USING_COM
    // Forward NetworkCurrentState change request
    core::Promise< void > promise;
    bool deliver{false};
    {
        std::unique_lock< std::mutex > const lck{stateMutex_};
        if (currentState != _toNmType(currentState_)) {
            deliver       = true;
            currentState_ = _toInternalType(currentState);
        }
    }
    stateCond_.notify_all();
    if (deliver) {
        _deliverNetworkCurrentStateChangeRequest(nmHandleName_, currentState_, std::move(promise));
    }
#endif
}

/// @brief Forward network change request
/// @param nmHandleName
/// @param newState
/// @param promise
void NMNetworkStateControlServer::_deliverNetworkCurrentStateChangeRequest(
    core::String const &nmHandleName,
    common::NetworkStateInternalType const &newState,
    core::Promise< void > &&promise) const noexcept
{
    log_.LogInfo()
        << "NMNetworkStateControlServer::_deliverNetworkCurrentStateChangeRequest(), begin with nmHandleName,"
        << nmHandleName.c_str() << "newState:" << common::NetworkStateTypeToString(newState).data();
    if (appendEventHandler_) {
        // NM's NetworkCurrentStateChange request
        common::Event event;
        event.type = common::EventType::kInNMNetworkStateChange;

        // NMNetworkCurrentStateChangeInfo information
        common::NetworkStateInfo *const data{new common::NetworkStateInfo()};
        data->nmHandleName   = nmHandleName;
        data->state          = newState;
        event.data           = data;
        event.requestPromise = std::move(promise);
        appendEventHandler_(std::move(event));
    } else {
        log_.LogWarn() << "NMNetworkStateControlServer::_deliverNetworkCurrentStateChangeRequest(), "
                          "appendEventHandler_ is nullptr";
    }
}

/// @brief Convert SM internal network state to NM network state
/// @param state SM internal network state
/// @return NM network state
nm::NetworkStateType NMNetworkStateControlServer::_toNmType(
    common::NetworkStateInternalType const &state) const noexcept
{
    std::ignore = appendEventHandler_;
    nm::NetworkStateType networkStateType{nm::NetworkStateType::kNoCom};
    switch (state) {
        case common::NetworkStateInternalType::kFullCom: {
            networkStateType = nm::NetworkStateType::kFullCom;
        } break;
        case common::NetworkStateInternalType::kNoCom: {  // NOLINT
            networkStateType = nm::NetworkStateType::kNoCom;
        } break;
        default: {
            networkStateType = nm::NetworkStateType::kNoCom;
        } break;
    }
    return networkStateType;
}

/// @brief Convert NM network state to SM internal network state
/// @param state NM network state
/// @return SM internal network state
common::NetworkStateInternalType NMNetworkStateControlServer::_toInternalType(
    nm::NetworkStateType const &state) const noexcept
{
    std::ignore = appendEventHandler_;
    common::NetworkStateInternalType networkStateInternalType{common::NetworkStateInternalType::kNoCom};
    switch (state) {
        case nm::NetworkStateType::kFullCom: {
            networkStateInternalType = common::NetworkStateInternalType::kFullCom;
        } break;
        case nm::NetworkStateType::kNoCom: {  // NOLINT
            networkStateInternalType = common::NetworkStateInternalType::kNoCom;
        } break;
        default: {
            networkStateInternalType = common::NetworkStateInternalType::kNoCom;
        } break;
    }
    return networkStateInternalType;
}

bool NMNetworkStateControlServer::_isServerReady() const noexcept
{
#ifdef ARA_ENABLE_NM_USING_COM
    return foundService_.load(std::memory_order_acquire);
#else
    return subscribedOK_.load(std::memory_order_acquire);
#endif
}

/// @brief Stop
void NMNetworkStateControlServer::Stop() noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::Stop()";
// Not found yet, stop searching
#ifdef ARA_ENABLE_NM_USING_COM
    if (!foundService_.load(std::memory_order_acquire)) {
        log_.LogDebug() << "NMNetworkStateControlServer::Stop(), try to StopFindService.";
        NetworkStateProxy::StopFindService(findHandle_);
    }
#endif
    for (auto &pair : waitChangeWorkingThreads_) {
        pair.second.join();
    }
}

/// @brief Set the network request state
/// @param requestedState
/// @return
core::Future< void > NMNetworkStateControlServer::AsyncSetNetworkRequestedState(
    common::NetworkStateInternalType const &requestedState) noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState(), begin with requestedState:"
                   << common::NetworkStateTypeToString(requestedState).c_str() << " foundService_:" << foundService_;
    std::shared_ptr< core::Promise< void > > promise{std::make_shared< core::Promise< void > >()};

    if (!_isServerReady()) {
        core::Promise< void > errorPromise{};
        log_.LogWarn()
            << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState(), hasn't found NetworkStateServer yet.";
        errorPromise.SetError(SMErrc::kNotFoundNetworkStateService);
        return errorPromise.get_future();
    }
#ifdef ARA_ENABLE_NM_USING_COM
    std::shared_ptr< core::Future< nm::NetworkStateType > > fu{std::make_shared< core::Future< nm::NetworkStateType > >(
        networkStateProxy_->NetworkRequestedState.Set(_toNmType(requestedState)))};
    if (requestedState == currentState_) {
        promise->set_value();
        log_.LogDebug() << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState(), requestedState is equal to "
                           "currentState_, currentState_:"
                        << common::NetworkStateTypeToString(currentState_);
    } else {
        std::ignore = fu->then([this, fu, requestedState, promise]() mutable {
            core::Result< nm::NetworkStateType > const res{fu->GetResult()};
            if (res.HasValue()) {
                {
                    if (requestedState == currentState_) {
                        promise->set_value();
                        log_.LogDebug() << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState(), "
                                           "requestedState is equal to currentState_, currentState_:"
                                        << common::NetworkStateTypeToString(currentState_);
                    } else {
                        _createWaitDoneThreads(requestedState, promise);
                    }
                }
            } else {
                core::ErrorCode errorCode{res.Error()};
                promise->SetError(std::move(errorCode));
            }
            fu      = nullptr;
            promise = nullptr;
        });
    }
#else
    {
        std::unique_lock< std::mutex > const lck{doChangeDoneThreadIDsMutex_};
        for (const auto &id : doChangeDoneThreadIDs_) {
            doChangeWorkingThreads_[id].join();
            std::ignore = doChangeWorkingThreads_.erase(id);
        }
        doChangeDoneThreadIDs_.clear();
    }
    std::thread th{[this, promise, requestedState] {
        ara::core::Result< void > res = networkStateProxy_->SetNetworkRequestedState(_toNmType(requestedState));
        if (res.HasValue()) {
            log_.LogDebug() << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState() by ipc, "
                               "SetNetworkRequestedState ok, requestedState:"
                            << common::NetworkStateTypeToString(requestedState);
            if (requestedState == currentState_) {
                promise->set_value();
                log_.LogDebug()
                    << "NMNetworkStateControlServer::AsyncSetNetworkRequestedState(), requestedState is equal to "
                       "currentState_, currentState_:"
                    << common::NetworkStateTypeToString(currentState_);
            } else {
                _createWaitDoneThreads(requestedState, promise);
            }
        } else {
            core::ErrorCode errorCode{res.Error()};
            promise->SetError(errorCode);
        }
    }};
    std::thread::id id{th.get_id()};
    std::ignore = doChangeWorkingThreads_.insert(std::pair< std::thread::id, std::thread >(id, std::move(th)));
#endif
    return promise->get_future();
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void NMNetworkStateControlServer::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "NMNetworkStateControlServer::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Wait for network current state
/// @param state Requested network state
/// @param promise Used to set the result
void NMNetworkStateControlServer::_waitForNetworkCurrentState(common::NetworkStateInternalType const &state,
                                                              std::shared_ptr< core::Promise< void > > const &promise)
{
    log_.LogInfo() << "NMNetworkStateControlServer::_waitForNetworkCurrentState(), request state:"
                   << common::NetworkStateTypeToString(state).c_str();

    bool ret{false};
    {
        std::unique_lock< std::mutex > lck{stateMutex_};
        std::function< bool() > const condFunc{
            [this, state]() noexcept -> bool { return (this->currentState_ == state); }};
        ret = stateCond_.wait_for(lck, common::GetkMaxWaitNetworkChangeTime(), condFunc);
    }
    if (ret) {
        log_.LogDebug()
            << "NMNetworkStateControlServer::_waitForNetworkCurrentState(), before timeout, currentState_ is "
               "equal to request state:"
            << common::NetworkStateTypeToString(state).c_str();
        promise->set_value();

    } else {
        log_.LogWarn() << "NMNetworkStateControlServer::_waitForNetworkCurrentState(), timeout, currentState_ is not "
                          "equal to request state:"
                       << common::NetworkStateTypeToString(state).c_str();
        promise->SetError(SMErrc::kRejected);
    }
    {
        std::unique_lock< std::mutex > const lck{waitChangeDoneThreadIDsMutex_};
        waitChangeDoneThreadIDs_.push_back(std::this_thread::get_id());
    }
}

void NMNetworkStateControlServer::_createWaitDoneThreads(common::NetworkStateInternalType const &state,
                                                         std::shared_ptr< core::Promise< void > > promise)  // NOLINT
{
    {
        std::unique_lock< std::mutex > const lck{waitChangeDoneThreadIDsMutex_};
        for (const auto &id : waitChangeDoneThreadIDs_) {
            waitChangeWorkingThreads_[id].join();
            std::ignore = waitChangeWorkingThreads_.erase(id);
        }
        waitChangeDoneThreadIDs_.clear();
    }
    std::thread th{&NMNetworkStateControlServer::_waitForNetworkCurrentState, this, state, promise};
    std::thread::id id{th.get_id()};
    std::ignore = waitChangeWorkingThreads_.insert(std::pair< std::thread::id, std::thread >(id, std::move(th)));
}

}  // namespace nm_comm
}  // namespace sm
}  // namespace ara
