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
/// @file       service_validation.h
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SERVICEVALIDATION_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SERVICEVALIDATION_H_
#include <isoft/uds/service_validation.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/service_validation/serviceAgent/service_validation_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class ServiceValidation : public isoft::uds::server::ServiceValidationInterface
{
public:
    using MetaInfoMap = isoft::uds::server::MetaInfoMap;
    ServiceValidation(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    ~ServiceValidation() override = default;

    /// @brief copy constructor
    /// @param other
    ServiceValidation(ServiceValidation const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return ServiceValidation&
    ServiceValidation &operator=(ServiceValidation const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    ServiceValidation(ServiceValidation &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return ServiceValidation&
    ServiceValidation &operator=(ServiceValidation &&other) noexcept = delete;

    isoft::uds::Result< void > Validate(std::vector< std::uint8_t > requestData,
                                        MetaInfoMap &metaInfo) noexcept override;

    /// @brief This method is called, when a diagnostic request has been finished,
    /// to notify about the outcome.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] status status/outcome of the service processing.
    /// @param[in] metaInfo MetaInfo of the request.
    /// @returns Returns nothing or an error
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00775}@tracestatus{draft}
    isoft::uds::Result< void > Confirmation(isoft::uds::server::ConfirmationStatusType status,
                                            MetaInfoMap &metaInfo) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::ServiceValidationAgent > agentPtr_;

    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SERVICEVALIDATION_H_