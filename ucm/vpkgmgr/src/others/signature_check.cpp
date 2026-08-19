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
/// @file       signature_check.cpp
/// @brief
/// @details
/// @date       2023-09-07
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#include "signature_check.h"

#include <ara/ucm/internal/crypto/crypto.h>
#include <ara/ucm/internal/extraction/filesystem.h>
#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include <fstream>
#include <streambuf>

#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

bool SignatureCheck::CheckSignature(ara::core::InstanceSpecifier &certPortId,
                                    ara::core::String const &filePath,
                                    ara::core::String const &signaturePath)
{
    ara::core::Result< ara::crypto::x509::X509Provider::Uptr > x509ProviderResult{ara::crypto::LoadX509Provider()};
    if (!x509ProviderResult.HasValue()) {
        LOG_ERROR << "LoadCertificate : errMsg=" << certPortId.ToString().data();
        return false;
    }
    ara::crypto::x509::X509Provider::Uptr x509Provider = std::move(x509ProviderResult).Value();
    ara::core::Result< ara::crypto::x509::Certificate::Uptr > certificateResult
        = x509Provider->LoadCertificate(certPortId);
    ara::crypto::x509::Certificate::Uptr certificatePtr = std::move(certificateResult).Value();

    const ara::crypto::x509::X509PublicKeyInfo &publicKeyInfo                = certificatePtr->SubjectPubKey(nullptr);
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > resultPublicKey = publicKeyInfo.GetPublicKey();
    if (!resultPublicKey.HasValue()) {
        LOG_ERROR << "GetPublicKey : errMsg" << certPortId.ToString().data();
        return false;
    }
    ara::crypto::cryp::PublicKey::Uptrc pPublicKey{std::move(resultPublicKey).Value()};

    // Load file
    std::ifstream fileIfs(filePath.c_str());
    if (!fileIfs.is_open()) {
        LOG_ERROR << "";
        return false;
    }
    ara::core::Vector< uint8_t > fileBuf(std::istreambuf_iterator< char >(fileIfs), {});
    fileIfs.close();
    ara::crypto::ReadOnlyMemRegion value(fileBuf.data(), fileBuf.size());

    // Load signature
    std::ifstream signatureIfs(signaturePath.c_str());
    if (!signatureIfs.is_open()) {
        LOG_ERROR << "";
        return false;
    }
    ara::core::Vector< uint8_t > signatureBuf(std::istreambuf_iterator< char >(signatureIfs), {});
    signatureIfs.close();
    ara::crypto::ReadOnlyMemRegion signature(signatureBuf.data(), signatureBuf.size());

    // Set ciphertext
    return _RsaDecode("RSA", *pPublicKey, value, signature);
}

// void SignatureCheck::_RsaEncode(ara::core::StringView stCryptoName, ara::crypto::cryp::PublicKey const &key,
//                                 ara::core::StringView stSrcData, ara::core::Vector<ara::core::Byte> &vecEncrypto,
//                                 bool suppressPadding) {
//     // static_cast<CryptoContext::AlgId>(nCrcID);
//     ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider = LoadCryptoProvider(ara::core::InstanceSpecifier(""));  //
//     ara::crypto::cryp::CryptoContext::AlgId algId = pCryptoProvider->ConvertToAlgId(stCryptoName);
//     if (algId == kAlgIdUndefined) {
//         LOG_ERROR << "ConvertToAlgId error";
//         return;
//     }
//     // Set plaintext
//     ara::crypto::ReadOnlyMemRegion srcCrypto((const uint8_t *)(stSrcData.data()), stSrcData.size());
//     // Encryption context
//     ara::core::Result<ara::crypto::cryp::EncryptorPublicCtx::Uptr> resultEncrypto =
//     pCryptoProvider->CreateEncryptorPublicCtx(algId); if (!resultEncrypto.HasValue()) {
//         LOG_ERROR << "";
//         return;
//     }
//     ara::crypto::cryp::EncryptorPublicCtx::Uptr const &pCtxEncrypt = resultEncrypto.Value();
//     // Set key
//     if (!pCtxEncrypt->SetKey(key).HasValue()) {
//         LOG_ERROR << "";
//         return;
//     }
//     ara::core::Result<ara::core::Vector<ara::core::Byte>> resultVecEncrypto =
//         pCtxEncrypt->ProcessBlock(srcCrypto, suppressPadding);
//     // After disabling automatic padding, many inputs may not work properly because the data size does not meet the standard requirements
//     if (suppressPadding && false == resultVecEncrypto.HasValue()) {
//         return;
//     }
//     if (!resultVecEncrypto.HasValue()) {
//         return;
//     }
//     vecEncrypto = std::move(resultVecEncrypto.Value());
// }

bool SignatureCheck::_RsaDecode(ara::core::StringView stCryptoName,
                                ara::crypto::cryp::PublicKey const &key,
                                ara::crypto::ReadOnlyMemRegion value,
                                ara::crypto::ReadOnlyMemRegion signature)
{
    if (value.empty() && signature.empty()) {
        return false;
    }
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider
        = ara::crypto::LoadCryptoProvider(ara::core::InstanceSpecifier("isoft"));  // TODO(yunfei)
    ara::crypto::cryp::CryptoContext::AlgId algId = pCryptoProvider->ConvertToAlgId(stCryptoName);
    if (algId == ara::crypto::kAlgIdUndefined) {
        LOG_ERROR << "";
        return false;
    }

    // Decryption context
    ara::core::Result< ara::crypto::cryp::VerifierPublicCtx::Uptr > resultDecrypto
        = pCryptoProvider->CreateVerifierPublicCtx(algId);
    if (!resultDecrypto.HasValue()) {
        LOG_ERROR << "";
        return false;
    }
    ara::crypto::cryp::VerifierPublicCtx::Uptr const &pCtxDecrypt = resultDecrypto.Value();
    // Set key
    if (!pCtxDecrypt->SetKey(key).HasValue()) {
        LOG_ERROR << "";
        return false;
    }
    ara::core::Result< bool > result = pCtxDecrypt->Verify(value, signature);
    if (!result.HasValue()) {
        LOG_ERROR << "";
        return false;
    }
    return result.Value();
}

ara::core::Vector< ara::core::Byte > SignatureCheck::_hexToBytes(core::String const &hex) const
{
    constexpr int32_t kInt_16_Base{16};
    ara::core::Vector< ara::core::Byte > res;
    for (auto i = 0U; i < hex.length(); i += 2) {
        core::String byteString = hex.substr(i, 2);
        ara::core::Byte byte    = static_cast< ara::core::Byte >(strtol(byteString.c_str(), nullptr, kInt_16_Base));
        res.push_back(byte);
    }
    return res;
}

ara::core::Result< void > SignatureCheck::CheckSignature(ara::core::String const &certSerialNumber,
                                                         ara::core::String const &issuer,
                                                         ara::core::String const &filePath,
                                                         ara::core::String const &signaturePath)
{
    // Check if software package manifest signature file exists
    if (!pkgmgr::Filesystem::DoesDirectoryExist(filePath.c_str())) {
        LOG_ERROR << "can't find signaturePath:" << signaturePath;
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
    }
    ara::crypto::x509::X509Provider::Uptr x509Provider{ara::crypto::LoadX509Provider()};
    if (nullptr == x509Provider.get()) {
        LOG_ERROR << "LoadX509Provider failed!";
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    ara::core::InstanceSpecifier iSpecify("isoft");  // TODO(yunfei)
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider = ara::crypto::LoadCryptoProvider(iSpecify);

    // Get certificate serial number
    // core::String certSerialNumber = manifest_.GetCertSerialNumber();
    // LOG_DEBUG << "get certSerialNumber:" << certSerialNumber;
    // if (certSerialNumber.empty()) {  // Check if certificate serial number is valid
    //     LOG_ERROR << "certSerialNumber is empty.";
    //     return ara::core::Result<void>::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
    // }

    // Certificate serial number is in hexadecimal, convert to Byte Vector
    ara::core::Vector< ara::core::Byte > snByteVec = _hexToBytes(certSerialNumber);

    // Get certificate issuer
    // ara::core::String issuer = manifest_.GetCertIssuer();
    // LOG_DEBUG << "get issuer:" << issuer;
    // if (issuer.empty()) {  // Check if issuer is empty
    //     LOG_ERROR << "issuer is empty.";
    //     return ara::core::Result<void>::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
    // }

    // Construct issuer DN
    auto resIssuerDn = x509Provider->BuildDn(issuer);
    if (!resIssuerDn) {  // Check if certificate issuer is valid
        LOG_ERROR << "failed to BuildDn with issuer:" << issuer;
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
    }
    ara::crypto::x509::X509DN::Uptrc issuerDn = std::move(resIssuerDn).Value();

    // Find certificate by serial number and issuer
    LOG_DEBUG << "try to FindCertBySn.";

    const uint8_t *pMemData{static_cast< const uint8_t * >(static_cast< const void * >(snByteVec.data()))};
    ara::crypto::x509::Certificate::Uptrc pCertFind
        = x509Provider->FindCertBySn(ara::crypto::ReadOnlyMemRegion(pMemData, snByteVec.size()), *issuerDn);
    if (!pCertFind) {
        LOG_ERROR << "failed to FindCertBySn with certSerialNumber:" << certSerialNumber << "and issuer:" << issuer;
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
    }

    // Export certificate
    LOG_DEBUG << "try to ExportPublicly.";
    auto resExportPem = pCertFind->ExportPublicly(ara::crypto::Serializable::kFormatPemEncoded);
    if (!resExportPem) {
        LOG_ERROR << "failed to ExportPublicly.";
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidPackageManifest);
    }
    ara::core::Vector< uint8_t > packagerCertData;
    for (size_t i = 0; i < resExportPem.Value().size(); i++) {
        packagerCertData.push_back(static_cast< uint8_t >(resExportPem.Value().at(i)));
    }

    // TODO(Han Zhibo): Verify certificate chain

    // Use certificate and software package manifest signature file to verify: software package manifest file
    pkgmgr::Crypto cr;
    if (!cr.CheckSignature(packagerCertData, filePath, signaturePath)) {
        LOG_ERROR << "failed to CheckSignature for filePath:" << filePath << "signaturePath:" << signaturePath;
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kAuthenticationFailed);
    }
    LOG_DEBUG << "succeed to CheckSignature for filePath:" << filePath << "signaturePath:" << signaturePath;
    return ara::core::Result< void >::FromValue();
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara