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
/// @file       tinyfs.h
/// @brief      tiny fs
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00013, SR_UCM_00025
/// @unit_name=tinyfs
/// @unit_description=tiny fs definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXTRACTION_TINY_FS_H_
#define ARA_UCM_PKGMGR_EXTRACTION_TINY_FS_H_

#include "alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace tinyfs {

/// @brief IsExist
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00117
/// @needwork = dd
/// @endcode
bool IsExist(AraString const& path);
/// @brief IsRegularFile
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00118
/// @needwork = dd
/// @endcode
bool IsRegularFile(AraString const& path);
/// @brief IsDirectory
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00119
/// @needwork = dd
/// @endcode
bool IsDirectory(AraString const& path);
/// @brief IsSymbolLink
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00120
/// @needwork = dd
/// @endcode
bool IsSymbolLink(AraString const& path);

/// @brief GetFiles
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00121
/// @needwork = dd
/// @endcode
AraVectorString GetFiles(AraString const& path, bool const useFullPath = true);
/// @brief GetFilesRecursive
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00122
/// @needwork = dd
/// @endcode
AraVectorString GetFilesRecursive(AraString const& path, bool const useFullPath = true);
/// @brief GetSubdirs
/// @param path
/// @param useFullPath
/// @return vector of string
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00123
/// @needwork = dd
/// @endcode
AraVectorString GetSubdirs(AraString const& path, bool const useFullPath = true);

/// @brief GetFileSize
/// @param path
/// @return File Size
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00124
/// @needwork = dd
/// @endcode
std::uint64_t GetFileSize(AraString const& path);
/// @brief GetDirectorySize
/// @param path
/// @return Directory Size
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00125
/// @needwork = dd
/// @endcode
std::uint64_t GetDirectorySize(AraString const& path);

/// @brief CreateDirectory
/// @param path
/// @param mode
/// @param recursive
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00126
/// @needwork = dd
/// @endcode
bool CreateDirectory(AraString const& path, std::int32_t const mode, bool const recursive);

/// @brief RemoveFile
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00127
/// @needwork = dd
/// @endcode
bool RemoveFile(AraString const& path);
/// @brief RemoveDirectory
/// @param path
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00128
/// @needwork = dd
/// @endcode
bool RemoveDirectory(AraString const& path);

/// @brief CopyFile
/// @param srcPath
/// @param dstPath
/// @param overWrite
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00129
/// @needwork = dd
/// @endcode
bool CopyFile(AraString const& srcPath, AraString const& dstPath, bool const overWrite = true);
/// @brief CopyDirectory
/// @param srcPath
/// @param dstPath
/// @param overWrite
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00130
/// @needwork = dd
/// @endcode
bool CopyDirectory(AraString const& srcPath, AraString const& dstPath, bool const overWrite = true);

/// @brief MoveFile
/// @param srcPath
/// @param dstPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00131
/// @needwork = dd
/// @endcode
bool MoveFile(AraString const& srcPath, AraString const& dstPath);
/// @brief MoveDirectory
/// @param srcPath
/// @param dstPath
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00132
/// @needwork = dd
/// @endcode
bool MoveDirectory(AraString const& srcPath, AraString const& dstPath);

/// @brief Basename
/// @param fpath
/// @param getLast
/// @return Base name
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00133
/// @needwork = dd
/// @endcode
AraString Basename(AraString const& fpath, bool const getLast = false);
/// @brief Lastname
/// @param fpath
/// @return Last name
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00134
/// @needwork = dd
/// @endcode
AraString Lastname(AraString const& fpath);
/// @brief Dirname
/// @param fpath
/// @param getParent
/// @return Dir name
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00135
/// @needwork = dd
/// @endcode
AraString Dirname(AraString const& fpath, bool const getParent = false);
/// @brief Parentname
/// @param fpath
/// @return parent name
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00136
/// @needwork = dd
/// @endcode
AraString Parentname(AraString const& fpath);
/// @brief GetCwd
/// @return cwd
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00137
/// @needwork = dd
/// @endcode
AraString GetCwd();
/// @brief GetFreeDiskSpace
/// @param path
/// @return FreeDiskSpace
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00138
/// @needwork = dd
/// @endcode
std::uint64_t GetFreeDiskSpace(AraString const& path) noexcept;

/// @brief Write specified content to a specified file (create if the file does not exist; overwrite if the file exists)
/// @param fPath
/// @param context
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00139
/// @needwork = dd
/// @endcode
void OverWriteToFile(AraString const& fPath, AraString const& context) noexcept;

/// @brief read md5 string from file
/// @param fPath
/// @return md5 string
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00140
/// @needwork = dd
/// @endcode
AraString ReadMD5FromFile(AraString const& fPath) noexcept;

/// @brief Unzip
/// @param zipFilePath
/// @param extractPath
/// @return result
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00008
/// @trace_id_dd=DD_UCM_00141
/// @needwork = dd
/// @endcode
bool Unzip(AraString const& zipFilePath, AraString const& extractPath);

}  // namespace tinyfs
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXTRACTION_TINY_FS_H_
