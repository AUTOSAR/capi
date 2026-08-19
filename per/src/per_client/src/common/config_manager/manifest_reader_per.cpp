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
/// @file       manifest_reader_per.cpp
/// @brief      AutoSar-Crypto Configuration
/// @details    Configuration file reader used by the Puhua persistence module.
/// @date       2022-12-13
/// @author     Han Jingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Configuration Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Configuration file reader used by the Puhua persistence module.
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2022-12-13 | 1.0.0   | Han Jingjing| Refactored the configuration file reading functionality of the persistence module |
///
/// ================================================================

#include "ara/per/internal/manifest/manifest_reader_per.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_data_type.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/initialize.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/manifest/manifest_syntax.h"

//********************************/
namespace manifest = ara::per::manifest;
/// @brief CRC redundancy
MR_HEADER(manifest::MConfigData_ReddCrc)
MR_FIELD(manifest::kRedundancyTag_AlgorithmFamily, algorithmFamily)
MR_FIELD(manifest::kRedundancyTag_AlgorithmLength, length)
MR_FOOTER
/// @brief M/N redundancy
MR_HEADER(manifest::MConfigData_ReddMofN)
MR_FIELD(manifest::kRedundancyTag_M, m)
MR_FIELD(manifest::kRedundancyTag_N, n)
MR_FOOTER
/// @brief CRC redundancy
MR_HEADER(manifest::MConfigData_ReddHash)
MR_FIELD(manifest::kRedundancyTag_AlgorithmFamily, algorithmFamily)
MR_FIELD(manifest::kRedundancyTag_AlgorithmLength, length)
MR_OPTIONAL_FIELD(manifest::kRedundancyTag_HashInitVecLen, nInitVectorLen)
MR_FOOTER
/// @brief Redundancy configuration
MR_HEADER(manifest::MConfigData_Redundancy)
MR_OPTIONAL_FIELD(manifest::kRedundancyTag_MofN, vecMofN)
MR_OPTIONAL_FIELD(manifest::kRedundancyTag_Crc, vecCrc)
MR_OPTIONAL_FIELD(manifest::kRedundancyTag_Hash, vecHash)
MR_FOOTER
//********************************/
/// @brief Persistence shared data
MR_HEADER(manifest::MConfigData_StorageBase)
MR_FIELD(manifest::kStorageName, stName)
MR_FIELD(manifest::kVersionPer, stVersionPer)
MR_STRING_OR_VECTOR_FIELD(manifest::kStoragePath, stPath)
MR_FIELD(manifest::kStorageMinSize, nSizeMin)
MR_FIELD(manifest::kStorageMaxSize, nSizeMax)
MR_FIELD(manifest::kStorageUpdateStrategy, stUpdateStrategy)
MR_OPTIONAL_FIELD(manifest::kStorageReddStrategy, stReddStrategy)
MR_OPTIONAL_FIELD(manifest::kStorageRedundancy, reddConfig)
MR_FOOTER
//********************************/
/// @brief KvStorage KV library configuration
MR_HEADER(manifest::MConfigData_StorageKv)
MR_OPTIONAL_FIELD(manifest::kKvStorageInitKvList, stInitFile)
MR_OPTIONAL_FIELD(manifest::kMaxCacheCount, nMaxCacheCount)        // Initial number of Cache pages
MR_OPTIONAL_FIELD(manifest::kInitPageCount, nInitPageCount)        // Initial number of Pages
MR_OPTIONAL_FIELD(manifest::kPageInitLen, nPageInitLen)            // Length of a single page
MR_OPTIONAL_FIELD(manifest::kFreeListCapacity, nFreeListCapacity)  // Number of free list bins
MR_OPTIONAL_FIELD(manifest::kMaxPageInGroup,
                  nMaxPageInGroup)                           // Group size, upper limit on the number of pages per group
MR_OPTIONAL_FIELD(manifest::kHashInitCount, nHashInitCount)  // Initial number of hash buckets
MR_OPTIONAL_FIELD(manifest::kHashMaxCapacity, nHashMaxCapacity)  // Maximum number of hash buckets
MR_OPTIONAL_FIELD(manifest::kAutoSaveWalog, nAutoSaveWalog)      // Whether to automatically save the Walog library
MR_OPTIONAL_FIELD(manifest::kMaxKvDataLen,
                  nMaxKvDataLen)  // Maximum allowed length of KV data: default 64 bytes, max up to nPage
MR_FOOTER
/// @brief KvStorage KV library configuration (including "specific parameters" for initializing different KV libraries: initial page size, etc.)
MR_HEADER(manifest::MConfig_StorageKv)
// Fields from the base class must also be listed one by one
MR_FIELD(manifest::kStorageIns, stStorageIns)
MR_FIELD(manifest::kStorageProperty, dataStorage)
MR_FIELD(manifest::kKvStorage, dataKv)
MR_FOOTER
//***************/
/// @brief FileStorage initial file information
MR_HEADER(manifest::MConfigData_InitFile)
MR_OPTIONAL_FIELD(manifest::kInitFileUpdateType, stUpdateType)  // Update strategy may not exist
MR_FIELD(manifest::kInitFileUri, stFileUri)
MR_FIELD(manifest::kInitFileName, stFileName)
MR_OPTIONAL_FIELD(manifest::kStorageRedundancy, reddConfig)
MR_FOOTER
/// @brief FileStorage File library configuration
MR_HEADER(manifest::MConfigData_StorageFile)
MR_FIELD(manifest::kFileStorageMaxFiles, nMaxFileCount)
MR_OPTIONAL_FIELD(manifest::kFileStorageFileList, vecInitFile)
MR_FOOTER
/// @brief FileStorage File library configuration
MR_HEADER(manifest::MConfig_StorageFile)
// Fields from the base class must also be listed one by one
MR_FIELD(manifest::kStorageIns, stStorageIns)
MR_FIELD(manifest::kStorageProperty, dataStorage)
MR_FIELD(manifest::kFileStorage, dataFile)
MR_FOOTER
//********************************/
/// @brief Port information
MR_HEADER(manifest::MConfigData_PerPort)
MR_FIELD(manifest::kPortIns, stPotIns)
MR_FIELD(manifest::kPortType, stPortType)
MR_FIELD(manifest::kStorageIns, stStorageIns)
MR_FOOTER
/// @brief Encryption key slot configuration for persistence: library object itself
MR_HEADER(manifest::MConfigData_KeySlotStorage)
MR_FIELD(manifest::kKeySlotIns, stKeySlotIns)
MR_OPTIONAL_FIELD(manifest::kCryptoAlgorithm, stCryptoAlgorithm)
MR_FIELD(manifest::kKeySlotUsage, stKeySlotUsage)
MR_FIELD(manifest::kVerificationHash, stVerificationHash)
MR_FIELD(manifest::kPerDeployment, stStorageName)
MR_FOOTER
/// @brief Encryption key slot configuration for persistence: object members within the library
MR_HEADER(manifest::MConfigData_KeySlotElement)
MR_FIELD(manifest::kKeySlotIns, stKeySlotIns)
MR_OPTIONAL_FIELD(manifest::kCryptoAlgorithm, stCryptoAlgorithm)
MR_FIELD(manifest::kKeySlotUsage, stKeySlotUsage)
MR_FIELD(manifest::kVerificationHash, stVerificationHash)
MR_FIELD(manifest::kPerDeployment, stStorageName)
MR_FIELD(manifest::kPerDeploymentElement, stElementName)
MR_FOOTER
//********************************/
namespace {
/// @brief //Conversion path: replace '$' symbol with the current path
/// @param stUriName Original file name
/// @param stVarPath Ara-Var path
/// @return
ara::core::String G_TransPath(ara::core::String const& stUriName, ara::core::String const& stVarPath) noexcept
{
    ara::core::String stReturn = stVarPath;
    if (false == stUriName.empty()) {
        ara::per::char8_t const chFlag{stUriName.front()};
        if (chFlag == ara::per::kChar_Dollar) {
            stReturn = ara::per::isoftkv::PFileOpt::GetWorkPath();
            stReturn += stUriName.substr(1U);
        } else if (chFlag == nai_path_sep) {
            // Only paths starting with "/" are absolute paths, no modification
            stReturn = stUriName;
        } else {
            stReturn = stVarPath;
            if (stReturn.back() != nai_path_sep) {
                stReturn += nai_path_sep;
            }
            stReturn += stUriName;
        }
    }
    return stReturn;
}
}  // namespace
namespace ara {
namespace per {
namespace manifest {
//********************************/
/// @brief Constructor
/// @param stVarPath
PManifestReader_Per::PManifestReader_Per(ara::core::StringView const& stVarPath) noexcept : stAraVarPath_{stVarPath} {}
/// @brief Initialize configuration file
/// @param stConfigFile
/// @return
bool PManifestReader_Per::InitManifest(ara::core::StringView const& stConfigFile) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const manifestRes{
        isoft::manifestreader::OpenManifest(stConfigFile)};
    ara::core::String::size_type nFindPos = stConfigFile.find_last_of("\\/");
    if (nFindPos == ara::core::String::npos) {
        stSelfEtcPath_ = "";
    } else {
        stSelfEtcPath_ = stConfigFile.substr(0, nFindPos);
    }

    if (false == manifestRes.operator bool()) {
        ara::per::isoftkv::LogError() << "Failed to load manifest database due to:" << manifestRes.Error();
        return false;
    }

    isoft::manifestreader::Manifest const* const pManifest{manifestRes.Value().get()};
    std::ignore = pManifest->Load(kVersionApp, stVersionApp_);
    std::ignore = _ReadPortMap_Kv(pManifest);
    std::ignore = _ReadPortMap_File(pManifest);
    std::ignore = _ReadStorageData_Kv(pManifest);
    std::ignore = _ReadStorageData_File(pManifest);
    std::ignore = _ReadKeySlot_Storage(pManifest);
    std::ignore = _ReadKeySlot_Element(pManifest);
    return true;
}
/// @brief For each KvStorage configuration, execute the callback function
/// @param pfun Callback function
/// @return Number of callbacks executed
uint32_t PManifestReader_Per::ForEachStorage_Kv(PManifestReader_Per::CB_ScanStorage_Kv const& pfun) noexcept
{
    if (nullptr == pfun) {
        return 0U;
    }
    uint32_t nScanCount{0U};
    for (auto& config : mapStorageKv_) {
        if (false == pfun(config.second)) {
            break;
        }
        nScanCount += 1U;
    }
    return nScanCount;
}
/// @brief For each FileStorage configuration, execute the callback function
/// @param pfun Callback function
/// @return Number of callbacks executed
uint32_t PManifestReader_Per::ForEachStorage_File(PManifestReader_Per::CB_ScanStorage_File const& pfun) noexcept
{
    if (nullptr == pfun) {
        return 0U;
    }
    uint32_t nScanCount{0U};
    for (auto& config : mapStorageFile_) {
        if (false == pfun(config.second)) {
            break;
        }
        nScanCount += 1U;
    }
    return nScanCount;
}
/// @brief Traverse each initialization file in a specific FileStorage and execute the callback function
/// @param port PortIns of the FileStorage
/// @param pfun Callback function
/// @return Number of callbacks executed
int32_t PManifestReader_Per::ForEachFileStorageInitFile(ara::core::String const& stPortIns,
                                                        CB_ScanInitFile const& pfun) const noexcept
{
    MConfig_StorageFile const* const pFindFileStorage{FindStorageByPort_File(stPortIns)};
    if (nullptr == pFindFileStorage) {
        ara::per::isoftkv::LogInfo() << "Failed to find manifest database :" << stPortIns.c_str();
        return 0;
    }
    if (pFindFileStorage->dataFile.vecInitFile.empty()) {
        return 0;
    }
    int32_t nReturn{0};
    nReturn = 0;
    for (MConfigData_InitFile const& fileData : pFindFileStorage->dataFile.vecInitFile) {
        if (pfun(fileData)) {
            nReturn += 1;
        }
    }
    return nReturn;
}
/// @brief Get the name of the KvStorage corresponding to the port
/// @param port
/// @return
ara::core::String PManifestReader_Per::GetStorageName_Kv(ara::core::String const& stPortIns) const noexcept
{
    MConfig_StorageKv const* const pFindPort{FindStorageByPort_Kv(stPortIns)};
    if (nullptr == pFindPort) {
        return ara::core::String{""};
    }
    return pFindPort->dataStorage.stName;
}
/// @brief Get the name of the FileStorage corresponding to the port
/// @param port
/// @return
ara::core::String PManifestReader_Per::GetStorageName_File(ara::core::String const& stPortIns) const noexcept
{
    MConfig_StorageFile const* const pFindPort{FindStorageByPort_File(stPortIns)};
    if (nullptr == pFindPort) {
        return ara::core::String{""};
    }
    return pFindPort->dataStorage.stName;
}
/// @brief Get the initialization Uri of the KvStorage
/// @param port
/// @return
ara::core::String PManifestReader_Per::GetKvStorageUpdateFile(ara::core::String const& stPortIns) const noexcept
{
    MConfig_StorageKv const* const pFindPort{FindStorageByPort_Kv(stPortIns)};
    if (nullptr == pFindPort) {
        return ara::core::String{""};
    }
    return pFindPort->dataKv.stInitFile;
}  // namespace manifest
/// @brief Find KvStorage by PortIns
/// @param portIns
/// @return Returns nullptr if not found
MConfig_StorageKv const* PManifestReader_Per::FindStorageByPort_Kv(ara::core::String const& stPortIns) const noexcept
{
    MAP_PortToStorage::const_iterator const itFindPort{mapPortToStorageKv_.find(stPortIns)};
    if (itFindPort == mapPortToStorageKv_.end()) {
        return nullptr;
    }
    return FindStorageByPer_Kv(itFindPort->second.stStorageIns);
}
/// @brief Find FileStorage by PortIns
/// @param portIns
/// @return Returns nullptr if not found
MConfig_StorageFile const* PManifestReader_Per::FindStorageByPort_File(
    ara::core::String const& stPortIns) const noexcept
{
    MAP_PortToStorage::const_iterator const itFindPort{mapPortToStorageFile_.find(stPortIns)};
    if (itFindPort == mapPortToStorageFile_.end()) {
        return nullptr;
    }
    return FindStorageByPer_File(itFindPort->second.stStorageIns);
}
/// @brief Find KvStorage by PerIns
/// @param stPerIns
/// @return Returns nullptr if not found
MConfig_StorageKv const* PManifestReader_Per::FindStorageByPer_Kv(ara::core::String const& stPerIns) const noexcept
{
    MAP_StorageKv::const_iterator const itFind{mapStorageKv_.find(stPerIns)};
    if (itFind == mapStorageKv_.end()) {
        return nullptr;
    }
    return &(itFind->second);
}  // namespace manifest
/// @brief Find FileStorage by PerIns
/// @param stPerIns
/// @return Returns nullptr if not found
MConfig_StorageFile const* PManifestReader_Per::FindStorageByPer_File(ara::core::String const& stPerIns) const noexcept
{
    MAP_StorageFile::const_iterator const itFind{mapStorageFile_.find(stPerIns)};
    if (itFind == mapStorageFile_.end()) {
        return nullptr;
    }
    return &(itFind->second);
}
/// @brief Find whether there is a library encryption configuration by the persistence library name
/// @param stStorageName
/// @return
MConfigData_KeySlotStorage const* PManifestReader_Per::FindKeySlot_Storage(
    ara::core::String const& stStorageName) const noexcept
{
#ifndef ARA_WITH_CRYPTO
    return nullptr;
#endif
    MAP_KeySlotStorage::const_iterator itFind = mapKeySlotStorage_.find(stStorageName);
    if (itFind == mapKeySlotStorage_.end()) {
        return nullptr;
    }
    return &(itFind->second);
}
/// @brief Find whether there is an entry encryption configuration for the library by the persistence library name
/// @param stElementName
/// @return
MConfigData_KeySlotElement const* PManifestReader_Per::FindKeySlot_Element(
    ara::core::String const& stElementName) const noexcept
{
    for (auto const& it : mapKeySlotElement_) {
        if (it.second.stElementName == stElementName) {
            return &(it.second);
        }
    }
    return nullptr;
}
/// @brief Traverse each KeySlotElement configuration and execute the callback function
/// @param stStorage
/// @param pfun Callback function
/// @return Number of callbacks executed
uint32_t PManifestReader_Per::ForEachKeySlotElement(ara::core::String const& stStorage,
                                                    PManifestReader_Per::CB_ScanElement const& pfun) const noexcept
{
    if (nullptr == pfun) {
        return 0U;
    }
    uint32_t nReturn{0U};
    for (auto const& it : mapKeySlotElement_) {
        MConfigData_KeySlotElement const& keySlotElement = it.second;
        if (keySlotElement.stStorageName == stStorage) {
            if (pfun(keySlotElement)) {
                nReturn += 1U;
            }
        }
    }
    return nReturn;
}
/// @brief Find the file initialization information identified by stFileName in the FileStorage corresponding to stPerIns
/// @param stPerIns
/// @param stFileName
/// @return
MConfigData_InitFile const* PManifestReader_Per::FindInitData_File(
    ara::core::String const& stPerIns, ara::core::StringView const& stFileName) const noexcept
{
    MConfig_StorageFile const* pFindStorage = FindStorageByPer_File(stPerIns);
    if (nullptr == pFindStorage) {
        return nullptr;
    }

    for (MConfigData_InitFile const& fileData : pFindStorage->dataFile.vecInitFile) {
        if (fileData.stFileName == stFileName) {
            return &fileData;
        }
    }
    return nullptr;
}
/// @brief Find the initialization file name in the KeyValueStorage corresponding to stPerIns
/// @param stPerIns
/// @return
ara::core::String PManifestReader_Per::FindInitData_Kv(ara::core::String const& stPerIns) const noexcept
{
    MConfig_StorageKv const* pFindStorage = FindStorageByPer_Kv(stPerIns);
    if (nullptr == pFindStorage) {
        return ara::core::String{""};
    }
    return pFindStorage->dataKv.stInitFile;
}
/// @brief Delete KvStorage
/// @param stStorageName Library name
/// @return Success or failure
bool PManifestReader_Per::DelStorage_Kv(ara::core::String const& stStorageName) noexcept
{
    MAP_StorageKv::iterator itFind{mapStorageKv_.find(stStorageName)};
    if (itFind == mapStorageKv_.end()) {
        return false;
    }
    itFind = mapStorageKv_.erase(itFind);
    return true;
}
/// @brief Delete FileStorage
/// @param stStorageName Library name
/// @return Success or failure
bool PManifestReader_Per::DelStorage_File(ara::core::String const& stStorageName) noexcept
{
    MAP_StorageFile::iterator itFind{mapStorageFile_.find(stStorageName)};
    if (itFind == mapStorageFile_.end()) {
        return false;
    }
    itFind = mapStorageFile_.erase(itFind);
    return true;
}
//********************************/
/// @brief Find PortIns by StorageIns
/// @param mapPortToStorage
/// @param stStorageIns
/// @return
ara::core::String PManifestReader_Per::_FindPortInsByStorageIns(MAP_PortToStorage const& mapPortToStorage,
                                                                ara::core::String const& stStorageIns)
{
    for (auto const& it : mapPortToStorage) {
        if (it.second.stStorageIns == stStorageIns) {
            return it.second.stPotIns;
        }
    }
    return ara::core::String{""};
}
/// @brief Read PortMap configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadPortMap_Kv(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfigData_PerPort > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kMapPortToKvStorage, vecData));
    for (MConfigData_PerPort const& readData : vecData) {
        mapPortToStorageKv_[readData.stPotIns] = readData;
    }
    return static_cast< uint32_t >(mapPortToStorageKv_.size());
}
/// @brief Read FilePortMap configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadPortMap_File(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfigData_PerPort > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kMapPortToFileStorage, vecData));
    for (MConfigData_PerPort const& readData : vecData) {
        mapPortToStorageFile_[readData.stPotIns] = readData;
    }
    return static_cast< uint32_t >(mapPortToStorageFile_.size());
}
/// @brief Read KV library configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadStorageData_Kv(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfig_StorageKv > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kKeyValueStorageSet, vecData));
    for (MConfig_StorageKv const& readData : vecData) {
        // Modify some values
        MConfig_StorageKv saveData;
        saveData                             = readData;
        saveData.stPortIns                   = _FindPortInsByStorageIns(mapPortToStorageKv_, readData.stStorageIns);
        saveData.dataStorage.stPath          = G_TransPath(readData.dataStorage.stPath, stAraVarPath_);
        saveData.dataKv.stInitFile           = G_TransPath(readData.dataKv.stInitFile, stSelfEtcPath_);
        mapStorageKv_[saveData.stStorageIns] = saveData;
    }
    return static_cast< uint32_t >(mapStorageKv_.size());
}
/// @brief Read File library configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadStorageData_File(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfig_StorageFile > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kFileStorageSet, vecData));
    for (MConfig_StorageFile const& readData : vecData) {
        // Modify some values
        MConfig_StorageFile saveData{readData};
        saveData.stPortIns          = _FindPortInsByStorageIns(mapPortToStorageFile_, readData.stStorageIns);
        saveData.dataStorage.stPath = G_TransPath(readData.dataStorage.stPath, stAraVarPath_);
        for (auto& fileData : saveData.dataFile.vecInitFile) {
            fileData.stFileUri = G_TransPath(fileData.stFileUri, stAraVarPath_);
        }
        mapStorageFile_[saveData.stStorageIns] = saveData;
    }
    return static_cast< uint32_t >(mapStorageFile_.size());
}
/// @brief Read KeySlotStorage configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadKeySlot_Storage(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfigData_KeySlotStorage > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kMapKeySlotToStorage, vecData));
    for (MConfigData_KeySlotStorage const& readData : vecData) {
        mapKeySlotStorage_[readData.stStorageName] = readData;
    }
    return static_cast< uint32_t >(mapKeySlotStorage_.size());
}
/// @brief Read KeySlotStorageElement configuration
/// @param pManifest Configuration file reader and interpreter
/// @return
uint32_t PManifestReader_Per::_ReadKeySlot_Element(isoft::manifestreader::Manifest const* const pManifest) noexcept
{
    PER_Assert(nullptr != pManifest);
    ara::core::Vector< MConfigData_KeySlotElement > vecData{};
    PER_Assert(isoft::kSuccess == pManifest->Load(manifest::kMapKeySlotToStorageElement, vecData));
    for (MConfigData_KeySlotElement const& readData : vecData) {
        ara::core::String stElementName   = readData.stStorageName + "/" + readData.stElementName;
        mapKeySlotElement_[stElementName] = readData;
    }
    return static_cast< uint32_t >(mapKeySlotElement_.size());
}

//********************************/
}  // namespace manifest
}  // namespace per
}  // namespace ara
