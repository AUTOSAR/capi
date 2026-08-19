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
/// @file       secoc_crypto.cpp
/// @brief
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/secoc/secoc_crypto.h"

#include "ara/com/internal/log/log.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief
/// @return bool
bool SecOcCryptoOp::Initialize() noexcept
{
    if (initialized_) {
        return true;
    }
    ComLogDebug(cryptoSpecifier_, algorithmName_, keySlotName_);
    ara::crypto::cryp::CryptoProvider::Uptr const pCryptoProvider{ara::crypto::LoadCryptoProvider(cryptoSpecifier_)};
    if (pCryptoProvider == nullptr) {
        ComLogError("CMAC provider null error");
        return false;
    }
    ara::crypto::cryp::CryptoProvider::AlgId const algId{pCryptoProvider->ConvertToAlgId(algorithmName_)};
    if (algId == ara::crypto::kAlgIdUndefined) {
        ComLogError("CMAC algId error");
        return false;
    }
    ara::core::Result< ara::crypto::cryp::MessageAuthnCodeCtx::Uptr > providerResult{
        pCryptoProvider->CreateMessageAuthCodeCtx(algId)};
    if (!providerResult) {
        ComLogError("CMAC error: ", providerResult);
        return false;
    }
    this->secOcCryptoCtx_ = std::move(std::move(providerResult).Value());

    ara::core::InstanceSpecifier iSpecify2(this->keySlotName_);
    ara::crypto::keys::KeyStorageProvider::Uptr pKeyProvider = ara::crypto::LoadKeyStorageProvider();
    auto resultKeySlot                                       = pKeyProvider->LoadKeySlot(iSpecify2);
    if (!resultKeySlot) {
        ComLogError("CMAC error: ", resultKeySlot);
        return false;
    }
    ara::crypto::keys::KeySlot::Uptr pKeySlot = std::move(resultKeySlot).Value();
    auto resultIoInterface                    = pKeySlot->Open(false, false);
    if (!resultIoInterface) {
        ComLogError("CMAC error: ", resultIoInterface);
        return false;
    }
    ara::crypto::IOInterface::Uptr pIoInterface = std::move(resultIoInterface).Value();
    ara::core::Result< ara::crypto::cryp::SymmetricKey::Uptrc > resultSrcKey
        = pCryptoProvider->LoadSymmetricKey(*(pIoInterface.get()));
    if (!resultSrcKey) {
        ComLogError("CMAC error: ", resultSrcKey);
        return false;
    }
    keySrc_ = std::move(std::move(resultSrcKey).Value());
    ara::core::Result< void > const setKeyResult{
        secOcCryptoCtx_->SetKey(*keySrc_.get(), ara::crypto::CryptoTransform::kMacGenerate)};
    if (!setKeyResult) {
        ComLogError("CMAC error: ", setKeyResult);
        return false;
    }
    initialized_ = true;
    return true;
}

/// @brief
/// @param[in] payload
/// @param[in] length
/// @param[in] macOutput
/// @return bool
bool SecOcCryptoOp::DoCrytoOperation(uint8_t const* const payload,
                                     uint16_t const length,
                                     ara::core::Vector< uint8_t >& macOutput) noexcept
{
    if (!initialized_) {
        return false;
    }
    const ara::crypto::ReadOnlyMemRegion iv(reinterpret_cast< uint8_t const* >("0123456789ABCDEF"), 16);
    ara::crypto::ReadOnlyMemRegion memRegion{payload, length};
    ara::core::Result< void > const startResult{secOcCryptoCtx_->Start(iv)};
    if (!startResult) {
        ComLogError("CMAC error: ", startResult);
        return false;
    }
    ara::core::Result< void > const updateResult{secOcCryptoCtx_->Update(memRegion)};
    if (!updateResult) {
        ComLogError("CMAC error: ", updateResult);
        return false;
    }
    ara::core::Result< std::unique_ptr< const ara::crypto::cryp::Signature > > const resultMacSignature{
        secOcCryptoCtx_->Finish(true)};
    if (!resultMacSignature) {
        ComLogError("CMAC error: ", resultMacSignature);
        return false;
    }
    ara::core::Result< bool > const checkResult{secOcCryptoCtx_->Check(*(resultMacSignature.Value().get()))};
    if (!checkResult) {
        ComLogError("CMAC error: ", checkResult);
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultMac{
        resultMacSignature.Value()->ExportPublicly(ara::crypto::Serializable::kFormatRawValueOnly)};
    if (!resultMac) {
        ComLogError("CMAC error: ", resultMac.Error());
        return false;
    }
    std::size_t const macSize{resultMac.Value().size()};
    if (macSize <= 0) {
        ComLogError("CMAC return macSize error");
        return false;
    }
    macOutput.resize(macSize);
    std::ignore = std::copy(reinterpret_cast< uint8_t const* >(resultMac.Value().data()),
                            reinterpret_cast< uint8_t const* >(resultMac.Value().data()) + macSize, macOutput.data());
    ara::core::Result< void > const resetResult{secOcCryptoCtx_->Reset()};
    if (!resetResult) {
        return false;
    }
    return true;
}

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara