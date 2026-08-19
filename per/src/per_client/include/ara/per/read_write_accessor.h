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
/// @file       read_write_accessor.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    File modification
/// @date       2021-04-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-27  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/FileStorage/File Accessor
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_READ_WRITE_ACCESSOR_H_
#define ARA_PER_READ_WRITE_ACCESSOR_H_

#include "ara/core/span.h"
#include "ara/per/per_error_domain.h"
#include "ara/per/read_accessor.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Bridge for calling functions belonging to FileStorage
/// @code{.isoft}
/// @interface_level=unit
/// @unit_name=ICallStorage
/// @traceid {SWS_PER_00343}
/// @endcode
class ICallStorage
{
public:
    /// @brief Default constructor
    ICallStorage() = default;
    /// @brief
    virtual ~ICallStorage() = default;
    /// @brief Copy constructor
    /// @param other Another object instance of this class
    ICallStorage(ICallStorage const &other) = default;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    ICallStorage(ICallStorage &&other) noexcept = default;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    ICallStorage &operator=(ICallStorage const &other) = default;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    ICallStorage &operator=(ICallStorage &&other) noexcept = default;
    /// @brief Call Storage-related functions
    /// @param pFileOpt Pointer to file operation object
    /// @return Success or failure
    virtual bool CallStorage_WriteReddData(isoftkv::PFileOpt const *const pFileOpt,
                                           ara::core::StringView const &stFileName) noexcept = 0;
    /// @brief Check if space allows writing new data
    /// @param nAmendLen
    /// @return
    virtual bool CheckSpace(int32_t nAmendLen) noexcept = 0;
};
}  // namespace isoftkv
//********************************/
/// @brief File content read/write operation class
/// @code{.isoft}
/// @unit_name=ReadWriteAccessor
/// @traceid {SWS_PER_00343}
/// @endcode
/// @brief ReadWriteAccessor is used to read and write file data.
/// It provides the WriteBinary and WriteText methods featuring a Result for controlled,
/// unformatted writing, and the operator<< method for simple formatted writing.
/// It also provides SyncToFile() to flush the buffer of the operating system to the storage.
class ReadWriteAccessor final : public ReadAccessor
{
private:
    std::unique_ptr< isoftkv::ICallStorage > pCallStorage_;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00462}
    /// @threadsafety {re-entrant}
    /// @endcode
    ReadWriteAccessor() = delete;
    /// @brief
    /// @param[in] a
    ReadWriteAccessor(ReadWriteAccessor const &a) = delete;
    /// @brief
    /// @param[in] a
    /// @return
    ReadWriteAccessor &operator=(ReadWriteAccessor const &a) = delete;
    /// @brief
    /// @param[in] a
    ReadWriteAccessor(ReadWriteAccessor &&a) noexcept = delete;
    /// @brief
    /// @param[in] a
    /// @return
    ReadWriteAccessor &operator=(ReadWriteAccessor &&a) noexcept = delete;
    /// @brief Configuration callback
    /// @param pCallStorage
    void AttachCallStorage(std::unique_ptr< isoftkv::ICallStorage > &&pCallStorage) noexcept;

public:
    /// @brief Constructor
    /// @threadsafety {re-entrant}
    /// @brief Normal constructor for ReadAccessor.
    /// @param[in] reddData Redundancy setting MofN
    explicit ReadWriteAccessor(isoftkv::PReddDataMofN const &reddData) noexcept;
    /// @brief
    ~ReadWriteAccessor();
    /// @brief Flush current file content to physical device.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00122}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Triggers flushing of the current file content to the physical storage.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< void > SyncToFile() noexcept;
    /// @brief Set file size, discarding excess content if file shrinks. File cursor will also adjust to a valid position.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00428}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Reduces the size of the file to ’size’,
    /// effectively removing the current content of the file beyond this size.
    /// The current file position is unchanged if it is lower than ’size’,
    /// or set to the last valid position in the file otherwise.
    /// If ’size’ is 0, the current file position will also be set to 0.
    /// @param[in] size New size of the file.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< void > SetFileSize(uint64_t const size) noexcept;
    /// @brief Write a string starting from current file position.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00166}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Writes the content of a StringView to the file.
    /// The time when the content is persisted depends on the implementation of Persistency.
    /// SyncToFile can be used to force Persistency to persist the file content.
    /// In case of an error, the file content might be corrupted,
    /// and the current position might or might not have changed.
    /// The expected state of the file for each supported error can be expected to be as follows:
    /// kPhysicalStorageFailure     The state of the file is unknown. It could have been entirely destroyed.
    /// kEncryptionFailed           The content of the file and the current position will have been updated,
    ///     butcould not be persisted. The persisted file will reflect an older version of the file.
    /// kOutOfStorageSpace          The content of the file will have been updated, but the part of the operation
    ///     that exceeded the quota will have been discarded. The current position will be at the end of the file.
    /// kNotInitialized             The content of the file and the current position have not been changed.
    /// @param[in] s A StringView containing the characters to be written.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< void > WriteText(ara::core::StringView const s) noexcept;
    /// @brief Write a Span byte stream starting from current file position.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00423}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Writes the content of a Span of Byte to the file.
    /// The time when the content is persisted depends on the implementation of Persistency.
    /// SyncToFile can be used to force Persistency to persist the file content.
    /// In case of an error, the file content might be corrupted,
    /// and the current position might or might not have changed.
    /// The expected state of the file for each supported error can be expected to be as follows:
    /// kPhysicalStorageFailure     The state of the file is unknown. It could have been entirely destroyed.
    /// kEncryptionFailed           The content of the file and the current position will have been updated,
    ///     butcould not be persisted. The persisted file will reflect an older version of the file.
    /// kOutOfStorageSpace          The content of the file will have been updated, but the part of the operation
    ///     that exceeded the quota will have been discarded. The current position will be at the end of the file.
    /// kNotInitialized             The content of the file and the current position have not been changed.
    /// @param[in] b A Span of Byte containing the bytes to be written.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< void > WriteBinary(ara::core::Span< ara::core::Byte const > const b) noexcept;
    /// @brief Write a string at current position using "<<" operator.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00125}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Writes the content of a StringView to the file.
    /// This operator is just a comfort feature for non-safety critical applications.
    /// If an error occurs during this operation, it is silently ignored.
    /// @param[in] s The StringView containing the characters to bewritten.
    /// @returns The ReadWriteAccessor object.
    ReadWriteAccessor &operator<<(ara::core::StringView const s) noexcept;

protected:
    /// @brief Traverse each FileOpt, execute callback function sequentially
    /// @param[in] eCode Error code returned when function encounters error
    /// @param[in] pfun Callback function
    /// @return
    ara::core::Result< void > _ForEachFileOptWrite(PerErrc eCode, CB_DealFileOpt_Const const &pfun) const noexcept;
    /// @brief Save possible redundancy check information
    /// @return
    ara::core::Result< void > _SaveFileReddData() const noexcept;
    /// @brief Check if space allows writing new data
    /// @param nAmendLen
    /// @return
    bool _CheckSpace(uint64_t nNewFileSize) noexcept;
    /// @brief Check if space allows writing new data: Calculation considers exceeding original file size from current position
    /// @param nAmendLen
    /// @return
    bool _CheckSpace_Write(uint32_t nWriteSize) noexcept;
};
//********************************/
}  // namespace per
}  // namespace ara

#endif
