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
/// @file       dlt_server.h
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
/// @unit_name = dlt_server
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __LOG_DLTCLIENT_HANDLER__
#define __LOG_DLTCLIENT_HANDLER__

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <string>

#include "Utils/src/log_buffer.h"
#include "aa_client_info.h"
#include "dlt_command_receiver.h"
#include "isoft/ipccpp/packet.h"
#include "isoft/ipccpp/server.h"
#include "isoft/naicpp/evloop.h"
#include "nai/io/nai_io.h"
#include "tcp_server.h"
namespace ara {
namespace log {
namespace internal {

constexpr std::uint8_t kInt8_0x05U{0x05U};
/// @brief  1. Send log data to external log clients 2. Receive commands from external log clients
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00299
/// @trace_id_dd=DD_LOG_00575
/// @needwork = ad
/// @endcode
class DltServer final
    : public DltCommandReceiver::Listener
    , public TcpServer::Listener
{
public:
    /// @brief Callback interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00300
    /// @trace_id_dd=DD_LOG_00576
    /// @needwork = ad
    /// @endcode
    class Listener
    {
    public:
        /// @briefMove constructor
        /// @param[in] object Listener&& object
        Listener(Listener &&object) noexcept = delete;
        /// @brief Move operation
        /// @param[in] object Listener&& object
        /// @return Listener&
        Listener &operator=(Listener &&object) noexcept = delete;

    public:
        /// @brief All events
        /// @param[in]  cmd command JSON
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00301
        /// @trace_id_dd=DD_LOG_00577
        /// @needwork = ad
        /// @endcode
        virtual void OnCmdEvent(std::string const &cmd) noexcept = 0;
        /// @brief Event of changing log level
        /// @param[in]  loglevel Log level
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00302
        /// @trace_id_dd=DD_LOG_00578
        /// @needwork = ad
        /// @endcode
        virtual void OnAllLogLevel(std::uint8_t const &loglevel) noexcept = 0;
        /// @brief  Event of changing log level
        /// @param[in]  apppID Application ID
        /// @param[in]  loglevel Log level
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00303
        /// @trace_id_dd=DD_LOG_00579
        /// @needwork = ad
        /// @endcode
        virtual void OnAppLogLevel(std::string const &apppID, std::uint8_t const &loglevel) noexcept = 0;
        /// @brief Change log level of a specified channel
        /// @param[in]  apppID Application ID
        /// @param[in]  contextID  Channel ID
        /// @param[in]  loglevel Log level
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00304
        /// @trace_id_dd=DD_LOG_00580
        /// @needwork = ad
        /// @endcode
        virtual void OnChannelLogLevel(std::string const &apppID,
                                       std::string const &contextID,
                                       std::uint8_t const &loglevel) noexcept = 0;
        /// @brief Free memory
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00305
        /// @trace_id_dd=DD_LOG_00581
        /// @needwork = ad
        /// @endcode
        virtual ~Listener() noexcept = default;
        /// @brief  Define a default constructor to create an instance of the Listener class. The default constructor does not require any parameters and can initialize the object to a default state.
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02351
        /// @trace_id_dd=DD_LOG_02750
        /// @needwork = ad
        /// @endcode
        Listener() = default;

        /// @brief  Disable copy construction
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02339
        /// @trace_id_dd=DD_LOG_02738
        /// @needwork = ad
        /// @endcode
        Listener(Listener const &) = delete;
        /// @brief Disable copy assignment
        /// @return
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02340
        /// @trace_id_dd=DD_LOG_02739
        /// @needwork = ad
        /// @endcode
        Listener &operator=(Listener const &) = delete;
    };

public:
    /// @brief Initialization function
    /// @param[in]  ecuID ecuID
    /// @param[in]  ip IP address
    /// @param[in]  port Port
    /// @param[in]  bufferSize Buffer size
    /// @param[in]  lis Callback function
    /// @return  0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00306
    /// @trace_id_dd=DD_LOG_00582
    /// @needwork = ad
    /// @endcode
    std::int32_t Init(std::string const &ecuId,
                      std::string const &ip,
                      std::uint16_t const &port,
                      std::size_t const &bufferSize,
                      Listener *const lis) noexcept;
    /// @brief  Destructor
    /// @return  0 ok ，<0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00590
    /// @needwork = dda
    /// @endcode
    std::int32_t Destroy() noexcept;

    /// @brief Receive log messages sent from AA
    /// @param[in]  buffer Cache address
    /// @param[in]  len Cache size
    /// @return Data size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00307
    /// @trace_id_dd=DD_LOG_00583
    /// @needwork = ad
    /// @endcode
    std::int32_t SendLogMessage(std::uint8_t *const buffer, std::size_t const &len) noexcept;
    /// @brief Get client status
    /// @return Client status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00308
    /// @trace_id_dd=DD_LOG_00584
    /// @needwork = ad
    /// @endcode
    std::int32_t ClientStates() const noexcept;

    /// @brief Set information
    /// @param[in]  infoPtr Client information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00309
    /// @trace_id_dd=DD_LOG_00585
    /// @needwork = ad
    /// @endcode
    void SetAAInfos(std::shared_ptr< AAClientInfoMap > const &infoPtr);

    /// @brief Receive parsed information
    /// @param[in]  clientFd Client ID
    /// @param[in]  msg Message structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00310
    /// @trace_id_dd=DD_LOG_00586
    /// @needwork = ad
    /// @endcode
    void OnCommandMessage(std::int32_t const &clientFd, DltMessage *msg) final;

    /// @brief Callback descriptor status
    /// @param[in]  connected  Whether connected
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00311
    /// @trace_id_dd=DD_LOG_00587
    /// @needwork = ad
    /// @endcode
    void OnSocketStatus(bool const &connected) noexcept final;

    /// @brief Socket data readable
    /// @param[in]  socketFd
    /// @return Number of bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00312
    /// @trace_id_dd=DD_LOG_00588
    /// @needwork = ad
    /// @endcode
    std::int32_t OnSocketRead(std::int32_t const &socketFd) noexcept final;

private:
    /// @brief Accept network data for processing, callback of Transport
    /// @param[in]  msgType Type
    /// @param[in]  mesg Message pointer
    /// @param[in]  mesgSize Message size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00589
    /// @needwork = dda
    /// @endcode
    void _OnChannelData(std::int32_t const &msgType,
                        std::uint8_t const *const mesg,
                        std::size_t const &mesgSize) noexcept;

    /// @brief Process client data
    /// @param[in]  sock  Descriptor
    /// @param[in]  msg Message structure
    /// @return  0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00591
    /// @needwork = dda
    /// @endcode
    std::int32_t ProcessLogClientData(std::int32_t const &sock, DltMessage *const msg) noexcept;

    /// @brief Directly send buffer to client
    /// @param[in]  sock
    /// @param[in]  data_buffer
    /// @param[in]  message_size
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00592
    /// @needwork = dda
    /// @endcode
    std::int32_t _SendBufferBackToClient(std::int32_t const &sock,
                                         void *const dataBuffer,
                                         std::int32_t const &messageSize) noexcept;
    /// @brief  Send segmented buffer to client
    /// @param[in]  sock  Descriptor
    /// @param[in]  data1 Data address
    /// @param[in]  size1 Data size
    /// @param[in]  data2 Data address
    /// @param[in]  size2 Data size
    /// @param[in]  serialheader Serialization flag
    /// @return Number of bytes sent. <0 failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00593
    /// @needwork = dda
    /// @endcode
    std::int32_t _SocketSendMsgToClient(std::int32_t const &sock,
                                        void *const data1,
                                        std::int32_t const &size1,
                                        void *const data2,
                                        std::int32_t const &size2,
                                        bool const &serialheader) noexcept;

    /// @brief Send result of control information
    /// @param[in]  sock Descriptor
    /// @param[in]  msg Message structure
    /// @param[in]  apid Application ID
    /// @param[in]  ctid Channel ID
    /// @return 0 ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00594
    /// @needwork = dda
    /// @endcode
    std::int32_t _SendControlMsg(std::int32_t const &sock,
                                 DltMessage *const msg,
                                 std::string const &apid,
                                 std::string const &ctid) noexcept;
    /// @brief Prepare control information according to the command
    /// @param[in]  sock Descriptor
    /// @param[in]  service_id Service ID
    /// @param[in]  status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00595
    /// @needwork = dda
    /// @endcode
    void _ProcessControlResponse(std::int32_t const &sock,
                                 std::uint32_t const &serviceId,
                                 std::uint8_t const &status) noexcept;
    /// @brief Function to handle getting version information
    /// @param[in]  sock Descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00596
    /// @needwork = dda
    /// @endcode
    void _ProcessControlgetSoftwareVersion(std::int32_t const &sock) noexcept;
    /// @brief  ProcessGetDefaultLogLevel
    /// @param[in]  sock Descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00597
    /// @needwork = dda
    /// @endcode
    void _ProcessGetDefaultLogLevel(std::int32_t const &sock) noexcept;
    /// @brief Get log information
    /// @param[in]  sock Descriptor
    /// @param[in]  msg  Message structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00598
    /// @needwork = dda
    /// @endcode
    void _ProcessGetLogInfo(std::int32_t const &sock, DltMessage *const msg) noexcept;

    /// @brief Handle log level
    /// @param[in]  sock Descriptor
    /// @param[in]  msg  Message structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00599
    /// @needwork = dda
    /// @endcode
    void _ProcessSetLogLevel(std::int32_t const &sock, DltMessage *const msg);
    /// @brief Initialize message
    /// @param[in]  msg
    /// @return
private:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00600
    /// @needwork = dda
    /// @endcode
    std::int32_t _InitDltMessage(DltMessage *const msg);
    /// @brief
    /// @param[in]  msg
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00601
    /// @needwork = dda
    /// @endcode
    void _FreeDltMessage(DltMessage *const msg);
    /// @brief
    /// @param[in]  cmdid
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00602
    /// @needwork = dda
    /// @endcode
    std::string _CmdTojson(std::uint32_t const &cmdid) noexcept;
    /// @brief Helper function to convert a command with one int parameter to a JSON string
    /// @param[in]  cmdid
    /// @param[in]  arg
    /// @return JSON string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00603
    /// @needwork = dda
    /// @endcode
    std::string _CmdTojsonArgInt(std::uint32_t const &cmdid, std::int32_t const &arg) noexcept;

    /// @brief
    /// @param[in]  loglevel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00604
    /// @needwork = dda
    /// @endcode
    void _updateLoglevel(std::uint8_t const &loglevel);
    /// @brief
    /// @param[in]  appid
    /// @param[in]  loglevel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00605
    /// @needwork = dda
    /// @endcode
    void _updateLoglevel(std::string const &appid, std::uint8_t const &loglevel) noexcept;
    /// @brief
    /// @param[in]  appid
    /// @param[in]  ctxid
    /// @param[in]  loglevel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00606
    /// @needwork = dda
    /// @endcode
    void _updateLoglevel(std::string const &appid, std::string const &ctxid, std::uint8_t const &loglevel) noexcept;
    /// @brief clients_ File descriptors of already connected clients
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00607
    /// @needwork = dda
    /// @endcode
    std::list< std::int32_t > clients_{};
    /// @brief tcp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00608
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< TcpServer > mTcpServer_{nullptr};
    /// @brief Server list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00609
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::ipc::IPCServer > ipcServer_{nullptr};

    /// @brief listenIP_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00611
    /// @needwork = dda
    /// @endcode
    std::string listenIP_{"0.0.0.0"};
    /// @brief buffer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00612
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< LogBuffer > buffer_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00613
    /// @needwork = dda
    /// @endcode
    Listener *mListener_{nullptr};

    /// @brief recivers_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00614
    /// @needwork = dda
    /// @endcode
    std::map< std::int32_t, std::shared_ptr< DltCommandReceiver > > recivers_;
    /// @brief defaultlogLevel kDebug
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00615
    /// @needwork = dda
    /// @endcode
    std::uint8_t defaultlogLevel_{kInt8_0x05U};
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00616
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< AAClientInfoMap > aainfos_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00617
    /// @needwork = dda
    /// @endcode
    std::mutex bufferLocker_{};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00618
    /// @needwork = dda
    /// @endcode
    std::uint32_t const kCommand_Read_Buffer{4096};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00299
    /// @trace_id_dd=DD_LOG_00619
    /// @needwork = dda
    /// @endcode
    std::string mEcuId_{"ECU1"};
};

}  // namespace internal

}  // namespace log

}  // namespace ara

#endif
