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
/// @file       campaign_state_processing.cpp
/// @brief      CampaignStateProcessing implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=CampaignStateProcessing
/// @unit_description=CampaignStateProcessing implementation
/// @endcode
///
/// ================================================================

#include "fsm/states/campaign_state_processing.h"

#include "fsm/fsm_manager.h"
#include "fsm/states/campaign_state_activating.h"
#include "fsm/states/campaign_state_canceling.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief DoSoftwareTransfer
/// @param accessor
/// @param spkgCB
/// @return int
/// @throws no
int32_t CampaignStateProcessing::DoSoftwareTransfer(StateAccessor const& accessor, SpkgCampaignCBType const& spkgCB)
{
    std::ignore = accessor;
    if (spkgCB) {
        return spkgCB();
    }
    LOG_ERROR << "can't run this code, please check logical!!!";
    return 0;
}

/// @brief DoActivate
/// @param accessor
/// @param activateCB
/// @return int
/// @throws no
int32_t CampaignStateProcessing::DoActivate(StateAccessor const& accessor, ActivateCBType const& activateCB)
{
    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateActivating >()};
    accessor.Reset(std::move(stateUPtr));

    if (activateCB) {
        return activateCB();
    }
    LOG_ERROR << "can't run this code, please check logical!!!";
    return 0;
}

/// @brief OnCancel
/// @param accessor
/// @param cancelCause
/// @param cancelCB
/// @param promise
/// @return int
/// @throws no
int32_t CampaignStateProcessing::OnCancel(StateAccessor const& accessor,
                                          pkgmgr::UCMMasterResolutionType cancelCause,
                                          CancelCBType const& cancelCB,
                                          ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "CampaignStateProcessing::OnCancel(), begin with cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding_, cancelCause)
                           .c_str();

    // Mark Campaign cancellation
    FsmManager::GetInstance()->SetCampaignCancelFlag();

    // If active, wait for FsmManager::ExecuteProcessStep to end
    FsmManager::GetInstance()->WaitProcessingFinish();

    // Reset Campaign cancellation mark
    FsmManager::GetInstance()->ResetCampaignCancelFlag();

    // Switch to CANCELLING state;
    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateCanceling >(cancelCause)};
    accessor.Reset(std::move(stateUPtr));

    // Execute callback
    std::ignore = cancelCB(static_cast< std::underlying_type< pkgmgr::UCMMasterResolutionType >::type >(cancelCause),
                           std::move(promise));

    LOG_DEBUG << "CampaignStateProcessing::OnCancel(), end.";
    return 0;
}

/// @brief OnAbort
/// @throws no
void CampaignStateProcessing::OnAbort() noexcept
{
    log_.LogDebug() << "CampaignStateProcessing::OnAbort(), begin.";

    // If active, wait for FsmManager::ExecuteProcessStep to end
    FsmManager::GetInstance()->WaitProcessingFinish();

    log_.LogDebug() << "CampaignStateProcessing::OnAbort(), end.";
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara