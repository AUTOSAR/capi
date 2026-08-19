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
/// @file       service_x27.h
/// @brief      This file provides SecurityAccess (0x27) service
/// @details
/// @date       2022-11-07
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=service_x27.h
/// @unit_description=This file provides service_x27 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X27_H_
#define ISOFT_UDS_SERVER_SERVICE_X27_H_

#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "common/persistence_service.h"
#include "common/timer.h"
#include "isoft/uds/security_access_management/security_access_management.h"
#include "securityLevel_validator.h"
#include "security_level_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief
/// The purpose of this service is to provide a method of accessing data and/or diagnostic services that are restricted for security, emission, or safety reasons.
/// Diagnostic services used to download/upload routines or data to the server and to read specific memory locations from the server are situations that may require secure access.
/// Improper procedures or data downloads to the server may damage electronic equipment or other vehicle components, or jeopardize the vehicle's emission, safety, or security standards.
class ServiceX27 : public GeneralService
{
public:
    ServiceX27(ServiceX27 const&) = delete;
    ServiceX27& operator=(ServiceX27 const&) = delete;
    ServiceX27(ServiceX27&&)                 = delete;
    ServiceX27& operator=(ServiceX27&&) = delete;

    /// @brief Constructor
    /// @param[in] conversationManager Session management
    /// @param[in] storage DID management
    explicit ServiceX27(std::shared_ptr< PersistenceService >& persistenceServicePtr) noexcept;

    ~ServiceX27() noexcept override;

    /// @brief Initialization, load configuration file
    /// @param[in] dmc Configuration file
    /// @param[in] node File file node
    /// @param[in] apt Access permission table
    /// @return 0: Success
    std::int32_t Initialize(std::set< SecurityAccessInstanceConfig > const& securityAccessInstanceTable,
                            std::uint32_t securityDelayTimeOnBoot,
                            bool sharedTimer,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManager,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManager,
                            std::uint32_t const p4ServerMax) noexcept;

    /// @brief get server level validator,include session server validator...
    /// @return vector
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

private:
    /// @brief see ISO 14229-1-2013 Figure I.1 — SecurityAccess state chart
    enum class SecurityLevelState
    {
        kLockNoActiveSeed        = 0,  /// All security levels locked. No active seed.
        kLockActiveSeedWaitKey   = 1,  /// All Security Levels Locked. Seed sent. Waiting for key.
        kUnlockNoActiveSeed      = 2,  /// One security level unlocked. No active seed.
        kUnlockActiveSeedWaitKey = 3   /// One security level unlocked. Seed sent. Waiting for key.
    };                                 /// @brief securityLevelManager_
    /// @brief SecurityLevelInfo
    struct SecurityLevelInfo
    {
        std::uint8_t requestSeedId{0U};
        SecurityLevelId id{0U};
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        std::shared_ptr< SubfunctionLevelSessionValidator<> > sessionValidator{nullptr};
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator<> > securityLevelValidator{nullptr};

        std::shared_ptr< SecurityAccessInterface > proxy{nullptr};
        std::size_t failedCounter{0U};
        std::size_t numFailedSecurityAccess{0U};
        /// @brief securityDelayTime
        std::uint32_t securityDelayTime{0U};
        /// @brief delayTimer
        std::shared_ptr< Timer > delayTimer{nullptr};
    };

    /// @brief storage_
    std::shared_ptr< PersistenceService > storage_{nullptr};
    /// @brief auth validator in server level
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_{nullptr};

    /// @brief session validator in server level
    std::shared_ptr< ServerLevelSessionValidator<> > serverSessionValidator_{nullptr};

    /// @brief security level validator in server level
    std::shared_ptr< ServerLevelSecurityLevelValidator<> > serverSecurityLevelValidator_{nullptr};
    /// @brief securityDelayTimeOnBoot_
    std::uint32_t securityDelayTimeOnBoot_{0U};

    /// @brief delayTimerShared_
    bool delayTimerShared_{false};

    /// @brief lastSubfunction_
    std::uint8_t lastSubfunction_{0U};

    /// @brief unlockSecurityLevelId_
    std::uint8_t unlockSecurityLevelId_{0U};

    /// @brief delayTimer
    std::shared_ptr< Timer > delayTimer_{nullptr};

    /// @brief instanceTable_
    std::map< std::uint8_t, std::shared_ptr< SecurityLevelInfo > > instanceTable_{};

    /// @brief manager all security level
    std::shared_ptr< SecurityLevelManager > securityLevelManager_{nullptr};

    /// @brief securityLevelState_
    SecurityLevelState securityLevelState_{SecurityLevelState::kLockNoActiveSeed};

public:

    /// @brief start this service
    /// @return 0: success
    std::int32_t Start() noexcept;

    /// @brief stop this service
    void Stop() noexcept;

private:
    /// @brief get x27 request min size
    /// @param subfunction request subfunction id
    /// @return min size
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief is support this subfunction in service
    /// @param subfunction request subfunction id
    /// @return true: support
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief get auth validator by request subfunction value
    /// @param subfunction request subfunction value
    /// @return auth validator
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief get session validator by request subfunction value
    /// @param subfunction request subfunction value
    /// @return session validator
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief support security level in x27 service
    /// @return true: support
    bool SupportSecurityLevel() noexcept override;

    /// @brief get security level validator by request subfunction value
    /// @param subfunction request subfunction value
    /// @return security level validator
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief parse request data and send response meesage
    /// @param context this request context
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Load failure count
    /// @param[in] securityLevelId Security level ID
    /// @return Failure count
    std::size_t _loadFailedCounter(SecurityLevelId const securityLevelId) noexcept;

    /// @brief Store failure count
    /// @param[in] securityLevelId Security level ID
    /// @param[in] value Failure count
    /// @return Storage result
    void _storeFailedCounter(SecurityLevelId const securityLevelId, std::size_t const value) noexcept;

    /// @brief Shared delay timer handler
    void _handleSharedDelayTimer() noexcept;

    /// @brief Delay timer handling
    /// @param[in] securityLevel Security level
    void _handleDelayTimer(std::shared_ptr< SecurityLevelInfo >& securityLevel) noexcept;

    /// @brief Confirm
    /// @param[in] context Request context
    /// @param[in] response Response data
    /// @param[in] result Transfer processing result
    void Confirm(RequestContext& requestContext, ResponseStatusType status) noexcept override;

    /// @brief Whether the current delay timer is active
    /// @param[in] securityLevelInfoPtr Security level object
    /// @return bool
    /// @retval true: Active
    /// @retval false: Inactive
    bool _activeDelayTimer(std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr) noexcept;

    /// @brief Check whether the accessDataRecordSize field length matches
    /// @param[in] securityLevelPtr Security level object
    /// @param[in] accessDataRecordSize Length of the current request's accessDataRecordSize field
    /// @return bool
    /// @retval true: Match
    /// @retval false: Mismatch
    inline bool _checkAccessDataRecordSize(std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                                           std::size_t const accessDataRecordSize) noexcept
    {
        bool match{false};
        if (securityLevelPtr->GetAccessDataRecordSize() == accessDataRecordSize) {
            match = true;
        }
        return match;
    }

    /// @brief Check whether the key field length matches
    /// @param[in] securityLevelPtr Security level object
    /// @param[in] keySize Length of the current request's key
    /// @param[in] securityLevelPtr Security level object
    /// @param[in] keySize Length of the current request's key
    /// @return bool
    /// @retval true: Match
    /// @retval false: Mismatch
    inline bool _checkKeySize(std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                              std::size_t const keySize) noexcept
    {
        bool match{false};
        if (securityLevelPtr->GetKeySize() == keySize) {
            match = true;
        }
        return match;
    }

    /// @brief Check whether the current incoming subfunction matches the previous subfunction
    /// @param[in] subfunction Current incoming subfunction
    /// @param[in] subfunction Current incoming subfunction
    /// @return bool
    /// @retval true: Match
    /// @retval false: Mismatch
    bool _matchRequestSeedAndSeedKey(std::uint8_t const subfunction) const noexcept
    {
        return lastSubfunction_ + 1U == subfunction;
    }

    /// @brief Used for state transition
    /// @param[in] context Request context
    /// @param[in] securityLevelPtr Security level
    /// @param[in] securityLevelInfoPtr
    void _handlerState(std::shared_ptr< RequestContext >& context,
                       std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                       std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr) noexcept;

    /// @brief Process RequestSeed request
    /// @param[in] context Request context
    /// @param[in] securityLevelPtr Security level object
    /// @param[in] securityLevelInfoPtr
    void _handlerRequestSeed(std::shared_ptr< RequestContext >& context,
                             std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                             std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr);

    /// @brief Process SeedKey request
    /// @param[in] context Request context
    /// @param[in] securityLevelPtr Security level object
    /// @param[in] securityLevelInfoPtr
    void _handlerSeedKey(std::shared_ptr< RequestContext >& context,
                         std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                         std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr);
    /// @brief get security level id. (seed+1)/2 or key/2
    /// @param subfunction request subfunction value
    /// @return security level id
    std::uint8_t _getRealSubfunction(std::uint8_t const subfunction);
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X27_H_