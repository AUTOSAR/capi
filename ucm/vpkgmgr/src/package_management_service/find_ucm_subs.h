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
/// @file       find_ucm_subs.h
/// @brief      Mainly discovers UCM and manages UCM
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Package Management Service
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=FindUcmSubs
/// @unit_description=Discovers UCM and manages UCM
/// @endcode
///
/// ================================================================

#ifndef __PACKAGE_MANAGEMENT_FIND_UCM_SUBS_H_
#define __PACKAGE_MANAGEMENT_FIND_UCM_SUBS_H_

#include <ara/core/promise.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <atomic>
#include <memory>
#include <mutex>

#include "fsm/task_executor/safe_queue.h"
#include "package_management_service/package_management_app.h"
#include "utils/alias.h"
#include "utils/single_ton.h"
#include "utils/types.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Mainly discovers UCM and manages UCM
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00393
/// @trace_id_dd=DD_UCM_Master_00782
/// @needwork = ad
/// @endcode
class FindUcmSubs : public Singleton< FindUcmSubs >
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00394
    /// @trace_id_dd=DD_UCM_Master_00783
    /// @needwork = ad
    /// @endcode
    FindUcmSubs() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00395
    /// @trace_id_dd=DD_UCM_Master_00784
    /// @needwork = ad
    /// @endcode
    ~FindUcmSubs() noexcept;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00396
    /// @trace_id_dd=DD_UCM_Master_00785
    /// @needwork = ad
    /// @endcode
    FindUcmSubs(FindUcmSubs const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00397
    /// @trace_id_dd=DD_UCM_Master_00786
    /// @needwork = ad
    /// @endcode
    FindUcmSubs& operator=(FindUcmSubs const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00398
    /// @trace_id_dd=DD_UCM_Master_00787
    /// @needwork = ad
    /// @endcode
    FindUcmSubs(FindUcmSubs&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00399
    /// @trace_id_dd=DD_UCM_Master_00788
    /// @needwork = ad
    /// @endcode
    FindUcmSubs& operator=(FindUcmSubs&& other) = delete;

    /// @brief Set timeout for ucm general API calls
    /// @param apiWaitTime  Timeout value
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00400
    /// @trace_id_dd=DD_UCM_Master_00789
    /// @needwork = ad
    /// @endcode
    inline void SetApiWaitTime(uint32_t const apiWaitTime) noexcept { apiWaitTime_ = apiWaitTime; }

    /// @brief Discover UCM subordinates
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00401
    /// @trace_id_dd=DD_UCM_Master_00790
    /// @needwork = ad
    /// @endcode
    bool FindService();

    /// @brief Stop discovery
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00402
    /// @trace_id_dd=DD_UCM_Master_00791
    /// @needwork = ad
    /// @endcode
    void StopFindService();

    /// @brief Check if the specified ucm has been discovered and is online
    /// @param ucmIds Array of specified ucms
    /// @return true: All specified ucms are discovered and online. false: Some specified ucms are not discovered or offline
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00403
    /// @trace_id_dd=DD_UCM_Master_00792
    /// @needwork = ad
    /// @endcode
    bool UcmIsAllExsit(AraList< ara::core::String > const& ucmIds);

    /// @brief Get all discovered ucm proxies
    /// @return Returns discovered ucm proxies, keyed by ucmId
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00404
    /// @trace_id_dd=DD_UCM_Master_00793
    /// @needwork = ad
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< PackageManagementApp > > const GetAllPmApp();

    /// @brief Get ucm proxies corresponding to ucms in the current vehicle package
    /// @param ucmIds
    /// @param ucmIdAppVpkgMap
    /// @return Returns ucm proxies corresponding to ucms in the current vehicle package, keyed by ucmId array
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00405
    /// @trace_id_dd=DD_UCM_Master_00794
    /// @needwork = ad
    /// @endcode
    bool GetVpkgPmApp(AraList< ara::core::String > const& ucmIds,
                      ara::core::Map< ara::core::String, PackageManagementAppPtr >& ucmIdAppVpkgMap);

    /// @brief Get ucm proxy based on the specified ucmid
    /// @param ucmId UCM id
    /// @return Returns ucm proxy
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00406
    /// @trace_id_dd=DD_UCM_Master_00795
    /// @needwork = ad
    /// @endcode
    PackageManagementAppPtr GetPackageManagementAppById(ara::core::String const& ucmId);

    /// @brief Set ucm flag: whether to restart the machine in this activity
    /// @param ucmId  UCM subordinate Id
    /// @param isReboot true: machine needs to restart during activation, false: machine does not need to restart during activation
    /// @return
    /// @throws no
    ///int32_t SetUcmIsReboot(ara::core::String const& ucmId, bool const isReboot);

    // /// @brief Get whether the specified ucm needs to restart the machine
    // /// @param ucmId UCM Id
    // /// @return true: machine needs to restart during activation, false: machine does not need to restart during activation
    // ///
    ///bool GetUcmIsReboot(ara::core::String const& ucmId);  // Not used

    /// @brief Get all software cluster information from ucms
    /// @return SwClusterInfoVectorType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00407
    /// @trace_id_dd=DD_UCM_Master_00796
    /// @needwork = ad
    /// @endcode
    AraList< ::ara::ucm::pkgmgr::SwClusterInfoType > GetAllSwClusterInfo();

    /// @brief GetAllSwClusterDescription
    /// @return SwDescVectorType
    /// @throws no
    ///pkgmgr::SwDescVectorType GetAllSwClusterDescription();.

private:
    /// @brief _findUcmHandler
    /// @param newAvailableHandles
    /// @param handler
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00797
    /// @needwork = dda
    /// @endcode
    void _findUcmHandler(
        ara::com::ServiceHandleContainer< pkgmgr::proxy::PackageManagementProxy::HandleType > newAvailableHandles,
        ara::com::FindServiceHandle const& handler);
    /// @brief _handleUcmServices
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00798
    /// @needwork = dda
    /// @endcode
    void _handleUcmServices();

private:
    /// @brief isBeginFindUcm_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00799
    /// @needwork = dda
    /// @endcode
    bool isBeginFindUcm_{false};
    /// @brief isHaveFindUcmThread_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00800
    /// @needwork = dda
    /// @endcode
    bool isHaveFindUcmThread_{false};
    /// @brief apiWaitTime_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00801
    /// @needwork = dda
    /// @endcode
    uint32_t apiWaitTime_{kInt32_3000U};

    ///using OptionalHandle = ara::core::Optional<pkgmgr::proxy::PackageManagementProxy::HandleType>;
    ///SafeQueue<OptionalHandle> ucmHandleQueue_;

    /// @brief ucmHandlesQueue_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00802
    /// @needwork = dda
    /// @endcode
    SafeQueue< ara::com::ServiceHandleContainer< pkgmgr::proxy::PackageManagementProxy::HandleType > > ucmHandlesQueue_;

    /// @brief ucmSubServiceHandler_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00803
    /// @needwork = dda
    /// @endcode
    ara::com::FindServiceHandle ucmSubServiceHandler_;
    /// @brief findUcmThread_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00804
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< std::thread > findUcmThread_;
    // Save ucm proxy client
    /// @brief pmMtx_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00805
    /// @needwork = dda
    /// @endcode
    std::mutex pmMtx_;
    /// @brief instanceIdAppMap_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00806
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, PackageManagementAppPtr > instanceIdAppMap_;
    /// @brief ucmIdAppMap_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00807
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, PackageManagementAppPtr > ucmIdAppMap_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00808
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("FindUcmSubs context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  //__PACKAGE_MANAGEMENT_FIND_UCM_SUBS_H_