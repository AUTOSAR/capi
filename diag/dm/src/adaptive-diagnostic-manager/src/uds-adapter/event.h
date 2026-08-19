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
/// @brief      This file provides the definition of Event and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG__FAULT_EVENT_H__
#define __ARA_DIAG__FAULT_EVENT_H__
#include <isoft/uds/fault_management/event_interface.h>

#include "gen_code/event/serviceAgent/event_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class Event : public isoft::uds::server::EventInterface
{
public:
    Event(Event const&) = delete;
    Event(Event&&)      = delete;
    Event& operator=(Event const&) = delete;
    Event& operator=(Event&&) = delete;
    explicit Event(uint32_t const& serviceInstanceId) noexcept;
    ~Event() noexcept override = default;

public:
    /// @brief Notify AA side of event status changes
    /// @param status Event status
    /// @return Returns 0 for successful call
    std::int32_t NotifyEventStatus(uint8_t const status) override;

    /// @brief Register callback for AA side to get event status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetEventStatus(isoft::uds::server::GetEventStatusCallBack const& callBack) override;

    /// @brief Register callback for AA side to get WIR status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetLatchedWIRStatus(isoft::uds::server::GetLatchedWIRStatusCallBack const& callBack) override;

    /// @brief Register callback for AA side to set WIR status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetLatchedWIRStatus(isoft::uds::server::SetLatchedWIRStatusCallBack const& callBack) override;

    /// @brief Register callback for AA side to get DTC number
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetDTCNumber(isoft::uds::server::GetDTCNumberCallBack const& callBack) override;

    /// @brief Register callback for AA side to get debounce status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetDebouncingStatus(isoft::uds::server::GetDebouncingStatusCallBack const& callBack) override;

    /// @brief Register callback for AA side to get FDC
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetFaultDetectionCounter(
        isoft::uds::server::GetFaultDetectionCounterCallBack const& callBack) override;

    /// @brief Register callback for AA side to get the test status of the event
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetTestComplete(isoft::uds::server::GetTestCompleteCallBack const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::EventAgent > eventAgent_;

    isoft::uds::server::GetEventStatusCallBack getEventStatusCallBack_{nullptr};
    isoft::uds::server::GetLatchedWIRStatusCallBack getLatchedWIRStatusCallBack_{nullptr};
    isoft::uds::server::SetLatchedWIRStatusCallBack setLatchedWIRStatusCallBack_{nullptr};
    isoft::uds::server::GetDTCNumberCallBack getDTCNumberCallBack_{nullptr};
    isoft::uds::server::GetDebouncingStatusCallBack getDebouncingStatusCallBack_{nullptr};
    isoft::uds::server::GetFaultDetectionCounterCallBack getFaultDetectionCounterCallBack_{nullptr};
    isoft::uds::server::GetTestCompleteCallBack getTestCompleteCallBack_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_