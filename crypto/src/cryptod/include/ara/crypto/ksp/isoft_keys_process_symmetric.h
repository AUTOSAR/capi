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
/// @file       isoft_keys_process_symmetric.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of symmetric encryption
/// @date       2022-08-17
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/symmetric encryption IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PKeysProcess_Symmetric
/// @unit_description=Symmetric encryption IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SYMMETRIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SYMMETRIC_H_

#include <functional>

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Symmetric encryption processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03074
/// @trace_id_dd=DD_CRYPTO_06100
/// @needwork = ad
/// @endcode
class PKeysProcess_Symmetric : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Symmetric >
{
public:
    /// @brief Constructor
    /// @name   PKeysProcess_Symmetric
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03075
    /// @trace_id_dd=DD_CRYPTO_06101
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Symmetric(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @name   ~PKeysProcess_Symmetric
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03076
    /// @trace_id_dd=DD_CRYPTO_06102
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Symmetric() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03077
    /// @trace_id_dd=DD_CRYPTO_06103
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Symmetric(PKeysProcess_Symmetric const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03078
    /// @trace_id_dd=DD_CRYPTO_06104
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Symmetric(PKeysProcess_Symmetric &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03079
    /// @trace_id_dd=DD_CRYPTO_06105
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Symmetric &operator=(PKeysProcess_Symmetric const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03080
    /// @trace_id_dd=DD_CRYPTO_06106
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Symmetric &operator=(PKeysProcess_Symmetric &&other) = delete;

protected:
    /// @brief Execute encryption logic
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param block Whether it is block encryption
    /// @return Common logic processing return has value if DoCipher success
    /// @code{.isoft}
    /// @tparam T_ClassType
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06107
    /// @needwork = dda
    /// @endcode
    template < typename T_ClassType >
    inline PResultLen _DoCipher_Common(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                                       keys::isoft_def::PIpcAutoPacket &aswMsg,
                                       bool const block = true) const noexcept;

protected:
    /// @brief Set key
    /// @name   SetKey
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if SetKey sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06108
    /// @needwork = dda
    /// @endcode
    virtual PResultLen SetKey(keys::isoft_def::PIpcPac_Head const *pReqHead,
                              keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES ECB encryption logic
    /// @name   DoCipher_Aes_Ecb
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06109
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Ecb(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES CBC encryption logic
    /// @name   DoCipher_Aes_Cbc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06110
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Cbc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES ECB encryption logic
    /// @name   DoCipher_Des_Ecb
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06111
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Ecb(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES CBC encryption logic
    /// @name   DoCipher_Des_Cbc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06112
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Cbc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief 3DES ECB encryption logic
    /// @name   DoCipher_3Des_Ecb
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06113
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_3Des_Ecb(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                         keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief 3DES CBC encryption logic
    /// @name   DoCipher_3Des_Cbc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01003
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06114
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_3Des_Cbc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                         keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES CFB64 stream encryption logic
    /// @name   DoCipher_Des_Ofb64_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06115
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Ofb64_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                 keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES OFB stream encryption logic
    /// @name   DoCipher_Des_Ofb_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06116
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Ofb_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                               keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES CTR stream encryption logic
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06117
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Ctr_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                               keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES CFB1 stream encryption logic
    /// @name   DoCipher_Aes_Cfb1_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06118
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Cfb1_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES CFB8 stream encryption logic
    /// @name   DoCipher_Aes_Cfb8_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06119
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Cfb8_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES CFB128 stream encryption logic
    /// @name   DoCipher_Aes_Cfb128_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06120
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Cfb128_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                  keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief AES OFB128 stream encryption logic
    /// @name   DoCipher_Aes_Ofb128_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01002
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06121
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Aes_Ofb128_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                  keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES CFB64 stream encryption logic
    /// @name   DoCipher_Des_Cfb64_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06122
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Cfb64_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                 keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief DES CFB stream encryption logic
    /// @name   DoCipher_Des_Cfb_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06123
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_Des_Cfb_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                               keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief 3DES CFB1 stream encryption logic
    /// @name   DoCipher_3Des_Cfb1_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06124
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_3Des_Cfb1_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                 keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief 3DES CFB64 stream encryption logic
    /// @name   DoCipher_3Des_Cfb64_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06125
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_3Des_Cfb64_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                  keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief 3DES OFB64 stream encryption logic
    /// @name   DoCipher_3Des_Ofb64_Stream
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DoCipher sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06126
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoCipher_3Des_Ofb64_Stream(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                  keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Return the size of the object payload stored in the underlying buffer of the IOInterface.
    /// @name   GetPayloadSize
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the object payload in the underlying buffer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06127
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPayloadSize(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SYMMETRIC_H_