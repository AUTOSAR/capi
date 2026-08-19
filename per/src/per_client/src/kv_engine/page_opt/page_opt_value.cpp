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
/// @file       page_opt_value.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Raw data
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Value Page Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptValue
/// @unit_description=Value data page
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

#include "ara/per/internal/isoftkv/page_opt_value.h"

#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param pCachePagePtr
PPageOptValue::PPageOptValue(PCachePagePtr const &pCachePagePtr) noexcept : PPageOptBase{pCachePagePtr} {}
/// @brief
/// @return
bool PPageOptValue::IsAccessReady() const noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        return false;
    }
    return IsHavePageType();
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptValue::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    return static_cast< uint16_t >(nMinValidLen);
}
/// @brief
/// @return
EBlockType PPageOptValue::PageClassType() const noexcept { return EBlockType::kValue; }
//***************/

/// @brief
/// @return
bool PPageOptValue::InitValueHead() noexcept
{
    bool const bReturn{InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID())};
    if (false == bReturn) {
        return false;
    }

    _LogDebug("[KV_Opt].InitValueHead", ":", "PageID", static_cast< int32_t >(GetPageID()));
    _OnOptSuccess(true);
    return bReturn;
}
/// @brief Add a ValueBlock data block: spanning pages
/// @param pBData
/// @param nDataLen
/// @param pSpreadBlock
/// @param eDataType
/// @return Return value is the BlockID
uint16_t PPageOptValue::AddValueBlock(uint8_t const *const pBData,
                                      uint16_t const nDataLen,
                                      PBlockSpread const *const pSpreadBlock,
                                      EDataType const eDataType) noexcept
{
    return AddBlock(PPageOptValue::PageClassType(), pBData, nDataLen, pSpreadBlock, eDataType);
}
/// @brief Delete a ValueBlock data block
/// @param nBlockID
/// @return
bool PPageOptValue::DelValueBlock(uint16_t const nBlockID) noexcept { return DelBlock(nBlockID); }
/// @brief Get the value of Value
/// @param nBlockID
/// @return
PValueData PPageOptValue::GetValueData(uint16_t const nBlockID) const noexcept
{
    PValueData valueData;
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageValue);
        return valueData;
    }
    PBlockHead const *const pFindBlock{_FindBlock(PPageOptValue::PageClassType(), nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return valueData;
    }
    valueData.cDataType = pFindBlock->cDataType;
    uint8_t *const pDataFind{_GetDataByOffset(pFindBlock->sOffset)};
    if (S_IsHaveSpread(*pFindBlock)) {
        valueData.sDataLen = pFindBlock->sLen - static_cast< uint16_t >(sizeof(PBlockSpread));
        valueData.pSpread  = T_TransPtr< PBlockSpread >(pDataFind);
        valueData.pData    = pDataFind + sizeof(PBlockSpread);
    } else {
        valueData.sDataLen = pFindBlock->sLen;
        valueData.pSpread  = nullptr;
        valueData.pData    = pDataFind;
    }
    return valueData;
}
/// @brief Find cross-page data
/// @param nBlockID
/// @return
PBlockSpread *PPageOptValue::FindBlockSpread(uint16_t const nBlockID) noexcept
{
    return PPageOptBase::FindBlockSpread(PPageOptValue::PageClassType(), nBlockID);
}
/// @brief Get the total length of Value
/// @param nBlockID
/// @param pReturnDataType
/// @return
uint32_t PPageOptValue::GetValueTotalLen(uint16_t const nBlockID, int32_t *const pReturnDataType) const noexcept
{
    uint32_t const nTotalLen{GetBlockTotalLen(PPageOptValue::PageClassType(), nBlockID)};
    if ((nTotalLen > 0U) && (nullptr != pReturnDataType)) {
        PBlockHead const *const pFindBlock{_FindBlock(PPageOptValue::PageClassType(), nBlockID)};
        *pReturnDataType = static_cast< int32_t >(pFindBlock->cDataType);
    }
    return nTotalLen;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
