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
/// @file       isoft_ipc_serverx509.h
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
/// <tr>2022-08-01  <td>1.0.0      <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/reuse function/IPC server side
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcServerX509
/// @unit_description=Certificate IPC server side
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_SERVERX509_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_SERVERX509_H_

#include <ara/core/map.h>
#include <isoft/ipccpp/server.h>
#include <isoft/naicpp/global_evloop.h>

#include <functional>

#include "ara/crypto/ipc/isoft_ipc_auto_message.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief X509 certificate manager
class PX509_Manager;
//********************************/
/// @brief IPC server side of KeyProvider provider
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02882
/// @trace_id_dd=DD_CRYPTO_05795
/// @needwork = ad
/// @endcode
class PIpcServerX509 final
{
public:
    /// @brief IPC message processing callback function: event
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02883
    /// @trace_id_dd=DD_CRYPTO_05796
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealIpcEvent(void const* const context,
                                      isoft::ipc::IPCServerHandleType const type,
                                      isoft::ipc::IPCPacket const* const pReqPacket) noexcept;
    /// @brief IPC message processing callback function: POST message
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02884
    /// @trace_id_dd=DD_CRYPTO_05797
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealMsgPost(void* const context,
                                     isoft::ipc::IPCServerHandleType const type,
                                     isoft::ipc::IPCPacket* const pReqPacket) noexcept;
    /// @brief IPC message processing callback function: REQ message
    /// @param context IPC message context information
    /// @param type IPC service handle type (IPC event type)
    /// @param pReqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02885
    /// @trace_id_dd=DD_CRYPTO_05798
    /// @needwork = ad
    /// @endcode
    static void CallBack_DealMsgReq(void* const context,
                                    isoft::ipc::IPCServerHandleType const type,
                                    isoft::ipc::IPCPacket* const pReqPacket) noexcept;

public:
    /// @brief Constructor with parameters
    /// @name   PIpcServerX509
    /// @param ipcProcessManager KeyProvider provider's IPC server-side key slot manager.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02886
    /// @trace_id_dd=DD_CRYPTO_05799
    /// @needwork = ad
    /// @endcode
    explicit PIpcServerX509(PX509_Manager& ipcProcessManager) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02887
    /// @trace_id_dd=DD_CRYPTO_05800
    /// @needwork = ad
    /// @endcode
    ~PIpcServerX509() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02888
    /// @trace_id_dd=DD_CRYPTO_05801
    /// @needwork = ad
    /// @endcode
    PIpcServerX509(PIpcServerX509&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02889
    /// @trace_id_dd=DD_CRYPTO_05802
    /// @needwork = ad
    /// @endcode
    PIpcServerX509& operator=(PIpcServerX509&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02890
    /// @trace_id_dd=DD_CRYPTO_05803
    /// @needwork = ad
    /// @endcode
    PIpcServerX509& operator=(PIpcServerX509 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02891
    /// @trace_id_dd=DD_CRYPTO_05804
    /// @needwork = ad
    /// @endcode
    PIpcServerX509(PIpcServerX509 const& other) = delete;
    /// @brief Preparations before starting the service
    /// @name Begin
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02892
    /// @trace_id_dd=DD_CRYPTO_05805
    /// @needwork = ad
    /// @endcode
    bool Begin() noexcept;
    /// @brief Start the service
    /// @name Working
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02893
    /// @trace_id_dd=DD_CRYPTO_05806
    /// @needwork = ad
    /// @endcode
    bool Working() noexcept;
    /// @brief Cleanup work before ending the service
    /// @name End
    /// @returns true or false
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02894
    /// @trace_id_dd=DD_CRYPTO_05807
    /// @needwork = ad
    /// @endcode
    bool End() noexcept;
    /// @brief Stop the service
    /// @name Stop
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02895
    /// @trace_id_dd=DD_CRYPTO_05808
    /// @needwork = ad
    /// @endcode
    int32_t Stop() noexcept;
    /// @brief Handle IPC packet
    /// @name   ProcessIpcPacket
    /// @param pReqPacket IPC packet
    /// @param type IPC service handle type (IPC event type)
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02896
    /// @trace_id_dd=DD_CRYPTO_05809
    /// @needwork = ad
    /// @endcode
    int32_t ProcessIpcPacket(isoft::ipc::IPCPacket* const pReqPacket,
                             isoft::ipc::IPCServerHandleType const type) noexcept;
    /// @brief Send IPC packet
    /// @name   SendIpcPacket
    /// @param pReqPacket IPC packet
    /// @param bComplete Whether complete
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02897
    /// @trace_id_dd=DD_CRYPTO_05810
    /// @needwork = ad
    /// @endcode
    int32_t SendIpcPacket(isoft::ipc::IPCPacket* const pReqPacket, bool const bComplete) const noexcept;
    /// @brief Create a new IPC packet
    /// @param nSessionID Temporary session ID
    /// @return IPCPacket pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02898
    /// @trace_id_dd=DD_CRYPTO_05811
    /// @needwork = ad
    /// @endcode
    isoft::ipc::IPCPacket* NewIpcPacket(uint64_t const nSessionID) const noexcept;

private:
    /// @brief Event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05812
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::naicpp::EvLoop > pEvLoop_{};
    /// @brief IPC server pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05813
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ::isoft::ipc::IPCServer > pIpcServer_{};
    /// @brief Certificate manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05814
    /// @needwork = dda
    /// @endcode
    PX509_Manager& ipcProcessManager_;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_SERVERX509_H_
