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
/// @file       event_interface.h
/// @brief      This file provides event_interface functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=da1fef1c-313f-482e-8d60-9b2358750987
/// @unit_name=EventInterface
/// @unit_description=This file provides event_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_EVENT_INTERFACE_H__
#define __ISOFT_UDS_FAULT_EVENT_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace uds {
namespace server {
/// @brief Event status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01370
/// @trace_id_dd=DD_UDS_02662
/// @needwork = ad
/// @endcode
enum class DebouncingState : std::uint8_t
{

    kNeutral = 0x00,  ///< Neutral (corresponds to FDC = 0)

    kTemporarilyDefective = 0x01,  ///< Temporarily Defective (corresponds to 0 < FDC < 127)

    kFinallyDefective = 0x02,  ///< finally Defective (corresponds to FDC = 127)

    kTemporarilyHealed = 0x04,  ///< temporarily healed (corresponds to -128 < FDC < 0)

    kFinallyHealed = 0x08  ///< finally healed (corresponds to FDC = -128)
};

/// @brief Get event status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01371
/// @trace_id_dd=DD_UDS_02663
/// @needwork = ad
/// @endcode
using GetEventStatusCallBack = std::function< std::uint8_t() >;
/// @brief Get latched WIR status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01372
/// @trace_id_dd=DD_UDS_02664
/// @needwork = ad
/// @endcode
using GetLatchedWIRStatusCallBack = std::function< bool() >;
/// @brief Set latched WIR status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01373
/// @trace_id_dd=DD_UDS_02665
/// @needwork = ad
/// @endcode
using SetLatchedWIRStatusCallBack = std::function< int32_t(bool) >;
/// @brief Get DTC count callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01374
/// @trace_id_dd=DD_UDS_02666
/// @needwork = ad
/// @endcode
using GetDTCNumberCallBack = std::function< std::int32_t(std::uint8_t const&, std::uint32_t&) >;
/// @brief Get debounce status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01375
/// @trace_id_dd=DD_UDS_02667
/// @needwork = ad
/// @endcode
using GetDebouncingStatusCallBack = std::function< DebouncingState() >;
/// @brief Get fault detection counter callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01376
/// @trace_id_dd=DD_UDS_02668
/// @needwork = ad
/// @endcode
using GetFaultDetectionCounterCallBack = std::function< std::int8_t() >;
/// @brief Get test completion callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01377
/// @trace_id_dd=DD_UDS_02669
/// @needwork = ad
/// @endcode
using GetTestCompleteCallBack = std::function< bool() >;
/// @brief Event interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01378
/// @trace_id_dd=DD_UDS_02670
/// @needwork = ad
/// @endcode
class EventInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01019
    /// @needwork = dda
    /// @endcode
    EventInterface(EventInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01020
    /// @needwork = dda
    /// @endcode
    EventInterface(EventInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01021
    /// @needwork = dda
    /// @endcode
    EventInterface& operator=(EventInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01022
    /// @needwork = dda
    /// @endcode
    EventInterface& operator=(EventInterface&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01126
    /// @trace_id_dd=DD_UDS_02240
    /// @needwork = ad
    /// @endcode
    EventInterface() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01127
    /// @trace_id_dd=DD_UDS_02241
    /// @needwork = ad
    /// @endcode
    virtual ~EventInterface() noexcept = default;

public:
    /// @brief Notify event status
    /// @param status Event status
    /// @return Returns 0 on successful call
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01128
    /// @trace_id_dd=DD_UDS_02242
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyEventStatus(uint8_t const status) = 0;

    /// @brief Register get event status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01129
    /// @trace_id_dd=DD_UDS_02243
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetEventStatus(GetEventStatusCallBack const& callBack) = 0;

    /// @brief Register get latched WIR status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01130
    /// @trace_id_dd=DD_UDS_02244
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetLatchedWIRStatus(GetLatchedWIRStatusCallBack const& callBack) = 0;

    /// @brief Register set latched WIR status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01131
    /// @trace_id_dd=DD_UDS_02245
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetLatchedWIRStatus(SetLatchedWIRStatusCallBack const& callBack) = 0;

    /// @brief Register get DTC count callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01132
    /// @trace_id_dd=DD_UDS_02246
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetDTCNumber(GetDTCNumberCallBack const& callBack) = 0;

    /// @brief Register get debounce status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01133
    /// @trace_id_dd=DD_UDS_02247
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetDebouncingStatus(GetDebouncingStatusCallBack const& callBack) = 0;

    /// @brief Register get fault detection counter callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01134
    /// @trace_id_dd=DD_UDS_02248
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetFaultDetectionCounter(GetFaultDetectionCounterCallBack const& callBack) = 0;

    /// @brief Register get test completion callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01135
    /// @trace_id_dd=DD_UDS_02249
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetTestComplete(GetTestCompleteCallBack const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_