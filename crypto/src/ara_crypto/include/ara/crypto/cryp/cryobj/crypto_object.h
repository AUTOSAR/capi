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
/// @file       crypto_object.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Common interface for all crypto objects recognizable by the crypto provider.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Symmetric Key
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=CryptoObject
/// @unit_description=Crypto Object
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_OBJECT_H_
#define ARA_CRYPTO_CRYP_CRYPTO_OBJECT_H_

#include <memory>

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/crypto_object_uid.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Common interface for all crypto objects recognizable by the crypto provider.
/// This interface (or any of its derivatives) represents an immutable (when completed) object that can be loaded into a transient transformation context.
/// @brief A common interface for all cryptograhic objects recognizable by the Crypto Provider.
///     This interface (or any its derivative) represents a non-mutable (after completion) object loadable to a
///     temporary transformation context.
/// @interface CryptoObject
/// @AUTOSAR_SWS {SWS_CRYPT_20500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02508
/// @trace_id_dd=DD_CRYPTO_05296
/// @needwork = ad
/// @endcode
class CryptoObject
{
public:
    /// @brief Unique identifier for this crypto object.
    /// @brief Unique identifier of this CryptoObject.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20504}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02509
    /// @trace_id_dd=DD_CRYPTO_05297
    /// @needwork = ad
    /// @endcode
    struct COIdentifier final
    {
        /// @brief Object type
        /// @brief type of objext
        /// @trace_id_sws={SWS_CRYPT_20506}
        /// @tracestatus={draft}
        /// @uptrace={RS_CRYPTO_02005}
        CryptoObjectType mCOType{CryptoObjectType::kUndefined};
        /// @brief Object identifier
        /// @brief object identifier
        /// @trace_id_sws={SWS_CRYPT_20507}
        /// @tracestatus={draft}
        /// @uptrace={RS_CRYPTO_02005}
        CryptoObjectUid mCouid{};
    };

public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the constant interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20502}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03214
    /// @trace_id_dd=DD_CRYPTO_06426
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< CryptoObject const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the  interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03215
    /// @trace_id_dd=DD_CRYPTO_06427
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< CryptoObject >;

public:
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20503}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02510
    /// @trace_id_dd=DD_CRYPTO_05298
    /// @needwork = ad
    /// @endcode
    virtual ~CryptoObject() noexcept = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoObject to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30208}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02009}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05299
    /// @needwork = dda
    /// @endcode
    CryptoObject &operator=(CryptoObject const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoObject to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30209}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05300
    /// @needwork = dda
    /// @endcode
    CryptoObject &operator=(CryptoObject &&other) = delete;

protected:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05303
    /// @needwork = dda
    /// @endcode
    CryptoObject() = default;

public:
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05301
    /// @needwork = dda
    /// @endcode
    CryptoObject(CryptoObject const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05302
    /// @needwork = dda
    /// @endcode
    CryptoObject(CryptoObject &&other) = delete;

public:
    /// @brief Cast down and move the unique smart pointer from the generic CryptoObject interface to a specific derived object.
    /// @brief Downcast and move unique smart pointer from the generic @c CryptoObject interface to concrete derived
    /// object.
    /// @param object  unique smart pointer to the constant generic @c CryptoObject interface
    /// @return unique smart pointer to downcasted constant interface of specified derived type
    /// @code{.isoft}
    /// @tparam ConcreteObject target type (derived from @c CryptoObject) for downcasting
    /// @error: SecurityErrorDomain::kBadObjectType      if an actual type of the @c object is not the specified @c
    /// ConcreteObject
    /// @trace_id_sws={SWS_CRYPT_20518}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02511
    /// @trace_id_dd=DD_CRYPTO_05304
    /// @needwork = ad
    /// @endcode
    template < class ConcreteObject >
    static ara::core::Result< typename ConcreteObject::Uptrc > Downcast(CryptoObject::Uptrc &&object) noexcept
    {
        // PRQA S 2025 QAC /// @qac: False positive
        /// @brief Return value type alias
        using PResult = ara::core::Result< typename ConcreteObject::Uptrc >;
        // PRQA L:QAC
        if (object == nullptr) {
            return PResult::FromValue(nullptr);
        }
        CryptoObject::Uptrc newObject{std::move(object)};
        ConcreteObject const *const pConcreteObject{dynamic_cast< ConcreteObject const * >(newObject.release())};
        if (pConcreteObject == nullptr) {
            return PResult::FromError(SecurityErrorDomain::Errc::kBadObjectType);
        }
        return PResult::FromValue(pConcreteObject);
    }
    /// @brief Return the CryptoPrimitiveId of this crypto object.
    /// @brief Return the CryptoPrimitivId of this CryptoObject.
    /// @return Crypto algorithm ID
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20505}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02512
    /// @trace_id_dd=DD_CRYPTO_05305
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept = 0;
    /// @brief Return the COIdentifier of the object, which includes the object's type and UID.
    ///         Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage. Several related objects of different types can share a COUID (e.g., private and public keys), but the combination of COUID and object type must always be unique!
    /// @brief Return the object's COIdentifier, which includes the object's type and UID.
    ///         An object that has no assigned @a COUID cannot be (securely) serialized / exported or saved to a
    ///         non-volatile storage. An object should not have a @a COUID if it is session and non-exportable
    ///         simultaneously A few related objects of different types can share a single @a COUID (e.g. private and
    ///         public keys), but a combination of @a COUID and object type must be unique always!
    /// @return the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
    /// not identifiable).
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20514}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02513
    /// @trace_id_dd=DD_CRYPTO_05306
    /// @needwork = ad
    /// @endcode
    virtual COIdentifier GetObjectId() const noexcept = 0;
    /// @brief Return the actual size of the object payload. Byte unit.
    /// @brief Return actual size of the object’s payload.
    ///         Returned value always must be less than or equal to the maximum payload size expected for this primitive
    ///         and object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
    ///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
    ///         properties, but their size is fixed and common for all crypto objects independently from their actual
    ///         type. During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers)
    ///         reserve space for an object’s meta-information automatically, according to their implementation details.
    /// @return size in bytes of the object's payload required for its storage
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20516}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02514
    /// @trace_id_dd=DD_CRYPTO_05307
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetPayloadSize() const noexcept = 0;
    /// @brief Return the COIdentifier of the crypto object this crypto object depends on.
    ///         For signature objects, this method must return a reference to the corresponding public key used for signature verification!
    ///         Unambiguous identification of a crypto object requires two components: CryptoObjectUid and CryptoObjectType.
    /// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
    ///         For signatures objects this method @b must return a reference to correspondent signature verification
    ///         public key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid
    ///         and @c CryptoObjectType.
    /// @return target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
    /// current object does not depend on another CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20515}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02515
    /// @trace_id_dd=DD_CRYPTO_05308
    /// @needwork = ad
    /// @endcode
    virtual COIdentifier HasDependence() const noexcept = 0;
    /// @brief Get the exportability property of the crypto object. An exportable object must have an assigned COUID (see GetObjectId()).
    /// @brief Get the exportability attribute of the crypto object. An exportable object must have an assigned @a COUID
    /// (see @c GetObjectId()).
    /// @return @c true if the object is exportable (i.e. if it can be exported outside the trusted environment of the
    /// Crypto Provider)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20513}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02516
    /// @trace_id_dd=DD_CRYPTO_05309
    /// @needwork = ad
    /// @endcode
    virtual bool IsExportable() const noexcept = 0;
    /// @brief Return the "session" (or "temporary") attribute of the object.
    ///         A temporary object cannot be saved to a persistent storage location pointed to by an IOInterface! A temporary object will be safely destroyed together with this interface instance!
    ///         Non-session objects must have an assigned COUID (see GetObjectId()).
    /// @brief Return the "session" (or "temporary") attribute of the object.
    ///      A temporary object cannot be saved to a persistent storage location pointed to by an IOInterface!
    ///      A temporary object will be securely destroyed together with this interface instance!
    ///      A non-session object must have an assigned @a COUID (see @c GetObjectId()).
    /// @return @c true if the object is temporay (i.e. its life time is limited by the current session only)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20512}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02517
    /// @trace_id_dd=DD_CRYPTO_05310
    /// @needwork = ad
    /// @endcode
    virtual bool IsSession() const noexcept = 0;
    /// @brief Save itself to the provided IOInterface. Crypto objects with the "session" attribute cannot be saved in a KeySlot.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20517}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02518
    /// @trace_id_dd=DD_CRYPTO_05311
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void >
    /// @brief Save itself to provided IOInterface
    /// A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @param container IOInterface representing underlying storage
    /// @return
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    ///         a KeySlot.
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (“@see
    ///         keys::KeySlotPrototypeProps)”
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    ///         if <tt>(container.Capacity() < this->StorageSize())</tt>
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    ///         IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    Save(IOInterface &container) const noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_CRYPTO_OBJECT_H_
