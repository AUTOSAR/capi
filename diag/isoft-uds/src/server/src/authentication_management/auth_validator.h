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
/// @file       auth_validator.h
/// @brief      This file provides auth_validator functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level=software
/// @trace_id_sr=35909ff6-691f-4fa2-aea7-fb13702241c3
/// @unit_name=AuthValidator
/// @unit_description=This file provides auth_validator functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_AUTHVALIDATOR_H_
#define ISOFT_UDS_SERVER_AUTHVALIDATOR_H_

#include "authentication_manager.h"
#include "common/log.h"
#include "common/validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Subfunction level authentication validator
/// @param SubfunctionType Subfunction type
/// @return Return value
/// @code{.isoft}
/// @needwork = dd
/// @interface_level=unit
/// @endcode
template < class SubfunctionType = std::uint8_t >
class SubfunctionLevelAuthValidator : public Validator
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00710
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelAuthValidator(SubfunctionLevelAuthValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00711
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelAuthValidator& operator=(SubfunctionLevelAuthValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00712
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelAuthValidator(SubfunctionLevelAuthValidator&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00713
    /// @needwork = dda
    /// @endcode
    SubfunctionLevelAuthValidator& operator=(SubfunctionLevelAuthValidator&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01426
    /// @needwork = dd
    /// @endcode
    explicit SubfunctionLevelAuthValidator() noexcept = default;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00714
    /// @needwork = dda
    /// @endcode

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01427
    /// @needwork = dd
    /// @endcode
    ~SubfunctionLevelAuthValidator() noexcept override = default;

    /// @brief Initialization function
    /// @param id Subfunction ID
    /// @param supportRoles Supported authentication roles
    /// @param authenticationManagerPtr Authentication management pointer
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01428
    /// @needwork = dd
    /// @endcode
    std::uint32_t Initialize(SubfunctionType id,
                             std::set< AuthRoleName > const& supportRoles,
                             std::shared_ptr< AuthenticationManager > const& authenticationManagerPtr) noexcept
    {
        id_           = id;
        supportRoles_ = supportRoles;
        if (authenticationManagerPtr.get() == nullptr) {
            LogWarn() << "paramter authenticationManagerPtr is nullptr,Please authenticationManagerPtr paramter";
        }
        if (supportRoles.empty()) {
            LogInfo() << "auth role  config is empty!";
        }
        authenticationManagerPtr_ = authenticationManagerPtr;
        return 0;
    }

    /// @brief Validate function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01429
    /// @needwork = dd
    /// @endcode
    bool Validate(std::shared_ptr< RequestContext > const& requestContextPtr) noexcept override
    {
        bool result{false};
        MessagePtr requestPtr{requestContextPtr->GetRequest()};
        Address const clientAddress{requestPtr->GetSA()};
        Result< isoft::uds::server::ClientAuthenticationPtr > getResult{
            authenticationManagerPtr_->GetClientAuthentication(clientAddress)};
        if (getResult.HasValue()) {
            ClientAuthenticationPtr clientAuthenticationPtr{getResult.Value()};
            std::uint8_t validateResult = clientAuthenticationPtr->Validate(requestPtr, supportRoles_);
            LogInfo() << "subfunction level: " << id_ << " auth result:" << validateResult;
            if (validateResult == 0) {
                result = true;
            }
        } else {
            LogWarn() << "get client authentication fais! client Address:" << clientAddress;
        }

        return result;
    }

private:
    /// @brief Subfunction ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01430
    /// @needwork = dd
    /// @endcode
    SubfunctionType id_;
    /// @brief Supported authentication roles
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01431
    /// @needwork = dd
    /// @endcode
    std::set< AuthRoleName > supportRoles_{};
    /// @brief Authentication management pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01432
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< AuthenticationManager > authenticationManagerPtr_{};
};

/// @brief Server level authentication validator
/// @param SubfunctionType Subfunction type
/// @return Return value
/// @code{.isoft}
/// @needwork = dd
/// @interface_level=unit
/// @endcode
template < class SubfunctionType = std::uint8_t >
class ServerLevelAuthValidator : public Validator
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00715
    /// @needwork = dda
    /// @endcode
    ServerLevelAuthValidator(ServerLevelAuthValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00716
    /// @needwork = dda
    /// @endcode
    ServerLevelAuthValidator& operator=(ServerLevelAuthValidator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00717
    /// @needwork = dda
    /// @endcode
    ServerLevelAuthValidator(ServerLevelAuthValidator&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00718
    /// @needwork = dda
    /// @endcode
    ServerLevelAuthValidator& operator=(ServerLevelAuthValidator&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01433
    /// @needwork = dd
    /// @endcode
    explicit ServerLevelAuthValidator() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01434
    /// @needwork = dd
    /// @endcode
    ~ServerLevelAuthValidator() noexcept override = default;

    /// @brief Register subfunction authentication validator
    /// @param id Subfunction ID
    /// @param subfunctionPtr Subfunction authentication validator pointer
    /// @return Subfunction authentication validator registration result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01435
    /// @needwork = dd
    /// @endcode
    std::int32_t RegisterSubfunctionLevel(
        SubfunctionType id, std::shared_ptr< SubfunctionLevelAuthValidator< SubfunctionType > > subfunctionPtr) noexcept
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
    /// @param context Request context pointer
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01436
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
            LogWarn() << "ServerLevelAuthValidator::Validate| auth validate fails, sid: "
                      << HexFormat(context->GetSid());
            context->Respond(NrcErrc::kAuthenticationRequired, true);
        }
        LogInfo() << "server level check auth result:" << result;
        return result;
    }

private:
    /// @brief Subfunction authentication validator table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01437
    /// @needwork = dd
    /// @endcode
    std::map< SubfunctionType, std::shared_ptr< SubfunctionLevelAuthValidator< SubfunctionType > > > subfunctionTable_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_AUTHVALIDATOR_H_