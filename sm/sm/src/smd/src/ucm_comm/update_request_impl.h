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
/// @file       update_request_impl.h
/// @brief      A implementation of ara::sm::skeleton::UpdateRequestSkeleton.
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/UCMComm
/// @unit_name=UpdateRequestImpl
/// @interface_level=uint
/// @unit_description=A implementation of ara::sm::skeleton::UpdateRequestSkeleton.
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @endcode
///
/// ================================================================

#ifndef INC_MSM_UPDATE_REQUESTS_H_
#define INC_MSM_UPDATE_REQUESTS_H_

#include <ara/log/logger.h>
#include <ara/sm/updaterequest_skeleton.h>

#include <functional>
#include <memory>
#include <string>

#include "event.h"

namespace ara {
namespace sm {
namespace ucm_comm {

/// @brief A implementation of ara::sm::skeleton::UpdateRequestSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @trace_id_ad=AD_SM_08118
/// @trace_id_dd=DD_SM_08442
/// @needwork = ad
/// @endcode
class UpdateRequestImpl : public ara::sm::skeleton::UpdateRequestSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::UpdateRequestSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00575
    /// @trace_id_dd=DD_SM_00691
    /// @needwork = ad
    /// @endcode
    ~UpdateRequestImpl() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The UpdateRequestImpl instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00576
    /// @trace_id_dd=DD_SM_00692
    /// @needwork = ad
    /// @endcode
    UpdateRequestImpl(UpdateRequestImpl const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The UpdateRequestImpl instance to be copyed
    /// @return the assigned UpdateRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00577
    /// @trace_id_dd=DD_SM_00693
    /// @needwork = ad
    /// @endcode
    UpdateRequestImpl &operator=(UpdateRequestImpl const &other) = delete;

    /// @brief Move constructor function
    /// @param other The UpdateRequestImpl instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00578
    /// @trace_id_dd=DD_SM_00694
    /// @needwork = ad
    /// @endcode
    UpdateRequestImpl(UpdateRequestImpl &&other) = default;

    /// @brief Move assignment function
    /// @param other The UpdateRequestImpl instance to be moved
    /// @return the assigned UpdateRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00579
    /// @trace_id_dd=DD_SM_00695
    /// @needwork = ad
    /// @endcode
    UpdateRequestImpl &operator=(UpdateRequestImpl &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00580
    /// @trace_id_dd=DD_SM_00696
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Method called, when a ResetMachine request is issued.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04005
    /// @trace_id_ad=AD_SM_00581
    /// @trace_id_dd=DD_SM_00697
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > ResetMachine() noexcept override;

    /// @brief  Method called, when a StartUpdateSession request is issued.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04001
    /// @trace_id_ad=AD_SM_00582
    /// @trace_id_dd=DD_SM_00698
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > StartUpdateSession() noexcept override;

    /// @brief Method called, when a PrepareUpdate request is issued.
    /// @param[in] functionGroupList  The list of FunctionGroups within the SoftwareCluster to be prepared to be updated.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04007
    /// @trace_id_ad=AD_SM_00583
    /// @trace_id_dd=DD_SM_00699
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > PrepareUpdate(
        ::ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept override;

    /// @brief Method called, when a VerifyUpdate request is issued.
    /// @param[in] functionGroupList The list of FunctionGroups within the SoftwareCluster to be verified.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04008
    /// @trace_id_ad=AD_SM_00584
    /// @trace_id_dd=DD_SM_00700
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > VerifyUpdate(
        ::ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept override;

    /// @brief Method called, when a PrepareRollback request is issued.
    /// @param[in] functionGroupList  The list of FunctionGroups within the SoftwareCluster to be prepared to roll back.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04009
    /// @trace_id_ad=AD_SM_00585
    /// @trace_id_dd=DD_SM_00701
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > PrepareRollback(
        ::ara::sm::common::UcmFunctionGroupListInternal const &functionGroupList) noexcept override;

    /// @brief Method called, when a StopUpdateSession request is issued.
    /// @return ara::com::Future  Holds no payload.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_04006
    /// @trace_id_ad=AD_SM_00586
    /// @trace_id_dd=DD_SM_00702
    /// @needwork = ad
    /// @endcode
    void StopUpdateSession() noexcept override;

private:
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08443
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08444
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"UpdateRequestImpl context"}))};
};

}  // namespace ucm_comm
}  // namespace sm
}  // namespace ara

#endif  // INC_MSM_UPDATE_REQUESTS_H_
