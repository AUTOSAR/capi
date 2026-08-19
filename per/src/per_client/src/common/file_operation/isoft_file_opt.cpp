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
/// @file       isoft_file_opt.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    File operation class used by KV storage
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operation
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-18 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/common/isoft_file_opt.h"

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_stat.h>
#include <nai/os/nai_system.h>
#include <nai/runtime/nai_errno.h>

#include "ara/per/internal/common/isoft_common_api.h"

namespace {
/// @brief Unit for converting nanoseconds to seconds
constexpr uint64_t kTimeNanoSecond{1000000000U};
/// @brief
/// @param tmNai
/// @return
inline uint64_t G_TansNaiTime(timespec const& tmNai) noexcept
{
    return static_cast< uint64_t >(tmNai.tv_sec) * kTimeNanoSecond + static_cast< uint64_t >(tmNai.tv_nsec);
}
}  // namespace
//********************************/
namespace ara {
namespace per {
namespace isoftkv {
/// @brief
class POptCopyFile final
{
private:
    /// @brief Use nai_mmap to allocate page-aligned heap memory
    PNaiMapBuff naiMapBuff_;
    /// @brief File operation handle
    nai_fd_t fdSrc_;
    /// @brief File operation handle
    nai_fd_t fdDst_;
    /// @brief Whether to automatically close the src file
    bool bAutoCloseSrc_;

public:
    /// @brief
    /// @param stFileNameSrc
    /// @param stFileNameNew
    /// @param nBuffLen
    POptCopyFile(ara::core::StringView const& stFileNameSrc,
                 ara::core::StringView const& stFileNameNew,
                 uint32_t const nBuffLen = 0U) noexcept
        : naiMapBuff_{nBuffLen}
        , fdSrc_{nai_file_open(stFileNameSrc.data(),
                               static_cast< int32_t >(NAI_O_CREAT | NAI_O_RDONLY | NAI_O_DIRECT),
                               static_cast< uint32_t >(kDefFileMode))}
        , fdDst_{nai_file_open(stFileNameNew.data(),
                               static_cast< int32_t >(NAI_O_CREAT | NAI_O_WRONLY | NAI_O_DIRECT),
                               static_cast< uint32_t >(kDefFileMode))}
        , bAutoCloseSrc_{true}
    {
    }
    /// @brief
    /// @param fd
    /// @param stFileNameNew
    /// @param nBuffLen
    POptCopyFile(nai_fd_t const fd, ara::core::StringView const& stFileNameNew, uint32_t const nBuffLen = 0U) noexcept
        : naiMapBuff_{nBuffLen}
        , fdSrc_{fd}
        , fdDst_{nai_file_open(stFileNameNew.data(),
                               static_cast< int32_t >(NAI_O_CREAT | NAI_O_WRONLY | NAI_O_DIRECT),
                               static_cast< uint32_t >(kDefFileMode))}
        , bAutoCloseSrc_{false}
    {
    }
    /// @brief
    ~POptCopyFile() noexcept
    {
        if (bAutoCloseSrc_) {
            std::ignore = nai_file_close(fdSrc_);
        }
        std::ignore = nai_file_close(fdDst_);
    }
    /// @brief
    /// @param a
    POptCopyFile(POptCopyFile const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptCopyFile& operator=(POptCopyFile const& a) = delete;
    /// @brief
    /// @param a
    POptCopyFile(POptCopyFile const&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptCopyFile& operator=(POptCopyFile const&& a) = delete;
    /// @brief
    /// @return
    bool IsOK() const noexcept
    {
        if (fdSrc_ == kNaiFdInvalid) {
            return false;
        }
        if (fdDst_ == kNaiFdInvalid) {
            return false;
        }
        return true;
    }
    /// @brief Add copy progress indication interface if needed in the future
    /// @param pfun
    /// @return
    bool DoCopy(std::function< void(uint64_t nTotalLen, uint64_t nCopyLen) > const& pfun = nullptr) const noexcept
    {
        uint64_t const nTotalLen{static_cast< uint64_t >(nai_file_get_size(fdSrc_))};
        uint64_t nCopyLen{0U};
        while (true) {
            if (nCopyLen >= nTotalLen) {
                break;
            }
            intptr_t const nReadLen{nai_file_pread(fdSrc_, naiMapBuff_.GetBuff(),
                                                   static_cast< std::size_t >(naiMapBuff_.GetLen()),
                                                   static_cast< nai_off64_t >(nCopyLen))};
            if (nReadLen < 0) {
                return false;
            }
            if (nReadLen == 0) {
                break;
            }
            intptr_t const nWriteLen{nai_file_pwrite(fdDst_, naiMapBuff_.GetBuff(),
                                                     static_cast< std::size_t >(naiMapBuff_.GetLen()),
                                                     static_cast< nai_off64_t >(nCopyLen))};
            if (nWriteLen < 0) {
                return false;
            }
            nCopyLen += static_cast< uint64_t >(nReadLen);
            if (nullptr != pfun) {
                pfun(nTotalLen, nCopyLen);
            }
        }
        std::ignore = nai_file_set_size(fdDst_, static_cast< nai_off64_t >(nTotalLen));
        return true;
    }
};
//********************************/
/// @brief
/// @return
/// @note To adapt to the latest storage media, the default is set to 4K; later refactoring will consider dynamic adaptation
uint32_t PFileOpt::GetNaiSectorSize() noexcept { return kInt32_4096U; }
/// @brief
/// @return
uint32_t PFileOpt::ReadOnly() noexcept { return static_cast< uint32_t >(NAI_O_RDONLY); }
/// @brief
/// @return
uint32_t PFileOpt::WriteOnly() noexcept { return static_cast< uint32_t >(NAI_O_CREAT | NAI_O_WRONLY); }
/// @brief
/// @return
uint32_t PFileOpt::ReadWrite() noexcept { return static_cast< uint32_t >(NAI_O_CREAT | NAI_O_RDWR); }
//***************/
/// @brief Get Nai error code
/// @return
int32_t PFileOpt::GetLastError() noexcept { return static_cast< int32_t >(nai_errno); }
/// @brief Check if a file exists
/// @param stFileName
/// @return
bool PFileOpt::IsFileExist(ara::core::StringView const& stFileName) noexcept
{
    nai_fd_t const fd{nai_file_open(stFileName.data(), NAI_O_RDONLY, static_cast< uint32_t >(kDefFileMode))};
    if (fd != kNaiFdInvalid) {
        std::ignore = nai_file_close(fd);
        return true;
    }
    return false;
}
/// @brief Delete a file
/// @param stFileName
/// @return
bool PFileOpt::DelFile(ara::core::StringView const& stFileName) noexcept
{
    intptr_t const nNaiReturn{nai_file_unlink(stFileName.data())};
    if ((nNaiReturn < 0) && (ENOENT != GetLastError())) {
        return false;
    }
    return true;
}
/// @brief Rename
/// @param stFileNameOld
/// @param stFileNameNew
/// @return
bool PFileOpt::RenameFile(ara::core::StringView const& stFileNameOld,
                          ara::core::StringView const& stFileNameNew) noexcept
{
    if (nai_path_rename(stFileNameOld.data(), stFileNameNew.data()) >= 0) {
        return true;
    }
    return false;
}
/// @brief Copy a file
/// @param stFileNameSrc
/// @param stFileNameDst
/// @return
bool PFileOpt::CopyFile(ara::core::StringView const& stFileNameSrc, ara::core::StringView const& stFileNameDst) noexcept
{
    // First copy the file to a temporary location
    ara::core::String stTempFile{stFileNameDst.data()};
    stTempFile += ".tmp";
    POptCopyFile const optCopyFile{stFileNameSrc, T_StringView(stTempFile)};
    if (false == optCopyFile.IsOK()) {
        return false;
    }
    if (false == optCopyFile.DoCopy()) {
        return false;
    }
    // Rename only after complete success
    if (false == PFileOpt::DelFile(stFileNameDst)) {
        return false;
    }
    if (nai_path_rename(stTempFile.data(), stFileNameDst.data()) < 0) {
        return false;
    }
    return true;
}
/// @brief Copy a file
/// @param stFileNameSrc
/// @param stFileNameDst
/// @return
bool PFileOpt::CopyFile(ara::core::String const& stFileNameSrc, ara::core::String const& stFileNameDst) noexcept
{
    return PFileOpt::CopyFile(T_StringView(stFileNameSrc), T_StringView(stFileNameDst));
}
/// @brief Set a new working path
/// @param stPath
/// @return
int32_t PFileOpt::SetWorkPath(ara::core::StringView const& stPath) noexcept { return nai_path_set_cwd(stPath.data()); }
/// @brief Get the current working directory
/// @return
ara::core::String PFileOpt::GetWorkPath() noexcept
{
    uint8_t chBuff[kInt_1024U]{0U};
    if (nai_path_get_cwd(static_cast< char8_t* >(static_cast< void* >(chBuff)), static_cast< std::size_t >(kInt_1024U))
        <= 0) {
        return ara::core::String{""};
    }
    if (chBuff[0] == 0U) {
        return ara::core::String{""};
    }
    chBuff[kInt_1024U - 1U] = 0U;
    return ara::core::String{static_cast< char8_t* >(static_cast< void* >(chBuff))};
}
/// @brief Get file information: creation time
/// @param stFileName
/// @return
uint64_t PFileOpt::GetFileCreateTime(ara::core::StringView const& stFileName) noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_stat(stFileName.data(), &fileStat, NAI_STAT_PERM) < 0) {
        return 0U;
    }
    return G_TansNaiTime(fileStat.st_ctim);
}
/// @brief Get file information: modification time
/// @param stFileName
/// @return
uint64_t PFileOpt::GetFileModifyTime(ara::core::StringView const& stFileName) noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_stat(stFileName.data(), &fileStat, NAI_STAT_PERM) < 0) {
        return 0U;
    }
    return G_TansNaiTime(fileStat.st_mtim);
}
/// @brief Get file information: creation time
/// @param stFileName
/// @return
uint64_t PFileOpt::GetFileAccessTime(ara::core::StringView const& stFileName) noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_stat(stFileName.data(), &fileStat, NAI_STAT_PERM) < 0) {
        return static_cast< uint64_t >(0U);
    }
    return G_TansNaiTime(fileStat.st_atim);
}
/// @brief Combine a file name with a path
/// @param stPath
/// @param stName
/// @return
ara::core::String PFileOpt::MakeFileName(ara::core::StringView const& stPath,
                                         ara::core::StringView const& stName) noexcept
{
    ara::core::String stFileName;
    stFileName.reserve(static_cast< ara::core::String::size_type >(kInt_0x100));
    if (false == stPath.empty()) {
        stFileName += stPath;
        if (stPath.back() != nai_path_sep) {
            stFileName += nai_path_sep;
        }
        stFileName += stName.data();
    } else {
        stFileName = stName;
    }
    return stFileName;
}
/// @brief Combine a file name with a path
/// @param stPath
/// @param stName
/// @return
ara::core::String PFileOpt::MakeFileName(ara::core::String const& stPath, ara::core::String const& stName) noexcept
{
    return MakeFileName(T_StringView(stPath), T_StringView(stName));
}
/// @brief Create a directory: 2021-11-11 Needs to be extended to recursively create directories
/// @param stPath
/// @param bLoop
/// @param nMode
/// @return
bool PFileOpt::MakeDir(ara::core::StringView const& stPath, bool const bLoop, int32_t const nMode) noexcept
{
    // Create directory with 0755 permissions // 1 means automatically create parent directory
    int32_t recursive{0};
    if (bLoop) {
        recursive = 1;
    }
    return nai_dir_create(stPath.data(), nMode, recursive) >= 0;
}
/// @brief Create a directory: 2021-11-11 Needs to be extended to recursively create directories
/// @param stPath
/// @param bLoop
/// @param nMode
/// @return
bool PFileOpt::MakeDir(ara::core::String const& stPath, bool const bLoop, int32_t const nMode) noexcept
{
    // Create directory with 0755 permissions // 1 means automatically create parent directory
    int32_t recursive{0};
    if (bLoop) {
        recursive = 1;
    }
    return nai_dir_create(stPath.data(), nMode, recursive) >= 0;
}
/// @brief Delete directory nai_dir_remove
/// @param stPath
/// @return
bool PFileOpt::RemoveDir(ara::core::StringView const& stPath) noexcept { return nai_dir_remove(stPath.data()) >= 0; }
//********************************/
/// @brief
/// @param nFileFlags
PFileOpt::PFileOpt(uint32_t const nFileFlags) noexcept : nFileFlags_{nFileFlags} {}
/// @brief Destructor
PFileOpt::~PFileOpt() noexcept { std::ignore = CloseFile(); }
/// @brief Read the first page of an HPKS file (N bytes from the beginning of the file)
/// @param pBuff
/// @param nBuffLen
/// @return
int32_t PFileOpt::ReadData(uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept
{
    return _ReadData(pBuff, nBuffLen);
}
/// @brief Write data
/// @param pVoidData
/// @param nDataLen
/// @return
int32_t PFileOpt::WriteData(const void* const pVoidData, uint32_t const nDataLen) const noexcept
{
    return _WriteData(pVoidData, nDataLen);
}
//***************/
/// @brief Write a string
/// @param stData
/// @return
int32_t PFileOpt::WriteString(ara::core::StringView const& stData) const noexcept
{
    if (stData.empty()) {
        return 0;
    }
    return WriteData(stData.data(), static_cast< uint32_t >(stData.size()));
}
/// @brief Prepare all necessary steps before reading and writing
/// @param stFileName
/// @param nPageLen
/// @return
bool PFileOpt::DoPrepareWork(ara::core::StringView const& stFileName, uint16_t const nPageLen) noexcept
{
    std::ignore = nPageLen;
    ara::core::String stTmpFileName;
    stTmpFileName = T_String(stFileName);
    if ((stFileName_ == stTmpFileName) && IsOpen()) {
        return true;
    }
    // First call unlock: regardless of whether it is locked, call it once
    if (false == UnlockFile()) {
    }
    // Then close the file
    if (false == CloseFile()) {
    }
    stFileName_ = stFileName;
    return OpenFile();
}
/// @brief Open an existing HPKS file and read the file header
/// @return
bool PFileOpt::OpenFile() noexcept
{
    if (stFileName_.empty()) {
        return false;
    }

    fd_ = nai_file_open(stFileName_.c_str(), static_cast< int32_t >(nFileFlags_), nFileMode_);
    if (fd_ == kNaiFdInvalid) {
        return false;
    }
    return true;
}
/// @brief Check if the file is open
/// @return
bool PFileOpt::IsOpen() const noexcept { return fd_ != kNaiFdInvalid; }
/// @brief Check if the cursor is at the end of the file
/// @return
bool PFileOpt::IsEof() const noexcept
{
    int64_t const nSeekPos{nai_file_tell(fd_)};
    if (nSeekPos < 0) {
        return true;
    }
    if (nSeekPos >= GetFileSize()) {
        return true;
    }
    return false;
}
/// @brief
/// @return
int64_t PFileOpt::GetFileSize() const noexcept { return nai_file_get_size(fd_); }
/// @brief Close the file
/// @return
bool PFileOpt::CloseFile() noexcept
{
    bool bReturn{true};
    if (IsOpen()) {
        if (nai_file_close(fd_) < 0) {
            bReturn = false;
        }
        fd_ = kNaiFdInvalid;
    }
    return bReturn;
}
/// @brief Reset the file size
/// @param nFileLen
/// @return
bool PFileOpt::ResizeFile(int64_t const nFileLen) const noexcept
{
    if (nai_file_set_size(fd_, nFileLen) >= 0) {
        return true;
    }
    return false;
}
/// @brief
/// @param eMoveType
/// @param nOffset
/// @return
bool PFileOpt::SeekPos(EFileSeekType const eMoveType, int64_t const nOffset) const noexcept
{
    if (nai_file_seek(fd_, nOffset, static_cast< int32_t >(eMoveType)) >= 0) {
        return true;
    }
    return false;
}
/// @brief Get the current cursor position
/// @return
int64_t PFileOpt::GetPos() const noexcept { return nai_file_tell(fd_); }
/// @brief Lock
/// @param eLockType
/// @return
bool PFileOpt::LockFile(int32_t const eLockType) const noexcept
{
    if (nai_file_lock(fd_, eLockType) >= 0) {
        return true;
    }
    return false;
}
/// @brief Unlock
/// @return
bool PFileOpt::UnlockFile() const noexcept
{
    if (nai_file_unlock(fd_) >= 0) {
        return true;
    }
    return false;
}
/// @brief Sync to disk
/// @return
bool PFileOpt::Flush() const noexcept
{
    if (nai_file_flush(fd_) >= 0) {
        return true;
    }
    return false;
}
/// @brief Delete the current file
/// @return
bool PFileOpt::DelFile() noexcept
{
    std::ignore = CloseFile();
    return DelFile(T_StringView(stFileName_));
}
/// @brief Make a copy of the current file
/// @param stFileNameDst
/// @param nBuffLen
/// @return
bool PFileOpt::CopyFile(ara::core::StringView const& stFileNameDst, uint32_t const nBuffLen) const noexcept
{
    // First copy the file to a temporary location
    ara::core::String stTempFile{stFileNameDst.data()};
    stTempFile += ".tmp";
    POptCopyFile const optCopyFile{fd_, T_StringView(stTempFile), nBuffLen};
    if (false == optCopyFile.IsOK()) {
        return false;
    }
    if (false == optCopyFile.DoCopy()) {
        return false;
    }
    // Rename only after complete success
    if (false == PFileOpt::DelFile(stFileNameDst)) {
        return false;
    }
    if (nai_path_rename(stTempFile.data(), stFileNameDst.data()) >= 0) {
        return true;
    }
    return false;
}
/// @brief Check the current cursor position
/// @return
int64_t PFileOpt::FileTell() const noexcept { return nai_file_seek(fd_, 0, 1); }
/// @brief
/// @param nNewFd
/// @return
bool PFileOpt::FileDup(nai_fd_t& nNewFd) const noexcept
{
    std::ignore = nNewFd;
    nNewFd      = nai_file_dup(fd_, 0);
    return nNewFd != kNaiFdInvalid;
}
/// @brief
/// @param ePollType
/// @return
bool PFileOpt::FilePoll(EFilePollType const ePollType) const noexcept
{
    uint32_t const nMSec{static_cast< uint32_t >(-1)};
    if (nai_file_poll(fd_, static_cast< int32_t >(ePollType), nMSec) >= 0) {
        return true;
    }
    return false;
}
/// @brief Whether reading is allowed
/// @return
bool PFileOpt::IsEnableRead() const noexcept
{
    if (true == T_IsAndData(nFileFlags_, static_cast< uint32_t >(NAI_O_RDWR))) {
        return true;
    }
    if (true == T_IsAndData(nFileFlags_, static_cast< uint32_t >(NAI_O_RDONLY))) {
        return true;
    }
    return false;
}
/// @brief Whether writing is allowed
/// @return
bool PFileOpt::IsEnableWrite() const noexcept
{
    if (true == T_IsAndData(nFileFlags_, static_cast< uint32_t >(NAI_O_RDWR))) {
        return true;
    }
    if (true == T_IsAndData(nFileFlags_, static_cast< uint32_t >(NAI_O_WRONLY))) {
        return true;
    }
    return false;
}
/// @brief Get file information
/// @return
uint64_t PFileOpt::GetCreateTime() const noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_file_stat(fd_, &fileStat, NAI_STAT_PERM) < 0) {
        return static_cast< uint64_t >(0U);
    }
    return G_TansNaiTime(fileStat.st_ctim);
}
/// @brief Get file information
/// @return
uint64_t PFileOpt::GetModifyTime() const noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_file_stat(fd_, &fileStat, NAI_STAT_PERM) < 0) {
        return static_cast< uint64_t >(0U);
    }
    return G_TansNaiTime(fileStat.st_mtim);
}
/// @brief Get file information
/// @return
uint64_t PFileOpt::GetAccessTime() const noexcept
{
    nai_stat_s fileStat;  // NOLINT
    if (nai_file_stat(fd_, &fileStat, NAI_STAT_PERM) < 0) {
        return static_cast< uint64_t >(0U);
    }
    return G_TansNaiTime(fileStat.st_atim);
}
//********************************/
/// @brief Read data
/// @param pBuff
/// @param nBuffLen
/// @return
int32_t PFileOpt::_ReadData(uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept
{
    if ((nullptr == pBuff) || (nBuffLen <= 0U)) {
        return 0;
    }
    // nReadLen may be smaller than nBuffLen
    return static_cast< int32_t >(nai_file_read(fd_, pBuff, static_cast< std::size_t >(nBuffLen)));
}
/// @brief Write data
/// @param pBData
/// @param nDataLen
/// @return
int32_t PFileOpt::_WriteData(const void* const pBData, uint32_t const nDataLen) const noexcept
{
    if ((nullptr == pBData) || (nDataLen <= 0U)) {
        return 0;
    }
    // Must not have the NAI_O_DIRECT option in nFileFlags_ to succeed
    return static_cast< int32_t >(nai_file_write(fd_, pBData, static_cast< std::size_t >(nDataLen)));
}
/// @brief Read data
/// @param pBuff
/// @param nBuffLen
/// @param nOffset
/// @return
int32_t PFileOpt::_ReadData(uint8_t* const pBuff, uint32_t const nBuffLen, int64_t const nOffset) const noexcept
{
    if ((nullptr == pBuff) || (nBuffLen <= 0U)) {
        return 0;
    }
    return static_cast< int32_t >(
        nai_file_pread(fd_, pBuff, static_cast< std::size_t >(nBuffLen), static_cast< nai_off64_t >(nOffset)));
}
/// @brief Write data
/// @param pBData
/// @param nDataLen
/// @param nOffset
/// @return
int32_t PFileOpt::_WriteData(uint8_t const* const pBData, uint32_t const nDataLen, int64_t const nOffset) const noexcept
{
    if ((nullptr == pBData) || (nDataLen <= 0U)) {
        return 0;
    }
    return static_cast< int32_t >(
        nai_file_pwrite(fd_, pBData, static_cast< std::size_t >(nDataLen), static_cast< nai_off64_t >(nOffset)));
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
