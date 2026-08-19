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
/// @file       server_manager.cpp
/// @brief      This file implements server_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @trace_id_sr=e175c450-1201-44a2-b2d7-6fcf23cdd168
/// @unit_name=ServerManager
/// @unit_description=This file provides server_manager functionality
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/uds/server_manager.h"

namespace isoft {
namespace uds {
namespace server {

std::int32_t ServerManager::Register(std::shared_ptr< Server > const& serverPtr) noexcept
{
    std::unique_lock< std::mutex > const lock{serverLock_};
    {
        Address const physicalAddress{serverPtr->GetPhysicalAddress()};
        std::map< Address, std::shared_ptr< Server > >::iterator const it{serverByPa_.find(physicalAddress)};
        if (it == serverByPa_.end()) {
            std::ignore = serverByPa_.insert({physicalAddress, serverPtr});
        } else {
            return 1;
        }
    }
    {
        std::vector< Address > const& functionalAddressTable{serverPtr->GetFunctionalAddress()};
        for (auto&& functionalAddress : functionalAddressTable) {
            std::map< Address, std::list< std::shared_ptr< Server > > >::iterator const it{
                serverByFa_.find(functionalAddress)};
            if (it == serverByFa_.end()) {
                std::list< std::shared_ptr< Server > > serverList{serverPtr};
                std::ignore = serverByFa_.insert({functionalAddress, serverList});
            } else {
                it->second.push_back(serverPtr);
            }
        }
    }

    return 0;
}

std::shared_ptr< Server > ServerManager::GetServerByPa(Address const address) noexcept
{
    std::unique_lock< std::mutex > const lock{serverLock_};
    std::map< Address, std::shared_ptr< Server > >::iterator const it{serverByPa_.find(address)};
    if (it == serverByPa_.end()) {
        return {nullptr};
    }
    return it->second;
}

std::list< std::shared_ptr< Server > > ServerManager::GetServerByFa(Address const address) noexcept
{
    std::unique_lock< std::mutex > const lock{serverLock_};
    std::map< Address, std::list< std::shared_ptr< Server > > >::iterator const it{serverByFa_.find(address)};
    if (it == serverByFa_.end()) {
        return std::list< std::shared_ptr< Server > >();
    }
    return it->second;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
