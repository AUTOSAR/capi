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
/// @file       doip_power_mode.cpp
/// @brief      This file provides the implementation of DoIPPowerMode.
/// @details
/// @date       2022-08-22
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/doip_power_mode.h"

#include <ara/core/instance_specifier.h>

#include "ara/diag/diag_error_domain.h"
#include "gen_code/power_mode/clientAgent/power_mode_agent.h"
#include "resolve.h"
#include "utility.h"

namespace ara {
namespace diag {
/// @brief Constructor of DoIPPowerMode
/// @param specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticDoIPPowerModeInterface
/// @param reentrancyType specifies if interface is callable fully- or
/// non-reentrant
///
/// @traceid{SWS_DM_00732}@tracestatus{draft}
DoIPPowerMode::DoIPPowerMode(ara::core::InstanceSpecifier const &specifier,  /// NOLINT
                             ReentrancyType const reentrancyType) noexcept
    : skeleton_{nullptr}, instanceSpecifier_{specifier}, reentrancyType_{reentrancyType}
{
}
/// @brief This Offer will enable the DM to forward request messages to this
/// handler
/// @return ara::core::Result<void> error code: kNotOffered, kGenericError,
/// kAlreadyOffered
///
/// @traceid{SWS_DM_00735}@tracestatus{draft}
ara::core::Result< void > DoIPPowerMode::Offer() noexcept
{
    if (skeleton_ != nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kAlreadyOffered);
    }

    ara::core::Result< internal::InstanceInfo > const result{internal::Resolve(instanceSpecifier_)};
    if (!result.HasValue()) {
        return ara::core::Result< void >::FromError(result.Error());
    }

    skeleton_ = std::make_shared< isoft::dm::dis::PowerModeAgent >(result.Value().serviceInstanceId);
    skeleton_->RegisterService(this);
    skeleton_->SetReentrancyType(reentrancyType_);
    return {};
}
/// @brief This StopOffer will disable the forwarding of request messages from
/// DM
///
/// @traceid{SWS_DM_00736}@tracestatus{draft}
void DoIPPowerMode::StopOffer() noexcept
{
    if (skeleton_ != nullptr) {
        skeleton_.reset();
    }
}

}  // namespace diag
}  // namespace ara
