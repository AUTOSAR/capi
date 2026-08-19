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
/// @file       signal_manager.cpp
/// @brief      This file provides the Signal Management class
/// @details
/// @date       2022-07-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "signal_manager.h"

#include <isoft/naicpp/global_evloop.h>
#include <nai/runtime/nai_errno.h>

namespace ara {
namespace diag {
namespace dmd {

/// @brief Destructor
SignalManager::~SignalManager() noexcept
{
    for (decltype(auto) it : handlerTable_) {
        std::ignore = nai_signal_close(it.second.get());
    }
}

/// @brief Process signal handling
/// @param[in] node
/// @param[in] events
/// @return Processing code
std::int32_t SignalManager::Process(nai_signal_t* const node, std::int32_t const events) noexcept
{
    SignalHandler* const handler{reinterpret_cast< SignalHandler* >(node)};
    if ((events & NAI_EV_MASK) == NAI_EV_SIGNAL) {
        if (static_cast< std::int32_t >(NAI_EV_SIGNAL_CODE(events)) == handler->GetNumber()) {
            handler->ExecProcessor();
            nai_signal_close(node);
        }
    }
    return 0;
}

/// @brief Register signal
/// @param[in] number Signal
/// @param[in] processor Signal trigger callback
/// @return Registration result
std::int32_t SignalManager::Register(std::int32_t number, std::function< void(std::int32_t) >&& processor) noexcept
{
    std::pair< ara::core::Map< std::int32_t, std::shared_ptr< SignalHandler > >::iterator, bool > const it{
        handlerTable_.emplace(number, std::make_shared< SignalHandler >())};
    decltype(auto) handler{it.first->second};
    handler->SetNumber(number);
    nai_signal_init(handler.get()) handler->SetProcessor(std::move(processor));

    if (nai_signal_set_cb(handler.get(), Process) == -1) {
        std::ignore = handlerTable_.erase(number);
        return kRegisterCodeFail_2;
    }
    std::shared_ptr< isoft::naicpp::EvLoop > loop{isoft::naicpp::GlobalGeneralEvLoop::Get()};
    if (nai_signal_open(handler.get(), loop->GetRawEvLoop(), number) == -1) {
        std::ignore = handlerTable_.erase(number);
        return kRegisterCodeFail_1;
    }
    return kRegisterCodeSuccess;
}

void SignalManager::Close()
{
    for (decltype(auto) it : handlerTable_) {
        std::ignore = nai_signal_close(it.second.get());
    }
    handlerTable_.clear();
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
