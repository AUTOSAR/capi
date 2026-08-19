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
/// @file       file_storage.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Puhua File System
/// @date       2021-04-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-13  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/FileStorage/File Storage Interface
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_FILE_STORAGE_H_
#define ARA_PER_FILE_STORAGE_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include "ara/per/read_write_accessor.h"
#include "ara/per/shared_handle.h"
#include "ara/per/unique_handle.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief
class PKvSystem;
/// @brief
class PConfigMuster_File;
}  // namespace isoftkv
/// @brief File descriptor enumeration
/// @code{.isoft}
/// @unit_name=OpenMode
/// @traceid {SWS_PER_00147}
/// @endcode
/// @brief This enumeration defines how a file shall be opened.
/// The values can be combined (using | and |=) as long as they do not contradict each other.
enum class OpenMode : uint32_t
{
    /// @brief Default initialization value: This value should not appear in normal logic
    kOpenMode_Error = 0,
    /// @brief Sets the seek position to the beginning of the file when the file is opened. This mode
    /// cannot be combined with kAtTheEnd.
    kAtTheBeginning = 1 << 0,
    /// @brief Sets the seek position to the end of the file when the file is opened. This mode cannot be
    /// combined with kAtTheBeginning or kTruncate.
    kAtTheEnd = 1 << 1,
    /// @brief Removes existing content when the file is opened. This mode cannot be combined with kAtTheEnd.
    kTruncate = 1 << 2,
    /// @brief Append to the end. Always seeks to the end of the file before writing.
    kAppend = 1 << 3,
    /// @brief
    kOpenMode_Mult_1 = kAtTheBeginning | kTruncate,
    /// @brief
    kOpenMode_Mult_2 = kAtTheBeginning | kAppend,
    /// @brief
    kOpenMode_Mult_3 = kAtTheEnd | kTruncate,
    /// @brief
    kOpenMode_Mult_4 = kAtTheEnd | kAppend,
};
/// @brief File creation status enumeration
/// @code{.isoft}
/// @unit_name=FileCreationState
/// @traceid {SWS_PER_00435}
/// @endcode
/// @brief This enumeration describes how and when a file was created.
enum class FileCreationState : uint32_t
{
    /// @brief Default status: Meaningless
    kDefault = 0,
    /// @brief The file was created by Persistency after installation of the application or after ResetPersistency.
    kCreatedDuringInstallion = 1,
    /// @brief The file was created by Persistency during anupdate.
    kCreatedDuringUpdate = 2,
    /// @brief The file was re-created due to a call to ResetFile or ResetAllFiles.
    kCreatedDuringReset = 3,
    /// @brief The file was re-created by Persistency after a corruption was detected.
    kCreatedDuringRecovery = 4,
    /// @brief The file was created by the application.
    kCreatedByApplication = 5,
};
//********************************/
/// @brief File modification status enumeration
/// @code{.isoft}
/// @unit_name=FileModificationState
/// @traceid {SWS_PER_00436}
/// @endcode
/// @brief This enumeration describes how and when a file was last modified.
enum class FileModificationState : uint32_t
{
    /// @brief Default status: Meaningless
    kDefault = 0,
    /// @brief The file was last modified by Persistency during an update.
    kModifiedDuringUpdate = 2,
    /// @brief The file was last modified by Persistency due to a call to ResetFile or ResetAllFiles.
    kModifiedDuringReset = 3,
    /// @brief The file was last modified by Persistency after a corruption was detected.
    kModifiedDuringRecovery = 4,
    /// @brief The file was last modified by the application.
    kModifiedByApplication = 5,
};
//********************************/
/// @brief File status information structure
/// @code{.isoft}
/// @unit_name=FileInfo
/// @traceid {SWS_PER_00437}
/// @endcode
/// @brief This structure contains additional information on a file returned by GetFileInfo.
struct FileInfo final
{
public:
    /// @traceid {SWS_PER_00441}
    /// @brief creationTime Time in nanoseconds since midnight 1970-01-01 UTC at which the file was created.
    uint64_t creationTime{0U};
    /// @traceid {SWS_PER_00442}
    /// @brief Time in nanoseconds since midnight 1970-01-01 UTC at which the file was last modified.
    uint64_t modificationTime{0U};
    /// @traceid {SWS_PER_00443}
    /// @brief Time in nanoseconds since midnight 1970-01-01 UTC at which the file was last accessed.
    uint64_t accessTime{0U};
    /// @traceid {SWS_PER_00444}
    /// @brief Information on how and by whom the file was created.
    FileCreationState fileCreationState{FileCreationState::kDefault};
    /// @traceid {SWS_PER_00445}
    /// @brief Information on how and by whom the file was last modified.
    FileModificationState fileModificationState{FileModificationState::kDefault};
};
//********************************/
/// @brief File storage library operation class: Contains a set of files identified by file names.
/// @code{.isoft}
/// @unit_name=FileStorage
/// @traceid {SWS_PER_00340}
/// @endcode
/// @brief The File Storage contains a set of files identified by their names.
class FileStorage
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00460}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Constructor for FileStorage.
    FileStorage() = delete;

protected:
    /// @brief Constructor using configuration properties
    /// @param[in] fileConfig
    explicit FileStorage(isoftkv::PConfigMuster_File const &fileConfig) noexcept;

private:
    /// @brief
    std::unique_ptr< isoftkv::PKvSystem > pKvSystem_{nullptr};
    /// @brief
    ara::core::String stRootPath_;
    /// @brief Configuration options
    std::unique_ptr< isoftkv::PConfigMuster_File > pFileConfig_;

public:
    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00326}
    /// @endcode
    /// @brief Move constructor for FileStorage.
    /// @param[in] fs The FileStorage object to be moved.
    FileStorage(FileStorage &&fs) noexcept;
    /// @brief Disable copy constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00328}
    /// @endcode
    /// @brief The copy constructor for FileStorage shall not be used.
    /// @param[in] a
    FileStorage(FileStorage const &a) = delete;
    /// @brief Move assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00327}
    /// @endcode
    /// @brief Move assignment operator for FileStorage..
    /// @param[in] fs The FileStorage object to be moved.
    /// @return The moved FileStorage object.
    FileStorage &operator=(FileStorage &&fs) &noexcept;
    /// @brief Disable copy assignment function
    /// @brief The copy constructor for FileStorage shall not be used.
    /// @param[in] a
    /// @return
    /// @traceid {SWS_PER_00329}
    FileStorage &operator=(FileStorage const &a) = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00330}
    /// @endcode
    /// @brief Destructor for FileStorage.
    virtual ~FileStorage() noexcept;

public:
    /// @brief Get all file names managed by this center
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00110}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::String > >
    /// @brief Returns a list of all currently available files of this File Storage.
    /// The list of files is only accurate if no file is added or deleted at the same time.
    /// @returns A Result containing a list of available files.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    GetAllFileNames() const noexcept;
    /// @brief Delete file
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00111}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Deletes a file from this File Storage.
    /// This operation will fail with kResourceBusy when the file is currently open.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    DeleteFile(ara::core::StringView const fileName) noexcept;
    /// @brief Check if file exists
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00112}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< bool >
    /// @brief Checks if a file exists in this File Storage.
    /// The result is only accurate if no file is added or deleted at the same time.
    /// E.g. when a file is removed in another thread directly after this function returned "true",
    /// the result is not valid anymore.
    /// @param[in] fileName Name of the file. May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing true if the file could be located or false if it couldn’t.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    FileExists(ara::core::StringView const fileName) const noexcept;
    /// @brief Restore file
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00337}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Recovers a file of this File Storage.
    /// This method allows to recover a single file when the redundancy checks fail.
    /// It will fail with kResourceBusy when the file is currently open.
    /// This method does a best-effort recovery of the file.
    /// After recovery, the file might show outdated or initial content, or might be lost.
    /// @param[in] fileName Name of the file.
    /// May correspond to the PersistencyFile.fileName of a configured file.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    RecoverFile(ara::core::StringView const fileName) noexcept;
    /// @brief Reset file
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00338}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< void >
    /// @brief Resets a file of this File Storage to its initial content.
    /// This method allows to reset a single file to its initial content.
    /// If the file is currently not available in the File Storage, it is re-created.
    /// It will fail with kResourceBusy when the file is currently open,
    /// and with kInitValueNotAvailable when deployment does not define an initial content for the file.
    /// @param[in] fileName Name of the file.
    /// May correspond to the PersistencyFile.fileName of a configured file.
    /// @returns A Result of void.
    /// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
    ResetFile(ara::core::StringView const fileName) noexcept;
    /// @brief Get current file occupied space in bytes
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00407}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< uint64_t >
    /// @brief
    /// Returns the space in bytes currently occupied by the content of a file of this File Storage.
    /// The returned size is only accurate if no other operation on the file takes place at the same time.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing the occupied space in bytes.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    GetCurrentFileSize(ara::core::StringView const fileName) const noexcept;
    /// @brief Get file status information
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00438}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< FileInfo >
    /// @brief Returns additional information on a file of this File Storage.
    /// The returned FileInfo struct contains information about the times when the file was created,
    /// last modified, and last accessed, and about how and by whom the file was created and last modified.
    /// The modificationTime, accessTime, and fileModificationState returned in the FileInfo
    /// are only accurate if the file is currently not open.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing a FileInfo struct.
    /// In case of anerror, it contains any of the errors defined below, or a vendor specific error.
    GetFileInfo(ara::core::StringView const fileName) const noexcept;
    /// @brief Open a readable and writable file
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00375}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// @brief Opens a file of this File Storage for reading and writing.
    /// The file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileReadWrite(ara::core::StringView const fileName) noexcept;
    /// @brief Open a readable and writable file using given read/write mode
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00113}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// @brief Opens a file of this File Storage for reading and writing with a defined mode.
    /// If not otherwise specified by the provided mode,
    /// the file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the PersistencyFile.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileReadWrite(ara::core::StringView const fileName, OpenMode const mode) noexcept;
    /// @brief Open a readable and writable file using given mode and given buffer
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00429}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// If not otherwise specified by the provided mode,
    /// the file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// The provided buffer will be used by the ReadWriteAccessor to implement block-wise
    /// reading and writing to speed up multiple small accesses to the file.
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @param[in] buffer Memory to be used for block-wise reading/writing.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    /// @brief Opens a file of this File Storage for reading and writing with a user provided buffer.
    OpenFileReadWrite(ara::core::StringView const fileName,
                      OpenMode const mode,
                      ara::core::Span< ara::core::Byte > buffer) noexcept;
    /// @brief Open a file in read-only mode
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00376}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadAccessor > >
    /// @brief Opens a file of this File Storage for reading.
    /// The file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the PersistencyFile.fileName of a configured file.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileReadOnly(ara::core::StringView const fileName) noexcept;
    /// @brief Open a file in read-only mode using given mode
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00114}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadAccessor > >
    /// @brief Opens a file of this File Storage for reading with a defined mode.
    /// If not otherwise specified by the provided mode,
    /// the file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileReadOnly(ara::core::StringView const fileName, OpenMode const mode) noexcept;
    /// @brief Open a file in read-only mode using given mode and given buffer
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00430}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadAccessor > >
    /// @brief Opens a file of this File Storage for reading with a user provided buffer.
    /// If not otherwise specified by the provided mode,
    /// the file is opened with the seek position set to the beginning (corresponding to kAtTheBeginning).
    /// The provided buffer will be used by the ReadAccessor to implement block-wise reading
    /// to speed up multiple small accesses to the file.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @param[in] buffer Memory to be used for block-wise reading.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileReadOnly(ara::core::StringView const fileName,
                     OpenMode const mode,
                     ara::core::Span< ara::core::Byte > buffer) noexcept;
    /// @brief Open a file in write-only mode
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00377}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// @brief Opens a file of this File Storage for writing.
    /// The file is truncated (corresponding to kTruncate).
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileWriteOnly(ara::core::StringView const fileName) noexcept;
    /// @brief Open file in write-only mode (AutoSar requirement is actually readable and writable)
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00115}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// @brief Opens a file of this File Storage for writing with a defined mode.
    /// If not otherwise specified by the provided mode, the file is truncated (corresponding to kTruncate).
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileWriteOnly(ara::core::StringView const fileName, OpenMode const mode) noexcept;
    /// @brief Open a file in write-only mode using given mode and given buffer
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00431}
    /// @threadsafety {re-entrant}
    /// @endcode
    ara::core::Result< UniqueHandle< ReadWriteAccessor > >
    /// @brief Opens a file of this File Storage for writing with a user provided buffer.
    /// If not otherwise specified by the provided mode, the file is truncated (corresponding to kTruncate).
    /// The provided buffer will be used by the ReadWriteAccessor to implement block-wise writing
    /// to speed up multiple small accesses to the file.
    /// If the file does not exist, it is created.
    /// The file will be closed when the returned UniqueHandle goes out of scope.
    /// @param[in] fileName Name of the file.
    /// May correspond to the Persistency File.fileName of a configured file.
    /// @param[in] mode Mode with which the file shall be opened.
    /// @param[in] buffer Memory to be used for block-wise writing.
    /// @returns A Result containing a UniqueHandle for the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    OpenFileWriteOnly(ara::core::StringView const fileName,
                      OpenMode const mode,
                      ara::core::Span< ara::core::Byte > buffer) noexcept;

public:
    /// @brief Initialize FileStorage using configuration in Manifest
    /// @param[in] storageIns The shortName path of a PortPrototype typed by aPersistencyFileStorageInterface.
    /// @param[in] bNewBuild Whether it is newly built
    /// @returns A Result of void.
    /// @threadsafety {re-entrant}
    ara::core::Result< void > InitStorage(ara::core::InstanceSpecifier const &storageIns, bool bNewBuild) noexcept;
    /// @brief Initialize FileStorage's KVS library using configuration generated by FileConfig
    /// @returns A Result of void.
    /// @threadsafety {re-entrant}
    ara::core::Result< void > PrepareKvSystem() noexcept;
    /// @brief Return Kv library engine pointer
    /// @return
    isoftkv::PKvSystem *GetKvSystem() const noexcept;
    /// @brief Backup current library
    /// @return
    ara::core::Result< bool > BackupMain() noexcept;
    /// @brief Delete main library
    /// @return
    ara::core::Result< bool > RemoveMain() noexcept;
    /// @brief Delete backup KV of main library
    /// @param bClose Whether to close KvSystem before deletion
    /// @return
    ara::core::Result< bool > RemoveMainBak(bool bClose) noexcept;
    /// @brief Restore current library using backup library
    /// @return
    ara::core::Result< bool > RecoverFromBackup() const noexcept;
    /// @brief Delete backup library
    /// @return
    ara::core::Result< bool > RemoveBackup() const noexcept;
    /// @brief Get full path file name
    /// @param[in] fileName
    /// @return
    ara::core::Result< ara::core::String > GetFileName(ara::core::StringView const &fileName) const noexcept;
    /// @brief Restore all files
    /// @returns A Result of void.
    /// @threadsafety {re-entrant}
    ara::core::Result< void > RecoverAllFile() noexcept;
    /// @threadsafety {re-entrant}
    /// @brief Reset all files
    /// @param fs port identifier
    /// @returns A Result of void.
    ara::core::Result< void > ResetStorage(ara::core::InstanceSpecifier const &fs) noexcept;
    /// @brief Get total space occupied by all files in bytes
    /// @returns A Result containing the occupied space in bytes.
    /// @threadsafety {re-entrant}
    ara::core::Result< uint64_t > GetStorageSpace() const noexcept;
    /// @brief Save file extension information and redundancy check information
    /// @param stFileName File name used as Key for Kv-Storage
    /// @param fileInfo Basic file information
    /// @param nReddAlgId Redundancy data encryption algorithm AlgId
    /// @param vecReddData Redundancy data
    /// @return Whether write was successful
    bool WriteCheckReddData(ara::core::StringView const &stFileName,
                            isoftkv::PFileInfoInKvs const &fileInfo,
                            uint64_t const nReddAlgId,
                            ara::core::Vector< uint8_t > const &vecReddData) const noexcept;
    /// @brief Get file extension information from KV library
    /// @param fileName
    /// @return PFileInfoInKvs structure saved in KV library
    isoftkv::PFileInfoInKvs GetFileInfoInKvs(ara::core::StringView const fileName) const noexcept;
    /// @brief Get file redundancy check information from KV library
    /// @param fileName
    /// @return Redundancy check information
    ara::core::Vector< uint8_t > GetFileReddData(ara::core::StringView const fileName) const noexcept;
    /// @brief Calculate redundancy check code for this file
    /// @param fileName
    /// @return
    ara::core::Vector< uint8_t > CalculateFileReddData(ara::core::StringView const fileName) const noexcept;
    /// @brief Get path-containing redundant file name corresponding to filename
    /// @param fileName KV library internal file Key
    /// @param nReddIndex Redundancy index
    /// @return Local path-containing file name
    ara::core::String GetReddFileName(ara::core::StringView const fileName, int32_t nReddIndex) const noexcept;
    /// @brief Get file extension information from KV library
    /// @param fileName
    /// @return PFileInfoInKvs structure saved in KV library
    isoftkv::PFileInfoInKvs_ReddExt ReadFileInfoEx(ara::core::StringView const fileName,
                                                   uint16_t nVersion) const noexcept;
    /// @brief Set file extension information in KV library
    /// @param fileName File name used as Key for Kv-Storage
    /// @param reddEx File extension information
    /// @return PFileInfoInKvs structure saved in KV library
    bool SaveFileInfoEx(ara::core::StringView const fileName,
                        isoftkv::PFileInfoInKvs_ReddExt const &fileInfoEx) noexcept;
    /// @brief Check if space allows writing new data
    /// @param nAddLen
    /// @return
    bool CheckSpace(int32_t nAddLen) noexcept;

protected:
    /// @brief Convert relative file name to full path file name
    /// @param[in] fileName
    /// @return
    ara::core::String _GetFileName(ara::core::StringView const &fileName) const noexcept;
    /// @brief Add new file information to Kv library
    /// @param[in] fileName
    /// @param[in] fileCreationState
    /// @param[in] fileModificationState
    /// @return
    ara::core::Result< bool > _AddNewFileToKv(ara::core::StringView const &fileName,
                                              FileCreationState const fileCreationState,
                                              FileModificationState const fileModificationState) const noexcept;
    /// @brief Traverse each redundant file, fileName is name in KV library (without path info)
    /// @param[in] fileName
    /// @param[in] pfun
    /// @return
    int32_t _ScanAllReddFile(ara::core::StringView const &fileName,
                             std::function< bool(ara::core::StringView const &stReddFile) > const &pfun) const noexcept;
    /// @brief Select correct file under M/N mode
    /// @param[in] fileName
    /// @return
    ara::core::String _SelectFileMofN(ara::core::StringView const &fileName) const noexcept;
    /// @brief Open file in read-only mode
    /// @param[in] fileName
    /// @param[in] pMode
    /// @param[in] pBuffer
    /// @return
    ara::core::Result< UniqueHandle< ReadAccessor > > _OpenFileReadOnly(
        ara::core::StringView const &fileName,
        OpenMode const *const pMode,
        ara::core::Span< ara::core::Byte > *const pBuffer) noexcept;
    /// @brief Open file in read-write mode
    /// @param[in] bWriteOnly
    /// @param[in] fileName
    /// @param[in] pMode
    /// @param[in] pBuffer
    /// @return
    ara::core::Result< UniqueHandle< ReadWriteAccessor > > _OpenFileWrite(
        bool const bWriteOnly,
        ara::core::StringView const &fileName,
        OpenMode const *const pMode,
        ara::core::Span< ara::core::Byte > *const pBuffer) noexcept;
    /// @brief Delete file
    /// @param fileName File name
    /// @param bRemoveFromKv Whether to delete corresponding file record from KV library
    /// @return
    ara::core::Result< void > _RemoveFile(ara::core::StringView const &fileName, bool bRemoveFromKv) noexcept;
    /// @brief Reset file to initialization state in Manifest configuration
    /// @param fileName
    /// @return
    ara::core::Result< void > _ResetFile(ara::core::StringView const &fileName) noexcept;
    /// @brief
    /// @param fileName
    /// @return
    ara::core::Result< void > _ResetFileNoLock(ara::core::StringView const fileName) noexcept;
    /// @brief
    /// @param fileName
    /// @return
    ara::core::Result< void > _RecoverFile(ara::core::StringView const &fileName) noexcept;
    /// @brief Execute logic for restoring a single file
    /// @param[in] fileName File name to be restored
    /// @param[out] pVecReport Redundancy indices of restored files
    /// @return Whether execution was successful
    bool _RecoverFile(ara::core::StringView const &fileName, ara::core::Vector< uint8_t > *pVecReport) noexcept;
    /// @brief Check if redundancy check information is consistent
    /// @param fileName
    /// @return If no redundancy check configuration, return true directly
    bool _CheckReddData(ara::core::StringView const &fileName) const noexcept;
    /// @brief Save file extension information and redundancy check information
    /// @param stFileName File name used as Key for Kv-Storage
    /// @param fileInfo Basic file information
    /// @param fileInfoEx File extension information
    /// @param vecReddData Redundancy data
    /// @return Whether write was successful
    bool _WriteCheckReddData(ara::core::StringView const &stFileName,
                             isoftkv::PFileInfoInKvs const &fileInfo,
                             isoftkv::PFileInfoInKvs_ReddExt const &fileInfoEx,
                             ara::core::Vector< uint8_t > const &vecReddData) const noexcept;
    /// @brief Check if file count exceeds limit
    /// @param nNewCount
    /// @return
    bool _CheckFileCount(int32_t nNewCount) const noexcept;
    /// @brief Copy file into FileStorage: Target file may require encryption
    /// @param fileName
    /// @param stSrcFile
    /// @param stDstFile
    /// @return
    bool _CopyStorageFile(ara::core::StringView const &fileName,
                          ara::core::StringView const &stSrcFile,
                          ara::core::StringView const &stDstFile);
};
//********************************/
/// @brief Open/create a file data center
/// @code{.isoft}
/// @unit_name=OpenFileStorage
/// @traceid {SWS_PER_00116}
/// @threadsafety {re-entrant}
/// @endcode
ara::core::Result< SharedHandle< FileStorage > >
/// @brief Opens a File Storage.
/// OpenFileStorage will fail with kResourceBusy
/// when the File Storage is currently being modifiedby a call from another thread
/// to UpdatePersistency, ResetPersistency, RecoverAllFiles, or ResetAllFiles.
/// Because multiple threads can access the same File Storage concurrently, the File Storage might
/// not be closed when the SharedHandle returned by this function goes out of scope.
/// It will only be closed when all SharedHandles that refer to the same File Storage went out of scope.
/// @param[in] fs  The shortName path of a PortPrototype typed by aPersistencyFileStorageInterface.
/// @returns A Result containing a SharedHandle for the File Storage.
/// In case of an error, it contains any of the errors defined below, or a vendor specific error.
OpenFileStorage(ara::core::InstanceSpecifier const &fs) noexcept;
//********************************/
/// @brief Restore all files
/// @code{.isoft}
/// @unit_name=RecoverAllFiles
/// @traceid {SWS_PER_00335}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Recovers a File Storage, including all files.
/// RecoverAllFiles recovers a File Storage when the redundancy checks fail.
/// It will fail with kResourceBusy when the File Storage is currently open, or when it is modified by
/// a call from another thread to UpdatePersistency, ResetPersistency, RecoverAllFiles, or ResetAllFiles.
/// This method does a best-effort recovery of all files.
/// After recovery, files might show outdated or initial content, or might be lost.
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyFileStorageInterface.
/// @returns A Result of void.
/// In case of an error, it contains anyof the errors defined below, or a vendor specific error.
ara::core::Result< void > RecoverAllFiles(ara::core::InstanceSpecifier const &fs) noexcept;
//********************************/
/// @brief Reset all files
/// @code{.isoft}
/// @unit_name=ResetAllFiles
/// @traceid {SWS_PER_00336}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Resets a File Storage, including all files.
/// ResetAllFiles resets a File Storage to the initial state, containing only the files which
/// were deployed from the manifest, with their initial content.
/// It will fail with kResourceBusy when the File Storage is currently open,
/// or when it is modified by a call from another thread
/// to UpdatePersistency, ResetPersistency, RecoverAllFiles, or ResetAllFiles.
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyFileStorageInterface.
/// @returns A Result of void.
/// In case of an error, it contains anyof the errors defined below, or a vendor specifi cerror.
ara::core::Result< void > ResetAllFiles(ara::core::InstanceSpecifier const &fs) noexcept;
//********************************/
/// @brief Get storage space occupied by current file data center in bytes (including redundancy and backup files)
/// @code{.isoft}
/// @unit_name=GetCurrentFileStorageSize
/// @traceid {SWS_PER_00406}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Returns the space in bytes currently occupied by a File Storage.
/// The returned size includes all meta data and the space used for redundancy and backups.
/// The returned size is only accurate if no other operation on the File Storage
/// takes place at the same time.
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyFileStorageInterface.
/// @returns A Result containing the occupied space in bytes.
/// In case of an error, it contains any of the errors defined below, or a vendor specific error.
ara::core::Result< uint64_t > GetCurrentFileStorageSize(ara::core::InstanceSpecifier const &fs) noexcept;
//********************************/
/// @brief File descriptor "|" operation
/// @code{.isoft}
/// @unit_name=operator|
/// @traceid {SWS_PER_00144}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Merges two OpenMode modifiers into one.
/// @param[in] left First OpenMode modifiers.
/// @param[in] right Second OpenMode modifiers.
/// @returns returns Merged OpenMode modifiers.
constexpr OpenMode operator|(OpenMode const left, OpenMode const right) noexcept
{
    uint32_t const nOrResult{static_cast< uint32_t >(left) | static_cast< uint32_t >(right)};
    // kAtTheBeginning and kAtTheEnd are mutually exclusive; kTruncate and kAppend are mutually exclusive;
    if ((static_cast< uint32_t >(OpenMode::kAtTheBeginning) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kAtTheEnd) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kTruncate) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kAppend) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kOpenMode_Mult_1) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kOpenMode_Mult_2) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kOpenMode_Mult_3) == nOrResult)
        || (static_cast< uint32_t >(OpenMode::kOpenMode_Mult_4) == nOrResult)) {
        return static_cast< OpenMode >(nOrResult);
    }
    return OpenMode::kOpenMode_Error;
}
//********************************/
/// @brief File descriptor "=|" operation
/// @code{.isoft}
/// @unit_name=operator|=
/// @traceid {SWS_PER_00434}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Merges an OpenMode modifier into this OpenMode.
/// @param[out] left First OpenMode modifiers.
/// @param[in] right Second OpenMode modifiers.
/// @returns returns The modified OpenMode.
OpenMode &operator|=(OpenMode &left, OpenMode const &right) noexcept;
//********************************/
/// @brief Delete a File data center: Internal interface 2023-08-11
/// @code{.isoft}
/// @unit_name=RemoveFileStorage
/// @endcode
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
ara::core::Result< void > RemoveFileStorage(ara::core::InstanceSpecifier const &fs) noexcept;
//********************************/
}  // namespace per
}  // namespace ara

#endif
