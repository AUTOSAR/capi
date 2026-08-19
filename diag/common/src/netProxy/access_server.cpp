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
/// @file       access_server.cpp
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "access_server.h"

#include <memory>
#include <vector>

#include "isoft/manifestreader/manifest_reader.h"
#include "log/log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nsomeip/net/nsi_message.h"
#include "serialization/serialization.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dis {
using ara::diag::common::LogDebug;
using ara::diag::common::LogError;

AccessServer &AccessServer::GetComProxy()
{
    static AccessServer s_Access_Server;
    return s_Access_Server;
}

bool AccessServer::_Init()
{
    nai_log_core.level = NAI_LOG_DEBUG;  //NOLINT
    nai_log_set_outfn(&nai_log_core, Nai_Log_Out, nullptr);
    // Initialize event loop
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

    if (-1 == nsi_routing_open(&routing_, evloop_, NSI_RTYPE_ROUTE)) {
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
    if (ioThread_ == nullptr) {
        LogError() << "io_thread create failed.";
    }
    ioThread_->detach();
    return true;
}

bool AccessServer::Init(AccessConfig const &cfg)
{
    std::uint32_t const numOfThreads{10U};
    ThreadPool::GetInstance().Initialize(numOfThreads);
    ThreadPool::GetInstance().Start();

    SetConfig(cfg);
    if (!_Init()) {
        LogError() << "AccessClient::Init false, error:" << nai_errno << ", app:" << &app  //NOLINT
                   << ", routing_:" << &routing_;                                          //NOLINT
        assert(false);
        return false;
    }
    return true;
}

void AccessServer::Offer()
{
    for (std::uint16_t eventId : config.processIdList) {
        _OfferEvent(eventId);
    }

    nsi_app_offer_service(&app, config.serviceId, config.instanceId, config.versionMajor, config.versionMinor);
    LogDebug() << "server offer service ->" << config.serviceId << ":" << config.instanceId;
}

void AccessServer::StopOffer()
{
    nsi_app_stop_service(&app, config.serviceId, config.instanceId, config.versionMajor, config.versionMinor);
}

bool AccessServer::Start(bool bStart)
{
    if (bStart) {
        LogDebug() << "Service start ...";
        Offer();

    } else {
        LogDebug() << "Service stop ...";
        Stop();
        StopOffer();
        nai_evloop_break(evloop_, 0);
        nsi_app_close(&app);
        nsi_routing_close(&routing_);
        nsi_conf_close(&conf_);
        nai_evloop_close(evloop_);

        delete ioThread_;
    }

    return true;
}

void AccessServer::Subscribe()
{
    nsi_app_subscribe(&app, config.clientServiceId, config.clientInstanceId, config.groupId, config.versionMajor,
                      NSI_EVENT_ANY);
    LogDebug() << "server subscribe service request for (" << config.clientServiceId << ":" << config.clientInstanceId
               << ":" << config.groupId << ")";
}

void AccessServer::_SetNsiCallBack()
{
    nsi_appcb_set_ud(&appcb_, config.clientServiceId, config.clientInstanceId, this);
    nsi_appcb_set_ud(&appcb_, config.serviceId, config.instanceId, this);
    nsi_appcb_set_available(&appcb_, config.clientServiceId, config.clientInstanceId, On_Available);
    nsi_appcb_set_available(&appcb_, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, On_Available);
    nsi_appcb_set_message_any(&appcb_, config.clientServiceId, config.clientInstanceId, On_Message);
    nsi_appcb_set_message_any(&appcb_, config.serviceId, config.instanceId, On_Message);
    // nsi_appcb_set_sent_any(&appcb_,config.clientServiceId,config.clientInstanceId,on_sent);
    // nsi_appcb_set_sent_any(&appcb_,config.serviceId,config.instanceId,on_sent);
    nsi_appcb_set_subscribe(&appcb_, config.clientServiceId, config.clientInstanceId, On_Subscribe);
    nsi_appcb_set_subscribe(&appcb_, config.serviceId, config.instanceId, On_Subscribe);
    nsi_appcb_set_subscribe_ack(&appcb_, config.clientServiceId, config.clientInstanceId, On_Subscribe_Ack);
    nsi_appcb_set_subscribe_ack(&appcb_, config.serviceId, config.instanceId, On_Subscribe_Ack);
}

int AccessServer::On_Available(nsi_app_t *a,
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
    auto *s     = static_cast< AccessServer * >(ud);
    if (serv == s->config.serviceId && inst == s->config.instanceId && avail == NSI_AVAIL_ONLINED) {
        LogDebug() << "server receives available for (" << serv << ":" << inst << "), online";
        s->Subscribe();
    }
    return 0;
}

int AccessServer::On_Subscribe(nsi_app_t *a,
                               void *ud,
                               nsi_cid_t cid,
                               nsi_serv_t serv,
                               nsi_inst_t inst,
                               nsi_gid_t gid,
                               nsi_eid_t eid,
                               const nsi_cred_t *cred)
{
    std::ignore = cred;
    auto *s     = static_cast< AccessServer * >(ud);
    if (serv == s->config.serviceId && inst == s->config.instanceId) {
        LogDebug() << "server receives subscribe request for (" << serv << ":" << inst << ":" << gid << ")";
        nsi_app_subscribe_ack(a, NSI_E_OK, cid, serv, inst, gid, eid);

        // sync service infos
        s->SyncServicesList(static_cast< std::uint16_t >(eid));
    }
    return 0;
}

int AccessServer::On_Subscribe_Ack(
    nsi_app_t *a, void *ud, int rcode, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid)
{
    std::ignore = a;
    auto *s     = static_cast< AccessServer * >(ud);
    if (rcode == NSI_E_OK && serv == s->config.serviceId && inst == s->config.instanceId && gid == s->config.groupId) {
        LogDebug() << "server receives subscribe ack for (" << serv << ":" << inst << ":" << gid << ":" << eid << ")";
    }
    return 0;
}

bool AccessServer::_LoadConfig(std::string const &json)  //NOLINT
{
    int ret = 0;
    nsi_conf_init(&conf_);
    auto manifestRes{isoft::manifestreader::ParseManifest(json.c_str())};
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
                LogError() << "AccessClient::_LoadConfig | IsInt Load failed";
                return;
            }
            std::ignore = nsi_conf_setopti(&conf_, key.data(), value);
        } else if (node.IsNumber()) {
            double value{};
            if ((ret = node.Load("", value)) != isoft::kSuccess) {
                LogError() << "AccessClient::_LoadConfig | IsNumber Load failed";
                return;
            }
            std::ignore = nsi_conf_setopti(&conf_, key.data(), value);
        } else {
            LogError() << "AccessClient::_LoadConfig |Load invalid";
            return;
        }
    }};

    std::ignore = manifest->IterateObject("", loader);
    if (ret != isoft::kSuccess) {
        return false;
    }
    ret = nsi_conf_finish(&conf_);
    if (ret < 0) {
        return false;
    }
    return true;
}

void AccessServer::_OfferEvent(uint16_t eventId)
{
    nsi_gid_t id[1];
    id[0] = eventId;  // groupId same as eventId
    nsi_event_info_t info;
    info.groups      = id;
    info.ngroup      = 1;
    info.reliability = NSI_RT_RELIABLE;
    info.type        = NSI_ET_EVENT;
    int nRet         = nsi_app_offer_event(&app, config.serviceId, config.instanceId, eventId, &info);
    if (nRet < 0) {
        LogDebug() << "server offer event for (" << config.serviceId << ":" << config.instanceId << ":" << eventId
                   << ":" << eventId << ") failed, error=" << nai_errno;
    }
    LogDebug() << "server offer event for (" << config.serviceId << ":" << config.instanceId << ":" << eventId << ":"
               << eventId << ")";
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft