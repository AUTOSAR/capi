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
/// @file       page_common_struct.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Centralized definition of KV storage related data structures
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-19  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2021-07-13  <td>1.0.0    <td>hanjingjing      <td>Modify valid PageID starting from 1, 0 invalid
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PHKV_PAGE_COMMON_STRUCT_H_
#define ARA_PER_PHKV_PAGE_COMMON_STRUCT_H_

#include <cstdint>

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {

///****************/
#pragma pack(push, 1)
/// @brief Important note: Important reminder
// 2021-05-25 Confirmed support for longest page is 64K, max support 4G pages
//********************************/
/// @brief Enum: Storage type
/// @code{.isoft}
/// @unit_name=EBlockType
/// @endcode
enum class EBlockType : uint8_t
{
    kBaseEmpty = 0,       // Empty
    kPageGroup = 1 << 0,  // Page grouping information
    kPageBoss  = 1 << 1,  // Page management data storage area
    kKeyHash   = 1 << 2,  // Hash mapping table
    kKvStore   = 1 << 3,  // KV node information
    kValue     = 1 << 4,  // Value storage zone
    kExt       = 1 << 5,  // Extended global data
    kWalog     = 1 << 6,  // PageID to file offset mapping table inside walog file
    kElse      = 1 << 7,
    kBlend     = 0xFF,
};
/// @brief Enum: Storage range
/// @code{.isoft}
/// @unit_name=EStoreRange
/// @endcode
enum class EStoreRange : uint8_t
{
    kSingle = 0,  // Single page storage
    kMulti  = 1,  // Multi-page storage
};
/// @brief Invalid Page number
/// @code{.isoft}
/// @unit_name=kInvalidPageID
/// @endcode
constexpr uint32_t kInvalidPageID{0U};
/// @brief 2021-05-31 Default value enumeration
/// @code{.isoft}
/// @unit_name=EDefaultValue
/// @endcode
enum class EDefaultValue : uint32_t
{
    /// @brief Default initialized Group count
    kInitGroupCount = 1U,
    /// @brief 16 free lists
    kInitFreeListCount = 16U,
    /// @brief 2021-05-13 Set max page size to 64K
    kInitMaxPageLen = 64 * 1024U,
    /// @brief Maximum Hash bucket count
    kDefMaxHashCapacity = 1000U,
    /// @brief Group size, maximum number of pages per group
    kDefMaxPageInGroup = 1000U,
    /// @brief Default mixed page number
    kDefStartPageID = 1U,
    /// @brief Default page size
    kDefPageLen = 4U * 1024U,
    /// @brief Default minimum space occupation
    kDefSpaceAmountMin = 4U * 1024U,
    /// @brief Default maximum space occupation
    kDefSpaceAmountMax = 64U * 1024U * 1024U,
    /// @brief Default free list classification count: 21 groups, exactly 5% per tier
    kDefFreeListCapacity = 21U,
    /// @brief Default maximum cache page count
    kDefMaxCacheCount = 32U,
    /// @brief Default initialized Page count
    kDefInitPageCount = 1U,
    /// @brief Default initialized Hash bucket count
    kDefInitHashCount = 7U,
    /// @brief Default whether to automatically save Walog library:
    /// Non-zero will automatically save currently changed cache pages to Walog file upon System exit and set SyncModel to 1 on next open.
    kDefAutoSaveWalog = 0U,
    /// @brief Default maximum KV data length
    kDefMaxKvDataLen = 64U,
    /// @brief Threshold for maximum length of KV data in single-page KV storage
    kLimitMaxKvDataLen = kDefPageLen / 2U,
    /// @brief 16 free list tiers
    kMaxFreeListCount = 28U,
    /// @brief Maximum storage capacity of version string in Ext page
    kMaxExtVersionLenLimit = 256U,
};
//****************/
/// @brief Version number
/// @code{.isoft}
/// @unit_name=PPerVersion
/// @endcode
struct PPerVersion final
{
public:
    /// @brief Major version number
    uint16_t sVersionMajor{0U};
    /// @brief Minor version number
    uint16_t sVersionMinor{0U};
    /// @brief Patch version number
    uint16_t sVersionPatch{0U};
    /// @brief Other information, 2023-08-24 no longer uses this item for version comparison, for reference only
    uint16_t sVersionElse{0U};
};
/// @brief Compare version numbers
/// @code{.isoft}
/// @unit_name=CompareVersion
/// @endcode
/// @param versionA
/// @param versionB
/// @return

inline int32_t CompareVersion(PPerVersion const& versionA, PPerVersion const& versionB) noexcept
{
    if (versionA.sVersionMajor != versionB.sVersionMajor) {
        return static_cast< int32_t >(versionA.sVersionMajor) - static_cast< int32_t >(versionB.sVersionMajor);
    }
    if (versionA.sVersionMinor != versionB.sVersionMinor) {
        return static_cast< int32_t >(versionA.sVersionMinor) - static_cast< int32_t >(versionB.sVersionMinor);
    }
    if (versionA.sVersionPatch != versionB.sVersionPatch) {
        return static_cast< int32_t >(versionA.sVersionPatch) - static_cast< int32_t >(versionB.sVersionPatch);
    }
    // 2023-08-24 No longer comparing Else version information
    if (versionA.sVersionElse != versionB.sVersionElse) {
        return static_cast< int32_t >(versionA.sVersionElse) - static_cast< int32_t >(versionB.sVersionElse);
    }
    return 0;
}
/// @brief Convert PPerVersion to string format
/// @code{.isoft}
/// @unit_name=Version_PerToString
/// @endcode
/// @param perVersion
/// @return PPerVersion{1,0,0,0} converts to "1.0.0"
inline ara::core::String Version_PerToString(ara::per::isoftkv::PPerVersion const& perVersion) noexcept
{
    ara::core::String stVersion;
    stVersion.reserve(kInt32_32U);
    stVersion += std::to_string(static_cast< uint32_t >(perVersion.sVersionMajor)).c_str();
    stVersion += ".";
    stVersion += std::to_string(static_cast< uint32_t >(perVersion.sVersionMinor)).c_str();
    stVersion += ".";
    stVersion += std::to_string(static_cast< uint32_t >(perVersion.sVersionPatch)).c_str();
    return stVersion;
}
/// @brief
/// @code{.isoft}
/// @unit_name=G_TransVersionString
/// @endcode
/// Similar to "1.0.0-alpha+001 1.0.0+20130313144700 1.0.0-beta+exp.sha.5114f85" such string converted to one PPerVersion structure + one description string
/// @param stVersionString
/// @param perVersion
/// @return "1.0.0-alpha+001" => "1.0.0" + ""-alpha+001"
inline bool G_TransVersionString(ara::core::StringView const& stVersionString,
                                 ara::per::isoftkv::PPerVersion& perVersion,
                                 ara::core::String* pstVersionName)
{
    ara::core::StringView::size_type nPosFind1{stVersionString.find_first_of('.')};
    if (nPosFind1 == ara::core::StringView::npos) {
        return false;
    }
    ara::core::StringView::size_type nPosFind2{stVersionString.find_first_of('.', nPosFind1 + 1)};
    if (nPosFind2 == ara::core::StringView::npos) {
        return false;
    }
    ara::core::StringView::size_type nPosFind3{stVersionString.find_first_not_of("0123456789", nPosFind2 + 1)};
    if (nPosFind3 == ara::core::StringView::npos) {
        nPosFind3 = stVersionString.size();
    }
    perVersion.sVersionMajor = ara::per::isoftkv::T_stoi< uint16_t >(stVersionString.data(), nPosFind1);
    perVersion.sVersionMinor
        = ara::per::isoftkv::T_stoi< uint16_t >(stVersionString.data() + nPosFind1 + 1, nPosFind2 - nPosFind1 - 1);
    perVersion.sVersionPatch
        = ara::per::isoftkv::T_stoi< uint16_t >(stVersionString.data() + nPosFind2 + 1, nPosFind3 - nPosFind2 - 1);
    perVersion.sVersionElse = 0;
    if (nullptr != pstVersionName) {
        if (nPosFind3 != stVersionString.size()) {
            *pstVersionName = stVersionString.substr(nPosFind3);
        }
    }
    return true;
}
/// @brief Convert version information type
/// @code{.isoft}
/// @unit_name=Version_StringToPer
/// @endcode
/// @param stVersionString
/// @return "1.0.0.DemoVersion" converts to PPerVersion{1,0,0,0}
inline PPerVersion Version_StringToPer(ara::core::StringView const& stVersionString) noexcept
{
    PPerVersion perVersion;
    if (false == G_TransVersionString(stVersionString, perVersion, nullptr)) {
        return PPerVersion{};
    }
    return perVersion;
}
/// @brief Check if version number is 0
/// @code{.isoft}
/// @unit_name=IsVersionZero
/// @endcode
/// @param version
/// @return
inline bool IsVersionZero(PPerVersion const& version) noexcept
{
    if (0 != version.sVersionMajor) {
        return false;
    }
    if (0 != version.sVersionMinor) {
        return false;
    }
    if (0 != version.sVersionPatch) {
        return false;
    }
    if (0 != version.sVersionElse) {
        return false;
    }
    return true;
}
/// @brief PageID starts counting from 1, sort number needs to start from 0
/// @code{.isoft}
/// @unit_name=TransPage_IDtoIndex
/// @endcode
/// @param nPageID
/// @return
inline uint32_t TransPage_IDtoIndex(uint32_t const nPageID) noexcept
{
    PER_Assert(nPageID > 0);
    return nPageID - 1;
}
/// @brief PageID starts counting from 1, sort number needs to start from 0
/// @code{.isoft}
/// @unit_name=TransPage_IndexToID
/// @endcode
/// @param nPageIndex
/// @return
inline uint32_t TransPage_IndexToID(uint32_t const nPageIndex) noexcept { return nPageIndex + 1; }
//********************************/
///@brief Page page header management information : Ctrl writes from top to bottom, data writes from bottom to top (2021-05-13 tentative byte length: 24)
/// @code{.isoft}
/// @unit_name=PPageHeadData
/// @endcode
struct PPageHeadData final
{
public:
    /// @brief Page type identifier: Value range is {"Raw", "Group", "Boss", "Key", "Value", "Redd", "File", "Blend";}
    /// In mixed type, stores the first type here
    char8_t cMagic[kInt_8]{"Page"};
    /// @brief Page number: 4-byte full information number
    uint32_t nPageID{0U};
    /// @brief Page type: Management page, independent data page, cross-page data page, etc., generally page 0 is management page
    uint8_t cPageType{0U};
    /// @brief Reserved field
    uint8_t cReserve{0U};
    /// @brief Page length: Max is 64K (valid length = m_nHeadLen + m_nTailLen)
    uint16_t sPageLen{0U};
    /// @brief Free length
    uint16_t sFreeLen{0U};
    /// @brief Number of data blocks managed by current page: Local
    uint16_t sCountBlock{0U};
    /// @brief Check code of this structure or CRC check code of this page: Need to determine whether to use 32 or 16-bit CRC
    uint32_t nCRC{0U};
};
/// @brief Identification data written at end of file after Walog sync completes (2022-04-22 structure total 64 bytes)
/// @code{.isoft}
/// @unit_name=PPageTailData
/// @endcode
struct PPageTailData final
{
public:
    /// @brief Total number of operated pages
    int32_t nPageSaveCount{0U};
    /// @brief Operation timestamp //2023-03-13 Type change: uint32_t=>uint64_t
    uint64_t tmOptTime{0U};
    /// @brief Sync strategy during Open: 0, automatic; non-zero, manual;
    uint32_t nSyncModel{0U};
    /// @brief CRC check code of PPageTailData structure
    uint32_t nTailCrc{0U};
    /// @brief Magic number
    char8_t chMagicWord[kInt_0x10]{"ph_kv_tail_flag"};
    /// @brief Reserved word
    uint32_t nReserve[kInt_8 - 1]{0U, 0U, 0U, 0U, 0U, 0U, 0U};
};
/// @brief Check if magic number matches this structure's requirement
/// @code{.isoft}
/// @unit_name=IsPhTail
/// @endcode
/// @param tailData
/// @return
inline bool IsPhTail(PPageTailData const& tailData) noexcept
{
    return 0
           == strncmp(static_cast< char8_t const* >(tailData.chMagicWord), "ph_kv_tail_flag",
                      sizeof(tailData.chMagicWord));
}
/// @brief
/// @code{.isoft}
/// @unit_name=PBlockHead
/// @endcode
struct PBlockHead final  // NOLINT
{                        // BlockHead: Length 8 bytes
public:
    /// @brief Data offset position: Relative to page start address
    uint16_t sOffset{0U};
    /// @brief Data length <= _nLenInit
    uint16_t sLen{0U};
    /// @brief Data block type: EBlockType
    uint8_t cBlockType{0U};
    /// @brief Unique ID of Block within Page data area: Increases sequentially starting from 1
    uint16_t sBlockID : 13;
    /// @brief 0: Single data; 1: Cross-page data EStoreRange
    uint16_t cSpread : 3;
    /// @brief Data type: Mainly used when cBlockType == EBlockType::kValue
    uint8_t cDataType{0U};
};
/// @brief
/// @code{.isoft}
/// @unit_name=PBlockIndex
/// @endcode
struct PBlockIndex final
{  // Block index data structure: Length 6 bytes
public:
    /// @brief Page.ID to which the Block belongs
    uint32_t nPageID{0U};
    /// @brief Block sequence number within Page data area
    uint16_t sBlockID{0U};
};
//********************************/    // Structure: Data block within this Page
/// @brief Structure: Cross-page data block
/// @code{.isoft}
/// @unit_name=PBlockSpread
/// @endcode
struct PBlockSpread final
{  // Cross-page data placed at data head: 24 bytes, presence controlled by PBlockHead.cSpread
public:
    /// @brief Data block ID, unique identifier for data spanning multiple blocks
    uint32_t nSpreadID{0U};
    /// @brief Total data length
    uint32_t nTotalLen{0U};
    /// @brief Total number of data blocks
    uint16_t nZoneTotal{0U};
    /// @brief Data block sequence number
    uint16_t nZoneIndex{0U};
    /// @brief Previous data block information
    PBlockIndex prevBlock;
    /// @brief Next data block information
    PBlockIndex nextBlock;
};
//********************************/
/// @brief Page management: Grouping
/// @code{.isoft}
/// @unit_name=PBlockElementGroup
/// @endcode
struct PBlockElementGroup final  // NOLINT
{                                // Page management: Group information, 4 bytes  // Array index is group number
public:
    /// @brief Group number of PageBoss page used for page management
    uint16_t sBossPageIndex{0U};
    /// @brief Valid page count: Max 2047
    uint16_t sFreePageCount : 11;
    /// @brief Max index of free list in Boss: Max free list index is 31
    uint16_t sMaxFreeIndex : 5;
};
/// @brief Initialize PBlockElementGroup structure
/// @code{.isoft}
/// @unit_name=InitElement
/// @endcode
/// @param elementData
/// @param nBossPageIndex
/// @param nFreePageCount
/// @param nMaxFreeIndex
inline void InitElement(PBlockElementGroup& elementData,
                        uint16_t const nBossPageIndex,
                        uint16_t const nFreePageCount,
                        uint16_t const nMaxFreeIndex) noexcept
{
    elementData.sBossPageIndex = nBossPageIndex;
    elementData.sFreePageCount = static_cast< uint16_t >(ara::per::isoftkv::T_Mod(nFreePageCount, ara::per::kInt_2E11));
    elementData.sMaxFreeIndex  = static_cast< uint16_t >(ara::per::isoftkv::T_Mod(nMaxFreeIndex, ara::per::kInt_2E5));
}
/// @brief PageGroup control structure
/// @code{.isoft}
/// @unit_name=PBlockHeadPageGroup
/// @endcode
struct PBlockHeadPageGroup final
{  // 2021-05-26    Tentative length 48 + 4 * X
public:
    /// @brief App version number
    PPerVersion versionSystem;
    /// @brief Persistence module version number
    PPerVersion versionPer;
    /// @brief Minimum storage space
    uint32_t nSpaceAmountMin{static_cast< uint32_t >(EDefaultValue::kDefSpaceAmountMin)};
    /// @brief Maximum storage space
    uint32_t nSpaceAmountMax{static_cast< uint32_t >(EDefaultValue::kDefSpaceAmountMax)};
    /// @brief Position of Key-Hash page
    uint32_t nHashPageID{0U};
    /// @brief Capacity per group after Group division
    uint16_t sGroupCapacity{static_cast< uint16_t >(EDefaultValue::kDefMaxPageInGroup)};
    /// @brief Length per page
    uint16_t sPageInitLen{static_cast< uint16_t >(EDefaultValue::kDefPageLen)};
    /// @brief Total page count
    uint32_t nPageTotal{0U};
    /// @brief Group count: Actual number of array elements
    uint16_t sElementCount{0U};
    /// @brief KvSystem version number
    PPerVersion versionKvSystem;
    /// @brief Reserved field
    uint8_t cReserve[kInt_2]{0U};
    /// @brief PBlockElementGroup array
    PBlockElementGroup arrayElement[static_cast< uint32_t >(EDefaultValue::kInitGroupCount)];
};
//********************************/
/// @brief Page management: Free list
/// @code{.isoft}
/// @unit_name=PBlockElementFreePage
/// @endcode
struct PBlockElementFreePage final
{  // Page management free page data: 4 bytes
public:
    /// @brief Page number: Parent
    uint16_t sPrevPageID{0U};
    /// @brief Page number: Child
    uint16_t sNextPageID{0U};
};
/// @brief Initialize PBlockElementFreePage structure
/// @code{.isoft}
/// @unit_name=InitElement
/// @endcode
/// @param element
inline void InitElement(PBlockElementFreePage& element) noexcept
{
    element.sPrevPageID = kInvalidPageID;
    element.sNextPageID = kInvalidPageID;
}
/// @brief
/// @code{.isoft}
/// @unit_name=PBlockHeadPageBoss
/// @endcode
struct PBlockHeadPageBoss final
{  // 2021-05-26 Tentative length 64 + 4 * (16 + X) bytes
public:
    /// @brief Belonging GroupID
    uint16_t sGroupID{0U};  //
    /// @brief Maximum page count: Max 1000 for 4K pages
    uint16_t sPageCountMax{0U};  //
    /// @brief Current page count: Max 1000 for 4K pages
    uint16_t sElementCount{0U};  //
    /// @brief Valid length of free index list sFreeList: Max 27
    uint16_t sFreeListCount{0U};  //
    /// @brief // 28    // Higher free list index means higher free rate: 0 is fully occupied, N is fully free
    uint16_t sFreeList[static_cast< int32_t >(EDefaultValue::kMaxFreeListCount)]{0U};
    /// @brief PBlockElementFreePage array: Potential out-of-bounds access, specific count determined by sElementCount
    PBlockElementFreePage arrayElement[static_cast< uint32_t >(EDefaultValue::kInitFreeListCount)]{};
};
//********************************/
/// @brief KeyHash management    // Exclusive page
/// @code{.isoft}
/// @unit_name=PBlockElementKeyHash
/// @endcode
struct PBlockElementKeyHash final
{  // With 4K page, supports 1000 hash indexes at once
public:
    /// @brief Page number of the hit Key: First page number
    uint32_t nPageID{0U};
};
/// @brief Initialize
/// @code{.isoft}
/// @unit_name=InitElement
/// @endcode
/// @param element
/// @param nID
inline void InitElement(PBlockElementKeyHash& element, uint32_t const nID) noexcept { element.nPageID = nID; }
/// @brief
/// @code{.isoft}
/// @unit_name=PBlockHeadKeyHash
/// @endcode
struct PBlockHeadKeyHash final
{  // 2021-06-03 Tentative length 16 + 4 * (7 + X) bytes
public:
    /// @brief Number of exclusive KV pages in Hash table
    uint32_t nKvPageCount{0U};
    /// @brief Maximum number of buckets in Hash table
    uint16_t sHashMaxCapacity{0U};
    /// @brief Current number of buckets in Hash table, number of elements in arrayElement
    uint16_t sElementCount{0U};
    /// @brief Hash type: Hash type can dynamically fall into buckets based on sHashCount
    uint8_t cHashType{0U};
    /// @brief Compression type
    uint8_t cCompressType{0U};
    /// @brief Initial number of buckets in Hash table: Also minimum number
    uint8_t cHashInitCount{0U};
    /// @brief Reserved field
    uint8_t cReserve[kInt_5]{0U};
    /// @brief Buckets in Hash table, forcibly changed to variable-length array when Hash table expands
    PBlockElementKeyHash arrayElement[static_cast< uint32_t >(EDefaultValue::kDefInitHashCount)];
};
//********************************/
/// @brief Redundancy type
/// @code{.isoft}
/// @unit_name=EReddType
/// @endcode
enum class EReddType : uint16_t
{
    kNone = 0,  // No redundancy
    kCrc  = 1,  // CRC check
    kMofN = 2,  // M/N redundancy
    kHash = 3,  // Hash algorithm introduced in 2022 standard
};
/// @brief Key-Value data pair
/// @code{.isoft}
/// @unit_name=PBlockElementKv
/// @endcode
struct PBlockElementKv final
{  // 2021-06-04 Tentative 8 bytes
public:
    /// @brief Sort number is also Hash value
    uint32_t nKeyHashID{0U};
    /// @brief Offset within this page
    uint16_t sOffset{0U};
    /// @brief Data area length: Max 64 bytes
    uint8_t cLen{0U};
    /// @brief Redundancy type
    uint8_t cReddType{0U};
};
/// @brief PBlockHeadKvStore forcibly occupies full page space
/// @code{.isoft}
/// @unit_name=PBlockHeadKvStore
/// @endcode
struct PBlockHeadKvStore final
{  // 2021-06-03 Tentative length: 32 + 8 * X bytes
public:
    /// @brief Continuous nKvStoreID pointed to by HashIndex
    uint32_t nKvStoreID{0U};
    /// @brief Total number of Kv-Elements managed by this module: Only data in the first block is correct
    uint32_t nKvTotal{0U};
    /// @brief Page number of the last KV-Element in this bucket: Only data in the first block is correct
    uint32_t nLastKvPageID{0U};
    /// @brief Module default allocation size
    uint16_t sInitLen{0U};
    /// @brief Free size: Logical size (excluding arrayElement[1])
    uint16_t sFreeLen{0U};
    /// @brief Number of Kv-Elements managed by this page
    uint16_t sElementCount{0U};
    /// @brief Previous page ID in case of cross-page KvStore
    uint32_t nPrevPageID{0U};
    /// @brief Next page ID in case of cross-page KvStore
    uint32_t nNextPageID{0U};
    /// @brief
    uint16_t nMaxValueLen{0U};
    /// @brief
    uint8_t cReserve[kInt_4]{0U};  // Reserved field
};
/// @brief Enum: Storage range
/// @code{.isoft}
/// @unit_name=EDataFormat
/// @endcode
enum class EDataFormat : uint8_t
{
    kNoData = 0,  // No such data
    kLocal,       // Max 256 bytes within this page: PDataLocal
    kBlockIndex,  // Points to data in other pages: PBlockIndex
    kHugeBlock,   // Large Block: May span several Pages
};
/// @brief Logic type:
/// @code{.isoft}
/// @unit_name=EDataLogic
/// @endcode
enum class EDataLogic : uint8_t
{
    kLogicKey = 0,  // Key data area
    kLogicValue,    // Value data area
    kLogicRedd,     // Redundancy data area
    kLogicCount,
    kLogicCrc64,  // Hash code of locally stored Key value
};
/// @brief KV data area consists of Key, Value, Redundance in sequence
/// @code{.isoft}
/// @unit_name=PElementKvData
/// @endcode
struct PElementKvData final  // NOLINT
{                            // Length: 3 bytes
public:
    /// @brief CRC16 encoding of KV structure and data (used for internal data verification, CRC16 of PBlockElementKv + PElementKvData + 3 * PDataLocal)
    uint16_t sCrc16{0U};
    /// @brief Whether locally stored Key value is complete
    uint8_t cKeyWhole : 2;
    /// @brief Data structure EDataFormat.Key
    uint8_t cFormatKey : 2;
    /// @brief Data structure EDataFormat.Value
    uint8_t cFormatValue : 2;
    /// @brief Data structure EDataFormat.Redundancy
    uint8_t cFormatRedd : 2;
    // Data area consists of Key, Value, Redundance in sequence
};
/// @brief Data format: Short
/// @code{.isoft}
/// @unit_name=PDataLocal
/// @endcode
struct PDataLocal final
{  // 2 bytes
public:
    /// @brief Data type
    uint8_t cDataType{0U};
    /// @brief Data length
    uint8_t cDataLen{0U};
};
/// @brief 12 bytes, mainly used for kValue page data return
/// @code{.isoft}
/// @unit_name=PValueData
/// @endcode
struct PValueData final
{
public:
    /// @brief Data length
    uint16_t sDataLen{0U};
    /// @brief Data type
    uint8_t cDataType{0U};
    /// @brief Reserved field
    uint8_t cReserve{0U};
    /// @brief Data
    uint8_t* pData{nullptr};
    /// @brief Extended data
    PBlockSpread* pSpread{nullptr};
};
/// @brief Block index of Value type, used for extended interface to read/write cross-page data
/// @code{.isoft}
/// @unit_name=PBlockValue
/// @endcode
struct PBlockValue final
{
public:
    /// @brief Page.ID to which the Block belongs
    uint32_t nPageID{0U};
    /// @brief Block sequence number within Page data area
    uint16_t sBlockID{0U};
    /// @brief Data length
    uint16_t sDataLen{0U};
};
/// @brief Whole page data page number data for large data pages
/// @code{.isoft}
/// @unit_name=PDataWholePage
/// @endcode
struct PDataWholePage final
{
public:
    /// @brief Page base class
    uint32_t nBasePageID{0U};
    /// @brief Number of continuous page numbers
    uint16_t sSeriesCount{0U};
};
/// @brief Super large data page, storage format is: (N - 1) * PageLen + nDataLen % PageLen
/// @code{.isoft}
/// @unit_name=PDataHugeBlock
/// @endcode
struct PDataHugeBlock final
{
public:
    /// @brief Total data length
    uint32_t nDataLen{0U};
    /// @brief Number of whole pages
    uint32_t nWholeCount{0U};
    /// @brief Link to external page where remaining data after modulo with whole page resides
    PBlockIndex tailBlock;
    /// @brief Number of arrayElement
    uint16_t sElementCount{0U};
    /// @brief Linked list of whole page data
    PDataWholePage arrayElement[1];
};
// //********************************/
/// @brief Value
/// @code{.isoft}
/// @unit_name=PBlockHeadValue
/// @endcode
struct PBlockHeadValue final
{
public:
    /// @brief Data area
    uint8_t arrayElement[1];
};
//********************************/
/// @brief Array subscript is storage position in Walog library: Length 4 bytes
/// @code{.isoft}
/// @unit_name=PBlockElementWalog
/// @endcode
struct PBlockElementWalog final
{
public:
    /// @brief PageID in Main library
    uint32_t nMainPageID{0U};
};
/// @brief PBlockHeadWalog forcibly occupies full page data, so no special handling needed for arrayElement
/// @code{.isoft}
/// @unit_name=PBlockHeadWalog
/// @endcode
struct PBlockHeadWalog final
{  // PageID to file position mapping in Walog log, length = 20 + 4 * X bytes
public:
    /// @brief Total number of pages in Walog library (including management pages)
    uint32_t nPageTotal{0U};
    /// @brief Total count: Only valid for the first page
    uint32_t nBasePageID{0U};
    /// @brief Maximum number of PBlockElementWalog allowed in this page
    uint16_t sArrayCount{0U};
    /// @brief Reserved
    uint16_t sReserve{0U};
    /// @brief Previous page ID in case of cross-page (Walog page number)
    uint32_t nPrevPageID{0U};
    /// @brief Next page ID in case of cross-page (Walog page number)
    uint32_t nNextPageID{0U};
    /// @brief At least one to prevent compiler warning
    PBlockElementWalog arrayElement[1];
};
//********************************/
/// @brief String type
/// @code{.isoft}
/// @unit_name=EVersionStringType
/// @endcode
enum EVersionStringType : uint32_t
{
    /// @brief String length array
    kVersionString_System = 0,
    /// @brief Version number string length: Persistence module system version number
    kVersionString_App = 1,
    /// @brief Version number string length: App
    kVersionString_Per = 2,
    kVersionString_Count,
};
/// @brief Unique data for stored files: Extended data
/// @code{.isoft}
/// @unit_name=PBlockHead_Ext
/// @endcode
struct PBlockHead_Ext final
{  // 2023-07-12    Tentative length 16
public:
    /// @brief String length array
    uint8_t nStringLen[static_cast< int32_t >(EVersionStringType::kVersionString_Count)]{0U};
    /// @brief To be extended
    uint8_t cReserve[kInt_0x10 - static_cast< int32_t >(EVersionStringType::kVersionString_Count)]{0U};
};
/// @brief Return length of nStringLen array in PBlockHead_Ext
/// @code{.isoft}
/// @unit_name=GetExtBlockStringCount
/// @endcode
/// @return
inline int32_t GetExtBlockStringCount() noexcept
{
    PBlockHead_Ext extBlock;
    uint32_t const nStringCount{sizeof(extBlock.nStringLen) / sizeof(extBlock.nStringLen[0])};
    return static_cast< int32_t >(nStringCount);
}
//********************************/
/// @brief Additional saved file information
/// @code{.isoft}
/// @unit_name=PFileInfoInKvs
/// @endcode
struct PFileInfoInKvs final
{  // Length 10
public:
    /// @brief @AUTOSAR_SWS {SWS_PER_00443} //Time in nanoseconds since midnight 1970-01-01 UTC at which the file was
    /// last accessed.
    uint64_t accessTime{0U};
    /// @brief @AUTOSAR_SWS {SWS_PER_00444} //Information on how and by whom the file was created.
    uint8_t fileCreationState{0U};
    /// @brief @AUTOSAR_SWS {SWS_PER_00445} //Information on how and by whom the file was last modified.
    uint8_t fileModificationState{0U};
};
/// @brief File information: Redundancy extension
/// @code{.isoft}
/// @unit_name=PFileInfoInKvs_ReddExt
/// @endcode
struct PFileInfoInKvs_ReddExt final
{  // Length 32
public:
    /// @brief Redundancy algorithm AlgID: Converted from "isoft" CryptoProvider
    uint64_t nAlgID{0U};
    /// @brief Version number of this structure, 0 as of 2023-08-02
    uint16_t nVersion{0U};
    /// @brief Redundancy type
    uint8_t nReddType{0U};
    /// @brief Calculation result length
    uint8_t nResultLen{0U};
    /// @brief Reserved words: From tail
    uint8_t chReserve[kInt_32 - kInt_10 - kInt_2]{0U};
};
//********************************/
/// @brief Get compile version number of KvSystem
/// @code{.isoft}
/// @unit_name=GetKvSystemVersion
/// @endcode
constexpr inline ara::core::StringView GetKvSystemVersion() noexcept
{
    return std::move(ara::core::StringView{"0.0.1.+2023-12-15-AP-PER2.1"});
}
//********************************/
#pragma pack(pop)
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
