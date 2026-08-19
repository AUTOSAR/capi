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
/// @file       server_user_manager.cpp
/// @brief      This file provides DOIP server TCP/UDP manager related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_user_manager.h"

#include "public_log.h"
#include "server_user_tcp.h"
#include "server_user_udp.h"

namespace isoft {
namespace doip {

/// @brief Constructor
ServerUserManager::ServerUserManager() = default;

/// @brief Destructor
ServerUserManager::~ServerUserManager() noexcept = default;

/// @brief Release resources
void ServerUserManager::Release()
{
    mapTcpUser_.clear();
    mapUdpUser_.clear();
}

/// @brief Add TCP user
/// @param[in] user TCP user object
void ServerUserManager::AddTcpUser(std::unique_ptr< ServerUserTcp > user)
{
    if (user.get() == nullptr) {
        LogError() << "ServerUserManager::AddTcpUser|user is null";
        return;
    }

    int32_t userFd      = user->GetFd();
    mapTcpUser_[userFd] = std::move(user);
    LogDebug() << "ServerUserManager::AddTcpUser|fd =" << userFd << "size =" << mapTcpUser_.size();
}

/// @brief Remove TCP user
/// @param[in] fd sock fd
void ServerUserManager::RemoveTcpUser(int32_t fd)
{
    ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator it = mapTcpUser_.find(fd);
    if (it == mapTcpUser_.end()) {
        return;
    }

    std::ignore = mapTcpUser_.erase(it);
    LogDebug() << "ServerUserManager::RemoveTcpUser|fd =" << fd << "size =" << mapTcpUser_.size();
}

/// @brief Get TCP user
/// @param[in] userFd sock fd
/// @return tcp user
ServerUserTcp* ServerUserManager::GetTcpUser(int32_t userFd)
{
    ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator it = mapTcpUser_.find(userFd);
    if (it == mapTcpUser_.end()) {
        return nullptr;
    }

    return it->second.get();
}

/// @brief Add UDP user
/// @param[in] user UDP user object
void ServerUserManager::AddUdpUser(std::unique_ptr< ServerUserUdp > user, uint8_t* data, size_t const size)
{
    if (user.get() == nullptr) {
        LogError() << "ServerUserManager::AddUdpUser|user is null";
        return;
    }

    int32_t userFd      = user->GetFd();
    mapUdpUser_[userFd] = std::move(user);
    LogDebug() << "ServerUserManager::AddUdpUser|fd =" << userFd << "size =" << mapUdpUser_.size();

    /// Process message
    mapUdpUser_[userFd]->Process(data, size);
}

/// @brief Remove UDP user
/// @param[in] fd sock fd
void ServerUserManager::RemoveUdpUser(int32_t fd)
{
    ara::core::Map< int32_t, std::unique_ptr< ServerUserUdp > >::iterator it = mapUdpUser_.find(fd);
    if (it == mapUdpUser_.end()) {
        return;
    }

    std::ignore = mapUdpUser_.erase(it);
    LogDebug() << "ServerUserManager::RemoveUdpUser|fd =" << fd << "size =" << mapUdpUser_.size();
}

/// @brief Get UDP user
/// @param[in] userFd sock fd
/// @return udp user
ServerUserUdp* ServerUserManager::GetUdpUser(int32_t userFd)
{
    ara::core::Map< int32_t, std::unique_ptr< ServerUserUdp > >::iterator it = mapUdpUser_.find(userFd);
    if (it == mapUdpUser_.end()) {
        return nullptr;
    }

    return it->second.get();
}

/// @brief Route activation handler (iso-13400 Figure 27)
/// @param[in] fd sock fd
/// @param[in] sa sa
/// @param[in] maxTesters Maximum concurrency
/// @param[in] cb Route activation result notification callback
void ServerUserManager::RoutingActivationHander(int32_t const fd,
                                                uint16_t const sa,
                                                uint32_t const maxTesters,
                                                std::function< void(RoutingActivationResponseType) > const& cb)
{
    ServerUserTcp* tcpUser = GetTcpUser(fd);
    if (tcpUser == nullptr) {
        LogError() << "ServerUserManager::RoutingActivationHander|tcpUser is null";
        return;
    }

    do {
        /// No active TCP connection currently, allocate SA directly
        size_t const activateSockets = GetActivateTcpNumber();
        if (activateSockets == 0U) {
            break;
        }

        /// Check if current socket is already registered
        ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator itFind = mapTcpUser_.find(fd);
        if (itFind != mapTcpUser_.end() && itFind->second.get() != nullptr
            && itFind->second->GetState() == DoipConnectionState::kActive && itFind->second->GetSa() == sa) {
            break;
        }

        /// DoIP-106
        if (itFind != mapTcpUser_.end() && itFind->second.get() != nullptr
            && itFind->second->GetState() == DoipConnectionState::kActive && itFind->second->GetSa() != sa) {
            cb(RoutingActivationResponseType::kBusy);
            return;
        }

        /// Check if SA is registered to another socket
        ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator itCheckSa = mapTcpUser_.begin();
        for (; itCheckSa != mapTcpUser_.end(); ++itCheckSa) {
            if (itCheckSa->second.get() == nullptr) {
                continue;
            }

            if (itCheckSa->second->GetState() != DoipConnectionState::kActive) {
                continue;
            }

            if (itCheckSa->second->GetSa() != sa) {
                continue;
            }

            std::function< void(bool const) > cb1 = [this, sa, fd, maxTesters, cb](bool const isAlive) {
                ServerUserTcp* user = GetTcpUser(fd);
                if (user == nullptr) {
                    LogError() << "ServerUserManager::RoutingActivationHander|cb1 user is null fd =" << fd;
                    return;
                }
                if (isAlive) {
                    cb(RoutingActivationResponseType::kAlreadyRegisteredTester);
                } else {
                    size_t const nowActivateSockets = GetActivateTcpNumber();
                    if (nowActivateSockets < maxTesters) {
                        user->SetSa(sa);
                        cb(RoutingActivationResponseType::kSuccessful);
                    } else {
                        cb(RoutingActivationResponseType::kNoSocketAvailable);
                    }
                }
            };
            itCheckSa->second->DoAliveCheckRequest(cb1, fd);
            return;
        }

        /// Check if there are still available resources
        if (activateSockets < maxTesters) {
            break;
        }
        std::function< void(bool const) > cb2 = [this, sa, fd, maxTesters, cb](bool const isAlive) {
            std::ignore         = isAlive;
            ServerUserTcp* user = GetTcpUser(fd);
            if (user == nullptr) {
                LogError() << "ServerUserManager::RoutingActivationHander|cb2 user is null fd =" << fd;
                return;
            }

            uint16_t const leftAliveCheckNum{static_cast< uint16_t >(user->GetAliveCheckNum() - 1U)};
            user->SetAliveCheckNum(leftAliveCheckNum);

            if (user->GetAliveCheckNum() <= 0U) {
                size_t const nowActivateSockets = GetActivateTcpNumber();
                if (nowActivateSockets < maxTesters) {
                    user->SetSa(sa);
                    cb(RoutingActivationResponseType::kSuccessful);
                } else {
                    cb(RoutingActivationResponseType::kNoSocketAvailable);
                }
            }
        };

        uint16_t aliveCheckNum = 0U;
        for (ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator itCalculate = mapTcpUser_.begin();
             itCalculate != mapTcpUser_.end(); ++itCalculate) {
            if (itCalculate->second.get() == nullptr) {
                continue;
            }

            if (itCalculate->second->GetState() != DoipConnectionState::kActive) {
                continue;
            }

            itCalculate->second->DoAliveCheckRequest(cb2, fd);
            ++aliveCheckNum;
        }
        tcpUser->SetAliveCheckNum(aliveCheckNum);
        return;
    } while (false);

    tcpUser->SetSa(sa);
    cb(RoutingActivationResponseType::kSuccessful);
}

/// @brief Get number of currently active socket connections
/// @return Number of active sockets
size_t ServerUserManager::GetActivateTcpNumber()
{
    size_t num{0U};
    for (ara::core::Map< int32_t, std::unique_ptr< ServerUserTcp > >::iterator it = mapTcpUser_.begin();
         it != mapTcpUser_.end(); ++it) {
        if (it->second.get() == nullptr) {
            continue;
        }

        if (it->second->GetState() != DoipConnectionState::kActive) {
            continue;
        }

        ++num;
    }

    return num;
}

}  // namespace doip
}  // namespace isoft