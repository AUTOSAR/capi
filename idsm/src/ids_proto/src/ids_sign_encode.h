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
/// @file       ids_sign_encode.h
/// @brief      Signature implementation
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
/// @unit_name=IdsmSignEncode
/// @unit_description=Signature implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SIGN_ENCODE_H_
#define ARA_IDSM_SIGN_ENCODE_H_
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
/// @brief Signature class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00108
/// @trace_id_dd=DD_IDSM_00186
/// @needwork = ad
/// @endcode
class IdsmSignEncode : public IdsmCrypto
{
public:
    /// @brief Determine whether signature is needed
    /// @return Returns true if IDSM supports signature, otherwise returns false
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00188
    /// @needwork = dda
    /// @endcode
    bool HasSign() const
    {
        if (!GetCrySlot().empty()) {
            return true;
        }
        return false;
    }
#ifdef ARA_WITH_CRYPTO
    /// @brief Generate signature
    /// @param data Data source for signature
    /// @param size Length of signature data source
    /// @param signature Signature
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00187
    /// @needwork = dda
    /// @endcode
    void GetSignature(uint8_t const* const data, size_t const size, BytesVecWithAlloc& signature);
#endif

public:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00189
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode() : IdsmSignEncode{"", ""} {}
    /// @brief Constructor
    /// @param slotName Name of the key slot used for signing
    /// @param algName Name of the signature algorithm
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00190
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode(ara::core::String const& slotName, ara::core::String const& algName) : IdsmCrypto{slotName, algName}
    {
    }
    /// @brief Copy constructor
    /// @param sign Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00191
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode(IdsmSignEncode const& sign) = delete;
    /// @brief Move constructor
    /// @param object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00192
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode(IdsmSignEncode&&) = default;
    /// @brief Copy assignment operator
    /// @param object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00193
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode& operator=(IdsmSignEncode const& sign) = delete;
    /// @brief Move assignment operator
    /// @param object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00194
    /// @needwork = dda
    /// @endcode
    IdsmSignEncode& operator=(IdsmSignEncode&& sign) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00195
    /// @needwork = dda
    /// @endcode
    ~IdsmSignEncode() override = default;

private:
#ifdef ARA_WITH_CRYPTO
    /// @brief Encryption initialization
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @throw Stack overflow exception
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00196
    /// @needwork = dda
    /// @endcode
    int32_t _signInitCrypto();
#endif

private:
#ifdef ARA_WITH_CRYPTO
    /// @brief Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00197
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider::Uptr cryptoProvider_;
    /// @brief Crypto IO operation interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00198
    /// @needwork = dda
    /// @endcode
    ara::crypto::IOInterface::Uptr ioInterface_;
    /// @brief Signature context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00199
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::SigEncodePrivateCtx::Uptr signCtx_{};
    /// @brief Encryption private key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00200
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::PrivateKey::Uptrc privateKey_{};
#endif
    /// @brief Initialization flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00201
    /// @needwork = dda
    /// @endcode
    bool isInit_{false};
};
}  // namespace idsm
}  // namespace ara
#endif
