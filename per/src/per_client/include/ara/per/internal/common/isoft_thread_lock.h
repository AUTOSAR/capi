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
/// @file       isoft_thread_lock.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Puhua thread lock encapsulation class
/// @date       2021-05-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-20  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_THREAD_LOCK_H_
#define ARA_PER_COMMON_PH_THREAD_LOCK_H_

#include <pthread.h>

#include "nai/os/nai_thread.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @code{.isoft}
/// @unit_name=PNaiLock
/// @endcode
class PNaiLock final
{
private:
    /// @brief
    nai_mutex_t naiMutex_{};

public:
    /// @brief
    PNaiLock() noexcept;
    ///  @brief
    ~PNaiLock() noexcept;
    /// @brief
    /// @param a
    PNaiLock(PNaiLock const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PNaiLock &operator=(PNaiLock const &a) = delete;
    /// @brief
    /// @param a
    PNaiLock(PNaiLock &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PNaiLock &operator=(PNaiLock &&a) = delete;

public:
    /// @brief
    /// @return
    bool Lock() noexcept;
    /// @brief
    /// @return
    bool TryLock() noexcept;
    /// @brief
    /// @return
    bool UnLock() noexcept;
};
//********************************/

/// @brief Mutex lock encapsulation
/// @code{.isoft}
/// @unit_name=PThreadLock
/// @endcode
class PThreadLock final
{
private:
    /// @brief
    pthread_mutex_t threadMutex_{};

public:
    /// @brief
    /// @param mutexAttr
    explicit PThreadLock(pthread_mutexattr_t const &mutexAttr) noexcept;
    /// @brief
    PThreadLock() noexcept;
    /// @brief
    ~PThreadLock() noexcept;
    /// @brief
    /// @param  a
    PThreadLock(PThreadLock const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PThreadLock &operator=(PThreadLock const &a) = delete;
    /// @brief
    /// @param  a
    PThreadLock(PThreadLock &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PThreadLock &operator=(PThreadLock &&a) = delete;

public:
    /// @brief
    /// @return
    bool Lock() noexcept;
    /// @brief
    /// @return
    bool TryLock() noexcept;
    /// @brief
    /// @return
    bool UnLock() noexcept;
    /// @brief
    /// @return
    inline pthread_mutex_t *GetMutexPtr() noexcept { return &threadMutex_; }
};
//***************/

/// @brief
/// @code{.isoft}
/// @unit_name=PAutoLock
/// @endcode
class PAutoLock final
{
private:
    /// @brief
    PThreadLock &threadLock_;

public:
    /// @brief
    /// @param threadLock
    explicit PAutoLock(PThreadLock &threadLock) noexcept;
    /// @brief
    ~PAutoLock() noexcept;
    /// @brief
    /// @param  a
    PAutoLock(PAutoLock const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLock &operator=(PAutoLock const &a) = delete;
    /// @brief
    /// @param  a
    PAutoLock(PAutoLock &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLock &operator=(PAutoLock &&a) = delete;

public:
};
//********************************/

/// @brief Read-write lock encapsulation
/// @code{.isoft}
/// @unit_name=PThreadLockRW
/// @endcode
class PThreadLockRW final
{
private:
    /// @brief QAC issue here is due to NAI internal implementation
    pthread_rwlock_t pthreadRwLock_{};
    /// @brief Number of repeated write locks within the same thread
    uint32_t nLoopCount_{0U};

public:
    /// @brief
    /// @param rwlockAttr
    explicit PThreadLockRW(pthread_rwlockattr_t const &rwlockAttr) noexcept;
    /// @brief
    PThreadLockRW() noexcept;
    /// @brief
    ~PThreadLockRW() noexcept;
    /// @brief
    /// @param  a
    PThreadLockRW(PThreadLockRW const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PThreadLockRW &operator=(PThreadLockRW const &a) = delete;
    /// @brief
    /// @param  a
    PThreadLockRW(PThreadLockRW &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PThreadLockRW &operator=(PThreadLockRW &&a) = delete;

public:
    /// @brief
    /// @return
    bool LockRead() noexcept;
    /// @brief
    /// @return
    bool TryLockRead() noexcept;
    /// @brief
    /// @return
    bool LockWrite() noexcept;
    /// @brief
    /// @return
    bool TryLockWrite() noexcept;
    /// @brief
    /// @return
    bool UnLockRead() noexcept;
    /// @brief
    /// @return
    bool UnLockWrite() noexcept;
};
//***************/

/// @brief Read lock
/// @code{.isoft}
/// @unit_name=PAutoLockRead
/// @endcode
class PAutoLockRead final
{
private:
    /// @brief Thread lock
    PThreadLockRW &threadLock_;
    /// @brief Whether lock was successful
    bool bLockSuccess_;

public:
    /// @brief
    /// @param threadLock
    explicit PAutoLockRead(PThreadLockRW &threadLock) noexcept;
    /// @brief
    PAutoLockRead() noexcept = delete;
    /// @brief
    ~PAutoLockRead() noexcept;
    /// @brief
    /// @param  a
    PAutoLockRead(PAutoLockRead const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLockRead &operator=(PAutoLockRead const &a) = delete;
    /// @brief
    /// @param  a
    PAutoLockRead(PAutoLockRead &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLockRead &operator=(PAutoLockRead &&a) = delete;
};
//***************/

/// @brief Write lock
/// @code{.isoft}
/// @unit_name=PAutoLockWrite
/// @endcode
class PAutoLockWrite final
{
private:
    /// @brief Thread lock
    PThreadLockRW &threadLock_;
    /// @brief Whether lock was successful
    bool bLockSuccess_;

public:
    /// @brief
    /// @param threadLock
    explicit PAutoLockWrite(PThreadLockRW &threadLock) noexcept;
    /// @brief
    PAutoLockWrite() noexcept = delete;
    /// @brief
    ~PAutoLockWrite() noexcept;
    /// @brief
    /// @param  a
    PAutoLockWrite(PAutoLockWrite const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLockWrite &operator=(PAutoLockWrite const &a) = delete;
    /// @brief
    /// @param  a
    PAutoLockWrite(PAutoLockWrite &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PAutoLockWrite &operator=(PAutoLockWrite &&a) = delete;
};
//********************************/
/// @brief Condition variable
/// @code{.isoft}
/// @unit_name=PThreadCond
/// @endcode
class PThreadCond final
{
private:
    /// @brief Condition variable
    pthread_cond_t threadCond_{};

public:
    /// @brief
    /// @param condAttr
    explicit PThreadCond(pthread_condattr_t const &condAttr) noexcept;
    /// @brief
    PThreadCond() noexcept;
    /// @brief
    ~PThreadCond() noexcept;
    /// @brief
    /// @param a
    PThreadCond(PThreadCond const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PThreadCond &operator=(PThreadCond const &a) = delete;
    /// @brief
    /// @param a
    PThreadCond(PThreadCond &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PThreadCond &operator=(PThreadCond &&a) = delete;

public:
    /// @brief Blocking wait
    /// @param threadLock
    /// @return
    bool Wait(PThreadLock &threadLock) noexcept;

    /// @brief Blocking wait with timeout
    /// @param threadLock
    /// @param tmData
    /// @return
    bool TimedWait(PThreadLock &threadLock, struct timespec const *const tmData) noexcept;
    /// @brief Send signal
    /// @return
    bool Signal() noexcept;
    /// @brief Broadcast signal
    /// @return
    bool Broadcast() noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
