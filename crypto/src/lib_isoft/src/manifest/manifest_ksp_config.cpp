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
/// @file       manifest_ksp_config.cpp
/// @brief      AutoSar-Crypto Configuration File: Server Side
/// @details
/// @date       2022-06-28
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Interpret Configuration File
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestKspConfig
/// @unit_description=Data structure interpreted from crypto_manifest.json file used by the encryption module
/// @endcode
///
/// ================================================================

#include "ara/crypto/manifest/manifest_ksp_config.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/manifest/manifest_crypto_common.h"

//********************************/
namespace manifest = ara::crypto::manifest;
//********************************/ //Key Slot
// PRQA S 2024,2025 QAC /// @qac: Cannot be modified within macro definition
MR_HEADER(manifest::PConfig_KeySlotAllowedModification)
MR_FIELD(manifest::GetkAllowContentTypeChange(), allowContentTypeChange)
MR_FIELD(manifest::GetkExportability(), exportability)
MR_FIELD(manifest::GetkMaxNumberOfAllowedUpdates(), maxNumberOfAllowedUpdates)
MR_FIELD(manifest::GetkRestrictUpdate(), restrictUpdate)
MR_FOOTER
//***************/
MR_HEADER(manifest::PConfig_KeySlotContentAllowedUsage)
MR_FIELD(manifest::GetkAllowedKeyslotUsage(), allowedKeyslotUsage)
MR_FOOTER
//***************/
MR_HEADER(manifest::PConfig_KeySlotData)
MR_FIELD(manifest::GetkSlotShortName(), shortName)
MR_FIELD(manifest::GetkAllocateShadowCopy(), allocateShadowCopy)
MR_FIELD(manifest::GetkCryptoAlgId(), cryptoAlgId)
MR_FIELD(manifest::GetkCryptoObjectType(), cryptoObjectType)
MR_FIELD(manifest::GetkKeySlotAllowedModification(), keySlotAllowedModification)
MR_FIELD(manifest::GetkKeySlotContentAllowedUsage(), keySlotContentAllowedUsage)
MR_FIELD(manifest::GetkSlotCapacity(), slotCapacity)
MR_FIELD(manifest::GetkSlotType(), slotType)
MR_OPTIONAL_FIELD(manifest::GetkAppName(), stAppName)
MR_OPTIONAL_FIELD(manifest::GetkFileName(), stFileName)
MR_FOOTER
// manifest::kCryptoKeySlots
MR_HEADER(manifest::PConfig_CryptoKeySlot)
MR_FIELD(manifest::GetkKeySlotInstance(), stKeySlotInstance)
MR_FIELD(manifest::GetkKeySlotData(), keySlotData)
MR_FOOTER
//********************************/ //Crypto Provider
MR_HEADER(manifest::PConfig_InstanceKeySlot)
MR_FIELD(manifest::GetkKeySlotInstance(), stKeySlotInstance)
MR_FOOTER
// manifest::kCryptoProviders
MR_HEADER(manifest::PConfig_CryptoProvider)
MR_FIELD(manifest::GetkProviderInstance(), stProviderInstance)
MR_FIELD(manifest::GetkKeySlotList(), vecKeySlot)
MR_FOOTER
//********************************/ //Certificate
MR_HEADER(manifest::PConfig_CertificateData)
MR_FIELD(manifest::GetkCertShortName(), stShortName)
MR_FIELD(manifest::GetkCertIsPrivate(), isPrivate)
MR_FOOTER
// manifest::kCryptoCerts
MR_HEADER(manifest::PConfig_CryptoCertificate)
MR_FIELD(manifest::GetkCertInstance(), stCertInstance)
MR_FIELD(manifest::GetkCertData(), certData)
MR_FOOTER
//***************/ //Certificate: Service
MR_HEADER(manifest::PConfig_ServiceCertificateData)
MR_FIELD(manifest::GetkServiceAlgFamily(), stAlgorithmFamily)
MR_FIELD(manifest::GetkServiceCertFormat(), stFormat)
MR_FIELD(manifest::GetkServiceMaximumLength(), nMaximumLength)
MR_FIELD(manifest::GetkServiceNextHigherCertificate(), stNextHigherCertificate)
MR_FOOTER
// manifest::kCryptoServiceCerts
MR_HEADER(manifest::PConfig_ServiceCertificate)
MR_FIELD(manifest::GetkCertInstance(), stCertInstance)
MR_FIELD(manifest::GetkkServiceCertData(), certData)
MR_FOOTER
// manifest::kCertToKeySlot
MR_HEADER(manifest::PConfig_MappingCertToKeySlot)
MR_FIELD(manifest::GetkCertInstance(), stCertInstance)
MR_FIELD(manifest::GetkKeySlotList(), vecKeySlot)
MR_FOOTER
//********************************/
// manifest::kCertToKeySlot
MR_HEADER(manifest::PConfig_CryptoTrustMasterProcess)
MR_FIELD(manifest::GetkProcessName(), stProcessName)
MR_FOOTER
// PRQA L:QAC
//********************************/
namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Key slot usage scope
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03245
/// @trace_id_dd=DD_CRYPTO_06484
/// @needwork = ad
/// @endcode
class MSlotAllowedUsage
{
public:
    /// @brief AllowedUsageFlags in the configuration file are identified by strings, so conversion to enums is required
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06457
    /// @needwork = dd
    /// @endcode
    using Map_AllowedUsage = ara::core::Map< ara::core::String, AllowedUsageFlags >;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06620
    /// @needwork = dd
    /// @endcode
    MSlotAllowedUsage() noexcept;
    /// @brief Copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06621
    /// @needwork = dd
    /// @endcode
    MSlotAllowedUsage(MSlotAllowedUsage const &other) = default;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06622
    /// @needwork = dd
    /// @endcode
    MSlotAllowedUsage(MSlotAllowedUsage &&other) = default;
    /// @brief Copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06623
    /// @needwork = dd
    /// @endcode
    MSlotAllowedUsage &operator=(MSlotAllowedUsage const &other) = default;
    /// @brief Move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06624
    /// @needwork = dd
    /// @endcode
    MSlotAllowedUsage &operator=(MSlotAllowedUsage &&other) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06647
    /// @needwork = dd
    /// @endcode
    virtual ~MSlotAllowedUsage() = default;
    /// @brief Convert string format of AllowedUsage
    /// @param stData String of AllowedUsage
    /// @return Usage restriction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06625
    /// @needwork = dd
    /// @endcode
    AllowedUsageFlags TransAllowedUsage(ara::core::String const &stData) const noexcept;

protected:
    /// @brief Find usage scope
    /// @param stData String of AllowedUsage
    /// @return Usage restriction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06626
    /// @needwork = dd
    /// @endcode
    AllowedUsageFlags _FindAllowedUsage(ara::core::String const &stData) const noexcept;
    /// @brief Split string
    /// @param stData Original string
    /// @param split Delimiter
    /// @param vecData Return result
    /// @return 0 fail sucess otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06627
    /// @needwork = dd
    /// @endcode
    static uint32_t StringSplit(ara::core::String const &stData,
                                char8_t const split,
                                ara::core::Vector< ara::core::String > &vecData) noexcept;

private:
    /// @brief MAP list storing usage scope
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03245
    /// @trace_id_dd=DD_CRYPTO_06628
    /// @needwork = dd
    /// @endcode
    Map_AllowedUsage mapAllowedUsage_;
};
//***************/
/// @brief Constructor
MSlotAllowedUsage::MSlotAllowedUsage() noexcept : mapAllowedUsage_{}  // NOLINT
{
    mapAllowedUsage_[{"ALLOW-DATA-ENCRYPTION"}]         = kAllowDataEncryption;
    mapAllowedUsage_[{"ALLOW-DATA-DECRYPTION"}]         = kAllowDataDecryption;
    mapAllowedUsage_[{"ALLOW-SIGNATURE"}]               = kAllowSignature;
    mapAllowedUsage_[{"ALLOW-VERIFICATION"}]            = kAllowVerification;
    mapAllowedUsage_[{"ALLOW-KEY-AGREEMENT"}]           = kAllowKeyAgreement;
    mapAllowedUsage_[{"ALLOW-KEY-DIVERSIFY"}]           = kAllowKeyDiversify;
    mapAllowedUsage_[{"ALLOW-DRNG-INIT"}]               = kAllowRngInit;
    mapAllowedUsage_[{"ALLOW-KDF-MATERIAL"}]            = kAllowKdfMaterial;
    mapAllowedUsage_[{"ALLOW-KEY-EXPORTING"}]           = kAllowKeyExporting;
    mapAllowedUsage_[{"ALLOW-KEY-IMPORTING"}]           = kAllowKeyImporting;
    mapAllowedUsage_[{"ALLOW-EXACT-MODE-ONLY"}]         = kAllowExactModeOnly;
    mapAllowedUsage_[{"ALLOW-DERIVED-DATA-ENCRYPTION"}] = kAllowDerivedDataEncryption;
    mapAllowedUsage_[{"ALLOW-DERIVED-DATA-DECRYPTION"}] = kAllowDerivedDataDecryption;
    mapAllowedUsage_[{"ALLOW-DERIVED-SIGNATURE"}]       = kAllowDerivedSignature;
    mapAllowedUsage_[{"ALLOW-DERIVED-VERIFICATION"}]    = kAllowDerivedVerification;
    mapAllowedUsage_[{"ALLOW-DERIVED-DIVERSIFY"}]       = kAllowDerivedKeyDiversify;
    mapAllowedUsage_[{"ALLOW-DERIVED-DRNG-INIT"}]       = kAllowDerivedRngInit;
    mapAllowedUsage_[{"ALLOW-DERIVED-KDF-MATERIAL"}]    = kAllowDerivedKdfMaterial;
    mapAllowedUsage_[{"ALLOW-DERIVED-KEY-EXPORTING"}]   = kAllowDerivedKeyExporting;
    mapAllowedUsage_[{"ALLOW-DERIVED-KEY-IMPORTING"}]   = kAllowDerivedKeyImporting;
    mapAllowedUsage_[{"ALLOW-DERIVED-EXACT-MODE-ONLY"}] = kAllowDerivedExactModeOnly;
}
/// @brief Convert string format usage scope to enum format
/// @param stData String of AllowedUsage
/// @return
AllowedUsageFlags MSlotAllowedUsage::TransAllowedUsage(ara::core::String const &stData) const noexcept
{
    ara::core::Vector< ara::core::String > vecData;
    char8_t const nCharComma{','};
    std::ignore = StringSplit(stData, nCharComma, vecData);
    AllowedUsageFlags allowedUsage{kAllowPrototypedOnly};
    for (auto &itData : vecData) {
        allowedUsage |= _FindAllowedUsage(itData);
    }
    return allowedUsage;
}
//***************/
/// @brief Find usage scope
/// @param stData String of AllowedUsage
/// @return
AllowedUsageFlags MSlotAllowedUsage::_FindAllowedUsage(ara::core::String const &stData) const noexcept
{
    Map_AllowedUsage::const_iterator const itFind{mapAllowedUsage_.find(stData)};
    if (itFind == mapAllowedUsage_.end()) {
        return kAllowPrototypedOnly;
    }
    return itFind->second;
}
/// @brief Split string
/// @param stData Original string
/// @param split Delimiter
/// @param vecData Return result
/// @return Split string number
uint32_t MSlotAllowedUsage::StringSplit(ara::core::String const &stData,
                                        char8_t const split,
                                        ara::core::Vector< ara::core::String > &vecData) noexcept
{
    if (stData.empty()) {
        return 0U;
    }
    ara::core::String::size_type i{0U};
    while (true) {
        if (i >= stData.size()) {
            break;
        }
        ara::core::String::size_type nPos{stData.find(split, i)};
        if (nPos == ara::core::String::npos) {
            nPos = stData.size();
        }
        ara::core::String stWord{stData.substr(i, nPos - i)};
        std::ignore = stWord.erase(stWord.find_last_not_of(" \t") + 1U);
        std::ignore = stWord.erase(0U, stWord.find_first_not_of(" \t"));
        vecData.push_back(stWord);
        i = nPos + 1U;
    }
    return static_cast< uint32_t >(vecData.size());
}
//********************************/
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if has init manisfest sucess false otherwise
bool PManifestKspConfig::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};

    if (!manifestRes.HasValue()) {
        return false;
    }
    isoft::manifestreader::Manifest *const pManifestRes{manifestRes.Value().get()};
    ara::crypto::isoft_def::LogInfo() << "PManifestKspConfig::LoadData ConfigName = " << stFileName.data();

    std::ignore = ReadMapData< PConfig_CryptoKeySlot >(
        pManifestRes, ara::core::String(manifest::GetkCryptoKeySlots()),
        [this](PConfig_CryptoKeySlot const &data) -> void { mapKeySlot_[data.stKeySlotInstance] = data; });
    std::ignore = ReadMapData< PConfig_CryptoProvider >(
        pManifestRes, ara::core::String(manifest::GetkCryptoProviders()),
        [this](PConfig_CryptoProvider const &data) -> void { mapCryptoProvider_[data.stProviderInstance] = data; });
    std::ignore = ReadMapData< PConfig_CryptoCertificate >(
        pManifestRes, ara::core::String(manifest::GetkCryptoCerts()),
        [this](PConfig_CryptoCertificate const &data) -> void { mapCryptoCert_[data.stCertInstance] = data; });
    std::ignore = ReadMapData< PConfig_ServiceCertificate >(
        pManifestRes, ara::core::String(manifest::GetkCryptoServiceCerts()),
        [this](PConfig_ServiceCertificate const &data) -> void { mapServiceCert_[data.stCertInstance] = data; });
    std::ignore = ReadMapData< PConfig_MappingCertToKeySlot >(
        pManifestRes, ara::core::String(manifest::GetkCertToKeySlot()),
        [this](PConfig_MappingCertToKeySlot const &data) -> void { mapCertToKeySlot_[data.stCertInstance] = data; });

    std::ignore = ReadMapData< PConfig_CryptoTrustMasterProcess >(
        pManifestRes, ara::core::String(manifest::GetkCryptoTrustMasterProcess()),
        [this](PConfig_CryptoTrustMasterProcess const &data) -> void {
            vectorCryptoTrusterMasterProcess_.push_back(data);
        });

    return true;
}
//********************************/
/// @brief Find slot by name
/// @param stSlotName Key slot name
/// @return Pointer to PCryptoKeySlot
PConfig_CryptoKeySlot const *PManifestKspConfig::FindKeySlotByName(
    ara::core::StringView const &stSlotName) const noexcept
{
    ara::core::StringView::size_type const nFindPos{stSlotName.find("/")};
    // If '/' is found in stSlotName, it indicates an identifier was entered
    if (ara::core::StringView::npos != nFindPos) {
        MAP_CryptoKeySlot::const_iterator const &itFind{
            mapKeySlot_.find(ara::core::String(stSlotName.data(), stSlotName.size()))};
        if (itFind == mapKeySlot_.end()) {
            return nullptr;
        }
        return &(itFind->second);
    }
    // Otherwise, search by ShortName: This logic does not apply to cases where "ShortName is duplicated in config"
    for (auto const &it : mapKeySlot_) {
        if (it.second.keySlotData.shortName.compare(stSlotName) == 0) {
            return &(it.second);
        }
    }
    return nullptr;
}
/// @brief Find certificate slot by name (use identifier if '/' is found, otherwise ShortName)
/// @param stCertName Certificate name
/// @return Instance of PCryptoKeySlot
PConfig_CryptoKeySlot const *PManifestKspConfig::FindCertSlotByName(
    ara::core::StringView const &stCertName) const noexcept
{
    PConfig_CryptoCertificate const *const pFindCert{FindCertByName(stCertName)};
    if (nullptr == pFindCert) {
        return nullptr;
    }
    MAP_CertToKeySlot::const_iterator const itFind{mapCertToKeySlot_.find(pFindCert->stCertInstance)};
    if (itFind == mapCertToKeySlot_.end()) {
        return nullptr;
    }
    if (itFind->second.vecKeySlot.empty()) {
        return nullptr;
    }
    // 2022-11-09 hanjingjing, forcibly get the first Slot
    ara::core::String const nKeySlotInstance{itFind->second.vecKeySlot[0U].stKeySlotInstance};
    return FindKeySlotByName({nKeySlotInstance.data(), nKeySlotInstance.size()});
}

/// @brief Find certificate slot by name (use identifier if '/' is found, otherwise ShortName)
/// @param stCertName Certificate name
/// @return Port name of the key slot
ara::core::Vector< ara::core::String > PManifestKspConfig::FindCertSlotNamesByName(
    ara::core::StringView const &stCertName) const noexcept
{
    ara::core::Vector< ara::core::String > vecSlotNames;
    PConfig_CryptoCertificate const *const pFindCert{FindCertByName(stCertName)};
    if (nullptr == pFindCert) {
        return vecSlotNames;
    }
    MAP_CertToKeySlot::const_iterator const itFind{mapCertToKeySlot_.find(pFindCert->stCertInstance)};
    if (itFind == mapCertToKeySlot_.end()) {
        return vecSlotNames;
    }
    if (itFind->second.vecKeySlot.empty()) {
        return vecSlotNames;
    }

    ara::core::Vector< PConfig_InstanceKeySlot > vecInstances{itFind->second.vecKeySlot};
    for (auto &itData : vecInstances) {
        vecSlotNames.push_back(itData.stKeySlotInstance);
    }
    return vecSlotNames;
}
/// @brief Find certificate slot by name (use identifier if '/' is found, otherwise ShortName)
/// @param stCertName Certificate name
/// @return Certificate fqn
ara::core::Vector< ara::crypto::manifest::PConfig_InstanceKeySlot > PManifestKspConfig::FindKeySlotByCertSlotName(
    ara::core::StringView const &stCertName) const noexcept
{
    PConfig_CryptoCertificate const *const pFindCert{FindCertByName(stCertName)};
    ara::core::Vector< ara::crypto::manifest::PConfig_InstanceKeySlot > failed;
    if (nullptr == pFindCert) {
        return failed;
    }
    MAP_CertToKeySlot::const_iterator const itFind{mapCertToKeySlot_.find(pFindCert->stCertInstance)};
    if (itFind == mapCertToKeySlot_.end()) {
        return failed;
    }
    if (itFind->second.vecKeySlot.empty()) {
        return failed;
    }
    // 2022-11-09 hanjingjing, forcibly get the first Slot
    return itFind->second.vecKeySlot;
}
/// @brief Find certificate slot by name (use identifier if '/' is found, otherwise ShortName)
/// @param stCertName Certificate name
/// @return Pointer to PCryptoCertificate
PConfig_CryptoCertificate const *PManifestKspConfig::FindCertByName(
    ara::core::StringView const &stCertName) const noexcept
{
    ara::core::StringView::size_type const nFindPos{stCertName.find("/")};
    // If '/' is found in stCertName, it indicates an identifier was entered
    if (ara::core::StringView::npos != nFindPos) {
        MAP_CryptoCertificate::const_iterator const &itFind{
            mapCryptoCert_.find(ara::core::String(stCertName.data(), stCertName.size()))};
        if (itFind == mapCryptoCert_.end()) {
            return nullptr;
        }
        return &(itFind->second);
    }
    // Otherwise, search by ShortName: This logic does not apply to cases where "ShortName is duplicated in config"
    for (auto const &it : mapCryptoCert_) {
        if (it.second.certData.stShortName == stCertName.data()) {
            return &(it.second);
        }
    }
    return nullptr;
}
/// @brief Find key slot properties by name
/// @param stSlotIns Key slot identifier: string format
/// @param stCryptoAlgID Crypto primitive ID: string format
/// @return
keys::KeySlotPrototypeProps::Uptr PManifestKspConfig::FindSlotPropsByName(
    ara::core::StringView const &stSlotIns, ara::core::String &stCryptoAlgID) const noexcept
{
    if (false == stCryptoAlgID.empty()) {
    }  // for qac
    PConfig_CryptoKeySlot const *const pFindSlot{FindKeySlotByName(stSlotIns)};
    if (nullptr == pFindSlot) {
        return {nullptr};
    }
    keys::KeySlotPrototypeProps::Uptr const pSlotProps{std::make_unique< keys::KeySlotPrototypeProps >()};
    if (nullptr == pSlotProps) {
        return {nullptr};
    }
    stCryptoAlgID = pFindSlot->keySlotData.cryptoAlgId;
    return AssembleSlotProps(pFindSlot);
}
//********************************/
/// @brief Assemble key slot property structure
/// @param pFindSlot Pointer to key slot
/// @return Instance of KeySlotPrototypeProps
keys::KeySlotPrototypeProps::Uptr PManifestKspConfig::AssembleSlotProps(
    PConfig_CryptoKeySlot const *const pFindSlot) noexcept
{
    if (nullptr == pFindSlot) {
        return {nullptr};
    }
    keys::KeySlotPrototypeProps::Uptr pSlotProps{std::make_unique< keys::KeySlotPrototypeProps >()};
    if (nullptr == pSlotProps) {
        return {nullptr};
    }
    // GetSlotAlgId function returns string type, KeySlotPrototypeProps.mAlgId type mismatch, temporarily return default value
    pSlotProps->mAlgId                  = kAlgIdUndefined;
    pSlotProps->mAllocateSpareSlot      = pFindSlot->keySlotData.allocateShadowCopy;
    pSlotProps->mAllowContentTypeChange = pFindSlot->keySlotData.keySlotAllowedModification.allowContentTypeChange;
    MSlotAllowedUsage const nSlotAllowedUsage{};
    pSlotProps->mContentAllowedUsage
        = nSlotAllowedUsage.TransAllowedUsage(pFindSlot->keySlotData.keySlotContentAllowedUsage.allowedKeyslotUsage);
    pSlotProps->mExportAllowed = pFindSlot->keySlotData.keySlotAllowedModification.exportability;
    pSlotProps->mMaxUpdateAllowed
        = static_cast< int32_t >(pFindSlot->keySlotData.keySlotAllowedModification.maxNumberOfAllowedUpdates);
    isoft::manifestreader::tps::CryptoKeySlotTypeEnum eSlotType{
        isoft::manifestreader::tps::CryptoKeySlotTypeEnum::kApplication};
    std::ignore = isoft::manifestreader::tps::FromString(pFindSlot->keySlotData.slotType, eSlotType);
    if (eSlotType == isoft::manifestreader::tps::CryptoKeySlotTypeEnum::kMachine) {
        pSlotProps->mSlotType = KeySlotType::kMachine;
    } else {
        pSlotProps->mSlotType = KeySlotType::kApplication;
    }
    pSlotProps->mSlotCapacity = static_cast< size_t >(pFindSlot->keySlotData.slotCapacity);
    isoft::manifestreader::tps::CryptoObjectTypeEnum eObjectType{
        isoft::manifestreader::tps::CryptoObjectTypeEnum::kPrivateKey};
    std::ignore = isoft::manifestreader::tps::FromString(pFindSlot->keySlotData.cryptoObjectType, eObjectType);
    pSlotProps->mObjectType = static_cast< CryptoObjectType >(eObjectType);

    return pSlotProps;
}
/// @brief Find crypto provider by key slot name
/// @param stSlotName Key slot name
/// @return Crypto provider to which the key slot belongs
ara::core::String PManifestKspConfig::FindCryptoProviderBySlotName(
    ara::core::StringView const &stSlotName) const noexcept
{
    ara::core::String stReturn{};
    PConfig_CryptoKeySlot const *const pCryptoKeySlot{FindKeySlotByName(stSlotName)};
    if (pCryptoKeySlot == nullptr) {
        return ara::core::String{};
    }
    if (nullptr != pCryptoKeySlot) {
        for (auto const &it : mapCryptoProvider_) {
            for (auto const &itData : it.second.vecKeySlot) {
                if (itData.stKeySlotInstance == pCryptoKeySlot->stKeySlotInstance) {
                    stReturn = itData.stKeySlotInstance;
                    return stReturn;
                }
            }
        }
    }
    return stReturn;
}
//********************************/
/// @brief Determine if the process is TrustMaster by process name
/// @param stProcessName Process name
/// @return true if trustmaster process false otherwise
bool PManifestKspConfig::IsTrustMaster(ara::core::StringView const &stProcessName) const noexcept
{
    size_t const nSize{vectorCryptoTrusterMasterProcess_.size()};
    for (size_t i{0U}; i < nSize; i++) {
        if (vectorCryptoTrusterMasterProcess_[i].stProcessName.compare(stProcessName) == 0) {
            return true;
        }
    }
    return false;
}
//********************************/

}  // namespace manifest
}  // namespace crypto
}  // namespace ara