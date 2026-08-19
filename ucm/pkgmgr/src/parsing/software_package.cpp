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
/// @file       software_package.cpp
/// @brief      software package implementation
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
/// @unit_description=software package implementation
/// @endcode
///
/// ================================================================

#include "software_package.h"

#include <algorithm>

#include "ara/crypto/common/base_id_types.h"  //use too many crypto headers////////TODO////mytodo////????
#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/sig_encode_private_ctx.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/x509_provider.h"
#include "ara/ucm/internal/crypto/crypto.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/const.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "config/config.h"
#include "storage/filesystem_swcl_manager.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Initialize a SoftwarePackage.
///
/// @param manifest The parsed SoftwarePackage manifest
/// @param path The full file path to the extracted software package archive
/// @param swcl The software cluster contained in this software package
///
/// @uptrace={SWS_UCM_00122, 308c0ea02d50c0ae02ebb2b997bd194b73151073}
/// @throw no
SoftwarePackage::SoftwarePackage(SoftwarePackageManifest manifest, AraString path, SoftwareCluster swcl)  // NOLINT
    : kMmanifest{std::move(manifest)}
    , kPath{std::move(path)}  // Call the copy constructor of SoftwarePackageManifest
    , kSoftwareCluster{std::move(swcl)}
    , pProviderX509_{ara::crypto::LoadX509Provider()}  // Call the copy constructor of SoftwareCluster
    , pCryptoProvider_{
          ara::crypto::LoadCryptoProvider(std::move(ara::core::InstanceSpecifier(std::move(AraStringView("isoft")))))}
{
}

/// @brief IsValidDeltaPackage
/// @param deltaPackageApplicableVersion
/// @param basedVersion
/// @returns bool
/// @throws no
bool SoftwarePackage::IsValidDeltaPackage(AraString const& deltaPackageApplicableVersion,
                                          AraString const& basedVersion) noexcept
{
    const bool equal{deltaPackageApplicableVersion == basedVersion};
    return ((!deltaPackageApplicableVersion.empty()) && equal);
}

/// @brief IsValidDeltaPackage
/// @returns bool
/// @throws no
bool SoftwarePackage::IsValidDeltaPackage()
{
    return IsValidDeltaPackage(kMmanifest.deltaPackageApplicableVersion,
                               kSoftwareCluster.GetSwclManifest().version.ToString());
}

/// @brief IsInvalidDeltaPackage
/// @param deltaPackageApplicableVersion
/// @param basedVersion
/// @returns bool
/// @throws no
bool SoftwarePackage::IsInvalidDeltaPackage(AraString const& deltaPackageApplicableVersion,
                                            AraString const& basedVersion) noexcept
{
    const bool notEqual{deltaPackageApplicableVersion != basedVersion};
    return (!deltaPackageApplicableVersion.empty()) && notEqual;
}

/// @brief IsLikelyDeltaPackage
/// @param deltaPackageApplicableVersion
/// @returns bool
/// @throws no
bool SoftwarePackage::IsLikelyDeltaPackage(AraString const& deltaPackageApplicableVersion) noexcept
{
    return !deltaPackageApplicableVersion.empty();
}

/// @brief IsNotDeltaPackage
/// @param deltaPackageApplicableVersion
/// @returns bool
/// @throws no
bool SoftwarePackage::IsNotDeltaPackage(AraString const& deltaPackageApplicableVersion) noexcept
{
    return deltaPackageApplicableVersion.empty();
}

/// @brief IsOSPackage
/// @param swclName
/// @returns bool
/// @throws no
bool SoftwarePackage::IsOSPackage(AraString const& swclName) { return swclName == kOS_SWCL_NAME; }

/// @brief AuthenAndCheckConsistency
/// @returns result
/// @throws no
AraResultVoid SoftwarePackage::AuthenAndCheckConsistency()
{
    LOGI << "begin...";

    // Verify software package
    AraResultVoid ret;
    ret = Authen4Swpkg();
    if (!ret.HasValue()) {
        return ret;
    }
    ret = _checkConsistency4Swpkg();
    if (!ret.HasValue()) {
        return ret;
    }

    // Verify validity
    if (!CheckValidity()) {
        ReturnVoidErrcEnumWithLongLog("CheckValidity failed", kPackageInconsistent);
    }

    LOGI << "end";
    return {};
}

/// @brief Authen4Swpkg
/// @returns result
/// @throws no
AraResultVoid SoftwarePackage::Authen4Swpkg()
{
    // Whether to verify the software package
    if (Config::GetInstance()->GetDisbaleAuthn()) {
        LOGD << "disbale_authn is true, do nothing";
        return {};
    }

    // Software package manifest file
    AraString const swpkgMnfPath{kPath + kPathSeparator + kSwpkgManifestFileName};
    // Software package manifest signature file
    AraString const swpkgMnfSigPath{kPath + kPathSeparator + kSwpkgManifestSigFileName};
    LOGD << "swpkgManifestPath:" << swpkgMnfPath.c_str() << "swpkgManifestSigPath:" << swpkgMnfSigPath.c_str();

    /// Verify software package
    // Does the software package manifest signature file exist?
    if (!tinyfsys::DoesFileExist(swpkgMnfSigPath)) {
        ReturnVoidErrcEnumWithLongLog("can't find swpkgManifestSigPath:" << swpkgMnfSigPath.c_str(),
                                      kAuthenticationFailed);
    }

    // Get the certificate serial number
    if (kMmanifest.certSerialNumber.empty()) {  // Is the certificate serial number valid?
        ReturnVoidErrcEnumWithLongLog("certSerialNumber is empty", kInvalidPackageManifest);
    }

    // The certificate serial number is in hexadecimal, convert to Byte Vector
    AraVector< AraByte > const snByteVec{strutil::HexStrToBytes(kMmanifest.certSerialNumber)};

    // Get the certificate issuer
    if (kMmanifest.certIssuer.empty()) {  // Is the certificate serial number empty?
        ReturnVoidErrcEnumWithLongLog("issuer is empty", kInvalidPackageManifest);
    }

    // Construct the issuer
    AraResult< ara::crypto::x509::X509DN::Uptrc > const resIssuerDn{pProviderX509_->BuildDn(kMmanifest.certIssuer)};
    if (!resIssuerDn.HasValue()) {  // Is the certificate issuer valid?
        ReturnVoidErrcEnumWithLongLog("failed to BuildDn with issuer:" << kMmanifest.certIssuer.c_str(),
                                      kInvalidPackageManifest);
    }

    // Find the certificate based on the certificate serial number and issuer
    ara::crypto::x509::Certificate::Uptrc const pCertFind{pProviderX509_->FindCertBySn(
        ara::crypto::ReadOnlyMemRegion(static_cast< uint8_t const* >(static_cast< void const* >(snByteVec.data())),
                                       snByteVec.size()),
        *(resIssuerDn.Value()))};
    if (nullptr == pCertFind) {
        ReturnVoidErrcEnumWithLongLog(
            "failed to FindCertBySn with certSerialNumber:" << kMmanifest.certSerialNumber.c_str()
                                                            << "and issuer:" << kMmanifest.certIssuer.c_str(),
            kAuthenticationFailed);
    }

    // Export the certificate
    AraResult< std::vector< ara::core::internal::Byte > > const retExportPem{
        pCertFind->ExportPublicly(ara::crypto::Serializable::kFormatPemEncoded)};
    if (!retExportPem.HasValue()) {
        ReturnVoidErrcEnumWithLongLog("failed to ExportPublicly", kInvalidPackageManifest);
    }
    AraVector< uint8_t > packagerCertData;
    for (size_t i{0U}; i < retExportPem.Value().size(); i++) {
        packagerCertData.push_back(static_cast< uint8_t >(retExportPem.Value().at(i)));
    }

    // TODO: Verify the certificate chain

    // Use the certificate and the software package manifest signature file to verify: the software package manifest file
    if (!Crypto().CheckSignature(packagerCertData, swpkgMnfPath, swpkgMnfSigPath)) {
        ReturnVoidErrcEnumWithLongLog("failed to CheckSignature for swpkgManifestPath with swpkgManifestSigPath",
                                      kAuthenticationFailed);
    }

    LOGD << "end and success";
    return {};
}

/// @brief _checkConsistency4Swpkg
/// @returns result
/// @throws no
AraResultVoid SoftwarePackage::_checkConsistency4Swpkg() const
{
    AraString const& deltaVer{kMmanifest.deltaPackageApplicableVersion};
    AraString curVerStr;
    AraString newVerStr;
    if (IsLikelyDeltaPackage(deltaVer)) {
        SwClusterStateType swclState;
        std::ignore = FileSystemSWCLManager::GetSwclStatus(kMmanifest.shortName, swclState, curVerStr, newVerStr);
        std::ignore = swclState;
    }
    if (IsInvalidDeltaPackage(deltaVer, curVerStr)) {
        ReturnVoidErrcEnumWithLongLog("IsInvalidDeltaPackage", kInvalidPackageManifest);
    }

    // Get the paths of all files under the decompression path path_, kPath is ExtractionPath
    AraVectorString const files{tinyfsys::GetFilesRecursive(kPath)};

    // Get all files from swpkgManifest and check if they exist in the decompressed directory
    for (auto const& it : kMmanifest.files) {
        AraString const filePath{kPath + kPathSeparator + it.first};

        if (std::find(files.begin(), files.end(), filePath) == files.end()) {
            if (IsValidDeltaPackage(deltaVer, curVerStr)) {
                // not changed file, which recorded in manifest Files, but not in delta package
                LOGD << "delta package, it is valid when cannot find:" << it.first.c_str()
                     << " under:" << kPath.c_str();
                continue;
            }

            ReturnVoidErrcEnumWithLongLog(
                "full package, it is invalid when cannot find:" << it.first.c_str() << " under:" << kPath.c_str(),
                kInvalidPackageManifest);
        }

        // Calculate the hash value and check if it matches the verification information in the manifest
        AraString const hash{Crypto().GetFileHash(filePath)};
        if (!strutil::CaseCompare(hash, it.second)) {
            ReturnVoidErrcEnumWithLongLog("hash:" << hash.c_str() << " mismatch with it.second:" << it.second.c_str()
                                                  << " which in " << kSwpkgManifestFileName
                                                  << " for filePath:" << filePath.c_str(),
                                          kPackageInconsistent);
        }
    }

    // Check all files in path_ and check if they exist in the files field of SWPKG_MANIFEST.json
    for (AraString const& rfpath : tinyfsys::GetFilesRecursiveRelative(kPath)) {
        AraString const fname{tinyfsys::ExtractFileName(rfpath)};
        const bool iSSigFile{kSwpkgManifestSigFileName == fname};
        if ((kSwpkgManifestFileName == fname) || iSSigFile) {
            continue;
        }

        if (kMmanifest.files.find(rfpath) == kMmanifest.files.cend()) {
            ReturnVoidErrcEnumWithLongLog(rfpath.c_str() << " is not specified in swpkgManifest",
                                          kInvalidPackageManifest);
        }
    }

    return {};
}

/// @brief CheckConsistency4Swcl
/// @returns result
/// @throws no
AraResultVoid SoftwarePackage::CheckConsistency4Swcl() const
{
    SoftwareClusterManifest const& swclMnf{kSoftwareCluster.GetSwclManifest()};
    AraString const& installPath{GetPath().GetSwclInstallDir(swclMnf.shortName, swclMnf.version.ToString())};

    // get all real files from installPath
    AraVectorString const realFiles{tinyfsys::GetFilesRecursive(installPath)};

    // get files list from swclManifest, and check if they exist in the installation directory
    for (auto const& it : swclMnf.files) {
        AraString const filePath{installPath + kPathSeparator + it.first};

        if (std::find(realFiles.begin(), realFiles.end(), filePath) == realFiles.end()) {
            ReturnVoidErrcEnumWithLongLog("can not find:" << it.first.c_str() << " under:" << installPath.c_str(),
                                          kInvalidPackageManifest);
        }

        // Calculate the hash value and check if it matches the verification information in the manifest
        AraString const hash{Crypto().GetFileHash(filePath)};
        if (!strutil::CaseCompare(hash, it.second)) {
            ReturnVoidErrcEnumWithLongLog("hash:" << hash.c_str() << " mismatch with it.second:" << it.second.c_str()
                                                  << " which in " << kSwclManifestFileName
                                                  << " for filePath:" << filePath.c_str(),
                                          kPackageInconsistent);
        }
    }

    // swcl need not check that whether real path files are in swcl manifest file
    return {};
}

/// @brief CheckValidity
/// @returns bool
/// @throws no
bool SoftwarePackage::CheckValidity()
{
    ActionType const action{kMmanifest.actionType};
    LOGI << "begin with action:" << strtype::ActionTypeToStr(action).c_str();

    if (action == ActionType::kRemove) {
        LOGD << "ActionType is kRemove, do not need check whether exes and processes exist";
        return true;
    }

    if (!kMmanifest.deltaPackageApplicableVersion.empty()) {
        LOGD << "Now is delta package, do not need check whether exes and processes exist";
        return true;
    }

    // Check if the exe contained in the software set exists and if it exists in the decompressed directory
    SoftwareClusterManifest const& swclMnf{kSoftwareCluster.GetSwclManifest()};
    for (AraString const& exeFQN : swclMnf.containedExecutables) {
        // Only take the last part of the exe name
        AraString const exeName{strutil::GetLastComponent(exeFQN)};
        LOGD << "get exeName:" << exeName.c_str();

        // Check if the exe exists in the bin directory
        AraString const exePath{kPath + kPathSeparator + kBIN_DIR_NAME + kPathSeparator + exeName};
        if (!tinyfsys::DoesFileExist(exePath)) {
            LOGE << "can't find exePath:" << exePath.c_str();
            return false;
        }
    }

    // Check if the process contained in the software set exists and if it exists in the decompressed directory
    for (AraString const& processFQN : swclMnf.containedProcesses) {
        // Only take the last part of the process name
        AraString const processName{strutil::GetLastComponent(processFQN)};
        LOGD << "get processName:" << processName.c_str();

        // Check if the process exists in the etc directory
        AraString const processPath{kPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator + processName};
        if (!tinyfsys::DoesDirectoryExist(processPath)) {
            LOGE << "can't find processPath:" << processPath.c_str();
            return false;
        }
    }

    LOGI << "end with ok";
    return true;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
