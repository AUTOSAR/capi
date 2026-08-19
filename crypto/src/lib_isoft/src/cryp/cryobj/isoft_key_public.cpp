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
/// @file       isoft_key_public.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Base
/// @unit_description=General asymmetric public key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Calculate the hash value of the public key value. The raw public key value BLOB can be obtained through the Serializable interface.
/// @param hashFunc Crypto context object for hash calculation
/// @return  a buffer preallocated for the resulting hash value
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPublic_Base::HashPublicKey(
    HashFunctionCtx &hashFunc) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c hashFunc context is not initialized
    if (false == hashFunc.IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the hash buffer is not enough for storing of the
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const pResultVec{ExportPublicly()};
    if (!pResultVec.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    ReadOnlyMemRegion const memKeyData{
        static_cast< uint8_t const * >(static_cast< void const * >(pResultVec.Value().data())),
        pResultVec.Value().size()};
    std::ignore = hashFunc.Update(memKeyData);
    ara::core::Result< ara::core::Vector< ara::core::Byte > > hashResult{hashFunc.Finish()};
    if (!hashResult.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return hashResult;
}
/// @brief General asymmetric public key interface.
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
