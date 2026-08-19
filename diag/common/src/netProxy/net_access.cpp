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
/// @file       net_access.cpp
/// @brief
/// @details
/// @date       2024-12-25
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "net_access.h"

#include <chrono>
#include <future>
#include <vector>

#include "log/log.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nsomeip/net/nsi_message.h"
#include "serialization/serialization.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {

// Confirm whether Buf size is reasonable
constexpr size_t kBufSize{4194304U};

constexpr nsi_method_t kFuncDataTransfer{5U};

// Minimum interval for function call timeout check
constexpr std::uint16_t kMinCheckInterval{50U};

using ara::diag::common::LogDebug;
using ara::diag::common::LogError;
using ara::diag::common::LogInfo;
using ara::diag::common::LogWarn;

nai_int_t Nai_Log_Out(
    nai_log_t *l, const char *func, nai_int_t line, nai_int_t level, nai_int_t err, const char *fmt, va_list va)
{
    std::ignore = l;
    std::uint32_t const bufLen{1024U};
    char szLogBuf[bufLen] = {0};
    int cnt               = vsnprintf(szLogBuf, sizeof(szLogBuf), fmt, va);
    std::ignore           = cnt;
    switch (level) {
        case NAI_LOG_DEBUG:
            LogDebug() << func << "@" << line << szLogBuf;
            break;
        case NAI_LOG_WARN:
            LogWarn() << func << "@" << line << szLogBuf;
            break;
        case NAI_LOG_INFO:
            LogInfo() << func << "@" << line << szLogBuf;
            break;
        case NAI_LOG_ERROR:
            LogError() << func << "@" << line << ",errno:" << err << "," << szLogBuf;
            break;
        default:
            LogInfo() << func << "@" << line << ",errno:" << err << "," << szLogBuf;
            break;
    }
    return 0;
}

void NetAccess::RegisterService(uint32_t const &serviceInstanceId, CallHandleAsync const &callHandle)
{
    {
        std::unique_lock< std::mutex > lk{mMtxServices_};
        mServices_[serviceInstanceId] = callHandle;
    }

    // Server-side dynamically registered services, service information synchronization processing to be implemented
    // The current server-side synchronizes all services provided by the server side when the client connection is established
    if (Node_Role::kClient
        == config
               .role) {  // When the client registers a service, dynamically synchronize the service to the server side
        _SyncServicesList();
    }
}

void NetAccess::UnregisterService(uint32_t const &serviceInstanceId)
{
    std::unique_lock< std::mutex > lk{mMtxServices_};
    auto iter = mServices_.find(serviceInstanceId);
    if (iter != mServices_.end()) {
        mServices_.erase(iter);
    }
}

bool NetAccess::IsServiceReady(uint32_t const &serviceInstanceId) noexcept
{
    bool result{false};
    std::unique_lock< std::mutex > lk{mMutexRemoteServices_};
    auto iter = mRemoteServices_.find(serviceInstanceId);
    if (iter != mRemoteServices_.end()) {
        result = iter->second.bReady;
    }
    return result;
}

void NetAccess::RegisterOnReady(FuncOnReadyCallBack const &cb) { funcOnReady = cb; }

void NetAccess::RegisterOnServiceReady(uint32_t const &sii, FuncOnReadyCallBack const &cb)
{
    std::unique_lock< std::mutex > lk{mMutexRemoteServices_};
    LogDebug() << "NetAccess::RegisterOnServiceReady" << sii;
    auto iter = mRemoteServices_.find(sii);
    if (iter != mRemoteServices_.end()) {
        iter->second.readyNotify = cb;
        if (iter->second.bReady) {
            // ThreadPool::GetInstance().Submit([this, sii]() {
            //     ServiceStatus const &ss = mRemoteServices_[sii];
            //     ss.readyNotify(ss.bReady);
            // });

            {
                std::unique_lock< std::mutex > lock(mtxOnServiceReadyTaskQueue_);
                onServiceReadyTaskQueue_.emplace_back([this, sii]() {
                    {
                        std::unique_lock< std::mutex > localLock{mMutexRemoteServices_};
                        if (mRemoteServices_.end() == mRemoteServices_.find(sii)) {
                            LogWarn() << __FUNCTION__ << "| can't find" << sii  //NOLINT
                                      << "in mRemoteServices_, size:" << mRemoteServices_.size();
                            assert(
                                false);  // Temporarily added to catch issues, delete after problem analysis is complete
                            return;
                        }
                    }
                    mRemoteServices_.at(sii).readyNotify(true);
                });
            }
            _HandleServiceReadyCB();
        }
        return;
    }

    ServiceStatus ss;
    ss.sii         = sii;
    ss.readyNotify = cb;
    mRemoteServices_.emplace(sii, std::move(ss));
}

std::future< std::vector< uint8_t > > NetAccess::CallFunc(uint32_t const &sii, uint16_t const &cii, FuncData &&funcData)
{
    std::promise< std::vector< uint8_t > > promiseRet;  // NOLINT
    auto ret = promiseRet.get_future();

    std::uint16_t channelId = _GetChannelId(sii);
    if (0 == channelId) {
        LogDebug() << "service " << sii << " is not ready.";
        promiseRet.set_value({});
        return ret;
    }

    // Save call information
    CallInfo callInfo;
    callInfo.callTime = std::chrono::steady_clock::now().time_since_epoch().count();
    callInfo.result   = std::move(promiseRet);
    auto callKey      = MakeCallKey(cii, funcData.funcId, funcData.callId);
    {
        std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
        auto iter = funcCallInfo_.find(callKey);
        if (iter != funcCallInfo_.end()) {
            // old data handle
        }
        funcCallInfo_[callKey] = std::move(callInfo);
    }
    LogDebug() << "NetAccess::CallFunc| callkey:" << callKey;

    // Send call data
    RpcData rpcData;
    rpcData.version          = 1;
    rpcData.funcData         = std::move(funcData);
    rpcData.channelId        = channelId;
    rpcData.serviceId        = static_cast< uint16_t >(sii >> (UINT16_WIDTH));
    rpcData.serviceInstancId = static_cast< uint16_t >(sii & (UINT16_MAX));
    rpcData.clientObjId      = cii;
    LogDebug() << "NetAccess::CallFunc| " << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
               << rpcData.funcData.callId << ").ssi" << sii;
    std::vector< uint8_t > buf{};
    if (serialize::Serialize(buf, rpcData) < 0) {
        return ret;
    }

    if (Node_Role::kClient == config.role) {
        _SendRequestMsg(&app, buf.data(), buf.size());
    } else if (Node_Role::kServer == config.role) {
        _SendNotifyMsg(&app, buf.data(), buf.size(), channelId);
    }

    // Create function call timeout detection thread
    if (!bCreateThreadForHandleTImeout_) {
        bCreateThreadForHandleTImeout_ = true;
        std::thread timerThread{[this]() { _HandleTaskTimeOut(); }};
        timerThread.detach();
    }
    mCvFuncCallInfo_.notify_all();
    return ret;
}

void NetAccess::CallFunc(uint32_t const &sii, uint16_t const &cii, FuncData &&funcData, FuncDataHandle &&funcDataHandle)
{
    // build call info
    ThreadPool::GetInstance().Submit([this, sii, cii, funcData, funcDataHandle]() {
        std::uint16_t channelId = _GetChannelId(sii);
        if (0 == channelId) {
            LogDebug() << "service " << sii << " is not ready.";
            return;
        }

        std::promise< std::vector< uint8_t > > promiseRet;
        auto ret = promiseRet.get_future();
        CallInfo callInfo;
        callInfo.callTime       = std::chrono::steady_clock::now().time_since_epoch().count();
        callInfo.result         = std::move(promiseRet);  // This is useless when there is a callback function
        callInfo.funcDataHandle = funcDataHandle;
        callInfo.bSync          = false;
        auto callKey            = MakeCallKey(cii, funcData.funcId, funcData.callId);

        {
            std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
            auto iter = funcCallInfo_.find(callKey);
            if (iter != funcCallInfo_.end()) {
                // old data handle
            }
            funcCallInfo_[callKey] = std::move(callInfo);
        }

        // Send call information
        RpcData rpcData;
        rpcData.version          = 1;
        rpcData.funcData         = std::move(funcData);  //NOLINT
        rpcData.channelId        = channelId;
        rpcData.connectId        = 1;
        rpcData.serviceId        = static_cast< uint16_t >(sii >> (UINT16_WIDTH));
        rpcData.serviceInstancId = static_cast< uint16_t >(sii & (UINT16_MAX));
        rpcData.clientObjId      = cii;
        LogDebug() << "NetAccess::CallFunc| " << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
                   << rpcData.funcData.callId << ")." << sii;
        std::vector< uint8_t > buf;
        if (serialize::Serialize(buf, rpcData) < 0) {
            return;
        }

        if (Node_Role::kClient == config.role) {
            _SendRequestMsg(&app, buf.data(), buf.size());
        } else if (Node_Role::kServer == config.role) {
            _SendNotifyMsg(&app, buf.data(), buf.size(), channelId);
        }
    });
}

void NetAccess::ReturnResult(CallRequest const &callData, RetData &&retData)
{
    // Thread execution
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, retData) < 0) {
        return;
    }

    // lock?
    CallContext *pCallContext = reinterpret_cast< CallContext * >(callData.callContext.get());
    RpcData &rpcData          = pCallContext->rpcData;
    rpcData.funcData.data     = std::move(payLoad);
    rpcData.funcData.funcType = FuncType::kFuncType_result;
    LogDebug() << "NetAccess::ReturnResult| " << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
               << rpcData.funcData.callId << ").";
    // serialize RpcData
    std::vector< uint8_t > buf;
    if (serialize::Serialize(buf, rpcData) < 0) {
        LogDebug() << "NetAccess::ReturnResult|Seriaize fails!";
        return;
    }

    if (Node_Role::kClient == config.role) {
        _SendRequestMsg(&app, buf.data(), buf.size());
    } else if (Node_Role::kServer == config.role) {
        _SendNotifyMsg(&app, buf.data(), buf.size(), pCallContext->rpcData.channelId);
    }
}

void NetAccess::RegisterOnCancel(uint32_t const &sii,
                                 uint8_t const &funcId,
                                 std::uint8_t const &callId,
                                 std::function< void() > const &notifier)
{
    LogDebug() << "NetAccess::RegisterOnCancel| sii:" << sii << " funcId:" << funcId << " callid:" << callId;
    std::unique_lock< std::mutex > lock{mMutexCancelCallBack_};
    uint64_t key = (static_cast< uint64_t >(sii) << UINT64_WIDTH) |            //NOLINT
                   (static_cast< uint64_t >(funcId) << UINT8_WIDTH) | callId;  //NOLINT
    mCancelCallback_.insert({key, notifier});
}

void NetAccess::UnregisterOnCancel(uint32_t const &sii, uint8_t const &funcId, std::uint8_t const &callId)
{
    LogDebug() << "NetAccess::UnregisterOnCancel| sii:" << sii << " funcId:" << funcId << " callid:" << callId;
    std::unique_lock< std::mutex > lock{mMutexCancelCallBack_};
    uint64_t key = (static_cast< uint64_t >(sii) << UINT64_WIDTH) |            //NOLINT
                   (static_cast< uint64_t >(funcId) << UINT8_WIDTH) | callId;  //NOLINT
    mCancelCallback_.erase(key);
}

void NetAccess::CancelFunctionCall(uint32_t const &serviceId, uint16_t const &instanceId, FuncData &&funcData)
{
    {
        std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
        auto callKey = MakeCallKey(instanceId, funcData.funcId, funcData.callId);
        auto iter    = funcCallInfo_.find(callKey);
        if (iter != funcCallInfo_.end()) {
            CallInfo ci = std::move(funcCallInfo_[callKey]);
            funcCallInfo_.erase(iter);
            if (ci.funcDataHandle != nullptr) {
                ci.funcDataHandle({});
            }
        }
    }
    funcData.funcType = FuncType::kFuncType_cancel;
    std::shared_ptr< FuncData > funcDataPtr{std::make_shared< FuncData >(std::move(funcData))};

    ThreadPool::GetInstance().Submit([this, serviceId, instanceId, funcDataPtr]() {
        std::uint16_t channelId = _GetChannelId(serviceId);
        if (0 == channelId) {
            LogDebug() << "service " << serviceId << " is not ready.";
            return;
        }
        // rpc encap
        RpcData rpcData;
        rpcData.version          = 1;
        rpcData.funcData         = *funcDataPtr;
        rpcData.channelId        = channelId;
        rpcData.connectId        = 1;
        rpcData.serviceId        = static_cast< uint16_t >(serviceId >> (UINT16_WIDTH));
        rpcData.serviceInstancId = static_cast< uint16_t >(serviceId & (UINT16_MAX));
        rpcData.clientObjId      = instanceId;

        std::vector< uint8_t > buf;
        if (serialize::Serialize(buf, rpcData) < 0) {
            return;
        }
        // Send cancel notification
        if (Node_Role::kClient == config.role) {
            _SendRequestMsg(&app, buf.data(), buf.size());
        } else if (Node_Role::kServer == config.role) {
            _SendNotifyMsg(&app, buf.data(), buf.size(), channelId);
        }
    });
}

void NetAccess::RegisterNotificationCallBack(uint16_t const &instanceId, NotifyCallBack &&notify)
{
    std::unique_lock< std::mutex > lock(mMtxNotifyCallBackMap_);
    std::ignore = mNotifyCallBackMap_.emplace(instanceId, std::move(notify));
}

// void NetAccess::RegisterNotification(uint32_t const &sii, uint8_t const &eventId) {
// }

void NetAccess::Notify(CallRequest const &callData)
{
    CallContext *const pCallContext{reinterpret_cast< CallContext * >(callData.callContext.get())};
    if (nullptr == pCallContext) {
        // logError()
        return;
    }
    RpcData &rpcData = pCallContext->rpcData;
    rpcData.funcData = callData.funcData;

    std::vector< uint8_t > data;
    if (serialize::Serialize(data, rpcData) < 0) {
        return;
    }

    if (Node_Role::kClient == config.role) {
        _SendRequestMsg(&app, data.data(), data.size());
    } else if (Node_Role::kServer == config.role) {
        _SendNotifyMsg(&app, data.data(), data.size(), pCallContext->rpcData.channelId);
    }
}

void NetAccess::SyncServicesList(std::uint16_t channelId)
{
    {
        std::unique_lock< std::mutex > lk(mMutexServicesSyncRunning_);
        if (servicesSyncRunning_.end() != servicesSyncRunning_.find(channelId)) {
            return;
        }
        servicesSyncRunning_.insert(channelId);
    }
    // Implement one-time service list synchronization
    ThreadPool::GetInstance().Submit([this, channelId]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(mServiceSyncInterval_));
        // services list
        std::vector< uint32_t > serviceList;
        {
            std::unique_lock< std::mutex > lk{mMtxServices_};
            serviceList.resize(mServices_.size());
            std::transform(mServices_.begin(), mServices_.end(), serviceList.begin(),
                           [channelId](std::pair< std::uint32_t, CallHandleAsync > const &p) {
                               LogDebug() << "NetAccess::SyncServicesList|" << channelId << " service" << p.first;
                               return p.first;
                           });
        }

        std::vector< uint8_t > funcDataBuf;
        if (serialize::Serialize(funcDataBuf, serviceList) < 0) {
            return;
        }

        FuncData funcData;
        funcData.funcId   = 0U;
        funcData.callId   = 1U;
        funcData.funcType = FuncType::kFuncType_syncInfo;
        funcData.data     = std::move(funcDataBuf);

        // rpc encap
        RpcData rpcData;
        rpcData.version          = 1;
        rpcData.funcData         = std::move(funcData);
        rpcData.channelId        = channelId;
        rpcData.connectId        = 1U;
        rpcData.serviceId        = 0U;
        rpcData.serviceInstancId = 0U;
        rpcData.clientObjId      = 0U;

        std::vector< uint8_t > buf;
        if (serialize::Serialize(buf, rpcData) < 0) {
            return;
        }

        if (Node_Role::kClient == config.role) {
            _SendRequestMsg(&app, buf.data(), buf.size());
        } else if (Node_Role::kServer == config.role) {
            _SendNotifyMsg(&app, buf.data(), buf.size(), channelId);
        }

        {  // Synchronization complete, delete corresponding data
            std::unique_lock< std::mutex > lk(mMutexServicesSyncRunning_);
            if (servicesSyncRunning_.end() != servicesSyncRunning_.find(channelId)) {
                servicesSyncRunning_.erase(channelId);
            }
        }
    });
}

int NetAccess::On_Message(nsi_app_t *a, void *ud, nsi_message_t *m)
{
    nsi_message_io_t io{};
    uint8_t buf[kBufSize];
    auto *na = static_cast< NetAccess * >(ud);

    do {
        auto const &msgType = m->hdr.type;
        if (NSI_MT_ERROR == msgType || NSI_TP_ERROR == msgType) {
            // error handle
            break;
        }
        // read message
        nsi_message_read_start(&io, m, 0);
        intptr_t r = nsi_message_read(&io, buf, sizeof(buf));
        nsi_message_read_end(&io);

        if (r < 0) {
            // output error
            break;
        }

        RpcData rpcData;
        std::vector< uint8_t > payLoad(buf, buf + r);
        if (serialize::Deserialize(payLoad, rpcData) < 0) {
            break;
        }

        if (FuncType::kFuncType_call == rpcData.funcData.funcType) {
            na->_HandleRequest(a, m, std::move(rpcData));
        } else if (FuncType::kFuncType_result == rpcData.funcData.funcType) {
            na->_HandleResponse(a, std::move(rpcData));
        } else if (FuncType::kFuncType_notify == rpcData.funcData.funcType) {
            na->_HandleNotification(a, std::move(rpcData));
        } else if (FuncType::kFuncType_cancel == rpcData.funcData.funcType) {
            na->_HandleCancel(a, std::move(rpcData));
        } else if (FuncType::kFuncType_syncInfo == rpcData.funcData.funcType) {
            na->_HandleSyncInfo(a, std::move(rpcData));
        }
    } while (false);
    // close message
    nsi_message_close(m);
    return 0;
}

int NetAccess::Dispatch(void *p)
{
    auto *e = static_cast< nai_evloop_t * >(p);
    nai_evloop_dispatch(e);
    return 0;
}

void NetAccess::_HandleRequest(nsi_app_t *a, nsi_message_t *m, RpcData &&rpcData)
{
    std::ignore = a;
    // Queue cache requests
    uint32_t channelId = m->hdr.client;
    std::ignore        = channelId;
    // Lock
    CallRequest cr;
    cr.funcData = rpcData.funcData;
    auto cc{std::make_shared< CallContext >()};
    cc->clientId   = rpcData.channelId;
    cc->rpcData    = rpcData;
    cr.callContext = std::move(cc);
    LogDebug() << "NetAccess::_HandleRequest| " << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
               << rpcData.funcData.callId << ").";
    ThreadPool::GetInstance().Submit([this, cr = std::move(cr)]() {
        uint16_t serviceId  = static_cast< CallContext * >(cr.callContext.get())->rpcData.serviceId;         //NOLINT
        uint16_t instanceId = static_cast< CallContext * >(cr.callContext.get())->rpcData.serviceInstancId;  //NOLINT
        uint32_t k          = static_cast< uint32_t >(serviceId << UINT16_WIDTH | instanceId);               //NOLINT
        // Impact of service deregistration on thread execution? Mark validity? No impact on current execution  TODO Implement optimization
        CallHandleAsync func{nullptr};
        {
            std::unique_lock< std::mutex > lk{mMtxServices_};
            if (mServices_.find(k) != mServices_.end() && mServices_[k] != nullptr) {
                func = mServices_[k];
            }
        }
        CallRequest crTmp = std::move(cr);  // NOLINT data copy optimization
        if (func) {
            func(std::move(crTmp));
        }
    });
}

void NetAccess::_HandleResponse(nsi_app_t *a, RpcData &&rpcData)
{
    std::ignore = a;
    ThreadPool::GetInstance().Submit([this, rpcData = std::move(rpcData)]() {
        CallInfo ci;
        auto callKey = MakeCallKey(rpcData.clientObjId, rpcData.funcData.funcId, rpcData.funcData.callId);
        {
            LogDebug() << "NetAccess::_HandleResponse| handle thread start do job. request lock";
            std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
            LogDebug() << "NetAccess::_HandleResponse| handle thread start do job. got lock";

            auto iter = funcCallInfo_.find(callKey);
            if (iter == funcCallInfo_.end()) {
                // output callinfo no found
                LogDebug() << "haven't find task (" << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
                           << rpcData.funcData.callId << ").";
                return;
            }
            LogDebug() << "NetAccess::_HandleResponse| callkey:" << callKey;
            ci = std::move(funcCallInfo_[callKey]);
            funcCallInfo_.erase(callKey);
        }
        LogDebug() << "NetAccess::_HandleResponse|start handle(" << rpcData.clientObjId << ":"
                   << rpcData.funcData.funcId << ":" << rpcData.funcData.callId << ").";
        if (ci.bSync) {
            LogDebug() << "NetAccess::_HandleResponse|set value";
            ci.result.set_value(rpcData.funcData.data);
            LogDebug() << "NetAccess::_HandleResponse|set value end";
        } else {
            if (ci.funcDataHandle != nullptr) {
                ci.funcDataHandle(rpcData.funcData.data);
            }
        }
    });
}

void NetAccess::_HandleNotification(nsi_app_t *a, RpcData &&rpcData)
{
    std::ignore = a;
    // Normal notification handle
    LogDebug() << "NetAccess::_HandleNotification| " << rpcData.clientObjId << ":" << rpcData.funcData.funcId << ":"
               << rpcData.funcData.callId << ").";
    ThreadPool::GetInstance().Submit([this, data = std::move(rpcData)]() { Notify(data); });
}

void NetAccess::_HandleCancel(nsi_app_t *a, RpcData &&rpcData)
{
    std::ignore = a;
    // handle cancellation operation
    ThreadPool::GetInstance().Submit([this, data = std::move(rpcData)]() {
        LogDebug() << "NetAccess::_HandleCancel| serviceId:" << data.serviceId
                   << " serviceInstancId:" << data.serviceInstancId << " funcId:" << data.funcData.funcId;
        std::unique_lock< std::mutex > lock{mMutexCancelCallBack_};
        uint64_t key = (static_cast< uint64_t >(data.serviceId) << isoft::serialize::kInt32_48)           //NOLINT
                       | (static_cast< uint64_t >(data.serviceInstancId) << isoft::serialize::kInt32_32)  //NOLINT
                       | (static_cast< uint64_t >(data.funcData.funcId) << isoft::serialize::kInt32_8)
                       | data.funcData.callId;  //NOLINT
        decltype(auto) iter = mCancelCallback_.find(key);
        if (iter != mCancelCallback_.end() && iter->second != nullptr) {
            iter->second();
        }
    });
}

void NetAccess::_HandleSyncInfo(nsi_app_t *a, RpcData &&rpcData)
{
    std::ignore = a;
    std::vector< std::uint32_t > serviceList;
    if (serialize::Deserialize(rpcData.funcData.data, serviceList) < 0) {
        return;
    }
    std::unique_lock< std::mutex > lk{mMutexRemoteServices_};
    for (auto const &serviceInstanceId : serviceList) {
        LogDebug() << "NetAccess::_HandleSyncInfo" << serviceInstanceId;
        auto iter = mRemoteServices_.find(serviceInstanceId);
        if (iter == mRemoteServices_.end()) {
            ServiceStatus ss;
            ss.bReady = true;
            ss.sii    = serviceInstanceId;
            ss.channelIds.push_back(rpcData.channelId);
            mRemoteServices_.emplace(serviceInstanceId, std::move(ss));
            continue;
        }
        iter->second.bReady = true;
        iter->second.channelIds.push_back(rpcData.channelId);

        LogDebug() << "NetAccess::_HandleSyncInfo" << serviceInstanceId << "callback call.";
        // When the remote service is provided, call the locally registered callback function
        if (nullptr != mRemoteServices_.at(serviceInstanceId).readyNotify) {
            std::unique_lock< std::mutex > lock(mtxOnServiceReadyTaskQueue_);
            onServiceReadyTaskQueue_.emplace_back([this, serviceInstanceId]() {
                {
                    std::unique_lock< std::mutex > localLock{mMutexRemoteServices_};
                    if (mRemoteServices_.end() == mRemoteServices_.find(serviceInstanceId)) {
                        LogWarn() << __FUNCTION__ << "| can't find" << serviceInstanceId  //NOLINT
                                  << "in mRemoteServices_, size:" << mRemoteServices_.size();
                        assert(false);  // Temporarily added to catch issues, delete after problem analysis is complete
                        return;
                    }
                }
                mRemoteServices_.at(serviceInstanceId).readyNotify(true);
            });
        }
        // Service offline processing logic
    }
    _HandleServiceReadyCB();
}

void NetAccess::Notify(RpcData const &rpcData)
{
    auto iter = mNotifyCallBackMap_.find(rpcData.clientObjId);
    if (iter != mNotifyCallBackMap_.end()) {
        iter->second(rpcData.funcData);
    }
}

void NetAccess::_SendNotifyMsg(nsi_app_t *a, const void *buf, size_t len, std::uint16_t channelId) const
{
    nsi_message_io_t io;  // NOLINT
    nsi_message_t *m = nsi_app_create_notification(a, config.serviceId, config.instanceId, channelId);
    if (m == nullptr) {
        LogWarn() << "NetAccess::_SendNotifyMsg| nsi_app_create_notification fails!";
        return;
    }
    LogInfo() << "NetAccess::_SendNotifyMsg| buf size:" << len << " channelId:" << channelId;
    do {
        if (-1 == nsi_message_write_start(&io, m)) {
            LogWarn() << "NetAccess::_SendNotifyMsg| nsi_message_write_start fails!";
            break;
        }
        if (-1 == nsi_message_write(&io, buf, len)) {
            LogWarn() << "NetAccess::_SendNotifyMsg| nsi_message_write fails!";
            break;
        }
        if (-1 == nsi_message_write_end(&io)) {
            LogWarn() << "NetAccess::_SendNotifyMsg| nsi_message_write_end fails!";
            break;
        }
        if (-1 == nsi_app_send(a, m)) {
            LogWarn() << "NetAccess::_SendNotifyMsg| nsi_app_send fails!";
            break;
        }
        LogInfo() << "NetAccess::_SendNotifyMsg| message hdr len:" << m->hdr.len << "msg hdr size:" << NSI_MSGHDR_SIZE
                  << " NSI_MSGEXT_SIZE:" << NSI_MSGEXT_SIZE;
        return;
    } while (false);

    nsi_message_close(m);
}

void NetAccess::_SendRequestMsg(nsi_app_t *a, const void *buf, size_t len) const
{
    if (a == nullptr || buf == nullptr) {
        LogWarn() << "NetAccess::_SendRequestMsg| a fails!";
        return;
    }
    LogInfo() << "NetAccess::_SendRequestMsg| buf size:" << len;
    nsi_message_io_t io{};  // NOLINT
    nsi_message_t *req{nullptr};
    req = nsi_app_create_request(a, config.serviceId, config.instanceId, kFuncDataTransfer);
    if (req == nullptr) {
        LogWarn() << "NetAccess::_SendRequestMsg| nsi_app_create_request fails!";
        return;
    }

    do {
        if (-1 == nsi_message_write_start(&io, req)) {
            LogWarn() << "NetAccess::_SendRequestMsg| nsi_message_write_start fails!";
            break;
        }
        if (-1 == nsi_message_write(&io, buf, len)) {
            LogWarn() << "NetAccess::_SendRequestMsg| nsi_message_write fails!";
            break;
        }
        if (-1 == nsi_message_write_end(&io)) {
            LogWarn() << "NetAccess::_SendRequestMsg| nsi_message_write_end fails!";
            break;
        }
        if (-1 == nsi_app_send(a, req)) {
            LogWarn() << "NetAccess::_SendRequestMsg| nsi_app_send fails!";
            break;
        }
        LogInfo() << "NetAccess::_SendRequestMsg| message hdr len:" << req->hdr.len;
        return;
    } while (false);

    nsi_message_close(req);
}

void NetAccess::_SendResponseMsg(nsi_app_t *a, nsi_message_t *req, const void *buf, size_t len) const  //NOLINT
{
    if (a == nullptr || req == nullptr || buf == nullptr) {
        LogWarn() << "NetAccess::_SendResponseMsg| nsi_app_send fails!";
        return;
    }

    nsi_message_io_t io;  // NOLINT

    do {
        if (-1 == nsi_message_write_start(&io, req)) {
            LogWarn() << "NetAccess::_SendResponseMsg| nsi_message_write_start fails!";
            break;
        }
        if (-1 == nsi_message_write(&io, buf, len)) {
            LogWarn() << "NetAccess::_SendResponseMsg| nsi_message_write fails!";
            break;
        }
        if (-1 == nsi_message_write_end(&io)) {
            LogWarn() << "NetAccess::_SendResponseMsg| nsi_message_write_end fails!";
            break;
        }
        if (-1 == nsi_app_send(a, req)) {
            LogWarn() << "NetAccess::_SendResponseMsg| nsi_app_send fails!";
            break;
        }
        return;
    } while (false);

    nsi_message_close(req);
}

std::uint16_t NetAccess::_GetChannelId(std::uint32_t const &sii)
{
    std::uint16_t channelId{0};
    if (Node_Role::kClient == config.role) {
        channelId = config.eventId;
    } else if (Node_Role::kServer == config.role) {
        std::unique_lock< std::mutex > lk{mMutexRemoteServices_};
        auto iter = mRemoteServices_.find(sii);
        if (iter != mRemoteServices_.end()) {
            channelId = *(iter->second.channelIds.rbegin());
        }
    }
    return channelId;
}

void NetAccess::_SyncServicesList()
{
    if (bServicesSyncRunning_) {
        return;
    }

    bServicesSyncRunning_ = true;
    ThreadPool::GetInstance().Submit([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(mServiceSyncInterval_));
        std::uint32_t sii       = 0U;
        std::uint16_t channelId = _GetChannelId(sii);
        if (0 == channelId) {
            LogDebug() << "service " << sii << " is not ready.";
            return;
        }
        // services list
        std::vector< uint32_t > serviceList;
        {
            std::unique_lock< std::mutex > lk{mMtxServices_};
            serviceList.resize(mServices_.size());
            std::transform(mServices_.begin(), mServices_.end(), serviceList.begin(),
                           [](std::pair< std::uint32_t, CallHandleAsync > const &p) {
                               LogDebug() << "NetAccess::_SyncServicesList| service" << p.first;
                               return p.first;
                           });
        }
        bServicesSyncRunning_ = false;

        std::vector< uint8_t > funcDataBuf;
        if (serialize::Serialize(funcDataBuf, serviceList) < 0) {
            return;
        }

        FuncData funcData;
        funcData.funcId   = 0U;
        funcData.callId   = 1U;
        funcData.funcType = FuncType::kFuncType_syncInfo;
        funcData.data     = std::move(funcDataBuf);

        // rpc encap
        RpcData rpcData;
        rpcData.version          = 1;
        rpcData.funcData         = std::move(funcData);
        rpcData.channelId        = channelId;
        rpcData.connectId        = 1U;
        rpcData.serviceId        = 0U;
        rpcData.serviceInstancId = 0U;
        rpcData.clientObjId      = 0U;

        std::vector< uint8_t > buf;
        if (serialize::Serialize(buf, rpcData) < 0) {
            return;
        }

        if (Node_Role::kClient == config.role) {
            _SendRequestMsg(&app, buf.data(), buf.size());
        } else if (Node_Role::kServer == config.role) {
            _SendNotifyMsg(&app, buf.data(), buf.size(), channelId);
        }
    });
}

void NetAccess::Stop() noexcept
{
    stop_ = true;
    while (!funcCallInfo_.empty()) {
        auto iter = funcCallInfo_.begin();
        std::vector< uint8_t > exitData{};

        RetData rd;
        rd.errorCode = static_cast< int32_t >(InnerErrorCode::kExit);
        if (serialize::Serialize(exitData, rd) < 0) {
            LogDebug() << "NetAccess::Stop| Serialize failed";
        }

        CallInfo &ci = iter->second;
        ci.result.set_value(exitData);
        LogWarn() << "NetAccess::Stop| erase exit taskId:" << iter->first;
        iter = funcCallInfo_.erase(iter);
    }
    mCvFuncCallInfo_.notify_all();
}
void NetAccess::_HandleTaskTimeOut()  //NOLINT
{
    std::uint64_t const nsPerMillisecond{1000000U};
    std::uint64_t callTimeoutInNs = callTimeout_ * nsPerMillisecond;
    int32_t const frequence{5};
    std::uint16_t checkFrequence = std::max(static_cast< std::uint16_t >(callTimeout_ / frequence), kMinCheckInterval);
    while (true) {
        {
            LogDebug() << "NetAccess::_HandleTaskTimeOut| cv wait. request lock";
            std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
            mCvFuncCallInfo_.wait(lock, [this]() -> bool { return !(funcCallInfo_.empty()) || stop_; });
            LogDebug() << "NetAccess::_HandleTaskTimeOut| cv wait. got lock " << lock.owns_lock();
            if (stop_) {
                return;
            }
        }
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(checkFrequence));
            {
                LogDebug() << "NetAccess::_HandleTaskTimeOut| to thread start. request lock";
                std::unique_lock< std::mutex > lock(mMtxFuncCallInfo_);
                LogDebug() << "NetAccess::_HandleTaskTimeOut| to thread start. got lock";
                if (funcCallInfo_.empty()) {
                    break;
                }

                std::uint64_t curTime = std::chrono::steady_clock::now().time_since_epoch().count();
                for (auto iter = funcCallInfo_.begin(); iter != funcCallInfo_.end();) {
                    if ((curTime - iter->second.callTime) > callTimeoutInNs) {
                        if (timeOutData_.empty()) {
                            RetData rd;
                            rd.errorCode = static_cast< int32_t >(InnerErrorCode::kFuncCallTimeout);
                            if (serialize::Serialize(timeOutData_, rd) < 0) {
                                LogDebug() << "NetAccess::_HandleTaskTimeOut| Serialize failed";
                            }
                        }

                        CallInfo &ci = iter->second;
                        if (ci.bSync) {
                            ci.result.set_value(timeOutData_);
                            LogWarn() << "NetAccess::_HandleTaskTimeOut| erase timeout taskId:" << iter->first;
                            iter = funcCallInfo_.erase(iter);
                            continue;
                        }
                    }
                    iter++;
                }
            }
        }
    }
    bCreateThreadForHandleTImeout_ = false;
}

void NetAccess::_HandleServiceReadyCB()
{
    if (!bThreadRunningForServiceReadyTask_) {
        bThreadRunningForServiceReadyTask_ = true;
        std::thread thd([this]() {
            constexpr uint16_t kSleepTimeForNewTask{500U};
            std::int16_t checkEmptyTimes{3};
            while (checkEmptyTimes > 0) {
                std::function< void() > serviceReadyCB;
                {
                    std::unique_lock< std::mutex > lock(mtxOnServiceReadyTaskQueue_);
                    if (!onServiceReadyTaskQueue_.empty()) {
                        // std::swap(serviceReadyCB, onServiceReadyTaskQueue_.front());
                        serviceReadyCB = onServiceReadyTaskQueue_.front();
                        onServiceReadyTaskQueue_.pop_front();
                    } else {
                        checkEmptyTimes--;
                    }
                }

                if (serviceReadyCB) {
                    serviceReadyCB();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(kSleepTimeForNewTask));
                }
            }
            bThreadRunningForServiceReadyTask_ = false;
        });
        thd.detach();
    }
}

}  // namespace dm
}  // namespace isoft