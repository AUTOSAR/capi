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
/// @file       gloabl_evloop.cpp
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#include <cassert>

#include "ara/core/result.h"
#include "isoft/core/core_error_domain.h"
#include "isoft/define.h"
#include "isoft/naicpp/evloop.h"
#include "isoft/naicpp/global_evloop.h"
#include "nai/runtime/nai_errno.h"

namespace isoft {
namespace naicpp {

/// @brief globalInfo_ event-loop share pointer
struct GlobalGeneralEvLoop::GlobalGeneralInfo *GlobalGeneralEvLoop::globalInfo_{nullptr};

///
/// @brief Global General Ev Loop Initialize (will be called automatically, user may not call it, can be called repeatedly)
///
/// @param runMode
/// @param enableExecutor Whether to enable the executor (default true)
/// @return ara::core::Result
ara::core::Result< void > GlobalGeneralEvLoop::Initialize(Mode const runMode, bool const enableExecutor) noexcept
{
    /* // static std::once_flag initialized;
    // std::call_once(initialized, [runMode, enableExecutor] {*/
    if (globalInfo_ != nullptr) {
        return {};
    }
    globalInfo_ = new GlobalGeneralInfo();
    if (globalInfo_ == nullptr) {
        return ara::core::Result< void >::FromError(std::move(isoft::core::CoreErrc::kFailure));
    }

    globalInfo_->mode_ = static_cast< std::uint8_t >(runMode);
    if (enableExecutor) {
        globalInfo_->enableExecutor_ = 1U;
    } else {
        globalInfo_->enableExecutor_ = 0U;
    }

    return {};
    //});
}

/// @brief Global General Ev Loop Deinitialize
void GlobalGeneralEvLoop::Deinitialize() noexcept
{
    if (globalInfo_ == nullptr) {
        return;
    }

    // check all std::shared_ptr<EvLoop> from GlobalGeneralEvLoop::Get are reseted
    assert(globalInfo_->loop_.expired());

    delete globalInfo_;
    globalInfo_ = nullptr;
}

/*
// int GlobalGeneralEvLoop::Dispatch() {
//     assert(globalInfo_->mode_ == Mode::kUserThread);
//     std::shared_ptr<EvLoop> loop = globalInfo_->loop_.lock();
//     if (loop == nullptr) return -1;

//     return loop->Run(true);
// }
*/

/// @brief Get the Global General Ev Loop object
/// @return std::shared_ptr<EvLoop>
std::shared_ptr< EvLoop > GlobalGeneralEvLoop::Get() noexcept
{
    std::lock_guard< std::mutex > const lock_guard{globalInfo_->lock_};
    std::shared_ptr< EvLoop > cur{globalInfo_->loop_.lock()};
    if (cur == nullptr) {
        cur = std::make_shared< EvLoop >();
        if (!cur) {
            return nullptr;
        }
        if (isoft::kSuccess != cur->Open(globalInfo_->enableExecutor_ == 1U)) {
            return nullptr;
        }
        if (globalInfo_->mode_ == static_cast< std::uint8_t >(Mode::kInsideThread)) {
            if (isoft::kSuccess != cur->Run(false)) {
                return nullptr;
            }
        }
        globalInfo_->loop_ = cur;
    }
    return cur;
}

}  // namespace naicpp
}  // namespace isoft
