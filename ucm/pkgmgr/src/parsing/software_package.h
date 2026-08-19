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
/// @file       software_package.h
/// @brief      software package
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwarePackage
/// @unit_description=software package
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_H_

#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/x509_provider.h"
#include "common/alias.h"
#include "software_cluster.h"
#include "software_package_manifest.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief A software package is the unit of deployment for Updates.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_10343
/// @needwork = ad
/// @endcode
class SoftwarePackage
{
public:
    /// @brief Initialize a SoftwarePackage.
    ///
    /// @param manifest The parsed SoftwarePackage manifest
    /// @param path The full file path to the extracted software package archive
    /// @param swcl The software cluster contained in this software package
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00122, 308c0ea02d50c0ae02ebb2b997bd194b73151073}
    /// @throw no
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10344
    /// @needwork = dda
    /// @endcode
    SoftwarePackage(SoftwarePackageManifest manifest, AraString path, SoftwareCluster swcl);
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10345
    /// @needwork = dda
    /// @endcode
    virtual ~SoftwarePackage() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10346
    /// @needwork = dda
    /// @endcode
    SoftwarePackage(SoftwarePackage const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10347
    /// @needwork = dda
    /// @endcode
    SoftwarePackage& operator=(SoftwarePackage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10348
    /// @needwork = dda
    /// @endcode
    SoftwarePackage(SoftwarePackage&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10349
    /// @needwork = dda
    /// @endcode
    SoftwarePackage& operator=(SoftwarePackage&& other) = delete;

    /// @brief Return the manifest that is part of this software package.
    ///
    /// @returns The manifest of this software package
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10350
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifest const& GetManifest() const noexcept { return kMmanifest; }

    /// @brief Return the absolute path to the extracted software package archive.
    ///
    /// @returns Absolute path to the location where this software package archive has been extracted to
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10351
    /// @needwork = dda
    /// @endcode
    AraString const& GetExtractionPath() const noexcept { return kPath; }

    /// @brief Return the SoftwareCluster
    /// @returns The software cluster of this software package
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10352
    /// @needwork = dda
    /// @endcode
    SoftwareCluster const& GetSoftwareCluster() const noexcept { return kSoftwareCluster; }

    /// @brief IsValidDeltaPackage
    /// @param deltaPackageApplicableVersion
    /// @param basedVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10353
    /// @needwork = dda
    /// @endcode
    static bool IsValidDeltaPackage(AraString const& deltaPackageApplicableVersion,
                                    AraString const& basedVersion) noexcept;
    /// @brief IsValidDeltaPackage
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10354
    /// @needwork = dda
    /// @endcode
    bool IsValidDeltaPackage();
    /// @brief IsInvalidDeltaPackage
    /// @param deltaPackageApplicableVersion
    /// @param basedVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10355
    /// @needwork = dda
    /// @endcode
    static bool IsInvalidDeltaPackage(AraString const& deltaPackageApplicableVersion,
                                      AraString const& basedVersion) noexcept;
    /// @brief IsLikelyDeltaPackage
    /// @param deltaPackageApplicableVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10356
    /// @needwork = dda
    /// @endcode
    static bool IsLikelyDeltaPackage(AraString const& deltaPackageApplicableVersion) noexcept;
    /// @brief IsNotDeltaPackage
    /// @param deltaPackageApplicableVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10357
    /// @needwork = dda
    /// @endcode
    static bool IsNotDeltaPackage(AraString const& deltaPackageApplicableVersion) noexcept;
    /// @brief IsOSPackage
    /// @param swclName
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10358
    /// @needwork = dda
    /// @endcode
    static bool IsOSPackage(AraString const& swclName);

    /// @brief AuthenAndCheckConsistency
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// Verify the package and check consistency
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10359
    /// @needwork = dda
    /// @endcode
    virtual AraResultVoid AuthenAndCheckConsistency();
    /// @brief CheckConsistency4Swcl
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10360
    /// @needwork = dda
    /// @endcode
    AraResultVoid CheckConsistency4Swcl() const;

private:
    /// @brief Authen4Swpkg
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// Verify the software package and check consistency
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10361
    /// @needwork = dda
    /// @endcode
    virtual AraResultVoid Authen4Swpkg();

    /// @brief _checkConsistency4Swpkg
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10362
    /// @needwork = dda
    /// @endcode
    AraResultVoid _checkConsistency4Swpkg() const;

    /// @brief CheckValidity
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// Check the validity of the package
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10363
    /// @needwork = dda
    /// @endcode
    virtual bool CheckValidity();

private:
    /// @brief The manifest of this software package.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10364
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifest const kMmanifest;
    /// @brief The abolsute path to the extracted software package.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10365
    /// @needwork = dda
    /// @endcode
    AraString const kPath;
    /// @brief The software cluster of this software package.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10366
    /// @needwork = dda
    /// @endcode
    SoftwareCluster const kSoftwareCluster;

    /// @brief crypto for check the software package.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10367
    /// @needwork = dda
    /// @endcode
    ara::crypto::x509::X509Provider::Uptr pProviderX509_;
    /// @brief crypto for check the software package.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10368
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_H_
