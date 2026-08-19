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
/// @file       security_access.h
/// @brief
/// @details
/// @date       2024-10-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SECURITYACCESS_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SECURITYACCESS_H_
#include <isoft/uds/security_access_management/security_access.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/security_access/serviceAgent/security_access_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class SecurityAccess : public isoft::uds::server::SecurityAccessInterface
{
public:
    using MetaInfoMap          = isoft::uds::server::MetaInfoMap;
    using CancellationHandler  = isoft::uds::server::CancellationHandler;
    using KeyCompareResultType = isoft::uds::server::KeyCompareResultType;
    SecurityAccess(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    ~SecurityAccess() override = default;

    /// @brief copy constructor
    /// @param other
    SecurityAccess(SecurityAccess const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return SecurityAccess&
    SecurityAccess &operator=(SecurityAccess const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    SecurityAccess(SecurityAccess &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return SecurityAccess&
    SecurityAccess &operator=(SecurityAccess &&other) noexcept = delete;

    /// @brief Called for any request messsage.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] securityAccessDataRecord Security Access payload
    /// @param[in] metaInfo MetaInfoMap of the request.
    /// @param[in] cancellationHandler Set if the current conversation is
    /// canceled.
    /// @return provided seed
    /// error code: kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
    ///> kRequestSequenceError, kRequestOutOfRange, kInvalidKey,
    /// kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    ///
    ///
    /// @traceid{SWS_DM_00764}@tracestatus{draft}
    isoft::uds::Result< std::vector< std::uint8_t > > GetSeed(
        std::vector< std::uint8_t > const &securityAccessDataRecord,
        MetaInfoMap &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    /// @brief This method is called, when a diagnostic request has been finished,
    /// to notify about the outcome.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] key The key to be validated
    /// @param[in] metaInfo MetaInfoMap of the request.
    /// @param[in] cancellationHandler Set if the current conversation is
    /// canceled.
    /// @return Result of the key validation.
    /// error code: kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
    /// kRequestSequenceError, kRequestOutOfRange, kInvalidKey,
    /// kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired
    ///
    ///
    /// @traceid{SWS_DM_00765}@tracestatus{draft}
    isoft::uds::Result< KeyCompareResultType > CompareKey(std::vector< std::uint8_t > const &key,
                                                          MetaInfoMap &metaInfo,
                                                          CancellationHandler cancellationHandler) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::SecurityAccessAgent > agentPtr_;

    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_SECURITYACCESS_H_