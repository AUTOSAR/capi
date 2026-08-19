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
/// @file       persistence_service.cpp
/// @brief      This file implements persistence_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=common
/// @trace_id_sr=89658027-3194-4f45-ba8a-ac6ff8ff4591
/// @unit_name=PersistenceService
/// @unit_description=This file provides persistence_service functionality
/// @interface_level=
/// @endcode
///
/// ================================================================

#include "persistence_service.h"
namespace isoft {
namespace uds {
namespace server {

// Define persistence interface

PersistenceService::PersistenceService(std::uint16_t serverAddr) noexcept
    : persistencePtr_{nullptr}, serverAddr_{serverAddr}
{
}

void PersistenceService::Register(std::shared_ptr< PersistenceInterface > const& persistencePtr) noexcept
{
    persistencePtr_ = persistencePtr;
}

std::shared_ptr< PersistenceInterface >& PersistenceService::GetPersistenceInstance() noexcept
{
    return persistencePtr_;
}

// Save data
bool PersistenceService::SaveData(const std::string& key, const std::vector< std::uint8_t >& value) noexcept
{
    if (persistencePtr_.get() != nullptr) {
        std::string serverKey{"udsServer|" + std::to_string(static_cast< std::uint32_t >(serverAddr_)) + "|"};
        std::ignore = serverKey.append(key);
        persistencePtr_->SaveData(serverKey, value);
        return persistencePtr_->SyncData();
    }
    return false;
}

// Load data
std::vector< std::uint8_t > PersistenceService::LoadData(const std::string& key) const noexcept
{
    if (persistencePtr_.get() != nullptr) {
        std::string serverKey{"udsServer|" + std::to_string(static_cast< std::uint32_t >(serverAddr_)) + "|"};
        std::ignore = serverKey.append(key);
        return persistencePtr_->LoadData(serverKey);
    }
    return {};
}

// Delete data
bool PersistenceService::RemoveData(const std::string& key) noexcept
{
    if (persistencePtr_.get() != nullptr) {
        std::string serverKey{"udsServer|" + std::to_string(static_cast< std::uint32_t >(serverAddr_)) + "|"};
        std::ignore = serverKey.append(key);
        persistencePtr_->RemoveData(serverKey);
        return persistencePtr_->SyncData();
    }
    return false;
}

// List all keys
std::vector< std::string > PersistenceService::ListKeys() const noexcept
{
    if (persistencePtr_.get() != nullptr) {
        std::string serverKey{"udsServer|" + std::to_string(static_cast< std::uint32_t >(serverAddr_)) + "|"};
        std::vector< std::string > serverkeyList{};
        std::vector< std::string > keyList{};
        serverkeyList = persistencePtr_->ListKeys();
        for (auto&& temp : serverkeyList) {
            if (temp.compare(serverKey) > 0) {
                std::string key = temp.substr(serverKey.size());
                keyList.push_back(key);
            }
        }
        return keyList;
    }
    return {};
}

}  // namespace server
}  // namespace uds
}  // namespace isoft