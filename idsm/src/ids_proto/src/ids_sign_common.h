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
/// @file       ids_sign_common.h
/// @brief      IDSM encryption base class
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
/// @unit_name=IdsmCrypto
/// @unit_description=IDSM encryption base class
/// @endcode
///
/// ================================================================

#ifndef IDS_SIGN_COMMON_H
#define IDS_SIGN_COMMON_H
#include <ara/core/string.h>

#include <iostream>
#ifdef ARA_WITH_CRYPTO
    #include "ara/crypto/common/entry_point.h"
    #include "ara/crypto/common/io_interface.h"
    #include "ara/crypto/cryp/cryobj/private_key.h"
    #include "ara/crypto/cryp/cryobj/public_key.h"
    #include "ara/crypto/keys/keyslot.h"
#endif
#include "ara/core/vector.h"
namespace ara {
namespace idsm {
/// @brief ARA byte array
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00110
/// @trace_id_dd=DD_IDSM_00217
/// @needwork = ad
/// @endcode
using AraBytesVec = ara::core::Vector< ara::core::Byte >;
/// @brief Signature memory allocator
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00111
/// @trace_id_dd=DD_IDSM_00218
/// @needwork = ad
/// @endcode
using SignAllocator = std::allocator< std::uint8_t >;
#ifdef ARA_WITH_CRYPTO
/// @brief Signature byte array
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00112
/// @trace_id_dd=DD_IDSM_00219
/// @needwork = ad
/// @endcode
using BytesVecWithAlloc = ara::crypto::ByteVector< SignAllocator >;
#endif
/// @brief IDSM encryption base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00113
/// @trace_id_dd=DD_IDSM_00220
/// @needwork = ad
/// @endcode
class IdsmCrypto
{
public:
    /// @brief Get key slot name
    /// @return Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00222
    /// @needwork = dda
    /// @endcode
    ara::core::String GetCrySlot() const noexcept { return crySlot_; }
    /// @brief Get encryption algorithm name
    /// @return Algorithm name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00223
    /// @needwork = dda
    /// @endcode
    ara::core::String GetCryAlg() const noexcept { return cryAlg_; }
#ifdef ARA_WITH_CRYPTO
    /// @brief Generate digital signature
    /// @param data Source data for digital signature
    /// @param size Length of source data for digital signature
    /// @param digest Generated digital signature
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00221
    /// @needwork = dda
    /// @endcode
    static void GenDigitDigest(std::uint8_t const* const data, size_t const size, AraBytesVec& digest);
#endif

protected:
#ifdef ARA_WITH_CRYPTO
    /// @brief Encryption initialization
    /// @return 0 on success, -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00224
    /// @needwork = dda
    /// @endcode
    int32_t _InitCrypto(ara::crypto::cryp::CryptoProvider::Uptr& provider, ara::crypto::IOInterface::Uptr& ioInterface);
#endif

public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00225
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmCrypto() = default;

protected:
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00226
    /// @needwork = dda
    /// @endcode
    IdsmCrypto() : IdsmCrypto{"", ""} {}
    /// @brief Parameterized constructor
    /// @param slotName Key slot name
    /// @param algName Algorithm name
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00227
    /// @needwork = dda
    /// @endcode
    IdsmCrypto(ara::core::String slotName, ara::core::String algName)
        : crySlot_{std::move(slotName)}, cryAlg_{std::move(algName)}
    {
    }
    /// @brief Copy constructor
    /// @param object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00228
    /// @needwork = dda
    /// @endcode
    IdsmCrypto(IdsmCrypto const&) = default;
    /// @brief Move constructor
    /// @param object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00229
    /// @needwork = dda
    /// @endcode
    IdsmCrypto(IdsmCrypto&&) = default;
    /// @brief Copy assignment operator
    /// @param object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00230
    /// @needwork = dda
    /// @endcode
    IdsmCrypto& operator=(IdsmCrypto const&) = default;
    /// @brief Move assignment operator
    /// @param object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00231
    /// @needwork = dda
    /// @endcode
    IdsmCrypto& operator=(IdsmCrypto&&) = default;

private:
    /// @brief Key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00232
    /// @needwork = dda
    /// @endcode
    ara::core::String crySlot_;
    /// @brief Algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00233
    /// @needwork = dda
    /// @endcode
    ara::core::String cryAlg_;
};
}  // namespace idsm
}  // namespace ara

#endif