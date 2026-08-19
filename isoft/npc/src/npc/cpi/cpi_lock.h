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
/// @file       cpi_lock.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_LOCK_H
#define __CPI_LOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4 && __GNUC_MINOR__ > 1)

    typedef long cpi_spin_t;

    #define cpi_spin_trylock(lock_ptr) (!__sync_bool_compare_and_swap((lock_ptr), 0, 1))
    #define cpi_spin_lock(lock_ptr)    while (!(__sync_bool_compare_and_swap((lock_ptr), 0, 1)))
    #define cpi_spin_unlock(lock_ptr)  __sync_bool_compare_and_swap((lock_ptr), 1, 0)
    #define cpi_spin_init(lock_ptr)    *(lock_ptr) = 0
    #define cpi_spin_destroy(lock_ptr) *(lock_ptr) = 0

#elif defined(i386) || defined(__x86_64)

typedef unsigned long cpi_spin_t;

extern void spin_lock(void *lock);
extern void spin_unlock(void *lock);
extern int spin_trylock(void *lock);
    #define cpi_spin_lock(lock_ptr)    spin_lock((lock_ptr))
    #define cpi_spin_unlock(lock_ptr)  spin_unlock((lock_ptr))
    #define cpi_spin_trylock(lock_ptr) spin_trylock((lock_ptr))
    #define cpi_spin_init(lock_ptr)    *(lock_ptr) = 0
    #define cpi_spin_destroy(lock_ptr) *(lock_ptr) = 0

#else

    #include <pthread>

typedef pthread_spinlock_t cpi_spin_t;

    #define cpi_spin_trylock(lock_ptr) pthread_spin_trylock((lock_ptr))
    #define cpi_spin_lock(lock_ptr)    pthread_spin_lock((lock_ptr))
    #define cpi_spin_unlock(lock_ptr)  pthread_spin_unlock((lock_ptr))
    #define cpi_spin_init(lock_ptr)    pthread_spin_init((lock_ptr), PTHREAD_PROCESS_PRIVATE)
    #define cpi_spin_destroy(lock_ptr) pthread_spin_destroy((lock_ptr))

#endif

#ifdef __cplusplus
}
#endif

#endif
