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
/// @file       filesystem_swcl_manager.h
/// @brief      Software cluster manager working with the filesystem
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwareClusterManager
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00032
/// @unit_name=FileSystemSWCLManager
/// @unit_description=Software cluster manager working with the filesystem directly
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_FILESYSTEMSWCLMANAGER_H_
#define ARA_UCM_PKGMGR_STORAGE_FILESYSTEMSWCLMANAGER_H_

#include "ara/ucm/pkgmgr/impl_type_gethistoryvectortype.h"
#include "common/alias.h"
#include "reversible_action.h"
#include "types/impl_type_actionexecutionInfotype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief software cluster manager working with the filesystem directly
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10271
/// @trace_id_dd=DD_UCM_10594
/// @needwork = ad
/// @endcode
class FileSystemSWCLManager
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10272
    /// @trace_id_dd=DD_UCM_10595
    /// @needwork = ad
    /// @endcode
    FileSystemSWCLManager() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10273
    /// @trace_id_dd=DD_UCM_10596
    /// @needwork = ad
    /// @endcode
    virtual ~FileSystemSWCLManager() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10274
    /// @trace_id_dd=DD_UCM_10597
    /// @needwork = ad
    /// @endcode
    FileSystemSWCLManager(FileSystemSWCLManager const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10275
    /// @trace_id_dd=DD_UCM_10598
    /// @needwork = ad
    /// @endcode
    FileSystemSWCLManager& operator=(FileSystemSWCLManager const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10276
    /// @trace_id_dd=DD_UCM_10599
    /// @needwork = ad
    /// @endcode
    FileSystemSWCLManager(FileSystemSWCLManager&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10277
    /// @trace_id_dd=DD_UCM_10600
    /// @needwork = ad
    /// @endcode
    FileSystemSWCLManager& operator=(FileSystemSWCLManager&& other) = delete;

    /// @brief Retrieve a list of SoftwareClusters that have pending changes.
    /// @return FutureGetSwClusterChangeInfo
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10278
    /// @trace_id_dd=DD_UCM_10601
    /// @trace_id_sr=SR_UCM_00011
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterChangeInfo GetSwClusterChangeInfo() const noexcept;

    /// @brief Retrieve a Software Clusters information list.
    /// @return FutureGetSwClusterDescription
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10279
    /// @trace_id_dd=DD_UCM_10602
    /// @trace_id_sr=SR_UCM_00002
    /// @needwork = ad
    /// @endcode
    static FutureGetSwClusterDescription GetSwClusterDescription() noexcept;

    /// @brief Retrieve a list of SoftwareClusters.
    /// @return FutureGetSwClusterInfo
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10280
    /// @trace_id_dd=DD_UCM_10603
    /// @trace_id_sr=SR_UCM_00002
    /// @needwork = ad
    /// @endcode
    static FutureGetSwClusterInfo GetSwClusterInfo() noexcept;

    /// @brief Retrieve all actions performed by UCM in provided time range.
    /// @param timestampGE
    /// @param timestampLT
    /// @return FutureGetHistory
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10281
    /// @trace_id_dd=DD_UCM_10604
    /// @trace_id_sr=SR_UCM_00032
    /// @needwork = ad
    /// @endcode
    static FutureGetHistory GetHistory(std::uint64_t const& timestampGE, std::uint64_t const& timestampLT) noexcept;

    /// @brief Resets the software clusters change info. vector
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10282
    /// @trace_id_dd=DD_UCM_10605
    /// @needwork = ad
    /// @endcode
    virtual void ResetSWCLChangeInfo();

    /// @brief Adds new software cluster change info
    /// @param swclName
    /// @param swclVersion
    /// @param actionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10283
    /// @trace_id_dd=DD_UCM_10606
    /// @needwork = ad
    /// @endcode
    virtual void AddSWCLChangeInfo(AraString const& swclName,
                                   AraString const& swclVersion,
                                   ActionType const actionType);

    /// @brief Return all SoftwareClusters that exist in SWCLs
    ///
    /// @returns vector of SoftwareClusters
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10284
    /// @trace_id_dd=DD_UCM_10607
    /// @needwork = ad
    /// @endcode
    static AraVector< SoftwareCluster > GetAllSwcls();

    /// @brief Return all dirs of SoftwareClusters that exist in SWCLs
    ///
    /// @returns vector of dirs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10285
    /// @trace_id_dd=DD_UCM_10608
    /// @needwork = ad
    /// @endcode
    static AraList< AraString > GetAllSwclDirs();

    /// @brief Return all software packages in state kPresent
    ///
    /// @returns vector of packages
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10286
    /// @trace_id_dd=DD_UCM_10609
    /// @needwork = ad
    /// @endcode
    static AraList< SoftwareCluster > GetPresentSwcls();

    /// @brief Return all processes that exist in SWCLs in state kPresent, kAdded and kUpdated.
    /// @returns vector of applications (i.e. processes)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10287
    /// @trace_id_dd=DD_UCM_10610
    /// @needwork = ad
    /// @endcode
    static AraList< SoftwareCluster > GetSWCLsForActivation();

    /// @brief Return all SoftwareClusters that exist in SWCLs in state kRemoved, kAdded and kUpdated.
    /// @returns vector of SoftwareClusters
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10288
    /// @trace_id_dd=DD_UCM_10611
    /// @needwork = ad
    /// @endcode
    static AraList< SoftwareCluster > GetSWCLsToActivate();

    /// @brief Return SoftwareClusters that prepare to update
    /// @returns vector of SoftwareClusters
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10289
    /// @trace_id_dd=DD_UCM_10612
    /// @needwork = ad
    /// @endcode
    static AraList< SoftwareCluster > GetSWCLsToPrepareUpdate();

    /// @brief clean software cluster
    /// @param swclName
    /// @param retainedVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10290
    /// @trace_id_dd=DD_UCM_10613
    /// @needwork = ad
    /// @endcode
    static bool CleanSwcl(AraString const& swclName, AraString const& retainedVersion);

    /// @brief Checks if the version of a software cluster is older than the existing one.
    /// @param swcl The software cluster to be checked.
    /// @returns True if the version of the software cluster is downgraded, false otherwise.
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00103, 72d6a4ff65df294f6ab37f5eee80221335635046}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10291
    /// @trace_id_dd=DD_UCM_10614
    /// @needwork = ad
    /// @endcode
    static bool IsSwclOldVersion(SoftwareCluster const& swcl);

    /// @brief platform_core is updated or not
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10292
    /// @trace_id_dd=DD_UCM_10615
    /// @needwork = ad
    /// @endcode
    static bool IsPlatformCoreUpdated();

    /// @brief non-platform_core is updated or not
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10293
    /// @trace_id_dd=DD_UCM_10616
    /// @needwork = ad
    /// @endcode
    static bool IsNonPlatformCoreUpdated();

    /// @brief os is updated or not
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10294
    /// @trace_id_dd=DD_UCM_10617
    /// @needwork = ad
    /// @endcode
    static bool IsOSUpdated();

    /// @brief Add one action to the queue of actions.
    ///
    /// Adds one action to the queue of the actions to be executed. Empty actions (nullptr) are ignored.
    /// @param action
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10295
    /// @trace_id_dd=DD_UCM_10618
    /// @needwork = ad
    /// @endcode
    virtual void AddAction(std::unique_ptr< ReversibleAction > action);

    /// @brief name and version of software cluster is in actions or not.
    /// @param swclName the name of the software cluster
    /// @param swclVer the version of the software cluster
    /// @return true if exist.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10296
    /// @trace_id_dd=DD_UCM_10619
    /// @needwork = ad
    /// @endcode
    virtual bool ExistsInActions(AraString const& swclName, AraString const& swclVer);

    /// @brief Persist all actions to the filesystem
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10297
    /// @trace_id_dd=DD_UCM_10620
    /// @needwork = ad
    /// @endcode
    virtual void PersistActionsToFilesystem() noexcept;

    /// @brief Recover all actions from the filesystem
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10298
    /// @trace_id_dd=DD_UCM_10621
    /// @needwork = ad
    /// @endcode
    virtual void RecoverActionsFromFilesystem() noexcept;

    /// @brief RecoverProcessedSwpkgAction
    /// @param actionExecutionInfoVec
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10299
    /// @trace_id_dd=DD_UCM_10622
    /// @needwork = ad
    /// @endcode
    void RecoverProcessedSwpkgAction(AraList< ActionExecutionInfoType > const& actionExecutionInfoVec);

    /// @brief getter returning action related information of this update sequence
    ///
    /// @return action related information, not the actual actions' ownership
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10300
    /// @trace_id_dd=DD_UCM_10623
    /// @needwork = ad
    /// @endcode
    AraList< GetHistoryType > GetReversibleActionsInfo() const;

    /// @brief GetActionExecutionInfo
    /// @returns ActionExecutionInfoType vector
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10301
    /// @trace_id_dd=DD_UCM_10624
    /// @needwork = ad
    /// @endcode
    AraList< ActionExecutionInfoType > GetActionExecutionInfo() const;

    /// @brief The strongest activation option
    ///
    /// @returns the strongest activation option: system reset or function group restart or nothing at all
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10302
    /// @trace_id_dd=DD_UCM_10625
    /// @needwork = ad
    /// @endcode
    virtual ActivateOptionType GetStrongestActivationOption() const noexcept;

    /// @brief Get the activation option for the SoftwareCluster
    /// @param swcl
    /// @returns the activation option: system reset or function group restart or nothing at all
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10303
    /// @trace_id_dd=DD_UCM_10626
    /// @needwork = ad
    /// @endcode
    virtual ActivateOptionType GetActivationOption4Swcl(SoftwareCluster const& swcl) const;

    /// @brief Commits all actions
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10304
    /// @trace_id_dd=DD_UCM_10627
    /// @needwork = ad
    /// @endcode
    virtual AraResultVoid CommitChanges();

    /// @brief Reverts all actions
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10305
    /// @trace_id_dd=DD_UCM_10628
    /// @needwork = ad
    /// @endcode
    virtual AraResultVoid RevertChanges();

    /// @brief MergeVar for all actions
    /// @returns result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10306
    /// @trace_id_dd=DD_UCM_10629
    /// @needwork = ad
    /// @endcode
    virtual AraResultVoid MergeVars();

    /// @brief update software cluster status
    /// @param swclName
    /// @param swclState
    /// @param version
    /// @param newVersion
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10307
    /// @trace_id_dd=DD_UCM_10630
    /// @needwork = ad
    /// @endcode
    static void UpdateSwclStatus(AraString const& swclName,
                                 SwClusterStateType const swclState,
                                 AraString const& version,
                                 AraString const& newVersion);

    /// @brief get software cluster status
    /// @param swclName
    /// @param swclState
    /// @param version
    /// @param newVersion
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10308
    /// @trace_id_dd=DD_UCM_10631
    /// @needwork = ad
    /// @endcode
    static bool GetSwclStatus(AraString const& swclName,
                              SwClusterStateType& swclState,
                              AraString& version,
                              AraString& newVersion);

    /// @brief convert SwClusterStateType to string
    /// @param swclState
    /// @returns string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10309
    /// @trace_id_dd=DD_UCM_10632
    /// @needwork = ad
    /// @endcode
    static AraString SwclStateToStr(SwClusterStateType const swclState);

    /// @brief convert SwClusterStateType from string
    /// @param swclStateStr
    /// @returns SwClusterStateType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10310
    /// @trace_id_dd=DD_UCM_10633
    /// @needwork = ad
    /// @endcode
    static SwClusterStateType SwclStateFromStr(AraString const& swclStateStr);

    /// @brief UpdateSWCLList
    /// @param activeSWCLs
    /// @param updateSwclCategorySet
    /// @param swclListFilePath
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10311
    /// @trace_id_dd=DD_UCM_10634
    /// @needwork = ad
    /// @endcode
    static bool UpdateSWCLList(AraList< SoftwareCluster > const& activeSWCLs,
                               std::set< AraString > const& updateSwclCategorySet,
                               AraString const& swclListFilePath);

private:
    /// @brief Parse system software packages' data and cache it
    /// @param forPreparingUpdate
    /// @returns SoftwareCluster vector
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10635
    /// @needwork = dda
    /// @endcode
    static AraVector< SoftwareCluster > IndexActionsAndSwcls(bool const forPreparingUpdate = false);

private:
    /// @brief The list of all executed actions
    /// The list of all executed actions is filled in PROCESSING and emptied in CLEANING_UP state.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10636
    /// @needwork = dda
    /// @endcode
    AraList< std::unique_ptr< ReversibleAction > > actions_{};

    /// @brief Storage for software clusters change info
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10637
    /// @needwork = dda
    /// @endcode
    SwClusterInfoVectorType swclChangeInfo_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_FILESYSTEMSWCLMANAGER_H_
