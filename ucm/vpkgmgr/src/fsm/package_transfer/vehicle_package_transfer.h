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
/// @file       vehicle_package_transfer.h
/// @brief      VehiclePackageTransfer header
/// @details
/// @date       2024-07-20
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
/// @unit_name=VehiclePackageTransfer
/// @unit_description=VehiclePackageTransfer header
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_EXECUTOR_VEHICLE_PACKAGE_TRANSFER_H_
#define VPKGMGR_SRC_EXECUTOR_VEHICLE_PACKAGE_TRANSFER_H_

#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>
#include <ara/ucm/pkgmgr/vehiclepackagemanagement_common.h>

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "fsm/campaign_steps/rollout_step.h"
#include "fsm/package_transfer/package_storage.h"
#include "fsm/parsing/step_info.h"
#include "fsm/parsing/vehicle_package_parser.h"
#include "package_management_service/package_management_app.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief TransferVehiclePackageOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferVehiclePackageOutput = pkgmgr::VehiclePackageManagement::TransferVehiclePackageOutput;

/// @brief VehiclePackageTransfer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00150
/// @trace_id_dd=DD_UCM_Master_00403
/// @needwork = ad
/// @endcode
class VehiclePackageTransfer
{
public:
    /// @brief UPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using UPtr = std::unique_ptr< VehiclePackageTransfer >;

    /// @brief Create
    /// @param packageStorage
    /// @param blockSize
    /// @param vpkTransferId
    /// @return UPtr
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00151
    /// @trace_id_dd=DD_UCM_Master_00404
    /// @needwork = ad
    /// @endcode
    static UPtr Create(PackageStorage::Ptr const& packageStorage,
                       uint32_t const blockSize,
                       pkgmgr::TransferIdType const vpkTransferId);
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00152
    /// @trace_id_dd=DD_UCM_Master_00405
    /// @needwork = ad
    /// @endcode
    virtual ~VehiclePackageTransfer() = default;

    /// @brief TransferVehiclePackage
    /// @param size
    /// @param promise
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00153
    /// @trace_id_dd=DD_UCM_Master_00406
    /// @needwork = ad
    /// @endcode
    int32_t TransferVehiclePackage(uint64_t const size, ara::core::Promise< TransferVehiclePackageOutput >&& promise);
    /// @brief TransferData
    /// @param id
    /// @param data
    /// @param blockCounter
    /// @param promise
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00154
    /// @trace_id_dd=DD_UCM_Master_00407
    /// @needwork = ad
    /// @endcode
    int32_t TransferData(pkgmgr::TransferIdType const& id,
                         pkgmgr::ByteVectorType const& data,
                         std::uint64_t const& blockCounter,
                         ara::core::Promise< void >&& promise);
    /// @brief TransferExit
    /// @param id
    /// @param promise
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00155
    /// @trace_id_dd=DD_UCM_Master_00408
    /// @needwork = ad
    /// @endcode
    int32_t TransferExit(pkgmgr::TransferIdType const& id, ara::core::Promise< void >&& promise);
    /// @brief ProcessVehiclePackage
    /// @param id
    /// @param promise
    /// @param triggerSoftpkgTransfer
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00156
    /// @trace_id_dd=DD_UCM_Master_00409
    /// @needwork = ad
    /// @endcode
    int32_t ProcessVehiclePackage(pkgmgr::TransferIdType const& id,
                                  ara::core::Promise< void >&& promise,
                                  bool const triggerSoftpkgTransfer = true);

    /// @brief GetVPKTransferID
    /// @return TransferIdType
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00157
    /// @trace_id_dd=DD_UCM_Master_00410
    /// @needwork = ad
    /// @endcode
    pkgmgr::TransferIdType GetVPKTransferID() const noexcept { return vpkTransferId_; }
    /// @brief ClearVPK
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00158
    /// @trace_id_dd=DD_UCM_Master_00411
    /// @needwork = ad
    /// @endcode
    void ClearVPK();

    /// @brief Vehicle package transfer completed
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00159
    /// @trace_id_dd=DD_UCM_Master_00412
    /// @needwork = ad
    /// @endcode
    bool IsVPKTransferred();

    /// @brief Check if vehicle package zip file exists
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00160
    /// @trace_id_dd=DD_UCM_Master_00413
    /// @needwork = ad
    /// @endcode
    bool IsVPKExist();

    /// @brief Get ucm identifiers involved in the vehicle package
    /// @return Returns list of ucm identifiers
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00161
    /// @trace_id_dd=DD_UCM_Master_00414
    /// @needwork = ad
    /// @endcode
    AraList< ara::core::String > const& GetUcmIds() const noexcept { return ucmIds_; }

    /// @brief Get the repository of the vehicle package
    /// @return Returns the repository of the vehicle package
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00162
    /// @trace_id_dd=DD_UCM_Master_00415
    /// @needwork = ad
    /// @endcode
    core::String GetRepository() const { return repository_; }

private:
    /// @brief _parseVehiclePackageManifest
    /// @param vpDir
    /// @param vpsPath
    /// @param swpsPath
    /// @param swclsPath
    /// @return parse result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00416
    /// @needwork = dda
    /// @endcode
    int32_t _parseVehiclePackageManifest(ara::core::String const& vpDir,
                                         ara::core::String const& vpsPath,
                                         ara::core::String const& swpsPath,
                                         ara::core::String const& swclsPath);
    /// @brief _getCampaignInfoFromParser
    /// @param vpParser
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00417
    /// @needwork = dda
    /// @endcode
    void _getCampaignInfoFromParser(VehiclePackageParser const& vpParser) const;
    /// @brief GetCondition
    /// @param policyVec
    /// @return str
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00418
    /// @needwork = dda
    /// @endcode
    static ara::core::String GetCondition(ara::core::Vector< ara::core::String > const& policyVec);

private:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00419
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer() = default;

public:
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00420
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer(VehiclePackageTransfer const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00421
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer(VehiclePackageTransfer&& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00422
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer& operator=(VehiclePackageTransfer const& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00423
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer& operator=(VehiclePackageTransfer&& other) = delete;

private:
    /// @brief packageStorage_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00424
    /// @needwork = dda
    /// @endcode
    PackageStorage::Ptr packageStorage_{nullptr};

    /// @brief Used to remember the current operation step for easy cancellation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00425
    /// @needwork = dda
    /// @endcode
    pkgmgr::TransferIdType vpkTransferId_{};
    /// @brief blocksize_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00426
    /// @needwork = dda
    /// @endcode
    uint32_t blocksize_{0U};

    /// @brief Vehicle package parser
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00427
    /// @needwork = dda
    /// @endcode
    VehiclePackageParser vpParser_{};

    /// @brief IDs of all UCMs within the domain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00428
    /// @needwork = dda
    /// @endcode
    AraList< ara::core::String > ucmIds_{};

    /// @brief Repository of the vehicle package
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00429
    /// @needwork = dda
    /// @endcode
    ara::core::String repository_{""};

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00430
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("VehiclePackageTransfer context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_EXECUTOR_VEHICLE_PACKAGE_TRANSFER_H_
