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
/// @file       isoft_certificate.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    X.509 Certificate interface, Puhua version.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing      <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/X.509 Certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PCertificate
/// @unit_description=X.509 Certificate Interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_PUHUA_CERTIFICATE_H_
#define ARA_CRYPTO_X509_PUHUA_CERTIFICATE_H_

#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "ara/crypto/x509/certificate.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
//********************************/ //2022-07-11 hanjingjing moved some non-standard interfaces from Certificate class here
/// @brief X.509 Certificate interface, Puhua version.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01396
/// @trace_id_dd=DD_CRYPTO_03656
/// @needwork = ad
/// @endcode
// PRQA S 2659 QAC  /// @qac Suspected inability to modify [2659]:
// Base class '::ara::crypto::x509::CertSignRequest' has non-deleted public copy/move special member functions.
class PCertificate : public Certificate
// PRQA L:QAC
{
public:
    /// @brief Constructor
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03657
    /// @needwork = dda
    /// @endcode
    explicit PCertificate(X509Provider& x509Provider) noexcept;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03658
    /// @needwork = dda
    /// @endcode
    PCertificate(PCertificate&& other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03659
    /// @needwork = dda
    /// @endcode
    PCertificate(PCertificate const& other) noexcept;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03660
    /// @needwork = dda
    /// @endcode
    PCertificate& operator=(PCertificate const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03661
    /// @needwork = dda
    /// @endcode
    PCertificate& operator=(PCertificate&& other) = delete;
    /// @brief Destructor
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03662
    /// @needwork = dda
    /// @endcode
    ~PCertificate() noexcept override = default;

public:
    /// @brief Load p7b format certificate chain from file system
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03663
    /// @needwork = dda
    /// @endcode
    static ara::core::Vector< Certificate::Uptr >
    /// @param x509Provider Certificate provider
    /// @param stFileName Certificate file
    /// @param nFormatID Certificate encoding format
    /// @return Certificate data
    LoadCertFile_P7b(X509Provider& x509Provider,
                     ara::core::StringView const& stFileName,
                     FormatId const nFormatID) noexcept;
    /// @brief Load P7B format certificate chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03664
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< ara::core::Vector< PCertificate::Uptr > >
    /// @brief Load certificate from certificate chain file
    /// @param x509Provider Certificate provider
    /// @param pBio Certificate memory data
    /// @param nFormatID Encoding format
    /// @return
    LoadCertP7b(X509Provider& x509Provider, BIO* const pBio, FormatId const nFormatID) noexcept;
    /// @brief Count number of certificates in chain
    /// @param pBio Certificate chain memory info
    /// @param nFormatID Certificate encoding format
    /// @return Number of certificates in chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03665
    /// @needwork = dda
    /// @endcode
    static int32_t CalculateCertCount(BIO* const pBio, FormatId const nFormatID) noexcept;
    /// @brief Check if FormatID is supported
    /// @param nFormatID Certificate encoding format
    /// @param bCheckForWrite Check for write support
    /// @return Whether FormatID is supported
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03666
    /// @needwork = dda
    /// @endcode
    static bool IsValidFormatID(FormatId const nFormatID, bool const bCheckForWrite = false) noexcept;
    /// @brief Verify certificate and modify statusLast_
    /// @param cert Certificate
    /// @param myRoot Root certificate
    /// @return Certificate status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03667
    /// @needwork = dda
    /// @endcode
    static Certificate::Status VerifyCert(Certificate const& cert, Certificate const* const myRoot) noexcept;

public:
    /// @brief Get "Public Key Algorithm Type"
    /// @return Public key algorithm type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03668
    /// @needwork = dda
    /// @endcode
    uint32_t GetPublicKeyAlgType() const noexcept;
    /// @brief Get "Public Key Algorithm Id"
    /// @return Public key algorithm Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03669
    /// @needwork = dda
    /// @endcode
    uint32_t GetPublicKeyAlgId() const noexcept;
    /// @brief Get "Public Key Length"
    /// @return Public key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03670
    /// @needwork = dda
    /// @endcode
    uint32_t GetPublicKeyLength() const noexcept;
    /// @brief Get public key data
    /// @return Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03671
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Byte > GetPublicKeyData() const noexcept;
    /// @brief Certificate signature algorithm type
    /// @return Signature algorithm type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03672
    /// @needwork = dda
    /// @endcode
    uint32_t GetSignatureAlgType() const noexcept;
    /// @brief The signature algorithm of the certificate. Refers to the algorithm used by the certificate for signing (including HASH algorithm).
    /// @return Certificate signature algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03673
    /// @needwork = dda
    /// @endcode
    uint32_t GetSignatureAlgOid() const noexcept;
    /// @brief Get signature string
    /// @return Signature string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03674
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GetSignatureString() const noexcept;
    /// @brief Get fingerprint stored in X509
    /// @param nHashType
    /// @return Fingerprint information stored in X509
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03675
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GetFingerprintInX509(
        int32_t const nHashType) const noexcept;
    /// @brief Get extension attribute "Enhanced Key Usage"
    /// @return Enhanced key usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03676
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint32_t > GetExtKeyUsage() const noexcept;

public:
    /// @brief Test get certificate fingerprint
    /// @param nSelectMode Selection mode
    /// @param nIndex Index
    /// @param hashCtx Hash context
    /// @param nOffset Offset
    /// @param nPlanLen Plan length
    /// @param nFingerType Finger type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03677
    /// @needwork = dda
    /// @endcode
    void GetFingerprintTest(uint32_t const nSelectMode,
                            int32_t& nIndex,
                            cryp::HashFunctionCtx& hashCtx,
                            int32_t const nOffset,
                            int32_t const nPlanLen,
                            int32_t const nFingerType = NID_sha1) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_PUHUA_CERTIFICATE_H_
