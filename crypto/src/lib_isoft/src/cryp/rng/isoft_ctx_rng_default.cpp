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
/// @file       isoft_ctx_rng_default.cpp
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
/// @unit_name=PCtxRng_Default
/// @unit_description=Default Random Number Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/rng/isoft_ctx_rng_default.h"

#include <openssl/rand.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for Random Number Generator context.
//********************************/
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxRng_Default::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_RandomGenerator >()};
}
/// @brief Return an allocated buffer containing a generated random sequence of the requested size.
/// @brief Return an allocated buffer with a generated random sequence of the requested size.
/// @param count number of random bytes to generate
/// @returns @c a buffer filled with the generated random sequence
/// @trace_id_sws={SWS_CRYPT_22915}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG state is
/// controlled by the application),
///             and has to be seeded by the application because it either has not already been seeded or ran out of
///             entropy.
/// @error: SecurityErrorDomain::kBusyResource           if this context implements a global RNG (i.e., the RNG state is
/// controlled by the stack and not the application)
///             that is currently out-of-entropy and therefore cannot provide the requested number of random bytes.
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxRng_Default::Generate(std::uint32_t const &count) noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG
    /// state is controlled by the application),
    ///             and has to be seeded by the application because it either has not already been seeded or ran out of
    ///             entropy.
    if (false == IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    // ipc
    if (_IsIpcKey()) {
        return _GenerateIpc(FUNC_NAME_RNG(Generate), count);
    }
    /// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG
    /// state is controlled by the application),
    ///             and has to be seeded by the application because it either has not already been seeded or ran out of
    ///             entropy.
    if (0 == RAND_status()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    ara::core::Vector< ara::core::Byte > vecRandom;
    vecRandom.resize(static_cast< std::size_t >(count));
    int32_t const ret{RAND_bytes(T_TransBytes(vecRandom.data()), static_cast< int32_t >(count))};
    /// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG
    /// state is controlled by the application),
    ///             and has to be seeded by the application because it either has not already been seeded or ran out of
    ///             entropy.
    if (ret == 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecRandom);
}
/// @brief Get ExtensionService instance.
/// @brief Get ExtensionService instance.
/// @returns  ExtensionService instance
/// @trace_id_sws={SWS_CRYPT_22902}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02006}
ExtensionService::Uptr PCtxRng_Default::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxRng_Default > >(*this)};
}
#if AP_VERSION_PUHUA == 20
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
/// This method is optional for implementations. Implementations of this method may "accumulate" entropy for future use.
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
///         This method is optional for implementation. An implementation of this method may "accumulate" provided
///         entropy for future use.
/// @param entropy  a memory region with the additional entropy value
/// @returns @c has value or true if the method is supported and the entropy has been updated successfully
/// @trace_id_sws={SWS_CRYPT_22914}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @threadsafety={Thread-safe}
bool PCtxRng_Default::AddEntropy(ReadOnlyMemRegion const &entropy) noexcept
{
    ara::core::Result< void > const entropySuccess{_addEntropy(entropy)};
    if (entropySuccess.HasValue() == false) {
        return false;
    }
    return true;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
/// This method is optional for implementations. Implementations of this method may "accumulate" entropy for future use.
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
///         This method is optional for implementation. An implementation of this method may "accumulate" provided
///         entropy for future use.
/// @param entropy  a memory region with the additional entropy value
/// @returns @c has value or true if the method is supported and the entropy has been updated successfully
/// @trace_id_sws={SWS_CRYPT_22914}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxRng_Default::AddEntropy(ReadOnlyMemRegion const &entropy) noexcept
{
    return _addEntropy(entropy);
}
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.
/// @param seed Secret seed
/// @returns  @c has value or true if the method is supported and the state has been set successfully
bool PCtxRng_Default::Seed(ReadOnlyMemRegion const &seed) noexcept
{
    ara::core::Result< void > const doSeedSuccess{_seedRom(seed)};
    if (doSeedSuccess.HasValue() == false) {
        return false;
    }
    return true;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.
/// @param seed Secret seed
/// @returns  @c has value or true if the method is supported and the state has been set successfully
ara::core::Result< void > PCtxRng_Default::Seed(ReadOnlyMemRegion const &seed) noexcept { return _seedRom(seed); }
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.
/// @brief Set the internal state of the RNG using the provided seed
/// @param seed  a memory region with the seed value
/// @returns @c has value or true if the method is supported and the state has been set successfully
bool PCtxRng_Default::Seed(SecretSeed const &seed) noexcept
{
    ara::core::Result< void > const doSeedSuccess{_seedSecretSeed(seed)};
    if (doSeedSuccess.HasValue() == false) {
        return false;
    }
    return true;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.
/// @brief Set the internal state of the RNG using the provided seed
/// @param seed  a memory region with the seed value
/// @returns @c has value or true if the method is supported and the state has been set successfully
ara::core::Result< void > PCtxRng_Default::Seed(SecretSeed const &seed) noexcept { return _seedSecretSeed(seed); }
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
bool PCtxRng_Default::SetKey(SymmetricKey const &key) noexcept
{
    ara::core::Result< void > const setKeySuccess{_setSymmetricKey(key)};
    if (setKeySuccess.HasValue() == false) {
        return false;
    }
    return true;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
ara::core::Result< void > PCtxRng_Default::SetKey(SymmetricKey const &key) noexcept { return _setSymmetricKey(key); }
#endif

/// @brief Encapsulate common code from both interfaces regarding the AddEntropy function (SWS_CRYPT_22914) due to return value changes between AP2.0 and AP3.0.
/// @param entropy Seed, ReadOnlyMemRegion type
/// @return Value present: success; no value: failure
ara::core::Result< void > PCtxRng_Default::_addEntropy(ReadOnlyMemRegion const &entropy) noexcept
{
    int32_t const nRandomness{0};
    RAND_add(entropy.data(), static_cast< int32_t >(entropy.size()), static_cast< double >(nRandomness));
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulate common code from both interfaces regarding the seed function (SWS_CRYPT_22911) due to return value changes between AP2.0 and AP3.0.
/// @param seed Seed, ReadOnlyMemRegion type
/// @return Value present: success; no value: failure
ara::core::Result< void > PCtxRng_Default::_seedRom(ReadOnlyMemRegion const &seed) noexcept
{
    RAND_seed(seed.data(), static_cast< int32_t >(seed.size()));
    _SetWorkState(EStreamWorkState::kStart);
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulate common code from both interfaces regarding the seed function (SWS_CRYPT_22912) due to return value changes between AP2.0 and AP3.0.
/// @param seed Seed, SecretSeed type
/// @return Value present: success; no value: failure
ara::core::Result< void > PCtxRng_Default::_seedSecretSeed(SecretSeed const &seed) noexcept
{
    // SWS_CRYPT_00503 :
    // If Seed() and SetKey() are called by SymmetricKey or SecretSeed without the allow usage flag kAllowRngInit, they return false without modifying the global state.
    //  if the provided SecretSeed is not allowed to be used for seeding.
    if (false == IsAllowedRngInit(seed.GetAllowedUsage())) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    PSecretSeed const *const pSeed{dynamic_cast< PSecretSeed const * >(&seed)};
    uint8_t const *const pData{pSeed->GetSecretSeedData()};
    size_t const len{pSeed->GetPayloadSize()};
    if ((len == 0U) || (pData == nullptr)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    RAND_seed(pData, static_cast< int32_t >(len));
    _SetWorkState(EStreamWorkState::kStart);
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulate common code from both interfaces regarding the SetKey function (SWS_CRYPT_22913) due to return value changes between AP2.0 and AP3.0.
/// @param key Key
/// @return Value present: success; no value: failure
ara::core::Result< void > PCtxRng_Default::_setSymmetricKey(SymmetricKey const &key) noexcept
{
    // SWS_CRYPT_00503 :
    // If Seed() and SetKey() are called by SymmetricKey or SecretSeed without the allow usage flag kAllowRngInit, they return false without modifying the global state.
    if (false == IsAllowedRngInit(key.GetAllowedUsage())) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(FUNC_NAME_RNG(SetKey), key);
    }

    PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&key)};
    if (pKeySymmetricBase == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    uint8_t const *const pData{pKeySymmetricBase->GetKeySymmetric()};
    size_t const len{pKeySymmetricBase->GetPayloadSize()};
    if ((len == 0U) || (pData == nullptr)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    RAND_seed(pData, static_cast< int32_t >(len));
    std::ignore = PCtxRng_Base::SetKey(key);
    return ara::core::Result< void >::FromValue();
}

//***************/
/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxRng_Default::GetAllowedUsage() const noexcept { return kAllowRngInit; }
/// @brief Get the maximum supported key length (in bits).
/// @brief Get maximal supported key length in bits.
/// @returns maximal supported length of the key in bits
/// @trace_id_sws={SWS_CRYPT_29044}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
/// @returns maximal supported length of the key in bits
std::size_t PCtxRng_Default::GetMaxKeyBitLength() const noexcept { return kInt_8U; }
/// @brief Get the minimum supported key length (in bits).
/// @brief Get minimal supported key length in bits.
/// @returns minimal supported length of the key in bits
/// @trace_id_sws={SWS_CRYPT_29043}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
std::size_t PCtxRng_Default::GetMinKeyBitLength() const noexcept { return kInt_1024U; }
/// @brief Verify support for a specific key length according to the context.
/// @brief Verify supportness of specific key length by the context.
/// @param keyBitLength Key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
/// @trace_id_sws={SWS_CRYPT_29048}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
bool PCtxRng_Default::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    // By default, check that the Key.BitLength is between the minimum and maximum values: if the Ctx has special requirements, please implement this virtual function in the subclass
    return (keyBitLength >= GetMinKeyBitLength()) && (keyBitLength <= GetMaxKeyBitLength());
}
//***************/
#include <openssl/err.h>
/// @brief Initialize the RNG. The return value indicates whether it is allowed to initialize the seed (for some globally implemented random number contexts, the seed cannot be initialized because public resources are occupied)
/// @returns  true if has already init false otherwise
bool PCtxRng_Default::InitlizeRng() noexcept
{
    int32_t const nRandStatus{RAND_status()};
    if (0 == nRandStatus) {
        return false;
    }
    _SetWorkState(EStreamWorkState::kStart);
    return true;
}
//***************/
/// @brief Set IPC via IPC mode
/// @param stFuncName Function name used for IPC call
/// @param key Symmetric key
/// @return true if set ipc key sucess false otherwise
ara::core::Result< void > PCtxRng_Default::_SetKeyIpc(ara::core::StringView const &stFuncName,
                                                      SymmetricKey const &key) noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pIpcKey == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }

    uint32_t const nSlotId{pIpcKey->GetSlotId()};

    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg, [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Setkey ipcReq;
                ipcReq.nIpcSlotID = nSlotId;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Setkey);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_Setkey *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Setkey >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    bool const setKeySuccess{PCtxRng_Base::SetKey(key)};
    if (false == setKeySuccess) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief Generate random numbers via IPC
/// @param stFuncName Function name used for IPC call
/// @param count Number of random bytes to generate
/// @return Generated random numbers
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxRng_Default::_GenerateIpc(
    ara::core::StringView const &stFuncName, std::uint32_t const &count) const noexcept
{
    uint32_t const nSlotId{_GetSlotID()};
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg, [nSlotId, count](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Rng ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                ipcReq.SetGenerateLen(count);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Setkey);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_Rng *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Rng >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    uint8_t *const pData{pIpcAsw->GetData()};
    if (pData == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    ara::core::Vector< ara::core::Byte > vecRandom;
    for (std::size_t i{0U}; i < count; ++i) {
        vecRandom.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecRandom);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
