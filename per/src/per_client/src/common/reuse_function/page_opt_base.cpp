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
/// @file       page_opt_base.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Key-Value storage page: Base class
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
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Logical page interpreter base class
/// @endcode
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_base.h"

#include "ara/core/map.h"
#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "isoft/crc/crc.h"

//********************************/
namespace {
/// @brief
using MapMagicWord = ara::core::Map< int32_t, ara::core::StringView >;
/// @brief Convert magic number used by KV from enum to string
/// @param mapMagicWord
void InitMagicWord(MapMagicWord &mapMagicWord) noexcept
{
    // All identifiers require length less than sizeof(PPageHeadData::cMagic) - 1
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kPageGroup)] = ara::core::StringView{"Group"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kPageBoss)]  = ara::core::StringView{"Boss"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kKeyHash)]   = ara::core::StringView{"Hash"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kKvStore)]   = ara::core::StringView{"KvStore"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kValue)]     = ara::core::StringView{"Value"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kExt)]       = ara::core::StringView{"Ext"};
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kWalog)]     = ara::core::StringView{"Walog"};
    // Appears during initialization
    mapMagicWord[static_cast< int32_t >(ara::per::isoftkv::EBlockType::kElse)] = ara::core::StringView{"Page"};
}
}  // namespace
//********************************/
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Check if given string is supported by KvPage.
/// @param stMagic
/// @return
bool IsKvPage(ara::core::StringView const &stMagic) noexcept
{
    MapMagicWord mapMagicWord;
    InitMagicWord(mapMagicWord);
    for (auto const it : mapMagicWord) {
        if (it.second == stMagic) {
            return true;
        }
    }
    return false;
}
/// @brief Convert magic number used by KvPage from enum to string
/// @param eType
/// @return
ara::core::StringView GetKvPageMagic(EBlockType const eType) noexcept
{
    MapMagicWord mapMagicWord;
    InitMagicWord(mapMagicWord);
    int32_t const nIndex{static_cast< int32_t >(eType)};
    MapMagicWord::iterator const itFind{mapMagicWord.find(nIndex)};
    if (mapMagicWord.end() == itFind) {
        return ara::core::StringView{""};
    }
    return mapMagicWord[nIndex];
}
/// @brief Convert nHashKeyID to index of Hash mapping array
/// @param nHashKeyID
/// @param nHashCount
/// @return
int32_t TransHashIndex(uint32_t const nHashKeyID, uint32_t const nHashCount) noexcept
{
    // //2021-07-30 Add debug code, manually control Hash value distribution
    uint32_t const nHashIndex{nHashKeyID % nHashCount};
    return static_cast< int32_t >(nHashIndex);
}
/// @brief Output version log
/// @param version
/// @return
ara::core::String FormatVersion(PPerVersion const &version) noexcept
{
    uint8_t chBuff[kInt_1024U]{0U};
    std::ignore = snprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), sizeof(chBuff), "%d.%d.%d.%d",
                           version.sVersionMajor, version.sVersionMinor, version.sVersionPatch, version.sVersionElse);
    if (0U == chBuff[0]) {  // This output parameter value is not subsequently checked.
        chBuff[0] = 0U;
    }
    return ara::core::String{T_TransChar(static_cast< uint8_t * >(chBuff))};
}
//********************************/
/// @brief
/// @param nPageID
/// @return
bool PPageOptBase::IsValidPageID(uint32_t const nPageID) noexcept { return nPageID != kInvalidPageID; }
/// @brief Copy data
/// @param pBDstData
/// @param pBSrcData
/// @param nDataLen
/// @return
bool PPageOptBase::MoveData(void *const pBDstData, const void *const pBSrcData, uint16_t const nDataLen) noexcept
{
    if ((nullptr == pBDstData) || (nullptr == pBSrcData)) {
        return false;
    }
    // Move amount too small, no need to move
    if (0U == nDataLen) {
        return true;
    }
    int32_t const nMoveLen{T_SubPtr(pBDstData, pBSrcData)};
    // Source and destination addresses are the same, no need to move
    if (0 == nMoveLen) {
        return true;
    }
    int32_t nLeftLen{static_cast< int32_t >(nDataLen)};
    while ((nLeftLen > nMoveLen) && (nMoveLen > 0))  // Handle possible overlap
    {
        std::ignore = T_Memcpy(T_TransBytes(pBDstData) + (nLeftLen - nMoveLen),
                               T_TransBytes(pBSrcData) + (nLeftLen - nMoveLen), static_cast< uint32_t >(nMoveLen));
        nLeftLen -= nMoveLen;
    }
    if (nLeftLen > 0) {
        std::ignore = T_Memcpy(pBDstData, pBSrcData, static_cast< uint32_t >(nLeftLen));
    }
    return true;
}
//********************************/
/// @brief Page data base class
/// @param pCachePage
PPageOptBase::PPageOptBase(PCachePagePtr const &pCachePage) noexcept : pCachePagePtr_{pCachePage}
{
    if (nullptr != pCachePage) {
        uint8_t *const pBData{(*pCachePage)->GetCache()};
        int32_t const nDataLen{static_cast< int32_t >((*pCachePage)->GetCacheLen())};
        if (true == PKvErrorCode::CheckParam(pBData, nDataLen)) {
            pMemoryPage_ = pCachePage->GetMemoryPage();
            pMemoryPage_->AttachOpt(this);
            // Verify data
            pPageHead_ = T_TransPtr< PPageHeadData >(pBData);
            if (pPageHead_->sCountBlock > 0U) {
                pBlockHead_ = T_TransPtr< PPageHeadData, PBlockHead >(pPageHead_ + 1);
            }
        }
    }
}
/// @brief
PPageOptBase::~PPageOptBase() noexcept { PPageOptBase::DetachCachePage(); }
/// @brief
/// @return
bool PPageOptBase::IsAccessReady() const noexcept { return _CheckBaseOpt(); }
/// @brief
/// @return
ara::core::StringView PPageOptBase::GetLogKvName() const noexcept
{
    if (nullptr == pCachePagePtr_) {
        return std::move(ara::core::StringView{""});
    }
    return std::move(pCachePagePtr_->GetLogKvName());
}
/// @brief Initialize Page data
/// @param pCachePage
/// @return
bool PPageOptBase::InitCachePage(PCachePagePtr const &pCachePage) noexcept { return _InitCachePage_Base(pCachePage); }
/// @brief
void PPageOptBase::DetachCachePage() noexcept
{
    if (nullptr != pCachePagePtr_) {
        if (nullptr != pMemoryPage_) {
            pMemoryPage_->DetachOpt(this);
            pMemoryPage_ = nullptr;
        }
        pCachePagePtr_ = nullptr;
    }
}
/// @brief Get length of page head data
/// @return
uint16_t PPageOptBase::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    return static_cast< uint16_t >(nMinValidLen);
}
/// @brief
/// @return
EBlockType PPageOptBase::PageClassType() const noexcept { return EBlockType::kBaseEmpty; }
/// @brief Reset page, retain PageID
/// @param bResetData
/// @return
bool PPageOptBase::ResetPage(bool const bResetData) noexcept
{
    if (false == _CheckBaseOpt()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }

    uint16_t const sPageLen{static_cast< uint16_t >(pMemoryPage_->GetCacheLen())};
    S_ClearMemory(pPageHead_, sPageLen);
    _InitPageHead(*pPageHead_, EBlockType::kBaseEmpty, pMemoryPage_->GetMainPageID(), sPageLen);
    if (bResetData) {
        S_ClearMemory(pPageHead_ + 1, pPageHead_->sFreeLen);
    }
    return true;
}
//********************************/
/// @brief Check if error code indicates insufficient space
/// @return
bool PPageOptBase::IsLackSpace() const noexcept { return EErrorPHKV::kPageLackSpace == GetLastError(); }
/// @brief Check if Block spans pages
/// @param nBlockID
/// @return
bool PPageOptBase::IsSpreadBlock(uint16_t const nBlockID) const noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PBlockHead const *const pFindBlock{_FindBlock(nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }
    return IsSpreadBlock(*pFindBlock);
}
/// @brief Check if Block spans pages
/// @param blockLocal
/// @return
bool PPageOptBase::IsSpreadBlock(PBlockHead const &blockLocal) noexcept
{
    if (0U != blockLocal.cSpread) {
        return true;
    }
    return false;
}
/// @brief
/// @param pBDstData
/// @param pBSrcData
/// @param nDataLen
void PPageOptBase::CopyData(void *const pBDstData, const void *const pBSrcData, uint16_t const nDataLen) const noexcept
{
    PER_Assert(pMemoryPage_->IsValidData(pBDstData, static_cast< int32_t >(nDataLen)));
    if (nullptr != pBSrcData) {
        std::ignore = PPageOptBase::MoveData(pBDstData, pBSrcData, nDataLen);
    } else {
        std::ignore = memset(pBDstData, 0, static_cast< std::size_t >(nDataLen));
    }
}
//***************/
/// @brief Initialize page
/// @param eBlockType
/// @param nPageID
/// @param bResetData
/// @return
bool PPageOptBase::InitPageHead(EBlockType const eBlockType, uint32_t const nPageID, bool const bResetData) noexcept
{
    if (false == _CheckBaseOpt()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    // Default: GetPageID() == 0
    if (GetPageType() == static_cast< uint8_t >(EBlockType::kBaseEmpty)) {
        uint16_t const sPageLen{static_cast< uint16_t >(pMemoryPage_->GetCacheLen())};
        S_ClearMemory(pPageHead_, sPageLen);
        _InitPageHead(*pPageHead_, eBlockType, nPageID, sPageLen);
        if (bResetData) {
            S_ClearMemory(pPageHead_ + 1, pPageHead_->sFreeLen);
        }
    }
    AddPageType(eBlockType);
    _OnOptSuccess(true);
    return true;
}
/// @brief Add a Block data block: Cross-page
/// @param eBlockType
/// @param pBData
/// @param nDataLen
/// @param pSpreadBlock
/// @param eDataType
/// @return Return value is BlockID
uint16_t PPageOptBase::AddBlock(EBlockType const eBlockType,
                                uint8_t const *const pBData,
                                uint16_t const nDataLen,
                                PBlockSpread const *const pSpreadBlock,
                                EDataType const eDataType) noexcept
{
    // 2021-08-23 hanjingjing: Allow pBData to be null pointer
    PER_Assert(CheckParam(static_cast< int32_t >(nDataLen)));
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return 0U;
    }
    uint16_t nRealDataLen{nDataLen};
    if (nullptr != pSpreadBlock) {
        nRealDataLen += sizeof(PBlockSpread);
    }
    if (false == _CheckPageCapacity(nRealDataLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return 0U;
    }
    EStoreRange eStoreType{EStoreRange::kSingle};
    if (nullptr != pSpreadBlock) {
        eStoreType = EStoreRange::kMulti;
    }
    PBlockHead *const pNewBlock{_AddBlockHead(eStoreType, eBlockType, nDataLen)};
    pNewBlock->cDataType = static_cast< uint8_t >(eDataType);
    std::ignore          = _BlockBodySaveData(*pNewBlock, pBData, nDataLen, pSpreadBlock);
    _SyncPageFreeLen(-1 * (static_cast< int32_t >(sizeof(PBlockHead)) + static_cast< int32_t >(nRealDataLen)));

    _SyncPageTypeByBlock();
    _OnOptSuccess(true);
    return static_cast< uint16_t >(pNewBlock->sBlockID);
}
/// @brief Delete a Block data block
/// @param nBlockID
/// @return
bool PPageOptBase::DelBlock(uint16_t const nBlockID) noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    int32_t nHeadLen{0};
    int32_t nBodyLen{0};
    std::ignore = _ForEachBlock(
        false, [this, nBlockID, &nHeadLen, &nBodyLen](PBlockHead *const pWorkBlock) noexcept -> int32_t {
            if (0 == nHeadLen) {
                if (pWorkBlock->sBlockID == nBlockID) {
                    nHeadLen = static_cast< int32_t >(S_GetBlockHeadSize());
                    nBodyLen = static_cast< int32_t >(pWorkBlock->sLen);
                }
            } else {
                _MoveBlock(pWorkBlock, nHeadLen, nBodyLen);
            }
            return 1;
        });
    if (nHeadLen > 0) {
        pPageHead_->sCountBlock -= 1U;
        if (pPageHead_->sCountBlock <= 0U) {
            pPageHead_->sCountBlock = 0U;
            pBlockHead_             = nullptr;
        }
    }
    _SyncPageFreeLen(nHeadLen + nBodyLen);
    // Clear free data area
    if (pPageHead_->sFreeLen > 0U) {
        CopyData(_GetFreeData(), nullptr, pPageHead_->sFreeLen);
    }
    _SyncPageTypeByBlock();
    // Regardless of whether found, consider deletion successful
    _OnOptSuccess(true);
    return true;
}
/// @brief Modify a Block
/// @param nBlockID
/// @param pBData
/// @param nDataLen
/// @return
bool PPageOptBase::AmendBlock(uint16_t const nBlockID, uint8_t const *const pBData, uint16_t const nDataLen) noexcept
{
    PH_CheckParam(pBData, static_cast< int32_t >(nDataLen), false);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PBlockHead *pFindBlock{nullptr};
    pFindBlock = _FindBlock(nBlockID);
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }
    int32_t const nAmendLen{static_cast< int32_t >(nDataLen) - static_cast< int32_t >(pFindBlock->sLen)};

    if (0 == nAmendLen) {  // Exactly fit
        _BlockBodyReplace(pFindBlock, pBData, nDataLen);
    } else if (nAmendLen < 0) {  // Shrink table
        // Save self first
        _BlockBodyReplace(pFindBlock, pBData, nDataLen);
        // Forward shift subsequent nodes to new position
        std::ignore = _MoveBlockList(pFindBlock, -nAmendLen, false);
    } else {  // Expand table
        if (false == _CheckPageCapacity(static_cast< uint16_t >(nAmendLen))) {
            PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
            return false;
        }
        // Reverse shift subsequent nodes first
        std::ignore = _MoveBlockList(pFindBlock, -nAmendLen, true);
        // Save self later
        _BlockBodyReplace(pFindBlock, pBData, nDataLen);
    }
    _OnOptSuccess(true);
    return true;
}  // namespace isoftkv
/// @brief Modify specific data within Block
/// @param nBlockID
/// @param nOffset
/// @param pBData
/// @param nDataLen
/// @return
bool PPageOptBase::AmendBlock(uint16_t const nBlockID,
                              uint16_t const nOffset,
                              uint8_t const *const pBData,
                              uint16_t const nDataLen) noexcept
{
    PH_CheckParam(pBData, static_cast< int32_t >(nDataLen), false);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PBlockHead *pFindBlock{nullptr};
    pFindBlock = _FindBlock(nBlockID);
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }
    if ((nOffset + nDataLen) > (pFindBlock->sLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageAmendBlock);
        return false;
    }
    _BlockBodyAmend(pFindBlock, nOffset, pBData, nDataLen);
    _OnOptSuccess(true);
    return true;
}
/// @brief Add data to specific Block
/// @param nBlockID
/// @param pAddData
/// @param nAddLen
/// @return
bool PPageOptBase::AddBlockData(uint16_t const nBlockID, uint8_t const *const pAddData, uint16_t const nAddLen) noexcept
{
    PH_CheckParam(pAddData, static_cast< int32_t >(nAddLen), false);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    if (false == _CheckPageCapacity(nAddLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return false;
    }
    PBlockHead *const pFindBlock{_FindBlock(nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }

    // Reverse shift subsequent nodes first
    std::ignore = _MoveBlockList(pFindBlock, -1 * static_cast< int32_t >(nAddLen), true);
    // Save self later
    _BlockBodyAddData(pFindBlock, pAddData, nAddLen);
    _SyncPageFreeLen(-1 * static_cast< int32_t >(nAddLen));

    _OnOptSuccess(true);
    return true;
}
/// @brief Add empty data to specific Block
/// @param nBlockID
/// @param nAddLen
/// @return
bool PPageOptBase::AddBlockData(uint16_t const nBlockID, uint16_t const nAddLen) noexcept
{
    PH_CheckDataLen(static_cast< int32_t >(nAddLen), false);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    if (false == _CheckPageCapacity(nAddLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return false;
    }
    PBlockHead *const pFindBlock{_FindBlock(nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }

    // Reverse shift subsequent nodes first
    std::ignore = _MoveBlockList(pFindBlock, -1 * static_cast< int32_t >(nAddLen), true);
    // Save self later
    _BlockBodyAddData(pFindBlock, nullptr, nAddLen);
    _SyncPageFreeLen(-1 * static_cast< int32_t >(nAddLen));

    _OnOptSuccess(true);
    return true;
}
/// @brief Reduce data from specific Block
/// @param nBlockID
/// @param nDecLen
/// @return
bool PPageOptBase::DecBlockData(uint16_t const nBlockID, uint16_t const nDecLen) noexcept
{
    PH_CheckDataLen(static_cast< int32_t >(nDecLen), false);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PBlockHead *const pFindBlock{_FindBlock(nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return false;
    }

    // Save self first
    _BlockBodyDecData(pFindBlock, nDecLen);
    // Forward shift subsequent nodes to new position
    std::ignore = _MoveBlockList(pFindBlock, static_cast< int32_t >(nDecLen), false);
    _SyncPageFreeLen(static_cast< int32_t >(nDecLen));

    _OnOptSuccess(true);
    return true;
}
/// @brief
/// @param eBlockType
/// @param nBlockID
/// @return
PBlockSpread *PPageOptBase::FindBlockSpread(EBlockType const eBlockType, uint16_t const nBlockID) noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return nullptr;
    }
    PBlockHead *const pFindBlock{_FindBlock(eBlockType, nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return nullptr;
    }

    _OnOptSuccess(false);
    return _GetBlockSpread(*pFindBlock);
}
/// @brief Copy specific Block to new page
/// @param nBlockID
/// @param pNewPageOpt
/// @return
bool PPageOptBase::CopyBlockToNewPage(uint16_t const nBlockID, PPageOptBase *const pNewPageOpt) noexcept
{
    PER_Assert(nullptr != pNewPageOpt);
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return false;
    }
    PBlockHead *const pFindBlock{_FindBlock(nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kErrorBlockID);
        return false;
    }
    PBlockSpread *const pSpreadBlock{_GetBlockSpread(*pFindBlock)};
    uint8_t *const pBlockData{_GetBlockDataHead(pFindBlock)};
    uint16_t nBlockDataLen{pFindBlock->sLen};
    if (nullptr != pSpreadBlock) {
        nBlockDataLen -= sizeof(PBlockSpread);
    }
    // This function does not verify if destination can accommodate
    uint16_t nNewBlockID{0U};
    nNewBlockID = pNewPageOpt->AddBlock(static_cast< EBlockType >(pFindBlock->cBlockType), pBlockData, nBlockDataLen,
                                        pSpreadBlock, static_cast< EDataType >(pFindBlock->cDataType));
    if (nNewBlockID <= 0U) {
        PER_OnOptFailed(pNewPageOpt->GetLastError());
        return false;
    }
    _OnOptSuccess(false);
    return true;
}
/// @brief Get total length of Block
/// @param eBlockType
/// @param nBlockID
/// @return
uint32_t PPageOptBase::GetBlockTotalLen(EBlockType const eBlockType, uint16_t const nBlockID) const noexcept
{
    if (false == PPageOptBase::IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return 0U;
    }
    PBlockHead const *const pFindBlock{_FindBlock(eBlockType, nBlockID)};
    if (nullptr == pFindBlock) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        LogError() << "[PPageOptBase::GetBlockTotalLen].FindBlock: KvName =[" << GetLogKvName().data()
                   << "], eBlockType =" << static_cast< uint32_t >(eBlockType) << ", nBlockID =" << nBlockID;
        return 0U;
    }
    PBlockSpread *const pBlockSpread{_GetBlockSpread(*pFindBlock)};
    if (nullptr != pBlockSpread) {
        return static_cast< uint32_t >(pBlockSpread->nTotalLen);
    }
    return static_cast< uint32_t >(pFindBlock->sLen);
}
//***************/  // Following functions must be called only after ensuring IsAccessReady() returns true
/// @brief Check if Block data has overlap
/// @return
bool PPageOptBase::CheckBlock() const noexcept
{
    PER_Assert(nullptr != pPageHead_);
    uint8_t *pBoundary{T_TransBytes(pPageHead_) + pPageHead_->sPageLen};
    int32_t nScanCount{0};
    nScanCount = _ForEachBlock(false, [this, &pBoundary](PBlockHead const *const pBlockLocal) noexcept -> int32_t {
        if (nullptr == pBlockLocal) {
            return 0;
        }
        uint8_t *const pBlockData{this->_GetDataByOffset(pBlockLocal->sOffset)};
        if (pBlockData >= pBoundary) {
            return -1;
        }
        uint8_t *const pBlockDataMax{pBlockData + pBlockLocal->sLen};
        if (pBlockDataMax != pBoundary) {
            return -1;
        }
        pBoundary = pBlockData;
        return 1;
    });
    return nScanCount >= static_cast< int32_t >(pPageHead_->sCountBlock);
}
/// @brief Calculate effective length of current page from head
/// @return
uint16_t PPageOptBase::GetPageHeadLen() const noexcept
{
    // Assume all class members are valid data
    uint16_t nLenTotal{0U};
    nLenTotal += sizeof(PPageHeadData);
    if (pPageHead_->sCountBlock > 0U) {
        nLenTotal += static_cast< uint16_t >(sizeof(PBlockHead) * pPageHead_->sCountBlock);
    }
    return nLenTotal;
}
/// @brief Calculate effective length of data area in current page
/// @return
uint16_t PPageOptBase::GetPageBodyLen() const noexcept
{
    uint16_t nLenTotal{0U};
    std::ignore = _ForEachBlock(false, [&nLenTotal](PBlockHead const *const pWorkBlock) noexcept -> int32_t {
        nLenTotal += pWorkBlock->sLen;
        return 1;
    });
    return nLenTotal;
}
/// @brief Recalculate page free length
/// @return
uint16_t PPageOptBase::SyncPageFreeLen() noexcept
{
    uint16_t const nOldFreeLen{pPageHead_->sFreeLen};
    uint16_t const nValidLen{static_cast< uint16_t >(GetPageHeadLen() + GetPageBodyLen())};
    pPageHead_->sFreeLen = pPageHead_->sPageLen - nValidLen;

    _OnOptSuccess(nOldFreeLen != pPageHead_->sFreeLen);
    return pPageHead_->sFreeLen;
}
/// @brief Get page free rate
/// @return
uint16_t PPageOptBase::GetFreeLen() const noexcept { return pPageHead_->sFreeLen; }
/// @brief Calculate CRC32 code of this page
/// @return
uint32_t PPageOptBase::CalculatePageCRC32() const noexcept
{
    PER_Assert((nullptr != pPageHead_) && (pPageHead_->sPageLen > 0U));
    uint32_t nCrc32{0U};
    nCrc32 = CalculateCrc32(pPageHead_,
                            static_cast< uint32_t >(static_cast< uint32_t >(sizeof(PPageHeadData))
                                                    - static_cast< uint32_t >(sizeof(pPageHead_->nCRC))),
                            nCrc32);
    // Skipped PPageHeadData.nCRC when calculating page CRC32
    nCrc32 = CalculateCrc32(
        pPageHead_ + 1, static_cast< uint32_t >(pPageHead_->sPageLen) - static_cast< uint32_t >(sizeof(PPageHeadData)),
        nCrc32);
    return nCrc32;
}
/// @brief Check if page CRC32 is correct
/// @return
bool PPageOptBase::CheckPageCRC32() const noexcept
{
    if (nullptr == pPageHead_) {
        return false;
    }
    return CalculatePageCRC32() == pPageHead_->nCRC;
}
/// @brief Update page CRC32
void PPageOptBase::UpdatePageCRC32() const noexcept
{
    PER_Assert(nullptr != pPageHead_);
    pPageHead_->nCRC = CalculatePageCRC32();
    _OnOptSuccess(true);
}
/// @brief Forward traverse all BlockData
/// @param bFromBack
/// @param pfun
/// @return
int32_t PPageOptBase::ForEachBlock(bool const bFromBack,
                                   std::function< int32_t(PBlockHead const *) > const &pfun) const noexcept
{
    return _ForEachBlock(bFromBack, pfun);
}
/// @brief Return possible PBlockSpread
/// @param workBlock
/// @return
PBlockSpread *PPageOptBase::GetBlockSpread(PBlockHead const &workBlock) const noexcept
{
    return _GetBlockSpread(workBlock);
}
//********************************/
/// @brief
/// @param bAddRef
void PPageOptBase::_OnOptSuccess(bool const bAddRef) const noexcept
{
    if (nullptr != pMemoryPage_) {
        pMemoryPage_->AddWriteRef(bAddRef);
    }
    PKvObject::_OnSuccess();
}
/// @brief Clear memory
/// @param pBDstData
/// @param nDataLen
void PPageOptBase::S_ClearMemory(void *const pBDstData, uint16_t const nDataLen) noexcept
{
    std::ignore = memset(pBDstData, 0, static_cast< std::size_t >(nDataLen));
}
/// @brief
/// @param pCachePage
void PPageOptBase::_AttachCachePage(PCachePagePtr const &pCachePage) noexcept
{
    if (nullptr == pCachePage) {
        return;
    }
    pCachePagePtr_ = pCachePage;
    pMemoryPage_   = pCachePage->GetMemoryPage();
    pMemoryPage_->AttachOpt(this);
}
/// @brief Initialize Page data
/// @param pCachePage
/// @return
bool PPageOptBase::_InitCachePage_Base(PCachePagePtr const &pCachePage) noexcept
{
    if (nullptr == pCachePage) {
        PER_OnOptFailed(EErrorPHKV::kNullParam);
        return false;
    }
    uint8_t *const pBData{(*pCachePage)->GetCache()};
    int32_t const nDataLen{static_cast< int32_t >((*pCachePage)->GetCacheLen())};
    PH_CheckParam(pBData, nDataLen, false);

    pPageHead_  = nullptr;
    pBlockHead_ = nullptr;
    DetachCachePage();
    _AttachCachePage(pCachePage);
    // Verify data
    pPageHead_ = T_TransPtr< PPageHeadData >(pBData);
    if (pPageHead_->sCountBlock > 0U) {
        pBlockHead_ = T_TransPtr< PPageHeadData, PBlockHead >(pPageHead_ + 1);
    }
    return PPageOptBase::IsAccessReady();
}
//***************/
/// @brief Synchronize PageHead.PageType according to Block type
/// @return
void PPageOptBase::_SyncPageTypeByBlock() const noexcept
{
    PER_Assert(nullptr != pPageHead_);
    uint8_t newPageType{0U};
    int32_t nScanCount{0};
    nScanCount = _ForEachBlock(false, [&newPageType](PBlockHead const *const pBlockHead) noexcept -> int32_t {
        if (nullptr == pBlockHead) {
            return 0;
        }
        newPageType |= pBlockHead->cBlockType;
        return 1;
    });
    if (nScanCount > 0) {
        pPageHead_->cPageType = newPageType;
    }
    PER_Assert(nScanCount == static_cast< int32_t >(pPageHead_->sCountBlock));
    return;
}
/// @brief Get start address of free area
/// @return
uint8_t *PPageOptBase::_GetFreeData() const noexcept
{
    if ((nullptr != pBlockHead_) && (pPageHead_->sCountBlock > 0U)) {
        return T_TransBytes(pBlockHead_ + pPageHead_->sCountBlock);
    }

    return T_TransBytes(pPageHead_ + 1);
}
/// @brief
/// @param pWorkBlock
/// @return
uint8_t *PPageOptBase::_GetBlockDataHead(PBlockHead const *const pWorkBlock) const noexcept
{
    PER_Assert(nullptr != pWorkBlock);
    uint16_t nOffset{pWorkBlock->sOffset};
    if (pWorkBlock->cSpread == static_cast< uint8_t >(EStoreRange::kMulti)) {
        nOffset += sizeof(PBlockSpread);
    }
    return _GetDataByOffset(nOffset);
}
/// @brief Return possible PBlockSpread
/// @param workBlock
/// @return
PBlockSpread *PPageOptBase::_GetBlockSpread(PBlockHead const &workBlock) const noexcept
{
    if (false == S_IsHaveSpread(workBlock)) {
        return nullptr;
    }
    return T_TransPtr< PBlockSpread >(_GetDataByOffset(workBlock.sOffset));
}
/// @brief Get maximum free capacity of Page page
/// @param pageHead
/// @return
uint16_t PPageOptBase::_GetMaxFreeLen(PPageHeadData const &pageHead) const noexcept
{
    return static_cast< uint16_t >(pageHead.sPageLen - PPageOptBase::GetMinValidLen());
}
/// @brief Initialize page
/// @param pageHead
/// @param eType
/// @param nPageID
/// @param sPageLen
void PPageOptBase::_InitPageHead(PPageHeadData &pageHead,
                                 EBlockType const eType,
                                 uint32_t const nPageID,
                                 uint16_t const sPageLen) const noexcept
{
    // Explicitly assign each member of PPageHeadData & pageHead
    ara::core::StringView const stPageType{std::move(GetKvPageMagic(eType))};
    std::ignore = memset(static_cast< void * >(pageHead.cMagic), 0, sizeof(pageHead.cMagic));
    if ('\0' == pageHead.cMagic[0]) {
        // This output parameter value is not subsequently checked.
    }
    std::size_t const nTypeLen{
        std::min< std::size_t >(stPageType.size(), static_cast< std::size_t >(sizeof(pageHead.cMagic)) - 1U)};
    std::ignore
        = T_Memcpy(static_cast< void * >(pageHead.cMagic), stPageType.data(), static_cast< uint32_t >(nTypeLen));
    if ('\0' == pageHead.cMagic[0]) {
        // This output parameter value is not subsequently checked.
    }
    PER_Assert(pageHead.cPageType == static_cast< uint8_t >(EBlockType::kBaseEmpty));
    pageHead.cPageType   = static_cast< uint8_t >(eType);
    pageHead.nPageID     = nPageID;
    pageHead.sPageLen    = sPageLen;
    pageHead.sFreeLen    = _GetMaxFreeLen(pageHead);  // Must be after pageHead.sPageLen initialization
    pageHead.sCountBlock = 0U;
    pageHead.cReserve    = 0U;
    pageHead.nCRC        = 0U;
    return;
}
//***************/
/// @brief Check validity of current page data
/// @return
bool PPageOptBase::_CheckBaseOpt() const noexcept
{
    if (nullptr == pPageHead_) {
        return false;
    }
    if (nullptr == pCachePagePtr_) {
        return false;
    }
    if (nullptr == pMemoryPage_) {
        return false;
    }
    if ((pPageHead_->sCountBlock > 0U) && (nullptr == pBlockHead_)) {
        return false;
    }

    return true;
}
/// @brief Verify if page can still add new data
/// @param nAddLen
/// @return
bool PPageOptBase::_CheckPageCapacity(uint16_t const nAddLen) const noexcept
{
    // Assume all class members are valid data
    uint16_t const nValidLen{static_cast< uint16_t >(GetPageHeadLen() + GetPageBodyLen())};
    return (nValidLen + nAddLen) <= pPageHead_->sPageLen;
}
/// @brief
/// @param bFromBack
/// @param pfun
/// @return
int32_t PPageOptBase::_ForEachBlock(bool const bFromBack,
                                    std::function< int32_t(PBlockHead *const) > const &pfun) noexcept
{
    int32_t nReturn{0};
    nReturn = 0;
    for (int32_t i{0}; i < static_cast< int32_t >(pPageHead_->sCountBlock); i++) {
        int32_t nIndex{i};
        if (bFromBack) {
            nIndex = static_cast< int32_t >(pPageHead_->sCountBlock) - i - 1;
        }
        PBlockHead *const pWorkBlock{pBlockHead_ + nIndex};
        int32_t const nFunCode{pfun(pWorkBlock)};
        if (nFunCode < 0) {
            break;
        }
        if (nFunCode == 0) {
            continue;
        }
        nReturn += 1;
    }
    return nReturn;
}
/// @brief
/// @param bFromBack
/// @param pfun
/// @return
int32_t PPageOptBase::_ForEachBlock(bool const bFromBack,
                                    std::function< int32_t(PBlockHead const *) > const &pfun) const noexcept
{
    int32_t nReturn{0};
    nReturn = 0;
    for (int32_t i{0}; i < static_cast< int32_t >(pPageHead_->sCountBlock); i++) {
        int32_t nIndex{i};
        if (bFromBack) {
            nIndex = static_cast< int32_t >(pPageHead_->sCountBlock) - i - 1;
        }
        PBlockHead const *const pWorkBlock{pBlockHead_ + nIndex};
        int32_t const nFunCode{pfun(pWorkBlock)};
        if (nFunCode < 0) {
            break;
        }
        if (nFunCode == 0) {
            continue;
        }
        nReturn += 1;
    }
    return nReturn;
}
/// @brief Check if contains cross-page data
/// @param workBlock
/// @return
bool PPageOptBase::S_IsHaveSpread(PBlockHead const &workBlock) noexcept
{
    return workBlock.cSpread == static_cast< uint8_t >(EStoreRange::kMulti);
}
//********************************/
/// @brief
/// @param nPlanID
/// @return
uint16_t PPageOptBase::_FindFreeBlockID(uint16_t const nPlanID) const noexcept
{
    bool bFindSame{true};
    int32_t nFindID{static_cast< int32_t >(nPlanID)};
    // Main logic is to prevent duplicate nBlockID
    while (bFindSame) {
        if (0 == nFindID) {
            nFindID = 1;
        }
        bFindSame = false;
        std::ignore
            = _ForEachBlock(false, [nFindID, &bFindSame](PBlockHead const *const pWorkBlock) noexcept -> int32_t {
                  if (static_cast< int32_t >(pWorkBlock->sBlockID) == nFindID) {
                      bFindSame = true;
                      return -1;
                  }
                  return 1;
              });
        if (bFindSame) {
            nFindID = (nFindID + 1) % kInt_0x10000;
        }
    }
    return static_cast< uint16_t >(nFindID);
}
/// @brief Find data block from local
/// @param eBlockType
/// @param nBlockID
/// @return
PBlockHead const *PPageOptBase::_FindBlock(EBlockType const eBlockType, uint16_t const nBlockID) const noexcept
{
    PBlockHead const *pBlockFind{nullptr};
    std::ignore = _ForEachBlock(
        false, [eBlockType, nBlockID, &pBlockFind](PBlockHead const *const pWorkBlock) noexcept -> int32_t {
            if (static_cast< uint8_t >(eBlockType) == pWorkBlock->cBlockType) {
                if ((false == PPageOptBase::IsValidPageID(static_cast< uint32_t >(nBlockID)))
                    || (pWorkBlock->sBlockID == nBlockID)) {
                    pBlockFind = pWorkBlock;
                    return -1;
                }
            }
            return 1;
        });
    return pBlockFind;
}
/// @brief Find data block from local
/// @param eBlockType
/// @param nBlockID
/// @return
PBlockHead *PPageOptBase::_FindBlock(EBlockType const eBlockType, uint16_t const nBlockID) noexcept
{
    PBlockHead *pBlockFind{nullptr};
    std::ignore
        = _ForEachBlock(false, [eBlockType, nBlockID, &pBlockFind](PBlockHead *const pWorkBlock) noexcept -> int32_t {
              if (static_cast< uint8_t >(eBlockType) == pWorkBlock->cBlockType) {
                  if ((false == PPageOptBase::IsValidPageID(static_cast< uint32_t >(nBlockID)))
                      || (pWorkBlock->sBlockID == nBlockID)) {
                      pBlockFind = pWorkBlock;
                      return -1;
                  }
              }
              return 1;
          });
    return pBlockFind;
}
/// @brief Find data block
/// @param nBlockID
/// @return
PBlockHead const *PPageOptBase::_FindBlock(uint16_t const nBlockID) const noexcept
{
    PBlockHead const *pBlockFind{nullptr};
    std::ignore = _ForEachBlock(false, [nBlockID, &pBlockFind](PBlockHead const *const pWorkBlock) noexcept -> int32_t {
        if (pWorkBlock->sBlockID == nBlockID) {
            pBlockFind = pWorkBlock;
            return -1;
        }
        return 1;
    });
    return pBlockFind;
}
/// @brief Find data block
/// @param nBlockID
/// @return
PBlockHead *PPageOptBase::_FindBlock(uint16_t const nBlockID) noexcept
{
    PBlockHead *pBlockFind{nullptr};
    std::ignore = _ForEachBlock(false, [nBlockID, &pBlockFind](PBlockHead *const pWorkBlock) noexcept -> int32_t {
        if (pWorkBlock->sBlockID == nBlockID) {
            pBlockFind = const_cast< PBlockHead * >(pWorkBlock);
            return -1;
        }
        return 1;
    });
    return pBlockFind;
}
/// @brief Find Block data area
/// @param nBlockID
/// @return
uint8_t *PPageOptBase::_FindBlockData(uint16_t const nBlockID) const noexcept
{
    PBlockHead const *const pBlockFind{_FindBlock(nBlockID)};
    if (nullptr == pBlockFind) {
        return nullptr;
    }

    return _GetBlockDataHead(pBlockFind);
}
/// @brief Add local data
/// @param eRangeType
/// @param eBlockType
/// @param nDataLen
/// @return
PBlockHead *PPageOptBase::_AddBlockHead(EStoreRange const eRangeType,
                                        EBlockType const eBlockType,
                                        uint16_t const nDataLen) noexcept
{
    uint16_t const nTailLen{GetPageBodyLen()};
    if (nullptr == pBlockHead_) {
        pBlockHead_             = T_TransPtr< PPageHeadData, PBlockHead >(pPageHead_ + 1);
        pPageHead_->sCountBlock = 0U;
    }
    uint16_t nSpreadLen{0U};
    if (eRangeType == EStoreRange::kMulti) {
        nSpreadLen = sizeof(PBlockSpread);
    }
    PBlockHead *pNewBlock{nullptr};
    pNewBlock = pBlockHead_ + pPageHead_->sCountBlock;
    pPageHead_->sCountBlock += 1U;
    // BlockID counts from 1, so pNewBlock->sBlockID needs +1
    pNewBlock->sBlockID
        = _FindFreeBlockID(static_cast< uint16_t >(pPageHead_->sCountBlock + static_cast< uint16_t >(1)));
    pNewBlock->sOffset    = pPageHead_->sPageLen - nTailLen - (nSpreadLen + nDataLen);
    pNewBlock->sLen       = nSpreadLen + nDataLen;
    pNewBlock->cBlockType = static_cast< uint8_t >(eBlockType);
    if (eBlockType == EBlockType::kBaseEmpty) {
        pNewBlock->cBlockType = static_cast< uint8_t >(eBlockType);
    }
    pNewBlock->cSpread = static_cast< uint8_t >(eRangeType);

    _OnOptSuccess(true);
    return pNewBlock;
}
/// @brief Save BlockBody data: Cross-page
/// @param workBlock
/// @param pBData
/// @param nDataLen
/// @param pSpreadBlock
/// @return
int32_t PPageOptBase::_BlockBodySaveData(PBlockHead const &workBlock,
                                         uint8_t const *const pBData,
                                         uint16_t const nDataLen,
                                         PBlockSpread const *const pSpreadBlock) const noexcept
{
    uint8_t *const pBDstData{_GetDataByOffset(workBlock.sOffset)};
    int32_t nIndex{0};
    if (S_IsHaveSpread(workBlock)) {
        if (nullptr != pSpreadBlock)  // Cross-page data
        {
            CopyData(pBDstData + nIndex, pSpreadBlock, sizeof(PBlockSpread));
        }
        nIndex += static_cast< int32_t >(sizeof(PBlockSpread));
    }
    // Copy data
    CopyData(pBDstData + nIndex, pBData, nDataLen);
    nIndex += static_cast< int32_t >(nDataLen);
    return nIndex;
}
/// @brief Save BlockBody data
/// @param pWorkBlock
/// @param pBData
/// @param nDataLen
void PPageOptBase::_BlockBodyReplace(PBlockHead *const pWorkBlock,
                                     uint8_t const *const pBData,
                                     uint16_t const nDataLen) const noexcept
{
    PER_Assert(nullptr != pWorkBlock);
    uint16_t nSpreadLen{0U};
    if (S_IsHaveSpread(*pWorkBlock)) {
        nSpreadLen += sizeof(PBlockSpread);
    }
    int32_t const nAmendLen{static_cast< int32_t >(pWorkBlock->sLen) - static_cast< int32_t >(nDataLen)};
    uint16_t const nNewOffset{static_cast< uint16_t >(pWorkBlock->sOffset - static_cast< uint16_t >(nAmendLen))};
    uint8_t *const pSrcData{_GetDataByOffset(pWorkBlock->sOffset)};
    if ((nSpreadLen > 0U) && (nAmendLen != 0)) {
        CopyData(pSrcData - nAmendLen, pSrcData, sizeof(PBlockSpread));
    }
    CopyData(pSrcData - nAmendLen + nSpreadLen, pBData, nDataLen);
    pWorkBlock->sLen    = nSpreadLen + nDataLen;
    pWorkBlock->sOffset = nNewOffset;
}
/// @brief
/// @param pWorkBlock
/// @param nOffset
/// @param pBData
/// @param nDataLen
void PPageOptBase::_BlockBodyAmend(PBlockHead const *const pWorkBlock,
                                   uint16_t nOffset,
                                   uint8_t const *const pBData,
                                   uint16_t const nDataLen) const noexcept
{
    PER_Assert(nullptr != pWorkBlock);
    if (S_IsHaveSpread(*pWorkBlock)) {
        nOffset += sizeof(PBlockSpread);
    }
    uint8_t *const pSrcData{_GetDataByOffset(pWorkBlock->sOffset)};
    CopyData(pSrcData + nOffset, pBData, nDataLen);
}
/// @brief Add data within BlockBody
/// @param pWorkBlock
/// @param pAddData
/// @param nAddLen
void PPageOptBase::_BlockBodyAddData(PBlockHead *const pWorkBlock,
                                     uint8_t const *const pAddData,
                                     uint16_t const nAddLen) const noexcept
{
    PER_Assert(nullptr != pWorkBlock);
    uint8_t *const pSrcData{_GetDataByOffset(pWorkBlock->sOffset)};
    CopyData(pSrcData - nAddLen, pSrcData, pWorkBlock->sLen);
    CopyData(pSrcData - nAddLen + pWorkBlock->sLen, pAddData, nAddLen);
    pWorkBlock->sLen += nAddLen;
    pWorkBlock->sOffset -= nAddLen;
}
/// @brief Reduce data within BlockBody
/// @param pWorkBlock
/// @param nDecLen
void PPageOptBase::_BlockBodyDecData(PBlockHead *const pWorkBlock, uint16_t const nDecLen) const noexcept
{
    uint8_t *const pSrcData{_GetBlockDataHead(pWorkBlock)};
    CopyData(pSrcData + nDecLen, pSrcData, static_cast< uint16_t >(pWorkBlock->sLen - nDecLen));
    pWorkBlock->sLen -= nDecLen;
    pWorkBlock->sOffset += nDecLen;
}
/// @brief Delete a Block
/// @param pMoveBlock
/// @param nMoveHeadLen
/// @param nMoveBodyLen
void PPageOptBase::_MoveBlock(PBlockHead *const pMoveBlock,
                              int32_t const nMoveHeadLen,
                              int32_t const nMoveBodyLen) const noexcept
{
    // Caller ensures no overwrite of other information after copying
    if (nMoveBodyLen != 0)  // Data area is saved in reverse order from page tail
    {
        uint8_t *const pWorkData{_GetDataByOffset(pMoveBlock->sOffset)};
        CopyData(pWorkData + nMoveBodyLen, pWorkData, pMoveBlock->sLen);
        pMoveBlock->sOffset = T_AddInt(pMoveBlock->sOffset, nMoveBodyLen);
    }
    if (nMoveHeadLen != 0)  // Head information is saved in order from page head: Ensure no self-overwrite
    {
        int32_t const nBlockLenWork{static_cast< int32_t >(S_GetBlockHeadSize())};
        uint8_t *const pCtrlData{T_TransBytes(pMoveBlock)};
        CopyData(pCtrlData - nMoveHeadLen, pCtrlData, static_cast< uint16_t >(nBlockLenWork));
    }
}
/// @brief Move all nodes after pWorkBlock (bInvertedOrder determines traversal order)
/// @param pMoveBlock
/// @param nAmendOffset
/// @param bInvertedOrder
/// @return
int32_t PPageOptBase::_MoveBlockList(PBlockHead *const pMoveBlock,
                                     int32_t const nAmendOffset,
                                     bool const bInvertedOrder) noexcept
{
    int32_t nMoveCount{0};
    nMoveCount = _ForEachBlock(bInvertedOrder,
                               [this, nAmendOffset, pMoveBlock](PBlockHead *const pWorkBlock) noexcept -> int32_t {
                                   if (T_SubPtr(pWorkBlock, pMoveBlock) <= 0) {
                                       return 0;
                                   }
                                   _MoveBlock(pWorkBlock, 0, nAmendOffset);
                                   return 1;
                               });
    return nMoveCount;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
