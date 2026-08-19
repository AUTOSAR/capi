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
/// @file       software_package_info.h
/// @brief      SoftwarePackageInfo header
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
/// @unit_name=SoftwarePackageInfo
/// @unit_description=SoftwarePackageInfo header
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_INFO_H_
#define VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_INFO_H_

#include <atomic>
#include <memory>
#include <utility>

#include "ara/ucm/pkgmgr/impl_type_actiontype.h"
#include "ara/ucm/pkgmgr/impl_type_swpackagestatetype.h"
#include "ara/ucm/pkgmgr/impl_type_transferidtype.h"
#include "fsm/parsing/step_info.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Union of software package and software cluster information, and transfer progress
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00317
/// @needwork = ad
/// @endcode
struct SoftwarePackageInfo
{
    /// @brief OTA->UCMM transfer id
    ara::ucm::pkgmgr::TransferIdType transferId{0};
    /// @brief UCMM->UCM transfer id
    ///ara::ucm::pkgmgr::TransferIdType subTransferId{0};.

    /// @brief shortName
    ara::core::String shortName;
    /// @brief fqn unique identifier
    ara::core::String fqn;
    /// @brief typeApproval This attribute contains country-specific certification information
    ara::core::String typeApproval;
    /// @brief license
    ara::core::String license;
    /// @brief releaseNotes
    ara::core::String releaseNotes;
    /// @brief actionType Upgrade type --- currently unused
    ara::ucm::pkgmgr::ActionType actionType;
    /// @brief isReboot true: need to restart ucm, false: no need to restart ucm    --- actually effective
    bool isReboot{false};
    /// @brief compressedSize Compressed package size in bytes
    uint32_t compressedSize{0U};

    /// @brief packagerSignature Signature certificate
    ///ara::core::String packagerSignature;.
    /// @brief minUCMSupportedVersion
    ///ara::core::String minUCMSupportedVersion{"1.0.0-0"};.
    /// @brief maxUCMSupportedVersion
    ///ara::core::String maxUCMSupportedVersion{"1.0.0-0"};.
    /// @brief version Version number
    ara::core::String version{"0.0.0-0"};
    /// @brief softwareCluster Software cluster name
    ara::core::String softwareCluster;

    /// Below are used to store information during transfer
    /// @brief storing
    SoftwarePackageStoringEnum storing{SoftwarePackageStoringEnum::kUcmMaster};
    /// @brief temporaryStorePath Storage path
    ara::core::String temporaryStorePath;
    /// @brief consecutiveBytesReceived
    std::atomic_uint64_t consecutiveBytesReceived{0};
    /// @brief consecutiveBlocksReceived
    std::atomic_uint64_t consecutiveBlocksReceived{0};
    /// @brief state
    ara::ucm::pkgmgr::SwPackageStateType state{ara::ucm::pkgmgr::SwPackageStateType::kTransferring};

    /// @brief retry If UCMM->UCM retransmission exceeds 2 times, delete the local package on ucmm and re-fetch from ota
    ///int32_t retry{0};
    /// @brief time Timestamp
    ///uint64_t time{0};
};

/// @brief SwPackageInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwPackageInfoPtr = std::shared_ptr< SoftwarePackageInfo >;

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_EXECUTOR_SOFTWARE_PACKAGE_INFO_H_