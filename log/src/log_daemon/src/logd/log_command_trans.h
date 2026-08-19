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
/// @file       log_command_trans.h
/// @brief      Template class for encapsulating log parameters
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
/// @unit_name = log_command_trans
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __internal_log_LogCommandTrans__
#define __internal_log_LogCommandTrans__

#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "aa_client_info.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Command sending, callbacks for received commands to the outside
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00289
/// @trace_id_dd=DD_LOG_00552
/// @needwork = ad
/// @endcode
class LogCommandTrans final
{
    /// @brief Define a map of application ID to socket
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00328
    /// @trace_id_dd=DD_LOG_01724
    /// @needwork = ad
    /// @endcode
    using AppIDSocketMap = std::map< std::string, std::uint64_t >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00290
    /// @trace_id_dd=DD_LOG_00553
    /// @needwork = ad
    /// @endcode
    LogCommandTrans() = default;
    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00363
    /// @trace_id_dd=DD_LOG_01762
    /// @needwork = ad
    /// @endcode
    LogCommandTrans(LogCommandTrans const &other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00364
    /// @trace_id_dd=DD_LOG_01763
    /// @needwork = ad
    /// @endcode
    LogCommandTrans &operator=(LogCommandTrans const &other) = delete;

    LogCommandTrans(LogCommandTrans &&other) noexcept
        : mapAppidSocket_(std::move(other.mapAppidSocket_))
        , appInfos_(std::move(other.appInfos_))
        , ipcServer_(std::move(other.ipcServer_))
        , mainLoop_(std::move(other.mainLoop_))
        , mStoping_(other.mStoping_)
    {
    }

    /// @brief Move assignment operator
    /// @param other
    /// @return Reference
    LogCommandTrans &operator=(LogCommandTrans &&other) noexcept
    {
        if (this != &other) {
            mapAppidSocket_ = std::move(other.mapAppidSocket_);
            appInfos_       = std::move(other.appInfos_);
            ipcServer_      = std::move(other.ipcServer_);
            mainLoop_       = std::move(other.mainLoop_);
            mStoping_       = other.mStoping_;
        }
        return *this;
    }
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00291
    /// @trace_id_dd=DD_LOG_00554
    /// @needwork = ad
    /// @endcode
    ~LogCommandTrans() noexcept;
    /// @brief Open in server mode
    /// @return 0 ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00292
    /// @trace_id_dd=DD_LOG_00555
    /// @needwork = ad
    /// @endcode
    std::int32_t Init() noexcept;
    /// @brief  Send to all clients
    /// @param[in]  mesg
    /// @param[in]  message_size
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00293
    /// @trace_id_dd=DD_LOG_00556
    /// @needwork = ad
    /// @endcode
    std::int32_t SendMessage(std::uint8_t *const mesg, std::size_t const &messageSize) noexcept;

    /// @brief Send data
    /// @param[in]  sessionid  Session ID
    /// @param[in]  data_buffer  Data pointer
    /// @param[in]  message_size Data length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00294
    /// @trace_id_dd=DD_LOG_00557
    /// @needwork = ad
    /// @endcode
    std::int32_t Send(std::uint64_t const &sessionid,
                      std::uint8_t *const dataBuffer,
                      std::size_t const &messageSize) noexcept;
    /// @brief Send data to a specified application
    /// @param[in]  appid  Application ID
    /// @param[in]  data_buffer  Data pointer
    /// @param[in]  message_size  Data size
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00295
    /// @trace_id_dd=DD_LOG_00558
    /// @needwork = ad
    /// @endcode
    std::int32_t SendByAppid(std::string const &appid,
                             std::uint8_t *const dataBuffer,
                             std::size_t const &messageSize) noexcept;
    /// @brief Get client information
    /// @return Client information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00296
    /// @trace_id_dd=DD_LOG_00559
    /// @needwork = ad
    /// @endcode
    inline std::shared_ptr< AAClientInfoMap > GetAAclientInfos() noexcept { return appInfos_; };

    /// @brief
    /// @return
    /// @needwork = ad
    bool SetStoping() noexcept;

    /// @brief
    /// @return
    /// @needwork = ad
    bool DeIniting() noexcept;

protected:
    /// @brief DoIpcResponse
    /// @param[in]  sessionid
    /// @param[in]  msg
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00560
    /// @needwork = dda
    /// @endcode
    void _DoIpcResponse(std::uint64_t const &sessionid, std::string const &msg) noexcept;
    /// @brief handleLibCmd
    /// @param[in] jsonstr
    /// @param[in] socketfd
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00561
    /// @needwork = dda
    /// @endcode
    void _HandleLibCmd(std::string const &jsonstr, std::uint64_t const &socketfd) noexcept;
    /// @brief IPC request callback function, called when receiving a request from a client.
    /// @param[in]  context - User parameter
    /// @param[in]  htype - Handle type
    /// @param[in]  reqPacket - Data packet
    /// @code{.isoft}
    /// @threadsafety={no}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00562
    /// @needwork = dda
    /// @endcode
    void _IpcServerRequestHandler(void *const context,
                                  isoft::ipc::IPCServerHandleType const &htype,
                                  isoft::ipc::IPCPacket *const reqPacket) noexcept;
    /// @brief IPC service connection callback function, called when a client establishes or disconnects.
    /// @param[in]  context - User parameter
    /// @param[in]  type - Type
    /// @param[in]  packet - Data packet
    /// @code{.isoft}
    /// @threadsafety={no}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00563
    /// @needwork = dda
    /// @endcode
    void _IpcServerConnectionHandler(void *const context,
                                     isoft::ipc::IPCServerHandleType const &type,
                                     isoft::ipc::IPCPacket *const packet);

private:
    /// @brief
    /// @param[in]  appid
    /// @param[in]  socketfd
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00564
    /// @needwork = dda
    /// @endcode
    void _addAAClient(std::string const &appid, std::uint64_t const &socketfd) noexcept;
    /// @brief
    /// @param[in]  socketfd
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00565
    /// @needwork = dda
    /// @endcode
    void _DelAAClient(std::uint64_t const &socketfd) noexcept;
    /// @brief
    /// @param[in]  appid
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00566
    /// @needwork = dda
    /// @endcode
    std::uint64_t _getSocketFdByAppID(std::string const &appid) noexcept;
    /// @brief
    /// @param[in]  sessionId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00567
    /// @needwork = dda
    /// @endcode
    void _RegStatusResponse(std::uint64_t const &sessionId) noexcept;

private:
    /// @brief mapAppidSocket_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00568
    /// @needwork = dda
    /// @endcode
    AppIDSocketMap mapAppidSocket_{};

    /// @brief appInfos_ Client information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00569
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< AAClientInfoMap > appInfos_{nullptr};

    /// @brief ipcServer_ - IPC server handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00570
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::ipc::IPCServer > ipcServer_{nullptr};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00571
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00289
    /// @trace_id_dd=DD_LOG_00572
    /// @needwork = dda
    /// @endcode
    std::string const kDaemonName{"logd"};

    /// @brief
    /// @needwork = dda
    bool mStoping_{false};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif