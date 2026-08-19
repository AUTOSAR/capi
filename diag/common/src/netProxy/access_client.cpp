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
/// @file       access_client.cpp
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "access_client.h"

#include <chrono>
#include <future>
#include <vector>

#include "isoft/manifestreader/manifest_reader.h"
#include "log/log.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nsomeip/net/nsi_message.h"
#include "serialization/serialization.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dic {

std::once_flag AccessClient::sS_Init_Instance_Flag_{};      //NOLINT
AccessClient *AccessClient::sS_Com_Proxy_Client_{nullptr};  //NOLINT

// constexpr nsi_method_t rpcMethdId{1};
// constexpr size_t bufSize{1024 * 2};
constexpr uint16_t kNet_Connect_Wait_Time{100};  // Network connection waiting time, unit milliseconds

using ara::diag::common::LogDebug;
using ara::diag::common::LogError;
using ara::diag::common::LogInfo;
using ara::diag::common::LogWarn;

AccessClient &AccessClient::GetComProxy()
{
    if (sS_Com_Proxy_Client_ != nullptr) {
        return *sS_Com_Proxy_Client_;
    }
    std::call_once(sS_Init_Instance_Flag_, []() { sS_Com_Proxy_Client_ = new AccessClient(); });
    return *sS_Com_Proxy_Client_;  //NOLINT
}

bool AccessClient::_Init()
{
    nai_log_core.level = NAI_LOG_DEBUG;  // NOLINT
    nai_log_set_outfn(&nai_log_core, Nai_Log_Out, nullptr);

    if (-1 == nai_init()) {
        return false;
    }

    evloop_ = nai_evloop_new();
    if (nullptr == evloop_) {
        return false;
    }

    if (-1 == nai_evloop_open(evloop_, 0, 0, "epoll")) {
        return false;
    }

    // Load configuration
    if (!_LoadConfig(config.nsiJson)) {
        return false;
    }

    if (-1 == nsi_routing_init(&routing_)) {
        return false;
    }

    if (-1 == nsi_routing_set_conf(&routing_, &conf_)) {
        return false;
    }

    if (-1 == nsi_routing_open(&routing_, evloop_, NSI_RTYPE_PROXY)) {
        return false;
    }

    if (-1 == nsi_app_init(&app)) {
        return false;
    }

    if (-1 == nsi_app_open_name(&app, &routing_, config.serverName.c_str())) {
        return false;
    }

    if (-1 == nsi_appcb_open(&appcb_, &app)) {
        return false;
    }

    _SetNsiCallBack();

    ioThread_ = new std::thread(Dispatch, evloop_);
    if (nullptr == ioThread_) {
        // log()
        return false;
    }
    ioThread_->detach();
    return true;
}

void AccessClient::_Offer()
{
    nsi_gid_t id[1];
    id[0] = config.groupId;
    nsi_event_info_t info;
    info.groups      = id;
    info.ngroup      = 1;
    info.reliability = NSI_RT_RELIABLE;
    info.type        = NSI_ET_EVENT;
    nsi_app_offer_event(&app, config.clientServiceId, config.clientInstanceId, config.eventId, &info);
    nsi_app_offer_service(&app, config.clientServiceId, config.clientInstanceId, config.versionMajor,
                          config.versionMinor);
    LogInfo() << "client offer service ->" << config.clientServiceId << ":" << config.clientInstanceId;
}

bool AccessClient::Init(AccessConfig const &cfg)
{
    // thread pool
    std::uint32_t const threadNum{10U};
    ThreadPool::GetInstance().Initialize(threadNum);
    ThreadPool::GetInstance().Start();
    // nsomeip init
    SetConfig(cfg);
    if (!_Init()) {
        LogError() << "AccessClient::Init false, error:" << nai_errno << ", app:" << &app  //NOLINT
                   << ", routing_:" << &routing_;                                          //NOLINT
        assert(false);
        return false;
    }
    return true;
}

bool AccessClient::Start(bool bStart)
{
    if (bStart) {
        // Offer();
        // request
        nsi_gid_t id[1];
        id[0] = config.groupId;
        nsi_event_info_t info;
        info.groups      = id;
        info.ngroup      = 1;
        info.reliability = NSI_RT_RELIABLE;
        info.type        = NSI_ET_EVENT;
        nsi_app_request_event(&app, config.serviceId, config.instanceId, config.eventId, &info);
        nsi_app_request_service(&app, config.serviceId, config.instanceId, config.versionMajor, config.versionMinor);
        LogDebug() << "client request service (" << config.serviceId << ":" << config.instanceId << ")";
        std::unique_lock< std::mutex > lk(mtxNetConnected_);
        cvNetConnected_.wait_for(lk, std::chrono::milliseconds(kNet_Connect_Wait_Time));
        if (bSubscribe_) {
            LogDebug() << "AccessClient::Start|net connect successed.";
        } else {
            LogWarn() << "AccessClient::Start|net connect failed in" << kNet_Connect_Wait_Time << "ms";
        }
    } else {
        LogInfo() << "Stopping...";
        // nsi_app_stop_service(&app,config.clientServiceId,config.clientInstanceId,TEST_MAJOR_VERSION,TEST_MINOR_VERSION);
        Stop();
        nsi_app_close(&app);
        nsi_routing_close(&routing_);
        nsi_conf_close(&conf_);
        nai_evloop_break(evloop_, 0);
        nai_evloop_close(evloop_);
        delete ioThread_;
    }
    return true;
}

bool AccessClient::_LoadConfig(std::string const &configJson)  //NOLINT
{
    int ret = 0;
    nsi_conf_init(&conf_);
    auto manifestRes{isoft::manifestreader::ParseManifest(configJson.c_str())};
    if (!manifestRes) {
        return false;
    }
    auto manifest{std::move(manifestRes).Value()};
    using Loader = std::function< void(ara::core::StringView const &, isoft::manifestreader::ManifestNode const &) >;
    Loader loader{[&](auto &key, auto &node) {
        if (ret != isoft::kSuccess) {
            return;
        }
        if (node.IsObject()) {
            if (!key.empty()) {
                std::ignore = nsi_conf_setopts(&conf_, "begin", key.data());
            }
            std::ignore = node.IterateObject("", loader);
            if (!key.empty()) {
                std::ignore = nsi_conf_setopts(&conf_, "end", "");
            }
        } else if (node.IsArray()) {
            std::ignore = node.IterateArray("", [&](auto idx, auto &subNode) {
                std::ignore = idx;
                loader(key, subNode);
            });
        } else if (node.IsString()) {
            ara::core::String value{};
            if (node.Load("", value) != isoft::kSuccess) {
                return;
            }
            if ((key == "netname") && (value.front() != '/')) {
                value = "/tmp/" + value;
            }
            std::ignore = nsi_conf_setopts(&conf_, key.data(), value.c_str());
        } else if (node.IsInt()) {
            int32_t value{};
            if ((ret = node.Load("", value)) != isoft::kSuccess) {
                LogError() << "AccessClient::LoadConfig | IsInt Load failed";
                return;
            }
            std::ignore = nsi_conf_setopti(&conf_, key.data(), value);
        } else if (node.IsNumber()) {
            double value{};
            if ((ret = node.Load("", value)) != isoft::kSuccess) {
                LogError() << "AccessClient::LoadConfig | IsNumber Load failed";
                return;
            }
            std::ignore = nsi_conf_setopti(&conf_, key.data(), value);
        } else {
            LogError() << "AccessClient::LoadConfig |Load invalid";
            return;
        }
    }};

    std::ignore = manifest->IterateObject("", loader);
    if (ret != isoft::kSuccess) {
        return false;
    }

    if (-1 == nsi_conf_finish(&conf_)) {
        return false;
    }
    return true;
}

void AccessClient::_SetNsiCallBack()
{
    nsi_appcb_set_ud(&appcb_, config.clientServiceId, config.clientInstanceId, this);
    nsi_appcb_set_ud(&appcb_, config.serviceId, config.instanceId, this);
    nsi_appcb_set_available(&appcb_, config.clientServiceId, config.clientInstanceId, On_Available);
    nsi_appcb_set_available(&appcb_, config.serviceId, config.instanceId, On_Available);
    nsi_appcb_set_message_any(&appcb_, config.clientServiceId, config.clientInstanceId, On_Message);
    nsi_appcb_set_message_any(&appcb_, config.serviceId, config.instanceId, On_Message);
    // nsi_appcb_set_sent_any(&appcb_,config.clientServiceId,config.clientInstanceId,on_sent);
    // nsi_appcb_set_sent_any(&appcb_,config.serviceId,config.instanceId,on_sent);
    nsi_appcb_set_subscribe(&appcb_, config.clientServiceId, config.clientInstanceId, On_Subscribe);
    nsi_appcb_set_subscribe(&appcb_, config.serviceId, config.instanceId, On_Subscribe);
    nsi_appcb_set_subscribe_ack(&appcb_, config.clientServiceId, config.clientInstanceId, On_Subscribe_Ack);
    nsi_appcb_set_subscribe_ack(&appcb_, config.serviceId, config.instanceId, On_Subscribe_Ack);
}

int AccessClient::On_Available(nsi_app_t *a,
                               void *ud,
                               int avail,
                               nsi_serv_t serv,
                               nsi_inst_t inst,
                               nsi_version_t major,
                               nsi_versmin_t minor,
                               const nsi_cred_t *cred)
{
    std::ignore = a;
    std::ignore = major;
    std::ignore = minor;
    std::ignore = cred;
    auto *c     = static_cast< AccessClient * >(ud);
    // Consider multiple instances, currently only single instance
    if (serv == c->config.serviceId && inst == c->config.instanceId && avail == NSI_AVAIL_ONLINED) {
        LogDebug() << "client receives available for (" << serv << ":" << inst << "), online";
    } else if (serv == c->config.serviceId && inst == c->config.instanceId && avail == NSI_AVAIL_OFFLINED) {
        LogDebug() << "client receives available for (" << serv << ":" << inst << "), offline";
        c->selfSubscribed_ = false;
        c->bSubscribe_     = false;
    } else if (serv == c->config.serviceId && inst == c->config.instanceId && avail == NSI_AVAIL_CHANGED) {
        LogDebug() << "client receives available for (" << serv << ":" << inst << "), changed";
    }
    bool bReady = (avail != NSI_AVAIL_OFFLINED);
    // When connection changes, set ready_ state
    if (c->ready != bReady) {
        c->ready = bReady;
    }
    // When online is received for the first time and connection changes occur, subscribe to the service
    if (!c->bSubscribe_ && (bReady == true || (avail == NSI_AVAIL_CHANGED))) {
        c->_Subscribe();
    }

    return 0;
}

int AccessClient::On_Subscribe(nsi_app_t *a,
                               void *ud,
                               nsi_cid_t cid,
                               nsi_serv_t serv,
                               nsi_inst_t inst,
                               nsi_gid_t gid,
                               nsi_eid_t eid,
                               const nsi_cred_t *cred)
{
    std::ignore = cred;
    auto *c     = static_cast< AccessClient * >(ud);
    if (serv == c->config.clientServiceId && inst == c->config.clientInstanceId) {
        LogDebug() << "client receives subscribe request for (" << serv << ":" << inst << ":" << gid << ")";
        nsi_app_subscribe_ack(a, NSI_E_OK, cid, serv, inst, gid, eid);
        c->selfSubscribed_ = true;
        if (c->selfSubscribed_) {
            c->funcOnReady(c->selfSubscribed_);
        }
    }
    return 0;
}

int AccessClient::On_Subscribe_Ack(
    nsi_app_t *a, void *ud, int rcode, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid)
{
    std::ignore = a;
    std::ignore = eid;
    auto *c     = static_cast< AccessClient * >(ud);
    if (rcode == NSI_E_OK && serv == c->config.serviceId && inst == c->config.instanceId && gid == c->config.groupId) {
        LogDebug() << "client receives subscribe ack for (" << serv << ":" << inst << ":" << gid << ")";
        c->bSubscribe_     = true;
        c->subscribeOther_ = true;
        c->selfSubscribed_ = true;
        if (c->selfSubscribed_ && nullptr != c->funcOnReady) {
            c->funcOnReady(c->selfSubscribed_);
        }
    }
    c->cvNetConnected_.notify_all();
    return 0;
}

void AccessClient::_Subscribe()
{
    nsi_app_subscribe(&app, config.serviceId, config.instanceId, config.groupId, config.versionMajor, config.eventId);
    LogDebug() << "client sends subscribe service request for (" << config.serviceId << ":" << config.instanceId << ":"
               << config.groupId << ")";
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft