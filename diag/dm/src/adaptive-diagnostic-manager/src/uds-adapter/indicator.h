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
/// @file       indicator.h
/// @brief      This file provides the definition of Indicator and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG_FAULT_INDICATOR_H__
#define __ARA_DIAG_FAULT_INDICATOR_H__
#include <isoft/uds/fault_management/indicator_interface.h>

#include "gen_code/indicator/serviceAgent/indicator_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class Indicator : public isoft::uds::server::IndicatorInterface
{
public:
    Indicator(Indicator const&) = delete;
    Indicator(Indicator&&)      = delete;
    Indicator& operator=(Indicator const&) = delete;
    Indicator& operator=(Indicator&&) = delete;
    explicit Indicator(uint32_t const& serviceInstanceId) noexcept;
    ~Indicator() noexcept override = default;

public:
    /// @brief Notify AA side of indicator changes
    /// @param indicatorType Indicator status
    /// @return Returns 0 for successful sending
    std::int32_t NotifyIndicator(isoft::uds::server::IndicatorType const& indicatorType) override;

    /// @brief Register callback for AA side to get indicator status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetIndicator(isoft::uds::server::GetIndicatorCallBack const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::IndicatorAgent > indicatorAgent_;

    isoft::uds::server::GetIndicatorCallBack getIndicatorCallBack_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_