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
/// @file       processlock.h
/// @brief
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_PROCESSLOCK_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_PROCESSLOCK_H_
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

#include <cstdint>
#include <memory>
#include <string>

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {
/// @brief process lock
class ProcessMutex final
{
public:
    /// @brief construct
    /// @param mutexRef
    explicit ProcessMutex(pthread_mutex_t &mutexRef) noexcept : mutex_{mutexRef}
    {
        std::ignore = pthread_mutex_lock(&(mutex_));
    };
    /// @brief destruct
    ~ProcessMutex() noexcept { std::ignore = pthread_mutex_unlock(&(mutex_)); };

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    /// TODO(zhoubo): copy assignment operator is explicitly defaulted but implicitly deleted
    ProcessMutex &operator=(ProcessMutex const &other) = delete;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    /// TODO(zhoubo): move assignment operator is explicitly defaulted but implicitly deleted
    ProcessMutex &operator=(ProcessMutex &&other) = delete;

    /// @brief copy constructor
    /// @param other - other object
    ProcessMutex(ProcessMutex const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    ProcessMutex(ProcessMutex &&) noexcept = default;

private:
    /// @brief mutex_
    pthread_mutex_t &mutex_;
};

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_PROCESSLOCK_H_