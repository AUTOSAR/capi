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
/// @file       communication_control.cpp
/// @brief      This file provides the implementation of CommunicationControl.
/// @details
/// @date       2022-07-25
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/communication_control.h"

#include <ara/core/instance_specifier.h>

#include "ara/diag/diag_error_domain.h"
#include "gen_code/communication_control/apiAgent/communication_control_agent.h"
#include "resolve.h"

namespace ara {
namespace diag {

/// @brief Class for an CommunicationControl
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an diagnostic
/// CommunicationControl interface
/// @param[in] reentrancyType specifies if interface is callable fully- or
/// non-reentrant
/// @throws on overflow
///
/// @traceid{SWS_DM_00806}@tracestatus{draft}
CommunicationControl::CommunicationControl(ara::core::InstanceSpecifier const &specifier,  // NOLINT
                                           ReentrancyType reentrancyType)                  // NOLINT
    : specifier_{specifier}, reentrancyType_{reentrancyType}, skeleton_{}
{
}

/// @brief This Offer will enable the DM to forward request messages to this
/// handler
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return ara::core::Result<void> error code:kNotOffered, kGenericError,
/// kAlreadyOffered
/// @throws on overflow
///
/// @traceid{SWS_DM_00809}@tracestatus{draft}
ara::core::Result< void > CommunicationControl::Offer()
{
    if (skeleton_ != nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kAlreadyOffered);
    }
    ara::core::Result< internal::InstanceInfo > const result{internal::Resolve(specifier_)};

    if (!result.HasValue()) {
        return ara::core::Result< void >::FromError(result.Error());
    }

    skeleton_ = std::make_shared< isoft::dm::dis::CommunicationControlAgent >(result.Value().serviceInstanceId);
    skeleton_->RegisterService(this);
    skeleton_->SetReentrancyType(reentrancyType_);
    return {};
}

/// @brief This StopOffer will disable the forwarding of request messages from
/// DM
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @throws on overflow
/// @traceid{SWS_DM_00810}@tracestatus{draft}
void CommunicationControl::StopOffer()
{
    if (skeleton_ != nullptr) {
        skeleton_.reset();
    }
}

}  // namespace diag
}  // namespace ara
