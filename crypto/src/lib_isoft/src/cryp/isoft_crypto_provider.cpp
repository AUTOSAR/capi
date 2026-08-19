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
/// @file       isoft_crypto_provider.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Crypto Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=PCryptoProvider
/// @unit_description=Crypto Provider
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_crypto_provider.h"

#include <openssl/pem.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/common/isoft_volatile_trusted_container.h"
#include "ara/crypto/cryp/ae/isoft_ctx_ae_stream_aes.h"
#include "ara/crypto/cryp/ae/isoft_ctx_ae_stream_des.h"
#include "ara/crypto/cryp/asymmetric/isoft_ctx_decryptor_private_rsa.h"
#include "ara/crypto/cryp/asymmetric/isoft_ctx_encryptor_public_rsa.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_signature.h"
#include "ara/crypto/cryp/dh/isoft_ctx_key_agreement_private.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_msg_recovery_public_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_sig_encode_private_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_ecdsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa_pss.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_ecdsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa_pss.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_crc.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_md5.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function_sha.h"
#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_aes.h"
#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_des.h"
#include "ara/crypto/cryp/kem/isoft_ctx_key_decapsulator_private_rsa.h"
#include "ara/crypto/cryp/kem/isoft_ctx_key_encapsulator_public_rsa.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_aes_cbc.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_des_cbc.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_hash_md5.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_hash_sha.h"
#include "ara/crypto/cryp/rng/isoft_ctx_rng_default.h"
#include "ara/crypto/cryp/rng/isoft_ctx_rng_global.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_des.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_des.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_pad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_unpad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"
#include "ara/crypto/internal/initialize.h"
#include "ara/crypto/keys/isoft_ipc_io_interface.h"
#include "ara/crypto/keys/isoft_ipc_key_provider.h"

namespace {
/// @brief Type alias: Serializable
using Serializable = ara::crypto::Serializable;
/// @brief Type alias: EPhCtxTypeID
using EPhCtxTypeID = ara::crypto::cryp::isoft_def::EPhCtxTypeID;
/// @brief Type alias: AlgId
using AlgId = ara::crypto::cryp::CryptoProvider::AlgId;
/// @brief Get the buffer size required to obtain a serialized object of the specified algorithm and format from a private key.
/// @param algId Crypto primitive ID of the encryption algorithm
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return The buffer size required to serialize the object in the specific format.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
std::size_t L_GetSizeFromPrivateKey(AlgId const algId, Serializable::FormatId const formatId) noexcept
{
    std::size_t nSize{};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kAsymmetricRsaKey:
        case EPhCtxTypeID::kAsymmetricRsa:  // Default uses 1024-bit length
        case EPhCtxTypeID::kAsymmetricRsaKey1024: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_607U;
            } else {
                nSize = ara::crypto::kInt_887U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey512: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_317U;
            } else {
                nSize = ara::crypto::kInt_493U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey2048: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_1190U;
            } else {
                nSize = ara::crypto::kInt_1675U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey4096: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_2347U;
            } else {
                nSize = ara::crypto::kInt_3243U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey8192: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_4651U;
            } else {
                nSize = ara::crypto::kInt_6363U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey:
        case EPhCtxTypeID::kAsymmetricEccKey128: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_70U;
            } else {
                nSize = ara::crypto::kInt_158U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey192: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_94U;
            } else {
                nSize = ara::crypto::kInt_190U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey256: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_118U;
            } else {
                nSize = ara::crypto::kInt_223U;
            }
        } break;
        default: {
        } break;
    }
    return nSize;
}
/// @brief Get the buffer size required to obtain a serialized object of the specified algorithm and format from a public key.
/// @param algId Crypto primitive ID of the encryption algorithm
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return The buffer size required to serialize the object in the specific format.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
std::size_t L_GetSizeFromPublicKey(AlgId const algId, Serializable::FormatId const formatId) noexcept
{
    std::size_t nSize{};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kAsymmetricRsaKey:
        case EPhCtxTypeID::kAsymmetricRsa:  // Default uses 1024-bit length
        case EPhCtxTypeID::kAsymmetricRsaKey1024: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_140U;
            } else {
                nSize = ara::crypto::kInt_251U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey512: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_74U;
            } else {
                nSize = ara::crypto::kInt_162U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey2048: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_270U;
            } else {
                nSize = ara::crypto::kInt_426U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey4096: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_526U;
            } else {
                nSize = ara::crypto::kInt_775U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey8192: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_1038U;
            } else {
                nSize = ara::crypto::kInt_1466U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey:
        case EPhCtxTypeID::kAsymmetricEccKey128: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_56U;
            } else {
                nSize = ara::crypto::kInt_130U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey192: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_72U;
            } else {
                nSize = ara::crypto::kInt_150U;
            }
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey256: {
            if (formatId == Serializable::kFormatDerEncoded) {
                nSize = ara::crypto::kInt_88U;
            } else {
                nSize = ara::crypto::kInt_174U;
            }
        } break;
        default: {
        } break;
    }
    return nSize;
}

}  // namespace
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Convert the common name of a cryptographic algorithm to the corresponding vendor-specific binary algorithm ID.
/// @param primitiveName Crypto primitive string
/// @returns  vendor specific binary algorithm ID or @c kAlgIdUndefined if a primitive with provided name is not
///           supported
CryptoProvider::AlgId PCryptoProvider::ConvertToAlgId(ara::core::StringView const &primitiveName) const noexcept
{
    if (false == internal::IsCryptoInitialize()) {
        return static_cast< uint64_t >(EPhCtxTypeID::kUnDefine);
    }
    return convertAlgID_.ConvertToAlgId(primitiveName);
}
/// @brief Convert a vendor-specific binary algorithm ID to the corresponding common name of the cryptographic algorithm.
/// @brief Convert a vendor specific binary algorithm ID to a correspondent common name of the crypto algorithm.
/// @param algId Crypto primitive ID of the encryption algorithm
/// @return the common name of the crypto algorithm (see "Crypto Primitives Naming Convention" for more details)
ara::core::Result< ara::core::String > PCryptoProvider::ConvertToAlgName(AlgId algId) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::String >);
    ara::core::String const stAlgName{convertAlgID_.ConvertToAlgName(algId).data()};
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (stAlgName.empty()) {
        return ara::core::Result< ara::core::String >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    return ara::core::Result< ara::core::String >::FromValue(stAlgName);
}
//********************************/
/// @brief Create a symmetric authenticated cipher context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< AuthCipherCtx::Uptr > PCryptoProvider::CreateAuthCipherCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< AuthCipherCtx::Uptr >);
    // if algId argument has an unsupported value : algId takes an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< AuthCipherCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    AuthCipherCtx::Uptr pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kAe_Aes: {
            pReturn = std::make_unique< PCtxAeStreamAes_Cbc >(*this);
        } break;
        case EPhCtxTypeID::kAe_Des: {
            pReturn = std::make_unique< PCtxAeStreamDes_Cbc >(*this);
        } break;
        default: {
        } break;
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm
    /// different from asymmetric encryption/decryption
    // The algID parameter specifies an encryption algorithm different from asymmetric encryption/decryption
    if (pReturn == nullptr) {
        return ara::core::Result< AuthCipherCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< AuthCipherCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a decryption private key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< DecryptorPrivateCtx::Uptr > PCryptoProvider::CreateDecryptorPrivateCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< DecryptorPrivateCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< DecryptorPrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    DecryptorPrivateCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kAsymmetricRsa)) {
        pReturn = std::make_unique< PCtxDecryptorPrivate_Rsa >(*this);
    }

    if (pReturn == nullptr) {
        /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm
        /// different from asymmetric encryption/decryption
        return ara::core::Result< DecryptorPrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< DecryptorPrivateCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create an encryption public key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< EncryptorPublicCtx::Uptr > PCryptoProvider::CreateEncryptorPublicCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< EncryptorPublicCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< EncryptorPublicCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    EncryptorPublicCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kAsymmetricRsa)) {
        pReturn = std::make_unique< PCtxEncryptorPublic_Rsa >(*this);
    }

    if (pReturn == nullptr) {
        /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm
        /// different from asymmetric encryption/decryption
        return ara::core::Result< EncryptorPublicCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< EncryptorPublicCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Construct a signature object from directly provided hash digest components.
/// @param hashAlgId Hash function algorithm ID
/// @param value Data involved in algorithm operation
/// @returns  unique smart pointer to the created @c Signature object
ara::core::Result< Signature::Uptrc > PCryptoProvider::CreateHashDigest(AlgId hashAlgId,
                                                                        ReadOnlyMemRegion const &value) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Signature::Uptrc >);
    /// @error SecurityErrorDomain::kInvalidInputSize  if @c the value argument has invalid size (i.e.incompatible with
    /// the hashAlgId argument)
    if (value.empty()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (value.size() > static_cast< size_t >(ara::crypto::cryp::isoft_def::ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ///@error SecurityErrorDomain::kUnknownIdentifier  if @c hashAlgId argument has unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(hashAlgId))) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    ///@error SecurityErrorDomain::kInvalidArgument    if @c hashAlgId argument specifies crypto algorithm different
    /// from a hash function
    if (!convertAlgID_.IsValidHashAlgID(static_cast< uint64_t >(hashAlgId))) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    /// @error: SecurityErrorDomain::kInvalidInputSize  if @c the value argument has invalid size (i.e.incompatible with
    /// the hashAlgId argument)
    /// When hash value is empty, return kInvalidInputSize. The verification of the hash algorithm is successful by default
    ara::core::Result< HashFunctionCtx::Uptr > const hashCtxTemp{CreateHashFunctionCtx(hashAlgId)};
    if (!hashCtxTemp.HasValue()) {
        return ara::core::Result< Signature::Uptrc >::FromError(hashCtxTemp.Error());
    }
    PCtxHashFunction *const hashCtx{dynamic_cast< PCtxHashFunction * >(hashCtxTemp->get())};
    if (value.empty()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (!hashCtx->CheckSize(value)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    PSignature::Uptr pSignature{std::make_unique< PSignature >(hashAlgId)};
    pSignature->SetSignatureData(hashAlgId, value.data(), static_cast< uint32_t >(value.size()));

    return ara::core::Result< Signature::Uptrc >::FromValue(std::move(pSignature));
}
/// @brief Create a hash function context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< HashFunctionCtx::Uptr > PCryptoProvider::CreateHashFunctionCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< HashFunctionCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< HashFunctionCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    HashFunctionCtx::Uptr pReturn;
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kHashCrc8: {
            pReturn = std::make_unique< PCtxHashFunctionCrc8 >(*this);
        } break;
        case EPhCtxTypeID::kHashCrc16: {
            pReturn = std::make_unique< PCtxHashFunctionCrc16 >(*this);
        } break;
        case EPhCtxTypeID::kHashCrc32: {
            pReturn = std::make_unique< PCtxHashFunctionCrc32 >(*this);
        } break;
        case EPhCtxTypeID::kHashCrc64: {
            pReturn = std::make_unique< PCtxHashFunctionCrc64 >(*this);
        } break;
        case EPhCtxTypeID::kHashMd5: {
            pReturn = std::make_unique< PCtxHashFunctionMd5 >(*this);
        } break;
        case EPhCtxTypeID::kHashSha1: {
            pReturn = std::make_unique< PCtxHashFunctionSha1 >(*this);
        } break;
        case EPhCtxTypeID::kHashSha2_224: {
            pReturn = std::make_unique< PCtxHashFunctionSha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kHashSha2_256: {
            pReturn = std::make_unique< PCtxHashFunctionSha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kHashSha2_384: {
            pReturn = std::make_unique< PCtxHashFunctionSha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kHashSha2_512: {
            pReturn = std::make_unique< PCtxHashFunctionSha2_512 >(*this);
        } break;
        default: {
        } break;
    }
    if (pReturn == nullptr) {
        return ara::core::Result< HashFunctionCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< HashFunctionCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a key agreement private key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< KeyAgreementPrivateCtx::Uptr > PCryptoProvider::CreateKeyAgreementPrivateCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< KeyAgreementPrivateCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< KeyAgreementPrivateCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    // RSA
    KeyAgreementPrivateCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kKey_agreement_private)) {
        pReturn = std::make_unique< PCtxKeyAgreementPrivate >(*this);
    }

    if (pReturn == nullptr) {
        /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm
        /// different from asymmetric encryption/decryption
        return ara::core::Result< KeyAgreementPrivateCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< KeyAgreementPrivateCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a KEM (Key Encapsulation Mechanism) key-decapsulator private key context.
/// @returns  unique smart pointer to the created context
/// @param algId Crypto primitive ID
ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr > PCryptoProvider::CreateKeyDecapsulatorPrivateCtx(
    AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    KeyDecapsulatorPrivateCtx::Uptr pReturn;
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kKem_decapsulator_rsa)) {
        pReturn = std::make_unique< PCtxKeyDecapsulatorPrivateRsa >(*this);
    }

    if (pReturn == nullptr) {
        return ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    return ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a key derivation function context.
/// @returns unique smart pointer to the created context
/// @param algId Crypto primitive ID
ara::core::Result< KeyDerivationFunctionCtx::Uptr > PCryptoProvider::CreateKeyDerivationFunctionCtx(
    AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< KeyDerivationFunctionCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< KeyDerivationFunctionCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    KeyDerivationFunctionCtx::Uptr pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kKdf_Des: {
            pReturn = std::make_unique< PCtxKdf_Des >(*this);
        } break;
        case EPhCtxTypeID::kKdf_3Des: {
            pReturn = std::make_unique< PCtxKdf_3Des >(*this);
        } break;
        case EPhCtxTypeID::kKdf_Aes: {
            pReturn = std::make_unique< PCtxKdf_Aes >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Def));
        } break;
        case EPhCtxTypeID::kKdf_Aes128: {
            pReturn = std::make_unique< PCtxKdf_Aes >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128));
        } break;
        case EPhCtxTypeID::kKdf_Aes192: {
            pReturn = std::make_unique< PCtxKdf_Aes >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192));
        } break;
        case EPhCtxTypeID::kKdf_Aes256: {
            pReturn = std::make_unique< PCtxKdf_Aes >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256));
        } break;
        default: {
        } break;
    }
    if (pReturn == nullptr) {
        return ara::core::Result< KeyDerivationFunctionCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< KeyDerivationFunctionCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a key encapsulation mechanism (KEM) key encapsulator public key context.
/// @returns  unique smart pointer to the created context
/// @param algId Crypto primitive ID
ara::core::Result< KeyEncapsulatorPublicCtx::Uptr > PCryptoProvider::CreateKeyEncapsulatorPublicCtx(
    AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< KeyEncapsulatorPublicCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< KeyEncapsulatorPublicCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    KeyEncapsulatorPublicCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kKem_encapsulator_rsa)) {
        pReturn = std::make_unique< PCtxKeyEncapsulatorPublicRsa >(*this);
    }

    if (pReturn == nullptr) {
        return ara::core::Result< KeyEncapsulatorPublicCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< KeyEncapsulatorPublicCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a symmetric message authentication code context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< MessageAuthnCodeCtx::Uptr > PCryptoProvider::CreateMessageAuthCodeCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< MessageAuthnCodeCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< MessageAuthnCodeCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    MessageAuthnCodeCtx::Uptr pReturn;
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kMac_Aes_Cbc_128: {
            pReturn = std::make_unique< PCtxMacAesCbc_128 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Aes_Cbc_192: {
            pReturn = std::make_unique< PCtxMacAesCbc_192 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Aes_Cbc_256: {
            pReturn = std::make_unique< PCtxMacAesCbc_256 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Des_Cbc: {
            pReturn = std::make_unique< PCtxMacDesCbc >(*this);
        } break;
        case EPhCtxTypeID::kMac_Des_Cbc_ede2: {
            pReturn = std::make_unique< PCtxMacDesCbc_Ede2 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Des_Cbc_ede3: {
            pReturn = std::make_unique< PCtxMacDesCbc_Ede3 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Md5: {
            pReturn = std::make_unique< PCtxMacHashMd5 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Sha1: {
            pReturn = std::make_unique< PCtxMacHashSha1 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Sha2_224: {
            pReturn = std::make_unique< PCtxMacHashSha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Sha2_256: {
            pReturn = std::make_unique< PCtxMacHashSha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Sha2_384: {
            pReturn = std::make_unique< PCtxMacHashSha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kMac_Hash_Sha2_512: {
            pReturn = std::make_unique< PCtxMacHashSha2_512 >(*this);
        } break;
        default: {
        } break;
    }
    if (pReturn == nullptr) {
        return ara::core::Result< MessageAuthnCodeCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< MessageAuthnCodeCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a message recovery public key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< MsgRecoveryPublicCtx::Uptr > PCryptoProvider::CreateMsgRecoveryPublicCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< MsgRecoveryPublicCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< MsgRecoveryPublicCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    MsgRecoveryPublicCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kDsv_MsgRecoveryRsa)) {
        pReturn = std::make_unique< PCtxDsv_MsgRecoveryPublic_Rsa >(*this);
    }

    if (pReturn == nullptr) {
        return ara::core::Result< MsgRecoveryPublicCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< MsgRecoveryPublicCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a random number generator (RNG) context.
/// @param algId Crypto primitive ID
/// @param initialize Whether to initialize
/// @returns  unique smart pointer to the created context
ara::core::Result< RandomGeneratorCtx::Uptr > PCryptoProvider::CreateRandomGeneratorCtx(AlgId algId,
                                                                                        bool initialize) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< RandomGeneratorCtx::Uptr >);
    if (kAlgIdDefault == algId) {
        RandomGeneratorCtx::Uptr pReturn{std::make_unique< PCtxRng_Default >(*this)};
        if (initialize) {
            PCtxRng_Default *const pWorkRandom{dynamic_cast< PCtxRng_Default * >(pReturn.get())};
            if (false == pWorkRandom->InitlizeRng()) {
                return ara::core::Result< RandomGeneratorCtx::Uptr >::FromError(
                    SecurityErrorDomain::Errc::kBusyResource);
            }
        }
        return ara::core::Result< RandomGeneratorCtx::Uptr >::FromValue(std::move(pReturn));
    }
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value or if <tt>(algId
    /// == kAlgIdDefault)</tt> and
    ///                                                 the CryptoProvider does not provide any RandomGeneratorCtx
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< RandomGeneratorCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    RandomGeneratorCtx::Uptr pReturn;
    /// @error: SecurityErrorDomain::kBusyResource       if <tt>(initialize == true)</tt> but the context currently
    /// cannot be seeded (e.g., due to a lack of entropy)
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kRandomGenerator)) {
        pReturn = std::make_unique< PCtxRng_Default >(*this);
        if (initialize) {
            PCtxRng_Default *const pWorkRandom{dynamic_cast< PCtxRng_Default * >(pReturn.get())};
            if (false == pWorkRandom->InitlizeRng()) {
                return ara::core::Result< RandomGeneratorCtx::Uptr >::FromError(
                    SecurityErrorDomain::Errc::kBusyResource);
            }
        }
    }

    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kRandomGeneratorglobal)) {
        pReturn = std::make_unique< PCtxRng_Global >(*this);
    }

    if (pReturn == nullptr) {
        return ara::core::Result< RandomGeneratorCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< RandomGeneratorCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a signature encoding private key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< SigEncodePrivateCtx::Uptr > PCryptoProvider::CreateSigEncodePrivateCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SigEncodePrivateCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SigEncodePrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from hash function
    SigEncodePrivateCtx::Uptr pReturn{nullptr};
    if (static_cast< uint64_t >(algId) == static_cast< uint64_t >(EPhCtxTypeID::kDsv_SigEncodeRsa)) {
        pReturn = std::make_unique< PCtxDsv_SigEncodePrivate_Rsa >(*this);
    }

    if (pReturn == nullptr) {
        return ara::core::Result< SigEncodePrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< SigEncodePrivateCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Construct a signature object from directly provided digital signature/MAC or authenticated encryption (AE/AEAD) components.
///         All integers in the digital signature BLOB value are always represented in big-endian byte order (i.e., MSF - Most Significant Byte first).
/// @returns  ara::core::Result<Signature::Uptrc> P
/// @param signAlgId Signature algorithm crypto primitive ID
/// @param value Data involved in algorithm operation
/// @param key Key material
/// @param hashAlgId Hash function algorithm ID
/// @return unique smart pointer to the created context
ara::core::Result< Signature::Uptrc > PCryptoProvider::CreateSignature(AlgId signAlgId,
                                                                       ReadOnlyMemRegion const &value,
                                                                       RestrictedUseObject const &key,
                                                                       AlgId hashAlgId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Signature::Uptrc >);
    // if signAlgId or hashAlgId arguments have unsupported values
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(signAlgId))) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    if (!convertAlgID_.IsValidHashAlgID(static_cast< uint64_t >(hashAlgId))) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    bool unknownIdentifier{false};

    // Check whether the signature algorithm (currently RSA) and the hash algorithm are compatible
    switch (static_cast< EPhCtxTypeID >(signAlgId)) {
        case EPhCtxTypeID::kDsv_SignerPrivateRsaMd5: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashMd5) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha1: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashSha1) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_224: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashSha2_224) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashSha2_256) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashSha2_384) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512: {
            if (static_cast< EPhCtxTypeID >(hashAlgId) != EPhCtxTypeID::kHashSha2_512) {
                return ara::core::Result< Signature::Uptrc >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleArguments);
            }
        } break;
        default: {
            unknownIdentifier = true;
        } break;
    }
    if (unknownIdentifier) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    PrivateKey const *const privatekey{dynamic_cast< PrivateKey const * >(&key)};
    if (nullptr == privatekey) {
        return ara::core::Result< Signature::Uptrc >::FromValue(nullptr);
    }

    ara::core::Result< SignerPrivateCtx::Uptr > const signatureResult{CreateSignerPrivateCtx(signAlgId)};
    if (false == signatureResult.HasValue()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    // Downcast
    PCtxDsv_SignerPrivate *const pDsvSignerPrivateCtx{
        dynamic_cast< PCtxDsv_SignerPrivate * >(signatureResult.Value().get())};
    if (nullptr == pDsvSignerPrivateCtx) {
        return ara::core::Result< Signature::Uptrc >::FromValue(nullptr);
    }

    // Check whether the key can be used for the specified signature algorithm
    if (false == pDsvSignerPrivateCtx->CheckKey(*privatekey)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    std::ignore = pDsvSignerPrivateCtx->SetKey(*privatekey);

    pDsvSignerPrivateCtx->SetHashAlgID(static_cast< EPhCtxTypeID >(hashAlgId));

    // Check value before using it
    if (value.empty()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (value.size() > static_cast< size_t >(ara::crypto::cryp::isoft_def::ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    return pDsvSignerPrivateCtx->SignPreHashed(hashAlgId, value);
}
/// @brief Create a signature private key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< SignerPrivateCtx::Uptr > PCryptoProvider::CreateSignerPrivateCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SignerPrivateCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SignerPrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from hash function
    SignerPrivateCtx::Uptr pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kDsv_SignerPrivateRsaMd5: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Md5 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha1: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Sha1 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_224: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Sha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Sha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Sha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Sha2_512 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssMd5: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Md5 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha1: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_224: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_256: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_384: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_512: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_SignerPrivateEcdsa: {
            pReturn = std::make_unique< PCtxDsv_SignerPrivate_Ecdsa >(*this);
        } break;
        default: {
        } break;
    }

    if (pReturn == nullptr) {
        return ara::core::Result< SignerPrivateCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< SignerPrivateCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a symmetric stream cipher context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< StreamCipherCtx::Uptr > PCryptoProvider::CreateStreamCipherCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< StreamCipherCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< StreamCipherCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    StreamCipherCtx::Uptr pReturn;
    switch (static_cast< EPhCtxTypeID >(algId)) {
        // DES
        case EPhCtxTypeID::kSymmetricDesCfb: {
            pReturn = std::make_unique< PCtxSymmetricStream_Des_Cfb >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricDesCfb64: {
            pReturn = std::make_unique< PCtxSymmetricStream_Des_Cfb64 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricDesOfb: {
            pReturn = std::make_unique< PCtxSymmetricStream_Des_Ofb >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricDesOfb64: {
            pReturn = std::make_unique< PCtxSymmetricStream_Des_Ofb64 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetric3DesCfb1: {
            pReturn = std::make_unique< PCtxSymmetricStream_3Des_Cfb1 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetric3DesCfb64: {
            pReturn = std::make_unique< PCtxSymmetricStream_3Des_Cfb64 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetric3DesOfb64: {
            pReturn = std::make_unique< PCtxSymmetricStream_3Des_Ofb64 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesCfb1: {
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Cfb1 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesCfb8: {
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Cfb8 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesCfb128: {
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Cfb128 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesOfb: {  // NOLINT
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Ofb128 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesOfb128: {
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Ofb128 >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesCtr: {
            pReturn = std::make_unique< PCtxSymmetricStream_Aes_Ctr >(*this);
        } break;
        default: {
            /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm
            /// different from symmetric stream cipher
        } break;
    }

    if (pReturn == nullptr) {
        return ara::core::Result< StreamCipherCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< StreamCipherCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a symmetric block cipher context.
/// @param algId Crypto primitive ID
/// @returns unique smart pointer to the created context
ara::core::Result< SymmetricBlockCipherCtx::Uptr > PCryptoProvider::CreateSymmetricBlockCipherCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SymmetricBlockCipherCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SymmetricBlockCipherCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    SymmetricBlockCipherCtx::Uptr pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        // DES
        case EPhCtxTypeID::kSymmetricDesEcb: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Des_Ecb >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricDesCbc: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Des_Cbc >(*this);
        } break;
        case EPhCtxTypeID::kSymmetric3DesEcb: {
            pReturn = std::make_unique< PCtxSymmetricBlock_3Des_Ecb >(*this);
        } break;
        case EPhCtxTypeID::kSymmetric3DesCbc: {
            pReturn = std::make_unique< PCtxSymmetricBlock_3Des_Cbc >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesEcb: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Ecb >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesEcb128: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Ecb >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128));
        } break;
        case EPhCtxTypeID::kSymmetricAesEcb192: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Ecb >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192));
        } break;
        case EPhCtxTypeID::kSymmetricAesEcb256: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Ecb >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256));
        } break;
        case EPhCtxTypeID::kSymmetricAesCbc: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Cbc >(*this);
        } break;
        case EPhCtxTypeID::kSymmetricAesCbc128: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Cbc >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128));
        } break;
        case EPhCtxTypeID::kSymmetricAesCbc192: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Cbc >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192));
        } break;
        case EPhCtxTypeID::kSymmetricAesCbc256: {
            pReturn = std::make_unique< PCtxSymmetricBlock_Aes_Cbc >(
                *this, static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256));
        } break;
        default: {
        } break;
    }
    if (pReturn == nullptr) {
        return ara::core::Result< SymmetricBlockCipherCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< SymmetricBlockCipherCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a symmetric key-wrap algorithm context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< SymmetricKeyWrapperCtx::Uptr > PCryptoProvider::CreateSymmetricKeyWrapperCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SymmetricKeyWrapperCtx::Uptr >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SymmetricKeyWrapperCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    SymmetricKeyWrapperCtx::Uptr pReturn;
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kWrap_Aes_pad: {
            pReturn = std::make_unique< PCtxSymmetricKeyWrapperAesPad >(*this);
            break;
        }
        case EPhCtxTypeID::kWrap_Aes_unpad: {
            pReturn = std::make_unique< PCtxSymmetricKeyWrapperAesUnPad >(*this);
            break;
        } break;
        case EPhCtxTypeID::kWrap_Des: {
            pReturn = std::make_unique< PCtxSymmetricKeyWrapperDes >(*this);
        } break;
        default: {
        } break;
    }
    if (pReturn == nullptr) {
        return ara::core::Result< SymmetricKeyWrapperCtx::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< SymmetricKeyWrapperCtx::Uptr >::FromValue(std::move(pReturn));
}
/// @brief Create a signature verification public key context.
/// @param algId Crypto primitive ID
/// @returns  unique smart pointer to the created context
ara::core::Result< VerifierPublicCtx::Uptr > PCryptoProvider::CreateVerifierPublicCtx(AlgId algId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< VerifierPublicCtx::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< VerifierPublicCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from hash function
    VerifierPublicCtx::Uptr pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kDsv_VerifierPublicRsaMd5: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Md5 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsaSha1: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Sha1 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_224: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Sha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_256: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Sha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_384: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Sha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsaSha2_512: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Sha2_512 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssMd5: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Md5 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha1: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_224: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_256: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_384: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicRsa_PssSha2_512: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 >(*this);
        } break;
        case EPhCtxTypeID::kDsv_VerifierPublicEcdsa: {
            pReturn = std::make_unique< PCtxDsv_VerifierPublic_Ecdsa >(*this);
        } break;
        default: {
        } break;
    }

    if (pReturn == nullptr) {
        return ara::core::Result< VerifierPublicCtx::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< VerifierPublicCtx::Uptr >::FromValue(std::move(pReturn));
}
//********************************/
/// @brief Allocate a Volatile Trusted Container according to a directly specified capacity.
/// The Volatile Trusted Container can be used to perform import operations. The current process acquires "owner" permission of the allocated container.
/// If (capacity == 0), the container capacity will be automatically selected based on the maximum size of the supported crypto objects.
/// Several volatile containers can coexist simultaneously without affecting each other.
/// @param capacity Capacity
/// @return unique smart pointer to an allocated volatile trusted container
ara::core::Result< VolatileTrustedContainer::Uptr > PCryptoProvider::AllocVolatileContainer(
    std::size_t capacity) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< VolatileTrustedContainer::Uptr >);
    if (capacity == 0U) {
        ara::core::Result< std::size_t > const resSize{
            GetSerializedSize(CryptoObjectType::kPrivateKey, static_cast< AlgId >(EPhCtxTypeID::kAsymmetricRsaKey8192),
                              Serializable::kFormatPemEncoded)};
        if (resSize.HasValue()) {
            capacity = resSize.Value();
        }
    }
    ara::crypto::keys::isoft_def::PVolatileTrustedContainer::Uptr pVolatileTrustedContainer{
        std::make_unique< ara::crypto::keys::isoft_def::PVolatileTrustedContainer >(capacity)};
    if (capacity == 0U) {
    }

    return ara::core::Result< VolatileTrustedContainer::Uptr >::FromValue(std::move(pVolatileTrustedContainer));
}
/// @brief Allocate a Volatile Trusted Container indirectly specified by the minimum capacity required to host any of the listed objects.
///         The Volatile Trusted Container can be used to perform import operations. The current process acquires "owner" permission of the allocated container.
///         The actual container capacity is calculated as the maximum storage size of all listed objects.
/// @param theObjectDef Default object type
/// @return unique smart pointer to an allocated volatile trusted container
ara::core::Result< VolatileTrustedContainer::Uptr > PCryptoProvider::AllocVolatileContainer(
    std::pair< AlgId, CryptoObjectType > const &theObjectDef) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< VolatileTrustedContainer::Uptr >);
    AlgId const algId{theObjectDef.first};
    CryptoObjectType const cryptoObjectType{theObjectDef.second};
    std::size_t capacity{0U};

    ara::core::Result< std::size_t > const resSize{GetPayloadStorageSize(cryptoObjectType, algId)};
    if (!resSize.HasValue()) {
        return ara::core::Result< VolatileTrustedContainer::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    capacity = resSize.Value();
    /// @error: SecurityErrorDomain::kInvalidArgument    if unsupported combination of object type and algorithm ID
    if (capacity == 0U) {
        return ara::core::Result< VolatileTrustedContainer::Uptr >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    return AllocVolatileContainer(capacity);
}
/// @brief Securely export a crypto object.
/// If (serialization.empty() == true), this method only returns the required size, but the content of transportContext remains unchanged!
/// Only an exportable and completed object (i.e., an object with a UUID) can be exported!
/// @param object   the crypto object for export
/// @param transportContext the symmetric key wrap context initialized by a transport key
/// @return the wrapped crypto object data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCryptoProvider::ExportSecuredObject(
    CryptoObject const &object, SymmetricKeyWrapperCtx &transportContext) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< ara::core::Byte > >);
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the object cannot be exported due to IsExportable()
    /// return false
    if (!object.IsExportable()) {
        if (object.IsSession()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c transportContext is not initialized
    if (!transportContext.IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    /// @error: SecurityErrorDomain::kIncompatibleObject  if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    if (object.GetObjectId().mCOType != CryptoObjectType::kSymmetricKey) {
        if (object.GetObjectId().mCOType != CryptoObjectType::kSecretSeed) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }

    // Find the object type based on the container
    if (object.GetObjectId().mCOType == CryptoObjectType::kSymmetricKey) {
        PKeySymmetric_Ipc const *const pKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&object)};
        if (pKeySymmetricIpc != nullptr) {
            crypto::isoft_def::LogInfo() << "Export Secured Object From SlotName:" << pKeySymmetricIpc->GetSlotName();
            return transportContext.WrapKeyMaterial(*pKeySymmetricIpc);
        }
        PKeySymmetric_Base const *const pKeySymmetric{dynamic_cast< PKeySymmetric_Base const * >(&object)};
        if (nullptr == pKeySymmetric) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        crypto::isoft_def::LogInfo() << "Export Secured Object From MemIoInterFace";
        return transportContext.WrapKeyMaterial(*pKeySymmetric);
    }
    PSecretSeed_Ipc const *const pSecretSeedIpc{dynamic_cast< PSecretSeed_Ipc const * >(&object)};
    if (pSecretSeedIpc != nullptr) {
        crypto::isoft_def::LogInfo() << "Export Secured Object From SlotName:" << pSecretSeedIpc->GetSlotName();
        return transportContext.WrapKeyMaterial(*pSecretSeedIpc);
    }
    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&object)};
    if (nullptr == pSecretSeed) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    crypto::isoft_def::LogInfo() << "Export Secured Object From MemIoInterFace";
    return transportContext.WrapKeyMaterial(*pSecretSeed);
}
/// @brief Securely export an object directly from the IOInterface (i.e., without the need for an intermediate crypto object creation).
///         If (serialized == nullptr), this method only returns the required size, but the content of transportContext remains unchanged.
///         This method can be used to re-export an object just imported, but under another transport key.
/// @returns  ara::core::Result<ara::core::Vector<ara::core::Byte> > P
/// @param container    the IOInterface that refers an object for export
/// @param transportContext the symmetric key wrap context initialized by a transport key
/// @return  the wrapped crypto object data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCryptoProvider::ExportSecuredObject(
    IOInterface const &container, SymmetricKeyWrapperCtx &transportContext) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< ara::core::Byte > >);
    /// the output data
    /// @error: SecurityErrorDomain::kEmptyContainer         if the @c container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    if (!transportContext.IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    if (container.GetCryptoObjectType() != CryptoObjectType::kSymmetricKey) {
        if (container.GetCryptoObjectType() != CryptoObjectType::kSecretSeed) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kModifiedResource);
    }
    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    if (pIoInterfaceIpc != nullptr) {
        ara::core::Result< CryptoObject::Uptrc > resCryptoObject{LoadObject(container)};
        if (!resCryptoObject.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        CryptoObject::Uptrc pCryptoObject{std::move(resCryptoObject).Value()};

        // Downcast
        ara::core::Result< RestrictedUseObject::Uptrc > const resRestrictedUseObject{
            CryptoObject::Downcast< RestrictedUseObject >(std::move(pCryptoObject))};
        if (!resRestrictedUseObject.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        RestrictedUseObject::Uptrc const &pRestrictedUseObject{resRestrictedUseObject.Value()};
        crypto::isoft_def::LogInfo() << "Export Secured Object From SlotName:"
                                     << pIoInterfaceIpc->GetSlotName().c_str();
        /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the @c serialized buffer is not enough for
        /// saving
        return transportContext.WrapKeyMaterial(*(pRestrictedUseObject));
    }

    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    if (pIoInterfaceMem != nullptr) {
        ara::core::Vector< uint8_t > vecData;
        bool const ret{pIoInterfaceMem->ReadKeyData(vecData)};
        if (!ret) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kEmptyContainer);
        }
        uint8_t *const pData{vecData.data()};
        ReadOnlyMemRegion const memData{pData, vecData.size()};
        // Determine object type based on container
        if (container.GetCryptoObjectType() == CryptoObjectType::kSymmetricKey) {
            ara::core::Result< SymmetricKey::Uptrc > const resSymmetricKey{
                GenerateSymmetricKeyEx(container.GetPrimitiveId(), memData, container.GetAllowedUsage(),
                                       container.IsObjectSession(), container.IsObjectExportable())};
            if (!resSymmetricKey.HasValue()) {
                return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                    SecurityErrorDomain::Errc::kIncompatibleObject);
            }
            crypto::isoft_def::LogInfo() << "Export Secured Object From MemIoInterFace";
            /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the @c serialized buffer is not enough
            /// for saving
            return transportContext.WrapKeyMaterial(*(resSymmetricKey.Value()));
        }
        ara::core::Result< SecretSeed::Uptrc > const resSecretSeed{
            GenerateSeedEx(container.GetPrimitiveId(), memData, container.GetAllowedUsage(),
                           container.IsObjectSession(), container.IsObjectExportable())};
        if (!resSecretSeed.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        crypto::isoft_def::LogInfo() << "Export Secured Object From MemIoInterFace";
        /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the @c serialized buffer is not enough
        /// for saving
        return transportContext.WrapKeyMaterial(*(resSecretSeed.Value()));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
        SecurityErrorDomain::Errc::kMissingArgument);
}
/// @brief Import a securely serialized object into persistent or volatile storage represented by IOInterface for subsequent processing.
/// @param container    the IOInterface for storing of the imported object
/// @param serialized   the memory region that contains a securely serialized object that should be imported to
/// the IOInterface
/// @param transportContext the symmetric key wrap context initialized by a transport key
/// @param isExportable Exportability attribute of the target object
/// @param expectedObject   the expected object type (default value @c CryptoObjectType::kUnknown means without check)
/// @return has value if ImportSecuredObject false otherwise
ara::core::Result< void > PCryptoProvider::ImportSecuredObject(IOInterface &container,
                                                               ReadOnlyMemRegion const &serialized,
                                                               SymmetricKeyWrapperCtx &transportContext,
                                                               bool isExportable,
                                                               CryptoObjectType expectedObject) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    if (container.GetCryptoObjectType() != CryptoObjectType::kSymmetricKey) {
        if (container.GetCryptoObjectType() != CryptoObjectType::kSecretSeed) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the
    /// deserialized object
    /// @error: kInsufficientCapacity if the capacity of the underlying resource pointed to
    //  by the provided IOInterface is insufficient to hold the deserialized CryptoObject. 2311
    if (container.GetCapacity() < serialized.size()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    if (!container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>,
    /// but the actual object type differs from the expected one  2023/8/29
    /// kBadCryptoObjectType does not exist; should be kBadObjectType
    if (expectedObject != CryptoObjectType::kUndefined) {
        if (container.GetCryptoObjectType() != expectedObject) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kBadObjectType);
        }
    }

    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    if (!transportContext.IsInitialized()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompleteArgState);
    }

    AllowedUsageFlags const transCtxAllowedUsage{transportContext.GetExtensionService()->GetAllowedUsage()};
    ///  @error: kUsageViolation if the flag kAllowKeyImporting of the Symmetric Key set
    ///  in the provided SymmetricKeyWrapperCtx is not set to TRUE. 2311
    if ((transCtxAllowedUsage & kAllowKeyImporting) != kAllowKeyImporting) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    if ((transCtxAllowedUsage & kAllowExport) == kAllowExport) {
        isExportable = true;
    }
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the @c serialized contains incorrect data
    ara::core::Result< SymmetricKey::Uptrc > const resSymmetricKey{
        GenerateSymmetricKeyEx(container.GetPrimitiveId(), serialized, container.GetAllowedUsage(),
                               container.IsObjectSession(), isExportable)};
    if (!resSymmetricKey.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const pResult{
        transportContext.WrapKeyMaterial(*(resSymmetricKey.Value()))};
    if (!pResult.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    ara::core::Vector< ara::core::Byte > vecData{pResult.Value()};
    ara::core::Byte *const pData{vecData.data()};
    ReadOnlyMemRegion const securedObject{static_cast< uint8_t const * >(static_cast< void const * >(pData)),
                                          vecData.size()};

    bool const ret{_importSecuredObjectEx(container, securedObject)};
    if (false == ret) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Import Secured Object
/// @param container the IOInterface for storing of the imported object
/// @param securedObject secured object
/// @return true means import success, or it returns false
bool PCryptoProvider::_importSecuredObjectEx(IOInterface const &container,
                                             ReadOnlyMemRegion const securedObject) noexcept
{
    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    if (pIoInterfaceIpc != nullptr) {
        uint32_t nSlotId{pIoInterfaceIpc->GetSlotID()};
        ara::core::String const nSlotName{pIoInterfaceIpc->GetSlotName()};
        if (nSlotId == 0U) {
            return false;
        }
        if (nSlotName.empty()) {
            return false;
        }
        keys::isoft_def::PIpcKeySlot::Uptr const pKeySlot{std::make_unique< keys::isoft_def::PIpcKeySlot >(
            pIoInterfaceIpc->GetIpcClient(), nSlotName.c_str(), nSlotId)};
        if (pKeySlot.get() == nullptr) {
            return false;
        }
        if (nSlotId == 0U) {
        }
        ara::core::Result< VolatileTrustedContainer::Uptr > const resultContainer{
            AllocVolatileContainer(securedObject.size())};
        if (!resultContainer.HasValue()) {
            return false;
        }
        IOInterface &ioInterfaceBase{resultContainer.Value()->GetIOInterface()};
        if (!ImportSlot(pKeySlot.get(), ioInterfaceBase, nSlotName, securedObject)) {
            return false;
        }
        crypto::isoft_def::LogInfo() << "Import Secured Object To SlotName:" << nSlotName.c_str();
    } else {
        keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
            dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
        if (pIoInterfaceMem != nullptr) {
            std::ignore = pIoInterfaceMem->SaveKeyData(securedObject);
        }
        crypto::isoft_def::LogInfo() << "Import Secured Object To MemIoInterFace";
    }
    return true;
}
/// @brief Export a public object from IOInterface (i.e., without intermediate creation of a crypto object).
/// @param container IO interface object
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return actual capacity required for the serialized data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCryptoProvider::ExportPublicObject(
    IOInterface const &container, Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< ara::core::Byte > >);
    /// @brief Result type alias
    using PResult = ara::core::Result< ara::core::Vector< ara::core::Byte > >;
    /// @error: SecurityErrorDomain::kEmptyContainer         if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return PResult::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error SecurityErrorDomain::kUnexpectedValue        if the container contains a secret crypto object
    if (container.GetCryptoObjectType() == CryptoObjectType::kSecretSeed) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (container.GetCryptoObjectType() == CryptoObjectType::kPrivateKey) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (container.GetCryptoObjectType() == CryptoObjectType::kSymmetricKey) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }

    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return PResult::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    // IOInterface does not distinguish between local and IPC here; processing logic is the same
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPublicKey{LoadPublicKey(container)};
    if (!resPublicKey.HasValue()) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    PKeyPublic_Base const *const pKeyPublic{dynamic_cast< PKeyPublic_Base const * >(resPublicKey.Value().get())};
    if (nullptr == pKeyPublic) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if <tt>(serialized.empty() == false)</tt>, but its capacity
    /// is not enough for storing result
    crypto::isoft_def::LogInfo() << "Export Public Object From IOInterface";
    return pKeyPublic->ExportPublicly(formatId);
}
/// @brief Import a publicly serialized object to the storage location pointed to by IOInterface for subsequent processing (does not allocate a crypto object).
///         If (expectedObject != CryptoObjectType::kUnknown) and the actual object type differs from the expected one, this method fails.
///         If the serialized object contains incorrect data, this method fails.
/// @param container    the IOInterface for storing of the imported object
/// @param serialized   the memory region that contains a securely serialized object that should be imported to
/// the IOInterface
/// @param expectedObject   the expected object type (default value @c CryptoObjectType::kUnknown means without check)
/// @return  has vlaue if ImportPublicObject sucess false otherwise
ara::core::Result< void > PCryptoProvider::ImportPublicObject(IOInterface &container,
                                                              ReadOnlyMemRegion const &serialized,
                                                              CryptoObjectType expectedObject) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writable.
    /// @threadsafety={Thread-safe}
    if (!container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>,
    /// but the actual object type differs from the expected one  2023/8/29
    /// kBadCryptoObjectType does not exist; should be kBadObjectType
    if (expectedObject != CryptoObjectType::kUndefined) {
        if (container.GetCryptoObjectType() != expectedObject) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kBadObjectType);
        }
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the
    /// de-serialized object
    if (container.GetCapacity() < serialized.size()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }

    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    if (pIoInterfaceIpc != nullptr) {
        uint32_t nSlotId{pIoInterfaceIpc->GetSlotID()};
        ara::core::String const nSlotName{pIoInterfaceIpc->GetSlotName()};
        if (nSlotId == 0U) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
        }
        if (nSlotName.empty()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
        }
        keys::isoft_def::PIpcKeySlot::Uptr const pKeySlot{std::make_unique< keys::isoft_def::PIpcKeySlot >(
            pIoInterfaceIpc->GetIpcClient(), nSlotName.c_str(), nSlotId)};
        if (pKeySlot.get() == nullptr) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
        }
        if (nSlotId == 0U) {
        }
        ara::core::Result< VolatileTrustedContainer::Uptr > const resultContainer{
            AllocVolatileContainer(serialized.size())};
        if (!resultContainer.HasValue()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
        }
        IOInterface &ioInterfaceBase{resultContainer.Value()->GetIOInterface()};
        if (!ImportSlot(pKeySlot.get(), ioInterfaceBase, nSlotName, serialized)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
        }
        crypto::isoft_def::LogInfo() << "Import Public Object To SlotName:" << nSlotName.c_str();
    }
    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    if (pIoInterfaceMem != nullptr) {
        std::ignore = pIoInterfaceMem->SaveKeyData(serialized);
    }
    crypto::isoft_def::LogInfo() << "Import Public Object To MemIoInterFace";
    return ara::core::Result< void >::FromValue();
}
//***************/
namespace {
/// @brief Template function for generating private keys
/// @tparam T_KeyType
/// @param nKeyBitLength Key length: in bits
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return Private key object instance
template < typename T_KeyType >
typename T_KeyType::Uptr T_MakePrivateKey(int32_t const nKeyBitLength,
                                          AllowedUsageFlags const allowedUsage,
                                          bool const isSession,
                                          bool const isExportable) noexcept(noexcept(std::make_unique< T_KeyType >()))
{
    typename T_KeyType::Uptr pRsaKey{std::move(std::make_unique< T_KeyType >())};
    if (pRsaKey) {
        std::ignore = pRsaKey->RandomInitKey(nKeyBitLength);
        pRsaKey->SetAllowedUsage(allowedUsage);
        pRsaKey->SetSession(isSession);
        pRsaKey->SetExportable(isExportable);
    }
    return pRsaKey;
}

/// @brief Template function for generating private keys
/// @tparam T_KeyType
/// @param stPrivateKey Private key
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return
/// @throws
template < typename T_KeyType >
typename T_KeyType::Uptr T_MakePrivateKey(ara::core::StringView const &stPrivateKey,
                                          AllowedUsageFlags const allowedUsage,
                                          bool const isSession,
                                          bool const isExportable) noexcept(noexcept(std::make_unique< T_KeyType >()))
{
    typename T_KeyType::Uptr pPrivateKey{std::move(std::make_unique< T_KeyType >())};
    if (pPrivateKey) {
        std::ignore = pPrivateKey->AttachKey(stPrivateKey);
        pPrivateKey->SetAllowedUsage(allowedUsage);
        pPrivateKey->SetSession(isSession);
        pPrivateKey->SetExportable(isExportable);
    }
    return pPrivateKey;
}

/// @brief Template function for generating private keys: ECC
/// @tparam T_KeyType
/// @param nGenKeyBitLength Key length: in bits
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return Private key object instance
template < typename T_KeyType >
typename T_KeyType::Uptr T_MakePrivateKeyEcc(
    int32_t const nGenKeyBitLength,
    AllowedUsageFlags const allowedUsage,
    bool const isSession,
    bool const isExportable) noexcept(noexcept(std::make_unique< T_KeyType >(nGenKeyBitLength)))
{
    typename T_KeyType::Uptr pEccKey{std::move(std::make_unique< T_KeyType >(nGenKeyBitLength))};
    if (pEccKey) {
        pEccKey->SetAllowedUsage(allowedUsage);
        pEccKey->SetSession(isSession);
        pEccKey->SetExportable(isExportable);
    }
    return pEccKey;
}

/// @brief IPC object generation template function:
/// @tparam T_KeyType
/// @param nSlotId Key slot ID
/// @param nIoInerfaceID IO interface ID
/// @param nSlotName Key slot name
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return Key object instance
template < typename T_KeyType >
typename T_KeyType::Uptr T_MakeObjectIpc(
    uint32_t const nSlotId,
    uint32_t const nIoInerfaceID,
    ara::core::StringView const &nSlotName,
    AllowedUsageFlags const allowedUsage,
    bool const isSession,
    bool const isExportable) noexcept(noexcept(std::make_unique< T_KeyType >(nSlotId, nIoInerfaceID, nSlotName)))
{
    typename T_KeyType::Uptr pKeyIpc{std::move(std::make_unique< T_KeyType >(nSlotId, nIoInerfaceID, nSlotName))};
    if (pKeyIpc) {
        pKeyIpc->SetAllowedUsage(allowedUsage);
        pKeyIpc->SetSession(isSession);
        pKeyIpc->SetExportable(isExportable);
    }
    return pKeyIpc;
}

}  // namespace

/// @brief Allocate a new private key context of the corresponding type and randomly generate the key value.
///         A common COUID should be shared for private and public keys. Any serializable (i.e., saveable/non-session or exportable) key must generate its own COUID!
/// @param algId Crypto primitive ID
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable  the exportability attribute of the target key (if @c true)
/// @returns smart unique pointer to the created private key object
ara::core::Result< PrivateKey::Uptrc > PCryptoProvider::GeneratePrivateKey(AlgId algId,
                                                                           AllowedUsageFlags allowedUsage,
                                                                           bool isSession,
                                                                           bool isExportable) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< PrivateKey::Uptrc >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    if ((allowedUsage & kAllowExport) == kAllowExport) {
        isExportable = true;
    }
    // Unique Key ID is now managed by convertAlgID_ (2023.9.4)
    // /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    PrivateKey::Uptrc pReturn{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kAsymmetricRsa: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_1024, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey512: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_512, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey1024: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_1024, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey2048: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_2048, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey4096: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_4096, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricRsaKey8192: {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >(kInt_8192, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey128: {
            pReturn = T_MakePrivateKeyEcc< PKeyPrivate_Ecc >(kInt_128, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey192: {
            pReturn = T_MakePrivateKeyEcc< PKeyPrivate_Ecc >(kInt_192, allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kAsymmetricEccKey256: {
            pReturn = T_MakePrivateKeyEcc< PKeyPrivate_Ecc >(kInt_256, allowedUsage, isSession, isExportable);
        } break;
        default: {
            /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
        } break;
    }
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    /// algorithm @c algId (note: it is an optional error condition for this method)
    if (pReturn == nullptr) {
        return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    return ara::core::Result< PrivateKey::Uptrc >::FromValue(std::move(pReturn));
}
/// @brief Generate a random Secret Seed object for the requested algorithm.
/// @param algId Crypto primitive ID
/// @param memKeyData Key data in memory
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @returns  smart unique pointer to the created private key object
ara::core::Result< SecretSeed::Uptrc > PCryptoProvider::GenerateSeedEx(AlgId const algId,
                                                                       ReadOnlyMemRegion const &memKeyData,
                                                                       AllowedUsageFlags const allowedUsage,
                                                                       bool const isSession,
                                                                       bool const isExportable) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SecretSeed::Uptrc >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    uint32_t nlen{0U};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kAe_Des:
        case EPhCtxTypeID::kKdf_Des:
        case EPhCtxTypeID::kKdf_3Des:
        case EPhCtxTypeID::kSymmetric3DesCfb1:
        case EPhCtxTypeID::kSymmetric3DesCfb64:
        case EPhCtxTypeID::kSymmetric3DesOfb64:
        case EPhCtxTypeID::kSymmetricDesCfb:
        case EPhCtxTypeID::kSymmetricDesCfb64:
        case EPhCtxTypeID::kSymmetricDesOfb:
        case EPhCtxTypeID::kSymmetricDesOfb64:
        case EPhCtxTypeID::kMac_Des_Cbc:
        case EPhCtxTypeID::kMac_Des_Cbc_ede2:
        case EPhCtxTypeID::kMac_Des_Cbc_ede3: {
            nlen = kInt_8U;
        } break;
        case EPhCtxTypeID::kAe_Aes:
        case EPhCtxTypeID::kKdf_Aes:
        case EPhCtxTypeID::kKdf_Aes128:
        case EPhCtxTypeID::kKdf_Aes192:
        case EPhCtxTypeID::kKdf_Aes256:
        case EPhCtxTypeID::kSymmetricAesCfb1:
        case EPhCtxTypeID::kSymmetricAesCfb8:
        case EPhCtxTypeID::kSymmetricAesCfb128:
        case EPhCtxTypeID::kSymmetricAesOfb128:
        case EPhCtxTypeID::kMac_Aes_Cbc_128:
        case EPhCtxTypeID::kMac_Aes_Cbc_192:
        case EPhCtxTypeID::kMac_Aes_Cbc_256:
        case EPhCtxTypeID::kRandomGenerator:
        case EPhCtxTypeID::kRandomGeneratorglobal: {
            nlen = kInt_16U;
        } break;
        case EPhCtxTypeID::kHashSha1:
        case EPhCtxTypeID::kHashSha2_224:
        case EPhCtxTypeID::kHashSha2_256: {
            nlen = kInt_64U;  // 512 bits is the input block size for SHA-1 algorithm.
            break;
        } break;
        case EPhCtxTypeID::kHashSha2_384:
        case EPhCtxTypeID::kHashSha2_512: {
            nlen = kInt_128U;
        } break;
        default: {
            nlen = 0U;
        } break;
    }

    if (nlen == 0U) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    PSecretSeed::Uptrc pcSecretSeed{nullptr};

    PSecretSeed::Uptr pSecretSeed{
        std::make_unique< PSecretSeed >(static_cast< void const * >(memKeyData.data()), nlen)};
    if (memKeyData.empty()) {
        std::ignore = pSecretSeed->RandomInitKey(static_cast< int32_t >(nlen) * kInt_8);
    }
    pSecretSeed->SetAllowedUsage(allowedUsage);
    pSecretSeed->SetExportable(isExportable);
    pSecretSeed->SetSession(isSession);
    pcSecretSeed = std::move(pSecretSeed);

    return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(pcSecretSeed));
}
/// @brief Generate a random Secret Seed object for the requested algorithm.
/// @returns  ara::core::Result<SecretSeed::Uptrc> P
/// @param algId Crypto primitive ID
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable  the exportability attribute of the target seed (if @c true)
/// @return smart unique pointer to the created private key object
ara::core::Result< SecretSeed::Uptrc > PCryptoProvider::GenerateSeed(AlgId algId,
                                                                     SecretSeed::Usage allowedUsage,
                                                                     bool isSession,
                                                                     bool isExportable) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SecretSeed::Uptrc >);
    if ((allowedUsage & kAllowExport) == kAllowExport) {
        isExportable = true;
    }
    return GenerateSeedEx(algId, ReadOnlyMemRegion(), allowedUsage, isSession, isExportable);
}
/// @brief Allocate a new symmetric key object and fill it with a newly generated random value.
///         Any serializable (i.e., saveable/non-session or exportable) key must generate its own COUID! By default, the crypto provider should use the best internal instance of all supported RNGs (ideally TRNG).
/// @returns  ara::core::Result<SymmetricKey::Uptrc> P
/// @param algId Crypto primitive ID
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return smart unique pointer to the created private key object
ara::core::Result< SymmetricKey::Uptrc > PCryptoProvider::GenerateSymmetricKey(AlgId algId,
                                                                               AllowedUsageFlags allowedUsage,
                                                                               bool isSession,
                                                                               bool isExportable) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SymmetricKey::Uptrc >);
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    if ((allowedUsage & kAllowExport) == kAllowExport) {
        isExportable = true;
    }
    return GenerateSymmetricKeyEx(algId, ReadOnlyMemRegion(), allowedUsage, isSession, isExportable);
}
//***************/
/// @brief Return the minimum capacity of the key slot required to hold the object payload.
///         The return value does not consider object meta-information attributes, but their size is fixed and common for all crypto objects regardless of their actual type.
///         During TrustedContainer allocation, the crypto provider (and key storage provider) automatically reserves space for the object's meta-information based on its implementation details.
/// @param cryptoObjectType Key material type
/// @param algId Crypto primitive ID
/// @returns  minimal size required for storing of the object in a @c TrustedContainer (persistent or volatile)
ara::core::Result< std::size_t > PCryptoProvider::GetPayloadStorageSize(CryptoObjectType cryptoObjectType,
                                                                        AlgId algId) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< std::size_t >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error SecurityErrorDomain::  if the arguments are incompatible
    if ((cryptoObjectType == CryptoObjectType::kUndefined) || (cryptoObjectType == CryptoObjectType::kSignature)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    std::size_t nSize{0U};
    switch (cryptoObjectType) {
        case CryptoObjectType::kPrivateKey:
        case CryptoObjectType::kPublicKey: {
            switch (static_cast< EPhCtxTypeID >(algId)) {
                case EPhCtxTypeID::kAsymmetricRsaKey:
                case EPhCtxTypeID::kAsymmetricRsa:  // Default to 1024-bit length
                case EPhCtxTypeID::kAsymmetricRsaKey1024: {
                    nSize = kInt_128U;
                } break;
                case EPhCtxTypeID::kAsymmetricRsaKey512: {
                    nSize = kInt_64U;
                } break;
                case EPhCtxTypeID::kAsymmetricRsaKey2048: {
                    nSize = kInt_256U;
                } break;
                case EPhCtxTypeID::kAsymmetricRsaKey4096: {
                    nSize = kInt_512U;
                } break;
                case EPhCtxTypeID::kAsymmetricRsaKey8192: {
                    nSize = kInt_1024U;
                } break;
                case EPhCtxTypeID::kAsymmetricEccKey:
                case EPhCtxTypeID::kAsymmetricEccKey128: {
                    nSize = kInt_40U;
                } break;
                case EPhCtxTypeID::kAsymmetricEccKey192: {
                    nSize = kInt_56U;
                } break;
                case EPhCtxTypeID::kAsymmetricEccKey256: {
                    nSize = kInt_72U;
                } break;
                default: {
                    nSize = 0U;
                } break;
            }
        } break;
        case CryptoObjectType::kSecretSeed: {
            switch (static_cast< EPhCtxTypeID >(algId)) {
                case EPhCtxTypeID::kAe_Des:
                case EPhCtxTypeID::kKdf_Des:
                case EPhCtxTypeID::kKdf_3Des:
                case EPhCtxTypeID::kSymmetric3DesCfb1:
                case EPhCtxTypeID::kSymmetric3DesCfb64:
                case EPhCtxTypeID::kSymmetric3DesOfb64:
                case EPhCtxTypeID::kSymmetricDesCfb:
                case EPhCtxTypeID::kSymmetricDesCfb64:
                case EPhCtxTypeID::kSymmetricDesOfb:
                case EPhCtxTypeID::kSymmetricDesOfb64:
                case EPhCtxTypeID::kMac_Des_Cbc:
                case EPhCtxTypeID::kMac_Des_Cbc_ede2:
                case EPhCtxTypeID::kMac_Des_Cbc_ede3: {
                    nSize = kInt_8U;
                } break;
                case EPhCtxTypeID::kAe_Aes:
                case EPhCtxTypeID::kKdf_Aes:
                case EPhCtxTypeID::kKdf_Aes128:
                case EPhCtxTypeID::kKdf_Aes192:
                case EPhCtxTypeID::kKdf_Aes256:
                case EPhCtxTypeID::kSymmetricAesCfb1:
                case EPhCtxTypeID::kSymmetricAesCfb8:
                case EPhCtxTypeID::kSymmetricAesCfb128:
                case EPhCtxTypeID::kSymmetricAesOfb128:
                case EPhCtxTypeID::kMac_Aes_Cbc_128:
                case EPhCtxTypeID::kMac_Aes_Cbc_192:
                case EPhCtxTypeID::kMac_Aes_Cbc_256:
                case EPhCtxTypeID::kRandomGenerator:
                case EPhCtxTypeID::kRandomGeneratorglobal: {
                    nSize = kInt_16U;
                } break;
                case EPhCtxTypeID::kHashSha1:
                case EPhCtxTypeID::kHashSha2_224:
                case EPhCtxTypeID::kHashSha2_256: {
                    nSize = kInt_64U;
                } break;
                case EPhCtxTypeID::kHashSha2_384:
                case EPhCtxTypeID::kHashSha2_512: {
                    nSize = kInt_128U;
                } break;
                default: {
                    nSize = 0U;
                } break;
            }
        } break;
        case CryptoObjectType::kSymmetricKey: {
            switch (static_cast< EPhCtxTypeID >(algId)) {
                case EPhCtxTypeID::kSymmetricDesKey:
                case EPhCtxTypeID::kSymmetricDesEcb:
                case EPhCtxTypeID::kSymmetricDesCbc: {
                    nSize = kInt_8U;
                } break;
                case EPhCtxTypeID::kSymmetric2DesKey: {
                    nSize = kInt_16U;
                } break;
                case EPhCtxTypeID::kSymmetric3DesKey:
                case EPhCtxTypeID::kSymmetric3DesEcb:
                case EPhCtxTypeID::kSymmetric3DesCbc:
                case EPhCtxTypeID::kSymmetric3DesCfb1:
                case EPhCtxTypeID::kSymmetric3DesCfb64:
                case EPhCtxTypeID::kSymmetric3DesOfb64: {
                    nSize = kInt_24U;
                } break;
                case EPhCtxTypeID::kSymmetricAesKey:
                case EPhCtxTypeID::kSymmetricAesKey128:
                case EPhCtxTypeID::kSymmetricAesEcb:
                case EPhCtxTypeID::kSymmetricAesEcb128:
                case EPhCtxTypeID::kSymmetricAesCbc:
                case EPhCtxTypeID::kSymmetricAesCbc128:
                case EPhCtxTypeID::kSymmetricAesCfb128:
                case EPhCtxTypeID::kSymmetricAesCfb:
                case EPhCtxTypeID::kSymmetricAesCfb1:
                case EPhCtxTypeID::kSymmetricAesCfb8:
                case EPhCtxTypeID::kSymmetricAesOfb:
                case EPhCtxTypeID::kSymmetricAesOfb128:
                case EPhCtxTypeID::kSymmetricAesCtr:
                case EPhCtxTypeID::kSymmetricAesGcm: {
                    nSize = kInt_16U;
                } break;
                case EPhCtxTypeID::kSymmetricAesEcb192:
                case EPhCtxTypeID::kSymmetricAesKey192:
                case EPhCtxTypeID::kSymmetricAesCbc192: {
                    nSize = kInt_24U;
                } break;
                case EPhCtxTypeID::kSymmetricAesKey256:
                case EPhCtxTypeID::kSymmetricAesEcb256:
                case EPhCtxTypeID::kSymmetricAesCbc256: {
                    nSize = kInt_32U;
                } break;
                default: {
                } break;
            }
        } break;
        default: {
        } break;
    }
    if (nSize == 0U) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    return ara::core::Result< std::size_t >::FromValue(nSize);
}
/// @brief Return the buffer size required to serialize an object in a specific format.
/// @param cryptoObjectType Crypto material type
/// @param algId Crypto algorithm primitive ID
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @returns size required for storing of the object serialized in the specified format
ara::core::Result< std::size_t > PCryptoProvider::GetSerializedSize(CryptoObjectType cryptoObjectType,
                                                                    AlgId algId,
                                                                    Serializable::FormatId formatId) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< std::size_t >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    if (!convertAlgID_.IsValidAlgID(static_cast< uint64_t >(algId))) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error SecurityErrorDomain::kIncompatibleArguments  if any pair of the arguments are incompatible
    if ((cryptoObjectType == CryptoObjectType::kUndefined) || (cryptoObjectType == CryptoObjectType::kSignature)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    /// @error SecurityErrorDomain::kIncompatibleArguments  if any pair of the arguments are incompatible
    if (((cryptoObjectType == CryptoObjectType::kSymmetricKey) || (cryptoObjectType == CryptoObjectType::kSecretSeed))
        && (formatId != Serializable::kFormatRawValueOnly)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    if (((cryptoObjectType == CryptoObjectType::kPrivateKey) || (cryptoObjectType == CryptoObjectType::kPublicKey))
        && (formatId == Serializable::kFormatDefault)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    if (formatId == Serializable::kFormatRawValueOnly) {
        return GetPayloadStorageSize(cryptoObjectType, algId);
    }

    std::size_t nSize{0U};
    switch (cryptoObjectType) {
        case CryptoObjectType::kPrivateKey: {
            nSize = L_GetSizeFromPrivateKey(algId, formatId);
        } break;
        case CryptoObjectType::kPublicKey: {
            nSize = L_GetSizeFromPublicKey(algId, formatId);
        } break;
        default: {
        } break;
    }
    if (nSize == 0U) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    return ara::core::Result< std::size_t >::FromValue(nSize);
}
//***************/
/// @brief Load any crypto object from the provided IOInterface.
/// @param container   the IOInterface that contains the crypto object for loading
/// @returns unique smart pointer to the created object
ara::core::Result< CryptoObject::Uptrc > PCryptoProvider::LoadObject(IOInterface const &container) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< CryptoObject::Uptrc >);
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }

    CryptoObjectType const &cryptoObjectType{container.GetCryptoObjectType()};
    if ((cryptoObjectType != CryptoObjectType::kPrivateKey) && (cryptoObjectType != CryptoObjectType::kPublicKey)
        && (cryptoObjectType != CryptoObjectType::kSecretSeed)
        && (cryptoObjectType != CryptoObjectType::kSymmetricKey)) {
        return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    CryptoObject::Uptrc pCryptoObject{nullptr};
    switch (cryptoObjectType) {
        case CryptoObjectType::kPrivateKey: {
            ara::core::Result< PrivateKey::Uptrc > result{LoadPrivateKey(container)};
            if (!result.HasValue()) {
                /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
                return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
            }
            pCryptoObject = std::move(std::move(result).Value());
        } break;
        case CryptoObjectType::kPublicKey: {
            ara::core::Result< PublicKey::Uptrc > result{LoadPublicKey(container)};
            if (!result.HasValue()) {
                /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
                return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
            }
            pCryptoObject = std::move(std::move(result).Value());
        } break;
        case CryptoObjectType::kSecretSeed: {
            ara::core::Result< SecretSeed::Uptrc > result{LoadSecretSeed(container)};
            if (!result.HasValue()) {
                /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
                return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
            }
            pCryptoObject = std::move(std::move(result).Value());
        } break;
        default: {
            ara::core::Result< SymmetricKey::Uptrc > result{LoadSymmetricKey(container)};
            if (!result.HasValue()) {
                /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
                return ara::core::Result< CryptoObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
            }
            pCryptoObject = std::move(std::move(result).Value());
        } break;
    }
    return ara::core::Result< CryptoObject::Uptrc >::FromValue(std::move(pCryptoObject));
}

/// @brief Load a private key from the provided IOInterface.
/// @param container IO interface
/// @returns  unique smart pointer to the PrivateKey
ara::core::Result< PrivateKey::Uptrc > PCryptoProvider::LoadPrivateKey(IOInterface const &container) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< PrivateKey::Uptrc >);
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }

    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    if (pIoInterfaceIpc != nullptr) {
        if (pIoInterfaceIpc->GetCryptoObjectType() != CryptoObjectType::kPrivateKey) {
            return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        std::size_t const nSize{pIoInterfaceIpc->GetPayloadSize()};
        if (_IsAsymmetricKeyEqualLen(CryptoObjectType::kPrivateKey, pIoInterfaceIpc->GetPrimitiveId(), nSize)
            == false) {
            return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }
        PAlgId_Asymmetric_EccKey const cryptoKey;
        if (cryptoKey.IsMinePrimitiveId(pIoInterfaceIpc->GetPrimitiveId())) {
            PKeyPrivate_Ipc_Ecc::Uptr pPrivateEcc{T_MakeObjectIpc< PKeyPrivate_Ipc_Ecc >(
                pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(),
                pIoInterfaceIpc->GetAllowedUsage(), pIoInterfaceIpc->IsObjectSession(),
                pIoInterfaceIpc->IsObjectExportable())};
            return ara::core::Result< PrivateKey::Uptrc >::FromValue(std::move(std::move(pPrivateEcc)));
        }
        PKeyPrivate_Ipc_Rsa::Uptr pPrivateRsa{T_MakeObjectIpc< PKeyPrivate_Ipc_Rsa >(
            pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(),
            pIoInterfaceIpc->GetAllowedUsage(), pIoInterfaceIpc->IsObjectSession(),
            pIoInterfaceIpc->IsObjectExportable())};
        return ara::core::Result< PrivateKey::Uptrc >::FromValue(std::move(std::move(pPrivateRsa)));
    }
    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    if (pIoInterfaceMem != nullptr) {
        if (pIoInterfaceMem->GetCryptoObjectType() != CryptoObjectType::kPrivateKey) {
            return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        ara::core::Vector< uint8_t > vecData;
        bool const ret{pIoInterfaceMem->ReadKeyData(vecData)};
        if (!ret) {
            return ara::core::Result< PrivateKey::Uptrc >::FromValue(nullptr);
        }
        if (_IsAsymmetricKeyEqualLen(CryptoObjectType::kPrivateKey, pIoInterfaceMem->GetPrimitiveId(), vecData.size())
            == false) {
            return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }

        PrivateKey::Uptrc pReturn{nullptr};
        PAlgId_Asymmetric_RsaKey const cryptoKey;
        std::size_t const nLen{vecData.size()};
        ara::core::String const stPrivatekey(T_StringView(vecData.data(), nLen));

        if (false == cryptoKey.IsMinePrimitiveId(pIoInterfaceMem->GetPrimitiveId())) {
            pReturn = T_MakePrivateKey< PKeyPrivate_Ecc >({stPrivatekey.data(), stPrivatekey.size()},
                                                          container.GetAllowedUsage(), container.IsObjectSession(),
                                                          container.IsObjectExportable());
        } else {
            pReturn = T_MakePrivateKey< PKeyPrivate_Rsa >({stPrivatekey.data(), stPrivatekey.size()},
                                                          container.GetAllowedUsage(), container.IsObjectSession(),
                                                          container.IsObjectExportable());
        }
        return ara::core::Result< PrivateKey::Uptrc >::FromValue(std::move(pReturn));
    }
    return ara::core::Result< PrivateKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
}

/// @brief Load a public key from the provided IOInterface.
/// @param container IO interface
/// @returns  ara::core::Result<PublicKey::Uptrc> P
ara::core::Result< PublicKey::Uptrc > PCryptoProvider::LoadPublicKey(IOInterface const &container) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< PublicKey::Uptrc >);
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }

    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    bool isExportable{false};
    if (pIoInterfaceIpc != nullptr) {
        if (pIoInterfaceIpc->GetCryptoObjectType() != CryptoObjectType::kPublicKey) {
            return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        std::size_t const nSize{pIoInterfaceIpc->GetPayloadSize()};
        if (_IsAsymmetricKeyEqualLen(CryptoObjectType::kPublicKey, pIoInterfaceIpc->GetPrimitiveId(), nSize) == false) {
            return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }

        PAlgId_Asymmetric_EccKey const cryptoKey;
        if (pIoInterfaceIpc->IsObjectExportable() == true) {
            if ((pIoInterfaceIpc->GetAllowedUsage() & kAllowExport) == kAllowExport) {
                isExportable = true;
            }
        }
        if (cryptoKey.IsMinePrimitiveId(pIoInterfaceIpc->GetPrimitiveId())) {
            PKeyPublic_Ipc_Ecc::Uptr pPublicEcc{T_MakeObjectIpc< PKeyPublic_Ipc_Ecc >(
                pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(),
                pIoInterfaceIpc->GetAllowedUsage(), pIoInterfaceIpc->IsObjectSession(), isExportable)};
            return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(std::move(pPublicEcc)));
        }
        PKeyPublic_Ipc_Rsa::Uptr pPublicRsa{T_MakeObjectIpc< PKeyPublic_Ipc_Rsa >(
            pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(),
            pIoInterfaceIpc->GetAllowedUsage(), pIoInterfaceIpc->IsObjectSession(), isExportable)};
        return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(std::move(pPublicRsa)));
    }

    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    if (pIoInterfaceMem != nullptr) {
        if (pIoInterfaceMem->GetCryptoObjectType() != CryptoObjectType::kPublicKey) {
            return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        ara::core::Vector< uint8_t > vecData;
        bool const ret{pIoInterfaceMem->ReadKeyData(vecData)};
        if (!ret) {
            return ara::core::Result< PublicKey::Uptrc >::FromValue();
        }
        if (_IsAsymmetricKeyEqualLen(CryptoObjectType::kPublicKey, pIoInterfaceMem->GetPrimitiveId(), vecData.size())
            == false) {
            return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }
        std::size_t const nLen{vecData.size()};
        ara::core::StringView stPublicKey{T_StringView(vecData.data(), nLen)};
        if (pIoInterfaceMem->IsObjectExportable() == true) {
            if ((pIoInterfaceMem->GetAllowedUsage() & kAllowExport) == kAllowExport) {
                isExportable = true;
            }
        }
        PAlgId_Asymmetric_RsaKey const cryptoKey;
        if (false == cryptoKey.IsMinePrimitiveId(pIoInterfaceMem->GetPrimitiveId())) {
            PKeyPublic_Rsa::Uptr pPublicRsa{std::make_unique< PKeyPublic_Rsa >(stPublicKey)};
            pPublicRsa->SetAllowedUsage(pIoInterfaceMem->GetAllowedUsage());
            pPublicRsa->SetExportable(isExportable);
            pPublicRsa->SetSession(pIoInterfaceMem->IsObjectSession());
            return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(std::move(pPublicRsa)));
        }
        PKeyPublic_Ecc::Uptr pPublicEcc{std::make_unique< PKeyPublic_Ecc >(stPublicKey)};
        if (stPublicKey.empty()) {
        }
        pPublicEcc->SetAllowedUsage(pIoInterfaceMem->GetAllowedUsage());
        pPublicEcc->SetExportable(isExportable);
        pPublicEcc->SetSession(pIoInterfaceMem->IsObjectSession());
        return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(std::move(pPublicEcc)));
    }

    return ara::core::Result< PublicKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
}
/// @brief Load a secret seed from the provided IOInterface.
/// @param container IO interface
/// @returns  unique smart pointer to the SecretSeed
ara::core::Result< SecretSeed::Uptrc > PCryptoProvider::LoadSecretSeed(IOInterface const &container) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SecretSeed::Uptrc >);
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }

    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    if (pIoInterfaceIpc != nullptr) {
        if (pIoInterfaceIpc->GetCryptoObjectType() != CryptoObjectType::kSecretSeed) {
            return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        CryptoProvider::AlgId const algId{container.GetPrimitiveId()};
        std::size_t const nSize{pIoInterfaceIpc->GetPayloadSize()};
        ara::core::Result< std::size_t > const resSize{GetPayloadStorageSize(CryptoObjectType::kSecretSeed, algId)};
        std::size_t nKeyLenMin{kInt_8U};
        if (resSize.HasValue()) {
            nKeyLenMin = resSize.Value();
        }
        if (nSize < nKeyLenMin) {
            return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }

        PSecretSeed_Ipc::Uptr pPublicEcc{T_MakeObjectIpc< PSecretSeed_Ipc >(
            pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(),
            pIoInterfaceIpc->GetAllowedUsage(), pIoInterfaceIpc->IsObjectSession(),
            pIoInterfaceIpc->IsObjectExportable())};
        return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(std::move(pPublicEcc)));
    }

    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    if (pIoInterfaceMem != nullptr) {
        if (pIoInterfaceMem->IsEmpty()) {
            return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
        }
        if (pIoInterfaceMem->GetCryptoObjectType() != CryptoObjectType::kSecretSeed) {
            return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }

        ara::core::Vector< uint8_t > vecData;
        bool const ret{pIoInterfaceMem->ReadKeyData(vecData)};
        if (!ret) {
            return ara::core::Result< SecretSeed::Uptrc >::FromValue();
        }

        CryptoProvider::AlgId const algId{container.GetPrimitiveId()};
        ara::core::Result< std::size_t > const resSize{GetPayloadStorageSize(CryptoObjectType::kSymmetricKey, algId)};
        std::size_t nKeyLenMin{kInt_8U};
        if (resSize.HasValue()) {
            nKeyLenMin = resSize.Value();
        }
        if (vecData.size() < nKeyLenMin) {
            return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }
        uint8_t *const pData{vecData.data()};
        PSecretSeed::Uptr pSecretSeed{std::make_unique< PSecretSeed >(pData, vecData.size())};
        pSecretSeed->SetAllowedUsage(container.GetAllowedUsage());

        return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(std::move(pSecretSeed)));
    }
    return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
}
/// @brief Load a symmetric key from the provided IOInterface.
/// @param container IO interface
/// @returns  unique smart pointer to the SymmetricKey
ara::core::Result< SymmetricKey::Uptrc > PCryptoProvider::LoadSymmetricKey(IOInterface const &container) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SymmetricKey::Uptrc >);
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    if (container.GetPayloadSize() == 0U) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (!container.IsValid()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    // IOInterface needs to distinguish between local and IPC branches for processing
    keys::isoft_def::PIoInterface_Ipc const *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc const * >(&container)};
    // Existence of pIoInterface_Kv indicates IPC branch
    if (pIoInterfaceIpc != nullptr) {
        // Create corresponding IPC version key using SlotID and InterfaceID here: Logic below needs further verification on correctness (hanjingjing 2022-09-07)
        CryptoObjectType const findObjectType{pIoInterfaceIpc->GetCryptoObjectType()};
        if (findObjectType != CryptoObjectType::kSymmetricKey) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
        CryptoProvider::AlgId const algId{container.GetPrimitiveId()};
        std::size_t const nSize{pIoInterfaceIpc->GetPayloadSize()};
        ara::core::Result< std::size_t > const resSize{GetPayloadStorageSize(CryptoObjectType::kSymmetricKey, algId)};
        std::size_t nKeyLenMin{kInt_8U};
        if (resSize.HasValue()) {
            nKeyLenMin = resSize.Value();
        }
        if (nSize < nKeyLenMin) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }
        PKeySymmetric_Ipc::Uptr pSymmetric{std::make_unique< PKeySymmetric_Ipc >(
            pIoInterfaceIpc->GetSlotID(), pIoInterfaceIpc->GetIoInterfaceID(), pIoInterfaceIpc->GetSlotName(), algId)};
        pSymmetric->SetAllowedUsage(pIoInterfaceIpc->GetAllowedUsage());
        pSymmetric->SetExportable(pIoInterfaceIpc->IsObjectExportable());
        pSymmetric->SetSession(pIoInterfaceIpc->IsObjectSession());
        return ara::core::Result< SymmetricKey::Uptrc >::FromValue(std::move(pSymmetric));
    }
    // Branches pIoInterfaceIpc and pIoInterfaceMem are mutually exclusive
    keys::isoft_def::PIoInterface_Mem const *const pIoInterfaceMem{
        dynamic_cast< keys::isoft_def::PIoInterface_Mem const * >(&container)};
    // Local IO processing branch
    if (pIoInterfaceMem != nullptr) {
        if (pIoInterfaceMem->GetCryptoObjectType() != CryptoObjectType::kSymmetricKey) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        ara::core::Vector< uint8_t > vecData;
        bool const ret{pIoInterfaceMem->ReadKeyData(vecData)};
        if (!ret) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromValue(nullptr);
        }
        if (vecData.empty()) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromValue(nullptr);
        }
        CryptoProvider::AlgId const algId{container.GetPrimitiveId()};
        ara::core::Result< std::size_t > const resSize{GetPayloadStorageSize(CryptoObjectType::kSymmetricKey, algId)};
        std::size_t nKeyLenMin{kInt_8U};
        if (resSize.HasValue()) {
            nKeyLenMin = resSize.Value();
        }
        if (vecData.size() < nKeyLenMin) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kResourceFault);
        }
        AllowedUsageFlags const allowedUsage{container.GetAllowedUsage()};
        uint8_t *const pData{vecData.data()};
        ReadOnlyMemRegion const memKeyData{pData, vecData.size()};
        return GenerateSymmetricKeyEx(algId, memKeyData, allowedUsage);
    }
    return ara::core::Result< SymmetricKey::Uptrc >::FromValue(nullptr);
}
//********************************/
namespace {
/// @brief Template function for generating symmetric keys
/// @tparam T_KeyType
/// @param nKeyBitLength Key length: in bits
/// @param memKeyData Key data in memory
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return Symmetric key instance
/// @throw ???
template < typename T_KeyType >
typename T_KeyType::Uptr T_MakeSymmetricKey(int32_t const nKeyBitLength,
                                            ReadOnlyMemRegion const &memKeyData,
                                            AllowedUsageFlags const allowedUsage,
                                            bool const isSession,
                                            bool const isExportable)
{
    typename T_KeyType::Uptr pSymmetricKey{
        std::move(std::make_unique< T_KeyType >(memKeyData.data(), memKeyData.size()))};
    if (memKeyData.empty()) {
        std::ignore = pSymmetricKey->RandomInitKey(nKeyBitLength);
    }
    pSymmetricKey->SetAllowedUsage(allowedUsage);
    pSymmetricKey->SetSession(isSession);
    pSymmetricKey->SetExportable(isExportable);
    return pSymmetricKey;
}
}  // namespace
/// @brief Allocate a new symmetric key object
/// @param algId Crypto primitive ID
/// @param memKeyData Key data in memory
/// @param allowedUsage Usage scope
/// @param isSession Whether it is a temporary session
/// @param isExportable Whether export is allowed
/// @return SymmetricKey instance
ara::core::Result< SymmetricKey::Uptrc > PCryptoProvider::GenerateSymmetricKeyEx(AlgId const algId,
                                                                                 ReadOnlyMemRegion const &memKeyData,
                                                                                 AllowedUsageFlags const allowedUsage,
                                                                                 bool const isSession,
                                                                                 bool const isExportable) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< SymmetricKey::Uptrc >);
    // Unique Key ID is now managed by convertAlgID_ (2023.9.5)
    // /// @error SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    SymmetricKey::Uptrc pSymmetricKey{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kSymmetricDesKey:  // Des.Key
        case EPhCtxTypeID::kSymmetricDesEcb:  // Des.Ecb
        case EPhCtxTypeID::kSymmetricDesCbc:  // Des.Cbc
        case EPhCtxTypeID::kSymmetricDesCfb:
        case EPhCtxTypeID::kSymmetricDesCfb64:
        case EPhCtxTypeID::kSymmetricDesOfb:
        case EPhCtxTypeID::kSymmetricDesOfb64: {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Des >(
                static_cast< int32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetric3DesKey:  // 3Des.Key
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_3Des >(
                static_cast< int32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetric2DesKey:  // 2Des.Key
        {
            pSymmetricKey = T_MakeSymmetricKey< PSymmetricKey_2Des >(
                static_cast< int32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength) * kInt_2, memKeyData,
                allowedUsage, isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetric3DesEcb:    // 3Des.Ecb
        case EPhCtxTypeID::kSymmetric3DesCbc:    // 3Des.Cbc
        case EPhCtxTypeID::kSymmetric3DesCfb1:   // 3Des.Cbc
        case EPhCtxTypeID::kSymmetric3DesCfb64:  // 3Des.Cbc
        case EPhCtxTypeID::kSymmetric3DesOfb64:  // 3Des.Cbc
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_3Des >(
                static_cast< int32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesKey:  // Aes.Key  // NOLINT
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Def), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesEcb:  // AES.Ecb
        case EPhCtxTypeID::kSymmetricAesCbc:  // AES.Cbc
        case EPhCtxTypeID::kSymmetricAesCfb:  // AES.Cfb
        case EPhCtxTypeID::kSymmetricAesCtr:  // AES.CTR
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Def), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesKey128:
        case EPhCtxTypeID::kSymmetricAesEcb128:  // AES.Ecb-128
        case EPhCtxTypeID::kSymmetricAesCbc128:  // AES.Cbc-128
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesKey192:
        case EPhCtxTypeID::kSymmetricAesEcb192:  // AES.Ecb-192
        case EPhCtxTypeID::kSymmetricAesCbc192:  // AES.Cbc-192
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesKey256:
        case EPhCtxTypeID::kSymmetricAesEcb256:  // AES.Ecb-256
        case EPhCtxTypeID::kSymmetricAesCbc256:  // AES.Cbc-256
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        case EPhCtxTypeID::kSymmetricAesCfb1:    // AES.Cfb-1
        case EPhCtxTypeID::kSymmetricAesCfb8:    // AES.Cfb-8
        case EPhCtxTypeID::kSymmetricAesCfb128:  // AES.Cfb-128
        case EPhCtxTypeID::kSymmetricAesOfb:     // AES.Ofb
        case EPhCtxTypeID::kSymmetricAesOfb128:  // AES.Ofb-128
        {
            pSymmetricKey = T_MakeSymmetricKey< PKeySymmetric_Aes >(
                static_cast< int32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128), memKeyData, allowedUsage,
                isSession, isExportable);
        } break;
        default: {
            pSymmetricKey = nullptr;
        } break;
    }
    if (pSymmetricKey == nullptr) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< SymmetricKey::Uptrc >::FromValue(std::move(pSymmetricKey));
}

/// @brief Import SecuredObject into key slot
/// @param pKeySlot Pointer to key slot object
/// @param ioInterfaceBase IO interface: base class object
/// @param nSlotName Key slot name
/// @param securedObject Secure object
/// @return true if import slot sucess false otherwise
bool PCryptoProvider::ImportSlot(keys::isoft_def::PIpcKeySlot *const pKeySlot,
                                 IOInterface &ioInterfaceBase,
                                 ara::core::String const &nSlotName,
                                 ReadOnlyMemRegion const &securedObject) noexcept
{
    if (nullptr == pKeySlot) {
        return false;
    }
    keys::isoft_def::PIoInterface *const pIoInterface{
        dynamic_cast< keys::isoft_def::PIoInterface * >(&ioInterfaceBase)};
    if (nullptr == pIoInterface) {
        return false;
    }
    bool const isInit{pIoInterface->InitIoInterface(nSlotName.c_str())};
    if (!isInit) {
        return false;
    }
    ara::core::Result< ara::crypto::keys::KeySlotContentProps > const resKeyCont{pKeySlot->GetContentProps()};
    if (!resKeyCont.HasValue()) {
        return false;
    }
    keys::KeySlotContentProps const &keyCont{resKeyCont.Value()};
    std::ignore = pIoInterface->SaveKeyContent(keyCont);
    ara::core::Result< keys::KeySlotPrototypeProps > const resSlotProps{pKeySlot->GetPrototypedProps()};
    if (!resSlotProps.HasValue()) {
        return false;
    }
    keys::KeySlotPrototypeProps const &slotProps{resSlotProps.Value()};
    std::ignore = pIoInterface->SaveSlotProps(slotProps);
    std::ignore = pIoInterface->SaveKeyData(securedObject, kAlgIdAny, ReadOnlyMemRegion());
    std::ignore = pIoInterface->CommitSaveAction();
    ara::core::Result< IOInterface::Uptr > const resultIoInterface{pKeySlot->Open(false, true)};
    if (!resultIoInterface.HasValue()) {
        return false;
    }
    ara::core::Result< void > const resSaveCopy{pKeySlot->SaveCopy(ioInterfaceBase)};
    if (!resSaveCopy.HasValue()) {
        return false;
    }
    keys::isoft_def::PIpcKeyProvider::Uptr const pIpcKeyProvider{
        std::make_unique< keys::isoft_def::PIpcKeyProvider >()};
    if (pIpcKeyProvider.get() == nullptr) {
        return false;
    }
    ara::core::Vector< ara::crypto::keys::KeySlot * > const transVec{pKeySlot};
    ara::core::Result< ara::crypto::keys::TransactionId > const transId{pIpcKeyProvider->BeginTransaction(transVec)};
    if (!transId.HasValue()) {
        return false;
    }
    ara::core::Result< void > const resVoid{pIpcKeyProvider->CommitTransaction(transId.Value())};
    if (!resVoid.HasValue()) {
        return false;
    }
    return true;
}
/// @brief Get public key information from certificate by serial number (Non-standard interface)
/// @param certSerialNumber Certificate serial number
/// @return Public key instance
ara::core::Result< PublicKey::Uptrc > PCryptoProvider::FindSubjectPubKey(
    ara::core::String const &certSerialNumber) noexcept
{
    ara::core::String const strPubKey{_FindSubjectPubKey(certSerialNumber)};
    if (strPubKey.empty()) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }
    PKeyPublic_Rsa::Uptr pPublicRsa{std::make_unique< PKeyPublic_Rsa >(strPubKey)};
    if (pPublicRsa->GetRsa() == nullptr) {
        PKeyPublic_Ecc::Uptr pPublicEcc{std::make_unique< PKeyPublic_Ecc >(strPubKey)};
        return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(pPublicEcc));
    }

    return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(pPublicRsa));
}
/// @brief Save public key serial number and certificate public key info (Non-standard interface)
/// @param certSerialNumber Certificate serial number
/// @param pubData Public key data
void PCryptoProvider::InsertSubjectPubKey(ara::core::String const &certSerialNumber,
                                          ara::core::String const &pubData) noexcept
{
    _InsertSubjectPubKey(certSerialNumber, pubData);
    // return ;
}
/// @brief Check if asymmetric key lengths are the same
/// @param cryptoObjectType Crypto material type
/// @param algId Crypto primitive ID
/// @param otherKeyLen Another key length
/// @return true if length id equal false otherwise
bool PCryptoProvider::_IsAsymmetricKeyEqualLen(CryptoObjectType const cryptoObjectType,
                                               AlgId const algId,
                                               std::size_t const otherKeyLen) const noexcept
{
    uint64_t nDerLen{0U};
    uint64_t nPemLen{0U};
    ara::core::Result< uint64_t > resMemData{
        GetSerializedSize(cryptoObjectType, algId, Serializable::kFormatPemEncoded)};
    if (resMemData.HasValue()) {
        nPemLen = resMemData.Value();
    }
    resMemData = GetSerializedSize(cryptoObjectType, algId, Serializable::kFormatDerEncoded);
    if (resMemData.HasValue()) {
        nDerLen = resMemData.Value();
    }
    /// RSA format private key is within the interval [len, len+5]
    if ((otherKeyLen == 0U)
        || (((otherKeyLen < nPemLen) || (otherKeyLen > nPemLen + kInt_5U))
            && ((otherKeyLen < nDerLen) || (otherKeyLen > nDerLen + kInt_5U)))) {
        return false;
    }
    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
