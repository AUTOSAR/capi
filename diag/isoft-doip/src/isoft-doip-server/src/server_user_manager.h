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
/// @file       server_user_manager.h
/// @brief      This file provides DOIP server TCP/UDP manager related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_USER_MANAGER_H_
#define ISOFT_DOIP_SERVER_USER_MANAGER_H_
#include <ara/core/map.h>

#include <memory>

#include "server_user_tcp.h"
#include "server_user_udp.h"

namespace isoft {
namespace doip {

class ServerUserManager final
{
public:
    /// @brief Constructor
    ServerUserManager();

    /// @brief Destructor
    ~ServerUserManager() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    ServerUserManager(ServerUserManager const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return ServerUserManager&
    ServerUserManager& operator=(ServerUserManager const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerUserManager(ServerUserManager&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServerUserManager&
    ServerUserManager& operator=(ServerUserManager&& other) = delete;

public:
    /// @brief Get user manager singleton
    /// @return Singleton
    static ServerUserManager& GetInstance()
    {
        static ServerUserManager s_Instance;
        return s_Instance;
    }

    /// @brief Release resources
    void Release();

public:
    /// @brief Add TCP user
    /// @param[in] user TCP user object
    void AddTcpUser(std::unique_ptr< ServerUserTcp > user);

    /// @brief Remove TCP user
    /// @param[in] fd sock fd
    void RemoveTcpUser(int32_t fd);

    /// @brief Get TCP user
    /// @param[in] userFd sock fd
    /// @return tcp user
    ServerUserTcp* GetTcpUser(int32_t userFd);

    /// @brief Add UDP user
    /// @param[in] user UDP user object
    void AddUdpUser(std::unique_ptr< ServerUserUdp > user, uint8_t* data, size_t const size);

    /// @brief Remove UDP user
    /// @param[in] fd sock fd
    void RemoveUdpUser(int32_t userFd);

    /// @brief Get UDP user
    /// @param[in] userFd sock fd
    /// @return udp user
    ServerUserUdp* GetUdpUser(int32_t userFd);

    /// @brief Route activation handler
    /// @param[in] fd sock fd
    /// @param[in] sa sa
    /// @param[in] maxTesters Maximum concurrency
    /// @param[in] cb Route activation result notification callback
    void RoutingActivationHander(int32_t const fd,
                                 uint16_t const sa,
                                 uint32_t const maxTesters,
                                 std::function< void(RoutingActivationResponseType) > const& cb);

    /// @brief Get number of currently active TCP connections
    /// @return Number of active TCP connections
    size_t GetActivateTcpNumber();

private:
    /// @brief TCP management
    ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > > mapTcpUser_{};

    /// @brief UDP management
    ara::core::Map< int32_t, std::unique_ptr< ServerUserUdp > > mapUdpUser_{};
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_USER_MANAGER_H_