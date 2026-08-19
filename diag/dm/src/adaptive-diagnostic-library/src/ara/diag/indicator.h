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
/// @brief      This file provides the definitions of Indicator and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INDICATOR_H_
#define ARA_DIAG_INDICATOR_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <functional>
#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class IndicatorAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the state of an indicator.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00740}@tracestatus{draft}
enum class IndicatorType
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

namespace api {
/// @brief Declare IndicatorProxyWrapper
class IndicatorProxyWrapper;
}  // namespace api

/// @brief DiagnosticIndicatorInterface provides functionality for handling indicators.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00741}@tracestatus{draft}
class Indicator
{
public:
    /// @brief Constructor for DiagnosticIndicatorInterface
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticIndicatorInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00742}@tracestatus{draft}
    explicit Indicator(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of DiagnosticIndicatorInterface
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00743}@tracestatus{draft}
    ~Indicator() noexcept = default;

    /// @brief copy constructor
    /// @param other
    Indicator(Indicator const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return Indicator&
    Indicator& operator=(Indicator const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    Indicator(Indicator&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return Indicator&
    Indicator& operator=(Indicator&& other) noexcept = default;

    /// @brief Get current Indicator
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current Indicator
    ///
    ///
    /// @traceid{SWS_DM_00744}@tracestatus{draft}
    ara::core::Result< IndicatorType > GetIndicator();

    /// @brief Register a notifier function which is called if the indicator is updated.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00745}@tracestatus{draft}
    ara::core::Result< void > SetNotifier(std::function< void(IndicatorType) > notifier);

private:
    std::shared_ptr< isoft::dm::dic::IndicatorAgent >
        /// @name proxy_
        proxy_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INDICATOR_H_