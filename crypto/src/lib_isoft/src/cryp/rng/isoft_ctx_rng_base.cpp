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
/// @file       isoft_ctx_rng_base.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Random Number Generation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=PCtxRng_Base
/// @unit_description=Random Number Generator Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/rng/isoft_ctx_rng_base.h"

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for Random Number Generator context.
/// @param cryptoProvider Encryption provider
/// @returns
PCtxRng_Base::PCtxRng_Base(PCryptoProvider& cryptoProvider) noexcept
    : PCtxRng_Base{cryptoProvider, nullptr, EStreamWorkState::kNotStarted}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @param pSymmetricKey Pointer to symmetric key
/// @param eWorkState Working state
PCtxRng_Base::PCtxRng_Base(PCryptoProvider& cryptoProvider,
                           SymmetricKey const* const pSymmetricKey,
                           EStreamWorkState const eWorkState) noexcept
    : RandomGeneratorCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pSymmetricKey_{pSymmetricKey}
    , eWorkState_{eWorkState}
{
}
//***************/  //CryptoContext interface
/// @brief Check if the encryption context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
/// @returns   @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
bool PCtxRng_Base::IsInitialized() const noexcept
{
    return (EStreamWorkState::kStart == eWorkState_) || (EStreamWorkState::kUpdate == eWorkState_);
}
/// @brief Get a reference to the Crypto Provider associated with this context.
/// @brief Get a reference to Crypto Provider of this context.
/// @returns a reference to Crypto Provider instance that provides this context
/// @trace_id_sws={SWS_CRYPT_20654}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02401}
/// @threadsafety={Thread-safe}
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider& PCtxRng_Base::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/  //RandomGeneratorCtx interface
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
/// @trace_id_sws={SWS_CRYPT_22913}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02001}
/// @uptrace={RS_CRYPTO_02003}
/// @threadsafety={Thread-safe}
/// @returns  bool
bool PCtxRng_Base::SetKey(SymmetricKey const& key) noexcept
{
    pSymmetricKey_ = &key;
    eWorkState_    = EStreamWorkState::kStart;
    return true;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
/// @trace_id_sws={SWS_CRYPT_22913}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02001}
/// @uptrace={RS_CRYPTO_02003}
/// @threadsafety={Thread-safe}
/// @returns  bool
ara::core::Result< void > PCtxRng_Base::SetKey(SymmetricKey const& key) noexcept
{
    pSymmetricKey_ = &key;
    eWorkState_    = EStreamWorkState::kStart;
    return ara::core::Result< void >::FromValue();
}
#endif
//***************/
/// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxRng_Base::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricKey_->GetPayloadSize() * kInt_8U;
}
/// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxRng_Base::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pSymmetricKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @returns a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxRng_Base::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pSymmetricKey_->GetAllowedUsage();
}
/// @brief Verify support for a specific key length based on the context.
/// @brief Verify supportness of specific key length by the context.
/// @param keyBitLength Key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
/// @trace_id_sws={SWS_CRYPT_29048}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
bool PCtxRng_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    // Default check is sufficient: Key.BitLength is between min and max values. If Ctx has special requirements, subclasses should implement this virtual function.
    if (keyBitLength < GetMinKeyBitLength()) {
        return false;
    }
    if (keyBitLength > GetMaxKeyBitLength()) {
        return false;
    }
    return true;
}
/// @brief Check if a key is set for this context.
/// @brief Check if a key has been set to this context.
/// @returns  true if a key has been set to this context false otherwise
/// @trace_id_sws={SWS_CRYPT_29049}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
bool PCtxRng_Base::IsKeyAvailable() const noexcept
{
    if (nullptr == pSymmetricKey_) {
        return false;
    }
    return true;
}
//***************/
/// @brief Check if RNG initialization is allowed
/// @param nAllowFlags Usage scope flag
/// @returns  true if can use init rng false otherwise
bool PCtxRng_Base::IsAllowedRngInit(AllowedUsageFlags const nAllowFlags) noexcept
{
    return (nAllowFlags & kAllowRngInit) == kAllowRngInit;
}
//***************/
/// @brief Check if key is IPC-based
/// @return true if is ipc key false otherwise
bool PCtxRng_Base::_IsIpcKey() const noexcept
{
    PKeySymmetric_Ipc const* const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const* >(pSymmetricKey_)};
    // ipc
    if (pPKeySymmetricIpc != nullptr) {
        return true;
    }
    return false;
}
//***************/
/// @brief Get slot ID
/// @return Slot ID
uint32_t PCtxRng_Base::_GetSlotID() const noexcept
{
    PKeySymmetric_Ipc const* const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const* >(pSymmetricKey_)};
    // ipc
    if (pPKeySymmetricIpc != nullptr) {
        return pPKeySymmetricIpc->GetSlotId();
    }
    return 0U;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
