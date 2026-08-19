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
/// @file       isoft_svr_io_interface_agent.h
/// @brief      AutoSar-Crypto encryption and decryption shared module
/// @details    Formal interface of IOInterface, used to save and load security objects: implementation version of KV database.
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>2022-04-12  <td>1.0.0    <td>HAN YUXIN      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSvrIoInterfaceAgent
/// @unit_description=Server-side proxy interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_SVR_IO_INTERFACE_AGENT_H_
#define ARA_CRYPTO_KEYS_PUHUA_SVR_IO_INTERFACE_AGENT_H_

#include "ara/crypto/common/isoft_io_agent_index.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem_trust.h"
#include "ara/crypto/common/isoft_thread_lock.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/ksp/isoft_io_interface_kv_share.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {

/// @brief Proxy interface of IOInterface on the IPC server side: could be PIoInterface_Kv (read-only) / PIoInterface_Mem (writable)
/// @brief Memory IO used within the PKeySlotLoader class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02937
/// @trace_id_dd=DD_CRYPTO_05868
/// @needwork = ad
/// @endcode
class PSvrIoInterfaceAgent
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02937
    /// @trace_id_dd=DD_CRYPTO_06537
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSvrIoInterfaceAgent >;

public:
    /// @brief Constructor
    /// @name  PSvrIoInterfaceAgent
    /// @param bWriteModal Whether it is write mode
    /// @param nSlotID Key slot ID
    /// @param pMainKvInterface KV storage IO interface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02938
    /// @trace_id_dd=DD_CRYPTO_05869
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent(bool const bWriteModal,
                         uint32_t const nSlotID,
                         PIoInterface_KvShare* const pMainKvInterface) noexcept;
    /// @brief Full parameter constructor (added for qac)
    /// @param bWriteModal Whether it is write mode
    /// @param nSlotID Key slot ID
    /// @param nIoInterfaceID IO interface ID
    /// @param pMainKvInterface KV storage IO interface
    /// @param pMemInterface Memory IO interface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02939
    /// @trace_id_dd=DD_CRYPTO_05870
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent(bool const bWriteModal,
                         uint32_t const nSlotID,
                         uint32_t const nIoInterfaceID,
                         PIoInterface_KvShare* const pMainKvInterface,
                         PIoInterface_Mem_Server::Uptr pMemInterface) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02940
    /// @trace_id_dd=DD_CRYPTO_05871
    /// @needwork = ad
    /// @endcode
    virtual ~PSvrIoInterfaceAgent() noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02941
    /// @trace_id_dd=DD_CRYPTO_05872
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent(PSvrIoInterfaceAgent const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02942
    /// @trace_id_dd=DD_CRYPTO_05873
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent(PSvrIoInterfaceAgent&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02943
    /// @trace_id_dd=DD_CRYPTO_05874
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent& operator=(PSvrIoInterfaceAgent const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02944
    /// @trace_id_dd=DD_CRYPTO_05875
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent& operator=(PSvrIoInterfaceAgent&& other) = delete;
    /// @brief Get slot ID
    /// @name   GetSlotID
    /// @returns SlotID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02945
    /// @trace_id_dd=DD_CRYPTO_05876
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetSlotID() const noexcept { return nSlotID_; }
    /// @brief Get IO interface ID
    /// @name   GetIoInterfaceID
    /// @returns IoInterfaceID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02946
    /// @trace_id_dd=DD_CRYPTO_05877
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetIoInterfaceID() const noexcept { return nIoInterfaceID_; }
    /// @brief Get IO interface
    /// @name   GetIoInterface
    /// @returns PIoInterface pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02947
    /// @trace_id_dd=DD_CRYPTO_05878
    /// @needwork = ad
    /// @endcode
    PIoInterface* GetIoInterface() const noexcept;

private:
    /// @brief Read/write mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05880
    /// @needwork = dda
    /// @endcode
    bool bWriteModal_{false};
    /// @brief Associated key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05881
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotID_{0U};
    /// @brief Iointerface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05882
    /// @needwork = dda
    /// @endcode
    uint32_t nIoInterfaceID_{0U};
    /// @brief The actual IO interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05883
    /// @needwork = dda
    /// @endcode
    PIoInterface_KvShare* pMainKvInterface_{nullptr};
    /// @brief Memory IO
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05884
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Server::Uptr pMemInterface_{nullptr};
    /// @brief Unique agent ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08786
    /// @needwork = dda
    /// @endcode
    static IoAgentIndex s_IoAgentIndex_;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_SVR_IO_INTERFACE_AGENT_H_
