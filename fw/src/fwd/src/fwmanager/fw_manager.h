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
/// @file       fw_manager.h
/// @brief      fw main processing class
/// @details    fw main processing class
/// @date       2024-12-16
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Manager
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0001,SR_FW_0003
/// @unit_name=FW_Manager
/// @unit_description=Firewall management class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_MANAGER_H_
#define ARA_FW_MANAGER_H_
#include <ara/exec/execution_client.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include "ara/fw/internal/msg_process.h"
#include "ara/fw/internal/serialization_error_domain.h"
#include "config/config_manager.h"
#include "filterchain/fw_filter_chain.h"
#include "ipc/ipc_server.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief  ipc server declear
class PIpcServer;
/// @brief fw Manager
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00524
/// @trace_id_dd=DD_FW_00839
/// @needwork = ad
/// @endcode
class FWManager final
{
public:
    /// @brief Default copy constructor note: changing to default here may cause
    /// errors in lower version compilers (declared to take const reference, but
    /// implicit declaration would take non-const)
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00525
    /// @trace_id_dd=DD_FW_00840
    /// @needwork = ad
    /// @endcode
    FWManager(FWManager const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00526
    /// @trace_id_dd=DD_FW_00841
    /// @needwork = ad
    /// @endcode
    FWManager(FWManager &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00527
    /// @trace_id_dd=DD_FW_00842
    /// @needwork = ad
    /// @endcode
    FWManager &operator=(FWManager const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00528
    /// @trace_id_dd=DD_FW_00843
    /// @needwork = ad
    /// @endcode
    FWManager &operator=(FWManager &&other) = delete;
    /// @brief fw manager constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00529
    /// @trace_id_dd=DD_FW_00844
    /// @needwork = ad
    /// @endcode
    FWManager() noexcept;
    /// @brief fw manager destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00530
    /// @trace_id_dd=DD_FW_00845
    /// @needwork = ad
    /// @endcode
    ~FWManager() = default;

public:
    /// @brief Initialize
    /// @return Success/Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00531
    /// @trace_id_dd=DD_FW_00846
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Run IPC
    /// @return Success/Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00532
    /// @trace_id_dd=DD_FW_00847
    /// @needwork = ad
    /// @endcode
    bool Run() noexcept;

    /// @brief End
    /// @return Success/Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00533
    /// @trace_id_dd=DD_FW_00848
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

public:
    /// @brief process IPC data from client.
    /// @param eventMsg event msg
    /// @return fw error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00534
    /// @trace_id_dd=DD_FW_00849
    /// @needwork = ad
    /// @endcode
    FwErrc ProcessIpcMsg(ClientEventMsg const &eventMsg) noexcept;

    /// @brief response result to client.
    /// @param fwErrorCode fw error code
    /// @param packet IPC packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00535
    /// @trace_id_dd=DD_FW_00850
    /// @needwork = ad
    /// @endcode
    void ResponseProcessResult(FwErrc const &fwErrorCode, ::isoft::ipc::IPCPacket *packet) noexcept;

    /// @brief Receive IPC packet processing
    /// @param pReqPacket IPC packet
    /// @return fw error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00536
    /// @trace_id_dd=DD_FW_00851
    /// @needwork = ad
    /// @endcode
    FwErrc ReceiveDataProcess(isoft::ipc::IPCPacket *const pReqPacket) noexcept;

private:
    /// @brief fw parse json configuration pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00852
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ConfigManager > upFWManifestReader_{};
    /// @brief Execution management pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00853
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ara::exec::ExecutionClient > upExeClient_{};
    /// @brief Filter chain management pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00854
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< FWFilterChain > upFWFilterChain_{};
    /// @brief ipc server pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00855
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< PIpcServer > upIpcServer_{};
};
}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif