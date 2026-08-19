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
/// @file       event.h
/// @brief      This file provides the definitions of Event and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_EVENT_H_
#define ARA_DIAG_EVENT_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class EventAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the type of the DTC format according to ISO 14229-1.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00642}@tracestatus{draft}
enum class DTCFormatType : std::uint8_t
{
    kDTCFormatOBD   = 0,  ///< SAE_J2012-DA_DTCFormat_00 as defined in ISO 15031-6 specification.
    kDTCFormatUDS   = 1,  ///< ISO_14229-1_DTCFormat     as defined in ISO 14229-1 specification.
    kDTCFormatJ1939 = 2   ///< SAE_J1939-73_DTCFormat    as defined in SAE J1939-73.
};

/// @brief Single event status bits
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00643}@tracestatus{draft}
enum class EventStatusBit : std::uint8_t
{
    kTestFailed,                         ///< bit 0: TestFailed
    kTestFailedThisOperationCycle,       ///< bit 1: TestFailedThisOperationCycle
    kTestNotCompletedThisOperationCycle  ///< bit 6: TestNotCompletedThisOperationCycle
};
namespace api {
/// @brief Declare EventProxyWrapper
class EventProxyWrapper;
}  // namespace api

/// @brief Class to implement operations on diagnostic Events.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00646}@tracestatus{draft}
class Event
{
public:
    /// @brief Current event status byte, bit-encoded
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00644}@tracestatus{draft}
    struct EventStatusByte
    {
        std::uint8_t eventStatusByte_{0U};  /// NOLINT
    };

    /// @brief Debounce status of event .
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00645}@tracestatus{draft}
    enum class DebouncingState : std::uint8_t
    {
        kNeutral              = 0x00,  ///< Neutral (corresponds to FDC = 0)
        kTemporarilyDefective = 0x01,  ///< Temporarily Defective (corresponds to 0 < FDC < 127)
        kFinallyDefective     = 0x02,  ///< finally Defective (corresponds to FDC = 127)
        kTemporarilyHealed    = 0x04,  ///< temporarily healed (corresponds to -128 < FDC < 0)
        kFinallyHealed        = 0x08   ///< finally healed (corresponds to FDC = -128)
    };

    /// @brief Constructor fct. for objects of class Event.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticEventInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00647}@tracestatus{draft}
    explicit Event(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of class Event
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00648}@tracestatus{draft}
    ~Event() noexcept = default;

    /// @brief copy constructor
    /// @param other
    Event(Event const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return EcuResetRequest&
    Event& operator=(Event const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    Event(Event&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return EcuResetRequest&
    Event& operator=(Event&& other) noexcept = default;

    /// @brief Returns the current diagnostic event status
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current diagnostic event status
    ///
    ///
    /// @traceid{SWS_DM_00649}@tracestatus{draft}
    ara::core::Result< EventStatusByte > GetEventStatus();

    /// @brief Register a notifier function which is called if a diagnostic event is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if a diagnostic event is changed.
    /// @return ara::core::Result<void> error code: kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00650}@tracestatus{draft}
    ara::core::Result< void > SetEventStatusChangedNotifier(std::function< void(EventStatusByte) > notifier);

    /// @brief Returns the current warning indicator status
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current warning indicator status
    ///
    ///
    /// @traceid{SWS_DM_00651}@tracestatus{draft}
    ara::core::Result< bool > GetLatchedWIRStatus();

    /// @brief Set the warning indicator status
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] status Limp-home status as determined by the AA. '0' means limp-home not active; '1' means limp-home
    ///>    active;
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00652}@tracestatus{draft}
    ara::core::Result< void > SetLatchedWIRStatus(bool status);

    /// @brief Returns the DTC-ID related to this event instance.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dtcFormat Define DTC format for the return value.
    /// @return DTC number in respective DTCFormatType. error code: kGenericError
    ///
    ///
    /// @traceid{SWS_DM_00653}@tracestatus{draft}
    ara::core::Result< std::uint32_t > GetDTCNumber(DTCFormatType dtcFormat);

    /// @brief Get the current debouncing status .
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return Return the current debouncing state of this event.
    ///
    ///
    /// @traceid{SWS_DM_00654}@tracestatus{draft}
    ara::core::Result< DebouncingState > GetDebouncingStatus();

    /// @brief Get the status if the event has matured to test completed (corresponds to FDC = -128 or FDC = 127).
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return Return the current test_completed-state of this event. "true", if FDC = -128 or FDC = 127; "false" in
    /// all other cases.
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00655}@tracestatus{draft}
    ara::core::Result< bool > GetTestComplete();

    /// @brief Returns the current value of Fault Detection Counter of this event.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return current FaultDetectionCounter value.
    ///
    ///
    /// @traceid{SWS_DM_00656}@tracestatus{draft}
    ara::core::Result< std::int8_t > GetFaultDetectionCounter();

private:
    std::shared_ptr< isoft::dm::dic::EventAgent >
        /// @name proxy_
        proxy_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_EVENT_H_