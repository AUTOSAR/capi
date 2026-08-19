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
/// @file       isoft_keys_process_public.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing related to public keys for asymmetric encryption
/// @date       2024-07-15
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/asymmetric public key IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Public
/// @unit_description=Asymmetric public key IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_PUBLIC_H_

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Public key processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03081
/// @trace_id_dd=DD_CRYPTO_06128
/// @needwork = ad
/// @endcode
class PKeysProcess_Public : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Public >
{
public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_Public
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03082
    /// @trace_id_dd=DD_CRYPTO_06129
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Public(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @name   ~PKeysProcess_Public
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03083
    /// @trace_id_dd=DD_CRYPTO_06130
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Public() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03084
    /// @trace_id_dd=DD_CRYPTO_06131
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Public(PKeysProcess_Public const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03085
    /// @trace_id_dd=DD_CRYPTO_06132
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Public(PKeysProcess_Public &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03086
    /// @trace_id_dd=DD_CRYPTO_06133
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Public &operator=(PKeysProcess_Public const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03087
    /// @trace_id_dd=DD_CRYPTO_06134
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Public &operator=(PKeysProcess_Public &&other) = delete;

protected:
    /// @brief Export RSA public key
    /// @name   ExportPublicKeyPublicly_Rsa
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if export pulickey sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06135
    /// @needwork = dda
    /// @endcode
    virtual PResultLen ExportPublicKeyPublicly_Rsa(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Export ECC public key
    /// @name   ExportPublicKeyPublicly_Ecc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if export pulickey sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06136
    /// @needwork = dda
    /// @endcode
    virtual PResultLen ExportPublicKeyPublicly_Ecc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Get the size of the ECC public key object payload
    /// @name   GetPublicKeyPayloadSize_Ecc
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the public key object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06137
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPublicKeyPayloadSize_Ecc(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Get the size of the RSA public key object payload
    /// @name   GetPublicKeyPayloadSize_Rsa
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the public key object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06138
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPublicKeyPayloadSize_Rsa(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  //
