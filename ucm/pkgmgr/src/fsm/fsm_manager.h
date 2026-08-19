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
/// @brief      FSM Manager header
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00021,SR_UCM_00007,SR_UCM_00030
/// @unit_name=FsmManager
/// @unit_description=FSM Manager header
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_FSM_MANAGER_H_
#define ARA_UCM_PKGMGR_FSM_FSM_MANAGER_H_

#include "ab_partition.h"
#include "common/alias.h"
#include "data/software_package_manager.h"
#include "executors/activate_executor.h"
#include "executors/cleanup_executor.h"
#include "executors/exception_cleanup_executor.h"
#include "executors/exception_rollback_executor.h"
#include "executors/help_executor.h"
#include "executors/process_executor.h"
#include "executors/rollback_executor.h"
#include "executors/verify_executor.h"
#include "package_manager_state.h"
#include "sm/sm_service.h"
#include "storage/filesystem_swcl_manager.h"
#include "types/impl_type_actionexecutionInfotype.h"
#include "util/locked_ptr_wrapper.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief fsm manager
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10104
/// @trace_id_dd=DD_UCM_10159
/// @needwork = ad
/// @endcode
class FsmManager
{
public:
    /// @brief destructor
    /// @param updateServiceInterfaceField
    /// @param swpkgManagerA
    /// @param swclManagerA
    /// @param smService
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10105
    /// @trace_id_dd=DD_UCM_10160
    /// @needwork = ad
    /// @endcode
    FsmManager(std::function< void(std::underlying_type< PackageManagerStatusType >::type const& newStatus) >
                   updateServiceInterfaceField,
               SoftwarePackageManager& swpkgManagerA,
               FileSystemSWCLManager& swclManagerA,
               std::unique_ptr< UpdateRequestService > smService) noexcept;
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10106
    /// @trace_id_dd=DD_UCM_10161
    /// @needwork = ad
    /// @endcode
    FsmManager() = delete;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10107
    /// @trace_id_dd=DD_UCM_10162
    /// @needwork = ad
    /// @endcode
    ~FsmManager() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10108
    /// @trace_id_dd=DD_UCM_10163
    /// @needwork = ad
    /// @endcode
    FsmManager(FsmManager const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10109
    /// @trace_id_dd=DD_UCM_10164
    /// @needwork = ad
    /// @endcode
    FsmManager& operator=(FsmManager const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10110
    /// @trace_id_dd=DD_UCM_10165
    /// @needwork = ad
    /// @endcode
    FsmManager(FsmManager&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10111
    /// @trace_id_dd=DD_UCM_10166
    /// @needwork = ad
    /// @endcode
    FsmManager& operator=(FsmManager&& other) = delete;

    /// @brief Activate the processed components.
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10112
    /// @trace_id_dd=DD_UCM_10167
    /// @trace_id_sr=SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00021,SR_UCM_00007,SR_UCM_00030
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Activate() const noexcept;

    /// @brief Finish the processing for the current set of
    /// processed Software Packages.
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10113
    /// @trace_id_dd=DD_UCM_10168
    /// @trace_id_sr=SR_UCM_00015
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Finish() const noexcept;

    /// @brief Process transferred Software Package.
    /// @param id TransferId
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10114
    /// @trace_id_dd=DD_UCM_10169
    /// @trace_id_sr=SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00026
    /// @needwork = ad
    /// @endcode
    AraFutureVoid ProcessSwPackage(TransferIdType const& id) const noexcept;

    /// @brief Abort an ongoing processing of a Software Package.
    /// @param id TransferId
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10115
    /// @trace_id_dd=DD_UCM_10170
    /// @trace_id_sr=SR_UCM_00020
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Cancel(TransferIdType const& id) const noexcept;

    /// @brief Revert the changes done by processing (ProcessSwPackage) of one
    /// or several software packages.
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10116
    /// @trace_id_dd=DD_UCM_10171
    /// @trace_id_sr=SR_UCM_00020
    /// @needwork = ad
    /// @endcode
    AraFutureVoid RevertProcessedSwPackages() const noexcept;

    /// @brief Rollback the system to the state
    /// before the packages were processed.
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10117
    /// @trace_id_dd=DD_UCM_10172
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Rollback() const noexcept;

    /// @brief Verify all the software clusters were updated.
    /// @throws no
    /// @return future with result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10118
    /// @trace_id_dd=DD_UCM_10173
    /// @needwork = ad
    /// @endcode
    void Verify() const noexcept;  //not used //mydel////////////////////

    /// @brief Done
    /// @throws no
    /// @code{.isoft}
    /// Internal messages
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10119
    /// @trace_id_dd=DD_UCM_10174
    /// @needwork = ad
    /// @endcode
    void Done() const noexcept;
    /// @brief DoneWithoutLock
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10120
    /// @trace_id_dd=DD_UCM_10175
    /// @needwork = ad
    /// @endcode
    void DoneWithoutLock() const noexcept;
    /// @brief OnProcessSwPackageSucceeded
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10121
    /// @trace_id_dd=DD_UCM_10176
    /// @needwork = ad
    /// @endcode
    void OnProcessSwPackageSucceeded() const noexcept;
    /// @brief OnProcessSwPackageFailed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10122
    /// @trace_id_dd=DD_UCM_10177
    /// @needwork = ad
    /// @endcode
    void OnProcessSwPackageFailed() const noexcept;
    /// @brief OnSuccess
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10123
    /// @trace_id_dd=DD_UCM_10178
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnSuccess() const noexcept;
    /// @brief OnSuccessWithoutLock
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10124
    /// @trace_id_dd=DD_UCM_10179
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnSuccessWithoutLock() const noexcept;
    /// @brief OnFailure
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10125
    /// @trace_id_dd=DD_UCM_10180
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnFailure() const noexcept;
    /// @brief OnFailureWithoutLock
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10126
    /// @trace_id_dd=DD_UCM_10181
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnFailureWithoutLock() const noexcept;

    /// @brief ContinueIfNecessary
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10127
    /// @trace_id_dd=DD_UCM_10182
    /// @needwork = ad
    /// @endcode
    void ContinueIfNecessary() noexcept;

    /// @brief Switch
    /// @param status PackageManagerStatusType
    /// @param cleanupAction FinalActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10128
    /// @trace_id_dd=DD_UCM_10183
    /// @needwork = ad
    /// @endcode
    void Switch(PackageManagerStatusType const status,
                FinalActionType const cleanupAction = FinalActionType::kCommit) const noexcept;
    /// @brief SwitchWithoutLock
    /// @param status PackageManagerStatusType
    /// @param cleanupAction FinalActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10129
    /// @trace_id_dd=DD_UCM_10184
    /// @needwork = ad
    /// @endcode
    void SwitchWithoutLock(PackageManagerStatusType const status,
                           FinalActionType const cleanupAction = FinalActionType::kCommit) const noexcept;

    /// @brief GetFinalActionType
    /// @throws no
    /// @return FinalActionType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10130
    /// @trace_id_dd=DD_UCM_10185
    /// @needwork = ad
    /// @endcode
    FinalActionType GetFinalActionType() const noexcept;

private:
    /// @brief _initState
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10186
    /// @needwork = dda
    /// @endcode
    void _initState() noexcept;

    /// @brief _recoverState
    /// @throws no
    /// @return PackageManagerState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10187
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< PackageManagerState > _recoverState() noexcept;

    // The following one is a named part of a contructor in fact
    /// @brief CreateState
    /// @param status PackageManagerStatusType
    /// @param finalActionIfCleaningUp FinalActionType
    /// @throws no
    /// @return PackageManagerState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10188
    /// @needwork = dda
    /// @endcode
    static std::unique_ptr< PackageManagerState > CreateState(PackageManagerStatusType const status,
                                                              FinalActionType finalActionIfCleaningUp) noexcept;

    /// @brief _onSwitchNewState
    /// @param st PackageManagerState
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10189
    /// @needwork = dda
    /// @endcode
    void _onSwitchNewState(PackageManagerState const& st) noexcept;

    /// @brief updateServiceInterfaceField_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10190
    /// @needwork = dda
    /// @endcode
    std::function< void(std::underlying_type< PackageManagerStatusType >::type const& newStatus) >
        updateServiceInterfaceField_{};

public:
    /// @brief SoftwarePackageManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10191
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManager&
        swpkgManager;  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief FileSystemSWCLManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10192
    /// @needwork = dda
    /// @endcode
    FileSystemSWCLManager&
        swclManager;  // NOLINT : The correct solution is to use private members and provide Get/Set functions

    /// @brief UpdateRequestService
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10193
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< UpdateRequestService > pSM{
        nullptr};  // NOLINT : The correct solution is to use private members and provide Get/Set functions

    /// @brief HelpExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10194
    /// @needwork = dda
    /// @endcode
    HelpExecutor
        mHelpExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief ProcessExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10195
    /// @needwork = dda
    /// @endcode
    ProcessExecutor
        mProcessExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief ActivateExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10196
    /// @needwork = dda
    /// @endcode
    ActivateExecutor
        mActivateExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief VerifyExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10197
    /// @needwork = dda
    /// @endcode
    VerifyExecutor
        mVerifyExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief CleanupExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10198
    /// @needwork = dda
    /// @endcode
    CleanupExecutor
        mCleanupExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief ABPartition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10199
    /// @needwork = dda
    /// @endcode
    RollbackExecutor
        mRollbackExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief ExceptionRollbackExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10200
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor
        mExceptionRollbackExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions
    /// @brief ExceptionCleanupExecutor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10201
    /// @needwork = dda
    /// @endcode
    ExceptionCleanupExecutor
        mExceptionCleanupExecutor{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions

    /// @brief ABPartition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10202
    /// @needwork = dda
    /// @endcode
    ABPartition
        mABPartition{};  // NOLINT : The correct solution is to use private members and provide Get/Set functions

private:
    /// @brief Pointer wrapper for package manager state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10203
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< LockedPtrWrapper< PackageManagerState > > state_{nullptr};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_FSM_MANAGER_H_
