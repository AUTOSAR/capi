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
/// @file       isoft_signature.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-25
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PSignature
/// @unit_description=Signature Storage
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_signature.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature container interface for storing digital signatures, hash digests, (Hash-based) Message Authentication Code (MAC/HMAC).
/// In the case of a key signature (digital signature or MAC/HMAC), the COUID of the signature verification key can be obtained by calling CryptoObject::hasdependency()!
//********************************/
/// @brief Constructor with parameters.
/// @param nHashAlgID Hash algorithm crypto primitive ID.
PSignature::PSignature(CryptoPrimitiveId::AlgId const nHashAlgID) noexcept
    : PSignature{false, false, nullptr, nHashAlgID, static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kUnDefine),
                 {}}
{
}
/// @brief Constructor with parameters.
/// @param nHashAlgID Hash algorithm crypto primitive ID.
/// @param nCryptoAlgId Encryption algorithm crypto primitive ID.
PSignature::PSignature(CryptoPrimitiveId::AlgId const nHashAlgID, CryptoPrimitiveId::AlgId const nCryptoAlgId) noexcept
    : PSignature{false, false, nullptr, nHashAlgID, nCryptoAlgId, {}}
{
}
/// @brief Constructor with parameters.
/// @param bExportable Whether exportable.
/// @param bSession Whether temporary session.
/// @param dataBuff Data buffer.
/// @param nHashAlgID Hash algorithm crypto primitive ID.
/// @param nCryptoAlgId Encryption algorithm crypto primitive ID.
/// @param dePendencedKeyId COIdentifier of the depended cryptobject.
PSignature::PSignature(bool const bExportable,
                       bool const bSession,
                       internal::PAutoBuff const *const dataBuff,
                       CryptoPrimitiveId::AlgId const nHashAlgID,
                       CryptoPrimitiveId::AlgId const nCryptoAlgId,
                       COIdentifier const &dePendencedKeyId) noexcept
    : Signature{}  // NOLINT
    , bExportable_{bExportable}
    , bSession_{bSession}
    , dataBuff_{}  // NOLINT
    , nHashAlgID_{nHashAlgID}
    , nCryptoAlgId_{nCryptoAlgId}
    , dePendencedKeyId_{dePendencedKeyId}
{
    std::ignore = dataBuff;
}
/// @brief Default copy assignment operator.
/// @param other Another instance of this class.
/// @return *this
PSignature &PSignature::operator=(PSignature const &other) noexcept
{
    if (this != &other) {
        bExportable_ = other.bExportable_;
        bSession_    = other.bSession_;
        bool const ret{dataBuff_.SetData(other.dataBuff_.Data(), other.dataBuff_.size(), true)};
        std::ignore   = ret;
        nHashAlgID_   = other.nHashAlgID_;
        nCryptoAlgId_ = other.nCryptoAlgId_;
    }
    return *this;
}
/// @brief Default move assignment operator.
/// @param other Another instance of this class.
/// @return *this
PSignature &PSignature::operator=(PSignature &&other) noexcept
{
    bExportable_ = other.bExportable_;
    bSession_    = other.bSession_;
    bool const ret{dataBuff_.SetData(other.dataBuff_.Data(), other.dataBuff_.size(), true)};
    std::ignore   = ret;
    nHashAlgID_   = other.nHashAlgID_;
    nCryptoAlgId_ = other.nCryptoAlgId_;
    other.dataBuff_.ResetData();
    std::ignore = std::move(other);
    return *this;
}
//*/ //CryptoObject Interface
/// @brief Returns the CryptoPrimitiveId of this cryptobject.
/// @return CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PSignature::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_SignatureObj >()};
}
/// @brief Returns the COIdentifier of the object, which includes the object type and UID.
/// Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage.
/// Several related objects of different types can share a COUID (e.g., private key and public key), but the combination of COUID and object type must always be unique!
/// @return the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
///     not identifiable).
CryptoObject::COIdentifier PSignature::GetObjectId() const noexcept
{
    COIdentifier keyID;
    keyID.mCOType              = CryptoObjectType::kUndefined;
    keyID.mCouid.mGeneratorUid = Uuid();
    keyID.mCouid.mVersionStamp = 0U;
    return keyID;
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
std::size_t PSignature::GetPayloadSize() const noexcept { return static_cast< std::size_t >(dataBuff_.size()); }
/// @brief Returns the COIdentifier of the cryptobject that this cryptobject depends on.
/// For a signature object, this method must return a reference to the corresponding signature verification public key!
/// Uniquely identifying a cryptobject requires two components: cryptobject UID and cryptobject type.
/// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
///         For signatures objects this method @b must return a reference to correspondent signature verification public
///         key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid and @c
///         CryptoObjectType.
/// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
///     current object does not depend on another CryptoObject
CryptoObject::COIdentifier PSignature::HasDependence() const noexcept { return dePendencedKeyId_; }
/// @brief Gets the exportability property of the encryption object. An exportable object must have an assigned COUID (see GetObjectId()).
/// @brief Get the exportability attribute of the crypto object. An exportable object must have an assigned @a COUID
///     (see @c GetObjectId()).
/// @returns @c true if the object is exportable (i.e. if it can be exported outside the trusted environment of the
///     Crypto Provider)
bool PSignature::IsExportable() const noexcept { return true; }
/// @brief Returns the "session" (or "temporary") property of the object.
/// A temporary object cannot be saved to a persistent storage location pointed to by IOInterface! A temporary object will be safely destroyed along with this interface instance!
/// Non-session objects must have an assigned COUID (see GetObjectId()).
/// @return @c true if the object is temporay (i.e. its life time is limited by the current session only)
bool PSignature::IsSession() const noexcept { return true; }
/// @brief Saves itself to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
///     A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @return
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
///     KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
///     <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
ara::core::Result< void > PSignature::Save(IOInterface &container) const noexcept
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
    if (dataBuff_.GetBuffLen() > container.GetCapacity()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (false == container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    if (false == container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see
    if (CryptoObjectType::kSignature != container.GetTypeRestriction()) {
        if (container.GetTypeRestriction() != CryptoObjectType::kUndefined) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    }
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
    if (false == _SaveToKeySlot(container)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
    }
    return ara::core::Result< void >::FromValue();
}
//*/ //Serializable Interface
/// @brief Serializes itself publicly.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
///     required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
///     type
ara::core::Result< ara::core::Vector< ara::core::Byte > > PSignature::ExportPublicly(
    Serializable::FormatId formatId) const noexcept
{
    /// If not exportable
    if (!IsExportable()) {
        std::ignore = formatId;
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }
    ara::core::Vector< ara::core::Byte > vecResult;
    uint8_t const *const pData{dataBuff_.Data()};
    uint32_t const dataLen{dataBuff_.size()};

    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }

    vecResult.resize(static_cast< std::size_t >(dataBuff_.size()));
    for (uint32_t i{0U}; i < dataLen; i++) {
        vecResult[static_cast< size_t >(i)] = static_cast< ara::core::Byte >(*(pData + i));
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
//*/ //Signature Interface
/// @brief Gets the hash size required by the current signature algorithm.
/// @return required hash size in bytes
std::size_t PSignature::GetRequiredHashSize() const noexcept { return static_cast< std::size_t >(dataBuff_.size()); }
/// @brief Sets the hash data.
/// @param nHashAlgID Hash algorithm crypto primitive ID.
/// @param pSignatureData Signature data.
/// @param nSignatureLength Signature data length.
void PSignature::SetSignatureData(CryptoPrimitiveId::AlgId const nHashAlgID,
                                  void const *const pSignatureData,
                                  uint32_t const nSignatureLength) noexcept
{
    nHashAlgID_ = nHashAlgID;
    dataBuff_.ResetBuff(nSignatureLength);
    bool const ret{dataBuff_.SetData(static_cast< uint8_t const * >(static_cast< void const * >(pSignatureData)),
                                     nSignatureLength)};
    std::ignore = ret;
}
/// @brief Sets the public key COUID that the signature depends on.
/// @param keyId IO interface ID corresponding to the key material.
void PSignature::SetDependence(COIdentifier const &keyId) noexcept { dePendencedKeyId_ = keyId; }
/// @brief: Saves the signature information to the IOInterface.
/// @name  _SaveToKeySlot
/// @param container IO interface.
/// @returns  true if save to keyslot sucess false otherwise
bool PSignature::_SaveToKeySlot(IOInterface &container) const noexcept
{
    keys::isoft_def::PIoInterface *const pIoInterface{dynamic_cast< keys::isoft_def::PIoInterface * >(&container)};
    if (nullptr == pIoInterface) {
        return false;
    }
    ReadOnlyMemRegion const memKeyData{T_TransBytes(dataBuff_.Data()), static_cast< size_t >(dataBuff_.size())};
    return pIoInterface->SaveKeyToSlot< PSignature >(this, memKeyData);
}

/// @brief When verifying a signature, retrieves the COUID of the public key stored in the signature for comparison with the public key COUID used in the verification context. This step is performed in the VerifyPrehashed() function of ph_ctx_dsv_verifier_public.cpp.
/// @return VerifyPrehashed() function in ph_ctx_dsv_verifier_public.cpp
/// @return Dependency COUID.
CryptoObjectUid PSignature::GetDependence() const noexcept { return dePendencedKeyId_.mCouid; }
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
