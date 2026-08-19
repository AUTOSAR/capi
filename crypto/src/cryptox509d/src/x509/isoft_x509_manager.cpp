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
/// @file       isoft_x509_manager.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2023-09-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/Certificate Manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PX509_Manager
/// @unit_description=Certificate manager
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_x509_manager.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/x509/isoft_x509_process_provider.h"
#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Constructor
PX509_Manager::PX509_Manager() : ipcServer_{*this}
{
    PH_ASSERT(manifestKspConfig_.InitManifest(GetConfigFileName()));
    pCryptoProvider_                                         = DefCryptoProvider();
    mapIpcProcess_[x509provider::GetName_PIpcX509Provider()] = std::make_unique< PX509Process_Provider >(*this);
}
/// @brief Destructor
PX509_Manager::~PX509_Manager() noexcept = default;
/// @brief Execute
void PX509_Manager::Running() const noexcept
{
    std::ignore = ipcServer_.Begin();
    std::ignore = ipcServer_.Working();
    std::ignore = ipcServer_.End();
}
//***************/
/// @brief Processes IPC messages
/// @name  ProcessIpcMsg
/// @param pReq IPC request data
/// @param nReqLen IPC request data length
/// @param aswMsg IPC managed response message
/// @returns  0 sucess fail otherwise
int32_t PX509_Manager::ProcessIpcMsg(uint8_t *const pReq, uint16_t const nReqLen, PIpcAutoPacket &aswMsg) noexcept
{
    try {
        PIpcPac_Head *const pPacHead{static_cast< PIpcPac_Head * >(static_cast< void * >(pReq))};
        ara::core::StringView const stFuncName{pPacHead->GetFuncName()};
        ara::core::StringView const stClassName{stFuncName.substr(0U, stFuncName.find("::"))};
        MAP_IpcProcess::iterator const itFind{mapIpcProcess_.find(stClassName)};
        ara::crypto::isoft_def::LogDebug() << "CryptoX509 IPC.Process <<<<< FuncName = " << stFuncName;
        if (itFind == mapIpcProcess_.end()) {
            // Command processing class object not found
            return ProcessCmd_Error(pPacHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);
        }
        if (itFind->second == nullptr) {
            // Command processing class object is empty (almost impossible)
            return ProcessCmd_Error(pPacHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);
        }
        PX509Process_Base::PResultLen const result{itFind->second->ProcessIpcMsg(pReq, nReqLen, aswMsg)};
        if (false == result.HasValue()) {
            // Command processing function not found
            return ProcessCmd_Error(pPacHead, aswMsg, static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
        }
        ara::crypto::isoft_def::LogDebug() << "CryptoX509 IPC.Process > >> >> SUCCESS.";
        return static_cast< int32_t >(result.Value());
    } catch (const std::exception &e) {
        return 0;
    }
}
/// @brief Handles error commands
/// @name  ProcessCmd_Error
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @return 0 sucess fail otherwise
int32_t PX509_Manager::ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                        PIpcAutoPacket &aswMsg,
                                        SecurityErrorDomain::Errc const nErrorCode) noexcept
{
    int32_t const nMsgLen{static_cast< int32_t >(pReqHead->GetHeadLen())
                          + static_cast< int32_t >(sizeof(PIpcAsw_LogicHead))};
    aswMsg.CreatePacket(static_cast< uint16_t >(nMsgLen));
    std::ignore = aswMsg.InitIpcHead(pReqHead, static_cast< uint16_t >(nMsgLen));
    PIpcAsw_LogicHead *const pPacAsw{aswMsg.GetIpcBody< PIpcAsw_LogicHead >()};
    pPacAsw->SetErrorID(static_cast< int32_t >(nErrorCode));
    ara::crypto::isoft_def::LogError() << "IPC.Process > >> >> ErrorID = " << pPacAsw->GetErrorID() << " .";
    return nMsgLen;
}
/// @brief Gets the crypto provider
/// @name  GetCryptoProvider
/// @returns  Crypto provider reference
cryp::CryptoProvider &PX509_Manager::GetCryptoProvider() const noexcept { return *(pCryptoProvider_.get()); }
///********************************/
//***************/
/// @brief Checks if the certificate exists: return value -1 means not found, 0 means found but empty, positive number means data length in the certificate slot
/// @param stCertName Certificate name
/// @return -1 means not found, 0 means found but empty, positive number means data length in the certificate slot
ara::core::Result< uint32_t > PX509_Manager::IsCertExist(ara::core::StringView const &stCertName) noexcept
{
    // Search in Json configuration; report error if not found
    manifest::PConfig_CryptoCertificate const *const pFindCertConfig{_FindConfigCert(stCertName)};
    if (nullptr == pFindCertConfig) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resVec{pSlotLoader->LoadCertData()};
    if (!resVec.HasValue()) {
        return ara::core::Result< uint32_t >::FromError(
            static_cast< SecurityErrorDomain::Errc >(resVec.Error().Value()));
    }
    ara::core::Vector< ara::core::Byte > const &vecData{resVec.Value()};
    return ara::core::Result< uint32_t >::FromValue(vecData.size());
}
/// @brief Loads certificate
/// @name  LoadCert
/// @param stCertName Certificate name
/// @param vecData Return result
/// @returns  Certificate data length if value exists, otherwise error
ara::core::Result< uint32_t > PX509_Manager::LoadCert(ara::core::StringView const &stCertName,
                                                      ara::core::Vector< uint8_t > &vecData) noexcept
{
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resVec{pSlotLoader->LoadCertData()};
    if (!resVec.HasValue()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    ara::core::Vector< ara::core::Byte > const &vecByteData{resVec.Value()};
    std::size_t const nVecLen{vecByteData.size()};

    for (size_t i{0U}; i < nVecLen; i++) {
        vecData.push_back(static_cast< uint8_t >(vecByteData[i]));
    }
    return ara::core::Result< uint32_t >::FromValue(nVecLen);
}
/// @brief Saves certificate
/// @name  SaveCert
/// @param stCertName Certificate name
/// @param pCertData Certificate data
/// @param nDataLen Data length
/// @return has value if save cert sucess false otherwise
ara::core::Result< uint32_t > PX509_Manager::SaveCert(ara::core::StringView const &stCertName,
                                                      uint8_t const *const pCertData,
                                                      uint16_t const nDataLen) noexcept
{
    // Search in Json configuration; report error if not found
    manifest::PConfig_CryptoCertificate const *const pFindCertConfig{_FindConfigCert(stCertName)};
    ara::core::StringView const crlIndexFileName{"index.txt"};
    // If InstanceSpecifier is incorrect (slot not allocated)
    if (nullptr == pFindCertConfig) {
        if (stCertName.compare(crlIndexFileName) != 0) {
            return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
    }
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }

    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< bool > const resBool{pSlotLoader->SaveCertData(pCertData, nDataLen)};
    if (!resBool.HasValue()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    return ara::core::Result< uint32_t >::FromValue(nDataLen);
}
/// @brief Deletes certificate  0 success -1 failure
/// @name  RemoveCert
/// @param stCertName Certificate name
/// @return 0 sucess false otherwise
ara::core::Result< uint32_t > PX509_Manager::RemoveCert(ara::core::StringView const &stCertName) noexcept
{
    // Search in Json configuration; report error if not found
    manifest::PConfig_CryptoCertificate const *const pFindCertConfig{_FindConfigCert(stCertName)};
    // If InstanceSpecifier is incorrect (slot not allocated)
    if (nullptr == pFindCertConfig) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< bool > const resBool{pSlotLoader->RemoveCert()};
    if (!resBool.HasValue()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    return ara::core::Result< uint32_t >::FromValue(0);
}
/// @brief Finds the corresponding crypto provider by slot name
/// @param stSlotName Key slot name
/// @return Crypto provider found by slot name
ara::core::String PX509_Manager::FindCryptoProviderBySlotName(ara::core::StringView const &stSlotName) noexcept
{
    return manifestKspConfig_.FindCryptoProviderBySlotName(stSlotName);
}
/// @brief Checks if the current process is the TrustMaster process by process name
/// @name  IsTrustMaster
/// @param stProcessName Process name
/// @return true if is trustmaster false otherwise
ara::core::Result< bool > PX509_Manager::IsTrustMaster(ara::core::StringView const &stProcessName) noexcept
{
    bool const isTrustMaster{manifestKspConfig_.IsTrustMaster(stProcessName)};
    return ara::core::Result< bool >::FromValue(isTrustMaster);
}
/// @brief Sets certificate extension information
/// @param stCertName Certificate name
/// @param nIndex Index
/// @return true if set cert exinfo sucess false otherwise
ara::core::Result< bool > PX509_Manager::SetCertExInfo(ara::core::StringView const &stCertName,
                                                       uint16_t const nIndex) noexcept
{
    /// Search in Json configuration; report error if not found
    manifest::PConfig_CryptoCertificate const *const pFindCertConfig{_FindConfigCert(stCertName)};
    // If InstanceSpecifier is incorrect (slot not allocated)
    if (nullptr == pFindCertConfig) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< bool > const resBool{pSlotLoader->SetCertExInfo(nIndex)};
    if (!resBool.HasValue()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< bool >::FromValue(false);
    }
    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Checks if the certificate signing request has a pending status
/// @param stCertName Certificate name
/// @return true if has already set pending
ara::core::Result< bool > PX509_Manager::IsCsrSetPending(ara::core::StringView const &stCertName) noexcept
{
    /// Search in Json configuration; report error if not found
    manifest::PConfig_CryptoCertificate const *const pFindCertConfig{_FindConfigCert(stCertName)};
    // If InstanceSpecifier is incorrect (slot not allocated)
    if (nullptr == pFindCertConfig) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader::Uptr const pSlotLoaderPtr{PSvrCertLoader::NewCertSlotLoader(*this, stCertName)};
    if (!pSlotLoaderPtr.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSvrCertLoader *const pSlotLoader{pSlotLoaderPtr.get()};
    ara::core::Result< bool > const resBool{pSlotLoader->IsCsrSetPending()};
    if (!resBool.HasValue()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    if (resBool.Value() == false) {
        return ara::core::Result< bool >::FromValue(false);
    }
    return ara::core::Result< bool >::FromValue(true);
}
//***************/
/// @brief Gets the filename of the configuration file
/// @return Filename of the configuration file
ara::core::String PX509_Manager::GetConfigFileName() noexcept
{
    ara::core::String stConfigFileName;

    isoft::ara_fsh::Platform const platForm;
    stConfigFileName = platForm.GetPlatformEtcDir();
    stConfigFileName += isoft::ara_fsh::Process::kCrypto;
    if (false == stConfigFileName.empty()) {
        return stConfigFileName;
    }

    isoft::ara_fsh::Process const fsh;
    stConfigFileName = fsh.GetCrypto();
    if (false == stConfigFileName.empty()) {
        return stConfigFileName;
    }

    stConfigFileName = manifest::GetkCryptoManifestFileName();
    return stConfigFileName;
}
/// @brief Gets the default CryptoProvider
/// @name  DefCryptoProvider
/// @returns  Default CryptoProvider
cryp::CryptoProvider::Uptr PX509_Manager::DefCryptoProvider() noexcept
{
    ara::core::InstanceSpecifier const iSpecify{ara::core::StringView{"def_crypto"}};
    cryp::CryptoProvider::Uptr pCrypto{LoadCryptoProvider(iSpecify)};
    return pCrypto;
}
/// @brief Finds the corresponding certificate slot configuration in the configuration file
/// @param stSlotName Key slot name
/// @return
keys::KeySlotPrototypeProps::Uptr PX509_Manager::_FindConfigCertSlot(
    ara::core::StringView const &stSlotName) const noexcept
{
    manifest::PConfig_CryptoKeySlot const *const pFindCertSlot{manifestKspConfig_.FindCertSlotByName(stSlotName)};
    if (nullptr == pFindCertSlot) {
        return {nullptr};
    }
    return manifest::PManifestKspConfig::AssembleSlotProps(pFindCertSlot);
}
/// @brief Gets the real key slot name
/// @param stSlotName Key slot name
/// @return Real key slot name
ara::core::Vector< ara::core::String > PX509_Manager::FindRealSlotNames(
    ara::core::StringView const &stSlotName) noexcept
{
    return manifestKspConfig_.FindCertSlotNamesByName(stSlotName);
}
/// @brief Finds the corresponding certificate slot configuration in the configuration file
/// @param stCertName Certificate name
/// @return
/// @throws
manifest::PConfig_CryptoCertificate const *PX509_Manager::_FindConfigCert(
    ara::core::StringView const &stCertName) const noexcept
{
    return manifestKspConfig_.FindCertByName(stCertName);
}

//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
