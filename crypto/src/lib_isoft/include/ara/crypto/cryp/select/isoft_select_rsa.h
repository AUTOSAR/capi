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
/// @file       isoft_select_rsa.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    RSA algorithm wrapper for Select
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSelectRsa
/// @unit_description=RSA Encryption/Decryption Algorithm Wrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SELECT_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_SELECT_RSA_H_

#include <openssl/rsa.h>

#include <functional>

#include "ara/core/vector.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief SelectBUFF length enumeration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00246
/// @trace_id_dd=DD_CRYPTO_01131
/// @needwork = ad
/// @endcode
enum class ESelectLength : std::uint32_t
{
    kMaxInputBuffLen  = 4096,
    kMaxOutputBuffLen = 1024,
};
/// @brief RSA algorithm wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00247
/// @trace_id_dd=DD_CRYPTO_01132
/// @needwork = ad
/// @endcode
class PSelectRsa
{
public:
    /// @brief RSA logic callback function
    /// @name   CB_RsaLogic
    /// @param nDataLen Data length
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param pOutputData Starting address of output result memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00247
    /// @trace_id_dd=DD_CRYPTO_06263
    /// @needwork = dd
    /// @endcode
    using CB_RsaLogic = std::function< int32_t(uint32_t nDataLen, uint8_t const* pInputData, uint8_t* pOutputData) >;
    /// @brief Return result type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00247
    /// @trace_id_dd=DD_CRYPTO_06264
    /// @needwork = dd
    /// @endcode
    using VecResult = ara::core::Result< ara::core::Vector< ara::core::Byte > >;
    /// @brief Alias for BOOL return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06265
    /// @needwork = dda
    /// @endcode
    using BoolResult = ara::core::Result< bool >;
    // enum    {   kMaxInputBuffLen_   = 1024 * 4, kMaxOutputBuffLen_   = 1024, };c
private:
    /// @brief Output BUFF
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01133
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff* pBuffOutput_;
    /// @brief RSA key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01134
    /// @needwork = dda
    /// @endcode
    EVP_PKEY* pRsaKey_;
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01135
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotId_;

public:
    /// @brief Parameterized constructor
    /// @name PSelectRsa
    /// @param nSlotId Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01136
    /// @needwork = dda
    /// @endcode
    explicit PSelectRsa(uint32_t const nSlotId) noexcept;
    /// @brief Parameterized constructor
    /// @name PSelectRsa
    /// @param pRsaKey RSA key resource
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01137
    /// @needwork = dda
    /// @endcode
    explicit PSelectRsa(EVP_PKEY* const pRsaKey) noexcept;
    /// @brief Parameterized constructor
    /// @param pBuffOutput Output buffer
    /// @param pRsaKey RSA key resource
    /// @param nSlotId Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01138
    /// @needwork = dda
    /// @endcode
    explicit PSelectRsa(internal::PAutoBuff* const pBuffOutput,
                        EVP_PKEY* const pRsaKey,
                        uint32_t const nSlotId) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01139
    /// @needwork = dda
    /// @endcode
    virtual ~PSelectRsa() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01140
    /// @needwork = dda
    /// @endcode
    PSelectRsa(PSelectRsa&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01141
    /// @needwork = dda
    /// @endcode
    PSelectRsa& operator=(PSelectRsa&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01142
    /// @needwork = dda
    /// @endcode
    PSelectRsa& operator=(PSelectRsa const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01143
    /// @needwork = dda
    /// @endcode
    PSelectRsa(PSelectRsa const& other) = delete;
    /// @brief Set external Buff
    /// @name   AttachOutputBuff
    /// @param pBuffOutput Output buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01144
    /// @needwork = dda
    /// @endcode
    inline void AttachOutputBuff(internal::PAutoBuff* const pBuffOutput) noexcept;
    /// @brief Execute RSA logic
    /// @name   DoRsaLogic
    /// @param bEncrypto Whether to encrypt
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param nPadding Data padding scheme
    /// @param cbFun Logic execution callback function
    /// @return Encryption/decryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01145
    /// @needwork = dda
    /// @endcode
    VecResult DoRsaLogic(bool const bEncrypto,
                         uint8_t const* const pInputData,
                         uint32_t const nDataLen,
                         int32_t const nPadding,
                         CB_RsaLogic const& cbFun) const noexcept;
    /// @brief Private key encryption
    /// @name   PrivateEncrypto
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nPadding Data padding scheme
    /// @returns Private key encryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01146
    /// @needwork = dda
    /// @endcode
    VecResult PrivateEncrypto(uint8_t const* const pInputData,
                              uint32_t const nInDataLen,
                              int32_t const nPadding) noexcept;
    /// @brief Local execution of private key encryption_Pss
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nInHashId Hash algorithm crypto primitive ID
    /// @param nInSaltLen Salt data length
    /// @return PSS private key encryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01147
    /// @needwork = dda
    /// @endcode
    VecResult PrivateEncryptoPssLocal(uint8_t const* const pInputData,
                                      uint32_t const nInDataLen,
                                      uint32_t const nInHashId,
                                      uint32_t const nInSaltLen) noexcept;
    /// @brief IPC execution of private key encryption_Pss
    /// @name   PrivateEncryptoPssIpc
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nInHashId Hash algorithm crypto primitive ID
    /// @param nInSaltLen Salt data length
    /// @returns PSS private key encryption result via IPC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01148
    /// @needwork = dda
    /// @endcode
    VecResult PrivateEncryptoPssIpc(uint8_t const* const pInputData,
                                    uint32_t const nInDataLen,
                                    uint32_t const nInHashId,
                                    uint32_t const nInSaltLen) noexcept;
    /// @brief Private key decryption
    /// @name   PrivateDecrypto
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nPadding Data padding scheme
    /// @returns Private key decryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01149
    /// @needwork = dda
    /// @endcode
    VecResult PrivateDecrypto(uint8_t const* const pInputData,
                              uint32_t const nInDataLen,
                              int32_t const nPadding) noexcept;
    /// @brief Public key encryption
    /// @name   PublicEncrypto
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nPadding Data padding scheme
    /// @returns Public key encryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01150
    /// @needwork = dda
    /// @endcode
    VecResult PublicEncrypto(uint8_t const* const pInputData,
                             uint32_t const nInDataLen,
                             int32_t const nPadding) noexcept;
    /// @brief Public key decryption
    /// @name   PublicDecrypto
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nPadding Data padding scheme
    /// @returns Public key decryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01151
    /// @needwork = dda
    /// @endcode
    VecResult PublicDecrypto(uint8_t const* const pInputData,
                             uint32_t const nInDataLen,
                             int32_t const nPadding) noexcept;
    /// @brief Public key decryption_Pss
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nInHashId Hash algorithm crypto primitive ID
    /// @param nInSaltLen Salt data length
    /// @param pInDigestData Digest data
    /// @param nInDigestDataLen Digest data length
    /// @return PSS public key decryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01152
    /// @needwork = dda
    /// @endcode
    BoolResult PublicDecryptoPssLocal(uint8_t const* const pInputData,
                                      uint32_t const nInDataLen,
                                      uint32_t const nInHashId,
                                      uint32_t const nInSaltLen,
                                      uint8_t const* const pInDigestData,
                                      uint32_t const nInDigestDataLen) noexcept;
    /// @brief Public key decryption_Pss (overload)
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nInHashId Hash algorithm crypto primitive ID
    /// @param nInSaltLen Salt data length
    /// @param pInDigestData Digest data
    /// @param nInDigestDataLen Digest data length
    /// @return PSS public key decryption result via IPC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01153
    /// @needwork = dda
    /// @endcode
    BoolResult PublicDecryptoPssIpc(uint8_t const* const pInputData,
                                    uint32_t const nInDataLen,
                                    uint32_t const nInHashId,
                                    uint32_t const nInSaltLen,
                                    uint8_t const* const pInDigestData,
                                    uint32_t const nInDigestDataLen) noexcept;
    /// @brief Get encryption block length
    /// @name   GetBlockSize
    /// @returns Encryption block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01154
    /// @needwork = dda
    /// @endcode
    uint32_t GetBlockSize() const noexcept;
    /// @brief Get RSA
    /// @returns RSA pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01155
    /// @needwork = dda
    /// @endcode
    EVP_PKEY* GetRSA() const noexcept;

protected:
    /// @brief Get key modulus length: Different Padding schemes correspond to different values in RSA.
    /// @name   _GetModulusSize
    /// @param nPadding Data padding scheme
    /// @returns Key modulus length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01156
    /// @needwork = dda
    /// @endcode
    int32_t _GetModulusSize(int32_t const nPadding) const noexcept;
    /// @brief Execute RSA logic
    /// @name   _DoRsaLogic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param nModulusSize Block size for large data segmentation
    /// @param cbFun Logic execution callback function
    /// @returns RSA logic processing result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01157
    /// @needwork = dda
    /// @endcode
    VecResult _DoRsaLogic(uint8_t const* const pInputData,
                          uint32_t const nDataLen,
                          int32_t const nModulusSize,
                          CB_RsaLogic const& cbFun) const noexcept;
    /// @brief Execute RSA logic via IPC
    /// @param stFuncName Function name used for IPC call
    /// @param slotId Key slot ID
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nInDataLen Input data length for algorithm operation
    /// @param nPadding Data padding scheme
    /// @return RSA logic processing result via IPC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01158
    /// @needwork = dda
    /// @endcode
    static VecResult DoRsaLogicIpc(ara::core::StringView const& stFuncName,
                                   uint32_t const slotId,
                                   uint8_t const* const pInputData,
                                   uint32_t const nInDataLen,
                                   int32_t const nPadding) noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SELECT_RSA_H_
