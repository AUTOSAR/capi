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
/// @file       semaphore_lock.h
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
/// @unit_name = semaphore_lock
/// @unit_description=Dlt lib's implemented log sinker, for locking when writing to files
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_SYS_SemaphoreLock__
#define __LOG_INTERNAL_SYS_SemaphoreLock__
// clang-format off
#include <cstdint>
#include <semaphore.h>
#include <string>
#include <sys/sem.h>
// clang-format on
namespace ara {
namespace log {
namespace internal {
/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00323
/// @trace_id_dd=DD_LOG_01719
/// @needwork = ad
/// @endcode
using Sem_T_Type = sem_t;
/// @brief Implementation of inter-process synchronization using semaphores
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00188
/// @trace_id_dd=DD_LOG_00350
/// @needwork = ad
/// @endcode
class SemaphoreLock
{
public:
    enum class SemaphoreError
    {
        kSUCCESS             = 0,
        kALREADY_INITIALIZED = -1,
        kCREATE_FAILED       = -2,
        kOPEN_FAILED         = -3,
        kNOT_INITIALIZED     = -4
    };
    struct SemaphoreConfig
    {
        std::string baseName{"_tmp_dltlog_internal_processSemLocker"};
        bool autoCleanup{true};
        int initialValue{1};
    };
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00189
    /// @trace_id_dd=DD_LOG_00351
    /// @needwork = ad
    /// @endcode
    SemaphoreLock() = default;
    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00351
    /// @trace_id_dd=DD_LOG_01750
    /// @needwork = ad
    /// @endcode
    SemaphoreLock(SemaphoreLock const& other) = default;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00352
    /// @trace_id_dd=DD_LOG_01751
    /// @needwork = ad
    /// @endcode
    SemaphoreLock& operator=(SemaphoreLock const& other) = default;

    /// @needwork = ad
    SemaphoreLock(SemaphoreLock&& other) = default;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00352
    /// @trace_id_dd=DD_LOG_01751
    /// @needwork = ad
    /// @endcode
    SemaphoreLock& operator=(SemaphoreLock&& other) = default;
    /// @brief Destructor
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00190
    /// @trace_id_dd=DD_LOG_00352
    /// @needwork = ad
    /// @endcode
    ~SemaphoreLock() noexcept;

    /// @brief Initialize
    /// @param[in]  pathName Unique path tag
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00191
    /// @trace_id_dd=DD_LOG_00353
    /// @needwork = ad
    /// @endcode
    std::int32_t Init(std::string const& pathName) noexcept;
    /// @brief Acquire the mutex lock. If the lock is already held by another process, the current process will block until it can acquire the lock.
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00192
    /// @trace_id_dd=DD_LOG_00354
    /// @needwork = ad
    /// @endcode
    void Lock() noexcept;

    /// @brief Release the mutex lock
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00193
    /// @trace_id_dd=DD_LOG_00355
    /// @needwork = ad
    /// @endcode
    void UnLock() noexcept;
    /// @brief Release internal resources
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00194
    /// @trace_id_dd=DD_LOG_00356
    /// @needwork = ad
    /// @endcode
    void Destroy() noexcept;
    /// @brief Current value of the semaphore
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00359
    /// @needwork = dda
    /// @endcode
    std::int32_t CurrentValue() noexcept;

    bool IsInitialized() const noexcept { return this->initOk_; }
    const std::string& GetSemaphoreName() const noexcept { return this->semName_; }
    bool IsFirstOwner() const noexcept { return this->first_; }

private:
    /// @brief Decrement the semaphore value by one, and block until the semaphore value is greater than zero
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00357
    /// @needwork = dda
    /// @endcode
    void _wait() noexcept;
    /// @brief Increment the semaphore value by one, waking up a process or thread waiting on this semaphore
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00358
    /// @needwork = dda
    /// @endcode
    void _post() noexcept;

private:
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00360
    /// @needwork = dda
    /// @endcode
    sem_t* semMutex_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00361
    /// @needwork = dda
    /// @endcode
    std::string semName_{"_tmp_dltlog_internal_processSemLocker"};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00362
    /// @needwork = dda
    /// @endcode
    bool first_{false};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00188
    /// @trace_id_dd=DD_LOG_00363
    /// @needwork = dda
    /// @endcode
    bool initOk_{false};
};

}  // namespace internal
}  // namespace log

}  // namespace ara
#endif