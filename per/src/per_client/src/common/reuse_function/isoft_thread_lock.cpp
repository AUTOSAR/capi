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
/// @file       isoft_thread_lock.cpp
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

#include "ara/per/internal/common/isoft_thread_lock.h"

#include <nai/os/nai_thread.h>

#include <cstring>

#include "ara/per/internal/common/isoft_assert.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
PNaiLock::PNaiLock() noexcept
{
    std::ignore = nai_memset(static_cast< void* >(&naiMutex_), 0, sizeof(naiMutex_));  // nai_mutex_init
    std::ignore = nai_mutex_open(&naiMutex_, 0);
}
/// @brief
PNaiLock::~PNaiLock() noexcept { std::ignore = nai_mutex_close(&naiMutex_); }
/// @brief
/// @return
bool PNaiLock::Lock() noexcept { return nai_mutex_lock(&naiMutex_) >= 0; }
/// @brief
/// @return
bool PNaiLock::TryLock() noexcept { return nai_mutex_trylock(&naiMutex_) >= 0; }
/// @brief
/// @return
bool PNaiLock::UnLock() noexcept { return nai_mutex_unlock(&naiMutex_) >= 0; }
//********************************/
/// @brief
/// @param mutexAttr
PThreadLock::PThreadLock(pthread_mutexattr_t const& mutexAttr) noexcept
{
    PER_Assert(0 == pthread_mutex_init(&threadMutex_, &mutexAttr));
}
/// @brief
PThreadLock::PThreadLock() noexcept
{  // Use default attributes
    PER_Assert(0 == pthread_mutex_init(&threadMutex_, nullptr));
}
/// @brief
PThreadLock::~PThreadLock() noexcept { std::ignore = pthread_mutex_destroy(&threadMutex_); }
/// @brief
/// @return
bool PThreadLock::Lock() noexcept { return 0 == pthread_mutex_lock(&threadMutex_); }
/// @brief
/// @return
bool PThreadLock::TryLock() noexcept { return 0 == pthread_mutex_trylock(&threadMutex_); }
/// @brief
/// @return
bool PThreadLock::UnLock() noexcept { return 0 == pthread_mutex_unlock(&threadMutex_); }
//********************************/
/// @brief
/// @param threadLock
PAutoLock::PAutoLock(PThreadLock& threadLock) noexcept : threadLock_{threadLock} { std::ignore = threadLock_.Lock(); }
/// @brief
PAutoLock::~PAutoLock() noexcept { std::ignore = threadLock_.UnLock(); }
//********************************/
/// @brief
/// @param rwlockAttr
PThreadLockRW::PThreadLockRW(pthread_rwlockattr_t const& rwlockAttr) noexcept
{
    PER_Assert(0 == pthread_rwlock_init(&pthreadRwLock_, &rwlockAttr));
}
/// @brief
PThreadLockRW::PThreadLockRW() noexcept
{
    // Use default read-write lock attributes
    PER_Assert(0 == pthread_rwlock_init(&pthreadRwLock_, nullptr));
}
/// @brief
PThreadLockRW::~PThreadLockRW() noexcept { std::ignore = pthread_rwlock_destroy(&pthreadRwLock_); }
/// @brief
/// @return
bool PThreadLockRW::LockRead() noexcept { return 0 == pthread_rwlock_rdlock(&pthreadRwLock_); }
/// @brief
/// @return
bool PThreadLockRW::TryLockRead() noexcept { return 0 == pthread_rwlock_tryrdlock(&pthreadRwLock_); }
/// @brief
/// @return
bool PThreadLockRW::LockWrite() noexcept
{
    if (pthread_rwlock_wrlock(&pthreadRwLock_) < 0) {
        return false;
    }
    nLoopCount_ += 1U;
    return true;
}
/// @brief
/// @return
bool PThreadLockRW::TryLockWrite() noexcept { return 0 == pthread_rwlock_trywrlock(&pthreadRwLock_); }
/// @brief
/// @return
bool PThreadLockRW::UnLockRead() noexcept { return 0 == pthread_rwlock_unlock(&pthreadRwLock_); }
/// @brief
/// @return
bool PThreadLockRW::UnLockWrite() noexcept
{
    nLoopCount_ -= 1U;
    if (nLoopCount_ > 0U) {
        return true;
    }
    return 0 == pthread_rwlock_unlock(&pthreadRwLock_);
}
//********************************/
/// @brief
/// @param threadLock
PAutoLockRead::PAutoLockRead(PThreadLockRW& threadLock) noexcept : threadLock_{threadLock}, bLockSuccess_{false}
{
    bLockSuccess_ = threadLock_.LockRead();
}
/// @brief
PAutoLockRead::~PAutoLockRead() noexcept
{
    if (bLockSuccess_) {
        std::ignore = threadLock_.UnLockRead();
    }
}
//********************************/
/// @brief
/// @param threadLock
PAutoLockWrite::PAutoLockWrite(PThreadLockRW& threadLock) noexcept : threadLock_{threadLock}, bLockSuccess_{false}
{
    // Write lock cannot be re-entered within the same thread, otherwise UnLock will mess up internal counter pthreadRwLock_.__align
    bLockSuccess_ = threadLock_.LockWrite();
}
/// @brief
PAutoLockWrite::~PAutoLockWrite() noexcept
{
    if (bLockSuccess_) {
        std::ignore = threadLock_.UnLockWrite();
    }
}
//********************************/
/// @brief
/// @param condAttr
PThreadCond::PThreadCond(pthread_condattr_t const& condAttr) noexcept
{
    PER_Assert(0 == pthread_cond_init(&threadCond_, &condAttr));
}
/// @brief
PThreadCond::PThreadCond() noexcept
// : g_threadCond_(PTHREAD_COND_INITIALIZER) // Static initialization of condition variable
{
    // Use default attributes
    PER_Assert(0 == pthread_cond_init(&threadCond_, nullptr));
}
/// @brief
PThreadCond::~PThreadCond() noexcept { std::ignore = pthread_cond_destroy(&threadCond_); }
/// @brief Blocking wait
/// @param threadLock
/// @return
bool PThreadCond::Wait(PThreadLock& threadLock) noexcept
{
    return 0 == pthread_cond_wait(&threadCond_, threadLock.GetMutexPtr());
}
/// @brief Blocking wait with timeout
/// @param threadLock
/// @param tmData
/// @return
bool PThreadCond::TimedWait(PThreadLock& threadLock, struct timespec const* const tmData) noexcept
{
    return 0 == pthread_cond_timedwait(&threadCond_, threadLock.GetMutexPtr(), tmData);
}
/// @brief Send signal
/// @return
bool PThreadCond::Signal() noexcept { return 0 == pthread_cond_signal(&threadCond_); }
/// @brief Broadcast signal
/// @return
bool PThreadCond::Broadcast() noexcept { return 0 == pthread_cond_broadcast(&threadCond_); }
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara