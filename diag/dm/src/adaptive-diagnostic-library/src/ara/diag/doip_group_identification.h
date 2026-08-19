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
/// @file       doip_group_identification.h
/// @brief      This file provides the definitions of DoIPGroupIdentification and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_GROUP_IDENTIFICATION_H_
#define ARA_DIAG_DOIP_GROUP_IDENTIFICATION_H_

#include <ara/core/array.h>
#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>
#include <memory>

#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
class GroupIdentificationAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @name kGroupIdentificationSize
int const kGroupIdentificationSize{6};

/// @brief DoIPGroupIdentificationInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00720}@tracestatus{draft}
class DoIPGroupIdentification
{
public:
    /// @brief Response data of positive response message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00721}@tracestatus{draft}
    struct GidStatus final
    {
        ara::core::Array< std::uint8_t, kGroupIdentificationSize >
            /// @name groupIdentification
            /// Value of gid
            groupIdentification;

        /// @name furtherActionRequired
        /// Next method to execute
        std::uint8_t furtherActionRequired;

        /// @name syncStatus
        /// Synchronization status with other gids
        std::uint8_t syncStatus;
    };

    /// @brief Constructor of DoIPGroupIdentification
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticDoIPGroupIdentificationInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    ///
    ///
    /// @traceid{SWS_DM_00722}@tracestatus{draft}
    explicit DoIPGroupIdentification(ara::core::InstanceSpecifier const &specifier,
                                     ReentrancyType const reentrancyType) noexcept;

    /// @brief Constructor of DoIPGroupIdentification
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPGroupIdentification(DoIPGroupIdentification const &) = default;

    /// @brief Constructor of DoIPGroupIdentification
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPGroupIdentification(DoIPGroupIdentification &&) = default;

    /// @brief Constructor of DoIPGroupIdentification
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPGroupIdentification&
    ///
    DoIPGroupIdentification &operator=(DoIPGroupIdentification const &) = default;

    /// @brief move Constructor of DoIPGroupIdentification
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPGroupIdentification&
    ///
    DoIPGroupIdentification &operator=(DoIPGroupIdentification &&) = default;

    /// @brief Destructor of DoIPGroupIdentification
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00723}@tracestatus{draft}
    virtual ~DoIPGroupIdentification() noexcept = default;

    /// @brief Called to get the current GID state for the DoIP protocol.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return group identification and state
    ///
    /// @traceid{SWS_DM_00724}@tracestatus{draft}
    virtual ara::core::Future< GidStatus > GetGidStatus() noexcept = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code : kNotOffered, kGenericError, kAlreadyOffered
    ///
    ///
    /// @traceid{SWS_DM_00725}@tracestatus{draft}
    ara::core::Result< void > Offer() noexcept;

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00726}@tracestatus{draft}
    void StopOffer() noexcept;

private:
    std::shared_ptr< isoft::dm::dis::GroupIdentificationAgent >
        /// @name skeleton_
        /// Implement the functions of the current class
        skeleton_;

    /// @name instanceSpecifier_
    /// Instance descriptor
    ara::core::InstanceSpecifier instanceSpecifier_;

    /// @name reentrancyType_
    /// Type fully; non-reentrant
    ReentrancyType reentrancyType_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOIP_GROUP_IDENTIFICATION_H_