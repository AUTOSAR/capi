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
/// @file       isoft_ipc_deal_x509.cpp
/// @brief      AutoSar-Crypto Key storage module
/// @details
/// @date       2022-11-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/X.509 certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcDealX509
/// @unit_description=Certificate IPC client
/// @endcode
///
/// ================================================================

#include "ara/crypto/ipc/isoft_ipc_deal_x509.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/ //
/// @brief Reinitialize
void PIpcDealX509::ReInit() const noexcept { aswMsg_.ReInit(); }
/// @brief Process LoadCert
/// @param stCertName Certificate name
/// @return Certificate byte data
ara::core::Result< ara::core::Vector< uint8_t > > PIpcDealX509::Ipc_LoadCert(
    ara::core::StringView const &stCertName) const noexcept
{
    PIpcClient *const pIpcClient{PIpcClient::GetInstance_X509()};
    if (false == pIpcClient->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the slot is not
    if (stCertName.empty()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{pIpcClient->DealIpcRequest(
        FUNC_NAME_CertProvider(LoadCert), aswMsg_, [stCertName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_LoadCert ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_LoadCert) + stCertName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_LoadCert *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_LoadCert * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    ara::core::Vector< uint8_t > vecData;
    uint8_t *const pWriteBuff{static_cast< uint8_t * >(static_cast< void * >(pIpcAsw + 1))};
    for (uint16_t i{0U}; i < pIpcAsw->GetCertLen(); ++i) {
        vecData.push_back(*(pWriteBuff + i));
    }
    return ara::core::Result< ara::core::Vector< uint8_t > >::FromValue(vecData);
}
/// @brief Process SaveCert
/// @return  >0 save sucess
/// @param stSlotName Key slot name
/// @param pData Data: starting address of memory
/// @param nLen Data length: in bytes
ara::core::Result< uint32_t > PIpcDealX509::Ipc_SaveCert(ara::core::StringView const &stSlotName,
                                                         uint8_t const *const pData,
                                                         uint16_t const nLen) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the slot is not
    if (stSlotName.empty()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(SaveCert), aswMsg_,
        [stSlotName, pData, nLen](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_SaveCert ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stSlotName.size());
                ipcReq.nDataLen = nLen;
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stSlotName.data(), static_cast< uint16_t >(stSlotName.size()));
                std::ignore     = pReqMsg->AddDataToIpc(pData, nLen);
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_SaveCert) + stSlotName.size() + nLen);
        })};
    if (false == bReturn) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_SaveCert *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_SaveCert * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< uint32_t >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< uint32_t >::FromValue(pIpcAsw->GetIpcCertID());
}
/// @brief Process RemoveCert
/// @return
/// @param stSlotName Key slot name
ara::core::Result< uint32_t > PIpcDealX509::Ipc_RemoveCert(ara::core::StringView const &stSlotName) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the slot is not
    if (stSlotName.empty()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(RemoveCert), aswMsg_, [stSlotName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_RemoveCert ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stSlotName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stSlotName.data(), static_cast< uint16_t >(stSlotName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_RemoveCert) + stSlotName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_RemoveCert *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_RemoveCert * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< uint32_t >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< uint32_t >::FromValue(pIpcAsw->GetIpcCertID());
}
/// @brief Process FindCertByDn
/// @param certIndex Certificate sequence number
/// @param subjectDn Subject DN
/// @param issuerDn Issuer DN
/// @param validityTimePoint
/// @return true if find cert false otherwise
bool PIpcDealX509::Ipc_FindCertByDn(std::size_t const &certIndex,
                                    x509::X509DN const &subjectDn,
                                    x509::X509DN const &issuerDn,
                                    time_t const validityTimePoint) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return false;
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(FindCertByDn), aswMsg_,
        [&subjectDn, &issuerDn, validityTimePoint, certIndex](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            ara::core::StringView const stSubjectDn{subjectDn.GetDnString().Value()};
            ara::core::StringView const stIssuerDn{issuerDn.GetDnString().Value()};
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_FindCertByDn ipcReq;
                ipcReq.validityTime  = validityTimePoint;
                ipcReq.nCertIndex    = certIndex;
                ipcReq.nSubjectDnLen = static_cast< uint16_t >(stSubjectDn.size());
                ipcReq.nIssuerDnLen  = static_cast< uint16_t >(stIssuerDn.size());
                std::ignore          = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(stSubjectDn.data(), static_cast< uint16_t >(stSubjectDn.size()));
                std::ignore = pReqMsg->AddDataToIpc(stIssuerDn.data(), static_cast< uint16_t >(stIssuerDn.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_FindCertByDn) + stSubjectDn.size()
                                           + stIssuerDn.size());
        })};
    return bReturn;
}
/// @brief Process FindCertByKeyIds
/// @throws Exception
/// @param subjectKeyId Subject key ID
/// @param authorityKeyId Certificate authority key ID
/// @return ture find sucess false otherwise
bool PIpcDealX509::Ipc_FindCertByKeyIds(ReadOnlyMemRegion const &subjectKeyId,
                                        ReadOnlyMemRegion const &authorityKeyId) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return false;
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(FindCertByKeyIds), aswMsg_,
        [&subjectKeyId, &authorityKeyId](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_FindCertByKeyIds ipcReq;
                ipcReq.nSubjectKeyIdLen   = static_cast< uint16_t >(subjectKeyId.size());
                ipcReq.nAuthorityKeyIdLen = static_cast< uint16_t >(authorityKeyId.size());
                std::ignore               = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(subjectKeyId.data(), static_cast< uint16_t >(subjectKeyId.size()));
                std::ignore
                    = pReqMsg->AddDataToIpc(authorityKeyId.data(), static_cast< uint16_t >(authorityKeyId.size()));
            }
            if (subjectKeyId[0U] == 0U) {
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_FindCertByKeyIds) + subjectKeyId.size()
                                           + authorityKeyId.size());
        })};
    return bReturn;
}
/// @brief Process FindCertBySn
/// @throws
/// @param sn SN data
/// @param issuerDn Issuer DN
/// @return true if find cert false otherwise
bool PIpcDealX509::Ipc_FindCertBySn(ReadOnlyMemRegion const &sn, x509::X509DN const &issuerDn) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return false;
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(FindCertBySn), aswMsg_,
        [&sn, &issuerDn](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            ara::core::StringView const stIssuerDn{issuerDn.GetDnString().Value()};
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_FindCertBySn ipcReq;
                ipcReq.nSnLen       = static_cast< uint16_t >(sn.size());
                ipcReq.nIssuerDnLen = static_cast< uint16_t >(stIssuerDn.size());
                std::ignore         = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore         = pReqMsg->AddDataToIpc(sn.data(), static_cast< uint16_t >(sn.size()));
                std::ignore = pReqMsg->AddDataToIpc(stIssuerDn.data(), static_cast< uint16_t >(stIssuerDn.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_FindCertBySn) + sn.size() + stIssuerDn.size());
        })};
    return bReturn;
}
/// @brief Return value of FindCertBy
/// @throws
/// @param stCertName Certificate name
/// @param certIndex Certificate sequence number
/// @return Certificate data
ara::core::Result< ara::core::Vector< uint8_t > > PIpcDealX509::Ipc_DealAswFindPacket(
    ara::core::String &stCertName, std::size_t &certIndex) const noexcept
{
    if (certIndex > 0U) {
    }  // for qac
    if (false == stCertName.empty()) {
    }  // for qac
    x509provider::PIpcAsw_FindCert *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_FindCert * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    certIndex = pIpcAsw->GetCertIndex();
    ara::core::Vector< uint8_t > vecData;
    uint8_t const *const pWriteBuff{pIpcAsw->GetCertData()};
    for (uint16_t i{0U}; i < pIpcAsw->GetCertDataLen(); ++i) {
        vecData.push_back(*(pWriteBuff + i));
    }
    stCertName = pIpcAsw->GetCertName();
    return ara::core::Result< ara::core::Vector< uint8_t > >::FromValue(vecData);
}
/// @brief Check whether the certificate slot exists
/// @param stCertName Certificate name
/// @return >0 cert exist
ara::core::Result< uint16_t > PIpcDealX509::Ipc_DealIsExist(ara::core::StringView const &stCertName) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< uint16_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the slot is not
    if (stCertName.empty()) {
        return ara::core::Result< uint16_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(IsCertExist), aswMsg_, [stCertName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_IsCertExist ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_IsCertExist) + stCertName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< uint16_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_IsCertExist *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_IsCertExist * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< uint16_t >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< uint16_t >::FromValue(pIpcAsw->GetFindLen());
}
/// @brief Check whether the process is the trustmaster process
/// @return true if is trust false otherwise
ara::core::Result< bool > PIpcDealX509::Ipc_IsTrustMaster() const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(IsTrustMaster), aswMsg_, [](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_IsTrustMaster const ipcReq;
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_IsTrustMaster));
        })};
    if (false == bReturn) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_IsTrustMaster *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_IsTrustMaster * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< bool >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< bool >::FromValue(pIpcAsw->GetIsSucess());
}
/// @brief Set root certificate
/// @param stCertName Certificate name
/// @param nIndex Index
/// @return true if set exinfo sucess false otherwise
ara::core::Result< bool > PIpcDealX509::Ipc_SetCertExInfo(ara::core::StringView const &stCertName,
                                                          uint16_t const nIndex) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(SetCertExInfo), aswMsg_,
        [stCertName, nIndex](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_SetCertExInfo ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                ipcReq.nIndex   = nIndex;
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_SetCertExInfo) + stCertName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_SetCertExInfo *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_SetCertExInfo * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< bool >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< bool >::FromValue(pIpcAsw->GetIsSucess());
}
/// @brief Get the name of the trusted root certificate
/// @return  Root certificate name
ara::core::Result< ara::core::Vector< ara::core::String > > PIpcDealX509::Ipc_GetAsRootOfTrust() const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(GetAsRootOfTrust), aswMsg_, [](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_GetAsRootOfTrust const ipcReq;
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_GetAsRootOfTrust));
        })};
    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_GetAsRootOfTrust *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_GetAsRootOfTrust * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            TransformErrorID(pIpcAsw->GetErrorID()));
    }

    ara::core::Vector< uint32_t > vecCertNameLens;
    uint32_t nIndex{0U};
    uint32_t nNum{0U};
    uint32_t *pWriteBuff{static_cast< uint32_t * >(static_cast< void * >(pIpcAsw + 1))};
    nNum = *pWriteBuff;
    pWriteBuff++;

    for (uint32_t i{0U}; i < nNum; i++) {
        vecCertNameLens.push_back(*pWriteBuff);
        pWriteBuff++;
    }
    ara::core::Vector< ara::core::String > nReturn;
    for (uint32_t &itData : vecCertNameLens) {
        nReturn.push_back(T_String(T_TransChar(pWriteBuff) + nIndex, static_cast< std::size_t >(itData)));
        nIndex += itData;
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(nReturn);
}

/// @brief Set root certificate
/// @param stCertName Certificate name
/// @return true has already set pending false otherwise
ara::core::Result< bool > PIpcDealX509::Ipc_IsCsrSetPending(ara::core::StringView const &stCertName) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(IsCsrSetPending), aswMsg_,
        [stCertName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_IsCsrSetPending ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_IsCsrSetPending) + stCertName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_IsCsrSetPending *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_IsCsrSetPending * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< bool >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< bool >::FromValue(pIpcAsw->GetIsSucess());
}
/// @brief Get all certificate signing request port names
/// @return  Certificate port name
ara::core::Result< ara::core::Vector< ara::core::String > > PIpcDealX509::Ipc_GetCsrNames() const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(Ipc_GetCsrNames), aswMsg_, [](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_GetCsrNames const ipcReq;
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_GetCsrNames));
        })};
    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_GetCsrNames *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_GetCsrNames * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            TransformErrorID(pIpcAsw->GetErrorID()));
    }

    ara::core::Vector< uint32_t > vecCertNameLens;
    uint32_t nIndex{0U};
    uint32_t nNum{0U};
    uint32_t *pWriteBuff{static_cast< uint32_t * >(static_cast< void * >(pIpcAsw + 1))};
    nNum = *pWriteBuff;
    pWriteBuff++;

    for (uint32_t i{0U}; i < nNum; i++) {
        vecCertNameLens.push_back(*pWriteBuff);
        pWriteBuff++;
    }
    ara::core::Vector< ara::core::String > nReturn;
    for (uint32_t &itData : vecCertNameLens) {
        nIndex += itData;
        nReturn.push_back(T_String(T_TransChar(pWriteBuff) + nIndex, static_cast< std::size_t >(itData)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(nReturn);
}

/// @brief OCSP request
/// @param certNames
/// @param ocspRestInfo
/// @return OCSP data information
ara::core::Result< ara::core::Vector< ara::core::Byte > > PIpcDealX509::Ipc_OcspRequest(
    ara::core::Vector< ara::core::StringView > const &certNames,
    ara::core::Vector< ara::core::Byte > const &ocspRestInfo) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    std::size_t certTotalLens{0U};
    for (auto const &itData : certNames) {
        certTotalLens += itData.size();
    }
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(Ipc_OcspResQuest), aswMsg_,
        [certNames, ocspRestInfo, certTotalLens](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_OcspReuest ipcReq{};
                ipcReq.nOcspResInfoLen = static_cast< uint16_t >(ocspRestInfo.size());
                std::size_t const nCertsLen{certNames.size()};
                ipcReq.nIssureCertNum = static_cast< uint16_t >(nCertsLen);
                std::ignore           = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(ocspRestInfo.data(), static_cast< uint16_t >(ocspRestInfo.size()));
                for (std::size_t i{0U}; i < nCertsLen; i++) {
                    std::size_t const ncertLen{certNames[i].size()};
                    std::ignore = pReqMsg->AddDataToIpc(&ncertLen, sizeof(ncertLen));
                    std::ignore
                        = pReqMsg->AddDataToIpc(T_TransChar(certNames[i].data()), static_cast< uint16_t >(ncertLen));
                }
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_OcspReuest) + certTotalLens
                                           + certNames.size() * sizeof(std::size_t));
        })};
    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_OcspReuest *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_OcspReuest * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            TransformErrorID(pIpcAsw->GetErrorID()));
    }

    uint8_t const *const resPonseData{pIpcAsw->GetOcspResponseInfo()};
    uint16_t const nLen{pIpcAsw->GetOcspResponseInfoLen()};

    ara::core::Vector< ara::core::Byte > vecReturn;
    vecReturn.reserve(static_cast< size_t >(nLen));

    for (uint16_t i{0U}; i < nLen; i++) {
        vecReturn.push_back(static_cast< ara::core::Byte >(*(resPonseData + i)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecReturn);
}
/// @brief IPC request for certificate revocation
/// @param stCertName Certificate name
/// @param issureSerialNum
/// @return true if revoke sucess false otherwise
ara::core::Result< bool > PIpcDealX509::Ipc_RevokeCert(
    ara::core::StringView const &stCertName, ara::core::Vector< ara::core::Byte > const &issureSerialNum) const noexcept
{
    PIpcClient *const pIpcClient{PIpcClient::GetInstance_X509()};
    if (false == pIpcClient->IsWorkerReady()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the slot is not
    if ((stCertName.empty()) || (issureSerialNum.empty())) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{pIpcClient->DealIpcRequest(
        FUNC_NAME_CertProvider(RevokeCert), aswMsg_,
        [stCertName, issureSerialNum](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_RevokeCert ipcReq;
                ipcReq.nNameLen            = static_cast< uint16_t >(stCertName.size());
                ipcReq.nIssureSerialNumLen = static_cast< uint16_t >(issureSerialNum.size());
                std::ignore                = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
                std::ignore
                    = pReqMsg->AddDataToIpc(issureSerialNum.data(), static_cast< uint16_t >(issureSerialNum.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_LoadCert) + stCertName.size()
                                           + issureSerialNum.size());
        })};

    if (false == bReturn) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    // Get the return result. The PIpcAsw_RevokeCert class stores the result of certificate revocation.
    x509provider::PIpcAsw_RevokeCert *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_RevokeCert * >(static_cast< void * >(aswMsg_.GetIpcBody()))};

    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< bool >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }

    return ara::core::Result< bool >::FromValue(pIpcAsw->GetRevokeSuccess());
}
/// @brief IPC request for generating CRL
/// @param stCertName Certificate name
/// @return CRL data information
ara::core::Result< ara::core::Vector< ara::core::Byte > > PIpcDealX509::Ipc_GenCRL(
    ara::core::StringView const &stCertName) const noexcept
{
    PIpcClient *const pIpcClient{PIpcClient::GetInstance_X509()};
    if (false == pIpcClient->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{pIpcClient->DealIpcRequest(
        FUNC_NAME_CertProvider(GenerateCRLists), aswMsg_,
        [stCertName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_GenLoadCert ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_GenLoadCert) + stCertName.size());
        })};

    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    // Get the returned CRL result
    x509provider::PIpcAsw_GenCRL *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_GenCRL * >(static_cast< void * >(aswMsg_.GetIpcBody()))};

    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            TransformErrorID(pIpcAsw->GetErrorID()));
    }

    // The return type is char*, which needs to be converted to ReadOnlyMemRegion
    uint8_t *const pWriteBuff{static_cast< uint8_t * >(static_cast< void * >(pIpcAsw + 1))};

    ara::core::Vector< ara::core::Byte > vecData;
    for (size_t i{0U}; i < pIpcAsw->GetDataLen(); ++i) {
        vecData.push_back(static_cast< ara::core::Byte >(*(pWriteBuff + i)));
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}

/// @brief Get the key slot name corresponding to the certificate
/// @param stCertName Certificate name
/// @return Key slot certificate name
ara::core::Result< ara::core::Vector< ara::core::String > > PIpcDealX509::Ipc_GetCertSlotNames(
    ara::core::StringView const &stCertName) const noexcept
{
    if (false == PIpcClient::GetInstance_X509()->IsWorkerReady()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Here you can also use the pattern of grouping packets => sending => unpacking return packets
    bool const bReturn{PIpcClient::GetInstance_X509()->DealIpcRequest(
        FUNC_NAME_CertProvider(Ipc_GetCertSlotNames), aswMsg_,
        [stCertName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                x509provider::PIpcReq_GetCertSlotNames ipcReq;
                ipcReq.nNameLen = static_cast< uint16_t >(stCertName.size());
                std::ignore     = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore     = pReqMsg->AddDataToIpc(stCertName.data(), static_cast< uint16_t >(stCertName.size()));
            }
            return static_cast< uint16_t >(sizeof(x509provider::PIpcReq_GetCertSlotNames) + stCertName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }
    x509provider::PIpcAsw_GetCertSlotNames *const pIpcAsw{
        static_cast< x509provider::PIpcAsw_GetCertSlotNames * >(static_cast< void * >(aswMsg_.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            TransformErrorID(pIpcAsw->GetErrorID()));
    }

    ara::core::Vector< ara::core::String > vecSlotNames;
    ara::core::String const nSlotNameOne{pIpcAsw->GetSlotNameOneString()};
    if (!nSlotNameOne.empty()) {
        vecSlotNames.push_back(nSlotNameOne);
    }
    ara::core::String const nSlotNameTwo{pIpcAsw->GetSlotNameTwoString()};
    if (!nSlotNameTwo.empty()) {
        vecSlotNames.push_back(nSlotNameTwo);
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(vecSlotNames);
}

//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
