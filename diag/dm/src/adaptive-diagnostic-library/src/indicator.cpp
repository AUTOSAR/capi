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
/// @file       indicator.cpp
/// @brief      This file provides the implementation of Indicator.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/indicator.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/indicator/clientAgent/indicator_agent.h"
#include "resolve.h"
#include "utility.h"

namespace ara {
namespace diag {
/// @brief Constructor for DiagnosticIndicatorInterface
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticIndicatorInterface
/// @throws on overflow
///
/// @traceid{SWS_DM_00742}@tracestatus{draft}
Indicator::Indicator(ara::core::InstanceSpecifier const &specifier)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::IndicatorAgent >(retrieveResult.Value().instanceId,
                                                                    retrieveResult.Value().serviceInstanceId);
    } else {
        internal::LogError() << "Indicator::Indicator -> not found specifier=" << std::move(specifier.ToString());
    }
}

/// @brief Get current Indicator
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current Indicator
/// @throws on overflow
///
/// @traceid{SWS_DM_00744}@tracestatus{draft}
ara::core::Result< IndicatorType > Indicator::GetIndicator()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Indicator::GetIndicator -> proxy is null.";
        return ara::core::Result< IndicatorType >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetIndicator();
}

/// @brief Register a notifier function which is called if the indicator is
/// updated.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier notifier function
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00745}@tracestatus{draft}
ara::core::Result< void > Indicator::SetNotifier(std::function< void(IndicatorType) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Indicator::SetNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "Indicator::SetNotifier|service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterIndicatorNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}
}  // namespace diag
}  // namespace ara
