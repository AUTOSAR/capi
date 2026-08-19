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
/// @file       file_sinker_no_locker.h
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

#ifndef __LOG_INTERNAL_FILESINKER_NOLOCKER__
#define __LOG_INTERNAL_FILESINKER_NOLOCKER__

#include <nai/os/nai_file.h>

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <tuple>

#include "Utils/src/log_buffer.h"
#include "buffer_mode.h"
#include "common.h"
#include "internal/dlt_constants.h"
#include "internal/dlt_structures.h"
#include "log_sinker_interface.h"
#include "rollover_helper.h"
#include "zip_compressor.h"
namespace ara {
namespace log {
namespace internal {
/// @brief Log file storage management class, splitting according to specified size and count, with built-in cache functionality that writes to disk only when the cache size is reached.
////  Using the unified interface Sinker. The current interface uses encapsulated binary log data.
////  When saving files, a corresponding file header needs to be added; the implementation should check the corresponding protocol.

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00167
/// @trace_id_dd=DD_LOG_00252
/// @needwork = ad
/// @endcode
class FileSinkerNoLocker final : public IlogSinker
{
public:
    enum class SaveLogOperationResult : int32_t
    {
        NOT_INITIALIZED    = -1,  // Not initialized
        BUFFER_PUSH_FAILED = -2,  // Buffer push failed
        FILE_WRITE_FAILED  = -3,  // File write failed
        INVALID_PARAMETERS = -4,  // Invalid parameters
        BUFFER_NULL        = -5,  // Buffer is null
        // Success: returns the number of bytes written (>= 0)
    };
    /// @brief  Shared data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00168
    /// @trace_id_dd=DD_LOG_00253
    /// @needwork = ad
    /// @endcode
    struct ShareData
    {
        /// @brief File index
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01694
        /// @needwork = dda
        /// @endcode
        std::size_t index;
        /// @brief Timestamp
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01695
        /// @needwork = dda
        /// @endcode
        std::uint32_t ts;
    };

    /// @brief Custom constructor, receives configuration. QAC requires member variables to be initialized consistently.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00169
    /// @trace_id_dd=DD_LOG_00254
    /// @needwork = ad
    /// @endcode
    FileSinkerNoLocker() = default;
    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00345
    /// @trace_id_dd=DD_LOG_01744
    /// @needwork = ad
    /// @endcode
    FileSinkerNoLocker(FileSinkerNoLocker const& other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00346
    /// @trace_id_dd=DD_LOG_01745
    /// @needwork = ad
    /// @endcode
    FileSinkerNoLocker& operator=(FileSinkerNoLocker const& other) = delete;
    /// @needwork = ad
    FileSinkerNoLocker(FileSinkerNoLocker&& other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00346
    /// @trace_id_dd=DD_LOG_01745
    /// @needwork = ad
    /// @endcode
    FileSinkerNoLocker& operator=(FileSinkerNoLocker&& other) = delete;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00170
    /// @trace_id_dd=DD_LOG_00255
    /// @needwork = ad
    /// @endcode
    ~FileSinkerNoLocker() noexcept final;
    /// @brief Used to receive parameters for initialization
    /// @param[in]  ecuId ECUID
    /// @param[in]  filename File name
    /// @param[in]  singleFilesize Size of a single file
    /// @param[in]  maxfileCount Number of log files
    /// @param[in]  buffersize Cache size
    /// @return  0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00171
    /// @trace_id_dd=DD_LOG_00256
    /// @needwork = ad
    /// @endcode
    bool Init(std::string const& ecuId,
              std::string const& filename,
              std::size_t const& singleFilesize,
              std::size_t const& maxfileCount,
              std::size_t const& buffersize) noexcept;

    /// @brief Save the log
    /// @param[in]  buffer1 Cache pointer 1
    /// @param[in]  len1  Cache length
    /// @param[in]  buffer2 Cache pointer 2
    /// @param[in]  len2 Cache length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00172
    /// @trace_id_dd=DD_LOG_00257
    /// @needwork = ad
    /// @endcode
    std::int32_t SaveLog(std::uint8_t* buffer1,
                         std::size_t const& len1,
                         std::uint8_t* buffer2,
                         std::size_t const& len2) noexcept final;

    std::int32_t SaveLogNoCheck(std::uint8_t* buffer1,
                                std::size_t const& len1,
                                std::uint8_t* buffer2,
                                std::size_t const& len2) noexcept;

    int32_t SaveLogWithTimeStamp(std::uint8_t* buffer1,
                                 std::size_t const& len1,
                                 std::uint8_t* buffer2,
                                 std::size_t const& len2,
                                 DltTimeStamp const& dltTimestamp) noexcept final;

    std::int32_t SavePlainTextLog(std::uint8_t* buffer1, std::size_t const& len1) noexcept;

    int32_t SaveLogWithTimeStampNocheck(std::uint8_t* buffer1,
                                        std::size_t const& len1,
                                        std::uint8_t* buffer2,
                                        std::size_t const& len2,
                                        DltTimeStamp const& dltTimestamp) noexcept;

    std::int32_t SavePlainTextLogNocheck(std::uint8_t* buffer1, std::size_t const& len1) noexcept;
    /// @brief Output the cache
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00173
    /// @trace_id_dd=DD_LOG_00258
    /// @needwork = ad
    /// @endcode
    bool FlushBuffer() noexcept final;

    bool SetLogLevel(std::string const& contextId, std::uint8_t const& logLevel) noexcept final
    {
        logLevels_[contextId] = logLevel;
        return true;
    }

    bool SetPlainText(bool const& plainText) noexcept
    {
        plainText_ = plainText;
        return true;
    }
    bool SetCompress(bool const& enableCompress) noexcept
    {
        compress_ = enableCompress;
        rolloverHelper_.SetCompress(compress_);
        return true;
    }

    std::string RolloverSuffix() noexcept { return rolloverHelper_.RolloverSuffix(); }

    bool GetCompress() const noexcept { return compress_; }

    bool OpenFile() noexcept;
    /// @brief Close the current log file
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00266
    /// @needwork = dda
    /// @endcode
    void CloseFile() noexcept;
    /// @brief Check if the file has exceeded the size limit
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00263
    /// @needwork = dda
    /// @endcode
    bool CheckFileOverSize() noexcept;

    void OnFileNeedReopen(bool needReopen) noexcept
    {
        if (needReopen) {
            std::lock_guard< decltype(bufferMutex_) > const lock{bufferMutex_};
            this->CloseFile();
            this->OpenFile();
        }
    }

    std::int32_t GetFileDescriptor() noexcept { return fileFd_; }

    std::string MainFileName() const noexcept;

private:
    using RolloverEntry = RolloverHelper::RolloverEntry;

    struct RolloverEntryLess
    {
        bool operator()(RolloverEntry const& lhs, RolloverEntry const& rhs) const noexcept;
    };

    inline bool _cleanCompletedCompressors()
    {
        CompressorList::iterator it = compressList_.begin();
        while (it != compressList_.end()) {
            if (*it && (*it)->IsCompleted()) {
                it = compressList_.erase(it);
            } else {
                ++it;
            }
        }
        return true;
    }
    inline std::string _getRolloverSuffix() noexcept { return rolloverHelper_.RolloverSuffix(); }

    bool _startZipTask(const std::string& newName) noexcept;
    /// @brief
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00259
    /// @needwork = dda
    /// @endcode
    std::list< std::string > _getFileList() noexcept;

    bool _safeStoull(const std::string& str, unsigned long long& outValue) noexcept;
    bool _removeFile(std::string const& fullPath) noexcept;

    /// @brief Determine whether the file header matches the required log file format
    /// @param str
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00261
    /// @needwork = dda
    /// @endcode
    bool _matchLoopName(std::string const& str) noexcept;
    /// @brief Reacquire the file descriptor
    /// @return true ok false failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00262
    /// @needwork = dda
    /// @endcode
    bool _resetFD() noexcept;

    /// @brief Get the size of the log file currently in use
    /// @return Log file size in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00264
    /// @needwork = dda
    /// @endcode
    std::size_t _getFileSize() const noexcept;

    /// @brief Open the log file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00265
    /// @needwork = dda
    /// @endcode

    /// @brief Whether the main file has changed
    /// @return true changed, false not changed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00267
    /// @needwork = dda
    /// @endcode
    bool _isChanged() const noexcept;

    /// @brief
    /// @param[in]  ts
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00270
    /// @needwork = dda
    /// @endcode
    std::string _getTsFileName(std::size_t const& index, std::size_t const& ts) const noexcept;

    void _setStorageheaderWithTimeStamp(DltStorageHeader* const storageheader,
                                        std::string const& strecu,
                                        DltTimeStamp const& timestamp) noexcept;
    /// @brief Split the path and the file name without suffix
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00272
    /// @needwork = dda
    /// @endcode
    void _seprateDirCleanname();

    /// @brief Compression completion callback (for std::bind, to avoid lambda)
    void _onCompressionFinished(std::string const& newName,
                                std::string const& compressedFile,
                                bool success,
                                std::string const& errorMsg) noexcept
    {
        std::ignore = compressedFile;
        std::ignore = errorMsg;
        std::ignore = newName;

        // Delete the source file
        if (success) {
            // if (_removeFile(newName) == false) {
            //     // LOGERROR(__func__) << "Failed to remove source file: " << newName;
            // }
        }
    }
    /// @brief Truncate the currently open file to 0 bytes
    bool _truncateOpenedFile() noexcept;

    /// @brief Shift indices of all archived files upward by 1 (processed from high to low)
    /// @param rolloverSuffix The suffix used for the current archive (related to compression configuration)
    /// @return true success, false process encountered an error
    bool _rotateArchivedFiles(std::string const& rolloverSuffix) noexcept;

private:
    /// @brief fileFd_ File descriptor of the log file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00275
    /// @needwork = dda
    /// @endcode
    nai_fd_t fileFd_{NAI_FD_INVALID};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00276
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< LogBuffer > logBuffer_{nullptr};

    RolloverHelper rolloverHelper_{};

    /// @brief uniqPath_ Shared memory path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00277
    /// @needwork = dda
    /// @endcode
    std::string uniqPath_{""};
    /// @brief logDir_ Log file storage path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00278
    /// @needwork = dda
    /// @endcode
    std::string logDir_{"/tmp/logtest/"};
    /// @brief  first_ Whether it is the first time opening
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00279
    /// @needwork = dda
    /// @endcode
    bool first_{false};
    /// @brief inited_ Whether initialization is ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00280
    /// @needwork = dda
    /// @endcode
    bool inited_{false};
    /// @brief ecuId
    /// @code{.isoft}
    ///---------log file info------------///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00281
    /// @needwork = dda
    /// @endcode
    std::string ecuId_{};
    /// @brief filename_ File name, excluding suffix; the full file name is filename_ + suffix_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00282
    /// @needwork = dda
    /// @endcode
    std::string filename_{};
    /// @brief File name without path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00283
    /// @needwork = dda
    /// @endcode
    std::string cleanName_{};
    /// @brief  singleFileSize_ Size of a single file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00284
    /// @needwork = dda
    /// @endcode
    std::size_t singleFileSize_{0U};
    /// @brief loopFileCount_  Maximum number of files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00285
    /// @needwork = dda
    /// @endcode
    std::size_t loopFileCount_{0U};
    /// @brief bufferSize_   File cache size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00286
    /// @needwork = dda
    /// @endcode
    std::size_t bufferSize_{0U};
    /// @brief suffix_  Suffix of the log file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00287
    /// @needwork = dda
    /// @endcode
    std::string suffix_{".dlt"};
    /// @brief bufferMutex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00167
    /// @trace_id_dd=DD_LOG_00288
    /// @needwork = dda
    /// @endcode
    std::mutex bufferMutex_;

    std::map< std::string, std::uint8_t > logLevels_;

    bool plainText_{false};

    bool compress_{false};

    CompressorList compressList_;

    std::mutex compressListMutex_;

    std::unique_ptr< BufferModeManager > bufferModeManager_{nullptr};
};

}  // namespace internal

}  // namespace log

}  // namespace ara
#endif