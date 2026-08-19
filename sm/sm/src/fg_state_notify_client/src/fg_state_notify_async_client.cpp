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
/// @file       fg_state_notify_async_client.cpp
/// @brief      Implementation of function group state information asynchronous notification client
/// @details
/// @date       2024-06-25
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyClient
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @unit_name=FGStateNotifyAsyncClient
/// @unit_description=Function group state information asynchronous notification client
/// @endcode
///
/// ================================================================

#include <ara/sm/internal/fg_state_notify_async_client.h>
#include <ara/sm/internal/fg_state_notify_error_domain.h>

#include "client.h"
#include "fg_state_internal_type.h"

namespace ara {
namespace sm {
namespace fg_state_notify_client {

/// @brief Client instance
std::unique_ptr< FGStateNotifyAsyncClient > FGStateNotifyAsyncClient::s_Instance_{nullptr};  // NOLINT

/// @brief Get instance
/// @return
FGStateNotifyAsyncClient *FGStateNotifyAsyncClient::GetInstance() noexcept
{
    static std::once_flag s_Flag{};
    std::call_once(s_Flag, []() { s_Instance_.reset(new FGStateNotifyAsyncClient()); });
    return s_Instance_.get();
}

/// @brief Destroy instance
void FGStateNotifyAsyncClient::Destroy() const noexcept
{
    clientImpl_->Destroy();
    s_Instance_.reset();
}

/// @brief Constructor
FGStateNotifyAsyncClient::FGStateNotifyAsyncClient() noexcept : clientImpl_{std::make_unique< fg_state_ipc::Client >()}
{
}

/// @brief Initialize
/// @param clientID
/// @return
core::Result< void > FGStateNotifyAsyncClient::Init(core::String const &clientID) const noexcept
{
    fg_state_ipc::FGStateNotifyResult const ret{clientImpl_->Init(clientID)};
    if (fg_state_ipc::FGStateNotifyResult::kSuccess == ret) {
        return {};
    }
    return core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(ret));
}

/// @brief Request all states
/// @param requestFGStateRespMsgHandler
/// @param timeout
/// @return
core::Result< void > FGStateNotifyAsyncClient::RequestAllFGState(
    std::function< RequestFGStateRespMsgHandlerType > const &requestFGStateRespMsgHandler,
    int32_t const &timeout) const noexcept
{
    std::function< void(
        fg_state_ipc::FGStateNotifyResult const,
        core::Vector< fg_state_ipc::FGStateInternalType > const) > const requestFGStateRespMsgHandlerImpl{
        [requestFGStateRespMsgHandler](fg_state_ipc::FGStateNotifyResult const ret,
                                       core::Vector< fg_state_ipc::FGStateInternalType > const &&allFGStateInternal) {
            core::Vector< FGStateType > allFGStates;
            for (const auto &FGInternal : allFGStateInternal) {
                FGStateType FG;
                FG.fgName  = FGInternal.fgName;
                FG.fgState = FGInternal.fgState;
                allFGStates.emplace_back(FG);
            }
            core::Result< void > res;
            if (fg_state_ipc::FGStateNotifyResult::kSuccess != ret) {
                res = core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(ret));
            }
            requestFGStateRespMsgHandler(res, allFGStates);
        }};
    fg_state_ipc::FGStateNotifyResult const internalRes{
        clientImpl_->RequestAllFGState(requestFGStateRespMsgHandlerImpl, timeout)};
    if (fg_state_ipc::FGStateNotifyResult::kSuccess != internalRes) {
        return core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(internalRes));
    }
    return core::Result< void >::FromValue();
}

/// @brief Subscribe to FG state
/// @param subscribeRespMsgHandler
/// @param fgStateChangeHandler
/// @param timeout
/// @return
core::Result< void > FGStateNotifyAsyncClient::SubscribeFGState(
    std::function< SubscribeRespMsgHandlerType > const &subscribeRespMsgHandler,
    std::function< void(FGStateType &fgState) > const &fgStateChangeHandler,
    int32_t const &timeout) const noexcept
{
    std::function< void(fg_state_ipc::FGStateNotifyResult const) > const subscribeRespMsgHandlerImpl{
        [subscribeRespMsgHandler](fg_state_ipc::FGStateNotifyResult const ret) {
            core::Result< void > res;
            if (fg_state_ipc::FGStateNotifyResult::kSuccess != ret) {
                res = core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(ret));
            }
            subscribeRespMsgHandler(res);
        }};

    std::function< void(fg_state_ipc::FGStateInternalType const) > const fgStateChangeHandlerImpl{
        [fgStateChangeHandler](fg_state_ipc::FGStateInternalType const &fgState) {
            FGStateType state;
            state.fgName  = fgState.fgName;
            state.fgState = fgState.fgState;
            fgStateChangeHandler(state);
        }};
    fg_state_ipc::FGStateNotifyResult const internalRes{
        clientImpl_->SubscribeFGState(subscribeRespMsgHandlerImpl, fgStateChangeHandlerImpl, timeout)};
    core::Result< void > res{};
    if (fg_state_ipc::FGStateNotifyResult::kSuccess != internalRes) {
        res = core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(internalRes));
    }
    return res;
}

/// @brief Unsubscribe from FG state
/// @param unsubscribeRespMsgHandler
/// @param timeout
/// @return
core::Result< void > FGStateNotifyAsyncClient::UnsubscribeFGState(
    std::function< UnsubscribeRespMsgHandlerType > const &unsubscribeRespMsgHandler,
    int32_t const &timeout) const noexcept
{
    std::function< void(fg_state_ipc::FGStateNotifyResult const) > const unsubscribeRespMsgHandlerImpl{
        [unsubscribeRespMsgHandler](fg_state_ipc::FGStateNotifyResult const ret) {
            core::Result< void > res;
            if (fg_state_ipc::FGStateNotifyResult::kSuccess != ret) {
                res = core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(ret));
            }
            unsubscribeRespMsgHandler(res);
        }};
    fg_state_ipc::FGStateNotifyResult const internalRes{
        clientImpl_->UnsubscribeFGState(unsubscribeRespMsgHandlerImpl, timeout)};
    core::Result< void > res{};
    if (fg_state_ipc::FGStateNotifyResult::kSuccess != internalRes) {
        res = core::Result< void >::FromError(static_cast< FGStateNotifyErrc >(internalRes));
    }
    return res;
}

}  // namespace fg_state_notify_client
}  // namespace sm
}  // namespace ara
