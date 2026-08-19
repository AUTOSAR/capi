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
/// @file       ids_sign_encode.cpp
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
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ids_sign_encode.h"

#include <chrono>
#include <iostream>

#ifdef ARA_WITH_CRYPTO
    #include "ara/crypto/cryp/sig_encode_private_ctx.h"
#endif
#include "ids_log.h"
namespace ara {
namespace idsm {
#ifdef ARA_WITH_CRYPTO
/// @brief
/// @return
/// @throw
int32_t IdsmSignEncode::_signInitCrypto()
{
    int32_t const initRes{_InitCrypto(cryptoProvider_, ioInterface_)};
    if (initRes != 0) {
        return -1;
    }

    /// @details Get the private key for asymmetric encryption
    // ara::crypto::cryp::CryptoProvider::Uptr provider;
    // GetCryProvider(provider);
    // ara::crypto::IOInterface::Uptr ioInterface;
    // GetCryIoInterface(ioInterface);
    ara::core::Result< ara::crypto::cryp::PrivateKey::Uptrc > rsaPrivateKey{
        cryptoProvider_->LoadPrivateKey(*ioInterface_)};
    if (!rsaPrivateKey.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, LoadPrivateKey interface return error: "
                      << rsaPrivateKey.Error();
        return -1;
    }
    privateKey_ = std::move(rsaPrivateKey).Value();

    /// @details Initialize the context for asymmetric encryption
    ara::core::String const algName{GetCryAlg()};
    if (algName != "RSA") {
        IDS_LOG_ERROR << "ids protocol: signature fail, not support crypto algorithm: " << algName.c_str();
        return -1;
    }
    ara::core::String const tempAlgName{"signencodeRsa"};
    ara::crypto::cryp::CryptoContext::AlgId const algId{cryptoProvider_->ConvertToAlgId(tempAlgName)};
    ara::core::Result< ara::crypto::cryp::SigEncodePrivateCtx::Uptr > tempCtx{
        cryptoProvider_->CreateSigEncodePrivateCtx(algId)};
    if (!tempCtx.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, CreateSigEncodePrivateCtx interface return error: "
                      << tempCtx.Error();
        return -1;
    }
    signCtx_ = std::move(tempCtx).Value();
    ara::core::Result< void > const keyResult{signCtx_->SetKey(*privateKey_)};
    if (!keyResult.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signatrue fail, SetKey interface return error: " << keyResult.Error();
        return -1;
    }
    return 0;
}
/// @brief
/// @param data
/// @param size
/// @param signature
/// @throw
void IdsmSignEncode::GetSignature(std::uint8_t const* const data, size_t const size, BytesVecWithAlloc& signature)
{
    std::ignore = signature;
    if (!isInit_) {
        int32_t const initRes{_signInitCrypto()};
        if (initRes != 0) {
            return;
        }
        isInit_ = true;
    }

    /// @brief Generate digital digest
    ara::core::Vector< ara::core::Byte > digest;
    GenDigitDigest(data, size, digest);
    if (digest.empty()) {
        return;
    }

    std::size_t const digestSize{digest.size()};
    /// @brief Generate signature for the digital digest
    ara::crypto::ReadOnlyMemRegion const digtData{
        static_cast< uint8_t const* >(static_cast< void const* >(digest.data())), digestSize};
    if (digest.empty()) {
    }
    ara::core::Result< BytesVecWithAlloc > const signRes{signCtx_->SignAndEncode< SignAllocator >(digtData)};
    if (!signRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signatrue fail, SignAndEncode interface return error. " << signRes.Error();
        return;
    }
    IDS_LOG_DEBUG << "ids protocol: generate signatrue success.";

    signature = signRes.Value();
}
#endif

}  // namespace idsm
}  // namespace ara
