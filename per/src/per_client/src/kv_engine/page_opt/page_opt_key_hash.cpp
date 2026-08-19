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
/// @file       page_opt_key_hash.cpp
/// @brief      Logical page for parsing Hash tables
/// @details
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptKeyHash
/// @unit_description=Logical page for parsing Hash tables
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-19 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_key_hash.h"

#include "ara/core/map.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "isoft/crc/crc.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @param pCachePagePtr
/// @param nDefBlockID
PPageOptKeyHash::PPageOptKeyHash(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID) noexcept
    : PPageOptSolo{pCachePagePtr, EBlockType::kKeyHash, nDefBlockID}
{
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptKeyHash::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    nMinValidLen += sizeof(PBlockHeadKeyHash);
    return static_cast< uint16_t >(nMinValidLen);
}
//***************/

/// @brief Calculate the Hash value of stKey
/// @param stKey
/// @return
uint64_t PPageOptKeyHash::ComputeHashKey(ara::core::StringView const &stKey) noexcept
{
    isoft::crc::BufferView const buffView{T_TransBytes(stKey.data()), stKey.size()};
    return isoft::crc::CRC::CalculateCRC64(buffView);
}
/// @brief
/// @param pBData
/// @param nDataLen
/// @return
uint64_t PPageOptKeyHash::ComputeHashKey(uint8_t const *const pBData, int32_t const nDataLen) noexcept
{
    isoft::crc::BufferView const buffView{pBData, static_cast< std::size_t >(nDataLen)};
    return isoft::crc::CRC::CalculateCRC64(buffView);
}
//***************/

/// @brief The capacity after Hash expansion must be double the previous size
/// @param nMaxCapacity
/// @param nInitCount
/// @param nDefKvPageID
/// @param nHashType
/// @param nCompressType
/// @return
bool PPageOptKeyHash::InitHashHead(uint16_t const nMaxCapacity,
                                   uint16_t const nInitCount,
                                   uint32_t const nDefKvPageID,
                                   uint8_t const nHashType,
                                   uint8_t const nCompressType) noexcept
{
    if (false == InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return false;
    }
    // PBlockHeadKeyHash
    uint16_t const nInitArrayCount{T_GetArrayCount< PBlockHeadKeyHash >()};
    uint16_t nMinBlockLen{sizeof(PBlockHeadKeyHash)};
    // Handle the fixed-size array part
    if (nInitCount > nInitArrayCount) {
        nMinBlockLen += static_cast< uint16_t >((nInitCount - nInitArrayCount)
                                                * static_cast< uint16_t >(sizeof(PBlockElementKeyHash)));
    }
    if (false == InitSoloBlock(EStoreRange::kSingle, nMinBlockLen)) {
        return false;
    }
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return false;
    }
    // The storage location may have changed, so re-obtain this value
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    pHashHead->nKvPageCount
        = 0U;  // Records the number of independent KV pages; mixed KV pages during initialization are not counted
    pHashHead->sHashMaxCapacity = nMaxCapacity;
    pHashHead->cHashInitCount   = static_cast< uint8_t >(nInitCount);
    pHashHead->sElementCount    = nInitCount;
    pHashHead->cHashType        = nHashType;
    pHashHead->cCompressType    = nCompressType;
    std::ignore                 = memset(static_cast< void * >(pHashHead->cReserve), 0, sizeof(pHashHead->cReserve));
    if (pHashHead->cReserve[0] == 0U) {  // This output parameter value is not subsequently checked.
    }
    int32_t i{0};
    for (i = 0; i < static_cast< int32_t >(nInitCount); i++) {
        S_SetHashValue(pHashHead, i, nDefKvPageID);
    }
    for (i = static_cast< int32_t >(nInitCount); i < static_cast< int32_t >(nInitArrayCount); i++) {
        S_SetHashValue(pHashHead, i, kInvalidPageID);
    }

    _LogDebug("[KV_Opt].InitHashHead", ":", "PageID", static_cast< int32_t >(GetPageID()), "nInitCount",
              static_cast< int32_t >(nInitCount));
    _OnOptSuccess(true);
    return true;
}
/// @brief Modify the value of the nHashIndex array element
/// @param nHashIndex
/// @param nHashPageID
/// @return
bool PPageOptKeyHash::UpdateHashValueByIndex(uint16_t const nHashIndex, uint32_t const nHashPageID) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return false;
    }
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    if (false == S_CheckArrayIndex(*pHashHead, static_cast< int32_t >(nHashIndex))) {
        PER_OnOptFailed(EErrorPHKV::kIndexHash);
        return false;
    }
    S_SetHashValue(pHashHead, static_cast< int32_t >(nHashIndex), nHashPageID);
    _OnOptSuccess(true);
    return true;
}
/// @brief Change all old PageID values to the new PageID
/// @param nOldPageID
/// @param nNewPageID
/// @return
bool PPageOptKeyHash::UpdateHashValue(uint32_t const nOldPageID, uint32_t const nNewPageID) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return false;
    }
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    S_UpdateHashValue(pHashHead, 0, static_cast< int32_t >(pHashHead->sElementCount), nOldPageID, nNewPageID);
    _OnOptSuccess(true);
    return true;
}
/// @brief
/// @param nBegin
/// @param nCount
/// @param nOldPageID
/// @param nNewPageID
/// @return
bool PPageOptKeyHash::UpdateHashValue(int32_t const nBegin,
                                      int32_t const nCount,
                                      uint32_t const nOldPageID,
                                      uint32_t const nNewPageID) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return false;
    }
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    S_UpdateHashValue(pHashHead, nBegin, nCount, nOldPageID, nNewPageID);
    _OnOptSuccess(true);
    return true;
}
/// @brief Double the Hash pool capacity: return value -1 on failure, otherwise the increment of expansion
/// @param nKvPageCount
/// @return
int32_t PPageOptKeyHash::TryToExpansionHashCount(uint32_t const nKvPageCount) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return -1;
    }
    PBlockHeadKeyHash *pHashHead{_GetHeadHash(true)};
    uint16_t nAddLen{0U};
    nAddLen = static_cast< uint16_t >(S_GetExpansionCount(*pHashHead, nKvPageCount) * sizeof(PBlockElementKeyHash));
    if (nAddLen <= 0U) {
        return 0;
    }
    // Do not expand
    if (false == _CheckPageCapacity(nAddLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return -1;
    }
    if (false == AddBlockData(GetSoloBlockID(), nAddLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageAddBlock);
        return -1;
    }
    pHashHead = _GetHeadHash(true);
    int32_t const nAddCount{S_ExpansionHashCount(pHashHead)};

    _OnOptSuccess(nAddCount > 0);
    return nAddCount;
}
/// @brief Halve the Hash pool capacity
/// @param nKvPageCount
/// @return
bool PPageOptKeyHash::TryToReduceHashCount(uint32_t const nKvPageCount) const noexcept
{
    // It needs to be ensured that after the page is reorganized
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return false;
    }
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    if (nKvPageCount > (static_cast< uint32_t >(pHashHead->sElementCount) / kInt_2U)) {
        return true;
    }
    int32_t const nDecCount{S_ReduceHashCount(pHashHead)};
    _OnOptSuccess(nDecCount > 0);
    return nDecCount > 0;
}
//***************/

/// @brief Find the page management data area
/// @return
PBlockHeadKeyHash *PPageOptKeyHash::GetHeadHash() const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (nullptr == pBlockHead) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return nullptr;
    }
    _OnOptSuccess(false);
    return T_TransPtr< PBlockHeadKeyHash >(pBlockHead);
}
/// @brief Find the Hash value
/// @param nSrcHashKey
/// @return
uint32_t PPageOptKeyHash::GetHashValue(uint64_t const nSrcHashKey) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    int32_t const nHashIndex{GetHashIndex(nSrcHashKey)};
    PER_Assert(S_CheckArrayIndex(*pHashHead, nHashIndex));
    return S_GetHashValue(pHashHead, nHashIndex);
}
/// @brief
/// @param stKey
/// @return
uint32_t PPageOptKeyHash::GetHashValue(ara::core::StringView const &stKey) const noexcept
{
    return GetHashValue(ComputeHashKey(stKey));
}
/// @brief Get the number of hash buckets
/// @return
uint16_t PPageOptKeyHash::GetHashCount() const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return static_cast< uint16_t >(pHashHead->sElementCount);
}
/// @brief Get the number of hash buckets
/// @return
uint16_t PPageOptKeyHash::GetHashCountHalf() const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return static_cast< uint16_t >(static_cast< uint16_t >(pHashHead->sElementCount) / kInt_2U);
}
/// @brief Calculate the hash array index from HashKeyID
/// @param nSrcHashKey
/// @return
int32_t PPageOptKeyHash::GetHashIndex(uint64_t const nSrcHashKey) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return TransHashIndex(static_cast< uint32_t >(nSrcHashKey % kInt_0x100000000U),
                          static_cast< uint32_t >(pHashHead->sElementCount));
}
/// @brief Calculate the hash array index from HashKeyID
/// @param nHashKeyID
/// @return
int32_t PPageOptKeyHash::GetHashIndex(uint32_t const nHashKeyID) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return TransHashIndex(nHashKeyID, static_cast< uint32_t >(pHashHead->sElementCount));
}
/// @brief
/// @param stKey
/// @return
int32_t PPageOptKeyHash::GetHashIndex(ara::core::StringView const &stKey) const noexcept
{
    return GetHashIndex(ComputeHashKey(stKey));
}
/// @brief Get the expanded paired index of nIndex
/// @param nHashIndex
/// @return
int32_t PPageOptKeyHash::GetHashIndexPair(int32_t const nHashIndex) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    int32_t nHashCount{0};
    nHashCount = static_cast< int32_t >(pHashHead->sElementCount);
    PER_Assert(nHashCount > 0);
    return (nHashIndex + nHashCount / kInt_2) % nHashCount;
}
/// @brief Find the hash index where nHashKeyID appears for the first time
/// @param nMatchPageID
/// @return
int32_t PPageOptKeyHash::FindFirstHashIndex(uint32_t const nMatchPageID) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    for (int32_t i{0}; i < static_cast< int32_t >(pHashHead->sElementCount); i++) {
        PBlockElementKeyHash const &keyHash{S_GetBlockElement(pHashHead, i)};
        if (keyHash.nPageID == nMatchPageID) {
            return i;
        }
    }
    return -1;
}
/// @brief Return the number of KeyPages
/// @return
uint32_t PPageOptKeyHash::GetKvPageCount() const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return pHashHead->nKvPageCount;
}
/// @brief Get the initial number of hash buckets
/// @return
uint8_t PPageOptKeyHash::GetHashInitCount() const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    return pHashHead->cHashInitCount;
}
/// @brief Get the number of hashes using the same PageID
/// @param nMatchPageID
/// @return
int32_t PPageOptKeyHash::GetMatchHashCount(uint32_t const nMatchPageID) const noexcept
{
    return ForEachHash(false, [nMatchPageID](int32_t, uint32_t const nPageID) noexcept -> int32_t {
        if (nPageID == nMatchPageID) {
            return 1;
        }
        return 0;
    });
}
/// @brief Maintain the number of KeyPages
/// @param nAddCount
void PPageOptKeyHash::AddKeyPageCount(int32_t const nAddCount) const noexcept
{
    PBlockHeadKeyHash *const pHashHead{_GetHeadHash(true)};
    pHashHead->nKvPageCount = T_AddInt(pHashHead->nKvPageCount, nAddCount);
}
/// @brief Iterate over all hash values: bExceptSameID indicates whether to remove the same PageID
/// @param bExceptSameID
/// @param pfun
/// @return
int32_t PPageOptKeyHash::ForEachHash(
    bool const bExceptSameID,
    std::function< int32_t(int32_t const nIndex, uint32_t const nPageID) > const &pfun) const noexcept
{
    PBlockHeadKeyHash const *const pHashHead{_GetHeadHash(true)};
    ara::core::Map< uint32_t, int32_t > mapPageID;
    int32_t nScanCount{0};
    for (int32_t i{0}; i < static_cast< int32_t >(pHashHead->sElementCount); i++) {
        uint32_t const nPageID{
            pHashHead->arrayElement[i].nPageID};  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
        bool bEnableScan{true};
        if (bExceptSameID) {
            int32_t &nPageCount{mapPageID[nPageID]};
            nPageCount += 1;
            if (nPageCount > 1) {
                bEnableScan = false;
            }
        }
        if (false == bEnableScan) {
            continue;
        }
        int32_t nReturn{0};
        nReturn = pfun(i, nPageID);
        if (0 == nReturn) {
            continue;
        }
        if (nReturn < 0) {
            break;
        }
        nScanCount += 1;
    }
    return nScanCount;
}
/// @brief Determine whether the current page is a Hash-exclusive page or a mixed page with only ExtString page
/// @return
bool PPageOptKeyHash::IsHashPageSolo() const noexcept
{
    if (IsSoloPage()) {
        return true;
    }
    uint8_t nBlendPageType{static_cast< uint8_t >(EBlockType::kExt)};
    if (false == IsHavePageType(nBlendPageType)) {
        return false;
    }
    nBlendPageType |= static_cast< uint8_t >(PageClassType());
    return PPageOptSolo::IsSoloPage(nBlendPageType);
}
//********************************/
/// @brief
/// @param pBlockHead
/// @param nElementIndex
/// @return
PBlockElementKeyHash &PPageOptKeyHash::S_GetBlockElement(PBlockHeadKeyHash *const pBlockHead,
                                                         int32_t const nElementIndex) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    return pBlockHead->arrayElement[nElementIndex];  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
}
/// @brief
/// @param pBlockHead
/// @param nElementIndex
/// @return
PBlockElementKeyHash const &PPageOptKeyHash::S_GetBlockElement(PBlockHeadKeyHash const *const pBlockHead,
                                                               int32_t const nElementIndex) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    return pBlockHead->arrayElement[nElementIndex];  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
}
/// @brief
/// @param pBlockHead
/// @param nHashIndex
/// @return
uint32_t PPageOptKeyHash::S_GetHashValue(PBlockHeadKeyHash const *const pBlockHead, int32_t const nHashIndex) noexcept
{
    PBlockElementKeyHash const &hashElement{S_GetBlockElement(pBlockHead, nHashIndex)};
    return hashElement.nPageID;
}
/// @brief
/// @param pBlockHead
/// @param nHashIndex
/// @param nHashPageID
void PPageOptKeyHash::S_SetHashValue(PBlockHeadKeyHash *const pBlockHead,
                                     int32_t const nHashIndex,
                                     uint32_t const nHashPageID) noexcept
{
    PBlockElementKeyHash &hashElement{S_GetBlockElement(pBlockHead, nHashIndex)};
    InitElement(hashElement, nHashPageID);
}
/// @brief
/// @param nData
/// @return
bool PPageOptKeyHash::S_IsPowerOf2(uint64_t const nData) noexcept { return ((nData & (nData - 1U)) == 0U); }
/// @brief Get the number of expansions needed
/// @param blockHead
/// @param nKvPageCount
/// @return
uint16_t PPageOptKeyHash::S_GetExpansionCount(PBlockHeadKeyHash const &blockHead, uint32_t const nKvPageCount) noexcept
{
    if (nKvPageCount <= blockHead.sElementCount) {
        return static_cast< uint16_t >(0U);
    }
    uint16_t const nNewHashCount{static_cast< uint16_t >(blockHead.sElementCount * kInt_2U)};
    if (nNewHashCount > blockHead.sHashMaxCapacity) {
        return static_cast< uint16_t >(0U);
    }
    uint16_t const nInitArrayCount{T_GetArrayCount< PBlockHeadKeyHash >()};
    if (nNewHashCount <= nInitArrayCount) {
        return static_cast< uint16_t >(0U);
    }
    return std::min< uint16_t >(static_cast< uint16_t >(nNewHashCount - blockHead.sElementCount),
                                static_cast< uint16_t >(nNewHashCount - nInitArrayCount));
}
//***************/

/// @brief
/// @param blockHead
/// @param nIndex
/// @return
bool PPageOptKeyHash::S_CheckArrayIndex(PBlockHeadKeyHash const &blockHead, int32_t const nIndex) noexcept
{
    return (nIndex >= 0) && (nIndex < static_cast< int32_t >(blockHead.sElementCount));
}
/// @brief Batch update all hash values in the interval [nBegin, nBegin + nCount)
/// @param pHashHead
/// @param nBegin
/// @param nCount
/// @param nOldPageID
/// @param nNewPageID
void PPageOptKeyHash::S_UpdateHashValue(PBlockHeadKeyHash *const pHashHead,
                                        int32_t const nBegin,
                                        int32_t const nCount,
                                        uint32_t const nOldPageID,
                                        uint32_t const nNewPageID) noexcept
{
    PER_Assert(nullptr != pHashHead);
    for (int32_t i{0}; i < nCount; i++) {
        int32_t const nHashIndex{nBegin + i};
        if ((nHashIndex >= 0) && (nHashIndex < static_cast< int32_t >(pHashHead->sElementCount))) {
            PBlockElementKeyHash &keyHash{S_GetBlockElement(pHashHead, nBegin + i)};
            if (keyHash.nPageID == nOldPageID) {
                keyHash.nPageID = nNewPageID;
            }
        }
    }
}
/// @brief Double the Hash pool capacity
/// @param pBlockHead
/// @return
int32_t PPageOptKeyHash::S_ExpansionHashCount(PBlockHeadKeyHash *const pBlockHead) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    uint16_t const nAddHashCount{pBlockHead->sElementCount};
    if ((pBlockHead->sElementCount + nAddHashCount) > pBlockHead->sHashMaxCapacity) {
        return 0;
    }
    for (int32_t i{0}; i < static_cast< int32_t >(pBlockHead->sElementCount); i++) {
        PBlockElementKeyHash const &oldKeyHash{S_GetBlockElement(pBlockHead, i)};
        S_SetHashValue(pBlockHead, static_cast< int32_t >(pBlockHead->sElementCount) + i, oldKeyHash.nPageID);
    }
    pBlockHead->sElementCount += nAddHashCount;
    return static_cast< int32_t >(nAddHashCount);
}
/// @brief Halve the Hash pool capacity
/// @param pBlockHead
/// @return
int32_t PPageOptKeyHash::S_ReduceHashCount(PBlockHeadKeyHash *const pBlockHead) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    if (pBlockHead->cHashInitCount == pBlockHead->sElementCount) {
        return 0;
    }
    uint16_t nDecHashCount{pBlockHead->sElementCount};
    nDecHashCount /= static_cast< uint16_t >(kInt_2);
    for (int32_t i{0}; i < static_cast< int32_t >(nDecHashCount); i++) {
        S_SetHashValue(pBlockHead, static_cast< int32_t >(nDecHashCount) + i, kInvalidPageID);
    }
    pBlockHead->sElementCount -= static_cast< uint16_t >(nDecHashCount);
    return static_cast< int32_t >(nDecHashCount);
}
/// @brief Find the page management data area
/// @param bCheck Whether to execute PER_Assert check
/// @return
PBlockHeadKeyHash *PPageOptKeyHash::_GetHeadHash(bool const bCheck) const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (bCheck) {
        PER_Assert(nullptr != pBlockHead);
    }
    return T_TransPtr< PBlockHeadKeyHash >(pBlockHead);
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
