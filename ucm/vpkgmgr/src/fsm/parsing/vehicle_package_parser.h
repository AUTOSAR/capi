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
/// @file       vehicle_package_parser.h
/// @brief      VehiclePackageParser header
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
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00039
/// @unit_name=VehiclePackageParser
/// @unit_description=VehiclePackageParser header
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_PARSING_VEHICLE_PACKAGE_PARSER_H_
#define VPKGMGR_SRC_PARSING_VEHICLE_PACKAGE_PARSER_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <ara/crypto/cryp/crypto_provider.h>
#include <ara/crypto/x509/certificate.h>
#include <ara/crypto/x509/x509_provider.h>
#include <ara/log/logger.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/manifestreader_error_domain.h>

#include <memory>
#include <utility>

#include "consts.h"
#include "fsm/campaign_steps/rollout_step.h"
#include "fsm/parsing/software_package_info.h"
#include "fsm/parsing/step_info.h"
#include "fsm/parsing/vehicle_package_parse_node.h"
#include "utils/alias.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Parse vehicle package
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00100
/// @trace_id_dd=DD_UCM_Master_00318
/// @needwork = ad
/// @endcode
class VehiclePackageParser final
{
public:
    /// @brief Constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00101
    /// @trace_id_dd=DD_UCM_Master_00319
    /// @needwork = ad
    /// @endcode
    VehiclePackageParser();
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00102
    /// @trace_id_dd=DD_UCM_Master_00320
    /// @needwork = ad
    /// @endcode
    ~VehiclePackageParser() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00103
    /// @trace_id_dd=DD_UCM_Master_00321
    /// @needwork = ad
    /// @endcode
    VehiclePackageParser(VehiclePackageParser const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00104
    /// @trace_id_dd=DD_UCM_Master_00322
    /// @needwork = ad
    /// @endcode
    VehiclePackageParser(VehiclePackageParser&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00105
    /// @trace_id_dd=DD_UCM_Master_00323
    /// @needwork = ad
    /// @endcode
    VehiclePackageParser& operator=(VehiclePackageParser const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00106
    /// @trace_id_dd=DD_UCM_Master_00324
    /// @needwork = ad
    /// @endcode
    VehiclePackageParser& operator=(VehiclePackageParser&& other) = delete;

    /// @brief Parse software package information and software cluster information in the vehicle package
    /// @param swpsPath Path to the software package information json file
    /// @param swclsPath Path to the software cluster information json file
    /// @return Returns 0: success, non-zero: failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00107
    /// @trace_id_dd=DD_UCM_Master_00325
    /// @needwork = ad
    /// @endcode
    int32_t ParseSoftwarePackage(ara::core::String const& swpsPath, ara::core::String const& swclsPath);

    /// @brief Parse vehicle package orchestration steps
    /// @param vpsPath Path to the vehicle package orchestration steps json file
    /// @return Returns 0: success, non-zero: failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00108
    /// @trace_id_dd=DD_UCM_Master_00326
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00039
    /// @needwork = ad
    /// @endcode
    int32_t ParseVehiclePackage(ara::core::String const& vpsPath);

    /// @brief Verify the legality of the vehicle package
    /// @param vpDir Vehicle package directory
    /// @return Returns 0: success, non-zero: failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00109
    /// @trace_id_dd=DD_UCM_Master_00327
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00039
    /// @needwork = ad
    /// @endcode
    int32_t CheckValidity(ara::core::String const& vpDir);

    /// @brief Get driver approval information orchestrated in the vehicle package
    /// @return Returns driver approval information orchestrated in the vehicle package
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00110
    /// @trace_id_dd=DD_UCM_Master_00328
    /// @needwork = ad
    /// @endcode
    ara::core::Map< ara::core::String, bool > GetDriverNotification() const { return driverNotifications_; }

    /// @brief Get installation/upgrade step operation
    /// @return Returns installation/upgrade step
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00111
    /// @trace_id_dd=DD_UCM_Master_00329
    /// @needwork = ad
    /// @endcode
    ara::core::Vector< RolloutStepInfoPtr >&& GetRolloutStepInfs() const noexcept
    {
        return std::move(rolloutStepInfos_);
    }

    /// @brief Get ucm identifiers involved in the vehicle package
    /// @return Returns list of ucm identifiers
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00112
    /// @trace_id_dd=DD_UCM_Master_00330
    /// @needwork = ad
    /// @endcode
    AraList< ara::core::String > const& GetUcmIds() const noexcept { return ucmIds_; }

    /// @brief GetRepository
    /// @return str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00113
    /// @trace_id_dd=DD_UCM_Master_00331
    /// @needwork = ad
    /// @endcode
    core::String GetRepository() const { return vehiclePackageInfo_.repository; }

    /// @brief Get list of software packages to be updated
    /// @return Returns list of software packages to be updated
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00114
    /// @trace_id_dd=DD_UCM_Master_00332
    /// @needwork = ad
    /// @endcode
    AraList< SwPackageInfoPtr > const& GetUpdateSwpkgs() const noexcept { return updateSwpkgs_; }

private:
    /// @brief ManifestNode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using ManifestNode = isoft::manifestreader::ManifestNode;
    /// @brief ParseSwpStep
    /// @param swpInfoPtr
    /// @param swpStepNode
    /// @param ucmInfoPtr
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00333
    /// @needwork = dda
    /// @endcode
    static int32_t ParseSwpStep(std::shared_ptr< SwpStepInfo > const& swpInfoPtr,
                                SoftwarePackageStepStruct const& swpStepNode,
                                std::shared_ptr< UcmStepInfo > const& ucmInfoPtr);
    /// @brief _parseUcmStep
    /// @param ucmInfoPtr
    /// @param ucmStepNode
    /// @param safetyPolicy
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00334
    /// @needwork = dda
    /// @endcode
    int32_t _parseUcmStep(std::shared_ptr< UcmStepInfo > const& ucmInfoPtr,
                          UcmProcessingStruct const& ucmStepNode,
                          ara::core::String const& safetyPolicy);
    /// @brief _parseRolloutStep
    /// @param rolloutInfoPtr
    /// @param rolloutStepNode
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00335
    /// @needwork = dda
    /// @endcode
    int32_t _parseRolloutStep(std::shared_ptr< RolloutStepInfo > const& rolloutInfoPtr,
                              RolloutQualificationStruct const& rolloutStepNode);

    /// @brief _authenAndCheckConsis4Vpkg
    /// @param vpDir
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// Verify vehicle package and check consistency
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00336
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _authenAndCheckConsis4Vpkg(ara::core::String const& vpDir);

    /// @brief FilesStruct
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00115
    /// @trace_id_dd=DD_UCM_Master_00337
    /// @needwork = ad
    /// @endcode
    class FilesStruct
    {
    public:
        /// @brief files_
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00000
        /// @trace_id_dd=DD_UCM_Master_00338
        /// @needwork = dda
        /// @endcode
        ara::core::Map< ara::core::String, ara::core::String > mapFiles;

    public:
        /// @brief ParseFileList
        /// @param node
        /// @param key
        /// @param files
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00116
        /// @trace_id_dd=DD_UCM_Master_00339
        /// @needwork = ad
        /// @endcode
        void ParseFileList(ManifestNode const& node,
                           ara::core::StringView const& key,
                           ara::core::Map< ara::core::String, ara::core::String >& files) const noexcept;

        /// @brief ManifestLoader
        /// @param node
        /// @return int
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_Master_00117
        /// @trace_id_dd=DD_UCM_Master_00340
        /// @needwork = ad
        /// @endcode
        int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node);
    };

private:
    /// @brief Vehicle package information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00341
    /// @needwork = dda
    /// @endcode
    VehiclePackageStruct vehiclePackageInfo_;

    /// @brief Vehicle package signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00342
    /// @needwork = dda
    /// @endcode
    ara::core::String packagerSignature_;

    /// @brief IDs of all UCMs within the domain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00343
    /// @needwork = dda
    /// @endcode
    AraList< ara::core::String > ucmIds_;

    /// @brief Driver notification enable status for each execution state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00344
    /// @needwork = dda
    /// @endcode
    mutable ara::core::Map< ara::core::String, bool > driverNotifications_;

    /// @brief rolloutStep step execution body
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00345
    /// @needwork = dda
    /// @endcode
    mutable ara::core::Vector< RolloutStepInfoPtr > rolloutStepInfos_;

    /// @brief Software packages sorted by step //key: software package name, value: software package information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00346
    /// @needwork = dda
    /// @endcode
    AraList< SwPackageInfoPtr > updateSwpkgs_;

    /// @brief pProviderX509_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00347
    /// @needwork = dda
    /// @endcode
    ara::crypto::x509::X509Provider::Uptr pProviderX509_;
    /// @brief pCryptoProvider_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00348
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00349
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("VehiclePackageParser context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_PARSER_H_
