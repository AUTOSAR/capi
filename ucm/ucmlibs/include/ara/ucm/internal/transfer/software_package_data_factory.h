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
/// @file       software_package_data_factory.h
/// @brief      The SoftwarePackageDataFactory interface which creates SoftwarePackageData.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=SoftwarePackageDataFactory
/// @unit_description=The SoftwarePackageDataFactory interface which creates SoftwarePackageData.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_FACTORY_H_
#define ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_FACTORY_H_

#include "per_transfer_status_storage.h"
#include "software_package_data.h"
#include "streamable_software_package_factory.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief An interface for creating SoftwarePackageData implementations of
/// StreamableSoftwarePackage interface.
///
/// The implementations created by this factory are used to parametrize
/// PackageManager service implementations.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00002
/// @trace_id_dd=DD_UCM_00294
/// @needwork = ad
/// @endcode
class SoftwarePackageDataFactory : public StreamableSoftwarePackageFactory
{
public:
    /// @brief default copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork = dda
    /// @endcode
    SoftwarePackageDataFactory(SoftwarePackageDataFactory const& other) = default;
    /// @brief default copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork = dda
    /// @endcode
    SoftwarePackageDataFactory& operator=(SoftwarePackageDataFactory const& other) = delete;
    /// @brief default move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork = dda
    /// @endcode
    SoftwarePackageDataFactory(SoftwarePackageDataFactory&& other) noexcept = default;
    /// @brief default move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork = dda
    /// @endcode
    SoftwarePackageDataFactory& operator=(SoftwarePackageDataFactory&& other) noexcept = delete;

public:
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00295
    /// @needwork = dda
    /// @endcode
    /// @brief Constructor.
    /// @param directory The directory for storing software package data.
    /// @throws no
    explicit SoftwarePackageDataFactory(AraString directory) : kDirectory{std::move(directory)} {}

    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00296
    /// @needwork = dda
    /// @endcode
    ~SoftwarePackageDataFactory() override = default;

    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00297
    /// @needwork = dda
    /// @endcode
    /// @brief Create a new SoftwarePackageData implementation.
    /// @param id The transfer ID.
    /// @param kVModelIdentifier The ModelIdentifier of the KV used to persist the transfer status.
    /// @return A unique pointer to the created SoftwarePackageData implementation.
    /// @throws no
    std::unique_ptr< StreamableSoftwarePackage > Create(TransferIdType id, AraStringView kVModelIdentifier) override
    {
        return std::make_unique< SoftwarePackageData >(id, kDirectory,
                                                       std::make_unique< PerTransferStatusStorage >(kVModelIdentifier));
        //"package_manager/package_manager_root/SwPackageTransferStatusPRPort"
    }

private:
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00298
    /// @needwork = dda
    /// @endcode
    const AraString kDirectory;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_FACTORY_H_
