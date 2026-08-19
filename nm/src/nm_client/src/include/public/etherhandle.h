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
/// @file       etherhandle.h
/// @brief      State machine instance user interface class
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
/// @unit_name=EtherHandle
/// @unit_description=State machine instance user interface class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NETWORK_ETHERHANDLE_H_
#define _ARA_NETWORK_ETHERHANDLE_H_

#include <ara/core/result.h>
#include <ara/core/string.h>

#include <cstdint>

namespace ara {
namespace nm {

/// @brief Forward declaration
class NmIpc;

/// @brief Ethernet instance operation class, non-standard
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100011
/// @trace_id_dd=DD_NM_00883
/// @needwork = ad
/// @endcode
class EtherHandle final
{
public:
    /// @brief  State machine state, consistent with daemon, starting from 2
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00884
    /// @needwork = dda
    /// @endcode
    enum class SMState : std::uint32_t
    {
        kBusSleep           = 2U,
        kPrepareBusSleep    = 3U,
        kRepeatMessageState = 4U,
        kNormalOperation    = 5U,
        kReadySleep         = 6U
    };

    /// @brief NM message send/receive control
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00885
    /// @needwork = dda
    /// @endcode
    enum class MessageCtrlType : std::uint8_t
    {
        kEnableAll     = 0U,
        kEnableReceive = 1U,
        kEnableSend    = 2U,
        kDisableAll    = 3U
    };

    /// @brief State machine change notification receiver
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00905
    /// @needwork = dda
    /// @endcode
    using EtherStateChangeNotifier = std::function< void(SMState const &) >;

    /// @brief External PN change notification receiver
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00906
    /// @needwork = dda
    /// @endcode
    using ExternalPnNotifier = std::function< void(ara::core::String const &) >;

    /// @brief Node detection result notifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00907
    /// @needwork = dda
    /// @endcode
    using DetectNodeResultNotifier = std::function< void(ara::core::String const &) >;

    /// @brief Constructor
    /// @param ipAddr IP address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00097
    /// @needwork = dda
    /// @endcode
    explicit EtherHandle(ara::core::String const &ipAddr) noexcept;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00098
    /// @needwork = dda
    /// @endcode
    EtherHandle() = default;

    /// @brief Move constructor
    /// @param other Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00099
    /// @needwork = dda
    /// @endcode
    EtherHandle(EtherHandle &&other) noexcept = default;

    /// @brief Move assignment
    /// @param other
    /// @return Reference
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00100
    /// @needwork = dda
    /// @endcode
    EtherHandle &operator=(EtherHandle &&other) &noexcept = default;

    /// @brief Constructor
    /// @param other Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00101
    /// @needwork = dda
    /// @endcode
    EtherHandle(EtherHandle const &other) = delete;

    /// @brief Assignment
    /// @param other Instance
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00102
    /// @needwork = dda
    /// @endcode
    EtherHandle &operator=(EtherHandle const &other) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00103
    /// @needwork = dda
    /// @endcode
    ~EtherHandle() noexcept;

    /// @brief Get state machine state
    /// @return State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00104
    /// @needwork = dda
    /// @endcode
    ara::core::Result< SMState > GetEtherStateMachineState() const noexcept;

    /// @brief  Set state machine state
    /// @param state State machine state
    /// @return Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00105
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetEtherStateMachineSate(SMState const state) noexcept;

    /// @brief Request node detection
    /// @param notifier Notifier
    /// @return Request result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00106
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RequestDetectNode(DetectNodeResultNotifier const &notifier) noexcept;

    /// @brief Wakeup notification
    /// @return Notification result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00107
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > NotifyWakeup() noexcept;

    /// @brief Set message control type
    /// @param type Message control type
    /// @return Setting result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00108
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetMessageCtrlType(MessageCtrlType const type) noexcept;

    /// @brief Get message control type
    /// @return Message control type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00109
    /// @needwork = dda
    /// @endcode
    ara::core::Result< MessageCtrlType > GetMessageCtrlType() noexcept;

    /// @brief Get initialization status
    /// @return Initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00110
    /// @needwork = dda
    /// @endcode
    ara::core::Result< std::uint32_t > GetInitState() noexcept;

    /// @brief Get external request PN list
    /// @return External request PN list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00111
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::String > GetExternalPnRequestList() noexcept;

    /// @brief Register state machine state change notifier
    /// @param notifier Notifier
    /// @return Whether registration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00112
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RegisterEtherStateChangeNotifier(EtherStateChangeNotifier const &notifier) noexcept;

    /// @brief Unregister state machine state change notifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00113
    /// @needwork = dda
    /// @endcode
    void UnregisterEtherStateChangeNotifier() noexcept;

    /// @brief Register external PN request notifier
    /// @param notifier Notifier
    /// @return Whether registration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00114
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RegisterExternalPnRequestNotifier(ExternalPnNotifier const &notifier) noexcept;

    /// @brief Unregister external PN request notifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00115
    /// @needwork = dda
    /// @endcode
    void UnregisterExternalPnRequestNotifier() noexcept;

    /// @brief Register external PN release notifier
    /// @param notifier Notifier
    /// @return Whether registration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00116
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > RegisterExternalPnReleaseNotifier(ExternalPnNotifier const &notifier) noexcept;

    /// @brief Unregister external PN release notifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00117
    /// @needwork = dda
    /// @endcode
    void UnregisterExternalPnReleaseNotifier() noexcept;

private:
    /// @brief Internal IPC communication handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00118
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< NmIpc > ipcPtr_{nullptr};

    /// @brief Ethernet instance IP address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00119
    /// @needwork = dda
    /// @endcode
    ara::core::String ipAddr_{};

    /// @brief State change notification registration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00132
    /// @needwork = dda
    /// @endcode
    EtherStateChangeNotifier stateChangeNotifier_;

    /// @brief External PN request registration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00129
    /// @needwork = dda
    /// @endcode
    ExternalPnNotifier pnRequestNotifier_;

    /// @brief External PN unregister registration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00131
    /// @needwork = dda
    /// @endcode
    ExternalPnNotifier pnReleaseNotifier_;

    /// @brief Node detection result notification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00133
    /// @needwork = dda
    /// @endcode
    DetectNodeResultNotifier nodeResultnotifier_;
};

/// @brief Type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00926
/// @needwork = ad
/// @endcode
using SMState = EtherHandle::SMState;

/// @brief Type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00927
/// @needwork = ad
/// @endcode
using MessageCtrlType = EtherHandle::MessageCtrlType;

}  // namespace nm
}  // namespace ara
#endif