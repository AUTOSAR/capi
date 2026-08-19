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
/// @file       service_x37.h
/// @brief      This file provides service_x37 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/transfer_managment
/// @interface_level=software
/// @trace_id_sr=7c778c69-edd0-426b-ab8b-87bdaf7c49fd
/// @unit_name=service_x37.h
/// @unit_description=This file provides service_x37 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X37_H_
#define ISOFT_UDS_SERVER_SERVICE_X37_H_

#include <cstdint>
#include <map>
#include <memory>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "transfer_manager.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief This file provides service_x37 functionality
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02425
/// @needwork = dd
/// @endcode
class ServiceX37 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00636
    /// @needwork = dda
    /// @endcode
    ServiceX37(ServiceX37 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00637
    /// @needwork = dda
    /// @endcode
    ServiceX37& operator=(ServiceX37 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00638
    /// @needwork = dda
    /// @endcode
    ServiceX37(ServiceX37&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00639
    /// @needwork = dda
    /// @endcode
    ServiceX37& operator=(ServiceX37&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01160
    /// @needwork = dd
    /// @endcode
    ServiceX37() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01161
    /// @needwork = dd
    /// @endcode
    ~ServiceX37() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] transferManagerPtr Transfer manager pointer
    /// @param[in] config Transfer management configuration
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01162
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                            TransferManagment& config) noexcept;

    /// @brief Get session ID validator
    /// @return Session ID validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01163
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param[in] context Request context
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01164
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param[in] request Request context
    /// @param[in] status Status
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01165
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

    /// @brief Get P4 server max connections
    /// @param[in] subfunction Subfunction
    /// @param[in] assistParam Assist parameter
    /// @return P4 server max connections
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01166
    /// @needwork = dd
    /// @endcode
    virtual std::uint32_t GetP4ServerMax(std::int16_t const subfunction = -1,
                                         std::int32_t const assistParam = -1) noexcept;

private:
    /// @brief Transfer manager pointer
    /// @name transferManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01167
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_;

    /// @brief P4 server max connections mapping table
    /// @name mapP4ServerMax_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01168
    /// @needwork = dd
    /// @endcode
    std::map< RequestServerType, uint32_t > mapP4ServerMax_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X37_H_