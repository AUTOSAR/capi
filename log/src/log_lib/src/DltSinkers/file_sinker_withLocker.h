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
/// @file       file_sinker_withLocker.h
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
/// @unit_name = file_sinker_withLocker
/// @unit_description=Dlt lib's implemented log sinker for writing to files
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_FILESINKER_COMMON__
#define __LOG_INTERNAL_FILESINKER_COMMON__

#include <nai/os/nai_file.h>

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Utils/src/log_buffer.h"
#include "file_sinker_no_locker.h"
#include "internal/dlt_constants.h"
#include "internal/dlt_structures.h"
#include "ipc_shared_state.h"
#include "log_sinker_interface.h"
#include "process_flock.h"
#include "semaphore_lock.h"
#include "zip_compressor.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Log file storage management class, splitting according to specified size and count, with built-in cache functionality that writes to disk only when the cache size is reached.
////  Using the unified interface Sinker. The current interface uses encapsulated binary log data.
////  When saving files, a corresponding file header needs to be added; the implementation should check the corresponding protocol.

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00181
/// @trace_id_dd=DD_LOG_00308
/// @needwork = ad
/// @endcode
class FileSinkerWithLocker final : public IlogSinker
{
public:
    class SemaphoreLockGuard
    {
    public:
        explicit SemaphoreLockGuard(SemaphoreLock& lock) : lock_(lock) { lock_.Lock(); }
        ~SemaphoreLockGuard() { lock_.UnLock(); }
        SemaphoreLockGuard(const SemaphoreLockGuard&) = delete;
        SemaphoreLockGuard& operator=(const SemaphoreLockGuard&) = delete;
        SemaphoreLockGuard(const SemaphoreLockGuard&&)           = delete;
        SemaphoreLockGuard& operator=(const SemaphoreLockGuard&&) = delete;

    private:
        SemaphoreLock& lock_;
    };
    // Use: replace manual Lock/UnLock in CheckFileOverSize with SemaphoreLockGuard guard(*processLocker_);
    /// @brief  Shared data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00182
    /// @trace_id_dd=DD_LOG_00309
    /// @needwork = ad
    /// @endcode
    using ShareData = LogShareData;

    /// @brief Custom constructor, receives configuration. QAC requires member variables to be initialized consistently.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00183
    /// @trace_id_dd=DD_LOG_00310
    /// @needwork = ad
    /// @endcode
    FileSinkerWithLocker() = default;
    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00349
    /// @trace_id_dd=DD_LOG_01748
    /// @needwork = ad
    /// @endcode
    FileSinkerWithLocker(FileSinkerWithLocker const& other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00350
    /// @trace_id_dd=DD_LOG_01749
    /// @needwork = ad
    /// @endcode
    FileSinkerWithLocker& operator=(FileSinkerWithLocker const& other) = delete;

    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00349
    /// @trace_id_dd=DD_LOG_01748
    /// @needwork = ad
    /// @endcode
    FileSinkerWithLocker(FileSinkerWithLocker const&& other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00350
    /// @trace_id_dd=DD_LOG_01749
    /// @needwork = ad
    /// @endcode
    FileSinkerWithLocker& operator=(FileSinkerWithLocker const&& other) = delete;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00184
    /// @trace_id_dd=DD_LOG_00311
    /// @needwork = ad
    /// @endcode
    ~FileSinkerWithLocker() noexcept final;
    /// @brief Used to receive parameters for initialization
    /// @param[in]  ecuId ECUID
    /// @param[in]  filename File name
    /// @param[in]  singleFilesize Size of a single file
    /// @param[in]  maxfileCount Number of log files
    /// @param[in]  buffersize Cache size
    /// @return  0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00185
    /// @trace_id_dd=DD_LOG_00312
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
    /// @trace_id_ad=AD_LOG_00186
    /// @trace_id_dd=DD_LOG_00313
    /// @needwork = ad
    /// @endcode
    std::int32_t SaveLogWithTimeStamp(std::uint8_t* buffer1,
                                      std::size_t const& len1,
                                      std::uint8_t* buffer2,
                                      std::size_t const& len2,
                                      DltTimeStamp const& dltTimestamp) noexcept final;

    std::int32_t SavePlainTextLog(std::uint8_t* buffer1, std::size_t const& len1) noexcept final;

    /// @brief Output the cache
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00187
    /// @trace_id_dd=DD_LOG_00314
    /// @needwork = ad
    /// @endcode
    bool FlushBuffer() noexcept final;
    bool SetLogLevel(std::string const& contextId, std::uint8_t const& logLevel) noexcept final
    {
        return fileSinkerImpl_.SetLogLevel(contextId, logLevel);
    }
    bool SetPlainText(bool const& plainText) noexcept { return fileSinkerImpl_.SetPlainText(plainText); }
    bool SetCompress(bool const& enableCompress) noexcept { return fileSinkerImpl_.SetCompress(enableCompress); }

private:
    /// @brief Resource cleanup
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00325
    /// @needwork = dda
    /// @endcode
    std::int32_t _destroy() noexcept;

    std::string _replaceStr(std::string str, std::string const& toReplaced, std::string const& newchars) noexcept
    {
        for (std::string::size_type pos{0U}; pos != std::string::npos; pos += newchars.length()) {
            pos = str.find(toReplaced, pos);
            if (pos != std::string::npos) {
                std::ignore = str.replace(pos, toReplaced.length(), newchars);
            } else {
                break;
            }
        }
        return str;
    }
    /// @brief Check if the file has exceeded the size limit
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00319
    /// @needwork = dda
    /// @endcode
    bool _checkWithLocker() noexcept;

private:
    FileSinkerNoLocker fileSinkerImpl_{};
    // IPC shared state helper (wraps shm + mmap lifecycle)
    IpcSharedState ipc_{};
    /// @brief fileFd_ File descriptor of the log file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00333
    /// @needwork = dda
    /// @endcode
    nai_fd_t fileFd_{NAI_FD_INVALID};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00334
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< LogBuffer > logBuffer_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00335
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ProcessThreadLock > processLocker_{nullptr};
    // removed raw shm path; managed by IpcSharedState
    /// @brief logDir_ Log file storage path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00339
    /// @needwork = dda
    /// @endcode
    std::string logDir_{"/tmp/logtest/"};
    // removed first_ flag; tracked inside IpcSharedState
    /// @brief inited_ Whether initialization is ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00341
    /// @needwork = dda
    /// @endcode
    bool inited_{false};
    /// @brief ecuId
    /// @code{.isoft}
    ///---------log file info------------///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00342
    /// @needwork = dda
    /// @endcode
    std::string ecuId_{};
    /// @brief filename_ File name, excluding suffix; the full file name is filename_ + suffix_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00343
    /// @needwork = dda
    /// @endcode
    std::string filename_{};
    /// @brief File name without path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00344
    /// @needwork = dda
    /// @endcode
    std::string cleanName_{};
    /// @brief  singleFileSize_ Size of a single file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00345
    /// @needwork = dda
    /// @endcode
    std::size_t singleFileSize_{0U};
    /// @brief loopFileCount_  Maximum number of files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00346
    /// @needwork = dda
    /// @endcode
    std::size_t loopFileCount_{0U};
    /// @brief bufferSize_   File cache size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00347
    /// @needwork = dda
    /// @endcode
    std::size_t bufferSize_{0U};
    /// @brief suffix_  Suffix of the log file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00348
    /// @needwork = dda
    /// @endcode
    std::string suffix_{".dlt"};
    /// @brief bufferMutex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00349
    /// @needwork = dda
    /// @endcode
    std::mutex bufferMutex_;

    std::map< std::string, std::uint8_t > logLevels_;
};

}  // namespace internal

}  // namespace log

}  // namespace ara
#endif