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
/// @file       service_x19.h
/// @brief      This file provides service_x19 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=225207ed-d7c2-4e18-8249-5e4f4df423ed
/// @unit_name=service_x19.h
/// @unit_description=This file provides service_x19 functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_SERVICE_X19_H__
#define __ISOFT_UDS_FAULT_SERVICE_X19_H__

#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "common/request_context.h"
#include "fault_management/fault_manager.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief This service allows the client to read the status of server resident diagnostic trouble code (DTC) information from any server or group of servers within the vehicle.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02528
/// @needwork = dd
/// @endcode
class ServiceX19 : public GeneralService
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00799
    /// @needwork = dda
    /// @endcode
    ServiceX19(ServiceX19 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00800
    /// @needwork = dda
    /// @endcode
    ServiceX19& operator=(ServiceX19 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00801
    /// @needwork = dda
    /// @endcode
    ServiceX19(ServiceX19&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00802
    /// @needwork = dda
    /// @endcode
    ServiceX19& operator=(ServiceX19&&) = delete;

    /// @brief Constructor
    /// @param[in] conversationManager Session management
    /// @param[in] eventManager DID management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01755
    /// @needwork = dd
    /// @endcode
    explicit ServiceX19(FaultManager& faultManager) noexcept : faultManager_{faultManager} {}

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01756
    /// @needwork = dd
    /// @endcode
    ~ServiceX19() noexcept override = default;

    /// @brief Initialization, load configuration file
    /// @param[in] dmc Configuration file
    /// @param[in] node File file node
    /// @param[in] apt Access permission table
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01757
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ReadDTCInformationInstanceConfig& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept;

    /// @brief Get SID validator
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01758
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

private:
    /// @brief Get size
    /// @param subfunction Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01759
    /// @needwork = dd
    /// @endcode
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief IsSupport function
    /// @param subfunction Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01760
    /// @needwork = dd
    /// @endcode
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief Get authentication validator of subfunction
    /// @param subfunction Parameter
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01761
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief Get session validator of subfunction
    /// @param subfunction Parameter
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01762
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief SupportSecurityLevel function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01763
    /// @needwork = dd
    /// @endcode
    bool SupportSecurityLevel() noexcept override;

    /// @brief Get SecurityLevel validator of subfunction
    /// @param[in] subfunction Subfunction ID
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01764
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief ParseAndResponse function
    /// @param[in] context Request context
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01765
    /// @needwork = dd
    /// @endcode
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Subfunction instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01766
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Subfunction ID
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint8_t subfunctionNumber{0U};
        /// @brief Authentication validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        /// @brief Session validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator{};
        /// @brief SecurityLevel validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator{nullptr};
    };

    /// @brief Subfunction instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @needwork = dd
    /// @endcode
    std::map< std::uint8_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @brief Authentication validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01767
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_;
    /// @brief Session validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01768
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator_{};
    /// @brief SecurityLevel validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01769
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator_{nullptr};

    /// @brief Fault manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01770
    /// @needwork = dd
    /// @endcode
    FaultManager& faultManager_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X19_H_