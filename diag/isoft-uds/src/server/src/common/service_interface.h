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
/// @file       service_interface.h
/// @brief      This file provides service_interface functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=46d07d93-c33e-4289-bc27-03a629cbae36
/// @unit_name=ServiceInterface
/// @unit_description=This file provides service_interface functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICEINTERFACE_H_
#define ISOFT_UDS_SERVER_SERVICEINTERFACE_H_

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "common/request_context.h"
#include "common/validator.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Service interface class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00853
    /// @needwork = dda
    /// @endcode
    ServiceInterface(ServiceInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00854
    /// @needwork = dda
    /// @endcode
    ServiceInterface& operator=(ServiceInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00855
    /// @needwork = dda
    /// @endcode
    ServiceInterface(ServiceInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00856
    /// @needwork = dda
    /// @endcode
    ServiceInterface& operator=(ServiceInterface&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit ServiceInterface() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual ~ServiceInterface() noexcept = default;

    /// @brief Get session ID validator
    /// @return Session ID validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual std::vector< ValidatorPtr > GetSidValidator() noexcept = 0;

    /// @brief Request processing function
    /// @param context Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual void Process(std::shared_ptr< RequestContext > context) noexcept = 0;

    /// @brief Confirm function
    /// @param status Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual void Confirm(RequestContext& request, ResponseStatusType status) noexcept = 0;

    /// @brief Get P4 server maximum time
    /// @param subfunction Subfunction
    /// @param assistParam Assist parameter
    /// @return Time, 0 invalid
    /// @return Time, 0 invalid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_
    /// @trace_id_dd=DD_UDS_00857
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00858
    /// @needwork = dda
    /// @endcode
    virtual std::uint32_t GetP4ServerMax(std::int16_t const subfunction = -1,
                                         std::int32_t const assistParam = -1) noexcept
    {
        std::ignore = subfunction;
        std::ignore = assistParam;
        return p4ServerMax_;
    }

protected:
    /// @brief P4 server maximum time
    /// @return Time, 0 invalid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint32_t p4ServerMax_{0U};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICEINTERFACE_H_