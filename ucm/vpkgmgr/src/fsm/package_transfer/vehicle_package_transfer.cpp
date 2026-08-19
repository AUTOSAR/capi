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
/// @file       vehicle_package_transfer.cpp
/// @brief      VehiclePackageTransfer implementation
/// @details
/// @date       2023-09-14
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehiclePackageTransfer
/// @unit_description=VehiclePackageTransfer implementation
/// @endcode
///
/// ================================================================

#include "fsm/package_transfer/vehicle_package_transfer.h"

#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include "ara/ucm/internal/extraction/filesystem.h"
#include "fsm/storage/history_database.h"
#include "fsm/storage/persistence.h"
#include "fsm/task_executor/task_queue.h"
#include "package_management_service/find_ucm_subs.h"
#include "services/concentrator.h"
#include "utils/ucmm_log.h"
#include "utils/utils.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetExtractionTargetDirectory
/// @param fullFilePath
/// @return str
/// @throw no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00155
/// @trace_id_dd=DD_UCM_Master_00936
/// @needwork = dd
/// @endcode
static ara::core::String GetExtractionTargetDirectory(core::String const& fullFilePath)
{
    ara::core::String filename{fullFilePath.substr(fullFilePath.find_last_of("/"))};
    ara::core::String const folder{fullFilePath.substr(0U, fullFilePath.size() - filename.size())};
    ara::core::String const extension{".zip"};
    filename = filename.substr(1U, filename.size() - extension.size() - 1U);
    ara::core::String targetFolder{folder + "/" + filename};
    LOG_DEBUG << "fullpath is" << fullFilePath.c_str() << "target is" << targetFolder.c_str();
    return targetFolder;
}

/// @brief Create
/// @param packageStorage
/// @param blockSize
/// @param vpkTransferId
/// @return UPtr
/// @throw
VehiclePackageTransfer::UPtr VehiclePackageTransfer::Create(PackageStorage::Ptr const& packageStorage,
                                                            uint32_t const blockSize,
                                                            pkgmgr::TransferIdType const vpkTransferId)
{
    LOG_INFO << "VehiclePackageTransfer::Create, begin with vpkTransferId:"
             << pkgmgr::helper::ToHexString(vpkTransferId).c_str();

    /// @brief Enable_Make_Shared
    struct Enable_Make_Shared : public VehiclePackageTransfer
    {
    };

    VehiclePackageTransfer::UPtr self{std::make_unique< Enable_Make_Shared >()};
    self->packageStorage_ = packageStorage;
    self->blocksize_      = blockSize;
    self->vpkTransferId_  = vpkTransferId;
    LOG_INFO << "end";
    return self;
}

/// @brief TransferVehiclePackage
/// @param size
/// @param promise
/// @return int
/// @throw
int32_t VehiclePackageTransfer::TransferVehiclePackage(uint64_t const size,
                                                       ara::core::Promise< TransferVehiclePackageOutput >&& promise)
{
    ara::core::Result< pkgmgr::TransferIdType > const result{packageStorage_->TransferStart(size)};
    if (!result.HasValue()) {
        log_.LogError() << "VehiclePackageTransfer::TransferVehiclePackage, failed to TransferStart with size:" << size
                        << ", so will TriggerCancel.";

        promise.SetError(result.Error());
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }
    vpkTransferId_ = result.Value();
    promise.set_value({vpkTransferId_, blocksize_});
    return 0;
}

/// @brief TransferData
/// @param id
/// @param data
/// @param blockCounter
/// @param promise
/// @return int
/// @throw
int32_t VehiclePackageTransfer::TransferData(pkgmgr::TransferIdType const& id,
                                             pkgmgr::ByteVectorType const& data,
                                             std::uint64_t const& blockCounter,
                                             ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "VehiclePackageTransfer::TransferData, begin with id:" << pkgmgr::helper::ToHexString(id).c_str()
                    << "vpkTransferId_:" << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
    if (id != vpkTransferId_) {
        log_.LogDebug() << "VehiclePackageTransfer::TransferData, InvalidTransferId id:"
                        << pkgmgr::helper::ToHexString(id).c_str()
                        << "becuase of vpkTransferId_:" << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
        return 1;
    }

    ara::core::Result< void > const result{packageStorage_->TransferData(vpkTransferId_, data, blockCounter)};
    if (!result.HasValue()) {
        log_.LogError() << "VehiclePackageTransfer::TransferData, failed to TransferData with vpkTransferId_:"
                        << pkgmgr::helper::ToHexString(vpkTransferId_).c_str() << ", so will TriggerCancel.";

        promise.SetError(result.Error());
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }

    promise.set_value();
    return 0;
}

/// @brief TransferExit
/// @param id
/// @param promise
/// @return int
/// @throw
int32_t VehiclePackageTransfer::TransferExit(pkgmgr::TransferIdType const& id, ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "VehiclePackageTransfer::TransferExit, begin with id:" << pkgmgr::helper::ToHexString(id).c_str()
                    << "vpkTransferId_:" << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
    if (id != vpkTransferId_) {
        log_.LogError() << "VehiclePackageTransfer::TransferExit, InvalidTransferId id:"
                        << pkgmgr::helper::ToHexString(id).c_str()
                        << "becuase of vpkTransferId_:" << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
        return 1;
    }

    ara::core::Result< ara::core::String > const result{packageStorage_->TransferExit(vpkTransferId_)};
    if (!result.HasValue()) {
        log_.LogError() << "VehiclePackageTransfer::TransferExit, failed to TransferExit vpkTransferId_:"
                        << pkgmgr::helper::ToHexString(vpkTransferId_).c_str() << ", so will TriggerCancel.";
        promise.SetError(result.Error());
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }

    FsmManager::GetInstance()->MarkProcessingVPStart();

    /// @brief Offload vehicle package parsing to a background thread
    class VkgParseWrap
    {
    public:
        ///virtual ~VkgParseWrap() { LOG_DEBUG << "~destruction~"; }

        /// @brief transferId
        pkgmgr::TransferIdType transferId{};
        /// @brief transferPromise
        ara::core::Promise< void > transferPromise{};
        /// @brief ptr
        VehiclePackageTransfer* ptr{nullptr};

        /// @brief Run
        /// @return int
        /// @throws no
        int32_t Run()
        {
            int32_t const ret{ptr->ProcessVehiclePackage(transferId, std::move(transferPromise))};
            return ret;
        }
    };

    std::shared_ptr< VkgParseWrap > executor{std::make_shared< VkgParseWrap >()};
    executor->transferId      = this->vpkTransferId_;
    executor->transferPromise = std::move(promise);
    executor->ptr             = this;

    TaskFuncType background{[executor]() -> int32_t { return executor->Run(); }};
    TaskQueue* const taskQueue{TaskQueue::GetInstance()};
    std::ignore = taskQueue->PushTask(std::move(background));
    return 0;
}

/// @brief Vehicle package transfer completed
/// @return bool
/// @throws no
bool VehiclePackageTransfer::IsVPKTransferred()
{
    log_.LogDebug() << "VehiclePackageTransfer::IsVPKTransferred, begin with vpkTransferId_:"
                    << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
    bool ret{false};
    ara::core::Result< ara::ucm::pkgmgr::SwPackageStateType > const res{
        packageStorage_->GetPackageStatus(vpkTransferId_)};
    if (res.HasValue()) {
        ret = (pkgmgr::SwPackageStateType::kTransferred == res.Value());
    } else {
        ret = false;
    }

    log_.LogDebug() << "VehiclePackageTransfer::IsVPKTransferred, end with ret:" << ret;
    return ret;
}

/// @brief Check if vehicle package zip file exists
/// @return bool
/// @throws no
bool VehiclePackageTransfer::IsVPKExist()
{
    log_.LogDebug() << "VehiclePackageTransfer::IsVPKExist, begin with vpkTransferId_:"
                    << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
    bool ret{false};
    ara::core::Result< ara::core::String > const res{packageStorage_->GetPackagePath(vpkTransferId_)};
    if (res) {
        ret = pkgmgr::Filesystem::DoesFileExist(res.Value());
    } else {
        ret = false;
    }

    log_.LogDebug() << "VehiclePackageTransfer::IsVPKExist, end with ret:" << ret;
    return ret;
}

/// @brief ClearVPK
/// @throws no
void VehiclePackageTransfer::ClearVPK()
{
    log_.LogDebug() << "VehiclePackageTransfer::ClearVPK, begin with vpkTransferId_:"
                    << pkgmgr::helper::ToHexString(vpkTransferId_).c_str();
    ara::core::Result< void > const result{packageStorage_->CleanVehiclePackage(vpkTransferId_)};
    if (!result.HasValue()) {
        log_.LogError() << "VehiclePackageTransfer::ClearVPK(), failed to CleanVehiclePackage for vpkTransferId_:"
                        << pkgmgr::helper::ToHexString(vpkTransferId_).c_str()
                        << "with error:" << result.Error().Message().data();
    }
    log_.LogDebug() << "VehiclePackageTransfer::ClearVPK, end.";
}

// UCMErrorDomainErrc::kMissingDependencies
// UCMErrorDomainErrc::kAuthenticationFailed
// UCMErrorDomainErrc::kOldVersion
// UCMErrorDomainErrc::IncompatiblePackageVersion
/// @brief ProcessVehiclePackage
/// @param id
/// @param promise
/// @param triggerSoftpkgTransfer
/// @return int
/// @throw
int32_t VehiclePackageTransfer::ProcessVehiclePackage(pkgmgr::TransferIdType const& id,
                                                      ara::core::Promise< void >&& promise,
                                                      bool const triggerSoftpkgTransfer)
{
    LOG_INFO << "begin process VehiclePackage"
             << "triggerSoftpkgTransfer:" << triggerSoftpkgTransfer;

    ara::core::Result< ara::core::String > result{packageStorage_->GetPackagePath(id)};
    if (!result.HasValue()) {
        log_.LogError() << "VehiclePackageTransfer::ProcessVehiclePackage, failed to GetPackagePath for id:"
                        << pkgmgr::helper::ToHexString(id).c_str() << ", so will TriggerCancel.";

        FsmManager::GetInstance()->MarkProcessingVPFinish();
        promise.SetError(result.Error());

        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }

    ara::core::String const packagePath{std::move(result).Value()};
    // FIXME(yunfei) Later modify to save vehicle package path in json

    ara::core::String const extractionDirectory{GetExtractionTargetDirectory(packagePath)};
    //  Extract the vehicle package if possible
    if (!packageStorage_->Extract(packagePath, extractionDirectory)) {
        log_.LogError() << "VehiclePackageTransfer::ProcessVehiclePackage, failed to Extract for id:"
                        << pkgmgr::helper::ToHexString(id).c_str() << "packagePath:" << packagePath.c_str()
                        << ", so will TriggerCancel.";

        FsmManager::GetInstance()->MarkProcessingVPFinish();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kPackageInconsistent);
        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }

    // Parse vehicle package manifest information
    ara::core::String const vpsPath{extractionDirectory + "/ucm_vps.json"};
    ara::core::String const swpsPath{extractionDirectory + "/ucm_swps.json"};
    ara::core::String const swclsPath{extractionDirectory + "/ucm_swcls.json"};
    if (0 != this->_parseVehiclePackageManifest(extractionDirectory, vpsPath, swpsPath, swclsPath)) {
        log_.LogError()
            << "VehiclePackageTransfer::ProcessVehiclePackage, failed to _parseVehiclePackageManifest for id:"
            << pkgmgr::helper::ToHexString(id).c_str() << "packagePath:" << packagePath.c_str()
            << ", so will TriggerCancel.";

        FsmManager::GetInstance()->MarkProcessingVPFinish();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kInvalidVehiclePackageManifest);
        return 1;
    }

    // Return value
    int32_t ret{0};

    /// Vehicle package parsing successful, whether to trigger software package transfer
    if (triggerSoftpkgTransfer) {
        bool driverNotified{false};
        for (auto const& iter : vpParser_.GetDriverNotification()) {
            if (iter.second) {
                driverNotified = true;
            }
        }
        std::ignore = FsmManager::GetInstance()->SaveDriverNotified(driverNotified);
        std::ignore = FsmManager::GetInstance()->SaveRepository(repository_);

        AraList< ara::core::String > const ucmIDs{GetUcmIds()};
        LOG_DEBUG << "VehiclePackageTransfer::ProcessVehiclePackage(), get ucmIDs:"
                  << Utils::ConcatenateStrings(ucmIDs).c_str();
        // Check if all involved ucms are online
        if (!FindUcmSubs::GetInstance()->UcmIsAllExsit(ucmIDs)) {
            FsmManager::GetInstance()->MarkProcessingVPFinish();
            promise.SetError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
            LOG_ERROR << "VehiclePackageTransfer::ProcessVehiclePackage(), can't find services for all ucmIDs:"
                      << Utils::ConcatenateStrings(ucmIDs).c_str() << ", so will TriggerIdle.";
            std::ignore = FsmManager::GetInstance()->SaveResolution(
                pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
            std::ignore
                = FsmManager::GetInstance()->TriggerIdle(pkgmgr::CampaignStateType::kVehiclePackageTransferring, true);

            ret = 1;
        } else {
            /// otaClient waits in the TransferExit function, return as soon as the vehicle package is validated
            promise.set_value();
            std::ignore = FsmManager::GetInstance()->TriggerSoftpkgTransfer();
            log_.LogDebug() << "VehiclePackageTransfer::ProcessVehiclePackage, end";
        }
    } else {
        /// otaClient waits in the TransferExit function, return as soon as the vehicle package is validated
        promise.set_value();
        log_.LogDebug() << "VehiclePackageTransfer::ProcessVehiclePackage, end beacause of triggerSoftpkgTransfer:"
                        << triggerSoftpkgTransfer;
        FsmManager::GetInstance()->MarkProcessingVPFinish();
    }

    return ret;
}

/// @brief _parseVehiclePackageManifest
/// @param vpDir
/// @param vpsPath
/// @param swpsPath
/// @param swclsPath
/// @return parse result
/// @throw
int32_t VehiclePackageTransfer::_parseVehiclePackageManifest(ara::core::String const& vpDir,
                                                             ara::core::String const& vpsPath,
                                                             ara::core::String const& swpsPath,
                                                             ara::core::String const& swclsPath)
{
    LOG_INFO << "begin parse SoftwarePackage manifest";
    if (0 != vpParser_.ParseSoftwarePackage(swpsPath, swclsPath)) {
        LOG_WARN << "Failed to parse the swps swcls manifest";
        return 1;
    }
    LOG_INFO << "end parse SoftwarePackage manifest";

    LOG_INFO << "begin parse VehiclePackage manifest";
    if (0 != vpParser_.ParseVehiclePackage(vpsPath)) {
        LOG_WARN << "Failed to parse the vehicle package manifest";
        return 2;
    }
    LOG_INFO << "end parse VehiclePackage manifest";

    // Check the vehicle package to ensure its validity
    if (0 != vpParser_.CheckValidity(vpDir)) {
        return 3;
    }

    /// Save ucms in the vehicle package
    ucmIds_ = vpParser_.GetUcmIds();
    log_.LogDebug() << "VehiclePackageTransfer::_parseVehiclePackageManifest, get ucmIds_:"
                    << Utils::ConcatenateStrings(ucmIds_).c_str();

    repository_ = vpParser_.GetRepository();
    log_.LogDebug() << "VehiclePackageTransfer::_parseVehiclePackageManifest, get repository_:" << repository_.c_str();

    _getCampaignInfoFromParser(vpParser_);
    return 0;
}

/// @brief _getCampaignInfoFromParser
/// @param vpParser
/// @throw
void VehiclePackageTransfer::_getCampaignInfoFromParser(VehiclePackageParser const& vpParser) const
{
    /// Driver operation notification information
    Concentrator* const concentrator{Concentrator::GetInstance()};
    concentrator->UpdateDriverNotifications(vpParser.GetDriverNotification());

    /// Software cluster information
    /// Must be before CreateRolloutStep, because swp_step_transfer.cpp uses GetSpkgByName
    FsmManager::GetInstance()->ReSetSwPackageInfos(vpParser.GetUpdateSwpkgs());

    // Execution step information
    ara::core::Vector< RolloutStepInfoPtr > rolloutStepInfos{std::move(vpParser.GetRolloutStepInfs())};
    ara::core::Vector< RolloutStep::Ptr > rolloutSteps;
    ara::core::Vector< ara::core::String > policyVec;
    for (const auto& rolloutInfo : rolloutStepInfos) {
        RolloutStep::Ptr rolloutStepPtr{std::make_shared< RolloutStep >()};
        std::ignore = rolloutStepPtr->Load(rolloutInfo);
        ara::core::String const policy{rolloutStepPtr->GetSafetyPolicy()};
        if (!policy.empty()) {
            policyVec.push_back(policy);
        }

        log_.LogDebug() << "VehiclePackageTransfer::_getCampaignInfoFromParser, get rolloutStepPtr->shortName:"
                        << rolloutStepPtr->GetShortName();

        rolloutSteps.push_back(rolloutStepPtr);
    }
    FsmManager::GetInstance()->UpdateRolloutSteps(std::move(rolloutSteps));

    ara::core::String const safetyCondition{GetCondition(policyVec)};
    concentrator->UpdateSafetyCondition(safetyCondition);
}

/// @brief GetCondition
/// @param policyVec
/// @return str
/// @throw
ara::core::String VehiclePackageTransfer::GetCondition(ara::core::Vector< ara::core::String > const& policyVec)
{
    if (policyVec.empty()) {
        return ara::core::String("none");
    }

    ara::core::String condition;

    std::size_t i{0U};
    for (; i < policyVec.size() - 1U; i++) {
        condition += (policyVec[i] + "#");
    }
    condition += policyVec[i];

    return condition;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
