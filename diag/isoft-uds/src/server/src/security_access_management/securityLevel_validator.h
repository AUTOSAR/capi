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
/// @file       securityLevel_validator.h
/// @brief      This file provides ServiceBase functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=securityLevel_validator.h
/// @unit_description=This file provides ServiceBase functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SECURITYLEVELVALIDATOR_H_
#define ISOFT_UDS_SERVER_SECURITYLEVELVALIDATOR_H_

#include "common/log.h"
#include "common/validator.h"
#include "security_level_manager.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief security level validator
/// @tparam SubfunctionType subfunction type
template < class SubfunctionType = std::uint8_t >
class SubfunctionLevelSecurityLevelValidator : public Validator
{
public:
    SubfunctionLevelSecurityLevelValidator(SubfunctionLevelSecurityLevelValidator const&) = delete;
    SubfunctionLevelSecurityLevelValidator& operator=(SubfunctionLevelSecurityLevelValidator const&) = delete;
    SubfunctionLevelSecurityLevelValidator(SubfunctionLevelSecurityLevelValidator&&)                 = delete;
    SubfunctionLevelSecurityLevelValidator& operator=(SubfunctionLevelSecurityLevelValidator&&) = delete;

    /// @brief Constructor
    explicit SubfunctionLevelSecurityLevelValidator() noexcept = default;

    /// @brief Destructor
    ~SubfunctionLevelSecurityLevelValidator() noexcept override = default;

    /// @brief init
    /// @param id subfunction value
    /// @param config security level id config
    /// @param securityLevelManagerPtr security manager object
    /// @return 0:success
    std::uint32_t Initialize(SubfunctionType id,
                             std::set< SecurityLevelId > const& config,
                             std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
    {
        id = id;
        if (securityLevelManagerPtr.get() == nullptr) {
            LogWarn() << "paramter securityLevelManagerPtr is nullptr,Please securityLevelManagerPtr paramter";
        }
        config_ = config;
        for (auto&& securityLevelId : config) {
            std::shared_ptr< SecurityLevel > securityLevelPtr
                = securityLevelManagerPtr->GetSecurityLevel(securityLevelId);
            if (securityLevelPtr.get() == nullptr) {
                LogWarn() << "security level id invalid! security level id:" << securityLevelId;
                return 0;
            }
            std::ignore = accessPermissionTable_.emplace(securityLevelPtr);
        }
        return 0;
    }

    /// @brief validate 
    /// @param  requestcontext content
    /// @return true:success
    bool Validate(std::shared_ptr< RequestContext > const&) noexcept override
    {
        bool result{true};
        if (accessPermissionTable_.empty()) {
            for (auto&& securityLevelId : config_) {
                std::shared_ptr< SecurityLevel > securityLevelPtr
                    = securityLevelManagerPtr_->GetSecurityLevel(securityLevelId);
                if (securityLevelPtr.get() == nullptr) {
                    LogWarn() << "security level id invalid! security level id:" << securityLevelId;
                    return false;
                }
                std::ignore = accessPermissionTable_.emplace(securityLevelPtr);
            }
        }
        for (auto&& securityLevel : accessPermissionTable_) {
            result = securityLevel->Check();
            if (result) {
                break;
            }
        }
        LogInfo() << "subfunction level: " << id_ << " check security level result:" << result;
        return result;
    }

private:
    SubfunctionType id_;
    std::set< SecurityLevelId > config_;
    std::shared_ptr< SecurityLevelManager > securityLevelManagerPtr_;
    std::set< std::shared_ptr< SecurityLevel > > accessPermissionTable_;
};

/// @brief seucity validator in server level
/// @tparam SubfunctionType subfunction type
template < class SubfunctionType = std::uint8_t >
class ServerLevelSecurityLevelValidator : public Validator
{
public:
    ServerLevelSecurityLevelValidator(ServerLevelSecurityLevelValidator const&) = delete;
    ServerLevelSecurityLevelValidator& operator=(ServerLevelSecurityLevelValidator const&) = delete;
    ServerLevelSecurityLevelValidator(ServerLevelSecurityLevelValidator&&)                 = delete;
    ServerLevelSecurityLevelValidator& operator=(ServerLevelSecurityLevelValidator&&) = delete;

    /// @brief Constructor
    explicit ServerLevelSecurityLevelValidator() noexcept = default;

    ~ServerLevelSecurityLevelValidator() noexcept override = default;

    std::int32_t RegisterSubfunctionLevel(
        SubfunctionType id,
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< SubfunctionType > > subfunctionPtr) noexcept
    {
        auto result = subfunctionTable_.find(id);
        if (result != subfunctionTable_.end()) {
            LogWarn() << "The Reason is duplicate registered subfunction id:" << id;
            return -1;
        }
        std::ignore = subfunctionTable_.emplace(id, subfunctionPtr);
        return 0;
    }

    bool Validate(std::shared_ptr< RequestContext > const& context) noexcept override
    {
        bool result{true};
        for (auto&& subfunction : subfunctionTable_) {
            result = subfunction.second->Validate(context);
            if (result) {
                break;
            }
        }
        if (!result) {
            context->Respond(NrcErrc::kSecurityAccessDenied, true);
        }
        LogInfo() << "server level check security level result:" << result;
        return result;
    }

private:
    std::map< SubfunctionType, std::shared_ptr< SubfunctionLevelSecurityLevelValidator< SubfunctionType > > >
        subfunctionTable_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SESSIONVALIDATOR_H_