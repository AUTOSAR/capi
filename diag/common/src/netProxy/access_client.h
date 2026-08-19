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
/// @file       access_client.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __ACCESS_CLIENT_H__
#define __ACCESS_CLIENT_H__

#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "netProxy/rpc_define.h"
#include "net_access.h"
#include "nsomeip/core/nsi_app.h"
#include "nsomeip/core/nsi_conf.h"
#include "nsomeip/core/nsi_routing.h"

namespace isoft {
namespace dm {
namespace dic {

using isoft::dm::FuncData;
/// @brief Callback function when network is ready
using FuncOnReadyCallBack = std::function< void(bool bReady) >;
/// @brief Callback function for processing remote call return data
using FuncDataHandle = std::function< void(std::vector< uint8_t > const &data) >;
/// @brief Callback function for processing remote event notification
using NotifyCallBack = std::function< void(FuncData const &) >;

/// @brief Function call related information encapsulation, used to save call information
struct CallInfo
{
    /// @name bSync
    /// sync call or async call
    bool bSync{true};
    std::uint64_t callTime;
    std::promise< std::vector< uint8_t > > result;
    FuncDataHandle funcDataHandle;
};

class AccessClient final : public NetAccess
{
public:
    static AccessClient &GetComProxy();

    /// @brief Whether network service is ready
    /// @return bool
    /// @throw unknown
    bool Ready() { return ready; };

    /// @brief
    /// Register callback function when network service is ready, called based on current network status after Start is called
    /// @return callback Callback function when network service is ready
    /// @throw unknown
    // void RegisterOnReady(FuncOnReadyCallBack callback) { funcOnReady =
    // callback; };

    bool Init(AccessConfig const &cfg);

    /// @brief Turn current network service on/off
    /// @param[in] bStart Turn on or off; true for on, false for off
    /// @return Whether the operation was successful
    /// @throw unknown
    bool Start(bool bStart);

    AccessClient()  = default;  //NOLINT
    ~AccessClient() = default;  //NOLINT

    AccessClient(const AccessClient &other)  = delete;
    AccessClient(const AccessClient &&other) = delete;
    AccessClient &operator=(const AccessClient &other) = delete;
    AccessClient &operator=(AccessClient const &&other) = delete;

private:
    bool _Init();
    bool _LoadConfig(std::string const &configJson);
    void _SetNsiCallBack();
    void _Subscribe();

    /// @brief Event notification call
    /// @param[in]  rpcData Function call encapsulation for a remote call
    /// @return void
    /// @throw unknown
    void _Notify(RpcData const &rpcData);

private:
    static int On_Available(nsi_app_t *a,
                            void *ud,
                            int avail,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_version_t major,
                            nsi_versmin_t minor,
                            const nsi_cred_t *cred);
    static int On_Subscribe(nsi_app_t *a,
                            void *ud,
                            nsi_cid_t cid,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_gid_t gid,
                            nsi_eid_t eid,
                            const nsi_cred_t *cred);
    static int On_Subscribe_Ack(
        nsi_app_t *a, void *ud, int rcode, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid);

    void _Offer();

private:
    // // for network ready
    // bool ready{false};
    // FuncOnReadyCallBack funcOnReady{nullptr};

    static std::once_flag sS_Init_Instance_Flag_;  //NOLINT
    static AccessClient *sS_Com_Proxy_Client_;     //NOLINT

    nai_evloop_t *evloop_{nullptr};
    nsi_conf_t conf_{};
    nsi_routing_t routing_{};
    nsi_appcb_t appcb_{};
    std::thread *ioThread_{nullptr};

    bool selfSubscribed_{false};
    bool subscribeOther_{false};
    bool bSubscribe_{false};

    std::mutex mtxNetConnected_;
    std::condition_variable cvNetConnected_;
};

}  // namespace dic
}  // namespace dm
}  // namespace isoft

#endif  // __ACCESS_CLIENT_H__