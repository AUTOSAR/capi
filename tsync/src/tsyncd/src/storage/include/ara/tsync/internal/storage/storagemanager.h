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
/// @file       storagemanager.h
/// @brief      network management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Storage
/// module_path=/TimeSync/Storage
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_STORAGE_STORAGEMANAGER_H_
#define ARA_TSYNC_INTERNAL_STORAGE_STORAGEMANAGER_H_

#include <ara/core/map.h>
#include <ara/core/string_view.h>

#include "timefilestorage.h"
#include "timekvstorage.h"

namespace ara {
namespace tsync {
namespace internal {
namespace storage {

/// @brief network management class
class StorageManager final
{
public:
    /// @brief iterator declaration
    using TimeKVStorageIterator = ara::core::Map< ara::core::String, std::shared_ptr< TimeKVStorage > >::iterator;
    /// @brief iterator declaration
    using TimeFileStorageIterator = ara::core::Map< ara::core::String, std::shared_ptr< TimeFileStorage > >::iterator;

    /// @brief constructor
    /// @param timeDomain
    explicit StorageManager() = default;

    /// @brief destructor
    ~StorageManager() noexcept
    {
        storageKVMap_.clear();
        storageFileMap_.clear();
    };

    /// @brief copy constructor is prohibited
    /// @param other - other object
    StorageManager(StorageManager const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    StorageManager &operator=(StorageManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    StorageManager(StorageManager &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    StorageManager &operator=(StorageManager &&) &noexcept = default;

    /// @brief create network manager
    /// @param timeDomain - time domain management handle
    /// @return handle, or nullptr.
    static std::unique_ptr< StorageManager > CreateManager() noexcept;

    /// @brief persist time
    /// @param type type
    /// @param timebaseName time base name
    /// @param keyStr kv storage key
    /// @param userData user data
    /// @param rateDeviation rate deviation
    /// @param globalTime time
    void SetStorageTime(StorageType type,
                        ara::core::String const &timebaseName,
                        ara::core::StringView keyStrView,
                        ara::core::String const &userData,
                        double const rateDeviation,
                        std::uint64_t const globalTime) noexcept;

    /// @brief retrieve time base content from persistent storage
    /// @param timebaseName time base name
    /// @param perInstanceSpecifier persistence descriptor
    /// @param kvSpecifier kv key
    /// @param fileSpecifier file descriptor
    /// @param userData - user data
    /// @param rateDeviation - rate deviation
    /// @param globalTime - last set global time value
    /// @return true, success; false, failure;
    bool GetStorageTime(ara::core::String const &timebaseName,
                        ara::core::StringView perInstanceSpecifier,
                        ara::core::StringView kvSpecifier,
                        ara::core::StringView fileSpecifier,
                        ara::core::String &userData,
                        double &rateDeviation,
                        std::uint64_t &globalTime) noexcept;

private:
    /// @name storageKVMap_ - map of time base providers and kv persistence, only used on daemon side
    ara::core::Map< ara::core::String, std::shared_ptr< TimeKVStorage > > storageKVMap_{};

    /// @name storageFileMap_ - map of time base providers and file persistence, only used on daemon side
    ara::core::Map< ara::core::String, std::shared_ptr< TimeFileStorage > > storageFileMap_{};

};  /// class StorageManager

}  // namespace storage
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_STORAGE_STORAGEMANAGER_H_
