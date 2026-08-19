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
/// @file       ids_sign_verify.h
/// @brief      Signature verification implementation
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmSignVerify
/// @unit_description=Signature verification implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SIGN_VERIFY_H_
#define ARA_IDSM_SIGN_VERIFY_H_
#include <ara/core/string.h>

#include <iostream>
#ifdef ARA_WITH_CRYPTO
    #include "ara/crypto/common/entry_point.h"
    #include "ara/crypto/common/io_interface.h"
    #include "ara/crypto/cryp/cryobj/private_key.h"
    #include "ara/crypto/cryp/cryobj/public_key.h"
    #include "ara/crypto/keys/keyslot.h"
#endif
#include "ids_sign_common.h"
namespace ara {
namespace idsm {
/// @brief Signature verification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00109
/// @trace_id_dd=DD_IDSM_00202
/// @needwork = ad
/// @endcode
class IdsmSignVerify : public IdsmCrypto
{
public:
#ifdef ARA_WITH_CRYPTO
    /// @brief Verify signature
    /// @param data Data source that generated the signature
    /// @param size Length of signature data source
    /// @param signature Signature to be compared
    /// @return Returns true if signature verification succeeds, otherwise returns false
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00203
    /// @needwork = dda
    /// @endcode
    bool VerifySignature(uint8_t const* const data, size_t const size, BytesVecWithAlloc const& signature);
#endif

public:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00204
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify() : IdsmSignVerify{"", ""} {}
    /// @brief Constructor
    /// @param slotName Key slot used for signing
    /// @param algName Signature algorithm
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00205
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify(ara::core::String const& slotName, ara::core::String const& algName) : IdsmCrypto{slotName, algName}
    {
    }
    /// @brief Copy constructor
    /// @param sign Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00206
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify(IdsmSignVerify const& sign) = delete;
    /// @brief Move constructor
    /// @param sign Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00207
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify(IdsmSignVerify&& sign) = default;
    /// @brief Copy assignment operator
    /// @param object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00208
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify& operator=(IdsmSignVerify const& sign) = delete;
    /// @brief Move assignment operator
    /// @param object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00209
    /// @needwork = dda
    /// @endcode
    IdsmSignVerify& operator=(IdsmSignVerify&&) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00210
    /// @needwork = dda
    /// @endcode
    ~IdsmSignVerify() override = default;

private:
#ifdef ARA_WITH_CRYPTO
    /// @brief Decryption initialization
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @throw Stack overflow exception
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00211
    /// @needwork = dda
    /// @endcode
    int32_t _signInitDecrypto();
#endif

private:
#ifdef ARA_WITH_CRYPTO
    /// @brief Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00212
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider::Uptr cryptoProvider_;
    /// @brief Crypto IO operation interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00213
    /// @needwork = dda
    /// @endcode
    ara::crypto::IOInterface::Uptr ioInterface_;
    /// @brief Signature decryption context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00214
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::MsgRecoveryPublicCtx::Uptr signDecryCtx_{};
    /// @brief Encryption public key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00215
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::PublicKey::Uptrc publicKey_{};
#endif
    /// @brief Initialization flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00216
    /// @needwork = dda
    /// @endcode
    bool isInit_{false};
};
}  // namespace idsm
}  // namespace ara

#endif
