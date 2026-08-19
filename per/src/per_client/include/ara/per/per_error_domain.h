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
/// @file       per_error_domain.h
/// @brief      Error handling
/// @details    AutoSar-AP data persistence storage module
/// @date       2021-04-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-28  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Error Handling
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PER_ERROR_DOMAIN_H_
#define ARA_PER_PER_ERROR_DOMAIN_H_

#include <utility>

#include "ara/core/exception.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"

namespace ara {
namespace per {
//********************************/
/// @brief Persistence error code enumeration
/// @code{.isoft}
/// @unit_name=PerErrc
/// @traceid {SWS_PER_00311}
/// @endcode
/// @brief Defines the errors for Persistency.
/// The enumeration values 0 - 255 are reserved for AUTOSAR assigned errors,
/// the stack provider is free to define additional errors starting from 256.
enum class PerErrc : ara::core::ErrorDomain::CodeType
{
    /// @brief Requested Key-Value storage or file storage not configured in AUTOSAR model.
    /// The requested Key-Value Storage or File Storage is not configured in the AUTOSAR model.
    kStorageNotFound = 1,
    /// @brief Provided key cannot be found in key-value storage.
    /// The provided key cannot be not found in the Key-Value Storage.
    kKeyNotFound = 2,
    /// @brief Failed to open file for writing, changing, or synchronizing keys because storage is configured as read-only.
    /// Opening a file for writing or changing, or synchronizing a key failed,
    /// because the storage is configured read-only.
    kIllegalWriteAccess = 3,
    /// @brief Error occurred when accessing physical storage, e.g., due to corrupted file system or hardware, or insufficient access permissions.
    /// An error occurred when accessing the physical storage, e.g. because of
    /// a corrupted file system or corrupted hardware, or because of insufficient access rights.
    kPhysicalStorageFailure = 4,
    /// @brief Unable to establish structural integrity of storage. This may occur with internal structure corruption of key-value storage or metadata corruption of file storage.
    /// The structural integrity of the storage could not be established.
    /// This can happen when the internal structure of a Key-Value Storage
    /// or the meta data of a File Storage is corrupted.
    kIntegrityCorrupted = 5,
    /// @brief Redundancy measure verification failed for a single key, entire key-value storage, single file, or entire file storage.
    /// The validation of redundancy measures failed for asingle key,
    /// for the whole Key-Value Storage, for a single file, or for the whole File Storage.
    kValidationFailed = 6,
    /// @brief Encryption or decryption failed for a single key, entire key-value storage, single file, or entire file storage.
    /// The encryption or decryption failed for a single key,
    /// for the whole Key-Value Storage, for a single file, or for the whole File Storage.
    kEncryptionFailed = 7,
    /// @brief Provided data type does not match stored data type.
    /// The provided data type does not match the stored data type.
    kDataTypeMismatch = 8,
    /// @brief Operation cannot be performed because no initial value is available.
    /// The operation could not be performed because no initial value is available.
    kInitValueNotAvailable = 9,
    /// @brief Operation cannot be performed because resources are currently busy.
    /// The operation could not be performed because the resource is currently busy.
    kResourceBusy = 10,
    /// @brief Allocated storage quota exceeded.
    /// The allocated storage quota was exceeded.
    kOutOfStorageSpace = 12,
    /// @brief Requested file cannot be found in file storage.
    /// The requested file cannot be not found in the FileStorage.
    kFileNotFound = 13,
    /// @brief Calling Persistency function or its class method before ara::core::Initialize() or after ara::core::Deinitialize().
    /// A function of Persistency or a method of one of its classes was called
    /// before ara::core::Initialize() or after ara::core::Deinitialize().
    kNotInitialized = 14,
    /// @brief SetPosition attempted to move to an unreachable position (i.e., less than zero or greater than current file size).
    /// SetPosition tried to move to a position that is not reachable
    /// (i.e. which is smaller than zero or greater than the current size of the file).
    kInvalidPosition = 15,
    /// @brief Application attempted to read from end of file or empty file.
    /// The application tried to read from the end of the file or from an empty file.
    kIsEof = 16,
    /// @brief Opening file failed due to invalid combination of requested OpenModes.
    /// Opening a file failed because the requested combination of OpenModes is invalid.
    kInvalidOpenMode = 17,
    /// @brief Attempted to set a new size larger than current file size.
    /// SetFileSize tried to set a new size that is bigger than the current file size.
    kInvalidSize = 18,
    /// @traceid {SWS_PER_00542}
    /// @brief 2211 standard The maximum number of files was exceeded.
    kTooManyFiles = 19,
    /// @traceid {SWS_PER_00543}
    /// @brief The allocated storage quota was exceeded.
    kQuotaExceeded = 20,
    /// @traceid {SWS_PER_00564}
    /// @brief Calculating or checking of the MAC failed for a single key-value pair, for the whole Key-Value Storage, for a single file, or for the whole File Storage.
    kAuthenticationFailed = 21,

    /// @brief Start of Puhua-specific error codes
    kPhErrorCode = 256,
    /// @brief Puhua: Error opening PHKV update file
    kPhOptUpdateFile = kPhErrorCode + 1,
    /// @brief Puhua: Error operating KV database file
    kPhOptKvFile = kPhErrorCode + 2,
    /// @brief Puhua: File version number error
    kPhFileVersion = kPhErrorCode + 3,
    /// @brief Puhua: Error initializing KV database file
    kPhKvLogicInit = kPhErrorCode + 4,
    /// @brief Puhua: Cache page error when operating KV database file
    kPhKvLogicCache = kPhErrorCode + 5,
    /// @brief Puhua: Buffer capacity error provided when reading KV data
    kPhKvReadBuffCapacity = kPhErrorCode + 6,
    /// @brief Puhua: Failed to create directory
    kPhKvCreateDirFailed = kPhErrorCode + 7,
};
//********************************/
/// @brief Persistence exception handling class.
/// @code{.isoft}
/// @unit_name=PerException
/// @traceid {SWS_PER_00354}
/// @endcode
/// @brief Exception type thrown by Persistency classes.
class PerException : public ara::core::Exception
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00355}
    /// @endcode
    /// @brief Construct a new Persistency exception object containing an error code.
    /// @param[in] errorCode The error code.
    explicit PerException(ara::core::ErrorCode errorCode) noexcept : ara::core::Exception{std::move(errorCode)} {}

public:
    /// @brief
    ~PerException() override = default;
    /// @brief
    /// @param[in] a
    PerException(PerException const &a) = default;
    /// @brief
    /// @param[in] a
    /// @return
    PerException &operator=(PerException const &a) = delete;
    /// @brief
    /// @param[in] a
    PerException(PerException &&a) noexcept = default;
    /// @brief
    /// @param[in] a
    /// @return
    PerException &operator=(PerException &&a) noexcept = delete;

private:
};
//********************************/
/// @brief
constexpr uint64_t const kInt64_Error{0x8000000000000101ULL};
/// @brief Persistence error handling class.
/// @code{.isoft}
/// @unit_name=PerErrorDomain
/// @traceid {SWS_PER_00312}
/// @endcode
/// @UniqueID 0x8000’0000’0000’0101
/// @brief Defines the error domain for Persistency.
class PerErrorDomain final : public ara::core::ErrorDomain
{
public:
    /// @brief Alias for error code value enumeration.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00411}
    /// @endcode
    /// @brief Alias for the error code value enumeration.
    using Errc = PerErrc;
    /// @brief Alias for exception handling class.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00412}
    /// @endcode
    /// @brief Alias for the error code value enumeration.
    using Exception = PerException;
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00313}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Creates a PerErrorDomain instance.
    PerErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}
    /// @brief Return error code name.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00314,SWS_PER_00353}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Returns the name of the error domain.
    /// @returns The name of the error domain.
    char8_t const *Name() const noexcept final { return "Per"; }
    /// @brief Return error description corresponding to error code.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00315}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Returns the message associated with the error code.
    /// @param[in] errorCode The error code number.
    /// @returns The message associated with the error code.
    char8_t const *Message(CodeType errorCode) const noexcept final
    {
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch-enum"
#endif
        char8_t const *pchReturn{nullptr};
        Errc const code{static_cast< Errc >(errorCode)};
        switch (code) {
            case PerErrc::kStorageNotFound: {
                pchReturn = "The requested Key-Value Storage or File Storage is not configured in the AUTOSAR model.";
            } break;
            case PerErrc::kKeyNotFound: {
                pchReturn = "The provided key cannot be not found in the Key-Value Storage.";
            } break;
            case PerErrc::kIllegalWriteAccess: {
                pchReturn
                    = "Opening a file for writing or changing, or synchronizing a key failed, because the storage is "
                      "configured read-only.";
            } break;
            case PerErrc::kPhysicalStorageFailure: {
                pchReturn = "An error occurred when accessing the physical storage.";
                // {pchReturn = "An error occurred when accessing the physical storage, e.g. because of a corrupted file
                // system or corrupted hardware, or because of insufficient access rights.";
            } break;
            case PerErrc::kIntegrityCorrupted: {
                pchReturn = "The structural integrity of the storage could not be established.";
                // {pchReturn = "The structural integrity of the storage could not be established. This can happen when
                // the internal structure of a Key-Value Storage or the meta data of a File Storage is corrupted.";
            } break;
            case PerErrc::kValidationFailed: {
                pchReturn = "The validation of redundancy measures failed.";
                // {pchReturn = "The validation of redundancy measures failed for asingle key, for the whole Key-Value
                // Storage, for asingle file, or for the whole File Storage.";
            } break;
            case PerErrc::kEncryptionFailed: {
                pchReturn = "The encryption or decryption failed.";
                // {pchReturn = "The encryption or decryption failed for a single key, for the whole Key-Value Storage,
                // for a single file, or for the whole File Storage.";
            } break;
            case PerErrc::kDataTypeMismatch: {
                pchReturn = "The provided data type does not match the stored data type.";
            } break;
            case PerErrc::kInitValueNotAvailable: {
                pchReturn = "The operation could not be performed because no initial value is available.";
            } break;
            case PerErrc::kResourceBusy: {
                pchReturn = "The operation could not be performed because the resource is currently busy.";
            } break;
            case PerErrc::kOutOfStorageSpace: {
                pchReturn = "The allocated storage quota was exceeded.";
            } break;
            case PerErrc::kFileNotFound: {
                pchReturn = "The requested file cannot be not found in the FileStorage.";
            } break;
            case PerErrc::kNotInitialized: {
                pchReturn
                    = "A function of Persistency or a method of one of its classes was called before "
                      "ara::core::Initialize() or after ara::core::Deinitialize().";
            } break;
            case PerErrc::kInvalidPosition: {
                pchReturn = "SetPosition tried to move to a position that is not reachable.";
            } break;
                // {pchReturn = "SetPosition tried to move to a position that is not reachable (i.e. which is smaller
                // than zero or greater than the current size of the file).";
            case PerErrc::kIsEof: {
                pchReturn = "The application tried to read from the end of the file or from an empty file.";
            } break;
            case PerErrc::kInvalidOpenMode: {
                pchReturn = "Opening a file failed because the requested combination of OpenModes is invalid.";
            } break;
            case PerErrc::kInvalidSize: {
                pchReturn = "SetFileSize tried to set a new size that is bigger than the current file size.";
            } break;
            case PerErrc::kTooManyFiles: {
                pchReturn = "The maximum number of files was exceeded.";
            } break;
            case PerErrc::kQuotaExceeded: {
                pchReturn = "The allocated storage quota was exceeded.";
            } break;
            case PerErrc::kAuthenticationFailed: {
                pchReturn
                    = "Calculating or checking of the MAC failed for a single key-value pair, for the whole Key-Value "
                      "Storage, for a single file, or for the whole File Storage.";
            } break;
            case PerErrc::kPhErrorCode: {
                pchReturn = "PUHUA: Error Code Start Line.";  //"PUHUA: Error code start line";
            } break;
            case PerErrc::kPhOptUpdateFile: {
                pchReturn
                    = "PUHUA: Find Update File Error When Open KvFile.";  //"PUHUA: Error opening PHKV update file";
            } break;
            case PerErrc::kPhOptKvFile: {
                pchReturn = "PUHUA: Find Error When Operate KvFile.";  //"PUHUA: Error operating KV database file";
            } break;
            case PerErrc::kPhFileVersion: {
                pchReturn = "PUHUA: Find File Version Error.";  //"PUHUA: File version number error";
            } break;
            case PerErrc::kPhKvLogicInit: {
                pchReturn = "PUHUA: Find Error When Init KvFile.";  //"PUHUA: Error initializing KV database file";
            } break;
            case PerErrc::kPhKvLogicCache: {
                pchReturn
                    = "PUHUA: Find CachePage Error When Operate KvFile.";  //"PUHUA: Cache page error when operating KV database file";
            } break;
            case PerErrc::kPhKvReadBuffCapacity: {
                pchReturn
                    = "PUHUA: Find Buff Capacity Error When Read KvData.";  //"PUHUA: Buffer capacity error provided when reading KV data";
            } break;
            case PerErrc::kPhKvCreateDirFailed: {
                pchReturn = "PUHUA: Create dir failed.";  //"PUHUA: Failed to create directory";
            } break;
            default: {
                pchReturn = "Unknown error";
            } break;
        }
        return pchReturn;
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
    }
    /// @brief Throw exception of persistence module.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00350}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Throws the exception associated with the error code.
    /// @param[in] errorCode The error to throw.
    /// @returns None
    /// @throw
    void ThrowAsException(ara::core::ErrorCode const &errorCode) const final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }

private:
    /// @brief Key ID for persistency error domain.
    /// @traceid {SWS_PER_00349}    //Deleted in 20-11 version
    static ErrorDomain::IdType const kId{kInt64_Error};
};
//********************************/
namespace internal_per {
/// @brief
PerErrorDomain const kPerErrorDomain{};  // NOLINT [fuchsia-statically-constructed-objects]
}  // namespace internal_per
/// @brief Get error code saved by persistence module.
/// @code{.isoft}
/// @unit_name=GetPerDomain
/// @traceid {SWS_PER_00352}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Returns the global PerErrorDomain object.
/// @returns The global PerErrorDomain object.
constexpr ara::core::ErrorDomain const &GetPerDomain() noexcept { return internal_per::kPerErrorDomain; }
/// @brief Construct error code
/// @code{.isoft}
/// @unit_name=MakeErrorCode
/// @traceid {SWS_PER_00351}
/// @threadsafety {re-entrant}
/// @endcode
constexpr ara::core::ErrorCode
/// @brief Creates an error code.
/// @param[in] code Error code number.
/// @param[in] data Vendor defined data associated with the error.
/// @returns  An ErrorCode object.
MakeErrorCode(PerErrc const code, ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetPerDomain(), data);
}
//********************************/
}  // namespace per
}  // namespace ara
#endif
