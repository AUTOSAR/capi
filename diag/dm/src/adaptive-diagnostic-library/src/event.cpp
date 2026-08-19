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
/// @file       event.cpp
/// @brief      This file provides the implementation of Event.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/event.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/event/clientAgent/event_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {

/// @brief Constructor fct. for objects of class Event.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticEventInterface
/// @throws on overflow
///
/// @traceid{SWS_DM_00647}@tracestatus{draft}
Event::Event(ara::core::InstanceSpecifier const &specifier)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::EventAgent >(retrieveResult.Value().instanceId,
                                                                retrieveResult.Value().serviceInstanceId);
    } else {
        internal::LogError() << "Event::Event|not found specifier =" << std::move(specifier.ToString());
    }
}

/// @brief Returns the current diagnostic event status
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current diagnostic event status
/// @throws on overflow
///
/// @traceid{SWS_DM_00649}@tracestatus{draft}
ara::core::Result< Event::EventStatusByte > Event::GetEventStatus()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetEventStatus|proxy is null.";
        return ara::core::Result< Event::EventStatusByte >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetEventStatus();
}

/// @brief Register a notifier function which is called if a diagnostic event is
/// changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if a diagnostic event is
/// changed.
/// @return ara::core::Result<void> error code: kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00650}@tracestatus{draft}
ara::core::Result< void > Event::SetEventStatusChangedNotifier(
    std::function< void(Event::EventStatusByte) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::SetEventStatusChangedNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "Event::SetEventStatusChangedNotifier|service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterEventStatusNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Returns the current warning indicator status
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current warning indicator status
/// @throws on overflow
///
/// @traceid{SWS_DM_00651}@tracestatus{draft}
ara::core::Result< bool > Event::GetLatchedWIRStatus()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetLatchedWIRStatus|proxy is null.";
        return ara::core::Result< bool >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetLatchedWIRStatus();
}

/// @brief Set the warning indicator status
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] status Limp-home status as determined by the AA. '0' means
/// limp-home not active; '1' means limp-home
///> active;
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00652}@tracestatus{draft}
ara::core::Result< void > Event::SetLatchedWIRStatus(bool status)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::SetLatchedWIRStatus|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->SetLatchedWIRStatus(status);
}

/// @brief Returns the DTC-ID related to this event instance.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] dtcFormat Define DTC format for the return value.
/// @return DTC number in respective DTCFormatType. error code: kGenericError
/// @throws on overflow
///
/// @traceid{SWS_DM_00653}@tracestatus{draft}
ara::core::Result< std::uint32_t > Event::GetDTCNumber(DTCFormatType dtcFormat)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetDTCNumber|proxy is null.";
        return ara::core::Result< std::uint32_t >::FromError(ara::diag::DiagErrc::kFailed);
    }

    return proxy_->GetDTCNumber(dtcFormat);
}

/// @brief Get the current debouncing status .
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return Return the current debouncing state of this event.
/// @throws on overflow
///
/// @traceid{SWS_DM_00654}@tracestatus{draft}
ara::core::Result< Event::DebouncingState > Event::GetDebouncingStatus()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetDebouncingStatus|proxy is null.";
        return ara::core::Result< Event::DebouncingState >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetDebouncingStatus();
}

/// @brief Get the status if the event has matured to test completed
/// (corresponds to FDC = -128 or FDC = 127).
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return Return the current test_completed-state of this event. "true", if
/// FDC = -128 or FDC = 127; "false" in all other cases.
/// @throws on overflow
///
/// @traceid{SWS_DM_00655}@tracestatus{draft}
ara::core::Result< bool > Event::GetTestComplete()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetTestComplete|proxy is null.";
        return ara::core::Result< bool >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetTestComplete();
}

/// @brief Returns the current value of Fault Detection Counter of this event.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return current FaultDetectionCounter value.
/// @throws on overflow
///
/// @traceid{SWS_DM_00656}@tracestatus{draft}
ara::core::Result< std::int8_t > Event::GetFaultDetectionCounter()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Event::GetFaultDetectionCounter|proxy is null.";
        return ara::core::Result< std::int8_t >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetFaultDetectionCounter();
}
}  // namespace diag
}  // namespace ara