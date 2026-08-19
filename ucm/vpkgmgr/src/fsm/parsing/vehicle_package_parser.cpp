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
/// @file       vehicle_package_parser.cpp
/// @brief      VehiclePackageParser implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehiclePackageParser
/// @unit_description=VehiclePackageParser impl
/// @endcode
///
/// ================================================================

#include "fsm/parsing/vehicle_package_parser.h"

#include "ara/ucm/internal/crypto/crypto.h"
#include "ara/ucm/internal/extraction/filesystem.h"
///#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include <memory>
#include <utility>

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/sig_encode_private_ctx.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/x509_provider.h"
#include "package_management_service/find_ucm_subs.h"
#include "utils/config/config.h"
#include "utils/helper.h"
#include "utils/ucmm_log.h"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define MANIFEST_CHECK_PARSER(ret)                                                                                     \
    if (isoft::kSuccess != (ret)) {                                                                                    \
        LOG_ERROR << __PRETTY_FUNCTION__;                                                                              \
        return (ret);                                                                                                  \
    }

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief alias ManifestNode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ManifestNode = isoft::manifestreader::ManifestNode;
/// @brief alias Manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using Manifest = isoft::manifestreader::Manifest;

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ara::crypto::Serializable;
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ara::crypto::x509::Certificate;
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ara::crypto::x509::X509DN;

/// @brief TransferStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t TransferStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    ara::core::String stor;
    int32_t ret{node.Load(std::move(ara::core::StringView(STORING)), stor)};
    // MANIFEST_CHECK_PARSER(ret)
    std::ignore = ret;

    bool const sucessful{isoft::manifestreader::tps::FromString< ara::core::String >(stor, this->storing)};
    if (!sucessful) {
        LOG_ERROR << "FromString error! storing=" << stor.c_str();
        // TODO(yunfei) Temporarily commented out, will enable after the generated model contains the Storing field
        ///return 1;
    }

    //convert vector to list just for QAC
    ara::core::Vector< ara::core::String > transferVec{};
    ret         = node.Load(std::move(ara::core::StringView(TRANSFER)), transferVec);
    std::ignore = std::copy(transferVec.begin(), transferVec.end(), std::back_inserter(this->transfer));
    MANIFEST_CHECK_PARSER(ret)

    return isoft::kSuccess;
}

/// @brief SoftwarePackageStepStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t SoftwarePackageStepStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    int32_t ret{node.Load(std::move(ara::core::StringView(SHORT_NAME)), this->shortName)};
    MANIFEST_CHECK_PARSER(ret)
    // ret = node.Load(std::move(ara::core::StringView(ACTIVATION_SWITCH)), this->activationSwitch);
    // MANIFEST_CHECK_PARSER(ret)
    ret = node.Load(std::move(ara::core::StringView(PROCESS)), this->process);
    if (ret != static_cast< int32_t >(isoft::manifestreader::ManifestReaderErrc::kKeyNotFound)) {
        MANIFEST_CHECK_PARSER(ret)
    }
    ret = node.Load(std::move(ara::core::StringView(TRANSFER)), this->transfer);
    MANIFEST_CHECK_PARSER(ret)
    return isoft::kSuccess;
}

/// @brief UcmProcessingStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t UcmProcessingStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    int32_t ret{node.Load(std::move(ara::core::StringView(SHORT_NAME)), this->shortName)};
    MANIFEST_CHECK_PARSER(ret)

    // convert vector to list just for QAC
    ara::core::Vector< SoftwarePackageStepStruct > softwarePackageStepVec{};
    ret = node.Load(std::move(ara::core::StringView(SOFTWARE_PACKAGE_STEPS)), softwarePackageStepVec);
    MANIFEST_CHECK_PARSER(ret)
    std::ignore = std::copy(softwarePackageStepVec.begin(), softwarePackageStepVec.end(),
                            std::back_inserter(this->softwarePackageStep));

    ret = node.Load(std::move(ara::core::StringView(UCM)), this->ucmStr);
    MANIFEST_CHECK_PARSER(ret)
    return isoft::kSuccess;
}

/// @brief RolloutQualificationStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t RolloutQualificationStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    int32_t ret{node.Load(std::move(ara::core::StringView(SHORT_NAME)), this->shortName)};
    MANIFEST_CHECK_PARSER(ret)
    LOG_DEBUG << "RolloutQualificationStruct::ManifestLoader, get shortName:" << shortName;

    ret = node.Load(std::move(ara::core::StringView(SAFETY_POLICY)), this->safetyPolicy);
    if (ret == static_cast< int32_t >(isoft::manifestreader::ManifestReaderErrc::kKeyNotFound)) {
        // SAFETY_POLICY not found
        ara::core::Vector< ara::core::String > safetyCondition;
        ret = node.Load(std::move(ara::core::StringView(SAFETY_CONDITION)), safetyCondition);
        MANIFEST_CHECK_PARSER(ret)
        // Use STL algorithms to convert the safety condition list into a safety policy string, multiple conditions separated by semicolons
        this->safetyPolicy
            = std::accumulate(std::next(safetyCondition.begin()), safetyCondition.end(), safetyCondition[0],
                              [](ara::core::String const& a, ara::core::String const& b) { return a + ";" + b; });
    } else {
        MANIFEST_CHECK_PARSER(ret)
    }

    // convert vector to list just for QAC
    ara::core::Vector< UcmProcessingStruct > ucmProcessingVec{};
    ret = node.Load(std::move(ara::core::StringView(UCM_STEPS)), ucmProcessingVec);
    MANIFEST_CHECK_PARSER(ret)
    std::ignore = std::copy(ucmProcessingVec.begin(), ucmProcessingVec.end(), std::back_inserter(this->ucmProcessing));

    return isoft::kSuccess;
}

/// @brief DriverNotificationStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t DriverNotificationStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    int32_t ret{node.Load(std::move(ara::core::StringView(APPROVAL_REQUIRED)), this->approvalRequired)};
    MANIFEST_CHECK_PARSER(ret)
    ret = node.Load(std::move(ara::core::StringView(NOTIFICATION_STATE)), this->notificationState);
    MANIFEST_CHECK_PARSER(ret)
    return isoft::kSuccess;
}

/// @brief UcmStruct ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t UcmStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    int32_t ret{node.Load(std::move(ara::core::StringView(SHORT_NAME)), this->shortName)};
    MANIFEST_CHECK_PARSER(ret)
    // ret = node.Load(std::move(ara::core::StringView(IDENTIFIER)), this->identifier);
    // MANIFEST_CHECK_PARSER(ret)
    // ret = node.Load(std::move(ara::core::StringView(UCM_MODULE_INSTANTIATION)), this->ucmModuleInstantiation);
    // MANIFEST_CHECK_PARSER(ret)
    ret = node.Load(std::move(ara::core::StringView(UCM_ID)), this->ucmId);
    MANIFEST_CHECK_PARSER(ret)
    return isoft::kSuccess;
}

/// @brief TransferStructXXXXXfestLoader
/// @return int
/// @throws no
VehiclePackageParser::VehiclePackageParser()
    : vehiclePackageInfo_{}
    , packagerSignature_{}
    , ucmIds_{}
    , driverNotifications_{}
    , rolloutStepInfos_{}
    , updateSwpkgs_{}
    , pProviderX509_{ara::crypto::LoadX509Provider()}
{
    ara::core::InstanceSpecifier const iSpecify(std::move(ara::core::StringView("isoft")));
    pCryptoProvider_ = ara::crypto::LoadCryptoProvider(iSpecify);
}

////////////////////// swps swcls package /////////////
/// @brief ActionTypeToEnum
/// @param actionType
/// @return Result
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00107
/// @trace_id_dd=DD_UCM_Master_00940
/// @needwork = dd
/// @endcode
static ara::core::Result< pkgmgr::ActionType, int32_t > ActionTypeToEnum(ara::core::StringView const& actionType)
{
    if (actionType == std::move(ara::core::StringView("install"))) {
        return ara::core::Result< pkgmgr::ActionType, int32_t >::FromValue(pkgmgr::ActionType::kInstall);
    }
    if (actionType == ara::core::StringView("remove")) {
        return ara::core::Result< pkgmgr::ActionType, int32_t >::FromValue(pkgmgr::ActionType::kRemove);
    }
    if (actionType == ara::core::StringView("update")) {
        return ara::core::Result< pkgmgr::ActionType, int32_t >::FromValue(pkgmgr::ActionType::kUpdate);
    }
    LOG_ERROR << "can't exector here!!!";
    return ara::core::Result< pkgmgr::ActionType, int32_t >::FromError(1);
}

// // Using XSD
/// pkgmgr::ActionType ActionTypeToEnum(isoft::manifestreader::tps::SoftwarePackageActionTypeEnum const actionType) {.
///     pkgmgr::ActionType ret{pkgmgr::ActionType::kUpdate};.
///     switch (actionType) {.
///         case isoft::manifestreader::tps::SoftwarePackageActionTypeEnum::kInstall:.
///             ret = pkgmgr::ActionType::kInstall;.
///             break;.
///         case isoft::manifestreader::tps::SoftwarePackageActionTypeEnum::kRemove:.
///             ret = pkgmgr::ActionType::kRemove;.
///             break;.
///         case isoft::manifestreader::tps::SoftwarePackageActionTypeEnum::kUpdate:.
///             ret = pkgmgr::ActionType::kUpdate;.
///             break;.
///         default:.
///             LOG_ERROR << "can't exector here!!!";.
///             break;.
///     }.
///     return ret;.
/// }.

/// @brief ActivateActionToBool
/// @param action
/// @return Result
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00107
/// @trace_id_dd=DD_UCM_Master_00941
/// @needwork = dd
/// @endcode
static ara::core::Result< bool, int32_t > ActivateActionToBool(ara::core::StringView const& action)
{
    if (std::move(ara::core::StringView("reboot")) == action) {
        return ara::core::Result< bool, int32_t >::FromValue(true);
    }
    if (std::move(ara::core::StringView("restartApplication")) == action) {
        return ara::core::Result< bool, int32_t >::FromValue(false);
    }
    if (std::move(ara::core::StringView("waitForReboot")) == action) {
        return ara::core::Result< bool, int32_t >::FromValue(false);
    }

    LOG_ERROR << "can't exector here!!!";
    return ara::core::Result< bool, int32_t >::FromError(1);
}

// Using XSD
/// bool ActivateActionToBool(isoft::manifestreader::tps::SoftwarePackageActivationActionEnum const&.
/// spkgActivationAction) {.
///     bool ret{false};.
///     switch (spkgActivationAction) {.
///         case isoft::manifestreader::tps::SoftwarePackageActivationActionEnum::kReboot:.
///             ret = true;.
///             break;.
///         case isoft::manifestreader::tps::SoftwarePackageActivationActionEnum::kRestartApplication:.
///             ret = false;.
///             break;.
///         case isoft::manifestreader::tps::SoftwarePackageActivationActionEnum::kWaitForReboot:.
///             ret = false;.
///             break;.
///         default:.
///             LOG_ERROR << "can't exector here!!!";.
///             ret = false;.
///     }.
///     return ret;.
/// }.

/// @brief Parse software package information and software cluster information in the vehicle package
/// @param swpsPath Path to the software package information json file
/// @param swclsPath Path to the software cluster information json file
/// @return Returns 0: success, non-zero: failure
/// @throws no
int32_t VehiclePackageParser::ParseSoftwarePackage(ara::core::String const& swpsPath,
                                                   ara::core::String const& swclsPath)
{
    // Parse swps to get software package information
    ara::core::Result< std::unique_ptr< Manifest > > swpsRes{
        isoft::manifestreader::OpenManifest(ara::core::StringView(swpsPath.c_str()))};
    if (!swpsRes.HasValue()) {
        LOG_ERROR << "Error reading software package manifest at " << swpsPath.c_str()
                  << " errmsg=" << swpsRes.Error().Message().data();
        return kVpkgError;
    }

    std::unique_ptr< Manifest > const swpsRootNode{std::move(swpsRes).Value()};
    bool isHaveError{false};
    ara::core::String const noneStr{"none"};  /// do not use NONE
    std::ignore = swpsRootNode->IterateArray(std::move(ara::core::StringView("")), [&, this](std::size_t const idx,
                                                                                             ManifestNode const& node) {
        SwPackageInfoPtr swPackagePtr{std::make_shared< SoftwarePackageInfo >()};
        swPackagePtr->shortName
            = node.GetValue< ara::core::String >(std::move(ara::core::StringView(SHORT_NAME)), noneStr);
        swPackagePtr->fqn = node.GetValue< ara::core::String >(std::move(ara::core::StringView(FQN)), noneStr);
        /// actionType
        ara::core::String const strActionType{
            node.GetValue< ara::core::String >(ara::core::StringView(ACTION_TYPE), noneStr)};
        ara::core::Result< pkgmgr::ActionType, int32_t > const result1{ActionTypeToEnum(strActionType)};
        if (!result1.HasValue()) {
            LOG_ERROR << "get actionType err, is=" << strActionType.c_str() << "please check";
            isHaveError = true;
            return;
        }
        swPackagePtr->actionType = result1.Value();
        // Using XSD
        // /// actionType
        /// ara::core::String strActionType = node.GetValue<ara::core::String>(ACTION_TYPE, noneStr);.
        /// isoft::manifestreader::tps::SoftwarePackageActionTypeEnum actionType;.
        /// bool successful = isoft::manifestreader::tps::FromString(strActionType, actionType);.
        /// if(!successful){.
        ///     LOG_ERROR << "get actionType of SoftwarePackage err, is=" << strActionType.c_str() << "please check";.
        ///     isHaveError = true;.
        ///     return;.
        /// }.
        /// swPackagePtr->actionType = ActionTypeToEnum(actionType);.

        ara::core::String const action{
            node.GetValue< ara::core::String >(ara::core::StringView(ACTIVATION_ACTION_TYPE), noneStr)};
        ara::core::Result< bool, int32_t > const result{ActivateActionToBool(action)};
        if (false == result.HasValue()) {
            LOG_ERROR << "get activationAction of SoftwarePackage err, is=" << action.c_str() << "please check";
            isHaveError = true;
            return;
        }
        swPackagePtr->isReboot = result.Value();

        // /// Using XSD
        // /// activationAction
        /// ara::core::String action = node.GetValue<ara::core::String>(ACTIVATION_ACTION_TYPE, noneStr);.
        /// isoft::manifestreader::tps::SoftwarePackageActivationActionEnum spkgActivationAction;.
        /// bool successful = isoft::manifestreader::tps::FromString(action, spkgActivationAction);.
        /// if (!successful) {.
        ///     LOG_ERROR << "get activationAction of SoftwarePackage err, is=" << action.c_str() << "please check";.
        ///     isHaveError = true;.
        ///     return;.
        /// }.
        /// swPackagePtr->isReboot = ActivateActionToBool(spkgActivationAction);.

        /// compressedSoftwarePackageSize
        swPackagePtr->compressedSize
            = node.GetValue< uint32_t >(std::move(ara::core::StringView(COMPRESSED_SOFTWARE_PACKAGE_SIZE)), 0U);
        if (0U == swPackagePtr->compressedSize) {
            LOG_ERROR << "compressedSoftwarePackageSize must isn't 0! idex=" << idx;
            isHaveError = true;
            return;
        }
        /// swPackagePtr->uncompressedSoftwarePackageSize
        ///   = node.GetValue<uint32_t>(UNCOMPRESSED_SOFTWARE_PACKAGE_SIZE,0U);.
        /// swPackagePtr->packagerId = node.GetValue<int32_t>(PACKAGER_ID, -1);.
        /// swPackagePtr->packagerSignature = node.GetValue<ara::core::String>(PACKAGER_SIGNATURE, noneStr);.
        swPackagePtr->softwareCluster
            = node.GetValue< ara::core::String >(std::move(ara::core::StringView(SOFTWARE_CLUSTER)), noneStr);
        /// swPackagePtr->minUCMSupportedVersion
        ///    = node.GetValue<ara::core::String>(MIN_UCM_SUPPORTED_VERSION,"0.0.0-0");.
        /// swPackagePtr->maxUCMSupportedVersion
        ///    = node.GetValue<ara::core::String>(MAX_UCM_SUPPORTED_VERSION, "0.0.0-0");.
        swPackagePtr->version = "0.0.0-0";  // Temporarily set to default value, will get version number from swcl later

        updateSwpkgs_.push_back(swPackagePtr);
    });

    if (isHaveError) {
        return kVpkgError;
    }

    // Parse swcls to get software package version number
    ara::core::Result< std::unique_ptr< Manifest > > swclsRes{
        isoft::manifestreader::OpenManifest(ara::core::StringView(swclsPath.c_str()))};
    if (!swclsRes.HasValue()) {
        LOG_ERROR << "Error reading software cluster manifest at " << swclsPath.c_str()
                  << " errmsg=" << swclsRes.Error().Message().data();
        return kVpkgError;
    }
    std::unique_ptr< Manifest > const swclsRootNode{std::move(swclsRes).Value()};
    isHaveError = false;
    std::ignore = swclsRootNode->IterateArray(
        std::move(ara::core::StringView("")), [&, this](std::size_t const idx, ManifestNode const& node) {
            std::ignore = idx;
            ara::core::String const fqn{
                node.GetValue< ara::core::String >(std::move(ara::core::StringView(FQN)), noneStr)};
            ara::core::String const version{
                node.GetValue< ara::core::String >(std::move(ara::core::StringView(VERSION)), "0.0.0-0")};
            ara::core::String const typeApproval{
                node.GetValue< ara::core::String >(std::move(ara::core::StringView(TYPE_APPROVAL)), noneStr)};
            ara::core::String const license{
                node.GetValue< ara::core::String >(std::move(ara::core::StringView(LICENSE)), noneStr)};
            ara::core::String const releaseNotes{
                node.GetValue< ara::core::String >(std::move(ara::core::StringView(RELEASE_NOTES)), noneStr)};

            if (version == "0.0.0-0") {
                LOG_ERROR << "Error reading software package manifest at " << swpsPath.c_str()
                          << " errmsg=" << swpsRes.Error().Message().data();
                isHaveError = true;
                return;
            }
            for (SwPackageInfoPtr& it : updateSwpkgs_) {
                if (fqn == it->softwareCluster) {
                    it->version      = version;
                    it->typeApproval = typeApproval;
                    it->license      = license;
                    it->releaseNotes = releaseNotes;
                    break;
                }
            }
        });

    if (isHaveError) {
        return kVpkgError;
    }
    return kVpkgSuccess;
}

/// @brief ParseSwpStep
/// @param swpInfoPtr
/// @param swpStepNode
/// @param ucmInfoPtr
/// @return int
/// @throws no
int32_t VehiclePackageParser::ParseSwpStep(std::shared_ptr< SwpStepInfo > const& swpInfoPtr,
                                           SoftwarePackageStepStruct const& swpStepNode,
                                           std::shared_ptr< UcmStepInfo > const& ucmInfoPtr)
{
    swpInfoPtr->shortName = swpStepNode.shortName;
    swpInfoPtr->ucmId     = ucmInfoPtr->ucmId;

    for (TransferStruct const& itTransfer : swpStepNode.transfer) {
        // TransferInfo
        if (false == itTransfer.transfer.empty()) {
            auto transferInfoPtr     = std::make_shared< TransferInfo >();
            transferInfoPtr->storing = itTransfer.storing;
            for (ara::core::String const& it : itTransfer.transfer) {
                TransferStepInfoPtr transferStepPtr{std::make_shared< TransferStepInfo >()};
                transferStepPtr->softwarePackageRef = it;
                transferInfoPtr->transferInfos.push_back(transferStepPtr);
            }

            swpInfoPtr->transferStepPtrList.push_back(transferInfoPtr);
        }
    }

    // process
    if (!swpStepNode.process.empty()) {
        swpInfoPtr->processStepPtr                     = std::make_shared< ProcessStepInfo >();
        swpInfoPtr->processStepPtr->softwarePackageRef = swpStepNode.process;
    }

    // activate
    swpInfoPtr->activateStep = swpStepNode.activationSwitch;
    return kVpkgSuccess;
}

/// @brief _parseUcmStep
/// @param ucmInfoPtr
/// @param ucmStepNode
/// @param safetyPolicy
/// @return int
/// @throws no
int32_t VehiclePackageParser::_parseUcmStep(std::shared_ptr< UcmStepInfo > const& ucmInfoPtr,
                                            UcmProcessingStruct const& ucmStepNode,
                                            ara::core::String const& safetyPolicy)
{
    std::ignore           = safetyPolicy;
    ucmInfoPtr->shortName = ucmStepNode.shortName;
    bool isHave{false};
    for (UcmStruct const& it : vehiclePackageInfo_.ucmVec) {
        if (it.shortName == ucmStepNode.ucmStr) {
            ucmInfoPtr->ucmId = it.ucmId;
            isHave            = true;
            break;
        }
    }
    if (!isHave) {
        LOG_ERROR << "_parseUcmStep can't find ucmId by shortName=" << ucmStepNode.ucmStr.c_str();
        return kVpkgError;
    }

    for (SoftwarePackageStepStruct const& it : ucmStepNode.softwarePackageStep) {
        SwpStepInfoPtr const swpStepPtr{std::make_shared< SwpStepInfo >()};
        int32_t const ret{ParseSwpStep(swpStepPtr, it, ucmInfoPtr)};
        if (ret != 0) {
            return kVpkgError;
        }
        ucmInfoPtr->swpInfos.push_back(swpStepPtr);
    }
    return kVpkgSuccess;
}

/// @brief _parseRolloutStep
/// @param rolloutInfoPtr
/// @param rolloutStepNode
/// @return int
/// @throws no
int32_t VehiclePackageParser::_parseRolloutStep(std::shared_ptr< RolloutStepInfo > const& rolloutInfoPtr,
                                                RolloutQualificationStruct const& rolloutStepNode)
{
    // Step name
    rolloutInfoPtr->shortName    = rolloutStepNode.shortName;
    rolloutInfoPtr->safetyPolicy = rolloutStepNode.safetyPolicy;

    //  Get UCM Step
    for (UcmProcessingStruct const& it : rolloutStepNode.ucmProcessing) {
        UcmStepInfoPtr const ucmInfoPtr{std::make_shared< UcmStepInfo >()};
        if (0 != this->_parseUcmStep(ucmInfoPtr, it, rolloutInfoPtr->safetyPolicy)) {
            return kVpkgError;
        }
        rolloutInfoPtr->ucmInfos.push_back(ucmInfoPtr);
    }
    return kVpkgSuccess;
}

/// @brief VehiclePackageParser::FilesStruct::ParseFileList
/// @param node
/// @param key
/// @param files
/// @throws no
void VehiclePackageParser::FilesStruct::ParseFileList(
    ManifestNode const& node,
    ara::core::StringView const& key,
    ara::core::Map< ara::core::String, ara::core::String >& files) const noexcept
{
    ara::core::String const value{
        node.GetValue< ara::core::String >(std::move(ara::core::StringView("")), ara::core::String(""))};
    if (value.empty()) {
        ara::core::String nextKey{key};
        if (!nextKey.empty()) {
            nextKey += "/";
        }
        std::ignore = node.IterateObject(
            std::move(ara::core::StringView("")),
            [this, &nextKey, &files](ara::core::StringView const& memberName, ManifestNode const& subNode) {
                ParseFileList(subNode, nextKey + memberName.data(), files);
            });
    } else {
        files[key.data()] = value;
    }
}
/// @brief VehiclePackageParser::FilesStruct::ManifestLoader
/// @param node
/// @return int
/// @throws no
int32_t VehiclePackageParser::FilesStruct::ManifestLoader(isoft::manifestreader::ManifestNode const& node)
{
    ParseFileList(node, "", mapFiles);
    return 0;
}

/// @brief Parse vehicle package orchestration steps
/// @param vpsPath Path to the vehicle package orchestration steps json file
/// @return Returns 0: success, non-zero: failure
/// @throws no
int32_t VehiclePackageParser::ParseVehiclePackage(ara::core::String const& vpsPath)
{
    // Start parsing
    ara::core::Result< std::unique_ptr< Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(std::move(ara::core::StringView(vpsPath.c_str())))};
    if (!manifestRes.HasValue()) {
        LOG_ERROR << "Error reading vps manifest at " << vpsPath.c_str()
                  << " errmsg=" << manifestRes.Error().Message().data();
        return kVpkgError;
    }

    std::unique_ptr< Manifest > const manifest{std::move(manifestRes).Value()};
    int32_t ret{manifest->Load(std::move(ara::core::StringView(SHORT_NAME)), vehiclePackageInfo_.shortName)};
    MANIFEST_CHECK_PARSER(ret)

    LOG_INFO << "begin driverNotification";
    // convert vector to list just for QAC
    ara::core::Vector< DriverNotificationStruct > driverNotificationVec{};
    ret = manifest->Load(std::move(ara::core::StringView(DRIVER_NOTIFICATIONS)), driverNotificationVec);
    MANIFEST_CHECK_PARSER(ret)
    std::ignore = std::copy(driverNotificationVec.begin(), driverNotificationVec.end(),
                            std::back_inserter(vehiclePackageInfo_.driverNotification));

    LOG_INFO << "begin packagerSignature";
    ret = manifest->Load(std::move(ara::core::StringView(PACKAGER_SIGNATURE)), vehiclePackageInfo_.packagerSignature);

    if ((isoft::kSuccess != ret)
        && (static_cast< int >(isoft::manifestreader::ManifestReaderErrc::kKeyNotFound)
            != ret)) {  // No packagerSignature field
        LOG_ERROR << "failed to parse packagerSignature.";
        return kVpkgError;
    }

    LOG_INFO << "begin rolloutQualification";
    // convert vector to list just for QAC
    ara::core::Vector< RolloutQualificationStruct > vecRolloutQualificationStructVec{};
    ret = manifest->Load(std::move(ara::core::StringView(VEHICLE_ROLLOUT_STEPS)), vecRolloutQualificationStructVec);
    MANIFEST_CHECK_PARSER(ret)
    std::ignore = std::copy(vecRolloutQualificationStructVec.begin(), vecRolloutQualificationStructVec.end(),
                            std::back_inserter(vehiclePackageInfo_.rolloutQualification));

    LOG_INFO << "begin ucm";
    // convert vector to list just for QAC
    ara::core::Vector< UcmStruct > ucmVec{};
    ret = manifest->Load(std::move(ara::core::StringView(UCM)), ucmVec);
    MANIFEST_CHECK_PARSER(ret)
    std::ignore = std::copy(ucmVec.begin(), ucmVec.end(), std::back_inserter(vehiclePackageInfo_.ucmVec));

    LOG_INFO << "begin ucmMasterFallback";
    ret = manifest->Load(std::move(ara::core::StringView(UCM_MASTER_FALLBACK)), vehiclePackageInfo_.ucmMasterFallback);
    MANIFEST_CHECK_PARSER(ret)

    LOG_INFO << "begin certSerialNumber";
    ret = manifest->Load(std::move(ara::core::StringView(CERTIFICATE_SERIAL_NUMBER)),
                         vehiclePackageInfo_.certSerialNumber);

    if ((isoft::kSuccess != ret)
        && (static_cast< int >(isoft::manifestreader::ManifestReaderErrc::kKeyNotFound)
            != ret)) {  // No certSerialNumber field
        LOG_ERROR << "failed to parse certSerialNumber.";
        return kVpkgError;
    }

    LOG_INFO << "begin certIssuer";
    ret = manifest->Load(std::move(ara::core::StringView(CERTIFICATE_ISSUER)), vehiclePackageInfo_.certIssuer);
    if ((isoft::kSuccess != ret)
        && (static_cast< int >(isoft::manifestreader::ManifestReaderErrc::kKeyNotFound)
            != ret)) {  // No certIssuer field
        LOG_ERROR << "failed to parse certIssuer.";
        return kVpkgError;
    }

    LOG_DEBUG << "begin repository";
    ret = manifest->Load(std::move(ara::core::StringView(REPOSITORY)), vehiclePackageInfo_.repository);
    MANIFEST_CHECK_PARSER(ret)

    // Parse files field
    FilesStruct files;
    ret                       = manifest->Load(std::move(ara::core::StringView("files")), files);
    std::ignore               = ret;
    vehiclePackageInfo_.files = std::move(files.mapFiles);

    // Get driver approval rule
    for (DriverNotificationStruct const& it : vehiclePackageInfo_.driverNotification) {
        std::pair< ara::core::Map< ara::core::String, bool >::iterator, bool > const pair{
            driverNotifications_.emplace(it.notificationState, it.approvalRequired)};
        if (!pair.second) {
            LOG_ERROR << "driverNotifications_ insert err: notification=" << it.notificationState.c_str();
            return kVpkgError;
        }
        LOG_VERBOSE << "vehiclePackages driverNotifications: " << it.notificationState.c_str()
                    << BoolToStr(it.approvalRequired).data();
    }

    // Get vehicle package verification certificate
    packagerSignature_ = vehiclePackageInfo_.packagerSignature;

    // Get vehicle package main steps
    for (RolloutQualificationStruct const& it : vehiclePackageInfo_.rolloutQualification) {
        RolloutStepInfoPtr const rolloutInfoPtr{std::make_shared< RolloutStepInfo >()};
        if (0 != this->_parseRolloutStep(rolloutInfoPtr, it)) {
            return kVpkgError;
        }
        rolloutStepInfos_.push_back(rolloutInfoPtr);
    }

    // ucmId
    for (UcmStruct const& it : vehiclePackageInfo_.ucmVec) {
        ucmIds_.emplace_back(it.ucmId);
    }
    return kVpkgSuccess;
}

// check vehicle package, to ensure it effectiveness

/// @brief Verify the legality of the vehicle package
/// @param vpDir
/// @return Returns 0: success, non-zero: failure
/// @throws no
int32_t VehiclePackageParser::CheckValidity(ara::core::String const& vpDir)
{
    // TODO(yunfei) ManifestAuthentication(Signature) verification
    if (!_authenAndCheckConsis4Vpkg(vpDir).HasValue()) {
        log_.LogError() << "VehiclePackageParser::CheckValidity, failed to _authenAndCheckConsis4Vpkg.";
        return 1;
    }

    // TODO(yunfei) Check the version legality of software packages

    // TODO(yunfei) Compare swClusterInfos_ with version info in software packages, temporarily not done

    return kVpkgSuccess;
}

/// @brief _authenAndCheckConsis4Vpkg
/// @param vpDir
/// @return result
/// @throws no
ara::core::Result< void > VehiclePackageParser::_authenAndCheckConsis4Vpkg(ara::core::String const& vpDir)
{
    log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, begin with vpDir:" << vpDir.c_str();
    ara::core::Result< void > ret{};

    pkgmgr::Crypto const cr;

    // Vehicle package manifest file
    core::String const vpkgManifestPath{vpDir + helper::kPathSeparator + kVpkgManifestFileName};
    log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, get vpkgManifestPath:"
                    << vpkgManifestPath.c_str();

    // Vehicle package manifest signature file
    core::String const vpkgManifestSigPath{vpDir + helper::kPathSeparator + kVpkgManifestSigFileName};
    log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, get vpkgManifestSigPath:"
                    << vpkgManifestSigPath.c_str();

    // Whether to verify software packages
    bool const disbaleAuthn{ara::ucm::vpkgmgr::Config::GetInstance()->GetDisbaleAuthn()};
    log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, get disbale_authn:" << disbaleAuthn;

    // Verify software packages
    if (!disbaleAuthn) {
        // Check if software package manifest signature file exists
        if (pkgmgr::Filesystem::DoesFileExist(vpkgManifestSigPath)) {
            // Get certificate serial number
            core::String const certSerialNumber = vehiclePackageInfo_.certSerialNumber;
            log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, get certSerialNumber:"
                            << certSerialNumber;
            if (certSerialNumber.empty()) {  // Check if certificate serial number is valid
                log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, certSerialNumber is empty.";
                return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
            }

            // Certificate serial number is in hexadecimal, convert to Byte Vector
            ara::core::Vector< ara::core::Byte > snByteVec{helper::HexToBytes(certSerialNumber)};

            // Get certificate issuer
            core::String const issuer = vehiclePackageInfo_.certIssuer;
            log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, get issuer:" << issuer;
            if (issuer.empty()) {  // Check if issuer is empty
                log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, issuer is empty.";
                return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
            }

            // Construct issuer DN
            auto resIssuerDn = pProviderX509_->BuildDn(issuer);
            if (!(resIssuerDn.HasValue())) {  // Check if certificate issuer is valid
                log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, failed to BuildDn with issuer:"
                                << issuer;
                return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
            }
            X509DN::Uptrc const issuerDn = std::move(resIssuerDn).Value();

            // Find certificate by serial number and issuer
            log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, try to FindCertBySn.";
            Certificate::Uptrc const pCertFind = pProviderX509_->FindCertBySn(
                ara::crypto::ReadOnlyMemRegion(
                    static_cast< const uint8_t* >(static_cast< const void* >(snByteVec.data())), snByteVec.size()),
                *issuerDn);
            if (pCertFind == nullptr) {
                log_.LogError()
                    << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, failed to FindCertBySn with certSerialNumber:"
                    << certSerialNumber << "and issuer:" << issuer;
                return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
            }

            // Export certificate
            log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, try to ExportPublicly.";
            auto const resExportPem = pCertFind->ExportPublicly(Serializable::kFormatPemEncoded);
            if (!(resExportPem.HasValue())) {
                log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, failed to ExportPublicly.";
                return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
            }
            ara::core::Vector< uint8_t > packagerCertData{};
            for (size_t i = 0U; i < resExportPem.Value().size(); i++) {
                packagerCertData.push_back(static_cast< uint8_t >(resExportPem.Value().at(i)));
            }

            // TODO(Han Zhibo): Verify certificate chain

            // Use certificate and software package manifest signature file to verify: software package manifest file
            if (!cr.CheckSignature(packagerCertData, vpkgManifestPath, vpkgManifestSigPath)) {
                log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, failed to CheckSignature for "
                                   "vpkgManifestPath:"
                                << vpkgManifestPath << "vpkgManifestSigPath:" << vpkgManifestSigPath;
                ret = ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
            } else {
                log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, succeed to CheckSignature for "
                                   "vpkgManifestPath:"
                                << vpkgManifestPath << "vpkgManifestSigPath:" << vpkgManifestSigPath;
            }
        } else {
            // Software package manifest signature file does not exist
            log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, can't find vpkgManifestSigPath:"
                            << vpkgManifestSigPath;
            ret = ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
        }
    }
    // Check for errors
    if (!ret) {
        log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, return beacuse of error:"
                        << ret.Error().Message().data();
        return ret;
    }

    // Get paths of all files under vpDir
    pkgmgr::AraVectorString files{pkgmgr::Filesystem::GetFilesRecursive(vpDir)};

    // Get all files in ucm_vps.json and check if they exist
    for (auto const& iter : vehiclePackageInfo_.files) {
        ara::core::String const filePath{vpDir + helper::kPathSeparator + iter.first};
        log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, try to check filePath:"
                        << filePath.c_str() << "which is in" << kVpkgManifestFileName;

        if (std::find(files.begin(), files.end(), filePath) == files.end()) {
            log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, cannot find:" << iter.first.c_str()
                            << "under:" << vpDir.c_str();
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
        }

        // Calculate hash value and check if it matches the verification info in the manifest
        const ara::core::String hash = cr.GetFileHash(filePath);
        if (!helper::CompareStrings(hash, iter.second)) {
            log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, hash:" << hash
                            << "mismatch with iter.second:" << iter.second << "which in" << kVpkgManifestFileName
                            << "for filePath:" + filePath;
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kPackageInconsistent);
        }
    }

    // Check all files in vpDir and verify they exist in the files field of ucm_vps.json
    for (ara::core::String const& filePath : pkgmgr::Filesystem::GetFilesRecursiveRelative(vpDir)) {
        ara::core::String const fileName{pkgmgr::Filesystem::ExtractFileName(filePath)};

        if (fileName == kVpkgManifestFileName) {
            continue;
        }
        if (fileName == kVpkgManifestSigFileName) {
            continue;
        }

        if (vehiclePackageInfo_.files.count(filePath) == 0U) {
            log_.LogError() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, filePath:" << filePath.c_str()
                            << " is not specified in the vpkgManifestPath:" << vpkgManifestPath.c_str();
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
        }
    }

    log_.LogDebug() << "VehiclePackageParser::_authenAndCheckConsis4Vpkg, end.";
    return {};
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
