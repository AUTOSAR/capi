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
/// @file       isoft_secret_seed_ipc.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-05-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Secret Seed
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PSecretSeed_Ipc
/// @unit_description=IPC Secret Seed
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_secret_seed_ipc.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Constructor with parameters.
/// @param nSlotId Key slot ID.
/// @param nIointerfaceId IO interface ID.
/// @param nSlotName Key slot name.
PSecretSeed_Ipc::PSecretSeed_Ipc(uint32_t const nSlotId,
                                 uint32_t const nIointerfaceId,
                                 ara::core::StringView const& nSlotName) noexcept
    : PSecretSeed_Ipc{false, false, kAllowPrototypedOnly, 0U, nSlotId, nIointerfaceId, nSlotName}
{
}
/// @brief Constructor with parameters.
/// @param bExportable Whether exportable.
/// @param bSession Whether temporary session.
/// @param usage Usage scope.
/// @param nBitlen Length in bits.
/// @param nSlotId Key slot ID.
/// @param nIointerfaceId IO interface ID.
/// @param nSlotName Key slot name.
PSecretSeed_Ipc::PSecretSeed_Ipc(bool const bExportable,
                                 bool const bSession,
                                 Usage const usage,
                                 uint32_t const nBitlen,
                                 uint32_t const nSlotId,
                                 uint32_t const nIointerfaceId,
                                 ara::core::StringView const& nSlotName) noexcept
    : SecretSeed{}  // NOLINT
    , bExportable_{bExportable}
    , bSession_{bSession}
    , usage_{usage}
    , nBitlen_{nBitlen}
    , nSlotId_{nSlotId}
    , nIointerfaceId_{nIointerfaceId}
    , nSlotName_{nSlotName}
{
}
/// @brief Clones this Secret Seed object into a new session object.
/// The created object instance is session and non-exportable. The AllowedUsageFlags property of the cloned object is the same as that of the source object!
/// If the size of the xorDelta parameter is less than the value size of this seed, only the corresponding leading bytes of the original seed are XOR-ed, and the rest are copied unchanged.
/// If the size of the xorDelta parameter is greater than the value size of this seed, the extra bytes of xorDelta are ignored.
/// @brief Clone this Secret Seed object to new session object.
///         Created object instance is session and non-exportable, @c AllowedUsageFlags attribute of the "cloned" object
///         is identical to this attribute of the source object! If size of the @c xorDelta argument is less than the
///         value size of this seed then only correspondent number of leading bytes of the original seed should be
///         XOR-ed, but the rest should be copied without change. If size of the @c xorDelta argument is larger than the
///         value size of this seed then extra bytes of the @c xorDelta should be ignored.
/// @param xorDelta  optional "delta" value that must be XOR-ed with the "cloned" copy of the original seed
/// @returns unique smart pointer to "cloned" session @c SecretSeed object
/// @returns  unique smart pointer to "cloned" session @c SecretSeed object
ara::core::Result< SecretSeed::Uptr > PSecretSeed_Ipc::Clone(ReadOnlyMemRegion const& xorDelta) const noexcept
{
    size_t const cloneLen{static_cast< size_t >(xorDelta.size())};
    uint8_t const* const pData{xorDelta.data()};

    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< SecretSeed::Uptr >::FromValue(nullptr);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(Clone), aswMsg,
        [this, cloneLen, pData](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                ipcReq.SetDataLen(static_cast< uint32_t >(cloneLen));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pData, static_cast< uint16_t >(cloneLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed) + cloneLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< SecretSeed::Uptr >::FromValue(nullptr);
    }

    keys::isoft_def::cryptoctx::PIpcAsw_Seed* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Seed >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< SecretSeed::Uptr >::FromValue(nullptr);
    }

    uint8_t* pDataRet{pIpcAsw->GetData()};
    uint32_t nDataLen{pIpcAsw->GetDataLen()};

    PSecretSeed::Uptr pSecretSeed{std::make_unique< PSecretSeed >(pDataRet, nDataLen)};
    if (nDataLen == 0U) {
    }
    pSecretSeed->SetAllowedUsage(usage_);
    pSecretSeed->SetSession(true);
    pSecretSeed->SetExportable(false);
    return ara::core::Result< SecretSeed::Uptr >::FromValue(std::move(pSecretSeed));
}
/// @brief According to the "counter" expression defined by the cryptographic algorithm associated with this object, sets the value of this seed object to "jump" to the specified step number from the initial state.
/// Steps can be positive or negative, corresponding to forward and backward directions of the "jump", but a value of 0 indicates copying the value only to this seed object. The seed size of the from parameter must be greater than or equal to this seed size.
/// @brief Set value of this seed object as a "jump" from an initial state to specified number of steps,
///           according to "counting" expression defined by a cryptographic algorithm associated with this object.
///       @c steps may have positive and negative values that correspond to forward and backward direction of the
///       "jump" respectively, but 0 value means only copy @c from value to this seed object.
///       Seed size of the @c from argument always must be greater or equal of this seed size.
/// @param from  source object that keeps the initial value for jumping from
/// @param steps  number of steps for the "jump"
/// @returns reference to this updated object
/// @error: SecurityErrorDomain::kIncompatibleObject  if this object and the @c from argument are associated
///     with incompatible cryptographic algorithms
/// @error: SecurityErrorDomain::kInvalidInputSize  if value size of the @c from seed is less then
///     value size of this one
ara::core::Result< void > PSecretSeed_Ipc::JumpFrom(SecretSeed const& from, std::int64_t steps) noexcept
{
    if (from.GetPayloadSize() < GetPayloadSize()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    uint8_t* pSrcData{nullptr};
    uint32_t nDataLen{0U};
    uint32_t nSlot{0U};
    PSecretSeed const* const fromSecretSeed{dynamic_cast< PSecretSeed const* >(&from)};
    if (fromSecretSeed != nullptr) {
        uint8_t const* const pdata{fromSecretSeed->GetData()};
        uint32_t const len{fromSecretSeed->GetDataLen()};
        nDataLen = len;
        std::unique_ptr< uint8_t[] > const memData{std::make_unique< uint8_t[] >(static_cast< size_t >(nDataLen))};
        pSrcData    = memData.get();
        std::ignore = memcpy(pSrcData, pdata, static_cast< size_t >(nDataLen));
    } else {
        PSecretSeed_Ipc const* const fromIpcSecretSeed{dynamic_cast< PSecretSeed_Ipc const* >(&from)};
        if (fromIpcSecretSeed == nullptr) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        nSlot = fromIpcSecretSeed->GetSlotId();
    }

    if (pSrcData == nullptr) {
        if (nSlot == 0U) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }

    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(JumpFrom), aswMsg,
        [this, steps, pSrcData, nDataLen](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                if (nSlotId_ != 0U) {
                    ipcReq.SetIpcSlotID(nSlotId_);
                }
                ipcReq.SetDataLen(nDataLen);
                ipcReq.SetSteps(steps);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                if (pSrcData != nullptr) {
                    std::ignore = pReqMsg->AddDataToIpc(pSrcData, static_cast< uint16_t >(nDataLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed) + nDataLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    keys::isoft_def::cryptoctx::PIpcAsw_Seed* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Seed >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    return ara::core::Result< void >::FromValue();
}
/// @brief According to the "counter" expression defined by the cryptographic algorithm associated with this object, sets the value of this seed object to "jump" to the specified step number from its current state.
/// Steps can be positive or negative, corresponding to forward and backward directions of the "jump", but a value of 0 indicates no change to the current seed value.
/// @brief Set value of this seed object as a "jump" from it's current state to specified number of steps,
///     according to "counting" expression defined by a cryptographic algorithm associated with this object.
///     @c steps may have positive and negative values that correspond to forward and backward direction of the
///     "jump" respectively, but 0 value means no changes of the current seed value.
/// @param steps  number of "steps" for jumping (forward or backward) from the current state
/// @returns reference to this updated object
SecretSeed& PSecretSeed_Ipc::Jump(std::int64_t steps) noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return *this;
    }
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(Jump), aswMsg, [this, steps](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                ipcReq.SetSteps(steps);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed));
        })};
    if (false == bDealIpc) {
        return *this;
    }
    return *this;
}
/// @brief Sets the next value of the secret seed according to the "counter" expression defined by the cryptographic algorithm associated with this object.
/// If the associated cryptographic algorithm does not specify a "counter" expression, the generic increment operation must be used as the default implementation (little-endian representation, i.e., the first byte is the least significant).
/// @brief Set next value of the secret seed according to "counting" expression defined by a cryptographic algorithm
///     associated with this object. If the associated cryptographic algorithm doesn't specify a "counting" expression
///     then generic increment operation must be implemented as default (little-endian notation, i.e. first byte is
///     least significant).
/// @returns  reference to this updated object
SecretSeed& PSecretSeed_Ipc::Next() noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return *this;
    }
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(Next), aswMsg, [this](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed));
        })};
    if (false == bDealIpc) {
        return *this;
    }
    return *this;
}
/// @brief XORs this seed object with another seed object and saves the result to this object.
/// If the seed sizes of this object and the source parameter are different, only the corresponding leading bytes of this seed object are updated.
/// @brief XOR value of this seed object with another one and save result to this object.
///       If seed sizes in this object and in the @c source argument are different then only correspondent number
///       of leading bytes in this seed object should be updated.
/// @param source  right argument for the XOR operation
/// @returns reference to this updated object
SecretSeed& PSecretSeed_Ipc::operator^=(SecretSeed const& source) & noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return *this;
    }

    uint8_t* pSrcData{nullptr};
    uint32_t nDataLen{0U};
    uint32_t nSlot{0U};
    PSecretSeed const* const fromSecretSeed{dynamic_cast< PSecretSeed const* >(&source)};
    if (fromSecretSeed != nullptr) {
        uint8_t const* const pdata{fromSecretSeed->GetData()};
        uint32_t const len{fromSecretSeed->GetDataLen()};

        nDataLen = len;
        std::unique_ptr< uint8_t[] > const memData{std::make_unique< uint8_t[] >(static_cast< size_t >(nDataLen))};
        pSrcData    = memData.get();
        std::ignore = memcpy(pSrcData, pdata, static_cast< size_t >(nDataLen));
    } else {
        PSecretSeed_Ipc const* const fromIpcSecretSeed{dynamic_cast< PSecretSeed_Ipc const* >(&source)};
        if (fromIpcSecretSeed == nullptr) {
            return *this;
        }
        nSlot = fromIpcSecretSeed->GetSlotId();
    }

    if (pSrcData == nullptr) {
        if (nSlot == 0U) {
            return *this;
        }
    }

    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(OperatorXor), aswMsg,
        [this, pSrcData, nDataLen](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                if (nSlotId_ != 0U) {
                    ipcReq.SetIpcSlotID(nSlotId_);
                }
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                if (pSrcData != nullptr) {
                    std::ignore = pReqMsg->AddDataToIpc(pSrcData, static_cast< uint16_t >(nDataLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed) + nDataLen);
        })};
    if (false == bDealIpc) {
        return *this;
    }

    keys::isoft_def::cryptoctx::PIpcAsw_Seed* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Seed >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return *this;
    }

    return *this;
}
/// @brief XORs the seed object with a memory area and saves the result to this object.
// If the seed sizes of this object and the source parameter are different, the leading bytes of this seed object are updated.
/// @brief XOR value of this seed object with provided memory region and save result to this object.
///       If seed sizes in this object and in the @c source argument are different then only correspondent number
///       of leading bytes of this seed object should be updated.
/// @param source  right argument for the XOR operation
/// @returns reference to this updated object
SecretSeed& PSecretSeed_Ipc::operator^=(ReadOnlyMemRegion const& source) & noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return *this;
    }

    uint8_t const* const pSrcData{source.data()};
    std::size_t const nDataLen{source.size()};

    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Seed(OperatorXor), aswMsg,
        [this, pSrcData, nDataLen](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pSrcData, static_cast< uint16_t >(nDataLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed) + nDataLen);
        })};
    if (false == bDealIpc) {
        return *this;
    }

    keys::isoft_def::cryptoctx::PIpcAsw_Seed* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Seed >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return *this;
    }
    return *this;
}
//*/ //CryptoObject Interface
/// @brief Returns the CryptoPrimitiveId of this cryptobject.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PSecretSeed_Ipc::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_SecretseedObj >()};
}
/// @brief Returns the COIdentifier of the object, which includes the object type and UID.
/// Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage.
/// Several related objects of different types can share a COUID (e.g., private key and public key), but the combination of COUID and object type must always be unique!
/// @returns  the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
///     not identifiable).
CryptoObject::COIdentifier PSecretSeed_Ipc::GetObjectId() const noexcept
{
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return CryptoObject::COIdentifier{};
    }
    keys::isoft_def::PIoInterface_Ipc::Uptr const pIpcInterface{
        std::make_unique< keys::isoft_def::PIoInterface_Ipc >(*ipcClient, nSlotName_)};
    if (pIpcInterface.get() == nullptr) {
        return CryptoObject::COIdentifier{};
    }
    pIpcInterface->SetSlotID(nSlotId_, nIointerfaceId_);

    CryptoObject::COIdentifier coIdentifier;

    coIdentifier.mCouid  = pIpcInterface->GetObjectId();
    coIdentifier.mCOType = GetkObjectType();

    return coIdentifier;
}
/// @brief Returns the actual size of the object payload. Unit: bytes.
/// @brief Return actual size of the object’s payload.
///         Returned value always must be less than or equal to the maximum payload size expected for this primitive and
///         object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
///         properties, but their size is fixed and common for all crypto objects independently from their actual type.
///         During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space
///         for an object’s meta-information automatically, according to their implementation details.
/// @returns size in bytes of the object's payload required for its storage
std::size_t PSecretSeed_Ipc::GetPayloadSize() const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return 0U;
    }

    bool const bDealIpc{
        ipcClient->DealIpcRequest(FUNC_NAME_Seed(GetPayloadSize), aswMsg,
                                  [this](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
                                      if (pReqMsg != nullptr) {
                                          keys::isoft_def::cryptoctx::PIpcReq_Seed ipcReq;
                                          ipcReq.SetIpcSlotID(nSlotId_);
                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                      }
                                      return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed));
                                  })};
    if (false == bDealIpc) {
        return 0U;
    }

    keys::isoft_def::cryptoctx::PIpcAsw_Seed* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Seed >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return 0U;
    }
    return static_cast< std::size_t >(pIpcAsw->GetDataLen());
}
/// @brief Returns the COIdentifier of the cryptobject that this cryptobject depends on.
/// For a signature object, this method must return a reference to the corresponding signature verification public key!
/// Uniquely identifying a cryptobject requires two components: cryptobject UID and cryptobject type.
/// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
///         For signatures objects this method @b must return a reference to correspondent signature verification public
///         key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid and @c
///         CryptoObjectType.
/// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
///     current object does not depend on another CryptoObject
CryptoObject::COIdentifier PSecretSeed_Ipc::HasDependence() const noexcept
{
    COIdentifier keyID;
    keyID.mCOType = CryptoObjectType::kUndefined;
    keyID.mCouid  = CryptoObjectUid();
    return keyID;
}
/// @brief Saves itself to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
/// A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @returns   has value if save sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_20517}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
///     KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
///     <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
///     @threadsafety{Thread-safe}
ara::core::Result< void > PSecretSeed_Ipc::Save(IOInterface& container) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    /// a KeySlot.
    if (IsSession()) {
        if (false == container.IsVolatile()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    /// if <tt>(container.Capacity() < this->StorageSize())</tt>
    if (GetPayloadSize() > container.GetCapacity()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    // /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    // IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (false == container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    if (false == container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see
    if (CryptoObjectType::kSecretSeed != container.GetTypeRestriction()) {
        if (container.GetTypeRestriction() != CryptoObjectType::kUndefined) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    }
    keys::isoft_def::PIoInterface_Ipc* const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc* >(&container)};
    if (pIoInterfaceIpc == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    uint32_t nSlotId{pIoInterfaceIpc->GetSlotID()};
    ara::core::String const nSlotName{pIoInterfaceIpc->GetSlotName()};
    if (nSlotId == 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (nSlotName.empty()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    keys::isoft_def::PIpcKeySlot::Uptr const pKeySlot{
        std::make_unique< keys::isoft_def::PIpcKeySlot >(pIoInterfaceIpc->GetIpcClient(), nSlotName.c_str(), nSlotId)};
    if (pKeySlot.get() == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (nSlotId == 0U) {
    }

    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    keys::isoft_def::PIoInterface_Ipc::Uptr const pIoInterface{
        std::make_unique< keys::isoft_def::PIoInterface_Ipc >(*ipcClient, nSlotName_)};
    pIoInterface->SetSlotID(nSlotId_, nIointerfaceId_);

    ara::core::Result< void > const resVoid{pKeySlot->SaveCopy(*pIoInterface)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    return ara::core::Result< void >::FromValue();
}

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
