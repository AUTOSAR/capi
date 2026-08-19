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
/// @file       page_opt_kv_store.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Key hash management page
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
/// @unit_name=PPageOptKvStore
/// @unit_description=Logical page for parsing data storage
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

#ifndef ARA_PER_PHKV_PAGE_OPT_KS_ELEMENT_H_
#define ARA_PER_PHKV_PAGE_OPT_KS_ELEMENT_H_

#include <ara/core/string_view.h>

#include <vector>

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_solo.h"

namespace ara {
namespace per {
namespace isoftkv {

//********************************/
/// @brief KV node data page
class PPageOptKvStore : public PPageOptSolo
{
public:
    /// @brief
    using CB_EachKvBool
        = std::function< bool(int32_t const, PBlockElementKv const *const, PElementKvData const *const) >;
    /// @brief
    using CB_EachKvIntConst
        = std::function< int32_t(int32_t const, PBlockElementKv const *const, PElementKvData const *const) >;
    /// @brief
    using CB_EachKvInt = std::function< int32_t(int32_t, PBlockElementKv *const, PElementKvData *const) >;

public:
    /// @brief
    /// @param pCachePagePtr
    explicit PPageOptKvStore(PCachePagePtr const &pCachePagePtr) noexcept;
    /// @brief
    PPageOptKvStore() noexcept = delete;
    /// @brief
    ~PPageOptKvStore() noexcept override = default;
    /// @brief
    /// @param a
    PPageOptKvStore(PPageOptKvStore const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptKvStore &operator=(PPageOptKvStore const &a) = delete;
    /// @brief
    /// @param a
    PPageOptKvStore(PPageOptKvStore &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptKvStore &operator=(PPageOptKvStore &&a) = delete;

public:
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;

    /// @brief
    /// @param nKvStoreID
    /// @param nLastKvPageID
    /// @param bResetData
    /// @return
    bool InitKvHead(uint32_t const nKvStoreID, uint32_t const nLastKvPageID, bool const bResetData) noexcept;
    /// @brief Reset KV-Store: retain cross-page information
    /// @param bKeepNext
    /// @return
    bool ResetKvStore(bool const bKeepNext) const noexcept;
    /// @brief Add a Kv data item
    /// @param nSrcKeyHash
    /// @param nReddType
    /// @param pBData
    /// @param nDataLen
    /// @return
    PBlockElementKv *AddKvElement(uint64_t const nSrcKeyHash,
                                  uint8_t const nReddType,
                                  uint8_t const *const pBData,
                                  uint16_t const nDataLen) const noexcept;
    /// @brief Add a Kv data item
    /// @param pKvElement
    /// @param pKvData
    /// @return
    PBlockElementKv *AddKvElement(PBlockElementKv const *const pKvElement,
                                  PElementKvData const *const pKvData) const noexcept;
    /// @brief Delete a KV-Element
    /// @param nKvIndex
    /// @return
    bool DelKvElement(int32_t const nKvIndex) noexcept;
    /// @brief Replace the entire KV storage structure and data
    /// @param nKvIndex
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool ReplaceKvElement(int32_t const nKvIndex, uint8_t const *const pBData, uint16_t const nDataLen) noexcept;
    /// @brief Replace the locally stored Value in KV
    /// @param nKvIndex
    /// @param eValueType
    /// @param pBData
    /// @param nDataLen
    /// @param eReddType
    /// @param vecReddData
    /// @return
    bool ReplaceLocalValue(int32_t const nKvIndex,
                           EDataType const eValueType,
                           uint8_t const *const pBData,
                           uint16_t const nDataLen,
                           EReddType eReddType,
                           ara::core::Vector< uint8_t > const &vecReddData) const noexcept;
    /// @brief Get the size of the locally stored Value data
    /// @param nKvIndex
    /// @return
    uint32_t GetLocalValueLen(int32_t const nKvIndex) const noexcept;
    /// @brief Get the size of the locally stored Redd check data
    /// @param nKvIndex
    /// @return
    uint32_t GetLocalReddLen(int32_t const nKvIndex) const noexcept;
    /// @brief Get the corresponding KV-Element
    /// @param nKvIndex
    /// @return
    PBlockElementKv *GetKvElement(int32_t const nKvIndex) const noexcept;
    /// @brief Get the corresponding KV-Data
    /// @param pKvElement
    /// @return
    PElementKvData *GetKvData(PBlockElementKv const *const pKvElement) const noexcept;
    /// @brief Find the BlockIndex data of the KV
    /// @param nKvIndex
    /// @param eDataLogic
    /// @return
    PBlockIndex *FindBlockIndex(int32_t const nKvIndex, EDataLogic const eDataLogic) const noexcept;
    /// @brief Traverse KV-Elements from the end, find the first KV whose length is less than or equal to nMaxLen
    /// @param nMaxLen
    /// @return
    int32_t FindKvElementByLen(uint16_t const nMaxLen) const noexcept;
    /// @brief Traverse KV-Elements
    /// @param bFromBack
    /// @param pfun
    /// @return
    int32_t ForEachKv(bool const bFromBack, CB_EachKvIntConst const &pfun) const noexcept;
    /// @brief Delete all KVs with the same hash index
    /// @param pfun
    /// @return
    int32_t DelKvElement(CB_EachKvBool const &pfun) const noexcept;
    /// @brief Split KV-Elements into two parts according to rules
    /// @param pPageLeft
    /// @param pPageRight
    /// @param pfun
    /// @return
    bool DivKvElement(PCachePagePtr const &pPageLeft,
                      PCachePagePtr const &pPageRight,
                      CB_EachKvBool const &pfun) const noexcept;
    /// @brief Update the CRC16 checksum corresponding to the KV-Element
    /// @param nKvIndex
    /// @return
    bool UpdateKvCRC16(int32_t const nKvIndex) const noexcept;
    // The following functions must be called only after ensuring IsAccessReady returns true
public:
    /// @brief Assemble a new KV storage structure
    /// @param pBBuff Output BUFF
    /// @param nMaxBuffLen BUFF length
    /// @param nSrcKeyHash Calculated Hash value of the Key
    /// @param stKey Key string
    /// @param eValueType Value type
    /// @param pBValue Value content
    /// @param nValueLen Value length
    /// @param nReddType Redundancy type
    /// @param pBRedd Redundancy data
    /// @param nReddLen Redundancy length
    /// @return Actual length of the KV storage structure
    static int32_t AssembleKvData(uint8_t *const pBBuff,
                                  uint16_t const nMaxBuffLen,
                                  uint64_t const nSrcKeyHash,
                                  ara::core::StringView const &stKey,
                                  EDataType const eValueType,
                                  uint8_t const *const pBValue,
                                  uint32_t const nValueLen,
                                  EReddType const eReddType,
                                  uint8_t const *const pBRedd,
                                  uint32_t const nReddLen) noexcept;
    /// @brief Get the total length of the external KV page data
    /// @param pKvElement
    /// @param nKeyLen
    /// @param nValueLen
    /// @param nReddLen
    /// @return
    int32_t ComputeKvExtBlockLen(PBlockElementKv *const pKvElement,
                                 int32_t const nKeyLen,
                                 int32_t const nValueLen,
                                 int32_t const nReddLen) const noexcept;
    /// @brief Get the corresponding KV.Value
    /// @param eDataLogic
    /// @param nKvIndex
    /// @param pBlockIndex
    /// @param pLocalData
    /// @return
    bool GetKvLocalData(EDataLogic const eDataLogic,
                        int32_t const nKvIndex,
                        PBlockIndex *&pBlockIndex,
                        PDataLocal *&pLocalData) const noexcept;
    /// @brief Get the number of the next page of the cross-page KV page
    /// @return
    uint32_t GetNextPageID() const noexcept;
    /// @brief
    /// @param nPageID
    void SetNextPageID(uint32_t const nPageID) const noexcept;
    /// @brief Get the number of the previous page of the cross-page KV page
    /// @return
    uint32_t GetPrevPageID() const noexcept;
    /// @brief
    /// @param nPageID
    void SetPrevPageID(uint32_t const nPageID) const noexcept;
    /// @brief Determine whether the current page is a single-page KV
    /// @return
    bool IsSimplePage() const noexcept;
    /// @brief Get the number of KV-Elements
    /// @return
    int32_t GetKvElementCount() const noexcept;
    /// @brief Get the space occupied by KV
    /// @return
    uint16_t GetKvSpaceUse() const noexcept;
    /// @brief Get the remaining space of KV
    /// @return
    uint16_t GetKvSpaceFree() const noexcept;
    /// @brief Get the KV-StoreID
    /// @return
    uint32_t GetKvStoreID() const noexcept;
    /// @brief Set the KV-StoreID
    /// @param nPageID
    void SetKvStoreID(uint32_t const nPageID) const noexcept;
    /// @brief Get the last page data of the cross-page Kv chain
    /// @return
    uint32_t GetLastKvPageID() const noexcept;
    /// @brief Set the last page data of the cross-page KV chain
    /// @param nPageID
    void SetLastKvPageID(uint32_t const nPageID) const noexcept;
    /// @brief Check if the index is valid
    /// @param nKvIndex
    /// @return
    bool CheckElementIndex(int32_t const nKvIndex) const noexcept;
    /// @brief Check if the hash value matches
    /// @param pKvElement
    /// @param nSrcKeyHash
    /// @return
    bool IsMatchHashKey(PBlockElementKv const *const pKvElement, uint64_t const nSrcKeyHash) const noexcept;
    /// @brief Find the KV index via a KvElement pointer
    /// @param pFindKvElement
    /// @return
    int32_t FindKvIndex(PBlockElementKv const *const pFindKvElement) const noexcept;
    /// @brief Count the hash value index with the highest frequency after grouping by nHashCount
    /// @param nExcepHashIndex
    /// @param nHashCount
    /// @return
    int32_t StatMaxHashCountIndex(int32_t const nExcepHashIndex, uint32_t const nHashCount) const noexcept;
    /// @brief Count the number of Kv pairs whose HashIndex matches the input value
    /// @param nStatHashIndex
    /// @param nHashCount
    /// @return
    int32_t StatKvCountByHashIndex(int32_t const nStatHashIndex, uint32_t const nHashCount) const noexcept;
    /// @brief Get PBlockHeadKvStore
    /// @return
    PBlockHeadKvStore *GetHeadKvStorage() const noexcept;
    /// @brief Check KV storage
    /// @param bPrint Whether to output KV information
    /// @return
    bool CheckKvList(bool bPrint = false) noexcept;

protected:
    /// @brief
    /// @param bFromBack
    /// @param pfun
    /// @return
    int32_t _ForEachKv(bool const bFromBack, CB_EachKvIntConst const &pfun) const noexcept;
    /// @brief Calculate the effective length of the KV control area: logical length
    /// @return
    int32_t _ComputeKvHeadLen() const noexcept;
    /// @brief Calculate the effective length of the KV data area
    /// @return
    int32_t _ComputeKvBodyLen() const noexcept;

protected:
    /// @brief Find the page management data area
    /// @return
    PBlockHeadKvStore *_GetKvHead() const noexcept;
    /// @brief Get the corresponding KV-Element
    /// @param nKvIndex
    /// @return
    PBlockElementKv *_GetKvElement(int32_t const nKvIndex) const noexcept;
    /// @brief Get the corresponding KV-Data
    /// @param pKvElement
    /// @return
    PElementKvData *_GetKvData(PBlockElementKv const *const pKvElement) const noexcept;
    /// @brief Move a Kv-Element
    /// @param pKvHead
    /// @param pFindElement
    /// @param nMoveHeadLen
    /// @param nMoveBodyLen
    void _MoveElement(PBlockHeadKvStore *const pKvHead,
                      PBlockElementKv *const pFindElement,
                      int32_t const nMoveHeadLen,
                      int32_t const nMoveBodyLen) const noexcept;
    /// @brief Move the KV control area of all nodes after a specific node
    /// @param pKvHead
    /// @param nFindKvIndex
    /// @param nMoveLen
    /// @return
    int32_t _MoveListElementHead(PBlockHeadKvStore *const pKvHead,
                                 int32_t const nFindKvIndex,
                                 int32_t const nMoveLen) noexcept;
    /// @brief Move the KV data area of all nodes after a specific node
    /// @param pKvHead
    /// @param nFindKvIndex
    /// @param nMoveLen
    /// @return
    int32_t _MoveListElementBody(PBlockHeadKvStore *const pKvHead,
                                 int32_t const nFindKvIndex,
                                 int32_t const nMoveLen) noexcept;
    /// @brief Initialize KV-Element data
    /// @param pKvHead
    /// @param pKvElement
    /// @param pBData
    /// @param nDataLen
    /// @param nKeyHashID
    /// @param nReddType
    void _SaveKvElementData(PBlockHeadKvStore const *const pKvHead,
                            PBlockElementKv *const pKvElement,
                            uint8_t const *const pBData,
                            uint8_t const nDataLen,
                            uint32_t const nKeyHashID,
                            uint8_t const nReddType) const noexcept;
    /// @brief Replace KV-Element data
    /// @param pKvHead
    /// @param pKvElement
    /// @param pBData
    /// @param nDataLen
    void _KvElementReplace(PBlockHeadKvStore *const pKvHead,
                           PBlockElementKv *const pKvElement,
                           uint8_t const *const pBData,
                           uint16_t const nDataLen) const noexcept;
    /// @brief Get the possibly existing 64-bit KeyHash
    /// @param pKvElement
    /// @return
    uint64_t _GetLocalHashKey64(PBlockElementKv const *const pKvElement) const noexcept;

protected:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
