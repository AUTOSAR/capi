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
/// @file       io_interface.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Formal interface of IOInterface for saving and loading secure objects.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=IO Interface Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_IO_INTERFACE_H_
#define ARA_CRYPTO_IO_INTERFACE_H_

#include <cstdint>

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/crypto_object_uid.h"

namespace ara {
namespace crypto {
//********************************/
//- @interface IOInterface
/// @brief Formal interface of IOInterface for saving and loading secure objects.
///         Actual saving and loading should be implemented via internal methods known to the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
/// @brief Formal interface of an IOInterface is used for saving and loading of security objects.
///       Actual saving and loading should be implemented by internal methods known to a trusted pair of Crypto Provider
///       and Storage Provider. Each object should be uniquely identified by its type and Crypto Object Unique
///       Identifier (@b COUID). This interface suppose that objects in the container are compressed i.e. have a minimal
///       size optimized for
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02090
/// @trace_id_dd=DD_CRYPTO_04858
/// @needwork = ad
/// @endcode
class IOInterface
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02090
    /// @trace_id_dd=DD_CRYPTO_06394
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< IOInterface >;
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the constant interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02090
    /// @trace_id_dd=DD_CRYPTO_06395
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< IOInterface const >;

public:
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10810}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02091
    /// @trace_id_dd=DD_CRYPTO_04859
    /// @needwork = ad
    /// @endcode
    virtual ~IOInterface() noexcept = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another IOInterface to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30202}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02092
    /// @trace_id_dd=DD_CRYPTO_04860
    /// @needwork = ad
    /// @endcode
    IOInterface& operator=(IOInterface const& other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another IOInterface to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30203}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02093
    /// @trace_id_dd=DD_CRYPTO_04861
    /// @needwork = ad
    /// @endcode
    IOInterface& operator=(IOInterface&& other) = delete;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02094
    /// @trace_id_dd=DD_CRYPTO_04862
    /// @needwork = ad
    /// @endcode
    IOInterface() = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02095
    /// @trace_id_dd=DD_CRYPTO_04863
    /// @needwork = ad
    /// @endcode
    IOInterface(IOInterface const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02096
    /// @trace_id_dd=DD_CRYPTO_04864
    /// @needwork = ad
    /// @endcode
    IOInterface(IOInterface&& other) = delete;

public:
    /// @brief Returns the actual allowed key/seed usage flags as defined by the key slot prototype of this "Actor" and the current content of the container.
    ///         Volatile containers have no prototype restrictions, but can define restrictions for the current instance of the object at runtime.
    ///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
    ///         This method is particularly useful for empty permanent prototype containers.
    /// @brief Return actual allowed key/seed usage flags defined by the key slot prototype for this "Actor" and current
    /// content of the container.
    ///       Volatile containers don't have any prototyped restrictions, but can have restrictions defined at run-time
    ///       for a current instance of object. A value returned by this method is bitwise AND of the common usage flags
    ///       defined at run-time and the usage flags defined by the UserPermissions prototype for current "Actor". This
    ///       method is especially useful for empty permanent prototyped containers.
    /// @return allowed key/seed usage flags
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10819}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02097
    /// @trace_id_dd=DD_CRYPTO_04865
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept = 0;
    /// @brief Returns the capacity of the underlying resource.
    /// @brief Return capacity of the underlying resource.
    /// @return capacity of the underlying buffer of this IOInterface (in bytes)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10813}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02098
    /// @trace_id_dd=DD_CRYPTO_04866
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetCapacity() const noexcept = 0;
    /// @brief Returns the crypto object type of the object referenced by this IOInterface.
    /// @brief Return the CryptoObjectType of the object referenced by this IOInterface.
    /// @return the CryptoObjectType stored inside the referenced resource
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10812}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02099
    /// @trace_id_dd=DD_CRYPTO_04867
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectType GetCryptoObjectType() const noexcept = 0;
    /// @brief Returns the COUID of the object stored in the IOInterface.
    ///         If the container is empty, this method returns CryptoObjectType::kUndefined. Unambiguous identification of a crypto object requires two components: CryptoObjectUid and CryptoObjectType.
    /// @brief Return @a COUID of an object stored to this IOInterface.
    ///       If the container is empty then this method returns @c CryptoObjectType::KUndefined.
    ///       Unambiguous identification of a crypto object requires both components: @c CryptoObjectUid and
    ///       @c CryptoObjectType.
    /// @return type of the content stored in the container
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10811}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02100
    /// @trace_id_dd=DD_CRYPTO_04868
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetObjectId() const noexcept = 0;
    /// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
    ///         If the container is empty, this method returns 0. The return value does not consider the object's meta-information attributes, but their size is fixed and common for all crypto objects independent of their actual type.
    ///         Space for the object's meta-information is provided automatically based on the object's implementation details.
    /// @brief Return size of an object payload stored in the underlying buffer of this IOInterface.
    ///       If the container is empty then this method returns 0.
    ///       Returned value does not take into account the object's meta-information properties,
    ///       but their size is fixed and common for all crypto objects independently from their actual type.
    ///       space for an object's meta-information automatically, according to their implementation details.
    /// @return size of an object payload stored in the underlying buffer of this IOInterface (in bytes)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10817}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02101
    /// @trace_id_dd=DD_CRYPTO_04869
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetPayloadSize() const noexcept = 0;
    /// @brief Gets the vendor-specific ID of the primitive.
    /// @brief Get vendor specific ID of the primitive.
    /// @return the binary Crypto Primitive ID
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10822}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02102
    /// @trace_id_dd=DD_CRYPTO_04870
    /// @needwork = ad
    /// @endcode
    virtual CryptoAlgId GetPrimitiveId() const noexcept = 0;
    /// @brief Returns the content type restriction of this interface (identifying what type of data this container can hold).
    /// @return an object type of allowed content (@c CryptoObjectType::kUndefined means without restriction)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10818}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02103
    /// @trace_id_dd=DD_CRYPTO_04871
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectType GetTypeRestriction() const noexcept = 0;
    /// @brief Returns the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
    /// @brief Return the "exportable" attribute of an object stored to the container.
    ///       The exportability of an object doesn't depend from the volatility of its container.
    /// @return @c true if an object stored to the container has set the "exportable" attribute
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10816}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02104
    /// @trace_id_dd=DD_CRYPTO_04872
    /// @needwork = ad
    /// @endcode
    virtual bool IsObjectExportable() const noexcept = 0;
    /// @brief Returns the "session" (or "temporary") attribute of the set object, e.g., from keyderivationfunctionctx::DeriveKey().
    ///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
    /// @brief Return the "session" (or "temporary") attribute of an object as set e.g. by
    /// KeyDerivationFunctionCtx::DeriveKey().
    ///       A "session" object can be stored to a VolatileTrustedContainer only!
    ///       If this IOInterface is linked to a KeySlot this returns always false.
    /// @return @c true if the object referenced by this IOInterface has set the "session" attribute
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10815}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02105
    /// @trace_id_dd=DD_CRYPTO_04873
    /// @needwork = ad
    /// @endcode
    virtual bool IsObjectSession() const noexcept = 0;
    /// @brief Returns the "volatile" attribute of the underlying buffer of this IOInterface.
    ///         "session" objects can only be stored in "volatile" containers. The contents of a "volatile" container are destroyed together with the interface instance.
    /// @brief Return volatility of the the underlying buffer of this IOInterface.
    ///      A "session" object can be stored to a "volatile" container only.
    ///      A content of a "volatile" container will be destroyed together with the interface instance.
    /// @return @c true if the container has a volatile nature (i.e. "temporary" or "in RAM") or @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10814}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02106
    /// @trace_id_dd=DD_CRYPTO_04874
    /// @needwork = ad
    /// @endcode
    virtual bool IsVolatile() const noexcept = 0;
    /// @brief Gets whether the underlying key storage is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface becomes invalid.
    /// @brief Get whether the underlying KeySlot is valid. An IOInterface is invalidated if the underlying resource has
    /// been modified after the IOInterface has been opened.
    /// @return true if the underlying resource can be valid, false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10823}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02107
    /// @trace_id_dd=DD_CRYPTO_04875
    /// @needwork = ad
    /// @endcode
    virtual bool IsValid() const noexcept = 0;
    /// @brief Gets whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
    /// @brief Get whether the underlying KeySlot is writable - if this IOInterface is linked to a
    /// VolatileTrustedContainer always return true.
    /// @return true if the underlying resource can be written
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10821}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02108
    /// @trace_id_dd=DD_CRYPTO_04876
    /// @needwork = ad
    /// @endcode
    virtual bool IsWritable() const noexcept = 0;

public:
};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_IO_INTERFACE_H_
