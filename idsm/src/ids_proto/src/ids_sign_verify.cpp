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
/// @file       ids_sign_verify.cpp
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
/// @unit_description=
/// @endcode
///
/// ================================================================

#include <iostream>
#ifdef ARA_WITH_CRYPTO
    #include "ara/crypto/cryp/msg_recovery_public_ctx.h"
#endif
#include "ids_log.h"
#include "ids_sign_verify.h"

namespace ara {
namespace idsm {
#ifdef ARA_WITH_CRYPTO
/// @brief
/// @return
/// @throw
int32_t IdsmSignVerify::_signInitDecrypto()
{
    if (_InitCrypto(cryptoProvider_, ioInterface_) != 0) {
        return -1;
    }

    // ara::crypto::cryp::CryptoProvider::Uptr provider;
    // GetCryProvider(provider);
    // ara::crypto::IOInterface::Uptr ioInterface;
    // GetCryIoInterface(ioInterface);
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > rsaPublicKey{
        cryptoProvider_->LoadPublicKey(*ioInterface_)};
    if (!rsaPublicKey.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, LoadPublicKey interface return error: "
                      << rsaPublicKey.Error().Message();
        return -1;
    }
    publicKey_ = std::move(rsaPublicKey).Value();
    ara::core::String const algName{GetCryAlg()};
    if (algName != "RSA") {
        IDS_LOG_ERROR << "ids protocol: signature fail, not support crypto algorithm: " << algName.c_str();
        return -1;
    }
    ara::core::String const tempAlgName{"msgrecoveryRsa"};
    ara::crypto::cryp::CryptoContext::AlgId const algId{cryptoProvider_->ConvertToAlgId(tempAlgName)};
    if (algId == ara::crypto::kAlgIdUndefined) {
        return -1;
    }

    /// @details Initialize the context for asymmetric decryption
    ara::core::Result< ara::crypto::cryp::MsgRecoveryPublicCtx::Uptr > tempCtx{
        cryptoProvider_->CreateMsgRecoveryPublicCtx(algId)};
    if (!tempCtx.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, CreateMsgRecoveryPublicCtx interface return error: "
                      << tempCtx.Error();
        return -1;
    }
    signDecryCtx_ = std::move(tempCtx).Value();
    ara::core::Result< void > const keyResult{signDecryCtx_->SetKey(*publicKey_)};
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
/// @return
/// @throw
bool IdsmSignVerify::VerifySignature(uint8_t const* const data, size_t const size, BytesVecWithAlloc const& signature)
{
    /// @details When verifying the first signature, obtain the crypto module related data structures necessary for signature verification
    if (isInit_ == false) {
        int32_t const initRes{_signInitDecrypto()};
        if (initRes != 0) {
            return false;
        }
        isInit_ = true;
    }

    ara::crypto::ReadOnlyMemRegion const signData{signature.data(), signature.size()};
    ara::core::Result< AraBytesVec > const resultVerify{signDecryCtx_->DecodeAndVerify(signData)};
    if (!resultVerify.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, DecodeAndVerify interface return error: "
                      << resultVerify.Error();
        return false;
    }

    ara::core::Vector< ara::core::Byte > digest;
    GenDigitDigest(data, size, digest);
    if (digest.empty()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, Get digital digest error";
        return false;
    }

    if (digest == resultVerify.Value()) {
        IDS_LOG_DEBUG << "ids protocol: verify signatrue success.";
        return true;
    }
    return false;
}
#endif
}  // namespace idsm
}  // namespace ara
