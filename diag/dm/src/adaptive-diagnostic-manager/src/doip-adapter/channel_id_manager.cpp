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
/// @file       channel_id_manager.cpp
/// @brief      This file provides the implementation of the ChannelId Management class
/// @details
/// @date       2022-08-19
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "channel_id_manager.h"

#include "log/log.h"
#include "serialization/serialization.h"

namespace ara {
namespace diag {
namespace doip {

/// @brief Maximum value of channel ID
std::uint32_t const kMaxChannelID{0xFFFFU};

/// @brief Initialization
/// @throw unknown
void ChannelIdManager::Initialize()
{
    // ara::core::InstanceSpecifier const kvIns{std::move(ara::core::StringView("dmd/root/DoIpStorage"))};
    // ara::core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > > const result_open{
    //     std::move(ara::per::OpenKeyValueStorage(kvIns))};
    // if (!result_open.HasValue()) {
    //     common::LogError() << "ChannelIdManager::Initialize|open storage fails:"
    //                        << std::move(result_open.Error().Message());
    //     return;
    // }

    // storage_ = result_open.Value();

    persistenceFilePtr_ = std::make_shared< dmd::PersistenceFile >();
    persistenceFilePtr_->Initialize("doip.db");
    // ara::core::Result< bool > const result_exist{
    //     std::move(storage_->KeyExists(std::move(ara::core::StringView("uds_channelID"))))};
    // if (!result_exist.HasValue()) {
    //     common::LogError() << "ChannelIdManager::Initialize|exists key error:"
    //                        << std::move(result_exist.Error().Message());
    //     return;
    // }

    // if (!result_exist.Value()) {
    //     return;
    // }

    if (!persistenceFilePtr_->KeyExists("uds_channelID")) {
        return;
    }

    // ara::core::Result< ara::core::Vector< uint64_t >, ara::core::ErrorCode > result_channel_id{
    //     std::move(storage_->GetValue< core::Vector< uint64_t > >(std::move(ara::core::StringView("uds_channelID"))))};
    // if (!result_channel_id.HasValue()) {
    //     common::LogError() << "ChannelIdManager::Initialize|GetValue uds_channelID but no have:"
    //                        << std::move(result_channel_id.Error().Message());
    //     return;
    // }

    std::vector< std::uint8_t > binChanneld = persistenceFilePtr_->LoadData("uds_channelID");
    if (binChanneld.empty()) {
        return;
    }
    ara::core::Vector< uint64_t > channelIdList;
    int32_t deserializeRes = isoft::serialize::Deserialize(binChanneld, channelIdList);
    if (deserializeRes < 0) {
        common::LogError() << "ChannelIdManager::Initialize|deserialize fail, deserializeRes: " << deserializeRes;
        return;
    }

    for (ara::core::Vector< uint64_t >::iterator it{channelIdList.begin()}; it != channelIdList.end(); ++it) {
        ChannelIdInfo info;
        info.channelId = *it;
        std::string peerIpKey("peer_ip_");
        std::ignore                                    = peerIpKey.append(std::to_string(*it));
        std::vector< std::uint8_t > const resultPeerIp = persistenceFilePtr_->LoadData(peerIpKey);
        if (!resultPeerIp.empty()) {
            deserializeRes = isoft::serialize::Deserialize(resultPeerIp, info.peerIp);
            if (deserializeRes < 0) {
                common::LogError() << "ChannelIdManager::Initialize|deserialize resultPeerIp fail, deserializeRes: "
                                   << deserializeRes;
                continue;
            }
            std::ignore = persistenceFilePtr_->RemoveData(peerIpKey);
        } else {
            common::LogError() << "ChannelIdManager::Initialize|read peerIpKey:" << peerIpKey.c_str() << " fails";
            continue;
        }

        std::string peerPortKey("peer_port_");
        std::ignore                                      = peerPortKey.append(std::to_string(*it));
        std::vector< std::uint8_t > const resultPeerPort = persistenceFilePtr_->LoadData(peerPortKey);
        if (!resultPeerPort.empty()) {
            deserializeRes = isoft::serialize::Deserialize(resultPeerPort, info.peerPort);
            if (deserializeRes < 0) {
                common::LogError() << "ChannelIdManager::Initialize|deserialize resultPeerPort fail, deserializeRes: "
                                   << deserializeRes;
                continue;
            }
            std::ignore = persistenceFilePtr_->RemoveData(peerPortKey);
        } else {
            common::LogError() << "ChannelIdManager::Initialize|read peerPortKey:" << peerPortKey.c_str() << " fails";
            continue;
        }

        std::string localIpKey("local_ip_");
        std::ignore                                     = localIpKey.append(std::to_string(*it));
        std::vector< std::uint8_t > const resultLocalIp = persistenceFilePtr_->LoadData(localIpKey);
        if (!resultLocalIp.empty()) {
            deserializeRes = isoft::serialize::Deserialize(resultLocalIp, info.peerIp);
            if (deserializeRes < 0) {
                common::LogError() << "ChannelIdManager::Initialize|deserialize resultLocalIp fail, deserializeRes: "
                                   << deserializeRes;
                continue;
            }
            std::ignore = persistenceFilePtr_->RemoveData(localIpKey);
        } else {
            common::LogError() << "ChannelIdManager::Initialize|read localIpKey:" << localIpKey.c_str() << " fails";
            continue;
        }

        std::string localPortKey("local_port_");
        std::ignore                                       = localPortKey.append(std::to_string(*it));
        std::vector< std::uint8_t > const resultLocalPort = persistenceFilePtr_->LoadData(localPortKey);
        if (!resultLocalPort.empty()) {
            deserializeRes = isoft::serialize::Deserialize(resultLocalPort, info.localPort);
            if (deserializeRes < 0) {
                common::LogError() << "ChannelIdManager::Initialize|deserialize resultLocalPort fail, deserializeRes: "
                                   << deserializeRes;
                continue;
            }
            std::ignore = persistenceFilePtr_->RemoveData(localPortKey);
        } else {
            common::LogError() << "ChannelIdManager::Initialize|read localPortKey:" << localPortKey.c_str() << " fails";
            continue;
        }

        std::string channelTa("ta_");
        std::ignore                                = channelTa.append(std::to_string(*it));
        std::vector< std::uint8_t > const resultTa = persistenceFilePtr_->LoadData(channelTa);
        if (!resultTa.empty()) {
            deserializeRes = isoft::serialize::Deserialize(resultTa, info.targetAdress);
            if (deserializeRes < 0) {
                common::LogError() << "ChannelIdManager::Initialize|deserialize ta_ fail, deserializeRes: "
                                   << deserializeRes;
                continue;
            }
            std::ignore = persistenceFilePtr_->RemoveData(channelTa);
        } else {
            common::LogError() << "ChannelIdManager::Initialize|read ta:" << channelTa.c_str() << " fails";
            continue;
        }

        core::String const key{info.localIp + ":" + std::to_string(static_cast< std::int32_t >(info.localPort))
                               + info.peerIp + ":" + std::to_string(static_cast< std::int32_t >(info.peerPort))};
        mapSaveChannelInfo_[key]   = info;
        mapChannelIdToInfo_[(*it)] = info;
    }

    // ara::core::Vector< uint64_t >&& channelIdVector{std::move(result_channel_id).Value()};
    // for (ara::core::Vector< uint64_t >::iterator it{std::move(channelIdVector.begin())};
    //      it != channelIdVector.end(); ++it) {
    //     ChannelIdInfo info;
    //     info.channel_id = *it;
    //     core::String peerIpKey("peer_ip_");
    //     std::ignore = peerIpKey.append(std::to_string(*it));
    //     ara::core::Result< core::String > const resultPeerIp{
    //         std::move(storage_->GetValue< core::String >(std::move(ara::core::StringView(peerIpKey.c_str()))))};
    //     if (resultPeerIp.HasValue()) {
    //         info.peer_ip = resultPeerIp.Value();
    //         std::ignore  = storage_->RemoveKey(std::move(ara::core::StringView(peerIpKey.c_str())));
    //     } else {
    //         common::LogError() << "ChannelIdManager::Initialize|read peerIpKey:" << peerIpKey.c_str() << " fails";
    //         continue;
    //     }

    //     core::String peerPortKey("peer_port_");
    //     std::ignore = peerPortKey.append(std::to_string(*it));
    //     ara::core::Result< uint32_t > const resultPeerPort{
    //         std::move(storage_->GetValue< uint32_t >(std::move(ara::core::StringView(peerPortKey.c_str()))))};
    //     if (resultPeerPort.HasValue()) {
    //         info.peer_port = static_cast< uint16_t >(resultPeerPort.Value());
    //         std::ignore    = info.peer_port;
    //         std::ignore    = storage_->RemoveKey(std::move(ara::core::StringView(peerPortKey.c_str())));
    //     } else {
    //         common::LogError() << "ChannelIdManager::Initialize|read peerPortKey:" << peerPortKey.c_str()
    //                            << " fails";
    //         continue;
    //     }

    //     core::String localIpKey("local_ip_");
    //     std::ignore = localIpKey.append(std::to_string(*it));
    //     ara::core::Result< core::String > const resultLocalIp{
    //         std::move(storage_->GetValue< core::String >(std::move(ara::core::StringView(localIpKey.c_str()))))};
    //     if (resultLocalIp.HasValue()) {
    //         info.local_ip = resultLocalIp.Value();
    //         std::ignore   = storage_->RemoveKey(std::move(ara::core::StringView(localIpKey.c_str())));
    //     } else {
    //         common::LogError() << "ChannelIdManager::Initialize|read localIpKey:" << localIpKey.c_str() << " fails";
    //         continue;
    //     }

    //     core::String localPortKey("local_port_");
    //     std::ignore = localPortKey.append(std::to_string(*it));
    //     ara::core::Result< uint32_t > const resultLocalPort{
    //         std::move(storage_->GetValue< uint32_t >(std::move(ara::core::StringView(localPortKey.c_str()))))};
    //     if (resultLocalPort.HasValue()) {
    //         info.local_port = static_cast< uint16_t >(resultLocalPort.Value());
    //         std::ignore     = storage_->RemoveKey(std::move(ara::core::StringView(localPortKey.c_str())));
    //     } else {
    //         common::LogError() << "ChannelIdManager::Initialize|read localPortKey:" << localPortKey.c_str()
    //                            << " fails";
    //         continue;
    //     }

    //     core::String channelTa("ta_");
    //     std::ignore = channelTa.append(std::to_string(*it));
    //     ara::core::Result< uint32_t > const resultTa{
    //         std::move(storage_->GetValue< uint32_t >(std::move(ara::core::StringView(channelTa.c_str()))))};
    //     if (resultTa.HasValue()) {
    //         info.ta_    = static_cast< uint16_t >(resultTa.Value());
    //         std::ignore = storage_->RemoveKey(std::move(ara::core::StringView(channelTa.c_str())));
    //     } else {
    //         common::LogError() << "ChannelIdManager::Initialize|read ta:" << channelTa.c_str() << " fails";
    //         continue;
    //     }

    //     core::String const key{std::move(info.local_ip + ":"
    //                                      + std::to_string(static_cast< std::int32_t >(info.local_port)) + info.peer_ip
    //                                      + ":" + std::to_string(static_cast< std::int32_t >(info.peer_port)))};
    //     mapSaveChannelInfo_[key]    = info;
    //     mapChannelIdToInfo_[(*it)] = info;
    // }
    // std::ignore = storage_->RemoveKey(std::move(ara::core::StringView("uds_channelID")));

    // std::ignore = storage_->SyncToStorage();

    std::ignore = persistenceFilePtr_->RemoveData("uds_channelID");
    std::ignore = persistenceFilePtr_->SyncData();
}

/// @brief Generate ChannelID
/// @param[in] localIp Local IP
/// @param[in] localPort Local port
/// @param[in] peerIp Remote IP
/// @param[in] peerPort Remote port
/// @param[out] newChannelId Output new ID
/// @return true: Need to reconnect this channel ID
/// @throw unknown
int32_t ChannelIdManager::Generate(core::String const& localIp,
                                   uint16_t const localPort,
                                   core::String const& peerIp,
                                   uint16_t const peerPort,
                                   uint64_t& newChannelId)
{
    std::ignore = newChannelId;
    core::String const key{localIp + ":" + std::to_string(static_cast< std::int32_t >(localPort)) + peerIp + ":"
                           + std::to_string(static_cast< std::int32_t >(peerPort))};

    int32_t ta = -1;
    do {
        core::Map< core::String, ChannelIdInfo >::iterator const resultSaveChannelInfo{mapSaveChannelInfo_.find(key)};
        if (resultSaveChannelInfo != mapSaveChannelInfo_.end()) {
            mapChannelInfo_[key] = resultSaveChannelInfo->second;
            std::ignore          = mapSaveChannelInfo_.erase(key);
            newChannelId         = resultSaveChannelInfo->second.channelId;
            ta                   = static_cast< int32_t >(resultSaveChannelInfo->second.targetAdress);
            break;
        }

        core::Map< core::String, ChannelIdInfo >::iterator const resultChannelInfo{mapChannelInfo_.find(key)};
        if (resultChannelInfo != mapChannelInfo_.end()) {
            newChannelId = resultChannelInfo->second.channelId;
            break;
        }
        uint64_t genChannelId{1U};
        for (; genChannelId < kMaxChannelID; genChannelId++) {
            core::Map< uint64_t, ChannelIdInfo >::iterator const it{mapChannelIdToInfo_.find(genChannelId)};
            if (it == mapChannelIdToInfo_.end()) {
                ChannelIdInfo info;
                info.channelId                    = genChannelId;
                info.localIp                      = localIp;
                info.localPort                    = localPort;
                info.peerIp                       = peerIp;
                info.peerPort                     = peerPort;
                mapChannelInfo_[key]              = info;
                mapChannelIdToInfo_[genChannelId] = info;
                newChannelId                      = genChannelId;
                break;
            }
        }
    } while (false);

    return ta;
}

/// @brief Save ChannelId
/// @param[in] channelId Value to be saved
/// @throw unknown
void ChannelIdManager::SaveChannelId(uint64_t const channelId, uint16_t const ta)
{
    core::Map< uint64_t, ChannelIdInfo >::iterator const it{mapChannelIdToInfo_.find(channelId)};
    if (it == mapChannelIdToInfo_.end()) {
        common::LogError() << "ChannelIdManager::SaveChannelId|invoid channel id : " << channelId;
        return;
    }

    // ara::core::Result< bool > const result_exist{
    //     std::move(storage_->KeyExists(std::move(ara::core::StringView("uds_channelID"))))};

    // if (!result_exist.HasValue()) {
    //     common::LogError() << "ChannelIdManager::SaveChannelId|key exists uds_channelID err:"
    //                        << std::move(result_exist.Error().Message());
    //     return;
    // }

    if (persistenceFilePtr_->KeyExists("uds_channelID")) {
        std::vector< std::uint8_t > binChanneld = persistenceFilePtr_->LoadData("uds_channelID");
        if (binChanneld.empty()) {
            return;
        }
        ara::core::Vector< uint64_t > channelIdVector;
        int32_t deserializeRes = isoft::serialize::Deserialize(binChanneld, channelIdVector);
        if (deserializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|deserialize fail, deserializeRes: "
                               << deserializeRes;
            return;
        }

        channelIdVector.push_back(channelId);
        std::vector< uint8_t > bin;
        int32_t serializeRes = isoft::serialize::Serialize(bin, channelIdVector);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize channelIdVector fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData("uds_channelID", bin);

        ChannelIdInfo const info{mapChannelIdToInfo_[channelId]};

        std::string peerIpKey("peer_ip_");
        std::ignore = peerIpKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.peerIp);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize peer_ip fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(peerIpKey, bin);

        std::string peerPortKey("peer_port_");
        std::ignore = peerPortKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.peerPort);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize peer_port_ fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(peerPortKey, bin);

        std::string localIpKey("local_ip_");
        std::ignore = localIpKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.localIp);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize localIpKey fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(localIpKey, bin);

        std::string localPortKey("local_port_");
        std::ignore = localPortKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.localPort);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize localPortKey fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(localPortKey, bin);
    } else {
        core::Vector< uint64_t > channelIdVector;
        channelIdVector.push_back(channelId);
        std::vector< uint8_t > bin;
        int32_t serializeRes = isoft::serialize::Serialize(bin, channelIdVector);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize channelIdVector fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData("uds_channelID", bin);

        ChannelIdInfo const info{mapChannelIdToInfo_[channelId]};

        std::string peerIpKey("peer_ip_");
        std::ignore = peerIpKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.peerIp);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::Initialize|serialize peer_ip fail, serializeRes: " << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(peerIpKey, bin);

        std::string peerPortKey("peer_port_");
        std::ignore = peerPortKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.peerPort);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize peer_port_ fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(peerPortKey, bin);

        std::string localIpKey("local_ip_");
        std::ignore = localIpKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.localIp);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize localIpKey fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(localIpKey, bin);

        std::string localPortKey("local_port_");
        std::ignore = localPortKey.append(std::to_string(channelId));
        bin.clear();
        serializeRes = isoft::serialize::Serialize(bin, info.localPort);
        if (serializeRes < 0) {
            common::LogError() << "ChannelIdManager::SaveChannelId|serialize localPortKey fail, serializeRes: "
                               << serializeRes;
            return;
        }
        std::ignore = persistenceFilePtr_->SaveData(localPortKey, bin);
    }

    // if (result_exist.Value()) {
    //     ara::core::Result< ara::core::Vector< uint64_t >, ara::core::ErrorCode > const result_channel_id{std::move(
    //         storage_->GetValue< core::Vector< uint64_t > >(std::move(ara::core::StringView("uds_channelID"))))};
    //     if (!result_channel_id.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|GetValue uds_channelID error: "
    //                            << result_channel_id.Error().Message();
    //         return;
    //     }
    //     ara::core::Vector< uint64_t > channelIdVector{result_channel_id.Value()};
    //     channelIdVector.push_back(channel_id);

    //     ChannelIdInfo const info{mapChannelIdToInfo_[channel_id]};

    //     core::String peerIpKey("peer_ip_");
    //     std::ignore = peerIpKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultPeerIp{std::move(
    //         storage_->SetValue< core::String >(std::move(ara::core::StringView(peerIpKey.c_str())), info.peer_ip))};
    //     if (!resultPeerIp.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set key:" << peerIpKey.c_str()
    //                            << " value: " << info.peer_ip.c_str() << " fails";
    //     }

    //     core::String peerPortKey("peer_port_");
    //     std::ignore = peerPortKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultPeerPort{std::move(storage_->SetValue< uint32_t >(
    //         std::move(ara::core::StringView(peerPortKey.c_str())), static_cast< uint32_t >(info.peer_port)))};
    //     if (!resultPeerPort.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set key:" << peerPortKey.c_str()
    //                            << " value: " << info.peer_port << " fails";
    //         std::ignore = storage_->RemoveKey(std::move(ara::core::StringView(peerPortKey.c_str())));
    //     }

    //     core::String localIpKey("local_ip_");
    //     std::ignore = localIpKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultLocalIp{std::move(
    //         storage_->SetValue< core::String >(std::move(ara::core::StringView(localIpKey.c_str())), info.local_ip))};
    //     if (!resultLocalIp.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set " << localIpKey.c_str()
    //                            << " value: " << info.local_ip.c_str() << " fails";
    //     }

    //     core::String localPortKey("local_port_");
    //     std::ignore = localPortKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultLocalPort{std::move(storage_->SetValue< uint32_t >(
    //         std::move(ara::core::StringView(localPortKey.c_str())), static_cast< uint32_t >(info.local_port)))};
    //     if (!resultLocalPort.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|read " << localPortKey.c_str()
    //                            << " value: " << info.local_port << " fails";
    //     }
    // } else {
    //     core::Vector< uint64_t > channelIdVector;
    //     channelIdVector.push_back(channel_id);
    //     std::ignore = storage_->SetValue< core::Vector< uint64_t > >(std::move(ara::core::StringView("uds_channelID")),
    //                                                                  channelIdVector);

    //     ChannelIdInfo const info{mapChannelIdToInfo_[channel_id]};

    //     core::String peerIpKey("peer_ip_");
    //     std::ignore = peerIpKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultPeerIp{std::move(
    //         storage_->SetValue< core::String >(std::move(ara::core::StringView(peerIpKey.c_str())), info.peer_ip))};
    //     if (!resultPeerIp.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set key:" << peerIpKey.c_str()
    //                            << " value: " << info.peer_ip.c_str() << " fails";
    //     }

    //     core::String peerPortKey("peer_port_");
    //     std::ignore = peerPortKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultPeerPort{std::move(storage_->SetValue< uint32_t >(
    //         std::move(ara::core::StringView(peerPortKey.c_str())), static_cast< uint32_t >(info.peer_port)))};
    //     if (!resultPeerPort.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set key:" << peerPortKey.c_str()
    //                            << " value: " << info.peer_port << " fails";
    //         std::ignore = storage_->RemoveKey(std::move(ara::core::StringView(peerPortKey.c_str())));
    //     }

    //     core::String localIpKey("local_ip_");
    //     std::ignore = localIpKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultLocalIp{std::move(
    //         storage_->SetValue< core::String >(std::move(ara::core::StringView(localIpKey.c_str())), info.local_ip))};
    //     if (!resultLocalIp.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|set " << localIpKey.c_str()
    //                            << " value: " << info.local_ip.c_str() << " fails";
    //     }

    //     core::String localPortKey("local_port_");
    //     std::ignore = localPortKey.append(std::to_string(channel_id));
    //     ara::core::Result< void > const resultLocalPort{std::move(storage_->SetValue< uint32_t >(
    //         std::move(ara::core::StringView(localPortKey.c_str())), static_cast< uint32_t >(info.local_port)))};
    //     if (!resultLocalPort.HasValue()) {
    //         common::LogError() << "ChannelIdManager::SaveChannelId|read " << localPortKey.c_str()
    //                            << " value: " << info.local_port << " fails";
    //     }
    // }

    // core::String channelTa("ta_");
    // std::ignore = channelTa.append(std::to_string(channel_id));
    // ara::core::Result< void > const resultTa{std::move(storage_->SetValue< uint32_t >(
    //     std::move(ara::core::StringView(channelTa.c_str())), static_cast< uint32_t >(ta)))};
    // if (!resultTa.HasValue()) {
    //     common::LogError() << "ChannelIdManager::SaveChannelId|set sa error" << resultTa.Error();
    // }

    // std::ignore = storage_->SyncToStorage();

    std::string channelTa("ta_");
    std::ignore = channelTa.append(std::to_string(channelId));
    std::vector< uint8_t > binTa;
    int32_t const serializeRes = isoft::serialize::Serialize(binTa, ta);
    if (serializeRes < 0) {
        common::LogError() << "ChannelIdManager::SaveChannelId|serialize channelTa fail, serializeRes: "
                           << serializeRes;
        return;
    }

    std::ignore = persistenceFilePtr_->SaveData(channelTa, binTa);
    std::ignore = persistenceFilePtr_->SyncData();
}

/// @brief Remove ChannelId
/// @param[in] channelId Value of ChannelId
/// @throw unknown
void ChannelIdManager::RemoveChannelId(uint64_t const channelId)
{
    core::Map< uint64_t, ChannelIdInfo >::iterator const it{mapChannelIdToInfo_.find(channelId)};
    if (it == mapChannelIdToInfo_.end()) {
        common::LogError() << "ChannelIdManager::RemoveChannelId|fails channelID =" << channelId;
        return;
    }

    core::String const key{it->second.localIp + ":" + std::to_string(static_cast< std::int32_t >(it->second.localPort))
                           + it->second.peerIp + ":"
                           + std::to_string(static_cast< std::int32_t >(it->second.peerPort))};
    std::ignore = mapChannelInfo_.erase(key);
    std::ignore = mapChannelIdToInfo_.erase(channelId);
}

}  // namespace doip
}  // namespace diag
}  // namespace ara