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
/// @file       daemon.h
/// @brief      Management class for daemon processes
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = daemon
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_Daemon__
#define __LOG_INTERNAL_Daemon__

#include <isoft/naicpp/terminating_handler.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "dlt_server.h"
#include "isoft/ipccpp/server.h"
#include "isoft/naicpp/evloop.h"
#include "isoft/naicpp/global_evloop.h"
#include "log_command_trans.h"
#include "log_trans.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"

namespace ara {
namespace log {
namespace internal {

constexpr std::size_t kInt32_10240U{10240U};
constexpr std::uint16_t kInt32_3490U{3490U};
/// @brief Main file of the daemon process, called in main function
/// Log data callbacks to Daemon for processing, whether to forward to file or print.
/// Daemon is used for data aggregation and command forwarding
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00284
/// @trace_id_dd=DD_LOG_00536
/// @needwork = ad
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @endcode
class Daemon final
    : public DltServer::Listener
    , public LogTrans::Listener
{
public:
    /// @brief Configuration information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00285
    /// @trace_id_dd=DD_LOG_00537
    /// @needwork = ad
    /// @endcode
    struct DaemonConfig
    {
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01575
        /// @needwork = dda
        /// @endcode
        std::string serverip{"0.0.0.0"};
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01576
        /// @needwork = dda
        /// @endcode
        std::uint16_t serverport{kInt32_3490U};
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01577
        /// @needwork = dda
        /// @endcode
        std::size_t queueSize{kInt32_10240U};
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01578
        /// @needwork = dda
        /// @endcode
        std::int8_t internalLogLevel{0x0};
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00000
        /// @trace_id_dd=DD_LOG_01579
        /// @needwork = dda
        /// @endcode
        std::string ecuid{"ECU1"};
    };
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00536
    /// @needwork = ad
    /// @endcode
    Daemon() noexcept : DltServer::Listener{}, LogTrans::Listener{}
    {
        // The constructor body can be left empty or add other initialization logic
    }
    /// @brief Initialize
    /// @return 0 ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00286
    /// @trace_id_dd=DD_LOG_00538
    /// @needwork = ad
    /// @endcode
    std::int32_t Init() noexcept;
    /// @brief Start the event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00287
    /// @trace_id_dd=DD_LOG_00539
    /// @needwork = ad
    /// @endcode
    void Run() noexcept;
    /// @brief Clean up resources
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00288
    /// @trace_id_dd=DD_LOG_00540
    /// @needwork = ad
    /// @endcode
    std::int32_t Destroy() noexcept;
    /// @brief Send heartbeat
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_10288
    /// @trace_id_dd=DD_LOG_10540
    /// @needwork = ad
    /// @endcode
    void SendheartBeat() noexcept;
    /// @brief
    /// @return
    /// @needwork = ad
    bool WaitingTimeout() noexcept;

    /// @brief OnLogBuffer
    /// @param[in]  mesg Message pointer
    /// @param[in]  mesgSize Message size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00541
    /// @needwork = dda
    /// @endcode
    void OnLogBuffer(uint8_t* mesg, std::size_t const& mesgSize) noexcept final;

    /// @brief General event callback
    /// @param[in]  jsonstr Instruction JSON
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00543
    /// @needwork = dda
    /// @endcode
    void OnCmdEvent(std::string const& jsonstr) noexcept final;
    /// @brief Change log level for all channels
    /// @param[in]  loglevel Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00544
    /// @needwork = dda
    /// @endcode
    void OnAllLogLevel(std::uint8_t const& loglevel) noexcept final;

    /// @brief Change log level for a specified IP
    /// @param[in]  apppID Corresponding AP ID
    /// @param[in]  loglevel Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00545
    /// @needwork = dda
    /// @endcode
    void OnAppLogLevel(std::string const& apppID, std::uint8_t const& loglevel) noexcept final;

    /// @brief Change log level for a specified AP and specified channel
    /// @param[in]  apppID Corresponding AP ID
    /// @param[in]  contextID Corresponding channel ID
    /// @param[in]  loglevel Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00546
    /// @needwork = dda
    /// @endcode
    void OnChannelLogLevel(std::string const& apppID,
                           std::string const& contextID,
                           std::uint8_t const& loglevel) noexcept final;

private:
    /// @brief Read configuration
    /// @param[in]  configPath Configuration path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00542
    /// @needwork = dda
    /// @endcode
    DaemonConfig _ReadConfig(std::string const& configPath) noexcept;

private:
    /// @brief mainLoop_ Event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00547
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};
    /// @brief dltServer_ Class for handling client connections
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00548
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< DltServer > dltServer_{nullptr};
    /// @brief  mLogTrans_ daemon backend instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00549
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< LogTrans > mLogTrans_{nullptr};
    /// @brief mLogCommandTrans_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00550
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< LogCommandTrans > mLogCommandTrans_{nullptr};

    /// @brief
    /// @needwork = dda
    std::shared_ptr< isoft::naicpp::TerminatingHandler > mTerminatingHandler_{nullptr};

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00284
    /// @trace_id_dd=DD_LOG_00551
    /// @needwork = dda
    /// @endcode
    bool mIsAllLogSet_{false};
};

}  // namespace internal

}  // namespace log
}  // namespace ara
#endif