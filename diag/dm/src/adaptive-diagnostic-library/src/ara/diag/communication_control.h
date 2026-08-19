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
/// @file       communication_control.h
/// @brief      This file provides the definitions of CommunicationControl and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_COMMUNICATION_CONTROL_H_
#define ARA_DIAG_COMMUNICATION_CONTROL_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>

#include "cancellation_handler.h"
#include "meta_info.h"
#include "reentrancy.h"
namespace isoft {
namespace dm {
namespace dis {
/// @brief Declare CommunicationControlSkeleton
class CommunicationControlAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/// @brief CommunicationControl interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00804}@tracestatus{draft}
class CommunicationControl
{
public:
    /// @brief All parameters of an UDS 0x28 communicationControl request, Read 9.5.2.1 of ISO 14229-1 for details.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00805}@tracestatus{draft}
    struct ComCtrlRequestParamsType
    {
        /// @brief Control type of CommunicationControl (0x28) service
        std::uint8_t controlType;
        /// @brief Communication type CommunicationControl (0x28) service
        std::uint8_t communicationType;
        /// @brief Node identification number
        std::uint16_t nodeIdentificationNumber;
    };

    /// @brief Class for an CommunicationControl
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an diagnostic CommunicationControl interface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00806}@tracestatus{draft}
    explicit CommunicationControl(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);

    /// @brief Destructor of class CommunicationControl
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00807}@tracestatus{draft}
    virtual ~CommunicationControl() noexcept = default;

    /// @brief copy constructor
    /// @param other
    CommunicationControl(CommunicationControl const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return CommunicationControl&
    CommunicationControl& operator=(CommunicationControl const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    CommunicationControl(CommunicationControl&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return CommunicationControl&
    CommunicationControl& operator=(CommunicationControl&& other) noexcept = default;

    /// @brief Called for CommunicationControl (x028) with any subfunction as subfunction value is part of argument
    ///        list. Typically provider of this interface is considered as part of the state management.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] controlType All UDS request parameters packed into a structure since it holds optional elements
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return ara::core::Future<void> error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
    /// kConditionsNotCorrect, kRequestOutOfRange
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00808}@tracestatus{draft}
    virtual ara::core::Future< void > CommCtrlRequest(ComCtrlRequestParamsType controlType,
                                                      MetaInfo& metaInfo,
                                                      CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code:kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00809}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00810}@tracestatus{draft}
    void StopOffer();

private:
    /// @brief specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @brief reentrancyType_
    ReentrancyType reentrancyType_;

    std::shared_ptr< isoft::dm::dis::CommunicationControlAgent >
        ///  @brief skeleton_
        skeleton_{};
    friend class isoft::dm::dis::CommunicationControlAgent;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_COMMUNICATION_CONTROL_H_