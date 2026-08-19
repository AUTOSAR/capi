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
/// @file       isoft_ctx_dsv_sig_encode_private.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-24
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SigEncodePrivate
/// @unit_description=Base class for private key context for asymmetric signature computation and short message encoding (like RSA)
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_sig_encode_private.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Private key context for asymmetric signature computation and short message encoding (like RSA).
/// A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided that the public keys are appropriately generated and kept confidential.
//********************************/
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SigEncodePrivate::PCtxDsv_SigEncodePrivate(PCryptoProvider& cryptoProvider) noexcept
    : PCtxDsv_SigEncodePrivate{cryptoProvider, nullptr}
{
}
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
/// @param pPrivateKey Pointer to the private key object.
PCtxDsv_SigEncodePrivate::PCtxDsv_SigEncodePrivate(PCryptoProvider& cryptoProvider,
                                                   PrivateKey const* const pPrivateKey) noexcept
    : SigEncodePrivateCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPrivateKey_{pPrivateKey}
{
}
//*/ //CryptoContext Interface
/// @brief Checks whether the encryption context is initialized and ready to use. It checks all required values, including: key value, IV/seed, etc.
/// @returns  true if has already inited false otherwis
bool PCtxDsv_SigEncodePrivate::IsInitialized() const noexcept { return IsKeyAvailable(); }
/// @brief Gets a reference to the Crypto Provider for this context.
/// @returns Reference to the crypto provider.
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider& PCtxDsv_SigEncodePrivate::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//*****/ //SigEncodePrivateCtx Interface
/// @brief Gets the maximum expected size of the input data block.
/// If (IsEncryption() == false), the value returned by this method is independent of the suppressPadding parameter, and the value will be equal to the block size.
/// @brief Get maximum expected size of the input data block.
///       If (IsEncryption() == false) then a value returned by this method is independent from
///       the @c suppressPadding argument and it will be equal to the block size.
/// @param suppressPadding Whether to suppress padding.
/// @return maximum size of the input data block in bytes
std::size_t PCtxDsv_SigEncodePrivate::GetMaxInputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return static_cast< std::size_t >(GetBlockSize());
    }
    return static_cast< std::size_t >(ESelectLength::kMaxInputBuffLen);
}
/// @brief Gets the maximum possible size of the output data block.
/// If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter, and will be equal to the block size.
/// @brief Get maximum possible size of the output data block.
///       If (IsEncryption() == true) then a value returned by this method is independent from the
///       @c suppressPadding argument and will be equal to the block size.
/// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of the
///     plain data block is used for the payload only
/// @returns maximum size of the output data block in bytes
std::size_t PCtxDsv_SigEncodePrivate::GetMaxOutputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return static_cast< std::size_t >(GetBlockSize());
    }
    return static_cast< std::size_t >(ESelectLength::kMaxOutputBuffLen);
}
/// @brief Processes (encrypts/decrypts) an input block according to the crypto configuration.
/// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
/// @param in  the input data block
/// @returns actual size of output data (it always <= out.size()) or 0 if the input data block has incorrect content
/// @error: SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
/// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxDsv_SigEncodePrivate::SignAndEncode(
    ReadOnlyMemRegion const& in) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error:  SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
    if (GetMaxInputSize() < in.size()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    // Compute signature
    return DoEncrypto(in.data(), static_cast< uint32_t >(in.size()), false);
}
/// @brief Clears the encryption context.
/// @returns has value if reset sucess false otherwise
ara::core::Result< void > PCtxDsv_SigEncodePrivate::Reset() noexcept
{
    pPrivateKey_ = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Sets (deploys) a key into the signature encoding private algorithm context.
/// @brief SWS_CRYPT_01822
/// @param key the source key object
/// @returns has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxDsv_SigEncodePrivate::SetKey(PrivateKey const& key) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// If the "allowed usage" restrictions of the provided key object prohibit the type of transformation associated with this context
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption)
        || ((kAllowSignature & nKeyAllowedUsage) != kAllowSignature)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPrivateKey_ = &key;
    return ara::core::Result< void >::FromValue();
}
//*/ //ExtensionService Interface
/// @brief Gets the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @returns actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxDsv_SigEncodePrivate::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPrivateKey_->GetPayloadSize() * kInt_8U;  // Convert to bit length
}
/// @brief Gets the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxDsv_SigEncodePrivate::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPrivateKey_->GetObjectId().mCouid;
}
/// @brief Gets the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, 0 must be returned (all flags reset).
/// @returns a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxDsv_SigEncodePrivate::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pPrivateKey_->GetAllowedUsage();
}
/// @brief Checks whether a key is set for this context.
/// @returns  true if a key has been set to this context false otherwise
bool PCtxDsv_SigEncodePrivate::IsKeyAvailable() const noexcept
{
    if (nullptr == pPrivateKey_) {
        return false;
    }
    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
