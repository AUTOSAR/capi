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
/// @file       remote_sinker.cpp
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

#include "remote_sinker.h"

#include <fcntl.h>
#include <isoft/ara_fsh/platform.h>
#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "Utils/src/private_log.h"
#include "Utils/src/usercommand_define.h"

/// TODO 1. Add member variables to record the backend status. If the backend is not started: cache logs; when the cache is full, try to connect and send; otherwise discard logs.
/// TODO 2. Distinguish between active stop and initialization failure. If actively stopped, there is no need to reinitialize each time a log is written.
/// TODO 3. If initialization fails, you can reinitialize each time you write.
namespace ara {
namespace log {
namespace internal {

/// @brief Static member initialization
RemoteSinker *RemoteSinker::instance_{nullptr};
std::mutex RemoteSinker::instanceMutex_;

/// @brief Get the singleton instance
RemoteSinker *RemoteSinker::Instance() noexcept { return instance_; }

/// @brief Initialize the singleton
RemoteSinker *RemoteSinker::InitInstance(std::uint32_t const &queueSize, std::string appid) noexcept
{
    std::lock_guard< std::mutex > lock(instanceMutex_);
    if (instance_ == nullptr) {
        instance_ = new RemoteSinker(queueSize, std::move(appid));
    }
    return instance_;
}

/// @brief Destroy the singleton
void RemoteSinker::DestroyInstance() noexcept
{
    std::lock_guard< std::mutex > lock(instanceMutex_);
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

/// @brief
/// @param[in]  queueSize
/// @param[in]  appid
RemoteSinker::RemoteSinker(std::uint32_t const &queueSize, std::string appid) noexcept
    : queueSiz_{queueSize}, appid_{std::move(appid)}
{
    std::uint32_t const libsideBuffer{1024U * 3};
    if (queueSiz_ < libsideBuffer) {
        queueSiz_ = libsideBuffer;
    }

    buffer_ = std::make_shared< LogBuffer >(queueSiz_);
}
/// @brief
RemoteSinker::~RemoteSinker() noexcept { _destroy(); }
/// @brief
/// @return
std::int32_t RemoteSinker::Init() noexcept
{
    if (mStopSending_) {
        LOGVERBOSE(__func__) << "RemoteSinker already stopped: " << appid_;
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerStopped);
    }

    if (this->_isLogdRunning() == false) {
        LOGERROR(__func__) << "logd is not running   " << appid_;
        backendReady_ = false;
        initFailed_   = true;
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerBackendNotRunning);
    }

    std::string const fullPath{kInternalPipeName_LIBLOG};

    pipeSock_ = nai_file_open(fullPath.c_str(), NAI_O_WRONLY, 0);
    if (pipeSock_ == NAI_FD_INVALID) {
        perror("open failed");
        LOGERROR("RemoteSinker pipe open  : ") << fullPath << appid_;
        backendReady_ = false;
        initFailed_   = true;
    } else {
        std::ignore = nai_file_set_cloexec(pipeSock_, 1);
        LOGVERBOSE("RemoteSinker pipe open : ") << fullPath << appid_;
        backendReady_ = true;
        initFailed_   = false;
    }
    if (backendReady_ && pipeSock_ != NAI_FD_INVALID) {
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerOk);
    }
    return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerPipeOpenFailed);
}

/// @brief When using the daemon, the daemon is definitely started
/// @return
bool RemoteSinker::FifoOK() const noexcept
{
    if (backendReady_ && pipeSock_ != NAI_FD_INVALID) {
        return true;
    }
    LOGVERBOSE(__func__) << "pipesock is -1";
    return false;
}
/// @brief
/// @param[in]  buffer
/// @param[in]  len
void RemoteSinker::_logToBuffer(uint8_t const *const buffer, size_t const &len) noexcept
{
    if (buffer_) {
        std::ignore = buffer_->Push(buffer, len);
    }
}

/// @brief
/// private
bool RemoteSinker::_isNumericDir(nai_dir_t const &entry) noexcept
{
    if ((entry.type & NAI_S_IFDIR) == 0U) {
        return false;
    }
    if (entry.name == nullptr || strlen(entry.name) == 0) {
        return false;
    }
    return std::all_of(entry.name, entry.name + strlen(entry.name), [](char c) { return std::isdigit(c); });
}

/// @brief
/// @param[in]  ptr1
/// @param[in]  len1
/// @param[in]  ptr2
/// @param[in]  len2
/// @return
std::int32_t RemoteSinker::SaveLog(std::uint8_t *ptr1,
                                   std::size_t const &len1,
                                   std::uint8_t *ptr2,
                                   std::size_t const &len2) noexcept
{
    std::size_t const totalLen{len1 + len2};
    if (totalLen == 0U) {
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerOk);
    }
    if ((ptr1 == nullptr && len1 > 0U) || (ptr2 == nullptr && len2 > 0U)) {
        LOGVERBOSE(__func__) << " null payload with non-zero length";
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerInvalidPayload);
    }
    if (this->mStopSending_) {
        LOGVERBOSE(__func__) << " remotesinker : stop sending ";
        return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerStopped);
    }
    std::lock_guard< decltype(bufferLock_) > loc{bufferLock_};

    bool fifoOk{this->FifoOK()};

    // 1. If the pipe is unavailable, try to reconnect; if reconnection fails, write to the cache; when the cache is full, force a reconnect again; otherwise discard
    if (fifoOk == false) {
        if (_tryInitIfNeeded(false) == false && this->FifoOK() == false) {
            if (buffer_->IsFull(totalLen)) {
                bool const forceOk{_tryInitIfNeeded(true)};
                fifoOk = this->FifoOK();
                if (forceOk == false || fifoOk == false) {
                    buffer_->Clear();
                    dropCount_++;

                    return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerBufferDropped);
                }
                // Force reconnection succeeded, first flush the existing cache
                this->_sendBuffer();
            }
            this->_logToBuffer(ptr1, len1);
            this->_logToBuffer(ptr2, len2);
            return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerOk);
        }
        // Reconnection succeeded, first flush the existing cache
        this->_sendBuffer();
        fifoOk = true;
    }

    // 2. Normal write
    std::int8_t const kDecTwo{2};
    nai_bufvec_t iov[kDecTwo]{};
    std::int32_t bytesWritten{-1};
    iov[0].buf   = ptr1;
    iov[0].len   = len1;
    iov[1].buf   = ptr2;
    iov[1].len   = len2;
    bytesWritten = static_cast< std::int32_t >(nai_file_writev(pipeSock_, iov, kDecTwo));
    if (bytesWritten != static_cast< std::int32_t >(totalLen)) {
        std::int32_t const localError{errno};
        switch (localError) {
            case ETIMEDOUT: {
                std::cout << " ETIMEDOUT -  connect timeout" << __func__ << std::endl;
                pipeSock_        = NAI_FD_INVALID;
                backendReady_    = false;
                initFailed_      = true;
                this->sendError_ = true;
                this->errorCount_++;
                return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerWriteTimeout);
            }
            case EBADF: {
                std::cout << "EBADF - handle not open " << __func__ << std::endl;
                pipeSock_        = NAI_FD_INVALID;
                backendReady_    = false;
                initFailed_      = true;
                this->sendError_ = true;
                this->errorCount_++;
                return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerBadFileDescriptor);
            }
            case EPIPE: {
                std::cout << " EPIPE - pipe error: " << __func__ << std::endl;
                pipeSock_     = NAI_FD_INVALID;
                backendReady_ = false;
                initFailed_   = true;
                if (_tryInitIfNeeded(true) == false || this->FifoOK() == false) {
                    this->_logToBuffer(ptr1, len1);
                    this->_logToBuffer(ptr2, len2);
                    this->sendError_ = true;
                    this->errorCount_++;
                    return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerOk);
                }
                bytesWritten = static_cast< std::int32_t >(nai_file_writev(pipeSock_, iov, kDecTwo));
                if (bytesWritten == static_cast< std::int32_t >(totalLen)) {
                    break;
                }
                this->_logToBuffer(ptr1, len1);
                this->_logToBuffer(ptr2, len2);
                this->sendError_ = true;
                this->errorCount_++;
                return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerBrokenPipe);
            }
            case EAGAIN: {
                std::cout << "EAGAIN - data could not be written : " << __func__ << appid_
                          << " bytesWritten: " << std::dec << bytesWritten << ", alllen:  " << totalLen << std::endl;
                if (_tryInitIfNeeded(true) == false || this->FifoOK() == false) {
                    this->_logToBuffer(ptr1, len1);
                    this->_logToBuffer(ptr2, len2);
                    this->sendError_ = true;
                    this->errorCount_++;
                    return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerOk);
                }
                bytesWritten = static_cast< std::int32_t >(nai_file_writev(pipeSock_, iov, kDecTwo));
                if (bytesWritten == static_cast< std::int32_t >(totalLen)) {
                    break;
                }
                this->_logToBuffer(ptr1, len1);
                this->_logToBuffer(ptr2, len2);
                this->sendError_ = true;
                this->errorCount_++;
                return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerWriteWouldBlock);
            }
            default: {
                std::cout << "unkown error ! " << __func__ << std::endl;
                this->sendError_ = true;
                this->errorCount_++;
                return static_cast< std::int32_t >(RemoteSinkerError::kRemoteSinkerUnknownWriteError);
            }
        }
    }
    this->errorCount_ = 0;
    this->sendError_  = false;
    backendReady_     = true;
    initFailed_       = false;

    return bytesWritten;
}

bool RemoteSinker::FlushBuffer() noexcept
{
    /// Batch send to remote
    return true;
}

/// @brief
/// @param[in]  status
void RemoteSinker::SetRegStatus(std::int32_t const &status) noexcept
{
    regStatus_ = status;
    if (mStopSending_) {
        return;
    }
    if (status == UserCommand::kRegAppStatusOK) {
        if (pipeSock_ == NAI_FD_INVALID || backendReady_ == false) {
            LOGVERBOSE("SetRegStatus reopen  pipe: ") << appid_;
            std::ignore = _tryInitIfNeeded(true);
        }
        if (buffer_->Len() > 0U) {
            std::lock_guard< decltype(bufferLock_) > const lock{bufferLock_};
            LOGVERBOSE("fifo ok  len>0 ,sendtopipe: ") << buffer_->Len() << appid_;
            this->_sendBuffer();
        }
    }
    if (status == UserCommand::kRegAppStatusFailed) {
        LOGVERBOSE(__func__) << appid_;
        std::ignore   = nai_file_close(pipeSock_);
        pipeSock_     = NAI_FD_INVALID;
        backendReady_ = false;
        initFailed_   = true;
    }
}
bool RemoteSinker::StopSending() noexcept
{
    LOGVERBOSE(__func__) << " remotesinker " << this->appid_;
    this->mStopSending_ = true;
    backendReady_       = false;
    if (this->pipeSock_ != NAI_FD_INVALID) {
        std::ignore     = nai_file_close(pipeSock_);
        this->pipeSock_ = NAI_FD_INVALID;
    }
    return true;
}
bool RemoteSinker::_isLogdRunning() noexcept
{
    LOGVERBOSE(__func__) << " RemoteSinker enter ";
    std::string const &procPath = "/proc";
    std::string const &targetName{"logd"};

    nai_dir_t dir;
    nai_dir_init(&dir);
    if (nai_dir_open(&dir, procPath.c_str()) < 0) {
        return false;
    }

    while (nai_dir_read(&dir) >= 0) {
        if (_isNumericDir(dir)) {
            std::string path{"/proc/" + std::string(dir.name) + "/comm"};
            std::ifstream file(path);
            if (file) {
                std::string cmdline;
                if (std::getline(file, cmdline)) {
                    if (cmdline == targetName) {
                        std::ignore = nai_dir_close(&dir);
                        return true;
                    }
                }
            }
        }
    }

    std::ignore = nai_dir_close(&dir);
    LOGVERBOSE(__func__) << " RemoteSinker leave ";
    return false;
}
/// @brief
void RemoteSinker::_destroy() noexcept
{
    if (this->pipeSock_ != NAI_FD_INVALID) {
        std::ignore     = nai_file_close(pipeSock_);
        this->pipeSock_ = NAI_FD_INVALID;
    }
    backendReady_ = false;
    initFailed_   = false;
    mStopSending_ = false;
    dropCount_    = 0;
    if (this->buffer_ != nullptr) {
        this->buffer_ = nullptr;
    }
}
/// @brief
void RemoteSinker::_sendBuffer() noexcept
{
    if (this->FifoOK() == false || this->mStopSending_) {
        return;
    }
    bool const checkBuffer{buffer_->Len() > 0U};
    if ((buffer_ != nullptr) && checkBuffer) {
        ssize_t dataSend{0};
        std::uint8_t *const pSend{buffer_->Data()};
        ssize_t const alldataLen{static_cast< ssize_t >(buffer_->Len())};
        while (dataSend < alldataLen) {
            ssize_t const ret{
                nai_file_write(pipeSock_, pSend + dataSend, static_cast< size_t >(alldataLen - dataSend))};
            LOGVERBOSE(__func__) << " RemoteSinker send ret " << ret;
            if (ret < 0) {
                break;
            }
            dataSend += ret;
        }
        buffer_->Clear();
    }
}

bool RemoteSinker::_tryInitIfNeeded(bool force) noexcept
{
    if (mStopSending_) {
        return false;
    }
    std::chrono::steady_clock::time_point const now = std::chrono::steady_clock::now();
    if (!force) {
        std::chrono::steady_clock::duration const elapsed = now - lastInitTry_;
        if (elapsed < reconnectInterval_) {
            return backendReady_;
        }
    }
    lastInitTry_           = now;
    std::int32_t const ret = Init();
    return (ret >= 0 && backendReady_);
}

}  // namespace internal
}  // namespace log
}  // namespace ara
