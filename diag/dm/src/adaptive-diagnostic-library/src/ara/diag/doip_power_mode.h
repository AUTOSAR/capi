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
/// @file       doip_power_mode.h
/// @brief      This file provides the definitions of DoIPPowerMode and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_POWER_MODE_H_
#define ARA_DIAG_DOIP_POWER_MODE_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <memory>

#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
class PowerModeAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief PowerMode as defined in ISO13400-2.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00730}@tracestatus{draft}
enum class PowerModeType : int
{
    kNotReady     = 0x00,  ///< not all ECUs accessible via DoIP can communicate
    kReady        = 0x01,  ///< all ECUs accessible via DoIP can communicate
    kNotSupported = 0x02   ///< the Diagnostic Information Power Mode Information Request message is not supported
};

/// @brief DiagnosticDoIPPowerModeInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00731}@tracestatus{draft}
class DoIPPowerMode
{
public:
    /// @brief Constructor of DoIPPowerMode
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticDoIPPowerModeInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    ///
    ///
    /// @traceid{SWS_DM_00732}@tracestatus{draft}
    explicit DoIPPowerMode(ara::core::InstanceSpecifier const &specifier, ReentrancyType const reentrancyType) noexcept;

    /// @brief Constructor of DoIPPowerMode
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPPowerMode(DoIPPowerMode const &) = default;

    /// @brief Constructor of DoIPPowerMode
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPPowerMode(DoIPPowerMode &&) = default;

    /// @brief Constructor of DoIPPowerMode
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPPowerMode&
    ///
    DoIPPowerMode &operator=(DoIPPowerMode const &) = default;

    /// @brief move Constructor of DoIPPowerMode
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPPowerMode&
    ///
    DoIPPowerMode &operator=(DoIPPowerMode &&) = default;

    /// @brief Destructor of DoIPPowerMode
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00733}@tracestatus{draft}
    virtual ~DoIPPowerMode() noexcept = default;

    /// @brief Called to get the current Power Mode for the DoIP protocol.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return current diagnostic power mode
    ///
    ///
    /// @traceid{SWS_DM_00734}@tracestatus{draft}
    virtual ara::core::Future< PowerModeType > GetDoIPPowerMode() noexcept = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code: kNotOffered, kGenericError, kAlreadyOffered
    ///
    ///
    /// @traceid{SWS_DM_00735}@tracestatus{draft}
    ara::core::Result< void > Offer() noexcept;

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00736}@tracestatus{draft}
    void StopOffer() noexcept;

private:
    std::shared_ptr< isoft::dm::dis::PowerModeAgent >
        /// @name skeleton_
        skeleton_;

    /// @name instance_specifier_
    ara::core::InstanceSpecifier instanceSpecifier_;

    /// @name reentrancy_type_
    ReentrancyType reentrancyType_;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOIP_POWER_MODE_H_