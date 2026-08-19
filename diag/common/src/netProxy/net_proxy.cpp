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
/// @file       net_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-25
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "netProxy/net_proxy.h"

#include "access_client.h"
#include "access_server.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "log/log.h"
#include "net_access.h"

namespace isoft {
namespace dm {

NetProxy &NetProxy::GetInstance()
{
    static NetProxy s_Np;
    return s_Np;
}

bool NetProxy::Init(std::string const &configJson, std::vector< std::uint16_t > &&processIdList)
{
    ara::diag::common::LogInfo() << "json :" << configJson;
    auto manifestRes{isoft::manifestreader::ParseManifest(configJson.c_str())};
    if (!manifestRes) {
        return false;
    }
    auto manifest{std::move(manifestRes).Value()};

    AccessConfig config;
    config.nsiJson          = configJson;
    config.clientServiceId  = 2U;
    config.clientInstanceId = 1U;
    config.serviceId        = 1U;
    config.instanceId       = 1U;
    config.groupId          = 1U;
    config.eventId          = 1U;
    config.processIdList    = std::move(processIdList);
    ara::core::String name{};
    std::uint16_t appId{0};
    // service name
    int32_t ret = manifest->IterateArray(
        ara::core::StringView("application"),
        [&name, &appId](std::size_t, isoft::manifestreader::ManifestNode const &node) {  //NOLINT
            if (0 != node.Load(ara::core::StringView("name"), name)) {
                return;
            }
            ara::core::String strTmp;
            if (0 != node.Load(ara::core::StringView("client"), strTmp)) {
                return;
            }
            appId = std::stoul(strTmp.data());
        });
    if (ret != 0) {
        return false;
    }
    config.serverName = name.data();

    /*
    // service info
    ret = manifest->IterateArray(
        std::move(ara::core::StringView("service")),
        [this, &config, &ret](std::size_t, isoft::manifestreader::ManifestNode const& node) {
            ara::core::String content;
            if (0 != node.Load(std::move(ara::core::StringView("service")), content)) { return; }
            config.serviceId = std::stoul(content.data());
            if (0 != node.Load(std::move(ara::core::StringView("instance")), content)) { return; }
            config.instanceId = std::stoul(content.data());
            if (0 != node.Load(std::move(ara::core::StringView("major")), content)) { return; }
            config.versionMajor = std::stoul(content.data());
            if (0 != node.Load(std::move(ara::core::StringView("minor")), content)) { return; }
            config.versionMinor = std::stoul(content.data());
            // group id
            int group_ret = node.IterateArray(std::move(ara::core::StringView("group")),
                [this, &config](std::size_t, isoft::manifestreader::ManifestNode const& group_node) {
                    ara::core::String tmp;
                    if (0 != group_node.Load(std::move(ara::core::StringView("group")), tmp)) { return; }
                    config.groupId = std::stoul(tmp.data());
                });
            if (group_ret != 0) {
                return;
            }
        });
    if (ret != 0) {
        return false;
    }
*/
    if (config.serverName == "access_server") {
        mIsServer_  = true;
        config.role = Node_Role::kServer;
        dis::AccessServer::GetComProxy().Init(config);
        mNetAccess_ = &(dis::AccessServer::GetComProxy());
    } else {
        config.role    = Node_Role::kClient;
        config.groupId = appId;
        config.eventId = appId;
        dic::AccessClient::GetComProxy().Init(config);
        mNetAccess_ = &(dic::AccessClient::GetComProxy());
    }

    return true;
}

bool NetProxy::Init(std::string const &configJson) { return Init(configJson, {}); }

void NetProxy::Start(bool bStart) const
{
    if (mIsServer_) {
        dis::AccessServer::GetComProxy().Start(bStart);
    } else {
        dic::AccessClient::GetComProxy().Start(bStart);
    }
}

NetAccess *NetProxy::GetNetAccess() { return mNetAccess_; }

}  // namespace dm
}  // namespace isoft