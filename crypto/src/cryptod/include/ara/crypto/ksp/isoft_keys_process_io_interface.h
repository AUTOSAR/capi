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
/// @file       isoft_keys_process_io_interface.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: IO interface
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_IoInterface
/// @unit_description=IO interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_IO_INTERFACE_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_IO_INTERFACE_H_

#include <ara/core/result.h>

#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief KeyProvider provider's IPC server side: IO interface
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03067
/// @trace_id_dd=DD_CRYPTO_06079
/// @needwork = ad
/// @endcode
class PKeysProcess_IoInterface : public PKeysProcess_T_Base< PKeysProcess_IoInterface >
{
public:
    /// @brief Constructor with parameters
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03068
    /// @trace_id_dd=DD_CRYPTO_06080
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_IoInterface(PKeys_Manager& lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03069
    /// @trace_id_dd=DD_CRYPTO_06081
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_IoInterface() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03070
    /// @trace_id_dd=DD_CRYPTO_06082
    /// @needwork = ad
    /// @endcode
    PKeysProcess_IoInterface(PKeysProcess_IoInterface const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03071
    /// @trace_id_dd=DD_CRYPTO_06083
    /// @needwork = ad
    /// @endcode
    PKeysProcess_IoInterface(PKeysProcess_IoInterface&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03072
    /// @trace_id_dd=DD_CRYPTO_06084
    /// @needwork = ad
    /// @endcode
    PKeysProcess_IoInterface& operator=(PKeysProcess_IoInterface const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03073
    /// @trace_id_dd=DD_CRYPTO_06085
    /// @needwork = ad
    /// @endcode
    PKeysProcess_IoInterface& operator=(PKeysProcess_IoInterface&& other) = delete;

public:  // Methods corresponding to PIoInterface_Ipc class
    /// @brief Return the actual allowed key/seed usage flags as defined by the key slot prototype and the current content of the container for this "Actor".
    /// @name   GetAllowedUsage
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Key/seed usage flags
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06086
    /// @needwork = dda
    /// @endcode
    PResultLen GetAllowedUsage(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the capacity of the underlying resource.
    /// @name   GetCapacity
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Capacity of the underlying resource
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06087
    /// @needwork = dda
    /// @endcode
    PResultLen GetCapacity(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the cryptobjecttype of the object referenced by this IOInterface.
    /// @name   GetCryptoObjectType
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Cryptobjecttype of the referenced object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06088
    /// @needwork = dda
    /// @endcode
    PResultLen GetCryptoObjectType(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the COUID of the object stored in the IOInterface.
    /// @name   GetObjectId
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns COUID of the object stored in the IOInterface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06089
    /// @needwork = dda
    /// @endcode
    PResultLen GetObjectId(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the size of the object payload stored in the underlying buffer of the IOInterface.
    /// @name   GetPayloadSize
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the object payload in the underlying buffer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06090
    /// @needwork = dda
    /// @endcode
    PResultLen GetPayloadSize(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get the vendor-specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Vendor-specific ID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06091
    /// @needwork = dda
    /// @endcode
    PResultLen GetPrimitiveId(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the content type restriction of this interface (identifying what type of data this container can hold).
    /// @name   GetTypeRestriction
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Content type restriction of the interface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06092
    /// @needwork = dda
    /// @endcode
    PResultLen GetTypeRestriction(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
    /// @name   IsObjectExportable
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if can Exportable false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06093
    /// @needwork = dda
    /// @endcode
    PResultLen IsObjectExportable(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the set "session" (or "temporary") attribute of the object
    /// @name   IsObjectSession
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if is Session Object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06094
    /// @needwork = dda
    /// @endcode
    PResultLen IsObjectSession(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Return the set "session" (or "temporary") attribute of the object
    /// @name   IsVolatile
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if is Volatile Object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06095
    /// @needwork = dda
    /// @endcode
    PResultLen IsVolatile(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get whether the underlying key database is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface will become invalid.
    /// @name   IsValid
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  true if is Valid Object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06096
    /// @needwork = dda
    /// @endcode
    PResultLen IsValid(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
    /// @name   IsWritable
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if is Writable Object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06097
    /// @needwork = dda
    /// @endcode
    PResultLen IsWritable(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;

protected:
    /// @brief Find IOInterface
    /// @param pReqHead IPC request packet header
    /// @return PIoInterface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06098
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PIoInterface* >
    /// @brief Find IOInterface
    /// @param pReqHead IPC request packet header
    /// @return PIoInterface pointer
    _FindIoInterface(PIpcPac_Head const* const pReqHead) const noexcept;
    /// @brief Processing logic
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param data Template type: data
    /// @return Template generic function return value
    /// @code{.isoft}
    /// @tparam T_Type
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06099
    /// @needwork = dda
    /// @endcode
    template < typename T_Type >
    inline static PResultLen DealLogic(PIpcPac_Head const* const pReqHead,
                                       PIpcAutoPacket& aswMsg,
                                       T_Type const& data) noexcept;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_IO_INTERFACE_H_
