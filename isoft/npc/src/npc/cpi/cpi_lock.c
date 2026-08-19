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
/// @file       cpi_lock.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#if defined(i386) || defined(__x86_64)
#define barrier() asm volatile ("": : :"memory")
#define cpu_relax() asm volatile ("pause\n": : :"memory")
static inline unsigned long xchg(void *ptr, unsigned long x)
{
    __asm__ __volatile__("xchg %0, %1"
                         :"=r" (x)
                         :"m" (*(volatile long *)ptr), "0"(x)
                         :"memory");
    return x;
}

void spin_lock(void *lock)
{
    unsigned long *l = (unsigned long *)lock;
    while (1) {
        if (!xchg(l, 1)) return;
        while (*l) cpu_relax();
    }
}

void spin_unlock(void *lock)
{
    unsigned long *l = (unsigned long *)lock;
    barrier();
    *l = 0;
}

int spin_trylock(void *lock)
{
    return !xchg(lock, 1);
}
#endif

