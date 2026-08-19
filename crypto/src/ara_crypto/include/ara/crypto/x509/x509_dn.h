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
/// @file       x509_dn.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/DN Distinguished Name
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03002
/// @unit_name=X509DN
/// @unit_description=Certificate DN Information
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_X509_X509_DN_H_
#define ARA_CRYPTO_X509_X509_X509_DN_H_

#include <openssl/x509.h>

#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/crypto/x509/x509_object.h"

namespace ara {
namespace crypto {
namespace x509 {
/// @brief Define constant 128U
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02634
/// @trace_id_dd=DD_CRYPTO_05456
/// @needwork = dd
/// @endcode
constexpr uint32_t kUint_128U{128U};
//********************************/
//- @interface X509DN
/// @brief X.509 DN (Distinguished Name) interface.
/// @brief Interface of X.509 Distinguished Name (DN).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02634
/// @trace_id_dd=DD_CRYPTO_05457
/// @needwork = ad
/// @endcode
class X509DN : public X509Object
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the constant interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03233
    /// @trace_id_dd=DD_CRYPTO_06445
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< X509DN const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03234
    /// @trace_id_dd=DD_CRYPTO_06446
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< X509DN >;

public:
    /// @brief Enumeration of identifiers for DN attributes.
    /// @brief Enumeration of DN attributes' identifiers.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40403}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02635
    /// @trace_id_dd=DD_CRYPTO_05458
    /// @needwork = ad
    /// @endcode
    enum class AttributeId : std::uint32_t
    {
        kUnSupport = UINT32_MAX,
        /// Common Name
        kCommonName = 0,
        /// Country
        kCountry = 1,
        /// State
        kState = 2,
        /// Locality
        kLocality = 3,
        /// Organization
        kOrganization = 4,
        /// Organization Unit
        kOrgUnit = 5,
        /// Street
        kStreet = 6,
        /// Postal Code
        kPostalCode = 7,
        /// Title
        kTitle = 8,
        /// Surname
        kSurname = 9,
        /// Given Name
        kGivenName = 10,
        /// Initials
        kInitials = 11,
        /// Pseudonym
        kPseudonym = 12,
        /// Generation Qualifier
        kGenerationQualifier = 13,
        /// Domain Component
        kDomainComponent = 14,
        /// Distinguished Name Qualifier
        kDnQualifier = 15,
        /// E-mail
        kEmail = 16,
        /// URI
        kUri = 17,
        /// DNS
        kDns = 18,
        /// Host Name (UNSTRUCTUREDNAME)
        kHostName = 19,
        /// IP Address (UNSTRUCTUREDADDRESS)
        kIpAddress = 20,
        /// Serial Numbers
        kSerialNumbers = 21,
        /// User ID
        kUserId = 22,
        /// Maximum supported value
        kMaxSupport = 23,
    };

public:
    /// @brief Get the DN attribute by ID (this method applies to all attributes except kOrgUnit and kDomainComponent).
    ///         The capacity of the output string must be sufficient to store the output value! If (attribute == nullptr), this method only returns the required buffer capacity.
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kUnknownIdentifier  if the @c id argument has unsupported value
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (attribute != nullptr), but @c attribute->capacity() is
    /// less than required for storing of the output
    /// @trace_id_sws={SWS_CRYPT_40413}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02636
    /// @trace_id_dd=DD_CRYPTO_05459
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::StringView >
    /// @brief Get DN attribute by its ID (this method is applicale to all attributes except @c kOrgUnit and
    ///           @c kDomainComponent).
    ///       Capacity of the output string must be enough for storing the output value!
    ///       If (attribute == nullptr) then method only returns required buffer capacity.
    /// @param id  the identifier of required attribute
    /// @returns StringView of the attribute
    GetAttribute(AttributeId id) const noexcept;
    /// @brief Return the DN attribute by ID and sequence index (this method applies to attributes kOrgUnit and kDomainComponent).
    ///         The capacity of the output string must be sufficient to store the output value! If (attribute == nullptr), this method only returns the required buffer capacity.
    /// @brief Return DN attribute by its ID and sequential index (this method is applicale to attributes @c kOrgUnit
    /// and @c kDomainComponent).
    ///       Capacity of the output string must be enough for storing the output value!
    ///       If (attribute == nullptr) then method only returns required buffer capacity.

    /// @param id  the identifier of required attribute
    /// @param indexof  the zero-based index of required component of the attribute
    /// @returns StringView of the attribute
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kUnknownIdentifier     if the @c id argument has unsupported value
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (attribute != nullptr),
    ///                but @c attribute->capacity() is less than required for storing of the output
    /// @error:  SecurityErrorDomain::kInvalidArgument       if (id != kOrgUnit) && (id != kDomainComponent) && (index
    /// >0)
    /// @error:  SecurityErrorDomain::kAboveBoundary         if ((id == kOrgUnit) || (id == kDomainComponent))
    ///                and the @c index value is greater than or equal to the actual number of components in the
    ///                specified attribute
    /// @trace_id_sws={SWS_CRYPT_40415}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02637
    /// @trace_id_dd=DD_CRYPTO_05460
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::StringView > GetAttribute(AttributeId id, uint32_t indexof) const noexcept;
    /// @brief Get the entire Distinguished Name (DN) as a single string. The capacity of the output string must be sufficient to store the output value! If (dn ==
    /// nullptr), this method only returns the required buffer capacity.
    /// @brief Get the whole Distinguished Name (DN) as a single string.
    ///       Capacity of the output string must be enough for storing the output value!
    ///       If (dn == nullptr) then method only returns required buffer capacity.
    /// @returns StringView of the whole DN string
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (dn != nullptr), but @c dn->capacity() is less than
    /// required for the output value storing
    /// @trace_id_sws={SWS_CRYPT_40411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02638
    /// @trace_id_dd=DD_CRYPTO_05461
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::StringView > GetDnString() const noexcept;
    /// @brief Check equality between this object and another Distinguished Name (DN) object.
    /// @brief Check for equality of this and another Distinguished Name (DN) objects.
    /// @param other Another instance of this class another instance of DN for comparison
    /// @returns @c true if the provided DN is identical to this one and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40417}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02639
    /// @trace_id_dd=DD_CRYPTO_05462
    /// @needwork = ad
    /// @qac [2066]possibly cannot be modified: Relational operator 'bool operator XXX(...)' is a member.
    /// @endcode
    // PRQA S 2066 QAC /// @qac: AUTOSAR standard interface
    virtual bool operator==(X509DN const& other) const noexcept;
    // PRQA L:QAC
    /// @brief Check inequality between this object and another Distinguished Name (DN) object.
    /// @brief Check for inequality of this and another Distinguished Name (DN) objects.
    /// @param other Another instance of this class another instance of DN for comparison
    /// @returns @c true if the provided DN is not identical to this one and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40418}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02640
    /// @trace_id_dd=DD_CRYPTO_05463
    /// @needwork = ad
    /// @qac [2066]possibly cannot be modified: Relational operator 'bool operator XXX(...)' is a member.
    /// @endcode
    // PRQA S 2066 QAC /// @qac: AUTOSAR standard interface
    bool operator!=(X509DN const& other) const noexcept { return !(*this == other); }
    // PRQA L:QAC
    /// @brief Set the DN attribute by ID (this method applies to all attributes except kOrgUnit and kDomainComponent).
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kUnknownIdentifier if the @c id argument has unsupported value
    /// @error:  SecurityErrorDomain::kUnexpectedValue   if the attribute string contains incorrect characters or it has
    /// unsupported length
    /// @trace_id_sws={SWS_CRYPT_40414}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02641
    /// @trace_id_dd=DD_CRYPTO_05464
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void >
    /// @brief Set DN attribute by its ID (this method is applicale to all attributes except @c kOrgUnit and @c
    /// kDomainComponent).
    /// @param id  the identifier of required attributet
    /// @param attribute  the attribute value
    /// @return
    SetAttribute(AttributeId id, ara::core::StringView const& attribute) const noexcept;
    /// @brief Set the DN attribute by DN ID and sequence index (applies to attributes kOrgUnit and kDomainComponent).
    /// @brief Set DN attribute by its ID and sequential index (this method is applicale to attributes @c kOrgUnit and
    /// @param id  the identifier of required attribute
    /// @param nIndex  the zero-based index of required component of the attribute
    /// @param attribute  the attribute value
    /// @return true if SetAttribute sucess false otherwise @c kDomainComponent).
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kUnknownIdentifier if the @c id argument has unsupported value
    /// @error:  SecurityErrorDomain::kUnexpectedValue   if the attribute string contains incorrect characters or it has
    /// unsupported length
    /// @error:  SecurityErrorDomain::kInvalidArgument   if (id != kOrgUnit) && (id != kDomainComponent) && (index > 0)
    /// @error:  SecurityErrorDomain::kAboveBoundary     if ((id == kOrgUnit) || (id == kDomainComponent)) and the @c
    /// index value is greater than the current number of components in the specified attribute
    /// @trace_id_sws={SWS_CRYPT_40416}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02642
    /// @trace_id_dd=DD_CRYPTO_05465
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetAttribute(AttributeId id,
                                                   uint32_t nIndex,
                                                   ara::core::StringView const& attribute) const noexcept;
    /// @brief Set the entire DN (Distinguished Name) from a single string.
    ///         [Error]: SecurityErrorDomain::kUnexpectedValue if the dn string has incorrect syntax.
    /// @brief Set whole Distinguished Name (DN) from a single string.
    ///             [Error]: SecurityErrorDomain::kUnexpectedValue  if the @c dn string has incorrect syntax
    /// @param dn  the single string containing the whole DN value in text format
    /// @return true if SetDn sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02643
    /// @trace_id_dd=DD_CRYPTO_05466
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetDn(ara::core::StringView const& dn) noexcept;

public:  // Serializable interface
    /// @brief Expose serialization itself.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02644
    /// @trace_id_dd=DD_CRYPTO_05467
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;
    /// @brief Use base class template functions
    using Serializable::ExportPublicly;

private:
    /// @brief 2022-06-08 hanjingjing assumes 128 is the maximum length of a DN string, specific information needs to be looked up in relevant documentation
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05468
    /// @needwork = dda
    /// @endcode
    uint32_t nMaxCapacity_{kUint_128U};
    /// @brief Re-formatted DN string
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05469
    /// @needwork = dda
    /// @endcode
    mutable ara::core::String stDnString_{};
    /// @brief Other attributes
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05470
    /// @needwork = dda
    /// @endcode
    mutable ara::core::Map< AttributeId, ara::core::Vector< ara::core::String > > mapAttribute_{};

public:
    /// @brief Constructor with parameters
    /// @param x509Provider X509 Certificate Provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02645
    /// @trace_id_dd=DD_CRYPTO_05471
    /// @needwork = ad
    /// @endcode
    explicit X509DN(X509Provider& x509Provider) noexcept;
    /// @brief Set maximum capacity
    /// @param nCapacity Maximum storage capacity threshold
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02646
    /// @trace_id_dd=DD_CRYPTO_05472
    /// @needwork = ad
    /// @endcode
    inline void SetMaxCapacity(uint32_t const nCapacity) noexcept { nMaxCapacity_ = nCapacity; }
    /// @brief Return maximum capacity
    /// @return Maximum capacity value
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02647
    /// @trace_id_dd=DD_CRYPTO_05473
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetmaxCapacity() const noexcept { return nMaxCapacity_; }
    /// @brief Get the number of attributes
    /// @return Number of attributes
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02648
    /// @trace_id_dd=DD_CRYPTO_05474
    /// @needwork = ad
    /// @endcode
    int32_t GetAttrCount() const noexcept;
    /// @brief Set the certificate name
    /// @param pX509Name Certificate name
    /// @return true if SetX509Name sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02649
    /// @trace_id_dd=DD_CRYPTO_05475
    /// @needwork = ad
    /// @endcode
    bool SetX509Name(X509_NAME const* const pX509Name) const noexcept;
    /// @brief Set DN information
    /// @param dn DN information
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return true if SetDn sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02650
    /// @trace_id_dd=DD_CRYPTO_05476
    /// @needwork = ad
    /// @endcode
    bool SetDn(ReadOnlyMemRegion const& dn, Serializable::FormatId const formatId) const noexcept;
    /// @brief Get attribute data information corresponding to the ID
    /// @param id Attribute ID
    /// @param nIndex Attribute index
    /// @return Attribute data information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02651
    /// @trace_id_dd=DD_CRYPTO_05477
    /// @needwork = ad
    /// @endcode
    ara::core::String* GetAttributeData(AttributeId const id, uint32_t const nIndex) const noexcept;

protected:
    /// @brief Convert AttributeId enumeration to ID used in Openssl
    /// @param kId Attribute ID
    /// @return ID used in Openssl
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05478
    /// @needwork = dda
    /// @endcode
    static int32_t TransNid(AttributeId const kId) noexcept;
    /// @brief Convert ID used in Openssl to AttributeId enumeration
    /// @param nId ID used in Openssl
    /// @return Attribute ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05479
    /// @needwork = dda
    /// @endcode
    static AttributeId TransKid(int32_t const nId) noexcept;
    /// @brief Convert AttributeId enumeration to string
    /// @param id Attribute ID
    /// @return Attribute ID string
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05480
    /// @needwork = dda
    /// @endcode
    static ara::core::String TransName(AttributeId const id) noexcept;
    /// @brief Check if the ID is supported
    /// @param id Attribute ID
    /// @return true if support this ID false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05481
    /// @needwork = dda
    /// @endcode
    static bool IsSupportAttributeId(AttributeId const id) noexcept;
    /// @brief // Format DN as string
    /// @param pfun Callback function operating on X509_NAME
    /// @return >0 if format sucess =0 failed
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05482
    /// @needwork = dda
    /// @endcode
    uint32_t _FormatDn(std::function< uint32_t(X509_NAME* pX509Name) > const& pfun) const noexcept;
    /// @brief Interpret string formatted DN: return value indicates whether stData has a format error
    /// @param pX509Name X509_NAME pointer
    /// @param stData DN data: string
    /// @return true if explain sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05483
    /// @needwork = dda
    /// @endcode
    static bool A2i_X509_NAME(X509_NAME* const pX509Name, ara::core::StringView const& stData) noexcept;
    /// @brief Get the string value of the attribute corresponding to the ID from pX509Name
    /// @param pX509Name X509_NAME pointer
    /// @param kID Attribute ID
    /// @return String value information of the corresponding attribute
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05484
    /// @needwork = dda
    /// @endcode
    static ara::core::String GetNameByID(X509_NAME* const pX509Name, AttributeId const kID) noexcept;
    /// @brief Get the string value of the attribute corresponding to the stack index from pX509Name
    /// @param pX509Name X509_NAME pointer
    /// @param nIndex Index
    /// @return String value information of the corresponding attribute
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05485
    /// @needwork = dda
    /// @endcode
    ara::core::String _GetNameByIndex(X509_NAME* pX509Name, int32_t nIndex) noexcept;

public:
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_X509_X509_DN_H_
