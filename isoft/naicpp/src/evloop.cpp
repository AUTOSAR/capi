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
/// @file       evloop.cpp
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#include "isoft/naicpp/evloop.h"

#include "isoft/define.h"

namespace isoft {
namespace naicpp {

///
/// @brief Initialize the evloop thread
///
/// @param enableExecutor Whether to enable the executor, whether Exec() is available
/// @returns  isoft::kSuccess Success
/// @returns   -1 Failure
int EvLoop::Open(bool const enableExecutor) noexcept
{
    this->naiLoop_ = nai_evloop_new();
    if (this->naiLoop_ == nullptr) {
        return -1;
    }

    int r{nai_evloop_open(this->naiLoop_, 0, 0, nullptr)};
    if (r != 0) {
        static_cast< void >(Close());
        return r;
    }
    if (enableExecutor) {
        r = InitExecutor();
        if (r != 0) {
            static_cast< void >(Close());
            return r;
        }
    }
    return isoft::kSuccess;
}

///
/// @brief Start the evloop thread
///
/// @param useUserThread
///             true: Run blocks, processing asynchronous events in the current thread
///             false: Run creates a new thread internally and returns
/// @note If useUserThread==true, waitCloseFinish needs to be set to false when calling Close;
/// @note If it is @ref @see GlobalGeneralEvLoop, ensure consistency with the GlobalGeneralEvLoop::Initialize mode: i.e., ensure
/// When GlobalGeneralEvLoop::Mode::kUserThread mode, useUserThread=true, and vice versa
/// @return int
int EvLoop::Run(bool const useUserThread) noexcept
{
    if (this->naiLoop_ == nullptr) {
        return -1;
    }

    if (!useUserThread) {
        naiLoopThread_ = std::thread([this]() { static_cast< void >(nai_evloop_dispatch(this->naiLoop_)); });
        if (false == naiLoopThread_.joinable()) {
            Stop();
            static_cast< void >(Close());
            return -1;
        }
        if (nai_evloop_wait(this->naiLoop_, NAI_EV_WAIT_RUNNING) < 0) {
            Stop();
            static_cast< void >(Close());
            return -1;
        }
        return isoft::kSuccess;
    }

    return nai_evloop_dispatch(this->naiLoop_);
}

///
/// @brief Close the evloop
/// EvLoop::Close must not be called within an Evloop Callback, guaranteed by the user
/// No callbacks generated internally? Ensure no deadlock concerns
///
/// @note Pay attention to different Run modes, the timing of calling Stop/Close differs
///       When Run with useUserThread=true, after calling Stop, wait for Run to return to safely close the EvLoop
///       When Run with useUserThread=false, you should call Stop first, then ensure that Close is not called within the Evloop (Evnode callback)
/// @return int
int EvLoop::Close() noexcept
{
    int ret;
    if (naiLoop_ == nullptr) {
        return -1;
    }

    static_cast< void >(nai_evloop_break(naiLoop_, 0));

    if (nai_evloop_in_dispatch(naiLoop_) == 0) {
        static_cast< void >(nai_evloop_wait(naiLoop_, NAI_EV_WAIT_BREAK));
    }
    if (naiLoopThread_.joinable()) {
        naiLoopThread_.join();
    }
    if (nodeExec_ != nullptr) {
        delete nodeExec_;
        nodeExec_ = nullptr;
    }
    ret = nai_evloop_done(naiLoop_);
    if (ret < 0) {
        /// TODO: Error handling
        assert(0);
    }
    ret = nai_evloop_close(naiLoop_);
    if (ret == -1) {
        assert(0);  // TODO: Identify the cause. If unavoidable, should delete, let the caller of EvLoop::Close() check the return value
        return isoft::kFailure;
    }
    naiLoop_ = nullptr;
    return isoft::kSuccess;
}

}  // namespace naicpp
}  // namespace isoft
