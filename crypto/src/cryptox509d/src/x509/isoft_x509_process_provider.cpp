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
/// @file       isoft_x509_process_provider.cpp
/// @brief      IPC server-side of X509Provider: Certificate provider logic processing
/// @details
/// @date       2023-09-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/Certificate Provider IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_04007
/// @unit_name=PX509Process_Provider
/// @unit_description=Certificate Provider IPC Service
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_x509_process_provider.h"

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/manifest/manifest_crypto_data.h"
#include "ara/crypto/manifest/manifest_ksp_config.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/openssl/isoft_openssl_gencrl.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/isoft_svr_cert_loader.h"
#include "ara/crypto/x509/isoft_x509_manager.h"
#include "ara/crypto/x509/isoft_x509_provider.h"
#include "ara/exec/internal/find_process_client.h"
#include "openssl/pem.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief Certificate search callback function type definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02875
/// @trace_id_dd=DD_CRYPTO_06467
/// @needwork = dd
/// @endcode
using CB_FindCert = std::function< bool(x509::Certificate *pCert) >;
namespace {
/// @brief Finds certificate
/// @param stCertName Certificate name
/// @param nDeleteFlag Deletion flag
/// @param lpcProcessManager LCP command processing manager
/// @param x509Provider Certificate provider
/// @param nProcessID Process ID
/// @param nStartIndex Start index
/// @param pFindFunc Search function callback
/// @return Certificate information
ara::core::Result< ara::core::Vector< uint8_t > > FindCert(ara::core::String &stCertName,
                                                           uint32_t const &nDeleteFlag,
                                                           PX509_Manager const &lpcProcessManager,
                                                           x509::isoft_def::PX509Provider &x509Provider,
                                                           uint32_t const nProcessID,
                                                           uint32_t const nStartIndex,
                                                           CB_FindCert const &pFindFunc) noexcept
{
    std::ignore = nProcessID;
    std::ignore = nStartIndex;
    std::ignore = nDeleteFlag;
    std::ignore = pFindFunc;
    if (false == stCertName.empty()) {
    }  // for qac
    ara::per::SharedHandle< ara::per::FileStorage > pFileStorage{PSvrCertLoader::GetCertStoragePtr()};
    if (nullptr == pFindFunc) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    if (false == pFileStorage.operator bool()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(SecurityErrorDomain::Errc::kBusyResource);
    }
    ara::core::Result< ara::core::Vector< ara::core::String > > const resultAllFile{pFileStorage->GetAllFileNames()};
    if (false == resultAllFile.HasValue()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    if (resultAllFile.Value().empty()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::Vector< uint8_t > vecReturn;
    for (uint32_t i{nStartIndex}; i < resultAllFile.Value().size(); ++i) {
        ara::core::StringView const stFileName{resultAllFile.Value().at(static_cast< size_t >(i))};
        ara::core::String certName;
        size_t const fileLen{stFileName.size()};
        /// Convert file name to certificate name
        for (size_t i1{0U}; i1 < fileLen; i1++) {
            if (stFileName[i1] == '_') {
                if ((i1 + 1U) < fileLen) {
                    if (stFileName[i1 + 1U] == '_') {
                        i1++;
                        certName += '/';
                    } else {
                        certName += stFileName[i1];
                    }
                } else {
                    certName += stFileName[i1];
                }
            } else {
                certName += stFileName[i1];
            }
        }
        PIoInterface_File::Uptr const pIoInterface{
            PSvrCertLoader::NewCertSlotIo(lpcProcessManager.GetCryptoProvider(), certName)};
        if (false == pIoInterface.operator bool()) {
            continue;
        }
        ara::core::Vector< ara::core::Byte > vecData;
        if (false == pIoInterface->ReadCertData(vecData)) {
            continue;
        }
        if (vecData.empty()) {
            continue;
        }
        ara::core::Byte *const pData{vecData.data()};
        ReadOnlyMemRegion const memCert{reinterpret_cast< uint8_t * >(pData), vecData.size()};
        ara::core::Result< ara::crypto::x509::Certificate::Uptr > const resultX509{
            x509Provider.ParseCert(memCert, Serializable::kFormatDefault)};
        if (resultX509.HasValue() == false) {
            continue;
        }
        x509::Certificate *const pCertInStorage{resultX509.Value().get()};
        if (pCertInStorage == nullptr) {
            continue;
        }
        pCertInStorage->SetCertSlotName(certName);
        if (false == pFindFunc(pCertInStorage)) {
            continue;
        }
        for (size_t j{0U}; j < vecData.size(); j++) {
            vecReturn.push_back(static_cast< uint8_t >(vecData[j]));
        }
        stCertName = certName;
        break;
    }
    return ara::core::Result< ara::core::Vector< uint8_t > >::FromValue(vecReturn);
}

/// @brief Finds certificate
/// @return Root certificate
ara::core::Result< ara::core::Vector< ara::core::String > > FindRootCert() noexcept
{
    ara::per::SharedHandle< ara::per::FileStorage > pFileStorage{PSvrCertLoader::GetCertStoragePtr()};
    if (false == pFileStorage.operator bool()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    ara::core::Result< ara::core::Vector< ara::core::String > > const resultAllFile{pFileStorage->GetAllFileNames()};
    if (false == resultAllFile.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    if (resultAllFile.Value().empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::Vector< ara::core::String > vecReturn;
    for (uint32_t i{0U}; i < resultAllFile.Value().size(); ++i) {
        ara::core::String stFileName{resultAllFile.Value().at(static_cast< size_t >(i))};
        ara::per::isoftkv::PFileInfoInKvs_ReddExt const pFileInfoExt{
            pFileStorage->ReadFileInfoEx(T_StringView(stFileName), 0U)};
        if (pFileInfoExt.chReserve[0] == 1U) {
            ara::core::String certName;
            size_t const fileLen{stFileName.size()};
            /// Convert file name to certificate name
            for (size_t i1{0U}; i1 < fileLen; i1++) {
                if (stFileName[i1] == '_') {
                    if ((i1 + 1U) < fileLen) {
                        if (stFileName[i1 + 1U] == '_') {
                            i1++;
                            certName += '/';
                        } else {
                            certName += stFileName[i1];
                        }
                    } else {
                        certName += stFileName[i1];
                    }
                } else {
                    certName += stFileName[i1];
                }
            }
            vecReturn.push_back(certName);
        }
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(vecReturn);
}

/// @brief Finds CSR
/// @param lpcProcessManager LCP command processing manager
/// @return Certificate Signing Request information
ara::core::Result< ara::core::Vector< ara::core::String > > FindCSR(PX509_Manager const &lpcProcessManager) noexcept
{
    ara::per::SharedHandle< ara::per::FileStorage > pFileStorage{PSvrCertLoader::GetCertStoragePtr()};
    if (false == pFileStorage.operator bool()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    ara::core::Result< ara::core::Vector< ara::core::String > > const resultAllFile{pFileStorage->GetAllFileNames()};
    if (false == resultAllFile.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    if (resultAllFile.Value().empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(
            SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::Vector< ara::core::String > vecReturn;
    x509::isoft_def::PX509Provider x509Provider;
    for (uint32_t i{0U}; i < resultAllFile.Value().size(); ++i) {
        ara::core::StringView const stFileName{resultAllFile.Value().at(static_cast< size_t >(i))};
        ara::core::String certName;
        size_t const fileLen{stFileName.size()};
        /// Convert file name to certificate name
        for (size_t i1{0U}; i1 < fileLen; i1++) {
            if (stFileName[i1] == '_') {
                if ((i1 + 1U) < fileLen) {
                    if (stFileName[i1 + 1U] == '_') {
                        i1++;
                        certName += '/';
                    } else {
                        certName += stFileName[i1];
                    }
                } else {
                    certName += stFileName[i1];
                }
            } else {
                certName += stFileName[i1];
            }
        }
        PIoInterface_File::Uptr const pIoInterface{
            PSvrCertLoader::NewCertSlotIo(lpcProcessManager.GetCryptoProvider(), certName)};
        if (false == pIoInterface.operator bool()) {
            continue;
        }
        ara::core::Vector< ara::core::Byte > vecData;
        if (false == pIoInterface->ReadCertData(vecData)) {
            continue;
        }
        if (vecData.empty()) {
            continue;
        }
        ara::core::Byte *const pData{vecData.data()};
        ReadOnlyMemRegion const memCert{reinterpret_cast< uint8_t * >(pData), vecData.size()};
        ara::core::Result< ara::crypto::x509::Certificate::Uptr > const resultX509{
            x509Provider.ParseCert(memCert, Serializable::kFormatDefault)};
        if (resultX509.HasValue() == true) {
            continue;
        }
        vecReturn.push_back(certName);
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(vecReturn);
}

}  // namespace
//********************************/
/// @brief Constructor: initializes mapProcessCmd_
/// @param lpcProcessManager LCP command processing manager
PX509Process_Provider::PX509Process_Provider(PX509_Manager &lpcProcessManager) noexcept
    : PX509Process_T_Base< PX509Process_Provider >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_CertProvider(IsCertExist), &PX509Process_Provider::IsCertExist);
    _InsertMapCmd(FUNC_NAME_CertProvider(LoadCert), &PX509Process_Provider::LoadCert);
    _InsertMapCmd(FUNC_NAME_CertProvider(SaveCert), &PX509Process_Provider::SaveCert);
    _InsertMapCmd(FUNC_NAME_CertProvider(RemoveCert), &PX509Process_Provider::RemoveCert);
    _InsertMapCmd(FUNC_NAME_CertProvider(FindCertByDn), &PX509Process_Provider::FindCertByDn);
    _InsertMapCmd(FUNC_NAME_CertProvider(FindCertByKeyIds), &PX509Process_Provider::FindCertByKeyIds);
    _InsertMapCmd(FUNC_NAME_CertProvider(FindCertBySn), &PX509Process_Provider::FindCertBySn);
    _InsertMapCmd(FUNC_NAME_CertProvider(IsTrustMaster), &PX509Process_Provider::IsTrustMaster);
    _InsertMapCmd(FUNC_NAME_CertProvider(SetCertExInfo), &PX509Process_Provider::SetCertExInfo);
    _InsertMapCmd(FUNC_NAME_CertProvider(GetAsRootOfTrust), &PX509Process_Provider::GetAsRootOfTrust);
    _InsertMapCmd(FUNC_NAME_CertProvider(IsCsrSetPending), &PX509Process_Provider::IsCsrSetPending);
    _InsertMapCmd(FUNC_NAME_CertProvider(Ipc_GetCsrNames), &PX509Process_Provider::Ipc_GetCsrNames);
    _InsertMapCmd(FUNC_NAME_CertProvider(Ipc_OcspResQuest), &PX509Process_Provider::Ipc_OcspResQuest);

    // Revoke a certificate
    _InsertMapCmd(FUNC_NAME_CertProvider(RevokeCert), &PX509Process_Provider::RevokeCert);

    // Generate CRL or delta CRL
    _InsertMapCmd(FUNC_NAME_CertProvider(GenerateCRLists), &PX509Process_Provider::GenerateCRLists);

    _InsertMapCmd(FUNC_NAME_CertProvider(Ipc_GetCertSlotNames), &PX509Process_Provider::Ipc_GetCertSlotNames);
}

//********************************/
/// @brief Checks if the certificate exists
/// @name   IsCertExist
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::IsCertExist(PIpcPac_Head const *const pReqHead,
                                                                 PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::IsCertExist";
    keys::isoft_def::x509provider::PIpcReq_IsCertExist const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_IsCertExist >()};
    ara::core::StringView const stCertName{
        static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pPacReq + 1)),
        static_cast< std::size_t >(pPacReq->nNameLen)};

    ara::core::Result< uint32_t > const result{lpcProcessManager_.IsCertExist(stCertName)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_IsCertExist))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_IsCertExist *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_IsCertExist >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetFindLen(static_cast< uint16_t >(result.Value()));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Loads certificate
/// @name  LoadCert
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::LoadCert(PIpcPac_Head const *const pReqHead,
                                                              PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::LoadCert";
    keys::isoft_def::x509provider::PIpcReq_LoadCert const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_LoadCert >()};
    ara::core::StringView const stCertName{
        static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pPacReq + 1)),
        static_cast< std::size_t >(pPacReq->nNameLen)};

    ara::core::Vector< uint8_t > vecData;
    uint32_t const nDeleteFlag{0U};
    ara::core::Result< uint32_t > const result{lpcProcessManager_.LoadCert(stCertName, vecData)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    size_t const nDatalen{vecData.size()};
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::x509provider::PIpcAsw_LoadCert))
        + static_cast< uint16_t >(nDatalen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_LoadCert *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_LoadCert >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIpcCertID(result.Value());
    pPacAsw->SetCertLen(static_cast< uint16_t >(nDatalen));
    pPacAsw->SetDeleteFlag(nDeleteFlag);
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), vecData.data(), nDatalen);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Saves certificate
/// @name  SaveCert
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::SaveCert(PIpcPac_Head const *const pReqHead,
                                                              PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::SaveCert";
    keys::isoft_def::x509provider::PIpcReq_SaveCert const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_SaveCert >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::Result< uint32_t > const result{
        lpcProcessManager_.SaveCert(pPacReq->GetName(), pPacReq->GetData(), pPacReq->nDataLen)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_SaveCert))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_SaveCert *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_SaveCert >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIpcCertID(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Deletes certificate
/// @name  RemoveCert
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::RemoveCert(PIpcPac_Head const *const pReqHead,
                                                                PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::RemoveCert";
    keys::isoft_def::x509provider::PIpcReq_RemoveCert const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_RemoveCert >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::Result< uint32_t > const result{lpcProcessManager_.RemoveCert(pPacReq->GetName())};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_RemoveCert))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_RemoveCert *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_RemoveCert >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIpcCertID(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Finds certificate by DN
/// @name  FindCertByDn
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::FindCertByDn(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::FindCertByDn";
    keys::isoft_def::x509provider::PIpcReq_FindCertByDn const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_FindCertByDn >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::String stCertName;
    uint32_t const nDeleteFlag{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const result{
        _FindCertByDn(stCertName, nDeleteFlag, pReqHead->nProcessID, pPacReq)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    return AssembleFindCertMsg(pReqHead, aswMsg, stCertName, nDeleteFlag, result.Value());
}
/// @brief Finds certificate by KeyID
/// @name  FindCertByKeyIds
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::FindCertByKeyIds(PIpcPac_Head const *const pReqHead,
                                                                      PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::FindCertByKeyIds";
    keys::isoft_def::x509provider::PIpcReq_FindCertByKeyIds const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_FindCertByKeyIds >()};
    ara::core::String stCertName;
    uint32_t const nDeleteFlag{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const result{
        _FindCertByKey(stCertName, nDeleteFlag, pReqHead->nProcessID, pPacReq)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    return AssembleFindCertMsg(pReqHead, aswMsg, stCertName, nDeleteFlag, result.Value());
}
/// @brief Finds certificate by SN
/// @name  FindCertBySn
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PX509Process_Base::PResultLen
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::FindCertBySn(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::FindCertBySn";
    keys::isoft_def::x509provider::PIpcReq_FindCertBySn const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_FindCertBySn >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::String stCertName;
    uint32_t const nDeleteFlag{0U};
    ara::core::Result< ara::core::Vector< uint8_t > > const result{
        _FindCertBySn(stCertName, nDeleteFlag, pReqHead->nProcessID, pPacReq)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    return AssembleFindCertMsg(pReqHead, aswMsg, stCertName, nDeleteFlag, result.Value());
}
/// @brief Checks if the process is a TrustMaster process
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return true if is trust master false otherwise
PX509Process_Base::PResultLen PX509Process_Provider::IsTrustMaster(PIpcPac_Head const *const pReqHead,
                                                                   PIpcAutoPacket &aswMsg) const noexcept
{
    bool isTrustMasterProcess{false};
#ifndef ARA_DEBUG_TRUSTMASTER
    ara::exec::internal::FindProcessClient const findProcessClient;
    ara::core::String nProcessName;
    ara::core::Result< void > const resVoid{
        findProcessClient.FindByPid(static_cast< uint32_t >(pReqHead->nProcessID), nProcessName)};
    if (resVoid.HasValue() == false) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }

    ara::core::Result< bool > const resBool{lpcProcessManager_.IsTrustMaster(nProcessName.c_str())};
    if (resBool.HasValue() == false) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    isTrustMasterProcess = resBool.Value();
#else
    isTrustMasterProcess = true;
#endif
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_IsTrustMaster))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_IsTrustMaster *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_IsTrustMaster >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIsSucess(isTrustMasterProcess);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Sets certificate extension information
/// @name   SetCertExInfo
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::SetCertExInfo(PIpcPac_Head const *const pReqHead,
                                                                   PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::SetCertExInfo";
    keys::isoft_def::x509provider::PIpcReq_SetCertExInfo const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_SetCertExInfo >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::Result< bool > const result{lpcProcessManager_.SetCertExInfo(pPacReq->GetName(), pPacReq->nIndex)};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_SetCertExInfo))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_SetCertExInfo *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_SetCertExInfo >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIsSucess(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Loads root certificate from persistent file and returns the corresponding certificate name
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::GetAsRootOfTrust(PIpcPac_Head const *const pReqHead,
                                                                      PIpcAutoPacket &aswMsg) const noexcept
{
    std::ignore = kCrlIndexFileName;
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::GetAsRootOfTrust";
    ara::core::Result< ara::core::Vector< ara::core::String > > resVec{FindRootCert()};
    if (!resVec.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    if (resVec.Value().empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    /// vecData contains the returned root certificate names; there may be multiple root certificates
    ara::core::Vector< ara::core::String > vecData{std::move(resVec).Value()};
    /// retLen represents the total length of all certificate names
    size_t retLen{0U};
    for (size_t i{0U}; i < vecData.size(); i++) {
        retLen += vecData[i].size();
    }
    /// This response packet consists of: packet header length + struct length + number of certificates + lengths of each certificate name + strings of each certificate name
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::x509provider::PIpcAsw_GetAsRootOfTrust)
                                                   + (vecData.size() + 1U) * sizeof(uint32_t) + retLen)};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_GetAsRootOfTrust *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_GetAsRootOfTrust >()};

    int32_t nIndex{0};
    /// pDataCertName points to the address storing the certificate name strings in the packet
    uint8_t *const pDataCertName{static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1))
                                 + (vecData.size() + 1U) * sizeof(uint32_t)};
    /// pDataCertNumSize points to the address storing the number of certificates in the packet; increments to point to the lengths of each certificate name
    uint32_t *pDataCertNumSize{static_cast< uint32_t * >(static_cast< void * >(pPacAsw + 1))};
    *pDataCertNumSize = static_cast< uint32_t >(vecData.size());
    pDataCertNumSize++;
    for (size_t i{0U}; i < vecData.size(); i++) {
        *pDataCertNumSize = static_cast< uint32_t >(vecData[i].size());
        pDataCertNumSize++;
        std::ignore = memcpy(T_TransVoid(pDataCertName + nIndex), T_TransVoid(vecData[i].data()), vecData[i].size());
        nIndex += static_cast< int32_t >(vecData[i].size());
    }
    pPacAsw->SetErrorID(0);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Checks if the certificate is set to pending
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::IsCsrSetPending(PIpcPac_Head const *const pReqHead,
                                                                     PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::IsCsrSetPending";
    keys::isoft_def::x509provider::PIpcReq_IsCsrSetPending const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_IsCsrSetPending >()};
    // 2022-08-11 Temporary simplified processing
    ara::core::Result< bool > const result{lpcProcessManager_.IsCsrSetPending(pPacReq->GetName())};
    if (false == result.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::x509provider::PIpcAsw_SetCertExInfo))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_IsCsrSetPending *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_IsCsrSetPending >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIsSucess(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Loads CSR from persistent file and returns the corresponding certificate name
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::Ipc_GetCsrNames(PIpcPac_Head const *const pReqHead,
                                                                     PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::Ipc_GetCsrNames";
    ara::core::Result< ara::core::Vector< ara::core::String > > resVec{FindCSR(lpcProcessManager_)};
    if (!resVec.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    if (resVec.Value().empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    /// vecData contains the returned root certificate names; there may be multiple root certificates
    ara::core::Vector< ara::core::String > vecData{std::move(resVec).Value()};
    /// retLen represents the total length of all certificate names
    size_t retLen{0U};
    for (size_t i{0U}; i < vecData.size(); i++) {
        retLen += vecData[i].size();
    }
    /// This response packet consists of: packet header length + struct length + number of certificates + lengths of each certificate name + strings of each certificate name
    uint16_t const nAswLen{static_cast< uint16_t >(
        static_cast< size_t >(pReqHead->GetHeadLen()) + sizeof(keys::isoft_def::x509provider::PIpcAsw_GetCsrNames)
        + (vecData.size() + 1U) * sizeof(uint32_t) + static_cast< size_t >(retLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_GetCsrNames *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_GetCsrNames >()};

    int32_t nIndex{0};
    /// pDataCertName points to the address storing the certificate name strings in the packet
    uint8_t *const pDataCertName{static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1))
                                 + (vecData.size() + 1U) * sizeof(uint32_t)};
    /// pDataCertNumSize points to the address storing the number of certificates in the packet; increments to point to the lengths of each certificate name
    uint32_t *pDataCertNumSize{static_cast< uint32_t * >(static_cast< void * >(pPacAsw + 1))};
    *pDataCertNumSize = static_cast< uint32_t >(vecData.size());
    pDataCertNumSize++;
    for (size_t i{0U}; i < vecData.size(); i++) {
        *pDataCertNumSize = static_cast< uint32_t >(vecData[i].size());
        pDataCertNumSize++;
        std::ignore = memcpy(T_TransVoid(pDataCertName + nIndex), T_TransVoid(vecData[i].data()), vecData[i].size());
        nIndex += static_cast< int32_t >(vecData[i].size());
    }
    pPacAsw->SetErrorID(0);
    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Gets the slot names corresponding to the certificate (0-2)
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::Ipc_GetCertSlotNames(PIpcPac_Head const *const pReqHead,
                                                                          PIpcAutoPacket &aswMsg) const noexcept
{
    LogInfo() << "PX509Process_Provider::Ipc_GetCertSlotNames";
    keys::isoft_def::x509provider::PIpcReq_GetCertSlotNames const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_GetCertSlotNames >()};
    ara::core::Vector< ara::core::String > vecData{lpcProcessManager_.FindRealSlotNames(pPacReq->GetName())};
    if (vecData.empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    /// retLen represents the total length of all slot names
    size_t retLen{0U};
    for (size_t i{0U}; i < vecData.size(); i++) {
        retLen += vecData[i].size();
    }
    /// This response packet consists of: packet header length + struct length + number of certificates + lengths of each certificate name + strings of each certificate name
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::x509provider::PIpcAsw_GetCertSlotNames)
                                                   + static_cast< size_t >(retLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_GetCertSlotNames *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_GetCertSlotNames >()};

    int32_t nSlotOneLen{0};
    int32_t nSlotTwoLen{0};
    int32_t nIndex{0};
    /// pDataCertName points to the address storing the slot name strings in the packet
    uint8_t *const pDataSlotName{static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1))};
    std::size_t const vecSize{vecData.size()};

    for (size_t i{0U}; i < vecSize; i++) {
        std::ignore = memcpy(T_TransVoid(pDataSlotName + nIndex), T_TransVoid(vecData[i].data()), vecData[i].size());
        if (i == 0U) {
            nSlotOneLen = static_cast< int32_t >(vecData[0U].size());
        }
        if (i == 1U) {
            nSlotTwoLen = static_cast< int32_t >(vecData[1U].size());
        }
        nIndex += static_cast< int32_t >(vecData[i].size());
    }
    pPacAsw->SetSlotNameOne(static_cast< uint16_t >(nSlotOneLen));
    pPacAsw->SetSlotNameTwo(static_cast< uint16_t >(nSlotTwoLen));
    pPacAsw->SetErrorID(0);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Creates a unique smart pointer
/// @param handle Template parameter: T requests resources
/// @param deleter Template parameter: D releases resources
/// @return Smart pointer object
/// @code{.isoft}
/// @tparam T
/// @tparam D
/// @interface_level=unit
/// @needwork = no
/// @endcode
template < typename T, typename D >
std::unique_ptr< T, D > Make_Handle(T *const handle, D const deleter) noexcept
{
    return std::unique_ptr< T, D >{handle, deleter};
}
/// @brief Processes OCSP request
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::Ipc_OcspResQuest(PIpcPac_Head const *const pReqHead,
                                                                      PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::Ipc_OcspResQuest";

    keys::isoft_def::x509provider::PIpcReq_OcspReuest const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_OcspReuest >()};

    uint8_t const *const pReqInfo{pPacReq->GetOcspResInfo()};
    uint16_t const pReqInfoLen{pPacReq->nOcspResInfoLen};
    /// Generate OCSP request
    auto const pBio{Make_Handle(BIO_new_mem_buf(pReqInfo, static_cast< int32_t >(pReqInfoLen)), BIO_free)};
    OCSP_REQUEST *const req{d2i_OCSP_REQUEST_bio(pBio.get(), nullptr)};  // NOLINT
    if (req == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    x509::isoft_def::PX509Provider x509Provider;
    ara::core::Vector< X509 * > vecX509s;
    /// Get the corresponding issuer certificate
    uint16_t const nIssureCertNums{pPacReq->nIssureCertNum};
    uint8_t const *pCertsData{pPacReq->GetIssureCert()};
    std::size_t const *ncertLen{nullptr};

    for (uint16_t i{0U}; i < nIssureCertNums; i++) {
        ncertLen = reinterpret_cast< std::size_t const * >(pCertsData);
        pCertsData += sizeof(std::size_t);
        ara::core::StringView const certName{reinterpret_cast< char8_t const * >(pCertsData), *ncertLen};
        pCertsData += *ncertLen;
        ara::core::Vector< uint8_t > vecCertData;
        auto const resultLoadCert{lpcProcessManager_.LoadCert(certName, vecCertData)};
        if (resultLoadCert.HasValue()) {
            uint8_t *const pData{vecCertData.data()};
            ReadOnlyMemRegion const memCert{pData, vecCertData.size()};
            auto resultCertParse{x509Provider.ParseCert(memCert)};
            if (resultCertParse.HasValue()) {
                ara::crypto::x509::Certificate::Uptr const pCertificate{std::move(resultCertParse).Value()};
                vecX509s.push_back(pCertificate->GetX509());
            }
        }
    }
    if (vecX509s.empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    STACK_OF(X509) *const rcaCerts{sk_X509_new_null()};  // NOLINT
    if (rcaCerts == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    for (std::size_t i{0U}; i < vecX509s.size(); i++) {
        if (sk_X509_push(rcaCerts, vecX509s[i]) == 0) {
            continue;
        }
    }

    X509 *const rsignerCert{openssl::isoft_def::LoadIssureCert()};
    EVP_PKEY *const psignerKey{openssl::isoft_def::LoadIssureKey()};

    OCSP_RESPONSE *resp{nullptr};
    EVP_MD const *const rsignMd{EVP_sha1()};
    uint64_t const flags{0U};
    int32_t const nmin{0};
    int32_t const ndays{1};
    int32_t const badsig{0};

    openssl::isoft_def::CA_DB *const newdb{
        static_cast< openssl::isoft_def::CA_DB * >(_LoadAndUpdateDb(kCrlIndexFileName))};

    if (newdb == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    openssl::isoft_def::Make_ocsp_response(&resp, req, newdb, rcaCerts, rsignerCert, psignerKey, rsignMd, flags, nmin,
                                           ndays, badsig);
    if (resp == nullptr) {
        openssl::isoft_def::Free_index(newdb);
        sk_X509_pop_free(rcaCerts, X509_free);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    auto const pResponDerData{Make_Handle(BIO_new(BIO_s_mem()), BIO_free)};
    int32_t const ret{ASN1_i2d_bio(reinterpret_cast< i2d_of_void * >(i2d_OCSP_RESPONSE), pResponDerData.get(),
                                   reinterpret_cast< uint8_t * >(resp))};
    if (ret <= 0) {
        Free_index(newdb);
        sk_X509_pop_free(rcaCerts, X509_free);
    }
    int32_t const nNeedLen{PH_BIO_pending(pResponDerData.get())};
    /// This response packet consists of: packet header length + struct length + number of certificates + lengths of each certificate name + strings of each certificate name
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::x509provider::PIpcAsw_OcspReuest)
                                                   + static_cast< size_t >(nNeedLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_OcspReuest *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_OcspReuest >()};

    uint8_t *const pRspInfo{static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1))};

    std::ignore = BIO_read(pResponDerData.get(), pRspInfo, nNeedLen);
    pPacAsw->SetOcspResponseInfoLen(static_cast< uint16_t >(nNeedLen));
    pPacAsw->SetErrorID(0);

    Free_index(newdb);
    sk_X509_pop_free(rcaCerts, X509_free);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Revokes a certificate
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Provider::PResultLen PX509Process_Provider::RevokeCert(PIpcPac_Head const *const pReqHead,
                                                                    PIpcAutoPacket &aswMsg) const noexcept
{
    LogInfo() << "PX509Process_Provider::RevokeCert";
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    // Client sends the name of the certificate to be revoked
    keys::isoft_def::x509provider::PIpcReq_RevokeCert const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_RevokeCert >()};
    ara::core::StringView const stCertName{
        static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pPacReq + 1)),
        static_cast< std::size_t >(pPacReq->nNameLen)};
    ara::core::StringView const stSerialNum{pPacReq->GetSerialNum()};
    if ((stCertName.empty()) || (stSerialNum.empty())) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    // Find the certificate to be revoked from the certificate slot by its name
    ara::core::Vector< uint8_t > vecData;
    ara::core::Result< uint32_t > const resVecDataLen{lpcProcessManager_.LoadCert(stCertName, vecData)};
    if (false == resVecDataLen.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, resVecDataLen.Error().Value());
    }

    ara::crypto::ReadOnlyMemRegion memCert(vecData.data(), vecData.size());
    X509 *revCert = LoadCertFile(memCert, Serializable::kFormatDefault);
    if (revCert == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    bool const bRevokeSuccess{_RevokeCert(revCert)};

    // Convert local data to network data and send out, returning the revocation result (True/False)
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::x509provider::PIpcAsw_RevokeCert)))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);

    keys::isoft_def::x509provider::PIpcAsw_RevokeCert *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_RevokeCert >()};

    pPacAsw->SetRevokeSuccess(bRevokeSuccess);
    pPacAsw->SetErrorID(0);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Generates CRL
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return
/// @throws
PX509Process_Base::PResultLen PX509Process_Provider::GenerateCRLists(PIpcPac_Head const *const pReqHead,
                                                                     PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    LogInfo() << "PX509Process_Provider::GenerateCRLists";

    // Generating CRL requires a certificate, so the certificate name must be passed in
    // Upon receiving the request, execute the CRL generation logic to obtain the CRL result

    // Try without signing first
    keys::isoft_def::x509provider::PIpcReq_GenLoadCert const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::x509provider::PIpcReq_GenLoadCert >()};
    ara::core::StringView const stCertName{
        static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pPacReq + 1)),
        static_cast< std::size_t >(pPacReq->nNameLen)};

    // Find the certificate to be revoked from the certificate slot by its name
    ara::core::Vector< uint8_t > vecData;
    ara::core::Result< uint32_t > const resVecDataLen{lpcProcessManager_.LoadCert(stCertName, vecData)};
    if (false == resVecDataLen.HasValue()) {
        return PX509Process_Base::ProcessCmd_Error(pReqHead, aswMsg, resVecDataLen.Error().Value());
    }

    uint8_t *pData{vecData.data()};
    // Obtain the certificate and execute the revocation logic
    X509 *const x509Root{
        d2i_X509(nullptr, const_cast< uint8_t const ** >(&pData), static_cast< int64_t >(resVecDataLen.Value()))};

    // Obtain the key corresponding to the certificate for subsequent signing
    EVP_PKEY *const pkey{openssl::isoft_def::LoadIssureKey()};
    if (nullptr == pkey) {
        LogInfo() << "pkey is nullptr!";
        return PResultLen::FromError(SecurityErrorDomain::Errc::kGenerateCRLFault);
    }

    int32_t pemLen{0};
    ara::core::Vector< uint8_t > pemCRLData{_GenerateCRL(pemLen, x509Root, pkey)};
    if (pemCRLData.data() == nullptr) {  // , x509, pkey
        return PResultLen::FromError(SecurityErrorDomain::Errc::kGenerateCRLFault);
    }

    // Convert local data to network data and send out, returning the generated CRL result
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::x509provider::PIpcAsw_GenCRL))
        + static_cast< uint16_t >(pemLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);

    keys::isoft_def::x509provider::PIpcAsw_GenCRL *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_GenCRL >()};

    pPacAsw->SetDataLen(static_cast< uint16_t >(pemLen));
    pPacAsw->SetErrorID(0);
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pemCRLData.data(),
                         static_cast< size_t >(pemLen));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Finds certificate
/// @name  _FindCertByDn
/// @param stCertName Certificate name
/// @param nDeleteFlag Deletion flag
/// @param nProcessID Process ID
/// @param pPacReq IPC request packet header
/// @returns  Certificate data information
ara::core::Result< ara::core::Vector< uint8_t > > PX509Process_Provider::_FindCertByDn(
    ara::core::String &stCertName,
    uint32_t const &nDeleteFlag,
    uint64_t const nProcessID,
    x509provider::PIpcReq_FindCertByDn const *const pPacReq) const noexcept
{
    x509::isoft_def::PX509Provider x509Provider;
    // Traverse all certificates in persistent storage to find the corresponding certificate
    ara::core::StringView const stSubjectDn{pPacReq->GetSubjectDn()};
    x509::X509DN dnSubject{x509Provider};
    ara::core::Result< void > const resultSubjectDn{dnSubject.SetDn(stSubjectDn)};
    if (false == resultSubjectDn.HasValue()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kAccessViolation);
    }
    ara::core::StringView const stIssuerDn{pPacReq->GetIssuerDn()};
    x509::X509DN dnIssuer{x509Provider};
    ara::core::Result< void > const resultIssuerDn{dnIssuer.SetDn(stIssuerDn)};
    if (false == resultIssuerDn.HasValue()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kAccessViolation);
    }
    // x509::X509Provider::kInvalidIndex_
    uint32_t nStartIndex{0U};
    if (SIZE_MAX != pPacReq->nCertIndex) {
        nStartIndex = static_cast< uint32_t >(pPacReq->nCertIndex);
    } else {
        nStartIndex = 0U;
    }
    time_t const validityTimePoint{pPacReq->validityTime};
    ara::core::Result< ara::core::Vector< uint8_t > > resultFindCert{FindCert(
        stCertName, nDeleteFlag, lpcProcessManager_, x509Provider, static_cast< uint32_t >(nProcessID), nStartIndex,
        [&dnSubject, &dnIssuer, validityTimePoint](x509::Certificate const *const pCertStorage) noexcept -> bool {
            if (nullptr == pCertStorage) {
                return false;
            }
            if (pCertStorage->SubjectDn() != dnSubject) {
                return false;
            }
            if (pCertStorage->IssuerDn() != dnIssuer) {
                return false;
            }
            if (pCertStorage->StartTime() > validityTimePoint) {
                return false;
            }
            if (pCertStorage->EndTime() < validityTimePoint) {
                return false;
            }
            return true;
        })};
    return resultFindCert;
}
namespace {
/// @brief Compares if two ReadOnlyMem regions are not equal
/// @param a ReadOnlyMemRegion object
/// @param b ReadOnlyMemRegion object
/// @return
/// @throws
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
/// @brief Finds certificate
ara::core::Result< ara::core::Vector< uint8_t > >
/// @name  _FindCertByKey
/// @param stCertName Certificate name
/// @param nDeleteFlag Deletion flag
/// @param nProcessID Process ID
/// @param pPacReq IPC request packet header
/// @returns  Certificate data information
PX509Process_Provider::_FindCertByKey(ara::core::String &stCertName,
                                      uint32_t const &nDeleteFlag,
                                      uint64_t const nProcessID,
                                      x509provider::PIpcReq_FindCertByKeyIds const *const pPacReq) const noexcept
{
    x509::isoft_def::PX509Provider x509Provider;
    // Traverse all certificates in persistent storage to find the corresponding certificate
    ara::crypto::ReadOnlyMemRegion const memSubjectKeyId{pPacReq->GetSubjectKeyId()};
    ara::crypto::ReadOnlyMemRegion const memAuthorityKeyId{pPacReq->GetAuthorityKeyId()};
    ara::core::Result< ara::core::Vector< uint8_t > > resultFindCert{FindCert(
        stCertName, nDeleteFlag, lpcProcessManager_, x509Provider, static_cast< uint32_t >(nProcessID), 0U,
        [memSubjectKeyId, memAuthorityKeyId](x509::Certificate const *const pCertStorage) -> bool {
            if (nullptr == pCertStorage) {
                return false;
            }
            ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSubKey{pCertStorage->SubjectKeyId()};
            if (false == resultSubKey.HasValue()) {
                return false;
            }
            ReadOnlyMemRegion const findMemSubKeyId{
                static_cast< uint8_t const * >(static_cast< void const * >(resultSubKey.Value().data())),
                resultSubKey.Value().size()};
            if (memSubjectKeyId != findMemSubKeyId) {
                return false;
            }
            ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultAutKey{
                pCertStorage->AuthorityKeyId()};
            if (false == resultAutKey.HasValue()) {
                return false;
            }
            ReadOnlyMemRegion const findMemAutKeyId{
                static_cast< uint8_t const * >(static_cast< void const * >(resultAutKey.Value().data())),
                resultAutKey.Value().size()};
            if (memAuthorityKeyId != findMemAutKeyId) {
                return false;
            }
            return true;
        })};
    return resultFindCert;
}
/// @brief Finds certificate
ara::core::Result< ara::core::Vector< uint8_t > >
/// @name  _FindCertBySn
/// @param stCertName Certificate name
/// @param nDeleteFlag Deletion flag
/// @param nProcessID Process ID
/// @param pPacReq IPC request packet header
/// @returns  Certificate data information
PX509Process_Provider::_FindCertBySn(ara::core::String &stCertName,
                                     uint32_t const &nDeleteFlag,
                                     uint64_t const nProcessID,
                                     x509provider::PIpcReq_FindCertBySn const *const pPacReq) const noexcept
{
    x509::isoft_def::PX509Provider x509Provider;
    // Traverse all certificates in persistent storage to find the corresponding certificate
    ara::crypto::ReadOnlyMemRegion const memSN{pPacReq->GetSerialNumber()};
    ara::core::StringView const stIssuerDn{pPacReq->GetIssuerDn()};

    x509::X509DN dnIssuer{x509Provider};
    ara::core::Result< void > const resultIssuerDn{dnIssuer.SetDn(stIssuerDn)};
    if (false == resultIssuerDn.HasValue()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kAccessViolation);
    }
    ara::core::Result< ara::core::Vector< uint8_t > > resultFindCert{FindCert(
        stCertName, nDeleteFlag, lpcProcessManager_, x509Provider, static_cast< uint32_t >(nProcessID), 0U,
        [memSN, &dnIssuer](x509::Certificate const *const pCertStorage) -> bool {
            if (nullptr == pCertStorage) {
                return false;
            }
            ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSN{pCertStorage->SerialNumber()};
            if (false == resultSN.HasValue()) {
                return false;
            }
            ReadOnlyMemRegion const findMemSN{
                static_cast< uint8_t const * >(static_cast< void const * >(resultSN.Value().data())),
                resultSN.Value().size()};
            if (memSN != findMemSN) {
                return false;
            }
            /// Some certificates do not have an Authority Key Identifier
            /// This logic should be useless here (_FindCertByKey is useful); temporarily commenting out this logic - Chang Zheng
            /// 2023-11-20
            if (pCertStorage->IssuerDn() != dnIssuer) {
                return false;
            }
            return true;
        })};
    return resultFindCert;
}
/// @brief Constructs the response packet
PX509Process_Base::PResultLen
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param stCertName Certificate name
/// @param nDeleteFlag Deletion flag
/// @param vecData Input data
/// @return Response packet length
PX509Process_Provider::AssembleFindCertMsg(PIpcPac_Head const *const pReqHead,
                                           PIpcAutoPacket &aswMsg,
                                           ara::core::String const &stCertName,
                                           uint32_t const nDeleteFlag,
                                           ara::core::Vector< uint8_t > const &vecData) noexcept
{
    size_t const nCertNamelen{stCertName.size()};
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::x509provider::PIpcAsw_FindCert)
                                                   + nCertNamelen + vecData.size())};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::x509provider::PIpcAsw_FindCert *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::x509provider::PIpcAsw_FindCert >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIpcCertID(0U);
    pPacAsw->SetCertType(0U);
    pPacAsw->SetCertIndex(static_cast< uint32_t >(SIZE_MAX));
    pPacAsw->SetDeleteFlag(nDeleteFlag);

    uint8_t *const pDataWrite{static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1))};
    int32_t nIndex{0};
    pPacAsw->SetCertNameLen(static_cast< uint16_t >(nCertNamelen));
    std::ignore = memcpy(pDataWrite + nIndex, stCertName.data(), nCertNamelen);
    nIndex += static_cast< int32_t >(nCertNamelen);
    pPacAsw->SetCertDataLen(static_cast< uint16_t >(vecData.size()));
    std::ignore = memcpy(pDataWrite + nIndex, vecData.data(), vecData.size());
    nIndex += static_cast< int32_t >(vecData.size());
    std::ignore = nIndex;
    return {pReqHead->nPacSize};
}
/// @brief Revokes a certificate, updating the index.txt file
/// @param revcert X509 certificate pointer
/// @return true if revoke cert sucess false otherwise
bool PX509Process_Provider::_RevokeCert(X509 *const revcert) const noexcept
{
    // Subsequently load from the key slot
    openssl::isoft_def::CA_DB *const pDb{
        static_cast< openssl::isoft_def::CA_DB * >(_LoadAndUpdateDb(kCrlIndexFileName))};
    bool ret{true};
    if (pDb == nullptr) {
        return false;
    }

    BIO *bioOut{BIO_new(BIO_s_mem())};
    if (openssl::isoft_def::RevokeCertificate(pDb, revcert, &bioOut) == false) {
        ret = false;
    }

    if (bioOut != nullptr) {
        int32_t const nLen{PH_BIO_pending(bioOut)};
        ara::core::Vector< ara::core::Byte > vecDataUpdate;
        vecDataUpdate.reserve(static_cast< size_t >(nLen));
        std::ignore = BIO_read(bioOut, vecDataUpdate.data(), nLen);
        std::ignore = lpcProcessManager_.SaveCert(
            kCrlIndexFileName, reinterpret_cast< uint8_t * >(vecDataUpdate.data()), static_cast< uint16_t >(nLen));
        std::ignore = BIO_free(bioOut);
        bioOut      = nullptr;
    }

    openssl::isoft_def::Clear_db(pDb);
    return ret;
}

/// @brief Generates Certificate Revocation List (CRL)
/// @param pemLen Returned PEM format certificate length
/// @param x509Root X509 root certificate
/// @param pkey EVP_PKEY key resource
/// @return CRL information
ara::core::Vector< uint8_t > PX509Process_Provider::_GenerateCRL(int32_t &pemLen,
                                                                 X509 *const x509Root,
                                                                 EVP_PKEY *const pkey) const noexcept
{
    if (pemLen == 0) {
    };
    /// Load database
    openssl::isoft_def::CA_DB *const pDb{
        static_cast< openssl::isoft_def::CA_DB * >(_LoadAndUpdateDb(kCrlIndexFileName))};
    ara::core::Vector< uint8_t > vecPemCRL;

    EVP_MD const *const dgst{openssl::isoft_def::ReadyDgest(pkey)};
    if (dgst == nullptr) {
        EVP_PKEY_free(pkey);
        return vecPemCRL;
    }

    // Generate Certificate Revocation List
    X509_CRL *crl{nullptr};
    crl = GenerateCRLs(pDb, x509Root, dgst, pkey);
    if (crl == nullptr) {  // x509, pkey,
        std::ignore = ClearCacheInGenCRL(pDb, crl, true, x509Root);
        return vecPemCRL;
    }

    /// Convert format for transmission
    BIO *const bio{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_X509_CRL(bio, crl);
    pemLen      = PH_BIO_pending(bio);
    vecPemCRL.resize(static_cast< size_t >(pemLen));
    std::ignore = BIO_read(bio, vecPemCRL.data(), pemLen);

    ara::crypto::openssl::isoft_def::Clear_db(pDb);
    X509_CRL_free(crl);
    std::ignore = BIO_free(bio);

    return vecPemCRL;
}
/// @brief Loads and updates the database
/// @param needUpdateDb Revocation list database filename
/// @param uniqueSubject Unique identifier for issued certificates; default value 0 allows multiple certificates with the same subject to exist simultaneously in the CA index file, otherwise only one is allowed
/// @return Revocation list database pointer
void *PX509Process_Provider::_LoadAndUpdateDb(ara::core::StringView const &dbFileName,
                                              bool const uniqueSubject) const noexcept
{
    BIO *bioOut{nullptr};
    ara::core::Vector< uint8_t > vecData;
    ara::core::Result< uint32_t > const result{lpcProcessManager_.LoadCert(dbFileName, vecData)};
    if (result.HasValue() == false) {
        return nullptr;
    }
    /// Load database
    bioOut = BIO_new(BIO_s_mem());
    uint8_t *const pData{vecData.data()};
    ara::crypto::openssl::isoft_def::CA_DB *const pDb{openssl::isoft_def::LoadAndUpdateDb(
        static_cast< int32_t >(uniqueSubject), reinterpret_cast< ara::crypto::char8_t * >(pData),
        static_cast< int32_t >(vecData.size()), &bioOut)};
    if (bioOut != nullptr) {
        int32_t const nLen{PH_BIO_pending(bioOut)};
        ara::core::Vector< ara::core::Byte > vecDataUpdate;
        vecDataUpdate.resize(static_cast< size_t >(nLen));
        std::ignore = BIO_read(bioOut, vecDataUpdate.data(), nLen);
        std::ignore = lpcProcessManager_.SaveCert(dbFileName, reinterpret_cast< uint8_t * >(vecDataUpdate.data()),
                                                  static_cast< uint16_t >(nLen));
        std::ignore = BIO_free(bioOut);
        bioOut      = nullptr;
    }
    return pDb;
}
/// @brief Loads a certificate from memory
/// @param memCert Certificate memory data
/// @param nFormatID Certificate encoding format
/// @return
X509 *PX509Process_Provider::LoadCertFile(
    ReadOnlyMemRegion const &memCert,
    Serializable::FormatId const nFormatID) noexcept  // Load a certificate from memory
{
    X509 *pX509{nullptr};
    if ((nullptr == pX509)
        && ((Serializable::kFormatDefault == nFormatID) || (Serializable::kFormatRawValueOnly == nFormatID))) {
        uint8_t const *pData{memCert.data()};
        // 2022-06-13 hanjingjing: Highly suspect this usage is incorrect (2023-08-01 Chang Zheng: d2i_X509 also handles der format data)
        pX509 = d2i_X509(nullptr, &pData, static_cast< int64_t >(memCert.size()));
    }
    if ((nullptr == pX509)
        && ((Serializable::kFormatDefault == nFormatID) || (Serializable::kFormatDerEncoded == nFormatID))) {
        BIO *const pBio{BIO_new_mem_buf(memCert.data(), static_cast< int32_t >(memCert.size()))};
        pX509       = d2i_X509_bio(pBio, nullptr);
        std::ignore = BIO_free(pBio);
    }
    if ((nullptr == pX509)
        && ((Serializable::kFormatDefault == nFormatID) || (Serializable::kFormatPemEncoded == nFormatID))) {
        /// The pBio here cannot be placed outside; when nFormatID is kFormatDefault (PEM format data), the pBio changes after executing d2i_X509_bio
        BIO *const pBio{BIO_new_mem_buf(memCert.data(), static_cast< int32_t >(memCert.size()))};
        pX509       = PEM_read_bio_X509(pBio, nullptr, nullptr, nullptr);
        std::ignore = BIO_free(pBio);
    }
    return pX509;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
