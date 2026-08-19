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
/// @brief      AutoSar-AP data persistent storage module
/// @details    Puhua thread lock wrapper class
/// @date       2021-05-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_COMMON_PH_THREAD_LOCK_H_
#define ARA_CRYPTO_COMMON_PH_THREAD_LOCK_H_

#include <pthread.h>

#include "nai/os/nai_thread.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief Wrapper for NAI thread lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00009
/// @trace_id_dd=DD_CRYPTO_00248
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PNaiLock final
{
private:
    /// @brief Mutex semaphore provided by NAI
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00249
    /// @needwork = dda
    /// @endcode
    nai_mutex_t naiMutex_{};

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00250
    /// @needwork = dda
    /// @endcode
    PNaiLock() noexcept;
    /// @brief Parameterized constructor
    /// @param naiMutex NAI mutex semaphore
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00251
    /// @needwork = dda
    /// @endcode
    explicit PNaiLock(nai_mutex_t const naiMutex) noexcept;
    /// @code{.isoft}
    ///  @brief Destructor
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00252
    /// @needwork = dda
    /// @endcode
    ~PNaiLock() noexcept;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00253
    /// @needwork = dda
    /// @endcode
    PNaiLock(PNaiLock const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00254
    /// @needwork = dda
    /// @endcode
    PNaiLock &operator=(PNaiLock const &a) = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00255
    /// @needwork = dda
    /// @endcode
    PNaiLock(PNaiLock &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00256
    /// @needwork = dda
    /// @endcode
    PNaiLock &operator=(PNaiLock &&a) = delete;

public:
    /// @brief Lock
    /// @return ture if lock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00257
    /// @needwork = dda
    /// @endcode
    bool Lock() noexcept;
    /// @brief Try to unlock
    /// @return ture if try lock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00258
    /// @needwork = dda
    /// @endcode
    bool TryLock() noexcept;
    /// @brief Unlock
    /// @return ture if try UnLock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00259
    /// @needwork = dda
    /// @endcode
    bool UnLock() noexcept;
};
//********************************/
/// @brief Wrapper for pthread mutex lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00010
/// @trace_id_dd=DD_CRYPTO_00260
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PThreadLock final
{
private:
    /// @brief Mutex semaphore
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00261
    /// @needwork = dda
    /// @endcode
    pthread_mutex_t threadMutex_{};

public:
    /// @brief Parameterized constructor
    /// @param mutexAttr Mutex semaphore
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00262
    /// @needwork = dda
    /// @endcode
    explicit PThreadLock(pthread_mutexattr_t const &mutexAttr) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00263
    /// @needwork = dda
    /// @endcode
    PThreadLock() noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00264
    /// @needwork = dda
    /// @endcode
    ~PThreadLock() noexcept;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00265
    /// @needwork = dda
    /// @endcode
    PThreadLock(PThreadLock const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00266
    /// @needwork = dda
    /// @endcode
    PThreadLock &operator=(PThreadLock const &a) = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00267
    /// @needwork = dda
    /// @endcode
    PThreadLock(PThreadLock &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00268
    /// @needwork = dda
    /// @endcode
    PThreadLock &operator=(PThreadLock &&a) = delete;

public:
    /// @brief Lock
    /// @return ture if lock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00269
    /// @needwork = dda
    /// @endcode
    bool Lock() noexcept;
    /// @brief Try to unlock
    /// @return ture if TryLock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00270
    /// @needwork = dda
    /// @endcode
    bool TryLock() noexcept;
    /// @brief Unlock
    /// @return ture if UnLock false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00271
    /// @needwork = dda
    /// @endcode
    bool UnLock() noexcept;
    /// @brief Get the mutex semaphore
    /// @return pthread_mutex_t pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00272
    /// @needwork = dda
    /// @endcode
    inline pthread_mutex_t *GetMutexPtr() noexcept { return &threadMutex_; }
};
//***************/
/// @brief Wrapper for pthread automatic mutex lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00011
/// @trace_id_dd=DD_CRYPTO_00273
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PAutoLock final
{
private:
    /// @brief Mutex lock object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00274
    /// @needwork = dda
    /// @endcode
    PThreadLock &threadLock_;

public:
    /// @brief Parameterized constructor
    /// @param threadLock Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00275
    /// @needwork = dda
    /// @endcode
    explicit PAutoLock(PThreadLock &threadLock) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00276
    /// @needwork = dda
    /// @endcode
    ~PAutoLock() noexcept;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00277
    /// @needwork = dda
    /// @endcode
    PAutoLock(PAutoLock const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00278
    /// @needwork = dda
    /// @endcode
    PAutoLock &operator=(PAutoLock const &a) = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00279
    /// @needwork = dda
    /// @endcode
    PAutoLock(PAutoLock &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00280
    /// @needwork = dda
    /// @endcode
    PAutoLock &operator=(PAutoLock &&a) = delete;

public:
};
//********************************/
/// @brief Wrapper for pthread read-write lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00012
/// @trace_id_dd=DD_CRYPTO_00281
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PThreadLockRW final
{
private:
    /// @brief The QAC here is caused by the internal writing style of NAI
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00282
    /// @needwork = dda
    /// @endcode
    pthread_rwlock_t pthreadRwLock_;
    /// @brief Number of times a write lock is re-acquired by the same thread
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00283
    /// @needwork = dda
    /// @endcode
    uint32_t nLoopCount_;

public:
    /// @brief Parameterized constructor
    /// @param rwlockAttr Read-write lock attribute
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00284
    /// @needwork = dda
    /// @endcode
    explicit PThreadLockRW(pthread_rwlockattr_t const &rwlockAttr) noexcept;
    /// @brief Parameterized constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00285
    /// @needwork = dda
    /// @endcode
    explicit PThreadLockRW(uint32_t const nLoopCount) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00286
    /// @needwork = dda
    /// @endcode
    ~PThreadLockRW() noexcept;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00287
    /// @needwork = dda
    /// @endcode
    PThreadLockRW(PThreadLockRW const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00288
    /// @needwork = dda
    /// @endcode
    PThreadLockRW &operator=(PThreadLockRW const &a) = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00289
    /// @needwork = dda
    /// @endcode
    PThreadLockRW(PThreadLockRW &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00290
    /// @needwork = dda
    /// @endcode
    PThreadLockRW &operator=(PThreadLockRW &&a) = delete;

public:
    /// @brief Lock: read
    /// @return ture if LockRead false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00291
    /// @needwork = dda
    /// @endcode
    bool LockRead() noexcept;
    /// @brief Try to unlock: read
    /// @return ture if TryLockRead false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00292
    /// @needwork = dda
    /// @endcode
    bool TryLockRead() noexcept;
    /// @brief Lock: write
    /// @return ture if LockWrite false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00293
    /// @needwork = dda
    /// @endcode
    bool LockWrite() noexcept;
    /// @brief Try to unlock: write
    /// @return ture if TryLockWrite false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00294
    /// @needwork = dda
    /// @endcode
    bool TryLockWrite() noexcept;
    /// @brief Unlock: read
    /// @return ture if UnLockRead false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00295
    /// @needwork = dda
    /// @endcode
    bool UnLockRead() noexcept;
    /// @brief Unlock: write
    /// @return ture if UnLockWrite false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00296
    /// @needwork = dda
    /// @endcode
    bool UnLockWrite() noexcept;
};
//***************/
/// @brief Pthread automatic read lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00013
/// @trace_id_dd=DD_CRYPTO_00297
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PAutoLockRead final
{
private:
    /// @brief Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00298
    /// @needwork = dda
    /// @endcode
    PThreadLockRW &threadLock_;
    /// @brief Whether locking succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00299
    /// @needwork = dda
    /// @endcode
    bool bLockSuccess_;

public:
    /// @brief Parameterized constructor
    /// @param threadLock Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00300
    /// @needwork = dda
    /// @endcode
    explicit PAutoLockRead(PThreadLockRW &threadLock) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00301
    /// @needwork = dda
    /// @endcode
    PAutoLockRead() noexcept = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00302
    /// @needwork = dda
    /// @endcode
    ~PAutoLockRead() noexcept;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00303
    /// @needwork = dda
    /// @endcode
    PAutoLockRead(PAutoLockRead const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00304
    /// @needwork = dda
    /// @endcode
    PAutoLockRead &operator=(PAutoLockRead const &a) = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00305
    /// @needwork = dda
    /// @endcode
    PAutoLockRead(PAutoLockRead &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00306
    /// @needwork = dda
    /// @endcode
    PAutoLockRead &operator=(PAutoLockRead &&a) = delete;
};
//***************/
/// @brief Pthread automatic write lock
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00014
/// @trace_id_dd=DD_CRYPTO_00307
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PAutoLockWrite final
{
private:
    /// @brief Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00308
    /// @needwork = dda
    /// @endcode
    PThreadLockRW &threadLock_;
    /// @brief Whether locking succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00309
    /// @needwork = dda
    /// @endcode
    bool bLockSuccess_;

public:
    /// @brief Parameterized constructor
    /// @param threadLock thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00310
    /// @needwork = dda
    /// @endcode
    explicit PAutoLockWrite(PThreadLockRW &threadLock) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00311
    /// @needwork = dda
    /// @endcode
    PAutoLockWrite() noexcept = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00312
    /// @needwork = dda
    /// @endcode
    ~PAutoLockWrite() noexcept;
    /// @brief Default copy constructor
    /// @param a another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00313
    /// @needwork = dda
    /// @endcode
    PAutoLockWrite(PAutoLockWrite const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00314
    /// @needwork = dda
    /// @endcode
    PAutoLockWrite &operator=(PAutoLockWrite const &a) = delete;
    /// @brief Default move constructor
    /// @param a another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00315
    /// @needwork = dda
    /// @endcode
    PAutoLockWrite(PAutoLockWrite &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00316
    /// @needwork = dda
    /// @endcode
    PAutoLockWrite &operator=(PAutoLockWrite &&a) = delete;
};
//********************************/
/// @brief pthread condition variable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00015
/// @trace_id_dd=DD_CRYPTO_00317
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PThreadCond final
{
private:
    /// @brief Condition variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00318
    /// @needwork = dda
    /// @endcode
    pthread_cond_t threadCond_{};

public:
    /// @brief Parameterized constructor
    /// @param condAttr condition variable attribute
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00319
    /// @needwork = dda
    /// @endcode
    explicit PThreadCond(pthread_condattr_t const &condAttr) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00320
    /// @needwork = dda
    /// @endcode
    PThreadCond() noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00321
    /// @needwork = dda
    /// @endcode
    ~PThreadCond() noexcept;
    /// @brief Default copy constructor
    /// @param a another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00322
    /// @needwork = dda
    /// @endcode
    PThreadCond(PThreadCond const &a) = delete;
    /// @brief Default copy assignment operator
    /// @param a another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00323
    /// @needwork = dda
    /// @endcode
    PThreadCond &operator=(PThreadCond const &a) = delete;
    /// @brief Default move constructor
    /// @param a another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00324
    /// @needwork = dda
    /// @endcode
    PThreadCond(PThreadCond &&a) = delete;
    /// @brief Default move assignment operator
    /// @param a another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00325
    /// @needwork = dda
    /// @endcode
    PThreadCond &operator=(PThreadCond &&a) = delete;

public:
    /// @brief Blocking wait
    /// @param threadLock thread lock
    /// @return ture if Wait false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00326
    /// @needwork = dda
    /// @endcode
    bool Wait(PThreadLock &threadLock) noexcept;

    /// @brief Blocking wait with timeout
    /// @param threadLock thread lock
    /// @param tmData timeout data
    /// @return ture if TimedWait false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00327
    /// @needwork = dda
    /// @endcode
    bool TimedWait(PThreadLock &threadLock, struct timespec const *const tmData) noexcept;
    /// @brief Signal
    /// @return ture if Signal false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00328
    /// @needwork = dda
    /// @endcode
    bool Signal() noexcept;
    /// @brief Broadcast
    /// @return ture if Broadcast false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00329
    /// @needwork = dda
    /// @endcode
    bool Broadcast() noexcept;
};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_COMMON_PH_THREAD_LOCK_H_
