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
/// @file       network_handle.h
/// @brief      Network handle instance user interface class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=software
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NetworkHandle
/// @unit_description=Network handle instance user interface class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NETWORK_HANDLE_H_
#define _ARA_NETWORK_HANDLE_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/result.h>

#include "internal/common.h"

namespace ara {
namespace nm {

/// @brief Forward declaration
class NmIpc;

/// @brief Logical network handle class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100005
/// @trace_id_dd=DD_NM_00876
/// @needwork = ad
/// @endcode
class NetworkHandle final
{
public:
    /// @brief Network state type definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00877
    /// @needwork = dda
    /// @endcode
    enum class NetworkStateType : std::uint32_t
    {
        kNoCom   = 0,
        kFullCom = 1
    };

    /// @brief Notification receiver type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00967
    /// @needwork = dda
    /// @endcode
    using NetworkStateChangeNotifier = std::function< void(NetworkStateType const &) >;

    /// @brief Constructor
    /// @param specifier Descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00064
    /// @needwork = dda
    /// @endcode
    explicit NetworkHandle(ara::core::InstanceSpecifier const &specifier) noexcept;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00065
    /// @needwork = dda
    /// @endcode
    NetworkHandle() = delete;

    /// @brief Move constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00066
    /// @needwork = dda
    /// @endcode
    NetworkHandle(NetworkHandle &&other) noexcept = default;

    /// @brief Move assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00067
    /// @needwork = dda
    /// @endcode
    NetworkHandle &operator=(NetworkHandle &&other) &noexcept = default;

    /// @brief Copy constructor
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00068
    /// @needwork = dda
    /// @endcode
    NetworkHandle(NetworkHandle const &other) = delete;

    /// @brief Copy assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00069
    /// @needwork = dda
    /// @endcode
    NetworkHandle &operator=(NetworkHandle const &other) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00070
    /// @needwork = dda
    /// @endcode
    ~NetworkHandle() noexcept;

    /// @brief Get network state
    /// @return Network state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00071
    /// @needwork = dda
    /// @endcode
    ara::core::Result< NetworkStateType > GetNetworkState() const noexcept;

    /// @brief  Get network request state
    /// @return Network request state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00072
    /// @needwork = dda
    /// @endcode
    ara::core::Result< NetworkStateType > GetNetworkRequestedState() const noexcept;

    /// @brief Set network request state
    /// @param networkState Request state
    /// @return Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00073
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetNetworkRequestedState(NetworkStateType const networkState) noexcept;

    /// @brief Register network state change notification
    /// @param notifier Notification receiver
    /// @return Registration result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00074
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RegisterNetworkStateChangeNotifier(NetworkStateChangeNotifier notifier) noexcept;

    /// @brief Register network state change notification
    /// @param notifier Notification receiver
    /// @param executor Executor
    /// @return Registration result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00075
    /// @needwork = dda
    /// @endcode
    template < typename ExecutorT >
    ara::core::Result< void > RegisterNetworkStateChangeNotifier(NetworkStateChangeNotifier notifier,
                                                                 ExecutorT &&executor) noexcept;

    /// @brief Unregister network state change notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00076
    /// @needwork = dda
    /// @endcode
    void UnregisterNetworkStateChangeNotifier() noexcept;

    /// @brief Register network request state change notification
    /// @param notifier Notification receiver
    /// @return Registration result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00077
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RegisterNetworkRequestedStateChangeNotifier(NetworkStateChangeNotifier notifier) noexcept;

    /// @brief Register network request state change notification
    /// @param notifier Notification receiver
    /// @param executor Executor
    /// @return Registration result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00078
    /// @needwork = dda
    /// @endcode
    template < typename ExecutorT >
    ara::core::Result< void > RegisterNetworkRequestedStateChangeNotifier(NetworkStateChangeNotifier notifier,
                                                                          ExecutorT &&executor) noexcept;

    /// @brief Unregister network request state change notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00079
    /// @needwork = dda
    /// @endcode
    void UnregisterNetworkRequestedStateChangeNotifier() noexcept;

private:
    /// @brief Internal IPC communication handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00080
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< NmIpc > ipcPtr_;

    /// @brief Logical network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00081
    /// @needwork = dda
    /// @endcode
    ara::core::String lnName_;

    /// @brief networkStateChange notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00085
    /// @needwork = dda
    /// @endcode
    NetworkStateChangeNotifier networkStateChangeNotifier_;

    /// @brief networkStateChange notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00086
    /// @needwork = dda
    /// @endcode
    NetworkStateChangeNotifier networkReuestStateChangeNotifier_;
};

/// @brief Type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00904
/// @needwork = ad
/// @endcode
using NetworkStateType = NetworkHandle::NetworkStateType;

}  // namespace nm
}  // namespace ara
#endif