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
/// @file       file.cpp
/// @brief      Utility file methods
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/loader/file.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <regex>
#include <string>
#include <vector>

namespace isoft {
namespace utils {
namespace file {

namespace {

/// @brief char redefinition
using Char8_t = char;

/// @brief Get the list of files in the specified directory. The list stores file paths.
/// @param dirPath Directory path
/// @param fullPath Whether the obtained file list names are full paths
/// @return std::vector< std::string > File list
std::vector< std::string > ReadFileList(std::string const &dirPath, bool const fullPath) noexcept  // PRQA S 2024
{
    std::vector< std::string > dirList;
    struct dirent *dirEntry{nullptr};

    DIR *const dirStream{::opendir(dirPath.data())};
    if (nullptr == dirStream) {
        return dirList;
    }

    while (true) {
        dirEntry = readdir(dirStream);  // NOLINT
        if (nullptr == dirEntry) {
            break;
        }

        std::string const dirEntryName{static_cast< Char8_t const * >(dirEntry->d_name)};
        if (dirEntryName == ".") {
            continue;
        }
        if (dirEntryName == "..") {
            continue;
        }

        if (fullPath) {
            dirList.emplace_back(dirPath + "/" + dirEntryName);
        } else {
            dirList.emplace_back(dirEntryName);
        }
    }

    std::ignore = ::closedir(dirStream);

    return dirList;
}

}  // namespace

/// @brief Search for files
/// @param dirPath Directory path
/// @param regStr Regular expression string
/// @param policy Search policy
/// @return File path
std::string Search(std::string const &dirPath, std::string const &regStr, SearchPolicy const &policy) noexcept
{
    std::string fpath;

    if (0 != access(dirPath.c_str(), F_OK | R_OK)) {
        return fpath;
    }

    struct stat stbuf
    {
    };
    if (0 != stat(dirPath.c_str(), &stbuf)) {
        return fpath;
    }

    if (!S_ISDIR(stbuf.st_mode)) {  // PRQA MS "S_ISDIR" ALL
        return fpath;
    }

    std::regex const reg{regStr};
    std::vector< std::string > fileList;
    std::vector< std::string > const allFileList{ReadFileList(dirPath, false)};
    std::ignore = std::for_each(allFileList.cbegin(), allFileList.cend(), [&fileList, &reg](auto &item) {
        if (std::regex_match(item, reg)) {
            fileList.push_back(item);
        }
    });

    if (fileList.empty()) {
        return fpath;
    }

    std::function< bool(std::string const &, std::string const &) > fLesser;
    fLesser = [](std::string const &s1, std::string const &s2) noexcept -> bool {
        if (s1.size() < s2.size()) {
            return true;
        }
        if (s1.size() == s2.size()) {
            return 0 > strncmp(s1.c_str(), s2.c_str(), s1.size());
        }
        return false;
    };

    std::function< bool(std::string const &, std::string const &) > fGreater;
    fGreater = [](std::string const &s1, std::string const &s2) noexcept -> bool {
        // Prioritize comparing length (descending)
        if (s1.size() != s2.size()) {
            return s1.size() > s2.size();  // Explicitly return boolean value
        }
        // When lengths are equal, compare lexicographically (descending)
        return s1 > s2;
    };

    // FIXME:: The Search mechanism needs improvement. Currently, v0001 is considered larger than v100.
    switch (policy) {
        case SearchPolicy::kMaxValue: {
            std::sort(fileList.begin(), fileList.end(), fGreater);  // PRQA S 3293
            break;
        }
        case SearchPolicy::kMinValue: {
            std::sort(fileList.begin(), fileList.end(), fLesser);
            break;
        }
        default: {
            break;
        }
    }
    return (dirPath + "/" + fileList[0U]);
}

}  // namespace file
}  // namespace utils
}  // namespace isoft