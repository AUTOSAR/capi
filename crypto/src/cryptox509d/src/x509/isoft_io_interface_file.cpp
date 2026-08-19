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
/// @file       isoft_io_interface_file.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2023-09-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PIoInterface_File
/// @unit_description=Interface for saving and loading security objects
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_io_interface_file.h"

#include "ara/core/vector.h"
#include "ara/crypto/common/isoft_log_api.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading security objects: an implementation version where all slots share the same KV store.
///         Actual saving and loading should be implemented through known internal methods of the trusted pair consisting of the crypto provider and storage provider.
///         Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes objects in the container are compressed, i.e., have a minimally optimized size.
//********************************/
/// @brief Parameterized constructor
/// @name  PIoInterface_File
/// @returns
/// @throws
/// @param cryptoProvider Crypto provider
/// @param stStorageName Persistent storage name
PIoInterface_File::PIoInterface_File(ara::crypto::cryp::CryptoProvider &cryptoProvider,
                                     ara::core::StringView const &stStorageName) noexcept
    : cryptoProvider_{cryptoProvider}, stStorageName_{stStorageName}
{
}
//***************/
/// @brief Initializes IO interface
/// @name  InitIoInterface
/// @param stCertName Certificate name
/// @returns true if init iointerface sucess false otherwise
bool PIoInterface_File::InitIoInterface(ara::core::StringView const &stCertName) noexcept
{
    ara::core::InstanceSpecifier const insKv{T_StringView(stStorageName_)};
    ara::core::Result< ara::per::SharedHandle< ara::per::FileStorage > > const resultOpen{
        ara::per::OpenFileStorage(insKv)};
    if (false == resultOpen.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "OpenFileStorage failed";
        return false;
    }
    pFileStorage_ = resultOpen.Value();
    /// Convert certificate name to file name: replace '/' in FQN with '__' because '/' in certificate names prevents file system creation
    for (size_t i{0U}; i < stCertName.size(); i++) {
        if (stCertName[i] == '/') {
            stFileName_ += '_';
            stFileName_ += '_';
        } else {
            stFileName_ += stCertName[i];
        }
    }

    if (!_initLoadCertData()) {
        ara::crypto::isoft_def::LogInfo() << "InitLoadCertData failed";
    }

    return true;
}
/// @brief Reads certificate data
/// @param vecData Return result
/// @name  ReadCertData
/// @returns  true if read cert data sucess false otherwise
bool PIoInterface_File::ReadCertData(ara::core::Vector< ara::core::Byte > &vecData) const noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "ReadCertData failed FileStorage nullptr";
        return false;
    }
    uint8_t const *const pData{certData_.Data()};
    uint32_t const nLen{certData_.size()};
    for (uint32_t i{0U}; i < nLen; i++) {
        vecData.push_back(static_cast< ara::core::Byte >(pData[i]));
    }
    return true;
}
/// @brief Saves certificate information to file and memory
/// @param pData Data: memory start address
/// @param nLen Data length: byte unit
/// @return true if save cert data sucess false otherwise
bool PIoInterface_File::SaveCertData(uint8_t const *const pData, uint16_t const nLen) noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "SaveCertData failed FileStorage nullptr";
        return false;
    }
    ara::core::Result< ara::per::UniqueHandle< ara::per::ReadWriteAccessor > > resultOpenFile{
        pFileStorage_->OpenFileReadWrite(T_StringView(stFileName_))};
    if (!resultOpenFile.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "SaveCertData OpenFileReadWrite failed ";
        return false;
    }
    ara::per::UniqueHandle< ara::per::ReadWriteAccessor > fileOpt{std::move(resultOpenFile).Value()};
    ara::core::Span< ara::core::Byte const > const byteSpan{T_TransPtr< ara::core::Byte >(pData),
                                                            static_cast< size_t >(nLen)};

    ara::core::Result< void > const resVoid{fileOpt->WriteBinary(byteSpan)};
    if (!resVoid.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "SaveCertData WriteBinary failed ";
        return false;
    }
    bool const setDataSuccess{certData_.SetData(pData, static_cast< uint32_t >(nLen))};
    if (setDataSuccess == true) {
        return true;
    }
    return false;
}
/// @brief Deletes certificate data
/// @return true if remove sucess false otherwise
bool PIoInterface_File::RemoveCert() noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "RemoveCert failed FileStorage nullptr";
        return false;
    }
    ara::core::Result< void > const resVoid{pFileStorage_->DeleteFile(T_StringView(stFileName_))};
    if (!resVoid.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "RemoveCert DeleteFile failed ";
        return false;
    }
    certData_.ResetData();
    return true;
}

/// @brief Sets certificate extension information
/// @param nIndex Index: extension field index number chReserve[20] 0~19
/// @return  true if set cert exinfo sucess false otherwise
bool PIoInterface_File::SetCertExInfo(uint16_t const nIndex) const noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "SetCertExInfo failed FileStorage nullptr";
        return false;
    }

    ara::per::isoftkv::PFileInfoInKvs_ReddExt pFileInfoExt{
        pFileStorage_->ReadFileInfoEx(T_StringView(stFileName_), 0U)};
    pFileInfoExt.chReserve[static_cast< size_t >(nIndex)] = 1U;  // NOLINT

    return pFileStorage_->SaveFileInfoEx(T_StringView(stFileName_), pFileInfoExt);
}

/// @brief Checks if the certificate signing request has a pending status
/// @return true if csr has already set pending false otherwise
bool PIoInterface_File::IsCsrSetPending() const noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "SetCertExInfo failed FileStorage nullptr";
        return false;
    }
    ara::per::isoftkv::PFileInfoInKvs_ReddExt const pFileInfoExt{
        pFileStorage_->ReadFileInfoEx(T_StringView(stFileName_), 0U)};
    return pFileInfoExt.chReserve[1U] == 1U;
}

/// @brief Initialization operation: reads information from the certificate file into memory
/// @name  _initLoadCertData
/// @returns  true if read cert info from kv sucess false otherwise
bool PIoInterface_File::_initLoadCertData() noexcept
{
    if (false == pFileStorage_.operator bool()) {
        ara::crypto::isoft_def::LogWarn() << "LoadCertData failed FileStorage nullptr";
        return false;
    }
    auto resultOpenFile{pFileStorage_->OpenFileReadOnly(T_StringView(stFileName_))};
    if (!resultOpenFile.HasValue()) {
        return false;
    }
    auto fileOpt{std::move(resultOpenFile).Value()};
    ara::core::Result< ara::core::String > const resultReadData{fileOpt->ReadText()};
    bool const hasValue{resultReadData.HasValue()};
    if (false == hasValue) {
        ara::crypto::isoft_def::LogWarn() << "LoadCertData ReadText failed ";
        return false;
    }
    ara::core::StringView const vecData{resultReadData.Value()};
    std::ignore = certData_.SetData(T_TransBytes(vecData.data()), static_cast< uint32_t >(vecData.size()));
    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
