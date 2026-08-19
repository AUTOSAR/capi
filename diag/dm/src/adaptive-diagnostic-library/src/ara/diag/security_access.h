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
/// @brief      This file provides the definitions of SecurityAccess and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_SECURITY_ACCESS_H_
#define ARA_DIAG_SECURITY_ACCESS_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>

#include <cstdint>
#include <memory>

#include "cancellation_handler.h"
#include "meta_info.h"
#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief SecurityAccessAgent
class SecurityAccessAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the status of the key compare
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00760}@tracestatus{draft}
enum class KeyCompareResultType
{
    kKeyValid   = 0x00,  ///< Key is valid
    kKeyInvalid = 0x01,  ///< Key is invalid
};

namespace api {
/// @brief Declare SecurityAccessSkeleton
class SecurityAccessSkeleton;
}  // namespace api

/// @brief DiagnosticSecurityAccessInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00761}@tracestatus{draft}
class SecurityAccess
{
public:
    /// @brief Constructor of SecurityAccess
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticSecurityAccessInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00762}@tracestatus{draft}
    explicit SecurityAccess(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);
    ///@brief Destructor of SecurityAccess
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///@traceid{SWS_DM_00763}@tracestatus{draft}
    virtual ~SecurityAccess() noexcept = default;

    /// @brief copy constructor
    /// @param other
    SecurityAccess(SecurityAccess const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return SecurityAccess&
    SecurityAccess& operator=(SecurityAccess const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    SecurityAccess(SecurityAccess&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return SecurityAccess&
    SecurityAccess& operator=(SecurityAccess&& other) noexcept = default;

    /// @brief Called for any request messsage.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] securityAccessDataRecord Security Access payload
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is canceled.
    /// @return provided seed
    /// error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
    ///> kRequestSequenceError, kRequestOutOfRange, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    ///
    ///
    /// @traceid{SWS_DM_00764}@tracestatus{draft}
    virtual ara::core::Future< ara::core::Span< std::uint8_t > > GetSeed(
        ara::core::Span< std::uint8_t > securityAccessDataRecord,
        MetaInfo& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief This method is called, when a diagnostic request has been finished, to notify about the outcome.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] key The key to be validated
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is canceled.
    /// @return Result of the key validation.
    /// error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
    /// kRequestSequenceError, kRequestOutOfRange, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired
    ///
    ///
    /// @traceid{SWS_DM_00765}@tracestatus{draft}
    virtual ara::core::Future< KeyCompareResultType > CompareKey(ara::core::Span< std::uint8_t > key,
                                                                 MetaInfo& metaInfo,
                                                                 CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code : kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00766}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00767}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::SecurityAccessAgent >
        /// @name skeleton_
        skeleton_{};
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_SECURITY_ACCESS_H_