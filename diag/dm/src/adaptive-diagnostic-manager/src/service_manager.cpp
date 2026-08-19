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
/// @file       service_manager.cpp
/// @brief
/// @details
/// @date       2025-11-11
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "service_manager.h"
namespace ara {
namespace diag {
namespace dmd {

ServiceManager& ServiceManager::GetInstance() noexcept
{
    ServiceManager s_instance;
    return s_instance;
}

std::tuple< std::uint16_t, std::uint16_t, std::uint16_t > ServiceManager::Register(std::string const serviceName,
                                                                                   std::string const serviceInstanceFqn,
                                                                                   std::string const clientFqn) noexcept
{
    std::uint16_t serviceId{}, serviceInstanceId{}, clientId{};
    auto findServiceName = serviceTable_.find(serviceName);
    if (findServiceName == serviceTable_.end()) {
        ServiceInfo serviceInfo{};
        serviceId             = serviceTable_.size() + 1;
        serviceInfo.serviceId = serviceId;
        ServiceInstanceInfo serviceInstanceInfo{};
        serviceInstanceId                     = 1U;
        serviceInstanceInfo.serviceInstanceId = serviceInstanceId;
        clientId                              = 1U;
        serviceInstanceInfo.clientInstanceIdTable.insert({clientFqn, clientId});
        serviceInfo.serviceInstanceIdTable.insert({serviceInstanceFqn, serviceInstanceInfo});
    } else {
        auto findServiceInstance = findServiceName->second.serviceInstanceIdTable.find(serviceInstanceFqn);
        if (findServiceInstance == findServiceName->second.serviceInstanceIdTable.end()) {
            serviceId = findServiceName->second.serviceId;
            ServiceInstanceInfo serviceInstanceInfo{};
            serviceInstanceId                     = findServiceName->second.serviceInstanceIdTable.size() + 1;
            serviceInstanceInfo.serviceInstanceId = serviceInstanceId;
            clientId                              = 1U;
            serviceInstanceInfo.clientInstanceIdTable.insert({clientFqn, clientId});
            findServiceName->second.serviceInstanceIdTable.insert({serviceInstanceFqn, serviceInstanceInfo});
        } else {
            serviceId               = findServiceName->second.serviceId;
            serviceInstanceId       = findServiceInstance->second.serviceInstanceId;
            clientId                = 1U;
            auto findClientInstance = findServiceInstance->second.clientInstanceIdTable.find(clientFqn);
            if (findClientInstance == findServiceInstance->second.clientInstanceIdTable.end()) {
                clientId = findServiceInstance->second.clientInstanceIdTable.size() + 1U;
                findServiceInstance->second.clientInstanceIdTable.insert({clientFqn, clientId});
            }
        }
    }
    return {serviceId, serviceInstanceId, clientId};
}

std::tuple< std::uint16_t, std::uint16_t, std::uint16_t > ServiceManager::GetServiceIdAndClientId(
    std::string const serviceName, std::string const serviceInstanceFqn, std::string const clientFqn) noexcept
{
    std::uint16_t serviceId{0xFFFF}, serviceInstanceId{0xFFFF}, clientId{0xFFFF};
    auto findServiceName = serviceTable_.find(serviceName);
    if (findServiceName != serviceTable_.end()) {
        auto findServiceInstance = findServiceName->second.serviceInstanceIdTable.find(serviceInstanceFqn);
        if (findServiceInstance != findServiceName->second.serviceInstanceIdTable.end()) {
            auto findClientInstance = findServiceInstance->second.clientInstanceIdTable.find(clientFqn);
            if (findClientInstance != findServiceInstance->second.clientInstanceIdTable.end()) {
                serviceId         = findServiceName->second.serviceId;
                serviceInstanceId = findServiceInstance->second.serviceInstanceId;
                clientId          = findClientInstance->second;
            }
        }
    }
    return {serviceId, serviceInstanceId, clientId};
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara