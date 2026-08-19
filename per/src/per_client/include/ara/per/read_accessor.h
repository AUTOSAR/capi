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
/// @file       read_accessor.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    File reading
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
/// @unit_name=ReadAccessor
/// @unit_description=File reading
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_READ_ACCESSOR_H_
#define ARA_PER_READ_ACCESSOR_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_check_redd.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/crypto/isoft_per_crypto.h"
#include "ara/per/internal/isoftkv/config_muster.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"

namespace ara {
namespace per {
//********************************/
/// @brief File read/write cursor enumeration
/// @code{.isoft}
/// export_level=/Per
/// @traceid {SWS_PER_00146}
/// @endcode
/// @brief Specification of origin used in MovePosition.
enum class Origin : uint32_t
{
    kBeginning = 0,  // Seek from the beginning of the file.
    kCurrent   = 1,  // Seek from the current position.
    kEnd       = 2,  // Seek from the end of the file.
};
//********************************/
/// @brief File content reading operation class
/// @code{.isoft}
/// export_level=/Per
/// @traceid {SWS_PER_00342}
/// @endcode
/// @brief ReadAccessor is used to read file data.
/// It provides binary and text mode methods for checking or getting the current byte/character
/// (PeekByte/PeekChar, GetByte/GetChar) methods for reading a section of a binary/text file
/// (ReadBinary/ReadText), a method to read a line of text (ReadLine), and methods for checking
/// and setting the current position in the file (GetPosition, SetPosition, MovePosition, IsEof)
/// and for checking the current size of the file (GetSize).
class ReadAccessor
{
public:
    /// @brief Cursor and buffer operation class using cached read/write
    class PAccessBuff;

private:
    /// @brief
    mutable ara::core::Vector< std::unique_ptr< isoftkv::PFileOpt > > vecReddFileOpt_;
    /// @brief
    isoftkv::PReddDataMofN reddDataMofN_;
    /// @brief File cursor and buffer
    std::unique_ptr< PAccessBuff > pAccessBuff_;
    /// @brief Encryption/decryption configuration
    std::unique_ptr< isoftkv::PConfigData_Crypto > pConfigCrypto_;
    /// @brief Redundancy error detection
    mutable isoftkv::PCheckRedd_File checkReddFile_;
    /// @brief Belonging Storage name
    ara::core::StringView stStorageIns_;
    /// @brief File identifier name within Storage (not necessarily the file name operated by FileOpt)
    ara::core::String stFileName_;

public:
    /// @brief Constructor
    /// @threadsafety {re-entrant}
    /// @brief Normal constructor for ReadAccessor.
    /// @param[in] reddData
    explicit ReadAccessor(isoftkv::PReddDataMofN const &reddData) noexcept;
    /// @brief Default constructor: Deleted
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00461}
    /// @threadsafety {re-entrant}
    /// @endcode
    ReadAccessor() = delete;
    /// @brief Disable copy constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00415}
    /// @endcode
    /// @brief The copy constructor for ReadAccessor shall not be used.
    /// @param[in] a
    ReadAccessor(ReadAccessor const &a) = delete;
    /// @brief Disable copy assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00416}
    /// @endcode
    /// @brief The copy assignment operator for ReadAccessor shall not be used.
    /// @param[in] a
    /// @return
    ReadAccessor &operator=(ReadAccessor const &a) = delete;
    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00413}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Move constructor for ReadAccessor.
    /// @param[in] ra The ReadAccessor object to be moved.
    ReadAccessor(ReadAccessor &&ra) noexcept = delete;
    /// @brief Move assignment function
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00414}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @param[in] ra The ReadAccessor object to be moved.
    /// @brief Move assignment operator for ReadAccessor.
    /// @returns The moved ReadAccessor object.
    ReadAccessor &operator=(ReadAccessor &&ra) &noexcept = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00417}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Destructor for ReadAccessor.
    virtual ~ReadAccessor() noexcept;
    /// @brief Bind a SpanBuffer
    /// @param[in] pSpanBuff
    void AttachSpanBuff(ara::core::Span< ara::core::Byte > *const pSpanBuff) const noexcept;
    /// @brief Set encryption configuration
    /// @param[in] configCrypto
    void AttachCryptoConfig(isoftkv::PConfigData_Crypto const &configCrypto) noexcept;
    /// @brief Initialize file operation class object
    /// @param[in] stFileName The filename to be access.
    /// @param[in] nFileFlags File operation mode parameter used by nai library
    /// @return Success or failure
    /// @threadsafety {no}
    ara::core::Result< bool > InitFileOpt(ara::core::StringView const &stFileName,
                                          uint32_t const nFileFlags = isoftkv::PFileOpt::ReadOnly()) noexcept;
    /// @brief Get file identifier name saved within Storage
    /// @return
    ara::core::StringView GetFileName_InStorage() const noexcept { return stFileName_; }
    /// @brief Initialize Storage information to which the file belongs
    /// @param stStorageIns Storage identifier
    /// @param stFileName
    void InitOwnStorage(ara::core::StringView const &stStorageIns, ara::core::StringView const &stFileName);
    /// @brief Verify possible PConfigData_Crypto.stVerfHash
    /// @return
    bool VerifCryptoHash() const noexcept;

public:
    /// @brief Return character at current file position without advancing position.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00167}
    /// @threadsafety {no}
    /// @endcode
    ara::core::Result< char8_t >
    /// @brief Returns the character at the current position of the file. The current position is not changed.
    /// @returns A Result containing a character.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    /// PerErrc::kPhysicalStorageFailure    Returned if access to the storage fails.
    /// PerErrc::kValidationFailed  Returned if the validity of stored data cannot be ensured.
    /// PerErrc::kEncryptionFailed  Returned if the decryption of stored data fails.
    /// PerErrc::kNotInitialized    Returned if this method is called after ara::core::Deinitialize.
    /// PerErrc::kIsEof             Returned if the current position is at the end of the file or if the file is empty.
    PeekChar() const noexcept;
    /// @brief Return byte at current file position without advancing position.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00418}
    /// @threadsafety {no}
    /// @endcode
    ara::core::Result< ara::core::Byte >
    /// @brief Returns the byte at the current position of the file.
    /// The current position is not changed.
    /// @returns A Result containing a byte.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    PeekByte() const noexcept;
    /// @brief Return character at current file position and advance position forward.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00168}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Returns the character at the current position of the file, advancing the current position.
    /// In case of an error, the current position is not changed.
    /// @returns A Result containing a character.
    /// In case of an error, it contains any of the errors defined below, or avendor specific error.
    ara::core::Result< char8_t > GetChar() const noexcept;
    /// @brief Return byte at current file position and advance position forward.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00419}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Returns the byte at the current position of the file, advancing the current position.
    /// In case of an error, the current position is not changed.
    /// @returns A Result containing a byte.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< ara::core::Byte > GetByte() const noexcept;
    /// @brief Read all characters from current position to end of file, move cursor to EOF, return a string.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00420}
    /// @threadsafety {no}
    /// @endcode
    ara::core::Result< ara::core::String >
    /// @brief Reads all remaining characters into a String, starting from the current position.
    /// The current position is set to the end of the file.
    /// In case of an error, the current position is not changed.
    /// @returns A Result containing a String.
    /// In case of an error, itcontains any of the errors defined below, or a vendor specific error.
    ReadText() const noexcept;
    /// @brief Read at most n characters from current position, advance cursor, return a string.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00165}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Reads a number of characters into a String, starting from the current position.
    /// The current position is advanced accordingly.
    /// If the end of the file is reached, the number of returned characters can be less than
    /// the requested number, and the current position is set to the end of the file.
    /// In case of an error, the current position is not changed.
    /// @param[in] nLen Number of characters to read.
    /// @returns A Result containing a String.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< ara::core::String > ReadText(uint64_t const nLen) const noexcept;
    /// @brief Read all bytes from current position to end of file, move cursor to EOF, return a vector byte stream.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00421}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Reads all remaining bytes into a Vector of Byte, starting from the current position.
    /// The current position is set to the end of the file.
    /// In case of an error, the current position is not changed.
    /// @returns A Result containing a Vector of Byte.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ReadBinary() const noexcept;
    /// @brief Read at most n bytes from current position, advance cursor, return a vector byte stream.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00422}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Reads a number of bytes into a Vector of Byte, starting from the current position.
    /// The current position is advanced accordingly.
    /// If the end of the file is reached, the number of returned bytes can be less than
    /// the requested number, and the current position is set to the end of the file.
    /// In case of an error, the current position is not changed.
    /// @param[in] nLen Number of bytes to read.
    /// @returns A Result containing a Vector of Byte.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ReadBinary(uint64_t const nLen) const noexcept;
    /// @brief Read a line of string, default line delimiter is '\n'. Read a whole line of data according to given delimiter, advance cursor, return a string.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00119}
    /// @threadsafety {no}
    /// @endcode
    /// @brief
    /// Reads a complete line of characters into a String, advancing the current position accordingly.
    /// The end of the line is demarcated by the delimiter,
    /// or by "\\n" (ASCII 0x10) if that parameter is omitted.
    /// The delimiter itself is not included in the returned String.
    /// If the end of the file is reached, the remaining characters are returned and the current
    /// positionis set to the end of the file.
    /// In case of an error, the current position is not changed.
    /// @param[in] delimiter The character that is used as delimiter.
    /// @returns A Result containing a String.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< ara::core::String > ReadLine(char8_t const delimiter = kChar_Enter) const noexcept;
    /// @brief Return current file size in bytes.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00424}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Returns the current size of a file in bytes.
    /// @returns The current size of the file in bytes.
    uint64_t GetSize() const noexcept;
    /// @brief Return current file cursor: Position offset relative to file start.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00162}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Returns the current position relative to the beginning of the file.
    /// The returned position may be at the end of the file.
    /// @returns The current position in the file in bytes from the beginning of the file.
    uint64_t GetPosition() const noexcept;
    /// @brief Set current file cursor: Position offset relative to file start.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00163}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Sets the current position relative to the beginning of the file.
    /// In case of an error, the current position is not changed.
    /// @param[in] nPosition Current position in the file in bytes from the beginning of the file.
    /// @returns A Result of void.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< void > SetPosition(uint64_t const nPosition) noexcept;
    /// @brief Move current file cursor by parameter, parameter can be positive, negative, or zero, base position is Origin enum.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00164}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Moves the current position in the file relative to the Origin.
    /// In case of an error, the current position is not changed.
    /// @param[in] origin Starting point from which to move ’offset’ bytes.
    /// @param[in] offset Offset in bytes relative to ’origin’.
    /// Can be positive in case of kBeginning and kCurrent and negative in case of kCurrent and kEnd.
    /// In case of kCurrent, an offset of zero will not change the current position.
    /// In case of kEnd, an offset of zero will set the position to the end of the file.
    /// @returns A Result containing the new position in bytes from the beginning of the file.
    /// In case of an error, it contains any of the errors defined below, or a vendor specific error.
    ara::core::Result< uint64_t > MovePosition(Origin const origin, int64_t const offset) noexcept;

public:
    /// @brief Return whether current file cursor has reached end of file.
    /// @code{.isoft}
    /// export_level=/Per
    /// @traceid {SWS_PER_00107}
    /// @threadsafety {no}
    /// @endcode
    /// @brief Checks if the current position is at end of file.
    /// @returns True if the current position is at the end of the file, false otherwise.
    bool IsEof() const noexcept;
    /// @brief Return whether current file exists.
    /// @return
    /// @threadsafety {no}
    bool IsExist() const noexcept;
    /// @brief Check if file is open (non-AutoSar standard function)
    /// @return
    /// @threadsafety {no}
    bool IsOpen() const noexcept;
    /// @brief Return currently operated file name
    /// @return
    /// @threadsafety {no}
    ara::core::StringView GetFileName() const noexcept;

protected:
    /// @brief Read simple data type
    /// @tparam T
    /// @param[in] bPeek
    /// @return
    /// @throws
    template < typename T >
    inline ara::core::Result< T > _ReadDataByMofN(bool const bPeek) const;
    /// @brief Get file content via M/N redundancy strategy
    /// @tparam T
    /// @param[in] nDirtyData
    /// @param[in] pfun
    /// @return
    /// @throws
    template < typename T >
    inline ara::core::Result< T > _ReadFileByMofN(
        uint64_t const nDirtyData,
        std::function< ara::core::Result< T >(isoftkv::PFileOpt const *const pFileOpt) > const pfun) const;
    /// @brief Automatically repair errors using M/N redundancy
    /// @return
    ara::core::Result< void > _AutoRecover() const noexcept;
    /// @brief Get file information via M/N redundancy strategy: Must be basic integer types
    /// @tparam T
    /// @param[in] nDirtyData
    /// @param[in] pfun
    /// @return
    /// @throws
    template < typename T >
    inline ara::core::Result< T > _GetFileInfoByMofN(
        uint64_t const nDirtyData,
        std::function< ara::core::Result< T >(isoftkv::PFileOpt const *const pFileOpt) > const pfun) const;

protected:
    /// @brief Get file operation cursor position
    /// @return
    ara::core::Result< uint64_t > _GetPosition() const noexcept;
    /// @brief Read a segment of string
    /// @param[in] pFileOpt
    /// @param[in] nLen
    /// @return
    ara::core::Result< ara::core::String > _ReadString(isoftkv::PFileOpt const *const pFileOpt,
                                                       uint64_t const nLen) const noexcept;
    /// @brief Read a line of data
    /// @param[in] pFileOpt
    /// @param[in] delimiter
    /// @return
    ara::core::Result< ara::core::String > _ReadLine(isoftkv::PFileOpt const *const pFileOpt,
                                                     char8_t const delimiter = kChar_Enter) const noexcept;
    /// @brief Read a segment of memory
    /// @param[in] pFileOpt
    /// @param[in] nLen
    /// @return
    ara::core::Result< ara::core::Vector< ara::core::Byte > > _ReadBinary(isoftkv::PFileOpt const *const pFileOpt,
                                                                          uint64_t const nLen) const noexcept;
    /// @brief Read data from redundancy data for recovery
    /// @param nIndex Redundancy file index
    /// @param reddCheckData Redundancy error recovery data
    /// @param autoBuff Auto buffer for saving read content
    /// @return Success or failure
    bool _ReddRecoverData_Read(uint8_t const nIndex,
                               isoftkv::PReddCheckData_File const &reddCheckData,
                               isoftkv::PAutoBuff &autoBuff) const noexcept;
    /// @brief Write data to redundancy data for recovery
    /// @param nIndex Redundancy file index
    /// @param reddCheckData Redundancy error recovery data
    /// @param autoBuff Auto buffer for saving read content
    /// @return Success or failure
    bool _ReddRecoverData_Write(uint8_t const nIndex,
                                isoftkv::PReddCheckData_File const &reddCheckData,
                                isoftkv::PAutoBuff const &autoBuff) const noexcept;

protected:
    /// @brief
    using CB_DealFileOpt_Const = std::function< bool(isoftkv::PFileOpt const *const pFileOpt) >;
    /// @brief Whether there is encryption event
    /// @return
    bool _IsHaveCryptoEvent() const noexcept;
    /// @brief Whether file is open
    /// @return
    bool _IsFileOptOpen() const noexcept;
    /// @brief Traverse all files
    /// @param[in] pfun
    /// @return
    int32_t _ForEachFileOpt(CB_DealFileOpt_Const const &pfun) const noexcept;
    /// @brief Get all FileOpt pointers
    /// @return
    ara::core::Vector< isoftkv::PFileOpt * > _GetFileOptVec() const noexcept;
    /// @brief Get main file
    /// @return
    isoftkv::PFileOpt *_GetMainFileOpt() const noexcept;
    /// @brief Get file operation object at specific index
    /// @param[in] i
    /// @return
    isoftkv::PFileOpt *_GetFileOpt(int32_t const i) const noexcept;
    /// @brief Get minimum required success count
    /// @return
    int32_t _GetLeastSuccessCount() const noexcept;
    /// @brief Check if ready to read file
    /// @return
    ara::core::Result< void > _CheckReadReady() const noexcept;
};
//********************************/
}  // namespace per
}  // namespace ara

#endif
