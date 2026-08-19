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
/// @file       campaign_state_base.h
/// @brief      CampaignStateBase
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
/// @unit_name=CampaignStateBase
/// @unit_description=CampaignStateBase impl
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CAMPAIGN_STATE_BASE_H_
#define VPKGMGR_SRC_FSM_CAMPAIGN_STATE_BASE_H_

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/ucm/pkgmgr/packagemanagement_proxy.h>
#include <ara/ucm/pkgmgr/vehiclepackagemanagement_skeleton.h>
#include <ara/ucm/vdai/vehicledriverapplication_skeleton.h>
#include <ara/ucm/vsm/vehiclestatemanager_skeleton.h>

#include <memory>

#include "ara/ucm/pkgmgr/impl_type_ucmmasterresolutiontype.h"
#include "fsm/locked_ptr_wrapper.h"
#include "fsm/task_executor/task_queue.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetSwClusterInfoOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterInfoOutput = pkgmgr::VehiclePackageManagement::GetSwClusterInfoOutput;
/// @brief SwPackageInventoryOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwPackageInventoryOutput = pkgmgr::VehiclePackageManagement::SwPackageInventoryOutput;
/// @brief TransferVehiclePackageOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferVehiclePackageOutput = pkgmgr::VehiclePackageManagement::TransferVehiclePackageOutput;

/// @brief GetSwInfoCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwInfoCBType = std::function< int32_t(ara::core::Promise< GetSwClusterInfoOutput >&& promise) >;
/// @brief SyncSwInfoCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SyncSwInfoCBType = std::function< void(pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
                                             ara::core::Promise< SwPackageInventoryOutput >&& promise) >;
/// @brief VpkgCampaignCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using VpkgCampaignCBType
    = std::function< int32_t(std::uint64_t const& size, ara::core::Promise< TransferVehiclePackageOutput >&& promise) >;

/// @brief SpkgCampaignCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SpkgCampaignCBType = std::function< int32_t() >;
/// @brief ProcessCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ProcessCBType = std::function< int32_t() >;
/// @brief ActivateCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ActivateCBType = std::function< int32_t() >;
/// @brief CancelCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using CancelCBType = std::function< int32_t(std::underlying_type< pkgmgr::UCMMasterResolutionType >::type cancelCause,
                                            ara::core::Promise< void >&& promise) >;
/// @brief VehicleCheckCBType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using VehicleCheckCBType = std::function< int32_t() >;

/// @brief CampaignStateTypeToString
/// @param type
/// @return type stringview
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00238
/// @trace_id_dd=DD_UCM_Master_00551
/// @needwork = ad
/// @endcode
ara::core::StringView CampaignStateTypeToString(pkgmgr::CampaignStateType const type);
/// @brief TransferStateTypeToString
/// @param type
/// @return type stringview
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00239
/// @trace_id_dd=DD_UCM_Master_00552
/// @needwork = ad
/// @endcode
ara::core::StringView TransferStateTypeToString(pkgmgr::TransferStateType const type);

/// @brief CampaignStateBase
class CampaignStateBase;
/// @brief StateAccessor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using StateAccessor = LockedPtrWrapper< CampaignStateBase >::Accessor;

/// @brief  Parent class for each activity state, does no substantive action processing, mainly used to return specific errors or report errors for incorrect state transitions.
/// For example, when the activity state is kVehiclePackageTransferring and SwPackageInventory is called, returns error code kOperationNotPermitted
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00240
/// @trace_id_dd=DD_UCM_Master_00553
/// @needwork = ad
/// @endcode
class CampaignStateBase
{
public:
    /// @brief Ptr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::unique_ptr< CampaignStateBase >;

    /// @brief constructor
    /// @param state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00241
    /// @trace_id_dd=DD_UCM_Master_00554
    /// @needwork = ad
    /// @endcode
    explicit CampaignStateBase(pkgmgr::CampaignStateType const state)
        : state_{state}
        , padding_{}
        , log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                      std::move(ara::core::StringView("CampaignStateBase context")),
                                      ara::log::LogLevel::kVerbose)}
    {
    }
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00242
    /// @trace_id_dd=DD_UCM_Master_00555
    /// @needwork = ad
    /// @endcode
    CampaignStateBase() = delete;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00243
    /// @trace_id_dd=DD_UCM_Master_00556
    /// @needwork = ad
    /// @endcode
    virtual ~CampaignStateBase() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00244
    /// @trace_id_dd=DD_UCM_Master_00557
    /// @needwork = ad
    /// @endcode
    CampaignStateBase(CampaignStateBase const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00245
    /// @trace_id_dd=DD_UCM_Master_00558
    /// @needwork = ad
    /// @endcode
    CampaignStateBase& operator=(CampaignStateBase const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00246
    /// @trace_id_dd=DD_UCM_Master_00559
    /// @needwork = ad
    /// @endcode
    CampaignStateBase(CampaignStateBase&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00247
    /// @trace_id_dd=DD_UCM_Master_00560
    /// @needwork = ad
    /// @endcode
    CampaignStateBase& operator=(CampaignStateBase&& other) = delete;

    /// @brief GetCampaignState
    /// @return CampaignStateType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00248
    /// @trace_id_dd=DD_UCM_Master_00561
    /// @needwork = ad
    /// @endcode
    pkgmgr::CampaignStateType GetCampaignState() const noexcept { return state_; }

    /// @brief Get UCM subordinate software cluster information
    /// @param accessor Activity state accessor
    /// @param getSwInfoCB Callback to get UCM software cluster information
    /// @return Returns software cluster information or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00249
    /// @trace_id_dd=DD_UCM_Master_00562
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Future< GetSwClusterInfoOutput > GetSwClusterInfo(StateAccessor const& accessor,
                                                                         GetSwInfoCBType const& getSwInfoCB);
    /// @brief Synchronously calculate which software clusters need to be updated
    /// @param accessor Activity state accessor
    /// @param availableSoftwarePackages List of newly released software cluster information
    /// @param syncSwInfoCB Software cluster information synchronization callback
    /// @return Returns list of software clusters that need to be upgraded
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00250
    /// @trace_id_dd=DD_UCM_Master_00563
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Future< SwPackageInventoryOutput > SwPackageInventory(
        StateAccessor const& accessor,
        pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
        SyncSwInfoCBType const& syncSwInfoCB);

    /// @brief Transfer vehicle package
    /// @param accessor Activity state accessor
    /// @param size Vehicle package size, in bytes
    /// @param vpkgCB Callback to start vehicle package transfer processing
    /// @return Returns vehicle package transfer id and block size, or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00251
    /// @trace_id_dd=DD_UCM_Master_00564
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Future< TransferVehiclePackageOutput > TransferVehiclePackage(StateAccessor const& accessor,
                                                                                     std::uint64_t const& size,
                                                                                     VpkgCampaignCBType const& vpkgCB);
    /// @brief GetCancelCause
    /// @return UCMMasterResolutionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00252
    /// @trace_id_dd=DD_UCM_Master_00565
    /// @needwork = ad
    /// @endcode
    virtual pkgmgr::UCMMasterResolutionType GetCancelCause() const noexcept
    {
        return pkgmgr::UCMMasterResolutionType::kSuccess;
    }

public:
    /// @brief Transition to kIdle state and perform related processing
    /// @param accessor Activity state accessor
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00253
    /// @trace_id_dd=DD_UCM_Master_00566
    /// @needwork = ad
    /// @endcode
    virtual int32_t DoIdle(StateAccessor const& accessor);

    /// @brief  Activity cancelled or execution failed, transition to kCanceling state and perform related processing
    /// @param accessor Activity state accessor
    /// @param cancelCause Reason for cancellation
    /// @param cancelCB
    /// @param promise
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00254
    /// @trace_id_dd=DD_UCM_Master_00567
    /// @needwork = ad
    /// @endcode
    virtual int32_t OnCancel(StateAccessor const& accessor,
                             pkgmgr::UCMMasterResolutionType cancelCause,
                             CancelCBType const& cancelCB,
                             ara::core::Promise< void >&& promise);

    /// @brief  Activity exit
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00255
    /// @trace_id_dd=DD_UCM_Master_00568
    /// @needwork = ad
    /// @endcode
    virtual void OnAbort() noexcept;

    /// @brief Transition to kSoftwarePackage_Transferring state and perform related processing
    /// @param accessor Activity state accessor
    /// @param spkgCB
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00256
    /// @trace_id_dd=DD_UCM_Master_00569
    /// @needwork = ad
    /// @endcode
    virtual int32_t DoSoftwareTransfer(StateAccessor const& accessor, SpkgCampaignCBType const& spkgCB);

    /// @brief Transition to kProcessing state, start coordinating ucm to install software packages (software package transfer exists in this process)
    /// @param accessor Activity state accessor
    /// @param processCB
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00257
    /// @trace_id_dd=DD_UCM_Master_00570
    /// @needwork = ad
    /// @endcode
    virtual int32_t DoProcess(StateAccessor const& accessor, ProcessCBType const& processCB);

    /// @brief Transition to kActivate state, start activation-related steps
    /// @param accessor Activity state accessor
    /// @param activateCB
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00258
    /// @trace_id_dd=DD_UCM_Master_00571
    /// @needwork = ad
    /// @endcode
    virtual int32_t DoActivate(StateAccessor const& accessor, ActivateCBType const& activateCB);

    /// @brief Transition to kVehicleCheck state, start VehicleCheck (OEM specific) related work
    /// @param accessor Activity state accessor
    /// @param vehicleCheckCB
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00259
    /// @trace_id_dd=DD_UCM_Master_00572
    /// @needwork = ad
    /// @endcode
    virtual int32_t DoVehicleCheck(StateAccessor const& accessor, VehicleCheckCBType const& vehicleCheckCB);

protected:
    /// @brief xxxx
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00573
    /// @needwork = dda
    /// @endcode
    pkgmgr::CampaignStateType const state_;  // NOLINT

    /// @brief xxxx
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00574
    /// @needwork = dda
    /// @endcode
    core::String padding_;  // NOLINT

    /// @brief xxxx
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00575
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_;  // NOLINT
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_CAMPAIGN_STATE_BASE_H_