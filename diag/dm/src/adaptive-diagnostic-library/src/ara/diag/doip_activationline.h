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
/// @file       doip_activationline.h
/// @brief      This file provides the definitions of DoIPActivationLine and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_ACTIVATIONONLINE_H_
#define ARA_DIAG_DOIP_ACTIVATIONONLINE_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>
#include <memory>

namespace isoft {
namespace dm {
namespace dis {
class ActivationLineAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief DiagnosticDoIPActivationLineInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00830}@tracestatus{draft}
class DoIPActivationLine
{
public:
    /// @brief Constructor of DoIPActivationLine
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticDoIPActivationLineInterface
    ///
    ///
    /// @traceid{SWS_DM_00831}@tracestatus{draft}
    explicit DoIPActivationLine(ara::core::InstanceSpecifier const &specifier) noexcept;

    /// @brief Constructor of DoIPActivationLine
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPActivationLine(DoIPActivationLine const &) = default;

    /// @brief Constructor of DoIPActivationLine
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPActivationLine(DoIPActivationLine &&) = default;

    /// @brief Constructor of DoIPActivationLine
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPActivationLine&
    ///
    DoIPActivationLine &operator=(DoIPActivationLine const &) = default;

    /// @brief move Constructor of DoIPActivationLine
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPActivationLine&
    ///
    DoIPActivationLine &operator=(DoIPActivationLine &&) = default;

    /// @brief Destructor of DoIPActivationLine
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00832}@tracestatus{draft}
    virtual ~DoIPActivationLine() noexcept = default;

    /// @brief Called to get the network interface Id (see DoIpNetworkConfiguration.networkInterfaceId)
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// for which this DoIPActivationLine instance is responsible.
    /// @note If the reported DoIpNetworkConfiguration.networkInterfaceId belongs to a DoIpNetworkConfiguration
    /// with property isActivationLineDependent = 'FALSE', this is an error!
    /// @return network interface id for which this activation line is responsible.
    ///
    ///
    /// @traceid{SWS_DM_00833}@tracestatus{draft}
    virtual ara::core::Future< std::uint8_t > GetNetworkInterfaceId() = 0;

    /// @brief Called to update current activation line state.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] state TRUE in case the activation line is active, else FALSE.
    ///
    ///
    /// @traceid{SWS_DM_00834}@tracestatus{draft}
    void UpdateActivationLineState(bool const state) noexcept;

    /// @brief Called to get the current activation line state.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return TRUE in case the activation line is active, else FALSE.
    ///
    ///
    /// @traceid{SWS_DM_00835}@tracestatus{draft}
    virtual ara::core::Future< bool > GetActivationLineState() noexcept = 0;

    /// @brief This Offer will enable the DM to listen to activation line state changes for the
    /// given interface.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code : kNotOffered, kGenericError, kAlreadyOffered
    ///
    ///
    /// @traceid{SWS_DM_00836}@tracestatus{draft}
    ara::core::Result< void > Offer() noexcept;

    /// @brief This StopOffer will disable the provision of activation line state to DM.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00837}@tracestatus{draft}
    void StopOffer() noexcept;

private:
    /// @name skeleton_
    /// Construct a shared object
    std::shared_ptr< isoft::dm::dis::ActivationLineAgent > skeleton_;

    /// @name instanceSpecifier_
    /// Instance descriptor
    ara::core::InstanceSpecifier instanceSpecifier_;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOIP_ACTIVATIONONLINE_H_