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
/// @file       manager_value.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Manage individual Value pages
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Value Page Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManagerValue
/// @unit_description=Manage individual Value pages
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-30 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/manager_value.h"

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param managerCache
/// @param managerPage
PManagerValue::PManagerValue(PManagerCache& managerCache, PManagerPage& managerPage) noexcept
    : managerCache_{managerCache}, managerPage_{managerPage}
{
}
/// @brief Check if PManagerValue is ready for read/write
/// @return
bool PManagerValue::IsAccessReady() const noexcept
{
    // The reason for not checking pageOptWalog_ is: in read-only mode, pageOptWalog_ is never enabled
    return managerCache_.IsAccessReady();
}
/// @brief Get the KV library name displayed in the log system
/// @return
ara::core::StringView PManagerValue::GetLogKvName() const noexcept { return std::move(managerCache_.GetLogKvName()); }
//***************/
/// @brief Save Local data
/// @param pBData
/// @param nDataLen
/// @param eDataType
/// @param pSavePageValue
/// @return
PBlockIndex PManagerValue::SetBlockValue(uint8_t const* const pBData,
                                         int32_t const nDataLen,
                                         EDataType const eDataType,
                                         PCachePagePtr const& pSavePageValue) noexcept
{
    PBlockIndex blockIndex;
    PPageOptValue pageOptValue{pSavePageValue};
    if (false == pageOptValue.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageValue);
        return blockIndex;
    }
    blockIndex.nPageID = pageOptValue.GetPageID();
    blockIndex.sBlockID
        = pageOptValue.AddBlock(EBlockType::kValue, pBData, static_cast< uint16_t >(nDataLen), nullptr, eDataType);
    PER_Assert(managerPage_.UpdateFreeList(pageOptValue));

    _LogDebug("[KV_Value].SetBlockValue", ":", "PageID", static_cast< int32_t >(blockIndex.nPageID), "BlockID",
              static_cast< int32_t >(blockIndex.sBlockID), "nDataLen", nDataLen);
    return blockIndex;
}
/// @brief Save external page data
/// @param pBData
/// @param nDataLen
/// @param eDataType
/// @param nCurPageID
/// @return
PBlockIndex PManagerValue::SetBlockValue(uint8_t const* const pBData,
                                         int32_t const nDataLen,
                                         EDataType const eDataType,
                                         uint32_t const nCurPageID) noexcept
{
    PBlockIndex blockReturn;
    PPageOptValue optValuePrev{nullptr};
    uint16_t nMaxValueValidLen{static_cast< uint16_t >(managerCache_.GetPageLen() - optValuePrev.GetMinValidLen())};
    // Cross-page
    if (nDataLen > static_cast< int32_t >(nMaxValueValidLen)) {
        PBlockSpread blockSpreadFirst;
        blockSpreadFirst.nSpreadID  = 0U;
        blockSpreadFirst.nTotalLen  = static_cast< uint32_t >(nDataLen);
        blockSpreadFirst.nZoneTotal = static_cast< uint16_t >(
            (static_cast< uint32_t >(nDataLen) + static_cast< uint32_t >(nMaxValueValidLen) - 1U)
            / static_cast< uint32_t >(nMaxValueValidLen));
        blockSpreadFirst.nZoneIndex = 0U;
        nMaxValueValidLen = static_cast< uint16_t >(nMaxValueValidLen - static_cast< uint16_t >(sizeof(PBlockSpread)));
        PBlockSpread* pSpreadPrev{nullptr};
        PBlockIndex prevBlock;
        int32_t nIndex{0};
        while (true) {
            if (nIndex >= nDataLen) {
                break;
            }
            int32_t const nSaveLen{std::min< int32_t >(nDataLen - nIndex, static_cast< int32_t >(nMaxValueValidLen))};
            uint16_t const nNeedSpace{
                static_cast< uint16_t >(static_cast< uint32_t >(nSaveLen) + sizeof(PBlockSpread))};
            PCachePagePtr pNewPageValue{managerPage_.PreparePageValue(nCurPageID, nNeedSpace)};
            PPageOptValue optValueWork{pNewPageValue};
            if (false == optValueWork.IsAccessReady()) {
                PER_OnOptFailed(managerPage_.GetLastError());
                return blockReturn;
            }
            if (blockSpreadFirst.nSpreadID <= 0U)  // nSpreadID takes the PageID of the first page of Value
            {
                blockSpreadFirst.nSpreadID = (*pNewPageValue)->GetMainPageID();
            }
            uint32_t const nPageID{optValueWork.GetPageID()};
            uint8_t const* pBDataSrc{pBData};
            if (nullptr != pBData) {
                pBDataSrc = pBData + nIndex;
            }
            uint16_t const nBlockID{optValueWork.AddBlock(
                EBlockType::kValue, pBDataSrc, static_cast< uint16_t >(nSaveLen), &blockSpreadFirst, eDataType)};
            // Maintain SpreadBlock doubly linked list
            PBlockSpread* const pSpreadWork{optValueWork.FindBlockSpread(nBlockID)};
            if (nullptr != pSpreadPrev) {
                pSpreadPrev->nextBlock.nPageID  = nPageID;
                pSpreadPrev->nextBlock.sBlockID = nBlockID;
            }
            if (nullptr != pSpreadWork) {
                pSpreadWork->prevBlock = prevBlock;
            }
            pSpreadPrev        = pSpreadWork;
            prevBlock.nPageID  = nPageID;
            prevBlock.sBlockID = nBlockID;
            PER_Assert(managerPage_.UpdateFreeList(optValueWork));
            // Ensure the previous page's data is in memory
            std::ignore = optValuePrev.InitCachePage(pNewPageValue);
            // Save the returned BlockIndex
            if ((false == PPageOptBase::IsValidPageID(blockReturn.nPageID)) && (blockReturn.sBlockID <= 0U)) {
                blockReturn.nPageID  = nPageID;
                blockReturn.sBlockID = nBlockID;
            }
            _LogDebug("[KV_Value].SetBlockValue", ":", "PageID", static_cast< int32_t >(nPageID), "BlockID",
                      static_cast< int32_t >(nBlockID), "nDataLen", nSaveLen, "nZoneIndex",
                      static_cast< int32_t >(blockSpreadFirst.nZoneIndex));
            blockSpreadFirst.nZoneIndex += 1U;
            nIndex += nSaveLen;
        }
    } else {
        PCachePagePtr const pNewPageValue{managerPage_.PreparePageValue(nCurPageID, static_cast< uint16_t >(nDataLen))};
        PPageOptValue optValueWork{pNewPageValue};
        if (false == optValueWork.IsAccessReady()) {
            return blockReturn;
        }
        uint16_t const nBlockID{
            optValueWork.AddBlock(EBlockType::kValue, pBData, static_cast< uint16_t >(nDataLen), nullptr, eDataType)};
        PER_Assert(managerPage_.UpdateFreeList(optValueWork));
        if ((false == PPageOptBase::IsValidPageID(blockReturn.nPageID)) && (blockReturn.sBlockID <= 0U)) {
            blockReturn.nPageID  = optValueWork.GetPageID();
            blockReturn.sBlockID = nBlockID;
        }
        _LogDebug("[KV_Value].SetBlockValue", ":", "PageID", static_cast< int32_t >(blockReturn.nPageID), "BlockID",
                  static_cast< int32_t >(blockReturn.sBlockID), "nDataLen", nDataLen);
    }
    return blockReturn;
}
/// @brief Delete the Value value
/// @param blockFirst
/// @return
bool PManagerValue::DelBlockValue(PBlockIndex const& blockFirst) noexcept
{
    std::ignore
        = _ForEachSpreadData(blockFirst, [this](PPageOptValue& pageOptValue, uint16_t const nBlockID) -> PBlockIndex {
              std::ignore = managerPage_.MoveCachePageToWalog(pageOptValue.GetCachePage());
              PBlockIndex blockIndex;
              PBlockSpread* const pBlockSpread{pageOptValue.FindBlockSpread(nBlockID)};
              if (nullptr != pBlockSpread) {
                  blockIndex = pBlockSpread->nextBlock;
              }
              // Delete this Block
              if (false == pageOptValue.DelBlock(nBlockID)) {
                  LogError() << "[PManagerValue::DelBlockValue] Delete Block Error : nBlockID =" << nBlockID;
              }
              // Maintain free space
              if (false == managerPage_.UpdateFreeList(pageOptValue)) {
                  LogError() << "[PManagerValue::DelBlockValue] Delete Block Error : nBlockID =" << nBlockID;
              }
              return blockIndex;
          });

    return true;
}
/// @brief Read data: return value is the total data length
/// @param blockFirst
/// @param pBuff
/// @param nMaxBuffLen
/// @return
int32_t PManagerValue::GetBlockValue(PBlockIndex const& blockFirst,
                                     uint8_t* const pBuff,
                                     int32_t const nMaxBuffLen) const noexcept
{
    int32_t nReadTotal{0};
    std::ignore = _ForEachSpreadData(
        blockFirst,
        [pBuff, nMaxBuffLen, &nReadTotal](PPageOptValue& pageOptValue,
                                          uint16_t const nBlockID) noexcept -> PBlockIndex {
            PValueData const valueData{pageOptValue.GetValueData(nBlockID)};
            if (nullptr != valueData.pData) {
                if ((nullptr != pBuff) && ((nReadTotal + static_cast< int32_t >(valueData.sDataLen)) <= nMaxBuffLen)) {
                    pageOptValue.CopyData(pBuff + nReadTotal, valueData.pData, valueData.sDataLen);
                }
                nReadTotal += static_cast< int32_t >(valueData.sDataLen);
            }
            PBlockIndex blockNext;
            if ((nReadTotal > nMaxBuffLen) && (nMaxBuffLen > 0)) {
                return blockNext;
            }
            if (nullptr != valueData.pSpread) {
                blockNext = valueData.pSpread->nextBlock;
            }
            return blockNext;
        });

    return nReadTotal;
}
/// @brief Read data: return value is the total data length
/// @param blockFirst
/// @param pfun
/// @return
int32_t PManagerValue::GetBlockValue(PBlockIndex const& blockFirst,
                                     PPageOptBase::CB_ReadData const& pfun) const noexcept
{
    int32_t nReadTotal{0};
    std::ignore = _ForEachSpreadData(
        blockFirst, [&pfun, &nReadTotal](PPageOptValue& pageOptValue, uint16_t const nBlockID) -> PBlockIndex {
            PBlockIndex blockNext;
            PValueData const valueData{pageOptValue.GetValueData(nBlockID)};
            if (nullptr != valueData.pData) {
                int32_t const nCopyLen{pfun(static_cast< int32_t >(valueData.cDataType), valueData.pData,
                                            static_cast< int32_t >(valueData.sDataLen), pageOptValue.GetPageID(),
                                            nBlockID)};
                if (nCopyLen < 0)  // Stop reading if an error occurs
                {
                    return blockNext;
                }
                nReadTotal += nCopyLen;
            }
            if (nullptr != valueData.pSpread) {
                blockNext = valueData.pSpread->nextBlock;
            }
            return blockNext;
        });

    return nReadTotal;
}
/// @brief Get the total length of the Value corresponding to BlockIndex
/// @param blockFirst
/// @param pReturnDataType
/// @return
int32_t PManagerValue::GetValueTotalLen(PBlockIndex const& blockFirst, int32_t* const pReturnDataType) noexcept
{
    if ((false == PPageOptBase::IsValidPageID(blockFirst.nPageID)) || (blockFirst.sBlockID <= 0U)) {
        PER_OnOptFailed(EErrorPHKV::kKvElementValue);
        return -1;
    }
    PCachePagePtr const pPageValue{managerPage_.LoadCachePage(blockFirst.nPageID, false)};
    if (nullptr == pPageValue) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return -1;
    }
    PPageOptValue optValue{pPageValue};
    if (false == optValue.IsAccessReady()) {
        PER_OnOptFailed(optValue.GetLastError());
        return -1;
    }
    // Note the type conversion here: unsigned to signed
    uint32_t const nReturn{optValue.GetValueTotalLen(blockFirst.sBlockID, pReturnDataType)};
    if (optValue.IsHaveError()) {
        PER_OnOptFailed(optValue.GetLastError());
        return -1;
    }
    return static_cast< int32_t >(nReturn);
}
//********************************/
/// @brief Iterate over the list of external Value data
/// @param blockFirst
/// @param pfun
/// @return
bool PManagerValue::_ForEachSpreadData(
    PBlockIndex const& blockFirst,
    std::function< PBlockIndex(PPageOptValue& pageOptValue, uint16_t const nBlockID) > const& pfun) const noexcept
{
    if ((false == PPageOptBase::IsValidPageID(blockFirst.nPageID)) || (blockFirst.sBlockID <= 0U)) {
        PER_OnOptFailed(EErrorPHKV::kKvElementValue);
        return false;
    }
    PBlockIndex blockWork{blockFirst};
    while (PPageOptBase::IsValidPageID(blockWork.nPageID)) {
        PCachePagePtr const pPageValue{managerPage_.LoadCachePage(blockWork.nPageID, false)};
        if (nullptr == pPageValue) {
            return false;
        }
        PPageOptValue pageOptValue{pPageValue};
        if (false == pageOptValue.IsAccessReady()) {
            PER_OnOptFailed(pageOptValue.GetLastError());
            return false;
        }
        blockWork = pfun(pageOptValue, blockWork.sBlockID);
        if (pageOptValue.IsHaveError()) {
            PER_OnOptFailed(pageOptValue.GetLastError());
            return false;
        }
    }

    _OnSuccess();
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
