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
/// @brief      Tool set file methods
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/utils/file.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

namespace isoft {
namespace utils {
namespace file {

/// @brief Redefine char
using Char8_t = char;

/// @brief Slash character
constexpr Char8_t const kCharSlash{'/'};

/// @brief Wave Character
constexpr Char8_t const kCharWave{'~'};

/// @brief Modify the attributes (permissions, owner, time, etc.) of the target file to match the source file
/// @param srcFileName Source file name
/// @param dstFileName Destination file name
/// @return 0 success; <0 failure
static int32_t ChangeAttr(std::string const &srcFileName, std::string const &dstFileName) noexcept
{
    struct stat attrOfSrc
    {
    };
    if (lstat(srcFileName.c_str(), &attrOfSrc) != 0) {
        std::ostringstream oss;
        oss << "ChangeAttr(): lstat(" << srcFileName << ") failed - ";
        perror(oss.str().c_str());
        return -1;
    }

 // Modify file attributes
    if (chmod(dstFileName.c_str(), attrOfSrc.st_mode) != 0) {
        std::ostringstream oss;
        oss << "ChangeAttr(): chmod(" << dstFileName << ") failed - ";
        perror(oss.str().c_str());
        return -1;
    }

 /// No need to modify file owner and time
    return 0;
}

/// @brief Copy file to another location
/// @param srcFileName Source file
/// @param dstFileName Destination file
/// @return 0 success; <0 failure
int32_t CopyFile(std::string const &srcFileName, std::string const &dstFileName) noexcept
{
    std::ifstream srcFile{srcFileName, std::ios::binary};
    if (srcFile.fail()) {
        perror("CopyFile(): Fail to open the source file - ");
        return -1;
    }

    std::ofstream dstFile{dstFileName, std::ios::binary};
    if (dstFile.fail()) {
        perror("CopyFile(): Fail to open the dst file - ");
        srcFile.close();
        return -1;
    }

    dstFile << srcFile.rdbuf();
    dstFile.close();
    srcFile.close();
    return ChangeAttr(srcFileName, dstFileName);
}

/// @brief Create nested directories
/// @param dirName Nested directory name
/// @return 0 success; <0 failure
int32_t MakeCascadeDir(std::string const &dirName) noexcept
{
    std::string dirNameCopy{dirName};

 /// Trim leading and trailing spaces
    std::ignore = dirNameCopy.erase(0U, dirNameCopy.find_first_not_of(" "));
    std::ignore = dirNameCopy.erase(dirNameCopy.find_last_not_of(" ") + 1U);
    if (dirNameCopy.empty()) {
        return 0;
    }

 /// If it is a relative path, convert to absolute path
    if (kCharSlash != dirNameCopy[0U]) {
        if (kCharWave == dirNameCopy[0U]) {
            std::ignore = dirNameCopy.erase(0U, 1U);
            dirNameCopy = secure_getenv("HOME") + dirNameCopy;  // PRQA S 5024
        } else {
            dirNameCopy = std::string(getcwd(nullptr, 0U)) + "/" + dirNameCopy;
        }
    }

 /// If the directory already exists, return
    if (0 == access(dirNameCopy.c_str(), F_OK)) {
        return 0;
    }

    uint64_t lastPos{0U};
    std::ignore = dirNameCopy.append("/");
 /// At this point dirName is already an absolute path, so skip the first character
    std::size_t const dirNameSize{dirNameCopy.size()};
    for (std::size_t i{1U}; i < dirNameSize; i++) {
        if (kCharSlash == dirNameCopy[i]) {
 /// If encountering an empty directory, skip
            if (lastPos + 1U == i) {
                lastPos = i;
                continue;
            }

            std::string const curDir{dirNameCopy.substr(0U, i)};
            if (0 != access(curDir.c_str(), F_OK)) {
                constexpr uint32_t kMode{493U};  // 0755
                if (0 != mkdir(curDir.c_str(), kMode)) {
                    std::ostringstream oss;
                    oss << "Create directory(" << curDir << ") failed - ";
                    perror(oss.str().c_str());
                    return -1;
                }
            }

            lastPos = i;
        }
    }

    return 0;
}

/// @brief Remove directory (including files in the directory)
/// @param dirName Directory name
/// @return 0 success; <0 failure
int32_t RemoveDir(std::string const &dirName) noexcept
{
    std::string dirNameCopy{dirName};
 /// Trim leading and trailing spaces
    std::ignore = dirNameCopy.erase(0U, dirNameCopy.find_first_not_of(" "));
    std::ignore = dirNameCopy.erase(dirNameCopy.find_last_not_of(" ") + 1U);
    if (dirNameCopy.empty()) {
        return 0;
    }

 /// If it is a relative path, convert to absolute path
    if (kCharSlash != dirNameCopy[0U]) {
        if (kCharWave == dirNameCopy[0U]) {
            std::ignore = dirNameCopy.erase(0U, 1U);
            dirNameCopy = std::string{secure_getenv("HOME")} + dirNameCopy;  // PRQA S 5024
        } else {
            dirNameCopy = std::string(getcwd(nullptr, 0U)) + "/" + dirNameCopy;
        }
    }

    DIR *const dirp{opendir(dirNameCopy.c_str())};
    if (dirp == nullptr) {
        std::ostringstream oss;
        oss << "Open Dir(" << dirNameCopy << ") failed - ";
        perror(oss.str().c_str());
        return -1;
    }

    struct dirent *dir{nullptr};
    struct stat st
    {
    };
    while (true) {
        dir = readdir(dirp);  // NOLINT
        if (dir == nullptr) {
            break;
        }
        std::string const tmpDirName{dir->d_name};
        if (tmpDirName == "." || tmpDirName == "..") {
            continue;
        }

        std::string const subPath{dirNameCopy + kCharSlash + static_cast< Char8_t const * >(dir->d_name)};
        if (lstat(subPath.c_str(), &st) != 0) {
            std::ostringstream oss;
            oss << "RemoveDir(): lstat(" << subPath << ") failed - ";
            perror(oss.str().c_str());
            continue;
        }

        if (S_ISDIR(st.st_mode)) {  // PRQA S ALL
 /// If it is a directory file, recursively delete
            if (RemoveDir(subPath) != 0) {
                std::cerr << "Remove Dir(" << subPath << ") failed !!!" << std::endl;
                std::ignore = closedir(dirp);
                return -1;
            }
        } else if (S_ISREG(st.st_mode)) {  // PRQA S ALL
 /// If it is a regular file, unlink
            std::ignore = unlink(subPath.c_str());
        } else {
            std::ostringstream oss;
            oss << "RemoveDir(): Get st_mode(" << subPath << ") failed - ";
            perror(oss.str().c_str());
            continue;
        }
    }

    if (rmdir(dirNameCopy.c_str()) != 0) {
        std::ostringstream oss;
        oss << "Remove Dir(" << dirNameCopy << ") failed - ";
        perror(oss.str().c_str());
        std::ignore = closedir(dirp);
        return -1;
    }

    std::ignore = closedir(dirp);
    return 0;
}

}  // namespace file
}  // namespace utils
}  // namespace isoft