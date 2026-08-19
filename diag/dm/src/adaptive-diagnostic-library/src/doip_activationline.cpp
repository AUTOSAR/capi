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
/// @file       doip_activationline.cpp
/// @brief      This file provides the implementation of DoIPActivationLine.
/// @details
/// @date       2022-07-25
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/doip_activationline.h"

#include <ara/core/instance_specifier.h>

#include "ara/diag/diag_error_domain.h"
#include "gen_code/activation_line/clientAgent/activation_line_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {
/// @brief Constructor of DoIPActivationLine
/// @param specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticDoIPActivationLineInterface
///
/// @traceid{SWS_DM_00831}@tracestatus{draft}
DoIPActivationLine::DoIPActivationLine(ara::core::InstanceSpecifier const &specifier) noexcept  /// NOLINT
    : skeleton_{nullptr}, instanceSpecifier_{specifier}
{
}
/// @brief This Offer will enable the DM to listen to activation line state
/// changes for the given interface.
/// @return ara::core::Result<void> error code : kNotOffered, kGenericError,
/// kAlreadyOffered
///
/// @traceid{SWS_DM_00836}@tracestatus{draft}
ara::core::Result< void > DoIPActivationLine::Offer() noexcept
{
    if (skeleton_ != nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kAlreadyOffered);
    }

    ara::core::Result< internal::InstanceInfo > const result{internal::Resolve(instanceSpecifier_)};
    if (!result.HasValue()) {
        return ara::core::Result< void >::FromError(result.Error());
    }

    skeleton_ = std::make_shared< isoft::dm::dis::ActivationLineAgent >(result.Value().serviceInstanceId);
    skeleton_->RegisterService(this);
    return {};
}
/// @brief This StopOffer will disable the provision of activation line state to
/// DM.
///
/// @traceid{SWS_DM_00837}@tracestatus{draft}
void DoIPActivationLine::StopOffer() noexcept
{
    if (skeleton_ != nullptr) {
        skeleton_.reset();
    }
}
/// @brief Called to update current activation line state.
/// @param state TRUE in case the activation line is active, else FALSE.
///
/// @traceid{SWS_DM_00834}@tracestatus{draft}
void DoIPActivationLine::UpdateActivationLineState(bool const state) noexcept
{
    skeleton_->UpdateActivationLineState(state);
}

}  // namespace diag
}  // namespace ara
