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
/// @file       isoft_keys_process_wrap.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of key wrapping
/// @date       2022-08-17
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key wrapping IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PKeysProcess_Wrap
/// @unit_description=Key wrapping IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_WRAP_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_WRAP_H_

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Wrapping operation type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02964
/// @trace_id_dd=DD_CRYPTO_05919
/// @needwork = ad
/// @endcode
enum class DoOperateWrap : uint32_t
{
    kDoUnwrap_Pad   = 0,
    kDoWrap_Pad     = 1,
    kDoUnwrap_UnPad = 2,
    kDoWrap_UnPad   = 3,
    kDoUnwrapDes    = 4,
    kDoWrapDes      = 5,
};
//********************************/
/// @brief Key wrapping logical processing class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02965
/// @trace_id_dd=DD_CRYPTO_05920
/// @needwork = ad
/// @endcode
class PKeysProcess_Wrap : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Wrap >
{
public:
    /// @brief Constructor
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02966
    /// @trace_id_dd=DD_CRYPTO_05921
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Wrap(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02967
    /// @trace_id_dd=DD_CRYPTO_05922
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Wrap() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02968
    /// @trace_id_dd=DD_CRYPTO_05923
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Wrap(PKeysProcess_Wrap const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02969
    /// @trace_id_dd=DD_CRYPTO_05924
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Wrap(PKeysProcess_Wrap &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02970
    /// @trace_id_dd=DD_CRYPTO_05925
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Wrap &operator=(PKeysProcess_Wrap const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02971
    /// @trace_id_dd=DD_CRYPTO_05926
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Wrap &operator=(PKeysProcess_Wrap &&other) = delete;

protected:
    /// @brief Unwrap padded key operation
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has value if  do aes pad unwrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05927
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoUnwrapKey_Pad(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                       keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Wrap padded key operation
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if  do aes pad wrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05928
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoWrapKeyMaterial_Pad(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                             keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Unwrap non-padded key operation
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if  do aes unpad unwrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05929
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoUnwrapKey_Unpad(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                         keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Wrap non-padded key operation
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if  do aes unpad wrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05930
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoWrapKeyMaterial_Unpad(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                               keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Unwrap based on DES key
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if do des unwrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01019
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05931
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoUnwrapDesKey(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Wrap based on DES key
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if do des wrap sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01019
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05932
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoWrapDesKeyMaterial(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                            keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Common logic processing for wrap operations
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param doOperator Wrapping operation type
    /// @return has value if do Operator sucess
    /// @code{.isoft}
    /// @tparam T_ClassType
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05933
    /// @needwork = dda
    /// @endcode
    template < typename T_ClassType >
    inline PResultLen _DoWrapOperator(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg,
                                      DoOperateWrap const doOperator) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif
