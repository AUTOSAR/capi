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
/// @brief      AutoSar-AP data persistent storage module
/// @details
/// @date       2021-05-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/ReuseFunction/ReuseFunctionModule
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_thread_lock.h"

#include <ara/core/string.h>
#include <nai/os/nai_thread.h>

#include <cstring>

#include "ara/crypto/common/isoft_assert.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief Constructor
PNaiLock::PNaiLock() noexcept : naiMutex_{}
{
    std::ignore = nai_memset(static_cast< void* >(&naiMutex_), 0, sizeof(naiMutex_));  // nai_mutex_init
    std::ignore = nai_mutex_open(&naiMutex_, 0);
}
/// @brief Parameterized constructor
/// @param naiMutex nai mutex semaphore
PNaiLock::PNaiLock(nai_mutex_t const naiMutex) noexcept : naiMutex_{naiMutex} {}
/// @brief Destructor
PNaiLock::~PNaiLock() noexcept { std::ignore = nai_mutex_close(&naiMutex_); }
/// @brief Lock
/// @return ture if lock false otherwise
bool PNaiLock::Lock() noexcept { return nai_mutex_lock(&naiMutex_) >= 0; }
/// @brief Try to unlock
/// @return ture if TryLock false otherwise
bool PNaiLock::TryLock() noexcept { return nai_mutex_trylock(&naiMutex_) >= 0; }
/// @brief Unlock
/// @return ture if UnLock false otherwise
bool PNaiLock::UnLock() noexcept { return nai_mutex_unlock(&naiMutex_) >= 0; }
//********************************/
/// @brief Parameterized constructor
/// @param mutexAttr mutex semaphore
PThreadLock::PThreadLock(pthread_mutexattr_t const& mutexAttr) noexcept : threadMutex_{}
{
    PH_ASSERT(0 == pthread_mutex_init(&threadMutex_, &mutexAttr));
}
/// @brief Constructor
PThreadLock::PThreadLock() noexcept : threadMutex_{}
{  // Use default attributes
    PH_ASSERT(0 == pthread_mutex_init(&threadMutex_, nullptr));
}
/// @brief Destructor
PThreadLock::~PThreadLock() noexcept { std::ignore = pthread_mutex_destroy(&threadMutex_); }
/// @brief Lock
/// @return ture if lock false otherwise
bool PThreadLock::Lock() noexcept { return 0 == pthread_mutex_lock(&threadMutex_); }
/// @brief Try to unlock
/// @return ture if TryLock false otherwise
bool PThreadLock::TryLock() noexcept { return 0 == pthread_mutex_trylock(&threadMutex_); }
/// @brief Unlock
/// @return ture if UnLock false otherwise
bool PThreadLock::UnLock() noexcept { return 0 == pthread_mutex_unlock(&threadMutex_); }
//********************************/
/// @brief Parameterized constructor
/// @param threadLock thread lock
PAutoLock::PAutoLock(PThreadLock& threadLock) noexcept : threadLock_{threadLock} { std::ignore = threadLock_.Lock(); }
/// @brief Destructor
PAutoLock::~PAutoLock() noexcept { std::ignore = threadLock_.UnLock(); }
//********************************/
/// @brief Parameterized constructor
/// @param rwlockAttr read-write lock attributes
PThreadLockRW::PThreadLockRW(pthread_rwlockattr_t const& rwlockAttr) noexcept : pthreadRwLock_{}, nLoopCount_{0U}
{
    PH_ASSERT(0 == pthread_rwlock_init(&pthreadRwLock_, &rwlockAttr));
}
/// @brief Parameterized constructor
PThreadLockRW::PThreadLockRW(uint32_t const nLoopCount) noexcept : pthreadRwLock_{}, nLoopCount_{nLoopCount}
{
    // Use default read-write lock attributes
    PH_ASSERT(0 == pthread_rwlock_init(&pthreadRwLock_, nullptr));
}
/// @brief Destructor
PThreadLockRW::~PThreadLockRW() noexcept { std::ignore = pthread_rwlock_destroy(&pthreadRwLock_); }
/// @brief Lock: read
/// @return ture if LockRead false otherwise
bool PThreadLockRW::LockRead() noexcept { return 0 == pthread_rwlock_rdlock(&pthreadRwLock_); }
/// @brief Try to unlock: read
/// @return ture if TryLockRead false otherwise
bool PThreadLockRW::TryLockRead() noexcept { return 0 == pthread_rwlock_tryrdlock(&pthreadRwLock_); }
/// @brief Lock: write
/// @return ture if LockWrite false otherwise
bool PThreadLockRW::LockWrite() noexcept
{
    if (pthread_rwlock_wrlock(&pthreadRwLock_) < 0) {
        return false;
    }
    nLoopCount_ += 1U;
    return true;
}
/// @brief Try to unlock: write
/// @return ture if TryLockWrite false otherwise
bool PThreadLockRW::TryLockWrite() noexcept { return 0 == pthread_rwlock_trywrlock(&pthreadRwLock_); }
/// @brief Unlock: read
/// @return ture if UnLockRead false otherwise
bool PThreadLockRW::UnLockRead() noexcept { return 0 == pthread_rwlock_unlock(&pthreadRwLock_); }
/// @brief Unlock: write
/// @return ture if UnLockWrite false otherwise
bool PThreadLockRW::UnLockWrite() noexcept
{
    nLoopCount_ -= 1U;
    if (nLoopCount_ > 0U) {
        return true;
    }
    return 0 == pthread_rwlock_unlock(&pthreadRwLock_);
}
//********************************/
/// @brief Parameterized constructor
/// @param threadLock thread lock
PAutoLockRead::PAutoLockRead(PThreadLockRW& threadLock) noexcept : threadLock_{threadLock}, bLockSuccess_{false}
{
    bLockSuccess_ = threadLock_.LockRead();
}
/// @brief Destructor
PAutoLockRead::~PAutoLockRead() noexcept
{
    if (bLockSuccess_) {
        std::ignore = threadLock_.UnLockRead();
    }
}
//********************************/
/// @brief Parameterized constructor
/// @param threadLock thread lock
PAutoLockWrite::PAutoLockWrite(PThreadLockRW& threadLock) noexcept : threadLock_{threadLock}, bLockSuccess_{false}
{
    // Write lock cannot be re-entered within the same thread, otherwise UnLock will affect the internal counter pthreadRwLock_.__align
    bLockSuccess_ = threadLock_.LockWrite();
}
/// @brief Destructor
PAutoLockWrite::~PAutoLockWrite() noexcept
{
    if (bLockSuccess_) {
        std::ignore = threadLock_.UnLockWrite();
    }
}
//********************************/
/// @brief Parameterized constructor
/// @param condAttr condition variable attributes
PThreadCond::PThreadCond(pthread_condattr_t const& condAttr) noexcept : threadCond_{}
{
    PH_ASSERT(0 == pthread_cond_init(&threadCond_, &condAttr));
}
/// @brief Constructor
PThreadCond::PThreadCond() noexcept : threadCond_{}
{
    // Use default attributes
    PH_ASSERT(0 == pthread_cond_init(&threadCond_, nullptr));
}
/// @brief Destructor
PThreadCond::~PThreadCond() noexcept { std::ignore = pthread_cond_destroy(&threadCond_); }
/// @brief Blocking wait
/// @param threadLock thread lock
/// @return ture if Wait false otherwise
bool PThreadCond::Wait(PThreadLock& threadLock) noexcept
{
    return 0 == pthread_cond_wait(&threadCond_, threadLock.GetMutexPtr());
}
/// @brief Blocking wait with timeout
/// @param threadLock thread lock
/// @param tmData timeout
/// @return ture if TimedWait false otherwise
bool PThreadCond::TimedWait(PThreadLock& threadLock, struct timespec const* const tmData) noexcept
{
    return 0 == pthread_cond_timedwait(&threadCond_, threadLock.GetMutexPtr(), tmData);
}
/// @brief Send signal
/// @return ture if Signal false otherwise
bool PThreadCond::Signal() noexcept { return 0 == pthread_cond_signal(&threadCond_); }
/// @brief Broadcast signal
/// @return ture if Broadcast false otherwise
bool PThreadCond::Broadcast() noexcept { return 0 == pthread_cond_broadcast(&threadCond_); }
//********************************/
}  // namespace crypto
}  // namespace ara
