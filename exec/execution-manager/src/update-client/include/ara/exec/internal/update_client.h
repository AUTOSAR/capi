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
/// @file       update_client.h
/// @brief      Update client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UpdateClient
/// @unit_name=UpdateClient
/// @unit_description=Used to update user software collections and manage their states.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_UPDATE_CLIENT_H_
#define ARA_EXEC_INTERNAL_UPDATE_CLIENT_H_

#include <ara/core/future.h>
#include <ara/core/vector.h>

#include <memory>

#include "ara/exec/internal/ums/swcl_info.h"

namespace ara {
namespace exec {
namespace internal {

/// @brief Update client class
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_EM_10007
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00709
/// @needwork = ad
/// @endcode
class UpdateClient  // PRQA S 5215
{
public:
 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00710
    /// @needwork = dda
    /// @endcode
    UpdateClient() noexcept;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00711
    /// @needwork = dda
    /// @endcode
    ~UpdateClient() noexcept;

 /// @brief Prohibit use of copy constructor
 /// @param other Other update client
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00712
    /// @needwork = dda
    /// @endcode
    UpdateClient(UpdateClient const& other) = delete;

 /// @brief Prohibit use of move constructor
 /// @param other Other update client
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00713
    /// @needwork = dda
    /// @endcode
    UpdateClient(UpdateClient&& other) = delete;

 /// @brief Prohibit use of copy assignment operator
 /// @param other Other update client
 /// @return New update client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00714
    /// @needwork = dda
    /// @endcode
    UpdateClient& operator=(UpdateClient const& other) = delete;

 /// @brief Prohibit use of move assignment operator
 /// @param other Other update client
 /// @return New update client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00715
    /// @needwork = dda
    /// @endcode
    UpdateClient& operator=(UpdateClient&& other) = delete;

 /// @brief User software cluster information list type definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using SwclInfoList = ara::core::Vector< ums::SwclInfo >;

 /// @brief Update user software cluster
 /// @param invalidSwcls Invalid software cluster list
 /// @param validSwcls Valid software cluster list
 /// @return Future used to get result
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00716
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > UpdateUserSwcls(SwclInfoList const& invalidSwcls,  // PRQA S 2024
                                              SwclInfoList const& validSwcls) const noexcept;

 /// @brief Get current user software cluster manifest name
 /// @return Future used to get result
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00717
    /// @needwork = dda
    /// @endcode
    ara::core::Future< ara::core::String > GetUserSwclManifest() const noexcept;  // PRQA S 2024

private:
 /// @brief Private implementation class declaration
    class PrivateImpl;

 /// @brief Private implementation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00718
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< PrivateImpl > privateImpl_;
};

}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_UPDATE_CLIENT_H_
