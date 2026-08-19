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
/// @file       isoft_ipc_deal_x509.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    IPC client for KeyProvider, handles IPC packets for X509
/// @date       2022-11-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-01  <td>1.0.0    <td>hanjingjing      <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/X.509 Certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcDealX509
/// @unit_description=Certificate IPC Client
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_DEAL_X509_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_DEAL_X509_H_

#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/x509/isoft_certificate.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Provide IPC client for KeyProvider externally
/// @brief Class for operating IPC; preferably call interfaces of the same object once
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01978
/// @trace_id_dd=DD_CRYPTO_04647
/// @needwork = ad
/// @endcode
class PIpcDealX509
{
private:
    /// @brief IPC response packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04648
    /// @needwork = dda
    /// @endcode
    mutable PIpcAutoPacket aswMsg_;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04649
    /// @needwork = dda
    /// @endcode
    PIpcDealX509() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04650
    /// @needwork = dda
    /// @endcode
    virtual ~PIpcDealX509() = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04651
    /// @needwork = dda
    /// @endcode
    PIpcDealX509(PIpcDealX509 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04652
    /// @needwork = dda
    /// @endcode
    PIpcDealX509 &operator=(PIpcDealX509 &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04653
    /// @needwork = dda
    /// @endcode
    PIpcDealX509 &operator=(PIpcDealX509 const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04654
    /// @needwork = dda
    /// @endcode
    PIpcDealX509(PIpcDealX509 const &other) = delete;
    /// @brief Re-initialize
    /// @name  ReInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04655
    /// @needwork = dda
    /// @endcode
    void ReInit() const noexcept;
    /// @brief Handle LoadCert
    /// @param stCertName Certificate name
    /// @returns Certificate byte data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04656
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > >
    /// @brief Handle LoadCert
    /// @param stCertName Certificate name
    /// @return Certificate byte data
    Ipc_LoadCert(ara::core::StringView const &stCertName) const noexcept;
    /// @brief Handle SaveCert
    /// @param stSlotName Key slot name
    /// @param pData Data: memory start address
    /// @param nLen Data length: in bytes
    /// @return >0 save sucess
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04657
    /// @needwork = dda
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    ara::core::Result< uint32_t > Ipc_SaveCert(ara::core::StringView const &stSlotName,
                                               uint8_t const *const pData,
                                               uint16_t const nLen) const noexcept;
    // PRQA L:QAC
    /// @brief Handle RemoveCert
    /// @param stSlotName Key slot name
    /// @return >0 remove sucess
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04658
    /// @needwork = dda
    /// @endcode
    ara::core::Result< uint32_t > Ipc_RemoveCert(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Handle FindCertByDn
    /// @param certIndex Certificate sequence number
    /// @param subjectDn Subject DN
    /// @param issuerDn Issuer DN
    /// @param validityTimePoint
    /// @return true if find cert false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04659
    /// @needwork = dda
    /// @endcode
    bool Ipc_FindCertByDn(std::size_t const &certIndex,
                          x509::X509DN const &subjectDn,
                          x509::X509DN const &issuerDn,
                          time_t const validityTimePoint) const noexcept;
    /// @brief Handle FindCertByKeyIds
    /// @param subjectKeyId Subject key identifier
    /// @param authorityKeyId Authority key identifier
    /// @return true if find cert false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04660
    /// @needwork = dda
    /// @endcode
    bool Ipc_FindCertByKeyIds(ReadOnlyMemRegion const &subjectKeyId,
                              ReadOnlyMemRegion const &authorityKeyId) const noexcept;
    /// @brief Handle FindCertBySn
    /// @param sn Serial number data
    /// @param issuerDn Issuer DN
    /// @return true if find cert false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04661
    /// @needwork = dda
    /// @endcode
    bool Ipc_FindCertBySn(ReadOnlyMemRegion const &sn, x509::X509DN const &issuerDn) const noexcept;
    /// @brief Handle return value of FindCertBy
    /// @param stCertName Certificate name
    /// @param certIndex Certificate sequence number
    /// @return Certificate data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04662
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > > Ipc_DealAswFindPacket(ara::core::String &stCertName,
                                                                            std::size_t &certIndex) const noexcept;
    /// @brief Check if certificate slot exists
    /// @param stCertName Certificate name
    /// @return >0 cert exist
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04663
    /// @needwork = dda
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    ara::core::Result< uint16_t > Ipc_DealIsExist(ara::core::StringView const &stCertName) const noexcept;
    // PRQA L:QAC
    /// @brief Check if IPC environment is trusted
    /// @return true if is trust false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04664
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Ipc_IsTrustMaster() const noexcept;
    /// @brief IPC set certificate extension information
    /// @param stCertName Certificate name
    /// @param nIndex Index
    /// @return true if set exinfo sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04665
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Ipc_SetCertExInfo(ara::core::StringView const &stCertName,
                                                uint16_t const nIndex) const noexcept;
    /// @brief Get trust root certificate name
    /// @return Root certificate name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04666
    /// @needwork = dda
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    ara::core::Result< ara::core::Vector< ara::core::String > > Ipc_GetAsRootOfTrust() const noexcept;
    // PRQA L:QAC
    /// @brief Get whether the corresponding CSR is set to pending state
    /// @param stCertName Certificate name
    /// @return true has already set pending false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04667
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Ipc_IsCsrSetPending(ara::core::StringView const &stCertName) const noexcept;
    /// @brief Get all CSR port names
    /// @return Certificate Port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04668
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::String > > Ipc_GetCsrNames() const noexcept;
    /// @brief OCSP request
    /// @param certNames Certificate names
    /// @param ocspRestInfo OCSP request data
    /// @return OCSP data information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04669
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Ipc_OcspRequest(
        ara::core::Vector< ara::core::StringView > const &certNames,
        ara::core::Vector< ara::core::Byte > const &ocspRestInfo) const noexcept;
    /// @brief Revoke certificate
    /// @param stCertName Certificate name
    /// @param issureSerialNum Certificate serial number
    /// @return true if revoke sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04670
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > Ipc_RevokeCert(
        ara::core::StringView const &stCertName,
        ara::core::Vector< ara::core::Byte > const &issureSerialNum) const noexcept;
    /// @brief Generate CRL
    /// @param stCertName Certificate name
    /// @return CRL data information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04671
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Ipc_GenCRL(
        ara::core::StringView const &stCertName) const noexcept;

    /// @brief Get key slot name corresponding to the certificate
    /// @param stCertName Certificate name
    /// @return Key slot certificate name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04672
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::String > > Ipc_GetCertSlotNames(
        ara::core::StringView const &stCertName) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_DEAL_X509_H_
