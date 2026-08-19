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
/// @file       log_trans.h
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
/// @unit_name = log_trans
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __LOG_DLT_BACKNED__
#define __LOG_DLT_BACKNED__

#include <cstdint>
#include <list>
#include <map>
#include <string>

#include "aa_client_info.h"
#include "isoft/ipccpp/server.h"
#include "isoft/naicpp/evloop.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Default data size
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00275
/// @trace_id_dd=DD_LOG_00521
/// @needwork = dd
/// @endcode
std::size_t const kFIFOBUFFERSIZE{1024U * 1024 * 4U};
/// @brief Default pipe size
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00275
/// @trace_id_dd=DD_LOG_00521
/// @needwork = dd
/// @endcode

std::size_t const kFIFO_PIPE_SIZE{1024U * 4U};

/// @brief
/// Log processing backend, this module mainly receives logs: 1. Receive logs from AA, 2. Receive logs from gateway, 3. Forward corresponding logs via callback function. Record AA connection information.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00275
/// @trace_id_dd=DD_LOG_00522
/// @needwork = ad
/// @endcode
class LogTrans
{
public:
    /// @brief Callback interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00276
    /// @trace_id_dd=DD_LOG_00523
    /// @needwork = ad
    /// @endcode
    class Listener
    {
    public:
        /// @brief Data callback
        /// @param[in]  mesg  Message pointer
        /// @param[in]  mesgSize  Message size
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00277
        /// @trace_id_dd=DD_LOG_00524
        /// @needwork = ad
        /// @endcode
        virtual void OnLogBuffer(uint8_t* mesg, std::size_t const& mesgSize) noexcept = 0;
        /// @brief Destructor, free memory
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00278
        /// @trace_id_dd=DD_LOG_00525
        /// @needwork = ad
        /// @endcode
        virtual ~Listener() noexcept = default;
        /// @brief  Define a default constructor to create an instance of the Listener class. The default constructor does not require any parameters and can initialize the object to a default state.
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02350
        /// @trace_id_dd=DD_LOG_02749
        /// @needwork = ad
        /// @endcode
        Listener() = default;
        /// @brief  Disable copy construction
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02300
        /// @trace_id_dd=DD_LOG_02700
        /// @needwork = ad
        /// @endcode
        Listener(Listener const&) = delete;
        /// @brief Disable copy assignment
        /// @return
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02301
        /// @trace_id_dd=DD_LOG_02701
        /// @needwork = ad
        /// @endcode
        Listener& operator=(Listener const&) = delete;
        // Add move constructor
        Listener(Listener&&) noexcept = delete;
        // Add move assignment operator
        Listener& operator=(Listener&&) noexcept = delete;
    };

    /// @brief Initialize required resources
    /// @param[in]  listener Listener interface
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00279
    /// @trace_id_dd=DD_LOG_00526
    /// @needwork = ad
    /// @endcode
    std::int32_t Init(Listener* const listener) noexcept;

    /// @brief Data readable
    /// @param[in]  socketfd Descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00281
    /// @trace_id_dd=DD_LOG_00528
    /// @needwork = ad
    /// @endcode
    void OnSocketRead(std::int32_t const& socketfd) noexcept;
    /// @brief  Socket close callback
    /// @param[in]  socketfd  Socket descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00282
    /// @trace_id_dd=DD_LOG_00529
    /// @needwork = ad
    /// @endcode
    void OnSocketClose(std::int32_t const& socketfd) noexcept;

    /// @brief Resource cleanup
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00283
    /// @trace_id_dd=DD_LOG_00530
    /// @needwork = ad
    /// @endcode
    std::int32_t Destroy() noexcept;

    /// @brief
    /// @return
    /// @needwork = ad
    bool SetStoping() noexcept;
    /// @brief
    /// @return
    /// @needwork = ad
    bool StopWaiting() noexcept;
    /// @brief wait for logs
    /// @param secs
    /// @return
    /// @needwork = ad
    bool WaitSeconds(std::int32_t const& secs) noexcept;

    /// @brief
    /// @return
    /// @needwork = ad
    bool ClosePipe() noexcept;

private:
    /// @brief Enable receiving IPC log messages from AA
    /// @return 0ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00275
    /// @trace_id_dd=DD_LOG_00531
    /// @needwork = dda
    /// @endcode
    std::int32_t _StartListenForAALog() noexcept;

private:
    /// @brief mainLoop_ Main loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00275
    /// @trace_id_dd=DD_LOG_00532
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00275
    /// @trace_id_dd=DD_LOG_00533
    /// @needwork = dda
    /// @endcode
    Listener* mListener_{nullptr};
    /// @brief fifoReadThread_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00275
    /// @trace_id_dd=DD_LOG_00534
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< std::thread > fifoReadThread_{nullptr};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00275
    /// @trace_id_dd=DD_LOG_00535
    /// @needwork = dda
    /// @endcode
    std::string mInternalPipeNameLogModule_{"/tmp/ara_log_internal_logdata_fifo_"};
};

}  // namespace internal

}  // namespace log

}  // namespace ara

#endif