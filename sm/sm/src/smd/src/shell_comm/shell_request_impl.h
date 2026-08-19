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
/// @file       shell_request_impl.h
/// @brief      A implementation of ara::sm::skeleton::ShellRequestSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/ShellComm
/// @unit_name=ShellRequestImpl
/// @interface_level=uint
/// @unit_description=A implementation of ara::sm::skeleton::ShellRequestSkeleton.
/// @trace_id_sr=SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef SHELL_REQUEST_IML_H_
#define SHELL_REQUEST_IML_H_

#include <ara/log/logger.h>
#include <ara/sm/shellrequest_skeleton.h>

#include "event.h"

namespace ara {
namespace sm {
namespace shell_comm {

/// @brief A implementation of ara::sm::skeleton::ShellRequestSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_10001
/// @trace_id_ad=AD_SM_08120
/// @trace_id_dd=DD_SM_08449
/// @needwork = ad
/// @endcode
class ShellRequestImpl : public ara::sm::skeleton::ShellRequestSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::ShellRequestSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00597
    /// @trace_id_dd=DD_SM_00717
    /// @needwork = ad
    /// @endcode
    ~ShellRequestImpl() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The ShellRequestImpl instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00598
    /// @trace_id_dd=DD_SM_00718
    /// @needwork = ad
    /// @endcode
    ShellRequestImpl(ShellRequestImpl const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ShellRequestImpl instance to be copyed
    /// @return the assigned ShellRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00599
    /// @trace_id_dd=DD_SM_00719
    /// @needwork = ad
    /// @endcode
    ShellRequestImpl &operator=(ShellRequestImpl const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ShellRequestImpl instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00600
    /// @trace_id_dd=DD_SM_00720
    /// @needwork = ad
    /// @endcode
    ShellRequestImpl(ShellRequestImpl &&other) = default;

    /// @brief Move assignment function
    /// @param other The ShellRequestImpl instance to be moved
    /// @return the assigned ShellRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00601
    /// @trace_id_dd=DD_SM_00721
    /// @needwork = ad
    /// @endcode
    ShellRequestImpl &operator=(ShellRequestImpl &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00602
    /// @trace_id_dd=DD_SM_00722
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Set state machine state
    /// @param smName State machine name
    /// @param sMStateRequest State machine state request sequence number
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00603
    /// @trace_id_dd=DD_SM_00723
    /// @needwork = ad
    /// @endcode
    core::Future< void > SetSMState(core::String const &smName, std::uint32_t const &sMStateRequest) noexcept override;

    /// @brief Set function group state
    /// @param fgName Function group name
    /// @param fgState Function group state
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00604
    /// @trace_id_dd=DD_SM_00724
    /// @needwork = ad
    /// @endcode
    core::Future< void > SetFGState(core::String const &fgName, core::String const &fgState) noexcept override;

    /// @brief Get state machine state
    /// @param smName State machine name
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00605
    /// @trace_id_dd=DD_SM_00725
    /// @needwork = ad
    /// @endcode
    core::Future< ShellRequestImpl::GetSMStateOutput > GetSMState(core::String const &smName) noexcept override;

    /// @brief Get function group state
    /// @param fgName Function group name
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00606
    /// @trace_id_dd=DD_SM_00726
    /// @needwork = ad
    /// @endcode
    core::Future< ShellRequestImpl::GetFGStateOutput > GetFGState(core::String const &fgName) noexcept override;

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    core::Future< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > GetAllFunctionGroupsInfo() noexcept override;

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) noexcept;

private:
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08450
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08451
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"ShellRequestImpl context"}))};
};

}  // namespace shell_comm
}  // namespace sm
}  // namespace ara
#endif  // SHELL_REQUEST_IML_H_