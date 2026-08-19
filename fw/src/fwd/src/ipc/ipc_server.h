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
/// @file       ipc_server.h
/// @brief      fw IPC processing logic
/// @details    fw IPC processing logic
/// @date       2024-12-17
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/IPC Communication Management
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001,SR_FW_0011
/// @unit_name=Ipc_Server
/// @unit_description=Firewall IPC processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_IPC_SERVER_H_
#define ARA_FW_IPC_SERVER_H_

#include <ara/core/map.h>
#include <ara/exec/state_client.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/evloop.h>

#include <functional>

#include "fwmanager/fw_manager.h"
namespace ara {
namespace fw {
namespace internal {
/// @brief Forward declaration
class FWManager;

/// @brief fw provides the IPC server side of the program
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00856
/// @needwork = dda
/// @endcode
class PIpcServer final
{
public:
    /// @brief Constructor
    /// @param fwManager fw main management class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00857
    /// @needwork = dda
    /// @endcode
    explicit PIpcServer(FWManager &fwManager) noexcept;

    /// @brief Destructor
    /// @exception
    /// @code{.isoft}
    /// @code {.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00858
    /// @needwork = dda
    /// @endcode
    ~PIpcServer() noexcept = default;

    /// @brief the move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00859
    /// @needwork = dda
    /// @endcode
    PIpcServer(PIpcServer &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return PIpcServer&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00860
    /// @needwork = dda
    /// @endcode
    PIpcServer &operator=(PIpcServer &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return PIpcServer&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00861
    /// @needwork = dda
    /// @endcode
    PIpcServer &operator=(PIpcServer const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00862
    /// @needwork = dda
    /// @endcode
    PIpcServer(PIpcServer const &other) = delete;

    /// @brief Begin
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00863
    /// @needwork = dda
    /// @endcode
    bool Init() noexcept;

    /// @brief Working
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00864
    /// @needwork = dda
    /// @endcode
    bool Start() noexcept;

    /// @brief Stop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00865
    /// @needwork = dda
    /// @endcode
    void Stop() noexcept;

public:
    /// @brief Handle connection
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00866
    /// @needwork = dda
    /// @endcode
    static void CB_IpcProcess_Connect(void *const context,
                                      isoft::ipc::IPCServerHandleType const type,
                                      isoft::ipc::IPCPacket *const pReqPacket) noexcept;

    /// @brief Disconnect message
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00867
    /// @needwork = dda
    /// @endcode
    static void CB_IpcProcess_DisConnect(void *const context,
                                         isoft::ipc::IPCServerHandleType const type,
                                         isoft::ipc::IPCPacket *const pReqPacket) noexcept;
    /// @brief Handle notification messages
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00868
    /// @needwork = dda
    /// @endcode
    static void CB_IpcProcess_MsgPost(void *const context,
                                      isoft::ipc::IPCServerHandleType const type,
                                      isoft::ipc::IPCPacket *const pReqPacket) noexcept;
    /// @brief Handle request messages
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00869
    /// @needwork = dda
    /// @endcode
    static void CB_IpcProcess_MsgReq(void *const context,
                                     isoft::ipc::IPCServerHandleType const type,
                                     isoft::ipc::IPCPacket *const pReqPacket) noexcept;

    /// @brief Send Ipc Packet
    /// @name SendIpcPacket
    /// @param pReqPacket IPC packet
    /// @param bComplete Whether completed
    /// @returns int32_t
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00870
    /// @needwork = dda
    /// @endcode
    int32_t SendIpcPacket(isoft::ipc::IPCPacket *const pReqPacket, bool const bComplete) const;

    /// @brief New Ipc Packet
    /// @param nSessionID Temporary session ID the Session ID
    /// @return the pointer : isoft::ipc::IPCPacket*
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00871
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCPacket *NewIpcPacket(uint64_t const nSessionID) const noexcept;  // Create a new IPC packet

private:
    /// @brief Message loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00872
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::naicpp::EvLoop > pEvLoop_{};
    /// @brief IPC service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00873
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ::isoft::ipc::IPCServer > pIpcServer_{};

    /// @brief FW main processing class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00874
    /// @needwork = dda
    /// @endcode
    FWManager &ipcProcessManager_;
};

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_SERVER_H_
