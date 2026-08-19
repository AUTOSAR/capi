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
/// @file       nmipc.h
/// @brief      IPC communication management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmIpc
/// @unit_description=IPC communication management
/// @endcode
///
/// ================================================================

#ifndef INTERNAL_NM_NMIPC_H
#define INTERNAL_NM_NMIPC_H
#include <ara/log/logger.h>

#include "../../../nmd/src/common/common.h"
#include "../../../nmd/src/common/ipccommand.h"
#include "ara/core/map.h"
#include "ara/core/string_view.h"
#include "common.h"
#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/client.h"
#include "isoft/ipccpp/packet.h"
#include "isoft/ipccpp/utility.h"
#include "isoft/naicpp/global_evloop.h"

namespace ara {
namespace nm {

/// @brief Log output interface
/// @returns logger instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00871
/// @needwork = ad
/// @endcode
static inline ara::log::Logger &NmLogger() noexcept
{
    static ara::log::Logger &s_Logger{ara::log::CreateLogger(std::move(ara::core::StringView("#NML")),
                                                             std::move(ara::core::StringView("Network management lib")),
                                                             ara::log::LogLevel::kVerbose)};
    return s_Logger;
}

/// @brief kNmIpcTimeout const declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100031
/// @trace_id_dd=DD_NM_00981
/// @needwork = ad
/// @endcode
std::int32_t const kNmIpcTimeout{3000};

/// @brief NmChar
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00903
/// @needwork = ad
/// @endcode
using NmChar = char;

/// @brief IPC communication management class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00872
/// @needwork = ad
/// @endcode
class NmIpc final
{
public:
    /// @brief Notification callback type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00909
    /// @needwork = dda
    /// @endcode
    using EtherNotifier = std::function< void(ara::core::String const &) >;

    /// @brief Notification callback type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00910
    /// @needwork = dda
    /// @endcode
    using EtherNotifierExecutor = std::function< void(EtherNotifier const &, ara::core::String const &) >;

    /// @brief Ethernet notification receiver type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00911
    /// @needwork = dda
    /// @endcode
    using IpcEtherNotifierPair = std::pair< EtherNotifier, EtherNotifierExecutor >;

    /// @brief Logical network notification receiver type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00912
    /// @needwork = dda
    /// @endcode
    using IpcChangeNotifier = std::function< void(std::uint32_t const) >;

    /// @brief Logical network notification executor type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00913
    /// @needwork = dda
    /// @endcode
    using IpcChangeNotifierExecutor = std::function< void(IpcChangeNotifier const &, std::uint32_t const) >;

    /// @brief Logical network notification receiver type declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00914
    /// @needwork = dda
    /// @endcode
    using IpcLnNotifierPair = std::pair< IpcChangeNotifier, IpcChangeNotifierExecutor >;

    /// @brief Copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00017
    /// @needwork = dda
    /// @endcode
    NmIpc(NmIpc const &other) = default;

    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00018
    /// @needwork = dda
    /// @endcode
    NmIpc &operator=(NmIpc const &other) = default;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00019
    /// @needwork = dda
    /// @endcode
    NmIpc() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00020
    /// @needwork = dda
    /// @endcode
    ~NmIpc() noexcept;

    /// @brief Move constructor
    /// @param other Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00021
    /// @needwork = dda
    /// @endcode
    NmIpc(NmIpc &&other) noexcept = default;

    /// @brief Move assignment
    /// @param other
    /// @return Reference
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00022
    /// @needwork = dda
    /// @endcode
    NmIpc &operator=(NmIpc &&other) &noexcept = default;

    /// @brief Get IPC handle
    /// @return IPC handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00023
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< NmIpc > GetNmIpcProHandler() noexcept
    {
        if (nullptr == s_IpcClientHandler_) {
            s_IpcClientHandler_ = std::make_shared< NmIpc >();
            if (0 != s_IpcClientHandler_->_init()) {
                s_IpcClientHandler_ = nullptr;
            }
        }
        return s_IpcClientHandler_;
    }

    /// @brief Close IPC handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00024
    /// @needwork = dda
    /// @endcode
    static void CloseNmIpcProHandler() noexcept
    {
        if (nullptr != s_IpcClientHandler_) {
            if (1 == s_IpcClientHandler_.use_count()) {
                s_IpcClientHandler_ = nullptr;
            }
        }
    }

    /// @brief IPC synchronous request
    /// @param cmd Request JSON string
    /// @param serverCode Server return code
    /// @return IPC communication result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00025
    /// @needwork = dda
    /// @endcode
    std::int32_t DoIpcSyncRequest(ara::core::String const &cmd, std::uint32_t &serverCode) noexcept;

    /// @brief IPC synchronous request
    /// @param cmd Request JSON string
    /// @param serverStr Server return string
    /// @return IPC communication result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00026
    /// @needwork = dda
    /// @endcode
    std::int32_t DoIpcSyncRequest(ara::core::String const &cmd, ara::core::String &serverStr) noexcept;

    /// @brief Register logical network notifier
    /// @param instance Logical network instance
    /// @param cmd Listen type
    /// @param notifier Notification handler
    /// @return Whether registration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00027
    /// @needwork = dda
    /// @endcode
    std::int32_t RegisterNotification(ara::core::String const &instance,
                                      internal::IpcCommand const cmd,
                                      IpcLnNotifierPair const &notifier) noexcept;

    /// @brief Register Ethernet instance notifier
    /// @param instance Ethernet instance
    /// @param cmd Listen type
    /// @param notifier Notification handler
    /// @return Whether registration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00028
    /// @needwork = dda
    /// @endcode
    std::int32_t RegisterNotification(ara::core::String const &instance,
                                      internal::IpcCommand const cmd,
                                      IpcEtherNotifierPair const &notifier) noexcept;

    /// @brief Unregister notifier
    /// @param instance Instance
    /// @param cmd Listen type
    /// @return Whether unregistration succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00029
    /// @needwork = dda
    /// @endcode
    std::int32_t UnregisterNotification(ara::core::String const &instance, internal::IpcCommand const cmd) noexcept;

private:
    /// @brief Initialize
    /// @return Initialization result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00030
    /// @needwork = dda
    /// @endcode
    std::int32_t _init() noexcept;

    /// @brief Release resources, called in destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00031
    /// @needwork = dda
    /// @endcode
    void _destroy() noexcept;

    /// @brief Handle control commands from daemon
    /// @param  jsonstr JSON string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00032
    /// @needwork = dda
    /// @endcode
    void _handledaemoncmd(ara::core::String const &jsonstr) noexcept;

    /// @brief IPC asynchronous send callback function
    /// @param  context - User parameter
    /// @param  status - IPC communication status
    /// @param  responsePacket - the packet responsed from peer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00033
    /// @needwork = dda
    /// @endcode
    static void IPCClientSendAsyncHandler(void *const context,
                                          isoft::ipc::IPCClientHandlerStatus const status,
                                          isoft::ipc::IPCPacket *const responsePacket) noexcept;

    /// @brief Send asynchronous IPC notification to server
    /// @param instance Ethernet instance
    /// @param cmd Listen type
    /// @return Processing result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00034
    /// @needwork = dda
    /// @endcode
    std::int32_t _sendAsyncIpcNotification(ara::core::String const &instance, internal::IpcCommand const cmd) noexcept;

private:
    /// @brief ipcClient_ - IPCClient handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00035
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient_{nullptr};

    /// @brief mainLoop_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00036
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief Static global variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00873
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< NmIpc > s_IpcClientHandler_;

    /// @brief lnNotifierMap_  Logical network registered notifiers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00038
    /// @needwork = dda
    /// @endcode
    ara::core::Map< internal::IpcCommand, ara::core::Map< ara::core::String, IpcLnNotifierPair > > lnNotifierMap_;

    /// @brief etherNotifierMap_  Ethernet instance registered notifiers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00039
    /// @needwork = dda
    /// @endcode
    ara::core::Map< internal::IpcCommand, ara::core::Map< ara::core::String, IpcEtherNotifierPair > > etherNotifierMap_;

    /// @brief LnNotifierIerator Logical network notifier iterator declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00921
    /// @needwork = dda
    /// @endcode
    using LnNotifierIerator = ara::core::Map< ara::core::String, IpcLnNotifierPair >::iterator;

    /// @brief EtherNotifierIerator Ethernet instance notifier iterator declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00922
    /// @needwork = dda
    /// @endcode
    using EtherNotifierIerator = ara::core::Map< ara::core::String, IpcEtherNotifierPair >::iterator;
};

}  // namespace nm
}  // namespace ara
#endif
