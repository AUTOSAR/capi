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
/// @file       isoft_keys_process_private.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing related to private keys for asymmetric encryption
/// @date       2022-08-17
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/asymmetric private key IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Private
/// @unit_description=Asymmetric private key IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_PRIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_PRIVATE_H_

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Private key processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02998
/// @trace_id_dd=DD_CRYPTO_05977
/// @needwork = ad
/// @endcode
class PKeysProcess_Private : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Private >
{
public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_Private
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02999
    /// @trace_id_dd=DD_CRYPTO_05978
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Private(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @name   ~PKeysProcess_Private
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03000
    /// @trace_id_dd=DD_CRYPTO_05979
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Private() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03001
    /// @trace_id_dd=DD_CRYPTO_05980
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Private(PKeysProcess_Private const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03002
    /// @trace_id_dd=DD_CRYPTO_05981
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Private(PKeysProcess_Private &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03003
    /// @trace_id_dd=DD_CRYPTO_05982
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Private &operator=(PKeysProcess_Private const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03004
    /// @trace_id_dd=DD_CRYPTO_05983
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Private &operator=(PKeysProcess_Private &&other) = delete;

protected:
    /// @brief Get RSA public key
    /// @name   GetPublicKey_Rsa
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has value if get publickey sucess else fail
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05984
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPublicKey_Rsa(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Get ECC public key
    /// @name   GetPublicKey_Ecc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has value if get publickey sucess else fail
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05985
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPublicKey_Ecc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Get the size of the ECC object payload
    /// @name   GetPayloadSize_Ecc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the ECC object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05986
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPayloadSize_Ecc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                          keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Get the size of the RSA object payload
    /// @name   GetPayloadSize_Rsa
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the RSA object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05987
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPayloadSize_Rsa(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                          keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Export RSA private key
    /// @name   ExportPrivateKeyPublicly_Rsa
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if Export PrivateKey sucess else fail
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05988
    /// @needwork = dda
    /// @endcode
    virtual PResultLen ExportPrivateKeyPublicly_Rsa(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                    keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Export ECC private key
    /// @name   ExportPrivateKeyPublicly_Ecc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has value if Export PrivateKey sucess else fail
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05989
    /// @needwork = dda
    /// @endcode
    virtual PResultLen ExportPrivateKeyPublicly_Ecc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                    keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Check whether the key is correct
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return 1 check sucess 0 fail
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05990
    /// @needwork = dda
    /// @endcode
    virtual PResultLen CheckKey(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_ASYMMETRIC_H_
