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
/// @file       package_storage.h
/// @brief      PackageStorage implementation
/// @details
/// @date       2023-10-30
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=PackageStorage
/// @unit_description=PackageStorage implementation
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_EXECUTOR_PACKAGE_STORAGE_H_
#define VPKGMGR_SRC_EXECUTOR_PACKAGE_STORAGE_H_

#include <ara/core/result.h>
#include <ara/core/string_view.h>
#include <ara/ucm/internal/extraction/filesystem.h>
#include <ara/ucm/internal/extraction/lib_poco_zip_extractor.h>
#include <ara/ucm/internal/extraction/software_package_extractor.h>  // pkgmgr::SoftwarePackageExtractor
#include <ara/ucm/internal/transfer/serial_id_generator.h>
#include <ara/ucm/internal/transfer/software_package_data.h>
#include <ara/ucm/internal/transfer/software_package_data_factory.h>
#include <ara/ucm/internal/transfer/streamable_software_package_factory.h>  // pkgmgr::StreamableSoftwarePackage

#include <memory>
#include <utility>

#include "fsm/package_transfer/synchronized_storage.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief PackageStorage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00480
/// @trace_id_dd=DD_UCM_Master_00454
/// @needwork = ad
/// @endcode
class PackageStorage
{
public:
    /// @brief shared_ptr of PackageStorage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< PackageStorage >;

    /// @brief Create
    /// @param vpkgDir
    /// @return shared_ptr of PackageStorage
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00455
    /// @needwork = dda
    /// @endcode
    static Ptr Create(ara::core::String const& vpkgDir);

    /// @brief TransferStart
    /// @param size
    /// @return result of TransferIdType
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00456
    /// @needwork = dda
    /// @endcode
    ara::core::Result< pkgmgr::TransferIdType > TransferStart(uint64_t const size);
    /// @brief TransferData
    /// @param id
    /// @param data
    /// @param blockCounter
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00457
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > TransferData(pkgmgr::TransferIdType const& id,
                                           pkgmgr::ByteVectorType const& data,
                                           std::uint64_t const& blockCounter) const;
    /// @brief TransferExit
    /// @param id
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00458
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::String > TransferExit(pkgmgr::TransferIdType const& id) const;
    /// @brief DeleteTransfer
    /// @param id
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00459
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > DeleteTransfer(pkgmgr::TransferIdType const& id) const;
    /// @brief CleanVehiclePackage
    /// @param id
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00460
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > CleanVehiclePackage(pkgmgr::TransferIdType const& id) const;
    /// @brief GetPackagePath
    /// @param id
    /// @return result with Package Path
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00461
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::String > GetPackagePath(pkgmgr::TransferIdType const& id) const;

    /// @brief Extract
    /// @param archivePath
    /// @param extractionDirectory
    /// @return bool
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00462
    /// @needwork = dda
    /// @endcode
    bool Extract(ara::core::String const& archivePath, ara::core::String const& extractionDirectory) const;
    /// @brief RetrievePackagesData
    /// @param vpkgDir
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00463
    /// @needwork = dda
    /// @endcode
    void RetrievePackagesData(ara::core::String const& vpkgDir) const;
    /// @brief GetPackageStatus
    /// @param id
    /// @return result of SwPackageStateType
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00464
    /// @needwork = dda
    /// @endcode
    ara::core::Result< pkgmgr::SwPackageStateType > GetPackageStatus(pkgmgr::TransferIdType const& id) const;

private:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00465
    /// @needwork = dda
    /// @endcode
    PackageStorage(void) = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00466
    /// @needwork = dda
    /// @endcode
    virtual ~PackageStorage(void) = default;

public:
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00467
    /// @needwork = dda
    /// @endcode
    PackageStorage(PackageStorage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00468
    /// @needwork = dda
    /// @endcode
    PackageStorage(PackageStorage&& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00469
    /// @needwork = dda
    /// @endcode
    PackageStorage& operator=(PackageStorage const& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00470
    /// @needwork = dda
    /// @endcode
    PackageStorage& operator=(PackageStorage&& other) = delete;

private:
    /// @brief generator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00471
    /// @needwork = dda
    /// @endcode
    pkgmgr::SerialIDGenerator< pkgmgr::TransferIdType > generator_;
    /// @brief softwarePackageExtractor_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00472
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< pkgmgr::SoftwarePackageExtractor > softwarePackageExtractor_;
    /// @brief softwarePackageFactory_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00473
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< pkgmgr::StreamableSoftwarePackageFactory > softwarePackageFactory_;
    /// @brief packagesData_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00474
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SynchronizedStorage< pkgmgr::TransferIdType, pkgmgr::StreamableSoftwarePackage > > packagesData_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif