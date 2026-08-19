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
/// @brief
/// @details
/// @date       2024-12-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_COMMUNICATIONCONTROL_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_COMMUNICATIONCONTROL_H_
#include <isoft/uds/com_management/communication_control.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/communication_control/serviceAgent/communication_control_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class CommunicationControl : public isoft::uds::server::CommunicationControlInterface
{
public:
    using MetaInfoMap              = isoft::uds::server::MetaInfoMap;
    using CancellationHandler      = isoft::uds::server::CancellationHandler;
    using ComCtrlRequestParamsType = isoft::uds::server::CommunicationControlInterface::ComCtrlRequestParamsType;
    CommunicationControl(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    /// @brief Default destructor
    ~CommunicationControl() override = default;
    /// @brief copy constructor
    /// @param other
    CommunicationControl(CommunicationControl const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return CommunicationControl&
    CommunicationControl &operator=(CommunicationControl const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    CommunicationControl(CommunicationControl &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return CommunicationControl&
    CommunicationControl &operator=(CommunicationControl &&other) noexcept = delete;

    /// @brief Called for CommunicationControl (x028) with any subfunction as
    /// subfunction value is part of argument
    ///        list. Typically provider of this interface is considered as part of
    ///        the state management.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] controlType All UDS request parameters packed into a structure
    /// since it holds optional elements
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return ara::core::Future<void> error code: kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
    /// kRequestOutOfRange
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00808}@tracestatus{draft}
    isoft::uds::Result< void > CommCtrlRequest(ComCtrlRequestParamsType controlType,
                                               MetaInfoMap &metaInfo,
                                               CancellationHandler cancellationHandler) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::CommunicationControlAgent > agentPtr_;
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_COMMUNICATIONCONTROL_H_