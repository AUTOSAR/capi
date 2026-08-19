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
/// @file       filesystem.cpp
/// @brief      The Filesystem definition for ucm.
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
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=Filesystem
/// @unit_description=Provide filesystem related functions
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/extraction/filesystem.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Get the list of sub-directories at the path specified as parameter
///
/// @param dir Directory to search directories in
///
/// @return A vector of strings containing the sub-directories
AraVectorString Filesystem::GetSubdirectories(AraString const& dir) noexcept
{
    return tinyfsys::GetSubdirectories(dir);
}

/// @brief Get the list of files at the path specified as parameter
///
/// @param dir Directory to search files in
///
/// @returns A vector of strings containing the file names of the files available at the given path
AraVectorString Filesystem::GetFiles(AraString const& dir) noexcept { return tinyfsys::GetFiles(dir); }

/// @brief Get the recursive list of filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths
AraVectorString Filesystem::GetFilesRecursive(AraString const& dir) noexcept
{
    return tinyfsys::GetFilesRecursive(dir);
}

/// @brief Get the recursive list of relative filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths relative to dir
/// @throws no
AraVectorString Filesystem::GetFilesRecursiveRelative(AraString const& dir)
{
    return tinyfsys::GetFilesRecursiveRelative(dir);
}

/// @brief Get the list of files at the path specified as parameter starting with the given prefix
///
/// @param dir Directory to search files in
/// @param starting Name prefix
///
/// @returns A vector of strings containing the file names of the files available at the given path and starting
/// with
/// the given prefix
///AraVectorString Filesystem::GetFilesStartingWith(AraString const& dir, AraString const& starting) noexcept {
///    ;;///////mytodo////
///}

/// @brief Extract the file name from the given path
///
/// @param path Path to file
///
/// @returns A string containing the file name
AraString Filesystem::ExtractFileName(AraString const& path) noexcept { return tinyfsys::ExtractFileName(path); }

/// @brief Get the file prefix from the given path
///
/// @param path Path to a file
///
/// @returns A string containing the file prefix
///AraString Filesystem::GetFilePrefix(AraString const& path) const noexcept {
///    ;////////
///}

/// @brief Removes a directory recursively
///
/// @param targetDir Path to the folder to remove
///
/// @return true if the directory was successfully removed,
/// false otherwise (non existent or not removed)
AraResultVoid Filesystem::RemoveDirectory(AraString const& targetDir) noexcept
{
    return tinyfsys::RemoveDirectory(targetDir);
}

/// @brief Checks if a directory exists at the given directory path
///
/// @param dir Full path to a directory
///
/// @return true if a directory exists at the given path, else false
bool Filesystem::DoesDirectoryExist(AraString const& dir) noexcept { return tinyfsys::DoesDirectoryExist(dir); }

/// @brief Returns the amount of free disk space available at the given path in number of bytes
///
/// @param path The target path for calculation free disk space
///
/// @return The number of free disk space in bytes available at the given path
AraResult< uint64_t > Filesystem::GetFreeDiskSpace(AraString const& path) noexcept
{
    return tinyfsys::GetFreeDiskSpace(path);
}

/// brief Moves the entire directory including all contained files/directories
/// located at sourceDirectory to targetDir
/// @note If a directory already exists at the target location, the operation is aborted
/// and no files will be overwritten
///
/// @param srcDir Path to the source folder
/// @param targetDir Path to the target folder
///
/// @return true if the directory has been moved, false otherwise
AraResultVoid Filesystem::MoveDirectory(AraString const& srcDir, AraString const& targetDir) noexcept
{
    return tinyfsys::MoveDirectory(srcDir, targetDir);
}

/// @brief Returns the size in bytes of the given directory.
///
/// @param path The absolute path to a directory.
///
/// @return The size in bytes of the given directory or -1 if the directory does not exist
AraResult< uint64_t > Filesystem::GetDirectorySize(AraString const& path) noexcept
{
    return tinyfsys::GetDirectorySize(path);
}

/// @brief Returns the checksum of the file
///
/// @param path The absolute path to a file.
///
/// @return checksum in uint32
///AraResult<uint32_t> Filesystem::GetFileChecksum(AraString const& filePath) noexcept {//not used////
///    return std::move(tinyfsys::GetFileChecksum(filePath));
///}

/// @brief Copies the directory at sourceDirectory recursively to targetDir
/// @note If a directory already exists at the target location, the operation is aborted
/// and no files will be overwritten
///
/// @param srcDir The path to the directory that should be copied
/// @param targetDir The destination path to the new location
///
/// @return true if the directory was successfully copied, else false
AraResultVoid Filesystem::CopyDirectory(AraString const& srcDir, AraString const& targetDir) noexcept
{
    return tinyfsys::CopyDirectory(srcDir, targetDir);
}

/// @brief Overwrite-Copies the directory at sourceDirectory recursively to targetDir
/// @note Both the sourceDirectory and the targetDir must exist.
///     The purpose is to overwrite files from sourceDirectory to targetDir
///
/// @param srcDir The path to the directory that should be copied
/// @param targetDir The destination path to the new location
///
/// @return true if the directory was successfully copied, else false
AraResultVoid Filesystem::OverwriteCopyDirectory(AraString const& srcDir, AraString const& targetDir) noexcept
{
    return tinyfsys::OverwriteCopyDirectory(srcDir, targetDir);
}

/// @brief Copies a file, identified by the sourcefile path parameter to the path
/// specified by the targetfile path parameter
/// @note If a file already exists at the target location, it will not be overwritten,
/// the copy operation is aborted.
///
/// @param sourceFile The path to the file to be copied
/// @param targetFile The destination path
///
/// @return true if the file has been copied successfully, else false
AraResultVoid Filesystem::CopyFile(AraString const& sourceFile, AraString const& targetFile) noexcept
{
    return tinyfsys::CopyFile(sourceFile, targetFile);
}

/// @brief Checks if a file exists at the given path
///
/// @param filePath The path to the file that is checked for existence
///
/// @return true if a file exists at the given path, else false
bool Filesystem::DoesFileExist(AraString const& filePath) noexcept { return tinyfsys::DoesFileExist(filePath); }

/// @brief Check if file is executable by owner
///
/// @param filePath The path to the file that is checked for execute permissions
///
/// @return true if executable bit set, else false
///bool Filesystem::IsOwnerExecutable(AraString const& filePath) noexcept {
///    ;//////
///}

}  //  namespace pkgmgr
}  //  namespace ucm
}  //  namespace ara
