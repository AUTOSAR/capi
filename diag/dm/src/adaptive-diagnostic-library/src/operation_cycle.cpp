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
/// @file       operation_cycle.cpp
/// @brief      This file provides the implementation of OperationCycle.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/operation_cycle.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/operationCycle/clientAgent/operationCycle_agent.h"
#include "resolve.h"
#include "utility.h"

namespace ara {
namespace diag {
/// @brief Constructor for DiagnosticOperationCycleInterface
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticOperationCycleInterface
/// @throws on overflow
///
/// @traceid{SWS_DM_00752}@tracestatus{draft}
OperationCycle::OperationCycle(ara::core::InstanceSpecifier const &specifier)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::OperationCycleAgent >(retrieveResult.Value().instanceId,
                                                                         retrieveResult.Value().serviceInstanceId);
    } else {
        internal::LogError() << "OperationCycle::OperationCycle|not found specifier="
                             << std::move(specifier.ToString());
    }
}

/// @brief Get current OperationCycle
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @returns the current OperationCycle
/// @throws on overflow
///
/// @traceid{SWS_DM_00754}@tracestatus{draft}
ara::core::Result< OperationCycleType > OperationCycle::GetOperationCycle()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "OperationCycle::GetOperationCycle|proxy is null.";
        return ara::core::Result< OperationCycleType >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetOperationCycle();
}

/// @brief Registering a notifier function which is called if the operation
/// cycle is changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier operation cycle changed callBack
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00755}@tracestatus{draft}
ara::core::Result< void > OperationCycle::SetNotifier(std::function< void(OperationCycleType) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "OperationCycle::SetNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "OperationCycle::SetNotifier|service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterOperationCycleNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Set OperationCycle
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] operationCycle current OperationCycle
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00756}@tracestatus{draft}
ara::core::Result< void > OperationCycle::SetOperationCycle(OperationCycleType operationCycle)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "OperationCycle::SetOperationCycle|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->SetOperationCycle(operationCycle);
}

}  // namespace diag
}  // namespace ara
