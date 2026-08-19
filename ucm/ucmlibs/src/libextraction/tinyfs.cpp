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
/// @file       tinyfs.cpp
/// @brief      tiny fs
/// @details
/// @date       2024-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=tinyfs
/// @unit_description=Provide tiny file system functionality
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/extraction/tinyfs.h"

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>

#include <array>
#include <functional>

#include "ara/ucm/internal/extraction/alias.h"
#include "ara/ucm/internal/extraction/assert.h"
#include "ara/ucm/internal/extraction/log.h"
#include "unzip.h"

////////////////////////////////////////////////
////////init something here/////////////////////
////////////////////////////////////////////////
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define TINY_FS_DIR_TRAVERSE_BEGIN(path, failedResult)                                                                 \
    nai_dir_t d{nullptr, nullptr, 0};                                                                                  \
                                                                                                                       \
    if (nai_dir_open(&d, (path).c_str()) < 0) {                                                                        \
        return failedResult;                                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    for (;;) {                                                                                                         \
        if (nai_dir_read(&d) < 0) {                                                                                    \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        if (0 == strncmp(d.name, ".", 1U)) {                                                                           \
            continue;                                                                                                  \
        }                                                                                                              \
        if (0 == strncmp(d.name, "..", 2U)) {                                                                          \
            continue;                                                                                                  \
        }
////////////////////////////////////////////////
////////do something here///////////////////////
////////////////////////////////////////////////
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define TINY_FS_DIR_TRAVERSE_END()                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    std::ignore = nai_dir_close(&d);                                                                                   \
    d           = nai_dir_t { nullptr, nullptr, 0 }
////////////////////////////////////////////////
////////finish something here///////////////////
////////////////////////////////////////////////

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace tinyfs {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00322
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kIFMT{static_cast< std::uint32_t >(NAI_S_IFMT)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00118,DD_UCM_00323
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kIFREG{static_cast< std::uint32_t >(NAI_S_IFREG)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00119,DD_UCM_00325
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kIFDIR{static_cast< std::uint32_t >(NAI_S_IFDIR)};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00120,DD_UCM_00327
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kIFLNK{static_cast< std::uint32_t >(NAI_S_IFLNK)};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129,DD_UCM_00140
/// @needwork = dd
/// @endcode
constexpr std::int32_t kOpenFlagRead{NAI_O_RDONLY};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129,DD_UCM_00139,DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr std::int32_t kOpenFlagWrite{NAI_O_CREAT | NAI_O_TRUNC | NAI_O_WRONLY};
///constexpr std::int32_t kMode0644{0644};  // 0644
///constexpr std::int32_t kMode0666{0666};  // 0666
///constexpr std::int32_t kMode0755{0755};  // 0755
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr std::int32_t kMode0644{420};  // 0644
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00139
/// @needwork = dd
/// @endcode
constexpr std::int32_t kMode0666{438};  // 0666
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130, DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr std::int32_t kMode0755{493};  // 0755

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130, DD_UCM_00133, DD_UCM_00135, DD_UCM_00137, DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr std::size_t kFilePathLen{512U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129, DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr std::size_t kFileReadLen{8192U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00140
/// @needwork = dd
/// @endcode
constexpr std::size_t kMD5DigestLen{16U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130, DD_UCM_00133, DD_UCM_00135, DD_UCM_00137, DD_UCM_00140, DD_UCM_00141
/// @needwork = dd
/// @endcode
constexpr Char_T kChar0{'\0'};

/// @brief sJoinPath
/// @param path
/// @param subName
/// @return joined path
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00329
/// @needwork = dd
/// @endcode
inline static AraString SJoinPath(AraString const& path, Char_T const* const subName)
{
    return path + AraString(1U, nai_path_sep) + subName;
}

/// @brief sIsFdValid
/// @param fd
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00330
/// @needwork = no
/// @endcode
inline static bool SIsFdValid(nai_fd_t const fd) noexcept { return (static_cast< nai_fd_t >(-1) != fd); }

/// @brief sMatchType
/// @param mode
/// @param mask
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00331
/// @needwork = no
/// @endcode
inline static bool SMatchType(std::uint32_t const mode, std::uint32_t const mask) noexcept
{
    return ((mode & kIFMT) == mask);
}
/// @brief sMatchPathStat
/// @param path
/// @param mask
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00332
/// @needwork = no
/// @endcode
static bool SMatchPathStat(AraString const& path, std::uint32_t const mask)
{
    nai_int_t r{0};
    nai_stat_t st{};

    r = nai_stat(path.c_str(), &st, NAI_STAT_BASIC | NAI_STAT_PERM);
    if (r < 0) {
        return false;
    }

    return SMatchType(st.st_mode, mask);
}
/// @brief sIsReg
/// @param mode unsigned mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00323
/// @needwork = dd
/// @endcode
inline static bool SIsReg(std::uint32_t const mode) noexcept { return SMatchType(mode, kIFREG); }
/// @brief sIsReg
/// @param mode signed mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00324
/// @needwork = dd
/// @endcode
inline static bool SIsReg(std::int32_t const mode) noexcept { return SIsReg(static_cast< std::uint32_t >(mode)); }
/// @brief sIsDir
/// @param mode unsigned mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00325
/// @needwork = dd
/// @endcode
inline static bool SIsDir(std::uint32_t const mode) noexcept { return SMatchType(mode, kIFDIR); }
/// @brief sIsDir
/// @param mode signed mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00326
/// @needwork = dd
/// @endcode
inline static bool SIsDir(std::int32_t const mode) noexcept { return SIsDir(static_cast< std::uint32_t >(mode)); }
/// @brief sIsLnk
/// @param mode unsigned mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00327
/// @needwork = dd
/// @endcode
inline static bool SIsLnk(std::uint32_t const mode) noexcept { return SMatchType(mode, kIFLNK); }
/// @brief sIsLnk
/// @param mode signed mode
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00328
/// @needwork = dd
/// @endcode
inline static bool SIsLnk(std::int32_t const mode) noexcept { return SIsLnk(static_cast< std::uint32_t >(mode)); }

/// @brief IsExist
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00117
/// @needwork = ad
/// @endcode
bool IsExist(AraString const& path)
{
    nai_stat_t st;
    return nai_stat(path.c_str(), &st, NAI_STAT_BASIC | NAI_STAT_PERM) >= 0;
}
/// @brief IsRegularFile
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00118
/// @needwork = dd
/// @endcode
bool IsRegularFile(AraString const& path) { return SMatchPathStat(path, kIFREG); }
/// @brief IsDirectory
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00119
/// @needwork = dd
/// @endcode
bool IsDirectory(AraString const& path) { return SMatchPathStat(path, kIFDIR); }
/// @brief IsSymbolLink
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00120
/// @needwork = dd
/// @endcode
bool IsSymbolLink(AraString const& path) { return SMatchPathStat(path, kIFLNK); }

/// @brief GetFiles
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00121
/// @needwork = dd
/// @endcode
AraVectorString GetFiles(AraString const& path, bool const useFullPath)
{
    AraVectorString files;
    AraString file;

    TINY_FS_DIR_TRAVERSE_BEGIN(path, files)
    {
        if (SIsReg(d.type)) {
            file = d.name;
            if (useFullPath) {
                file = SJoinPath(path, d.name);
            }
            files.push_back(file);
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    return files;
}
/// @brief GetFilesRecursive
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00122
/// @needwork = dd
/// @endcode
AraVectorString GetFilesRecursive(AraString const& path, bool const useFullPath)
{
    AraVectorString files;
    AraString file;
    AraVectorString subfiles;

    TINY_FS_DIR_TRAVERSE_BEGIN(path, files)
    {
        if (SIsReg(d.type)) {
            file = d.name;
            if (useFullPath) {
                file = SJoinPath(path, d.name);
            }
            files.push_back(file);

        } else if (SIsDir(d.type)) {
            subfiles = GetFilesRecursive(SJoinPath(path, d.name), useFullPath);
            for (AraString const& it : subfiles) {
                files.push_back(it);
            }
        } else {
            ;
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    return files;
}
/// @brief GetSubdirs
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00123
/// @needwork = dd
/// @endcode
AraVectorString GetSubdirs(AraString const& path, bool const useFullPath)
{
    AraVectorString subdirs;
    AraString subdir;

    TINY_FS_DIR_TRAVERSE_BEGIN(path, subdirs)
    {
        if (SIsDir(d.type)) {
            subdir = d.name;
            if (useFullPath) {
                subdir = SJoinPath(path, d.name);
            }
            subdirs.push_back(subdir);
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    return subdirs;
}

/// @brief GetFileSize
/// @param path
/// @return File Size
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00124
/// @needwork = dd
/// @endcode
std::uint64_t GetFileSize(AraString const& path)
{
    std::uint64_t size{0U};
    nai_int_t r{0};
    nai_stat_t st;

    r = nai_stat(path.c_str(), &st, NAI_STAT_BASIC | NAI_STAT_PERM);
    if (r < 0) {
        return size;
    }

    size = st.st_size;
    return size;
}
/// @brief GetDirectorySize
/// @param path
/// @return Directory Size
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00125
/// @needwork = dd
/// @endcode
std::uint64_t GetDirectorySize(AraString const& path)
{
    std::uint64_t dirSize{0U};

    TINY_FS_DIR_TRAVERSE_BEGIN(path, dirSize)
    {
        if (SIsReg(d.type)) {
            dirSize += GetFileSize(SJoinPath(path, d.name));
        } else if (SIsDir(d.type)) {
            dirSize += GetDirectorySize(SJoinPath(path, d.name));
        } else {
            ;  ///////////other file must be cal into dirSize///////////////////////////mytodo////
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    return dirSize;
}

/// @brief CreateDirectory
/// @param path
/// @param mode
/// @param recursive
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00126
/// @needwork = dd
/// @endcode
bool CreateDirectory(AraString const& path, std::int32_t const mode, bool const recursive)
{
    return (nai_dir_create(path.c_str(), mode, static_cast< std::int32_t >(recursive)) >= 0);
}
/// @brief RemoveFile
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00127
/// @needwork = dd
/// @endcode
bool RemoveFile(AraString const& path) { return (nai_file_unlink(path.c_str()) >= 0); }
/// @brief RemoveDirectory
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00128
/// @needwork = dd
/// @endcode
bool RemoveDirectory(AraString const& path)
{
    bool success{true};

    TINY_FS_DIR_TRAVERSE_BEGIN(path, success)
    {
        if (SIsDir(d.type)) {
            if (!RemoveDirectory(SJoinPath(path, d.name))) {
                success = false;
            }
        } else {
            if (!RemoveFile(SJoinPath(path, d.name))) {  ///////Can any other type of file be deleted?//////
                success = false;
            }
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    if (nai_dir_remove(path.c_str()) < 0) {
        success = false;
    }

    return success;
}

/// @brief sCopySymbolLink
/// @param srcLink
/// @param dstLink
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130
/// @needwork = dd
/// @endcode
static bool SCopySymbolLink(AraString const& srcLink, AraString const& dstLink)
{
    if (IsExist(dstLink)) {
        return false;
    }

    std::array< Char_T, kFilePathLen > originPath{};
    originPath.fill(kChar0);

    intptr_t const readRet{nai_path_readlink(srcLink.c_str(), originPath.data(), originPath.size())};
    if (readRet < 0) {
        return false;
    }

    // TODO(hanzhibo): if originPath is empty, continue to create link??

    // need not copy file mode

    nai_int_t const linkRet{nai_path_symlink(originPath.data(), dstLink.c_str())};
    return linkRet >= 0;
}
/// @brief sCopyFileMode
/// @param srcPath
/// @param dstPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129
/// @needwork = dd
/// @endcode
static bool SCopyFileMode(AraString const& srcPath, AraString const& dstPath)
{
    nai_int_t ret{0};
    nai_stat_t st;

    ret = nai_stat(srcPath.c_str(), &st, NAI_STAT_BASIC | NAI_STAT_PERM);
    if (ret < 0) {
        return false;
    }

    ret = nai_path_set_mode(dstPath.c_str(), static_cast< nai_int_t >(st.st_mode));
    return ret >= 0;
}
/// @brief CopyFile
/// @param srcPath
/// @param dstPath
/// @param overWrite
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129
/// @needwork = dd
/// @endcode
bool CopyFile(AraString const& srcPath, AraString const& dstPath, bool const overWrite)
{
    bool success{false};
    intptr_t rret{-1};
    intptr_t wret{-1};
    std::size_t nread{0U};
    std::array< std::uint8_t, kFileReadLen > buf{};

    if (IsExist(dstPath)) {
        if (!overWrite) {
            return success;
        }

        if (!IsRegularFile(dstPath)) {
            return success;
        }
    }

    nai_fd_t const sfd{nai_file_open(srcPath.c_str(), kOpenFlagRead)};
    if (!SIsFdValid(sfd)) {
        return success;
    }

    nai_fd_t const dfd{nai_file_open(dstPath.c_str(), kOpenFlagWrite)};
    if (!SIsFdValid(dfd)) {
        std::ignore = nai_file_close(sfd);
        return success;
    }

    if (!SCopyFileMode(srcPath, dstPath)) {
        std::ignore = nai_file_close(sfd);
        std::ignore = nai_file_close(dfd);
        return success;
    }

    do {
        /// clear buf
        success     = true;
        std::ignore = nai_memset(buf.data(), 0, buf.size());

        /// read
        rret = nai_file_read(sfd, buf.data(), buf.size());
        if (rret == 0) {
            break;
        }
        if (rret < 0) {
            success = false;
            break;
        };

        /// write
        nread = static_cast< size_t >(rret);
        wret  = nai_file_write(dfd, buf.data(), nread);
        if (static_cast< size_t >(wret) != nread) {
            success = false;
            break;
        }

        /// finish
        if (nread != buf.size()) {
            break;
        }
    } while (true);

    std::ignore = nai_file_close(sfd);
    std::ignore = nai_file_close(dfd);
    return success;
}
/// @brief CopyDirectory
/// @param srcPath
/// @param dstPath
/// @param overWrite
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130
/// @needwork = dd
/// @endcode
bool CopyDirectory(AraString const& srcPath, AraString const& dstPath, bool const overWrite)
{
    bool success{false};

    if (IsExist(dstPath)) {
        if (!overWrite) {
            return success;
        }
        if (!IsDirectory(dstPath)) {
            return success;
        }
    } else {
        if (!CreateDirectory(dstPath, kMode0755, true)) {
            return success;
        }
    }

    // Directory may be empty
    success = true;
    TINY_FS_DIR_TRAVERSE_BEGIN(srcPath, success)
    {
        if (SIsReg(d.type)) {
            success = CopyFile(SJoinPath(srcPath, d.name), SJoinPath(dstPath, d.name), overWrite);

        } else if (SIsDir(d.type)) {
            success = CopyDirectory(SJoinPath(srcPath, d.name), SJoinPath(dstPath, d.name), overWrite);

        } else if (SIsLnk(d.type)) {
            success = SCopySymbolLink(SJoinPath(srcPath, d.name), SJoinPath(dstPath, d.name));
        } else {
            ;
        }

        if (!success) {
            return success;
        }
    }
    TINY_FS_DIR_TRAVERSE_END();

    return success;
}

/// @brief MoveFile
/// @param srcPath
/// @param dstPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00131
/// @needwork = dd
/// @endcode
bool MoveFile(AraString const& srcPath, AraString const& dstPath)
{
    return (nai_path_rename(srcPath.c_str(), dstPath.c_str()) >= 0);
}
/// @brief MoveDirectory
/// @param srcPath
/// @param dstPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00132
/// @needwork = dd
/// @endcode
bool MoveDirectory(AraString const& srcPath, AraString const& dstPath)
{
    return (nai_path_rename(srcPath.c_str(), dstPath.c_str()) >= 0);
}

/// @brief Basename
/// @param fpath
/// @param getLast
/// @return Base name
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00133
/// @needwork = dd
/// @endcode
AraString Basename(AraString const& fpath, bool const getLast)
{
    AraString ret(kFilePathLen, kChar0);
    AraString emptyStr;
    std::size_t const flen{fpath.size()};

    if (flen == 0U) {
        return fpath;
    }
    if (flen >= ret.size()) {
        return emptyStr;
    }

    std::int32_t end{static_cast< std::int32_t >(flen) - 1};
    std::size_t tmp{0U};
    if (getLast) {
        for (; end >= 0; end--) {
            tmp = static_cast< std::size_t >(end);
            if (fpath[tmp] != nai_path_sep) {
                break;
            }
        }
    } else {
        tmp = static_cast< std::size_t >(end);
        if (fpath[tmp] == nai_path_sep) {
            return emptyStr;
        }
    }

    std::int32_t lastSep{end};
    for (; lastSep >= 0; lastSep--) {
        tmp = static_cast< std::size_t >(lastSep);
        if (fpath[tmp] == nai_path_sep) {
            break;
        }
    }

    std::size_t i{static_cast< std::size_t >(lastSep) + 1U};
    std::size_t j{0U};
    for (; i <= static_cast< std::size_t >(end); i++) {
        ret[j] = fpath[i];
        j++;
    }

    ret = ret.substr(0U, j);
    return ret;
}

/// @brief Lastname
/// @param fpath
/// @return Last name
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00134
/// @needwork = dd
/// @endcode
AraString Lastname(AraString const& fpath) { return Basename(fpath, true); }

/// @brief Dirname
/// @param fpath
/// @param getParent
/// @return Dir name
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00135
/// @needwork = dd
/// @endcode
AraString Dirname(AraString const& fpath, bool const getParent)
{
    AraString ret(kFilePathLen, kChar0);
    AraString emptyStr;
    std::size_t const flen{fpath.size()};

    if (flen == 0U) {
        return fpath;
    }
    if (flen >= ret.size()) {
        return emptyStr;
    }

    std::int32_t end{static_cast< std::int32_t >(flen) - 1};
    std::size_t tmp{0U};
    if (getParent) {
        for (; end >= 0; end--) {
            tmp = static_cast< std::size_t >(end);
            if (fpath[tmp] != nai_path_sep) {
                break;
            }
        }
    } else {
        tmp = static_cast< std::size_t >(end);
        if (fpath[tmp] == nai_path_sep) {
            return fpath;
        }
    }

    std::int32_t lastSep{-1};
    for (std::int32_t i{end}; i >= 0; i--) {
        tmp = static_cast< std::size_t >(i);
        if (lastSep != -1) {
            ret[tmp] = fpath[tmp];
            continue;
        }
        if (fpath[tmp] == nai_path_sep) {
            lastSep = i;
        }
    }

    for (; lastSep >= 0; lastSep--) {
        tmp = static_cast< std::size_t >(lastSep);
        if (ret[tmp] != nai_path_sep) {
            break;
        }
    }

    ret = ret.substr(0U, tmp);
    return ret;
}

/// @brief Parentname
/// @param fpath
/// @return parent name
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00136
/// @needwork = dd
/// @endcode
AraString Parentname(AraString const& fpath) { return Dirname(fpath, true); }

/// @brief GetCwd
/// @return cwd
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00137
/// @needwork = dd
/// @endcode
AraString GetCwd()
{
    AraString ret;

    std::array< Char_T, kFilePathLen > cwd{};
    cwd.fill(kChar0);

    std::ignore = nai_path_get_cwd(cwd.data(), cwd.size());

    ret = cwd.data();
    return ret;
}

/// @brief GetFreeDiskSpace
/// @param path
/// @return FreeDiskSpace
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00138
/// @needwork = dd
/// @endcode
std::uint64_t GetFreeDiskSpace(AraString const& path) noexcept
{
    std::uint64_t size{0U};

    struct statfs stfs
    {
    };
    if (statfs(path.c_str(), &stfs) < 0) {
        return size;
    }

    size = static_cast< std::uint64_t >(stfs.f_bsize) * stfs.f_bavail;
    return size;
}

/// @brief Write specified content to a specified file (create if the file does not exist; overwrite if the file exists)
/// @param fPath
/// @param context
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00139
/// @needwork = dd
/// @endcode
void OverWriteToFile(AraString const& fPath, AraString const& context) noexcept
{
    nai_fd_t fd{0};
    intptr_t r{0};

    fd = nai_file_open(fPath.c_str(), kOpenFlagWrite, static_cast< nai_int_t >(kMode0666));
    AssertWithLog(SIsFdValid(fd));

    r = nai_file_write(fd, context.c_str(), context.size());
    AssertWithLog(static_cast< size_t >(r) == context.size());

    r = nai_file_close(fd);
    AssertWithLog(r == 0);
}

/// @brief read md5 string from file
/// @param fPath
/// @return md5 string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00140
/// @needwork = dd
/// @endcode
AraString ReadMD5FromFile(AraString const& fPath) noexcept
{
    AraString mD5Str;
    std::size_t const mD5StrLen{kMD5DigestLen * 2U};
    std::array< Char_T, mD5StrLen + 1U > buf{};
    buf.fill(kChar0);

    // open file
    nai_fd_t const fd{nai_file_open(fPath.c_str(), kOpenFlagRead)};
    if (!SIsFdValid(fd)) {
        return mD5Str;
    }

    // get file size
    nai_off64_t const pos{nai_file_seek(fd, 0, NAI_SEEK_END)};  /////////////////////use nai_file_size///////mytodo////
    if (static_cast< size_t >(pos) != mD5StrLen) {
        return mD5Str;
    }
    std::ignore = nai_file_seek(fd, 0, NAI_SEEK_SET);

    // read file
    intptr_t const r{nai_file_read(fd, buf.data(), mD5StrLen)};
    if (static_cast< size_t >(r) != mD5StrLen) {
        return mD5Str;
    }

    // close file
    std::ignore = nai_file_close(fd);

    mD5Str = buf.data();
    return mD5Str;
}

/// @brief sUnzipCurrentFile
/// @param unzFP
/// @param extractPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00141
/// @needwork = dd
/// @endcode
static bool SUnzipCurrentFile(UnzFile const unzFP, AraString const& extractPath)
{
    bool success{false};
    std::int32_t rret{UNZ_OK};
    std::size_t nread{0U};
    intptr_t wret{-1};
    std::array< Char_T, kFilePathLen > fname{};
    std::array< std::uint8_t, kFileReadLen >
        buf{};  ////Provide a parameter to let users control the data block size////mytodo////

    // Get info about current file
    Unz_File_Info fileInfo;
    fname.fill(kChar0);
    if (UnzGetCurrentFileInfo(unzFP, &fileInfo, fname.data(), fname.size(), nullptr, 0U, nullptr, 0U) != UNZ_OK) {
        LOGE << "could not read file info";
        return success;
    }

    AraString const extractFilePath{SJoinPath(extractPath, fname.data())};
    AraString const extractSubDir{Dirname(extractFilePath)};
    if (!IsDirectory(extractSubDir)) {
        if (!CreateDirectory(extractSubDir, kMode0755, true)) {
            LOGE << "create extract subdir failed for " << extractSubDir.c_str();  //////0755////mytodo////
            return success;
        }
    }

    // Check if this entry is a directory or file
    // Entry is a directory, so create it
    if (fname[static_cast< std::size_t >(strlen(fname.data())) - 1U] == nai_path_sep) {  // NOLINT
        ///LOGD << "--dir-- origin fname:" << fname.data();//mydel//
        ///LOGD << "--dir-- extract subdir:" << extractSubDir;//mydel//
        return true;
    }

    // Entry is a file, so extract it
    ///LOGD << "--file-- origin fname:" << fname;//mydel//
    ///LOGD << "--file-- extract fpath:" << extractFilePath;//mydel//
    if (UnzOpenCurrentFile(unzFP) != UNZ_OK) {
        LOGE << "open origin current file failed";
        return success;
    }

    nai_fd_t const dfd{nai_file_open(extractFilePath.c_str(), kOpenFlagWrite)};
    if (!SIsFdValid(dfd)) {
        LOGE << "open extract file failed";
        std::ignore = UnzCloseCurrentFile(unzFP);
        return success;
    }

    std::ignore = nai_path_set_mode(
        extractFilePath.c_str(), kMode0644);  /////////Otherwise the file permissions will be messed up////mytodo//////

    do {
        /// clear buf
        success     = true;
        std::ignore = nai_memset(buf.data(), 0, buf.size());

        /// read
        rret = UnzReadCurrentFile(unzFP, buf.data(), static_cast< std::uint32_t >(buf.size()));
        if (rret == 0) {
            break;
        }
        if (rret < 0) {
            LOGE << "read origin current file failed";
            success = false;
            break;
        };

        /// write
        nread = static_cast< size_t >(rret);
        wret  = nai_file_write(dfd, buf.data(), nread);
        if (static_cast< size_t >(wret) != nread) {
            LOGE << "write extract file failed";
            success = false;
            break;
        }

        /// finish
        if (nread != buf.size()) {
            break;
        }
    } while (true);

    std::ignore = nai_file_close(dfd);
    std::ignore = UnzCloseCurrentFile(unzFP);
    return success;
}

/// @brief Unzip
/// @param zipFilePath
/// @param extractPath
/// @return result
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00141
/// @needwork = dd
/// @endcode
bool Unzip(AraString const& zipFilePath, AraString const& extractPath)
{
    LOGD << "zipFilePath:" << zipFilePath.c_str() << " extractPath:" << extractPath.c_str();  //
    bool success{false};

    // Open the zip file
    UnzFile const unzFP{UnzOpen64(zipFilePath.c_str())};
    if (unzFP == nullptr) {
        LOGE << "could not found zip file " << zipFilePath.c_str();
        return success;
    }

    // Get info about the zip file
    Unz_Global_Info globalInfo;
    if (UnzGetGlobalInfo(unzFP, &globalInfo) != UNZ_OK) {
        LOGE << "could not read global info";
        std::ignore = UnzClose(unzFP);
        return success;
    }

    if (!IsDirectory(extractPath)) {
        if (!CreateDirectory(extractPath, kMode0755, true)) {
            LOGE << "create extract dir failed";
            std::ignore = UnzClose(unzFP);
            return success;
        }
    }

    // Loop to extract all files
    std::size_t const num{static_cast< std::size_t >(globalInfo.numberEntry)};
    std::size_t i{0U};
    std::size_t next{0U};
    for (; i < num; ++i) {
        success = true;

        if (!SUnzipCurrentFile(unzFP, extractPath)) {
            LOGE << "sUnzipCurrentFile failed";
            success = false;
            break;
        }

        // Go the the next entry listed in the zip file
        next = i + 1U;
        if (next < num) {
            if (UnzGoToNextFile(unzFP) != UNZ_OK) {
                LOGE << "cound not read next file at index " << next;
                success = false;
                break;
            }
        }
    }

    std::ignore = UnzClose(unzFP);
    return success;
}

}  // namespace tinyfs
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
