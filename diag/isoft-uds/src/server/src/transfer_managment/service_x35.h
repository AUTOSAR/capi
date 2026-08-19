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
/// @file       service_x35.h
/// @brief      This file provides service_x35 functionality
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
/// @trace_id_sr=3c60794a-89d5-44eb-9913-58d9a89f1805
/// @unit_name=service_x35.h
/// @unit_description=This file provides service_x35 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X35_H_
#define ISOFT_UDS_SERVER_SERVICE_X35_H_

#include <memory>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "transfer_manager.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief The client uses the RequestUpload service to initiate data transfer (upload) from the server to the client.
/// After the server receives the requestUpload request message, the server should take all necessary actions to send data before sending a positive response message.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02418
/// @needwork = dd
/// @endcode
class ServiceX35 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00620
    /// @needwork = dda
    /// @endcode
    ServiceX35(ServiceX35 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00621
    /// @needwork = dda
    /// @endcode
    ServiceX35& operator=(ServiceX35 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00622
    /// @needwork = dda
    /// @endcode
    ServiceX35(ServiceX35&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00623
    /// @needwork = dda
    /// @endcode
    ServiceX35& operator=(ServiceX35&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01111
    /// @needwork = dd
    /// @endcode
    ServiceX35() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01112
    /// @needwork = dd
    /// @endcode
    ~ServiceX35() noexcept override = default;

    /// @brief Initialize service
    /// @param[in] dmc Configuration file
    /// @param[in] node File file node
    /// @param[in] apt Access permission table
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01113
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                            uint32_t const p4ServerMax) noexcept;

    /// @brief Get session ID validator
    /// @return Session ID validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01114
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process request
    /// @param[in] context Request context
    /// @param[in] sessionId Session ID
    /// @param[in] securityLevelId Security level
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01115
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Transfer manager pointer
    /// @name transferManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01116
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X35_H_