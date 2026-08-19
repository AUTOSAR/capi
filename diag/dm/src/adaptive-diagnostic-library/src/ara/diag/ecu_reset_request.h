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
/// @file       ecu_reset_request.h
/// @brief      This file provides the definitions of EcuResetRequest and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_ECU_RESET_REQUEST_H_
#define ARA_DIAG_ECU_RESET_REQUEST_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/optional.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <memory>

#include "cancellation_handler.h"
#include "meta_info.h"
namespace isoft {
namespace dm {
namespace dis {
/// @brief Declare CommunicationControlSkeleton
class EcuResetRequestAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/// @brief The type of the requested reset.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_01008}@tracestatus{draft}
enum class LastResetType : std::uint32_t
{
    kRegular       = 0,  ///< regular shutdown.
    kUnexpected    = 1,  ///< unexpected reset.
    kSoftReset     = 2,  ///< Diagnostic softReset.
    kHardReset     = 3,  ///< Diagnostic hardReset.
    kKeyOffOnReset = 4,  ///< Diagnostic keyOffOnReset.
    kCustomReset   = 5,  ///< Diagnostic kCustomReset.
};

/// @brief The type of the requested reset.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_01007}@tracestatus{draft}
enum class ResetRequestType : std::uint32_t
{
    kSoftReset     = 0,  ///<  softReset.
    kHardReset     = 1,  ///<  hardReset
    kKeyOffOnReset = 2,  ///<  keyOffOnReset
    kCustomReset   = 3   ///< kCustomReset
};

/// @brief Service EcuReset Request interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_01009}@tracestatus{draft}
class EcuResetRequest
{
public:
    /// @brief Constructor of EcuResetRequest
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier An InstanceSpecifier linking this instance with the PortPrototype in the manifest
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01010}@tracestatus{draft}
    explicit EcuResetRequest(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of EcuResetRequest
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_01011}@tracestatus{draft}
    virtual ~EcuResetRequest() noexcept = default;

    /// @brief copy constructor
    /// @param other
    EcuResetRequest(EcuResetRequest const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return EcuResetRequest&
    EcuResetRequest& operator=(EcuResetRequest const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    EcuResetRequest(EcuResetRequest&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return EcuResetRequest&
    EcuResetRequest& operator=(EcuResetRequest&& other) noexcept = default;

    /// @brief interface for subFunction En-/DisableRapidShutdown
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The type of the last machine reset. error code: kRequestFailed
    ///
    /// @traceid{SWS_DM_01015}@tracestatus{draft}
    virtual ara::core::Result< LastResetType > GetLastResetCause() = 0;

    /// @brief interface for subFunction En-/DisableRapidShutdown
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] enable when enable is set to true the rapid shutdown will be enabled, setting enable to false will
    /// disable rapid shutdown
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is canceled.
    /// @return ara::core::Future<void> error code: kRejected, kResetTypeNotSupported
    /// @throws on overflow
    /// @traceid{SWS_DM_01012}@tracestatus{draft}
    virtual ara::core::Future< void > EnableRapidShutdown(bool enable,
                                                          MetaInfo const& metaInfo,
                                                          CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for any EcuRest subFunction, except En-/DisableRapidShutdown.
    /// StateManagement needs to evalute carefully if the request to restart parts or the whole machine.
    /// Once the request to reset is accepted, the StateManagement has to rely on this decision for the
    /// ExecuteReset() trigger.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] resetType Type of the requested reset.
    /// @param[in] id id of the custom reset type. Will only be evaluated when resetType is "custom"
    /// @param[in] metaInfo[in] MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is canceled.
    /// @return ara::core::Future<void> error code : kRejected, kRequestFailed, kCustomResetTypeNotSupported,
    /// kResetTypeNotSupported
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01013}@tracestatus{draft}
    virtual ara::core::Future< void > RequestReset(ResetRequestType resetType,
                                                   ara::core::Optional< std::uint8_t > id,
                                                   MetaInfo const& metaInfo,
                                                   CancellationHandler cancellationHandler)
        = 0;

    /// @brief StateManagement has to execute the requested reset.
    /// Called after DM sent the response message to tester.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] metaInfo
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01014}@tracestatus{draft}
    virtual void ExecuteReset(MetaInfo metaInfo) = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code : kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01016}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_01017}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    std::shared_ptr< isoft::dm::dis::EcuResetRequestAgent >
        /// @name skeleton_
        skeleton_{};
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_ECU_RESET_REQUEST_H_