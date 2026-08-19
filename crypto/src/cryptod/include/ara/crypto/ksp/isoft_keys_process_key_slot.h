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
/// @file       isoft_keys_process_key_slot.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of key slots
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key slot IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_05004
/// @unit_name=PKeysProcess_KeySlot
/// @unit_description=Logical processing of key slots
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_KEY_SLOT_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_KEY_SLOT_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <functional>

#include "ara/crypto/ksp/isoft_keys_process_base.h"
#include "ara/crypto/ksp/isoft_svr_key_slot_loader.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief KeyProvider provider's IPC server side: logical processing of key slots
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03088
/// @trace_id_dd=DD_CRYPTO_06139
/// @needwork = ad
/// @endcode
class PKeysProcess_KeySlot : public PKeysProcess_T_Base< PKeysProcess_KeySlot >
{
public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_KeySlot
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03089
    /// @trace_id_dd=DD_CRYPTO_06140
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_KeySlot(PKeys_Manager& lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @name   ~PKeysProcess_KeySlot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03090
    /// @trace_id_dd=DD_CRYPTO_06141
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_KeySlot() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03091
    /// @trace_id_dd=DD_CRYPTO_06142
    /// @needwork = ad
    /// @endcode
    PKeysProcess_KeySlot(PKeysProcess_KeySlot const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03092
    /// @trace_id_dd=DD_CRYPTO_06143
    /// @needwork = ad
    /// @endcode
    PKeysProcess_KeySlot(PKeysProcess_KeySlot&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03093
    /// @trace_id_dd=DD_CRYPTO_06144
    /// @needwork = ad
    /// @endcode
    PKeysProcess_KeySlot& operator=(PKeysProcess_KeySlot const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03094
    /// @trace_id_dd=DD_CRYPTO_06145
    /// @needwork = ad
    /// @endcode
    PKeysProcess_KeySlot& operator=(PKeysProcess_KeySlot&& other) = delete;

public:
    /// @brief Clear the key slot
    /// @name   Clear
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if clear sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06146
    /// @needwork = dda
    /// @endcode
    PResultLen Clear(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Check whether the key slot is empty
    /// @name   IsEmpty
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if is empty false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06147
    /// @needwork = dda
    /// @endcode
    PResultLen IsEmpty(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get key content attributes
    /// @name   GetKeyContent
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if GetKeyContent sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06148
    /// @needwork = dda
    /// @endcode
    PResultLen GetKeyContent(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get key attributes
    /// @name   GetSlotProps
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  has vlaue if GetSlotProps sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06149
    /// @needwork = dda
    /// @endcode
    PResultLen GetSlotProps(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Open this key slot and return an IOInterface to its content.
    /// @name   Open
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Open sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06150
    /// @needwork = dda
    /// @endcode
    PResultLen Open(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Close read/write of the key slot for a specific process
    /// @name   Close
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Close sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06151
    /// @needwork = dda
    /// @endcode
    PResultLen Close(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Copy the contents of one slot to another key slot
    /// @name   SaveCopy
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if SaveCopy sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06152
    /// @needwork = dda
    /// @endcode
    PResultLen SaveCopy(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Copy a mem iointerface to the key slot
    /// @name   SaveMemory
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if SaveMemory sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06153
    /// @needwork = dda
    /// @endcode
    PResultLen SaveMemory(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get the current provider
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return   has vlaue if MyProvider sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06154
    /// @needwork = dda
    /// @endcode
    PResultLen MyProvider(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Find key slot attributes from the configuration file
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has vlaue if GetKeySlotPropsFromJson sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06155
    /// @needwork = dda
    /// @endcode
    PResultLen GetKeySlotPropsFromJson(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Reset key slot information (clear key slot content) using key slot attribute information from the configuration file
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has vlaue if ResetKeySlot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06156
    /// @needwork = dda
    /// @endcode
    PResultLen ResetKeySlot(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;

protected:
    /// @brief Find a slotloader object
    /// @name   _FindSlotLoaderByReqHead
    /// @param pReqHead IPC request packet header
    /// @returns PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06157
    /// @needwork = dda
    /// @endcode
    PSvrKeySlotLoader* _FindSlotLoaderByReqHead(PIpcPac_Head const* const pReqHead) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_KEY_SLOT_H_
