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
/// @file       authentication.cpp
/// @brief
/// @details
/// @date       2022-07-25
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/instance_specifier.h>
#include <ara/diag/authentication.h>

#include "ara/diag/diag_error_domain.h"
#include "gen_code/authentication/apiAgent/authentication_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {

Authentication::Authentication(ara::core::InstanceSpecifier const &specifier,  // NOLINT
                               ReentrancyType reentrancyType)                  // NOLINT
    : specifier_{specifier}, reentrancyType_{reentrancyType}, skeleton_{}
{
}

/// @brief This Offer will enable the DM to forward request messages to this
/// handler
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return ara::core::Result<void> error code: kNotOffered, kGenericError,
/// kAlreadyOffered
/// @throws on overflow
///
/// @traceid{SWS_DM_00792}@tracestatus{draft}
ara::core::Result< void > Authentication::Offer()
{
    if (skeleton_.get() != nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kAlreadyOffered);
    }
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier_)};
    if (!retrieveResult.HasValue()) {
        return ara::core::Result< void >::FromError(retrieveResult.Error());
    }
    skeleton_
        = std::make_shared< isoft::dm::dis::AuthenticationAgent >(std::move(retrieveResult).Value().serviceInstanceId);
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
/// @traceid{SWS_DM_00793}@tracestatus{draft}
void Authentication::StopOffer()
{
    if (skeleton_ != nullptr) {
        skeleton_.reset();
    }
}
}  // namespace diag
}  // namespace ara
