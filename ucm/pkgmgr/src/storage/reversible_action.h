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
/// @file       reversible_action.h
/// @brief      reversible action
/// @details
/// @date       2022-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00017,RS_UCM_00005, RS_UCM_00012
/// @unit_name=ReversibleAction
/// @unit_description=Base class for all swcl actions
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_BASE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_BASE_ACTION_H_

#include <algorithm>
#include <chrono>
#include <iterator>
#include <utility>

#include "ara/ucm/pkgmgr/impl_type_activateoptiontype.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "ara/ucm/pkgmgr/impl_type_transferidtype.h"
#include "common/alias.h"
#include "parsing/software_package.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10268
/// @trace_id_dd=DD_UCM_10513
/// @needwork = dd
/// @endcode
constexpr const char8_t* const kAppVarDirName{"var"};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10268
/// @trace_id_dd=DD_UCM_10514
/// @needwork = dd
/// @endcode
constexpr const char8_t* const kEmptyVersion{"0.0.0"};

/// @brief Base class for all swcl actions.
///
/// An action may perform either installation, update or uninstall of a software package.
/// Subclasses of ReversibleAction implement actions
/// by moving files on a filesystem within a single partition.    --- Should refer to: not using two partitions
/// Regardless of the result of the execution the action can be undone
/// ？If execution fails, should not revert/commit, right? See allowCommitOrRevert_ below
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @uptrace={SWS_UCM_00024, 172b762f2b191a7eb048ce985f423291856573bf}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10266
/// @trace_id_dd=DD_UCM_10515
/// @needwork = ad
/// @endcode
class ReversibleAction
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10516
    /// @needwork = dda
    /// @endcode
    ReversibleAction() = delete;

    /// @brief Common constructor for all swcl-based actions.
    ///
    /// @param destinationPath The location, where the package is supposed to be located
    /// @param swclPath The location(without version), where the package is supposed to be located
    /// @param activateOptionType
    /// @param deltaPackageApplicableVersion
    /// @param swclName The input package to the action
    /// @param curSwclVersion The input package to the action
    /// @param swclVersion The input package to the action
    /// @param package The input package to the action
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10517
    /// @needwork = dda
    /// @endcode
    ReversibleAction(AraString destinationPath,
                     AraString swclPath,
                     ActivateOptionType const activateOptionType,
                     AraString deltaPackageApplicableVersion,
                     AraString swclName,
                     AraString curSwclVersion,
                     AraString swclVersion,
                     std::unique_ptr< SoftwarePackage > package)
        : targetPath_{std::move(destinationPath)}
        , swclPath_{std::move(swclPath)}
        , activateOptionType_{activateOptionType}
        , deltaPackageApplicableVersion_{std::move(deltaPackageApplicableVersion)}
        , swclName_{std::move(swclName)}
        , curSwclVersion_{std::move(curSwclVersion)}
        , swclVersion_{std::move(swclVersion)}
        , package_{std::move(package)}
    {
    }

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10518
    /// @needwork = dda
    /// @endcode
    virtual ~ReversibleAction() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10519
    /// @needwork = dda
    /// @endcode
    ReversibleAction(ReversibleAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10520
    /// @needwork = dda
    /// @endcode
    ReversibleAction& operator=(ReversibleAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10521
    /// @needwork = dda
    /// @endcode
    ReversibleAction(ReversibleAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10522
    /// @needwork = dda
    /// @endcode
    ReversibleAction& operator=(ReversibleAction&& other) = delete;

    /// @brief Execute the action defined by the provided package.
    ///
    /// @returns no value if execution was successful, else see UCMErrorDomainErrc
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10523
    /// @needwork = dda
    /// @endcode
    virtual AraResult< ResultType > Execute() = 0;

    /// @brief Revert modifications and cleanup temporary or obsolete artifacts created by the action.
    ///
    /// @returns no value if cleanup was successful, else see UCMErrorDomainErrc
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10524
    /// @needwork = dda
    /// @endcode
    virtual AraResult< ResultType > RevertChanges() = 0;

    /// @brief Commit modifications and cleanup temporary or obsolete artifacts created by the action.
    ///
    /// @returns no value if cleanup was successful, else see UCMErrorDomainErrc
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10525
    /// @needwork = dda
    /// @endcode
    virtual AraResult< ResultType > CommitChanges() = 0;

    /// @brief Merge var directory as to the action.
    ///
    /// @returns no value if cleanup was successful, else see UCMErrorDomainErrc
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10526
    /// @needwork = dda
    /// @endcode
    virtual AraResult< ResultType > MergeVar() { return AraResult< ResultType >{ResultType::kSuccessfull}; }

    /// @brief Get the ActionType
    /// @returns ActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10527
    /// @needwork = dda
    /// @endcode
    virtual ActionType GetActionType() = 0;

    /// @brief Get the ActivationOption.
    /// @return ActivateOptionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10528
    /// @needwork = dda
    /// @endcode
    virtual ActivateOptionType GetActivationOption() const noexcept { return activateOptionType_; }

    /// Getter that @returns a reference to the package associated with this action
    /// virtual const SoftwarePackage& GetSoftwarePackage() const { return *package_; }

    /// @brief Get the deltaPackageApplicableVersion.
    /// @return deltaPackageApplicableVersion
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10529
    /// @needwork = dda
    /// @endcode
    AraString GetdeltaPackageApplicableVersion() const { return deltaPackageApplicableVersion_; }

    /// @brief Get the swclName
    /// @return swclName
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10530
    /// @needwork = dda
    /// @endcode
    AraString GetSwclName() const { return swclName_; }

    /// @brief Get the curSwclVersion_
    /// @return curSwclVersion_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10531
    /// @needwork = dda
    /// @endcode
    AraString GetCurSwclVersion() const { return curSwclVersion_; }

    /// @brief Get the swclVersion_.
    ///
    /// @brief GetSwclVersion
    /// @return swclVersion_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10532
    /// @needwork = dda
    /// @endcode
    AraString GetSwclVersion() const { return swclVersion_; }

    /// @brief GetResolution
    /// @return resolution_
    /// @throws no
    /// @code{.isoft}
    /// Getter that @returns The resolution of the Action
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10533
    /// @needwork = dda
    /// @endcode
    virtual ResultType const& GetResolution() const noexcept { return resolution_; }

    /// @brief GetTimeStamp
    /// @return actionTimeStamp_
    /// @throws no
    /// @code{.isoft}
    /// Getter that @returns The timestamp converted to string when the Action has been performed
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10534
    /// @needwork = dda
    /// @endcode
    virtual std::uint64_t GetTimeStamp() const noexcept { return actionTimeStamp_; }

    /// @brief ShortInfo
    /// @return short info of action
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10535
    /// @needwork = dda
    /// @endcode
    AraString ShortInfo() const
    {
        return "name: " + swclName_ + ", curVer: " + curSwclVersion_ + ", swclVer: " + swclVersion_
               + ", targetPath: " + targetPath_;
    }

    /// @brief RecoverExecutionInfo
    /// @param actionResolution
    /// @param actionTimeStamp
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10536
    /// @needwork = dda
    /// @endcode
    virtual void RecoverExecutionInfo(ResultType actionResolution, std::uint64_t actionTimeStamp) noexcept
    {
        assert(0U == actionTimeStamp_);
        actionTimeStamp_     = actionTimeStamp;
        resolution_          = actionResolution;
        allowCommitOrRevert_ = true;
    }

protected:
    // Find the current version of the software cluster from the installation directory
    // isForRemove: Whether to search for the purpose of Remove
    /// @brief _GetCurrentVersion
    /// @param isForRemove
    /// @return current swcl version
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10537
    /// @needwork = dda
    /// @endcode
    AraString _GetCurrentVersion(bool const isForRemove = false) const
    {
        std::ignore = isForRemove;
        return curSwclVersion_;
        // log_.LogDebug() << "ReversibleAction::_GetCurrentVersion, begin with isForRemove:" << isForRemove;.

        // const String finalDestPathParentDirectory =.
        //     targetPath_.substr(0, targetPath_.find_last_of("/"));.

        // // Current version
        // AraString curVersion;.

        // // New version
        // AraString newVersion = swclVersion_;.
        // if (isForRemove) newVersion = "";.

        // // Get all subdirectories under finalDestPathParentDirectory (different versions of the software cluster)
        // ara::core::Vector<String> directories = fs_.GetSubdirectories(finalDestPathParentDirectory);.
        // for (const auto& dir : directories) {.
        //     const String version = dir.substr((targetPath_.find_last_of("/") + 1));.
        //     log_.LogDebug() << "ReversibleAction::_GetCurrentVersion, dir = " << dir << ", version = " << version;.
        //     if (0 != version.compare(newVersion)) {.
        //         // Get the current version
        //         curVersion = version;.
        //         break;.
        //     }.
        // }.

        // log_.LogDebug() << "ReversibleAction::_GetCurrentVersion, end with curVersion:" << curVersion;.
        // return curVersion;.
    }

    /// @brief CheckConsistency
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// Consistency check
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10538
    /// @trace_id_sr=RS_UCM_00012
    /// @needwork = dda
    /// @endcode
    bool _CheckConsistency() const;

protected:
    /// @brief The final, not temporary location of the SWCL, a full path to the directory INCLUDING a folder with
    /// the name of the SWCL and a subfolder with its version.
    ///
    /// A path like "/A/B/C/SWCL1/1.0.0" where the "SWCL1" is a name of the SWCL, "1.0.0" is the version of the SWCL and
    /// the "/A/B/C/" part is common for allSWCLs.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10539
    /// @needwork = dda
    /// @endcode
    AraString targetPath_{""};  // NOLINT

    /// @brief swclPath_
    /// @code{.isoft}
    /// the "/A/B/C/SWCL1/"
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10540
    /// @needwork = dda
    /// @endcode
    AraString swclPath_{""};  // NOLINT

    /// @brief activateOptionType
    /// @code{.isoft}
    /// Activation type
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10541
    /// @needwork = dda
    /// @endcode
    ActivateOptionType activateOptionType_{ActivateOptionType::kWaitForReboot};  // NOLINT

    /// @brief deltaPackageApplicableVersion
    /// @code{.isoft}
    /// Applicable version information for delta package
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10542
    /// @needwork = dda
    /// @endcode
    AraString deltaPackageApplicableVersion_{""};  // NOLINT

    /// @brief swclName
    /// @code{.isoft}
    /// Software set name
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10543
    /// @needwork = dda
    /// @endcode
    AraString swclName_{""};  // NOLINT

    /// @brief curSwclVersion_
    /// @code{.isoft}
    /// Current version of the software set
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10544
    /// @needwork = dda
    /// @endcode
    AraString curSwclVersion_{""};  // NOLINT

    /// @brief swclVersion_
    /// @code{.isoft}
    /// Software set version
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10545
    /// @needwork = dda
    /// @endcode
    AraString swclVersion_{""};  // NOLINT

    /// @brief Input package used in execute step
    /// Owning pointer is used for flexibility
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10546
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackage > package_{nullptr};  // NOLINT

    /// @brief Resolution of the Action
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10547
    /// @needwork = dda
    /// @endcode
    ResultType resolution_{ResultType::kFailed};  // NOLINT

    /// @brief point in time (in Unix time) when the action has been executed successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10548
    /// @needwork = dda
    /// @endcode
    std::uint64_t actionTimeStamp_{0U};  // NOLINT

    /// @brief allows to forbid the call to RevertChanges() and CommitChanges() if Execute() has not been
    /// successfully called on that instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10549
    /// @needwork = dda
    /// @endcode
    bool allowCommitOrRevert_{false};  // NOLINT

protected:
    /// @brief Clean up persistent data
    /// @brief GetActionType
    /// @param removingVersion Swcl Version to be removed.
    /// @param keptVersion Swcl Version to be kept.
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10550
    /// @trace_id_sr=SR_UCM_00017,RS_UCM_00005
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > _CleanUpPersistentData(AraString const& removingVersion, AraString const& keptVersion);
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_SWCL_BASE_ACTION_H_
