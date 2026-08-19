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
/// @file       remote_sinker.h
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
/// @trace_id_sr=LOG_SR_00009
/// @unit_name = remote_sinker
/// @unit_description=Sinker implemented by the Dlt lib for sending logs to DltLogd and then forwarding to dlt client
/// @endcode
///
/// ================================================================

#ifndef __LOG_remote_sinker__
#define __LOG_remote_sinker__

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>

#include "Utils/src/log_buffer.h"
#include "log_sinker_interface.h"

namespace ara {
namespace log {

namespace internal {
using LogLevelMap    = std::map< std::string, std::uint8_t >;
using LogLevelMapPtr = std::shared_ptr< LogLevelMap >;

/// @brief Connect to the daemon and send logs to the daemon. This channel only sends logs. Use mkfifo named pipe.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00174
/// @trace_id_dd=DD_LOG_00289
/// @needwork = ad
/// @endcode
class RemoteSinker final : public IlogSinker
{
public:
    enum RemoteSinkerError : std::int32_t
    {
        kRemoteSinkerOk                = 0,
        kRemoteSinkerStopped           = -1,
        kRemoteSinkerBackendNotRunning = -2,
        kRemoteSinkerPipeOpenFailed    = -3,
        kRemoteSinkerInvalidPayload    = -4,
        kRemoteSinkerBufferDropped     = -5,
        kRemoteSinkerWriteTimeout      = -6,
        kRemoteSinkerBadFileDescriptor = -7,
        kRemoteSinkerBrokenPipe        = -8,
        kRemoteSinkerWriteWouldBlock   = -9,
        kRemoteSinkerUnknownWriteError = -10
    };

    /// @brief Constructor
    /// @param[in]  queueSize  Cache size
    /// @param[in]  appid  Application ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00175
    /// @trace_id_dd=DD_LOG_00290
    /// @needwork = ad
    /// @endcode
    explicit RemoteSinker(std::uint32_t const& queueSize, std::string appid) noexcept;

    RemoteSinker(RemoteSinker&&) noexcept = delete;
    RemoteSinker& operator=(RemoteSinker&&) noexcept = delete;

    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00347
    /// @trace_id_dd=DD_LOG_01746
    /// @needwork = ad
    /// @endcode
    RemoteSinker(RemoteSinker const& other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00348
    /// @trace_id_dd=DD_LOG_01747
    /// @needwork = ad
    /// @endcode
    RemoteSinker& operator=(RemoteSinker const& other) = delete;
    /// @brief  Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00176
    /// @trace_id_dd=DD_LOG_00291
    /// @needwork = ad
    /// @endcode
    ~RemoteSinker() noexcept final;
    /// @brief Initialization function, executed once
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00177
    /// @trace_id_dd=DD_LOG_00292
    /// @needwork = ad
    /// @endcode
    std::int32_t Init() noexcept;

    /// @brief Save the log
    /// @param[in]  ptr1  Pointer
    /// @param[in]  len1  Length
    /// @param[in]  ptr2 Pointer
    /// @param[in]  len2 Length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00178
    /// @trace_id_dd=DD_LOG_00293
    /// @needwork = ad
    /// @endcode
    int32_t SaveLog(std::uint8_t* ptr1,
                    std::size_t const& len1,
                    std::uint8_t* ptr2,
                    std::size_t const& len2) noexcept final;

    /// @brief
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00179
    /// @trace_id_dd=DD_LOG_00294
    /// @needwork = ad
    /// @endcode
    bool FlushBuffer() noexcept final;
    /// @brief Set the status
    /// @param[in]  status Status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00180
    /// @trace_id_dd=DD_LOG_00295
    /// @needwork = ad
    /// @endcode
    void SetRegStatus(std::int32_t const& status) noexcept;

    /// @brief
    /// @return
    /// @needwork = ad
    bool StopSending() noexcept;
    bool SetLogLevel(std::string const& contextId, std::uint8_t const& logLevel) noexcept final
    {
        logLevels_[contextId] = logLevel;
        return true;
    }
    /// @brief Determine whether the FIFO is connected
    /// @return true ok, false not connected
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00298
    /// @needwork = dda
    /// @endcode
    bool FifoOK() const noexcept;

    /// @brief Get the singleton instance
    /// @return Singleton pointer, returns nullptr if not initialized
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00181
    /// @trace_id_dd=DD_LOG_00308
    /// @needwork = ad
    /// @endcode
    static RemoteSinker* Instance() noexcept;

    /// @brief Initialize the singleton
    /// @param[in] queueSize Cache size
    /// @param[in] appid Application ID
    /// @return Singleton pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00182
    /// @trace_id_dd=DD_LOG_00309
    /// @needwork = ad
    /// @endcode
    static RemoteSinker* InitInstance(std::uint32_t const& queueSize, std::string appid) noexcept;

    /// @brief Destroy the singleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00183
    /// @trace_id_dd=DD_LOG_00310
    /// @needwork = ad
    /// @endcode
    static void DestroyInstance() noexcept;

private:
    bool _tryInitIfNeeded(bool force) noexcept;
    /// @brief
    /// @return
    /// @needwork = dda
    bool _isLogdRunning() noexcept;

    /// @brief Resource cleanup function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00296
    /// @needwork = dda
    /// @endcode
    void _destroy() noexcept;
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00297
    /// @needwork = dda
    /// @endcode
    void _sendBuffer() noexcept;

    /// @brief
    /// @param[in]  buffer
    /// @param[in]  len
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00299
    /// @needwork = dda
    /// @endcode
    void _logToBuffer(uint8_t const* const buffer, size_t const& len) noexcept;

    /// @brief
    /// @param entry
    /// @return
    bool _isNumericDir(nai_dir_t const& entry) noexcept;

    /// private methods end
private:
    /// @brief pipeSock_ The socket already opened by the client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00300
    /// @needwork = dda
    /// @endcode
    nai_fd_t pipeSock_{NAI_FD_INVALID};

    /// @brief queueSiz_ How many messages to cache when not connected to the Daemon
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00301
    /// @needwork = dda
    /// @endcode
    std::uint32_t queueSiz_{0};

    /// @brief pipename_ Path tag for inter-process communication
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00302
    /// @needwork = dda
    /// @endcode
    std::string pipename_{};

    /// @brief Status of the command channel, whether it has been registered with the daemon
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00303
    /// @needwork = dda
    /// @endcode
    std::int32_t regStatus_{-1};

    /// @brief buffer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00304
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< LogBuffer > buffer_{nullptr};

    /// @brief bufferLock_
    /// @code{.isoft}
    /// todo Lock operations on buffer_
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00305
    /// @needwork = dda
    /// @endcode
    std::mutex bufferLock_;

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00306
    /// @needwork = dda
    /// @endcode
    std::string appid_{};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_10174
    /// @trace_id_dd=DD_LOG_10306
    /// @needwork = dda
    /// @endcode
    bool sendError_{false};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_10100
    /// @trace_id_dd=DD_LOG_10300
    /// @needwork = dda
    /// @endcode
    std::uint64_t errorCount_{0};

    /// @brief Count of discarded logs
    std::uint64_t dropCount_{0};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00174
    /// @trace_id_dd=DD_LOG_00307
    /// @needwork = dda
    /// @endcode
    std::string const kInternalPipeName_LIBLOG{"/tmp/ara_log_internal_logdata_fifo_"};

    /// @brief
    /// @needwork = dda
    bool mStopSending_{false};

    /// @brief Whether the backend is ready
    bool backendReady_{false};

    /// @brief Whether the most recent initialization attempt failed
    bool initFailed_{false};

    /// @brief Timestamp of the last initialization attempt
    std::chrono::steady_clock::time_point lastInitTry_{};

    /// @brief Retry interval
    std::chrono::milliseconds const reconnectInterval_{std::chrono::milliseconds(500)};

    /// Filtering of each sinker for the context
    LogLevelMap logLevels_;

    /// @brief Singleton instance
    static RemoteSinker* instance_;

    /// @brief Singleton mutex
    static std::mutex instanceMutex_;
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif