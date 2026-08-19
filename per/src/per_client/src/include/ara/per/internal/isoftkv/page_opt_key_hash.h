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
/// @file       page_opt_key_hash.h
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

#ifndef ARA_PER_PHKV_PAGE_OPT_KEY_HASH_H_
#define ARA_PER_PHKV_PAGE_OPT_KEY_HASH_H_

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_solo.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief Key index: Hash
class PPageOptKeyHash : public PPageOptSolo
{
public:
    /// @brief
    /// @param a
    PPageOptKeyHash(PPageOptKeyHash const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptKeyHash &operator=(PPageOptKeyHash const &a) = delete;
    /// @brief
    /// @param a
    PPageOptKeyHash(PPageOptKeyHash &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptKeyHash &operator=(PPageOptKeyHash &&a) = delete;
    /// @brief
    /// @param pCachePagePtr
    /// @param nDefBlockID
    explicit PPageOptKeyHash(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID = 0U) noexcept;
    /// @brief
    ~PPageOptKeyHash() noexcept override = default;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;

public:
    /// @brief Calculate the Hash value of stKey
    /// @param stKey
    /// @return
    static uint64_t ComputeHashKey(ara::core::StringView const &stKey) noexcept;
    /// @brief Calculate the Hash value of stKey
    /// @param pBData
    /// @param nDataLen
    /// @return
    static uint64_t ComputeHashKey(uint8_t const *const pBData, int32_t const nDataLen) noexcept;

public:
    /// @brief
    /// @param nMaxCapacity
    /// @param nInitCount
    /// @param nDefKvPageID
    /// @param nHashType
    /// @param nCompressType
    /// @return
    bool InitHashHead(uint16_t const nMaxCapacity,
                      uint16_t const nInitCount,
                      uint32_t const nDefKvPageID,
                      uint8_t const nHashType,
                      uint8_t const nCompressType) noexcept;
    /// @brief Modify the value of the nHashIndex array element
    /// @param nHashIndex
    /// @param nHashPageID
    /// @return
    bool UpdateHashValueByIndex(uint16_t const nHashIndex, uint32_t const nHashPageID) const noexcept;
    /// @brief Change all old PageID values to the new PageID
    /// @param nOldPageID
    /// @param nNewPageID
    /// @return
    bool UpdateHashValue(uint32_t const nOldPageID, uint32_t const nNewPageID) const noexcept;
    /// @brief
    /// @param nBegin
    /// @param nCount
    /// @param nOldPageID
    /// @param nNewPageID
    /// @return
    bool UpdateHashValue(int32_t const nBegin,
                         int32_t const nCount,
                         uint32_t const nOldPageID,
                         uint32_t const nNewPageID) const noexcept;
    /// @brief Double the Hash pool capacity: return value -1 on failure, otherwise the increment of expansion
    /// @param nKvPageCount
    /// @return
    int32_t TryToExpansionHashCount(uint32_t const nKvPageCount) noexcept;
    /// @brief Halve the Hash pool capacity
    /// @param nKvPageCount
    /// @return
    bool TryToReduceHashCount(uint32_t const nKvPageCount) const noexcept;
    // The following functions must be called only after ensuring IsAccessReady returns true
public:
    /// @brief Find the page management data area
    /// @return
    PBlockHeadKeyHash *GetHeadHash() const noexcept;
    /// @brief Find the Hash value
    /// @param nSrcHashKey
    /// @return
    uint32_t GetHashValue(uint64_t const nSrcHashKey) const noexcept;
    /// @brief Find the Hash value
    /// @param stKey
    /// @return
    uint32_t GetHashValue(ara::core::StringView const &stKey) const noexcept;
    /// @brief Get the current number of hash buckets
    /// @return
    uint16_t GetHashCount() const noexcept;
    /// @brief Get half of the current number of hash buckets
    /// @return
    uint16_t GetHashCountHalf() const noexcept;
    /// @brief
    /// @param nSrcHashKey
    /// @return
    int32_t GetHashIndex(uint64_t const nSrcHashKey) const noexcept;
    /// @brief
    /// @param nHashKeyID
    /// @return
    int32_t GetHashIndex(uint32_t const nHashKeyID) const noexcept;
    /// @brief
    /// @param stKey
    /// @return
    int32_t GetHashIndex(ara::core::StringView const &stKey) const noexcept;
    /// @brief Get the expanded paired index of nIndex
    /// @param nHashIndex
    /// @return
    int32_t GetHashIndexPair(int32_t const nHashIndex) const noexcept;
    /// @brief Find the hash index where nHashKeyID appears for the first time
    /// @param nMatchPageID
    /// @return
    int32_t FindFirstHashIndex(uint32_t const nMatchPageID) const noexcept;
    /// @brief Return the number of KeyPages
    /// @return
    uint32_t GetKvPageCount() const noexcept;
    /// @brief Get the initial number of hash buckets
    /// @return
    uint8_t GetHashInitCount() const noexcept;
    /// @brief Get the number of hashes using the same PageID
    /// @param nMatchPageID
    /// @return
    int32_t GetMatchHashCount(uint32_t const nMatchPageID) const noexcept;
    /// @brief Maintain the number of KeyPages
    /// @param nAddCount
    void AddKeyPageCount(int32_t const nAddCount) const noexcept;
    /// @brief Iterate over all hash values: bExceptSameID indicates whether to remove the same PageID
    /// @param bExceptSameID
    /// @param pfun
    /// @return
    int32_t ForEachHash(
        bool const bExceptSameID,
        std::function< int32_t(int32_t const nIndex, uint32_t const nPageID) > const &pfun) const noexcept;
    /// @brief Determine whether the current page is a Hash-exclusive page or a mixed page with only ExtString page
    /// @return
    bool IsHashPageSolo() const noexcept;

protected:
    /// @brief
    /// @param pBlockHead
    /// @param nElementIndex
    /// @return
    static PBlockElementKeyHash &S_GetBlockElement(PBlockHeadKeyHash *const pBlockHead,
                                                   int32_t const nElementIndex) noexcept;
    /// @brief
    /// @param pBlockHead
    /// @param nElementIndex
    /// @return
    static PBlockElementKeyHash const &S_GetBlockElement(PBlockHeadKeyHash const *const pBlockHead,
                                                         int32_t const nElementIndex) noexcept;
    /// @brief
    /// @param pBlockHead
    /// @param nHashIndex
    /// @return
    static uint32_t S_GetHashValue(PBlockHeadKeyHash const *const pBlockHead, int32_t const nHashIndex) noexcept;
    /// @brief
    /// @param pBlockHead
    /// @param nHashIndex
    /// @param nHashPageID
    static void S_SetHashValue(PBlockHeadKeyHash *const pBlockHead,
                               int32_t const nHashIndex,
                               uint32_t const nHashPageID) noexcept;
    /// @brief
    /// @param nData
    /// @return
    static bool S_IsPowerOf2(uint64_t const nData) noexcept;
    /// @brief Get the number of expansions needed
    /// @param blockHead
    /// @param nKvPageCount
    /// @return
    static uint16_t S_GetExpansionCount(PBlockHeadKeyHash const &blockHead, uint32_t const nKvPageCount) noexcept;

    /// @brief
    /// @param blockHead
    /// @param nIndex
    /// @return
    static bool S_CheckArrayIndex(PBlockHeadKeyHash const &blockHead, int32_t const nIndex) noexcept;
    /// @brief Batch update all hash values in the interval [nBegin, nBegin + nCount)
    /// @param pHashHead
    /// @param nBegin
    /// @param nCount
    /// @param nOldPageID
    /// @param nNewPageID
    static void S_UpdateHashValue(PBlockHeadKeyHash *const pHashHead,
                                  int32_t const nBegin,
                                  int32_t const nCount,
                                  uint32_t const nOldPageID,
                                  uint32_t const nNewPageID) noexcept;
    /// @brief Double the Hash pool capacity
    /// @param pBlockHead
    /// @return
    static int32_t S_ExpansionHashCount(PBlockHeadKeyHash *const pBlockHead) noexcept;
    /// @brief Halve the Hash pool capacity
    /// @param pBlockHead
    /// @return
    static int32_t S_ReduceHashCount(PBlockHeadKeyHash *const pBlockHead) noexcept;

protected:
    /// @brief Find the page management data area
    /// @param bCheck Whether to execute PER_Assert check
    /// @return
    PBlockHeadKeyHash *_GetHeadHash(bool const bCheck = true) const noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
