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
/// @file       sm_service.h
/// @brief      sm service
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
/// @unit_name=UpdateRequestService
/// @unit_description=sm service
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SM_SM_SERVICE_H_
#define ARA_UCM_PKGMGR_SM_SM_SERVICE_H_

#include <memory>

#include "ara/sm/updaterequest_proxy.h"
#include "common/alias.h"
#include "sm_adapter.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief UpdateRequestProxy
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using UpdateRequestProxy = ara::sm::proxy::UpdateRequestProxy;
/// @brief ServiceProxySPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ServiceProxySPtr = std::shared_ptr< UpdateRequestProxy >;

/// @brief sm service
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10359
/// @trace_id_dd=DD_UCM_10863
/// @needwork = ad
/// @endcode
class UpdateRequestService : public UpdateRequestAdapter
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10360
    /// @trace_id_dd=DD_UCM_10864
    /// @needwork = ad
    /// @endcode
    UpdateRequestService() noexcept = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10361
    /// @trace_id_dd=DD_UCM_10865
    /// @needwork = ad
    /// @endcode
    ~UpdateRequestService() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10362
    /// @trace_id_dd=DD_UCM_10866
    /// @needwork = ad
    /// @endcode
    UpdateRequestService(UpdateRequestService const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10363
    /// @trace_id_dd=DD_UCM_10867
    /// @needwork = ad
    /// @endcode
    UpdateRequestService& operator=(UpdateRequestService const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10364
    /// @trace_id_dd=DD_UCM_10868
    /// @needwork = ad
    /// @endcode
    UpdateRequestService(UpdateRequestService&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10365
    /// @trace_id_dd=DD_UCM_10869
    /// @needwork = ad
    /// @endcode
    UpdateRequestService& operator=(UpdateRequestService&& other) = delete;

    /// @brief SetMaxWaitTime
    /// @param maxWaitTime
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10366
    /// @trace_id_dd=DD_UCM_10870
    /// @needwork = ad
    /// @endcode
    void SetMaxWaitTime(std::int32_t const maxWaitTime) noexcept;

    /// @brief init sm adapter
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10367
    /// @trace_id_dd=DD_UCM_10871
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;
    /// @brief AsyncInit
    /// @param cb
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10368
    /// @trace_id_dd=DD_UCM_10872
    /// @needwork = ad
    /// @endcode
    void AsyncInit(FunctionVoid&& cb) noexcept;

    /// @brief start update session with sm
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10369
    /// @trace_id_dd=DD_UCM_10873
    /// @needwork = ad
    /// @endcode
    AraFutureVoid StartUpdateSession() noexcept override;
    /// @brief stop update session with sm
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10370
    /// @trace_id_dd=DD_UCM_10874
    /// @needwork = ad
    /// @endcode
    void StopUpdateSession() noexcept override;
    /// @brief reset machine with sm
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10371
    /// @trace_id_dd=DD_UCM_10875
    /// @needwork = ad
    /// @endcode
    AraFutureVoid ResetMachine() noexcept override;

    /// @brief prepare update for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10372
    /// @trace_id_dd=DD_UCM_10876
    /// @needwork = ad
    /// @endcode
    AraFutureVoid PrepareUpdate(AraVectorString const& functionGroups) noexcept override;
    /// @brief verify update for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10373
    /// @trace_id_dd=DD_UCM_10877
    /// @needwork = ad
    /// @endcode
    AraFutureVoid VerifyUpdate(AraVectorString const& functionGroups) noexcept override;
    /// @brief prepare rollback for function group list with sm
    /// @param functionGroups
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10374
    /// @trace_id_dd=DD_UCM_10878
    /// @needwork = ad
    /// @endcode
    AraFutureVoid PrepareRollback(AraVectorString const& functionGroups) noexcept override;

    /// @brief RetryResetMachine
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10375
    /// @trace_id_dd=DD_UCM_10879
    /// @needwork = ad
    /// @endcode
    bool RetryResetMachine() noexcept;
    /// @brief RetryPrepareUpdate
    /// @param functionGroups
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10376
    /// @trace_id_dd=DD_UCM_10880
    /// @needwork = ad
    /// @endcode
    bool RetryPrepareUpdate(AraVectorString const& functionGroups) noexcept;
    /// @brief RetryVerifyUpdate
    /// @param functionGroups
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10377
    /// @trace_id_dd=DD_UCM_10881
    /// @needwork = ad
    /// @endcode
    bool RetryVerifyUpdate(AraVectorString const& functionGroups) noexcept;
    /// @brief RetryPrepareRollback
    /// @param functionGroups
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10378
    /// @trace_id_dd=DD_UCM_10882
    /// @needwork = ad
    /// @endcode
    bool RetryPrepareRollback(AraVectorString const& functionGroups) noexcept;

private:
    /// @brief retry update method
    /// @param method
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10883
    /// @needwork = dda
    /// @endcode
    bool _retryUpdateMethod(std::function< AraFutureVoid() >&& method) const noexcept;

    /// @brief callback after async find sm service
    /// @param handles
    /// @param handler
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10884
    /// @needwork = dda
    /// @endcode
    void _genConfiguredUpdateRequestService(ara::com::ServiceHandleContainer< UpdateRequestProxy::HandleType > handles,
                                            ara::com::FindServiceHandle const& handler) noexcept;

private:
    /// @brief real sm service proxy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10885
    /// @needwork = dda
    /// @endcode
    ServiceProxySPtr service_{nullptr};
    /// @brief lock when init service_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10886
    /// @needwork = dda
    /// @endcode
    std::mutex serviceLock_{};
    /// @brief max wait time when call update method, ms
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10887
    /// @needwork = dda
    /// @endcode
    std::int32_t maxWaitTime_{0};
    /// @brief retry count after failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10888
    /// @needwork = dda
    /// @endcode
    std::int32_t retryCount_{kInt32_10};
    /// @brief retry interval after failure, ms
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10889
    /// @needwork = dda
    /// @endcode
    std::int32_t retryInterval_{kInt32_100};
    /// @brief callback for business
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10890
    /// @needwork = dda
    /// @endcode
    FunctionVoid businessCallback_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SM_SM_SERVICE_H_
