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
/// @file       ipc_shared_state.cpp
/// @brief      IPC helper for cross-process shared state (POSIX shm + mmap).
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "ipc_shared_state.h"

#include <isoft/ara_fsh/platform.h>
#include <nai/os/nai_dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>

#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {

namespace {
constexpr char kFileSinkerShmPrefix[] = "/ara_log_LT_INTERNAL_FILESINKER_INDEX_MEME_I";
}

bool IpcSharedState::OpenForFile(std::string const& uniqStr, std::string const& logDir) noexcept
{
    std::string const fullKey{std::string{kFileSinkerShmPrefix} + isoft::ara_fsh::Platform().GetSysrootMd5Str()
                              + uniqStr};
    return OpenShm(fullKey, logDir);
}

bool IpcSharedState::OpenShm(std::string const& fullKey, std::string const& logDir) noexcept
{
    if (IsOpen()) {
        return true;
    }

    path_ = fullKey;
    fd_   = shm_open(path_.c_str(), O_RDWR, 0U);
    if (fd_ == -1) {
        // Fix: only attempt to create when the error is ENOENT
        if (errno == ENOENT) {
            fd_    = shm_open(path_.c_str(), O_CREAT | O_RDWR | O_EXCL, 0600U);
            first_ = (fd_ >= 0);
        }
        // If creation fails (e.g., already exists), try to open again
        if (fd_ < 0) {
            fd_    = shm_open(path_.c_str(), O_RDWR, 0U);
            first_ = false;
        }
    }

    if (fd_ < 0) {
        return false;
    }

    if (ftruncate(fd_, static_cast< off_t >(sizeof(LogShareData))) != 0) {
        close(fd_);
        fd_ = -1;
        if (this->first_) {
            std::int32_t ret{shm_unlink(path_.c_str())};
            if (ret != 0) {
                LOGERROR(__func__) << "shm_unlink failed: " << path_ << ", ret: " << ret << ", errno: " << errno;
            }
        }
        return false;
    }

    void* addr = mmap(nullptr, sizeof(LogShareData), PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (addr == MAP_FAILED) {
        close(fd_);
        fd_ = -1;
        if (this->first_) {
            std::int32_t ret{shm_unlink(path_.c_str())};
            if (ret != 0) {
                LOGERROR(__func__) << "shm_unlink failed: " << path_ << ", ret: " << ret << ", errno: " << errno;
            }
        }
        return false;
    }

    data_ = static_cast< LogShareData* >(addr);
    if (first_) {
        *data_ = local_;
        nai_stat_t st;
        nai_memset(&st, 0, sizeof(st));
        nai_fd_t dirFd = nai_file_open(logDir.c_str(), NAI_O_RDONLY | NAI_O_DIRECTORY, 0);
        if (dirFd == NAI_FD_INVALID || nai_file_stat(dirFd, &st, NAI_STAT_BASIC) < 0) {
            if (dirFd != NAI_FD_INVALID) {
                nai_file_close(dirFd);
            }
            if (nai_dir_create(logDir.c_str(), 0755, 1) < 0) {
                LOGERROR(__func__) << "Failed to create log directory: " << logDir << ", errno: " << errno;
            }
        } else {
            nai_file_close(dirFd);
        }
    } else {
        local_ = *data_;
    }

    return true;
}

void IpcSharedState::CloseShm() noexcept
{
    if (data_ != nullptr) {
        if (munmap(data_, sizeof(LogShareData)) != 0) {
            LOGERROR(__func__) << "munmap failed, errno: " << errno;
        }
        data_ = nullptr;
    }
    if (fd_ >= 0) {
        if (close(fd_) != 0) {
            LOGERROR(__func__) << "close failed, errno: " << errno;
        }
        fd_ = -1;
    }
    if (!path_.empty() && this->first_) {
        std::int32_t ret{shm_unlink(path_.c_str())};
        if (ret != 0) {
            LOGERROR(__func__) << "shm_unlink failed: " << path_ << ", ret: " << ret << ", errno: " << errno;
        }
        path_.clear();
    }
}

bool IpcSharedState::IsChanged() const noexcept { return (data_ != nullptr) && (local_.index != data_->index); }

}  // namespace internal
}  // namespace log
}  // namespace ara
