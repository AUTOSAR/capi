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
/// @file       service_x34.h
/// @brief      This file provides service_x34 functionality
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
/// @trace_id_sr=e1eaa62e-dd4a-415c-bda6-75307e25cdb8
/// @unit_name=service_x34.h
/// @unit_description=This file provides service_x34 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X34_H_
#define ISOFT_UDS_SERVER_SERVICE_X34_H_

#include <memory>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "transfer_manager.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief The client uses the requestDownload service to initiate data transfer (download) from the client to the server.
/// After the server receives the requestDownload request message, the server should take all necessary actions to receive data before sending a positive response message.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02417
/// @needwork = dd
/// @endcode
class ServiceX34 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00616
    /// @needwork = dda
    /// @endcode
    ServiceX34(ServiceX34 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00617
    /// @needwork = dda
    /// @endcode
    ServiceX34& operator=(ServiceX34 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00618
    /// @needwork = dda
    /// @endcode
    ServiceX34(ServiceX34&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00619
    /// @needwork = dda
    /// @endcode
    ServiceX34& operator=(ServiceX34&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01105
    /// @needwork = dd
    /// @endcode
    ServiceX34() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01106
    /// @needwork = dd
    /// @endcode
    ~ServiceX34() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] transferManagerPtr Transfer manager pointer
    /// @param[in] p4ServerMax Server max P4
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01107
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                            uint32_t const p4ServerMax) noexcept;

    /// @brief Get session ID validator
    /// @return Returns session ID validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01108
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
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
    /// @trace_id_dd=DD_UDS_01109
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Transfer manager pointer
    /// @name transferManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01110
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X34_H_