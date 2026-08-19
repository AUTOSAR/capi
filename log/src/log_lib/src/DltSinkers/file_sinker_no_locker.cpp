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
/// @file       file_sinker_no_locker.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltSinkers
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00007
/// @unit_name = file_sinker_no_locker
/// @unit_description=Dlt lib's implemented log sinker for writing to files
/// @endcode
///
/// ================================================================

#include "file_sinker_no_locker.h"

#include <isoft/ara_fsh/platform.h>
#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>
#include <sys/time.h>

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <list>
#include <memory>
#include <string>
#include <tuple>

#include "Utils/src/private_log.h"

#ifdef __qnx__
    #include <sys/neutrino.h>
    #include <sys/types.h>
    #include <unistd.h>
#else
    #include <fcntl.h>     // File control options (e.g., O_CREAT, O_RDWR)
    #include <sys/mman.h>  // Shared memory related functions
    #include <unistd.h>    // File descriptor operations (e.g., ftruncate)
#endif

namespace ara {
namespace log {
namespace internal {
/// @brief Log save operation return value enumeration

/// @brief FileSinkerNoLocker
FileSinkerNoLocker::~FileSinkerNoLocker() noexcept
{
    if (this->inited_) {
        // First stop the BufferModeManager
        if (bufferModeManager_) {
            bufferModeManager_->Stop();
        }
        std::ignore = FlushBuffer();
        this->CloseFile();
    }
}
/// @brief
/// @param[in]  ecuId
/// @param[in]  filename
/// @param[in]  singleFilesize
/// @param[in]  maxfileCount
/// @param[in]  buffersize
/// @return
bool FileSinkerNoLocker::Init(std::string const& ecuId,
                              std::string const& filename,
                              std::size_t const& singleFilesize,
                              std::size_t const& fileCount,
                              std::size_t const& buffersize) noexcept
{
    ecuId_          = ecuId;
    filename_       = filename;
    singleFileSize_ = singleFilesize;
    loopFileCount_  = fileCount;
    if (loopFileCount_ == 0) {
        loopFileCount_ = 1;
    }

    // When loopFileCount_ == 1, it means only one file is kept
    bufferSize_ = buffersize;
    /// 1000024 about 1MB, byte-aligned.
    std::int32_t const maxBuffer{100000024};
    if (bufferSize_ > 0 && bufferSize_ < maxBuffer) {
        logBuffer_ = std::make_shared< LogBuffer >(bufferSize_);

    } else {
        logBuffer_ = std::make_shared< LogBuffer >(0);
    }
    LOGVERBOSE("filesinker::") << __func__ << ",bufferSize_: " << bufferSize_;
    LOGVERBOSE("singleFileSize:") << singleFileSize_;
    LOGVERBOSE("loopFileCount_:") << loopFileCount_;

    _seprateDirCleanname();
    rolloverHelper_.UpdateConfig({logDir_, cleanName_, suffix_, compress_, loopFileCount_});
    nai_dir_create(this->logDir_.c_str(), 0755, 1);
    if (OpenFile() == false) {
        return false;
    }

    inited_ = true;

    // Initialize the BufferModeManager
    auto flushCallback = [this]() -> bool { return this->FlushBuffer(); };
    bufferModeManager_ = std::make_unique< BufferModeManager >(flushCallback);
    bufferModeManager_->SetTimeout(3000);  // Default timeout 3 seconds
    if (!bufferModeManager_->Start()) {
        LOGERROR("filesinker::") << __func__ << ", Failed to start BufferModeManager";
        // Failure to start does not prevent initialization, just logs an error
    }

    return true;
}
int32_t FileSinkerNoLocker::SaveLog(std::uint8_t* buffer1,
                                    std::size_t const& len1,
                                    std::uint8_t* buffer2,
                                    std::size_t const& len2) noexcept
{
    std::ignore = buffer1;
    std::ignore = len1;
    std::ignore = buffer2;
    std::ignore = len2;
    return 0;
}
int32_t FileSinkerNoLocker::SaveLogWithTimeStamp(std::uint8_t* buffer1,
                                                 std::size_t const& len1,
                                                 std::uint8_t* buffer2,
                                                 std::size_t const& len2,
                                                 DltTimeStamp const& dltTimestamp) noexcept
{
    if ((len1 == 0) && (len2 == 0)) {
        return static_cast< int32_t >(SaveLogOperationResult::INVALID_PARAMETERS);
    }
    this->CheckFileOverSize();
    return this->SaveLogWithTimeStampNocheck(buffer1, len1, buffer2, len2, dltTimestamp);
}

int32_t FileSinkerNoLocker::SaveLogWithTimeStampNocheck(std::uint8_t* buffer1,
                                                        std::size_t const& len1,
                                                        std::uint8_t* buffer2,
                                                        std::size_t const& len2,
                                                        DltTimeStamp const& dltTimestamp) noexcept
{
    LOGVERBOSE(__func__) << len2;
    if (inited_ == false) {
        return static_cast< int32_t >(SaveLogOperationResult::NOT_INITIALIZED);
    }
    if ((len1 == 0 && len2 == 0) || (buffer1 == nullptr && buffer2 == nullptr)) {
        return static_cast< int32_t >(SaveLogOperationResult::INVALID_PARAMETERS);
    }

    std::lock_guard< decltype(bufferMutex_) > const lock{bufferMutex_};
    {
        DltStorageHeader storageheader{};
        _setStorageheaderWithTimeStamp(&storageheader, ecuId_, dltTimestamp);
        if (fileFd_ != NAI_FD_INVALID) {
            if (logBuffer_->IsFull(sizeof(DltStorageHeader) + len1 + len2)) {
                std::int8_t const kIntTwo{2};
                ssize_t bytesWritten{0};
                /// If there is data in the cache, write it; it's possible the cache is empty
                if (logBuffer_->Len() > 0U) {
                    std::int32_t const iovLen{4};
                    nai_bufvec_t iov[iovLen]{};
                    std::int8_t const kIntThree{3};
                    iov[0].buf = logBuffer_->Data();
                    iov[0].len = logBuffer_->Len();

                    iov[1].buf = reinterpret_cast< uint8_t* >(&storageheader);
                    iov[1].len = sizeof(DltStorageHeader);

                    iov[kIntTwo].buf = buffer1;
                    iov[kIntTwo].len = len1;

                    iov[kIntThree].buf = buffer2;
                    iov[kIntThree].len = len2;

                    bytesWritten = nai_file_writev(fileFd_, iov, iovLen);
                    logBuffer_->Clear();

                } else {
                    std::int32_t const iovLen{3};
                    nai_bufvec_t iov[iovLen]{};

                    iov[0].buf = reinterpret_cast< uint8_t* >(&storageheader);
                    iov[0].len = sizeof(DltStorageHeader);

                    iov[1].buf = buffer1;
                    iov[1].len = len1;

                    iov[kIntTwo].buf = buffer2;
                    iov[kIntTwo].len = len2;

                    bytesWritten = nai_file_writev(fileFd_, iov, iovLen);
                }
                if (bytesWritten < 0) {
                    std::ignore = LOGERROR(" bytesWritten : error");
                    return static_cast< int32_t >(SaveLogOperationResult::FILE_WRITE_FAILED);
                }
                // File write succeeded, record the write time and return the number of bytes written
                if (bufferModeManager_) {
                    bufferModeManager_->RecordWrite();
                }
                return static_cast< int32_t >(bytesWritten);

            } else {
                if (logBuffer_ != nullptr) {
                    std::size_t headerPushed
                        = logBuffer_->Push(reinterpret_cast< std::uint8_t* >(&storageheader), sizeof(DltStorageHeader));
                    if (headerPushed == 0) {
                        return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                    }
                    std::size_t buffer1Pushed = logBuffer_->Push(buffer1, len1);
                    if (buffer1Pushed == 0 && len1 > 0) {
                        return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                    }
                    std::size_t buffer2Pushed = logBuffer_->Push(buffer2, len2);
                    if (buffer2Pushed == 0 && len2 > 0) {
                        return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                    }
                    // Buffer write succeeded, return the total number of bytes written
                    std::int32_t totalBytes = static_cast< int32_t >(headerPushed + buffer1Pushed + buffer2Pushed);
                    return totalBytes;
                } else {
                    return static_cast< int32_t >(SaveLogOperationResult::BUFFER_NULL);
                }
            }
        } else {
            /// If there is space in the buffer, cache it first
            if (logBuffer_ != nullptr && (logBuffer_->IsFull(sizeof(DltStorageHeader) + len1 + len2) == false)) {
                std::size_t headerPushed
                    = logBuffer_->Push(reinterpret_cast< std::uint8_t* >(&storageheader), sizeof(DltStorageHeader));
                if (headerPushed == 0) {
                    return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                }
                std::size_t buffer1Pushed = logBuffer_->Push(buffer1, len1);
                if (buffer1Pushed == 0 && len1 > 0) {
                    return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                }
                std::size_t buffer2Pushed = logBuffer_->Push(buffer2, len2);
                if (buffer2Pushed == 0 && len2 > 0) {
                    return static_cast< int32_t >(SaveLogOperationResult::BUFFER_PUSH_FAILED);
                }
                // Buffer write succeeded, return the total number of bytes written
                std::int32_t totalBytes = static_cast< int32_t >(headerPushed + buffer1Pushed + buffer2Pushed);
                return totalBytes;
            } else {
                LOGERROR(__func__) << ": no buffer no file opened .";
                return static_cast< int32_t >(SaveLogOperationResult::BUFFER_NULL);
            }
        }
    }
}

int32_t FileSinkerNoLocker::SavePlainTextLog(std::uint8_t* buffer1, std::size_t const& len1) noexcept
{
    this->CheckFileOverSize();
    return this->SavePlainTextLogNocheck(buffer1, len1);
}
/// @brief
/// @param[in]  buffer1
/// @param[in]  len1
/// @param[in]  buffer2
/// @param[in]  len2
/// @return
int32_t FileSinkerNoLocker::SavePlainTextLogNocheck(std::uint8_t* buffer1, std::size_t const& len1) noexcept
{
    LOGVERBOSE(__func__) << len1;
    if (inited_ == false) {
        return -1;
    }
    if ((buffer1 != nullptr) && (len1 > 0U) && (buffer1[len1 - 1U] == '\0')) {
        buffer1[len1 - 1U] = '\n';
    }
    std::lock_guard< decltype(bufferMutex_) > const lock{bufferMutex_};
    /// Buffer operations
    {
        if (fileFd_ != NAI_FD_INVALID) {
            if (logBuffer_->IsFull(len1)) {
                ssize_t bytesWritten{0};
                /// If there is data in the cache, write it; it's possible the cache is empty
                if (logBuffer_->Len() > 0U) {
                    std::int32_t const iovLen{2};
                    nai_bufvec_t iov[iovLen]{};

                    iov[0].buf = logBuffer_->Data();
                    iov[0].len = logBuffer_->Len();

                    iov[1].buf = buffer1;
                    iov[1].len = len1;

                    bytesWritten = nai_file_writev(fileFd_, iov, iovLen);
                    logBuffer_->Clear();

                } else {
                    bytesWritten = nai_file_write(fileFd_, buffer1, len1);
                }
                if (bytesWritten < 0) {
                    std::ignore = LOGERROR(" bytesWritten : error");
                    return -1;
                }
                if (bufferModeManager_) {
                    bufferModeManager_->RecordWrite();
                }

            } else {
                if (logBuffer_ != nullptr) {
                    std::ignore = logBuffer_->Push(buffer1, len1);
                }
            }
        } else {
            if (logBuffer_ != nullptr && (logBuffer_->IsFull(len1) == false)) {
                std::ignore = logBuffer_->Push(buffer1, len1);
            } else {
                LOGERROR(__func__) << ": no buffer no file opened .";
            }
        }
    }
    return 0;
}

/// @brief
/// @return
bool FileSinkerNoLocker::FlushBuffer() noexcept
{
    if (inited_ == false) {
        return false;
    }

    if (fileFd_ != NAI_FD_INVALID) {
        std::lock_guard< decltype(bufferMutex_) > const lock{bufferMutex_};
        if ((logBuffer_ != nullptr) && logBuffer_->Len() > 0U) {
            std::ignore = nai_file_write(fileFd_, logBuffer_->Data(), logBuffer_->Len());
            logBuffer_->Clear();
        }
    }
    return true;
}
/// Only get archived files, not the currently used file.
std::list< std::string > FileSinkerNoLocker::_getFileList() noexcept { return rolloverHelper_.GetFileList(); }

bool FileSinkerNoLocker::_matchLoopName(std::string const& str) noexcept { return rolloverHelper_.MatchLoopName(str); }

/**
 * @brief Safely convert a string to unsigned long long
 * @return true on successful conversion, false on failure (private)
 */
bool FileSinkerNoLocker::_safeStoull(const std::string& str, unsigned long long& outValue) noexcept
{
    if (str.empty()) {
        return false;
    }
    char* endptr{nullptr};
    errno                  = 0;
    unsigned long long res = strtoull(str.c_str(), &endptr, 10);
    if (errno == ERANGE && res == ULLONG_MAX) {
        return false;
    }
    if (endptr == str.c_str()) {
        return false;
    }
    if (*endptr != '\0') {
        return false;
    }
    outValue = res;
    return true;
}
/// @brief check
/// @return
bool FileSinkerNoLocker::CheckFileOverSize() noexcept
{
    std::lock_guard< decltype(bufferMutex_) > const lock{bufferMutex_};
    LOGVERBOSE(__func__) << ": enter ";
    if (inited_ == false) {
        LOGVERBOSE(__func__) << "inited_ failed ,do nothing , leave ";
        return false;
    }
    /// Close file, rename file, open file; no exceptions in between
    std::size_t const fsize{_getFileSize()};

    if ((fsize > singleFileSize_) && (0U < singleFileSize_)) {
        LOGVERBOSE(__func__) << ": change file  ";
        CloseFile();
        std::chrono::milliseconds const sec{std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch())};

        // Check whether the system time is earlier than the timestamp of the newest log file
        std::size_t newTs = sec.count();

        std::string const rolloverSuffix{this->_getRolloverSuffix()};
        std::ignore = _rotateArchivedFiles(rolloverSuffix);

        std::string const newName{_getTsFileName(1, newTs)};
        if (nai_path_rename(MainFileName().c_str(), newName.c_str()) == 0) {
            LOGVERBOSE("File renamed successfully to : ") << newName;
            std::ignore = OpenFile();
        } else {
            LOGERROR("rename failed") << newName;
            if (OpenFile()) {
                std::ignore = _truncateOpenedFile();
            }
        }
        if (this->loopFileCount_ == 1) {
            // When only one file is used, delete the old file
            std::ignore = _removeFile(newName);
        }

        if (this->compress_) {
            std::ignore = this->_startZipTask(newName);
        }
        return true;
    }

    LOGVERBOSE(__func__) << ": leave";
    return false;
}

bool FileSinkerNoLocker::_startZipTask(const std::string& newName) noexcept
{
    if (inited_ == false) {
        return false;
    }
    std::lock_guard< std::mutex > lock(compressListMutex_);  // Lock
    this->_cleanCompletedCompressors();
    // Create a ZipCompressor instance for asynchronous compression
    ZipCompressorPtr compressor = std::make_shared< ZipCompressor >();
    compressList_.push_back(compressor);
    bool success
        = compressor->CompressAsync(newName, true,
                                    std::bind(&FileSinkerNoLocker::_onCompressionFinished, this, newName,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    if (!success) {
        LOGERROR("Failed to start async compression, proceeding with file management: ") << newName;
        // If asynchronous compression cannot be started, still delete the file
        std::ignore = _removeFile(newName);
    }

    return success;
}

/// @brief
/// @return
std::size_t FileSinkerNoLocker::_getFileSize() const noexcept
{
    if (fileFd_ == NAI_FD_INVALID) {
        return 0;
    }
    nai_off64_t size = nai_file_seek(fileFd_, 0, NAI_SEEK_END);
    return size >= 0 ? static_cast< std::size_t >(size) : 0;
}

/// @brief
bool FileSinkerNoLocker::OpenFile() noexcept
{
    LOGVERBOSE(__func__) << " mainfile name" << MainFileName();
    std::int32_t const maskInt{0666};

    fileFd_ = nai_file_open(MainFileName().c_str(), NAI_O_CREAT | NAI_O_APPEND | NAI_O_WRONLY, maskInt);
    nai_file_set_cloexec(fileFd_, 1);
    if (fileFd_ == NAI_FD_INVALID) {
        LOGERROR(__func__) << "error open log file";
        return false;
    }

    // Only write the UTF-8 BOM header (EF BB BF) when creating a new file or when the file is empty
    if (_getFileSize() == 0U) {
        std::uint8_t const kUtf8Bom[] = {0xEFU, 0xBBU, 0xBFU};
        if (nai_file_write(fileFd_, kUtf8Bom, sizeof(kUtf8Bom)) < 0) {
            LOGERROR(__func__) << "failed to write utf8 bom";
            std::ignore = nai_file_close(fileFd_);
            fileFd_     = NAI_FD_INVALID;
            return false;
        }
    }

    return true;
}
/// @brief
void FileSinkerNoLocker::CloseFile() noexcept
{
    std::ignore = nai_file_close(fileFd_);
    fileFd_     = NAI_FD_INVALID;
}

/// @brief
/// @return
std::string FileSinkerNoLocker::MainFileName() const noexcept { return logDir_ + "/" + cleanName_ + suffix_; }

bool FileSinkerNoLocker::_removeFile(std::string const& fullPath) noexcept
{
#ifdef __qnx__
    std::int32_t const ret = remove(fullPath.c_str());
#else
    std::int32_t const ret = unlink(fullPath.c_str());
#endif
    return (ret == 0);
}
bool FileSinkerNoLocker::_truncateOpenedFile() noexcept
{
    if (fileFd_ <= 0) {
        return false;
    }
    if (ftruncate(fileFd_, 0) != 0) {
        return false;
    }
    return lseek(fileFd_, 0, SEEK_SET) != -1;
}

bool FileSinkerNoLocker::RolloverEntryLess::operator()(RolloverEntry const& lhs,
                                                       RolloverEntry const& rhs) const noexcept
{
    if (lhs.index == rhs.index) {
        return lhs.filename < rhs.filename;
    }
    return lhs.index < rhs.index;
}

bool FileSinkerNoLocker::_rotateArchivedFiles(std::string const& rolloverSuffix) noexcept
{
    rolloverHelper_.SetLoopFileCount(loopFileCount_);
    return rolloverHelper_.RotateIndexedFiles(rolloverSuffix);
}

/// @brief
/// @param[in]  ts
/// @return
std::string FileSinkerNoLocker::_getTsFileName(std::size_t const& index, std::size_t const& ts) const noexcept
{
    std::string filename{logDir_};
    std::ignore = filename.append("/");
    std::ignore = filename.append(cleanName_);
    std::ignore = filename.append("_");
    std::ignore = filename.append(std::to_string(index));
    std::ignore = filename.append("_");
    std::ignore = filename.append(std::to_string(ts));
    std::ignore = filename.append(suffix_);
    return filename;
}

void FileSinkerNoLocker::_setStorageheaderWithTimeStamp(DltStorageHeader* const storageheader,
                                                        std::string const& strecu,
                                                        DltTimeStamp const& timestamp) noexcept
{
    bool const checkstr{strecu.empty()};
    if ((storageheader == nullptr) || checkstr) {
        return;
    }

    std::ignore                 = memcpy(static_cast< void* >(storageheader->ecu), strecu.data(), strecu.size());
    storageheader->seconds      = static_cast< std::uint32_t >(timestamp.seconds);
    storageheader->microseconds = static_cast< std::uint32_t >(timestamp.uSeconds);
}

void FileSinkerNoLocker::_seprateDirCleanname()
{
    std::size_t found{filename_.find_last_of('/')};
    std::string tmpfilename{};
    if (std::string::npos != found) {
        logDir_     = filename_.substr(0U, found + 1U);
        tmpfilename = filename_.substr(found + 1U);
    }
    if (logDir_.empty()) {
        logDir_ = "./";
    }

    LOGVERBOSE("logfilename logdir_") << logDir_;

    Char8_T const kCharDot{'.'};
    found = tmpfilename.find_last_of(kCharDot);
    if (std::string::npos != found) {
        suffix_    = tmpfilename.substr(found);
        cleanName_ = tmpfilename.substr(0U, found);
    }
    LOGVERBOSE("logfilename suffix_") << suffix_;
    LOGVERBOSE("logfilename cleanName_") << cleanName_;
}

}  // namespace internal
}  // namespace log

}  // namespace ara
