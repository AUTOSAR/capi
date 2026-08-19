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
/// @file       software_package_manager.cpp
/// @brief      SoftwarePackageManager implementation
/// @details
/// @date       2023-11-01
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=SoftwarePackageManager
/// @unit_description=SoftwarePackageManager implementation
/// @endcode
///
/// ================================================================

#include "fsm/package_transfer/software_package_manager.h"

#include <ara/ucm/internal/transfer/helper.h>

#include <algorithm>

#include "fsm/storage/history_database.h"
#include "services/concentrator.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Create
/// @param packageStorage
/// @param spkgInfos
/// @param blockSize
/// @param retrievedSwpkFQN2TransferIDMap
/// @return UPtr
/// @throw
SoftwarePackageManager::UPtr SoftwarePackageManager::Create(
    PackageStorage::Ptr const& packageStorage,
    AraList< SwPackageInfoPtr > const& spkgInfos,
    uint32_t const blockSize,
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType > retrievedSwpkFQN2TransferIDMap)
{
    /// @brief Enable_Make_Shared
    struct Enable_Make_Shared : public SoftwarePackageManager
    {
    };

    SoftwarePackageManager::UPtr self{std::make_unique< Enable_Make_Shared >()};
    self->packageStorage_ = packageStorage;
    for (AraList< SwPackageInfoPtr >::const_iterator iter{spkgInfos.begin()}; iter != spkgInfos.end(); iter++) {
        SwPackageInfoPtr const spkg{*iter};
        self->spkgInfos_.push_back((spkg));
        std::ignore = self->allSpkgInfoMap_.emplace(spkg->fqn, spkg);
    }
    self->blocksize_             = blockSize;
    self->swpkFQN2TransferIDMap_ = std::move(retrievedSwpkFQN2TransferIDMap);
    return self;
}

/// @brief GetTransferdSpkgByName
/// @param name
/// @return SwPackageInfoPtr
/// @throw
SwPackageInfoPtr const SoftwarePackageManager::GetTransferdSpkgByName(ara::core::String const& name) const
{
    std::map< pkgmgr::SwNameType, SwPackageInfoPtr >::const_iterator const pair{spkgTransferdMap_.find(name)};
    if (pair == spkgTransferdMap_.end()) {
        return {nullptr};
    }
    return pair->second;
}
/// @brief GetSpkgByName
/// @param spkgFqn
/// @return SwPackageInfoPtr
/// @throw
SwPackageInfoPtr const SoftwarePackageManager::GetSpkgByName(ara::core::String const& spkgFqn) const
{
    std::map< pkgmgr::SwNameType, SwPackageInfoPtr >::const_iterator const pair{allSpkgInfoMap_.find(spkgFqn)};
    if (pair == allSpkgInfoMap_.end()) {
        return {nullptr};
    }
    return pair->second;
}

/// @brief TransferNextSwpkg
/// @throw
void SoftwarePackageManager::TransferNextSwpkg()
{
    log_.LogDebug() << "SoftwarePackageManager::TransferNextSwpkg(), begin.";
    if ((false == spkgInfos_.empty()) && (transferingSpkgs_.size() <= kParallelTransferNum)
        && (curSpkgInfoList_.size() <= kParallelTransferNum)) {
        /// There is a scenario: the order of triggering ota to transfer software packages is as follows: A2 -> B2 -> A1 -> B1
        /// When the OTA side receives the A2 request and starts transferring A2, it triggers notification for B2 transfer. Since A2 transfer is very fast (A2 is a remove software package),
        /// When A2 TransferExit occurs, B2 has not yet started TransferStart, which then triggers notification for A1 transfer, causing B2 to be unable to transfer subsequently. So B2 needs to be cached.
        SwPackageInfoPtr const curSpkgInfo{spkgInfos_.front()};
        curSpkgInfoList_.push_back(curSpkgInfo);
        spkgInfos_.pop_front();
        /// Notify otaClient to transfer the next software package
        Concentrator* const concentrator{Concentrator::GetInstance()};
        concentrator->NotificationSwpkg(pkgmgr::SwNameVersionType{curSpkgInfo->fqn, curSpkgInfo->version});
    }
}
/// @brief TransferStart
/// @param spkgName
/// @param promise
/// @return result
/// @throw
int32_t SoftwarePackageManager::TransferStart(pkgmgr::SwNameType const& spkgName,
                                              ara::core::Promise< TransferStartOutput >&& promise)
{
    std::lock_guard< std::mutex > const lock{mtx_};
    LOG_INFO << "begin transfer software package=" << spkgName.c_str();

    std::list< SwPackageInfoPtr >::iterator const iter{
        std::find_if(curSpkgInfoList_.begin(), curSpkgInfoList_.end(),
                     [&spkgName](SwPackageInfoPtr const& node) noexcept { return node->fqn == spkgName; })};
    if (iter == curSpkgInfoList_.end()) {
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kUnexpectedPackage);
        ara::core::String fqns;
        for (SwPackageInfoPtr const& it : curSpkgInfoList_) {
            fqns += (it->fqn + " ");
        }
        LOG_ERROR << "the swName=" << spkgName.c_str()
                  << "is not need transfer! the  cached need swNames:" << fqns.c_str();
        return 1;
    }

    SwPackageInfoPtr const& swpkgInfo{*iter};
    ara::core::Result< pkgmgr::TransferIdType > const result{packageStorage_->TransferStart(swpkgInfo->compressedSize)};
    if (!result.HasValue()) {
        promise.SetError(result.Error());
        return 3;
    }
    pkgmgr::TransferIdType const packageId{result.Value()};
    LOG_INFO << "transfer software package=" << swpkgInfo->fqn.c_str()
             << "transferId=" << pkgmgr::helper::ToHexString(packageId).c_str();

    // Save mapping from software package fqn to transfer ID
    std::ignore = swpkFQN2TransferIDMap_.emplace(spkgName, packageId);

    swpkgInfo->transferId = packageId;
    std::ignore           = transferingSpkgs_.emplace(packageId, swpkgInfo);  // Move to the transferring queue
    std::ignore           = curSpkgInfoList_.erase(iter);                     // Remove from waiting transfer cache
    promise.set_value({packageId, blocksize_});

    TransferNextSwpkg();
    return 0;
}
/// @brief TransferData
/// @param id
/// @param data
/// @param blockCounter
/// @param promise
/// @return result
/// @throw
int32_t SoftwarePackageManager::TransferData(pkgmgr::TransferIdType const& id,
                                             pkgmgr::ByteVectorType const& data,
                                             std::uint64_t const& blockCounter,
                                             ara::core::Promise< void >&& promise)
{
    ara::core::Result< void > const result{packageStorage_->TransferData(id, data, blockCounter)};
    if (!result.HasValue()) {
        promise.SetError(result.Error());
        return 1;
    }
    std::map< pkgmgr::TransferIdType, SwPackageInfoPtr >::iterator const iter{transferingSpkgs_.find(id)};
    if (iter == transferingSpkgs_.end()) {
        LOG_ERROR << "can't come here, please check logical of code!!! transferId="
                  << pkgmgr::helper::ToHexString(id).c_str();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
        return 1;
    }
    SwPackageInfoPtr const& spkgInfo{iter->second};
    spkgInfo->consecutiveBlocksReceived = blockCounter;
    spkgInfo->consecutiveBytesReceived += data.size();
    ///std::map<pkgmgr::TransferIdType, SwPackageInfoPtr>::iterator pair{std::move(transferingSpkgs_.find(id))};
    promise.set_value();
    return 0;
}
/// @brief TransferExit
/// @param id
/// @param promise
/// @return exit result
/// @throw
int32_t SoftwarePackageManager::TransferExit(pkgmgr::TransferIdType const& id, ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "SoftwarePackageManager::TransferExit(), begin with id:"
                    << pkgmgr::helper::ToHexString(id).c_str();
    ara::core::Result< ara::core::String > const result{packageStorage_->TransferExit(id)};
    if (!result.HasValue()) {
        promise.SetError(result.Error());
        return 1;
    }
    /// Transfer the next software package
    std::lock_guard< std::mutex > const lock{mtx_};
    std::map< pkgmgr::TransferIdType, SwPackageInfoPtr >::iterator const iter{transferingSpkgs_.find(id)};
    if (iter == transferingSpkgs_.end()) {
        LOG_ERROR << "can't come here, please check logical of code!!! transferId="
                  << pkgmgr::helper::ToHexString(id).c_str();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
        return 1;
    }

    SwPackageInfoPtr const spkgInfo{iter->second};
    spkgInfo->temporaryStorePath = std::move(result).Value();
    spkgInfo->state              = pkgmgr::SwPackageStateType::kTransferred;
    std::ignore                  = transferingSpkgs_.erase(id);
    std::ignore                  = spkgTransferdMap_.emplace(spkgInfo->fqn, spkgInfo);

    promise.set_value();
    TransferNextSwpkg();
    return 0;
}
/// @brief DeleteTransfer
/// @param id
/// @param promise
/// @throw
void SoftwarePackageManager::DeleteTransfer(pkgmgr::TransferIdType const& id, ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "SoftwarePackageManager::DeleteTransfer(), begin with id:"
                    << pkgmgr::helper::ToHexString(id).c_str();
    promise.SetResult(packageStorage_->DeleteTransfer(id));
}
/// @brief ClearSWPKGs
/// @throw
void SoftwarePackageManager::ClearSWPKGs()
{
    log_.LogDebug() << "SoftwarePackageManager::ClearSWPKGs(), begin.";

    // Delete software package on ucmm
    for (ara::core::Map< ara::core::String, pkgmgr::TransferIdType >::iterator iter{swpkFQN2TransferIDMap_.begin()};
         iter != swpkFQN2TransferIDMap_.end(); iter++) {
        ara::core::String const& swpkFQN{iter->first};
        pkgmgr::TransferIdType const& transferID{iter->second};
        log_.LogDebug() << "SoftwarePackageManager::ClearSWPKGs(), get swpkFQN:" << swpkFQN.c_str()
                        << "TransferID:" << pkgmgr::helper::ToHexString(transferID).c_str();

        // Delete software package
        ara::core::Result< void > const result{packageStorage_->DeleteTransfer(transferID)};
        if (!result.HasValue()) {
            log_.LogError() << "SoftwarePackageManager::ClearSWPKGs(), failed to DeleteTransfer for transferID:"
                            << pkgmgr::helper::ToHexString(transferID).c_str()
                            << "with error:" << result.Error().Message().data();
        }
    }

    // Delete software packages on the involved ucms
    for (auto const& pair : FindUcmSubs::GetInstance()->GetAllPmApp()) {  // Get service proxy corresponding to ucmID
        PackageManagementAppPtr const& ucmSubApp{pair.second};
        core::String ucmId{ucmSubApp->GetId()};
        log_.LogDebug() << "SoftwarePackageManager::ClearSWPKGs(), get ucmId:" << ucmId.c_str();

        // Get all software package ids transferred to ucm
        ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > vecTransferId{
            HistoryDatabase::GetInstance()->GetUCMTransferInfo(ucmId)};
        for (auto transferId : vecTransferId) {
            log_.LogDebug() << "SoftwarePackageManager::ClearSWPKGs(), get transferId:"
                            << pkgmgr::helper::ToHexString(transferId).c_str();
            std::ignore = ucmSubApp->DeleteSoftwarePackage(transferId);
        }

        // Clear the stored software package ids transferred to ucm
        HistoryDatabase::GetInstance()->ClearUCMTransferInfo(ucmId);
    }

    log_.LogDebug() << "SoftwarePackageManager::ClearSWPKGs(), end.";
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
