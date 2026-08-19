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
/// @file       terminating_handler.h
/// @brief
/// @details
/// @date       2022-11-08
/// @author     jian.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_TERMINIATING_HANDLER_H
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_TERMINIATING_HANDLER_H

#include <signal.h>

#include <atomic>
#include <memory>

#include "isoft/naicpp/evloop.h"
#include "isoft/naicpp/global_evloop.h"
#include "nai/io/nai_signal.h"

namespace isoft {
namespace naicpp {

/// @brief Process lifecycle control class. When a SIGTERM signal is received, the process should execute the exit procedure, otherwise it will be killed by timeout.
class TerminatingHandler final
{
private:
    /// @brief Internal context, used for passing parameters to nai_signal_set_cb.
    struct Context
    {
        std::shared_ptr< isoft::naicpp::EvLoop > mainLoop = nullptr;
        nai_signal_t signalTerm;
        std::function< void() > termCB = nullptr;
    } context_;

public:
    /// @brief Constructor
    /// @param termCB - This function will be called after the process receives the SIGTERM signal.
    TerminatingHandler(std::function< void() > termCB = nullptr) { context_.termCB = termCB; }

    ~TerminatingHandler(void)
    {
        if (context_.mainLoop) {
            nai_signal_close(&context_.signalTerm);
        }
        context_.mainLoop = nullptr;
    };

    /// @brief Initialize (register) termination signal response
    /// @retval 0 - Success
    /// @retval <0 - Failure, the user should end the process after this function call fails, otherwise the process will not be able to exit normally
    /// @note Unless the mainLoop has finished dispatching, the destructor of TerminatingHandler must be in the dispatching thread of the evloop
    int Register(std::shared_ptr< isoft::naicpp::EvLoop > evloop = nullptr) noexcept
    {
        if (context_.mainLoop != nullptr)
            return -1;

        if (evloop == nullptr) {
            evloop = isoft::naicpp::GlobalGeneralEvLoop::Get();
        }

        if (nullptr == evloop)
            return -2;

        nai_signal_init(&context_.signalTerm);
        nai_signal_set_cb(&context_.signalTerm, signalTermCb);

        if (0 > nai_signal_open(&context_.signalTerm, evloop->GetRawEvLoop(), SIGTERM))
            return -3;
        context_.mainLoop = evloop;  // init successed
        return 0;
    }

private:
    /// @brief NAI signal callback function
    static int signalTermCb(nai_signal_t *sig, int events)
    {
        Context *ctx = nai_containof(sig, Context, signalTerm);
        if (NAI_EV_SIGNAL != (events & NAI_EV_MASK))
            return 0;
        if (SIGTERM == NAI_EV_SIGNAL_CODE(events)) {
            ctx->mainLoop->Stop();

            if (ctx->termCB != nullptr) {
                ctx->termCB();
            }

            nai_signal_close(&ctx->signalTerm);
            ctx->mainLoop = nullptr;
        }
        return 0;
    }

};  /// class TerminatingHandler

}  // namespace naicpp
}  // namespace isoft
#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_TERMINIATING_HANDLER_H
