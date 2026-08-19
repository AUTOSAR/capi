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
/// @file       service_x38.h
/// @brief      This file provides service_x38 functionality
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
/// @trace_id_sr=a8c4265c-ca44-40c6-bb88-956fa3042bc3
/// @unit_name=service_x38.h
/// @unit_description=This file provides service_x38 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X38_H_
#define ISOFT_UDS_SERVER_SERVICE_X38_H_

#include <memory>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "transfer_manager.h"

namespace isoft {
namespace uds {
namespace server {

class Server;

/// @brief The client uses the RequestUpload service to initiate data transfer (upload) from the server to the client.
/// After the server receives the requestUpload request message, the server should take all necessary actions to send data before sending a positive response message.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02420
/// @needwork = dd
/// @endcode
class ServiceX38 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00632
    /// @needwork = dda
    /// @endcode
    ServiceX38(ServiceX38 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00633
    /// @needwork = dda
    /// @endcode
    ServiceX38& operator=(ServiceX38 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00634
    /// @needwork = dda
    /// @endcode
    ServiceX38(ServiceX38&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00635
    /// @needwork = dda
    /// @endcode
    ServiceX38& operator=(ServiceX38&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01122
    /// @needwork = dd
    /// @endcode
    ServiceX38() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01123
    /// @needwork = dd
    /// @endcode
    ~ServiceX38() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] transferManagerPtr Transfer manager pointer
    /// @param[in] p4ServerMax 4-byte max value
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01124
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                            uint32_t const p4ServerMax) noexcept;

    /// @brief Get session ID validator
    /// @return Session ID validator vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01125
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param[in] context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01126
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param[in] request Request context
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01127
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Transfer manager pointer
    /// @name transferManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01128
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X38_H_