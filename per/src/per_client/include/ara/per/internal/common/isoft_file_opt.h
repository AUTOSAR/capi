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
/// @file       isoft_file_opt.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    File operation class used by KV storage
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-18  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operations
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_FILE_OPT_H_
#define ARA_PER_COMMON_PH_FILE_OPT_H_

#include <nai/os/nai_file.h>

#include <vector>

#include "ara/per/internal/common/isoft_data_type.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Enumeration of file cursor movement origin
/// @code{.isoft}
/// @unit_name=EFileSeekType
/// @endcode
enum class EFileSeekType : int32_t
{
    kSeekSet = 0,  // File beginning SEEK_SET 0
    kSeekCur,      // Current position SEEK_CUR 1
    kSeekEnd,      // File end SEEK_END 2
};
/// @brief Write lock: Exclusive lock
constexpr int32_t kFileLockWrite = 0;
/// @brief Read lock: Shared lock    // NAI_FLOCK_READ          0x01
constexpr int32_t kFileLockRead = 1;
/// @brief Try lock, return 0 if locked, check error code if not NAI_FLOCK_TRY
constexpr int32_t kFileLockTry = 2;
/// @brief Event type
/// @code{.isoft}
/// @unit_name=EFilePollType
/// @interface_level=unit
/// @endcode
enum class EFilePollType : int32_t
{
    kPollRead      = 1,  // NAI_POLL_READ           0x01
    kPollWrite     = 2,  // NAI_POLL_WRITE          0x02
    kPollReadWrite = 3,  // NAI_POLL_READWRITE      0x03
};
/// @brief NAI_FD_INVALID
constexpr nai_fd_t kNaiFdInvalid{-1};
/// @brief File read/write permission under Linux: Default file open mode 0666 (decimal 438)
constexpr uint32_t kDefFileMode{438U};
/// @brief Directory read/write permission under Linux: Default mode 0755 (decimal 493)
constexpr int32_t kDefDirMode{493};
//********************************/
/// @brief File operation base class
/// @code{.isoft}
/// @unit_name=PFileOpt
/// @endcode
class PFileOpt
{
public:
    /// @brief Get sector size, different from storage medium sector here. Fixed at 4096 bytes currently.
    /// @return Sector size
    static uint32_t GetNaiSectorSize() noexcept;
    /// @brief Get file read-only flag, does not represent file read/write attribute
    /// @return
    static uint32_t ReadOnly() noexcept;
    /// @brief Get file write-only flag, does not represent file read/write attribute
    /// @return
    static uint32_t WriteOnly() noexcept;
    /// @brief Get file read/write flag, does not represent file read/write attribute
    /// @return
    static uint32_t ReadWrite() noexcept;
    /// @brief Get Nai error code
    /// @return
    static int32_t GetLastError() noexcept;
    /// @brief Check if file exists
    /// @param stFileName
    /// @return
    static bool IsFileExist(ara::core::StringView const &stFileName) noexcept;
    /// @brief Delete file
    /// @param stFileName
    /// @return
    static bool DelFile(ara::core::StringView const &stFileName) noexcept;
    /// @brief Rename
    /// @param stFileNameOld
    /// @param stFileNameNew
    /// @return
    static bool RenameFile(ara::core::StringView const &stFileNameOld,
                           ara::core::StringView const &stFileNameNew) noexcept;
    /// @brief Copy file
    /// @param stFileNameSrc
    /// @param stFileNameDst
    /// @return
    static bool CopyFile(ara::core::StringView const &stFileNameSrc,
                         ara::core::StringView const &stFileNameDst) noexcept;
    /// @brief Copy file
    /// @param stFileNameSrc
    /// @param stFileNameDst
    /// @return
    static bool CopyFile(ara::core::String const &stFileNameSrc, ara::core::String const &stFileNameDst) noexcept;
    /// @brief Set new working path
    /// @param stPath
    /// @return
    static int32_t SetWorkPath(ara::core::StringView const &stPath) noexcept;
    /// @brief Get current working directory
    /// @return
    static ara::core::String GetWorkPath() noexcept;
    /// @brief Get file information: Creation time
    /// @param stFileName
    /// @return
    static uint64_t GetFileCreateTime(ara::core::StringView const &stFileName) noexcept;
    /// @brief Get file information: Modification time
    /// @param stFileName
    /// @return
    static uint64_t GetFileModifyTime(ara::core::StringView const &stFileName) noexcept;
    /// @brief Get file information: Access time
    /// @param stFileName
    /// @return
    static uint64_t GetFileAccessTime(ara::core::StringView const &stFileName) noexcept;
    /// @brief Combine path-containing file name
    /// @param stPath
    /// @param stName
    /// @return
    static ara::core::String MakeFileName(ara::core::StringView const &stPath,
                                          ara::core::StringView const &stName) noexcept;
    /// @brief Combine path-containing file name
    /// @param stPath
    /// @param stName
    /// @return
    static ara::core::String MakeFileName(ara::core::String const &stPath, ara::core::String const &stName) noexcept;
    /// @brief Create directory: Whether to recursively create parent directories
    /// @param stPath
    /// @param bLoop
    /// @param nMode
    /// @return
    static bool MakeDir(ara::core::StringView const &stPath,
                        bool const bLoop,
                        int32_t const nMode = kDefDirMode) noexcept;
    /// @brief Create directory: Whether to recursively create parent directories
    /// @param stPath
    /// @param bLoop
    /// @param nMode
    /// @return
    static bool MakeDir(ara::core::String const &stPath, bool const bLoop, int32_t const nMode = kDefDirMode) noexcept;
    /// @brief Delete directory
    /// @param stPath
    /// @return
    static bool RemoveDir(ara::core::StringView const &stPath) noexcept;

private:
    /// @brief File name
    ara::core::String stFileName_;
    /// @brief File descriptor
    nai_fd_t fd_{kNaiFdInvalid};
    /// @brief File read/write permission under Linux
    uint32_t nFileMode_{kDefFileMode};
    /// @brief File open parameters
    uint32_t nFileFlags_;

public:
    /// @brief Parameterized constructor
    /// @param nFileFlags File flags
    explicit PFileOpt(uint32_t const nFileFlags) noexcept;
    /// @brief
    PFileOpt() noexcept = delete;
    /// @brief
    /// @param  a
    PFileOpt(PFileOpt const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PFileOpt &operator=(PFileOpt const &a) = delete;
    /// @brief
    /// @param  a
    PFileOpt(PFileOpt &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PFileOpt &operator=(PFileOpt &&a) = delete;
    /// @brief Destructor
    virtual ~PFileOpt() noexcept;
    /// @brief Read data
    /// @param pBuff
    /// @param nBuffLen
    /// @return
    virtual int32_t ReadData(uint8_t *const pBuff, uint32_t const nBuffLen) const noexcept;
    /// @brief Write data
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    virtual int32_t WriteData(const void *const pVoidData, uint32_t const nDataLen) const noexcept;

public:
    /// @brief Write string
    /// @param stData
    /// @return
    int32_t WriteString(ara::core::StringView const &stData) const noexcept;
    /// @brief Get file name
    /// @return
    inline ara::core::String const &GetFileName() const noexcept { return stFileName_; }
    /// @brief Get file read/write handle
    /// @return
    inline nai_fd_t GetFd() const noexcept { return fd_; }
    /// @brief Set file open flags
    /// @param nFileFlags
    inline void SetFileFlags(uint32_t const nFileFlags) noexcept { nFileFlags_ = nFileFlags; }
    /// @brief Get file open flags
    /// @return
    inline uint32_t GetFileFlags() const noexcept { return nFileFlags_; }
    /// @brief Prepare everything before reading/writing
    /// @param stFileName
    /// @param nPageLen
    /// @return
    virtual bool DoPrepareWork(ara::core::StringView const &stFileName, uint16_t const nPageLen = 0U) noexcept;
    /// @brief Open existing HPKS file and read file header
    /// @return
    bool OpenFile() noexcept;
    /// @brief Check if file is open
    /// @return
    virtual bool IsOpen() const noexcept;
    /// @brief Check if cursor is at end of file
    /// @return
    bool IsEof() const noexcept;
    /// @brief Get file length
    /// @return
    virtual int64_t GetFileSize() const noexcept;
    /// @brief Close file
    /// @return
    bool CloseFile() noexcept;
    /// @brief Reset file size
    /// @param nFileLen
    /// @return
    bool ResizeFile(int64_t const nFileLen) const noexcept;
    /// @brief Move file operation cursor
    /// @param eMoveType
    /// @param nOffset
    /// @return
    bool SeekPos(EFileSeekType const eMoveType, int64_t const nOffset) const noexcept;
    /// @brief Get current cursor position
    /// @return
    int64_t GetPos() const noexcept;
    /// @brief Lock
    /// @param eLockType
    /// @return
    bool LockFile(int32_t const eLockType) const noexcept;
    /// @brief Unlock
    /// @return
    bool UnlockFile() const noexcept;
    /// @brief Sync to disk
    /// @return
    bool Flush() const noexcept;
    /// @brief Delete current file
    /// @return
    bool DelFile() noexcept;
    /// @brief Copy current file to another
    /// @param stFileNameDst
    /// @param nBuffLen
    /// @return
    bool CopyFile(ara::core::StringView const &stFileNameDst, uint32_t const nBuffLen = 0U) const noexcept;
    /// @brief View current cursor position
    /// @return
    int64_t FileTell() const noexcept;
    /// @brief Duplicate file descriptor
    /// @param nNewFd
    /// @return
    bool FileDup(nai_fd_t &nNewFd) const noexcept;
    /// @brief Wait for file descriptor to be ready for I/O
    /// @param ePollType
    /// @return
    bool FilePoll(EFilePollType const ePollType) const noexcept;
    /// @brief Allow read
    /// @return
    bool IsEnableRead() const noexcept;
    /// @brief Allow write
    /// @return
    bool IsEnableWrite() const noexcept;
    /// @brief Get file information: Creation time
    /// @return
    uint64_t GetCreateTime() const noexcept;
    /// @brief Get file information: Modification time
    /// @return
    uint64_t GetModifyTime() const noexcept;
    /// @brief Get file information: Access time
    /// @return
    uint64_t GetAccessTime() const noexcept;

protected:
    /// @brief Read data
    /// @param pBuff
    /// @param nBuffLen
    /// @return Actual number of bytes read
    int32_t _ReadData(uint8_t *const pBuff, uint32_t const nBuffLen) const noexcept;
    /// @brief Write data
    /// @param pBData
    /// @param nDataLen
    /// @return
    int32_t _WriteData(const void *const pBData, uint32_t const nDataLen) const noexcept;
    /// @brief Read data
    /// @param pBuff
    /// @param nBuffLen
    /// @param nOffset
    /// @return Actual number of bytes read
    int32_t _ReadData(uint8_t *const pBuff, uint32_t const nBuffLen, int64_t const nOffset) const noexcept;
    /// @brief Write data
    /// @param pBData
    /// @param nDataLen
    /// @param nOffset
    /// @return
    int32_t _WriteData(uint8_t const *const pBData, uint32_t const nDataLen, int64_t const nOffset) const noexcept;

private:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
