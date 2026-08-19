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
/// @file       service_manager.h
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2025-11-11
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_DM_SERVER_MANAGER_H_
#define _ARA_DIAG_DM_SERVER_MANAGER_H_
#include <map>
#include <string>
#include <tuple>
namespace ara {
namespace diag {
namespace dmd {

/// @brief ServiceManager
class ServiceManager
{
public:
    /// @brief Copy constructor is prohibited
    /// @param[in]  other
    ServiceManager(ServiceManager const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    ServiceManager& operator=(ServiceManager const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in]  right
    ServiceManager(ServiceManager&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    ServiceManager& operator=(ServiceManager&& right) = delete;

    /// @brief Constructor
    ServiceManager() noexcept = default;

    /// @brief Destructor
    ~ServiceManager() = default;

    static ServiceManager& GetInstance() noexcept;

    std::tuple< std::uint16_t, std::uint16_t, std::uint16_t > Register(std::string const serviceName,
                                                                       std::string const instanceFqn,
                                                                       std::string const portFqn) noexcept;

    std::tuple< std::uint16_t, std::uint16_t, std::uint16_t > GetServiceIdAndClientId(
        std::string const serviceName, std::string const serviceInstanceFqn, std::string const clientFqn) noexcept;

private:
    struct ServiceInstanceInfo
    {
        std::uint16_t serviceInstanceId;
        std::map< std::string, std::uint16_t > clientInstanceIdTable;
    };
    struct ServiceInfo
    {
        std::uint16_t serviceId;
        std::map< std::string, ServiceInstanceInfo > serviceInstanceIdTable;
    };

    std::map< std::string, ServiceInfo > serviceTable_;
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // _ARA_DIAG_DM_SERVER_MANAGER_H_