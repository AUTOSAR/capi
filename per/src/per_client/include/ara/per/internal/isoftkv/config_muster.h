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
/// @file       config_muster.h
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
///
/// @code{.isoft}
/// @module_path=/per/Common/Configuration Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PHKV_PH_CONFIG_MUSTER_H_
#define ARA_PER_PHKV_PH_CONFIG_MUSTER_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>

#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/manifest/manifest_reader_per.h"
#include "ara/per/internal/manifest/manifest_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Enum values correspond to Table A.15: PersistencyCollectionLevelUpdateStrategyEnum
/// @code{.isoft}
/// @unit_name=EUpdateStrategy_Storage
/// @endcode
enum class EUpdateStrategy_Storage : uint16_t
{
    /// @brief keepExisting
    kKeepExisting = 0,
    /// @brief delete
    kDelete = 1,
    /// @brief Undefined
    kUndefine = 255,
};
/// @brief Enum values correspond to Table A.22: PersistencyElementLevelUpdateStrategyEnum
/// @code{.isoft}
/// @unit_name=EUpdateStrategy_Element
/// @endcode
enum class EUpdateStrategy_Element : uint16_t
{
    /// @brief overwrite
    kOverwrite = 0,
    /// @brief keepExisting
    kKeepExisting = 1,
    /// @brief delete
    kDelete = 2,
    /// @brief Undefined
    kUndefine = 255,
};
/// @brief Enum values correspond to Table A.30: PersistencyRedundancyEnum
/// @code{.isoft}
/// @unit_name=EReddStrategy
/// @endcode
enum class EReddStrategy : uint16_t
{
    kStorage = 0,  // For entire Storage
    kNone    = 1,  // No redundancy
    kElement = 2,  // For each KV-Element/File
};
#pragma pack(push, 1)
/// @brief M-N redundancy attributes
/// @code{.isoft}
/// @unit_name=PReddDataMofN
/// @endcode
class PReddDataMofN final
{
public:
    /// @brief MofN.M
    uint16_t m{0U};
    /// @brief MofN.N
    uint16_t n{0U};
    /// @brief Both values being 0 indicate MofN redundancy rule is not used
    /// @return
    bool IsValid() const noexcept { return (m > 0U) && (n > 0U); }
    /// @brief Get denominator N in M/N redundancy configuration
    /// @return
    uint32_t GetReddCountN() const noexcept
    {
        if (IsValid()) {
            return static_cast< uint32_t >(n);
        }
        return 1U;
    }
};
/// @brief CRC redundancy attributes
/// @code{.isoft}
/// @unit_name=PReddData_CheckSum
/// @endcode
class PReddData_CheckSum
{
public:
    /// @brief Algorithm identifier
    ara::core::String stAlgorithmFamily;
    /// @brief
    uint16_t nLength{0U};
};
/// @brief CRC redundancy attributes
/// @code{.isoft}
/// @unit_name=PReddDataCrc
/// @endcode
typedef PReddData_CheckSum PReddDataCrc;
/// @brief HASH redundancy attributes
/// @code{.isoft}
/// @unit_name=PReddDataHash
/// @endcode
class PReddDataHash : public PReddData_CheckSum
{
public:
    /// @brief Initialization vector length
    uint16_t nInitVecLen{0U};
};
/// @brief Redundancy configuration
/// @code{.isoft}
/// @unit_name=PReddConfigData
/// @endcode
class PReddConfigData final
{
public:
    /// @brief Redundancy type
    EReddType eReddType{EReddType::kNone};
    /// @brief MofN
    PReddDataMofN reddMofN{};
    /// @brief CRC
    PReddDataCrc reddCrc{};
    /// @brief CRC
    PReddDataHash reddHash{};

public:
    /// @brief
    void Reset() noexcept
    {
        eReddType  = EReddType::kNone;
        reddMofN.m = 0U;
        reddMofN.n = 0U;
        reddCrc.stAlgorithmFamily.clear();
        reddCrc.nLength = 0U;
        reddHash.stAlgorithmFamily.clear();
        reddHash.nLength     = 0U;
        reddHash.nInitVecLen = 0U;
    }
    /// @brief
    /// @return
    bool IsValidRedd() const noexcept { return eReddType != EReddType::kNone; }
    /// @brief Check if redundancy strategy is MofN
    /// @return
    bool IsReddMofN() const noexcept;
    /// @brief Check if redundancy strategy is CRC or Hash (redundancy types requiring verification)
    /// @return
    bool IsReddCheck() const noexcept;
    /// @brief Set CRC redundancy
    /// @param reddData Json configuration read from manifest document
    void SetReddConfig_Crc(ara::per::manifest::MConfigData_ReddCrc const &reddData) noexcept;
    /// @brief Set M/N redundancy
    /// @param reddData Json configuration read from manifest document
    void SetReddConfig_MofN(ara::per::manifest::MConfigData_ReddMofN const &reddData) noexcept;
    /// @brief Set Hash redundancy
    /// @param reddData Json configuration read from manifest document
    void SetReddConfig_Hash(ara::per::manifest::MConfigData_ReddHash const &reddData) noexcept;
};
/// @brief Enum values correspond to A.4: CryptoKeySlotUsageEnum
/// @code{.isoft}
/// @unit_name=ECryptoKeySlotUsage
/// @endcode
enum class ECryptoKeySlotUsage : uint32_t
{
    /// @brief Verify
    kVerification = 0,
    /// @brief Encrypt
    kEncryption = 1,
    /// @brief Judgment of whether Crypto configuration exists
    kHaveCrypto = 255,
};
/// @brief Encryption configuration
/// @code{.isoft}
/// @unit_name=PConfigData_Crypto
/// @endcode
class PConfigData_Crypto final
{
public:
    /// @brief Encryption algorithm identifier name
    ara::core::String stCryptoAlgorithm{""};
    /// @brief Encryption configuration: Key slot name used
    ara::core::String stKeySlotName{""};
    /// @brief Encryption configuration: Encryption usage scope
    ECryptoKeySlotUsage eKeySlotUsage{ECryptoKeySlotUsage::kVerification};
    /// @brief Encryption configuration: Verification Hash string
    ara::core::String stVerfHash{""};
};
#pragma pack(pop)
//********************************/
/// @brief Storage configuration class
/// @code{.isoft}
/// @unit_name=PConfigMuster
/// @endcode
class PConfigMuster
{
public:
    /// @brief Check if redundancy strategy is CRC or Hash (redundancy types requiring verification)
    /// @return
    static bool IsReddCheckType(EReddType eReddType) noexcept;
    // Properties likely to be set in configuration file
private:
    /// @brief Whether configuration is effective
    bool bConfigValid_{false};
    /// @brief Identifier name
    ara::core::String stStorageIns_{};
    /// @brief Storage name
    ara::core::String stStorageName_{};
    /// @brief Working path
    ara::core::String stWorkPath_{};
    /// @brief APP version number: String
    ara::core::String stVersionApp_{};
    /// @brief Per version number: String
    ara::core::String stVersionPer_{};
    /// @brief Minimum occupied space
    uint64_t nSpaceAmountMin_{static_cast< uint64_t >(EDefaultValue::kDefSpaceAmountMin)};
    /// @brief Maximum occupied space
    uint64_t nSpaceAmountMax_{static_cast< uint64_t >(EDefaultValue::kDefSpaceAmountMax)};
    /// @brief Update strategy: "kKeepExisting", "kOverWrite", "kDelete"
    ara::core::String stUpdateStrategy_{};
    /// @brief Redundancy strategy: "none", "redundant", "redundantPerElement"
    EReddStrategy eReddStrategy_{EReddStrategy::kNone};
    /// @brief Encryption configuration
    PConfigData_Crypto cryptoData_{};

private:
    /// @brief Global redundancy configuration
    PReddConfigData globleReddConfig_{};
    /// @brief Element-level encryption configuration
    ara::core::Map< ara::core::String, PConfigData_Crypto > mapElementCrypto_{};

public:
    /// @brief
    /// @return
    inline bool IsValid() const noexcept { return bConfigValid_; }
    /// @brief
    /// @param data
    inline void SetConfigValid(bool const data) noexcept { bConfigValid_ = data; }
    /// @brief Get Storage identifier
    /// @return
    inline ara::core::String const &GetStorageIns() const noexcept { return stStorageIns_; }
    /// @brief Set Storage identifier
    /// @param data
    inline void SetStorageIns(ara::core::String const &data) noexcept { stStorageIns_ = data; }
    /// @brief Get Storage name
    /// @return
    inline ara::core::String const &GetStorageName() const noexcept { return stStorageName_; }
    /// @brief Set Storage name
    /// @param data
    inline void SetStorageName(ara::core::String const &data) noexcept { stStorageName_ = data; }
    /// @brief
    /// @return
    inline ara::core::String const &GetWorkPath() const noexcept { return stWorkPath_; }
    /// @brief
    /// @param data
    inline void SetWorkPath(ara::core::String const &data) noexcept { stWorkPath_ = data; }
    /// @brief
    /// @return
    inline ara::core::String const &GetVersionApp() const noexcept { return stVersionApp_; }
    /// @brief
    /// @param data
    inline void SetVersionApp(ara::core::String const &data) noexcept { stVersionApp_ = data; }
    /// @brief Get version number of persistent data
    /// @return
    inline ara::core::String const &GetVersionPer() const noexcept { return stVersionPer_; }
    /// @brief Set version number of persistent data
    /// @param data
    inline void SetVersionPer(ara::core::String const &data) noexcept { stVersionPer_ = data; }
    /// @brief
    /// @return
    inline uint64_t GetSpaceAmountMin() const noexcept { return nSpaceAmountMin_; }
    /// @brief
    /// @param data
    inline void SetSpaceAmountMin(uint64_t const &data) noexcept { nSpaceAmountMin_ = data; }
    /// @brief
    /// @return
    inline uint64_t GetSpaceAmountMax() const noexcept { return nSpaceAmountMax_; }
    /// @brief
    /// @param data
    inline void SetSpaceAmountMax(uint64_t const &data) noexcept { nSpaceAmountMax_ = data; }
    /// @brief
    /// @return
    inline ara::core::String const &GetUpdateStrategy() const noexcept { return stUpdateStrategy_; }
    /// @brief
    /// @param data
    inline void SetUpdateStrategy(ara::core::String const &data) noexcept { stUpdateStrategy_ = data; }
    /// @brief
    /// @return
    inline EReddStrategy const &GetReddStrategy() const noexcept { return eReddStrategy_; }
    /// @brief
    /// @param data
    inline void SetReddStrategy(EReddStrategy const &data) noexcept { eReddStrategy_ = data; }
    /// @brief
    /// @return
    inline PConfigData_Crypto const &GetCryptoConfig() const noexcept { return cryptoData_; }
    /// @brief Get redundancy configuration information: CRC
    /// @return
    inline PReddDataCrc const &GetReddData_Crc() const noexcept { return globleReddConfig_.reddCrc; }
    /// @brief Get redundancy configuration information: Hash
    /// @return
    inline PReddDataHash const &GetReddData_Hash() const noexcept { return globleReddConfig_.reddHash; }

protected:
    /// @brief
    /// @return
    inline PReddConfigData const *_GetGlobleReddConfig() const noexcept { return &globleReddConfig_; }
    /// @brief
    /// @param stFileName
    /// @param cryptoData
    inline void _AddElementCrypto(ara::core::String const &stFileName, PConfigData_Crypto &&cryptoData) noexcept
    {
        mapElementCrypto_[stFileName] = std::move(cryptoData);
    }

public:
    /// @brief
    /// @throws
    PConfigMuster() = default;
    /// @brief
    virtual ~PConfigMuster() = default;
    /// @brief
    /// @param a
    /// @throws
    PConfigMuster(PConfigMuster const &a) = default;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    PConfigMuster &operator=(PConfigMuster const &a) = default;
    /// @brief
    /// @param a
    PConfigMuster(PConfigMuster &&a) noexcept;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    PConfigMuster &operator=(PConfigMuster &&a) noexcept;

public:
    /// @brief Set all Config
    /// @param config
    /// @return
    bool SetConfig(PConfigMuster const &config) noexcept;
    /// @brief Set redundancy configuration
    /// @param stReddStrategy
    /// @param reddConfig
    void SetReddConfig(ara::core::StringView const &stReddStrategy,
                       manifest::MConfigData_Redundancy const &reddConfig) noexcept;
    /// @brief Set redundancy configuration
    /// @param eReddStrategy
    /// @param reddConfig
    void SetReddConfig(EReddStrategy const eReddStrategy, PReddConfigData const &reddConfig) noexcept;
    /// @brief Whether redundancy switch is turned on
    /// @return
    bool IsEnableRedd() const noexcept;
    /// @brief Check if redundancy strategy is MofN
    /// @return
    bool IsReddMofN() const noexcept;
    /// @brief Check if redundancy strategy is CRC or Hash (redundancy types requiring verification)
    /// @return
    bool IsReddCheckType() const noexcept;
    /// @brief Get M in M/N configuration
    /// @return
    uint32_t GetReddCountM() const noexcept;
    /// @brief Get N in M/N configuration
    /// @return
    uint32_t GetReddCountN() const noexcept;
    /// @brief Get redundancy type
    /// @return
    EReddType GetReddType() const noexcept;
    /// @brief Assemble configuration file through Json configuration
    /// @param stVersionApp
    /// @param pStorageConfig
    /// @return
    bool AssembleConfig(ara::core::String const &stVersionApp,
                        manifest::MConfig_StorageBase const *const pStorageConfig) noexcept;
    /// @brief Check if encryption operation is supported: Storage
    /// @param eKeySlotUsage Determine key usage
    /// @return
    bool IsHaveCrypto_Storage(ECryptoKeySlotUsage eKeySlotUsage) const noexcept;
    /// @brief Check if encryption operation is supported: Element encryption + verification
    /// @param stElement
    /// @return
    PConfigData_Crypto const *IsHaveCrypto_Element(ara::core::String const &stElement) const noexcept;
    /// @brief Check if encryption operation is supported: Element encryption
    /// @param stElement
    /// @return
    PConfigData_Crypto const *IsHaveCrypto_ElementEncryption(ara::core::String const &stElement) const noexcept;
    /// @brief Set encryption
    /// @param stCryptoAlgorithm
    /// @param stKeySlotName
    /// @param stKeySlotUsage
    /// @param stVerfHash
    /// @return
    bool SetCryptoConfig(ara::core::String const &stCryptoAlgorithm,
                         ara::core::String const &stKeySlotName,
                         ara::core::String const &stKeySlotUsage,
                         ara::core::String const &stVerfHash) noexcept;
};
//********************************/
/// @brief Configuration used by FS
/// @code{.isoft}
/// @unit_name=PConfigMuster_File
/// @endcode
class PConfigMuster_File final : public PConfigMuster
{
private:
    /// @brief
    ara::core::Map< ara::core::String, PReddConfigData >
        mapFileRedd_{};  // Special redundancy configuration for each file
    uint32_t nMaxFileCount_{-1U};

public:
    /// @brief
    /// @throws
    PConfigMuster_File() = default;
    /// @brief
    ~PConfigMuster_File() noexcept final = default;
    /// @brief
    /// @param a
    /// @throws
    PConfigMuster_File(PConfigMuster_File const &a) = default;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    PConfigMuster_File &operator=(PConfigMuster_File const &a) = default;
    /// @brief
    /// @param a
    PConfigMuster_File(PConfigMuster_File &&a) noexcept = delete;
    /// @brief
    /// @param a
    /// @return
    PConfigMuster_File &operator=(PConfigMuster_File &&a) noexcept = delete;

public:
    /// @brief Assemble configuration file needed by FileStorage through Json configuration
    /// @param portIns Port identifier
    /// @return Success or failure
    bool AssembleFileConfig(ara::core::InstanceSpecifier const &portIns) noexcept;
    /// @brief Assemble configuration file needed by FileStorage through Json configuration
    /// @param manifrestRead Configuration reader
    /// @param configFile Configuration in configuration file
    /// @return Success or failure
    bool AssembleFileConfig(ara::per::manifest::PManifestReader_Per const &manifrestRead,
                            ara::per::manifest::MConfig_StorageFile const &configFile) noexcept;
    /// @brief Find MofN redundancy configuration for specific file
    /// @param stFileName
    /// @return
    PReddDataMofN FindFileReddMofN(ara::core::StringView const &stFileName) const noexcept;
    /// @brief Get configured maximum file count
    /// @return
    uint32_t GetMaxFileCount() const noexcept { return nMaxFileCount_; }
    /// @brief Configure maximum file count
    /// @param nMaxFileCount
    void GetMaxFileCount(uint32_t nMaxFileCount) noexcept { nMaxFileCount_ = nMaxFileCount; }
};
/// @brief Configuration collection used by PKvSystem
/// PConfigMusterKv only saves attributes needed by PKvSystem, ignoring attributes like update strategy, initialization files, etc.
/// @code{.isoft}
/// @unit_name=PConfigMuster_Kv
/// @endcode
class PConfigMuster_Kv final : public PConfigMuster
{
    // Properties unlikely to be set in configuration file
private:
    /// @brief Initial Cache page count
    uint32_t nMaxCacheCount_{static_cast< uint32_t >(EDefaultValue::kDefMaxCacheCount)};
    /// @brief Initial Page count
    uint32_t nInitPageCount_{static_cast< uint32_t >(EDefaultValue::kDefInitPageCount)};
    /// @brief Single page length
    uint32_t nPageInitLen_{static_cast< uint32_t >(EDefaultValue::kDefPageLen)};
    /// @brief Free list classification count
    uint32_t nFreeListCapacity_{static_cast< uint32_t >(EDefaultValue::kDefFreeListCapacity)};
    /// @brief Group size, maximum number of pages per group
    uint32_t nMaxPageInGroup_{static_cast< uint32_t >(EDefaultValue::kDefMaxPageInGroup)};
    /// @brief Initial Hash bucket count
    uint32_t nHashInitCount_{static_cast< uint32_t >(EDefaultValue::kDefInitHashCount)};
    /// @brief Maximum Hash bucket count
    uint32_t nHashMaxCapacity_{static_cast< uint32_t >(EDefaultValue::kDefMaxHashCapacity)};
    /// @brief Whether to automatically save Walog library
    uint32_t nAutoSaveWalog_{static_cast< uint32_t >(EDefaultValue::kDefAutoSaveWalog)};
    /// @brief Maximum KV data length
    uint32_t nMaxKvDataLen_{static_cast< uint32_t >(EDefaultValue::kDefMaxKvDataLen)};
    /// @brief Initialize KV library UpdateFile file name
    ara::core::String stInitUpdateFile_{};

private:
    /// @brief Hash type used by Hash page // Configuration item not used as of 2021-10-22
    uint32_t nHashType_{0U};
    /// @brief Compression type // Configuration item not used as of 2021-10-22
    uint32_t nCompressType_{0U};

public:
    /// @brief
    /// @return
    inline uint32_t GetMaxCacheCount() const noexcept { return nMaxCacheCount_; }
    /// @brief
    /// @param data
    inline void SetMaxCacheCount(uint32_t const &data) noexcept { nMaxCacheCount_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetInitPageCount() const noexcept { return nInitPageCount_; }
    /// @brief
    /// @param data
    inline void SetInitPageCount(uint32_t const &data) noexcept { nInitPageCount_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetPageInitLen() const noexcept { return nPageInitLen_; }
    /// @brief
    /// @param data
    inline void SetPageInitLen(uint32_t const &data) noexcept { nPageInitLen_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetFreeListCapacity() const noexcept { return nFreeListCapacity_; }
    /// @brief
    /// @param data
    inline void SetFreeListCapacity(uint32_t const &data) noexcept { nFreeListCapacity_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetMaxPageInGroup() const noexcept { return nMaxPageInGroup_; }
    /// @brief
    /// @param data
    inline void SetMaxPageInGroup(uint32_t const &data) noexcept { nMaxPageInGroup_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetHashInitCount() const noexcept { return nHashInitCount_; }
    /// @brief
    /// @param data
    inline void SetHashInitCount(uint32_t const &data) noexcept { nHashInitCount_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetHashMaxCapacity() const noexcept { return nHashMaxCapacity_; }
    /// @brief
    /// @param data
    inline void SetHashMaxCapacity(uint32_t const &data) noexcept { nHashMaxCapacity_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetAutoSaveWalog() const noexcept { return nAutoSaveWalog_; }
    /// @brief
    /// @param data
    inline void SetAutoSaveWalog(uint32_t const &data) noexcept { nAutoSaveWalog_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetMaxKvDataLen() const noexcept { return nMaxKvDataLen_; }
    /// @brief
    /// @param data
    inline void SetMaxKvDataLen(uint32_t const &data) noexcept { nMaxKvDataLen_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetHashType() const noexcept { return nHashType_; }
    /// @brief
    /// @param data
    inline void SetHashType(uint32_t const &data) noexcept { nHashType_ = data; }
    /// @brief
    /// @return
    inline uint32_t GetCompressType() const noexcept { return nCompressType_; }
    /// @brief
    /// @param data
    inline void SetCompressType(uint32_t const &data) noexcept { nCompressType_ = data; }
    /// @brief Return initialized UpdateFile file name
    inline ara::core::StringView GetInitUpdateFile() noexcept { return T_StringView(stInitUpdateFile_); }
    /// @brief Set initialized UpdateFile file name
    /// @param data
    inline void SetInitUpdateFile(ara::core::String const &data) noexcept { stInitUpdateFile_ = data; }

public:
    /// @brief
    /// @throws
    PConfigMuster_Kv() = default;
    /// @brief
    /// @throws
    ~PConfigMuster_Kv() noexcept final = default;
    /// @brief
    /// @param a
    /// @throws
    PConfigMuster_Kv(PConfigMuster_Kv const &a) = default;
    /// @brief
    /// @param a
    /// @return
    PConfigMuster_Kv &operator=(PConfigMuster_Kv const &a) noexcept = default;
    /// @brief
    /// @param a
    PConfigMuster_Kv(PConfigMuster_Kv &&a) noexcept;
    /// @brief
    /// @param a
    /// @return
    PConfigMuster_Kv &operator=(PConfigMuster_Kv &&a) noexcept;

public:
    /// @brief Assemble KV configuration file through Json configuration
    /// @param portIns port identifier
    /// @return Success or failure
    bool AssembleKvConfig(ara::core::InstanceSpecifier const &portIns) noexcept;
    /// @brief Assemble KV configuration file through Json configuration
    /// @param stVersionApp APP version number
    /// @param configKv Configuration in configuration file
    /// @return Success or failure
    bool AssembleKvConfig(ara::core::String const &stVersionApp, manifest::MConfig_StorageKv const &configKv) noexcept;

protected:
    /// @brief
    /// @param a
    void _MoveConstructor(PConfigMuster_Kv &&a) noexcept;
};
//********************************/
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @code{.isoft}
/// @unit_name=TransUpdate_Storage
/// @endcode
/// @param stData String format enum
/// @return Corresponding enum value
EUpdateStrategy_Storage TransUpdate_Storage(ara::core::String const &stData) noexcept;
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @code{.isoft}
/// @unit_name=TransUpdate_Element
/// @endcode
/// @param stData String format enum
/// @return Corresponding enum value
EUpdateStrategy_Element TransUpdate_Element(ara::core::String const &stData) noexcept;
/// @brief Convert string to enum: EStorageUpdateStrategy
/// @code{.isoft}
/// @unit_name=TransKeySlotUsage
/// @interface_level=unit
/// @endcode
/// @param stData String format enum
/// @return Corresponding enum value
ECryptoKeySlotUsage TransKeySlotUsage(ara::core::String const &stData) noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
