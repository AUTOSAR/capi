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
/// @file       isoft_ipc_server.h
/// @brief      AutoSar-Crypto key storage module
/// @details    IPC server side of KeyProvider provider
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>2022-08-01  <td>1.0.0    <td>HAN YUXIN      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/reuse function/IPC server
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcServer
/// @unit_description=IPC communication server encapsulation
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_SERVER_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_SERVER_H_

#include <ara/core/map.h>
#include <isoft/ipccpp/server.h>
#include <isoft/naicpp/global_evloop.h>

#include <functional>

#include "ara/crypto/ipc/isoft_ipc_auto_message.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief forward declaration
class PKeys_Manager;
//*********************************/
/// @brief IPC server side of KeyProvider provider
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03138
/// @trace_id_dd=DD_CRYPTO_06214
/// @needwork = ad
/// @endcode
class PIpcServer final
{
public:
    /// @brief Handle connection/disconnection messages
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03139
    /// @trace_id_dd=DD_CRYPTO_06215
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealIpcEvent(void* const context,
                                      isoft::ipc::IPCServerHandleType const type,
                                      isoft::ipc::IPCPacket* const pReqPacket) noexcept;
    /// @brief Handle notification messages
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03140
    /// @trace_id_dd=DD_CRYPTO_06216
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealMsgPost(void* const context,
                                     isoft::ipc::IPCServerHandleType const type,
                                     isoft::ipc::IPCPacket* const pReqPacket) noexcept;
    /// @brief Handle request messages
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03141
    /// @trace_id_dd=DD_CRYPTO_06217
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealMsgReq(void* const context,
                                    isoft::ipc::IPCServerHandleType const type,
                                    isoft::ipc::IPCPacket* const pReqPacket) noexcept;

protected:
    /// @brief Full parameter constructor (added when modifying qac)
    /// @param ipcProcessManager KeyProvider provider's IPC server-side key slot manager.
    /// @param pEvLoop Event loop
    /// @param pIpcServer IPC service
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06218
    /// @needwork = dda
    /// @endcode
    PIpcServer(PKeys_Manager& ipcProcessManager,
               std::shared_ptr< ::isoft::naicpp::EvLoop > pEvLoop,
               std::unique_ptr< ::isoft::ipc::IPCServer > pIpcServer) noexcept;

public:
    /// @brief Constructor with parameters
    /// @name PIpcServer
    /// @param ipcProcessManager KeyProvider provider's IPC server-side key slot manager.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03142
    /// @trace_id_dd=DD_CRYPTO_06219
    /// @needwork = ad
    /// @endcode
    explicit PIpcServer(PKeys_Manager& ipcProcessManager) noexcept;
    /// @brief the destructor
    /// @name ~PIpcServer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03143
    /// @trace_id_dd=DD_CRYPTO_06220
    /// @needwork = ad
    /// @endcode
    ~PIpcServer() noexcept = default;
    /// @brief the move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03144
    /// @trace_id_dd=DD_CRYPTO_06221
    /// @needwork = ad
    /// @endcode
    PIpcServer(PIpcServer&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03145
    /// @trace_id_dd=DD_CRYPTO_06222
    /// @needwork = ad
    /// @endcode
    PIpcServer& operator=(PIpcServer&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03146
    /// @trace_id_dd=DD_CRYPTO_06223
    /// @needwork = ad
    /// @endcode
    PIpcServer& operator=(PIpcServer const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03147
    /// @trace_id_dd=DD_CRYPTO_06224
    /// @needwork = ad
    /// @endcode
    PIpcServer(PIpcServer const& other) = delete;
    /// @brief Begin
    /// @name Begin
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03148
    /// @trace_id_dd=DD_CRYPTO_06225
    /// @needwork = ad
    /// @endcode
    bool Begin() noexcept;
    /// @brief Working
    /// @name Working
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03149
    /// @trace_id_dd=DD_CRYPTO_06226
    /// @needwork = ad
    /// @endcode
    bool Working() noexcept;
    /// @brief End
    /// @name End
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03150
    /// @trace_id_dd=DD_CRYPTO_06227
    /// @needwork = ad
    /// @endcode
    bool End() noexcept;
    /// @brief Stop
    /// @name Stop
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03151
    /// @trace_id_dd=DD_CRYPTO_06228
    /// @needwork = ad
    /// @endcode
    int32_t Stop() noexcept;
    /// @brief Handle IPC events
    /// @name ProcessIpcEvent
    /// @param pReqPacket IPC packet
    /// @param type IPC service handle type (IPC event type)
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03152
    /// @trace_id_dd=DD_CRYPTO_06229
    /// @needwork = ad
    /// @endcode
    int32_t ProcessIpcEvent(isoft::ipc::IPCPacket* const pReqPacket,
                            isoft::ipc::IPCServerHandleType const type) noexcept;
    /// @brief Process Ipc Packet
    /// @name ProcessIpcPacket
    /// @param pReqPacket IPC packet
    /// @param type IPC service handle type (IPC event type)
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03153
    /// @trace_id_dd=DD_CRYPTO_06230
    /// @needwork = ad
    /// @endcode
    int32_t ProcessIpcPacket(isoft::ipc::IPCPacket* const pReqPacket,
                             isoft::ipc::IPCServerHandleType const type) noexcept;
    /// @brief Send Ipc Packet
    /// @name SendIpcPacket
    /// @param pReqPacket IPC packet
    /// @param bComplete Whether complete
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03154
    /// @trace_id_dd=DD_CRYPTO_06231
    /// @needwork = ad
    /// @endcode
    int32_t SendIpcPacket(isoft::ipc::IPCPacket* const pReqPacket, bool const bComplete) const noexcept;
    /// @brief New Ipc Packet
    /// @param nSessionID Temporary session ID
    /// @return IPCPacket pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03155
    /// @trace_id_dd=DD_CRYPTO_06232
    /// @needwork = ad
    /// @endcode
    isoft::ipc::IPCPacket* NewIpcPacket(uint64_t const nSessionID) const noexcept;  // Create a new IPC packet

private:
    /// @brief Message loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06233
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::naicpp::EvLoop > pEvLoop_{nullptr};
    /// @brief IPC service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06234
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ::isoft::ipc::IPCServer > pIpcServer_{nullptr};
    /// @brief Key slot manager object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06235
    /// @needwork = dda
    /// @endcode
    PKeys_Manager& ipcProcessManager_;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_SERVER_H_
