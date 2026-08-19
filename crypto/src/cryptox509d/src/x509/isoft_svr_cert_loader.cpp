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
/// @file       isoft_svr_cert_loader.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2023-09-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/Certificate Slot Storage
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PSvrCertLoader
/// @unit_description=Certificate slot storage
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_svr_cert_loader.h"

#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/x509/isoft_x509_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
namespace {
/// @brief Certificate slot storage name; this name must not change and must be used in Arxml
/// @return Certificate slot storage name
inline ara::core::StringView GetCertStorageName() noexcept
{
    return ara::core::StringView{"cryptox509d/cryptox509d_swc_root/prPort_fileStorage_cert"};
}
}  // namespace
//********************************/
/// @brief Creates a new certificate slot Loader
/// @param keySlotManager Key slot manager
/// @param stCertName Certificate name
/// @return
PSvrCertLoader::Uptr PSvrCertLoader::NewCertSlotLoader(PX509_Manager &keySlotManager,
                                                       ara::core::StringView const &stCertName) noexcept
{
    return std::make_unique< PSvrCertLoader >(keySlotManager, GetCertStorageName(), stCertName);
}
/// @brief Gets the certificate storage pointer
/// @returns  Certificate persistent storage pointer
ara::per::SharedHandle< ara::per::FileStorage > PSvrCertLoader::GetCertStoragePtr() noexcept
{
    ara::per::SharedHandle< ara::per::FileStorage > pCertStorage;
    ara::core::InstanceSpecifier const insKv{GetCertStorageName()};
    ara::core::Result< ara::per::SharedHandle< ara::per::FileStorage > > const resultOpen{
        ara::per::OpenFileStorage(insKv)};
    if (resultOpen.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "GetCertStoragePtr OpenFileStorage Sucess ";
        pCertStorage = resultOpen.Value();
    } else {
        ara::crypto::isoft_def::LogWarn() << "GetCertStoragePtr OpenFileStorage Failed ";
    }
    return pCertStorage;
}
/// @brief Gets the certificate loading operation class
/// @param cryptoProvider Crypto provider
/// @param stCertName Certificate name
/// @return
PIoInterface_File::Uptr PSvrCertLoader::NewCertSlotIo(cryp::CryptoProvider &cryptoProvider,
                                                      ara::core::StringView const &stCertName) noexcept
{
    PIoInterface_File::Uptr pFileIo{std::make_unique< PIoInterface_File >(cryptoProvider, GetCertStorageName())};
    if (false == pFileIo->InitIoInterface(stCertName)) {
        ara::crypto::isoft_def::LogWarn() << "NewCertSlotIo InitIoInterface failed ";
        return {nullptr};
    }
    return pFileIo;
}
//********************************/
/// @brief Parameterized constructor
/// @param keySlotManager Key slot manager
/// @param stStorageName Persistent storage name
/// @param stCertName Certificate name
PSvrCertLoader::PSvrCertLoader(PX509_Manager &keySlotManager,
                               ara::core::StringView const &stStorageName,
                               ara::core::StringView const &stCertName) noexcept
    : ipcProcessManager_{keySlotManager}, stStorageName_{stStorageName}, stCertName_{stCertName}
{
    pIoInterface_ = _MakeIoInterface();
}
//********************************/
/// @brief Returns the PIOInterface used by itself
/// @returns  PIoInterface_File::Uptr
PIoInterface_File::Uptr PSvrCertLoader::_MakeIoInterface() const noexcept
{
    PIoInterface_File::Uptr pIoInterface{
        std::make_unique< PIoInterface_File >(ipcProcessManager_.GetCryptoProvider(), stStorageName_)};
    if (false == pIoInterface->InitIoInterface({stCertName_.data(), stCertName_.size()})) {
        ara::crypto::isoft_def::LogWarn() << "InitIoInterface failed";
        return {nullptr};
    }
    return pIoInterface;
}

/// @brief Saves KeyData via IPC
/// @param pData Data: memory start address
/// @param nLen Data length: byte unit
/// @returns true if save cert sucess false otherwise
ara::core::Result< bool > PSvrCertLoader::SaveCertData(uint8_t const *const pData, uint16_t const nLen) const noexcept
{
    if (!pIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    bool const ret{pIoInterface_->SaveCertData(pData, nLen)};
    return ara::core::Result< bool >::FromValue(ret);
}
/// @brief Saves KeyData via IPC
/// @returns Certificate data information
ara::core::Result< ara::core::Vector< ara::core::Byte > > PSvrCertLoader::LoadCertData() const noexcept
{
    if (!pIoInterface_.operator bool()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    ara::core::Vector< ara::core::Byte > vecData;
    if (!pIoInterface_->ReadCertData(vecData)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Deletes certificate
/// @return  true if remove cert sucess false otherwise
ara::core::Result< bool > PSvrCertLoader::RemoveCert() const noexcept
{
    if (!pIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    bool const ret{pIoInterface_->RemoveCert()};
    return ara::core::Result< bool >::FromValue(ret);
}
/// @brief Sets extension information
/// @param nIndex Index
/// @return  true if set cert exinfo sucess false otherwise
ara::core::Result< bool > PSvrCertLoader::SetCertExInfo(uint16_t const nIndex) const noexcept
{
    if (!pIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    bool const ret{pIoInterface_->SetCertExInfo(nIndex)};
    return ara::core::Result< bool >::FromValue(ret);
}
/// @brief Checks if CSR has a pending status
/// @return  true if has already set pending false otherwise
ara::core::Result< bool > PSvrCertLoader::IsCsrSetPending() const noexcept
{
    if (!pIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromValue(false);
    }
    bool const ret{pIoInterface_->IsCsrSetPending()};
    return ara::core::Result< bool >::FromValue(ret);
}

//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
