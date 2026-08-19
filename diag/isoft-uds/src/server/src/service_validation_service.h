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
/// @file       service_validation_service.h
/// @brief      This file provides service_validation_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=software
/// @trace_id_sr=8c25caf1-6707-49df-99c4-260a8a629818
/// @unit_name=ServiceValidationService
/// @unit_description=This file provides service_validation_service functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICEVALIDATIONSERVICE_H_
#define ISOFT_UDS_SERVER_SERVICEVALIDATIONSERVICE_H_

#include <cstdint>
#include <memory>

#include "common/request_context.h"
#include "isoft/uds/service_validation.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Service validation service
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02406
/// @needwork = dd
/// @endcode
class ServiceValidationService
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00575
    /// @needwork = dda
    /// @endcode
    ServiceValidationService(ServiceValidationService const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00576
    /// @needwork = dda
    /// @endcode
    ServiceValidationService& operator=(ServiceValidationService const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00577
    /// @needwork = dda
    /// @endcode
    ServiceValidationService(ServiceValidationService&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00578
    /// @needwork = dda
    /// @endcode
    ServiceValidationService& operator=(ServiceValidationService&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01040
    /// @needwork = dd
    /// @endcode
    explicit ServiceValidationService();

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01041
    /// @needwork = dd
    /// @endcode
    ~ServiceValidationService() noexcept = default;

    /// @brief Add manufacturer service validation
    /// @param serviceValidation Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01042
    /// @needwork = dd
    /// @endcode
    void AddManufacturerServiceValidation(
        std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept;

    /// @brief Add supplier service validation
    /// @param serviceValidation Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01043
    /// @needwork = dd
    /// @endcode
    void AddSupplierServiceValidation(std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept;

    /// @brief Validate manufacturer service
    /// @param request Request context
    /// @returns Whether validation passed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01044
    /// @needwork = dd
    /// @endcode
    bool ManufacturerServiceValidate(std::shared_ptr< RequestContext > const& request) noexcept;

    /// @brief Validate supplier service
    /// @param request Request context
    /// @returns Whether validation passed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01045
    /// @needwork = dd
    /// @endcode
    bool SupplierServiceValidate(std::shared_ptr< RequestContext > const& request) noexcept;

    /// @brief Confirm manufacturer service
    /// @param status Status
    /// @param metaInfo Meta data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01046
    /// @needwork = dd
    /// @endcode
    void ManufacturerConfirmation(ConfirmationStatusType status, MetaInfoMap& metaInfo) noexcept;

    /// @brief Confirm supplier service
    /// @param status Status
    /// @param metaInfo Meta data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01047
    /// @needwork = dd
    /// @endcode
    void SupplierConfirmation(ConfirmationStatusType status, MetaInfoMap& metaInfo) noexcept;

private:
    /// @brief Service validation interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01048
    /// @needwork = dd
    /// @endcode
    using ServiceValidationInterfacePtr = std::shared_ptr< ServiceValidationInterface >;

    /// @brief Manufacturer service validation table
    /// @name manufacturerTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01049
    /// @needwork = dd
    /// @endcode
    std::vector< ServiceValidationInterfacePtr > manufacturerTable_;
    /// @brief Supplier service validation table
    /// @name supplierTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01050
    /// @needwork = dd
    /// @endcode
    std::vector< ServiceValidationInterfacePtr > supplierTable_;

    /// @brief Manufacturer service confirmation table
    /// @name requestManufacturerConfirmTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01051
    /// @needwork = dd
    /// @endcode
    std::map< std::string, std::vector< ServiceValidationInterfacePtr > > requestManufacturerConfirmTable_;

    /// @brief Supplier service confirmation table
    /// @name requestSupplierConfirmTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01052
    /// @needwork = dd
    /// @endcode
    std::map< std::string, std::vector< ServiceValidationInterfacePtr > > requestSupplierConfirmTable_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICEVALIDATIONSERVICE_H_