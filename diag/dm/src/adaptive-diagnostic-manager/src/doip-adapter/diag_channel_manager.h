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
/// @file       diag_channel_manager.h
/// @brief      This file provides the definition and implementation of the Diagnostic Connection Management class
/// @details
/// @date       2022-08-25
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_DIAG_CONNECTION_MANAGER_H_
#define ARA_DIAG_DOIP_DIAG_CONNECTION_MANAGER_H_
#include <mutex>

#include "common.h"
#include "diag_channel.h"
#include "log/log.h"
namespace ara {
namespace diag {
namespace doip {
/// @name DiagChannelPtr
/// Diagnostic connection pointer
using DiagChannelPtr = std::shared_ptr< DiagChannel >;

/// @brief Diagnostic connection management
/// Mainly used to store diagnostic connections and close diagnostic connections
class DiagChannelManager final
{
private:
    /* data */
public:
    /// @brief Constructor
    /// @throw unknown
    DiagChannelManager() noexcept = default;

    /// @brief Destructor
    /// @throw unknown
    ~DiagChannelManager() = default;

    /// @brief Copy constructor is prohibited
    /// @param[in] other
    DiagChannelManager(DiagChannelManager const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in] right
    /// @return Reference
    /// @throw unknown
    DiagChannelManager& operator=(DiagChannelManager const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in] right
    DiagChannelManager(DiagChannelManager&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in] right
    /// @return Reference
    /// @throw unknown
    DiagChannelManager& operator=(DiagChannelManager&& right) = delete;

    /// @brief Get diagnostic connection pointer by channel ID
    /// @param[in] channelId Channel ID
    /// @return Diagnostic connection pointer
    /// @throw unknown
    DiagChannelPtr GetDiagChannelPtr(UdsChannelID const channelId)
    {
        auto iter = mapDiagConnectionPtr_.find(channelId);
        if (iter == mapDiagConnectionPtr_.end()) {
            return DiagChannelPtr{nullptr};
        }
        return iter->second;
    }

    /// @brief Get diagnostic connection pointer by SA
    /// @param[in] networkId Network port ID
    /// @return Diagnostic connection pointer
    DiagChannelPtr GetDiagChannelPtr(uint16_t const sa) noexcept
    {
        for (auto&& pair : mapDiagConnectionPtr_) {
            if (pair.second.get() == nullptr) {
                continue;
            }

            if (sa != pair.second->GetSa()) {
                continue;
            }

            return pair.second;
        }

        return std::shared_ptr< DiagChannel >(nullptr);
    }

    /// @brief Get diagnostic connection pointer by network port ID and fd
    /// @param[in] networkId Network port ID
    /// @param[in] fd fd
    /// @return Diagnostic connection pointer
    DiagChannelPtr GetDiagChannelPtr(uint8_t const networkId, int32_t const fd) noexcept
    {
        for (auto&& pair : mapDiagConnectionPtr_) {
            if (pair.second.get() == nullptr) {
                continue;
            }
            uint8_t const netId  = pair.second->NetworkId();
            int32_t const sockFd = pair.second->Fd();
            if (netId != networkId || sockFd != fd) {
                continue;
            }

            return pair.second;
        }

        return std::shared_ptr< DiagChannel >(nullptr);
    }

    /// @brief Add diagnostic connection pointer
    /// @param[in] diagConnetionPtr Diagnostic connection pointer
    /// @throw unknown
    void AddDiagConnectionPtr(DiagChannelPtr const& diagConnetionPtr)
    {
        if (diagConnetionPtr.get() == nullptr) {
            common::LogError() << "DiagChannelManager::AddDiagConnectionPtr|diagConnetionPtr is nullptr";
            return;
        }
        mapDiagConnectionPtr_[diagConnetionPtr->GetIdentifier()] = diagConnetionPtr;
    }

    /// @brief Remove diagnostic connection pointer by network port ID and fd
    /// @param[in] networkId Network port ID
    /// @param[in] fd fd
    /// @throw unknown
    void RemoveDiagConnectionPtr(uint8_t const networkId, int32_t const fd)
    {
        for (auto&& pair : mapDiagConnectionPtr_) {
            if (pair.second.get() == nullptr) {
                continue;
            }
            uint8_t const netId  = pair.second->NetworkId();
            int32_t const sockFd = pair.second->Fd();
            if (netId != networkId || sockFd != fd) {
                continue;
            }

            pair.second->Stop();

            std::ignore = mapDiagConnectionPtr_.erase(pair.second->GetIdentifier());
            break;
        }
    }

    /// @brief Close all diagnostic connections
    void Stop() noexcept
    {
        for (auto&& pair : mapDiagConnectionPtr_) {
            if (pair.second.get() == nullptr) {
                continue;
            }
            pair.second->Stop();
        }
    }

private:
    /// @name mapDiagConnectionPtr_
    /// Each channel ID corresponds to one diagnostic connection
    ara::core::Map< UdsChannelID, DiagChannelPtr > mapDiagConnectionPtr_{};
};

}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_DOIP_DIAG_CONNECTION_MANAGER_H_