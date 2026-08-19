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
/// @file       data_transfer.h
/// @brief      transfer software package to UCM using PackageManagement service
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=DataTransfer
/// @unit_description=transfer software package to UCM using PackageManagement service
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_
#define ARA_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_

#include <ara/log/logger.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "ara/ucm/pkgmgr/packagemanagement_proxy.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00380
/// @trace_id_dd=DD_UCM_Master_00765
/// @needwork = ad
/// @endcode
template < class PackageManagementService >
/// @brief DataTransfer
class DataTransfer
{
public:
    /// @brief Initializes this object with a reference to the PackageManagement
    /// service
    ///
    /// @param packageManagementService The instance of the PackageManagement
    /// service to be used
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00381
    /// @trace_id_dd=DD_UCM_Master_00766
    /// @needwork = ad
    /// @endcode
    explicit DataTransfer(std::shared_ptr< PackageManagementService > const packageManagementService);
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00382
    /// @trace_id_dd=DD_UCM_Master_00767
    /// @needwork = ad
    /// @endcode
    virtual ~DataTransfer() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00383
    /// @trace_id_dd=DD_UCM_Master_00768
    /// @needwork = ad
    /// @endcode
    DataTransfer(DataTransfer const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00384
    /// @trace_id_dd=DD_UCM_Master_00769
    /// @needwork = ad
    /// @endcode
    DataTransfer& operator=(DataTransfer const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00385
    /// @trace_id_dd=DD_UCM_Master_00770
    /// @needwork = ad
    /// @endcode
    DataTransfer(DataTransfer&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00386
    /// @trace_id_dd=DD_UCM_Master_00771
    /// @needwork = ad
    /// @endcode
    DataTransfer& operator=(DataTransfer&& other) = delete;

    // /// @brief Sets the block size of transmitted data blocks
    // ///
    // /// @param size the block size in number of bytes
    /// virtual void SetBlockSize(uint32_t size);

    /// @brief Initializes a new session
    ///
    /// @param size The size of the data that should be transmitted
    ///
    /// @return Boolean indicating whether the session was successfully initiated
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00387
    /// @trace_id_dd=DD_UCM_Master_00772
    /// @needwork = ad
    /// @endcode
    virtual bool InitSession(uint64_t size);

    /// @brief Transfers data to UCM sending the provided package
    /// to UCM, possibly by using multiple smaller data blocks accoring to the
    /// configured block size.
    ///
    /// @param packagePath The file that should be sent to UCM
    ///
    /// @return Boolean indicating whether the data was successfully sent
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00388
    /// @trace_id_dd=DD_UCM_Master_00773
    /// @needwork = ad
    /// @endcode
    virtual bool Transfer(ara::core::String const& packagePath);

    /// @brief Exits the current session
    ///
    /// @return Boolean indicating whether this session was successfully exited
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00389
    /// @trace_id_dd=DD_UCM_Master_00774
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ExitSession();

    /// @brief Delete SoftwarePackage
    ///
    /// @param transferId Id of the package to be deleted
    ///
    /// @returns Enum indicating whether deleting package was sucessful
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00390
    /// @trace_id_dd=DD_UCM_Master_00775
    /// @needwork = ad
    /// @endcode
    virtual bool DeleteSwPackage(ara::ucm::pkgmgr::TransferIdType const& transferId);

    /// @brief Get SoftwarePackages
    ///
    /// @returns List of all Software Packages that have successfully transferd
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00391
    /// @trace_id_dd=DD_UCM_Master_00776
    /// @needwork = ad
    /// @endcode
    virtual pkgmgr::PackageManagement::GetSwPackagesOutput GetSwPackages();

    /// @brief Returns the session id of the current session.
    ///
    /// @return The id of the current session
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00392
    /// @trace_id_dd=DD_UCM_Master_00777
    /// @needwork = ad
    /// @endcode
    virtual pkgmgr::TransferIdType GetTransferId() noexcept;

private:
    /// @brief The instance of the PackageManagement service used
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00778
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< PackageManagementService > packageManagementService_;

    /// @brief The configured block size for transmitting data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00779
    /// @needwork = dda
    /// @endcode
    uint32_t blockSize_;

    /// @brief The id of the current transfer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00780
    /// @needwork = dda
    /// @endcode
    pkgmgr::TransferIdType currentSession_;

    /// @brief For logging errors during transferring.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00781
    /// @needwork = dda
    /// @endcode
    log::Logger& logger_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_
