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
/// @file       session_validator.h
/// @brief      This file provides session_validator functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/session_management
/// @interface_level=software
/// @trace_id_sr=6712702a-a9f4-4823-9e9f-85a27539221b
/// @unit_name=SessionValidator
/// @unit_description=This file provides session_validator functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SESSIONVALIDATOR_H_
#define ISOFT_UDS_SERVER_SESSIONVALIDATOR_H_

#include "common/log.h"
#include "common/validator.h"
#include "session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief SubfunctionLevelSessionValidator class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02407
/// @needwork = dd
/// @endcode
template < class SubfunctionType = std::uint8_t >
class SubfunctionLevelSessionValidator : public Validator
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00594
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelSessionValidator(SubfunctionLevelSessionValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00595
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelSessionValidator& operator=(SubfunctionLevelSessionValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00596
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelSessionValidator(SubfunctionLevelSessionValidator&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00597
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelSessionValidator& operator=(SubfunctionLevelSessionValidator&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01053
    /// @needwork = dd
    /// @endcode
    explicit SubfunctionLevelSessionValidator() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01054
    /// @needwork = dd
    /// @endcode
    ~SubfunctionLevelSessionValidator() noexcept override = default;

    /// @brief Initialization function
    /// @param id Subfunction ID
    /// @param config Session configuration
    /// @param sessionManagerPtr Session manager pointer
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01055
    /// @needwork = dd
    /// @endcode
    std::uint32_t Initialize(SubfunctionType id,
                             std::set< SessionId > const& config,
                             std::shared_ptr< SessionManager > const& sessionManagerPtr) noexcept
    {
        id_ = id;
        if (sessionManagerPtr.get() == nullptr) {
            LogWarn() << "paramter sessionManagerPtr is nullptr,Please sessionManagerPtr paramter";
        }
        if (config.empty()) {
            LogWarn() << "paramter config is empty! Please Check";
            return -1;
        }
        for (auto&& sessionId : config) {
            std::shared_ptr< Session > sessionPtr = sessionManagerPtr->GetSession(sessionId);
            if (sessionPtr.get() == nullptr) {
                LogWarn() << "session id invalid! session id:" << sessionId;
                return -1;
            }
            std::ignore = accessPermissionTable_.emplace(sessionPtr);
        }
        return 0;
    }

    /// @brief Validate function
    /// @param context Request context pointer
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01056
    /// @needwork = dd
    /// @endcode
    bool Validate(std::shared_ptr< RequestContext > const&) noexcept override
    {
        bool result{false};
        for (auto&& session : accessPermissionTable_) {
            result = session->Check();
            if (result) {
                break;
            }
        }
        LogInfo() << "subfunction level: " << id_ << " check session result:" << result;
        return result;
    }

private:
    /// @brief Subfunction ID
    /// @name id_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01057
    /// @needwork = dd
    /// @endcode
    SubfunctionType id_;
    /// @brief Session permission table
    /// @name accessPermissionTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01058
    /// @needwork = dd
    /// @endcode
    std::set< std::shared_ptr< Session > > accessPermissionTable_;
};

/// @brief ServerLevelSessionValidator class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02408
/// @needwork = dd
/// @endcode
template < class SubfunctionType = std::uint8_t >
class ServerLevelSessionValidator : public Validator
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00598
    /// @needwork = dda
    /// @endcode
    ServerLevelSessionValidator(ServerLevelSessionValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00599
    /// @needwork = dda
    /// @endcode
    ServerLevelSessionValidator& operator=(ServerLevelSessionValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00600
    /// @needwork = dda
    /// @endcode
    ServerLevelSessionValidator(ServerLevelSessionValidator&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00601
    /// @needwork = dda
    /// @endcode
    ServerLevelSessionValidator& operator=(ServerLevelSessionValidator&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01059
    /// @needwork = dd
    /// @endcode
    explicit ServerLevelSessionValidator() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01060
    /// @needwork = dd
    /// @endcode
    ~ServerLevelSessionValidator() noexcept override = default;

    /// @brief Register subfunction level session validator
    /// @param id Subfunction ID
    /// @param subfunctionPtr Subfunction level session validator pointer
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01061
    /// @needwork = dd
    /// @endcode
    std::int32_t RegisterSubfunctionLevel(
        SubfunctionType id,
        std::shared_ptr< SubfunctionLevelSessionValidator< SubfunctionType > > subfunctionPtr) noexcept
    {
        auto result = subfunctionTable_.find(id);
        if (result != subfunctionTable_.end()) {
            LogWarn() << "The Reason is duplicate registered subfunction id:" << id;
            return -1;
        }
        std::ignore = subfunctionTable_.emplace(id, subfunctionPtr);
        return 0;
    }

    /// @brief Validate function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01062
    /// @needwork = dd
    /// @endcode
    bool Validate(std::shared_ptr< RequestContext > const& context) noexcept override
    {
        bool result{false};
        for (auto&& subfunction : subfunctionTable_) {
            result = subfunction.second->Validate(context);
            if (result) {
                break;
            }
        }
        if (!result) {
            LogWarn() << "ServerLevelSessionValidator::Validate| session validate fails, sid: "
                      << HexFormat(context->GetSid());
            context->Respond(NrcErrc::kServiceNotSupportedInActiveSession, true);
        }
        LogInfo() << "server level check session result:" << result;
        return result;
    }

private:
    /// @brief Subfunction level session validator table
    /// @name subfunctionTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01063
    /// @needwork = dd
    /// @endcode
    std::map< SubfunctionType, std::shared_ptr< SubfunctionLevelSessionValidator< SubfunctionType > > >
        subfunctionTable_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SESSIONVALIDATOR_H_