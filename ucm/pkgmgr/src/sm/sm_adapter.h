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
/// @file       sm_adapter.h
/// @brief      sm adapter
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
/// @unit_name=UpdateRequestAdapter
/// @unit_description=sm adapter
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SM_SM_ADAPTER_H_
#define ARA_UCM_PKGMGR_SM_SM_ADAPTER_H_

#include "common/alias.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief sm adapter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10345
/// @trace_id_dd=DD_UCM_10849
/// @needwork = ad
/// @endcode
class UpdateRequestAdapter : NonCopyNonMove
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10346
    /// @trace_id_dd=DD_UCM_10850
    /// @needwork = ad
    /// @endcode
    UpdateRequestAdapter() noexcept = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10347
    /// @trace_id_dd=DD_UCM_10851
    /// @needwork = ad
    /// @endcode
    virtual ~UpdateRequestAdapter() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10348
    /// @trace_id_dd=DD_UCM_10852
    /// @needwork = ad
    /// @endcode
    UpdateRequestAdapter(UpdateRequestAdapter const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10349
    /// @trace_id_dd=DD_UCM_10853
    /// @needwork = ad
    /// @endcode
    UpdateRequestAdapter& operator=(UpdateRequestAdapter const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10350
    /// @trace_id_dd=DD_UCM_10854
    /// @needwork = ad
    /// @endcode
    UpdateRequestAdapter(UpdateRequestAdapter&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10351
    /// @trace_id_dd=DD_UCM_10855
    /// @needwork = ad
    /// @endcode
    UpdateRequestAdapter& operator=(UpdateRequestAdapter&& other) = delete;

    /// @brief init sm adapter
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10352
    /// @trace_id_dd=DD_UCM_10856
    /// @needwork = ad
    /// @endcode
    virtual bool Init() noexcept = 0;

    /// @brief start update session with sm
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10353
    /// @trace_id_dd=DD_UCM_10857
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid StartUpdateSession() noexcept = 0;

    /// @brief stop update session with sm
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10354
    /// @trace_id_dd=DD_UCM_10858
    /// @needwork = ad
    /// @endcode
    virtual void StopUpdateSession() noexcept = 0;

    /// @brief reset machine with sm
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10355
    /// @trace_id_dd=DD_UCM_10859
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid ResetMachine() noexcept = 0;

    /// @brief prepare update for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10356
    /// @trace_id_dd=DD_UCM_10860
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid PrepareUpdate(AraVectorString const& functionGroups) noexcept = 0;

    /// @brief verify update for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10357
    /// @trace_id_dd=DD_UCM_10861
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid VerifyUpdate(AraVectorString const& functionGroups) noexcept = 0;

    /// @brief prepare rollback for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10358
    /// @trace_id_dd=DD_UCM_10862
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid PrepareRollback(AraVectorString const& functionGroups) noexcept = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SM_SM_ADAPTER_H_
