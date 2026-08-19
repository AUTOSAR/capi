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
/// @file       isoft_keys_process_asymmetric.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of asymmetric encryption
/// @date       2022-08-17
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/asymmetric encryption IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Asymmetric
/// @unit_description=Asymmetric processing logic class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_ASYMMETRIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_ASYMMETRIC_H_

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Encryption/decryption type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03044
/// @trace_id_dd=DD_CRYPTO_06035
/// @needwork = ad
/// @endcode
enum class DoOperate : uint32_t
{
    kDoPublicEncrypt  = 0,  // Public key encryption
    kDoPrivateDecrypt = 1,  // Private key decryption
    kDoPrivateEncrypt = 2,  // Private key encryption
    kDoPublicDecrypt  = 3,  // Public key decryption
};
//********************************/
/// @brief Asymmetric processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03045
/// @trace_id_dd=DD_CRYPTO_06036
/// @needwork = ad
/// @endcode
class PKeysProcess_Asymmetric : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Asymmetric >
{
public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_Asymmetric
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03046
    /// @trace_id_dd=DD_CRYPTO_06037
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Asymmetric(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @name   ~PKeysProcess_Asymmetric
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03047
    /// @trace_id_dd=DD_CRYPTO_06038
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Asymmetric() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03048
    /// @trace_id_dd=DD_CRYPTO_06039
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Asymmetric(PKeysProcess_Asymmetric const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03049
    /// @trace_id_dd=DD_CRYPTO_06040
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Asymmetric(PKeysProcess_Asymmetric &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03050
    /// @trace_id_dd=DD_CRYPTO_06041
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Asymmetric &operator=(PKeysProcess_Asymmetric const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03051
    /// @trace_id_dd=DD_CRYPTO_06042
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Asymmetric &operator=(PKeysProcess_Asymmetric &&other) = delete;
    /// @brief Handle RSA IPC operation function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03051
    /// @trace_id_dd=DD_CRYPTO_06481
    /// @needwork = dd
    /// @endcode
    using CB_RsaLogicIpc = std::function< int32_t(
        uint32_t nDataLen, uint8_t const *pInputData, uint8_t *pOutputData, uint32_t nPadding, RSA *pRsa) >;

    /// @brief Handle RSA operation function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03051
    /// @trace_id_dd=DD_CRYPTO_06482
    /// @needwork = dd
    /// @endcode
    using CB_DoRsa = std::function< ara::core::Vector< ara::core::Byte >(uint32_t nDataLen,
                                                                         uint8_t const *pInputData,
                                                                         uint32_t nPadding,
                                                                         int32_t nRsaDatalen,
                                                                         uint8_t const *pRsaInputData) >;

protected:
    /// @brief Set key
    /// @name   SetKey
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if set key sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06043
    /// @needwork = dda
    /// @endcode
    virtual PResultLen SetKey(keys::isoft_def::PIpcPac_Head const *pReqHead,
                              keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Private key decryption
    /// @name   Private_Decrypt
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Private_Decrypt sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06044
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Private_Decrypt(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                       keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Private key encryption
    /// @name   Private_Encrypt
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Private_Encrypt sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01009
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06045
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Private_Encrypt(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                       keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief PSS private key encryption
    /// @name   Private_Encrypt_Pss
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Private_Encrypt_Pss sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01009
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06046
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Private_Encrypt_Pss(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                           keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Public key decryption
    /// @name   Public_Decrypt
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Public_Decrypt sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01009
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06047
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Public_Decrypt(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief PSS public key decryption
    /// @name   Public_Decrypt_Pss
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Public_Decrypt_Pss sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01009
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06048
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Public_Decrypt_Pss(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                          keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Public key encryption
    /// @name   Public_Encrypt
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Public_Encrypt sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06049
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Public_Encrypt(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief ECC verification
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has vlaue if Ecc_verify sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01010
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06050
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Ecc_verify(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                  keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief ECC signature
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return  has vlaue if Ecc_sign sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01010
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06051
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Ecc_sign(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief IPC-based RSA encryption logic
    /// @name   DoRsaIpc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param operateType Encryption/decryption type 0 public key encryption 1 private key decryption 2 private key encryption 3 public key decryption
    /// @return has vlaue if DoRsaIpc sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06052
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DoRsaIpc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                keys::isoft_def::PIpcAutoPacket &aswMsg,
                                DoOperate operateType) const noexcept;

    /// @brief Key generation logic
    /// @name   Compute_key
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Compute_key sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01013
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06053
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Compute_key(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Unwrap key encryption key (KEK).
    /// @name   DecapsulateKey
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if DecapsulateKey sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_CRYPTO_01015
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06054
    /// @needwork = dda
    /// @endcode
    virtual PResultLen DecapsulateKey(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_ASYMMETRIC_H_
