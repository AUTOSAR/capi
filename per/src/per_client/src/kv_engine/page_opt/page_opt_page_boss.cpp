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
/// @file       page_opt_page_boss.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Page management
/// @date       2021-05-10
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptPageBoss
/// @unit_description=Logical page for parsing management Boss pages
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-10 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_page_boss.h"

#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/manager_cache.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Operation class for the BossBlock data of the PageBoss page
class POptBossBlock final
{
private:
    PBlockHeadPageBoss *const kBossHeadPtr;

public:
    /// @brief Constructor
    /// @param pBossHead
    explicit POptBossBlock(PBlockHeadPageBoss *const pBossHead) : kBossHeadPtr{pBossHead} {};
    /// @brief Check whether pBossHead_ is validly initialized
    /// @return
    bool IsValid() const noexcept { return nullptr != kBossHeadPtr; }
    /// @brief Check whether the BlockElement array index is valid
    /// @param nIndex
    /// @return
    bool CheckElementIndex(int32_t const nIndex) const noexcept
    {
        if (nullptr == kBossHeadPtr) {
            return false;
        }
        return (nIndex >= 0) && (nIndex < static_cast< int32_t >(kBossHeadPtr->sElementCount));
    }
    /// @brief Check the number of BlockElements
    /// @return
    bool CheckElementCount() const noexcept
    {
        if (nullptr == kBossHeadPtr) {
            return false;
        }
        return kBossHeadPtr->sElementCount <= kBossHeadPtr->sPageCountMax;
    }
    /// @brief Check whether the free list array index is valid
    /// @param nIndex
    /// @return
    bool CheckFreeListIndex(int32_t const nIndex) const noexcept
    {
        if (nullptr == kBossHeadPtr) {
            return false;
        }
        // The maximum is the completely free list
        return (nIndex >= 0) && (nIndex <= static_cast< int32_t >(kBossHeadPtr->sFreeListCount))
               && (nIndex <= static_cast< int32_t >(ara::per::isoftkv::EDefaultValue::kMaxFreeListCount));
    }
    /// @brief Get free list data
    /// @param blockHead
    /// @param nIndex
    /// @return
    uint16_t GetFreeListData(int32_t const nIndex) const noexcept
    {
        PER_Assert(CheckFreeListIndex(nIndex));
        return kBossHeadPtr->sFreeList[nIndex];  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    /// @brief Get PBlockElementFreePage
    /// @param pBossHead
    /// @param nPageID
    /// @return
    PBlockElementFreePage &GetBlockElement(uint16_t const nPageIndex) const noexcept
    {
        PER_Assert(nullptr != kBossHeadPtr);
        PER_Assert(CheckElementIndex(nPageIndex));
        return kBossHeadPtr->arrayElement[nPageIndex];  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    /// @brief Initialize the PBlockElementFreePagep structure
    /// @param [out] element
    static void InitBlockElement(PBlockElementFreePage &element) noexcept
    {
        element.sPrevPageID = kInvalidPageID;
        element.sNextPageID = kInvalidPageID;
    }
    /// @brief Initialize the BlockElement of the Boss page itself
    /// @param nBossPageIndex Page index converted from the PageID of the Boss page
    void InitBlockElement_BossPage(uint16_t const nBossPageIndex) const noexcept
    {
        PER_Assert(0 == kBossHeadPtr->sElementCount);
        kBossHeadPtr->sElementCount += 1U;
        PBlockElementFreePage &elementBoss{GetBlockElement(nBossPageIndex)};
        InitElement(elementBoss);
    }

public:
    /// @brief Move nPageID to the first position of the free list at nFreeIndex
    /// @param nPageIndex Page subscript (converted from the uint32_t PageID saved by the Boss page) managed by this Boss page
    /// @param nListType Free list type
    void MovePageToFreeList(uint16_t const nPageIndex, int32_t const nListType) const noexcept
    {
        PER_Assert(nullptr != kBossHeadPtr);
        // Sentinel
        uint16_t const nFreeSentryID{GetFreeListData(nListType)};
        PBlockElementFreePage &workPage{GetBlockElement(nPageIndex)};
        workPage.sPrevPageID = kInvalidPageID;
        workPage.sNextPageID = nFreeSentryID;
        if (PPageOptBase::IsValidPageID(static_cast< uint32_t >(nFreeSentryID))) {
            PBlockElementFreePage &sentryPage{GetBlockElement(nFreeSentryID)};
            sentryPage.sPrevPageID = nPageIndex;
        }
        _SetFreeListData(nListType, nPageIndex);
    }
    /// @brief Move nPageID to the tail of the last free list at nFreeIndex
    /// @param nPageIndex Page subscript (converted from the uint32_t PageID saved by the Boss page) managed by this Boss page
    void MovePageToFreeTail(uint16_t const nPageIndex) const noexcept
    {
        PER_Assert(nullptr != kBossHeadPtr);
        int32_t const nListType{static_cast< int32_t >(kBossHeadPtr->sFreeListCount) - 1};
        PBlockElementFreePage &workPage{GetBlockElement(nPageIndex)};
        workPage.sNextPageID = kInvalidPageID;
        uint16_t const nTailPageID{_FindListTailPage(nListType)};
        if (PPageOptBase::IsValidPageID(static_cast< uint32_t >(nTailPageID))) {
            PBlockElementFreePage &tailPage{GetBlockElement(nTailPageID)};
            tailPage.sNextPageID = nPageIndex;
            workPage.sPrevPageID = nTailPageID;
        } else {
            workPage.sPrevPageID = kInvalidPageID;
            _SetFreeListData(nListType, nPageIndex);
        }
    }
    /// @brief Remove nPageID from its original doubly linked list
    /// @param nPageIndex Page subscript (converted from the uint32_t PageID saved by the Boss page) managed by this Boss page
    void CutPageFromList(uint16_t const nPageIndex) const noexcept
    {
        PER_Assert(nullptr != kBossHeadPtr);
        PBlockElementFreePage &workPage{GetBlockElement(nPageIndex)};
        // Hand over to its predecessor
        if (PPageOptBase::IsValidPageID(static_cast< uint32_t >(workPage.sPrevPageID))) {
            PBlockElementFreePage &prevPage{GetBlockElement(workPage.sPrevPageID)};
            prevPage.sNextPageID = workPage.sNextPageID;
        }
        // Hand over to its successor
        if (PPageOptBase::IsValidPageID(static_cast< uint32_t >(workPage.sNextPageID))) {
            PBlockElementFreePage &nextPage{GetBlockElement(workPage.sNextPageID)};
            nextPage.sPrevPageID = workPage.sPrevPageID;
        }
        // Hand over sentinel duties: if there is no predecessor, it means it is the sentinel itself
        if (false == PPageOptBase::IsValidPageID(static_cast< uint32_t >(workPage.sPrevPageID))) {
            for (int32_t i{0}; i <= static_cast< int32_t >(kBossHeadPtr->sFreeListCount); i++) {
                if (GetFreeListData(i) == nPageIndex) {
                    _SetFreeListData(i, workPage.sNextPageID);
                    break;
                }
            }
        }
        // Reset its own social relationships
        workPage.sPrevPageID = kInvalidPageID;
        workPage.sNextPageID = kInvalidPageID;
    }
    /// @brief Initialize the table header
    /// @param nGroupID
    /// @param nFreeListLen
    /// @param nPageCountMax
    void InitBossHead(uint16_t const nGroupID, uint16_t const nFreeListLen, uint16_t const nPageCountMax) const noexcept
    {
        // 2021-07-21 Pages are considered full by default upon initialization
        PER_Assert(nullptr != kBossHeadPtr);
        kBossHeadPtr->sGroupID       = nGroupID;
        kBossHeadPtr->sPageCountMax  = nPageCountMax;
        kBossHeadPtr->sElementCount  = 0U;
        kBossHeadPtr->sFreeListCount = nFreeListLen;
        for (uint16_t &freeData : kBossHeadPtr->sFreeList) {
            freeData = 0U;
        }
        for (PBlockElementFreePage &elementData : kBossHeadPtr->arrayElement) {
            InitElement(elementData);
        }
    }

protected:
    /// @brief Find the last node of the linked list
    /// @param nListType Free list type
    /// @return
    uint16_t _FindListTailPage(int32_t const nListType) const noexcept
    {
        PER_Assert(nullptr != kBossHeadPtr);
        uint16_t nFindPageID{GetFreeListData(nListType)};
        while (PPageOptBase::IsValidPageID(static_cast< uint32_t >(nFindPageID))) {
            PBlockElementFreePage &pageWork{GetBlockElement(nFindPageID)};
            if (false == PPageOptBase::IsValidPageID(static_cast< uint32_t >(pageWork.sNextPageID))) {
                break;
            }
            nFindPageID = pageWork.sNextPageID;
        }
        return nFindPageID;
    }
    /// @brief Set free list data
    /// @param blockHead
    /// @param nIndex
    /// @param nData
    void _SetFreeListData(int32_t const nIndex, uint16_t const nData) const noexcept
    {
        PER_Assert(CheckFreeListIndex(nIndex));
        kBossHeadPtr->sFreeList[nIndex] = nData;  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
};
//********************************/
/// @brief
/// @param pCachePagePtr
/// @param nDefBlockID
PPageOptPageBoss::PPageOptPageBoss(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID) noexcept
    : PPageOptSolo{pCachePagePtr, EBlockType::kPageBoss, nDefBlockID}
{
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptPageBoss::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    nMinValidLen += sizeof(PBlockHeadPageBoss);
    return static_cast< uint16_t >(nMinValidLen);
}
//***************/
/// @brief Initialize the BOSSBlock header of the BossPage
/// @param nGroupID
/// @param nPageCapacity
/// @param nFreeListCount
/// @param nPageCountMax
/// @return
bool PPageOptPageBoss::InitBossHead(uint16_t const nGroupID,
                                    uint16_t const nPageCapacity,
                                    uint16_t const nFreeListCount,
                                    uint16_t const nPageCountMax) noexcept
{
    if (false == InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return false;
    }
    PCachePagePtr pCachTemp = GetCachePage();
    uint16_t const nMinBlockLen{sizeof(PBlockHeadPageBoss)};
    if (false == InitSoloBlock(EStoreRange::kSingle, nMinBlockLen)) {
        return false;
    }
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return false;
    }
    uint16_t nMaxPageCapacity{
        static_cast< uint16_t >((GetPageHead()->sPageLen - GetMinValidLen()) / sizeof(PBlockElementFreePage))};
    nMaxPageCapacity += static_cast< uint16_t >(EDefaultValue::kInitFreeListCount);
    nMaxPageCapacity = std::min(nMaxPageCapacity, nPageCountMax);
    POptBossBlock const optBossBlock{_GetHeadBoss(true)};
    optBossBlock.InitBossHead(nGroupID, nFreeListCount, nMaxPageCapacity);
    uint16_t nBossPageIndex = static_cast< uint16_t >(TransPage_IDtoIndex(GetPageID()) % nPageCapacity);
    optBossBlock.InitBlockElement_BossPage(nBossPageIndex);

    _LogDebug("[KV_Opt].InitBossHead", ":", "PageID", static_cast< int32_t >(GetPageID()), "nGroupID",
              static_cast< int32_t >(nGroupID), "nPageCountMax", static_cast< int32_t >(nPageCountMax));
    _OnOptSuccess(true);
    return true;
}
/// @brief Transfer the page where nIndex resides into a new list
/// @param nPageID
/// @param nFreeLen
/// @return
bool PPageOptPageBoss::UpdateFreePage(uint16_t const nPageIndex, uint16_t const nFreeLen) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return false;
    }
    PBlockHeadPageBoss *pBossHead{_GetHeadBoss(true)};
    if (nPageIndex >= static_cast< int32_t >(pBossHead->sElementCount)) {
        uint16_t const nInitArrayCount{T_GetArrayCount< PBlockHeadPageBoss >()};
        if (pBossHead->sElementCount >= nInitArrayCount) {
            if (false == _CheckPageCapacity(sizeof(PBlockElementFreePage))) {
                PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
                return false;
            }
            PBlockElementFreePage elementBoss;  // The index is its own identifier
            InitElement(elementBoss);
            if (false == AddBlockData(GetSoloBlockID(), T_TransBytes(&elementBoss), sizeof(elementBoss))) {
                PER_OnOptFailed(EErrorPHKV::kPageAddBlock);
                return false;
            }
            pBossHead = _GetHeadBoss(true);
        } else {
            POptBossBlock::InitBlockElement(pBossHead->arrayElement[pBossHead->sElementCount]);  // NOLINT
        }
        pBossHead->sElementCount += 1U;
    }
    // BlockHead may change
    POptBossBlock const optBossBlock{_GetHeadBoss(true)};
    // Remove from original linked list
    optBossBlock.CutPageFromList(nPageIndex);
    int32_t const nListType{_ComputeFreeListIndex(nFreeLen)};
    // Add to new linked list
    if (optBossBlock.CheckFreeListIndex(nListType)) {
        optBossBlock.MovePageToFreeList(nPageIndex, nListType);
    }
    _LogDebug("[PH_System].UpdateFreePage", ":", "nPageIndex", static_cast< int32_t >(nPageIndex), "ListType",
              nListType);
    _OnOptSuccess(true);
    return true;
}
/// @brief Does not actually delete page data and the page; actual deletion can only be done during disk defragmentation
/// @param nPageIndex
/// @return
bool PPageOptPageBoss::DelOldPage(uint16_t const nPageIndex) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return false;
    }
    POptBossBlock const optBossBlock{_GetHeadBoss(true)};
    if (false == optBossBlock.CheckElementIndex(nPageIndex)) {
        PER_OnOptFailed(EErrorPHKV::kIndexBoss);
        return false;
    }
    // Remove from original linked list
    optBossBlock.CutPageFromList(nPageIndex);
    // Add to the end of the free list
    optBossBlock.MovePageToFreeTail(nPageIndex);
    // Does not actually delete page data and the page; actual deletion can only be done during disk defragmentation

    _OnOptSuccess(true);
    return true;
}
//***************/
/// @brief Get the number of free lists
/// @return
int32_t PPageOptPageBoss::GetFreeListCapacity() const noexcept
{
    PBlockHeadPageBoss *const pBossHead{_GetHeadBoss(true)};
    return static_cast< int32_t >(pBossHead->sFreeListCount);
}
/// @brief Check whether the capacity of the BossPage can still accommodate a new element
/// @return
bool PPageOptPageBoss::CheckBossPageCapacity() const noexcept
{
    PBlockHeadPageBoss *const pBossHead{_GetHeadBoss(true)};
    uint16_t const nInitArrayCount{T_GetArrayCount< PBlockHeadPageBoss >()};
    if (pBossHead->sElementCount >= nInitArrayCount) {
        return _CheckPageCapacity(sizeof(PBlockElementFreePage));
    }
    return true;
}
/// @brief Get the GroupID in the BlockBoss header
/// @return
uint16_t PPageOptPageBoss::GetGroupID() const noexcept
{
    PBlockHeadPageBoss *const pBossHead{_GetHeadBoss(true)};
    return pBossHead->sGroupID;
}
/// @brief Find a free page, return the page number (-1 on failure, 0 indicates need to create a new one)
/// @param nNeedSpace
/// @return
uint16_t PPageOptPageBoss::FindFreePageID(uint16_t const nNeedSpace) const noexcept
{
    POptBossBlock const optBossBlock{_GetHeadBoss(false)};
    if (false == optBossBlock.IsValid()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return 0U;
    }
    // Must add one more tier to ensure it can be saved
    int32_t nListType{_ComputeFreeListIndex(nNeedSpace) + 1};
    while (optBossBlock.CheckFreeListIndex(nListType)) {
        uint16_t const nFirstPageID{optBossBlock.GetFreeListData(nListType)};
        if (false
            == PPageOptBase::IsValidPageID(static_cast< uint32_t >(nFirstPageID)))  // Cannot use 32-bit null check here
        {
            nListType += 1;
            continue;
        }
        return nFirstPageID;
    }
    if (false == optBossBlock.CheckElementCount()) {
        _OnOptFailed(EErrorPHKV::kPageLackSpace);
    }
    return static_cast< uint16_t >(kInvalidPageID);
}
/// @brief Find the page management data area
/// @return
PBlockHeadPageBoss *PPageOptPageBoss::GetHeadBoss() const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (nullptr == pBlockHead) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return nullptr;
    }

    _OnOptSuccess(false);
    return T_TransPtr< PBlockHeadPageBoss >(pBlockHead);
}
//********************************/
/// @brief Calculate the free list index
/// @param nFreeLen
/// @return
int32_t PPageOptPageBoss::_ComputeFreeListIndex(uint16_t const nFreeLen) const noexcept
{
    // The larger the free list index, the higher the free rate: 0 means completely full, N means completely free
    PBlockHeadPageBoss *const pBossHead{_GetHeadBoss(true)};
    if (0 == static_cast< int32_t >(pBossHead->sFreeListCount)) {
        return 0;
    }
    uint16_t const nMaxFreeLen{_GetMaxFreeLen(*GetPageHead())};
    uint16_t const nMinFreeLen{static_cast< uint16_t >(nMaxFreeLen / pBossHead->sFreeListCount / 10U)};
    if (nFreeLen < nMinFreeLen)  // For a 4K page, this is 20 bytes
    {
        return 0;
    }
    if (nFreeLen >= nMaxFreeLen) {
        return static_cast< int32_t >(pBossHead->sFreeListCount);
    }
    // The remaining grouping is N - 1
    int32_t const nNewCount{static_cast< int32_t >(pBossHead->sFreeListCount) - 1};
    // Step size must be +1
    int32_t const nFreeStep{
        (static_cast< int32_t >(nMaxFreeLen) - static_cast< int32_t >(nMinFreeLen) + (nNewCount - 1)) / nNewCount};
    return (static_cast< int32_t >(nFreeLen) - static_cast< int32_t >(nMinFreeLen) + nFreeStep - 1) / nFreeStep;
}
/// @brief Find the page management data area
/// @param bCheck Whether to execute PER_Assert check
/// @return
PBlockHeadPageBoss *PPageOptPageBoss::_GetHeadBoss(bool const bCheck) const noexcept
{
    uint8_t *const pBossHead{_GetMineBlockData()};
    if (bCheck) {
        PER_Assert(nullptr != pBossHead);
    }
    return T_TransPtr< PBlockHeadPageBoss >(pBossHead);
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
