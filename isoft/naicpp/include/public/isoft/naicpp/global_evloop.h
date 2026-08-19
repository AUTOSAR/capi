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
/// @file       global_evloop.h
/// @brief      GlobalGeneralEvLoop class
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================
///
/// Example usage:
/// A: In the main thread:
/// int main() {
/// GlobalGeneralEvLoop::Initialize(Mode::kUserThread);  // Optional, but must be at the very beginning of the process
/// std::shared_ptr<EvLoop> loop = GlobalGeneralEvLoop::Get();
/// // Other usages are the same as EvLoop, but lifecycle-related methods (Open Stop Close) of the loop cannot be operated
/// // do something
/// loop.rest() // release loop after run returns
/// // GlobalGeneralEvLoop::Deinitialize(Mode::kUserThread); // Optional, but must be at the very end of the process
/// }
/// B: Use GlobalGeneralEvLoop's own thread
/// Any module:
/// // GlobalGeneralEvLoop::Initialize(Mode::kInsideThread); // Not needed, it will automatically initialize with kInsideThread
/// std::shared_ptr<EvLoop> loop = GlobalGeneralEvLoop::Get();
/// // Other usages are the same as EvLoop, but lifecycle-related methods (Open Run Stop Close) of EvLoop cannot be operated
/// // do something
/// loop.rest() // release loop when the module no longer uses the loop
/// // GlobalGeneralEvLoop::Deinitialize(Mode::kUserThread); // Optional, but must be at the very end of the process
/// }
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_GLOBAL_EVLOOP_H_
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_GLOBAL_EVLOOP_H_

#include <nai/io/nai_event.h>

#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "ara/core/result.h"
#include "isoft/naicpp/evloop.h"
#include "isoft/types.h"
namespace isoft {
namespace naicpp {

///
/// @brief Event loop encapsulation
///
class GlobalGeneralEvLoop final
{
public:
    /// @brief default constructor is delete
    ///
    /// @traceid
    /// @uptrace
    GlobalGeneralEvLoop() noexcept = delete;

    /// @brief Mode Thread mode
    enum class Mode : std::uint8_t
    {
        kUserThread = 1,  // Use a user-provided thread, the user needs to call GlobalGeneralEvLoop::Get()EvLoop in their own thread and then call Run(true)
        kInsideThread  // Use an internally automatically created thread
    };

    ///
    /// @brief Global General Ev Loop Initialize (will be called automatically, user may not call it, can be called repeatedly)
    /// @details Prevents module A from initializing with false first in the application, and then module B wanting to initialize with true and failing)
    /// @param runMode
    /// @param enableExecutor Whether to enable the executor (default true)
    /// @return ara::core::Result
    static ara::core::Result< void > Initialize(Mode const runMode        = Mode::kInsideThread,
                                                bool const enableExecutor = true) noexcept;

    ///
    /// @brief Global General Ev Loop Deinitialize
    /// @details Ensure that all EvLoops held via Get() have been released before calling
    /// Generally, it does not need to be called. If troubleshooting such as memory leak is needed, calling it can achieve a "graceful" shutdown
    ///
    static void Deinitialize() noexcept;

    ///
    /// @brief Get the Global General Ev Loop object
    /// @details EvLoop will be responsible for establishing a global general evloop for common logic sharing
    /// @note The usage of the returned Evloop needs to ensure it is always held, otherwise it might be recycled, including releasing after adding asynchronous events is not allowed.
    ///       The correct approach is to always hold it until the module no longer uses this Evloop and has cleaned up all asynchronous nodes added through this Evloop
    /// @note   GlobalGeneralEvLoop is responsible for managing the EvLoop lifecycle (Open Run Stop Close), users should not call it
    /// @return std::shared_ptr<EvLoop>
    static std::shared_ptr< EvLoop > Get() noexcept;

private:
    // Do not use static class objects to prevent calls before main()
    // TODO(jiawei-l3) If the actual situation is that GlobalGeneralEvLoop must be used, change from share_ptr (globalInfo_) to raw
    // EvLoop*, handle directly in ara::core::de/Initialize
    ///
    /// @brief GlobalGeneralInfo
    struct GlobalGeneralInfo final
    {
        /// @name loop_
        /// loop_ pointer
        std::weak_ptr< EvLoop > loop_;

        /// @name lock_
        /// Mutex lock
        std::mutex lock_;

        /// @name mode_
        /// Thread mode
        std::uint8_t mode_ : 2;

        /// @name enableExecutor_
        /// Whether to enable the executor
        std::uint8_t enableExecutor_ : 1;
    };

    /// @name globalInfo_
    /// Global information
    static struct GlobalGeneralInfo* globalInfo_;
};

}  // namespace naicpp
}  // namespace isoft

#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_GLOBAL_EVLOOP_H_
