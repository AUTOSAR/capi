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
/// @file       networkmanager.cpp
/// @brief      PTP management class
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/ptp/networkmanager.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger& LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief create network manager
/// @param timeDomain - time domain management handle
/// @return handle, or nullptr.
std::shared_ptr< NetworkManager > NetworkManager::CreateManager(
    std::shared_ptr< timedomain::TDManager > const& timeDomain) noexcept
{
    std::shared_ptr< NetworkManager > man;
    man = std::make_shared< NetworkManager >(timeDomain);
    if (nullptr != man) {
        if (0 != man->_init()) {
            man = nullptr;
        }
    }
    return man;
}

/// @brief initialize context
/// @return 0 - success
/// @return <0 - failure
std::int32_t NetworkManager::_init() noexcept
{
    if (nullptr == timeDomainManager_) {
        return kRET_E1;
    }
    ara::core::Map< ara::core::String, ara::core::Vector< Network::Address > > connectorMap;
    ara::core::Map< ara::core::String, config::TSTimeStampTypeEnum > timestamepModeMap;
    ara::core::Map< ara::core::String, bool > syncPhcMap;
    /// Scan all configured time domains, fill context information into ptpContext
    for (internal::TimeDomainId domainId{0U}; true == domainId.IsValid(); ++domainId) {
        config::TimeDomainSet::Domain const* const timeDomainCfg{timeDomainManager_->GetConfig(domainId)};
        if (nullptr == timeDomainCfg) {
            continue;
        }
        if (timeDomainCfg->IsSyncGlobalToPhc()) {
            if (syncPhcMap.end() == syncPhcMap.find(timeDomainCfg->GetGlobalTimeMaster().GetEthernetConnector())) {
                syncPhcMap[timeDomainCfg->GetGlobalTimeMaster().GetEthernetConnector()] = true;
            }
        }
        config::TimeDomainSet::DomainProperty const& property{timeDomainCfg->GetGlobalTimeDomainProperty()};
        // destinationPhysicalAddress
        Network::Address destAddr;
        static_cast< void >(destAddr.FromStr(property.GetDestinationPhysicalAddress().c_str()));
        // master
        if (timeDomainCfg->GetIsMasterOnThisMachine()) {
            connectorMap[timeDomainCfg->GetGlobalTimeMaster().GetEthernetConnector()].push_back(destAddr);
            timestamepModeMap[timeDomainCfg->GetGlobalTimeMaster().GetEthernetConnector()]
                = timeDomainCfg->GetTimeStampType();
            continue;
        }

        // slave
        if (false == timeDomainCfg->GetSlave().empty()) {
            if (!timeDomainCfg->GetIsMasterOnThisMachine()) {
                for (std::size_t i{0U}; i < timeDomainCfg->GetSlave().size(); ++i) {
                    if (!timeDomainCfg->GetSlave()[i].GetEthernetConnector().empty()) {
                        ara::core::String const netname{
                            isoft::osi::network::GetDeviceName(timeDomainCfg->GetSlave()[i].GetEthernetConnector())};
                        Network::Address addr;
                        if (0 == isoft::osi::network::GetMacAddress(netname.c_str(), addr)) {
                            connectorMap[timeDomainCfg->GetSlave()[i].GetEthernetConnector()].push_back(addr);
                        }
                        connectorMap[timeDomainCfg->GetSlave()[i].GetEthernetConnector()].push_back(destAddr);
                        timestamepModeMap[timeDomainCfg->GetSlave()[i].GetEthernetConnector()]
                            = timeDomainCfg->GetTimeStampType();
                        if (timeDomainCfg->IsSyncGlobalToPhc()) {
                            if (syncPhcMap.end()
                                == syncPhcMap.find(timeDomainCfg->GetSlave()[i].GetEthernetConnector())) {
                                syncPhcMap[timeDomainCfg->GetSlave()[i].GetEthernetConnector()] = true;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    std::int32_t err{0};
    for (auto const& item : connectorMap) {
        std::shared_ptr< Network > cur{std::make_shared< Network >()};
        ara::core::String const netname{isoft::osi::network::GetDeviceName(item.first)};
        if (0U == netname.length()) {
            LOG().Error() << "NetworkManager Open, cant find connector"
                          << ", connectorIP=" << item.first;
            err = -1;
            cur = nullptr;
            break;
        }
        err = cur->Open(netname, timestamepModeMap[item.first], connectorMap[item.first],
                        syncPhcMap.end() != syncPhcMap.find(item.first));
        if (0 != err) {
            LOG().Error() << "NetworkManager Open fail"
                          << ", connectorIP=" << item.first << ", ifname=" << netname;
            cur = nullptr;
            break;
        }
        networkMap_[item.first] = cur;
    }
    return err;
}

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara