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
/// @file       fw_client.h
/// @brief      Firewall client processing
/// @details    Firewall client processing
/// @date       2025-03-19
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/lib client
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Firewall_Libclient
/// @unit_description=Firewall external interface IPC communication processing
/// class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_IDS_CLIENT_H_
#define _ARA_FW_IDS_CLIENT_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/log/logger.h>
#include <isoft/ipccpp/client.h>
#include <isoft/naicpp/evloop.h>

#include <cstdint>
#include <memory>
#include <mutex>

#include "ara/fw/common/common.h"
#include "ara/fw/internal/msg_process.h"

namespace ara {
namespace fw {
namespace internal {
/// @brief
// class IPCClient;

/// @brief Type redefinition
using IPCClientPtr = std::shared_ptr< ::isoft::ipc::IPCClient >;
/// @brief fw client client processing actual processing class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00098
/// @trace_id_dd=DD_FW_00169
/// @needwork = ad
/// @endcode
class FWClient final
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @code {.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00099
    /// @trace_id_dd=DD_FW_00170
    /// @needwork = ad
    /// @endcode
    ~FWClient() = default;
    /// @brief Get the unique instance of FWClient
    /// @return
    /// @exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00100
    /// @trace_id_dd=DD_FW_00171
    /// @needwork = ad
    /// @endcode
    static FWClient *GetInstance();
    /// @brief FWClient instance destruction
    /// @return
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00101
    /// @trace_id_dd=DD_FW_00172
    /// @needwork = ad
    /// @endcode
    void Destroy();
    /// @brief IDSM client initialization
    /// @return 0 success, -1 failure
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00102
    /// @trace_id_dd=DD_FW_00173
    /// @needwork = ad
    /// @endcode
    int32_t Init();
    /// @brief Set whether the client is connected to the server flag
    /// @param bConnect Whether connected flag
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00103
    /// @trace_id_dd=DD_FW_00174
    /// @needwork = ad
    /// @endcode
    void SetConnect(bool const bConnect) noexcept;

    /// @brief Switch firewall state
    /// @param specifierId Port instance identifier
    /// @param fwstatus Firewall state value
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00104
    /// @trace_id_dd=DD_FW_00175
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SwitchFirewallState(ara::core::InstanceSpecifier const &specifierId, FWState fwStatus);

public:
    /// @brief Parameterless constructor
    /// @exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00105
    /// @trace_id_dd=DD_FW_00176
    /// @needwork = ad
    /// @endcode
    FWClient() = default;

    /// @brief Copy constructor
    /// @param client Client instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00106
    /// @trace_id_dd=DD_FW_00177
    /// @needwork = ad
    /// @endcode
    FWClient(FWClient const &client) = delete;

    /// @brief Move constructor
    /// @param client Client instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00107
    /// @trace_id_dd=DD_FW_00178
    /// @needwork = ad
    /// @endcode
    FWClient(FWClient &&client) = delete;

    /// @brief Copy assignment operator
    /// @param client Client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00108
    /// @trace_id_dd=DD_FW_00179
    /// @needwork = ad
    /// @endcode
    FWClient &operator=(FWClient const &client) = delete;

    /// @brief Move assignment operator
    /// @param client Client instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00109
    /// @trace_id_dd=DD_FW_00180
    /// @needwork = ad
    /// @endcode
    FWClient &operator=(FWClient &&client) = delete;

    /// @brief Send data function
    /// @param evMsg Message event
    /// @return 0 success, non-zero failure
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00110
    /// @trace_id_dd=DD_FW_00181
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SendEventMsg(Chunk const &evMsg);

    /// @brief
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00111
    /// @trace_id_dd=DD_FW_00182
    /// @needwork = ad
    /// @endcode
    inline int32_t SysErr() noexcept { return errno; };

private:
    /// @brief mux  lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00183
    /// @needwork = dda
    /// @endcode
    static std::mutex sS_Mtx_;
    /// @brief fw selegon instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00184
    /// @needwork = dda
    /// @endcode
    static std::unique_ptr< FWClient > sS_Instance_;
    /// @brief  connect check flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00185
    /// @needwork = dda
    /// @endcode
    bool bConnect_{false};

    /// @brief event loop pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00186
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > evLoopPtr_{nullptr};
    /// @name clientPtr_
    /// @brief Use forward declaration to hide eventloop related header files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00187
    /// @needwork = dda
    /// @endcode
    IPCClientPtr clientPtr_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // _ARA_FW_IDS_CLIENT_