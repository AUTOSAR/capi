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
/// @file       condition.cpp
/// @brief      This file provides the implementation of Condition.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/condition.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/condition/clientAgent/condition_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {
/// @brief Constructor of Condition Class
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticConditionInterface
/// @throws on overflow
///
/// @traceid{SWS_DM_00712}@tracestatus{draft}

Condition::Condition(ara::core::InstanceSpecifier const& specifier)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::ConditionAgent >(retrieveResult.Value().instanceId,
                                                                    retrieveResult.Value().serviceInstanceId);
    } else {
        internal::LogError() << "Condition::Condition -> not found specifier=" << std::move(specifier.ToString());
    }
}

/// @brief Get current condition
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current condition
/// @throws on overflow
///
/// @traceid{SWS_DM_00714}@tracestatus{draft}
ara::core::Result< ConditionType > Condition::GetCondition()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Condition::GetCondition -> proxy is null.";
        return ara::core::Result< ConditionType >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetCondition();
}

/// @brief Set condition
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] condition current condition
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00715}@tracestatus{draft}
ara::core::Result< void > Condition::SetCondition(ConditionType condition)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Condition::SetCondition -> proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->SetCondition(condition);
}
}  // namespace diag
}  // namespace ara