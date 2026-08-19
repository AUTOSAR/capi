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
/// @file       fsm_manager.h
/// @brief      FsmManager impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Fsm
/// @module_path=/UCM Master/Fsm
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00033,SR_UCM_00034,SR_UCM_00042,SR_UCM_00039,SR_UCM_00043,SR_UCM_00038
/// @unit_name=FsmManager
/// @unit_description=FsmManager impl
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_FSM_MANAGER_H_
#define VPKGMGR_SRC_FSM_FSM_MANAGER_H_

#include <ara/log/log_stream.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/vehiclepackagemanagement_common.h>
#include <ara/ucm/vdai/vehicledriverapplication_common.h>

#include <memory>

#include "fsm/campaign_steps/rollout_step.h"
#include "fsm/package_transfer/software_package_manager.h"
#include "fsm/package_transfer/vehicle_package_transfer.h"
#include "fsm/states/campaign_state_base.h"
#include "package_management_service/find_ucm_subs.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief VehiclePackageManagementApplication
class VehiclePackageManagementApplication;

/// @brief alias TransferStartOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferStartOutput = pkgmgr::VehiclePackageManagement::TransferStartOutput;

/// @brief Mainly responsible for OTA driver interface, vehicle status interface remote call implementation, and coordinating the state flow of the upgrade activity.
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00009
/// @trace_id_dd=DD_UCM_Master_00049
/// @needwork = ad
/// @endcode
class FsmManager : public Singleton< FsmManager >
{
public:
    /// @brief destructor
    ///~FsmManager() = default;

    /// @brief Initialize
    /// @param vpApplication
    /// @return Returns 0: success, non-zero: error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00010
    /// @trace_id_dd=DD_UCM_Master_00050
    /// @needwork = ad
    /// @endcode
    int32_t Init(VehiclePackageManagementApplication* const vpApplication);

    /// @brief Notify the driver of the current activity status of the ota_client
    /// @param state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00011
    /// @trace_id_dd=DD_UCM_Master_00051
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = ad
    /// @endcode
    static void UpdateCurrentStatus(pkgmgr::CampaignStateType const& state);

    /// @brief DoWhileSwitchingNewState
    /// @param pre
    /// @param s
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00012
    /// @trace_id_dd=DD_UCM_Master_00052
    /// @needwork = ad
    /// @endcode
    void DoWhileSwitchingNewState(CampaignStateBase const* const pre, CampaignStateBase const& s);

    /// @brief RetrieveState
    /// @return CampaignStateBase unique_ptr
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00013
    /// @trace_id_dd=DD_UCM_Master_00053
    /// @needwork = ad
    /// @endcode
    std::unique_ptr< CampaignStateBase > RetrieveState();

    /// @brief Check if the last update is complete; if not, continue the last update activity
    /// @param state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00014
    /// @trace_id_dd=DD_UCM_Master_00054
    /// @needwork = ad
    /// @endcode
    void ContinueLastUpdateCamaign(pkgmgr::CampaignStateType const state);

    /// @brief Set the data size for one transfer when ota_client transfers vehicle packages or software packages to VUCM
    /// @param blockSize Data size for one transfer, in bytes
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00015
    /// @trace_id_dd=DD_UCM_Master_00055
    /// @needwork = ad
    /// @endcode
    inline void SetBlockSize(uint32_t const blockSize) noexcept { blockSize_ = blockSize; }

    /// @brief After vehicle package parsing, this is the software package information to be upgraded in this activity
    /// @param spkgInfos List of software package information
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00016
    /// @trace_id_dd=DD_UCM_Master_00056
    /// @needwork = ad
    /// @endcode
    void ReSetSwPackageInfos(AraList< SwPackageInfoPtr > const& spkgInfos);

    /// @brief Get transferring software package information based on software package name fqn
    /// @param name Software package name fqn
    /// @return Returns smart pointer to software package information on success, null pointer on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00017
    /// @trace_id_dd=DD_UCM_Master_00057
    /// @needwork = ad
    /// @endcode
    SwPackageInfoPtr const GetTransferdSpkgByName(ara::core::String const& name) const;

    /// @brief Get software package information based on software package name fqn
    /// @param spkgFqn Software package name fqn
    /// @return Returns smart pointer to software package information on success, null pointer on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00018
    /// @trace_id_dd=DD_UCM_Master_00058
    /// @needwork = ad
    /// @endcode
    SwPackageInfoPtr const GetSpkgByName(ara::core::String const& spkgFqn) const;

    /// @brief Save Campaign repository
    /// @param repository
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00019
    /// @trace_id_dd=DD_UCM_Master_00059
    /// @needwork = ad
    /// @endcode
    bool SaveRepository(ara::core::String const& repository) const;

    /// @brief Save Campaign driverNotified
    /// @param driverNotified
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00020
    /// @trace_id_dd=DD_UCM_Master_00060
    /// @needwork = ad
    /// @endcode
    bool SaveDriverNotified(bool const driverNotified) const;

    /// @brief Save Campaign Resolution
    /// @param resolution
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00021
    /// @trace_id_dd=DD_UCM_Master_00061
    /// @needwork = ad
    /// @endcode
    bool SaveResolution(pkgmgr::UCMMasterResolutionType const resolution) const;

    /// @brief Record execution errors during the upgrade activity
    /// @param ucmStepError
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00022
    /// @trace_id_dd=DD_UCM_Master_00062
    /// @needwork = ad
    /// @endcode
    bool SaveUcmStepError(pkgmgr::UCMStepErrorType const& ucmStepError) const;

    // driver or ota
public:
    /// @brief Allow the update activity to proceed
    /// @param promise promise
    /// @return 0: execution successful, non-zero: error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00023
    /// @trace_id_dd=DD_UCM_Master_00063
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    int32_t AllowCampaign(ara::core::Promise< void >&& promise);

    /// @brief Cancel the current update activity
    /// @param disableCampaign true: not only cancel the current update activity, but also disable subsequent update activities
    /// @param promise
    /// @return 0: execution successful, non-zero: error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00024
    /// @trace_id_dd=DD_UCM_Master_00064
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    int32_t CancelCampaign(bool const& disableCampaign, ara::core::Promise< void >&& promise);

    /// @brief Get historical activity records within a specified time period
    /// @param timestampGE
    /// @param timestampLT
    /// @return CampaignHistoryVectorType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00025
    /// @trace_id_dd=DD_UCM_Master_00065
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00034
    /// @needwork = ad
    /// @endcode
    static pkgmgr::CampaignHistoryVectorType GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                std::uint64_t const& timestampLT);

    // driver
public:
    /// @brief List of general information of software clusters existing in the adaptive platform
    /// @return Returns a Future of the general software cluster information list or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00026
    /// @trace_id_dd=DD_UCM_Master_00066
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< vdai::VehicleDriverApplication::GetSwClusterDescriptionOutput > GetSwClusterDescription() const;

    /// @brief List of general information of software packages that are part of the current active activity processed by VUCM
    /// @return Returns a Future of the general software package information list or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00027
    /// @trace_id_dd=DD_UCM_Master_00067
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< vdai::VehicleDriverApplication::GetSwPackageDescriptionOutput > GetSwPackageDescription() const;

    /// @brief Get overall software package installation progress
    /// @return Returns progress 0-100 or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00028
    /// @trace_id_dd=DD_UCM_Master_00068
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< vdai::VehicleDriverApplication::GetSwProcessProgressOutput > GetSwProcessProgress() const;

    /// @brief Get overall software package transfer progress (VUCM -> UCM)
    /// @return Returns progress 0-100 or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00029
    /// @trace_id_dd=DD_UCM_Master_00069
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< vdai::VehicleDriverApplication::GetSwTransferProgressOutput > GetSwTransferProgress() const;

    // ota
public:
    /// @brief Get the list of software clusters in the kPresent state on the platform.
    /// @return Returns a future of a software cluster list or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00030
    /// @trace_id_dd=DD_UCM_Master_00070
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00033
    /// @needwork = ad
    /// @endcode
    ara::core::Future< pkgmgr::VehiclePackageManagement::GetSwClusterInfoOutput > GetSwClusterInfo();

    /// @brief Get software package information that is part of the current active activity processed by VUCM
    /// @return Returns a future of a software package information list or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00031
    /// @trace_id_dd=DD_UCM_Master_00071
    /// @needwork = ad
    /// @endcode
    ara::core::Future< pkgmgr::VehiclePackageManagement::GetSwPackagesOutput > GetSwPackages() const;

    /// @brief Transfer vehicle package
    /// @param size Size of the vehicle package compressed package, in bytes
    /// @return   Returns a future of a vehicle package transfer information handle or a specific error code SR_UCM_00039
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00032
    /// @trace_id_dd=DD_UCM_Master_00072
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00039
    /// @needwork = ad
    /// @endcode
    ara::core::Future< TransferVehiclePackageOutput > TransferVehiclePackage(std::uint64_t const& size);

    /// @brief Transfer software package
    /// @param stSoftwarePackageName Software package name, version
    /// @return   Returns a software package transfer information or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00033
    /// @trace_id_dd=DD_UCM_Master_00073
    /// @needwork = ad
    /// @endcode
    ara::core::Future< TransferStartOutput > TransferStart(pkgmgr::SwNameType const& stSoftwarePackageName) const;

    /// @brief Transfer a block of software package or vehicle package data
    /// @param id Transfer id
    /// @param data Transfer data block
    /// @param blockCounter Block counter value of the current block
    /// @return
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00034
    /// @trace_id_dd=DD_UCM_Master_00074
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > TransferData(pkgmgr::TransferIdType const& id,
                                           pkgmgr::ByteVectorType const& data,
                                           std::uint64_t const& blockCounter) const;

    /// @brief Transfer of software package or vehicle package (OTA->VUCM) to VUCM is complete.
    /// @param id Transfer handle
    /// @return Returns a void future or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00035
    /// @trace_id_dd=DD_UCM_Master_00075
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > TransferExit(pkgmgr::TransferIdType const& id) const;

    /// @brief Delete the vehicle package or software package with the specified id
    /// @param id Transfer id
    /// @return Returns void, or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00036
    /// @trace_id_dd=DD_UCM_Master_00076
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > DeleteTransfer(pkgmgr::TransferIdType const& id);

    /// @brief Delete the software package with the specified id
    /// @param id Transfer id
    /// @return Returns void, or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00037
    /// @trace_id_dd=DD_UCM_Master_00077
    /// @needwork = ad
    /// @endcode
    bool CleanSWPackage(pkgmgr::TransferIdType const& id) const;

    /// @brief Check if it can process
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00038
    /// @trace_id_dd=DD_UCM_Master_00078
    /// @needwork = ad
    /// @endcode
    bool CanProcess();

public:
    /// @brief Trigger synchronization of software cluster information (kIdle->kSync->kIdle)
    /// @param newSwpkgs Newly published software cluster information
    /// @return Software cluster information to be upgraded or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00039
    /// @trace_id_dd=DD_UCM_Master_00079
    /// @needwork = ad
    /// @endcode
    ara::core::Future< SwPackageInventoryOutput > TriggerSync(pkgmgr::SwNameVersionVectorType const& newSwpkgs) const;

    /// @brief Trigger vehicle package transfer
    /// @param size Vehicle package size
    /// @param promise
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00040
    /// @trace_id_dd=DD_UCM_Master_00080
    /// @needwork = ad
    /// @endcode
    int32_t TriggerVpkgTransfer(std::uint64_t const& size,
                                ara::core::Promise< TransferVehiclePackageOutput >&& promise);

    /// @brief Trigger software package transfer
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00041
    /// @trace_id_dd=DD_UCM_Master_00081
    /// @needwork = ad
    /// @endcode
    int32_t TriggerSoftpkgTransfer();

    /// @brief Trigger software package installation; in this state, software package transfer includes: OTA->VUCM VUCM->UCM
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00042
    /// @trace_id_dd=DD_UCM_Master_00082
    /// @needwork = ad
    /// @endcode
    bool TriggerProcess();

    /// @brief Trigger activation (after software package transfer and installation steps in this activity are completed)
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00043
    /// @trace_id_dd=DD_UCM_Master_00083
    /// @needwork = ad
    /// @endcode
    int32_t TriggerActivate();

    /// @brief Trigger VehicleCheck OEM specific
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00044
    /// @trace_id_dd=DD_UCM_Master_00084
    /// @needwork = ad
    /// @endcode
    int32_t TriggerVehicleCheck();

    /// @brief Trigger activity cancellation
    /// @param cancelCause Reason for cancellation
    /// @param promise
    /// @param delSwpks
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00045
    /// @trace_id_dd=DD_UCM_Master_00085
    /// @needwork = ad
    /// @endcode
    int32_t TriggerCancel(pkgmgr::UCMMasterResolutionType const cancelCause,
                          ara::core::Promise< void >&& promise,
                          bool const delSwpks = false);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00046
    /// @trace_id_dd=DD_UCM_Master_00086
    /// @needwork = ad
    /// @endcode
    int32_t TriggerCancelWithAccessor(StateAccessor const& accessor,
                                      pkgmgr::UCMMasterResolutionType const cancelCause,
                                      ara::core::Promise< void >&& promise,
                                      bool const delSwpks = false);

    /// @brief Trigger activity cancellation
    /// @param cancelCause
    /// @param delSwpks
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00047
    /// @trace_id_dd=DD_UCM_Master_00087
    /// @needwork = ad
    /// @endcode
    void TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType const cancelCause, bool const delSwpks = false) const;

    /// @brief Planned addition
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00048
    /// @trace_id_dd=DD_UCM_Master_00088
    /// @needwork = ad
    /// @endcode
    bool CanWorkAsMaster();

    /// @brief Check if can continue moving forward
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00049
    /// @trace_id_dd=DD_UCM_Master_00089
    /// @needwork = ad
    /// @endcode
    bool CanMoveForward();

    /// @brief Exit Campaign
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00050
    /// @trace_id_dd=DD_UCM_Master_00090
    /// @needwork = ad
    /// @endcode
    void Abort();

    /// @brief CleanUpAllUCMs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00051
    /// @trace_id_dd=DD_UCM_Master_00091
    /// @needwork = ad
    /// @endcode
    void CleanUpAllUCMs() const;

    /// @brief Trigger transition to kIdle state, perform related resource cleanup and reset
    /// @param fromState
    /// @param delSwpks
    /// @return int32_t
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00052
    /// @trace_id_dd=DD_UCM_Master_00092
    /// @needwork = ad
    /// @endcode
    int32_t TriggerIdle(pkgmgr::CampaignStateType const& fromState, bool const delSwpks = false);

    /// @brief Reset
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00053
    /// @trace_id_dd=DD_UCM_Master_00093
    /// @needwork = ad
    /// @endcode
    void Reset() noexcept;

    /// @brief Mark Campaign cancellation
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00054
    /// @trace_id_dd=DD_UCM_Master_00094
    /// @needwork = ad
    /// @endcode
    void SetCampaignCancelFlag() noexcept;

    /// @brief Reset Campaign cancellation mark
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00055
    /// @trace_id_dd=DD_UCM_Master_00095
    /// @needwork = ad
    /// @endcode
    void ResetCampaignCancelFlag() noexcept;

    /// @brief  Terminate
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00056
    /// @trace_id_dd=DD_UCM_Master_00096
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

    /// @brief Count the number of software packages to be transferred in this upgrade activity (exception: the same software package may be transferred multiple times)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00057
    /// @trace_id_dd=DD_UCM_Master_00097
    /// @needwork = ad
    /// @endcode
    inline void TransferAdd() noexcept { transferTotal_++; };
    /// @brief Increment by 1 for each software package processed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00058
    /// @trace_id_dd=DD_UCM_Master_00098
    /// @needwork = ad
    /// @endcode
    inline void TransferredAdd() noexcept { transferredNum_++; };
    /// @brief GetTransferProgress
    /// @return uint8_t
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00059
    /// @trace_id_dd=DD_UCM_Master_00099
    /// @needwork = ad
    /// @endcode
    uint8_t GetTransferProgress() const noexcept;

    /// @brief Similarly for software package transfer
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00060
    /// @trace_id_dd=DD_UCM_Master_00100
    /// @needwork = ad
    /// @endcode
    inline void ProcessAdd() noexcept { processTotal_++; };
    /// @brief Increment by 1 for each software package processed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00061
    /// @trace_id_dd=DD_UCM_Master_00101
    /// @needwork = ad
    /// @endcode
    inline void ProcessedAdd() noexcept { processedNum_++; };
    /// @brief GetProcessProgress
    /// @return uint8_t
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00062
    /// @trace_id_dd=DD_UCM_Master_00102
    /// @needwork = ad
    /// @endcode
    uint8_t GetProcessProgress() const noexcept;

    /// @brief Update execution steps
    /// @param rolloutSteps
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00063
    /// @trace_id_dd=DD_UCM_Master_00103
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void UpdateRolloutSteps(ara::core::Vector< RolloutStep::Ptr >&& rolloutSteps) noexcept;

    /// @brief Update heartbeat
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00064
    /// @trace_id_dd=DD_UCM_Master_00104
    /// @needwork = ad
    /// @endcode
    void UpdateHeartbeat();

    /// @brief GetVarDataDir
    /// @return String
    /// @throws no
    /// @code{.isoft}
    /// export_level=/UCMM/UCM PackageManager/UCM::Storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00065
    /// @trace_id_dd=DD_UCM_Master_00105
    /// @needwork = ad
    /// @endcode
    ara::core::String GetVarDataDir() const;

    /// @brief GetStatusFilePath
    /// @return String
    /// @throws no
    /// @code{.isoft}
    /// export_level=/UCM/UCM PackageManager/UCM::Storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00066
    /// @trace_id_dd=DD_UCM_Master_00106
    /// @needwork = ad
    /// @endcode
    ara::core::String GetStatusFilePath() const;

    /// @brief GetStatusMD5FilePath
    /// @return String
    /// @throws no
    /// @code{.isoft}
    /// export_level=/UCM/UCM PackageManager/UCM::Storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00067
    /// @trace_id_dd=DD_UCM_Master_00107
    /// @needwork = ad
    /// @endcode
    ara::core::String GetStatusMD5FilePath() const;

    /// @brief Whether Finish has started
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// export_level=/UCM/UCM PackageManager/UCM::Storage
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00068
    /// @trace_id_dd=DD_UCM_Master_00108
    /// @needwork = ad
    /// @endcode
    bool HasBegunFinish() const noexcept { return beginFinish_; }

private:
    /// @brief _createState
    /// @param status
    /// @param cancelCause
    /// @return CampaignStateBase::Ptr
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00109
    /// @needwork = dda
    /// @endcode
    CampaignStateBase::Ptr _createState(pkgmgr::CampaignStateType const status,
                                        pkgmgr::UCMMasterResolutionType const cancelCause) const;

    /// @brief _saveStatus
    /// @param state
    /// @param cancelCause
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00110
    /// @needwork = dda
    /// @endcode
    void _saveStatus(pkgmgr::CampaignStateType const& state,
                     pkgmgr::UCMMasterResolutionType const cancelCause
                     = pkgmgr::UCMMasterResolutionType::kSuccess) const;

    /// @brief Execute installation steps in a background thread
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00111
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = dda
    /// @endcode
    int32_t _executeProcessStep();

    /// @brief Execute activation steps in a background thread
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00112
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = dda
    /// @endcode
    int32_t _executeActivateStep();

    /// @brief Execute rollback steps in a background thread
    /// @param cancelCause
    /// @param fromState
    /// @param promise
    /// @param delSwpks
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00113
    /// @needwork = dda
    /// @endcode
    int32_t _executeCancel(pkgmgr::UCMMasterResolutionType const cancelCause,
                           pkgmgr::CampaignStateType const fromState,
                           std::shared_ptr< ara::core::Promise< void > > const& promise,
                           bool const delSwpks = false);

    /// @brief Execute VehicleCheck (OEM specific) steps in a background thread
    /// @return 0 on success, non-zero on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00114
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = dda
    /// @endcode
    int32_t _executeVehicleCheck();

    /// @brief Reload the vehicle package from the last update
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00115
    /// @needwork = dda
    /// @endcode
    void _reloadLastVehiclePackage();

    /// @brief Heartbeat update
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00116
    /// @needwork = dda
    /// @endcode
    void _updateHeartBeat();

public:
    /// @brief Mark if processing of the vehicle package has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00069
    /// @trace_id_dd=DD_UCM_Master_00117
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkProcessingVPStart() noexcept;

    /// @brief Mark the completion of VehiclePackageTransfer::ProcessVehiclePackage
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00070
    /// @trace_id_dd=DD_UCM_Master_00118
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkProcessingVPFinish() noexcept;

    /// @brief Wait for VehiclePackageTransfer::ProcessVehiclePackage to finish / not yet executed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00071
    /// @trace_id_dd=DD_UCM_Master_00119
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitProcessingVPFinish();

    /// @brief Mark if software package transfer has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00072
    /// @trace_id_dd=DD_UCM_Master_00120
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkTransferringStart() noexcept;

    /// @brief Mark the end of software package transfer in SOFTWAREPACKAGE_TRANSFERRING state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00073
    /// @trace_id_dd=DD_UCM_Master_00121
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkTransferringFinish() noexcept;

    /// @brief  Wait for the software package transfer in this state within FsmManager::ExecuteProcessStep to end:
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00074
    /// @trace_id_dd=DD_UCM_Master_00122
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitTransferringFinish();

    /// @brief Mark if software package processing has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00075
    /// @trace_id_dd=DD_UCM_Master_00123
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkProcessingStart() noexcept;

    /// @brief Mark the end of software package processing
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00076
    /// @trace_id_dd=DD_UCM_Master_00124
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkProcessingFinish() noexcept;

    /// @brief Wait for FsmManager::ExecuteProcessStep to end
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00077
    /// @trace_id_dd=DD_UCM_Master_00125
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitProcessingFinish();

    /// @brief Mark if Activate has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00078
    /// @trace_id_dd=DD_UCM_Master_00126
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkActivatingStart() noexcept;

    /// @brief Mark the completion of FsmManager::ExecuteActivateStep
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00079
    /// @trace_id_dd=DD_UCM_Master_00127
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkActivatingFinish() noexcept;

    /// @brief Wait for FsmManager::ExecuteActivateStep to end
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00080
    /// @trace_id_dd=DD_UCM_Master_00128
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitActivatingFinish();

    /// @brief Mark if VehicleCheck preparation has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00081
    /// @trace_id_dd=DD_UCM_Master_00129
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkVehicleCheckPreparingStart() noexcept;

    /// @brief Mark the completion of VehicleCheck preparation
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00082
    /// @trace_id_dd=DD_UCM_Master_00130
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkVehicleCheckPreparingFinish() noexcept;

    /// @brief Wait for the end of preparation in ExecuteVehicleCheck
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00083
    /// @trace_id_dd=DD_UCM_Master_00131
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitVehicleCheckPreparingFinish();

    /// @brief Mark if Finish has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00084
    /// @trace_id_dd=DD_UCM_Master_00132
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkVehicleCheckFinishingStart() noexcept;

    /// @brief Mark the completion of Finish
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00085
    /// @trace_id_dd=DD_UCM_Master_00133
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkVehicleCheckFinishingFinish() noexcept;

    /// @brief Wait for the end of Finish in ExecuteVehicleCheck
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00086
    /// @trace_id_dd=DD_UCM_Master_00134
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitVehicleCheckFinishingFinish();

    /// @brief Mark if cancellation has started
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00087
    /// @trace_id_dd=DD_UCM_Master_00135
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkCancellingStart() noexcept;

    /// @brief Mark the completion of FsmManager::ExecuteCancel
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00088
    /// @trace_id_dd=DD_UCM_Master_00136
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void MarkCancellingFinish() noexcept;

    /// @brief Wait for FsmManager::ExecuteCancel to end
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00089
    /// @trace_id_dd=DD_UCM_Master_00137
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00043
    /// @needwork = ad
    /// @endcode
    void WaitCancellingFinish();

private:
    /// @brief varDataDir_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00138
    /// @needwork = dda
    /// @endcode
    ara::core::String varDataDir_;  // ucmm data directory, currently stores: vpkgs, status.json, history.json

    /// @brief Directory address for storing vehicle packages and software packages
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00139
    /// @needwork = dda
    /// @endcode
    ara::core::String vpkgsDir_;
    /// @brief Current activity state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00140
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< LockedPtrWrapper< CampaignStateBase > > state_;
    /// @brief UCM subordinate discovery instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00141
    /// @needwork = dda
    /// @endcode
    FindUcmSubs* subs_{nullptr};

    /// @brief Mainly responsible for storing vehicle packages and software packages. Pure package transfer handling (shared by software packages and vehicle packages)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00142
    /// @needwork = dda
    /// @endcode
    PackageStorage::Ptr packageStorage_;
    /// @brief Vehicle package transfer handling
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00143
    /// @needwork = dda
    /// @endcode
    VehiclePackageTransfer::UPtr vpkgTransfer_;
    /// @brief Software package transfer handling
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00144
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager::UPtr spkgTransferManager_;

    /// @brief ID of the UCM on the local machine
    ///ara::core::String ucmId_;.
    /// @brief Wait time in milliseconds
    ///uint32_t waitTime_;
    /// @brief Data block size for one transfer of vehicle package/software package from ota->VUCM, in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00145
    /// @needwork = dda
    /// @endcode
    uint32_t blockSize_;

    /// @name terminate_
    ///  Flag indicating that application is shutting down.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00146
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > terminate_{false};

    /// @brief Campaign cancellation flag false: not cancelling, true: cancelling
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00147
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > campaignCancelFlag_{
        false};  // campaignCancelFlag_ is used to notify campaign end; cancelExecuting_ is used to mark the execution status of Cancel

    /// @brief transferTotal_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00148
    /// @needwork = dda
    /// @endcode
    uint32_t transferTotal_;
    /// @brief transferredNum_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00149
    /// @needwork = dda
    /// @endcode
    std::atomic_uint32_t transferredNum_;

    /// @brief processTotal_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00150
    /// @needwork = dda
    /// @endcode
    uint32_t processTotal_;
    /// @brief processedNum_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00151
    /// @needwork = dda
    /// @endcode
    std::atomic_uint32_t processedNum_;

    /// @brief  Software package steps of the current campaign
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00152
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< RolloutStep::Ptr > rolloutSteps_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00153
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("FsmManager context")),
                                                  ara::log::LogLevel::kVerbose)};

    /// @brief Key for persistent storage of Campaign history records
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00154
    /// @needwork = dda
    /// @endcode
    core::String campaignHistoryKey_{};

    /// @brief beginFinish_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00155
    /// @needwork = dda
    /// @endcode
    bool beginFinish_{false};  // Whether Finish has started
    /// @brief lastHeatBeatTime_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00156
    /// @needwork = dda
    /// @endcode
    std::uint64_t lastHeatBeatTime_{0};  // Last heartbeat time
    /// @brief allowCampaign_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00157
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > allowCampaign_{true};  // Whether updates are allowed
    /// @brief retrievedSwpkFQN2TransferIDMap_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00158
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType >
        retrievedSwpkFQN2TransferIDMap_;  // Mapping of software package FQN to its transfer ID retrieved from persistence
    /// @brief preState_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00159
    /// @needwork = dda
    /// @endcode
    pkgmgr::CampaignStateType preState_{
        pkgmgr::CampaignStateType::kIdle};  // Previous CampaignState, currently only used during Cancel

    /// @name condMutex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00160
    /// @needwork = dda
    /// @endcode
    std::mutex condMutex_;

    /// @brief Flag indicating whether vehicle package is being processed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00161
    /// @needwork = dda
    /// @endcode
    bool processingVP_{false};

    /// @name processingVPCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00162
    /// @needwork = dda
    /// @endcode
    std::condition_variable processingVPCondition_;

    /// @brief Flag indicating whether software package transfer has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00163
    /// @needwork = dda
    /// @endcode
    bool transferring_{false};

    /// @name transferringCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00164
    /// @needwork = dda
    /// @endcode
    std::condition_variable transferringCondition_;

    /// @brief Flag indicating whether software package processing has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00165
    /// @needwork = dda
    /// @endcode
    bool processing_{false};

    /// @name processingCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00166
    /// @needwork = dda
    /// @endcode
    std::condition_variable processingCondition_;

    /// @name hasTriggerredProcessMutex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00167
    /// @needwork = dda
    /// @endcode
    std::mutex hasTriggerredProcessMutex_;
    /// @brief Flag indicating whether processing state has been triggered
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00168
    /// @needwork = dda
    /// @endcode
    bool hasTriggerredProcess_{false};
    /// @code{.isoft}
    /// @nbriefame Whether processing state was successfully triggered
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00169
    /// @needwork = dda
    /// @endcode
    bool triggerredProcessSuccess_{false};

    /// @brief Flag indicating whether Activate has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00170
    /// @needwork = dda
    /// @endcode
    bool activating_{false};

    /// @name activatingCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00171
    /// @needwork = dda
    /// @endcode
    std::condition_variable activatingCondition_;

    /// @brief Flag indicating whether vehicleCheck preparation has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00172
    /// @needwork = dda
    /// @endcode
    bool vehicleCheckPreparing_{false};

    /// @name vehicleCheckPreparingCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00173
    /// @needwork = dda
    /// @endcode
    std::condition_variable vehicleCheckPreparingCondition_;

    /// @brief Flag indicating whether Finish has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00174
    /// @needwork = dda
    /// @endcode
    bool vehicleCheckFinishing_{false};

    /// @name vehicleCheckFinishingCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00175
    /// @needwork = dda
    /// @endcode
    std::condition_variable vehicleCheckFinishingCondition_;

    /// @brief Flag indicating whether cancellation execution has started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00176
    /// @needwork = dda
    /// @endcode
    bool cancelExecuting_{false};

    /// @brief cancellingCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00177
    /// @needwork = dda
    /// @endcode
    std::condition_variable cancellingCondition_;

    /// @brief abortMutex_ abort condition variable mutex
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00178
    /// @needwork = dda
    /// @endcode
    std::mutex abortMutex_;

    /// @brief hasAborted_ Whether already exited
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00179
    /// @needwork = dda
    /// @endcode
    bool hasAborted_{false};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _PROCESSOR_FSM_MANAGER_H__
