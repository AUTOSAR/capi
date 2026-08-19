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
/// @file       page_opt_page_group.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Page group management
/// @date       2021-05-26
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptPageGroup
/// @unit_description=Logical page for parsing page group management
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-26 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_page_group.h"

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace {
/// @brief
/// @param pBlockHead
/// @param nElementIndex
/// @return
ara::per::isoftkv::PBlockElementGroup &G_GetBlockElement(ara::per::isoftkv::PBlockHeadPageGroup *const pBlockHead,
                                                         int32_t const nElementIndex) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    return *(pBlockHead->arrayElement + nElementIndex);
}
/// @brief
/// @param pBlockHead
/// @param nElementIndex
/// @return
ara::per::isoftkv::PBlockElementGroup const &G_GetBlockElement(
    ara::per::isoftkv::PBlockHeadPageGroup const *const pBlockHead, int32_t const nElementIndex) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    return *(pBlockHead->arrayElement + nElementIndex);
}
/// @brief Calculate the group number (GroupID) based on PageID
/// @param nPageID PageID
/// @param nGroupCapacity Maximum group capacity
/// @return
uint16_t G_TransGroupID(uint32_t const nPageID, uint16_t const nGroupCapacity) noexcept
{
    return static_cast< uint16_t >(ara::per::isoftkv::TransPage_IDtoIndex(nPageID) / nGroupCapacity);
}
/// @brief Calculate the page sequence number within the group's Boss page based on PageID
/// @param nPageID PageID
/// @param nGroupCapacity Maximum group capacity
/// @return
uint16_t G_TransBossIndex(uint32_t const nPageID, uint16_t const nGroupCapacity) noexcept
{
    return static_cast< uint16_t >(ara::per::isoftkv::TransPage_IDtoIndex(nPageID) % nGroupCapacity);
}
}  // namespace
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param pCachePagePtr
/// @param nDefBlockID
PPageOptPageGroup::PPageOptPageGroup(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID) noexcept
    : PPageOptSolo{pCachePagePtr, EBlockType::kPageGroup, nDefBlockID}
{
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptPageGroup::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    nMinValidLen += sizeof(PBlockHeadPageGroup);
    return static_cast< uint16_t >(nMinValidLen);
}
//***************/
/// @brief Initialize GroupHead data
/// @param versionSystem
/// @param versionPer
/// @param nSpaceAmountMin
/// @param nSpaceAmountMax
/// @param nHashPageID
/// @param nPageInitLen
/// @return
bool PPageOptPageGroup::InitGroupHead(PPerVersion const versionSystem,
                                      PPerVersion const versionPer,
                                      uint32_t const nSpaceAmountMin,
                                      uint32_t const nSpaceAmountMax,
                                      uint32_t const nHashPageID,
                                      uint16_t const nPageInitLen) noexcept
{
    if (false == InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return false;
    }
    uint16_t const nMinBlockLen{static_cast< uint16_t >(sizeof(PBlockHeadPageGroup))};
    // 2023-08-24 hanjingjing, modified PageGroup's Block to single-page mode (multi-page mode is not yet supported)
    if (false == InitSoloBlock(EStoreRange::kSingle, nMinBlockLen)) {
        return false;
    }
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    pGroupHead->versionSystem   = versionSystem;
    pGroupHead->versionPer      = versionPer;
    pGroupHead->nSpaceAmountMin = nSpaceAmountMin;
    pGroupHead->nSpaceAmountMax = nSpaceAmountMax;
    pGroupHead->nHashPageID     = static_cast< uint32_t >(EDefaultValue::kDefStartPageID);
    uint16_t nMaxGroupCount{
        static_cast< uint16_t >((GetPageHead()->sPageLen - GetMinValidLen()) / sizeof(PBlockElementGroup))};
    nMaxGroupCount += static_cast< uint16_t >(EDefaultValue::kInitGroupCount);
    pGroupHead->sGroupCapacity = std::min(static_cast< uint16_t >(EDefaultValue::kDefMaxPageInGroup), nMaxGroupCount);
    // DEBUG
    // pGroupHead->sGroupCapacity = 10U;
    pGroupHead->sPageInitLen  = nPageInitLen;
    pGroupHead->nPageTotal    = 1U;  // Includes this page itself, so at least one page
    pGroupHead->sElementCount = 0U;  //
    for (auto &elementData : pGroupHead->arrayElement) {
        InitElement(elementData, 0U, 0U, 0U);
    }
    _LogDebug("[KV_Opt].InitGroupHead", ":", "PageID", static_cast< int32_t >(GetPageID()), "nHashPageID",
              static_cast< int32_t >(nHashPageID), "nPageInitLen", static_cast< int32_t >(nPageInitLen),
              "nSpaceAmountMin", static_cast< int32_t >(nSpaceAmountMin), "nSpaceAmountMax",
              static_cast< int32_t >(nSpaceAmountMax));
    _OnOptSuccess(true);
    return true;
}
/// @brief Add a GroupElement node
/// @param nBossPageID
/// @param nFreePageCount
/// @param nMaxFreeIndex
/// @return
bool PPageOptPageGroup::AddGroupElement(uint32_t const nBossPageID,
                                        uint16_t const nFreePageCount,
                                        uint16_t const nMaxFreeIndex) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    PBlockHeadPageGroup *pGroupHead{_GetHeadGroup(true)};
    uint16_t const nInitArrayCount{T_GetArrayCount< PBlockHeadPageGroup >()};
    uint16_t const nBossID{TransBossPageIndex(nBossPageID)};
    if (pGroupHead->sElementCount >= nInitArrayCount) {
        if (false == _CheckPageCapacity(sizeof(PBlockElementGroup))) {
            _OnOptFailed(EErrorPHKV::kPageLackSpace);
            return false;
        }
        PBlockElementGroup elementGroup;
        InitElement(elementGroup, nBossID, nFreePageCount, nMaxFreeIndex);
        if (false == AddBlockData(GetSoloBlockID(), T_TransBytes(&elementGroup), sizeof(elementGroup))) {
            PER_OnOptFailed(EErrorPHKV::kPageAddBlock);
            return false;
        }
    } else {
        PBlockElementGroup &elementData{
            G_GetBlockElement(pGroupHead, static_cast< int32_t >(pGroupHead->sElementCount))};
        InitElement(elementData, nBossID, nFreePageCount, nMaxFreeIndex);
    }
    // The storage location may have changed, so re-obtain this value
    pGroupHead = _GetHeadGroup(true);
    pGroupHead->sElementCount += static_cast< uint16_t >(1);

    _OnOptSuccess(true);
    return true;
}
/// @brief Modify a data item within a group
/// @param nPageID
/// @param nNewFreeCount
/// @param nMaxFreeIndex
/// @return
bool PPageOptPageGroup::AmendGroupData(uint32_t const nPageID,
                                       uint16_t const nNewFreeCount,
                                       uint16_t const nMaxFreeIndex) const noexcept
{
    PBlockElementGroup *const pGroupElement{FindGroupElement(nPageID)};
    if (nullptr == pGroupElement) {
        return false;
    }
    pGroupElement->sFreePageCount = nNewFreeCount;
    pGroupElement->sMaxFreeIndex  = nMaxFreeIndex;
    _OnOptSuccess(true);
    return true;
}
/// @brief
/// @param nPageID
/// @param nAddCount
/// @return
bool PPageOptPageGroup::AddFreeCount(uint32_t const nPageID, int32_t const nAddCount) const noexcept
{
    PBlockElementGroup *const pGroupElement{FindGroupElement(nPageID)};
    if (nullptr == pGroupElement) {
        return false;
    }
    pGroupElement->sFreePageCount = T_AddInt(static_cast< uint16_t >(pGroupElement->sFreePageCount), nAddCount);
    _OnOptSuccess(true);
    return true;
}
/// @brief
/// @param nPageID
/// @param nAddCount
/// @return
bool PPageOptPageGroup::UpdateMaxFreeIndex(uint32_t const nPageID, int32_t const nAddCount) const noexcept
{
    PBlockElementGroup *const pGroupElement{FindGroupElement(nPageID)};
    if (nullptr == pGroupElement) {
        return false;
    }
    pGroupElement->sMaxFreeIndex = T_AddInt(static_cast< uint16_t >(pGroupElement->sMaxFreeIndex), nAddCount);
    _OnOptSuccess(true);
    return true;
}
/// @brief Update BossPageID
/// @param nOldBossPageID
/// @param nNewBossPageID
/// @return
bool PPageOptPageGroup::UpdateBossPageID(uint16_t const nOldBossPageID, uint16_t const nNewBossPageID) const noexcept
{
    PBlockElementGroup *const pGroupElement{FindGroupElement(static_cast< uint32_t >(nOldBossPageID))};
    if (nullptr == pGroupElement) {
        return false;
    }
    uint16_t const sOldBossID{TransBossPageIndex(static_cast< uint32_t >(nOldBossPageID))};
    if (pGroupElement->sBossPageIndex != sOldBossID) {
        PER_OnOptFailed(EErrorPHKV::kErrorBossID);
        return false;
    }
    pGroupElement->sBossPageIndex = TransBossPageIndex(static_cast< uint32_t >(nNewBossPageID));
    _OnOptSuccess(true);
    return true;
}
/// @brief
/// @param nPageID
/// @return
PBlockElementGroup *PPageOptPageGroup::FindGroupElement(uint32_t const nPageID) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return nullptr;
    }
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    uint16_t const nGroupID{G_TransGroupID(nPageID, pGroupHead->sGroupCapacity)};
    PBlockElementGroup *const pGroupElement{_FindGroupElement(static_cast< int32_t >(nGroupID))};
    if (nullptr == pGroupElement) {
        PER_OnOptFailed(EErrorPHKV::kIndexGroup);
        return nullptr;
    }
    _OnOptSuccess(false);
    return pGroupElement;
}
//***************/
/// @brief
/// @return
uint32_t PPageOptPageGroup::GetPageTotal() const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    return pGroupHead->nPageTotal;
}
/// @brief Get the page number for creating a new page
/// @return
uint32_t PPageOptPageGroup::GetNewPageID() const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    return pGroupHead->nPageTotal + 1U;
}
/// @brief
/// @return
uint16_t PPageOptPageGroup::GetPageInitLen() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    return pHeadGroup->sPageInitLen;
}
/// @brief
/// @return
uint16_t PPageOptPageGroup::GetGroupCapacity() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    return pHeadGroup->sGroupCapacity;
}
/// @brief Get the capacity limit
/// @return
uint32_t PPageOptPageGroup::GetSpaceAmountMax() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    return pHeadGroup->nSpaceAmountMax;
}
/// @brief
/// @return
uint16_t PPageOptPageGroup::GetGroupCount() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    return pHeadGroup->sElementCount;
}
/// @brief
/// @return
uint32_t PPageOptPageGroup::GetHashPageID() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    return pHeadGroup->nHashPageID;
}
/// @brief
/// @param nHashPageID
void PPageOptPageGroup::SetHashPageID(uint32_t const nHashPageID) const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    pHeadGroup->nHashPageID = nHashPageID;
    _OnOptSuccess(true);
}
/// @brief Set HashPageID
void PPageOptPageGroup::IncreasePageTotal() const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    pHeadGroup->nPageTotal += 1U;
    _OnOptSuccess(true);
}
/// @brief Get the GroupID via PageID
/// @param nPageID
/// @return
uint16_t PPageOptPageGroup::TransGroupID(uint32_t const nPageID) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return 0U;
    }
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    return G_TransGroupID(
        nPageID, pGroupHead->sGroupCapacity);  // 2021-06-16 GroupIndex and GroupID are consistent, starting from 0
}
/// @brief Convert a 16-bit BossID to a 32-bit value
/// @param nGroupID
/// @param nPageID16
/// @return
uint32_t PPageOptPageGroup::TransPageID(uint16_t const nGroupID, uint16_t const nPageIndex) const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(false)};
    if (nullptr == pGroupHead) {
        return static_cast< uint32_t >(-1);
    }
    return static_cast< uint32_t >(nGroupID) * static_cast< uint32_t >(pGroupHead->sGroupCapacity)
           + TransPage_IndexToID(static_cast< uint32_t >(nPageIndex));
}
/// @brief Convert a 32-bit PageID to the 16-bit page sequence number within its group
/// @param nPageID32
/// @return
uint16_t PPageOptPageGroup::TransBossPageIndex(uint32_t const nPageID32) const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(false)};
    if (nullptr == pGroupHead) {
        return static_cast< uint16_t >(-1);
    }
    return G_TransBossIndex(nPageID32, pGroupHead->sGroupCapacity);
}
/// @brief Find the PageID of the PageBoss to which nCurPageID belongs
/// @param nCurPageID
/// @return PageID of the found BossPage
uint32_t PPageOptPageGroup::FindBossPageID(uint32_t const nCurPageID) const noexcept
{
    uint16_t const nCurGroupID{TransGroupID(nCurPageID)};
    PBlockHeadPageGroup const *const pGroupHead{_GetHeadGroup(false)};
    if (nullptr == pGroupHead) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return 0U;
    }
    if (nCurGroupID >= pGroupHead->sElementCount) {
        return 0U;
    }
    uint16_t const nGroupIndex{static_cast< uint16_t >((nCurGroupID) % (pGroupHead->sElementCount))};
    PBlockElementGroup const &elementData{G_GetBlockElement(pGroupHead, static_cast< int32_t >(nGroupIndex))};
    return TransPageID(nGroupIndex, elementData.sBossPageIndex);
}
/// @brief Get the BossID of the group to which the given PageID belongs
/// @param nPageID
/// @return
uint32_t PPageOptPageGroup::GetBossPageID(uint32_t const nPageID) const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    uint16_t const nGroupID{G_TransGroupID(nPageID, pGroupHead->sGroupCapacity)};
    PBlockElementGroup *const pGroupElement{_FindGroupElement(static_cast< int32_t >(nGroupID))};
    if (nullptr == pGroupElement) {
        return kInvalidPageID;
    }
    return TransPageID(nGroupID, pGroupElement->sBossPageIndex);
}
/// @brief Find the page management data area
/// @return
PBlockHeadPageGroup *PPageOptPageGroup::GetHeadGroup() const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (nullptr == pBlockHead) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return nullptr;
    }
    _OnOptSuccess(false);
    return T_TransPtr< PBlockHeadPageGroup >(pBlockHead);
}
/// @brief Get the PER format version number information saved in the library: persistent data
/// @return
PPerVersion const &PPageOptPageGroup::GetPerVersion_Per() const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    return pGroupHead->versionPer;
}
/// @brief Get the PER format version number information saved in the library: KvSystem
/// @return
PPerVersion const &PPageOptPageGroup::GetPerVersion_Sys() const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    return pGroupHead->versionSystem;
}
/// @brief Set the PER format version number information saved in the library: persistent data
/// @param version
void PPageOptPageGroup::SetPerVersion_Per(PPerVersion const &version) const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    pHeadGroup->versionPer = version;
    _OnOptSuccess(true);
}
/// @brief Set the PER format version number information saved in the library: KvSystem
/// @param version
void PPageOptPageGroup::SetPerVersion_System(PPerVersion const &version) const noexcept
{
    PBlockHeadPageGroup *const pHeadGroup{_GetHeadGroup(true)};
    pHeadGroup->versionSystem = version;
    _OnOptSuccess(true);
}
//********************************/
/// @brief Get GroupHead
/// @param bCheck Whether to execute PER_Assert
/// @return
PBlockHeadPageGroup *PPageOptPageGroup::_GetHeadGroup(bool const bCheck) const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (bCheck) {
        PER_Assert(nullptr != pBlockHead);
    }
    return T_TransPtr< PBlockHeadPageGroup >(pBlockHead);
}
/// @brief
/// @param nGroupID
/// @return
PBlockElementGroup *PPageOptPageGroup::_FindGroupElement(int32_t const nGroupID) const noexcept
{
    PBlockHeadPageGroup *const pGroupHead{_GetHeadGroup(true)};
    if (false == S_CheckArrayIndex(*pGroupHead, nGroupID)) {
        return nullptr;
    }

    PBlockElementGroup &elementData{G_GetBlockElement(pGroupHead, nGroupID)};
    return &elementData;
}
/// @brief
/// @param blockHead
/// @param nIndex
/// @return
bool PPageOptPageGroup::S_CheckArrayIndex(PBlockHeadPageGroup const &blockHead, int32_t const nIndex) noexcept
{
    return (nIndex >= 0) && (nIndex < static_cast< int32_t >(blockHead.sElementCount));
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
