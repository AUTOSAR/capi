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
/// @file       ipc_shared_state.h
/// @brief      IPC helper for cross-process shared state (POSIX shm + mmap).
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_IPC_SHARED_STATE__
#define __LOG_INTERNAL_IPC_SHARED_STATE__

#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ara {
namespace log {
namespace internal {

struct LogShareData
{
    std::size_t index{0};
};

class IpcSharedState
{
public:
    IpcSharedState() = default;
    ~IpcSharedState() { CloseShm(); }

    // fullKey: fully derived shm name (e.g. includes prefix + md5 + user key)
    // logDir: directory to ensure existence when first creating the shared segment
    bool OpenShm(std::string const& fullKey, std::string const& logDir) noexcept;
    void CloseShm() noexcept;

    bool IsOpen() const noexcept { return data_ != nullptr; }

    LogShareData* Data() noexcept { return data_; }
    LogShareData const* Data() const noexcept { return data_; }

    // Local snapshot helpers
    void SetLocal(LogShareData const& v) noexcept { local_ = v; }
    LogShareData& Local() noexcept { return local_; }
    LogShareData const& Local() const noexcept { return local_; }
    void SyncLocalFromShared() noexcept
    {
        if (data_ != nullptr) {
            local_ = *data_;
        }
    }

    void SyncSharedFromLocal() noexcept
    {
        if (data_ != nullptr) {
            *data_ = local_;
        }
    }
    std::size_t GetIndex() const noexcept { return local_.index; }

    std::size_t IncrementIndex() noexcept
    {
        ++local_.index;
        SyncSharedFromLocal();
        return local_.index;
    }

    // Derive the shared-memory key for file sinkers and open the segment.
    bool OpenForFile(std::string const& uniqStr, std::string const& logDir) noexcept;

    bool IsChanged() const noexcept;

    // Write back local snapshot into shared block.
    void PublishLocal() noexcept
    {
        if (data_ != nullptr) {
            *data_ = local_;
        }
    }

private:
    int fd_{-1};
    LogShareData* data_{nullptr};
    std::string path_{};
    std::atomic< bool > first_{false};
    LogShareData local_{};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif
