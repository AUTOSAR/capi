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
/// @file       net_access.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __NET_ACCESS__
#define __NET_ACCESS__

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "nai/runtime/nai_log.h"
#include "netProxy/rpc_define.h"
#include "nsomeip/core/nsi_app.h"
#include "nsomeip/core/nsi_conf.h"
#include "nsomeip/core/nsi_routing.h"
#include "serialization/serialization/common_data_type.h"

namespace isoft {
namespace dm {

/// @brief Callback function when network is ready
using FuncOnReadyCallBack = std::function< void(bool bReady) >;
/// @brief Callback function for processing remote call return data
using FuncDataHandle = std::function< void(std::vector< uint8_t > const& data) >;
/// @brief Callback function for processing remote event notification
using NotifyCallBack = std::function< void(FuncData const&) >;
/// @brief Call cancel callback function
using CancelHandleFunctionCB = std::function< void() >;
/// @brief Service call callback function
using CallHandleAsync = std::function< void(CallRequest&& callData) >;
std::uint16_t const kDefaultCallTimeout{1000U};
/// @brief Function call related information encapsulation, used to save call information
struct CallInfo
{
    /// @name bSync
    /// sync call or async call
    bool bSync{true};
    std::uint64_t callTime{0};
    std::promise< std::vector< uint8_t > > result;
    FuncDataHandle funcDataHandle;
};

struct CallContext : public CallContextBase  //NOLINT
{
    RpcData rpcData;
};

enum class Node_Role : uint8_t
{
    kServer = 0,
    kClient
};

struct AccessConfig
{
    nsi_version_t versionMajor{1U};
    nsi_versmin_t versionMinor{1U};
    std::string nsiJson;
    std::string serverName;
    uint16_t clientServiceId;
    uint16_t clientInstanceId;
    uint16_t serviceId;
    uint16_t instanceId;
    uint16_t groupId;
    uint16_t eventId;
    Node_Role role;
    std::vector< std::uint16_t > processIdList;
};

nai_int_t Nai_Log_Out(
    nai_log_t* l, const char* func, nai_int_t line, nai_int_t level, nai_int_t err, const char* fmt, va_list va);

struct ServiceStatus
{
    std::uint32_t sii{0U};
    std::vector< std::uint16_t > channelIds;
    bool bReady{false};
    FuncOnReadyCallBack readyNotify{nullptr};
};

/**
 * Abstract network layer
 */
class NetAccess
{
public:
    NetAccess()  = default;  //NOLINT
    ~NetAccess() = default;  //NOLINT

    NetAccess(const NetAccess& other)  = delete;
    NetAccess(const NetAccess&& other) = delete;
    NetAccess& operator=(const NetAccess& other) = delete;
    NetAccess& operator=(NetAccess const&& other) = delete;

private:
    /// <serverId_serviceInstanceId -> serverObj_dataHandle>
    std::unordered_map< std::uint32_t, CallHandleAsync > mServices_{};
    std::mutex mMtxServices_{};

    // instanceId_funcId_callId -> funcData
    std::unordered_map< uint32_t, CallInfo > funcCallInfo_{};
    //
    std::mutex mMtxFuncCallInfo_{};
    std::condition_variable mCvFuncCallInfo_{};

    std::unordered_map< uint16_t, NotifyCallBack > mNotifyCallBackMap_{};
    std::mutex mMtxNotifyCallBackMap_{};

    // (serviceId + instanceId + funcId) -> cancel function
    std::unordered_map< std::uint64_t, CancelHandleFunctionCB > mCancelCallback_{};
    std::mutex mMutexCancelCallBack_{};

    // Remote service set
    std::unordered_map< std::uint32_t, ServiceStatus > mRemoteServices_{};
    std::mutex mMutexRemoteServices_{};
    // Service synchronization related
    std::atomic_bool bServicesSyncRunning_{false};
    // Record the status of service synchronization, those in the set are being synchronized, deleted from the set after synchronization is complete
    std::unordered_set< std::uint32_t > servicesSyncRunning_{};
    std::mutex mMutexServicesSyncRunning_{};

    std::uint16_t mServiceSyncInterval_{0U};  // Milliseconds

    std::vector< std::uint8_t > timeOutData_{};

    // Call timeout, unit: milliseconds
    uint16_t callTimeout_{kDefaultCallTimeout};

    // call timeout
    std::atomic_bool bCreateThreadForHandleTImeout_{false};

    std::atomic_bool stop_{false};

    // Callback task queue triggered when service is available
    std::deque< std::function< void() > > onServiceReadyTaskQueue_{};
    std::mutex mtxOnServiceReadyTaskQueue_{};
    std::atomic_bool bThreadRunningForServiceReadyTask_{false};

public:
    AccessConfig config;  //NOLINT
    nsi_app_t app{};      //NOLINT
    // for network ready
    bool ready{false};                         //NOLINT
    FuncOnReadyCallBack funcOnReady{nullptr};  //NOLINT

public:
    void SetConfig(AccessConfig const& cfg) { config = cfg; };

    /// @brief Register service callback
    /// @param[in]  serviceInstanceId  Current service instance ID
    /// @param[in]  callHandle Service callback function
    /// @return void
    /// @throw unknown
    void RegisterService(uint32_t const& serviceInstanceId, CallHandleAsync const& callHandle);

    /// @brief Register service callback
    /// @param[in]  serviceInstanceId  Current service instance ID
    /// @return void
    /// @throw unknown
    void UnregisterService(uint32_t const& serviceInstanceId);

    bool IsServiceReady(uint32_t const& serviceInstanceId) noexcept;

    // Network connection Ready
    void RegisterOnReady(FuncOnReadyCallBack const& cb);
    // Service Ready
    void RegisterOnServiceReady(uint32_t const& sii, FuncOnReadyCallBack const& cb);
    // void RegisterService(uint32_t const& sii, _HandleRequest const& cb);

    /// @brief Synchronous remote call
    /// @param[in]  funcData Function call encapsulation for a remote call
    /// @return std::future<std::vector<uint8_t>> Result data (RpcData) serialized stream, needs deserialization
    /// @throw unknown
    std::future< std::vector< uint8_t > > CallFunc(uint32_t const& sii, uint16_t const& cii, FuncData&& funcData);
    /// @brief Asynchronous remote call
    /// @param[in]  funcData Function call encapsulation for a remote call
    /// @param[in]  func Callback function for processing the result of the current remote call
    /// @return void
    /// @throw unknown
    void CallFunc(uint32_t const& ssi, uint16_t const& cii, FuncData&& funcData, FuncDataHandle&& func);  //NOLINT
    void ReturnResult(CallRequest const& callData, RetData&& retData);
    void RegisterOnCancel(uint32_t const& sii,
                          uint8_t const& funcId,
                          std::uint8_t const& callId,
                          std::function< void() > const& notifier);
    void UnregisterOnCancel(uint32_t const& sii, uint8_t const& funcId, std::uint8_t const& callId);

    void CancelFunctionCall(uint32_t const& serviceId, uint16_t const& instanceId, FuncData&& funcData);

    /// @brief Register event notification callback function
    /// @param[in]  instanceId Current object instance ID
    /// @param[in]  notify Notification callback processing function
    /// @return void
    /// @throw unknown
    void RegisterNotificationCallBack(uint16_t const& instanceId, NotifyCallBack&& notify);

    // Notification ?
    // void RegisterNotification(uint32_t const& sii, uint8_t const &eventId);
    void Notify(CallRequest const& callData);

    /// @brief Event notification call
    /// @param[in]  rpcData Function call encapsulation for a remote call
    /// @return void
    /// @throw unknown
    void Notify(RpcData const& rpcData);

    // sync services to net node
    void SyncServicesList(std::uint16_t channelId);

    void Stop() noexcept;

public:
    static int On_Message(nsi_app_t* a, void* ud, nsi_message_t* m);
    static int Dispatch(void* p);

private:
    void _HandleRequest(nsi_app_t* a, nsi_message_t* m, RpcData&& rpcData);
    void _HandleResponse(nsi_app_t* a, RpcData&& rpcData);
    void _HandleNotification(nsi_app_t* a, RpcData&& rpcData);
    void _HandleCancel(nsi_app_t* a, RpcData&& rpcData);
    void _HandleSyncInfo(nsi_app_t* a, RpcData&& rpcData);

    void _SendNotifyMsg(nsi_app_t* a, const void* buf, size_t len, std::uint16_t channelId) const;
    void _SendRequestMsg(nsi_app_t* a, const void* buf, size_t len) const;
    void _SendResponseMsg(nsi_app_t* a, nsi_message_t* req, const void* buf, size_t len) const;
    std::uint16_t _GetChannelId(std::uint32_t const& sii);

    // sync services to net node
    void _SyncServicesList();

    void _HandleTaskTimeOut();

    void _HandleServiceReadyCB();
};

}  // namespace dm
}  // namespace isoft

#endif  // __NET_ACCESS__