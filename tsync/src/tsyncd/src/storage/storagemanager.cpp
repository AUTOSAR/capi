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
/// @file       storagemanager.cpp
/// @brief      PTP management class
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/storage/storagemanager.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace storage {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief create network manager
/// @param timeDomain - time domain management handle
/// @return handle, or nullptr.
std::unique_ptr< StorageManager > StorageManager::CreateManager() noexcept
{
    return std::make_unique< StorageManager >();
}

/// @brief persist time
/// @param type type
/// @param timebaseName time base name
/// @param keyStr kv storage key
/// @param userData user data
/// @param rateDeviation rate deviation
/// @param globalTime time
/// @return 0 - success
/// @return <0 - failure, and set errno
void StorageManager::SetStorageTime(StorageType type,
                                    ara::core::String const &timebaseName,
                                    ara::core::StringView keyStrView,
                                    ara::core::String const &userData,
                                    double const rateDeviation,
                                    std::uint64_t const globalTime) noexcept
{
    /// @traceid{SWS_TS_00212}
    /// Time persistence: when process exits, store time value to persistent storage
    if (StorageType::kStorageKv == type) {
        TimeKVStorageIterator it{storageKVMap_.find(timebaseName)};
        if (storageKVMap_.end() != it) {
            std::ignore = it->second->StoreTime(std::move(keyStrView), userData, rateDeviation, globalTime);
        }
    } else {
        TimeFileStorageIterator it{storageFileMap_.find(timebaseName)};
        if (storageFileMap_.end() != it) {
            std::ignore = it->second->StoreTime(userData, rateDeviation, globalTime);
        }
    }
}
/// @brief retrieve time base content from persistent storage
/// @param timebaseName time base name
/// @param perInstanceSpecifier persistence descriptor
/// @param kvSpecifier kv key
/// @param fileSpecifier file descriptor
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param globalTime - last set global time value
/// @return true, success; false, failure;
bool StorageManager::GetStorageTime(ara::core::String const &timebaseName,
                                    ara::core::StringView perInstanceSpecifier,
                                    ara::core::StringView kvSpecifier,
                                    ara::core::StringView fileSpecifier,
                                    ara::core::String &userData,
                                    double &rateDeviation,
                                    std::uint64_t &globalTime) noexcept
{
    if (fileSpecifier.empty()) {
        /// kv storage
        std::shared_ptr< TimeKVStorage > pTimeStorage{std::make_shared< TimeKVStorage >(perInstanceSpecifier)};
        if (!pTimeStorage->Open()) {
            LOG().Error() << "StorageManager::GetTime kvstorage open error," << perInstanceSpecifier;
            return false;
        }
        storageKVMap_[timebaseName] = pTimeStorage;
        return pTimeStorage->GetTime(kvSpecifier, userData, rateDeviation, globalTime);
    }

    /// file storage
    std::shared_ptr< TimeFileStorage > pTimeStorage{
        std::make_shared< TimeFileStorage >(perInstanceSpecifier, fileSpecifier)};
    if (!pTimeStorage->Open()) {
        LOG().Error() << "StorageManager::GetTime filestorage open error," << perInstanceSpecifier;
        return false;
    }
    storageFileMap_[timebaseName] = pTimeStorage;
    return pTimeStorage->GetTime(userData, rateDeviation, globalTime);
}

}  // namespace storage
}  // namespace internal
}  // namespace tsync
}  // namespace ara