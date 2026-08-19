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
/// @file       x509_dn.cpp
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/DN Distinguished Name
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03002
/// @unit_name=X509DN
/// @unit_description=Certificate DN Information
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/x509_dn.h"

#include <openssl/evp.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief X.509 DN (Distinguished Name) interface.
//********************************/
/// @brief Publicly serialize itself.
/// @return
/// @param formatId Data format: Raw, DER, PEM, etc.
ara::core::Result< ara::core::Vector< ara::core::Byte > > X509DN::ExportPublicly(FormatId formatId) const noexcept
{
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }

    ara::core::Vector< ara::core::Byte > vecData;
    switch (formatId) {
        case kFormatDerEncoded: {
            std::function< uint32_t(X509_NAME * pX509Name) > const funcFormatDer{
                [&vecData](X509_NAME *const pX509Name) -> int32_t {
                    int32_t const nNeedLen{i2d_X509_NAME(pX509Name, nullptr)};
                    std::unique_ptr< uint8_t[] > const pBuffUptr{new uint8_t[static_cast< uint64_t >(nNeedLen)]};
                    uint8_t *pBuff{pBuffUptr.get()};

                    std::ignore = i2d_X509_NAME(pX509Name, &pBuff);  // pBuff address increases by nNeedLen length
                    pBuff -= nNeedLen;
                    vecData.reserve(static_cast< std::size_t >(nNeedLen));
                    for (int32_t i{0}; i < nNeedLen; i++) {
                        vecData.push_back(ara::core::Byte(*(pBuff + i)));
                    }
                    return static_cast< int32_t >(vecData.size());
                }};
            std::ignore = _FormatDn(funcFormatDer);
        } break;
        case kFormatPemEncoded: {
            std::function< uint32_t(X509_NAME * pX509Name) > const funcFormatPem{
                [&vecData](X509_NAME *const pX509Name) -> int32_t {
                    // PEM_ASN1_write_bio( i2d_X509_NAME, )
                    int32_t const nNeedLen{i2d_X509_NAME(pX509Name, nullptr)};
                    std::unique_ptr< uint8_t[] > const pBuffUptr{new uint8_t[static_cast< uint64_t >(nNeedLen)]};
                    uint8_t *pBuff{pBuffUptr.get()};

                    std::ignore = i2d_X509_NAME(pX509Name, &pBuff);
                    pBuff -= nNeedLen;
                    int32_t const nBase64Len{nNeedLen * 8 / 6 + 3};
                    uint8_t *const pBuffBase64{new uint8_t[static_cast< uint64_t >(nBase64Len)]};
                    int32_t const nLen{EVP_EncodeBlock((pBuffBase64), pBuff, nNeedLen)};
                    vecData.reserve(static_cast< std::size_t >(nBase64Len));
                    for (int32_t i{0}; i < nLen; i++) {
                        vecData.push_back(ara::core::Byte(*(pBuffBase64 + i)));
                    }
                    delete[] pBuffBase64;
                    return static_cast< int32_t >(vecData.size());
                }};
            std::ignore = _FormatDn(funcFormatPem);
        } break;
        default: {
        } break;
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecData));
}
//********************************/ //X509DN     interface
/// @brief Get DN attribute by ID (this method applies to all attributes except kOrgUnit and kDomainComponent).
///         The output string capacity must be sufficient to store the output value! If (attribute == nullptr), the method only returns the required buffer capacity.
/// @param id the identifier of required attribute
/// @return StringView of the attribute
ara::core::Result< ara::core::StringView > X509DN::GetAttribute(AttributeId id) const noexcept
{
    /// @error:  SecurityErrorDomain::kUnknownIdentifier  if the @c id argument has unsupported value
    if (false == IsSupportAttributeId(id)) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error:  SecurityErrorDomain::kInvalidArgument       if (id == kOrgUnit) || (id == kDomainComponent)
    if (((id == AttributeId::kOrgUnit)) || ((id == AttributeId::kDomainComponent))) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (attribute != nullptr), but @c attribute->capacity() is
    /// less than required for storing of the output
    ara::core::String *const pFindAttribute{GetAttributeData(id, 0U)};
    if (nullptr == pFindAttribute) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// Here only test if size memory can be allocated, no logic, return error if allocation fails
    ara::core::String strOut;
    try {
        strOut.reserve(static_cast< std::size_t >(pFindAttribute->size()));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return ara::core::Result< ara::core::StringView >::FromValue(*pFindAttribute);
}
/// @brief Return DN attribute by ID and order index (this method applies to attributes kOrgUnit and kDomainComponent).
///         The output string capacity must be sufficient to store the output value! If (attribute == nullptr), the method only returns the required buffer capacity.
/// @param id the identifier of required attribute
/// @param indexof the zero-based index of required component of the attribute
/// @return StringView of the attribute
ara::core::Result< ara::core::StringView > X509DN::GetAttribute(AttributeId id, uint32_t indexof) const noexcept
{
    /// @error:  SecurityErrorDomain::kUnknownIdentifier  if the @c id argument has unsupported value
    if (false == IsSupportAttributeId(id)) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error:  SecurityErrorDomain::kInvalidArgument       if (id == kOrgUnit) || (id == kDomainComponent)
    if ((id != AttributeId::kOrgUnit) && (id != AttributeId::kDomainComponent)) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error:  SecurityErrorDomain::kAboveBoundary         if ((id == kOrgUnit) || (id == kDomainComponent))
    ///                and the @c index value is greater than or equal to the actual number of components in the
    ///                specified attribute
    ara::core::String *const pFindAttribute{GetAttributeData(id, indexof)};
    if (nullptr == pFindAttribute) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kAboveBoundary);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (attribute != nullptr), but @c attribute->capacity() is
    /// less than required for storing of the output
    return ara::core::Result< ara::core::StringView >::FromValue(*pFindAttribute);
}
/// @brief Get the entire Distinguished Name (DN) as a single string. Output string capacity must be sufficient to store the output value! If (dn ==
/// nullptr), the method only returns the required buffer capacity.
/// @return StringView of the whole DN string
ara::core::Result< ara::core::StringView > X509DN::GetDnString() const noexcept
{
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if (dn != nullptr), but @c dn->capacity() is less than
    /// required for the output value storing
    std::ignore = _FormatDn([this](X509_NAME const *const pX509Name) -> int32_t {
        stDnString_ = X509_NAME_oneline(pX509Name, nullptr,
                                        0);  // 2022-06-14 hanjingjing test: Return value uses "/" as separator
        return static_cast< int32_t >(stDnString_.size());
    });
    /// Here only test if size memory can be allocated, no logic, return error if allocation fails
    ara::core::String strOut;
    try {
        strOut.reserve(static_cast< std::size_t >(stDnString_.size()));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::StringView >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return ara::core::Result< ara::core::StringView >::FromValue(stDnString_);
}
/// @brief Check if this object and another Distinguished Name (DN) object are equal.
/// @param other Another instance of this class for comparison
/// @return @c true if the provided DN is identical to this one and @c false otherwise
bool X509DN::operator==(X509DN const &other) const noexcept
{
    if (mapAttribute_.size() != other.mapAttribute_.size()) {
        return false;
    }
    for (auto &itVec : mapAttribute_) {
        for (uint32_t i{0U}; i < itVec.second.size(); i++) {
            ara::core::String *const pFindOther{other.GetAttributeData(itVec.first, i)};
            if (nullptr == pFindOther) {
                return false;
            }

            if (itVec.second.at(static_cast< std::size_t >(i)) != *pFindOther) {
                return false;
            }
        }
    }
    return true;
}
/// @brief Set DN attribute by ID (this method applies to all attributes except kOrgUnit and kDomainComponent).
/// @param id the identifier of required attributet
/// @param attribute the attribute value
/// @return ara::core::Result< void >
ara::core::Result< void > X509DN::SetAttribute(AttributeId id, ara::core::StringView const &attribute) const noexcept
{
    /// @error:  SecurityErrorDomain::kUnknownIdentifier  if the @c id argument has unsupported value
    if (false == IsSupportAttributeId(id)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error:  SecurityErrorDomain::kInvalidArgument       if (id == kOrgUnit) || (id == kDomainComponent)
    if ((id == AttributeId::kOrgUnit) || (id == AttributeId::kDomainComponent)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error:  SecurityErrorDomain::kUnexpectedValue   if the attribute string contains incorrect characters or it has
    /// unsupported length
    ara::core::Vector< ara::core::String > &vecData{mapAttribute_[id]};
    vecData.clear();
    vecData.push_back(ara::core::String(attribute));
    return ara::core::Result< void >::FromValue();
}
/// @brief Set DN attribute by DN ID and order index (applies to attributes kOrgUnit and kDomainComponent).
/// @brief Set DN attribute by its ID and sequential index (this method is applicale to attributes @c kOrgUnit and @c
/// kDomainComponent).
/// @param id  the identifier of required attribute
/// @param nIndex  the zero-based index of required component of the attribute
/// @param attribute  the attribute value
/// @return true if SetAttribute sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_40416}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error:  SecurityErrorDomain::kUnknownIdentifier if the @c id argument has unsupported value
/// @error:  SecurityErrorDomain::kUnexpectedValue   if the attribute string contains incorrect characters or it has
/// unsupported length
/// @error:  SecurityErrorDomain::kInvalidArgument   if (id != kOrgUnit) && (id != kDomainComponent) && (index > 0)
/// @error:  SecurityErrorDomain::kAboveBoundary     if ((id == kOrgUnit) || (id == kDomainComponent)) and the @c index
/// value is greater than the current number of components in the specified attribute
/// @threadsafety={Thread-safe}
ara::core::Result< void > X509DN::SetAttribute(AttributeId id,
                                               uint32_t nIndex,
                                               ara::core::StringView const &attribute) const noexcept
{
    /// @error:  SecurityErrorDomain::kUnknownIdentifier  if the @c id argument has unsupported value
    if (false == IsSupportAttributeId(id)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error:  SecurityErrorDomain::kUnexpectedValue   if the attribute string contains incorrect characters or it has
    /// unsupported length
    /// @error:  SecurityErrorDomain::kInvalidArgument       if (id == kOrgUnit) || (id == kDomainComponent)
    if ((id != AttributeId::kOrgUnit) && (id != AttributeId::kDomainComponent)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error:  SecurityErrorDomain::kAboveBoundary     if ((id == kOrgUnit) || (id == kDomainComponent)) and the @c
    /// index value is greater than the current number of components in the specified attribute
    ara::core::Vector< ara::core::String > &vecData{mapAttribute_[id]};
    if (nIndex > vecData.size()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAboveBoundary);
    }
    if (nIndex == 0U) {
        vecData.push_back(ara::core::String(attribute));
    } else {
        vecData[static_cast< std::size_t >(nIndex)] = attribute;
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Set the entire DN (Distinguished Name) from a single string. [Error]:
/// SecurityErrorDomain::kUnexpectedValue if dn string has incorrect syntax.
/// @param dn dn string
/// @return true if SetDn sucess false otherwise
ara::core::Result< void > X509DN::SetDn(ara::core::StringView const &dn) noexcept
{
    ///  SecurityErrorDomain::kUnexpectedValue  if the @c dn string has incorrect syntax
    X509_NAME *const pX509Name{X509_NAME_new()};
    bool const bSuccess{A2i_X509_NAME(pX509Name, dn)};
    if (bSuccess) {
        std::ignore = SetX509Name(pX509Name);
    }
    X509_NAME_free(pX509Name);
    if (false == bSuccess) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
namespace {
/// @brief String conversion
/// @param pString ASN.1 string
/// @return String
ara::core::String Asn1ToString(ASN1_OCTET_STRING const *const pString) noexcept
{
    uint8_t *pData{nullptr};
    int32_t const nLen{ASN1_STRING_to_UTF8(&pData, pString)};
    ara::core::String stData(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pData)),
                             static_cast< std::size_t >(nLen));
    OPENSSL_free(pData);
    return stData;
}
}  // namespace
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider object
/// @throws
X509DN::X509DN(X509Provider &x509Provider) noexcept : X509Object{x509Provider} {}
/// @brief Get attribute count
/// @return Number of attributes
int32_t X509DN::GetAttrCount() const noexcept
{
    int32_t nCount{0};
    for (auto const &it : mapAttribute_) {
        AttributeId const id{it.first};
        if ((id != AttributeId::kOrgUnit) && (id != AttributeId::kDomainComponent)) {
            if (false == it.second.empty()) {
                nCount += 1;
            }
        } else {
            nCount += static_cast< int32_t >(it.second.size());
        }
    }
    return nCount;
}
/// @brief Set X509name
/// @param pX509Name x509name
/// @return true if SetX509Name sucess false otherwise
bool X509DN::SetX509Name(X509_NAME const *const pX509Name) const noexcept
{
    if (nullptr == pX509Name) {
        return false;
    }
    mapAttribute_.clear();
    int32_t const nCount{X509_NAME_entry_count(pX509Name)};
    for (int32_t i{0}; i < nCount; i++) {
        X509_NAME_ENTRY *const pEntryName{X509_NAME_get_entry(pX509Name, i)};
        ASN1_STRING *const pString{X509_NAME_ENTRY_get_data(pEntryName)};
        ASN1_OBJECT *const pObject{X509_NAME_ENTRY_get_object(pEntryName)};
        int32_t const nId{OBJ_obj2nid(pObject)};
        X509DN::AttributeId const kId{TransKid(nId)};
        ara::core::String const stData{Asn1ToString(pString)};
        ara::core::Vector< ara::core::String > &vecData{mapAttribute_[kId]};
        vecData.push_back(stData);
    }
    return true;
}
/// @brief Set dn
/// @param dn DN information
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return true if SetDn sucess false otherwise
bool X509DN::SetDn(ReadOnlyMemRegion const &dn, Serializable::FormatId const formatId) const noexcept
{
    if (((Serializable::kFormatDefault == formatId) || (Serializable::kFormatDerEncoded == formatId))) {
        uint8_t const *pData{dn.data()};
        X509_NAME *const pX509Name{d2i_X509_NAME(nullptr, &pData, static_cast< int64_t >(dn.size()))};
        if (nullptr == pX509Name) {
            return false;
        }
        return SetX509Name(pX509Name);
    }
    if (((Serializable::kFormatDefault == formatId) || (Serializable::kFormatPemEncoded == formatId))) {
        int32_t const nNeedLen{static_cast< int32_t >(dn.size())};
        uint8_t *const pBuff{new uint8_t[static_cast< size_t >(nNeedLen)]};
        int32_t const nLen{EVP_DecodeBlock(pBuff,
                                           static_cast< uint8_t const * >(static_cast< void const * >(dn.data())),
                                           static_cast< int32_t >(dn.size()))};
        uint8_t const *pWorkBuf{pBuff};

        X509_NAME *const pX509Name{d2i_X509_NAME(nullptr, &pWorkBuf, static_cast< int64_t >(nLen))};
        delete[] pBuff;
        if (nullptr == pX509Name) {
            return false;
        }
        return SetX509Name(pX509Name);
    }
    return false;
}
/// @brief Get attribute data
/// @param id Attribute ID
/// @param nIndex Attribute index
/// @return Attribute data information
ara::core::String *X509DN::GetAttributeData(AttributeId const id, uint32_t const nIndex) const noexcept
{
    ara::core::Map< AttributeId, ara::core::Vector< ara::core::String > >::iterator const itFind{
        mapAttribute_.find(id)};
    if (nIndex >= itFind->second.size()) {
        return nullptr;
    }
    return &(itFind->second.at(static_cast< std::size_t >(nIndex)));
}
/// @brief Convert AttributeId enum to ID used in Openssl
/// @param kId Attribute ID
/// @return ID used in Openssl
int32_t X509DN::TransNid(AttributeId const kId) noexcept
{
    int32_t nID{0};
    switch (kId) {
        case AttributeId::kCommonName: {
            nID = NID_commonName;
        } break;  // Common Name
        case AttributeId::kCountry: {
            nID = NID_countryName;
        } break;  // Country
        case AttributeId::kState: {
            nID = NID_stateOrProvinceName;
        } break;  // State/Province
        case AttributeId::kLocality: {
            nID = NID_localityName;
        } break;  // Locality
        case AttributeId::kOrganization: {
            nID = NID_organizationName;
        } break;  // Organization Name
        case AttributeId::kOrgUnit: {
            nID = NID_organizationalUnitName;
        } break;  // Organizational Unit
        case AttributeId::kStreet: {
            nID = NID_streetAddress;
        } break;  // Street
        case AttributeId::kPostalCode: {
            nID = NID_postalCode;
        } break;  // Postal Code
        case AttributeId::kTitle: {
            nID = NID_title;
        } break;  // Title
        case AttributeId::kSurname: {
            nID = NID_surname;
        } break;  // Surname
        case AttributeId::kGivenName: {
            nID = NID_givenName;
        } break;  // Given Name
        case AttributeId::kInitials: {
            nID = NID_initials;
        } break;  // Initials
        case AttributeId::kPseudonym: {
            nID = NID_pseudonym;
        } break;  // Pseudonym
        case AttributeId::kGenerationQualifier: {
            nID = NID_generationQualifier;
        } break;  // Generation Qualifier
        case AttributeId::kDomainComponent: {
            nID = NID_domainComponent;
        } break;  // Domain Component
        case AttributeId::kDnQualifier: {
            nID = NID_dnQualifier;
        } break;  // DN Qualifier
        case AttributeId::kEmail: {
            nID = NID_pkcs9_emailAddress;
        } break;  // NID_Mail
        case AttributeId::kUri: {
            nID = NID_id_smime_spq_ets_sqt_uri;
        } break;  // Uniform Resource Identifier
        case AttributeId::kDns: {
            nID = NID_dnsName;
        } break;  // Domain Name
        case AttributeId::kHostName: {
            nID = NID_host;
        } break;  // Hostname: Doubtful //NID_pkcs9_unstructuredName
        case AttributeId::kIpAddress: {
            nID = NID_pkcs9_unstructuredAddress;
        } break;  // IP Address
        case AttributeId::kSerialNumbers: {
            nID = NID_serialNumber;
        } break;  // Serial Number
        case AttributeId::kUserId: {
            nID = NID_userId;
        } break;  // User ID
        default: {
            nID = NID_undef;
        } break;
    }
    return nID;
}
/// @brief Convert ID used in Openssl to AttributeId enum
/// @param nId Attribute ID
/// @return Attribute ID
X509DN::AttributeId X509DN::TransKid(int32_t const nId) noexcept
{
    AttributeId kId;
    switch (nId) {
        case NID_commonName: {
            kId = AttributeId::kCommonName;
        } break;  // Common Name
        case NID_countryName: {
            kId = AttributeId::kCountry;
        } break;  // Country
        case NID_stateOrProvinceName: {
            kId = AttributeId::kState;
        } break;  // State/Province
        case NID_localityName: {
            kId = AttributeId::kLocality;
        } break;  // Locality
        case NID_organizationName: {
            kId = AttributeId::kOrganization;
        } break;  // Organization Name
        case NID_organizationalUnitName: {
            kId = AttributeId::kOrgUnit;
        } break;  // Organizational Unit
        case NID_streetAddress: {
            kId = AttributeId::kStreet;
        } break;  // Street
        case NID_postalCode: {
            kId = AttributeId::kPostalCode;
        } break;  // Postal Code
        case NID_title: {
            kId = AttributeId::kTitle;
        } break;  // Title
        case NID_surname: {
            kId = AttributeId::kSurname;
        } break;  // Surname
        case NID_givenName: {
            kId = AttributeId::kGivenName;
        } break;  // Given Name
        case NID_initials: {
            kId = AttributeId::kInitials;
        } break;  // Initials
        case NID_pseudonym: {
            kId = AttributeId::kPseudonym;
        } break;  // Pseudonym
        case NID_generationQualifier: {
            kId = AttributeId::kGenerationQualifier;
        } break;  // Generation Qualifier
        case NID_domainComponent: {
            kId = AttributeId::kDomainComponent;
        } break;  // Domain Component
        case NID_dnQualifier: {
            kId = AttributeId::kDnQualifier;
        } break;  // DN Qualifier
        case NID_pkcs9_emailAddress: {
            kId = AttributeId::kEmail;
        } break;  // NID_Mail
        case NID_id_smime_spq_ets_sqt_uri: {
            kId = AttributeId::kUri;
        } break;  // Uniform Resource Identifier
        case NID_dnsName: {
            kId = AttributeId::kDns;
        } break;  // Domain Name
        case NID_host: {
            kId = AttributeId::kHostName;
        } break;  // Hostname: Doubtful //NID_pkcs9_unstructuredName
        case NID_pkcs9_unstructuredAddress: {
            kId = AttributeId::kIpAddress;
        } break;  // IP Address
        case NID_serialNumber: {
            kId = AttributeId::kSerialNumbers;
        } break;  // Serial Number
        case NID_userId: {
            kId = AttributeId::kUserId;
        } break;  // User ID
        default: {
            kId = AttributeId::kUnSupport;
        } break;
    }
    return kId;
}
/// @brief Convert AttributeId enum to string
/// @param id Attribute ID
/// @return Attribute ID string
ara::core::String X509DN::TransName(AttributeId const id) noexcept
{
    ara::core::String stName;
    switch (id) {
        case AttributeId::kCommonName: {
            stName = SN_commonName;
        } break;  // Common Name
        case AttributeId::kCountry: {
            stName = SN_countryName;
        } break;  // Country
        case AttributeId::kState: {
            stName = SN_stateOrProvinceName;
        } break;  // State/Province
        case AttributeId::kLocality: {
            stName = SN_localityName;
        } break;  // Locality
        case AttributeId::kOrganization: {
            stName = SN_organizationName;
        } break;  // Organization Name
        case AttributeId::kOrgUnit: {
            stName = SN_organizationalUnitName;
        } break;  // Organizational Unit
        case AttributeId::kStreet: {
            stName = SN_streetAddress;
        } break;  // Street
        case AttributeId::kPostalCode: {
            stName = LN_postalCode;
        } break;  // Postal Code
        case AttributeId::kTitle: {
            stName = SN_title;
        } break;  // Title
        case AttributeId::kSurname: {
            stName = SN_surname;
        } break;  // Surname
        case AttributeId::kGivenName: {
            stName = SN_givenName;
        } break;  // Given Name
        case AttributeId::kInitials: {
            stName = SN_initials;
        } break;  // Initials
        case AttributeId::kPseudonym: {
            stName = LN_pseudonym;
        } break;  // Pseudonym
        case AttributeId::kGenerationQualifier: {
            stName = LN_generationQualifier;
        } break;  // Generation Qualifier
        case AttributeId::kDomainComponent: {
            stName = SN_domainComponent;
        } break;  // Domain Component
        case AttributeId::kDnQualifier: {
            stName = SN_dnQualifier;
        } break;  // DN Qualifier
        case AttributeId::kEmail: {
            stName = LN_pkcs9_emailAddress;
        } break;
        case AttributeId::kUri: {
            stName = SN_id_smime_spq_ets_sqt_uri;
        } break;  // Uniform Resource Identifier
        case AttributeId::kDns: {
            stName = LN_dnsName;
        } break;  // Domain Name
        case AttributeId::kHostName: {
            stName = SN_host;
        } break;  // Hostname: Doubtful
        case AttributeId::kIpAddress: {
            stName = LN_pkcs9_unstructuredAddress;
        } break;  // IP Address
        case AttributeId::kSerialNumbers: {
            stName = LN_serialNumber;
        } break;  // Serial Number
        case AttributeId::kUserId: {
            stName = SN_userId;
        } break;  // User ID
        default: {
            stName = SN_undef;
        } break;
    }
    return stName;
}
/// @brief Check if ID is supported
/// @param id Attribute ID
/// @return true if support this ID false otherwise
bool X509DN::IsSupportAttributeId(AttributeId const id) noexcept
{
    return (id >= AttributeId::kCommonName) && (id < AttributeId::kMaxSupport);
}
/// @brief Format DN as string
/// @param pfun Callback function
/// @return >0 if format sucess =0 failed
uint32_t X509DN::_FormatDn(std::function< uint32_t(X509_NAME *pX509Name) > const &pfun) const noexcept
{
    X509_NAME *const pX509Name{X509_NAME_new()};
    int32_t nCount{0};
    for (auto &it : mapAttribute_) {
        if (it.second.empty()) {
            continue;
        }
        AttributeId const kId{it.first};
        int32_t const nId{TransNid(kId)};
        ara::core::String const stKey{OBJ_nid2sn(nId)};

        ara::core::Vector< ara::core::String > &vecData{it.second};

        for (auto &stData : vecData) {
            int32_t const nCode{
                X509_NAME_add_entry_by_txt(pX509Name, stKey.data(), MBSTRING_ASC,  // NOLINT
                                           static_cast< uint8_t const * >(static_cast< void const * >(stData.data())),
                                           static_cast< int32_t >(stData.size()), -1, 0)};
            if (nCode > 0) {
                nCount += 1;
            }
        }
    }
    if (nCount > 0) {
    }  // for qac
    int32_t const nReturn{static_cast< int32_t >(pfun(pX509Name))};
    X509_NAME_free(pX509Name);
    return static_cast< uint32_t >(nReturn);
}
/// @brief Interpret DN in string format: Return value indicates if stData has format errors
/// @param pX509Name x509name
/// @param stData DN data: String
/// @return true if explain sucess false otherwise
bool X509DN::A2i_X509_NAME(X509_NAME *const pX509Name, ara::core::StringView const &stData) noexcept
{
    /// @brief ara::core::StringView declaration
    using MStringView = ara::core::StringView;
    MStringView::size_type nPosFind{0U};
    while (true) {
        if ((nPosFind > stData.size()) || (MStringView::npos == nPosFind)) {
            break;
        }
        MStringView::size_type nPosNew{stData.find_first_of(",/", nPosFind)};
        if (MStringView::npos == nPosNew) {
            nPosNew = stData.size();
        }
        MStringView const stAttribute{stData.data() + nPosFind, nPosNew - nPosFind};
        MStringView::size_type const nPosEqual{stAttribute.find_first_of("=")};
        if (MStringView::npos != nPosEqual) {
            MStringView::size_type const nPosStart{stAttribute.find_first_not_of("=/, \t")};
            ara::core::String const stKey(stAttribute.data() + nPosStart, nPosEqual - nPosStart);
            ara::core::String const stValue(stAttribute.data() + nPosEqual + 1U, stAttribute.size() - nPosEqual - 1U);
            int32_t const nCode{
                X509_NAME_add_entry_by_txt(pX509Name, stKey.data(), MBSTRING_ASC,  // NOLINT
                                           static_cast< uint8_t const * >(static_cast< void const * >(stValue.data())),
                                           static_cast< int32_t >(stValue.size()), -1, 0)};
            if (0 == nCode) {
                ara::crypto::isoft_def::LogError() << "ERROR: X509_NAME_add_entry_by_txt";
                return false;
            }
        }
        nPosFind = nPosNew + 1U;
    }
    return true;
}
/// @brief Get string value of attribute corresponding to ID from pX509Name
/// @param pX509Name x509 name
/// @param kID Attribute ID
/// @return String value information of corresponding attribute
ara::core::String X509DN::GetNameByID(X509_NAME *const pX509Name, AttributeId const kID) noexcept
{
    int32_t const nID{TransNid(kID)};
    ara::core::Vector< char8_t > vecChBuff;
    vecChBuff.resize(kInt_1024U);

    int32_t const nNameLen{
        X509_NAME_get_text_by_NID(pX509Name, nID, static_cast< char8_t * >(vecChBuff.data()), kInt_1024)};
    if (-1 == nNameLen) {
        return ara::core::String{};
    }
    return ara::core::String(static_cast< char8_t * >(vecChBuff.data()));
}
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
