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
/// @file       page_opt_solo.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Base class for exclusive blocks (exclusive means at most one per page)
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=unit
/// @trace_id_sr=
/// @unit_name=PPageOptSolo
/// @unit_description=Key-Value storage page: Base class for exclusive blocks (exclusive means at most one per page)
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

#include "ara/per/internal/isoftkv/page_opt_solo.h"

#include <vector>

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @param pCachePagePtr
/// @param eBlockType
/// @param nDefBlockID
PPageOptSolo::PPageOptSolo(PCachePagePtr const &pCachePagePtr,
                           EBlockType const eBlockType,
                           uint16_t const nDefBlockID) noexcept
    : PPageOptBase{pCachePagePtr}
    , eSoloBlockType_{eBlockType}
    , nDefBlockID_{nDefBlockID}
    , pSoloBlock_{nullptr}
    , nSoloBlockID_{0U}
{
    if (nullptr != pCachePagePtr) {
        // Filter out error reports from uninitialized empty memory
        if (IsHavePageType(static_cast< uint8_t >(eSoloBlockType_))) {
            PBlockHead *const pFindBlock{_FindBlock(eSoloBlockType_, nDefBlockID_)};
            if (nullptr != pFindBlock) {
                nSoloBlockID_ = pFindBlock->sBlockID;
                if (IsSoloPage(eSoloBlockType_)) {
                    pSoloBlock_ = pFindBlock;
                }
            }
        }
    }
}
/// @brief Initialize Page data
/// @param pCachePagePtr
/// @return
bool PPageOptSolo::InitCachePage(PCachePagePtr const &pCachePagePtr) noexcept
{
    PER_Assert(nullptr != pCachePagePtr);
    // Prevent repeated calls to the base class AttachCachePage
    if ((false == PPageOptBase::IsAccessReady()) || (false == IsSameCachePage(pCachePagePtr))) {
        if (false == PPageOptBase::_InitCachePage_Base(pCachePagePtr)) {
            PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
            return false;
        }
    }
    if (false == _InitCachePage_Solo(pCachePagePtr)) {
        return false;
    }
    _OnOptSuccess(false);
    return PPageOptSolo::IsAccessReady();
}
/// @brief Check if ready for read/write
/// @return
bool PPageOptSolo::IsAccessReady() const noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        return false;
    }
    if (GetSoloBlockID() <= 0U) {
        return false;
    }
    if (nullptr == _GetMineBlockData()) {
        return false;
    }
    return IsHavePageType();
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptSolo::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    return static_cast< uint16_t >(nMinValidLen);
}
/// @brief
/// @return
inline EBlockType PPageOptSolo::PageClassType() const noexcept { return eSoloBlockType_; }
//***************/
/// @brief Reset the page, retaining the PageID
/// @param bResetData
/// @return
bool PPageOptSolo::ResetPage(bool const bResetData) noexcept
{
    _ResetData();
    return PPageOptBase::ResetPage(bResetData);
}
/// @brief Create an empty SoloBlock
/// @param eRangeType
/// @param nBlockDataLen
/// @return
bool PPageOptSolo::InitSoloBlock(EStoreRange const eRangeType, uint16_t const nBlockDataLen) noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PH_CheckDataLen(static_cast< int32_t >(nBlockDataLen), false);

    PBlockHead *const pFindBlock{_FindBlock(eSoloBlockType_, static_cast< uint16_t >(0U))};
    if (nullptr != pFindBlock) {
        if (pFindBlock->sLen == nBlockDataLen) {
            // Reuse old space directly: initialize all to 0
            S_ClearMemory(_GetDataByOffset(pFindBlock->sOffset), pFindBlock->sLen);
        } else if (pFindBlock->sLen > nBlockDataLen)  // Shrink table
        {
            uint16_t const nDecLen{static_cast< uint16_t >(pFindBlock->sLen - nBlockDataLen)};
            // Save itself first
            _BlockBodyDecData(pFindBlock, nDecLen);
            // Move subsequent nodes to new positions in forward order
            std::ignore = _MoveBlockList(pFindBlock, static_cast< int32_t >(nDecLen), false);
            _SyncPageFreeLen(static_cast< int32_t >(nDecLen));
        } else  // Expand table
        {
            uint16_t const nAddLen{static_cast< uint16_t >(nBlockDataLen - pFindBlock->sLen)};
            if (false == _CheckPageCapacity(nAddLen)) {
                _OnOptFailed(EErrorPHKV::kPageLackSpace);
                return false;
            }
            // Move subsequent nodes in reverse order first
            std::ignore = _MoveBlockList(pFindBlock, -1 * static_cast< int32_t >(nAddLen), true);
            // Then save itself
            _BlockBodyAddData(pFindBlock, nullptr, nAddLen);
            _SyncPageFreeLen(-1 * static_cast< int32_t >(nAddLen));
        }
    } else {
        uint16_t const nAddLen{static_cast< uint16_t >(S_GetBlockHeadLen(eRangeType) + nBlockDataLen)};
        if (false == _CheckPageCapacity(nAddLen)) {
            _OnOptFailed(EErrorPHKV::kPageLackSpace);
            return false;
        }
        PBlockHead *pNewBlock{nullptr};
        pNewBlock   = _AddBlockHead(eRangeType, eSoloBlockType_, nBlockDataLen);
        std::ignore = _BlockBodySaveData(*pNewBlock, nullptr, nBlockDataLen, nullptr);
        _SyncPageFreeLen(-1 * static_cast< int32_t >(nAddLen));
        std::ignore = _AttachSoloBlock();
    }

    _OnOptSuccess(true);
    return true;
}
/// @brief This design is no longer supported as of 2021-06-23, the interface is retained
/// @return
PBlockSpread *PPageOptSolo::FindBlockSpread() noexcept
{
    return PPageOptBase::FindBlockSpread(eSoloBlockType_, nSoloBlockID_);
}
//********************************/

/// @brief
void PPageOptSolo::_ResetData() noexcept
{
    pSoloBlock_   = nullptr;
    nSoloBlockID_ = 0U;
}
/// @brief Find the page management data area
/// @return
uint8_t *PPageOptSolo::_GetMineBlockData() const noexcept
{
    uint8_t *pFindBlockHead{nullptr};
    if (IsSoloPage(eSoloBlockType_) && (nullptr != pSoloBlock_)) {
        pFindBlockHead = _GetBlockDataHead(pSoloBlock_);
    } else {
        pFindBlockHead = _FindBlockData(GetSoloBlockID());
    }
    return pFindBlockHead;
}
/// @brief
/// @return
bool PPageOptSolo::_AttachSoloBlock() noexcept
{
    _ResetData();
    PBlockHead *const pFindBlock{_FindBlock(eSoloBlockType_, nDefBlockID_)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kErrorBlockID);
        return false;
    }
    nSoloBlockID_ = pFindBlock->sBlockID;
    if (IsSoloPage(eSoloBlockType_)) {
        pSoloBlock_ = pFindBlock;
    }
    return true;
}
/// @brief Initialize Page data
/// @param pCachePagePtr
/// @return
bool PPageOptSolo::_InitCachePage_Solo(PCachePagePtr const &pCachePagePtr) noexcept
{
    PER_Assert(nullptr != pCachePagePtr);
    // Filter out error reports from uninitialized empty memory
    if (IsHavePageType(static_cast< uint8_t >(eSoloBlockType_))) {
        if (false == _AttachSoloBlock()) {
            PER_OnOptFailed(EErrorPHKV::kErrorBlockID);
            return false;
        }
    }
    _OnOptSuccess(false);
    return true;
}
/// @brief Check if the remaining space meets the requirement
/// @param eRangeType
/// @param nBlockDataLen
/// @return
bool PPageOptSolo::_CheckBlockSpace(EStoreRange const eRangeType, uint16_t const nBlockDataLen) const noexcept
{
    return (S_GetBlockHeadLen(eRangeType) + nBlockDataLen) <= (GetPageHead()->sFreeLen);
}
/// @brief Get the length of the block control data
/// @param eRangeType
/// @return
uint16_t PPageOptSolo::S_GetBlockHeadLen(EStoreRange const eRangeType) noexcept
{
    // PBlockHead is placed at the head of the control area, while PBlockSpread is placed at the head of the data area
    uint16_t nHeadLen{static_cast< uint16_t >(sizeof(PBlockHead))};
    if (eRangeType == EStoreRange::kMulti) {
        nHeadLen += static_cast< uint16_t >(sizeof(PBlockSpread));
    }
    return nHeadLen;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
