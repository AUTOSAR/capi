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
/// @file       persistence_file.h
/// @brief      This file provides the Data Storage class
/// @details
/// @date       2025-03-24
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_DM_PERSISTENCE_FILE_H_
#define _ARA_DIAG_DM_PERSISTENCE_FILE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/uds/persistence_interface.h>

#include <memory>
#include <mutex>

// #include <getopt.h>

#include "ara/core/string.h"
namespace ara {
namespace diag {
namespace dmd {

using isoft::uds::server::PersistenceInterface;
class PersistenceFile : public PersistenceInterface
{
public:
    PersistenceFile(PersistenceFile const&) = delete;
    PersistenceFile(PersistenceFile&&)      = delete;
    PersistenceFile& operator=(PersistenceFile const&) = delete;
    PersistenceFile& operator=(PersistenceFile&&) = delete;

    PersistenceFile() = default;

    ~PersistenceFile() override = default;

    bool Initialize(std::string const& fileName);

    // Save data
    bool SaveData(const std::string& key, const std::vector< std::uint8_t >& value) override;

    // Load data
    std::vector< std::uint8_t > LoadData(const std::string& key) const override;

    // Delete data
    bool RemoveData(const std::string& key) override;

    /// @brief Whether the key exists
    /// @param key Key value
    /// @return true: Corresponding key exists
    bool KeyExists(std::string const& key) override;

    /// @brief Data persistence to disk
    /// @return true: Success
    bool SyncData() override;

    // List all keys
    std::vector< std::string > ListKeys() const override;

private:
    // Check if the folder exists, create it if not
    bool _checkDirectoryExists(std::string const& path);

    // Check if the file exists, create it if not
    bool _checkFileExists(std::string const& path);

private:
    /// @brief File name
    std::string filePathName_;
    /// @brief Lock
    mutable std::recursive_mutex dbMutex_;
    /// @brief Storage content  key: Key value  value: Data corresponding to the key
    ara::core::Map< ara::core::String, ara::core::Vector< uint8_t > > mapDb_;
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif