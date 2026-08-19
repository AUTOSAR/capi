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
/// @file       isoft_keys_process_secret_seed.h
/// @brief      AutoSar-Crypto secret seed module
/// @details    KeyProvider provider's IPC server side: logical processing of symmetric encryption
/// @date       2023-09-06
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/secret seed IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeysProcess_SecretSeed
/// @unit_description=Secret seed IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SECRET_SEED_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SECRET_SEED_H_

#include <functional>

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Seed operation type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03059
/// @trace_id_dd=DD_CRYPTO_06064
/// @needwork = ad
/// @endcode
enum class DoOperateSecretSeed : uint32_t
{
    kDoClone       = 0,  // Clone operation
    kDoJumpFrom    = 1,  // Jump operation
    kDoJump        = 2,  // Jump operation
    kDoNext        = 3,  // Next operation
    kDoOperatorXor = 4,  // XOR operation
};
//********************************/
/// @brief Seed processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03060
/// @trace_id_dd=DD_CRYPTO_06065
/// @needwork = ad
/// @endcode
class PKeysProcess_SecretSeed : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_SecretSeed >
{
public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_SecretSeed
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03061
    /// @trace_id_dd=DD_CRYPTO_06066
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_SecretSeed(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03062
    /// @trace_id_dd=DD_CRYPTO_06067
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_SecretSeed() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03063
    /// @trace_id_dd=DD_CRYPTO_06068
    /// @needwork = ad
    /// @endcode
    PKeysProcess_SecretSeed(PKeysProcess_SecretSeed const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03064
    /// @trace_id_dd=DD_CRYPTO_06069
    /// @needwork = ad
    /// @endcode
    PKeysProcess_SecretSeed(PKeysProcess_SecretSeed &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03065
    /// @trace_id_dd=DD_CRYPTO_06070
    /// @needwork = ad
    /// @endcode
    PKeysProcess_SecretSeed &operator=(PKeysProcess_SecretSeed const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03066
    /// @trace_id_dd=DD_CRYPTO_06071
    /// @needwork = ad
    /// @endcode
    PKeysProcess_SecretSeed &operator=(PKeysProcess_SecretSeed &&other) = delete;

protected:
    /// @brief Clone this SecretSeed object to a new session object.
    /// @name   Clone
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06072
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Clone(keys::isoft_def::PIpcPac_Head const *pReqHead,
                             keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Set the value of this seed object to "jump" from the initial state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
    ///         Steps may have positive and negative values, corresponding to forward and backward directions of the "jump", but a value of 0 means only copying the value to this seed object. The seed size of the from parameter must be greater than or equal to this seed size.
    /// @name   JumpFrom
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06073
    /// @needwork = dda
    /// @endcode
    virtual PResultLen JumpFrom(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Set the value of this seed object to "jump" from its current state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
    ///         Steps may have positive and negative values, corresponding to forward and backward directions of the "jump", but a value of 0 means the current seed value is unchanged.
    /// @name   Jump
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06074
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Jump(keys::isoft_def::PIpcPac_Head const *pReqHead,
                            keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Set the next value of the secret seed according to the "count" expression defined by the cryptographic algorithm associated with this object.
    ///        If the associated cryptographic algorithm does not specify a "count" expression, then a generic increment operation must be used as the default implementation (little-endian representation, i.e., the first byte is the least significant).
    /// @name   Next
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06075
    /// @needwork = dda
    /// @endcode
    virtual PResultLen Next(keys::isoft_def::PIpcPac_Head const *pReqHead,
                            keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief XOR this seed object with another seed object and save the result to this object.
    ///      If the seed sizes of this object and the source parameter are different, only the corresponding leading bytes in this seed object should be updated.
    /// @name   OperatorXor
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06076
    /// @needwork = dda
    /// @endcode
    virtual PResultLen OperatorXor(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                   keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Return the size of the object payload stored in the underlying buffer of the IOInterface.
    /// @name   GetPayloadSize
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns Size of the object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06077
    /// @needwork = dda
    /// @endcode
    virtual PResultLen GetPayloadSize(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Common method for seed operations
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param doOperator Wrapper operation type
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06078
    /// @needwork = dda
    /// @endcode
    virtual PResultLen SecretSeedOperator(keys::isoft_def::PIpcPac_Head const *pReqHead,
                                          keys::isoft_def::PIpcAutoPacket &aswMsg,
                                          DoOperateSecretSeed doOperator) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_SECRET_SEED_H_