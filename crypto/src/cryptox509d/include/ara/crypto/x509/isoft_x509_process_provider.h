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
/// @file       isoft_x509_process_provider.h
/// @brief      X509Provider provider's IPC server side: logical processing of certificate providers
/// @details
/// @date       2023-09-24
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/certificate manager/certificate provider IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_04007
/// @unit_name=PX509Process_Provider
/// @unit_description=Certificate provider IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_X509_PROCESS_PROVIDER_H_
#define ARA_CRYPTO_KEYS_PUHUA_X509_PROCESS_PROVIDER_H_

#include "ara/crypto/common/serializable.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/isoft_x509_process_base.h"
#include "ara/crypto/x509/isoft_x509_provider.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief PX509_Manager
class PX509_Manager;

//********************************/
/// @brief KeyProvider provider's IPC server side: logical processing of key providers
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02875
/// @trace_id_dd=DD_CRYPTO_05763
/// @needwork = ad
/// @endcode
class PX509Process_Provider : public PX509Process_T_Base< PX509Process_Provider >
{
public:
    /// @brief Constructor
    /// @name   PX509Process_Provider
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02876
    /// @trace_id_dd=DD_CRYPTO_05764
    /// @needwork = ad
    /// @endcode
    explicit PX509Process_Provider(PX509_Manager& lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02877
    /// @trace_id_dd=DD_CRYPTO_05765
    /// @needwork = ad
    /// @endcode
    ~PX509Process_Provider() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02878
    /// @trace_id_dd=DD_CRYPTO_05766
    /// @needwork = ad
    /// @endcode
    PX509Process_Provider(PX509Process_Provider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02879
    /// @trace_id_dd=DD_CRYPTO_05767
    /// @needwork = ad
    /// @endcode
    PX509Process_Provider(PX509Process_Provider&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02880
    /// @trace_id_dd=DD_CRYPTO_05768
    /// @needwork = ad
    /// @endcode
    PX509Process_Provider& operator=(PX509Process_Provider const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02881
    /// @trace_id_dd=DD_CRYPTO_05769
    /// @needwork = ad
    /// @endcode
    PX509Process_Provider& operator=(PX509Process_Provider&& other) = delete;

private:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05770
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const kCrlIndexFileName{"index.txt"};

public:
    /// @brief Check whether the key slot exists
    /// @name   IsCertExist
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if cert exist false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05771
    /// @needwork = dda
    /// @endcode
    PResultLen IsCertExist(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Load certificate
    /// @name   LoadCert
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if load cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05772
    /// @needwork = dda
    /// @endcode
    PResultLen LoadCert(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Save certificate
    /// @name   SaveCert
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Save cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05773
    /// @needwork = dda
    /// @endcode
    PResultLen SaveCert(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Delete certificate
    /// @name   RemoveCert
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Remove cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05774
    /// @needwork = dda
    /// @endcode
    PResultLen RemoveCert(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Find certificate by DN
    /// @name   FindCertByDn
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  has vlaue if Find cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05775
    /// @needwork = dda
    /// @endcode
    PResultLen FindCertByDn(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Find certificate by key ID
    /// @name   FindCertByKeyIds
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Find cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05776
    /// @needwork = dda
    /// @endcode
    PResultLen FindCertByKeyIds(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Find certificate by SN
    /// @name   FindCertBySn
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has vlaue if Find cert sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05777
    /// @needwork = dda
    /// @endcode
    PResultLen FindCertBySn(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Check whether it is the trust master process
    /// @name   IsTrustMaster
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if is trust master false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05778
    /// @needwork = dda
    /// @endcode
    PResultLen IsTrustMaster(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Set certificate extension information
    /// @name   SetCertExInfo
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns true if set exinfo sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05779
    /// @needwork = dda
    /// @endcode
    PResultLen SetCertExInfo(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get the root of trust certificate
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if get sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05780
    /// @needwork = dda
    /// @endcode
    PResultLen GetAsRootOfTrust(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Check whether the certificate is set to pending
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return true if already set pending false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05781
    /// @needwork = dda
    /// @endcode
    PResultLen IsCsrSetPending(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get CSR name
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if get csr sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05782
    /// @needwork = dda
    /// @endcode
    PResultLen Ipc_GetCsrNames(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Handle OCSP request
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if Ocsp ResQuest sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05783
    /// @needwork = dda
    /// @endcode
    PResultLen Ipc_OcspResQuest(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Revoke a certificate
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return true if revoke cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05784
    /// @needwork = dda
    /// @endcode
    PResultLen RevokeCert(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Generate CRL
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if generate crl list sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05785
    /// @needwork = dda
    /// @endcode
    PResultLen GenerateCRLists(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;
    /// @brief Get the slot name corresponding to the certificate
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has vlaue if get cert slotnames sucss false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05786
    /// @needwork = dda
    /// @endcode
    PResultLen Ipc_GetCertSlotNames(PIpcPac_Head const* const pReqHead, PIpcAutoPacket& aswMsg) const noexcept;

protected:
    /// @brief Find certificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05787
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > >
    /// @brief Find certificate
    /// @throws
    /// @name   _FindCertByDn
    /// @param stCertName Certificate name
    /// @param nDeleteFlag Deletion flag
    /// @param nProcessID Process ID
    /// @param pPacReq IPC request packet header
    /// @return Certificate data information
    _FindCertByDn(ara::core::String& stCertName,
                  uint32_t const& nDeleteFlag,
                  uint64_t const nProcessID,
                  x509provider::PIpcReq_FindCertByDn const* const pPacReq) const noexcept;
    /// @brief Find certificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05788
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > >
    /// @name   _FindCertByKey
    /// @param stCertName Certificate name
    /// @param nDeleteFlag Deletion flag
    /// @param nProcessID Process ID
    /// @param pPacReq IPC request packet header
    /// @return Certificate data information
    _FindCertByKey(ara::core::String& stCertName,
                   uint32_t const& nDeleteFlag,
                   uint64_t const nProcessID,
                   x509provider::PIpcReq_FindCertByKeyIds const* const pPacReq) const noexcept;
    /// @brief Find certificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05789
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > >
    /// @name   _FindCertBySn
    /// @param stCertName Certificate name
    /// @param nDeleteFlag Deletion flag
    /// @param nProcessID Process ID
    /// @param pPacReq IPC request packet header
    /// @return Certificate data information
    _FindCertBySn(ara::core::String& stCertName,
                  uint32_t const& nDeleteFlag,
                  uint64_t const nProcessID,
                  x509provider::PIpcReq_FindCertBySn const* const pPacReq) const noexcept;
    /// @brief Assemble the return packet
    /// @name   AssembleFindCertMsg
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param stCertName Certificate name
    /// @param nDeleteFlag Deletion flag
    /// @param vecData Input data
    /// @return Length of the return packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05790
    /// @needwork = dda
    /// @endcode
    static PResultLen AssembleFindCertMsg(PIpcPac_Head const* const pReqHead,
                                          PIpcAutoPacket& aswMsg,
                                          ara::core::String const& stCertName,
                                          uint32_t const nDeleteFlag,
                                          ara::core::Vector< uint8_t > const& vecData) noexcept;
    /// @brief Revoke a certificate and generate a new index.txt
    /// @param revcert X509 certificate pointer
    /// @return true if revoke cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05791
    /// @needwork = dda
    /// @endcode
    bool _RevokeCert(X509* const revcert) const noexcept;
    /// @brief Generate Certificate Revocation List (CRL)
    /// @param pemLen Length of the returned PEM format certificate
    /// @param x509Root X509 root certificate
    /// @param pkey EVP_PKEY key resource
    /// @return CRL information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05792
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint8_t > _GenerateCRL(int32_t& pemLen,
                                              X509* const x509Root,
                                              EVP_PKEY* const pkey) const noexcept;
    /// @brief Load and update the revocation list database
    /// @param dbFileName Revocation list database file name
    /// @param unique_subject Unique identifier for issued certificates; default value is zero
    /// Allows multiple certificates with the same subject to coexist in the CA index file; otherwise only one can exist
    /// @return Revocation list database pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05793
    /// @needwork = dda
    /// @endcode
    void* _LoadAndUpdateDb(ara::core::StringView const& dbFileName, bool const uniqueSubject = false) const noexcept;
    /// @brief Load a certificate from memory
    /// @param memCert Certificate memory data
    /// @param nFormatID Certificate encoding format
    /// @return Certificate pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05794
    /// @needwork = dda
    /// @endcode
    static X509* LoadCertFile(ReadOnlyMemRegion const& memCert, Serializable::FormatId const nFormatID) noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_X509_PROCESS_PROVIDER_H_
