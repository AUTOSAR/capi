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
/// @file       isoft_keys_process_provider.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of key providers
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key provider IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PSvrProcess_KeyProvider
/// @unit_description=Key provider IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_PROVIDER_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_PROVIDER_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <functional>

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief KeyProvider provider's IPC server side: logical processing of key providers
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02957
/// @trace_id_dd=DD_CRYPTO_05904
/// @needwork = ad
/// @endcode
class PSvrProcess_KeyProvider : public PKeysProcess_T_Base< PSvrProcess_KeyProvider >
{
public:
    /// @brief Constructor with parameters
    /// @name   PSvrProcess_KeyProvider
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02958
    /// @trace_id_dd=DD_CRYPTO_05905
    /// @needwork = ad
    /// @endcode
    explicit PSvrProcess_KeyProvider(PKeys_Manager& lpcProcessManager) noexcept;
    /// @brief Destructor
    /// @name   ~PSvrProcess_KeyProvider
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02959
    /// @trace_id_dd=DD_CRYPTO_05906
    /// @needwork = ad
    /// @endcode
    ~PSvrProcess_KeyProvider() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02960
    /// @trace_id_dd=DD_CRYPTO_05907
    /// @needwork = ad
    /// @endcode
    PSvrProcess_KeyProvider(PSvrProcess_KeyProvider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02961
    /// @trace_id_dd=DD_CRYPTO_05908
    /// @needwork = ad
    /// @endcode
    PSvrProcess_KeyProvider(PSvrProcess_KeyProvider&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02962
    /// @trace_id_dd=DD_CRYPTO_05909
    /// @needwork = ad
    /// @endcode
    PSvrProcess_KeyProvider& operator=(PSvrProcess_KeyProvider const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02963
    /// @trace_id_dd=DD_CRYPTO_05910
    /// @needwork = ad
    /// @endcode
    PSvrProcess_KeyProvider& operator=(PSvrProcess_KeyProvider&& other) = delete;

public:
    /// @brief Load key slot
    /// @name   LoadKeySlot
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns slotID if sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05001
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05911
    /// @needwork = dda
    /// @endcode
    PResultLen LoadKeySlot(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Start transaction
    /// @name   BeginTransaction
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Transaction ID if sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05912
    /// @needwork = dda
    /// @endcode
    PResultLen BeginTransaction(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Commit transaction
    /// @name   CommitTransaction
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05913
    /// @needwork = dda
    /// @endcode
    PResultLen CommitTransaction(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Roll back transaction
    /// @name   RollbackTransaction
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has value if suecess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05914
    /// @needwork = dda
    /// @endcode
    PResultLen RollbackTransaction(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get the pointer to the registered update observer.
    /// @name   GetRegisteredObserver
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns ObserverID if get sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05915
    /// @needwork = dda
    /// @endcode
    PResultLen GetRegisteredObserver(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Consumer registers an update observer.
    /// @name   RegisterObserver
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns pid if sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05916
    /// @needwork = dda
    /// @endcode
    PResultLen RegisterObserver(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Unsubscribe the update observer from change monitoring of the specified slot.
    /// @name   UnsubscribeObserver
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns slotId if sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05917
    /// @needwork = dda
    /// @endcode
    PResultLen UnsubscribeObserver(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Find key slot
    /// @name   FindKeySlot
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has value if find keyslot sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05918
    /// @needwork = dda
    /// @endcode
    PResultLen FindKeySlot(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_PROVIDER_H_
