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
/// @file       ucm_status_storage.h
/// @brief      UCM status storage
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
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UcmStatusStorage
/// @unit_description=UCM status storage
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_UCM_STATUS_STORAGE_H_
#define ARA_UCM_PKGMGR_FSM_UCM_STATUS_STORAGE_H_

#include "package_manager_state.h"
#include "types/impl_type_actionexecutionInfotype.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ucm status storage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10092
/// @trace_id_dd=DD_UCM_10147
/// @needwork = ad
/// @endcode
class UcmStatusStorage : NonCopyNonMove
{
public:
    /// @brief CheckResultType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10093
    /// @trace_id_dd=DD_UCM_10148
    /// @needwork = ad
    /// @endcode
    enum class CheckResultType : int8_t
    {
        kUCMStatusMD5Consistent    = 0,   // md5 of ucm_status.json is equal with md5 in ucm_status.md5
        kUCMStatusMD5NotConsistent = -1,  // md5 of ucm_status.json is not equal with md5 in ucm_status.md5
        kUCMStatusAllFileEmpty     = -2,  // no ucm_status.json and ucm_status.md5
        kUCMStatusFileEmpty        = -3,  // no ucm_status.json
        kUCMStatusMD5FileEmpty     = -4   // no ucm_status.md5
    };

public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10094
    /// @trace_id_dd=DD_UCM_10149
    /// @needwork = ad
    /// @endcode
    UcmStatusStorage() = delete;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10095
    /// @trace_id_dd=DD_UCM_10150
    /// @needwork = ad
    /// @endcode
    ~UcmStatusStorage() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10096
    /// @trace_id_dd=DD_UCM_10151
    /// @needwork = ad
    /// @endcode
    UcmStatusStorage(UcmStatusStorage const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10097
    /// @trace_id_dd=DD_UCM_10152
    /// @needwork = ad
    /// @endcode
    UcmStatusStorage& operator=(UcmStatusStorage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10098
    /// @trace_id_dd=DD_UCM_10153
    /// @needwork = ad
    /// @endcode
    UcmStatusStorage(UcmStatusStorage&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10099
    /// @trace_id_dd=DD_UCM_10154
    /// @needwork = ad
    /// @endcode
    UcmStatusStorage& operator=(UcmStatusStorage&& other) = delete;

    /// @brief check consistency between ucm status file and ucm status md5 file
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10100
    /// @trace_id_dd=DD_UCM_10155
    /// @needwork = ad
    /// @endcode
    static CheckResultType CheckConsistency() noexcept;

    /// @brief remove ucm status md5 file
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10101
    /// @trace_id_dd=DD_UCM_10156
    /// @needwork = ad
    /// @endcode
    static void BeginSaveTransaction() noexcept;

    /// @brief update ucm status file
    /// @param st
    /// @param actionExecutionInfos
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_10157
    /// @needwork = ad
    /// @endcode
    static void SaveCurrentStatus(PackageManagerState const& st,
                                  AraList< ActionExecutionInfoType > const& actionExecutionInfos) noexcept;

    /// @brief generate ucm status md5 file
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10103
    /// @trace_id_dd=DD_UCM_10158
    /// @needwork = ad
    /// @endcode
    static void CommitSaveTransaction() noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_UCM_STATUS_STORAGE_H_
