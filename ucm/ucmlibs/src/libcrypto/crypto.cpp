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
/// @file       crypto.cpp
/// @brief      The Crypto definition for ucm.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=Crypto
/// @unit_description=Crypto definitions provided for UCM
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/crypto/crypto.h"

#include <nai/os/nai_file.h>
#include <nai/runtime/nai_util.h>

#include <fstream>

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/sig_encode_private_ctx.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/x509_provider.h"
#include "ara/ucm/internal/crypto/base64.hpp"
#include "ara/ucm/internal/crypto/crypto_error_domain.h"
#include "ara/ucm/internal/extraction/assert.h"
#include "ara/ucm/internal/extraction/log.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00148
/// @needwork = dd
/// @endcode
constexpr size_t kMD5DigestLen{16U};

/// @brief constructor
/// @throws no
Crypto::Crypto()
    : pProviderX509_{ara::crypto::LoadX509Provider()}
    , pCryptoProvider_{
          ara::crypto::LoadCryptoProvider(std::move(ara::core::InstanceSpecifier(std::move(AraStringView("isoft")))))}
{
}

/// @brief Checks validity of a signature
/// @param certificate certificate that contains a public key
/// @param filePath The absolute path to a file to check
/// @param signaturePath The absolute path to a signature
///
/// @return true if signature is valid, false otherwise
/// @throws no
bool Crypto::CheckSignature(AraVector< uint8_t > const& certificate,
                            AraString const& filePath,
                            AraString const& signaturePath) const
{
    std::ignore = certificate;
    LOGD << "begin with filePath:" << filePath.c_str() << "signaturePath:" << signaturePath.c_str();

    // Get the signature verification context
    Result_getVerifierPublicCtx verifierPublicCtxRes{_getVerifierPublicCtx(certificate)};
    if (!verifierPublicCtxRes.HasValue()) {
        LOGE << "_getVerifierPublicCtx failed, errc:" << verifierPublicCtxRes.Error().Message().data();
        return false;
    }

    // Signature verification context
    VerifierPublicCtx::Uptr verifierPublicCtx;
    PublicKey::Uptrc pPublicKey;
    std::tie(verifierPublicCtx, pPublicKey) = std::move(verifierPublicCtxRes).Value();

    // Hash the filePath content
    ResultCreateHashFunctionCtx const hashFunctionCtxRes{_sHA256HashFile(filePath)};
    if (!hashFunctionCtxRes.HasValue()) {
        LOGE << "_sHA256HashFile failed, errc:" << hashFunctionCtxRes.Error().Message().data();
        return false;
    }

    // Get the hash context
    HashFunctionCtx::Uptr const& hashFunctionCtx{hashFunctionCtxRes.Value()};

    // Load signature data
    AraVector< uint8_t > sigFileBuf{LoadFile(signaturePath)};
    LOGD << "get sigFileBuf.size:" << sigFileBuf.size();

    // Base64 decode the signature
    AraString const signatureBuf{
        Base64::Decode(std::string(reinterpret_cast< Char_T const* >(sigFileBuf.data()), sigFileBuf.size()))};
    LOGD << "get signatureBuf.size:" << signatureBuf.size();

    ara::crypto::ReadOnlyMemRegion const memSignature{reinterpret_cast< uint8_t const* >(signatureBuf.c_str()),
                                                      signatureBuf.size()};

    // Verify signature
    AraResult< bool > const verifyRes{verifierPublicCtx->VerifyPrehashed(*hashFunctionCtx, memSignature)};
    if (!verifyRes.HasValue()) {
        LOGE << "VerifyPrehashed failed, errc:" << verifyRes.Error().Message().data();
        return false;
    }

    LOGD << "end with true";
    return true;
}

/// @brief convert vector of byte to hex str
/// @param data vector of byte
/// @throws no
/// @return hex str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00147
/// @needwork = no
/// @endcode
static AraString Bytes2HexStr(AraVector< AraByte > const& data)
{
    std::vector< Char_T > const hexmap{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::size_t const len{data.size()};
    std::size_t i{0U};
    std::size_t d{0U};
    std::size_t const twoU{2U};
    std::size_t const high{0xF0U};
    std::size_t const fourU{4U};
    std::size_t const low{0x0FU};

    AraString ret(len * twoU, ' ');

    for (std::int32_t end{static_cast< std::int32_t >(len) - 1}; end >= 0; --end) {
        i                  = static_cast< std::size_t >(end);
        d                  = static_cast< std::size_t >(static_cast< UChar_T >(data[i]));
        ret[twoU * i]      = hexmap[(d & high) >> fourU];
        ret[twoU * i + 1U] = hexmap[(d & low)];
    }

    return ret;
}

/// @brief Returns the SHA-256 hash of the file
///
/// @param filePath The absolute path to a file.
///
/// @return SHA-256 hash in AraString
/// @throws no
AraString Crypto::GetFileHash(AraString const& filePath) const
{
    AraString ret;

    // Hash the filePath content
    ResultCreateHashFunctionCtx const hashFunctionCtxRes{_sHA256HashFile(filePath)};
    if (!hashFunctionCtxRes.HasValue()) {
        LOGE << "_sHA256HashFile failed, errc:" << hashFunctionCtxRes.Error().Message().data();
        return ret;
    }

    // Get the hash context
    HashFunctionCtx::Uptr const& hashFunctionCtx{hashFunctionCtxRes.Value()};

    // Get the digest
    AraResult< AraVector< AraByte > > digestRes{hashFunctionCtx->GetDigest()};
    if (!digestRes.HasValue()) {
        LOGE << "GetDigest failed, errc:" << digestRes.Error().Message().data();
        return ret;
    }

    // Get the digest
    AraVector< AraByte > const digest{std::move(std::move(digestRes).Value())};

    // Convert to hexadecimal value
    ret = Bytes2HexStr(digest);
    return ret;
}

/// @brief Get the MD5 value of a file content (a 32-character lowercase hexadecimal string); if the file does not exist, return an empty string
/// @param fileName
/// @returns md5 string
/// @throws no
AraString Crypto::MD5ForFile(AraString const& fileName) const
{
    // (void)fileName;.
    // return "7B064DAD507C266A161FFC73C53DCDC5";.

    // Check if the file exists
    if (!tinyfsys::DoesFileExist(fileName)) {
        return "";
    }

    // Calculate the MD5 value

    // Initialize context
    AlgId const algId{pCryptoProvider_->ConvertToAlgId(AraStringView("md5"))};
    ResultCreateHashFunctionCtx const retCreateHash{pCryptoProvider_->CreateHashFunctionCtx(algId)};
    if (!retCreateHash.HasValue()) {
        LOGE << "CreateHashFunctionCtx failed, errc:" << retCreateHash.Error().Message().data();
        return "";
    }
    HashFunctionCtx::Uptr const& pHashCrc{retCreateHash.Value()};

    AraResultVoid hashRet{pHashCrc->Start()};
    if (!hashRet.HasValue()) {
        LOGE << "pHashCrc Start failed, errc:" << hashRet.Error().Message().data();
        return "";
    }

    // Open file
    nai_fd_t const fd{nai_file_open(fileName.c_str(), NAI_O_RDONLY)};
    AssertWithLog(static_cast< nai_fd_t >(-1) != fd);
    intptr_t r{nai_file_seek(fd, 0, 0)};
    AssertWithLog(0 == r);

    // Buffer
    constexpr uint16_t kNUM_1024{1024U};
    AraVector< uint8_t > buf(kNUM_1024, 0U);

    // Read file
    while (true) {
        // Clear buffer
        std::ignore = nai_memset(buf.data(), 0, buf.size());
        r           = nai_file_read(fd, buf.data(), buf.size());
        AssertWithLog(r >= 0);
        if (r > 0) {
            ara::crypto::ReadOnlyMemRegion const memRegion{buf.data(), static_cast< size_t >(r)};
            hashRet = pHashCrc->Update(memRegion);
            AssertWithLog(hashRet.HasValue());
        } else {
            break;
        }
    }

    // Close file
    r = nai_file_close(fd);
    AssertWithLog(0 == r);

    // Finalize and get result
    AraVector< AraByte > const resultVec{pHashCrc->Finish().Value()};

    // Length of MD5 string produced per character
    size_t const mD5LenPerChar{2U};
    size_t const mD5FullLenPerChar{3U};

    // Convert MD5 value to hexadecimal
    size_t const len{resultVec.size()};
    AssertWithLog(len == kMD5DigestLen);
    size_t const mD5StrLen{kMD5DigestLen * 2U};
    AraString mD5Str(mD5StrLen, '\0');
    for (size_t i{0U}; i < len; i++) {
        std::ignore
            = snprintf(&mD5Str[i * mD5LenPerChar], mD5FullLenPerChar, "%02x", static_cast< UChar_T >(resultVec[i]));
    }

    // Return result
    return mD5Str;
}

/// @brief LoadFile
/// @param filePath
/// @return file body
/// @throws no
AraVector< uint8_t > Crypto::LoadFile(AraString const& filePath)
{
    std::ifstream instream(filePath.c_str(), std::ios::binary);
    std::vector< uint8_t > data{};
    if (instream) {
        data = std::vector< uint8_t >{std::istreambuf_iterator< Char_T >(instream),
                                      std::istreambuf_iterator< Char_T >()};
    }
    return data;  // automatic objects returned from a function by value can be moved even if they are l-values
}

/// @brief _getVerifierPublicCtx
/// @param certificate
/// @return result
/// @throws no
Result_getVerifierPublicCtx Crypto::_getVerifierPublicCtx(AraVector< uint8_t > const& certificate) const
{
    LOGD << "begin";

    // Parse certificate
    ara::crypto::ReadOnlyMemRegion const memCert{certificate.data(), certificate.size()};
    AraResult< std::unique_ptr< ara::crypto::x509::Certificate > > const retParseCert{
        pProviderX509_->ParseCert(memCert)};
    if (!retParseCert.HasValue()) {
        LOGE << "ParseCert failed, errc:" << retParseCert.Error().Message().data();
        return Result_getVerifierPublicCtx::FromError(retParseCert.Error());
    }

    // Get certificate
    ara::crypto::x509::Certificate const* const pCertificate{retParseCert.Value().get()};

    // Load public key
    AraResult< PublicKey::Uptrc > retPublicKey{pCertificate->SubjectPubKey(nullptr).GetPublicKey()};
    if (!retPublicKey.HasValue()) {
        LOGE << "failed to SubjectPubKey with error:" << retPublicKey.Error().Message().data();
        return Result_getVerifierPublicCtx::FromError(retPublicKey.Error());
    }

    // Get public key
    PublicKey::Uptrc pPublicKey{std::move(std::move(retPublicKey).Value())};
    assert(nullptr != pPublicKey);

    // Algorithm ID
    AraString const algName{"VerifyRsa-Pss-Sha2-256"};
    AlgId const algId{pCryptoProvider_->ConvertToAlgId(algName)};
    if (ara::crypto::kAlgIdUndefined == algId) {
        LOGE << "failed to ConvertToAlgId for algName:" << algName.c_str();
        return Result_getVerifierPublicCtx::FromError(CryptoErrc::kGeneralError);
    }

    // Create signature verification context
    AraResult< VerifierPublicCtx::Uptr > retCreateVerifier{pCryptoProvider_->CreateVerifierPublicCtx(algId)};
    if (!retCreateVerifier.HasValue()) {
        LOGE << "failed to CreateVerifierPublicCtx with error:" << retCreateVerifier.Error().Message().data();
        return Result_getVerifierPublicCtx::FromError(retCreateVerifier.Error());
    }
    VerifierPublicCtx::Uptr verifierPublicCtx{std::move(std::move(retCreateVerifier).Value())};

    // Set public key for signature verification context
    AraResultVoid const retSetKey{verifierPublicCtx->SetKey(*pPublicKey)};
    if (!retSetKey.HasValue()) {
        LOGE << "failed to SetKey with error:" << retSetKey.Error().Message().data();
        return Result_getVerifierPublicCtx::FromError(retSetKey.Error());
    }

    LOGD << "end";
    return Result_getVerifierPublicCtx::FromValue(std::make_tuple(std::move(verifierPublicCtx), std::move(pPublicKey)));
    // return std::move(Result_getVerifierPublicCtx::FromValue(std::make_tuple(verifierPublicCtx.get(),
    // pPublicKey.get())));
}

/// @brief _sHA256HashFile
/// @param filePath
/// @return result
/// @throws no
ResultCreateHashFunctionCtx Crypto::_sHA256HashFile(AraString const& filePath) const
{
    LOGD << "begin";

    // Create hash algorithm ID
    AraString const sha256AlgName{"sha2-256"};
    AlgId const hashAlgId{pCryptoProvider_->ConvertToAlgId(sha256AlgName)};
    if (ara::crypto::kAlgIdUndefined == hashAlgId) {
        LOGE << "ConvertToAlgId failed";
        return ResultCreateHashFunctionCtx::FromError(CryptoErrc::kGeneralError);
    }

    // Create hash function context
    ResultCreateHashFunctionCtx retHashCtx{pCryptoProvider_->CreateHashFunctionCtx(hashAlgId)};
    if (!retHashCtx.HasValue()) {
        LOGE << "CreateHashFunctionCtx failed, errc:" << retHashCtx.Error().Message().data();
        return retHashCtx;
    }

    // Get hash function
    HashFunctionCtx::Uptr const& hashCtx{retHashCtx.Value()};

    // Set seed
    AraString const stIv("                                                                ");
    ara::crypto::ReadOnlyMemRegion const iv{reinterpret_cast< uint8_t const* >(stIv.data()), stIv.size()};

    // Start calculation
    AraResultVoid ret{hashCtx->Start(iv)};
    if (!ret.HasValue()) {
        LOGE << "hashCtx Start failed, errc:" << ret.Error().Message().data();
        return ResultCreateHashFunctionCtx::FromError(ret.Error());
    }

    // Open file
    nai_fd_t const fd{nai_file_open(filePath.c_str(), NAI_O_RDONLY)};  // PRQA S 3600
    if (fd == NAI_FD_INVALID) {                                        // PRQA S 3080
        LOGE << "failed to nai_file_open with filePath:" << filePath.c_str();
        return ResultCreateHashFunctionCtx::FromError(CryptoErrc::kGeneralError);
    }
    intptr_t r{nai_file_seek(fd, 0, 0)};
    if (0 != r) {  // PRQA S 2410
        std::ignore = nai_file_close(fd);
        LOGE << "failed to nai_file_seek.";
        return ResultCreateHashFunctionCtx::FromError(CryptoErrc::kGeneralError);
    }

    // Buffer
    constexpr uint16_t kNUM_1024{1024U};
    AraVector< uint8_t > buf(kNUM_1024, 0U);  // PRQA S 2410

    // Read file
    while (true) {
        r = nai_file_read(fd, buf.data(), buf.size());  // PRQA S 3840
        if (r < 0) {
            std::ignore = nai_file_close(fd);
            LOGE << "failed to nai_file_read.";
            return ResultCreateHashFunctionCtx::FromError(CryptoErrc::kGeneralError);
        }
        if (r > 0) {
            // Update
            ara::crypto::ReadOnlyMemRegion const memSrc{buf.data(), static_cast< size_t >(r)};
            ret = hashCtx->Update(memSrc);
            if (!ret.HasValue()) {
                LOGE << "hashCtx Update failed, errc:" << ret.Error().Message().data();
                return ResultCreateHashFunctionCtx::FromError(ret.Error());
            }
        } else {
            break;
        }
    }

    // Close file
    std::ignore = nai_file_close(fd);

    // Calculation complete
    AraResult< AraVector< AraByte > > const retFinish{hashCtx->Finish()};
    if (!retFinish.HasValue()) {
        LOGE << "hashCtx Finish failed, errc:" << retFinish.Error().Message().data();
        return ResultCreateHashFunctionCtx::FromError(retFinish.Error());
    }

    LOGD << "end.";
    return retHashCtx;
}

}  //  namespace pkgmgr
}  //  namespace ucm
}  //  namespace ara
