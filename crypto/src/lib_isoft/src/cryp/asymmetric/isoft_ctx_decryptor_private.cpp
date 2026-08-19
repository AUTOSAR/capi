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
/// @file       isoft_ctx_decryptor_private.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-03-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/AsymmetricEncryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxDecryptorPrivate
/// @unit_description=Asymmetric decryption private key context interface base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/asymmetric/isoft_ctx_decryptor_private.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
PCtxDecryptorPrivate::PCtxDecryptorPrivate(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDecryptorPrivate{cryptoProvider, nullptr, nullptr}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @param pPrivateKey pointer to private key object
/// @param buffInput input buffer
PCtxDecryptorPrivate::PCtxDecryptorPrivate(PCryptoProvider &cryptoProvider,
                                           PrivateKey const *const pPrivateKey,
                                           internal::PAutoBuff const *const buffInput) noexcept
    : DecryptorPrivateCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPrivateKey_{pPrivateKey}
    , buffInput_{static_cast< uint32_t >(ESelectLength::kMaxInputBuffLen)}
{
    std::ignore = buffInput;
}
//***************/  //CryptoContext interface
/// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
/// @name  IsInitialized
/// @returns  true if has already init false otherwise
bool PCtxDecryptorPrivate::IsInitialized() const noexcept { return pPrivateKey_ != nullptr; }
/// @brief Get the reference to the Crypto Provider for this context.
/// @name  MyProvider
/// @returns  crypto provider instance
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxDecryptorPrivate::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/  //DecryptorPrivateCtx interface
/// @brief Process (encrypt/decrypt) an input block according to the encryptor configuration.
/// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
/// @name  ProcessBlock
/// @param in  the input data block
/// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill the
///     whole block of the plain data
/// @returns actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has incorrect
///     content
/// @trace_id_sws={SWS_CRYPT_20812}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02202}
/// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is violated
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the transformation
///     result
/// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxDecryptorPrivate::ProcessBlock(
    ReadOnlyMemRegion const &in, bool suppressPadding) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if ((false == IsKeyAvailable()) || (false == IsInitialized())) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kIncorrectInputSize    if the mentioned above rules about the input size is
    /// violated
    if (0U != (in.size() % GetBlockSize())) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    if (static_cast< std::size_t >(ESelectLength::kMaxOutputBuffLen) < GetBlockSize()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    // Encryption/decryption calculation   //2022-03-02 hanjingjing assumes PCtxDecryptorPrivate is only responsible for decryption
    return ExecuteDecryptoLogic(in.data(), static_cast< uint32_t >(in.size()), suppressPadding);
}
/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  has value if reset sucess false otherwise
ara::core::Result< void > PCtxDecryptorPrivate::Reset() noexcept
{
    pPrivateKey_ = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Set (deploy) a key for the decryptor private algorithm context.
/// @name  SetKey
/// @param key private key
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxDecryptorPrivate::SetKey(PrivateKey const &key) noexcept
{
    /// symmetric key context
    ///         If the provided key object is incompatible with this symmetric key context
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if ((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPrivateKey_ = &key;
    return ara::core::Result< void >::FromValue();
}
//********************************/ //PServiceCrypto interface
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @name  GetActualKeyBitLength
/// @returns length of the key set for the context
std::size_t PCtxDecryptorPrivate::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPrivateKey_->GetPayloadSize() * kInt_8U;  // Convert to bit length
}
/// @brief Get the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @name  GetActualKeyCOUID
/// @returns  COUID of the key set for the context
CryptoObjectUid PCtxDecryptorPrivate::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPrivateKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @name  GetAllowedUsage
/// @returns  allowed usage of the key object
AllowedUsageFlags PCtxDecryptorPrivate::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pPrivateKey_->GetAllowedUsage();
}
/// @brief Verify support for a specific key length according to the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength key length: in bits
/// @returns  true if key support false otherwise
bool PCtxDecryptorPrivate::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    // Default implementation checks whether Key.BitLength is between the minimum and maximum values: if the Ctx has special requirements, please override this virtual function in the subclass
    if (keyBitLength < GetMinKeyBitLength()) {
        return false;
    }
    if (keyBitLength > GetMaxKeyBitLength()) {
        return false;
    }
    return true;
}
/// @brief Check whether a key is set for this context.
/// @name  IsKeyAvailable
/// @returns  true if has already set key false otherwise
bool PCtxDecryptorPrivate::IsKeyAvailable() const noexcept
{
    if (nullptr == pPrivateKey_) {
        return false;
    }
    return true;
}
/// @brief Get the maximum expected size of the input data block. With the suppressPadding parameter, it will equal the block size.
/// @name  GetMaxInputSize
/// @param suppressPadding whether to suppress padding
/// @returns  maximum expected size
std::size_t PCtxDecryptorPrivate::GetMaxInputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(buffInput_.GetBuffLen());
}
/// @brief Get the maximum possible size of the output data block.
///         If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
/// @name  GetMaxOutputSize
/// @param suppressPadding whether to suppress padding
/// @returns  maximum possible size of the output block
std::size_t PCtxDecryptorPrivate::GetMaxOutputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(ESelectLength::kMaxOutputBuffLen);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
