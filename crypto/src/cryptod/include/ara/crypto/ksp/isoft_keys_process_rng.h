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
/// @file       isoft_keys_process_rng.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of random numbers
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/random number generation IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=PKeysProcess_Rng
/// @unit_description=Random number generation IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_RNG_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_RNG_H_

#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Random number processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03052
/// @trace_id_dd=DD_CRYPTO_06055
/// @needwork = ad
/// @endcode
class PKeysProcess_Rng : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Rng >
{
public:
    /// @brief Constructor with parameters
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03053
    /// @trace_id_dd=DD_CRYPTO_06056
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Rng(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03054
    /// @trace_id_dd=DD_CRYPTO_06057
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Rng() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03055
    /// @trace_id_dd=DD_CRYPTO_06058
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Rng(PKeysProcess_Rng const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03056
    /// @trace_id_dd=DD_CRYPTO_06059
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Rng(PKeysProcess_Rng &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03057
    /// @trace_id_dd=DD_CRYPTO_06060
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Rng &operator=(PKeysProcess_Rng const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03058
    /// @trace_id_dd=DD_CRYPTO_06061
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Rng &operator=(PKeysProcess_Rng &&other) = delete;

public:
    /// @brief Set key
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if setkey sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06062
    /// @needwork = dda
    /// @endcode
    PResultLen SetKey(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Generate random number
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if generate sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06063
    /// @needwork = dda
    /// @endcode
    PResultLen Generate(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_RNG_H_