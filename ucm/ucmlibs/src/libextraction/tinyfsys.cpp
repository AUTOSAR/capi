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
/// @file       tinyfsys.cpp
/// @brief      tiny Filesystem
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
/// @unit_name=tinyfsys
/// @unit_description=Provide tiny file system functionality
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/extraction/tinyfsys.h"

#include "ara/ucm/internal/extraction/log.h"
#include "ara/ucm/internal/extraction/tinyfs.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace tinyfsys {

/// @brief Get the list of sub-directories at the path specified as parameter
///
/// @param dir Directory to search directories in
///
/// @return A vector of strings containing the sub-directories
/// @throws no
AraVectorString GetSubdirectories(AraString const& dir) { return tinyfs::GetSubdirs(dir); }

/// @brief Get the list of files at the path specified as parameter
///
/// @param dir Directory to search files in
///
/// @returns A vector of strings containing the file names of the files available at the given path
/// @throws no
AraVectorString GetFiles(AraString const& dir) { return tinyfs::GetFiles(dir); }

/// @brief Get the recursive list of filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths
/// @throws no
AraVectorString GetFilesRecursive(AraString const& dir) { return tinyfs::GetFilesRecursive(dir); }

/// @brief Get the recursive list of relative filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths relative to dir
/// @throws no
AraVectorString GetFilesRecursiveRelative(AraString const& dir)
{
    AraVectorString relativeFiles;
    AraString relativeFile;
    Char_T const sep{'/'};

    for (AraString& file : tinyfs::GetFilesRecursive(dir)) {
        // erase the front part of the path to make the path relative
        relativeFile = file.substr(dir.length());
        // if '/' at the begin, trim it
        if (!relativeFile.empty()) {
            if (relativeFile[0U] == sep) {
                relativeFile = relativeFile.substr(1U);
            }
        }
        relativeFiles.emplace_back(relativeFile);
    }

    return relativeFiles;
}

///AraVectorString GetFilesStartingWith(AraString const& dir, AraString const& starting) {
///    ;
///}

/// @brief Extract the file name from the given path
///
/// @param path Path to file
///
/// @returns A string containing the file name
/// @throws no
AraString ExtractFileName(AraString const& path) { return tinyfs::Lastname(path); }

///AraString GetFilePrefix(AraString const& path) {
///    ;
///}

/// @brief RemoveFile
/// @param targetFile
/// @returns result
/// @throws no
AraResultVoid RemoveFile(AraString const& targetFile)
{
    if (!DoesFileExist(targetFile)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    if (!tinyfs::RemoveFile(targetFile)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kRemoveFile);
    }

    return {};
}

/// @brief Removes a directory recursively
///
/// @param targetDir Path to the folder to remove
///
/// @return true if the directory was successfully removed,
/// false otherwise (non existent or not removed)
/// @throws no
AraResultVoid RemoveDirectory(AraString const& targetDir)
{
    if (!DoesDirectoryExist(targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    if (!tinyfs::RemoveDirectory(targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kRemoveDirectory);
    }

    return {};
}

/// @brief Checks if a directory exists at the given directory path
///
/// @param dir Full path to a directory
///
/// @return true if a directory exists at the given path, else false
/// @throws no
bool DoesDirectoryExist(AraString const& dir) { return tinyfs::IsDirectory(dir); }

/// @brief Returns the amount of free disk space available at the given path in number of bytes
///
/// @param path The target path for calculation free disk space
///
/// @return The number of free disk space in bytes available at the given path
/// @throws no
AraResult< std::uint64_t > GetFreeDiskSpace(AraString const& path)
{
    std::uint64_t const freeSize{tinyfs::GetFreeDiskSpace(path)};
    if (freeSize <= 0U) {
        return AraResult< std::uint64_t >::FromError(UcmFilesystemErrc::kGetFreeDiskSpace);
    }
    return AraResult< std::uint64_t >::FromValue(freeSize);
}

/// @brief Moves the entire directory including all contained files/directories
/// located at sourceDirectory to targetDir
/// @note If a directory already exists at the target location, the operation is aborted
/// and no files will be overwritten
///
/// @param srcDir Path to the source folder
/// @param targetDir Path to the target folder
///
/// @return true if the directory has been moved, false otherwise
/// @throws no
AraResultVoid MoveDirectory(AraString const& srcDir, AraString const& targetDir)
{
    if (!DoesDirectoryExist(srcDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }
    if (DoesDirectoryExist(targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kFileAlreadyExists);
    }

    if (!tinyfs::MoveDirectory(srcDir, targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kRenameFile);
    }

    return {};
}

/// @brief Returns the size in bytes of the given directory.
///
/// @param path The absolute path to a directory.
///
/// @return The size in bytes of the given directory or -1 if the directory does not exist
/// @throws no
AraResult< std::uint64_t > GetDirectorySize(AraString const& path)
{
    if (!DoesDirectoryExist(path)) {
        return AraResult< std::uint64_t >::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    std::uint64_t const directorySize{tinyfs::GetDirectorySize(path)};
    if (directorySize <= 0U) {
        return AraResult< std::uint64_t >::FromError(UcmFilesystemErrc::kDetectRegularFile);
    }

    return AraResult< std::uint64_t >::FromValue(directorySize);
}

///AraResult<uint32_t> GetFileChecksum(AraString const& filePath) {//not used////
///    ;
///}

/// @brief Copies the directory at sourceDirectory recursively to targetDir
/// @note If a directory already exists at the target location, the operation is aborted
/// and no files will be overwritten
///
/// @param srcDir The path to the directory that should be copied
/// @param targetDir The destination path to the new location
///
/// @return true if the directory was successfully copied, else false
/// @throws no
AraResultVoid CopyDirectory(AraString const& srcDir, AraString const& targetDir)
{
    if (!DoesDirectoryExist(srcDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }
    if (DoesDirectoryExist(targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kFileAlreadyExists);
    }

    if (!tinyfs::CopyDirectory(srcDir, targetDir, false)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kCopyDirectory);
    }

    return {};
}

/// @brief Overwrite-Copies the directory at sourceDirectory recursively to targetDir
///
/// @param srcDir The path to the directory that should be copied
/// @param targetDir The destination path to the new location
///
/// @return true if the directory was successfully copied, else false
/// @throws no
AraResultVoid OverwriteCopyDirectory(AraString const& srcDir, AraString const& targetDir)
{
    if (!DoesDirectoryExist(srcDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }
    if (!DoesDirectoryExist(targetDir)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    if (!tinyfs::CopyDirectory(srcDir, targetDir, true)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kCopyDirectory);
    }

    return {};
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
/// @throws no
AraResultVoid CopyFile(AraString const& sourceFile, AraString const& targetFile)
{
    if (!DoesFileExist(sourceFile)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    if (DoesFileExist(targetFile)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kFileAlreadyExists);
    }

    if (!tinyfs::CopyFile(sourceFile, targetFile)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kCopyFile);
    }

    return {};
}

/// @brief Checks if a file exists at the given path
///
/// @param filePath The path to the file that is checked for existence
///
/// @return true if a file exists at the given path, else false
/// @throws no
bool DoesFileExist(AraString const& filePath) { return tinyfs::IsRegularFile(filePath); }

///bool IsOwnerExecutable(AraString const& filePath) {
///    ;
///}

/// @brief Unzip
/// @param zipFilePath
/// @param extractPath
/// @return result
/// @throws no
AraResultVoid Unzip(AraString const& zipFilePath, AraString const& extractPath)
{
    if (!tinyfs::Unzip(zipFilePath, extractPath)) {
        return AraResultVoid::FromError(UcmFilesystemErrc::kCopyFile);
    }

    return {};
}

}  //  namespace tinyfsys
}  //  namespace pkgmgr
}  //  namespace ucm
}  //  namespace ara
