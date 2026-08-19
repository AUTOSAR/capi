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
/// @file       manager_kv_store.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Kv storage area manager used by Puhua Key-Value Storage
/// @date       2021-05-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/KV Storage Area Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManagerKvStore
/// @unit_description=KV Storage Area Management Class
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-20 | 1.0.0   | hanjingjing  | Initial version created, functionality separated from kv_system |
///
/// ================================================================

#ifndef ARA_PER_PHKV_MANAGER_KV_STORE_BASE_H_
#define ARA_PER_PHKV_MANAGER_KV_STORE_BASE_H_

#include <string>

#include "ara/core/vector.h"
#include "ara/per/internal/crypto/i_redd_calculate.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/manager_page.h"
#include "ara/per/internal/isoftkv/manager_value.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_base.h"
#include "ara/per/internal/isoftkv/page_opt_key_hash.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Kv storage area manager used by Puhua KvStorage
class PManagerKvStore : public PKvObject
{
private:
    /// @brief
    PConfigMuster_Kv &kvConfigMuster_;
    /// @brief
    PManagerCache &managerCache_;
    /// @brief
    PManagerPage &managerPage_;
    // Execution class for cache operations
private:
    /// @brief KeyHash resident memory page
    PPageOptKeyHash pageOptHash_;
    /// @brief Value page manager
    PManagerValue managerValue_;

private:
    /// @brief Callback function for calculating redundant CRC/Hash
    std::unique_ptr< IReddAlgorithm > pReddAlgorithm_;

public:
    /// @brief
    using CB_ScanKv = std::function< int32_t(PCachePagePtr const &pPageKv, int32_t const nKvIndex) >;
    /// @brief
    /// @param kvConfigMuster
    /// @param managerCache
    /// @param managerPage
    PManagerKvStore(PConfigMuster_Kv &kvConfigMuster, PManagerCache &managerCache, PManagerPage &managerPage) noexcept;
    /// @brief
    ~PManagerKvStore() noexcept override = default;
    /// @brief
    /// @param a
    PManagerKvStore(PManagerKvStore const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerKvStore &operator=(PManagerKvStore const &a) = delete;
    /// @brief
    /// @param a
    PManagerKvStore(PManagerKvStore &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerKvStore &operator=(PManagerKvStore &&a) = delete;
    /// @brief Check if ready for read/write
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the Kv library name output by the logging system
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;
    /// @brief Set the interface object for calculating redundant CRC/HASH
    /// @param [in] pfun Externally configured object pointer
    /// @return Success or failure
    bool AttachReddAlgorithm(std::unique_ptr< IReddAlgorithm > pfun) noexcept;
    /// @brief Get the redundancy check
    /// @return
    IReddAlgorithm *GetReddAlgorithm() const noexcept;

public:
    /// @brief Initialize the resident memory HashOpt
    /// @return
    bool InitOptPageHash() noexcept;
    /// @brief Release the resident memory Opt
    /// @return
    bool ReleasePageOpt() noexcept;
    /// @brief Check if a key exists
    /// @param stKey
    /// @return
    bool IsKeyExist(ara::core::StringView const &stKey) noexcept;
    /// @brief Search the entire file for the value corresponding to the key
    /// @param stKey
    /// @param pfun
    /// @return
    bool ReadValueByKey(ara::core::StringView const &stKey, PPageOptBase::CB_ReadData const &pfun) noexcept;
    /// @brief Get the potentially existing redundancy check data for a KV pair
    /// @param stKey
    /// @param [out] vecReddData
    /// @return Success or failure
    bool ReadReddDataByKey(ara::core::StringView const &stKey, ara::core::Vector< uint8_t > &vecReddData) noexcept;
    /// @brief Write a new KV-Element
    /// @param stKey
    /// @param eValueType
    /// @param pBValue
    /// @param nValueLen
    /// @return
    bool WriteValue(ara::core::StringView const &stKey,
                    EDataType const eValueType,
                    uint8_t const *const pBValue,
                    uint32_t const nValueLen) noexcept;
    /// @brief Write a new KV-Element
    /// @param stKey
    /// @param eValueType
    /// @param nValueTotal
    /// @return
    bool WriteValue(ara::core::StringView const &stKey,
                    EDataType const eValueType,
                    uint32_t const nValueTotal) noexcept;
    /// @brief Delete a KV-Element
    /// @param stKey
    /// @return
    bool DeleteKvElement(ara::core::StringView const &stKey) noexcept;
    /// @brief Enumerate all key values
    /// @param nFirstPageID
    /// @param bDebugLog
    /// @param pfun
    /// @return
    int32_t ScanKvByPageID(uint32_t const nFirstPageID, bool const bDebugLog, CB_ScanKv const &pfun) noexcept;
    /// @brief Iterate over all KV pairs, executing the callback function for each
    /// @param pfun
    /// @return
    int32_t ScanAllKvPair(CB_ScanKv const &pfun) noexcept;
    /// @brief Enumerate all PageIDs encountered during reading the Value of a KV-Element
    /// @param stKey
    /// @param pfun
    /// @return
    ara::core::Vector< uint32_t > EnumReadValuePageID(ara::core::StringView const &stKey) noexcept;

public:
    /// @brief Check if there is any KV-Element (true if there is at least one, false if none)
    /// @return
    bool IsHaveKvElement() const noexcept;
    /// @brief Get the key value
    /// @param pKvPage
    /// @param nKvIndex
    /// @param pfun
    /// @return
    bool GetKvKeyData(PCachePagePtr const &pKvPage,
                      int32_t const nKvIndex,
                      PPageOptBase::CB_ReadData const &pfun) const noexcept;
    /// @brief Get the value value: without CRC/Hash redundancy check
    /// @param pKvPage
    /// @param nKvIndex
    /// @param pfun
    /// @return
    bool GetKvValueData(PCachePagePtr const &pKvPage,
                        int32_t const nKvIndex,
                        PPageOptBase::CB_ReadData const &pfun) const noexcept;
    /// @brief Get the length of the value
    /// @param stKey
    /// @param pReturnDataType
    /// @return
    int32_t GetKvValueLen(ara::core::StringView const &stKey, int32_t *const pReturnDataType) noexcept;
    /// @brief Get the length of the value
    /// @param pKvPage
    /// @param nKvIndex
    /// @param pReturnDataType
    /// @return
    int32_t GetKvValueLen(PCachePagePtr const &pKvPage,
                          int32_t const nKvIndex,
                          int32_t *const pReturnDataType) noexcept;
    /// @brief Transcode ValueData
    /// @param nHaveCopyLen
    /// @param pReadBuff
    /// @param nBuffLen
    /// @param pReadData
    /// @param nReadLen
    /// @return
    static int32_t CopyValueData(int32_t const nHaveCopyLen,
                                 uint8_t *const pReadBuff,
                                 int32_t const nBuffLen,
                                 uint8_t const *const pReadData,
                                 int32_t const nReadLen) noexcept;
    /// @brief Get the key value returning MString
    /// @param pKvPage
    /// @param nKvIndex
    /// @return
    ara::core::String GetKvKeyString(PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept;
    /// @brief Print the Key values within a KV page
    /// @return
    int32_t Debug_PrintKvStore() noexcept;

protected:
    /// @brief Find the PageID and ArrayIndex where KvElement is located: nFindPageID is the expected saved PageID,
    /// @param stKey
    /// @param bCheckValid
    /// @param nFindPageID
    /// @param nFindKvIndex
    /// @return nFindPageID and nFindKvIndex are both non -1 to indicate a successful find
    bool _FindKvElement(ara::core::StringView const &stKey,
                        bool const bCheckValid,
                        uint32_t &nFindPageID,
                        int32_t &nFindKvIndex) noexcept;
    /// @brief Check if stKey exists
    /// @param stKey
    /// @return
    bool _FindKvElement(ara::core::StringView const &stKey) noexcept;
    /// @brief Return the corresponding KV.key
    /// @param pKvPage
    /// @param nKvIndex
    /// @param stKey
    /// @return
    bool _GetKvKeyData(PCachePagePtr const &pKvPage, int32_t const nKvIndex, ara::core::String &stKey) const noexcept;
    /// @brief Delete the corresponding KV.Value
    /// @param pKvPage
    /// @param nKvIndex
    /// @return
    bool _DelKvValueData(PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept;
    /// @brief Copy all Elements from the old KV page to the new page
    /// @param pPageOptOld
    /// @param pPageOptNew
    /// @return
    bool _CopyKvElement(PPageOptKvStore *const pPageOptOld, PPageOptKvStore *const pPageOptNew) const noexcept;
    /// @brief Maintain the free space list in the management page (boss)
    /// @param optKvPage
    /// @return
    bool _UpdateFreeListKv(PPageOptKvStore const &optKvPage) noexcept;
    /// @brief Connect the new KV page to the old KV page
    /// @param pKvPage
    /// @param pKvPageNew
    void _LinkKvPage(PCachePagePtr const &pKvPage, PCachePagePtr const &pKvPageNew) noexcept;
    /// @brief Attempt to move a KV-Element from the last page of a cross-page chain to the current page
    /// @param pOptKvPage
    /// @return
    bool _TryToFillKvPage(PPageOptKvStore *const pOptKvPage) noexcept;
    /// @brief Attempt to add a KV-Element, returns 1 if successfully added, 0 if insufficient space, -1 on error
    /// @param pWorkKvPage
    /// @param stKey
    /// @param eValueType
    /// @param pBValue
    /// @param nValueLen
    /// @return
    int32_t _TryToAddKvElement(PCachePagePtr const &pWorkKvPage,
                               ara::core::StringView const &stKey,
                               EDataType const eValueType,
                               uint8_t const *const pBValue,
                               int32_t const nValueLen) noexcept;
    /// @brief
    /// @param pWorkKvPage
    /// @param nKvIndex
    /// @param eDataLogic
    /// @param pSavePageValue
    /// @param pBData
    /// @param nDataLen
    /// @param eDataType
    /// @return
    bool _SaveBlockValue(PCachePagePtr const &pWorkKvPage,
                         int32_t const nKvIndex,
                         EDataLogic const eDataLogic,
                         PCachePagePtr const &pSavePageValue,
                         uint8_t const *const pBData,
                         int32_t const nDataLen,
                         EDataType const eDataType) noexcept;
    /// @brief Reclaim all Next pages of the pKvPage page, returns the number of reclaimed pages
    /// @param pKvPage
    /// @return
    int32_t _RecoverAllNextKvPage(PCachePagePtr const &pKvPage) noexcept;
    /// @brief Get the key value
    /// @param pKvPage
    /// @param nKvIndex
    /// @param pfun
    /// @return
    bool _GetKvKeyData(PCachePagePtr const &pKvPage,
                       int32_t const nKvIndex,
                       PPageOptBase::CB_ReadData const &pfun) const noexcept;
    /// @brief Get the value value: without checking CRC/Hash redundancy data
    /// @param pKvPage
    /// @param nKvIndex
    /// @param pfun
    /// @return
    bool _GetKvValueData(PCachePagePtr const &pKvPage,
                         int32_t const nKvIndex,
                         PPageOptBase::CB_ReadData const &pfun) const noexcept;
    /// @brief Verify the redundancy information of a specific KV pair
    /// @param pKvPage
    /// @param nKvIndex
    /// @return Whether verification passed
    bool _CheckReddData(PCachePagePtr const &pKvPage, int32_t const nKvIndex) const noexcept;
    /// @brief Compare the Redd data saved in the library with the input Redd data
    /// @param pKvPage KV cache page
    /// @param nKvIndex KV pair number
    /// @param [in] vecReddData Input Redd data
    /// @return Whether consistent
    bool _CompareReddData(PCachePagePtr const &pKvPage,
                          int32_t const nKvIndex,
                          ara::core::Vector< uint8_t > const &vecReddData) const noexcept;
    /// @brief Get the redundancy information of a specific KV pair
    /// @param pKvPage
    /// @param nKvIndex
    /// @param [out] vecReddData Output Redd data
    /// @return Whether verification passed
    bool _GetReddData(PCachePagePtr const &pKvPage,
                      int32_t const nKvIndex,
                      ara::core::Vector< uint8_t > &vecReddData) const noexcept;

protected:
    /// @brief Create a new Kv page
    /// @param nCurPageID
    /// @param nKvStoreID
    /// @return
    PCachePagePtr _MakeNewSoloKvPage(uint32_t const nCurPageID, uint32_t const nKvStoreID) noexcept;
    /// @brief Create a new Kv page
    /// @param pCacheKv
    /// @return
    PCachePagePtr _MakeNewSoloKvPage(PCachePagePtr const &pCacheKv) noexcept;
    /// @brief Preparations before writing Kv data
    /// @param stKey
    /// @param nFindKvIndex
    /// @return
    PCachePagePtr _PrepareWriteKvPage(ara::core::StringView const &stKey, int32_t &nFindKvIndex) noexcept;
    /// @brief Prepare to add a new KV page: may trigger Hash page expansion and independence
    /// @param pCacheKv
    /// @param stKey
    /// @return
    PCachePagePtr _DivKvPage(PCachePagePtr const &pCacheKv, ara::core::StringView const &stKey) noexcept;
    /// @brief Change a KV page from a mixed page to an independent page, and return the new page
    /// @param pKvPageOld
    /// @return
    PCachePagePtr _SoloPageKv(PCachePagePtr const &pKvPageOld) noexcept;
    /// @brief Change a Hash page from a mixed page to an independent page, and return the new page
    /// @return
    PCachePagePtr _SoloPageHash() noexcept;
    /// @brief From the page starting at nFirstPageID, split the KV associated pages into two parts based on the Hash value size, and return the page where stKey should operate
    /// @param nFirstPageID
    /// @param nKeyHashIndex
    /// @return
    PCachePagePtr _DivKvPageByHashHalf(uint32_t const nFirstPageID, int32_t const nKeyHashIndex) noexcept;
    /// @brief Split all KV pairs of a specific HashIndex into a new page
    /// @param nFirstPageID
    /// @param nKeyHashIndex
    /// @return
    PCachePagePtr _DivKvPageByHashIndex(uint32_t const nFirstPageID, int32_t const nKeyHashIndex) noexcept;
    /// @brief Split a single page into two pages, and return the page where stKey should operate
    /// @param pCacheKv
    /// @param nKeyHashIndex
    /// @param nHashIndexMove
    /// @return
    PCachePagePtr _DivKvPageSimple(PCachePagePtr const &pCacheKv,
                                   int32_t const nKeyHashIndex,
                                   int32_t const nHashIndexMove) noexcept;
    /// @brief Starting from the page at nFirstPageID, split the KV page into two parts according to the callback function (pPageSaveLeft + new KV page)
    /// @param nFirstPageID
    /// @param pchLogType
    /// @param pPageSaveLeft
    /// @param pfun
    /// @return Returns the newly created KV page
    PCachePagePtr _DivKvPageComplex(char8_t const *const pchLogType,
                                    uint32_t const nFirstPageID,
                                    PCachePagePtr &pPageSaveLeft,
                                    PPageOptKvStore::CB_EachKvBool const &pfun) noexcept;
    /// @brief Save the KV-Element of the sub-page split from the KV page
    /// @param pPageTemp
    /// @param pPageSave
    /// @return
    PCachePagePtr _SaveSplitKvElementLeft(PCachePagePtr const &pPageTemp, PCachePagePtr const &pPageSave) noexcept;
    /// @brief Save the KV-Element of the sub-page split from the KV page
    /// @param pPageTemp
    /// @param pPageSave
    /// @param nCurPageID
    /// @return
    PCachePagePtr _SaveSplitKvElementRight(PCachePagePtr const &pPageTemp,
                                           PCachePagePtr const &pPageSave,
                                           uint32_t const nCurPageID) noexcept;

protected:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
