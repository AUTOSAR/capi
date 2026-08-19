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
/// @file       tinyfsys.h
/// @brief      tiny Filesystem
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
/// @unit_name=tinyfsys
/// @unit_description=tiny Filesystem definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXTRACTION_TINY_FSYS_H_
#define ARA_UCM_PKGMGR_EXTRACTION_TINY_FSYS_H_

#include "alias.h"
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"  //////////////////////////////mydel////

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief UcmFilesystemErrc
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00082
/// @needwork = ad
/// @endcode
enum class UcmFilesystemErrc : ara::core::ErrorDomain::CodeType
{
    kDetectRegularFile = 1,
    kNoSuchFileOrDirectory,
    kFileAlreadyExists,
    kCreateDirectory,
    kCreateDirectoryIterator,
    kRemoveFile,
    kRemoveDirectory,
    kCopyFile,
    kCopyDirectory,
    kRenameFile,
    kGetFreeDiskSpace
};

///#if defined(__GNUC__)
///#pragma GCC diagnostic push
///#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
///#endif

/// @brief UcmFilesystemErrorDomain
/// @code{.isoft}
/// @domainid{0x8000000000000702}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00083
/// @needwork = dd
/// @endcode
class UcmFilesystemErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief IdType
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00084
    /// @needwork = dd
    /// @endcode
    constexpr static ara::core::ErrorDomain::IdType kId{0x8000000000000702U};

public:
    /// @brief Errc
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00084
    /// @needwork = no
    /// @endcode
    using Errc = UcmFilesystemErrc;
    /// @brief Exception
    ///using Exception = UcmFilesystemException;

    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00085
    /// @needwork = dda
    /// @endcode
    constexpr UcmFilesystemErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    /// @brief Name
    /// @returns name
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00086
    /// @needwork = dda
    /// @endcode
    Char_T const* Name() const noexcept final { return "Filesystem"; }

    /// @brief Message
    /// @param errorCode
    /// @returns error string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00087
    /// @needwork = dda
    /// @endcode
    Char_T const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept final
    {
        Errc const errc{static_cast< Errc >(errorCode)};
        if (Errc::kRemoveDirectory == errc) {
            return "error removing directory";
        }
        if (Errc::kNoSuchFileOrDirectory == errc) {
            return "no such file or directory";
        }
        if (Errc::kGetFreeDiskSpace == errc) {
            return "error getting free disk space";
        }
        if (Errc::kCreateDirectory == errc) {
            return "error creating directory";
        }
        if (Errc::kCreateDirectoryIterator == errc) {
            return "error creating directory iterator";
        }
        if (Errc::kRenameFile == errc) {
            return "rename error";
        }
        if (Errc::kDetectRegularFile == errc) {
            return "regular file detection error";
        }
        if (Errc::kCopyDirectory == errc) {
            return "error copying directory";
        }
        if (Errc::kFileAlreadyExists == errc) {
            return "file already exists";
        }
        if (Errc::kCopyFile == errc) {
            return "error copying file";
        }
        return "unknown filesystem error";
    }

    /// @brief ThrowAsException
    /// @param errorCode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00006
    /// @trace_id_dd=DD_UCM_00088
    /// @needwork = dda
    /// @endcode
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) final
    {
        std::ignore = errorCode;
    }
};

///#if defined(__GNUC__)
///#pragma GCC diagnostic pop
///#endif

namespace internal {
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00089
/// @needwork = ad
/// @endcode
constexpr UcmFilesystemErrorDomain kUcmFilesystemErrorDomain;
}  // namespace internal

/// @brief GetUcmFilesystemDomain
/// @returns ErrorDomain
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00090
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorDomain const& GetUcmFilesystemDomain() noexcept
{
    return internal::kUcmFilesystemErrorDomain;
}

/// @brief MakeErrorCode
/// @param code
/// @param data
/// @returns ErrorCode
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00091
/// @needwork = ad
/// @endcode
constexpr ara::core::ErrorCode MakeErrorCode(UcmFilesystemErrorDomain::Errc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetUcmFilesystemDomain(), data);
}

/// @brief This class contains the additions to the copied filesystem code
/// that have been implemented by UCM
namespace tinyfsys {

/// @brief Get the list of sub-directories at the path specified as parameter
///
/// @param dir Directory to search directories in
///
/// @return A vector of strings containing the sub-directories
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00092
/// @needwork = dd
/// @endcode
AraVectorString GetSubdirectories(AraString const& dir);

/// @brief Get the list of files at the path specified as parameter
///
/// @param dir Directory to search files in
///
/// @returns A vector of strings containing the file names of the files available at the given path
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00093
/// @needwork = dd
/// @endcode
AraVectorString GetFiles(AraString const& dir);

/// @brief Get the recursive list of filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00094
/// @needwork = dd
/// @endcode
AraVectorString GetFilesRecursive(AraString const& dir);

/// @brief Get the recursive list of relative filepaths to all files in the directory
///
/// @param dir Directory to search in
///
/// @returns List of full filepaths relative to dir
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00095
/// @needwork = dd
/// @endcode
AraVectorString GetFilesRecursiveRelative(AraString const& dir);

/// @brief Get the list of files at the path specified as parameter starting with the given prefix
///
/// @param dir Directory to search files in
/// @param starting Name prefix
///
/// @returns A vector of strings containing the file names of the files available at the given path and starting
/// with
/// the given prefix
///AraVectorString GetFilesStartingWith(AraString const& dir, AraString const& starting);////not used//mytodo////

/// @brief Extract the file name from the given path
///
/// @param path Path to file
///
/// @returns A string containing the file name
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00096
/// @needwork = dd
/// @endcode
AraString ExtractFileName(AraString const& path);

/// @brief Get the file prefix from the given path
///
/// @param path Path to a file
///
/// @returns A string containing the file prefix
/// @throws no
///AraString GetFilePrefix(AraString const& path);//////not used//mytodo////

/// @brief RemoveFile
/// @param targetFile
/// @returns result
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00097
/// @needwork = dd
/// @endcode
AraResultVoid RemoveFile(AraString const& targetFile);

/// @brief Removes a directory recursively
///
/// @param targetDir Path to the folder to remove
///
/// @return true if the directory was successfully removed,
/// false otherwise (non existent or not removed)
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00098
/// @needwork = dd
/// @endcode
AraResultVoid RemoveDirectory(AraString const& targetDir);

/// @brief Returns the size in bytes of the given directory.
///
/// @param path The absolute path to a directory.
///
/// @return The size in bytes of the given directory or -1 if the directory does not exist
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00099
/// @needwork = dd
/// @endcode
AraResult< std::uint64_t > GetDirectorySize(AraString const& path);

/// @brief Returns the checksum of the file
///
/// @param path The absolute path to a file.
///
/// @return checksum in uint32
///AraResult<uint32_t> GetFileChecksum(AraString const& filePath);//////not used, can be removed/////////////mydel////

/// @brief Returns the amount of free disk space available at the given path in number of bytes
///
/// @param path The target path for calculation free disk space
///
/// @return The number of free disk space in bytes available at the given path
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00100
/// @needwork = dd
/// @endcode
AraResult< std::uint64_t > GetFreeDiskSpace(AraString const& path);  ////not used but useful//

/// @brief Moves the entire directory including all contained files/directories
/// located at sourceDirectory to targetDir
/// @param srcDir Path to the source folder
/// @param targetDir Path to the target folder
///
/// @return true if the directory has been moved, false otherwise
/// @throws no
/// @code{.isoft}
/// @note If a directory already exists at the target location, the operation is aborted
/// and no files will be overwritten
///
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00101
/// @needwork = dd
/// @endcode
AraResultVoid MoveDirectory(AraString const& srcDir, AraString const& targetDir);

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
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00102
/// @needwork = dd
/// @endcode
AraResultVoid CopyDirectory(AraString const& srcDir, AraString const& targetDir);

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
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00103
/// @needwork = dd
/// @endcode
AraResultVoid OverwriteCopyDirectory(AraString const& srcDir, AraString const& targetDir);

/// @brief Checks if a file exists at the given path
///
/// @param filePath The path to the file that is checked for existence
///
/// @return true if a file exists at the given path, else false
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00104
/// @needwork = dd
/// @endcode
bool DoesFileExist(AraString const& filePath);

/// @brief Check if file is executable by owner
///
/// @param filePath The path to the file that is checked for execute permissions
///
/// @return true if executable bit set, else false
///bool IsOwnerExecutable(AraString const& filePath);///////not used//mytodo//

/// @brief Checks if a directory exists at the given directory path
///
/// @param dir Full path to a directory
///
/// @return true if a directory exists at the given path, else false
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00105
/// @needwork = dd
/// @endcode
bool DoesDirectoryExist(AraString const& dir);

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
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00106
/// @needwork = dd
/// @endcode
AraResultVoid CopyFile(AraString const& sourceFile, AraString const& targetFile);

/// @brief Unzip
/// @param zipFilePath
/// @param extractPath
/// @return result
/// @throws no
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00006
/// @trace_id_dd=DD_UCM_00107
/// @needwork = dd
/// @endcode
AraResultVoid Unzip(AraString const& zipFilePath, AraString const& extractPath);

}  // namespace tinyfsys

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXTRACTION_TINY_FSYS_H_
