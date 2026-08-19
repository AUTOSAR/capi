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
/// @file       nai_thread.h
/// @brief      thread and synchronization interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details this implementation provides threads, thread local, mutexes, 
 *          spin locks, condition variables, once, and basic atomic operations.
 *
 * @details reentrant mutexes and spin locks are not provided, and users are 
 *          required to plan the logical organization and the scope of the 
 *          lock and avoid reentrant situations.
 *
 * @details the code example is as follows:
 *
 * @par     implement a thread proc
 * @code
 *          nai_thread_api my_thread_proc(void* arg)
 *          {
 *              ...
 *
 *              return 0;
 *          };
 * @endcode
 *
 * @par     simply create a thread
 * @code
 *          nai_int_t r;
 *          void* arg;
 *          nai_thread_t t;
 *
 *          nai_thread_init(&t);
 *          r = nai_thread_create(0, 0, my_thread_proc, arg);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create a thread attributes
 * @code
 *          nai_int_t r;
 *          nai_int_t stack_size;
 *          nai_int_t guard_size;
 *          nai_thread_attr_t a;
 *
 *          nai_thread_attr_init(&a);
 *
 *          r = nai_thread_attr_open(&a);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_thread_attr_set_stacksize(&a, stack_size);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_thread_attr_set_guardsize(&a, guard_size);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_thread_attr_set_sched(
 *              &a, NAI_SCHED_NORMAL, NAI_PRIORITY_LOWEST);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create a thread with attributes
 * @code
 *          nai_int_t r;
 *          void* arg;
 *          nai_thread_t t;
 *          nai_thread_attr_t* a;
 *
 *
 *          nai_thread_init(&t);
 *          r = nai_thread_create(0, a, my_thread_proc, arg);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     wait for the thread to exit
 * @code
 *          nai_int_t r;
 *          nai_thread_t* t;
 *
 *          r = nai_thread_join(t);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     wait a condition variable
 * @code
 *          nai_int_t* condition;
 *          nai_cond_t* c;
 *          nai_mutex_t* m;
 *
 *          nai_mutex_lock(m);
 *
 *          // waiting when the condition is not met
 *          while (*condition == 0) {
 *              nai_cond_wait(c, m);
 *          };
 *
 *          // process
 *          ...
 *
 *          nai_mutex_unlock(m);
 * @endcode
 *
 * @par     signal a condition variable
 * @code
 *          nai_int_t* condition;
 *          nai_cond_t* c;
 *          nai_mutex_t* m;
 *
 *          nai_mutex_lock(m);
 *
 *          // send a notification signal when the condition is met
 *          if (*condition == 0) {
 *              *condition = 1;
 *              nai_cond_singal(c);
 *          };
 *
 *          nai_mutex_unlock(m);
 * @endcode
 *
 * @par     make the compiler guarantee the order of execution
 * @code
 *          nai_int_t a;
 *          nai_int_t b;
 *          nai_int_t c;
 *
 *          // modify variables a and b
 *          a ++;
 *          b --;
 *
 *          // tell the compiler that the assignment of variable c 
 *          // should be performed after modifying variables a and b, 
 *          // and do not change the execution order due to optimization.
 *          nai_memory_barrier();
 *
 *          // here, variables a and b have been modified
 *          c = 0;
 *
 * @endcode
 */

#ifndef _THREAD_H_NAI
#define _THREAD_H_NAI

#pragma once

#include "nai_system.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_CPUSET_T
    #define _NAI_TYPEDEF_CPUSET_T
    typedef struct nai_cpuset_s nai_cpuset_t;
#endif

#if defined(_WIN32)

    #define NAI_ONCE_INIT 0

    #define nai_thread_api unsigned int __stdcall

    typedef struct nai_thread_attr_s
    {
        nai_int_t sched;    /**< the scheduling class */
        nai_int_t priority; /**< the scheduling priority */
        size_t stack_size;  /**< the stack size */
        size_t guard_size;  /**< the gurad size of stack */
        struct
        {
            size_t size;             /**< the size of the cpuset */
            const nai_cpuset_t* set; /**< pointer to the cpuset */
        } cpu;
    } nai_thread_attr_t;

    typedef void* HANDLE;
    typedef HANDLE nai_thread_t;
    typedef unsigned int nai_thread_key_t;
    typedef HANDLE nai_cond_t;
    typedef struct _RTL_CRITICAL_SECTION* nai_mutex_t;
    typedef struct _RTL_CRITICAL_SECTION* nai_spin_t;
    typedef struct nai_rwlock_s* nai_rwlock_t;
    typedef int32_t nai_once_t;
    typedef unsigned int nai_tid_t;
    typedef unsigned int(__stdcall* nai_thread_entry_f)(void*);
    typedef void (*nai_thread_key_free_f)(void*);

#elif (NAI_HAVE_PTHREAD_H)

    #include <pthread.h>

    #define NAI_ONCE_INIT PTHREAD_ONCE_INIT

    #define nai_thread_api void*

typedef pthread_t nai_thread_t;
typedef pthread_attr_t nai_thread_attr_t;
typedef pthread_key_t nai_thread_key_t;
typedef pthread_mutex_t nai_mutex_t;
typedef pthread_cond_t nai_cond_t;
typedef pthread_rwlock_t nai_rwlock_t;
typedef pthread_once_t nai_once_t;
typedef pthread_t nai_tid_t;
typedef void* (*nai_thread_entry_f)(void*);
typedef void (*nai_thread_key_free_f)(void*);

    #if (NAI_HAVE_PTHREAD_SPINLOCK)
typedef pthread_spinlock_t nai_spin_t;
    #else
typedef pthread_mutex_t nai_spin_t;
    #endif

#else

    #error "nai_thread is not implemented, unsupported platform!"

#endif

//////////////////////////////////////////////////////////////////////////////
// thread attr

/**
 * initial the structure of the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @return  void
 */
#define nai_thread_attr_init(a) nai_memset((void*)(a), 0, sizeof(*(a)))

    /**
 * open the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_open(nai_thread_attr_t* a);

    /**
 * set stack size of the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @param   stack_size the new size of stack
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_set_stacksize(nai_thread_attr_t* a, size_t stack_size);

    /**
 * set guard size of the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @param   guard_size the new size of guard
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_set_guardsize(nai_thread_attr_t* a, size_t guard_size);

    /**
 * set scheduling paramters of the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @param   sched   the scheduling class, see @ref NAI_SCHED
 * @param   priority the scheduling priority, see @ref NAI_PRIORITY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when sched class is not normal and negative priority is set, 
 *          the actual priority is (sched_min_priority + -priority)
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_set_sched(nai_thread_attr_t* a, nai_int_t sched, nai_int_t priority);

    /**
 * set the cpuset of affinity
 * @param   a       pointer to the process attributes
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the function does not copy the cpuset, 
 *          the caller needs to keep the memory of the cpuset
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_set_affinity(nai_thread_attr_t* a, size_t size, const nai_cpuset_t* set);

    /**
 * close the thread attributes
 * @param   a       pointer to the thread attrubutes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_attr_close(nai_thread_attr_t* a);

//////////////////////////////////////////////////////////////////////////////
// thread

/**
 * initial the thread
 * @param   t       pointer to the thread
 * @return  void
 */
#define nai_thread_init(t) nai_memset((void*)(t), 0, sizeof(*(t)))

    /**
 * create a new thread
 * @param   t       pointer to the thread
 * @param   a       pointer to the thread attributes
 * @param   ent     the entry of thread
 * @param   arg     the argument of thread
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_create(nai_thread_t* t, const nai_thread_attr_t* a, nai_thread_entry_f ent, void* arg);

    /**
 * set scheduling paramters of the thread
 * @param   t       pointer to the thread
 * @param   sched   the scheduling class, see @ref NAI_SCHED
 * @param   priority the scheduling priority, see @ref NAI_PRIORITY
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_set_sched(nai_thread_t* t, nai_int_t sched, nai_int_t priority);

    /**
 * get the cpuset of affinity
 * @param   t       pointer to the process
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset to receive
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_get_affinity(nai_thread_t* t, size_t size, nai_cpuset_t* set);

    /**
 * set the cpuset of affinity
 * @param   t       pointer to the process attributes
 * @param   size    the size of the cpuset
 * @param   set     pointer to the cpuset
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_set_affinity(nai_thread_t* t, size_t size, const nai_cpuset_t* set);

    /**
 * wait a thread until the thead exit
 * @param   t       pointer to the thread
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_join(nai_thread_t* t);

    /**
 * detach and close the handle of thread
 * @param   t       pointer to the thread
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_detach(nai_thread_t* t);

    /**
 * get the current thread id
 * @return  the value of current thread id
 */
    NAI_EXTERN
    nai_tid_t nai_thread_id();

    /**
 * get a thread local condition
 * @retval  !=0     the address of the condition is returned on success
 * @retval  0       an error occurred, see #nai_errno
 * @note    the conditon is used in current thread only
 */
    NAI_EXTERN
    nai_cond_t* nai_thread_local_cond();

    //////////////////////////////////////////////////////////////////////////////
    // thread storage

#define NAI_THREAD_KEY_NIL ((nai_thread_key_t)-1)

/**
 * initial the thread local key
 * @param   t       pointer to the thread local key
 * @return  void
 */
#define nai_thread_key_init(t)                                                                                         \
    {                                                                                                                  \
        *(t) = NAI_THREAD_KEY_NIL;                                                                                     \
    }

    /**
 * open the thread local key
 * @param   t       pointer to the thread local key
 * @param   cb      pointer to the free callback, can be null
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_key_open(nai_thread_key_t* t, nai_thread_key_free_f cb);

    /**
 * set the value of the thread local
 * @param   t       pointer to the thread local key
 * @param   value   pointer of the new value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_key_set(nai_thread_key_t* t, void* value);

    /**
 * get the value of the thread local
 * @param   t       pointer to the thread local key
 * @return  the value of thread local
 */
    NAI_EXTERN
    void* nai_thread_key_get(nai_thread_key_t* t);

    /**
 * close the thread local key
 * @param   t       pointer to the thread local key
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_thread_key_close(nai_thread_key_t* t);

//////////////////////////////////////////////////////////////////////////////
// mutex

/**
 * initial a mutex
 * @param   m       pointer to the mutex
 * @return  void
 */
#define nai_mutex_init(m) nai_memset((void*)(m), 0, sizeof(*(m)))

    /**
 * create a new mutex
 * @param   m       pointer to the mutex
 * @param   shared  indicates that it can be shared between processes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the mutex is non-recursive
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
    NAI_EXTERN
    nai_int_t nai_mutex_open(nai_mutex_t* m, nai_int_t shared);

    /**
 * try lock the mutex
 * @param   m       pointer to the mutex
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the mutex is non-recursive
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the mutex is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_mutex_trylock(nai_mutex_t* m);

    /**
 * lock the mutex
 * @param   m       pointer to the mutex
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the mutex is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_mutex_lock(nai_mutex_t* m);

    /**
 * unlock the mutex
 * @param   m       pointer to the mutex
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_mutex_unlock(nai_mutex_t* m);

    /**
 * close the mutex
 * @param   m       pointer to the mutex
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_mutex_close(nai_mutex_t* m);

//////////////////////////////////////////////////////////////////////////////
// cond

/**
 * initial the condtion
 * @param   c       pointer to the condition
 * @return  void
 */
#define nai_cond_init(c) nai_memset((void*)(c), 0, sizeof(*(c)))

    /**
 * open the condition
 * @param   c       pointer to the condition
 * @param   shared  indicates that it can be shared between processes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
    NAI_EXTERN
    nai_int_t nai_cond_open(nai_cond_t* c, nai_int_t shared);

    /**
 * wait the condtion until signaled
 * @param   c       pointer to the condition
 * @param   m       pointer to the mutex work with the condition
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_cond_wait(nai_cond_t* c, nai_mutex_t* m);

    /**
 * wait the condition with a timeout
 * @param   c       pointer to the condition
 * @param   m       pointer to the mutex work with the condition
 * @param   abstime in mirco-seconds, 
 *                  blocked until abstime has already been passed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the value of abstime is from the monotonic clock(#nai_tick_to_usec)
 **/
    NAI_EXTERN
    nai_int_t nai_cond_timedwait(nai_cond_t* c, nai_mutex_t* m, uint64_t abstime);

    /**
 * signal a condition
 * @param   c       pointer to the condition
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_cond_signal(nai_cond_t* c);

    /**
 * close the condition
 * @param   c       pointer to the condition
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_cond_close(nai_cond_t* c);

//////////////////////////////////////////////////////////////////////////////
// spinlock

/**
 * initial the spinlock
 * @param   l       pointer to the spinlock
 * @return  void
 */
#define nai_spin_init(l) nai_memset((void*)(l), 0, sizeof(*(l)))

    /**
 * open the spinlock
 * @param   l       pointer to the spinlock
 * @param   shared  indicates that it can be shared between processes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the spinlock is non-recursive
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
    NAI_EXTERN
    nai_int_t nai_spin_open(nai_spin_t* l, nai_int_t shared);

    /**
 * try lock the spinlock
 * @param   l       pointer to the spinlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the spinlock is non-recursive
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the spinlock is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_spin_trylock(nai_spin_t* l);

    /**
 * lock the spinlock
 * @param   l       pointer to the spinlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the spinlock is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_spin_lock(nai_spin_t* l);

    /**
 * unlock the spinlock
 * @param   l       pointer to the spinlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_spin_unlock(nai_spin_t* l);

    /**
 * close the spinlock
 * @param   l       pointer to the spinlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_spin_close(nai_spin_t* l);

//////////////////////////////////////////////////////////////////////////////
// rwlock

/**
 * initial the rwlock
 * @param   l       pointer to the rwlock
 * @return  void
 */
#define nai_rwlock_init(l) nai_memset((void*)(l), 0, sizeof(*(l)))

    /**
 * open the rwlock
 * @param   l       pointer to the rwlock
 * @param   shared  indicates that it can be shared between processes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_open(nai_rwlock_t* l, nai_int_t shared);

    /**
 * try lock the read lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the write lock is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_rd_trylock(nai_rwlock_t* l);

    /**
 * lock the read lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_rd_lock(nai_rwlock_t* l);

    /**
 * unlock the read lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_rd_unlock(nai_rwlock_t* l);

    /**
 * try lock the write lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the rwlock is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_wr_trylock(nai_rwlock_t* l);

    /**
 * lock the write lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_wr_lock(nai_rwlock_t* l);

    /**
 * unlock the write lock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the rwlock is non-recursive
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_wr_unlock(nai_rwlock_t* l);

    /**
 * close the rwlock
 * @param   l       pointer to the rwlock
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_rwlock_close(nai_rwlock_t* l);

    //////////////////////////////////////////////////////////////////////////////
    // once

#ifndef _NAI_TYPEDEF_ONCE_ENTRY_F
    #define _NAI_TYPEDEF_ONCE_ENTRY_F
    typedef void (*nai_once_entry_f)();
#endif

    /**
 * dynamic package initialization
 * @param   p       pointer to the once which initialized by #NAI_ONCE_INIT
 * @param   once    the initialize callback of once
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_once(nai_once_t* p, nai_once_entry_f once);

    //////////////////////////////////////////////////////////////////////////////
    // atomic

#ifndef _NAI_TYPEDEF_ATOMIC32_T
    #define _NAI_TYPEDEF_ATOMIC32_T
    typedef int32_t nai_atomic32_t;
#endif
#ifndef _NAI_TYPEDEF_ATOMIC64_T
    #define _NAI_TYPEDEF_ATOMIC64_T
    typedef int64_t nai_atomic64_t;
#endif
#ifndef _NAI_TYPEDEF_ATOMIC_T
    #define _NAI_TYPEDEF_ATOMIC_T
    #if NAI_SIZEOF_VOID_P == 4
    typedef nai_atomic32_t nai_atomic_t;
    #else
    typedef nai_atomic64_t nai_atomic_t;
    #endif
#endif

    /**
 * increment the atomic
 * @param   x       pointer to the atomic
 * @return  the incremented value of the atomic
 */
    NAI_EXTERN
    nai_atomic_t nai_atomic_inc(nai_atomic_t* x);

    /**
 * decrement the atomic
 * @param   x       pointer to the atomic
 * @return  the decremented value of the atomic
 */
    NAI_EXTERN
    nai_atomic_t nai_atomic_dec(nai_atomic_t* x);

    /**
 * add the specified value to the atomic
 * @param   x       pointer to the atomic
 * @param   a       the value which used to added
 * @return  the added value of the atomic
 */
    NAI_EXTERN
    nai_atomic_t nai_atomic_add(nai_atomic_t* x, nai_atomic_t a);

    /**
 * compare and swap the atomic
 * @param   x       pointer to the atomic
 * @param   c       the value which used to compare
 * @param   n       the value which used to swap
 * @return  the old value of the atomic
 */
    NAI_EXTERN
    nai_atomic_t nai_atomic_cas(nai_atomic_t* x, nai_atomic_t c, nai_atomic_t n);

    /**
 * increment the atomic32
 * @param   x       pointer to the atomic32
 * @return  the incremented value of the atomic32
 */
    NAI_EXTERN
    nai_atomic32_t nai_atomic32_inc(nai_atomic32_t* x);

    /**
 * decrement the atomic32
 * @param   x       pointer to the atomic32
 * @return  the decremented value of the atomic32
 */
    NAI_EXTERN
    nai_atomic32_t nai_atomic32_dec(nai_atomic32_t* x);

    /**
 * add the specified value to the atomic32
 * @param   x       pointer to the atomic32
 * @param   a       the value which used to added
 * @return  the added value of the atomic32
 */
    NAI_EXTERN
    nai_atomic32_t nai_atomic32_add(nai_atomic32_t* x, nai_atomic32_t a);

    /**
 * compare and swap the atomic32
 * @param   x       pointer to the atomic32
 * @param   c       the value which used to compare
 * @param   n       the value which used to swap
 * @return  the old value of the atomic32
 */
    NAI_EXTERN
    nai_atomic32_t nai_atomic32_cas(nai_atomic32_t* x, nai_atomic32_t c, nai_atomic32_t n);

    /**
 * add the specified value to the float
 * @param   p       pointer to the float
 * @param   a       the value which used to added
 * @return  the added value of the float
 */
    NAI_EXTERN
    float nai_atomic_addf(float* p, float a);

    /**
 * subtract the specified value to the float
 * @param   p       pointer to the float
 * @param   a       the value which used to added
 * @return  the subtracted value of the float
 */
    NAI_EXTERN
    float nai_atomic_subf(float* p, float a);

    /**
 * try lock a atomic lock
 * @param   x       pointer to the atomic
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the atomic lock is non-recursive
 * @note    should use 0 to initial the atomic32
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the mutex is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_atomic_trylock(nai_atomic_t* x);

    /**
 * lock a atomic lock
 * @param   x       pointer to the atomic
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the atomic lock is non-recursive
 * @note    should use 0 to initial the atomic32
 */
    NAI_EXTERN
    nai_int_t nai_atomic_lock(nai_atomic_t* x);

    /**
 * unlock a atomic lock
 * @param   x       pointer to the atomic
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atomic_unlock(nai_atomic_t* x);

    /**
 * try lock a atomic32 lock
 * @param   x       pointer to the atomic32
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the atomic32 lock is non-recursive
 * @note    should use 0 to initial the atomic32
 * @note    if fail and nai_errno is setted to EBUSY 
 *          means the mutex is alreay locked.
 */
    NAI_EXTERN
    nai_int_t nai_atomic32_trylock(nai_atomic32_t* x);

    /**
 * lock a atomic32 lock
 * @param   x       pointer to the atomic32
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the atomic32 lock is non-recursive
 * @note    should use 0 to initial the atomic32
 */
    NAI_EXTERN
    nai_int_t nai_atomic32_lock(nai_atomic32_t* x);

    /**
 * unlock a atomic32 lock
 * @param   x       pointer to the atomic32
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_atomic32_unlock(nai_atomic32_t* x);

    //////////////////////////////////////////////////////////////////////////////
    // cpu pause

#if defined(_MSC_VER) && (defined(_X86_) || defined(_AMD64_))
    #define nai_cpu_pause() _mm_pause()
#elif defined(__GUNC__) && (__i386__ || __i386 || __amd64__ || __amd64)
    #define nai_cpu_pause() __asm__("pause")
#else
    #define nai_cpu_pause() (void)0
#endif

    //////////////////////////////////////////////////////////////////////////////
    // memory barrier

#if (__darwin__)
    #if 0
        #include <libkern/OSAtomic.h>
        #define nai_memory_barrier() OSMemoryBarrier()
    #else
        #include <stdatomic.h>
        #define nai_memory_barrier() atomic_thread_fence(memory_order_release)
    #endif
#elif (NAI_HAVE_GCC_ATOMIC)
    #define nai_memory_barrier() __sync_synchronize()
#elif defined(__GNUC__)
    #define nai_memory_barrier() __asm__ volatile("" ::: "memory")
#else
    #define nai_memory_barrier() (void)0
#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
