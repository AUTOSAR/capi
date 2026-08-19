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
/// @file       idsm_async_sender.h
/// @brief      Asynchronous sender: This sender uses a separate thread to interact with the event generation thread via a security event pool
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=AsyncSender
/// @unit_description=Asynchronous sender: This sender uses a separate thread to interact with the event generation thread via a security event pool
/// @endcode
///
/// ================================================================

#ifndef IDSM_ASYNC_SENDER_H_
#define IDSM_ASYNC_SENDER_H_
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <vector>

#include "ara/idsm/internal/event.h"
#include "idsm_sender.h"
#include "nai/runtime/nai_types.h"

namespace ara {
namespace idsm {

/// @brief Asynchronous transmission class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00269
/// @trace_id_dd=DD_IDSM_00840
/// @needwork = ad
/// @endcode
class AsyncSender : public IdsmSender
{
public:
    /// @brief Send data to peer
    /// @param eventTrans Data to transmit
    /// @param highPriority Whether to jump the queue
    /// @return Number of bytes sent, returns -1 on error
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00841
    /// @needwork = dda
    /// @endcode
    int64_t Send(Message const& eventTrans, bool highPriority = false) override;
    /// @brief Register callback function
    /// @param callback Callback function for communication exceptions
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00842
    /// @needwork = dda
    /// @endcode
    void RegisterCallback(SendCallBack const& callback);

public:
    /// @brief Constructor and initialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00843
    /// @needwork = dda
    /// @endcode
    AsyncSender() : AsyncSender{ara::core::String{""}, 0U} {}
    /// @brief Constructor and initialization
    /// @param ip Peer IP address
    /// @param p Port on which the peer application listens
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00844
    /// @needwork = dda
    /// @endcode
    AsyncSender(ara::core::String const& ip, uint16_t const p) : IdsmSender{ip, p} { _init(); }
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00845
    /// @needwork = dda
    /// @endcode
    ~AsyncSender() override = default;

public:
    /// @brief Copy constructor
    /// @param sender Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00846
    /// @needwork = dda
    /// @endcode
    AsyncSender(AsyncSender const& sender) = delete;
    /// @brief Move constructor
    /// @param sender Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00847
    /// @needwork = dda
    /// @endcode
    AsyncSender(AsyncSender&& sender) = delete;
    /// @brief Copy assignment operator
    /// @param sender Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00848
    /// @needwork = dda
    /// @endcode
    AsyncSender& operator=(AsyncSender const& sender) = delete;
    /// @brief Move assignment operator
    /// @param sender Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00849
    /// @needwork = dda
    /// @endcode
    AsyncSender& operator=(AsyncSender&& sender) = delete;

private:
    /// @brief Thread entry function of the asynchronous sender
    /// @param thrArg Thread entry parameter, AsyncSender object
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00850
    /// @needwork = dda
    /// @endcode
    static void Run(void* const thrArg);
    /// @brief Start asynchronous write thread
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00851
    /// @needwork = dda
    /// @endcode
    void _init();
    /// @brief Establish connection to peer
    /// @return 0 on success, -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00852
    /// @needwork = dda
    /// @endcode
    int32_t _createSocket();
    /// @brief Get set of data to be sent
    /// @param eventsTrans Set of data to be sent
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00853
    /// @needwork = dda
    /// @endcode
    void _get(std::list< Message >& eventsTrans);
    /// @brief Send data
    /// @param eventTrans Data to send
    /// @return Length of data sent, returns -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00854
    /// @needwork = dda
    /// @endcode
    int64_t _syncWrite(Message const& eventTrans);

private:
    /// @brief Socket file descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00855
    /// @needwork = dda
    /// @endcode
    nai_fd_t fd_{-1};
    /// @brief Store producer messages
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00856
    /// @needwork = dda
    /// @endcode
    std::list< Message > records_{};
    /// @brief Condition variable for synchronizing producer and consumer threads
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00857
    /// @needwork = dda
    /// @endcode
    std::condition_variable cond_{};
    /// @brief Mutex lock used by condition variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00858
    /// @needwork = dda
    /// @endcode
    std::mutex condiMutex_{};
    /// @brief Communication exception event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00859
    /// @needwork = dda
    /// @endcode
    Message errMsg_{};
    /// @brief Registered callback function, called when communication fails
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00860
    /// @needwork = dda
    /// @endcode
    SendCallBack handler_{};
};

}  // namespace idsm
}  // namespace ara

#endif