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
/// @file       vehicle_package_parse_node.h
/// @brief      VehiclePackageParseNode header
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
/// @unit_name=VehiclePackageParseNode
/// @unit_description=VehiclePackageParseNode header
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_NODE_H_
#define ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_NODE_H_

#include "utils/alias.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "fsm/parsing/step_info.h"

// none
// ucm
// ucmMaster
/// @brief TransferStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00266
/// @needwork = ad
/// @endcode
class TransferStruct
{
public:
    /// @brief storing
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00267
    /// @needwork = dda
    /// @endcode
    SoftwarePackageStoringEnum storing{SoftwarePackageStoringEnum::kNone};
    /// @brief transfer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00268
    /// @needwork = dda
    /// @endcode
    AraList< ara::core::String > transfer{};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00269
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief SoftwarePackageStepStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00270
/// @needwork = dd
/// @endcode
class SoftwarePackageStepStruct
{
public:
    /// @brief shortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00271
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{""};
    /// @brief activationSwitch
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00272
    /// @needwork = dda
    /// @endcode
    bool activationSwitch{false};
    /// @brief process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00273
    /// @needwork = dda
    /// @endcode
    ara::core::String process{""};
    /// @brief transfer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00274
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< TransferStruct > transfer{};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00275
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief UcmProcessingStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00276
/// @needwork = dd
/// @endcode
class UcmProcessingStruct
{
public:
    /// @brief shortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00277
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{""};
    /// @brief softwarePackageStep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00278
    /// @needwork = dda
    /// @endcode
    AraList< SoftwarePackageStepStruct > softwarePackageStep{};
    /// @brief ucm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00279
    /// @needwork = dda
    /// @endcode
    ara::core::String ucmStr{""};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00280
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief RolloutQualificationStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00281
/// @needwork = dd
/// @endcode
class RolloutQualificationStruct
{
public:
    /// @brief shortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00282
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{""};
    /// @brief safetyPolicy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00283
    /// @needwork = dda
    /// @endcode
    ara::core::String safetyPolicy{""};
    /// @brief ucmProcessing
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00284
    /// @needwork = dda
    /// @endcode
    AraList< UcmProcessingStruct > ucmProcessing{};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00285
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief DriverNotificationStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00286
/// @needwork = dd
/// @endcode
class DriverNotificationStruct
{
public:
    /// @brief approvalRequired
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00287
    /// @needwork = dda
    /// @endcode
    bool approvalRequired{false};
    /// @brief notificationState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00288
    /// @needwork = dda
    /// @endcode
    ara::core::String notificationState{""};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00289
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief UcmStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00290
/// @needwork = dd
/// @endcode
class UcmStruct
{
public:
    /// @brief shortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00291
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{""};
    /// @brief identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00292
    /// @needwork = dda
    /// @endcode
    ara::core::String identifier{""};  // Currently unused
    /// @brief ucmModuleInstantiation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00293
    /// @needwork = dda
    /// @endcode
    // ara::core::String ucmModuleInstantiation{""};  // Currently unused
    /// @brief ucmId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00294
    /// @needwork = dda
    /// @endcode
    ara::core::String ucmId{""};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00295
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
};

/// @brief VehiclePackageStruct
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00483
/// @trace_id_dd=DD_UCM_Master_00296
/// @needwork = dd
/// @endcode
class VehiclePackageStruct
{
public:
    /// @brief shortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00297
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{""};
    /// @brief driverNotification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00298
    /// @needwork = dda
    /// @endcode
    AraList< DriverNotificationStruct > driverNotification{};
    /// @brief packagerSignature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00299
    /// @needwork = dda
    /// @endcode
    ara::core::String packagerSignature{""};
    /// @brief certSerialNumber
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00300
    /// @needwork = dda
    /// @endcode
    ara::core::String certSerialNumber{""};
    /// @brief certIssuer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00301
    /// @needwork = dda
    /// @endcode
    ara::core::String certIssuer{""};
    /// @brief repository
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00302
    /// @needwork = dda
    /// @endcode
    ara::core::String repository{""};
    /// @brief rolloutQualification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00303
    /// @needwork = dda
    /// @endcode
    AraList< RolloutQualificationStruct > rolloutQualification{};
    /// @brief ucm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00304
    /// @needwork = dda
    /// @endcode
    AraList< UcmStruct > ucmVec{};
    /// @brief ucmMasterFallback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00305
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > ucmMasterFallback{};
    /// @brief files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00306
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ara::core::String > files{};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @throw
    ///int32_t ManifestLoader(const isoft::manifestreader::ManifestNode& node);
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_NODE_H_