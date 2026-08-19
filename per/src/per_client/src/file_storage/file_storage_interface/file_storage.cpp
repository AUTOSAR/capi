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
/// @file       file_storage.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Puhua File System
/// @date       2021-04-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/FileStorage/File Storage Interface
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-04-13 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/file_storage.h"

#include <algorithm>

#include "ara/core/result.h"
#include "ara/per/internal/common/isoft_file_opt_crypto.h"
#include "ara/per/internal/common/isoft_file_refuse_opt.h"
#include "ara/per/internal/common/isoft_per_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/crypto/isoft_per_redd_calculate.h"
#include "ara/per/internal/initialize.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/isoftkv/shared_ptr_manager.h"
#include "ara/per/internal/manifest/manifest_instance.h"
#include "ara/per/internal/open_storage_logic.h"
#include "ara/per/per_error_domain.h"
#include "ara/per/recovery.h"

//********************************/
namespace {
/// @brief The root path key of FileStorage within the KV library. If your file name conflicts with this, various errors will occur; in short, you're out of luck.
/// @return
constexpr inline ara::core::StringView G_GetFileStorageRootPathKey() noexcept
{
    constexpr ara::core::StringView kRootPathKey{"_kPhRootPath_"};
    return kRootPathKey;
}
/// @brief Iterate over all files
/// @param pKvSystem
/// @param pfun
/// @return
int32_t G_ScanAllFile(ara::per::isoftkv::PKvSystem *const pKvSystem,
                      ara::per::isoftkv::CB_DealStringView const &pfun) noexcept
{
    if (nullptr == pKvSystem) {
        return 0;
    }
    PER_Assert(nullptr != pKvSystem);
    if (pKvSystem->IsNullStore()) {
        return 0;
    }
    int32_t nFileCount{0};
    ara::per::isoftkv::CB_DealStringView pfunWork;
    pfunWork = [&nFileCount, &pfun](ara::core::StringView const &stFileName) -> bool {
        ara::core::StringView const &stRootPath{G_GetFileStorageRootPathKey()};
        if (stFileName == stRootPath) {
            return false;
        }
        if (pfun(stFileName)) {
            nFileCount += 1;
        }
        return true;
    };
    std::ignore = pKvSystem->ScanAllKey(pfunWork);
    return nFileCount;
}
/// @brief
/// @param pKvSystem
/// @return
inline ara::log::LogStream G_LogInfo(ara::per::isoftkv::PKvSystem const *const pKvSystem) noexcept
{
    ara::log::LogStream logStream{ara::per::isoftkv::LogInfo()};
    logStream << pKvSystem->GetLogKvName().data();
    return logStream;
}
/// @brief
/// @param portIns
/// @return
ara::core::String G_FindFileStorageUpdateStrategy(ara::core::InstanceSpecifier const &portIns) noexcept
{
    ara::core::String stDefUpdateType;
    ara::per::manifest::MConfig_StorageFile const *const pFindStorage{
        ara::per::manifest::PManifestInstance::FindStorage_File(portIns)};
    if (nullptr != pFindStorage) {
        stDefUpdateType = pFindStorage->dataStorage.stUpdateStrategy;
    }
    return stDefUpdateType;
}
/// @brief Determine whether modeAll contains modeOne
/// @param modeAll
/// @param modeOne
/// @return
inline bool G_IsHaveOpenMode(ara::per::OpenMode const modeAll, ara::per::OpenMode const modeOne) noexcept
{
    uint32_t const nModeAll{static_cast< uint32_t >(modeAll)};
    uint32_t const nModeOne{static_cast< uint32_t >(modeOne)};
    return (nModeAll & nModeOne) == nModeOne;
}
/// @brief Calculate the file's redundancy CRC/Hash code
/// @param [in] configMuster Configuration information
/// @param [in] fileOpenedFile operation object
/// @param [out] vecResult Calculation result
/// @return AlgID of the verification algorithm used
uint64_t G_CalculateReddData(ara::per::isoftkv::PConfigMuster const &configMuster,
                             ara::per::isoftkv::PFileOpt const &fileOpt,
                             ara::core::Vector< uint8_t > &vecResult) noexcept
{
    // First calculate potentially existing redundancy check data
    std::unique_ptr< ara::per::isoftkv::IReddAlgorithm > pReddAlgorithm{
        ara::per::isoftkv::MakeReddCheckAlgorithm(configMuster)};
    if (!pReddAlgorithm) {
        return 0U;
    }
    pReddAlgorithm->Reset();
    if (false == fileOpt.IsOpen()) {
        return 0;
    }
    if (fileOpt.GetFileSize() > 0) {
        int64_t const nOldPos{fileOpt.GetPos()};
        fileOpt.SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, 0);
        while (false == fileOpt.IsEof()) {
            uint8_t buff[ara::per::kInt_512U]{0U};
            // Note here calling the base class's raw data read function (requires calculating the hash code of the ciphertext)
            int32_t nReadLen = fileOpt.PFileOpt::ReadData(buff, ara::per::kInt_512U);
            if (nReadLen <= 0) {
                break;
            }
            if (false == pReddAlgorithm->CalculateReddData(buff, static_cast< uint32_t >(nReadLen))) {
                return 0U;
            }
        }
        fileOpt.SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nOldPos);
    }
    vecResult = pReddAlgorithm->GetResult();
    return pReddAlgorithm->GetAlgID();
}
/// @brief Calculate the file's redundancy CRC/Hash code
/// @param [in] configMuster Configuration information
/// @param [in] stFileName Name of the file to calculate
/// @param [out] vecResult Calculation result
/// @return AlgID of the verification algorithm used
uint64_t G_CalculateReddData(ara::per::isoftkv::PConfigMuster const &configMuster,
                             ara::core::StringView const &stFileName,
                             ara::core::Vector< uint8_t > &vecResult) noexcept
{
    if (false == ara::per::isoftkv::PFileOpt::IsFileExist(stFileName)) {
        std::unique_ptr< ara::per::isoftkv::IReddAlgorithm > pReddAlgorithm{
            ara::per::isoftkv::MakeReddCheckAlgorithm(configMuster)};
        if (!pReddAlgorithm) {
            return 0U;
        }
        return pReddAlgorithm->GetAlgID();
    }
    ara::per::isoftkv::PFileOpt fileOpt(ara::per::isoftkv::PFileOpt::ReadOnly());
    if (false == fileOpt.DoPrepareWork(stFileName)) {
        return 0;
    }
    return G_CalculateReddData(configMuster, fileOpt, vecResult);
}
/// @brief Get file extension information from the KV library
/// @param pKvSystem
/// @param fileName
/// @param pFileInfoInKv
/// @param pReddExt
/// @param pVecReddData
/// @return
bool G_GetFileInfoInKv(ara::per::isoftkv::PKvSystem *pKvSystem,
                       ara::core::StringView const fileName,
                       ara::per::isoftkv::PFileInfoInKvs *pFileInfoInKv,
                       ara::per::isoftkv::PFileInfoInKvs_ReddExt *pReddExt,
                       ara::core::Vector< uint8_t > *pVecReddData) noexcept
{
    PER_Assert(nullptr != pKvSystem);
    uint8_t pBuff[ara::per::kInt_1024U]{0U};
    int32_t const nReadLen{
        pKvSystem->ReadValueBinary< ara::per::kInt_1024U >(fileName, static_cast< uint8_t * >(pBuff))};
    if (pBuff[0] == 0U) {  // This output parameter value is not subsequently checked.
        pBuff[0] = 0U;
    }
    if ((nReadLen <= 0)) {
        return false;
    }
    uint32_t nIndex{0U};
    ara::per::isoftkv::PFileInfoInKvs *const pReadFileInfo{
        ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PFileInfoInKvs >(static_cast< uint8_t * >(pBuff + nIndex))};
    if (nullptr != pFileInfoInKv) {
        *pFileInfoInKv = *pReadFileInfo;
    }
    nIndex += sizeof(ara::per::isoftkv::PFileInfoInKvs);

    ara::per::isoftkv::PFileInfoInKvs_ReddExt *const pReadReddInfo{
        ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PFileInfoInKvs_ReddExt >(
            static_cast< uint8_t * >(pBuff + nIndex))};
    if (nullptr != pReddExt) {
        *pReddExt = *pReadReddInfo;
    }
    nIndex += sizeof(ara::per::isoftkv::PFileInfoInKvs_ReddExt);

    if (nullptr != pVecReddData) {
        for (uint32_t i = 0; i < pReadReddInfo->nResultLen; i++) {
            pVecReddData->push_back(*(pBuff + nIndex + i));
        }
    }
    nIndex += pReadReddInfo->nResultLen;
    return true;
}
}  // namespace
//********************************/
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
class PCallStorage : public ICallStorage
{
private:
    FileStorage &ownerStorage_;
    isoftkv::PConfigMuster_File const &configMuster_;

public:
    /// @brief Constructor
    /// @param pOwnerStorage
    explicit PCallStorage(FileStorage &ownerStorage, isoftkv::PConfigMuster_File const &configMuster)
        : ownerStorage_(ownerStorage), configMuster_(configMuster)
    {
    }
    /// @brief Invoke Storage-related functionality
    /// @param pFileOpt Pointer to the file operation object
    /// @return Whether successful
    bool CallStorage_WriteReddData(isoftkv::PFileOpt const *const pFileOpt,
                                   ara::core::StringView const &stFileName) noexcept override
    {
        if (nullptr == pFileOpt) {
            return false;
        }
        if (false == configMuster_.IsReddCheckType()) {
            return true;
        }
        ara::core::Vector< uint8_t > vecReddData;
        uint64_t const nReddAlgId{G_CalculateReddData(configMuster_, *pFileOpt, vecReddData)};

        // Read saved ReddData from KV library for verification
        isoftkv::PFileInfoInKvs const fileInfoRead{ownerStorage_.GetFileInfoInKvs(stFileName)};
        return ownerStorage_.WriteCheckReddData(stFileName, fileInfoRead, nReddAlgId, vecReddData);
    }
    /// @brief Check if space allows writing new data
    /// @param nAddLen
    /// @return
    bool CheckSpace(int32_t nAddLen) noexcept override { return ownerStorage_.CheckSpace(nAddLen); }
};
}  // namespace isoftkv
//********************************/
/// @brief Constructor
/// @param fileConfig
FileStorage::FileStorage(isoftkv::PConfigMuster_File const &fileConfig) noexcept
    : pFileConfig_{std::make_unique< isoftkv::PConfigMuster_File >()}
{
    PER_Assert(fileConfig.IsValid());
    *pFileConfig_ = fileConfig;
}
/// @brief Move constructor
/// @param fs
FileStorage::FileStorage(FileStorage &&fs) noexcept
    : pKvSystem_{std::move(fs.pKvSystem_)}
    , stRootPath_{std::move(fs.stRootPath_)}
    , pFileConfig_{std::move(fs.pFileConfig_)}
{
    fs.pKvSystem_.reset();
    fs.stRootPath_.clear();
}
/// @brief Move assignment function
/// @param fs
/// @return
FileStorage &FileStorage::operator=(FileStorage &&fs) &noexcept
{
    pKvSystem_   = std::move(std::move(fs).pKvSystem_);
    stRootPath_  = std::move(fs.stRootPath_);
    pFileConfig_ = std::move(fs.pFileConfig_);
    fs.pKvSystem_.reset();
    fs.stRootPath_.clear();
    fs.pFileConfig_.reset();
    return *this;
}
/// @brief Destructor
FileStorage::~FileStorage() noexcept
{
    // Save the data of its own KV system before the file center exits
    if (nullptr != pKvSystem_) {
        if (pKvSystem_->IsAccessReady()) {
            std::ignore = pKvSystem_->SyncWalogToMain(false);
        }
        std::ignore = pKvSystem_->CloseSystem();
        isoftkv::POpenStorageLogic< FileStorage >::DegStorage(pKvSystem_->GetSystemName());
    }
}
/// @brief Get all file names managed by this center
/// @return
ara::core::Result< ara::core::Vector< ara::core::String > > FileStorage::GetAllFileNames() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);

    ara::core::Vector< ara::core::String > vecKeyList;
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [&vecKeyList](ara::core::StringView const &stFileName) -> bool {
        vecKeyList.push_back(isoftkv::T_String(stFileName));
        return true;
    };
    int32_t const nFileCount{G_ScanAllFile(pKvSystem_.get(), pfunWork)};
    G_LogInfo(GetKvSystem()) << "[FileStorage].GetAllFileNames : FileCount =" << nFileCount;
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(std::move(vecKeyList));
}
/// @brief Delete a file
/// @param fileName
/// @return
ara::core::Result< void > FileStorage::DeleteFile(ara::core::StringView const fileName) noexcept
{
    /// 2025-11-13 niuliming: [SWS_PER_00111] File is not allowed to be opened
    if (!ara::per::isoftkv::PFileRefuseOpt::GetInstance()->AllowOperation(fileName)) {
        ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    ara::core::Result< void > res{_RemoveFile(fileName, true)};
    ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
    return res;
}
/// @brief Check if a file exists
/// @param fileName
/// @return
ara::core::Result< bool > FileStorage::FileExists(ara::core::StringView const fileName) const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bExist{pKvSystem_->IsKeyExist(fileName)};
    if (false == bExist) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< bool >(pKvSystem_->GetLastError());
        }
        return ara::core::Result< bool >::FromValue(false);
    }
    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};

    int32_t nReddCount{1};
    if (reddDataMofN.IsValid()) {
        nReddCount = static_cast< int32_t >(reddDataMofN.n);
    }
    PerErrc perError{PerErrc::kPhErrorCode};
    int nScanCount{0};
    uint32_t const nFileFlags{isoftkv::PFileOpt::ReadOnly()};
    ara::per::isoftkv::PConfigData_Crypto cryptoConfig{pFileConfig_->GetCryptoConfig()};
    nScanCount = isoftkv::ScanAllReddFile(
        stFileName, nReddCount,
        [nFileFlags, &perError, cryptoConfig](ara::core::StringView const &stNewFileName) noexcept -> bool {
            std::unique_ptr< isoftkv::PFileOpt_Crypto > pFileOpt;
            pFileOpt = std::make_unique< isoftkv::PFileOpt_Crypto >(nFileFlags);
            if (false == pFileOpt->DoPrepareWork(stNewFileName)) {
                perError = PerErrc::kFileNotFound;
                return false;
            }
            if (cryptoConfig.stKeySlotName.empty() == false) {
                if (false == pFileOpt->BuildCrypto(cryptoConfig.stKeySlotName, cryptoConfig.stCryptoAlgorithm)) {
                    perError = PerErrc::kEncryptionFailed;
                    return false;
                }
            }
            pFileOpt.release();
            //vecReddFileOpt_.push_back(std::move(pFileOpt));
            return true;
        });
    if (PerErrc::kPhErrorCode == perError) {
        return ara::core::Result< bool >::FromValue(nReddCount == nScanCount);
    }
    return ara::core::Result< bool >::FromError(perError);
}
ara::core::Result< void > FileStorage::RecoverFile(ara::core::StringView const fileName) noexcept
{
    /// 2025-11-13 niuliming: [SWS_PER_00376] File is not allowed to be opened
    if (!ara::per::isoftkv::PFileRefuseOpt::GetInstance()->AllowOperation(fileName)) {
        ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    ara::core::Result< void > res{_RecoverFile(fileName)};
    ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
    return res;
}
/// @brief Restore a file
/// @param fileName
/// @return
ara::core::Result< void > FileStorage::_RecoverFile(ara::core::StringView const &fileName) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bExist{pKvSystem_->IsKeyExist(fileName)};
    if (false == bExist) {
        return ara::core::Result< void >::FromError(PerErrc::kFileNotFound);
    }
    ara::core::Result< void > result{ara::core::Result< void >::FromValue()};
    isoftkv::PReddDataMofN reddMonN{pFileConfig_->FindFileReddMofN(fileName)};
    if (false == reddMonN.IsValid()) {
        return result;
    }
    ara::core::Vector< uint8_t > reportedInstances;
    if (false == _RecoverFile(fileName, &reportedInstances)) {
        result = _ResetFileNoLock(fileName);
        for (uint8_t i = 0; i < static_cast< uint8_t >(reddMonN.n); i++) {
            reportedInstances.push_back(i);
        }
    } else {
        if (pKvSystem_->IsHaveError()) {
            result = ara::per::isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    if (IsHaveRecoverReport()) {
        ara::core::InstanceSpecifier const storageIns{pFileConfig_->GetStorageIns()};
        ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kFileRecovered};
        ara::core::Vector< ara::core::String > reportedElements;
        reportedElements.push_back(isoftkv::T_String(fileName));
        // An empty reportedInstances indicates the Recover method is not needed
        if (false == result.HasValue()) {
            recoveryReportKind = ara::per::RecoveryReportKind::kFileRecoveryFailed;
        }
        RecoveryReport(storageIns, recoveryReportKind, reportedElements, reportedInstances);
    }
    std::ignore = pKvSystem_->SyncWalogToMain(true);
    return result;
}
/// @brief Reset a file
/// @param fileName
/// @return
ara::core::Result< void > FileStorage::ResetFile(ara::core::StringView const fileName) noexcept
{
    /// 2025-11-13 niuliming: [SWS_PER_00376] File is not allowed to be opened
    if (!ara::per::isoftkv::PFileRefuseOpt::GetInstance()->AllowOperation(fileName)) {
        ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    ara::core::Result< void > res{_ResetFileNoLock(fileName)};
    ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
    return res;
}
/// @brief Reset a file
/// @param fileName
/// @return
ara::core::Result< void > FileStorage::_ResetFileNoLock(ara::core::StringView const fileName) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    // First delete any existing
    ara::core::Result< void > resultRemove{_RemoveFile(fileName, true)};
    if (false == resultRemove.HasValue()) {
        return resultRemove;
    }
    // Then rebuild from configuration
    ara::core::Result< void > resultReset{_ResetFile(fileName)};
    if (false == resultReset.HasValue()) {
        return resultReset;
    }
    if (false == pKvSystem_->SyncWalogToMain(true)) {
        return ara::core::Result< void >::FromError(TransErrorCode(pKvSystem_->GetLastError()));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the current file's occupied space in bytes
/// @param fileName
/// @return
ara::core::Result< uint64_t > FileStorage::GetCurrentFileSize(ara::core::StringView const fileName) const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bExist{pKvSystem_->IsKeyExist(fileName)};
    if (false == bExist) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< uint64_t >(pKvSystem_->GetLastError());
        }
    }

    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    ReadAccessor fileAccessor{reddDataMofN};
    ara::core::Result< bool > resultInit{
        fileAccessor.InitFileOpt(isoftkv::T_StringView(stFileName), isoftkv::PFileOpt::ReadOnly())};
    if (false == resultInit.HasValue()) {
        return isoftkv::T_ErrorResult< bool, uint64_t >(resultInit);
    }
    return ara::core::Result< uint64_t >::FromValue(fileAccessor.GetSize());
}
/// @brief Get file status information
/// @param fileName
/// @return
ara::core::Result< FileInfo > FileStorage::GetFileInfo(ara::core::StringView const fileName) const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< FileInfo >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    ara::core::String const stFileInPath{_GetFileName(fileName)};
    // Populate file information
    isoftkv::PFileInfoInKvs fileInfoRead;
    if (false == G_GetFileInfoInKv(pKvSystem_.get(), fileName, &fileInfoRead, nullptr, nullptr)) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< FileInfo >(pKvSystem_->GetLastError());
        }
    }
    FileInfo fileInfo;
    fileInfo.accessTime            = isoftkv::PFileOpt::GetFileAccessTime(isoftkv::T_StringView(stFileInPath));
    fileInfo.creationTime          = isoftkv::PFileOpt::GetFileCreateTime(isoftkv::T_StringView(stFileInPath));
    fileInfo.modificationTime      = isoftkv::PFileOpt::GetFileModifyTime(isoftkv::T_StringView(stFileInPath));
    fileInfo.fileCreationState     = static_cast< FileCreationState >(fileInfoRead.fileCreationState);
    fileInfo.fileModificationState = static_cast< FileModificationState >(fileInfoRead.fileModificationState);
    return ara::core::Result< FileInfo >::FromValue(std::move(fileInfo));
}
/// @brief Open a readable and writable file
/// @param fileName
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileReadWrite(
    ara::core::StringView const fileName) noexcept
{
    return _OpenFileWrite(false, fileName, nullptr, nullptr);
}
/// @brief Open a readable and writable file using the given mode
/// @param fileName
/// @param mode
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileReadWrite(
    ara::core::StringView const fileName, OpenMode const mode) noexcept
{
    return _OpenFileWrite(false, fileName, &mode, nullptr);
}
/// @brief Open a readable and writable file using the given mode and buffer
/// @param fileName
/// @param mode
/// @param buffer
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileReadWrite(
    ara::core::StringView const fileName, OpenMode const mode, ara::core::Span< ara::core::Byte > buffer) noexcept
{
    return _OpenFileWrite(false, fileName, &mode, &buffer);
}
/// @brief Open a file in read-only mode
/// @param fileName
/// @return
ara::core::Result< UniqueHandle< ReadAccessor > > FileStorage::OpenFileReadOnly(
    ara::core::StringView const fileName) noexcept
{
    return _OpenFileReadOnly(fileName, nullptr, nullptr);
}
/// @brief Open a file in read-only mode using the given mode
/// @param fileName
/// @param mode
/// @return
ara::core::Result< UniqueHandle< ReadAccessor > > FileStorage::OpenFileReadOnly(ara::core::StringView const fileName,
                                                                                OpenMode const mode) noexcept
{
    return _OpenFileReadOnly(fileName, &mode, nullptr);
}
/// @brief Open a file in read-only mode using the given mode and buffer
/// @param fileName
/// @param mode
/// @param buffer
/// @return
ara::core::Result< UniqueHandle< ReadAccessor > > FileStorage::OpenFileReadOnly(
    ara::core::StringView const fileName, OpenMode const mode, ara::core::Span< ara::core::Byte > buffer) noexcept
{
    return _OpenFileReadOnly(fileName, &mode, &buffer);
}
/// @brief Open a file in write-only mode
/// @param fileName
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileWriteOnly(
    ara::core::StringView const fileName) noexcept
{
    return _OpenFileWrite(true, fileName, nullptr, nullptr);
}
/// @brief
/// @param fileName
/// @param mode
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileWriteOnly(
    ara::core::StringView const fileName, OpenMode const mode) noexcept
{
    return _OpenFileWrite(true, fileName, &mode, nullptr);
}
/// @brief Open a file in write-only mode using the given mode and buffer
/// @param fileName
/// @param mode
/// @param buffer
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::OpenFileWriteOnly(
    ara::core::StringView const fileName, OpenMode const mode, ara::core::Span< ara::core::Byte > buffer) noexcept
{
    return _OpenFileWrite(true, fileName, &mode, &buffer);
}
//********************************/
/// @brief Initialize FileStorage using configuration from Manifest
/// @param storageIns
/// @param appVersion
/// @param perVersion
/// @return
ara::core::Result< void > FileStorage::InitStorage(ara::core::InstanceSpecifier const &storageIns,
                                                   bool bNewBuild) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    std::ignore = bNewBuild;
    ara::core::Result< void > const resultInit{PrepareKvSystem()};
    if (false == resultInit.HasValue()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);

    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kStorageNotFound);
    }
    ara::core::String stDefUpdateType{G_FindFileStorageUpdateStrategy(storageIns)};
    // Initialize files managed by the file system according to the files in the Manifest configuration
    ara::core::Map< ara::core::String, ara::core::String > mapAddFile{};
    // Delete unused files SWS_PER_00395
    ara::core::Map< ara::core::String, int32_t > mapFileInUse{};
    isoftkv::EUpdateStrategy_Storage eUpdateStorage{isoftkv::TransUpdate_Storage(stDefUpdateType)};
    if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
        ara::core::Result< ara::core::Vector< ara::core::String > > resultAllFile{GetAllFileNames()};
        if (resultAllFile.HasValue()) {
            for (auto const &stFileName : resultAllFile.Value()) {
                mapFileInUse[stFileName] = 0;
            }
        }
    }
    ara::core::String const stPortIns{isoftkv::T_String(storageIns.ToString())};
    int32_t nScanCount{0};
    nScanCount = pManifestPer->ForEachFileStorageInitFile(
        stPortIns,
        [this, &stDefUpdateType, &mapAddFile, &mapFileInUse,
         eUpdateStorage](manifest::MConfigData_InitFile const &initFile) -> bool {
            // Perform update operation based on conditions
            ara::core::String const &stFileName{initFile.stFileName};
            isoftkv::EUpdateStrategy_Element eUpdateElement{isoftkv::TransUpdate_Element(initFile.stUpdateType)};
            if (eUpdateElement == isoftkv::EUpdateStrategy_Element::kUndefine) {
                eUpdateElement = isoftkv::TransUpdate_Element(stDefUpdateType);
            }
            if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
                mapFileInUse[initFile.stFileName] += 1;
            }
            bool bAddNewFile{false};
            switch (eUpdateElement) {
                case isoftkv::EUpdateStrategy_Element::kOverwrite: {
                    bAddNewFile = true;
                    G_LogInfo(GetKvSystem()) << "[FileStorage].UpdateStrategy.kOverwrite[" << stFileName.data()
                                             << "], AddNewFile =" << bAddNewFile;
                } break;
                case isoftkv::EUpdateStrategy_Element::kKeepExisting: {
                    if (false == this->FileExists(isoftkv::T_StringView(stFileName))) {
                        bAddNewFile = true;
                    }
                    G_LogInfo(GetKvSystem()) << "[FileStorage].UpdateStrategy.kKeepExisting[" << stFileName.data()
                                             << "], AddNewFile =" << bAddNewFile;
                } break;
                case isoftkv::EUpdateStrategy_Element::kDelete: {
                    ara::core::Result< void > resultDel{this->_RemoveFile(isoftkv::T_StringView(stFileName), true)};
                    G_LogInfo(GetKvSystem()) << "[FileStorage].UpdateStrategy.kDelete[" << stFileName.data()
                                             << "], Result =" << resultDel.HasValue();
                } break;
                default: {
                    G_LogInfo(GetKvSystem()) << "[FileStorage].UpdateStrategy.unknown[" << initFile.stUpdateType.data();
                    return false;
                } break;
            }
            if (bAddNewFile) {
                mapAddFile[stFileName] = initFile.stFileUri;
            }
            return true;
        });
    for (auto &it : mapAddFile) {
        // Handle M/N redundancy when creating a new file
        ara::core::String const &stFileName{it.first};
        ara::core::StringView const stFileUri{std::move(isoftkv::T_StringView(it.second))};
        isoftkv::CB_DealStringView pfunWork;
        pfunWork = [this, stFileName, stFileUri](ara::core::StringView const &stNewFileName) noexcept -> bool {
            // 2023-12-11 hanyuxin add possible encryption initialization
            return _CopyStorageFile(stFileName, stFileUri, stNewFileName);
        };
        std::ignore = _ScanAllReddFile(isoftkv::T_StringView(stFileName), pfunWork);
        // The file content updated here should set FileInfo.fileCreationState = FileCreationState.kCreatedDuringUpdate
        ara::core::Result< bool > resultAdd{_AddNewFileToKv(isoftkv::T_StringView(stFileName),
                                                            FileCreationState::kCreatedDuringUpdate,
                                                            FileModificationState::kModifiedDuringUpdate)};
        if (false == resultAdd.HasValue()) {
            return isoftkv::T_ErrorResult< bool, void >(resultAdd);
        }
        G_LogInfo(GetKvSystem()) << "[FileStorage].InitStorage.AddFile[" << it.first.c_str()
                                 << "], Result =" << resultAdd.Value();
    }
    // Delete unused files SWS_PER_00395
    if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
        for (auto &it : mapFileInUse) {
            if (it.second <= 0) {
                ara::core::Result< void > resultDel{this->_RemoveFile(it.first, true)};
                G_LogInfo(GetKvSystem()) << "[FileStorage].InitStorage.DeleteFile[" << it.first.c_str()
                                         << "], Result =" << resultDel.HasValue();
            }
        }
    }
    std::ignore = pKvSystem_->UpdateVersion();
    std::ignore = pKvSystem_->SyncWalogToMain(true);
    G_LogInfo(GetKvSystem()) << "[FileStorage].InitStorage : FileCount =" << nScanCount;
    return ara::core::Result< void >::FromValue();
}
/// @brief Initialize the FileStorage's KVS library using configuration generated from FileConfig
/// @return
ara::core::Result< void > FileStorage::PrepareKvSystem() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    stRootPath_ = pFileConfig_->GetWorkPath();
    if (nullptr == GetKvSystem()) {
        isoftkv::PConfigMuster_Kv kvConfig;
        std::ignore = kvConfig.SetConfig(*pFileConfig_);
        pKvSystem_  = isoftkv::MakeUniqueKvSystem(kvConfig, true);
        pKvSystem_->SetBakeupType(isoftkv::EKvBakType::kKvBakToDirent);  // File system backup uses directory method
    }
    if (nullptr == GetKvSystem()) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    bool bRebuildKvSystem{true};
    if (false == pKvSystem_->IsExist()) {
        std::ignore = pKvSystem_->NewSystem(1U);
    } else  // If the Kv library exists and is not empty, continue checking whether it is the Kv library used by FileStorage
    {
        if (false == pKvSystem_->IsNullStore()) {
            if (pKvSystem_->IsKeyExist(G_GetFileStorageRootPathKey())) {
                ara::core::String stSaveWorkPath;
                stSaveWorkPath = pKvSystem_->ReadValueString(G_GetFileStorageRootPathKey());
                if (stSaveWorkPath != stRootPath_)  // Wrong format, delete and rebuild
                {
                    std::ignore = pKvSystem_->RemoveSystem();
                    std::ignore = pKvSystem_->NewSystem(1U);
                } else {
                    bRebuildKvSystem = false;  // Only in this case is rebuilding the KV library not required
                }
            }
        }
    }
    std::ignore = pKvSystem_->OpenSystem();
    if (false == pKvSystem_->IsAccessReady()) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    // Initialize the KVS library used by FileStorage
    if (bRebuildKvSystem) {
        if (false == pKvSystem_->WriteValue(G_GetFileStorageRootPathKey(), stRootPath_)) {
            return ara::core::Result< void >::FromError(PerErrc::kIllegalWriteAccess);
        }
    }
    // Set App and Per version numbers here
    return ara::core::Result< void >::FromValue();
}
/// @brief Return the engine pointer of the Kv library
/// @return
isoftkv::PKvSystem *FileStorage::GetKvSystem() const noexcept
{
    if (nullptr == pKvSystem_) {
        return nullptr;
    }
    return pKvSystem_.get();
}
/// @brief Backup the current library
/// @return
ara::core::Result< bool > FileStorage::BackupMain() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< bool >::FromError(PerErrc::kStorageNotFound);
    }
    // Regardless of whether the KvSystem is empty, start the backup logic
    isoftkv::PKvSystem &kvSystem{*pKvSystem_};
    // Find the SRC directory, create the DST directory, iterate through the KvStorage content, back up files one by one, and finally back up itself
    ara::core::String stBakPath;
    stBakPath = kvSystem.GetBakPath();
    if (isoftkv::PFileOpt::IsFileExist(stBakPath) == false) {
        if (!isoftkv::PFileOpt::MakeDir(stBakPath, true, isoftkv::kDefDirMode)) {
            return ara::core::Result< bool >::FromError(PerErrc::kPhKvCreateDirFailed);
        }
    }
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [this, stBakPath](ara::core::StringView const &fileName) noexcept -> bool {
        ara::core::String stFileSrc;
        stFileSrc = _SelectFileMofN(fileName);
        //
        ara::core::String stFileDst;
        stFileDst = isoftkv::PFileOpt::MakeFileName(isoftkv::T_StringView(stBakPath), fileName);
        return isoftkv::PFileOpt::CopyFile(isoftkv::T_StringView(stFileSrc), isoftkv::T_StringView(stFileDst));
    };
    int32_t nFileCount{G_ScanAllFile(&kvSystem, pfunWork)};
    // KV library backup is placed in the working directory, not the backup directory
    bool const bReturn{kvSystem.BackupSystem(true)};
    if (bReturn) {
        nFileCount += 1;
    }
    return ara::core::Result< bool >::FromValue(nFileCount > 0);
}
/// @brief Delete the main library
/// @return
ara::core::Result< bool > FileStorage::RemoveMain() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< bool >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [this](ara::core::StringView const &stKeyView) noexcept -> bool {
        /// First delete files
        ara::core::Result< void > resultRemove{_RemoveFile(stKeyView, false)};
        if (false == resultRemove.HasValue()) {
            return false;
        }
        return true;
    };
    /// Iterate over all files and delete them one by one
    std::ignore = G_ScanAllFile(pKvSystem_.get(), pfunWork);
    // Delete KV storage library files
    std::ignore = pKvSystem_->RemoveSystem();
    return ara::core::Result< bool >::FromValue();
}
/// @brief Delete the backup KV of the main library
/// @param bClose Whether to close the KvSystem before deletion
/// @return
ara::core::Result< bool > FileStorage::RemoveMainBak(bool bClose) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< bool >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);
    if (bClose) {
        std::ignore = pKvSystem_->CloseSystem();
    }
    std::ignore = pKvSystem_->RemoveBackupFile();
    return ara::core::Result< bool >::FromValue();
}
/// @brief Restore the current library using the backup library
/// @return
ara::core::Result< bool > FileStorage::RecoverFromBackup() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< bool >::FromError(PerErrc::kStorageNotFound);
    }
    isoftkv::PKvSystem &kvSystem{*pKvSystem_};
    // Restore KV first, then files
    bool const bRecover{kvSystem.RecoverFromBackup()};
    if (false == bRecover) {
        return ara::core::Result< bool >::FromValue(false);
    }
    int32_t nRecoverTotal{0};
    if (kvSystem.OpenSystem()) {
        nRecoverTotal += 1;
    }

    ara::core::Vector< ara::core::String > vecKeyList{};
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [&vecKeyList](ara::core::StringView const &stFileName) -> bool {
        vecKeyList.push_back(isoftkv::T_String(stFileName));
        return true;
    };
    std::ignore = G_ScanAllFile(&kvSystem, pfunWork);
    ara::core::String const stBakPath{kvSystem.GetBakPath()};
    for (ara::core::String &stFileName : vecKeyList) {
        ara::core::String stBakName;
        stBakName
            = isoftkv::PFileOpt::MakeFileName(isoftkv::T_StringView(stBakPath), isoftkv::T_StringView(stFileName));
        isoftkv::CB_DealStringView pfunCopy;
        pfunCopy = [stBakName](ara::core::StringView const &stNewFileName) noexcept -> bool {
            return isoftkv::PFileOpt::CopyFile(isoftkv::T_StringView(stBakName), stNewFileName);
        };
        nRecoverTotal += _ScanAllReddFile(isoftkv::T_StringView(stFileName), pfunCopy);
    }
    return ara::core::Result< bool >::FromValue(nRecoverTotal > 0);
}
/// @brief Delete the backup library
/// @return
ara::core::Result< bool > FileStorage::RemoveBackup() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< bool >::FromError(PerErrc::kStorageNotFound);
    }
    // First find the backup directory
    isoftkv::PConfigMuster_Kv bakConfig{pKvSystem_->GetBakConfigMuster()};
    std::unique_ptr< isoftkv::PKvSystem > pKvSystemBak{isoftkv::MakeUniqueKvSystem(bakConfig, false)};
    isoftkv::PKvSystem &kvSystemBak{*(pKvSystemBak.get())};
    ara::core::StringView stBakWorkPath{isoftkv::T_StringView(bakConfig.GetWorkPath())};
    if (false == kvSystemBak.IsExist()) {
        if (isoftkv::PFileOpt::IsFileExist(stBakWorkPath)) {
            std::ignore = isoftkv::PFileOpt::RemoveDir(stBakWorkPath);
        }
        return ara::core::Result< bool >::FromValue(true);
    }
    std::ignore = kvSystemBak.OpenSystem();
    if (false == kvSystemBak.IsNullStore()) {
        if (false == kvSystemBak.IsAccessReady()) {
            if (isoftkv::PFileOpt::IsFileExist(stBakWorkPath)) {
                std::ignore = isoftkv::PFileOpt::RemoveDir(stBakWorkPath);
            }
            return ara::core::Result< bool >::FromValue(false);
        }
        // Iterate through KvStorage content, delete files one by one, finally delete itself
        isoftkv::CB_DealStringView pfunWork;
        pfunWork = [stBakWorkPath](ara::core::StringView const &fileName) noexcept -> bool {
            ara::core::String const stBakFileUri{isoftkv::PFileOpt::MakeFileName(stBakWorkPath, fileName)};
            return isoftkv::PFileOpt::DelFile(isoftkv::T_StringView(stBakFileUri));
        };
        std::ignore = G_ScanAllFile(&kvSystemBak, pfunWork);
    }
    // Close the backup library's KvSystem before deleting files
    std::ignore = kvSystemBak.CloseSystem();
    std::ignore = kvSystemBak.RemoveSystem();
    isoftkv::PFileOpt::RemoveDir(stBakWorkPath);
    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Get the full path file name, converting a relative file name to a full path file name
/// @param fileName
/// @return
ara::core::Result< ara::core::String > FileStorage::GetFileName(ara::core::StringView const &fileName) const noexcept
{
    /// @brief
    using PResult = ara::core::Result< ara::core::String >;
    if (false == FileExists(fileName).Value()) {
        return PResult::FromError(PerErrc::kFileNotFound);
    }
    PResult result{_GetFileName(fileName)};
    return result;
}
/// @brief Restore all files
/// @return
ara::core::Result< void > FileStorage::RecoverAllFile() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);
    int32_t nFileTotal{0};
    ara::core::Vector< uint8_t > reportedInstances;
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [this, &nFileTotal, &reportedInstances](ara::core::StringView const &stFileName) noexcept -> bool {
        nFileTotal += 1;
        ara::core::Vector< uint8_t > reportedTemp;
        bool const bReturn{_RecoverFile(stFileName, &reportedTemp)};
        for (auto &nReddIndex : reportedTemp) {
            if (reportedInstances.end() == std::find(reportedInstances.begin(), reportedInstances.end(), nReddIndex)) {
                reportedInstances.push_back(nReddIndex);
            }
        }

        return bReturn;
    };
    int32_t nFileCount{0};
    nFileCount = G_ScanAllFile(pKvSystem_.get(), pfunWork);
    G_LogInfo(GetKvSystem()) << "[FileStorage].RecoverAllFile : FileCount =" << nFileCount;
    // Restore callback
    if (IsHaveRecoverReport()) {
        ara::core::InstanceSpecifier const storageIns{pFileConfig_->GetStorageIns()};
        ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kFileStorageRecovered};
        if (nFileCount < nFileTotal) {
            recoveryReportKind = ara::per::RecoveryReportKind::kFileStorageRecoveryFailed;
        }
        RecoveryReport(storageIns, recoveryReportKind, ara::core::Vector< ara::core::String >{}, reportedInstances);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Reset all files
/// @param fs Port identifier
/// @return
ara::core::Result< void > FileStorage::ResetStorage(ara::core::InstanceSpecifier const &fs) noexcept
{
    // Delete first
    ara::core::Result< bool > resultRemove{RemoveMain()};
    if (false == resultRemove.HasValue()) {
        return isoftkv::T_ErrorResult< bool, void >(resultRemove);
    }
    resultRemove = RemoveMainBak(true);
    if (false == resultRemove.HasValue()) {
        return isoftkv::T_ErrorResult< bool, void >(resultRemove);
    }
    return InitStorage(fs, true);
}
/// @brief Get the total space occupied by all files in bytes
/// @return
ara::core::Result< uint64_t > FileStorage::GetStorageSpace() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);
    ara::core::Vector< ara::core::String > vecAllFile{};
    // Count the size of all files including redundant files
    int32_t nFileCount{0};
    nFileCount = G_ScanAllFile(pKvSystem_.get(), [&vecAllFile](ara::core::StringView const &fileName) -> bool {
        vecAllFile.push_back(isoftkv::T_String(fileName));
        return true;
    });
    uint64_t nFileSpaceTotal{0U};
    uint64_t *const pnFileSpaceTotal{&nFileSpaceTotal};
    for (auto &it : vecAllFile) {
        isoftkv::CB_DealStringView pfunWork;
        pfunWork = [pnFileSpaceTotal](ara::core::StringView const &stReddFile) noexcept -> bool {
            isoftkv::PFileOpt fileOpt{isoftkv::PFileOpt::ReadOnly()};
            if (fileOpt.DoPrepareWork(stReddFile)) {
                *pnFileSpaceTotal += static_cast< uint64_t >(fileOpt.GetFileSize());
            }
            return true;
        };
        std::ignore = _ScanAllReddFile(isoftkv::T_StringView(it), pfunWork);
    }
    // Add the size of the KV library: already includes the backup library size
    nFileSpaceTotal += pKvSystem_->GetSpaceSize();
    // Also add the size of the backup library: backup files and backup KV library size
    ara::core::String const stBakKvFile{pKvSystem_->GetBakFileName(isoftkv::ECacheSource::kMain)};
    if (isoftkv::PFileOpt::IsFileExist(isoftkv::T_StringView(stBakKvFile))) {
        isoftkv::PConfigMuster_Kv const configKvBak{pKvSystem_->GetBakConfigMuster()};
        std::unique_ptr< isoftkv::PKvSystem > pKvSystemBak{isoftkv::MakeUniqueKvSystem(configKvBak, false)};
        isoftkv::PKvSystem &kvSystemBak{*(pKvSystemBak.get())};
        ara::core::StringView const stBakPath{kvSystemBak.GetWorkPath()};
        if (kvSystemBak.OpenSystem()) {
            isoftkv::CB_DealStringView pfunWork;
            pfunWork = [pnFileSpaceTotal, stBakPath](ara::core::StringView const &fileName) noexcept -> bool {
                ara::core::String const stBakFile{isoftkv::PFileOpt::MakeFileName(stBakPath, fileName)};
                isoftkv::PFileOpt fileOpt{isoftkv::PFileOpt::ReadOnly()};
                if (fileOpt.DoPrepareWork(isoftkv::T_StringView(stBakFile))) {
                    *pnFileSpaceTotal += static_cast< uint64_t >(fileOpt.GetFileSize());
                }
                return true;
            };
            nFileCount += G_ScanAllFile(&kvSystemBak, pfunWork);
        }
    }
    if (nFileSpaceTotal > 0U) {  // Apparent: An object in an unspecified state is used.
    }
    G_LogInfo(GetKvSystem()) << "[FileStorage].GetFileSize : FileCount =" << nFileCount
                             << ", FileSpaceTotal =" << static_cast< int32_t >(nFileSpaceTotal);

    return ara::core::Result< uint64_t >::FromValue(nFileSpaceTotal);
}
/// @brief Save file extension information and redundancy check information
/// @param stFileName
/// @param fileInfo
/// @param nReddAlgId
/// @param vecReddData
/// @return
bool FileStorage::WriteCheckReddData(ara::core::StringView const &stFileName,
                                     isoftkv::PFileInfoInKvs const &fileInfo,
                                     uint64_t const nReddAlgId,
                                     ara::core::Vector< uint8_t > const &vecReddData) const noexcept
{
    // Set PFileInfoInKvs_ReddExt
    isoftkv::PFileInfoInKvs_ReddExt reddEx;
    reddEx.nReddType  = static_cast< uint8_t >(pFileConfig_->GetReddType());
    reddEx.nAlgID     = nReddAlgId;
    reddEx.nResultLen = static_cast< uint8_t >(vecReddData.size());
    return _WriteCheckReddData(stFileName, fileInfo, reddEx, vecReddData);
}
/// @brief Get file extension information from the KV library
/// @param fileName
/// @return PFileInfoInKvs structure saved in the KV library
ara::per::isoftkv::PFileInfoInKvs FileStorage::GetFileInfoInKvs(ara::core::StringView const fileName) const noexcept
{
    ara::per::isoftkv::PFileInfoInKvs fileInfoInKvs;
    std::ignore = G_GetFileInfoInKv(pKvSystem_.get(), fileName, &fileInfoInKvs, nullptr, nullptr);
    return fileInfoInKvs;
}
/// @brief Get file redundancy check information from the KV library
/// @param fileName
/// @return Redundancy check information
ara::core::Vector< uint8_t > FileStorage::GetFileReddData(ara::core::StringView const fileName) const noexcept
{
    ara::core::Vector< uint8_t > vecReddData;
    std::ignore = G_GetFileInfoInKv(pKvSystem_.get(), fileName, nullptr, nullptr, &vecReddData);
    return vecReddData;
}
/// @brief Calculate the redundancy check code for the file
/// @param fileName
/// @return
ara::core::Vector< uint8_t > FileStorage::CalculateFileReddData(ara::core::StringView const fileName) const noexcept
{
    ara::core::Vector< uint8_t > vecReddData;
    if (false == pFileConfig_->IsReddCheckType()) {
        return vecReddData;
    }
    if (nullptr == pKvSystem_) {
        return vecReddData;
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bExist{pKvSystem_->IsKeyExist(fileName)};
    if (false == bExist) {
        return vecReddData;
    }
    ara::core::String const stFileName{_GetFileName(fileName)};
    if (0U == G_CalculateReddData(*(pFileConfig_.get()), stFileName, vecReddData)) {
        return vecReddData;
    }
    return vecReddData;
}
/// @brief Get the redundant file name with path corresponding to filename
/// @param fileName File key within the KV library
/// @param nReddIndex Redundancy index
/// @return Local file name with path
ara::core::String FileStorage::GetReddFileName(ara::core::StringView const fileName, int32_t nReddIndex) const noexcept
{
    ara::core::String const stFileName{_GetFileName(fileName)};
    return isoftkv::MakeReddFileName(stFileName, nReddIndex);
}
/// @brief Get file extension information from the KV library
/// @param fileName
/// @return PFileInfoInKvs structure saved in the KV library
isoftkv::PFileInfoInKvs_ReddExt FileStorage::ReadFileInfoEx(ara::core::StringView const fileName,
                                                            uint16_t nVersion) const noexcept
{
    ara::per::isoftkv::PFileInfoInKvs_ReddExt fileInfoEx;
    std::ignore = G_GetFileInfoInKv(pKvSystem_.get(), fileName, nullptr, &fileInfoEx, nullptr);
    if (fileInfoEx.nVersion != nVersion) {
        return isoftkv::PFileInfoInKvs_ReddExt{};
    }
    return fileInfoEx;
}
/// @brief Set file extension information in the KV library
/// @param fileName
/// @return PFileInfoInKvs structure saved in the KV library
bool FileStorage::SaveFileInfoEx(ara::core::StringView const fileName,
                                 isoftkv::PFileInfoInKvs_ReddExt const &fileInfoEx) noexcept
{
    ara::per::isoftkv::PFileInfoInKvs fileInfo;
    ara::per::isoftkv::PFileInfoInKvs_ReddExt reddEx;
    ara::core::Vector< uint8_t > vecReddData;
    if (false == G_GetFileInfoInKv(pKvSystem_.get(), fileName, &fileInfo, &reddEx, &vecReddData)) {
        return false;
    }
    if (fileInfoEx.nVersion != reddEx.nVersion) {
        return false;
    }
    uint8_t nResultLen = reddEx.nResultLen;  // This value cannot be modified
    reddEx             = fileInfoEx;
    reddEx.nResultLen  = nResultLen;
    return _WriteCheckReddData(fileName, fileInfo, reddEx, vecReddData);
}
/// @brief Check if space allows writing new data
/// @param nAddLen
/// @return
bool FileStorage::CheckSpace(int32_t nAddLen) noexcept
{
    ara::core::Result< uint64_t > resultSpace{GetStorageSpace()};
    if (false == resultSpace.HasValue()) {
        return false;
    }
    uint64_t nNewSpace = isoftkv::T_AddInt< uint64_t >(resultSpace.Value(), nAddLen);
    if (nNewSpace < pFileConfig_->GetSpaceAmountMin()) {
        return false;
    }
    if (nNewSpace > pFileConfig_->GetSpaceAmountMax()) {
        return false;
    }
    return true;
}
//********************************/
/// @brief Convert relative file name to full path file name
/// @param fileName
/// @return
ara::core::String FileStorage::_GetFileName(ara::core::StringView const &fileName) const noexcept
{
    return isoftkv::PFileOpt::MakeFileName(isoftkv::T_StringView(stRootPath_), fileName);
}
/// @brief Add a new file
/// @param fileName
/// @param fileCreationState
/// @param fileModificationState
/// @return
ara::core::Result< bool > FileStorage::_AddNewFileToKv(ara::core::StringView const &fileName,
                                                       FileCreationState const fileCreationState,
                                                       FileModificationState const fileModificationState) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    if (nullptr == pKvSystem_) {
        G_LogInfo(GetKvSystem()) << "Find nullptr == pKvSystem_"
                                 << ": _AddNewFileToKv.fileName =" << fileName.data();
        return ara::core::Result< bool >::FromError(PerErrc::kIntegrityCorrupted);
    }
    isoftkv::PFileInfoInKvs fileInfo;
    fileInfo.accessTime            = isoftkv::TM_NowSecond();
    fileInfo.fileCreationState     = static_cast< uint8_t >(fileCreationState);
    fileInfo.fileModificationState = static_cast< uint8_t >(fileModificationState);
    /// Whether redundancy is CRC or Hash
    bool bResult{false};
    if (false == pFileConfig_->IsReddCheckType()) {
        bResult = pKvSystem_->WriteValue(fileName, isoftkv::T_TransBytes(&fileInfo), sizeof(fileInfo));
    } else {
        // First calculate potentially existing redundancy check data
        ara::core::Vector< uint8_t > vecReddData;
        ara::core::String const stFileName{_GetFileName(fileName)};
        uint64_t const nReddAlgId{G_CalculateReddData(*(pFileConfig_.get()), stFileName, vecReddData)};
        if (0U == nReddAlgId) {
            return ara::core::Result< bool >(PerErrc::kIntegrityCorrupted);
        }
        bResult = WriteCheckReddData(fileName, fileInfo, nReddAlgId, vecReddData);
    }
    if (false == bResult) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< bool >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< bool >::FromValue(bResult);
}
/// @brief Iterate over each redundant file; fileName is the name stored in the KV library memory (without path information)
/// @param fileName
/// @param pfun
/// @return
int32_t FileStorage::_ScanAllReddFile(
    ara::core::StringView const &fileName,
    std::function< bool(ara::core::StringView const &stReddFile) > const &pfun) const noexcept
{
    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [pfun](ara::core::StringView const &stNewFileName) noexcept -> bool { return pfun(stNewFileName); };
    return isoftkv::ScanAllReddFile(isoftkv::T_StringView(stFileName),
                                    static_cast< int32_t >(reddDataMofN.GetReddCountN()), pfunWork);
}
/// @brief Select the correct file in M/N mode
/// @param fileName
/// @return
ara::core::String FileStorage::_SelectFileMofN(ara::core::StringView const &fileName) const noexcept
{
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    ara::core::String const stUriName{_GetFileName(fileName)};
    // Return directly if M/N redundancy is not configured
    if (false == reddDataMofN.IsValid()) {
        return stUriName;
    }
    // Configure M/N redundancy
    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PSelectMofN const selectMofN{reddDataMofN.m, reddDataMofN.n};
    isoftkv::CB_DealInt32 pfunInt32;
    pfunInt32 = [stFileName](int32_t const nIndex) noexcept -> uint64_t {
        ara::core::String const stNewFileName{isoftkv::MakeReddFileName(stFileName, nIndex)};
        return static_cast< uint64_t >(isoftkv::PSelectMofN::CalFileHashID(stNewFileName));
    };
    int32_t const nMainIndex{selectMofN.SelectGoal(static_cast< uint64_t >(0), pfunInt32)};
    if ((nMainIndex < 0) || (nMainIndex >= static_cast< int32_t >(reddDataMofN.n))) {
        return stUriName;
    }
    return isoftkv::MakeReddFileName(stFileName, nMainIndex);
}
/// @brief Open a file
/// @param fileName
/// @param pMode
/// @param pBuffer
/// @return
ara::core::Result< UniqueHandle< ReadAccessor > > FileStorage::_OpenFileReadOnly(
    ara::core::StringView const &fileName,
    OpenMode const *const pMode,
    ara::core::Span< ara::core::Byte > *const pBuffer) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< UniqueHandle< ReadAccessor > >::FromError(PerErrc::kNotInitialized);
    }
    if (nullptr == pKvSystem_) {
        return ara::core::Result< UniqueHandle< ReadAccessor > >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);

    /// 2025-11-13 niuliming: [SWS_PER_00376] File is not allowed to be opened
    if (!ara::per::isoftkv::PFileRefuseOpt::GetInstance()->AllowOperation(fileName)) {
        ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kResourceBusy);
    }
    bool const bExist{pKvSystem_->IsKeyExist(fileName)};
    if (false == bExist) {
        ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
        return ara::core::Result< UniqueHandle< ReadAccessor > >::FromError(PerErrc::kFileNotFound);
    }
    // Perform redundancy check for read-only files
    if (pFileConfig_->IsEnableRedd()) {
        if (pFileConfig_->IsReddCheckType()) {
            if (false == _CheckReddData(fileName)) {
                ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
                return ara::core::Result< UniqueHandle< ReadAccessor > >::FromError(PerErrc::kValidationFailed);
            }
        } else {  // Attempt an automatic recovery
            std::ignore = _RecoverFile(fileName);
        }
    }
    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    std::unique_ptr< ReadAccessor > pFileAccessor{std::make_unique< ReadAccessor >(reddDataMofN)};
    // Set encryption configuration
    isoftkv::PConfigData_Crypto const *const pFindCrypto{
        pFileConfig_->IsHaveCrypto_Element(isoftkv::T_String(fileName))};
    if (nullptr != pFindCrypto) {
        pFileAccessor->AttachCryptoConfig(*pFindCrypto);
    } else {
        if (pFileConfig_->IsHaveCrypto_Storage(isoftkv::ECryptoKeySlotUsage::kHaveCrypto)) {
            pFileAccessor->AttachCryptoConfig(pFileConfig_->GetCryptoConfig());
        }
    }
    if (false == pFileAccessor->VerifCryptoHash()) {
        ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
        return ara::core::Result< UniqueHandle< ReadAccessor > >::FromError(PerErrc::kAuthenticationFailed);
    }
    // Handle Span.buffer
    if (nullptr != pBuffer) {
        pFileAccessor->AttachSpanBuff(pBuffer);
    }
    // Create file operation class object
    ara::core::Result< bool > resultInit{
        pFileAccessor->InitFileOpt(isoftkv::T_StringView(stFileName), isoftkv::PFileOpt::ReadOnly())};
    if (false == resultInit.HasValue()) {
        ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
        return isoftkv::T_ErrorResult< bool, UniqueHandle< ReadAccessor > >(resultInit);
    }
    // Handle mode
    if (nullptr != pMode) {
        if (G_IsHaveOpenMode(*pMode, OpenMode::kAtTheBeginning)) {
            std::ignore = pFileAccessor->MovePosition(Origin::kBeginning, 0);
        }
        if ((G_IsHaveOpenMode(*pMode, OpenMode::kAtTheEnd)) || (G_IsHaveOpenMode(*pMode, OpenMode::kAppend))) {
            std::ignore = pFileAccessor->MovePosition(Origin::kEnd, 0);
        }
    }

    pFileAccessor->InitOwnStorage(pFileConfig_->GetStorageIns(), fileName);
    return ara::core::Result< UniqueHandle< ReadAccessor > >::FromValue(
        UniqueHandle< ReadAccessor >{std::move(pFileAccessor)});
}
/// @brief Open a file in read-write mode
/// @param bWriteOnly
/// @param fileName
/// @param pMode
/// @param pBuffer
/// @return
ara::core::Result< UniqueHandle< ReadWriteAccessor > > FileStorage::_OpenFileWrite(
    bool const bWriteOnly,
    ara::core::StringView const &fileName,
    OpenMode const *const pMode,
    ara::core::Span< ara::core::Byte > *const pBuffer) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kNotInitialized);
    }
    isoftkv::PKvSystem *const pKvSystem{GetKvSystem()};
    if (nullptr == pKvSystem_) {
        return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem);

    /// 2025-11-13 niuliming: [SWS_PER_00377], [SWS_PER_00375] File is not allowed to be opened
    if (!ara::per::isoftkv::PFileRefuseOpt::GetInstance()->AllowOperation(fileName)) {
        ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kResourceBusy);
    }
    if (false == pKvSystem->IsKeyExist(fileName)) {
        if (false == _CheckFileCount(1)) {
            ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
            return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kTooManyFiles);
        }
        ara::core::Result< bool > resultAdd{_AddNewFileToKv(fileName, FileCreationState::kCreatedByApplication,
                                                            FileModificationState::kModifiedByApplication)};
        if (false == resultAdd.HasValue()) {
            ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
            return isoftkv::T_ErrorResult< bool, UniqueHandle< ReadWriteAccessor > >(resultAdd);
        }
        std::ignore = pKvSystem->SyncWalogToMain(true);
    }
    // Perform redundancy check for read-write files
    if (pFileConfig_->IsEnableRedd()) {
        if (pFileConfig_->IsReddCheckType()) {
            if (false == _CheckReddData(fileName)) {
                ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
                return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kValidationFailed);
            }
        } else {
            // Attempt an automatic recovery
            std::ignore = _RecoverFile(fileName);
        }
    }
    ara::core::String const stFileName{_GetFileName(fileName)};
    // Handle mode behavior: file clearing behavior
    if (nullptr != pMode) {
        if (G_IsHaveOpenMode(*pMode, OpenMode::kTruncate)) {
            isoftkv::PFileOpt fileOpt{isoftkv::PFileOpt::ReadWrite()};
            if (fileOpt.DoPrepareWork(isoftkv::T_StringView(stFileName))) {
                std::ignore = fileOpt.ResizeFile(0);
            }
        }
    }
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    std::unique_ptr< ReadWriteAccessor > pFileAccessor{std::make_unique< ReadWriteAccessor >(reddDataMofN)};
    // Set encryption configuration
    isoftkv::PConfigData_Crypto const *const pFindCrypto{
        pFileConfig_->IsHaveCrypto_Element(isoftkv::T_String(fileName))};
    if (nullptr != pFindCrypto) {
        pFileAccessor->AttachCryptoConfig(*pFindCrypto);
    } else {
        if (pFileConfig_->IsHaveCrypto_Storage(isoftkv::ECryptoKeySlotUsage::kHaveCrypto)) {
            pFileAccessor->AttachCryptoConfig(pFileConfig_->GetCryptoConfig());
        }
    }
    // Handle Span.buffer
    if (nullptr != pBuffer) {
        pFileAccessor->AttachSpanBuff(pBuffer);
    }
    // Create file operation class object
    uint32_t nFlags{isoftkv::PFileOpt::ReadWrite()};
    if (bWriteOnly) {
        nFlags = isoftkv::PFileOpt::WriteOnly();
    }
    if (nFlags > 0U) {
        // This integral or enum output parameter value is not subsequently checked.
    }
    ara::core::Result< bool > resultInit{pFileAccessor->InitFileOpt(isoftkv::T_StringView(stFileName), nFlags)};
    if (false == resultInit.HasValue()) {
        ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
        return isoftkv::T_ErrorResult< bool, UniqueHandle< ReadWriteAccessor > >(resultInit);
    }
    if (false == pFileAccessor->IsExist()) {
        ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(fileName);
        return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromError(PerErrc::kFileNotFound);
    }
    // Handle mode behavior
    if (nullptr != pMode) {
        if (G_IsHaveOpenMode(*pMode, OpenMode::kAtTheBeginning)) {
            std::ignore = pFileAccessor->MovePosition(Origin::kBeginning, 0);
        }
        if ((G_IsHaveOpenMode(*pMode, OpenMode::kAtTheEnd)) || (G_IsHaveOpenMode(*pMode, OpenMode::kAppend))) {
            std::ignore = pFileAccessor->MovePosition(Origin::kEnd, 0);
        }
    }
    pFileAccessor->AttachCallStorage(std::make_unique< isoftkv::PCallStorage >(*this, *pFileConfig_));
    pFileAccessor->InitOwnStorage(pFileConfig_->GetStorageIns(), fileName);
    return ara::core::Result< UniqueHandle< ReadWriteAccessor > >::FromValue(
        UniqueHandle< ReadWriteAccessor >{std::move(pFileAccessor)});
}
/// @brief Delete a file
/// @param fileName File name
/// @param bRemoveFromKv Whether to delete the corresponding file record from the KV library
/// @return
ara::core::Result< void > FileStorage::_RemoveFile(ara::core::StringView const &fileName, bool bRemoveFromKv) noexcept
{
    if (nullptr == pKvSystem_) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    PER_Assert(nullptr != pKvSystem_);
    /// First delete any existing
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [](ara::core::StringView const &stReddFile) noexcept -> bool {
        return isoftkv::PFileOpt::DelFile(stReddFile);
    };
    int32_t const nFileCount{_ScanAllReddFile(fileName, pfunWork)};
    if (nFileCount <= 0) {
        return ara::core::Result< void >::FromError(PerErrc::kInvalidSize);
    }
    if (bRemoveFromKv) {
        if (false == pKvSystem_->RemoveKey(fileName)) {
            if (isoftkv::EErrorPHKV::kKvElementNotFind != pKvSystem_->GetLastError()) {
                return ara::core::Result< void >::FromError(isoftkv::TransErrorCode(pKvSystem_->GetLastError()));
            }
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Reset file to its initial state as in the Manifest configuration
/// @param fileName
/// @return
ara::core::Result< void > FileStorage::_ResetFile(ara::core::StringView const &fileName) noexcept
{
    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return ara::core::Result< void >::FromError(PerErrc::kInitValueNotAvailable);
    }
    manifest::MConfigData_InitFile const *pFindInitData{
        pManifestPer->FindInitData_File(pFileConfig_->GetStorageIns(), fileName)};
    if (nullptr == pFindInitData) {
        isoftkv::LogError() << pFileConfig_->GetStorageName().c_str()
                            << "[FileStorage].ResetFile Not Find StorageConfig, StorageIns = "
                            << pFileConfig_->GetStorageIns().c_str();
        return ara::core::Result< void >::FromError(PerErrc::kInitValueNotAvailable);
    }
    if (pFindInitData->stUpdateType == manifest::kUpdateDataDelete) {
        return ara::core::Result< void >::FromError(PerErrc::kInitValueNotAvailable);
    }
    // Handle M/N redundancy when creating a new file
    ara::core::StringView const stFileUri{std::move(isoftkv::T_StringView(pFindInitData->stFileUri))};
    ara::core::StringView const stFileLocal{std::move(isoftkv::T_StringView(pFindInitData->stFileName))};
    isoftkv::CB_DealStringView pfunWork;
    pfunWork = [stFileUri](ara::core::StringView const &stNewFileName) noexcept -> bool {
        return isoftkv::PFileOpt::CopyFile(stFileUri, stNewFileName);
    };
    std::ignore = _ScanAllReddFile(stFileLocal, pfunWork);
    std::ignore = pKvSystem_->RemoveKey(fileName);
    ara::core::Result< bool > resultAdd{_AddNewFileToKv(stFileLocal, FileCreationState::kCreatedDuringReset,
                                                        FileModificationState::kModifiedDuringReset)};
    if (false == resultAdd.HasValue()) {
        return isoftkv::T_ErrorResult< bool, void >(resultAdd);
    }
    // Function does not call SyncWalogToMain(true) internally
    return ara::core::Result< void >::FromValue();
}
/// @brief Execute logic to recover a single file
/// @param[in] fileName Name of the file to recover
/// @param[out] pVecReport Redundant indices of the recovered file
/// @return Whether execution was successful
bool FileStorage::_RecoverFile(ara::core::StringView const &fileName, ara::core::Vector< uint8_t > *pVecReport) noexcept
{
    if (nullptr != pVecReport) {
        pVecReport->clear();
    }
    isoftkv::PReddDataMofN const reddDataMofN{pFileConfig_->FindFileReddMofN(fileName)};
    if (reddDataMofN.GetReddCountN() <= 1U) {
        return false;
    }
    // First use M/N to select the correct file and the corrupted file
    ara::core::String const stFileName{_GetFileName(fileName)};
    isoftkv::PSelectMofN const selectMofN{reddDataMofN.m, reddDataMofN.n};
    isoftkv::CB_DealInt32 pfunInt32;
    ara::core::Vector< uint64_t > vecHashID;
    pfunInt32 = [&stFileName, &vecHashID](int32_t const nIndex) noexcept -> uint64_t {
        ara::core::String const stNewFileName{isoftkv::MakeReddFileName(stFileName, nIndex)};
        uint32_t const nHashID{isoftkv::PSelectMofN::CalFileHashID(stNewFileName)};
        vecHashID.push_back(nHashID);
        return static_cast< uint64_t >(nHashID);
    };
    // If the correct redundant file cannot be selected, reset directly to the initial state
    int32_t const nMainIndex{selectMofN.SelectGoal(static_cast< uint64_t >(0), pfunInt32)};
    if ((nMainIndex < 0) || (nMainIndex >= static_cast< int32_t >(reddDataMofN.n))) {
        return false;
    }
    // Then overwrite the corrupted file with the correct file
    ara::core::String const stFileNameMain{isoftkv::MakeReddFileName(stFileName, nMainIndex)};
    for (int32_t i = 0; i < static_cast< int32_t >(vecHashID.size()); i++) {
        if ((nMainIndex != i) && (vecHashID[nMainIndex] != vecHashID[i])) {
            if (isoftkv::PFileOpt::CopyFile(isoftkv::T_StringView(stFileNameMain),
                                            isoftkv::T_StringView(isoftkv::MakeReddFileName(stFileName, i)))) {
                if (nullptr != pVecReport) {
                    pVecReport->push_back(i);
                }
            }
        }
    }
    if (pVecReport->empty()) {
        return true;
    }
    // Update file status
    ara::core::Result< bool > resultAdd{_AddNewFileToKv(fileName, FileCreationState::kCreatedDuringRecovery,
                                                        FileModificationState::kModifiedDuringRecovery)};
    if (false == resultAdd.HasValue()) {
        return false;
    }
    if (true == resultAdd.Value()) {
        std::ignore = pKvSystem_->SyncWalogToMain(true);
    }
    return true;
}
/// @brief Check if redundancy check information is consistent
/// @param stFileName
/// @return If no redundancy check configuration exists, returns true directly
bool FileStorage::_CheckReddData(ara::core::StringView const &fileName) const noexcept
{
    // 2023-08-02 Only read-only files undergo redundancy check
    if (false == pFileConfig_->IsReddCheckType()) {
        return true;
    }
    ara::core::String const stFileName{_GetFileName(fileName)};
    ara::core::Vector< uint8_t > vecReddData;
    std::ignore = G_CalculateReddData(*(pFileConfig_.get()), isoftkv::T_StringView(stFileName), vecReddData);
    // Read saved ReddData from KV library for verification
    ara::core::Vector< uint8_t > vecReddDataFromKv;
    if (false == G_GetFileInfoInKv(pKvSystem_.get(), fileName, nullptr, nullptr, &vecReddDataFromKv)) {
        if (pKvSystem_->IsHaveError()) {
            // return isoftkv::T_ErrorResult<UniqueHandle<ReadAccessor> >(pKvSystem_->GetLastError()));
            return false;
        }
    }
    // 2023-08-02 Compare whether the two ReddData are consistent
    return vecReddData == vecReddDataFromKv;
}
/// @brief Save file extension information and redundancy check information
/// @param stFileName File name used as the Key in Kv-Storage
/// @param fileInfo Basic file information
/// @param fileInfoEx Extended file information
/// @param vecReddData Redundancy data
/// @return Whether write was successful
bool FileStorage::_WriteCheckReddData(ara::core::StringView const &stFileName,
                                      isoftkv::PFileInfoInKvs const &fileInfo,
                                      isoftkv::PFileInfoInKvs_ReddExt const &fileInfoEx,
                                      ara::core::Vector< uint8_t > const &vecReddData) const noexcept
{
    // 2023-08-02 Use fixed 512-byte stack memory as cache
    uint32_t nTotalNeedLen = 0;
    nTotalNeedLen += static_cast< uint32_t >(sizeof(isoftkv::PFileInfoInKvs));
    nTotalNeedLen += static_cast< uint32_t >(sizeof(isoftkv::PFileInfoInKvs_ReddExt));
    nTotalNeedLen += static_cast< uint32_t >(vecReddData.size());
    if (nTotalNeedLen > kInt_512U) {
        G_LogInfo(GetKvSystem()) << "Find ReddData.Len > 512U"
                                 << ": _WriteCheckReddData.stFileName =" << stFileName.data();
        return false;
    }
    PER_Assert(nTotalNeedLen <= kInt_512U);
    uint8_t buffTemp[kInt_512U]{0U};
    uint32_t nIndex = 0;
    // Transcode PFileInfoInKvs structure
    memcpy(isoftkv::T_TransBytes(buffTemp + nIndex), isoftkv::T_TransBytes(&fileInfo), sizeof(fileInfo));
    nIndex += sizeof(fileInfo);
    // Set PFileInfoInKvs_ReddExt
    memcpy(isoftkv::T_TransBytes(buffTemp + nIndex), isoftkv::T_TransBytes(&fileInfoEx), sizeof(fileInfoEx));
    nIndex += sizeof(fileInfoEx);
    // Record ReddData
    memcpy(isoftkv::T_TransBytes(buffTemp + nIndex), vecReddData.data(), fileInfoEx.nResultLen);
    nIndex += fileInfoEx.nResultLen;
    return pKvSystem_->WriteValue(stFileName, isoftkv::T_TransBytes(buffTemp), nIndex);
}
/// @brief Check if the number of files exceeds the limit
/// @param nNewCount
/// @return
bool FileStorage::_CheckFileCount(int32_t nNewCount) const noexcept
{
    if (false == pKvSystem_.operator bool()) {
        return false;
    }
    ara::core::Vector< ara::core::String > vecKeyList;
    pKvSystem_->EnumAllKey(vecKeyList);
    uint32_t nCountNow{static_cast< uint32_t >(vecKeyList.size())};
    nCountNow -= 1U;  //
    nCountNow = isoftkv::T_AddInt< uint32_t >(nCountNow, nNewCount);
    if (nCountNow > pFileConfig_->GetMaxFileCount()) {
        return false;
    }
    return true;
}
/// @brief Copy a file into FileStorage: the target file may need encryption
/// @param fileName
/// @param stSrcFile
/// @param stDstFile
/// @return
bool FileStorage::_CopyStorageFile(ara::core::StringView const &fileName,
                                   ara::core::StringView const &stSrcFile,
                                   ara::core::StringView const &stDstFile)
{
    isoftkv::PConfigData_Crypto const *pFindCrypto{pFileConfig_->IsHaveCrypto_Element(isoftkv::T_String(fileName))};
    if (nullptr == pFindCrypto) {
        if (pFileConfig_->IsHaveCrypto_Storage(isoftkv::ECryptoKeySlotUsage::kHaveCrypto)) {
            pFindCrypto = &(pFileConfig_->GetCryptoConfig());
        }
    }
    if (nullptr == pFindCrypto) {
        return isoftkv::PFileOpt::CopyFile(stSrcFile, stDstFile);
    }
    if (pFindCrypto->stKeySlotName.empty()) {
        G_LogInfo(GetKvSystem()) << "Find Empty KeySlot When Init File [" << fileName.data() << "]";
        return isoftkv::PFileOpt::CopyFile(stSrcFile, stDstFile);
    }
    isoftkv::PFileOpt fileOptSrc{isoftkv::PFileOpt::ReadOnly()};
    if (false == fileOptSrc.DoPrepareWork(stSrcFile)) {
        return false;
    }
    isoftkv::PFileOpt_Crypto fileOptDst{isoftkv::PFileOpt::WriteOnly()};
    if (false == fileOptDst.DoPrepareWork(stDstFile)) {
        return false;
    }
    if (false == pFindCrypto->stKeySlotName.empty()) {
        if (false == fileOptDst.BuildCrypto(pFindCrypto->stKeySlotName, pFindCrypto->stCryptoAlgorithm)) {
            return false;
        }
    }
    int64_t nFileSize = fileOptSrc.GetFileSize();
    while (nFileSize > 0) {
        constexpr int32_t kBuffLen = 1024;
        uint8_t pBBuff[kBuffLen]   = {0};
        int32_t nReadLen           = fileOptSrc.ReadData(pBBuff, kBuffLen);
        if (nReadLen <= 0) {
            return false;
        }
        int32_t nWriteLen
            = fileOptDst.WriteData(static_cast< const void * >(pBBuff), static_cast< uint32_t >(nReadLen));
        if (nWriteLen != nReadLen) {
            return false;
        }
        nFileSize -= nReadLen;
    }
    return true;
}
//********************************/
/// @brief Open/Create a file data center
/// @param fs
/// @return
ara::core::Result< SharedHandle< FileStorage > > OpenFileStorage(ara::core::InstanceSpecifier const &fs) noexcept
{
    /// @brief The function logic consists of the following SWS_PER: SWS_PER_00396, SWS_PER_00387, SWS_PER_00446, SWS_PER_00382, SWS_PER_00383
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< SharedHandle< FileStorage > >::FromError(PerErrc::kNotInitialized);
    }
    // 2022-11-22 Pre-determine here whether the configured minimum space requirement is met
    ara::core::Result< std::shared_ptr< FileStorage > > const resultOpen{
        isoftkv::POpenStorageLogic< FileStorage >::DoOpenLogic(fs)};
    if (false == resultOpen.HasValue()) {
        return isoftkv::T_ErrorResult< std::shared_ptr< FileStorage >, SharedHandle< FileStorage > >(resultOpen);
    }
    SharedHandle< FileStorage > const pSharedHandle{resultOpen.Value()};
    return ara::core::Result< SharedHandle< FileStorage > >::FromValue(pSharedHandle);
}
/// @brief Restore all files
/// @param fs
/// @return
ara::core::Result< void > RecoverAllFiles(ara::core::InstanceSpecifier const &fs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< FileStorage >::IsStorageBusy(fs)) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    std::shared_ptr< FileStorage > pFindStorage{isoftkv::POpenStorageLogic< FileStorage >::NewStorage(fs)};
    if (nullptr != pFindStorage) {
        std::ignore = pFindStorage->RecoverAllFile();
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Reset all files
/// @param fs Port identifier
/// @return
ara::core::Result< void > ResetAllFiles(ara::core::InstanceSpecifier const &fs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< FileStorage >::IsStorageBusy(fs)) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    // if (false == isoftkv::POpenStorageLogic<FileStorage>::IsStorageExist(fs)) {
    //     return ara::core::Result<void>::FromValue());
    // }
    std::shared_ptr< FileStorage > pFindStorage{isoftkv::POpenStorageLogic< FileStorage >::NewStorage(fs)};
    if (nullptr != pFindStorage) {
        std::ignore = pFindStorage->ResetStorage(fs);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the storage space occupied by the current file data center in bytes (including redundant and backup files)
/// @param fs
/// @return
ara::core::Result< uint64_t > GetCurrentFileStorageSize(ara::core::InstanceSpecifier const &fs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< FileStorage >::IsStorageBusy(fs)) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kResourceBusy);
    }
    if (false == isoftkv::POpenStorageLogic< FileStorage >::IsStorageExist(fs)) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kStorageNotFound);
    }

    ara::core::Result< SharedHandle< FileStorage > > const resultOpen{OpenFileStorage(fs)};
    if (false == resultOpen.HasValue()) {
        return isoftkv::T_ErrorResult< SharedHandle< FileStorage >, uint64_t >(resultOpen);
    }
    return resultOpen.Value()->GetStorageSpace();
}
/// @brief The "=|" operation for file operators
/// @param left
/// @param right
/// @return
OpenMode &operator|=(OpenMode &left, OpenMode const &right) noexcept
{
    uint32_t const nResult{static_cast< uint32_t >(left) | static_cast< uint32_t >(right)};
    left = static_cast< OpenMode >(nResult);
    return left;
}
//********************************/
/// @brief Delete a File data center: 2023-08-11 internal interface
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
ara::core::Result< void > RemoveFileStorage(ara::core::InstanceSpecifier const &fs) noexcept
{
    return isoftkv::POpenStorageLogic< FileStorage >::RemoveStorage(fs);
}
//********************************/
}  // namespace per
}  // namespace ara
