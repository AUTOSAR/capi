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
/// @file       config_muster.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Configuration reading
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-06-09  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================

#include "ara/per/internal/isoftkv/config_muster.h"

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/manifest/manifest_instance.h"
#include "ara/per/internal/manifest/manifest_syntax.h"
namespace {
//***************/
/// @brief Redundancy configuration
/// @param reddConfig
/// @return
ara::per::isoftkv::PReddConfigData G_AssembleReddConfig(
    ara::per::manifest::MConfigData_Redundancy const &reddConfig) noexcept
{
    ara::per::isoftkv::PReddConfigData reddConfigAssemble;
    // Interpret redundancy settings
    reddConfigAssemble.Reset();
    // Ignored "redundancyHandling.scope" as of 2021-10-22
    if (false == reddConfig.vecCrc.empty()) {
        reddConfigAssemble.SetReddConfig_Crc(reddConfig.vecCrc[0U]);
    }
    if (false == reddConfig.vecMofN.empty()) {
        reddConfigAssemble.SetReddConfig_MofN(reddConfig.vecMofN[0U]);
    }
    if (false == reddConfig.vecHash.empty()) {
        reddConfigAssemble.SetReddConfig_Hash(reddConfig.vecHash[0U]);
    }
    return reddConfigAssemble;
}
//***************/
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Check if redundancy strategy is MofN
/// @return
bool PReddConfigData::IsReddMofN() const noexcept { return eReddType == EReddType::kMofN; }
/// @brief Set CRC redundancy
/// @param reddData Json configuration read from manifest document
void PReddConfigData::SetReddConfig_Crc(ara::per::manifest::MConfigData_ReddCrc const &reddData) noexcept
{
    eReddType                 = EReddType::kCrc;
    reddCrc.stAlgorithmFamily = reddData.algorithmFamily;
    reddCrc.nLength           = static_cast< uint16_t >(reddData.length);
    return;
}
/// @brief Set M/N redundancy
/// @param reddData Json configuration read from manifest document
void PReddConfigData::SetReddConfig_MofN(ara::per::manifest::MConfigData_ReddMofN const &reddData) noexcept
{
    eReddType  = EReddType::kMofN;
    reddMofN.m = static_cast< uint16_t >(reddData.m);
    reddMofN.n = static_cast< uint16_t >(reddData.n);
    return;
}
/// @brief Set Hash redundancy
/// @param reddData Json configuration read from manifest document
void PReddConfigData::SetReddConfig_Hash(ara::per::manifest::MConfigData_ReddHash const &reddData) noexcept
{
    eReddType                  = EReddType::kHash;
    reddHash.stAlgorithmFamily = reddData.algorithmFamily;
    reddHash.nLength           = static_cast< uint16_t >(reddData.length);
    reddHash.nInitVecLen       = static_cast< uint16_t >(reddData.nInitVectorLen);
    return;
}
//********************************/
/// @brief Check if redundancy strategy is CRC or Hash (redundancy types requiring verification)
/// @return
bool PConfigMuster::IsReddCheckType(EReddType eReddType) noexcept
{
    return (EReddType::kCrc == eReddType) || (EReddType::kHash == eReddType);
}
/// @brief
/// @param a
PConfigMuster::PConfigMuster(PConfigMuster &&a) noexcept
    : bConfigValid_{a.bConfigValid_}
    , stStorageIns_{std::move(std::move(a).stStorageIns_)}
    , stStorageName_{std::move(std::move(a).stStorageName_)}
    , stWorkPath_{std::move(a.stWorkPath_)}
    , stVersionApp_{std::move(a.stVersionApp_)}
    , stVersionPer_{std::move(a.stVersionPer_)}
    , nSpaceAmountMin_{a.nSpaceAmountMin_}
    , nSpaceAmountMax_{a.nSpaceAmountMax_}
    , stUpdateStrategy_{std::move(a.stUpdateStrategy_)}
    , eReddStrategy_{std::move(a.eReddStrategy_)}
    , cryptoData_{std::move(a.cryptoData_)}
    , globleReddConfig_{std::move(a.globleReddConfig_)}
    , mapElementCrypto_{std::move(a.mapElementCrypto_)}
{
}
/// @brief
/// @param a
/// @return
PConfigMuster &PConfigMuster::operator=(PConfigMuster &&a) noexcept
{
    bConfigValid_     = a.bConfigValid_;
    stStorageIns_     = std::move(a.stStorageIns_);
    stStorageName_    = std::move(a.stStorageName_);
    stWorkPath_       = std::move(a.stWorkPath_);
    stVersionApp_     = std::move(a.stVersionApp_);
    stVersionPer_     = std::move(a.stVersionPer_);
    nSpaceAmountMin_  = a.nSpaceAmountMin_;
    nSpaceAmountMax_  = a.nSpaceAmountMax_;
    stUpdateStrategy_ = std::move(a.stUpdateStrategy_);
    eReddStrategy_    = std::move(a.eReddStrategy_);
    cryptoData_       = std::move(a.cryptoData_);
    globleReddConfig_ = std::move(a.globleReddConfig_);
    mapElementCrypto_ = std::move(a.mapElementCrypto_);
    return *this;
}
/// @brief Set redundancy configuration
/// @param stReddStrategy
/// @param reddConfig
void PConfigMuster::SetReddConfig(ara::core::StringView const &stReddStrategy,
                                  manifest::MConfigData_Redundancy const &reddConfig) noexcept
{
    // Interpret redundancy strategy
    SetReddStrategy(EReddStrategy::kNone);
    if (manifest::kRedundancyDataNone == stReddStrategy) {
        SetReddStrategy(EReddStrategy::kNone);
    } else if (manifest::kRedundancyDataRedundant == stReddStrategy) {
        SetReddStrategy(EReddStrategy::kStorage);
    } else if (manifest::kRedundancyDataPerElement == stReddStrategy) {
        SetReddStrategy(EReddStrategy::kElement);
    } else {
    }
    // Interpret redundancy settings
    globleReddConfig_ = G_AssembleReddConfig(reddConfig);
}
/// @brief Set redundancy configuration
/// @param eReddStrategy
/// @param reddConfig
void PConfigMuster::SetReddConfig(EReddStrategy const eReddStrategy, PReddConfigData const &reddConfig) noexcept
{
    SetReddStrategy(eReddStrategy);
    globleReddConfig_ = reddConfig;
}
/// @brief Whether redundancy switch is turned on
/// @return
bool PConfigMuster::IsEnableRedd() const noexcept
{
    if (EReddStrategy::kNone == eReddStrategy_) {
        return false;
    }
    return true;
}
/// @brief Check if redundancy strategy is MofN
/// @return
bool PConfigMuster::IsReddMofN() const noexcept
{
    if (false == IsEnableRedd()) {
        return false;
    }
    return globleReddConfig_.IsReddMofN();
}
/// @brief Check if redundancy strategy is CRC or Hash (redundancy types requiring verification)
/// @return
bool PConfigMuster::IsReddCheckType() const noexcept
{
    if (false == IsEnableRedd()) {
        return false;
    }
    return IsReddCheckType(globleReddConfig_.eReddType);
}
/// @brief Get redundancy type
/// @return
EReddType PConfigMuster::GetReddType() const noexcept
{
    if (false == IsEnableRedd()) {
        return EReddType::kNone;
    }
    return globleReddConfig_.eReddType;
}
/// @brief Get M in M/N configuration
/// @return
uint32_t PConfigMuster::GetReddCountM() const noexcept
{
    if (false == IsReddMofN()) {
        return 0U;
    }
    return static_cast< uint32_t >(globleReddConfig_.reddMofN.m);
}
/// @brief Get N in M/N configuration
/// @return
uint32_t PConfigMuster::GetReddCountN() const noexcept
{
    if (false == IsReddMofN()) {
        return 1U;
    }
    return static_cast< uint32_t >(globleReddConfig_.reddMofN.n);
}
//***************/
/// @brief Set Config
/// @param config
/// @return
bool PConfigMuster::SetConfig(PConfigMuster const &config) noexcept
{
    if (false == config.IsValid()) {
        return false;
    }
    *this = config;
    return true;
}
/// @brief Assemble configuration file through Json configuration
/// @param pStorageConfig
/// @return
bool PConfigMuster::AssembleConfig(ara::core::String const &stVersionApp,
                                   manifest::MConfig_StorageBase const *const pStorageConfig) noexcept
{
    if (nullptr == pStorageConfig) {
        return false;
    }
    SetStorageIns(pStorageConfig->stStorageIns);
    SetStorageName(pStorageConfig->dataStorage.stName);
    SetWorkPath(pStorageConfig->dataStorage.stPath);
    SetVersionApp(stVersionApp);
    SetVersionPer(pStorageConfig->dataStorage.stVersionPer);
    SetSpaceAmountMin(pStorageConfig->dataStorage.nSizeMin);
    SetSpaceAmountMax(pStorageConfig->dataStorage.nSizeMax);
    SetUpdateStrategy(pStorageConfig->dataStorage.stUpdateStrategy);
    SetReddConfig(T_StringView(pStorageConfig->dataStorage.stReddStrategy), pStorageConfig->dataStorage.reddConfig);

    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return false;
    }
    manifest::MConfigData_KeySlotStorage const *const pFindKeySlot{
        pManifestPer->FindKeySlot_Storage(pStorageConfig->stStorageIns)};
    if (nullptr != pFindKeySlot) {
        std::ignore = SetCryptoConfig(pFindKeySlot->stCryptoAlgorithm, pFindKeySlot->stKeySlotIns,
                                      pFindKeySlot->stKeySlotUsage, pFindKeySlot->stVerificationHash);
    }
    // Set encryption configuration
    std::ignore = pManifestPer->ForEachKeySlotElement(
        pStorageConfig->stStorageIns, [this](manifest::MConfigData_KeySlotElement const &keySlotElement) -> bool {
            PConfigData_Crypto configCrypto;
            configCrypto.stCryptoAlgorithm = keySlotElement.stCryptoAlgorithm;
            configCrypto.stKeySlotName     = keySlotElement.stKeySlotIns;
            configCrypto.eKeySlotUsage     = TransKeySlotUsage(keySlotElement.stKeySlotUsage);
            configCrypto.stVerfHash        = keySlotElement.stVerificationHash;
            _AddElementCrypto(keySlotElement.stElementName, std::move(configCrypto));
            return true;
        });
    SetConfigValid(true);
    return true;
}
/// @brief Check if encryption operation is supported: Storage
/// @return
bool PConfigMuster::IsHaveCrypto_Storage(ECryptoKeySlotUsage eKeySlotUsage) const noexcept
{
    if (cryptoData_.stKeySlotName.empty()) {
        return false;
    }
    switch (eKeySlotUsage) {
        case ECryptoKeySlotUsage::kVerification: {
            return (cryptoData_.eKeySlotUsage == eKeySlotUsage) && (false == cryptoData_.stVerfHash.empty());
        } break;
        case ECryptoKeySlotUsage::kEncryption: {
            return cryptoData_.eKeySlotUsage == eKeySlotUsage;
        } break;
        default: {
            return true;
        } break;
    }
    return true;
}
/// @brief Check if encryption operation is supported: Element
/// @param stElement
/// @return
PConfigData_Crypto const *PConfigMuster::IsHaveCrypto_Element(ara::core::String const &stElement) const noexcept
{
    ara::core::Map< ara::core::String, PConfigData_Crypto >::const_iterator itFind;
    itFind = mapElementCrypto_.find(stElement);
    if (itFind == mapElementCrypto_.end()) {
        return nullptr;
    }
    return &(itFind->second);
}
/// @brief Check if encryption operation is supported: Element encryption
/// @param stElement
/// @return
PConfigData_Crypto const *PConfigMuster::IsHaveCrypto_ElementEncryption(
    ara::core::String const &stElement) const noexcept
{
    PConfigData_Crypto const *pFindCrypto = IsHaveCrypto_Element(stElement);
    if (nullptr == pFindCrypto) {
        return nullptr;
    }
    if (ECryptoKeySlotUsage::kEncryption != pFindCrypto->eKeySlotUsage) {
        return nullptr;
    }
    return pFindCrypto;
}
/// @brief Set encryption
/// @param stCryptoAlgorithm
/// @param stKeySlotName
/// @param stKeySlotUsage
/// @param stVerfHash
/// @return
bool PConfigMuster::SetCryptoConfig(ara::core::String const &stCryptoAlgorithm,
                                    ara::core::String const &stKeySlotName,
                                    ara::core::String const &stKeySlotUsage,
                                    ara::core::String const &stVerfHash) noexcept
{
    cryptoData_.stCryptoAlgorithm = stCryptoAlgorithm;
    cryptoData_.stKeySlotName     = stKeySlotName;
    cryptoData_.eKeySlotUsage     = TransKeySlotUsage(stKeySlotUsage);
    cryptoData_.stVerfHash        = stVerfHash;
    return true;
}
//********************************/
/// @brief Assemble configuration file needed by FileStorage through Json configuration
/// @param manifrestRead Configuration reader
/// @param portIns Port identifier
/// @return Success or failure
bool PConfigMuster_File::AssembleFileConfig(ara::core::InstanceSpecifier const &portIns) noexcept
{
    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return false;
    }
    manifest::MConfig_StorageFile const *const pFindConfig{
        ara::per::manifest::PManifestInstance::FindStorage_File(portIns)};
    if (nullptr == pFindConfig) {
        return false;
    }
    return AssembleFileConfig(*pManifestPer, *pFindConfig);
}
/// @brief Assemble configuration file needed by FileStorage through Json configuration
/// @brief Assemble configuration file needed by FileStorage through Json configuration
/// @param manifrestRead Configuration reader
/// @param configFile Configuration in configuration file
/// @return Success or failure
bool PConfigMuster_File::AssembleFileConfig(ara::per::manifest::PManifestReader_Per const &manifrestRead,
                                            ara::per::manifest::MConfig_StorageFile const &configFile) noexcept
{
    // Redundancy configuration
    for (manifest::MConfigData_InitFile const &initFile : configFile.dataFile.vecInitFile) {
        PReddConfigData const reddConfig{G_AssembleReddConfig(initFile.reddConfig)};
        if (reddConfig.IsValidRedd()) {
            mapFileRedd_[initFile.stFileName] = reddConfig;
        }
    }
    nMaxFileCount_ = configFile.dataFile.nMaxFileCount;
    return AssembleConfig(manifrestRead.GetVersionApp(), &configFile);
}
/// @brief Find MofN redundancy configuration for specific file
/// @param stFileName
/// @return
PReddDataMofN PConfigMuster_File::FindFileReddMofN(ara::core::StringView const &stFileName) const noexcept
{
    PReddDataMofN reddData;
    PReddConfigData const *pFindReddConfig{nullptr};
    switch (GetReddStrategy()) {
        case EReddStrategy::kStorage: {
            pFindReddConfig = _GetGlobleReddConfig();
        } break;
        case EReddStrategy::kNone: {
        } break;
        case EReddStrategy::kElement: {
            // Use global setting if specific setting is not found
            pFindReddConfig = _GetGlobleReddConfig();
            ara::core::Map< ara::core::String, PReddConfigData >::const_iterator itFind;
            itFind = mapFileRedd_.find(isoftkv::T_String(stFileName));
            if (itFind != mapFileRedd_.end()) {
                pFindReddConfig = &(itFind->second);
            }
        } break;
    }
    reddData.m = 0U;
    reddData.n = 1U;  // Must have at least one
    if (nullptr != pFindReddConfig) {
        if (pFindReddConfig->IsReddMofN()) {
            reddData = pFindReddConfig->reddMofN;
        }
    } else {
    }
    return reddData;
}
//********************************/
/// @brief
/// @param a
PConfigMuster_Kv::PConfigMuster_Kv(PConfigMuster_Kv &&a) noexcept : PConfigMuster{std::move(a)}
{
    _MoveConstructor(std::move(a));
}
/// @brief
/// @param a
/// @return
PConfigMuster_Kv &PConfigMuster_Kv::operator=(PConfigMuster_Kv &&a) noexcept
{
    *static_cast< PConfigMuster * >(this) = std::move(a);
    _MoveConstructor(std::move(a));
    return *this;
}
/// @brief
/// @param portIns
/// @return
bool PConfigMuster_Kv::AssembleKvConfig(ara::core::InstanceSpecifier const &portIns) noexcept
{
    manifest::MConfig_StorageKv const *const pFindConfig{manifest::PManifestInstance::FindStorage_Kv(portIns)};
    if (nullptr == pFindConfig) {
        return false;
    }
    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return false;
    }
    return AssembleKvConfig(pManifestPer->GetVersionApp(), *pFindConfig);
}
/// @brief Assemble KV configuration file through Json configuration
/// @param configKv Configuration in configuration file
/// @return Success or failure
bool PConfigMuster_Kv::AssembleKvConfig(ara::core::String const &stVersionApp,
                                        manifest::MConfig_StorageKv const &configKv) noexcept
{
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nMaxCacheCount) {
        SetMaxCacheCount(configKv.dataKv.nMaxCacheCount);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nInitPageCount) {
        SetInitPageCount(configKv.dataKv.nInitPageCount);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nPageInitLen) {
        SetPageInitLen(configKv.dataKv.nPageInitLen);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nFreeListCapacity) {
        SetFreeListCapacity(configKv.dataKv.nFreeListCapacity);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nMaxPageInGroup) {
        SetMaxPageInGroup(configKv.dataKv.nMaxPageInGroup);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nHashInitCount) {
        SetHashInitCount(configKv.dataKv.nHashInitCount);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nHashMaxCapacity) {
        SetHashMaxCapacity(configKv.dataKv.nHashMaxCapacity);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nAutoSaveWalog) {
        SetAutoSaveWalog(configKv.dataKv.nAutoSaveWalog);
    }
    if (static_cast< uint32_t >(-1) != configKv.dataKv.nMaxKvDataLen) {
        SetMaxKvDataLen(configKv.dataKv.nMaxKvDataLen);
    }
    SetInitUpdateFile(configKv.dataKv.stInitFile);
    return AssembleConfig(stVersionApp, &configKv);
}
/// @brief
/// @param a
void PConfigMuster_Kv::_MoveConstructor(PConfigMuster_Kv &&a) noexcept
{
    nMaxCacheCount_    = std::move(a.nMaxCacheCount_);
    nInitPageCount_    = std::move(a.nInitPageCount_);
    nPageInitLen_      = std::move(a.nPageInitLen_);
    nFreeListCapacity_ = std::move(a.nFreeListCapacity_);
    nMaxPageInGroup_   = std::move(a.nMaxPageInGroup_);
    nHashInitCount_    = std::move(a.nHashInitCount_);
    nHashMaxCapacity_  = std::move(a.nHashMaxCapacity_);
    nAutoSaveWalog_    = std::move(a.nAutoSaveWalog_);
    nMaxKvDataLen_     = std::move(a.nMaxKvDataLen_);
    nHashType_         = std::move(a.nHashType_);
    nCompressType_     = std::move(a.nCompressType_);
    stInitUpdateFile_  = std::move(a.stInitUpdateFile_);
}
//********************************/
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @param stData String format enum
/// @return Corresponding enum value
EUpdateStrategy_Storage TransUpdate_Storage(ara::core::String const &stData) noexcept
{
    EUpdateStrategy_Storage eData{EUpdateStrategy_Storage::kUndefine};
    if (stData == manifest::kUpdateDataKeepExist) {
        eData = EUpdateStrategy_Storage::kKeepExisting;
    } else if (stData == manifest::kUpdateDataDelete) {
        eData = EUpdateStrategy_Storage::kDelete;
    }
    return eData;
}
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @param stData String format enum
/// @return Corresponding enum value
EUpdateStrategy_Element TransUpdate_Element(ara::core::String const &stData) noexcept
{
    EUpdateStrategy_Element eData{EUpdateStrategy_Element::kUndefine};
    if (stData == manifest::kUpdateDataOverWrite) {
        eData = EUpdateStrategy_Element::kOverwrite;
    } else if (stData == manifest::kUpdateDataKeepExist) {
        eData = EUpdateStrategy_Element::kKeepExisting;
    } else if (stData == manifest::kUpdateDataDelete) {
        eData = EUpdateStrategy_Element::kDelete;
    }
    return eData;
}
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @param stData String format enum
/// @return Corresponding enum value
ECryptoKeySlotUsage TransKeySlotUsage(ara::core::String const &stData) noexcept
{
    ECryptoKeySlotUsage eKeySlotUsage = ECryptoKeySlotUsage::kEncryption;
    if (manifest::kKeySlotUsage_encryption == stData) {
        eKeySlotUsage = ECryptoKeySlotUsage::kEncryption;
    } else if (manifest::kKeySlotUsage_verification == stData) {
        eKeySlotUsage = ECryptoKeySlotUsage::kVerification;
    }
    return eKeySlotUsage;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
