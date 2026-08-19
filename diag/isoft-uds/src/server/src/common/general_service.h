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
/// @file       general_service.h
/// @brief      This file provides general_service functionality
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
/// @trace_id_sr=939abe32-1901-40cf-8013-e8b5074131c5
/// @unit_name=GeneralService
/// @unit_description=This file provides general_service functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_GENERALSERVICE_H_
#define ISOFT_UDS_SERVER_GENERALSERVICE_H_

#include "common/service_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief General service class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02583
/// @needwork = dd
/// @endcode
class GeneralService : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00844
    /// @needwork = dda
    /// @endcode
    GeneralService(GeneralService const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00845
    /// @needwork = dda
    /// @endcode
    GeneralService& operator=(GeneralService const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00846
    /// @needwork = dda
    /// @endcode
    GeneralService(GeneralService&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00847
    /// @needwork = dda
    /// @endcode
    GeneralService& operator=(GeneralService&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01969
    /// @needwork = dd
    /// @endcode
    explicit GeneralService() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01970
    /// @needwork = dd
    /// @endcode
    ~GeneralService() noexcept override = default;

    /// @brief Request processing function
    /// @param context Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01971
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01972
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

protected:
    /// @brief Get size
    /// @param subfunction Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01973
    /// @needwork = dd
    /// @endcode
    virtual std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept = 0;

    /// @brief IsSupport function
    /// @param subfunction Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01974
    /// @needwork = dd
    /// @endcode
    virtual bool IsSupport(std::uint8_t subfunction) noexcept = 0;

    /// @brief Get subfunction authentication validator
    /// @param subfunction Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01975
    /// @needwork = dd
    /// @endcode
    virtual std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept = 0;

    /// @brief Get subfunction session validator
    /// @param subfunction Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01976
    /// @needwork = dd
    /// @endcode
    virtual std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept = 0;

    /// @brief SupportSecurityLevel function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01977
    /// @needwork = dd
    /// @endcode
    virtual bool SupportSecurityLevel() noexcept = 0;

    /// @brief Get subfunction security level validator
    /// @param subfunction Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01978
    /// @needwork = dd
    /// @endcode
    virtual std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept = 0;

    /// @brief ParseAndResponse function
    /// @code{.isoft}
    /// @ /// @return Return value
    /// @needwork = dd
    /// @endcode
    virtual void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_BASE_H_