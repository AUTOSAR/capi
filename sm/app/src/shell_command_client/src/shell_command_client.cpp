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
/// @file       shell_command_client.cpp
/// @brief
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================

#include "shell_command_client.h"

#include <ara/core/initialization.h>

#include <iostream>
#include <thread>

#include "ara/sm/impl_type_transitionitem.h"

ShellCommandClient::~ShellCommandClient() { _destroy(); }

int32_t ShellCommandClient::SetFgState(std::string const& fgName, std::string const& fgState)
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        ara::core::Future< void > fu{
            shellRequestProxy_->SetFGState(ara::core::String(fgName), ara::core::String(fgState))};
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            ara::core::future_status const futureStatus{fu.wait_for(std::chrono::milliseconds(kSingleTimeoutMs))};
            if (ara::core::future_status::ready == futureStatus) {  // Whether timeout
                ara::core::Result< void > const res{fu.GetResult()};
                if (res.HasValue()) {
                    std::cout << "SetFgState Succeed: " << fgName << "." << fgState << std::endl;
                    return 0;
                }
                std::cerr << "SetFgState fails for fgName: " << fgName << ", fgState: " << fgState << ", "
                          << res.Error().Message() << std::endl;
                return SHELL_GENERAL_ERR;
            }
            cnt++;
        }
        if (kMaxTimeoutCnt <= cnt) {
            std::cerr << "SetFgState fails for fgName: " << fgName << ", wait timeout" << std::endl;
            return SHELL_TIMEOUT_ERR;
        }
    }
    return SHELL_PROXY_ERR;
}
int32_t ShellCommandClient::GetFgState(std::string const& fgName)
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        ara::core::Future< ara::sm::ShellRequest::GetFGStateOutput > fu{
            shellRequestProxy_->GetFGState(ara::core::String(fgName))};
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            ara::core::future_status const futureStatus{fu.wait_for(std::chrono::milliseconds(kSingleTimeoutMs))};
            if (ara::core::future_status::ready == futureStatus) {  // Whether timeout
                ara::core::Result< ara::sm::ShellRequest::GetFGStateOutput > const res{fu.GetResult()};
                if (res.HasValue()) {
                    std::cout << "GetFgState Succeed: " << fgName << "." << res.Value().FGState << std::endl;
                    return 0;
                }
                std::cerr << "GetFGState fails for fgName: " << fgName << ", " << res.Error().Message() << std::endl;
                return SHELL_GENERAL_ERR;
            }
            cnt++;
        }
        if (kMaxTimeoutCnt <= cnt) {
            std::cerr << "GetFGState fails for fgName: " << fgName << ", wait timeout" << std::endl;
            return SHELL_TIMEOUT_ERR;
        }
    }
    return SHELL_PROXY_ERR;
}
int32_t ShellCommandClient::SetSmState(std::string const& smName, uint32_t const& requestNo)
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        ara::core::Future< void > fu{shellRequestProxy_->SetSMState(ara::core::String(smName), requestNo)};
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            ara::core::future_status const futureStatus{fu.wait_for(std::chrono::milliseconds(kSingleTimeoutMs))};
            if (ara::core::future_status::ready == futureStatus) {  // Whether timeout
                ara::core::Result< void > const res{fu.GetResult()};
                if (res.HasValue()) {
                    std::cout << "SetSmState Succeed: " << smName << "." << requestNo << std::endl;
                    return 0;
                }
                std::cerr << "SetSMState fails for smName: " << smName << ", requestNo: " << requestNo << ", "
                          << res.Error().Message() << std::endl;
                return SHELL_GENERAL_ERR;
            }
            cnt++;
        }
        if (kMaxTimeoutCnt <= cnt) {
            std::cerr << "SetSMState fails for smName: " << smName << ", wait timeout" << std::endl;
            return SHELL_TIMEOUT_ERR;
        }
    }
    return SHELL_PROXY_ERR;
}
int32_t ShellCommandClient::GetSmState(std::string const& smName)
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        ara::core::Future< ara::sm::ShellRequest::GetSMStateOutput > fu{
            shellRequestProxy_->GetSMState(ara::core::String(smName))};
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            ara::core::future_status const futureStatus{fu.wait_for(std::chrono::milliseconds(kSingleTimeoutMs))};
            if (ara::core::future_status::ready == futureStatus) {  // Whether timeout
                ara::core::Result< ara::sm::ShellRequest::GetSMStateOutput > const res{fu.GetResult()};
                if (res.HasValue()) {
                    std::cout << "GetSmState Succeed: " << smName << "." << res.Value().SMState
                              << std::endl;  // This output is used to cooperate with the IDE to implement the state machine state feedback function and cannot be deleted
                    return 0;
                }
                std::cerr << "GetSMState fails for smName: " << smName << ", " << res.Error().Message() << std::endl;
                return SHELL_GENERAL_ERR;
            }
            cnt++;
        }
        if (kMaxTimeoutCnt <= cnt) {
            std::cerr << "GetSMState fails for smName: " << smName << ", wait timeout" << std::endl;
            return SHELL_TIMEOUT_ERR;
        }
    }
    return SHELL_PROXY_ERR;
}

int32_t ShellCommandClient::GetAllInfos()
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        ara::core::Future< ara::sm::ShellRequest::GetAllFunctionGroupsInfoOutput > fu{
            shellRequestProxy_->GetAllFunctionGroupsInfo()};
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            ara::core::future_status const futureStatus{fu.wait_for(std::chrono::milliseconds(kSingleTimeoutMs))};
            if (ara::core::future_status::ready == futureStatus) {  // Whether timeout
                ara::core::Result< ara::sm::ShellRequest::GetAllFunctionGroupsInfoOutput > const res{fu.GetResult()};
                if (res.HasValue()) {
                    std::cout << "GetAllFunctionGroupsInfo succeed" << std::endl;
                    ara::sm::FunctionGroupsInfoList infoList{res.Value().infos};
                    for (ara::sm::FunctionGroupsInfo const& info : infoList) {
                        std::cout << "info.smFQN: " << info.smFQN << std::endl;
                        std::cout << "info.smStates: " << _concatenateStrings(info.smStateList) << std::endl;
                        std::cout << "info.managedBySm: " << info.managedBySm << std::endl;
                        std::cout << "\n";
                        for (ara::sm::FunctionGroupInfo const& fgInfo : info.fgInfoList) {
                            std::cout << "fgInfo.fgFQN: " << fgInfo.fgFQN << std::endl;
                            std::cout << "fgInfo.fgStates: " << _concatenateStrings(fgInfo.fgStateList) << std::endl;
                        }
                        std::cout << "\n";
                        for (ara::sm::TransitionItem const& item : info.transitionList) {
                            std::cout << "requestID: " << item.requestID << ", currentSmState: " << item.currentSmState
                                      << ", nextSmState: " << item.nextSmState << std::endl;
                        }
                        std::cout << "\n\n";
                    }
                    return 0;
                }
                std::cerr << "GetAllFunctionGroupsInfo fails, " << res.Error().Message() << std::endl;
                return SHELL_GENERAL_ERR;
            }
            cnt++;
        }
        if (kMaxTimeoutCnt <= cnt) {
            std::cerr << "GetAllFunctionGroupsInfo fails, wait timeout" << std::endl;
            return SHELL_TIMEOUT_ERR;
        }
    }
    return SHELL_PROXY_ERR;
}

int32_t ShellCommandClient::SubscribeSMStateInfo()
{
    int32_t const ret{_init()};
    bool const proxyValid{shellRequestProxy_};
    if ((0 == ret) && proxyValid) {
        std::ignore = shellRequestProxy_->NotifyStateMachineStateChanged.SetReceiveHandler(
            [this]() noexcept { _stateMachienStateChangeHandler(); });
        std::ignore = shellRequestProxy_->NotifyStateMachineStateChanged.Subscribe(TEN_UL);
        uint32_t cnt{0UL};
        while (cnt < kMaxTimeoutCnt) {
            std::this_thread::sleep_for(std::chrono::milliseconds(kSingleTimeoutMs));
            cnt++;
        }
        return 0;
    }
    std::cerr << "SubscribeSMStateInfo fails" << std::endl;
    return SHELL_PROXY_ERR;
}
int32_t ShellCommandClient::_init()
{
    int32_t ret{0};
    if (!initialized_) {
        if (ara::core::Initialize().HasValue()) {
            initialized_ = true;
            std::ignore  = _findService();
        } else {
            ret = -1;
        }
    }
    return ret;
}
void ShellCommandClient::_destroy() noexcept
{
    shellRequestProxy_.reset();
    if (initialized_) {
        if (ara::core::Deinitialize().HasValue()) {
            initialized_ = false;
        }
    }
}

int32_t ShellCommandClient::_findService()
{
    std::promise< void > signalPromise;
    std::future< void > signalFuture{signalPromise.get_future()};
    ara::log::Logger const& logger{
        ara::log::CreateLogger("FIN", "Find Service Callback context", ara::log::LogLevel::kVerbose)};

    ara::core::Optional< ara::sm::proxy::ShellRequestProxy::HandleType > serviceInstanceHandle{};

    ara::core::Result< ara::com::FindServiceHandle > findHandleRes{ara::sm::proxy::ShellRequestProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle, this](
            ara::com::ServiceHandleContainer< ara::sm::proxy::ShellRequestProxy::HandleType > handles,
            ara::com::FindServiceHandle findServiceHandle) {
            {
                std::lock_guard< std::mutex > const lock{serviceMutex_};
                if (shellRequestProxy_ != nullptr) {
                    return;
                }
                if (handles.empty()) {
                    return;
                }
                shellRequestProxy_ = std::make_unique< ara::sm::proxy::ShellRequestProxy >(
                    ara::sm::proxy::ShellRequestProxy::Create(handles[0U]).Value());
            }
            // Stop searching
            ara::sm::proxy::ShellRequestProxy::StopFindService(std::move(findServiceHandle));
            signalPromise.set_value();
        },
        ara::core::InstanceSpecifier{
            ara::core::StringView{"shell_request_client/shell_request_client_root/ShellRequestRPort"}})};

    std::ignore = signalFuture.wait_for(std::chrono::milliseconds(FIVE_THOUSAND_L));

    if (shellRequestProxy_) {
        return 0;
    }
    ara::sm::proxy::ShellRequestProxy::StopFindService(std::move(findHandleRes).Value());
    std::cerr << "ShellCommandClient::_findService() fails" << std::endl;
    return SHELL_PROXY_ERR;
}

void ShellCommandClient::_stateMachienStateChangeHandler() noexcept
{
    std::cout << "ShellCommandClient::_stateMachienStateChangeHandler()\n";
    std::ignore = shellRequestProxy_->NotifyStateMachineStateChanged.GetNewSamples([this](auto const& sample) {
        ara::sm::StateMachineInfo const newInfo{*sample};
        std::cout << "ShellCommandClient::_stateMachienStateChangeHandler(), get a new state info, smFQN: "
                  << newInfo.smFQN << ", smState: " << newInfo.smState << std::endl;
    });
}
ara::core::String ShellCommandClient::_concatenateStrings(ara::core::Vector< ara::core::String > const& strVec) const
{
    ara::core::String strRet;
    size_t const seperatorSize{kSeperator.size()};
    for (ara::core::String const& s : strVec) {  // PRQA S 2961
        std::ignore = strRet.append(s);
        std::ignore = strRet.append(kSeperator);
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}