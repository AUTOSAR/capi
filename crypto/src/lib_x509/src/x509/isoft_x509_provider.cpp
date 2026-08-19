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
/// @file       isoft_x509_provider.cpp
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/Certificate Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_04001
/// @unit_name=PX509Provider
/// @unit_description=X.509 Provider
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_x509_provider.h"

#include <memory>

#include "ara/core/span.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private.h"
#include "ara/crypto/cryp/signature_service.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/internal/initialize.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_deal_x509.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/manifest/manifest_instance.h"
#include "ara/crypto/x509/isoft_cert_sign_request.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/isoft_ocsp_request.h"
#include "ara/crypto/x509/isoft_ocsp_response.h"
#include "ara/crypto/x509/x509_dn.h"
#include "ara/crypto/x509/x509_extensions.h"
#include "openssl/ossl_typ.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {

/// @brief Generate a certificate object using memory data
/// @param x509Provider Certificate provider
/// @param stCertName Certificate name
/// @param vecData Memory data
/// @return Returns a smart pointer to the certificate object
ara::core::Result< x509::Certificate::Uptr > PX509Provider::MakeCertificate(
    x509::X509Provider &x509Provider,
    ara::core::StringView const &stCertName,
    ara::core::Vector< uint8_t > const &vecData) noexcept
{
    // Create unique pointer
    x509::isoft_def::PCertificate::Uptr pCertficate{std::make_unique< x509::isoft_def::PCertificate >(x509Provider)};
    if (false == vecData.empty()) {  // Indicates non-empty
        ReadOnlyMemRegion const memCert{vecData.data(), vecData.size()};
        if (false == pCertficate->LoadCertFile(memCert, x509::Certificate::kFormatDefault)) {
            return ara::core::Result< x509::Certificate::Uptr >::FromError(
                SecurityErrorDomain::Errc::kUnreservedResource);
        }
        pCertficate->SetCertSlotName(stCertName);
        return ara::core::Result< x509::Certificate::Uptr >::FromValue(std::move(pCertficate));
    }
    return ara::core::Result< x509::Certificate::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
}

/// @brief Generate a certificate signing request object using memory data
/// @param x509Provider Certificate provider
/// @param stCsrName     Certificate signing request name
/// @param vecData Certificate signing request data
/// @return Certificate signing request instance
ara::core::Result< x509::CertSignRequest::Uptr > PX509Provider::MakeCertSignRequest(
    x509::X509Provider &x509Provider,
    ara::core::StringView const &stCsrName,
    ara::core::Vector< uint8_t > const &vecData) noexcept
{
    x509::isoft_def::PCertSignRequest::Uptr pCsr{std::make_unique< x509::isoft_def::PCertSignRequest >(x509Provider)};
    if (false == vecData.empty())  // Indicates non-empty
    {
        ReadOnlyMemRegion const memCert{vecData.data(), vecData.size()};
        if (false == pCsr->LoadCertFile(memCert, x509::Certificate::kFormatDefault)) {
            return ara::core::Result< x509::CertSignRequest::Uptr >::FromError(
                SecurityErrorDomain::Errc::kUnreservedResource);
        }
        pCsr->SetStorageFile(stCsrName);
        return ara::core::Result< x509::CertSignRequest::Uptr >::FromValue(std::move(pCsr));
    }
    return ara::core::Result< x509::CertSignRequest::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
}
//********************************/
/// @brief Create an empty X.500 DN (Distinguished Name) structure. If (0 == capacity), the maximum supported capacity must be reserved (by the implementation).
/// @param capacity number of bytes that should be reserved for the content of the target @c X509DN object
/// @return Unique smart pointer to created empty @c X509DN object
ara::core::Result< X509DN::Uptr > PX509Provider::CreateEmptyDn(std::size_t capacity) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< X509DN::Uptr >);
    X509DN::Uptr pX509Dn{std::make_unique< X509DN >(*this)};
    pX509Dn->SetMaxCapacity(static_cast< uint32_t >(capacity));
    return ara::core::Result< X509DN::Uptr >::FromValue(std::move(pX509Dn));
}
/// @brief Create a complete X.500 Distinguished Name structure based on the provided string representation.
/// @param dn string representation of the Distinguished Name
/// @return unique smart pointer for the created @c X509DN object
ara::core::Result< X509DN::Uptrc > PX509Provider::BuildDn(ara::core::StringView const &dn) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< X509DN::Uptrc >);
    X509DN::Uptr pX509Dn{std::make_unique< X509DN >(*this)};
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the @c dn argument has unsupported length (too large)
    if (dn.size()
        > pX509Dn
              ->GetmaxCapacity())  // 2022-06-08 hanjingjing assumes 2K is the max DN string size, details need document review
    {
        return ara::core::Result< X509DN::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c dn argument has incorrect format
    ara::core::Result< void > const result{pX509Dn->SetDn(dn)};
    if (false == result.HasValue()) {
        return ara::core::Result< X509DN::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< X509DN::Uptrc >::FromValue(std::move(pX509Dn));
}
/// @brief Decode X.500 Distinguished Name structure from provided serialized format.
/// @param dn DER/PEM-encoded representation of the Distinguished Name
/// @param formatId input format identifier @c kFormatDefault means auto-detect
/// @return unique smart pointer for the created @c X509DN object
ara::core::Result< X509DN::Uptrc > PX509Provider::DecodeDn(ReadOnlyMemRegion const &dn,
                                                           Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< X509DN::Uptrc >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    if (false == PCertificate::IsValidFormatID(formatId)) {
        return ara::core::Result< X509DN::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    X509DN::Uptr pX509Dn{std::make_unique< X509DN >(*this)};
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c dn argument cannot be parsed
    if (false == pX509Dn->SetDn(dn, formatId)) {
        return ara::core::Result< X509DN::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< X509DN::Uptrc >::FromValue(std::move(pX509Dn));
}
//***************/
/// @brief Parse the serialized representation of a certificate and create its instance. Offline verification of the parsed certificate can be done by calling VerifyCertByCrl().
///         After verification, the certificate can be imported into session or persistent storage for subsequent search and use.
///         If the parsed certificate is not imported, it will be lost after the returned instance is destroyed! Only imported certificates can be found via search and apply for automatic verification!
/// @param cert DER/PEM-encoded certificate
/// @param formatId input format identifier @c kFormatDefault means auto-detect
/// @return unique smart pointer to created certificate
ara::core::Result< Certificate::Uptr > PX509Provider::ParseCert(ReadOnlyMemRegion const &cert,
                                                                Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Certificate::Uptr >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    if (false == PCertificate::IsValidFormatID(formatId)) {
        return ara::core::Result< Certificate::Uptr >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c cert argument cannot be parsed
    PCertificate::Uptr pCertficate{std::make_unique< PCertificate >(*this)};
    if (pCertficate != nullptr) {
        if (false == pCertficate->LoadCertFile(cert, formatId)) {
            return ara::core::Result< Certificate::Uptr >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
    }
    return ara::core::Result< Certificate::Uptr >::FromValue(std::move(pCertficate));
}
/// @brief Count the number of certificates in a serialized certificate chain represented by a single BLOB.
/// @param certChain DER/PEM-encoded certificate chain in form of a single BLOB
/// @param formatId input format identifier @c kFormatDefault means auto-detect
/// @return number of certificates in the chain
ara::core::Result< std::size_t > PX509Provider::CountCertsInChain(ReadOnlyMemRegion const &certChain,
                                                                  Serializable::FormatId formatId) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< std::size_t >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    if (false == PCertificate::IsValidFormatID(formatId)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    BIO *const pBio{BIO_new_mem_buf(certChain.data(), static_cast< int32_t >(certChain.size()))};
    int32_t nCertCount{static_cast< int32_t >(PCertificate::CalculateCertCount(pBio, formatId))};
    std::ignore = BIO_free(pBio);
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c certChain argument cannot be pre-parsed
    if (nCertCount <= 0) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< std::size_t >::FromValue(nCertCount);
    if (nCertCount == 0) {
    }
}
/// @brief 2011 Standard Interface
/// @brief Parse the serialized representation of a certificate chain and create their instances. Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
///         After verification, certificates can be saved to session or persistent storage for subsequent search and use.
///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found via search and apply for automatic verification!
///         Certificates in the result vector will be placed from the Root CA certificate (index zero) to the final end-entity certificate (last used index of the vector).
/// @param outcome  an output vector for imported certificates
/// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
/// @param formatId  input format identifier @c kFormatDefault means auto-detect
/// @return has value if parase certchain sucess false otherwise
ara::core::Result< void > PX509Provider::ParseCertChain(ara::core::Vector< Certificate::Uptr > &outcome,
                                                        ReadOnlyMemRegion const &certChain,
                                                        Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    ara::core::Result< ara::core::Vector< Certificate::Uptr > > resVecCertPtr{ParseCertChain(certChain, formatId)};
    if (resVecCertPtr.HasValue() == false) {
        return ara::core::Result< void >::FromError(resVecCertPtr.Error());
    }
    outcome.clear();
    outcome = std::move(resVecCertPtr).Value();

    return ara::core::Result< void >::FromValue();
}
/// @brief 2311 Standard Interface
/// @brief Parse the serialized representation of a certificate chain and create their instances. Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
///         After verification, certificates can be saved to session or persistent storage for subsequent search and use.
///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found via search and apply for automatic verification!
///         Certificates in the result vector will be placed from the Root CA certificate (index zero) to the final end-entity certificate (last used index of the vector).
/// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
/// @param formatId  input format identifier @c kFormatDefault means auto-detect
/// @return an output vector for imported certificates
ara::core::Result< ara::core::Vector< Certificate::Uptr > > PX509Provider::ParseCertChain(
    ReadOnlyMemRegion const &certChain, Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< Certificate::Uptr > >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    if (false == PCertificate::IsValidFormatID(formatId)) {
        return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    BIO *const pBio{BIO_new_mem_buf(certChain.data(), static_cast< int32_t >(certChain.size()))};
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of @c outcome vector is less than actual
    /// number of certificates in the chain
    ara::core::Result< ara::core::Vector< PCertificate::Uptr > > resCert{
        PCertificate::LoadCertP7b(*this, pBio, formatId)};
    if (!resCert.HasValue()) {
        return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resCert.Error().Value()));
    }
    ara::core::Vector< Certificate::Uptr > outcome{};
    ara::core::Vector< PCertificate::Uptr > vecCert{std::move(resCert).Value()};
    for (auto &itData : vecCert) {
        outcome.push_back(std::move(itData));
    }
    std::ignore = BIO_free(pBio);
    /// @error: SecurityErrorDomain::kInvalidArgument        if the @c certChain argument cannot be parsed
    if (outcome.empty()) {
        return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromValue(std::move(outcome));
}
/// @brief 2011 Standard Interface
/// @brief Parse the serialized representation of a certificate chain and create their instances. Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
///         After verification, certificates can be imported into session or persistent storage for subsequent search and use. The capacity of the result vector must equal the size of the certChain vector.
///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found via search and apply for automatic verification!
///         Certificates in the result vector will be placed from the Root CA certificate (index zero) to the final end-entity certificate (last used index of the vector).
/// @param outcome an output vector for imported certificates
/// @param certChain DER/PEM-encoded certificate chain in form of a single BLOB
/// @param formatId input format identifier @c kFormatDefault means auto-detect
/// @return has value if parase certchain sucess false otherwise
ara::core::Result< void > PX509Provider::ParseCertChain(ara::core::Vector< Certificate::Uptr > &outcome,
                                                        ara::core::Vector< ReadOnlyMemRegion > const &certChain,
                                                        Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);

    ara::core::Result< ara::core::Vector< Certificate::Uptr > > resVecCertPtr{ParseCertChain(certChain, formatId)};
    if (resVecCertPtr.HasValue() == false) {
        return ara::core::Result< void >::FromError(resVecCertPtr.Error());
    }
    outcome.clear();
    outcome = std::move(resVecCertPtr).Value();

    return ara::core::Result< void >::FromValue();
}
/// @brief 2311 Standard Interface
/// @brief Parse the serialized representation of a certificate chain and create their instances. Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
///         After verification, certificates can be imported into session or persistent storage for subsequent search and use. The capacity of the result vector must equal the size of the certChain vector.
///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found via search and apply for automatic verification!
///         Certificates in the result vector will be placed from the Root CA certificate (index zero) to the final end-entity certificate (last used index of the vector).
/// @param certChain DER/PEM-encoded certificate chain in form of a single BLOB
/// @param formatId input format identifier @c kFormatDefault means auto-detect
/// @return an output vector for imported certificates
ara::core::Result< ara::core::Vector< Certificate::Uptr > > PX509Provider::ParseCertChain(
    ara::core::Vector< ReadOnlyMemRegion > const &certChain, Serializable::FormatId formatId) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< Certificate::Uptr > >);
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    if (false == PCertificate::IsValidFormatID(formatId)) {
        return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    ara::core::Vector< Certificate::Uptr > outcome{};
    /// @error: SecurityErrorDomain::kInvalidArgument        if an element of @c certChain argument cannot be parsed
    for (auto const &itData : certChain) {
        PCertificate::Uptr pCertficate{std::make_unique< PCertificate >(*this)};
        if (pCertficate != nullptr) {
            if (false == pCertficate->LoadCertFile(itData, formatId)) {
                return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
                    SecurityErrorDomain::Errc::kInvalidArgument);
            }
        }
        outcome.push_back(std::move(pCertficate));
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c outcome vector is less than number of
    /// elements in the @c certChain
    if (outcome.size() < certChain.size()) {
        return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return ara::core::Result< ara::core::Vector< Certificate::Uptr > >::FromValue(std::move(outcome));
}
/// @brief 2011 Standard Interface
/// @brief Verify the status of the provided certificate only through locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
/// @param cert target certificate for verification
/// @param myRoot root certificate to be used for verification - if this is nullptr, use machine root
/// certificates
/// @return verification status of the provided certificate
Certificate::Status PX509Provider::VerifyCert(Certificate &cert, Certificate::Uptr myRoot) noexcept
{
    // Signal certificate revocation via application or functional cluster
    return VerifyCert(cert, *(myRoot.get()));
}
/// @brief 2311 Standard Interface
/// @brief Verify the status of the provided certificate only through locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
/// @param cert target certificate for verification
/// @param myRoot root certificate to be used for verification - if this is nullptr, use machine root
/// certificates
/// @return verification status of the provided certificate
Certificate::Status PX509Provider::VerifyCert(Certificate const &cert, Certificate const &myRoot) noexcept
{
    if (false == internal::IsCryptoInitialize()) {
        return Certificate::Status::kInvalid;
    }

    // X.509 Provider shall verify if the certificate is valid SWS_CRYPT_20905
    ara::crypto::x509::Certificate::Status certStatus{PCertificate::VerifyCert(cert, &myRoot)};

    // Preliminary idea:
    // Check if the certificate is revoked
    // Export the current certificate's serial number, check if it exists in the CRL. If yes, the certificate is revoked, verifyCert fails.
    ara::core::Result< bool > const checkResult{_CheckCertRevoked(cert)};
    if (checkResult.HasValue() == false) {
        return ara::crypto::x509::Certificate::Status::kUnknown;
    }
    if (checkResult.Value() == true) {
        // SWS_CRYPT_20910 X509 Certificate Revocation: Internal signal for certificate revocation
        cert.SetCertStatus(Certificate::Status::kRevoked);
        certStatus = Certificate::Status::kRevoked;
    }
    // Signal certificate revocation via application or functional cluster
    return certStatus;
}
/// @brief Verify certificate chain
/// @param chain target certificate for verification
/// @param myRoot root certificate to be used for verification - if this is nullptr, use machine root
/// certificates
/// @return  verification status of the provided certificate
Certificate::Status PX509Provider::VerifyCertChain2(
    ara::core::Span< std::reference_wrapper< Certificate const > > chain, Certificate const *const myRoot) noexcept
{
    if (myRoot == nullptr) {
        return Certificate::Status::kUnknown;
    }
    Certificate const *pWorkRoot{myRoot};
    for (auto const &itData : chain) {
        Certificate const *const pWorkCert{&itData.get()};
        if (nullptr == pWorkCert) {
            return Certificate::Status::kNotAvailable;
        }
        Certificate::Status const nStatus{PCertificate::VerifyCert(*pWorkCert, pWorkRoot)};
        if (PCertificate::Status::kValid != nStatus) {
            return nStatus;
        }
        pWorkRoot = pWorkCert;
    }
    return Certificate::Status::kValid;
}
/// @brief Verify the status of the provided certificate chain only through locally stored CA certificates and CRLs.
///         The verification status of the certificate chain is certificate:: status::kValid; it is only valid if all certificates in the chain are in this state!
///         Certificates in the chain (provided by the container vector) must be placed from the Root CA certificate (index zero) to the target end-entity certificate (last used vector index). Verification is performed in the same order.
///         If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with certificates in the chain.
/// @param chain  Certificate chain
/// @param myRoot root certificate to be used for verification - if this is nullptr, use machine root
/// @return verification status of the provided certificate chain
Certificate::Status PX509Provider::_VerifyCertChain(
    ara::core::Span< std::reference_wrapper< Certificate const > > const &chain,
    Certificate::Uptr const myRoot) const noexcept
{
    if (false == internal::IsCryptoInitialize()) {
        return PCertificate::Status::kInvalid;
    }
    Certificate *const pWorkRoot{myRoot.get()};
    /// if this is nullptr, use machine root
    if (nullptr == pWorkRoot) {
        Certificate::Status nStatus{Certificate::Status::kUnknown};
        /// If null, load from persistent file
        if (vecCertRootOfTrust_.empty()) {
            keys::isoft_def::PIpcDealX509 const ipcDealX509;
            ara::core::Result< ara::core::Vector< ara::core::String > > resVec{ipcDealX509.Ipc_GetAsRootOfTrust()};
            if (!resVec.HasValue()) {
                return PCertificate::Status::kNotAvailable;
            }
            ara::core::Vector< ara::core::String > vecData{std::move(resVec).Value()};
            size_t const nDataLen{vecData.size()};
            for (size_t i{0U}; i < nDataLen; i++) {
                ara::core::Result< ara::core::Vector< uint8_t > > const resultLoad{
                    ipcDealX509.Ipc_LoadCert({vecData[i].data(), vecData[i].size()})};
                if (false == resultLoad.HasValue()) {
                    continue;
                }
                ara::core::Result< x509::Certificate::Uptr > resCertificate{nullptr};
                resCertificate = MakeCertificate(_getSelf(), T_StringView(vecData[i]), resultLoad.Value());
                if (!resCertificate.HasValue()) {
                    continue;
                }
                x509::Certificate::Uptr pCertificate{std::move(resCertificate).Value()};
                std::ignore = pCertificate->SetAsRootOfTrust(true);
                vecCertRootOfTrust_.push_back(std::move(pCertificate));
            }
        }
        for (auto &itData : vecCertRootOfTrust_) {
            nStatus = VerifyCertChain2(chain, itData.get());
            if (nStatus == Certificate::Status::kValid) {
                return nStatus;
            }
        }
        return nStatus;
    }
    return VerifyCertChain2(chain, pWorkRoot);
}
/// @brief 2011 Standard Interface
/// @brief Verify the status of the provided certificate chain only through locally stored CA certificates and CRLs.
///         The verification status of the certificate chain is certificate:: status::kValid; it is only valid if all certificates in the chain are in this state!
///         Certificates in the chain (provided by the container vector) must be placed from the Root CA certificate (index zero) to the target end-entity certificate (last used vector index). Verification is performed in the same order.
///         If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with certificates in the chain.
/// @param chain  Certificate chain
/// @param myRoot root certificate to be used for verification - if this is nullptr, use machine root
/// @return verification status of the provided certificate chain
Certificate::Status PX509Provider::VerifyCertChain(ara::core::Span< Certificate::Uptr const > const &chain,
                                                   Certificate::Uptr myRoot) const noexcept
{
    ara::core::Vector< std::reference_wrapper< Certificate const > > vecCert{};
    vecCert.reserve(chain.size());
    for (auto const &i : chain) {
        if (i.get() == nullptr) {
            return Certificate::Status::kNotAvailable;
        }
        vecCert.push_back(*(i.get()));
    }

    return _VerifyCertChain(ara::core::Span< std::reference_wrapper< Certificate const > >{vecCert}, std::move(myRoot));
}
/// @brief 2311 Standard Interface
/// @brief 2311 Standard Interface: Verify the status of the provided certificate chain only through locally stored CA certificates and CRLs.
///         The verification status of the certificate chain is certificate:: status::kValid; it is only valid if all certificates in the chain are in this state!
///         Certificates in the chain (provided by the container vector) must be placed from the Root CA certificate (index zero) to the target end-entity certificate (last used vector index). Verification is performed in the same order.
///         If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with certificates in the chain.
/// @param chain  target certificate chain for verification
/// certificates
/// @returns verification status of the provided certificate chain
Certificate::Status PX509Provider::VerifyCertChain(
    ara::core::Span< std::reference_wrapper< Certificate const > > chain) const noexcept
{
    return _VerifyCertChain(chain);
}
/// @brief Set the specified CA certificate as a "Trusted Root Certificate".
///         Only certificates saved in volatile or persistent storage will be marked as "Trust Root"!
///         Only CA certificates are trust root certificates! Multiple certificates on one ECU can be marked as "Trust Root".
///         Only applications with "Trust Administrator" permissions are authorized to call this method!
/// @param caCert a valid CA certificate that should be trusted
/// @return has value if SetAsRootOfTrust sucess false otherwise
ara::core::Result< void > PX509Provider::SetAsRootOfTrust(Certificate const &caCert) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    if (Certificate::Status::kValid != caCert.GetStatus()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate doesn't belong to a CA
    if (false == caCert.IsCa()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    if (false == caCert.IsRoot()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    /// @error: SecurityErrorDomain::kAccessViolation        if the method called by an application without the "Trust
    /// Master" permission
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< bool > const resBool{ipcDealX509.Ipc_IsTrustMaster()};
    if (resBool.HasValue() == false) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }

    PCertificate const *const pCert{dynamic_cast< PCertificate const * >(&caCert)};
    if (nullptr == pCert) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    bool const ret{pCert->SetAsRootOfTrust(true)};
    if (!ret) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ara::core::StringView const stCertName{pCert->GetCertSlotName()};
    if (!stCertName.empty()) {
        std::ignore = ipcDealX509.Ipc_SetCertExInfo(stCertName, 0U);
    }
    Certificate::Uptr pNewCert{caCert.CloneSelf()};
    vecCertRootOfTrust_.push_back(std::move(pNewCert));
    return ara::core::Result< void >::FromValue();
}
/// @brief 2011 Standard Interface
/// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP
/// stapling". This method updates the Certificate::Status associated with the certificate.
/// @brief Check certificate status by directly provided OCSP response.
///       This method may be used for implementation of the "OCSP stapling".
///       This method updates the @c Certificate::Status associated with the certificate.
/// @param cert  a certificate that should be verified
/// @param ocspResponse  an OCSP response
/// @returns @c true if the certificate is verified successfully and @c false otherwise
/// @trace_id_sws={SWS_CRYPT_40629}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
/// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
/// @threadsafety={Thread-safe}
ara::core::Result< bool > PX509Provider::CheckCertStatus(Certificate &cert,
                                                         OcspResponse const &ocspResponse) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< bool >);
    ara::core::Result< ara::crypto::x509::Certificate::Status > const resStatus{_CheckCertStatus(cert, ocspResponse)};
    if (resStatus.HasValue() == false) {
        return ara::core::Result< bool >::FromError(resStatus.Error());
    }
    return ara::core::Result< bool >::FromValue(true);
}
/// @brief 2311 Standard Interface
/// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP
/// stapling". This method updates the Certificate::Status associated with the certificate.
/// @brief Check certificate status by directly provided OCSP response.
///       This method may be used for implementation of the "OCSP stapling".
///       This method updates the @c Certificate::Status associated with the certificate.
/// @param cert  a certificate that should be verified
/// @param ocspResponse  an OCSP response
/// @param rootCert  root certificate
/// @returns @c Certificate status: 2311 standard modified return value to Certificate::Status, to return error codes, returns ara::core::Result< Certificate::Status >
/// @trace_id_sws={SWS_CRYPT_40629}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
/// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
/// @threadsafety={Thread-safe}
ara::core::Result< Certificate::Status > PX509Provider::CheckCertStatus(Certificate const &cert,
                                                                        OcspResponse const &ocspResponse,
                                                                        Certificate const &rootCert) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Certificate::Status >);

    /// Root certificate verifies if cert is issued by the root certificate
    Certificate::Status const certStatus{PCertificate::VerifyCert(cert, &rootCert)};
    if (Certificate::Status::kNotAvailable == certStatus) {
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    return _CheckCertStatus(cert, ocspResponse);
}
/// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP
/// stapling". This method updates the Certificate::Status associated with the certificate.
/// @param cert  a certificate that should be verified
/// @param ocspResponse  an OCSP response
/// @returns @c true if the certificate is verified successfully and @c false otherwise
/// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
/// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
/// @threadsafety={Thread-safe}
ara::core::Result< Certificate::Status > PX509Provider::_CheckCertStatus(
    Certificate const &cert, OcspResponse const &ocspResponse) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Certificate::Status >);
    if (Certificate::Status::kInvalid == cert.GetStatus()) {
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    POcspResponse const *const ppOcspResponse{dynamic_cast< POcspResponse const * >(&ocspResponse)};
    if (ppOcspResponse == nullptr) {
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kRuntimeFault);
    }
    OCSP_RESPONSE *const pocspResponse{ppOcspResponse->GetOCSP_RESPONSE()};
    int32_t const i{OCSP_response_status(pocspResponse)};
    if (i != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kRuntimeFault);
    }

    Certificate *const issuer{_GetIssureCertificate(cert)};
    if (issuer == nullptr) {
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    OCSP_BASICRESP *const bs{OCSP_response_get1_basic(pocspResponse)};
    if (bs == nullptr) {
        OCSP_BASICRESP_free(bs);
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    EVP_MD const *const certIdMd{EVP_sha1()};
    OCSP_CERTID *const id{OCSP_cert_to_id(certIdMd, cert.GetX509(), issuer->GetX509())};

    int32_t status{0};
    int32_t reason{0};
    ASN1_GENERALIZEDTIME *rev{nullptr};
    ASN1_GENERALIZEDTIME *thisupd{nullptr};
    ASN1_GENERALIZEDTIME *nextupd{nullptr};
    if (OCSP_resp_find_status(bs, id, &status, &reason, &rev, &thisupd, &nextupd) == 0) {
        OCSP_BASICRESP_free(bs);
        return ara::core::Result< Certificate::Status >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    if (reason == 0) {
    }

    if (status == V_OCSP_CERTSTATUS_GOOD) {
        cert.SetCertStatus(Certificate::Status::kValid);
        OCSP_BASICRESP_free(bs);
        return ara::core::Result< Certificate::Status >::FromValue(Certificate::Status::kValid);
    }

    if (status == V_OCSP_CERTSTATUS_REVOKED) {
        cert.SetCertStatus(Certificate::Status::kInvalid);
        OCSP_BASICRESP_free(bs);
        return ara::core::Result< Certificate::Status >::FromValue(Certificate::Status::kInvalid);
    }

    cert.SetCertStatus(Certificate::Status::kUnknown);
    OCSP_BASICRESP_free(bs);
    return ara::core::Result< Certificate::Status >::FromValue(Certificate::Status::kUnknown);
}
/// @brief Check the status of a certificate list via directly provided OCSP response. This method can be used to implement "OCSP
/// stapling". This method updates the Certificate::Status associated with certificates in the list.
/// @brief Check status of a certificates list by directly provided OCSP response.
///       This method may be used for implementation of the "OCSP stapling".
///       This method updates the @c Certificate::Status associated with the certificates in the list.
/// @param certList  a certificates list that should be verified
/// @param ocspResponse  an OCSP response
/// @returns @c true if the certificates list is verified successfully and @c false otherwise
/// @trace_id_sws={SWS_CRYPT_40630}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
/// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
/// @threadsafety={Thread-safe}
/// @return
ara::core::Result< bool > PX509Provider::CheckCertStatus(ara::core::Vector< Certificate * > const &certList,
                                                         OcspResponse const &ocspResponse) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< bool >);

    if (certList.empty()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    for (auto const &itData : certList) {
        ara::core::Result< Certificate::Status > const resBool{_CheckCertStatus(*itData, ocspResponse)};
        if (!resBool.HasValue()) {
            return ara::core::Result< bool >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resBool.Error().Value()));
        }
    }
    return ara::core::Result< bool >::FromValue(true);
}
//***************/
#if AP_VERSION_PUHUA == 20
/// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
///         If some certificates saved in persistent or volatile storage are listed in the imported CRL, their status must be automatically updated to "status::kInvalid".
///         If some of these certificates were already opened during this operation, this status change will be immediately available (via method call Certificate::GetStatus())!
///         All certificates with status kInvalid should be automatically removed from the corresponding storage (immediately if the certificate is not currently in use, otherwise immediately upon closing).
/// @brief SWS_CRYPT_40620
/// @brief Jira--[SWS_CRYPT_20903] X.509 Provider allows importing and updating CRLs
/// @name  ImportCrl()
/// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
/// @returns no return value
ara::core::Result< bool > PX509Provider::ImportCrl(ReadOnlyMemRegion const &crl) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< bool >);
    return _ImportCrl(crl);
}
#elif AP_VERSION_PUHUA == 30
/// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
///         If some certificates saved in persistent or volatile storage are listed in the imported CRL, their status must be automatically updated to "status::kInvalid".
///         If some of these certificates were already opened during this operation, this status change will be immediately available (via method call Certificate::GetStatus())!
///         All certificates with status kInvalid should be automatically removed from the corresponding storage (immediately if the certificate is not currently in use, otherwise immediately upon closing).
/// @brief SWS_CRYPT_40620
/// @brief Jira--[SWS_CRYPT_20903] X.509 Provider allows importing and updating CRLs
/// @name  ImportCrl()
/// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
/// @returns no return value
ara::core::Result< void > PX509Provider::ImportCrl(ReadOnlyMemRegion const &crl) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    ara::core::Result< bool > resImportCrl{_ImportCrl(crl)};
    if (resImportCrl.HasValue() == false) {
        return ara::core::Result< void >::FromError(resImportCrl.Error());
    }
    return ara::core::Result< void >::FromValue();
}
#endif

/// @brief Implement core logic for ImportCrl
/// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
/// @returns no return value
ara::core::Result< bool > PX509Provider::_ImportCrl(ReadOnlyMemRegion const &crl) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< bool >);

    // General process: 1. Parse CRL, get X509_CRL object: d2i_X509_CRL
    //              2. Verify based on X509_CRL: X509_CRL_verify
    //             3. Get revoked serial number list from X509_CRL: ASN1_INTEGER *X509_REVOKED_get0_serialNumber(X509_REVOKED *x);
    // 1. Import crl and update local CRL list, this is fine.
    // ReadOnlyMemRegion -> BIO
    BIO *const bp{BIO_new_mem_buf(crl.data(), static_cast< int32_t >(crl.size()))};
    // Get X509_CRL
    X509_CRL *const pX509Crl{d2i_X509_CRL_bio(bp, nullptr)};

    /// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB is not a CRL/DeltaCRL
    if (pX509Crl == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }

    /// @error: SecurityErrorDomain::kRuntimeFault       if the CRL validation has failed
    /// 2. Verify CRL signature
    /// Ensure X.509 provider has imported the certificate that issued this crl beforehand
    ara::core::Vector< PCertificate::Uptr >::iterator iter{vecCertVolatile_.begin()};
    for (; iter != vecCertVolatile_.end(); ++iter) {
        X509 *const pX509{(*iter)->GetX509()};
        EVP_PKEY *const evpKey{X509_get_pubkey(pX509)};
        // Return value of X509_CRL_verify function indicates verification result:
        // Return 1: Verification successful, CRL signature valid, issued by CA corresponding to provided public key
        // Return 0: Verification failed, CRL signature invalid, data untrustworthy
        if (X509_CRL_verify(pX509Crl, evpKey) == 1) {
            /// CRL only needs to be verified by one trust anchor, no need to pass all.
            break;
        }
    }
    /// Even the last one failed verification, return error
    if (iter == vecCertVolatile_.end()) {
        X509_CRL_free(pX509Crl);
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kRuntimeFault);
    }

    /// 3. Get revoked serial number list
    STACK_OF(X509_REVOKED) *const nRevoked{X509_CRL_get_REVOKED(pX509Crl)};
    if (nullptr == nRevoked) {
        X509_CRL_free(pX509Crl);
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kRuntimeFault);
    }
    int32_t const allSerialNum{sk_X509_REVOKED_num(nRevoked)};
    if (-1 == allSerialNum) {
        X509_CRL_free(pX509Crl);
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kRuntimeFault);
    }

    // nRevoked has many revoked certificate serial numbers, get the serial number of a certain certificate via loop and pass it into importCRLists
    for (int32_t i{0}; i < allSerialNum; ++i) {
        /// sk_X509_REVOKED_value: Get the i-th revoked entry from OpenSSL stack (STACK).
        X509_REVOKED const *const pRevoked{sk_X509_REVOKED_value(nRevoked, i)};  //NOLINT
        /// X509_REVOKED_get0_serialNumber: Extract certificate serial number from entry (returns ASN1_INTEGER type pointer, no manual release needed)
        ASN1_INTEGER const *const pSerial{X509_REVOKED_get0_serialNumber(pRevoked)};
        /// ASN1_STRING_length: Get data length (number of bytes)
        uint32_t const len{static_cast< uint32_t >(ASN1_STRING_length(pSerial))};
        /// ASN1_STRING_get0_data: Get pointer to serial number binary data (e.g., hex 1001 might be stored as \x10\x01).
        uint8_t const *const data{ASN1_STRING_get0_data(pSerial)};

        // SWS_CRYPT_20903
        // After verification passes, compare the imported importCRLists with the local CRL list, add to local if not present
        bool const exist{std::any_of(localCRLists_.begin(), localCRLists_.end(),
                                     [data, len](ara::core::Vector< uint8_t > const &r) noexcept -> bool {
                                         if (r.size() != len) {
                                             return false;
                                         }
                                         if (0 != memcmp(r.data(), data, static_cast< std::size_t >(len))) {
                                             return false;
                                         }
                                         return true;
                                     })};

        if (false == exist) {
            ara::core::Vector< uint8_t > const serNum(data, data + len);
            // If not present, add to localCRLists
            /// Update local crl list
            localCRLists_.push_back(serNum);
        }
    }
    X509_CRL_free(pX509Crl);
    return ara::core::Result< bool >::FromValue();
}
/// @brief Import certificate into volatile or persistent storage.
///         Only imported certificates can be found via search and apply for automatic verification! A certificate can only be imported into one of the storages: volatile or persistent.
///         Therefore, if you import a certificate already saved in persistent storage into volatile storage, nothing changes.
///         However, if you import a certificate already saved in volatile storage into persistent storage, it will be "moved" to the persistent domain.
///         If the application successfully imports a certificate corresponding to a CSR existing in the store, that CSR should be deleted.
/// @brief Import the certificate to volatile or persistent storage.
///       Only imported certificate may be found by a search and applied for automatic verifications!
///       A certificate can be imported to only one of storage: volatile or persistent. Therefore if you
///       import a certificate already kept in the persistent storage to the volatile one then nothing changes.
///       But if you import a certificate already kept in the volatile storage to the persistent one then it is
///       "moved" to the persistent realm.
///       If an application successfully imports a certificate that correspond to a CSR existing in the storage
///       then this CSR should be removed.
/// @param cert  a valid certificate that should be imported
/// @param iSpecify  optionally a valid InstanceSpecifier can be provided that points to a CertificateSlot
///                      for persistent storage of the certificate, otherwise the certificate shall be stored
///                      in volatile (session) storage
/// @returns has value if Import sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_40621}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
/// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate has partial collision with a matched
/// CSR in the storage
/// @error: SecurityErrorDomain::kContentDuplication     if the provided certificate already exists in the storage
/// @error: SecurityErrorDomain::kAccessViolation        if the InstanceSpecifier points to a CertificateSlot, which the
/// application may only read
/// @threadsafety={Thread-safe}
ara::core::Result< void > PX509Provider::Import(
    Certificate const &cert, ara::core::Optional< ara::core::InstanceSpecifier > const &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    if (Certificate::Status::kValid != cert.GetStatus()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Fill vecCsrVolatile_
    _PushToCsrVolatile();

    // Check for partial conflicts between provided certificate and matching CSR in storage
    bool const ret{_CheckCollision(cert)};
    if (false == ret) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    // Use has_value to determine whether storing in memory or disk
    bool const hasValue{iSpecify.has_value()};
    if (hasValue == false)  // Store in memory
    {
        StorageIndex nCertIndex{0U};
        time_t tmFind{cert.StartTime()};
        tmFind += (cert.EndTime() - tmFind) / kInt_2;
        Certificate::Uptrc const pFindCert{FindCertByDn(cert.SubjectDn(), cert.IssuerDn(), tmFind, nCertIndex)};
        if (false == pFindCert.operator bool()) {
            Certificate::Uptr pNewCert{cert.CloneSelf()};
            vecCertVolatile_.push_back(std::move(pNewCert));
        }
        if (nCertIndex == 0U) {
        }
    } else {  // Store on disk
        /// Using whether stCertName is empty to judge disk storage is unreliable, because if iSpecify is set to empty, ToString() fails when converting string to stringView
        ara::core::String const stCertName{
            manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify->ToString())};
        /// @error: SecurityErrorDomain::kAccessViolation        if the InstanceSpecifier points to a CertificateSlot, which
        /// the application may only read
        // If APP's permission for this certificate slot is read-only
        if (!manifest::PManifestInstance::Get()->IsCertPortWriteAccess(iSpecify->ToString().data())) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
        }

        /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate has partial collision with a
        /// matched CSR in the storage
        // If partial conflict exists between provided certificate and matched CSR
        X509DN const &issuerDn{cert.IssuerDn()};
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSn{cert.SerialNumber()};
        if (resultSn.HasValue() == false) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
        ReadOnlyMemRegion const memSnData{T_TransBytes(resultSn.Value().data()), resultSn.Value().size()};
        /// Search persistent file for existence of certificate to be saved
        Certificate::Uptrc const pCertFind{FindCertBySn(memSnData, issuerDn)};
        /// @error: SecurityErrorDomain::kContentDuplication     if the provided certificate already exists in the
        /// storage
        if (pCertFind != nullptr) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentDuplication);
        }

        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultExport{
            cert.ExportPublicly(Certificate::kFormatDerEncoded)};
        if (false == resultExport.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resultExport.Error().Value()));
        }
        keys::isoft_def::PIpcDealX509 const ipcDealX509;
        ara::core::Result< uint32_t > const resultIpc{ipcDealX509.Ipc_SaveCert(
            {stCertName.data(), stCertName.size()},
            static_cast< uint8_t const * >(static_cast< void const * >(resultExport.Value().data())),
            static_cast< uint16_t >(resultExport.Value().size()))};
        if (false == resultIpc.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resultIpc.Error().Value()));
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulate logic for saving loaded CSR into this function to reduce complexity of Import function
void PX509Provider::_PushToCsrVolatile() noexcept
{
    if (vecCsrVolatile_.empty()) {
        keys::isoft_def::PIpcDealX509 const ipcDealX509;
        ara::core::Result< ara::core::Vector< ara::core::String > > resVec{ipcDealX509.Ipc_GetCsrNames()};
        if (resVec.HasValue()) {
            ara::core::Vector< ara::core::String > vecData{std::move(resVec).Value()};
            for (auto &itData : vecData) {
                ara::core::Result< ara::core::Vector< uint8_t > > const resultLoad{
                    ipcDealX509.Ipc_LoadCert(T_StringView(itData))};
                if (false == resultLoad.HasValue()) {
                    continue;
                }
                PX509Provider *const pSelf{const_cast< PX509Provider * >(this)};  /// Remove const attribute
                ara::core::Result< x509::CertSignRequest::Uptr > resCertSignRequest{nullptr};
                resCertSignRequest = MakeCertSignRequest(*pSelf, T_StringView(itData), resultLoad.Value());
                if (!resCertSignRequest.HasValue()) {
                    continue;
                }
                vecCsrVolatile_.push_back(std::move(std::move(resCertSignRequest).Value()));
            }
        }
    }
}
/// @brief Check for partial conflicts between provided certificate and matching CSR in storage
/// @param cert Provided certificate
/// @return 1 indicates check passed, -1 indicates conflict
bool PX509Provider::_CheckCollision(Certificate const &cert) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject if provided certificate has partial collision with a
    /// matched CSR in the storage
    for (auto &itData : vecCsrVolatile_) {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resCertDns{
            cert.SubjectDn().ExportPublicly(Serializable::kFormatDerEncoded)};
        if (!resCertDns.HasValue()) {
            continue;
        }
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resCsrDns{
            itData->SubjectDn().ExportPublicly(Serializable::kFormatDerEncoded)};
        if (!resCsrDns.HasValue()) {
            continue;
        }

        ara::core::Vector< ara::core::Byte > const &vecCertDns{resCertDns.Value()};
        ara::core::Vector< ara::core::Byte > const &vecCsrDns{resCsrDns.Value()};
        if (vecCertDns.size() != vecCsrDns.size()) {
            continue;
        }
        size_t j{0U};
        for (j = 0U; j < vecCertDns.size(); j++) {
            if (vecCertDns[j] != vecCsrDns[j]) {
                break;
            }
        }
        if (j == vecCertDns.size()) {
            return false;
        }
    }
    return true;
}
/// @brief Import csr
/// @param csr a valid certificate that should be imported
/// @param iSpecify optionally a valid InstanceSpecifier can be provided that points to a CertificateSlot
///                      for persistent storage of the certificate, otherwise the certificate shall be stored
///                      in volatile (session) storage
/// @return has value if ImportCsr sucess false otherwise
ara::core::Result< void > PX509Provider::ImportCsr(
    CertSignRequest const &csr, ara::core::Optional< ara::core::InstanceSpecifier > const &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    // If APP's permission for this certificate slot is read-only
    if (!manifest::PManifestInstance::Get()->IsCertPortWriteAccess(iSpecify->ToString().data())) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }

    PCertSignRequest const *const pCertSignRequest{dynamic_cast< PCertSignRequest const * >(&csr)};
    if (pCertSignRequest == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify->ToString())};
    if (false == stCertName.empty()) {
        /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate has partial collision with a
        /// matched CSR in the storage
        // If partial conflict exists between provided certificate and matched CSR
        keys::isoft_def::PIpcDealX509 const ipcDealX509;
        ara::core::Result< uint16_t > const resultIsExist{
            ipcDealX509.Ipc_DealIsExist({stCertName.data(), stCertName.size()})};
        if (false == resultIsExist.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resultIsExist.Error().Value()));
        }
        /// @error: SecurityErrorDomain::kContentDuplication     if the provided certificate already exists in the
        /// storage
        if (resultIsExist.Value() != 0U) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentDuplication);
        }
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultExport{
            csr.ExportPublicly(Certificate::kFormatDerEncoded)};
        if (false == resultExport.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resultExport.Error().Value()));
        }
        ara::core::Result< uint32_t > const resultIpc{ipcDealX509.Ipc_SaveCert(
            {stCertName.data(), stCertName.size()},
            static_cast< uint8_t const * >(static_cast< void const * >(resultExport.Value().data())),
            static_cast< uint16_t >(resultExport.Value().size()))};
        if (false == resultIpc.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resultIpc.Error().Value()));
        }
        pCertSignRequest->SetStorageFile(stCertName);
    }
    PCertSignRequest::Uptr pNewCsr{pCertSignRequest->CloneSelf()};
    vecCsrVolatile_.push_back(std::move(pNewCsr));
    return ara::core::Result< void >::FromValue();
}
#if AP_VERSION_PUHUA == 20
/// @brief 2011 Standard Interface
/// @brief Remove the specified certificate (volatile or persistent) from storage and destroy it.
/// @param cert a unique smart pointer to a certificate that should be removed
/// @return @c true if the certificate was found and removed from the storage, @c false if it was not found
bool PX509Provider::Remove(Certificate::Uptrc &&cert) noexcept
{
    bool const bReturn{_Remove(cert.get())};
    std::ignore = std::move(cert);
    return bReturn;
}
#elif AP_VERSION_PUHUA == 30
/// @brief 2011 Standard Interface
/// @brief Remove the specified certificate (volatile or persistent) from storage and destroy it.
/// @param cert a unique smart pointer to a certificate that should be removed
/// @return @c true if the certificate was found and removed from the storage, @c false if it was not found
bool PX509Provider::Remove(Certificate::Uptrc cert) noexcept { return _Remove(cert.get()); }
#endif
/// @brief Core logic for Remove function, suitable for calls from both 2011 and 2311 standard interfaces
/// @param cert  a unique smart pointer to a certificate that should be removed
/// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
bool PX509Provider::_Remove(Certificate const *const cert) noexcept
{
    if (false == internal::IsCryptoInitialize()) {
        return false;
    }
    // First search volatile storage Volatile, delete if found
    ara::core::Vector< PCertificate::Uptr >::const_iterator itFind{vecCertVolatile_.cend()};
    for (ara::core::Vector< PCertificate::Uptr >::const_iterator it{vecCertVolatile_.cbegin()};
         it != vecCertVolatile_.cend(); it++) {
        if ((*it)->GetStorageFile() != cert->GetStorageFile()) {
            continue;
        }
        if ((*it)->SubjectDn() != cert->SubjectDn()) {
            continue;
        }
        if ((*it)->IssuerDn() != cert->IssuerDn()) {
            continue;
        }
        if ((*it)->SerialNumber().Value() != cert->SerialNumber().Value()) {
            continue;
        }
        itFind = it;
        break;
    }

    if (itFind != vecCertVolatile_.cend()) {
        std::ignore = vecCertVolatile_.erase(itFind);
        return true;
    }

    // Delete root certificate (delete if it is a root certificate)
    ara::core::Vector< Certificate::Uptr >::const_iterator itFindRoot{vecCertRootOfTrust_.cend()};
    for (ara::core::Vector< Certificate::Uptr >::const_iterator it{vecCertRootOfTrust_.cbegin()};
         it != vecCertRootOfTrust_.cend(); it++) {
        if ((*it)->GetStorageFile() != cert->GetStorageFile()) {
            continue;
        }
        if ((*it)->SubjectDn() != cert->SubjectDn()) {
            continue;
        }
        if ((*it)->IssuerDn() != cert->IssuerDn()) {
            continue;
        }
        if ((*it)->SerialNumber().Value() != cert->SerialNumber().Value()) {
            continue;
        }
        itFindRoot = it;
        break;
    }

    if (itFindRoot != vecCertRootOfTrust_.cend()) {
        std::ignore = vecCertRootOfTrust_.erase(itFindRoot);
    }

    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< uint32_t > const resultIpc{ipcDealX509.Ipc_RemoveCert(cert->GetCertSlotName())};
    if (false == resultIpc.HasValue()) {
        return false;
    }
    std::ignore = cert;
    return true;
}
//***************/
/// @brief Set the "pending" state associated with the CSR, indicating the CSR has been sent to the CA.
///         If the CSR is already marked "pending", this method does nothing. Only applications with "CA Connector" permission are authorized to call this method!
/// @brief Set the "pending" status associated to the CSR that means that the CSR already sent to CA.
///       This method do nothing if the CSR already marked as "pending".
///       Only an application with permissions "CA Connector" has the right to call this method!
/// @param request  certificate signing request that should be marked as "pending"
/// @return has value if SetPendingStatus sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_40624}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kAccessViolation    if the method called by an application without the "CA Connector"
/// permission
/// @threadsafety={Thread-safe}
ara::core::Result< void > PX509Provider::SetPendingStatus(CertSignRequest const &request) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    std::ignore = request;
    /// @error: SecurityErrorDomain::kAccessViolation    if the method called by an application without the "CA
    /// Connector" permission
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    PCertSignRequest const *const pCertSignRequest{dynamic_cast< PCertSignRequest const * >(&request)};
    if (pCertSignRequest == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::StringView const stCertName{pCertSignRequest->GetStorageFile()};
    if (stCertName.empty()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }
    ara::core::Result< bool > const resBool{ipcDealX509.Ipc_SetCertExInfo(stCertName, 1U)};
    if (!resBool.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Parse user-provided Certificate Signing Request (CSR).
/// @param csr the buffer containing a certificate signing request
/// @param withMetaData specifies the format of the buffer content: @c TRUE means the object has been
///                          previously serialized by using the Serializable interface; @c FALSE means
///                          the CSR was exported using the CertSignRequest::ExportASN1CertSignRequest() interface
/// @return unique smart pointer to the certificate signing request
ara::core::Result< CertSignRequest::Uptrc > PX509Provider::ParseCertSignRequest(ReadOnlyMemRegion const &csr,
                                                                                bool withMetaData) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< CertSignRequest::Uptrc >);
    isoft_def::PCertSignRequest::Uptr pCentSignRquest{std::make_unique< isoft_def::PCertSignRequest >(*this)};
    Serializable::FormatId const nFormatId{withMetaData ? Serializable::kFormatDerEncoded
                                                        : isoft_def::PCertSignRequest::kFormatPemEncoded};
    /// @error: SecurityErrorDomain::kUnsupportedFormat  is returned in case the provided buffer does not contain the
    /// expected format
    if (false == pCentSignRquest->LoadCertFile(csr, nFormatId)) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    return ara::core::Result< CertSignRequest::Uptrc >::FromValue(std::move(pCentSignRquest));
}
/// @brief Create a certificate request for the private key loaded into the context.
/// @param signerCtx the fully-configured SignerPrivateCtx to be used for signing this certificate request
/// @param derSubjectDN the DER-encoded subject distinguished name (DN) of the private key owner
/// @param x509Extensions the DER-encoded X.509 Extensions that should be included to the certification request
/// @param version the format version of the target certification request
/// @return unique smart pointer to created certification request
ara::core::Result< CertSignRequest::Uptrc > PX509Provider::CreateCertSignRequest(
    cryp::SignerPrivateCtx::Uptr signerCtx,
    ReadOnlyMemRegion const &derSubjectDN,
    ReadOnlyMemRegion const &x509Extensions,
    uint32_t version) const noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< CertSignRequest::Uptrc >);
    isoft_def::PCertSignRequest::Uptr pReqCert{std::make_unique< isoft_def::PCertSignRequest >(_getSelf())};
    /// @error: SecurityErrorDomain::kUnexpectedValue    if any of arguments has incorrect/unsupported value
    if (false == pReqCert->SetVersion(static_cast< int32_t >(version))) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (false == pReqCert->SetSubjectDn(derSubjectDN, Serializable::kFormatDerEncoded)) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (false == pReqCert->SetExtension(x509Extensions, Serializable::kFormatDerEncoded)) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    cryp::isoft_def::PCtxDsv_SignerPrivate *const pDsvPrivate{
        static_cast< cryp::isoft_def::PCtxDsv_SignerPrivate * >(static_cast< void * >(signerCtx.get()))};
    if (false == pReqCert->SetPublicKey(pDsvPrivate->GetPrivateKey())) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    cryp::SignatureService::Uptr const pSignService{signerCtx->GetSignatureService()};
    CryptoAlgId const nHashAlgId{pSignService->GetRequiredHashAlgId()};
    uint32_t const nHashSize{static_cast< uint32_t >(pSignService->GetRequiredHashSize())};
    uint32_t const nSignatureSize{static_cast< uint32_t >(pSignService->GetSignatureSize())};
    if (false == pReqCert->SetSignatur(nHashAlgId, nHashSize, nSignatureSize)) {
        return ara::core::Result< CertSignRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    ReadOnlyMemRegion const exTensions{x509Extensions.data(), x509Extensions.size()};
    return ara::core::Result< CertSignRequest::Uptrc >::FromValue(std::move(pReqCert));
}
//***************/
/// @brief Create an OCSP request for the specified certificate. This method can be used to implement "OCSP stapling".
/// @brief Create OCSP request for specified certificate.
///       This method may be used for implementation of the "OCSP stapling".
/// @param cert  a certificate that should be verified
/// @param signer  an optional pointer to initialized signer context (if the request should be signed)
/// @returns unique smart pointer to the created OCSP request
/// @trace_id_sws={SWS_CRYPT_40626}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificate is invalid
/// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
/// @threadsafety={Thread-safe}
ara::core::Result< OcspRequest::Uptrc > PX509Provider::CreateOcspRequest(
    Certificate const &cert, ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const &signer) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< OcspRequest::Uptrc >);
    ara::core::Vector< Certificate const * > certList;
    certList.push_back(&cert);
    return CreateOcspRequest(certList, signer);
}
/// @brief Create an OCSP request for the specified certificate list. This method can be used to implement "OCSP stapling".
/// @brief Create OCSP request for specified list of certificates.
///       This method may be used for implementation of the "OCSP stapling".
/// @param certList  a certificates' list that should be verified
/// @param signer  an optional pointer to initialized signer context (if the request should be signed)
/// @returns unique smart pointer to the created OCSP request
/// @trace_id_sws={SWS_CRYPT_40627}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
/// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
/// @threadsafety={Thread-safe}
ara::core::Result< OcspRequest::Uptrc > PX509Provider::CreateOcspRequest(
    ara::core::Vector< Certificate const * > const &certList,
    ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const &signer) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< OcspRequest::Uptrc >);

    if (certList.empty()) {
        return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    cryp::SignerPrivateCtx *pSignerPrivateCtx{nullptr};
    if (signer.has_value()) {
        if (signer.operator->()->operator->()->IsInitialized() == false) {
            pSignerPrivateCtx = signer.operator->()->get();
        }
    }

    if (pSignerPrivateCtx != nullptr) {
        if (pSignerPrivateCtx->IsInitialized() == false) {
            return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompleteArgState);
        }
    }

    POcspRequest::Uptr ppOcspRequest{std::make_unique< POcspRequest >(*this)};
    if (ppOcspRequest == nullptr) {
        return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompleteArgState);
    }

    Certificate const *subjectCertificate{nullptr};
    Certificate const *issureCertificate{nullptr};

    for (std::size_t i{0U}; i < certList.size(); i++) {
        subjectCertificate = certList[i];
        if (i == 0U) {
            issureCertificate = _GetIssureCertificate(*subjectCertificate);
            if (issureCertificate == nullptr) {
                return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
            }
        } else {
            issureCertificate = certList[i - 1U];
        }

        if (Certificate::Status::kInvalid == subjectCertificate->GetStatus()) {
            return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }

        ara::core::Result< ara::core::Vector< ara::core::Byte > > resCertData{
            subjectCertificate->ExportPublicly(Serializable::kFormatPemEncoded)};
        if (resCertData.HasValue() == false) {
            return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
        ara::core::Vector< ara::core::Byte > vecCertData{std::move(resCertData).Value()};
        ara::core::Byte *const pData{vecCertData.data()};
        ReadOnlyMemRegion const subjectCertData{T_TransBytes(pData), vecCertData.size()};

        resCertData = issureCertificate->ExportPublicly(Serializable::kFormatPemEncoded);
        if (resCertData.HasValue() == false) {
            return ara::core::Result< OcspRequest::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
        ara::core::Vector< ara::core::Byte > vecCaCertData{std::move(resCertData).Value()};
        ara::core::Byte *const pCaData{vecCaCertData.data()};
        ReadOnlyMemRegion const issureCertData{T_TransBytes(pCaData), vecCaCertData.size()};

        std::ignore = ppOcspRequest->AttachResqust(subjectCertData, issureCertData);
    }
    return ara::core::Result< OcspRequest::Uptrc >::FromValue(std::move(ppOcspRequest));
}
/// @brief Send an ocsp request and get ocsp response content
/// @param certNames Issuer certificates (possibly multiple)
/// @param pOcspRequest ocsp request
/// @return ocsp response data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PX509Provider::SendOcspRequest(
    ara::core::Vector< ara::core::StringView > const &certNames, OcspRequest *pOcspRequest) noexcept
{
    if (certNames.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > resOcspData{
        pOcspRequest->ExportPublicly(Serializable::kFormatDerEncoded)};
    if (resOcspData.HasValue() == false) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    ara::core::Vector< ara::core::Byte > const vecData{std::move(resOcspData).Value()};

    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    return ipcDealX509.Ipc_OcspRequest(certNames, vecData);
}
#if AP_VERSION_PUHUA == 20
/// @brief Parse serialized OCSP response and create corresponding interface instance. This method can be used to implement "OCSP stapling".
/// @brief Parse serialized OCSP response and create correspondent interface instance.
///       This method may be used for implementation of the "OCSP stapling".
/// @param response  a serialized OCSP response
/// @returns unique smart pointer to the created OCSP response instance
/// @trace_id_sws={SWS_CRYPT_40628}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
/// @threadsafety={Thread-safe}
OcspResponse::Uptrc PX509Provider::ParseOcspResponse(ReadOnlyMemRegion const &response) const noexcept
{
    ara::core::Result< OcspResponse::Uptrc > resOcspResponse{_ParseOcspResponse(response)};
    if (false == resOcspResponse.HasValue()) {
        return nullptr;
    }
    return std::move(resOcspResponse).Value();
}
#elif AP_VERSION_PUHUA == 30
/// @brief Parse serialized OCSP response and create corresponding interface instance. This method can be used to implement "OCSP stapling".
/// @brief Parse serialized OCSP response and create correspondent interface instance.
///       This method may be used for implementation of the "OCSP stapling".
/// @param response  a serialized OCSP response
/// @returns unique smart pointer to the created OCSP response instance
/// @trace_id_sws={SWS_CRYPT_40628}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
/// @threadsafety={Thread-safe}
ara::core::Result< OcspResponse::Uptrc > PX509Provider::ParseOcspResponse(
    ReadOnlyMemRegion const &response) const noexcept
{
    return _ParseOcspResponse(response);
}
#endif
/// @brief Used to implement core logic of standard interface ParseOcspResponse
/// @param response  a serialized OCSP response
/// @returns unique smart pointer to the created OCSP response instance
ara::core::Result< OcspResponse::Uptrc > PX509Provider::_ParseOcspResponse(
    ReadOnlyMemRegion const &response) const noexcept
{
    PH_CheckInit_RetValue(ara::core::Result< OcspResponse::Uptrc >::FromValue());
    BIO *const pResDerBio{BIO_new_mem_buf(response.data(), static_cast< int32_t >(response.size()))};
    /// @qac Cannot fix [2427, 3116, 3144, 3389]: The first operand of the conditional operator is converted implicitly to bool.
    // PRQA S 228,2427,2428,3048,3080,3116,3144,3389 QAC /// @qac: AUTOSAR standard interface
    OCSP_RESPONSE *resp{d2i_OCSP_RESPONSE_bio(pResDerBio, nullptr)};  // NOLINT
    // PRQA L:QAC
    std::ignore = BIO_free(pResDerBio);

    /// Here it is null, possibly PEM format
    if (resp == nullptr) {
        BIO *const pResPemBio{BIO_new_mem_buf(response.data(), static_cast< int32_t >(response.size()))};
        // PRQA S 228,3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
        resp = static_cast< OCSP_RESPONSE * >(static_cast< void * >(
            PEM_ASN1_read_bio(reinterpret_cast< void *(*)(void **, uint8_t const **, int64_t) >(&d2i_OCSP_RESPONSE),
                              PEM_STRING_OCSP_RESPONSE, pResPemBio, nullptr, nullptr, nullptr)));
        // PRQA L:QAC
        std::ignore = BIO_free(pResPemBio);
    }
    /// Still null here indicates incorrect format
    if (resp == nullptr) {
        return ara::core::Result< OcspResponse::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }

    POcspResponse::Uptr ppOcspResponse{std::make_unique< POcspResponse >(_getSelf())};
    if (ppOcspResponse == nullptr) {
        return ara::core::Result< OcspResponse::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    std::ignore = ppOcspResponse->AttachResponse(resp);
    return ara::core::Result< OcspResponse::Uptrc >::FromValue(std::move(ppOcspResponse));
}
//***************/
/// @brief Find a certificate by its Subject and Issuer Distinguished Names (DN).
///         Parameter certIndex represents the internal index of the current certificate in the store. Starting certificate search from the beginning requires setting: certIndex = kInvalidIndex.
/// @param subjectDn  subject DN of the target certificate
/// @param issuerDn   issuer DN of the target certificate
/// @param validityTimePoint a time point when the target certificate should be valid
/// @param certIndex  an index for iteration through all suitable certificates in the storage
/// @return  unique smart pointer to found certificate or @c nullptr if nothing is found
Certificate::Uptrc PX509Provider::FindCertByDn(X509DN const &subjectDn,
                                               X509DN const &issuerDn,
                                               time_t validityTimePoint,
                                               StorageIndex &certIndex) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    if (certIndex > 0U) {
    }  // for qac
    StorageIndex nIndex{GetkInvalidIndex()};
    for (auto &itData : vecCertVolatile_) {
        nIndex += 1U;
        if (itData->SubjectDn() != subjectDn) {
            continue;
        }
        if (itData->IssuerDn() != issuerDn) {
            continue;
        }
        if (itData->StartTime() > validityTimePoint) {
            continue;
        }
        if (itData->EndTime() < validityTimePoint) {
            continue;
        }
        Certificate::Uptrc pFindVolatile{itData->CloneSelf()};
        certIndex = nIndex - 1U;
        return pFindVolatile;
    }
    if (nIndex == 0U) {
    }
    // Continue search on IPC-Server
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    if (false == ipcDealX509.Ipc_FindCertByDn(certIndex, subjectDn, issuerDn, validityTimePoint)) {
        return {nullptr};
    }
    ara::core::String stCertName;
    std::size_t nCertIndex{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const resultFind{
        ipcDealX509.Ipc_DealAswFindPacket(stCertName, nCertIndex)};
    if (false == resultFind.HasValue()) {
        return {nullptr};
    }
    certIndex = nCertIndex;
    ara::core::Result< Certificate::Uptr > resultCert{
        MakeCertificate(*this, T_StringView(stCertName), resultFind.Value())};
    if (false == resultCert.HasValue()) {
        return {nullptr};
    }
    return {std::move(resultCert).Value()};
}
namespace {
/// @brief Determine that two ReadOnlyMemRegions are not equal
/// @param a ReadOnlyMemRegion object
/// @param b ReadOnlyMemRegion object
/// @return true not equal false equal
bool operator!=(ReadOnlyMemRegion const &a, ReadOnlyMemRegion const &b) noexcept
{
    if (a.size() != b.size()) {
        return true;
    }
    uint8_t const *const pDataA{a.data()};
    uint8_t const *const pDataB{b.data()};
    int32_t const nResult{memcmp(pDataA, pDataB, a.size())};
    return nResult != 0;
}
}  // namespace
/// @brief Find a certificate by its SKID and AKID.
/// @param subjectKeyId     Subject Key Identifier
/// @param authorityKeyId   Authority Key Identifier
/// @return
Certificate::Uptrc PX509Provider::FindCertByKeyIds(ReadOnlyMemRegion const &subjectKeyId,
                                                   ReadOnlyMemRegion const &authorityKeyId) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    for (auto &itData : vecCertVolatile_) {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSubKey{itData->SubjectKeyId()};
        if (false == resultSubKey.HasValue()) {
            continue;
        }
        ReadOnlyMemRegion const findMemSubKeyId{
            static_cast< uint8_t const * >(static_cast< void const * >(resultSubKey.Value().data())),
            resultSubKey.Value().size()};
        if (subjectKeyId != findMemSubKeyId) {
            continue;
        }
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultAutKey{itData->AuthorityKeyId()};
        if (false == resultAutKey.HasValue()) {
            continue;
        }
        ReadOnlyMemRegion const findMemAutKeyId{
            static_cast< uint8_t const * >(static_cast< void const * >(resultAutKey.Value().data())),
            resultAutKey.Value().size()};
        if (authorityKeyId != findMemAutKeyId) {
            continue;
        }
        Certificate::Uptrc pFindVolatile{itData->CloneSelf()};
        return pFindVolatile;
    }
    // Continue search on IPC-Server
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    if (false == ipcDealX509.Ipc_FindCertByKeyIds(subjectKeyId, authorityKeyId)) {
        return {nullptr};
    }
    ara::core::String stCertName;
    std::size_t certIndex{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const resultFind{
        ipcDealX509.Ipc_DealAswFindPacket(stCertName, certIndex)};
    if (false == resultFind.HasValue()) {
        return {nullptr};
    }
    if (certIndex == 0U) {
    }
    ara::core::Result< Certificate::Uptr > resultCert{
        MakeCertificate(*this, T_StringView(stCertName), resultFind.Value())};
    return {std::move(resultCert).Value()};
}
/// @brief Find a certificate based on its serial number.
/// @param sn serial number of the target certificate
/// @param issuerDn authority's Distinguished Names (DN)
/// @return unique smart pointer to found certificate or @c nullptr if nothing is found
Certificate::Uptrc PX509Provider::FindCertBySn(ReadOnlyMemRegion const &sn, X509DN const &issuerDn) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    for (auto &itData : vecCertVolatile_) {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSn{itData->SerialNumber()};
        if (false == resultSn.HasValue()) {
            continue;
        }
        ReadOnlyMemRegion const memSn{
            static_cast< uint8_t const * >(static_cast< void const * >(resultSn.Value().data())),
            resultSn.Value().size()};
        if (sn != memSn) {
            continue;
        }
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultAutKey{itData->AuthorityKeyId()};
        if (false == resultAutKey.HasValue()) {
            continue;
        }
        if (itData->IssuerDn() != issuerDn) {
            continue;
        }
        Certificate::Uptrc pFindVolatile{itData->CloneSelf()};
        return pFindVolatile;
    }
    // Continue search on IPC-Server
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    if (false == ipcDealX509.Ipc_FindCertBySn(sn, issuerDn)) {
        return {nullptr};
    }
    ara::core::String stCertName;
    std::size_t certIndex{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const resultFind{
        ipcDealX509.Ipc_DealAswFindPacket(stCertName, certIndex)};
    if (false == resultFind.HasValue()) {
        return {nullptr};
    }
    if (certIndex == 0U) {
    }
    ara::core::Result< Certificate::Uptr > resultCert{
        MakeCertificate(*this, T_StringView(stCertName), resultFind.Value())};
    if (resultCert.HasValue() == false) {
        return {nullptr};
    }
    return {std::move(resultCert).Value()};
}
//***************/
/// @brief Clear volatile certificate storage. After executing this command, certificates previously imported into volatile storage cannot be found via search, but it does not affect already loaded certificate instances!
void PX509Provider::CleanupVolatileStorage() noexcept
{
    if (false == internal::IsCryptoInitialize()) {
        return;
    }
    /// @brief Result type alias
    using PVector = ara::core::Vector< PCertificate::Uptr >;
    PVector::iterator it{vecCertVolatile_.begin()};
    while (true) {
        if (it == vecCertVolatile_.end()) {
            break;
        }
        if ((*it)->IsVolatileStorage()) {
            // PRQA S 3079 QAC /// @qac: Seems unfixable [3079]
            // Tried various methods still unable to resolve. Non-const iterator converted to a const iterator.
            it = vecCertVolatile_.erase(it);
            // PRQA L:QAC
        } else {
            it++;
        }
    }
}
/// @brief Create an empty X.509 Extensions structure. If (0 == capacity), the maximum supported capacity must be reserved (by the implementation).
/// @param capacity Capacity size
/// @return  Shared smart pointer to created empty @c X509X509Extensions object
ara::core::Result< X509Extensions::Uptr > PX509Provider::CreateEmptyExtensions(std::size_t capacity) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< X509Extensions::Uptr >);
    std::ignore = capacity;
    X509Extensions::Uptr pX509Extensions{std::make_unique< X509Extensions >(*this)};
    return ara::core::Result< X509Extensions::Uptr >::FromValue(std::move(pX509Extensions));
}
/// @brief Load a certificate from persistent certificate storage.
/// @param iSpecify Certificate port name
/// @return an unique smart pointer to the instantiated certificate
ara::core::Result< Certificate::Uptr > PX509Provider::LoadCertificate(ara::core::InstanceSpecifier &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< Certificate::Uptr >);
    /// @error: SecurityErrorDomain::kUnreservedResource     if the InstanceSpecifier is incorrect (the certificate
    /// cannot be found)
    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify.ToString())};
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< ara::core::Vector< uint8_t > > const resultLoad{
        ipcDealX509.Ipc_LoadCert({stCertName.data(), stCertName.size()})};
    if (false == resultLoad.HasValue()) {
        return ara::core::Result< Certificate::Uptr >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resultLoad.Error().Value()));
    }
    return MakeCertificate(*this, T_StringView(stCertName), resultLoad.Value());
}

/// @brief Load a Certificate Signing Request from persistent certificate storage (non-standard interface)
/// @param iSpecify Certificate port name
/// @return an unique smart pointer to CertSignRequest
ara::core::Result< CertSignRequest::Uptr > PX509Provider::LoadCertSignRequest(
    ara::core::InstanceSpecifier &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< CertSignRequest::Uptr >);
    /// @error: SecurityErrorDomain::kUnreservedResource     if the InstanceSpecifier is incorrect (the certificate
    /// cannot be found)
    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify.ToString())};
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< ara::core::Vector< uint8_t > > const resultLoad{
        ipcDealX509.Ipc_LoadCert({stCertName.data(), stCertName.size()})};
    if (false == resultLoad.HasValue()) {
        return ara::core::Result< CertSignRequest::Uptr >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resultLoad.Error().Value()));
    }
    ara::core::Vector< uint8_t > vecData{resultLoad.Value()};
    // Create unique pointer
    x509::isoft_def::PCertSignRequest::Uptr pCertSignRequest{
        std::make_unique< x509::isoft_def::PCertSignRequest >(*this)};
    if (false == vecData.empty())  // Indicates non-empty
    {
        uint8_t *const pData{vecData.data()};
        ReadOnlyMemRegion const memCert{pData, vecData.size()};
        if (false == pCertSignRequest->LoadCertFile(memCert, x509::Certificate::kFormatDefault)) {
            return ara::core::Result< x509::CertSignRequest::Uptr >::FromError(
                SecurityErrorDomain::Errc::kUnreservedResource);
        }
        pCertSignRequest->SetStorageFile(stCertName);
        return ara::core::Result< x509::CertSignRequest::Uptr >::FromValue(std::move(pCertSignRequest));
    }
    return ara::core::Result< x509::CertSignRequest::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
}

/// @brief Get key slot information corresponding to the certificate
/// @param iSpecify Certificate port identifier
/// @return Port name of the key slot corresponding to the certificate
ara::core::Result< ara::core::Vector< ara::core::String > > PX509Provider::GetCertificateSlots(
    ara::core::InstanceSpecifier &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< ara::core::Vector< ara::core::String > >);
    /// @error: SecurityErrorDomain::kUnreservedResource     if the InstanceSpecifier is incorrect (the certificate
    /// cannot be found)
    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify.ToString())};

    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< ara::core::Vector< ara::core::String > > const resulSlotNames{
        ipcDealX509.Ipc_GetCertSlotNames({stCertName.data(), stCertName.size()})};
    if (false == resulSlotNames.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resulSlotNames.Error().Value()));
    }
    ara::core::Vector< ara::core::String > const &vecData{resulSlotNames.Value()};
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(vecData);
}

/// @brief Check if CSR has pending state set (non-standard interface)
/// @param request Certificate signing request
/// @return has value if has already set pedding false otherwise
ara::core::Result< bool > PX509Provider::IsCsrPenddingStatus(CertSignRequest const &request) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< bool >);
    /// @error: SecurityErrorDomain::kUnreservedResource     if the InstanceSpecifier is incorrect (the certificate
    /// cannot be found)
    PCertSignRequest const *const pCertSignRequest{dynamic_cast< PCertSignRequest const * >(&request)};
    if (pCertSignRequest == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::StringView const stStorageFile{pCertSignRequest->GetStorageFile()};
    if (stStorageFile.empty()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(stStorageFile)};
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    ara::core::Result< bool > const resultLoad{ipcDealX509.Ipc_IsCsrSetPending({stCertName.data(), stCertName.size()})};
    if (false == resultLoad.HasValue()) {
        return ara::core::Result< bool >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resultLoad.Error().Value()));
    }
    bool const ret{resultLoad.Value()};
    return ara::core::Result< bool >::FromValue(ret);
}
/// @brief Get the issuing certificate corresponding to the given certificate
/// @param cert Certificate object
/// @return Pointer to certificate structure
Certificate *PX509Provider::_GetIssureCertificate(Certificate const &cert) const noexcept
{
    /// Root certificate is null, load from persistence
    if (vecCertRootOfTrust_.empty()) {
        keys::isoft_def::PIpcDealX509 const ipcDealX509;
        ara::core::Result< ara::core::Vector< ara::core::String > > resVec{ipcDealX509.Ipc_GetAsRootOfTrust()};
        if (!resVec.HasValue()) {
            return nullptr;
        }
        ara::core::Vector< ara::core::String > vecData{std::move(resVec).Value()};
        for (auto &itData : vecData) {
            ara::core::Result< ara::core::Vector< uint8_t > > const resultLoad{
                ipcDealX509.Ipc_LoadCert(T_StringView(itData))};
            if (false == resultLoad.HasValue()) {
                continue;
            }
            ara::core::Result< x509::Certificate::Uptr > resCertificate{nullptr};
            resCertificate = MakeCertificate(_getSelf(), T_StringView(itData), resultLoad.Value());
            if (!resCertificate.HasValue()) {
                continue;
            }
            vecCertRootOfTrust_.push_back(std::move(resCertificate).Value());
        }
    }

    for (auto &itData : vecCertRootOfTrust_) {
        if (cert.IssuerDn() == itData->SubjectDn()) {
            return itData.get();
        }
    }
    return nullptr;
}
/// @brief Check certificate validity based on local CRL list
/// @name  _CheckCertRevoked
/// @param cert Certificate
/// @return true or false
ara::core::Result< bool > PX509Provider::_CheckCertRevoked(Certificate const &cert) const noexcept
{
    // Preliminary idea
    // Get certificate serial number, search local crl list for this serial number. If found, certificate is revoked, return true; otherwise, certificate not revoked, return false;
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resCertSerialNumber{cert.SerialNumber()};
    ara::core::Vector< ara::core::Byte > certSerialNumber{resCertSerialNumber.Value()};

    uint8_t *const pData{T_TransBytes(certSerialNumber.data())};
    std::size_t const len{certSerialNumber.size()};
    bool const exist{std::any_of(localCRLists_.begin(), localCRLists_.end(),
                                 [pData, len](ara::core::Vector< uint8_t > const &r) noexcept -> bool {
                                     if (r.size() != len) {
                                         return false;
                                     }
                                     if (0 != memcmp(r.data(), pData, static_cast< std::size_t >(len))) {
                                         return false;
                                     }
                                     return true;
                                 })};

    return ara::core::Result< bool >::FromValue(exist);
}
/// @brief External interface for revoking certificates
/// @brief [SWS_CRYPT_20908] X509 Certificate Revocation: X.509 Provider shall support certificate revocation request generation.
/// @param revokedISpecify Certificate port name
/// @return has vlaue if revoke sucess false otherwise
ara::core::Result< bool > PX509Provider::RevokeCertificate(ara::core::InstanceSpecifier &revokedISpecify) noexcept
{
    ara::core::String const stCertName{
        manifest::PManifestInstance::Get()->TransName_PortToCert(revokedISpecify.ToString())};
    if (stCertName.empty()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::Result< Certificate::Uptr > resRevokedCert{LoadCertificate(revokedISpecify)};
    if (!resRevokedCert.HasValue()) {
        return ara::core::Result< bool >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resRevokedCert.Error().Value()));
    }
    Certificate::Uptr const pRevokedCert{std::move(resRevokedCert).Value()};
    Certificate *const issureCert{_GetIssureCertificate(*pRevokedCert)};
    if (issureCert == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resSerialNum{issureCert->SerialNumber()};
    if (!resSerialNum.HasValue()) {
        return ara::core::Result< bool >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resSerialNum.Error().Value()));
    }
    ara::core::Vector< ara::core::Byte > const vecData{std::move(resSerialNum).Value()};

    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    // Ipc_RevokeCert will request the server via IPC to revoke the certificate
    return ipcDealX509.Ipc_RevokeCert(stCertName, vecData);
}
/// @brief External interface for generating CRL
/// @param iSpecify Port name
/// @return
ara::core::Result< ara::core::Vector< ara::core::Byte > > PX509Provider::GenerateCertRevokeList(
    ara::core::InstanceSpecifier &iSpecify) const noexcept
{
    ara::core::String const stCertName{manifest::PManifestInstance::Get()->TransName_PortToCert(iSpecify.ToString())};
    if (stCertName.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::PIpcDealX509 const ipcDealX509;
    return ipcDealX509.Ipc_GenCRL(stCertName);
}
/// @brief Return non-const self via const function
/// @return Non-const self
PX509Provider &PX509Provider::_getSelf() const noexcept
{
    // PRQA S 3066 QAC /// @qac: Return non-const self via const function
    PX509Provider *const pSelf{const_cast< PX509Provider * >(this)};
    // PRQA L:QAC
    return *pSelf;
}

//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara
