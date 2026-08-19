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
/// @file       persistence_file.cpp
/// @brief      This file provides the implementation of the Data Storage class
/// @details
/// @date       2025-03-24
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "persistence_file.h"

#include <isoft/ara_fsh/platform.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "log/log.h"
#include "serialization/serialization.h"
#include "serialization/serialization/common_data_type.h"
#include "thread/therad_safe.h"

namespace ara {
namespace diag {
namespace dmd {

using ara::diag::common::LogError;
using ara::diag::common::LogInfo;
using ara::diag::common::LogWarn;

bool PersistenceFile::SaveData(const std::string& key, const std::vector< std::uint8_t >& value)
{
    std::lock_guard< std::recursive_mutex > const lock(dbMutex_);
    mapDb_[ara::core::String(key)] = value;
    LogInfo() << "PersistenceFile::SaveData|key =" << key.c_str() << "value_size =" << value.size();
    return true;
}

bool PersistenceFile::Initialize(std::string const& fileName)
{
    if (fileName.empty()) {
        LogError() << "PersistenceFile::Initialize|file_name is tmpty";
        return false;
    }

    isoft::ara_fsh::Platform platform;
    std::string dir = platform.GetAraVarDir();
    LogInfo() << "PersistenceFile::Initialize|get ara_var_dir =" << dir;

    if (dir.find("var") == std::string::npos) {
        dir = "/var/";
    }

    // Ensure the folder exists
    if (!_checkDirectoryExists(dir)) {
        LogError() << "PersistenceFile::Initialize|create dir var fail";
        return false;
    }

    dir = dir + "dmd/";

    // Ensure the folder exists
    if (!_checkDirectoryExists(dir)) {
        LogError() << "PersistenceFile::Initialize|create dir dmd fail";
        return false;
    }

    filePathName_ = dir + fileName;

    // Ensure the file exists
    if (!_checkFileExists(filePathName_)) {
        return false;
    }

    std::ifstream file(filePathName_, std::ios::binary | std::ios::ate);
    if (!file) {
        LogError() << "PersistenceFile::Initialize|open file fail, filePathName: " << filePathName_
                   << "error: " << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector< uint8_t > buffer(size);
    if (!file.read(reinterpret_cast< char* >(buffer.data()), size)) {
        LogError() << "PersistenceFile::Initialize|read file fail, filePathName: " << filePathName_
                   << "error: " << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    if (!buffer.empty()) {
        int32_t const deserializeRes = isoft::serialize::Deserialize(buffer, mapDb_);
        if (deserializeRes < 0) {
            LogError() << "PersistenceFile::Initialize|deserialize fail, filePathName: " << filePathName_
                       << "deserializeRes: " << deserializeRes;
            return false;
        }
    }

    LogInfo() << "PersistenceFile::Initialize|success filePathName = " << filePathName_
              << "buffer_size = " << buffer.size() << "key_size = " << mapDb_.size();
    return true;
}

std::vector< std::uint8_t > PersistenceFile::LoadData(const std::string& key) const
{
    std::lock_guard< std::recursive_mutex > const lock(dbMutex_);
    std::vector< std::uint8_t > tem;
    ara::core::Map< ara::core::String, ara::core::Vector< uint8_t > >::const_iterator it
        = mapDb_.find(ara::core::String(key));
    if (it == mapDb_.end()) {
        LogInfo() << "PersistenceFile::LoadData|no data key = " << key;
        return tem;
    }

    tem.insert(tem.end(), it->second.begin(), it->second.end());
    return tem;
}

bool PersistenceFile::RemoveData(const std::string& key)
{
    std::lock_guard< std::recursive_mutex > lock(dbMutex_);
    ara::core::Map< ara::core::String, ara::core::Vector< uint8_t > >::const_iterator it
        = mapDb_.find(ara::core::String(key));
    if (it == mapDb_.end()) {
        return true;
    }

    mapDb_.erase(it);
    return true;
}

bool PersistenceFile::KeyExists(std::string const& key)
{
    std::lock_guard< std::recursive_mutex > lock(dbMutex_);
    ara::core::Map< ara::core::String, ara::core::Vector< uint8_t > >::const_iterator it
        = mapDb_.find(ara::core::String(key));
    return it != mapDb_.end();
}

bool PersistenceFile::SyncData()
{
    std::lock_guard< std::recursive_mutex > lock(dbMutex_);
    /// Reason for this modification (When clearing all DTCs, mapDb_ has no elements, leading to failure to delete DTC data)
    // if (mapDb_.empty()) {
    //     LogInfo() << "PersistenceFile::SyncData|no data need fell to db, filePathName" << filePathName_;
    //     return true;
    // }
    std::ofstream file(filePathName_, std::ios::binary | std::ios::trunc);  // Overwrite write
    if (!file) {
        LogError() << "PersistenceFile::SyncData|open file fail, filePathName" << filePathName_
                   << "error: " << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    std::vector< uint8_t > buffer;
    int32_t const serializeRes = isoft::serialize::Serialize(buffer, mapDb_);
    if (serializeRes < 0) {
        LogError() << "PersistenceFile::SyncData|serialize fail, filePathName" << filePathName_
                   << "serializeRes: " << serializeRes;
        return false;
    }

    if (!file.write(reinterpret_cast< const char* >(buffer.data()), static_cast< std::streamsize >(buffer.size()))) {
        LogError() << "PersistenceFile::SyncData|wirte file fail, filePathName" << filePathName_
                   << "error: " << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    LogInfo() << "PersistenceFile::SyncData|success filePathName = " << filePathName_
              << "buffer_size = " << buffer.size() << "serializeRes = " << serializeRes
              << "key_size = " << mapDb_.size();
    return true;
}

std::vector< std::string > PersistenceFile::ListKeys() const
{
    std::lock_guard< std::recursive_mutex > lock(dbMutex_);
    std::vector< std::string > tem;
    for (auto const& pair : mapDb_) {
        tem.emplace_back(std::string(pair.first.c_str()));
    }

    return tem;
}

// Check if the folder exists, create it if not
bool PersistenceFile::_checkDirectoryExists(std::string const& path)
{
    struct stat info
    {
    };
    if (stat(path.c_str(), &info) != 0) {
        // Folder does not exist, attempt to create
        if (mkdir(path.c_str(), isoft::serialize::kInt32_0755U) != 0) {
            LogError() << "PersistenceFile::_checkDirectoryExists|can not create dir: " << path
                       << ",error: " << isoft::threadsafe::Safe_Strerror(errno);
            return false;
        }
        // std::string cmd = "mkdir -p " + path;
        // LogInfo() << "PersistenceFile::_checkDirectoryExists|create dir cmd =" << cmd;
        // int result = system(cmd.c_str());
        // if (result != 0) {
        //     LogError() << "PersistenceFile::_checkDirectoryExists|can not create dir, result =" << result
        //                << ",error: " << strerror(errno);
        //     return false;  // Command failed.
        // }
    } else if (0 == (info.st_mode & S_IFDIR)) {
        // Path exists but is not a folder
        LogError() << "PersistenceFile::_checkDirectoryExists|path exists, but is is not a dir:" << path;
        return false;
    }
    return true;
}

// Check if the file exists, create it if not
bool PersistenceFile::_checkFileExists(std::string const& path)
{
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file) {
        LogError() << "PersistenceFile::_checkFileExists|can not create file:" << path
                   << ", error: " << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }
    return true;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara