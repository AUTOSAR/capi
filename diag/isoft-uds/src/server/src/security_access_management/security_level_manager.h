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
/// @file       security_level_manager.h
/// @brief      This file provides security level management functionality
/// @details
/// @date       2024-10-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=security_level_manager.h
/// @unit_description=This file provides ServiceBase functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_DIAGNOSTICSECURITYLEVELMANAGER_H_
#define ISOFT_UDS_SERVER_DIAGNOSTICSECURITYLEVELMANAGER_H_

#include <memory>
#include <mutex>

#include "common/request_context.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "session_management/session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief security level class
class SecurityLevel
{
public:
    /// @brief Copy constructor is prohibited
    /// @param
    SecurityLevel(SecurityLevel const&) = delete;
    /// @brief Copy assignment constructor is prohibited
    /// @param
    /// @return
    SecurityLevel& operator=(SecurityLevel const&) = delete;
    /// @brief Move constructor
    /// @param
    SecurityLevel(SecurityLevel&&) = delete;
    /// @brief Move assignment constructor
    /// @param
    /// @return
    SecurityLevel& operator=(SecurityLevel&&) = delete;

public:
    /// @brief constructor
    /// @param currentId current security level id
    explicit SecurityLevel(SecurityLevelId& currentId) noexcept;

    /// @brief Destructor
    ~SecurityLevel() noexcept = default;

    /// @brief load config file
    /// @param config config value
    /// @return 0: success
    std::int32_t Initialize(SecurityLevelConfig const& config) noexcept;

    /// @brief get security level short name
    /// @return string: short name
    std::string GetShortName() const noexcept;

    /// @brief accessDataRecordSize
    /// @return size 
    std::size_t GetAccessDataRecordSize() const noexcept;

    /// @brief keySize

    std::size_t GetKeySize() const noexcept;

    /// @brief numFailedSecurityAccess
    std::size_t GetNumFailedSecurityAccess() const noexcept;

    /// @brief securityDelayTime
    ///   Unit: seconds
    std::uint32_t GetSecurityDelayTime() const noexcept;

    /// @brief seedSize
    std::size_t GetSeedSize() const noexcept;

    /// @brief id
    SecurityLevelId GetId() const noexcept;

    /// @brief check this current security level equal this config
    /// @return true:success
    bool Check() const noexcept;

private:
    SecurityLevelId& currentId_;
    SecurityLevelConfig config_;
};

/// @brief DiagnosticSessionControl service is used to enable different diagnostic sessions in the server.
class SecurityLevelManager final
{
public:
    SecurityLevelManager(SecurityLevelManager const&) = delete;
    SecurityLevelManager& operator=(SecurityLevelManager const&) = delete;
    SecurityLevelManager(SecurityLevelManager&&)                 = delete;
    SecurityLevelManager& operator=(SecurityLevelManager&&) = delete;

public:
    /// @brief Constructor
    SecurityLevelManager() noexcept = default;

    /// @brief Destructor
    ~SecurityLevelManager() noexcept = default;

    /// @brief Init this object
    /// @param securityLevelTable config all security level object 
    /// @param sessionManager session manager object
    /// @return 0: success
    std::int32_t Initialize(std::set< SecurityLevelConfig > const& securityLevelTable,
                            std::shared_ptr< SessionManager >& sessionManager) noexcept;

    /// @brief get current security level
    /// @return security level id
    SecurityLevelId GetCurrentSecurityLevel() const noexcept;

    /// @brief get security level object
    /// @param id security level id
    /// @return security level objecrt
    std::shared_ptr< SecurityLevel > GetSecurityLevel(SecurityLevelId id) noexcept;

    /// @brief add notify when security level changge
    /// @param callback callback function
    /// @return id
    NotifyOfSecurityLevelChangeCallbackId AddNotifyOfSecurityLevelChange(
        NotifyOfSecurityLevelChangeCallback& callback) noexcept;

    /// @brief remove callback object
    /// @param id callback id
    void RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId id) noexcept;

    /// @brief update this security level id
    /// @param id security level id
    void SetSecurityLevel(SecurityLevelId id) noexcept;

    /// @brief seucirty timer timeout
    /// @return true:timeout
    bool SecurityTimerElapsed() const noexcept;

    /// @brief update security timer state
    /// @param elapsed true:timeout
    void SetSecurityTimerElapsed(bool elapsed) noexcept;

private:
    /// @brief notify when security level changge
    void _notify() noexcept;

private:
    SecurityLevelId currentSecurityLevelType_{0U};
    std::map< SecurityLevelId, std::shared_ptr< SecurityLevel > > table_{};
    std::shared_ptr< SessionManager > sessionManager_{nullptr};
    NotifyOfSessionChangeCallbackId notifyOfSessionChangeCallbackId_{0U};
    std::recursive_mutex lock_{};
    std::map< NotifyOfSecurityLevelChangeCallbackId, NotifyOfSecurityLevelChangeCallback >
        notifyOfSecurityLevelChangeCallbackTable_{};
    bool securityTimerElapsed_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_DIAGNOSTICSECURITYLEVELMANAGER_H_