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
/// @file       thread_pool.h
/// @brief      This file provides thread_pool functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=unit
/// @trace_id_sr=3ca19626-8d54-4895-9c60-c08f94178c50
/// @unit_name=ThreadPool
/// @unit_description=This file provides thread_pool functionality
/// @endcode
///
/// ================================================================

#ifndef ARA_DIAG_COMMON_THREAD_POOL_H_
#define ARA_DIAG_COMMON_THREAD_POOL_H_

// #include <atomic>
// #include <condition_variable>
// #include <functional>
// #include <mutex>
// #include <queue>
// #include <thread>
// #include <map>
#include "thread_pool/thread_pool.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Thread pool
using isoft::ThreadPool;


}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_COMMON_THREAD_POOL_H_