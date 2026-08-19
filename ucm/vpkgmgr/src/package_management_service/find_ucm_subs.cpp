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
/// @file       find_ucm_subs.cpp
/// @brief      Mainly discovers UCM and manages UCM
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=FindUcmSubs
/// @unit_description=Discovers UCM and manages UCM
/// @endcode
///
/// ================================================================

#include "find_ucm_subs.h"

#include <ara/com/internal/runtime.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/optional.h>

#include <algorithm>
#include <set>
#include <utility>

#include "utils/ucmm_log.h"
#include "utils/utils.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief _findUcmHandler
/// @param newAvailableHandles
/// @param handler
/// @throws no
void FindUcmSubs::_findUcmHandler(
    ara::com::ServiceHandleContainer< pkgmgr::proxy::PackageManagementProxy::HandleType > newAvailableHandles,
    ara::com::FindServiceHandle const& handler)
{
    static_cast< void >(handler);

    log_.LogDebug() << "FindUcmSubs::_findUcmHandler(),"
                    << "get newAvailableHandles.size():" << newAvailableHandles.size();
    // handle array enters queue
    std::ignore = ucmHandlesQueue_.Push(std::move(newAvailableHandles));

    if (!isHaveFindUcmThread_) {
        isHaveFindUcmThread_ = true;
        findUcmThread_       = std::make_unique< std::thread >(&FindUcmSubs::_handleUcmServices, this);
    }
}
/// @brief HandleUcmServices
/// @throws no
void FindUcmSubs::_handleUcmServices()
{
    while (isHaveFindUcmThread_) {
        // Newly notified handles of all available ucm services
        ara::com::ServiceHandleContainer< pkgmgr::proxy::PackageManagementProxy::HandleType > newAvailableHandleVec;
        bool const r{ucmHandlesQueue_.TimeoutMovePop(newAvailableHandleVec, 100U)};
        if (!r) {
            continue;
        }

        AraList< pkgmgr::proxy::PackageManagementProxy::HandleType > newAvailableHandles{newAvailableHandleVec.begin(),
                                                                                         newAvailableHandleVec.end()};
        log_.LogDebug() << "FindUcmSubs::_handleUcmServices(),"
                        << "get newAvailableHandles.size():" << newAvailableHandles.size();

        // Process newly notified handles of all available ucm services
        std::set< ara::core::String > newAvailableInstanceIdSet;  // Set of newly notified available ucm service IDs
        for (pkgmgr::proxy::PackageManagementProxy::HandleType const& iter : newAvailableHandles) {
            ara::core::String const newAvailableInstanceId{std::move(iter.GetInstanceId().ToString())};
            log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), get a newAvailableInstanceId:"
                            << newAvailableInstanceId.c_str();

            // Save to set of newly notified available ucm service IDs
            std::ignore = newAvailableInstanceIdSet.insert(newAvailableInstanceId);

            // Check if already exists
            ara::core::Map< ara::core::String, PackageManagementAppPtr >::iterator const instanceIdAppIter{
                instanceIdAppMap_.find(newAvailableInstanceId)};
            if (instanceIdAppMap_.end() == instanceIdAppIter) {  // Did not exist before
                ara::core::Result< pkgmgr::proxy::PackageManagementProxy > ucmProxyRes{
                    ara::core::Result< pkgmgr::proxy::PackageManagementProxy >::FromError(
                        pkgmgr::UCMErrorDomainErrc::kGeneralReject)};
                ara::core::Result< pkgmgr::proxy::PackageManagementProxy > ucmProxy4QAC{
                    pkgmgr::proxy::PackageManagementProxy::Create(iter)};
                ucmProxyRes.Swap(ucmProxy4QAC);
                if (!ucmProxyRes.HasValue()) {
                    log_.LogError() << "FindUcmSubs::_handleUcmServices(), failed to create PackageManagementProxy for "
                                       "newAvailableInstanceId:"
                                    << newAvailableInstanceId.c_str()
                                    << "with Error:" << ucmProxyRes.Error().Message().data();
                    continue;
                }

                log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), succeeded to create PackageManagementProxy for "
                                   "newAvailableInstanceId:"
                                << newAvailableInstanceId.c_str();

                // Get UCMIdentifier
                std::shared_ptr< pkgmgr::proxy::PackageManagementProxy > ucmProxy{
                    std::make_shared< pkgmgr::proxy::PackageManagementProxy >(std::move(ucmProxyRes).Value())};
                // TODO(Han Zhibo): Handle unexpected restart exit
                ara::core::Result< pkgmgr::PackageManagement::GetIdOutput > const result{ucmProxy->GetId().GetResult()};
                if (!result.HasValue()) {
                    log_.LogError() << "FindUcmSubs::_handleUcmServices(), failed to GetId for newAvailableInstanceId:"
                                    << newAvailableInstanceId.c_str()
                                    << "with Error:" << result.Error().Message().data();
                    continue;
                }
                pkgmgr::UCMIdentifierType const ucmId{result.Value().id};
                log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), get ucmId:" << ucmId.c_str()
                                << "for newAvailableInstanceId:" << newAvailableInstanceId.c_str();

                PackageManagementAppPtr ucmProxyApp{
                    std::make_shared< PackageManagementApp >(ucmId, ucmProxy, apiWaitTime_)};
                if (!ucmProxyApp->StatusSubscribe()) {
                    log_.LogError() << "FindUcmSubs::_handleUcmServices(), failed to StatusSubscribe for ucmId:"
                                    << ucmId.c_str() << "newAvailableInstanceId:" << newAvailableInstanceId.c_str();
                    continue;
                }

                log_.LogDebug()
                    << "FindUcmSubs::_handleUcmServices(), succeeded to make PackageManagementApp for ucmId:"
                    << ucmId.c_str() << "newAvailableInstanceId:" << newAvailableInstanceId.c_str()
                    << "status:" << PackageManagementApp::CurrentStatusToString(ucmProxyApp->GetCurrentStatus()).data();

                // Initially service available
                ucmProxyApp->OnServiceAvailable();
                std::lock_guard< std::mutex > const lock{pmMtx_};
                std::ignore = instanceIdAppMap_.emplace(newAvailableInstanceId, ucmProxyApp);
                std::ignore = ucmIdAppMap_.emplace(ucmId, ucmProxyApp);
            } else {  // Already existed
                log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), there is an entry in instanceIdAppMap_ for "
                                   "newAvailableInstanceId:"
                                << newAvailableInstanceId.c_str();
                if (!(instanceIdAppIter->second->IsServiceAvailable())) {
                    instanceIdAppIter->second->OnServiceAvailable();
                }
            }
        }

        log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), get newAvailableInstanceIdSet:"
                        << Utils::ConcatenateStrings(newAvailableInstanceIdSet).c_str();

        // Process existing ucm service ID to its App(Proxy) mapping: those not in newAvailableInstanceIdSet are set to OnServiceUnavailable
        for (auto const& iter : instanceIdAppMap_) {
            ara::core::String const& instanceId{iter.first};
            log_.LogDebug() << "FindUcmSubs::_handleUcmServices(), get a instanceId:" << instanceId.c_str()
                            << " in instanceIdAppMap_";
            if (newAvailableInstanceIdSet.find(instanceId) == newAvailableInstanceIdSet.end()) {
                log_.LogDebug() << "FindUcmSubs::_handleUcmServices(),"
                                << " can't find instanceId:" << instanceId.c_str()
                                << " in newAvailableInstanceIdSet so will OnServiceUnavailable its App(Proxy).";
                iter.second->OnServiceUnavailable();
            }
        }
    }
    isHaveFindUcmThread_ = false;
}

/// @brief Discover UCM subordinates
/// @return bool
/// @throws no
bool FindUcmSubs::FindService()
{
    if (isBeginFindUcm_) {
        LOG_WARN << "have already begin";
        return false;
    }
    ara::core::InstanceSpecifier const portSpecifier{ara::core::InstanceSpecifier(
        std::move(ara::core::StringView("package_manager/package_manager_root/PackageManagementPPort")))};
    LOG_INFO << "Port In Executable Ref:" << portSpecifier.ToString().data();

    /***************************************************************/
    LOG_INFO << "begin";
    isBeginFindUcm_ = true;
    ucmSubServiceHandler_
        = pkgmgr::proxy::PackageManagementProxy::StartFindService(
              [this](ara::com::ServiceHandleContainer< pkgmgr::proxy::PackageManagementProxy::HandleType >
                         newAvailableHandles,
                     ara::com::FindServiceHandle const& handler) {
                  this->_findUcmHandler(std::move(newAvailableHandles),
                                        handler);  // FindUcmHandler will start a thread inside
              },
              ara::com::InstanceIdentifier(std::move(ara::core::StringView("ANY"))))
              .Value();
    return true;
}

/// @brief destructor
FindUcmSubs::~FindUcmSubs() noexcept
{
    log_.LogDebug() << "FindUcmSubs::~FindUcmSubs(), begin.";
    if (nullptr != findUcmThread_) {
        if (findUcmThread_->joinable()) {
            findUcmThread_->join();
            std::ignore = findUcmThread_.release();
        }
    }
    log_.LogDebug() << "FindUcmSubs::~FindUcmSubs(), end.";
}

/// @brief Stop discovery
/// @throws no
void FindUcmSubs::StopFindService()
{
    log_.LogDebug() << "FindUcmSubs::StopFindService(), begin.";

    // Stop discovering ucms
    pkgmgr::proxy::PackageManagementProxy::StopFindService(ucmSubServiceHandler_);
    isHaveFindUcmThread_ = false;
    ucmHandlesQueue_.Stop();

    log_.LogDebug() << "FindUcmSubs::StopFindService(), end.";
    return;
}

/// @brief Check if the specified ucm has been discovered and is online
/// @param ucmIds Array of specified ucms
/// @return true: All specified ucms are discovered and online. false: Some specified ucms are not discovered or offline
/// @throws no
bool FindUcmSubs::UcmIsAllExsit(AraList< ara::core::String > const& ucmIds)
{
    std::lock_guard< std::mutex > const lock{pmMtx_};
    for (auto const& ucmID : ucmIds) {
        ara::core::Map< ara::core::String, PackageManagementAppPtr >::iterator const mapIter{ucmIdAppMap_.find(ucmID)};
        if (mapIter == ucmIdAppMap_.end()) {
            LOG_WARN << "ucmId=" << ucmID.c_str() << " service don't find";
            return false;
        }
        if (!mapIter->second->UpdateStatus()) {
            LOG_WARN << "ucmId=" << ucmID.c_str() << " is offline!";
            return false;
        }
    }
    return true;
}

/// @brief Get all discovered ucm proxies
/// @return Returns discovered ucm proxies, keyed by ucmId
/// @throws no
ara::core::Map< ara::core::String, PackageManagementAppPtr > const FindUcmSubs::GetAllPmApp()
{
    /// Should be ucm proxies from the vehicle package, not all ucm proxies
    std::lock_guard< std::mutex > const lock{pmMtx_};
    return ucmIdAppMap_;
}

/// @brief Get ucm proxies corresponding to ucms in the current vehicle package
/// @param ucmIds
/// @param ucmIdAppVpkgMap
/// @return Returns ucm proxies corresponding to ucms in the current vehicle package, keyed by ucmId array
/// @throws no
bool FindUcmSubs::GetVpkgPmApp(AraList< ara::core::String > const& ucmIds,
                               ara::core::Map< ara::core::String, PackageManagementAppPtr >& ucmIdAppVpkgMap)
{
    std::lock_guard< std::mutex > const lock{pmMtx_};
    for (auto const& ucmID : ucmIds) {
        ara::core::Map< ara::core::String, PackageManagementAppPtr >::iterator const mapIter{ucmIdAppMap_.find(ucmID)};
        if (mapIter == ucmIdAppMap_.end()) {
            log_.LogWarn() << "FindUcmSubs::GetVpkgPmApp(),"
                           << "ucmId:" << ucmID.c_str() << " service don't find";
            return false;
        }
        if (!mapIter->second->UpdateStatus()) {
            log_.LogWarn() << "FindUcmSubs::GetVpkgPmApp(),"
                           << "ucmId:" << ucmID.c_str() << " is offline!";
            return false;
        }
        std::ignore = ucmIdAppVpkgMap.emplace(mapIter->first, mapIter->second);
    }

    return true;
}

/// @brief Get ucm proxy based on the specified ucmid
/// @param ucmId UCM id
/// @return Returns ucm proxy
/// @throws no
PackageManagementAppPtr FindUcmSubs::GetPackageManagementAppById(ara::core::String const& ucmId)
{
    PackageManagementAppPtr pmaPtr;
    std::lock_guard< std::mutex > const lock{pmMtx_};
    ara::core::Map< ara::core::String, PackageManagementAppPtr >::iterator const iter{ucmIdAppMap_.find(ucmId)};
    if (iter != ucmIdAppMap_.end()) {
        pmaPtr = iter->second;
    }
    return pmaPtr;
}

/// @brief Get all software cluster information from ucms
/// @return SwClusterInfoVectorType
/// @throws no
AraList< ::ara::ucm::pkgmgr::SwClusterInfoType > FindUcmSubs::GetAllSwClusterInfo()
{
    AraList< ::ara::ucm::pkgmgr::SwClusterInfoType > swClusterInfos;
    // Call the function to make a copy, to prevent being blocked by ucmIdAppMap_ operations when new ucms are discovered
    for (auto const& pair : this->GetAllPmApp()) {
        PackageManagementAppPtr const& ucmSubApp{pair.second};
        LOG_INFO << "begin get infos from " << ucmSubApp->GetId().c_str();
        pkgmgr::SwClusterInfoVectorType swInfos{ucmSubApp->GetSwClusterInfo()};
        if (false == swInfos.empty()) {
            std::ignore = swClusterInfos.insert(swClusterInfos.cend(), swInfos.begin(), swInfos.end());
        }
    }

    return swClusterInfos;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara