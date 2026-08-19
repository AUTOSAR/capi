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
/// @file       filesystem.h
/// @brief      The Filesystem definition for ucm.
/// @details
/// @date       2022-06-13
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00013, SR_UCM_00025
/// @unit_name=Filesystem
/// @unit_description=Filesystem definitions provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FILESYSTEM_H_
#define ARA_UCM_PKGMGR_FILESYSTEM_H_

#include "tinyfsys.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class contains the additions to the copied filesystem code
/// that have been implemented by UCM
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00009
/// @trace_id_dd=DD_UCM_00067
/// @needwork = ad
/// @endcode
class Filesystem
{
public:
    /// @brief Get the list of sub-directories at the path specified as parameter
    ///
    /// @param dir Directory to search directories in
    ///
    /// @return A vector of strings containing the sub-directories
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00068
    /// @needwork = dda
    /// @endcode
    static AraVectorString GetSubdirectories(AraString const& dir) noexcept;

    /// @brief Get the list of files at the path specified as parameter
    ///
    /// @param dir Directory to search files in
    ///
    /// @returns A vector of strings containing the file names of the files available at the given path
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00069
    /// @needwork = dda
    /// @endcode
    static AraVectorString GetFiles(AraString const& dir) noexcept;

    /// @brief Get the recursive list of filepaths to all files in the directory
    ///
    /// @param dir Directory to search in
    ///
    /// @returns List of full filepaths
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00070
    /// @needwork = dda
    /// @endcode
    static AraVectorString GetFilesRecursive(AraString const& dir) noexcept;

    /// @brief Get the recursive list of relative filepaths to all files in the directory
    ///
    /// @param dir Directory to search in
    ///
    /// @returns List of full filepaths relative to dir
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00071
    /// @needwork = dda
    /// @endcode
    static AraVectorString GetFilesRecursiveRelative(AraString const& dir);

    /// @brief Get the list of files at the path specified as parameter starting with the given prefix
    ///
    /// @param dir Directory to search files in
    /// @param starting Name prefix
    ///
    /// @returns A vector of strings containing the file names of the files available at the given path and starting
    /// with
    /// the given prefix
    ///AraVectorString GetFilesStartingWith(AraString const& dir, AraString const& starting) noexcept;//////not used//mytodo////

    /// @brief Extract the file name from the given path
    ///
    /// @param path Path to file
    ///
    /// @returns A string containing the file name
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00072
    /// @needwork = dda
    /// @endcode
    static AraString ExtractFileName(AraString const& path) noexcept;

    /// @brief Get the file prefix from the given path
    ///
    /// @param path Path to a file
    ///
    /// @returns A string containing the file prefix
    ///AraString GetFilePrefix(AraString const& path) const noexcept;//////not used//mytodo////

    /// @brief Removes a directory recursively
    ///
    /// @param targetDir Path to the folder to remove
    ///
    /// @return true if the directory was successfully removed,
    /// false otherwise (non existent or not removed)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00073
    /// @needwork = dda
    /// @endcode
    static AraResultVoid RemoveDirectory(AraString const& targetDir) noexcept;

    /// @brief Returns the size in bytes of the given directory.
    ///
    /// @param path The absolute path to a directory.
    ///
    /// @return The size in bytes of the given directory or -1 if the directory does not exist
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00074
    /// @needwork = dda
    /// @endcode
    static AraResult< uint64_t > GetDirectorySize(AraString const& path) noexcept;

    /// @brief Returns the checksum of the file
    ///
    /// @param path The absolute path to a file.
    ///
    /// @return checksum in uint32
    ///AraResult<uint32_t> GetFileChecksum(AraString const& filePath) noexcept;//////not used, can be removed/////////////mydel//

    /// @brief Returns the amount of free disk space available at the given path in number of bytes
    ///
    /// @param path The target path for calculation free disk space
    ///
    /// @return The number of free disk space in bytes available at the given path
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00075
    /// @needwork = dda
    /// @endcode
    static AraResult< uint64_t > GetFreeDiskSpace(AraString const& path) noexcept;  ////not used but useful//

    /// @param srcDir Path to the source folder
    /// @param targetDir Path to the target folder
    ///
    /// @return true if the directory has been moved, false otherwise
    /// @throws no
    /// @code{.isoft}
    /// brief Moves the entire directory including all contained files/directories
    /// located at sourceDirectory to targetDir
    /// @note If a directory already exists at the target location, the operation is aborted
    /// and no files will be overwritten
    ///
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00076
    /// @needwork = dda
    /// @endcode
    static AraResultVoid MoveDirectory(AraString const& srcDir, AraString const& targetDir) noexcept;

    /// @brief Copies the directory at sourceDirectory recursively to targetDir
    /// @param srcDir The path to the directory that should be copied
    /// @param targetDir The destination path to the new location
    ///
    /// @return true if the directory was successfully copied, else false
    /// @throws no
    /// @code{.isoft}
    /// @note If a directory already exists at the target location, the operation is aborted
    /// and no files will be overwritten
    ///
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00077
    /// @needwork = dda
    /// @endcode
    static AraResultVoid CopyDirectory(AraString const& srcDir, AraString const& targetDir) noexcept;

    /// @brief Overwrite-Copies the directory at sourceDirectory recursively to targetDir
    /// @param srcDir The path to the directory that should be copied
    /// @param targetDir The destination path to the new location
    ///
    /// @return true if the directory was successfully copied, else false
    /// @throws no
    /// @code{.isoft}
    /// @note Both the sourceDirectory and the targetDir must exist.
    ///     The purpose is to overwrite files from sourceDirectory to targetDir
    ///
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00078
    /// @needwork = dda
    /// @endcode
    static AraResultVoid OverwriteCopyDirectory(AraString const& srcDir, AraString const& targetDir) noexcept;

    /// @brief Checks if a file exists at the given path
    ///
    /// @param filePath The path to the file that is checked for existence
    ///
    /// @return true if a file exists at the given path, else false
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00079
    /// @needwork = dda
    /// @endcode
    static bool DoesFileExist(AraString const& filePath) noexcept;

    /// @brief Check if file is executable by owner
    ///
    /// @param filePath The path to the file that is checked for execute permissions
    ///
    /// @return true if executable bit set, else false
    ///bool IsOwnerExecutable(AraString const& filePath) noexcept;///////not used//mytodo//

    /// @brief Checks if a directory exists at the given directory path
    ///
    /// @param dir Full path to a directory
    ///
    /// @return true if a directory exists at the given path, else false
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00080
    /// @needwork = dda
    /// @endcode
    static bool DoesDirectoryExist(AraString const& dir) noexcept;

    /// @brief Copies a file, identified by the sourcefile path parameter to the path
    /// specified by the targetfile path parameter
    /// @param sourceFile The path to the file to be copied
    /// @param targetFile The destination path
    ///
    /// @return true if the file has been copied successfully, else false
    /// @throws no
    /// @code{.isoft}
    /// @note If a file already exists at the target location, it will not be overwritten,
    /// the copy operation is aborted.
    ///
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00009
    /// @trace_id_dd=DD_UCM_00081
    /// @needwork = dda
    /// @endcode
    static AraResultVoid CopyFile(AraString const& sourceFile, AraString const& targetFile) noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FILESYSTEM_H_
