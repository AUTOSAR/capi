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
/// @file       page_opt_base.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Logical page interpreter base class
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Features
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Logical page interpreter base class
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

#ifndef ARA_PER_PHKV_PAGE_OPT_BASE_H_
#define ARA_PER_PHKV_PAGE_OPT_BASE_H_

#include <functional>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {

/// @brief
class PConsoleLog;
//********************************/     // Page data base class
/// @brief Determine whether the given string is supported by KvPage.
/// @code{.isoft}
/// @unit_name=IsKvPage
/// @endcode
/// @param stMagic
/// @return
bool IsKvPage(ara::core::StringView const &stMagic) noexcept;
/// @brief Convert the magic word used by KvPage from enum to string
/// @code{.isoft}
/// @unit_name=GetKvPageMagic
/// @endcode
/// @param eType
/// @return
ara::core::StringView GetKvPageMagic(EBlockType const eType) noexcept;
/// @brief Convert nHashKeyID to the index of the hash mapping array
/// @code{.isoft}
/// @unit_name=TransHashIndex
/// @endcode
/// @param nHashKeyID
/// @param nHashCount
/// @return
int32_t TransHashIndex(uint32_t const nHashKeyID, uint32_t const nHashCount) noexcept;
/// @brief Format version string
/// @code{.isoft}
/// @unit_name=FormatVersion
/// @endcode
/// @param version
/// @return
ara::core::String FormatVersion(PPerVersion const &version) noexcept;
//********************************/

/// @brief Page data base class: Data storage uses a strategy of storing from both ends towards the middle
/// @code{.isoft}
/// @unit_name=PPageOptBase
/// @endcode
class PPageOptBase : public PKvObject
{
private:
    /// @brief This is the memory storing page data, specific interpretation is implemented by derived classes
    PPageHeadData *pPageHead_{nullptr};
    /// @brief Starting address of the local data array
    PBlockHead *pBlockHead_{nullptr};
    /// @brief Bound cache page
    PCachePagePtr pCachePagePtr_;
    /// @brief Memory page
    PMemoryPage *pMemoryPage_{nullptr};

public:
    /// @brief
    using CB_ReadData = std::function< int32_t(int32_t const nDataType,
                                               uint8_t const *const pReadData,
                                               int32_t const nReadLen,
                                               uint32_t const nPageID,
                                               uint16_t const nBlockID) >;
    /// @brief
    /// @param nPageID
    /// @return
    static bool IsValidPageID(uint32_t const nPageID) noexcept;
    /// @brief Copy data
    /// @param pBDstData
    /// @param pBSrcData
    /// @param nDataLen
    /// @return
    static bool MoveData(void *const pBDstData, const void *const pBSrcData, uint16_t const nDataLen) noexcept;

public:
    /// @brief
    /// @param pCachePage
    explicit PPageOptBase(PCachePagePtr const &pCachePage) noexcept;
    /// @brief Default constructor: deleted
    PPageOptBase() = delete;
    /// @brief
    ~PPageOptBase() noexcept override;
    /// @brief
    /// @param a
    PPageOptBase(PPageOptBase const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptBase &operator=(PPageOptBase const &a) = delete;
    /// @brief
    /// @param a
    PPageOptBase(PPageOptBase &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptBase &operator=(PPageOptBase &&a) = delete;
    /// @brief Check if ready for read/write
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;

public:
    /// @brief Initialize Page data
    /// @param pCachePage
    /// @return
    virtual bool InitCachePage(PCachePagePtr const &pCachePage) noexcept;
    /// @brief Unbind the cache page operation object
    void DetachCachePage() noexcept;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    virtual uint16_t GetMinValidLen() const noexcept;
    /// @brief Get the defined type of the page operation class
    /// @return
    virtual EBlockType PageClassType() const noexcept;
    /// @brief Reset the page, retaining the PageID
    /// @param bResetData
    /// @return
    virtual bool ResetPage(bool const bResetData) noexcept;

public:
    /// @brief
    /// @return
    inline PMemoryPage *GetMemoryPage() const noexcept { return pMemoryPage_; }
    /// @brief
    /// @return
    inline PBlockHead *GetBlockHead() const noexcept { return pBlockHead_; }
    /// @brief
    /// @return
    inline PPageHeadData *GetPageHead() const noexcept { return pPageHead_; }
    /// @brief Check if the error code indicates insufficient space
    /// @return
    bool IsLackSpace() const noexcept;
    /// @brief Return the smart pointer
    /// @return
    inline PCachePagePtr GetCachePage() const noexcept { return pCachePagePtr_; }
    /// @brief Check if a Block spans pages
    /// @param nBlockID
    /// @return
    bool IsSpreadBlock(uint16_t const nBlockID) const noexcept;
    /// @brief Check if a Block spans pages
    /// @param blockLocal
    /// @return
    static bool IsSpreadBlock(PBlockHead const &blockLocal) noexcept;
    /// @brief
    /// @param pBDstData
    /// @param pBSrcData
    /// @param nDataLen
    void CopyData(void *const pBDstData, const void *const pBSrcData, uint16_t const nDataLen) const noexcept;
    /// @brief
    /// @param pCachePagePtr
    /// @return
    inline bool IsSameCachePage(PCachePagePtr const &pCachePagePtr) const noexcept
    {
        return pCachePagePtr == pCachePagePtr_;
    }

public:
    /// @brief Reset the page
    /// @param eBlockType
    /// @param nPageID
    /// @param bResetData
    /// @return
    bool InitPageHead(EBlockType const eBlockType, uint32_t const nPageID, bool const bResetData = false) noexcept;
    /// @brief Add a Block data block: spanning pages
    /// @param eBlockType
    /// @param pBData
    /// @param nDataLen
    /// @param pSpreadBlock
    /// @param eDataType
    /// @return Return value is the BlockID
    uint16_t AddBlock(EBlockType const eBlockType,
                      uint8_t const *const pBData,
                      uint16_t const nDataLen,
                      PBlockSpread const *const pSpreadBlock,
                      EDataType const eDataType = EDataType::kDefault) noexcept;
    /// @brief Delete a Block data block
    /// @param nBlockID
    /// @return
    bool DelBlock(uint16_t const nBlockID) noexcept;
    /// @brief Modify a Block
    /// @param nBlockID
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool AmendBlock(uint16_t const nBlockID, uint8_t const *const pBData, uint16_t const nDataLen) noexcept;
    /// @brief Modify specific data within a Block
    /// @param nBlockID
    /// @param nOffset
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool AmendBlock(uint16_t const nBlockID,
                    uint16_t const nOffset,
                    uint8_t const *const pBData,
                    uint16_t const nDataLen) noexcept;
    /// @brief Add data to a specific Block
    /// @param nBlockID
    /// @param pAddData
    /// @param nAddLen
    /// @return
    bool AddBlockData(uint16_t const nBlockID, uint8_t const *const pAddData, uint16_t const nAddLen) noexcept;
    /// @brief Add empty data to a specific Block
    /// @param nBlockID
    /// @param nAddLen
    /// @return
    bool AddBlockData(uint16_t const nBlockID, uint16_t const nAddLen) noexcept;
    /// @brief Remove data from a specific Block
    /// @param nBlockID
    /// @param nDecLen
    /// @return
    bool DecBlockData(uint16_t const nBlockID, uint16_t const nDecLen) noexcept;
    /// @brief Copy a specific Block to a new page
    /// @param eBlockType
    /// @param nBlockID
    /// @return
    PBlockSpread *FindBlockSpread(EBlockType const eBlockType, uint16_t const nBlockID) noexcept;
    /// @brief
    /// @param nBlockID
    /// @param pNewPageOpt
    /// @return
    bool CopyBlockToNewPage(uint16_t const nBlockID, PPageOptBase *const pNewPageOpt) noexcept;
    /// @brief Get the total length of a Block
    /// @param eBlockType
    /// @param nBlockID
    /// @return
    uint32_t GetBlockTotalLen(EBlockType const eBlockType, uint16_t const nBlockID) const noexcept;
    // The following functions must be called only after ensuring IsAccessReady() returns true
public:
    /// @brief
    /// @return
    inline uint32_t GetPageID() const noexcept
    {
        PER_Assert(nullptr != pPageHead_);
        return pPageHead_->nPageID;
    }
    /// @brief
    /// @return
    inline uint8_t GetPageType() const noexcept
    {
        PER_Assert(nullptr != pPageHead_);
        return pPageHead_->cPageType;
    }
    /// @brief Check if the local page type is included
    /// @param nPageType
    /// @return
    inline bool IsHavePageType(uint8_t const nPageType) const noexcept
    {
        if (nullptr == pPageHead_) {
            return false;
        }
        return (pPageHead_->cPageType & nPageType) > 0U;
    }
    /// @brief
    /// @return
    inline bool IsHavePageType() const noexcept { return IsHavePageType(static_cast< uint8_t >(PageClassType())); }
    /// @brief Set the page type
    /// @param nPageType
    inline void SetPageType(uint8_t const nPageType) noexcept { pPageHead_->cPageType = nPageType; }
    /// @brief Add a type to the page
    /// @param eBlockType
    inline void AddPageType(EBlockType const eBlockType) noexcept
    {
        pPageHead_->cPageType |= static_cast< uint8_t >(eBlockType);
    }
    /// @brief Remove a type from the page
    /// @param eBlockType
    inline void DelPageType(EBlockType const eBlockType) noexcept
    {
        pPageHead_->cPageType &= ~(static_cast< uint8_t >(eBlockType));
    }
    /// @brief Determine if the current page is an exclusive page
    /// @param nPageType
    /// @return
    inline bool IsSoloPage(uint8_t nPageType) const noexcept
    {
        if (nullptr == pPageHead_) {
            return false;
        }
        nPageType ^= pPageHead_->cPageType;
        if (nPageType > 0U) {
            return false;
        }
        return true;
    }
    /// @brief Determine if the current page is an exclusive page
    /// @param ePageType
    /// @return
    inline bool IsSoloPage(EBlockType const ePageType) const noexcept
    {
        return IsSoloPage(static_cast< uint8_t >(ePageType));
    }
    /// @brief Determine if the current page is an exclusive page
    /// @return
    inline bool IsSoloPage() const noexcept { return IsSoloPage(PageClassType()); }
    /// @brief Check if Block data overlaps
    /// @return
    bool CheckBlock() const noexcept;
    /// @brief Calculate the effective length from the beginning of the current page
    /// @return
    uint16_t GetPageHeadLen() const noexcept;
    /// @brief Calculate the effective length of the data area of the current page
    /// @return
    uint16_t GetPageBodyLen() const noexcept;
    /// @brief Recalculate the free length of the page
    /// @return
    uint16_t SyncPageFreeLen() noexcept;
    /// @brief Get the free capacity of the page
    /// @return
    uint16_t GetFreeLen() const noexcept;
    /// @brief Calculate the CRC32 code of this page's data
    /// @return
    uint32_t CalculatePageCRC32() const noexcept;
    /// @brief Check if the page's CRC32 is correct
    /// @return
    bool CheckPageCRC32() const noexcept;
    /// @brief Update the page's CRC32
    void UpdatePageCRC32() const noexcept;
    /// @brief Iterate over all BlockData in forward order
    /// @param bFromBack
    /// @param pfun
    /// @return
    int32_t ForEachBlock(bool const bFromBack, std::function< int32_t(PBlockHead const *) > const &pfun) const noexcept;
    /// @brief Return the possibly existing PBlockSpread
    /// @param workBlock
    /// @return
    PBlockSpread *GetBlockSpread(PBlockHead const &workBlock) const noexcept;

protected:  // business-independent
    /// @brief
    /// @param bAddRef
    void _OnOptSuccess(bool const bAddRef) const noexcept;
    /// @brief Bind the cache page operation object
    /// @param pCachePage
    void _AttachCachePage(PCachePagePtr const &pCachePage) noexcept;
    /// @brief Initialize Page data
    /// @param pCachePage
    /// @return
    bool _InitCachePage_Base(PCachePagePtr const &pCachePage) noexcept;

protected:  // PageHead related
    /// @brief
    /// @param nOffset
    /// @return
    inline uint8_t *_GetDataByOffset(uint16_t const nOffset) const noexcept
    {
        return T_OffsetPtr(T_TransBytes(pPageHead_), static_cast< int32_t >(nOffset));
    }
    /// @brief Maintain page free length
    /// @param nAmendLen
    inline void _SyncPageFreeLen(int32_t const nAmendLen) noexcept
    {
        pPageHead_->sFreeLen = T_AddInt(pPageHead_->sFreeLen, nAmendLen);
    }
    /// @brief Synchronously modify the PageType in PageHead based on the Block type
    /// @return
    void _SyncPageTypeByBlock() const noexcept;
    /// @brief Get the starting address of the free area
    /// @name _GetFreeData
    /// @return
    uint8_t *_GetFreeData() const noexcept;
    /// @brief Skip over potentially existing PBlockSpread
    /// @param pWorkBlock
    /// @return
    uint8_t *_GetBlockDataHead(PBlockHead const *const pWorkBlock) const noexcept;
    /// @brief Return the possibly existing PBlockSpread
    /// @param workBlock
    /// @return
    PBlockSpread *_GetBlockSpread(PBlockHead const &workBlock) const noexcept;
    /// @brief Get the maximum free capacity of the page
    /// @param pageHead
    /// @return
    uint16_t _GetMaxFreeLen(PPageHeadData const &pageHead) const noexcept;
    /// @brief Reset the page
    /// @param pageHead
    /// @param eType
    /// @param nPageID
    /// @param sPageLen
    void _InitPageHead(PPageHeadData &pageHead,
                       EBlockType const eType,
                       uint32_t const nPageID,
                       uint16_t const sPageLen) const noexcept;
    /// @brief Check the validity of the current page data
    /// @return
    bool _CheckBaseOpt() const noexcept;
    /// @brief Verify if the page can still accommodate new data
    /// @param nAddLen
    /// @return
    bool _CheckPageCapacity(uint16_t const nAddLen) const noexcept;
    /// @brief Iterate over all BlockData in forward order
    /// @param bFromBack
    /// @param pfun
    /// @return
    int32_t _ForEachBlock(bool const bFromBack, std::function< int32_t(PBlockHead *) > const &pfun) noexcept;
    /// @brief Iterate over all BlockData in forward order
    /// @param bFromBack
    /// @param pfun
    /// @return
    int32_t _ForEachBlock(bool const bFromBack,
                          std::function< int32_t(PBlockHead const *) > const &pfun) const noexcept;

protected:
    /// @brief
    /// @tparam T
    /// @return
    template < typename T >
    static uint16_t T_GetArrayCount() noexcept
    {
        return static_cast< uint16_t >(static_cast< uint16_t >(sizeof(T{}.arrayElement))
                                       / static_cast< uint16_t >(sizeof(T{}.arrayElement[0])));
    }
    /// @brief Return the length of the pBlockData control structure
    /// @param pWorkBlock
    /// @return
    static inline uint16_t S_GetBlockHeadSize() noexcept { return static_cast< uint16_t >(sizeof(PBlockHead)); }
    /// @brief Check if it contains cross-page data
    /// @param workBlock
    /// @return
    static bool S_IsHaveSpread(PBlockHead const &workBlock) noexcept;
    /// @brief Clear memory
    /// @param pBDstData
    /// @param nDataLen
    static void S_ClearMemory(void *const pBDstData, uint16_t const nDataLen) noexcept;

protected:  // business logic related
    /// @brief Find a data block
    /// @param nPlanID
    /// @return
    uint16_t _FindFreeBlockID(uint16_t const nPlanID) const noexcept;
    /// @brief Find a data block
    /// @param eBlockType
    /// @param nBlockID
    /// @return
    PBlockHead const *_FindBlock(EBlockType const eBlockType, uint16_t const nBlockID) const noexcept;
    /// @brief Find a data block
    /// @param eBlockType
    /// @param nBlockID
    /// @return
    PBlockHead *_FindBlock(EBlockType const eBlockType, uint16_t const nBlockID) noexcept;
    /// @brief Find the Block data area
    /// @param nBlockID
    /// @return
    PBlockHead const *_FindBlock(uint16_t const nBlockID) const noexcept;
    /// @brief Find the Block data area
    /// @param nBlockID
    /// @return
    PBlockHead *_FindBlock(uint16_t const nBlockID) noexcept;
    /// @brief
    /// @param nBlockID
    /// @return
    uint8_t *_FindBlockData(uint16_t const nBlockID) const noexcept;
    /// @brief Add a local data header
    /// @param eRangeType
    /// @param eBlockType
    /// @param nDataLen
    /// @return
    PBlockHead *_AddBlockHead(EStoreRange const eRangeType,
                              EBlockType const eBlockType,
                              uint16_t const nDataLen) noexcept;
    /// @brief Save BlockBody data: spanning pages
    /// @param workBlock
    /// @param pBData
    /// @param nDataLen
    /// @param pSpreadBlock
    /// @return
    int32_t _BlockBodySaveData(PBlockHead const &workBlock,
                               uint8_t const *const pBData,
                               uint16_t const nDataLen,
                               PBlockSpread const *const pSpreadBlock) const noexcept;
    /// @brief Replace BlockBody data
    /// @param pWorkBlock
    /// @param pBData
    /// @param nDataLen
    void _BlockBodyReplace(PBlockHead *const pWorkBlock,
                           uint8_t const *const pBData,
                           uint16_t const nDataLen) const noexcept;
    /// @brief Replace BlockBody data
    /// @param pWorkBlock
    /// @param nOffset
    /// @param pBData
    /// @param nDataLen
    void _BlockBodyAmend(PBlockHead const *const pWorkBlock,
                         uint16_t nOffset,
                         uint8_t const *const pBData,
                         uint16_t const nDataLen) const noexcept;
    /// @brief Increase data within BlodkBody
    /// @param pWorkBlock
    /// @param pAddData
    /// @param nAddLen
    void _BlockBodyAddData(PBlockHead *const pWorkBlock,
                           uint8_t const *const pAddData,
                           uint16_t const nAddLen) const noexcept;
    /// @brief Decrease data within BlodkBody
    /// @param pWorkBlock
    /// @param nDecLen
    void _BlockBodyDecData(PBlockHead *const pWorkBlock, uint16_t const nDecLen) const noexcept;
    /// @brief Move a Block: move the header up, move the tail down
    /// @param pMoveBlock
    /// @param nMoveHeadLen
    /// @param nMoveBodyLen
    void _MoveBlock(PBlockHead *const pMoveBlock,
                    int32_t const nMoveHeadLen,
                    int32_t const nMoveBodyLen) const noexcept;
    /// @brief Move all nodes after the pWorkBlock node (bInvertedOrder specifies whether to traverse in reverse order)
    /// @param pMoveBlock
    /// @param nAmendOffset
    /// @param bInvertedOrder
    /// @return
    int32_t _MoveBlockList(PBlockHead *const pMoveBlock,
                           int32_t const nAmendOffset,
                           bool const bInvertedOrder) noexcept;

protected:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
