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
/// @file       idsm_server.h
/// @brief      Security event receiver
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/event receiver handler
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0001
/// @unit_name=IdsmServer
/// @unit_description=Security event receiver
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SERVER_H_
#define ARA_IDSM_SERVER_H_
#include <ara/core/map.h>
#include <ara/exec/state_client.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/evloop.h>

#include <memory>

#include "ara/core/vector.h"
#include "ara/idsm/internal/idsm_error_domain.h"
#include "ara/idsm/internal/message_process.h"

namespace ara {
namespace idsm {
/// @brief Forward declaration
class EventProcer;
/// @brief IDSM service class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00142
/// @trace_id_dd=DD_IDSM_00317
/// @needwork = ad
/// @endcode
class IdsmServer
{
public:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00318
    /// @needwork = dda
    /// @endcode
    IdsmServer() = default;
    /// @brief Copy constructor
    /// @param s Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00319
    /// @needwork = dda
    /// @endcode
    IdsmServer(IdsmServer const &s) = delete;
    /// @brief Move constructor
    /// @param s Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00320
    /// @needwork = dda
    /// @endcode
    IdsmServer(IdsmServer &&s) = default;
    /// @brief Copy assignment operator
    /// @param s Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00321
    /// @needwork = dda
    /// @endcode
    IdsmServer &operator=(IdsmServer const &s) = delete;
    /// @brief Move assignment operator
    /// @param s Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00322
    /// @needwork = dda
    /// @endcode
    IdsmServer &operator=(IdsmServer &&s) = default;
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00323
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmServer() = default;
    /// @brief Initialize
    /// @return 0 on success, non-zero on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00143
    /// @trace_id_dd=DD_IDSM_00324
    /// @needwork = ad
    /// @endcode
    int32_t Init() noexcept;
    /// @brief Start event loop
    /// @return 0 on success, non-zero on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00144
    /// @trace_id_dd=DD_IDSM_00325
    /// @needwork = ad
    /// @endcode
    int32_t Start() noexcept;
    /// @brief Stop all services && clean up service resources
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00145
    /// @trace_id_dd=DD_IDSM_00326
    /// @needwork = ad
    /// @endcode
    void Stop() const noexcept;

private:
    /// @brief Process data sent by user
    /// @param packet User data
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00327
    /// @needwork = dda
    /// @endcode
    void _handleReceiveData(::isoft::ipc::IPCPacket *const packet);
    /// @brief Initialize IPC
    /// @return 0 on success, non-zero on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00328
    /// @needwork = dda
    /// @endcode
    int32_t _initIpc() noexcept;
    /// @brief Initialize function group states
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00329
    /// @needwork = dda
    /// @endcode
    int32_t _initFGNotify();

private:
    /// @brief Global event loop (must be the first variable, otherwise need to reset in legal order in destructor)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00332
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::naicpp::EvLoop > evLoopPtr_;
    /// @brief Timer runs once per second
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00330
    /// @needwork = dda
    /// @endcode
    int32_t feedInterval_{1};
    /// @brief Timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00331
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr feedTimer_;
    /// @brief IPC service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00333
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ::isoft::ipc::IPCServer > ipcServer_;
    /// @brief Event handler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00334
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< EventProcer > evProcesser_;
};

}  // namespace idsm
}  // namespace ara

#endif