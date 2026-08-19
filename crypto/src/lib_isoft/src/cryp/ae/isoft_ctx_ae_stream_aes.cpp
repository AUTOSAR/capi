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
/// @file       isoft_ctx_ae_stream_aes.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-05-10
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/AuthenticatedEncryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01011
/// @unit_name=PCtxAeStreamAes_Base
/// @unit_description=Authenticated encryption base class based on AES stream cipher
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/ae/isoft_ctx_ae_stream_aes.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_stream.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Authenticated encryption using AES stream cipher.
/// @tparam T_Mac
/// @tparam T_CtxSymmetricStream
/// @param cryptoProvider crypto provider
template < typename T_Mac, typename T_CtxSymmetricStream >
inline PCtxAeStreamAes_Base< T_Mac, T_CtxSymmetricStream >::
    PCtxAeStreamAes_Base(PCryptoProvider& cryptoProvider) noexcept(noexcept(std::make_unique< T_Mac >(
        cryptoProvider)) && noexcept(std::make_unique< T_CtxSymmetricStream >(cryptoProvider)))
    : PCtxAe_Base< T_Mac, T_CtxSymmetricStream >{cryptoProvider}
{
}

template class PCtxAeStreamAes_Base< PCtxMacAesCbc_128, PCtxSymmetricStream_Aes_Cfb128 >;

/// @brief Get the BlockService instance.
/// @return BlockService instance
BlockService::Uptr PCtxAeStreamAes_Cbc::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxAeStreamAes_Cbc > >(*this)};
}
/// @brief Returns a CryptoPrimtivId instance containing the instance identifier.
/// @return CryptoPrimtivId instance
CryptoPrimitiveId::Uptr PCtxAeStreamAes_Cbc::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Ae_Stream_Aes_Cbc >()};
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara