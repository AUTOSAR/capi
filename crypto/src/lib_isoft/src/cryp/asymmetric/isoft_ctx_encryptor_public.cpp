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
/// @file       isoft_ctx_encryptor_public.cpp
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
/// @unit_name=PCtxEncryptorPublic
/// @unit_description=Asymmetric encryption public key context interface base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/asymmetric/isoft_ctx_encryptor_public.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Asymmetric encryption public key context interface.
//********************************/
/// @brief Asymmetric encryption module. Public key encryption
/// @param cryptoProvider Encryption provider
/// @returns
/// @throws
PCtxEncryptorPublic::PCtxEncryptorPublic(PCryptoProvider& cryptoProvider) noexcept
    : PCtxEncryptorPublic{cryptoProvider, nullptr, nullptr}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @param pPublicKey Public key
/// @param buffInput Input buffer
PCtxEncryptorPublic::PCtxEncryptorPublic(PCryptoProvider& cryptoProvider,
                                         PublicKey const* const pPublicKey,
                                         internal::PAutoBuff const* const buffInput) noexcept
    : EncryptorPublicCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPublicKey_{pPublicKey}
    , buffInput_{static_cast< uint32_t >(ESelectLength::kMaxInputBuffLen)}
{
    std::ignore = buffInput;
}
//***************/  //CryptoContext interface
/// @brief Check if the encryption context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
/// @name  IsInitialized
/// @returns  true if has already init false otherwise
bool PCtxEncryptorPublic::IsInitialized() const noexcept { return pPublicKey_ != nullptr; }
/// @brief Get a reference to the Crypto Provider associated with this context.
/// @name  MyProvider
/// @returns  Reference to the encryption provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider& PCtxEncryptorPublic::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/  //EncryptorPublicCtx interface
/// @brief Process an input block according to the encryptor configuration (encryption/decryption).
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
/// @name  ProcessBlock
/// @param in  the input data block
/// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill the
/// whole block of the plain data
/// @returns actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has incorrect
///     content
/// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is violated
/// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
PCtxEncryptorPublic::ProcessBlock(ReadOnlyMemRegion const& in, bool suppressPadding) const noexcept
{
    // The meaning of suppressPadding is ambiguous: does it refer to using the padding scheme built into the RSA interface, or does it mean performing PKCS7Padding before calling the RSA interface
    // 2022-03-10 hanjingjing adopted the internal RSA padding scheme
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if ((false == IsKeyAvailable()) || (false == IsInitialized())) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    // Check if the sizes of the input and output data meet expectations
    uint32_t const nNeedOutputLen{_CheckSize(in, suppressPadding)};
    /// @error: SecurityErrorDomain::kIncorrectInputSize    if the mentioned above rules about the input size is
    /// violated
    if (0U == nNeedOutputLen) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    /// transformation result
    if (nNeedOutputLen > static_cast< uint32_t >(ESelectLength::kMaxOutputBuffLen)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    // Encryption/Decryption calculation
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    /// transformation result
    return ExecuteEncryptoLogic(in.data(), static_cast< uint32_t >(in.size()), suppressPadding);
}
/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  ara::core::Result<void>
ara::core::Result< void > PCtxEncryptorPublic::Reset() noexcept
{
    pPublicKey_ = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Set (deploy) a key for the public algorithm context of the encryptor.
/// @name  SetKey
/// @param key Public key
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxEncryptorPublic::SetKey(PublicKey const& key) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    ///         If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    ///         If the "Allowed Usage" restrictions of the provided key object prohibit the transformation type associated with this context
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if ((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPublicKey_ = &key;
    return ara::core::Result< void >::FromValue();
}
//********************************/ //PServiceCrypto interface
/// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
/// @name  GetActualKeyBitLength
/// @returns  Actual bit length of the key set in the context
std::size_t PCtxEncryptorPublic::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPublicKey_->GetPayloadSize() * kInt_8U;  // Convert bit length
}
/// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
/// @name  GetActualKeyCOUID
/// @returns  COUID of the key set in the context
CryptoObjectUid PCtxEncryptorPublic::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPublicKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @name  GetAllowedUsage
/// @returns  Allowed usage
AllowedUsageFlags PCtxEncryptorPublic::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pPublicKey_->GetAllowedUsage();
}
/// @brief Check if a key is set for this context.
/// @name  IsKeyAvailable
/// @returns  ture if already set key false otherwise
bool PCtxEncryptorPublic::IsKeyAvailable() const noexcept
{
    if (nullptr == pPublicKey_) {
        return false;
    }
    return true;
}
/// @brief Get the maximum expected size of the input data block. With the suppressPadding parameter, it will be equal to the block size.
/// @name  GetMaxInputSize
/// @param suppressPadding Whether padding is suppressed
/// @returns  Maximum expected input size
std::size_t PCtxEncryptorPublic::GetMaxInputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(buffInput_.GetBuffLen());
}
/// @brief Get the maximum possible size of the output data block.
///     If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will be equal to the block size.
/// @name  GetMaxOutputSize
/// @param suppressPadding Whether padding is suppressed
/// @returns  Maximum expected output size
std::size_t PCtxEncryptorPublic::GetMaxOutputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(ESelectLength::kMaxOutputBuffLen);
}
//********************************/
/// @brief Check input parameters    // Must be clear: here it is encryption only
/// @name  _CheckSize
/// @param in Input data buffer
/// @param suppressPadding Whether padding is suppressed
/// @returns  ture if check sucess false otherwise
uint32_t PCtxEncryptorPublic::_CheckSize(ReadOnlyMemRegion const& in, bool const suppressPadding) const noexcept
{
    ///         Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    ///         >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    ///         GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects
    ///         that: in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case
    ///         (out.size() < GetMaxOutputSize()) should be used with caution, only if you are strictly certain about
    ///         the size of the output data! In case of (suppressPadding == true) the actual size of plain text should
    ///         be equal to full size of the plain data block (defined by the algorithm)!
    if (static_cast< std::size_t >(ESelectLength::kMaxOutputBuffLen) < GetMaxOutputSize(suppressPadding)) {
        return 0U;
    }
    bool bCheckSize{false};  // Check if input and output data sizes meet expectations
    uint32_t nNeedOutputLen{0U};
    if (suppressPadding) {
        bCheckSize     = (0U == (in.size() % GetModulusSize(suppressPadding)));
        nNeedOutputLen = static_cast< uint32_t >(in.size());
    } else  // Encryption and padding required
    {
        uint32_t const nNeedInputLen{static_cast< uint32_t >(in.size())};
        if (nNeedInputLen > GetMaxInputSize(suppressPadding)) {
            bCheckSize = false;
        } else {
            if (in.empty()) {
                bCheckSize = false;
            } else {
                bCheckSize = true;
            }
        }
        nNeedOutputLen = nNeedInputLen;
    }
    if (bCheckSize) {
        return nNeedOutputLen;
    }
    return 0U;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
