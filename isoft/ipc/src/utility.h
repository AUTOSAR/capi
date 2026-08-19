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
/// @file       utility.h
/// @brief
/// @details
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef _UTILITY_H_IPC
#define _UTILITY_H_IPC

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#define ipc_container_of(pointer, type, member) ((type*)((uintptr_t)pointer - offsetof(type, member)))

typedef atomic_flag ipc_spinlock_t;

#define IPC_SPINLOCK_INITIALIZER ATOMIC_FLAG_INIT

#define ipc_spinlock_init(LOCK) atomic_flag_clear(LOCK)

#define ipc_spinlock_lock(LOCK) while (atomic_flag_test_and_set_explicit((LOCK), memory_order_acquire))

#define ipc_spinlock_unlock(LOCK) atomic_flag_clear_explicit((LOCK), memory_order_release)

#define ipc_assert(SHOULD_BE_TRUE)                                                                                     \
    if (!(SHOULD_BE_TRUE))                                                                                             \
    assert(0)

#endif
