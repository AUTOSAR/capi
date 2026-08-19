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
/// @file       indicator_interface.h
/// @brief      This file provides indicator_interface functionality
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
/// @trace_id_sr=786e1325-4517-4fcb-804d-e5d99a7a6b16
/// @unit_name=IndicatorInterface
/// @unit_description=This file provides indicator_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_INDICATOR_INTERFACE_H__
#define __ISOFT_UDS_FAULT_INDICATOR_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace uds {
namespace server {

/// @brief Indicator status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01379
/// @trace_id_dd=DD_UDS_02671
/// @needwork = ad
/// @endcode
enum class IndicatorType : std::uint8_t
{
    kOff                  = 0x00,  ///< Indicator off mode {default}
    kContinuous           = 0x01,  ///< Indicator continuously on mode
    kBlinking             = 0x02,  ///< Indicator blinking mode
    kBlinkingOrContinuous = 0x03,  ///< Indicator blinking Or continuously on mode
    kSlowFlash            = 0x04,  ///< Indicator slow flashing mode
    kFastFlash            = 0x05,  ///< Indicator fast flashing mode
    kOnDemand             = 0x06,  ///< Indicator on-demand mode
    kShort                = 0x07,  ///< Indicator short mode
};

/// @brief Get indicator status callback
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
using GetIndicatorCallBack = std::function< IndicatorType() >;

/// @brief Indicator interface
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
class IndicatorInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01023
    /// @needwork = dda
    /// @endcode
    IndicatorInterface(IndicatorInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01024
    /// @needwork = dda
    /// @endcode
    IndicatorInterface(IndicatorInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01025
    /// @needwork = dda
    /// @endcode
    IndicatorInterface& operator=(IndicatorInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01026
    /// @needwork = dda
    /// @endcode
    IndicatorInterface& operator=(IndicatorInterface&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01136
    /// @trace_id_dd=DD_UDS_02250
    /// @needwork = ad
    /// @endcode
    IndicatorInterface() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01137
    /// @trace_id_dd=DD_UDS_02251
    /// @needwork = ad
    /// @endcode
    virtual ~IndicatorInterface() noexcept = default;

public:
    /// @brief Notify indicator status
    /// @param indicatorType Indicator status
    /// @return Returns 0 on successful sending
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01138
    /// @trace_id_dd=DD_UDS_02252
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyIndicator(IndicatorType const& indicatorType) = 0;
    /// @brief Register get indicator status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01139
    /// @trace_id_dd=DD_UDS_02253
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetIndicator(GetIndicatorCallBack const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_