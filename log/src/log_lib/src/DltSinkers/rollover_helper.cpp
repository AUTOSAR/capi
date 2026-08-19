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
/// @file       rollover_helper.cpp
/// @brief      Common helpers for file-based log rollover.
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "rollover_helper.h"

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>

#include <cstdio>
#include <cstring>
#include <tuple>

namespace ara {
namespace log {
namespace internal {

std::list< std::string > RolloverHelper::GetFileList() noexcept
{
    nai_dir_t dir;
    nai_dir_init(&dir);
    std::list< std::string > logFileList;
    if (nai_dir_open(&dir, config_.logDir.c_str()) < 0) {
        return logFileList;
    }

    while (nai_dir_read(&dir) >= 0) {
        if ((dir.type & NAI_S_IFDIR) != 0U || dir.name == nullptr || strlen(dir.name) == 0) {
            continue;
        }
        std::string const filename(dir.name);
        if (MatchLoopName(filename)) {
            std::list< std::string >::iterator const it{
                std::lower_bound(logFileList.begin(), logFileList.end(), filename)};
            std::ignore = logFileList.insert(it, filename);
        }
    }
    std::ignore = nai_dir_close(&dir);
    return logFileList;
}
std::string RolloverHelper::RolloverSuffix() noexcept
{
    if (config_.compress) {
        return ".gz";
    }
    return config_.suffix;
}

bool RolloverHelper::MatchLoopName(std::string const& str) noexcept
{
    std::string const prefix{config_.cleanName + "_"};
    std::string suffix{RolloverSuffix()};

    if (str.size() < suffix.size()) {
        return false;
    }
    bool const subOK{str.substr(str.size() - suffix.size()) == suffix};
    if (str.substr(0U, prefix.size()) == prefix && subOK) {
        return true;
    }
    return false;
}

std::vector< RolloverHelper::RolloverEntry > RolloverHelper::CollectIndexedFiles(
    std::string const& rolloverSuffix) noexcept
{
    std::vector< RolloverEntry > parsed;
    std::string const prefix{config_.cleanName + "_"};

    std::list< std::string > const logFileList = GetFileList();
    for (std::string const& fname : logFileList) {
        if (fname.size() <= prefix.size() + rolloverSuffix.size()) {
            continue;
        }
        if (fname.compare(0U, prefix.size(), prefix) != 0U) {
            continue;
        }
        if (fname.compare(fname.size() - rolloverSuffix.size(), rolloverSuffix.size(), rolloverSuffix) != 0) {
            continue;
        }
        std::size_t const tsPos = fname.find('_', prefix.size());
        if (tsPos == std::string::npos) {
            continue;
        }
        std::string const idxStr = fname.substr(prefix.size(), tsPos - prefix.size());
        std::string const tsStr  = fname.substr(tsPos + 1U, fname.size() - rolloverSuffix.size() - tsPos - 1U);

        try {
            std::size_t const idx = static_cast< std::size_t >(std::stoul(idxStr));
            parsed.push_back(RolloverEntry{idx, tsStr, fname});
        } catch (...) {
            // ignore invalid entry
        }
    }

    std::sort(parsed.begin(), parsed.end(), [](RolloverEntry const& lhs, RolloverEntry const& rhs) {
        if (lhs.index == rhs.index) {
            return lhs.filename < rhs.filename;
        }
        return lhs.index < rhs.index;
    });

    return parsed;
}

bool RolloverHelper::RotateIndexedFiles(std::string const& rolloverSuffix) noexcept
{
    if (config_.loopFileCount == 0U || config_.loopFileCount == 1U) {
        return true;
    }

    bool success{true};
    std::vector< RolloverEntry > rolledList = CollectIndexedFiles(rolloverSuffix);
    /// First rename the oldest file
    std::vector< RolloverEntry >::reverse_iterator it = rolledList.rbegin();
    for (; it != rolledList.rend(); ++it) {
        std::size_t const newIndex{it->index + 1U};
        std::string const oldPath{config_.logDir + "/" + it->filename};
        /// Once this is executed, the removeOldFile function should no longer be needed; delete when updating each time.
        if (newIndex >= config_.loopFileCount) {
            if (nai_file_unlink(oldPath.c_str()) != 0) {
                success = false;
            }
            continue;
        }
        std::string name{config_.cleanName};
        name.push_back('_');
        name.append(std::to_string(newIndex));
        name.push_back('_');
        name.append(it->ts);
        name.append(rolloverSuffix);
        std::string const newPath{config_.logDir + "/" + name};

        if (rename(oldPath.c_str(), newPath.c_str()) != 0) {
            std::ignore = nai_file_unlink(oldPath.c_str());  // Delete the target file to prevent rename failure, which could cause file accumulation
            success = false;
        }
    }
    return success;
}

std::int32_t RolloverHelper::RemoveOldestFiles() noexcept
{
    std::list< std::string > filelist{GetFileList()};
    if (config_.loopFileCount == 0U) {
        for (std::string const& filename : filelist) {
            std::string const fullPath{config_.logDir + "/" + filename};
            std::ignore = nai_file_unlink(fullPath.c_str());
        }
        return 0;
    }

    if (filelist.size() > config_.loopFileCount) {
        std::size_t filesToDelete             = filelist.size() - config_.loopFileCount;
        std::list< std::string >::iterator it = filelist.begin();

        // Delete the oldest file based on timestamp order
        for (std::size_t i = 0; i < filesToDelete && it != filelist.end(); ++i, ++it) {
            std::string const filename = *it;
            std::string const fullPath{config_.logDir + "/" + filename};
            std::int32_t const ret{nai_file_unlink(fullPath.c_str())};
            std::ignore = ret;
        }
    }
    return 0;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
