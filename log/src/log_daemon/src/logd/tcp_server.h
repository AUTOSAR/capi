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
/// @file       tcp_server.h
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
/// @unit_name = tcp_server
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOG__
#define __LOG_INTERNAL_LOG__
#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "isoft/naicpp/evloop.h"
#include "nai/io/nai_event.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Implement the transport interface, providing server-side IPC functionality
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00238
/// @trace_id_dd=DD_LOG_00462
/// @needwork = ad
/// @endcode
class TcpServer
{
public:
    /// @brief Listener interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00239
    /// @trace_id_dd=DD_LOG_00463
    /// @needwork = ad
    /// @endcode
    class Listener
    {
    public:
        /// @brief Connection status callback
        /// @param[in]  conn Connection status
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00240
        /// @trace_id_dd=DD_LOG_00464
        /// @needwork = ad
        /// @endcode
        virtual void OnSocketStatus(bool const& conn) noexcept = 0;
        /// @brief Data readable callback
        /// @param[in]  socketFd  Socket descriptor
        /// @return Read result
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00241
        /// @trace_id_dd=DD_LOG_00465
        /// @needwork = ad
        /// @endcode
        virtual std::int32_t OnSocketRead(std::int32_t const& socketFd) noexcept = 0;
        /// @brief  Destructor, free memory
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00242
        /// @trace_id_dd=DD_LOG_00466
        /// @needwork = ad
        /// @endcode
        virtual ~Listener() noexcept = default;
        /// @brief  Constructor
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02348
        /// @trace_id_dd=DD_LOG_02747
        /// @needwork = ad
        /// @endcode
        Listener() = default;
        /// @brief  Disable copy construction
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02305
        /// @trace_id_dd=DD_LOG_02705
        /// @needwork = ad
        /// @endcode
        Listener(Listener const&) = delete;
        /// @brief Disable copy assignment
        /// @return
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02306
        /// @trace_id_dd=DD_LOG_02706
        /// @needwork = ad
        /// @endcode
        Listener& operator=(Listener const&) = delete;

        // Add move constructor
        Listener(Listener&&) noexcept = default;
        // Add move assignment operator
        Listener& operator=(Listener&&) noexcept = default;
    };

    /// @brief Used for event callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00243
    /// @trace_id_dd=DD_LOG_00467
    /// @needwork = ad
    /// @endcode
    struct StreamInfo
    {
        /// @brief sockStream_ - NAI socket handle
        nai_stream_t sockStream{};
        /// @brief  Class pointer
        TcpServer* data{nullptr};
        /// @brief Socket descriptor
        std::int32_t socketfd{0};
    };
    /// @brief Constructor
    /// @param[in]  ip  IP address
    /// @param[in]  port Port
    /// @param[in]  lsner Listener
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00244
    /// @trace_id_dd=DD_LOG_00468
    /// @needwork = ad
    /// @endcode
    TcpServer(std::string ip, std::uint16_t const& port, TcpServer::Listener* const lsner) noexcept;
    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00359
    /// @trace_id_dd=DD_LOG_01758
    /// @needwork = ad
    /// @endcode
    TcpServer(TcpServer const& other) = default;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00360
    /// @trace_id_dd=DD_LOG_01759
    /// @needwork = ad
    /// @endcode
    TcpServer& operator=(TcpServer const& other) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00245
    /// @trace_id_dd=DD_LOG_00469
    /// @needwork = ad
    /// @endcode
    ~TcpServer() noexcept;

    // Add move constructor
    TcpServer(TcpServer&&) noexcept = default;
    // Add move assignment operator
    TcpServer& operator=(TcpServer&&) noexcept = default;
    /// @brief Open
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00246
    /// @trace_id_dd=DD_LOG_00470
    /// @needwork = ad
    /// @endcode
    std::int32_t Open() noexcept;
    /// @brief Send
    /// @param[in]  data_buffer Data pointer
    /// @param[in]  message_size Data size
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00247
    /// @trace_id_dd=DD_LOG_00471
    /// @needwork = ad
    /// @endcode
    std::int32_t Send(uint8_t* const dataBuffer, std::size_t const& messageSize) noexcept;
    /// @brief Number of clients
    /// @return Count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00248
    /// @trace_id_dd=DD_LOG_00472
    /// @needwork = ad
    /// @endcode
    std::int32_t ClientCount() const noexcept;

    /// @brief Add data stream
    /// @param[in]  socketfd Socket descriptor
    /// @param[in]  handler Callback handler pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00249
    /// @trace_id_dd=DD_LOG_00473
    /// @needwork = ad
    /// @endcode
    void AddStream(std::int32_t const& socketfd, TcpServer* const handler) noexcept;

    /// @brief Delete the socket and its handler that need to be monitored by NAI
    /// @param[in]  socketid Socket descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00250
    /// @trace_id_dd=DD_LOG_00474
    /// @needwork = ad
    /// @endcode
    void DeleteStream(std::int32_t const& socketid) noexcept;

    /// @brief NAI callback function
    /// @param[in]  stream Data stream
    /// @param[in]  events Events
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00251
    /// @trace_id_dd=DD_LOG_00475
    /// @needwork = ad
    /// @endcode
    static std::int32_t NAIMessageEventHandler(nai_stream_t* const stream, std::int32_t const events) noexcept;
    /// @brief Set fd to non-blocking mode
    /// @param[in]  fd Socket descriptor
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00252
    /// @trace_id_dd=DD_LOG_00476
    /// @needwork = ad
    /// @endcode
    void SetNonBlocking(std::int32_t const& fd) noexcept;
    /// @brief  Close
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00253
    /// @trace_id_dd=DD_LOG_00477
    /// @needwork = ad
    /// @endcode
    void CloseStream() noexcept;

protected:
    /// @brief Readable callback
    /// @param[in]  socketfd Socket descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00478
    /// @needwork = dda
    /// @endcode
    void _OnSocketRead(std::int32_t const& socketfd) noexcept;
    /// @brief Close callback
    /// @param[in]  socketfd Socket descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00479
    /// @needwork = dda
    /// @endcode
    void _OnSocketClose(std::int32_t const& socketfd) noexcept;

private:
    /// @brief
    /// @param[in]  port
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00480
    /// @needwork = dda
    /// @endcode
    std::int32_t _AddLisentPort(std::uint16_t const& port) noexcept;

private:
    /// @brief Listening port
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00481
    /// @needwork = dda
    /// @endcode
    nai_fd_t listenfd_{NAI_FD_INVALID};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00482
    /// @needwork = dda
    /// @endcode
    std::set< std::int32_t > clientSet_{};
    /// @brief mainLoop_ - Main event loop handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00483
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};
    /// @brief streams_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00484
    /// @needwork = dda
    /// @endcode
    std::list< std::shared_ptr< StreamInfo > > streams_{};
    /// @brief serverIp_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00485
    /// @needwork = dda
    /// @endcode
    std::string serverIp_{"0.0.0.0"};
    /// @brief serverPort_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00486
    /// @needwork = dda
    /// @endcode
    std::uint16_t serverPort_{0U};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00238
    /// @trace_id_dd=DD_LOG_00487
    /// @needwork = dda
    /// @endcode
    Listener* mListener_{nullptr};
};

}  // namespace internal

}  // namespace log

}  // namespace ara
#endif