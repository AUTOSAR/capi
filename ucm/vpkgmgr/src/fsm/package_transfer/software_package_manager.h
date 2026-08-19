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
/// @file       software_package_manager.h
/// @brief      SoftwarePackageManager implementation
/// @details
/// @date       2023-11-01
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
/// @unit_name=SoftwarePackageManager
/// @unit_description=SoftwarePackageManager implementation
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_TRANSFER_MANAGER_H_
#define VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_TRANSFER_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/vehiclepackagemanagement_common.h>

#include <list>
#include <memory>
#include <mutex>

#include "fsm/package_transfer/package_storage.h"
#include "fsm/parsing/software_package_info.h"
#include "utils/alias.h"
#include "utils/single_ton.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief TransferStartOutput
using TransferStartOutput = pkgmgr::VehiclePackageManagement::TransferStartOutput;

/// @brief SoftwarePackageManager
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00137
/// @trace_id_dd=DD_UCM_Master_00374
/// @needwork = ad
/// @endcode
class SoftwarePackageManager
{
public:
    /// @brief UPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using UPtr = std::unique_ptr< SoftwarePackageManager >;

    /// @brief Create
    /// @param packageStorage
    /// @param spkgInfos
    /// @param blockSize
    /// @param retrievedSwpkFQN2TransferIDMap
    /// @return UPtr
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00138
    /// @trace_id_dd=DD_UCM_Master_00375
    /// @needwork = ad
    /// @endcode
    static UPtr Create(PackageStorage::Ptr const &packageStorage,
                       AraList< SwPackageInfoPtr > const &spkgInfos,
                       uint32_t const blockSize,
                       ara::core::Map< ara::core::String, pkgmgr::TransferIdType > retrievedSwpkFQN2TransferIDMap);

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00139
    /// @trace_id_dd=DD_UCM_Master_00376
    /// @needwork = ad
    /// @endcode
    virtual ~SoftwarePackageManager() = default;

    /// @brief GetSwpkFQN2TransferIDMap
    /// @return TransferIdType
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00140
    /// @trace_id_dd=DD_UCM_Master_00377
    /// @needwork = ad
    /// @endcode
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType > GetSwpkFQN2TransferIDMap() const
    {
        return swpkFQN2TransferIDMap_;
    }
    /// @brief GetAllSpkgInfos
    /// @return map of SwPackageInfoPtr
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00141
    /// @trace_id_dd=DD_UCM_Master_00378
    /// @needwork = ad
    /// @endcode
    ara::core::Map< ara::core::String, SwPackageInfoPtr > const &GetAllSpkgInfos() const noexcept
    {
        return allSpkgInfoMap_;
    }
    /// @brief GetTransferdSpkgByName
    /// @param name
    /// @return SwPackageInfoPtr
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00142
    /// @trace_id_dd=DD_UCM_Master_00379
    /// @needwork = ad
    /// @endcode
    SwPackageInfoPtr const GetTransferdSpkgByName(ara::core::String const &name) const;
    /// @brief GetSpkgByName
    /// @param spkgFqn
    /// @return SwPackageInfoPtr
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00143
    /// @trace_id_dd=DD_UCM_Master_00380
    /// @needwork = ad
    /// @endcode
    SwPackageInfoPtr const GetSpkgByName(ara::core::String const &spkgFqn) const;

    /// @brief TransferNextSwpkg
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00144
    /// @trace_id_dd=DD_UCM_Master_00381
    /// @needwork = ad
    /// @endcode
    void TransferNextSwpkg();
    /// @brief TransferStart
    /// @param spkgName
    /// @param promise
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00145
    /// @trace_id_dd=DD_UCM_Master_00382
    /// @needwork = ad
    /// @endcode
    int32_t TransferStart(pkgmgr::SwNameType const &spkgName, ara::core::Promise< TransferStartOutput > &&promise);
    /// @brief TransferData
    /// @param id
    /// @param data
    /// @param blockCounter
    /// @param promise
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00146
    /// @trace_id_dd=DD_UCM_Master_00383
    /// @needwork = ad
    /// @endcode
    int32_t TransferData(pkgmgr::TransferIdType const &id,
                         pkgmgr::ByteVectorType const &data,
                         std::uint64_t const &blockCounter,
                         ara::core::Promise< void > &&promise);
    /// @brief TransferExit
    /// @param id
    /// @param promise
    /// @return exit result
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00147
    /// @trace_id_dd=DD_UCM_Master_00384
    /// @needwork = ad
    /// @endcode
    int32_t TransferExit(pkgmgr::TransferIdType const &id, ara::core::Promise< void > &&promise);
    /// @brief DeleteTransfer
    /// @param id
    /// @param promise
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00148
    /// @trace_id_dd=DD_UCM_Master_00385
    /// @needwork = ad
    /// @endcode
    void DeleteTransfer(pkgmgr::TransferIdType const &id, ara::core::Promise< void > &&promise);
    /// @brief ClearSWPKGs
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00149
    /// @trace_id_dd=DD_UCM_Master_00386
    /// @needwork = ad
    /// @endcode
    void ClearSWPKGs();

protected:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00387
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager() = default;

public:
    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00388
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager(SoftwarePackageManager const &other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00389
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager(SoftwarePackageManager &&other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00390
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager &operator=(SoftwarePackageManager const &other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00391
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager &operator=(SoftwarePackageManager &&other) = delete;

private:
    /// @brief Size of one transfer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00392
    /// @needwork = dda
    /// @endcode
    uint32_t blocksize_{kInt32_40U * kInt32_1024U};
    /// @brief Data is for query only, no modification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00393
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, SwPackageInfoPtr >
        allSpkgInfoMap_;  /// Software information collection, used for quick lookup of software package information

    /// @brief Number of parallel transfers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00394
    /// @needwork = dda
    /// @endcode
    uint32_t const kParallelTransferNum{3};
    /// @brief Used for transferring software packages based on step execution order
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00395
    /// @needwork = dda
    /// @endcode
    std::list< SwPackageInfoPtr > spkgInfos_;
    /// @brief Transferring software package information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00396
    /// @needwork = dda
    /// @endcode
    std::list< SwPackageInfoPtr > curSpkgInfoList_;
    /// @brief Transferring software package information key: software package fqn, value: software package transfer class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00397
    /// @needwork = dda
    /// @endcode
    std::map< pkgmgr::TransferIdType, SwPackageInfoPtr > transferingSpkgs_;

    /// @brief mtx_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00398
    /// @needwork = dda
    /// @endcode
    std::mutex mtx_;
    /// @brief Transferred software packages key: software package fqn, value: software package transfer class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00399
    /// @needwork = dda
    /// @endcode
    std::map< pkgmgr::SwNameType, SwPackageInfoPtr > spkgTransferdMap_;
    /// @brief packageStorage_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00400
    /// @needwork = dda
    /// @endcode
    PackageStorage::Ptr packageStorage_;

    /// @brief  Mapping from software package FQN to its TransferIdType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00401
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType > swpkFQN2TransferIDMap_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00402
    /// @needwork = dda
    /// @endcode
    ara::log::Logger &log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("SoftwarePackageManager context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_TRANSFER_H_
