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
/// @file       service_x36.h
/// @brief      This file provides service_x36 functionality
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
/// @trace_id_sr=d127a46b-b430-4945-b255-b3e288a35ada
/// @unit_name=service_x36.h
/// @unit_description=This file provides service_x36 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X36_H_
#define ISOFT_UDS_SERVER_SERVICE_X36_H_

#include <cstdint>
#include <map>
#include <memory>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "transfer_manager.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief The client uses the TransferData service to transfer data from the client to the server (download) or from the server to the client (upload).
////The data transfer direction is defined by the previous RequestDownload or RequestUpload service. If the client initiates
/// RequestDownload, the data to be downloaded is contained in the transferRequestParameter parameter in the TransferData request message.
/// parameter. If the client initiates RequestUpload, the data to be uploaded is contained in the
/// transferResponseParameter parameter information(s) in the TransferData response. The TransferData service request includes a
/// blockSequenceCounter for improved error handling in case of TransferData service failure in a sequence of multiple TransferData requests. When receiving a RequestDownload(Ox34) or RequestUpload (Ox35) request message, the server
/// The server's blockSequenceCounter must be initialized to one. This means that the first TransferData (Ox36) request message following the RequestDownload(Ox34) or RequestUpload (Ox35) request message starts with a blockSequenceCounter of 1.
///
///
/// 
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02419
/// @needwork = dd
/// @endcode
class ServiceX36 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00624
    /// @needwork = dda
    /// @endcode
    ServiceX36(ServiceX36 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00625
    /// @needwork = dda
    /// @endcode
    ServiceX36& operator=(ServiceX36 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00626
    /// @needwork = dda
    /// @endcode
    ServiceX36(ServiceX36&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00627
    /// @needwork = dda
    /// @endcode
    ServiceX36& operator=(ServiceX36&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01117
    /// @needwork = dd
    /// @endcode
    ServiceX36() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01118
    /// @needwork = dd
    /// @endcode
    ~ServiceX36() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] transferManagerPtr Transfer manager pointer
    /// @param[in] config Configuration
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01119
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                            TransferManagment& config) noexcept;

    /// @brief Get session ID validator function
    /// @return Session ID validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01120
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param[in] context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01121
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00628
    /// @needwork = dda
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00629
    /// @needwork = dda
    /// @endcode
    virtual std::uint32_t GetP4ServerMax(std::int16_t const subfunction = -1,
                                         std::int32_t const assistParam = -1) noexcept;

private:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00630
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00631
    /// @needwork = dda
    /// @endcode
    std::map< RequestServerType, uint32_t > mapP4ServerMax_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X36_H_