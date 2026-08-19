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
/// @file       manager_kv_store.cpp
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

#include "ara/per/internal/isoftkv/manager_kv_store.h"

#include <ara/core/map.h>

#include <algorithm>

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/isoft_kv_debug.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/page_opt_key_hash.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"
#include "ara/per/internal/isoftkv/page_opt_value.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Class for calculating redundant check data
class PCalculateReddData final : public PKvObject
{
private:
    EReddType eReddType_{EReddType::kNone};
    IReddAlgorithm *pReddAlgorithm_;

public:
    PCalculateReddData(EReddType eReddType, IReddAlgorithm *pReddAlgorithm)
        : eReddType_(eReddType), pReddAlgorithm_(pReddAlgorithm)
    {
    }
    /// @brief
    ~PCalculateReddData() noexcept override = default;
    /// @brief
    /// @param a
    PCalculateReddData(PCalculateReddData const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCalculateReddData &operator=(PCalculateReddData const &a) = delete;
    /// @brief
    /// @param a
    PCalculateReddData(PCalculateReddData &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCalculateReddData &operator=(PCalculateReddData &&a) = delete;
    /// @brief
    /// @return
    bool IsAccessReady() const noexcept override { return nullptr != pReddAlgorithm_; }
    /// @brief
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override { return ara::core::StringView{""}; }
    /// @brief Calculate redundancy check value (required for CRC/Hash, not for M-N)
    /// @param [in] pBSrcData Raw data to calculate the check value
    /// @param [in] nSrcLen Length of raw data to calculate the check value
    /// @param [in] bNeedReset Whether to reset the related state of the redundancy check calculator (generally needed on first call)
    /// @return Actual length of the calculated redundancy check data (-1 for failure, 0 for no check needed)
    uint32_t CalculateReddData(uint8_t const *const pBSrcData, uint32_t const nSrcLen, bool bNeedReset) const noexcept
    {
        // Redundancy check
        PER_Assert(nullptr != pBSrcData);
        PER_Assert(nSrcLen > 0);
        if (false == PConfigMuster::IsReddCheckType(eReddType_)) {
            PER_OnOptFailed(EErrorPHKV::kReddCrypto);
            return 0U;
        }
        if (nullptr == pReddAlgorithm_) {
            PER_OnOptFailed(EErrorPHKV::kReddCrypto);
            return 0U;
        }
        // Calculate redundancy value of CRC/Hash type
        if (bNeedReset) {
            pReddAlgorithm_->Reset();
        }
        if (false == pReddAlgorithm_->CalculateReddData(pBSrcData, nSrcLen)) {
            return 0U;
        }
        return 1U;
    }
    /// @brief Calculate redundancy check value (required for CRC/Hash, not for M-N)
    /// @param [in] pBKey Key data
    /// @param [in] nKeyLen Key length
    /// @param [in] pBValue Value data
    /// @param [in] nValueLen Value length
    /// @return Actual length of the calculated redundancy check data (-1 for failure, 0 for no check needed)
    ara::core::Vector< uint8_t > CalculateReddData(uint8_t const *const pBKey,
                                                   uint32_t const nKeyLen,
                                                   uint8_t const *const pBValue,
                                                   uint32_t const nValueLen) const noexcept
    {
        // Redundancy check
        if (false == PConfigMuster::IsReddCheckType(eReddType_)) {
            return ara::core::Vector< uint8_t >{};
        }
        if (CalculateReddData(pBKey, nKeyLen, true) <= 0) {
            LogError() << "An error occurred while calculating the CRC/Hash code for Key["
                       << T_StringView(static_cast< void const * >(pBKey), nKeyLen).data() << "].";
            PER_OnOptFailed(EErrorPHKV::kReddCrypto);
            return ara::core::Vector< uint8_t >{};
        }
        if (CalculateReddData(pBValue, nValueLen, false) <= 0) {
            LogError() << "An error occurred while calculating the CRC/Hash code for Key["
                       << T_StringView(static_cast< void const * >(pBKey), nKeyLen).data() << "].";
            PER_OnOptFailed(EErrorPHKV::kReddCrypto);
            return ara::core::Vector< uint8_t >{};
        }
        return GetCalculateReddData();
    }
    /// @brief Return the calculated redundancy check value (required for CRC/Hash, not for M-N)
    /// @return Actual length of the calculated redundancy check data (-1 for failure, 0 for no check needed)
    ara::core::Vector< uint8_t > GetCalculateReddData() const noexcept
    {
        if (false == PConfigMuster::IsReddCheckType(eReddType_)) {
            return ara::core::Vector< uint8_t >{};
        }
        if (nullptr == pReddAlgorithm_) {
            PER_OnOptFailed(EErrorPHKV::kReddCrypto);
            return ara::core::Vector< uint8_t >{};
        }
        return pReddAlgorithm_->GetResult();
    }
};
//********************************/
/// @brief
/// @param kvConfigMuster
/// @param managerCache
/// @param managerPage
PManagerKvStore::PManagerKvStore(PConfigMuster_Kv &kvConfigMuster,
                                 PManagerCache &managerCache,
                                 PManagerPage &managerPage) noexcept
    : kvConfigMuster_{kvConfigMuster}
    , managerCache_{managerCache}
    , managerPage_{managerPage}
    , pageOptHash_{nullptr, 0U}
    , managerValue_{managerCache, managerPage}
{
}
/// @brief Check if ready for read/write
/// @return
bool PManagerKvStore::IsAccessReady() const noexcept
{
    if (false == managerCache_.IsAccessReady()) {
        return false;
    }
    if (false == managerPage_.IsAccessReady()) {
        return false;
    }
    if (false == pageOptHash_.IsAccessReady()) {
        return false;
    }
    return true;
}
/// @brief Get the Kv library name output by the logging system
/// @return
ara::core::StringView PManagerKvStore::GetLogKvName() const noexcept { return std::move(managerCache_.GetLogKvName()); }
/// @brief Set the interface object for calculating redundant CRC/Hash
/// @param [in] pfun Externally configured object pointer
/// @return Success or failure
bool PManagerKvStore::AttachReddAlgorithm(std::unique_ptr< IReddAlgorithm > pfun) noexcept
{
    if (!pfun) {
        return false;
    }
    if (pfun->GetReddType() != kvConfigMuster_.GetReddType()) {
        return false;
    }
    pReddAlgorithm_.release();
    pReddAlgorithm_ = std::move(pfun);
    return true;
}
/// @brief Get the redundancy check
/// @return
IReddAlgorithm *PManagerKvStore::GetReddAlgorithm() const noexcept { return pReddAlgorithm_.get(); }
//***************/
/// @brief Initialize the resident memory HashOpt
/// @return
bool PManagerKvStore::InitOptPageHash() noexcept
{
    // Resident memory PageHash
    PCachePagePtr const pPageHash{managerPage_.LoadCacheHash()};
    if (nullptr == pPageHash) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return false;
    }
    if (false == pageOptHash_.InitCachePage(pPageHash)) {
        PER_OnOptFailed(pageOptHash_.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Release the resident memory Opt
/// @return
bool PManagerKvStore::ReleasePageOpt() noexcept
{
    pageOptHash_.DetachCachePage();
    return true;
}
/// @brief Check if a key exists
/// @param stKey
/// @return
bool PManagerKvStore::IsKeyExist(ara::core::StringView const &stKey) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    return _FindKvElement(stKey);
}
/// @brief Search the entire file for the value corresponding to the key
/// @param stKey
/// @param pfun
/// @return
bool PManagerKvStore::ReadValueByKey(ara::core::StringView const &stKey, PPageOptBase::CB_ReadData const &pfun) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    uint32_t nFindPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    if (false == _FindKvElement(stKey, true, nFindPageID, nFindKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return false;
    }
    // Load the found target page
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = managerPage_.LoadCachePage(nFindPageID, false);
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return false;
    }
    return GetKvValueData(pFindPageKv, nFindKvIndex, pfun);
}
/// @brief Get the potentially existing redundancy check data for a KV pair
/// @param stKey
/// @param [out] vecReddData
/// @return Success or failure
bool PManagerKvStore::ReadReddDataByKey(ara::core::StringView const &stKey,
                                        ara::core::Vector< uint8_t > &vecReddData) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    uint32_t nFindPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    if (false == _FindKvElement(stKey, true, nFindPageID, nFindKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return false;
    }
    // Load the found target page
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = managerPage_.LoadCachePage(nFindPageID, false);
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return false;
    }
    return _GetReddData(pFindPageKv, nFindKvIndex, vecReddData);
}
/// @brief Write a new KV-Element
/// @param stKey
/// @param eValueType
/// @param pBValue
/// @param nValueLen
/// @return
bool PManagerKvStore::WriteValue(ara::core::StringView const &stKey,
                                 EDataType const eValueType,
                                 uint8_t const *const pBValue,
                                 uint32_t const nValueLen) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    int32_t nFindKvIndex{-1};
    PCachePagePtr pKvPageWork;
    pKvPageWork = _PrepareWriteKvPage(stKey, nFindKvIndex);
    if (nullptr == pKvPageWork) {
        PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
        return false;
    }
    // If there is old data and the old data's KV area can directly accommodate the new value, then save directly
    if (nFindKvIndex >= 0) {
        // Determine whether the old data area can directly accommodate the current new data; if so, save directly.
        PPageOptKvStore optKvPage{pKvPageWork};
        if (optKvPage.GetLocalValueLen(nFindKvIndex) == nValueLen) {
            // Also need to replace the CRC and Hash in the redundancy handling
            PCalculateReddData calculateReddData{kvConfigMuster_.GetReddType(), pReddAlgorithm_.get()};
            ara::core::Vector< uint8_t > const vecReddData{calculateReddData.CalculateReddData(
                T_TransBytes(stKey.data()), static_cast< uint32_t >(stKey.size()), pBValue, nValueLen)};
            if (calculateReddData.IsHaveError()) {
                PER_OnOptFailed(calculateReddData.GetLastError());
                return false;
            }
            bool bEnableReplace{true};
            if (optKvPage.GetLocalReddLen(nFindKvIndex) != static_cast< uint32_t >(vecReddData.size())) {
                bEnableReplace = false;
            }
            EReddType const eReddType{kvConfigMuster_.GetReddType()};
            if (bEnableReplace) {
                bool const bReplace{optKvPage.ReplaceLocalValue(
                    nFindKvIndex, eValueType, pBValue, static_cast< uint16_t >(nValueLen), eReddType, vecReddData)};
                if (bReplace) {
                    // Note: return directly here
                    _LogDebug("[KV_Manager].ReplaceLocalValue", ": ", "PageID",
                              static_cast< int32_t >(optKvPage.GetPageID()), "KvIndex", nFindKvIndex);
                    _OnSuccess();
                    return true;
                }
            }
        }
        if (false == _DelKvValueData(pKvPageWork, nFindKvIndex)) {
            PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
            return false;
        }
    }
    int32_t nAddResult{0};
    while (true) {
        // First attempt: regardless of the state of the Kv page, try to add it once
        nAddResult = _TryToAddKvElement(pKvPageWork, stKey, eValueType, pBValue, static_cast< int32_t >(nValueLen));
        if (nAddResult != 0) {
            break;
        }
        // Change mixed page to Solo page: KvPage page
        PCachePagePtr pKvPageSolo;
        pKvPageSolo = _SoloPageKv(pKvPageWork);
        if (nullptr == pKvPageSolo) {
            PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
            return false;
        }
        // Second attempt: there might be space after the KV mixed page is made independent
        if (pKvPageSolo.operator bool()) {
            if ((*pKvPageSolo)->GetMainPageID() != (*pKvPageWork)->GetMainPageID()) {
                nAddResult
                    = _TryToAddKvElement(pKvPageSolo, stKey, eValueType, pBValue, static_cast< int32_t >(nValueLen));
                if (nAddResult != 0) {
                    break;
                }
                pKvPageWork = pKvPageSolo;
            }
        }
        // Still cannot fit on the exclusive KV page; continue expanding the KV storage page (split one KvPage into two pages according to rules)
        PCachePagePtr pKvPageDiv;
        // This may trigger a KeyHash expansion event
        pKvPageDiv = _DivKvPage(pKvPageWork, stKey);
        // Third attempt: adding a new KV storage page (KeyHash expansion) may provide new space
        if (nullptr != pKvPageDiv) {
            nAddResult = _TryToAddKvElement(pKvPageDiv, stKey, eValueType, pBValue, static_cast< int32_t >(nValueLen));
            if (nAddResult != 0) {
                break;
            }
            pKvPageWork = pKvPageDiv;
        }
        // Still cannot fit; create a new KV page (store data in the new page, and set the cross-page chain of the original page)
        PCachePagePtr pKvPageNew;
        pKvPageNew = _MakeNewSoloKvPage(pKvPageWork);
        if (nullptr == pKvPageNew) {
            PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
            return false;
        }
        // pKvPageWork needs to be protected
        PPageOptKvStore const optKvOld{pKvPageWork};
        // Fourth attempt: if unsuccessful, it's a failure
        nAddResult = _TryToAddKvElement(pKvPageNew, stKey, eValueType, pBValue, static_cast< int32_t >(nValueLen));
        if (nAddResult <= 0) {
            break;
        }
        _LinkKvPage(pKvPageWork, pKvPageNew);
        _LogDebug("[KV_Manager].AppendNewKv", ": ", "PageID", static_cast< int32_t >((*pKvPageWork)->GetMainPageID()),
                  "NewPageID", static_cast< int32_t >((*pKvPageNew)->GetMainPageID()));
        pKvPageWork = pKvPageNew;
        break;
    }

    if (nAddResult <= 0) {
        PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Write a new KV-Element
/// @param stKey
/// @param eValueType
/// @param nValueTotal
/// @return
bool PManagerKvStore::WriteValue(ara::core::StringView const &stKey,
                                 EDataType const eValueType,
                                 uint32_t const nValueTotal) noexcept
{
    return WriteValue(stKey, eValueType, nullptr, nValueTotal);
}
/// @brief Delete a KV-Element
/// @param stKey
/// @return
bool PManagerKvStore::DeleteKvElement(ara::core::StringView const &stKey) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    uint32_t nFindKvPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    if (false == _FindKvElement(stKey, true, nFindKvPageID, nFindKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return false;
    }
    // Load the found target page
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = managerPage_.LoadCachePage(nFindKvPageID, true);
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return false;
    }
    return _DelKvValueData(pFindPageKv, nFindKvIndex);
}
/// @brief Enumerate all key values
/// @param nFirstPageID
/// @param bDebugLog
/// @param pfun
/// @return
int32_t PManagerKvStore::ScanKvByPageID(uint32_t const nFirstPageID,
                                        bool const bDebugLog,
                                        CB_ScanKv const &pfun) noexcept
{
    int32_t nKvTotal{0};
    ara::core::Map< uint32_t, int32_t > mapKeyCount;
    uint32_t nWorkPageID{nFirstPageID};
    while (PPageOptBase::IsValidPageID(nWorkPageID)) {
        PCachePagePtr pKvPage{nullptr};
        pKvPage = managerPage_.LoadCachePage(nWorkPageID, false);
        PPageOptKvStore pageOptKv{pKvPage};
        if (false == pageOptKv.IsAccessReady()) {
            PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
            return nKvTotal;
        }
        mapKeyCount[nWorkPageID] = pageOptKv.GetKvElementCount();
        nKvTotal += pageOptKv.ForEachKv(
            false,
            [&pKvPage, &pfun](int32_t const nKvIndex, PBlockElementKv const *, PElementKvData const *) -> int32_t {
                std::ignore = pfun(pKvPage, nKvIndex);
                return 1;
            });
        nWorkPageID = pageOptKv.GetNextPageID();
    }
    // Output statistical logs
    if (bDebugLog) {
        ara::core::String stMsg{"    PageID|KvCount = {"};
        int32_t nPageCount{0};
        for (auto &it : mapKeyCount) {
            if (nPageCount > 0) {
                stMsg += ", ";
            }
            stMsg += std::to_string(it.first).c_str();
            stMsg += "|";
            stMsg += std::to_string(it.second).c_str();
            nPageCount += 1;
        }
        stMsg += "}";
        _LogDetail("[KV_Debug].ScanKvByPageID", ": ", "PageTotal", nPageCount, "KvTotal", nKvTotal, "FirstPage",
                   static_cast< int32_t >(nFirstPageID));
        _LogDetail(stMsg.c_str(), nullptr);
    }
    _OnSuccess();
    return nKvTotal;
}
/// @brief Iterate over all KV pairs, executing the callback function for each
/// @param pfun
/// @return
int32_t PManagerKvStore::ScanAllKvPair(CB_ScanKv const &pfun) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return -1;
    }
    int32_t nKvTotal{0};
    std::ignore = pageOptHash_.ForEachHash(
        true, [this, &pfun, &nKvTotal](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
            std::ignore = nIndex;
            nKvTotal += ScanKvByPageID(nPageID, false, pfun);
            return 1;
        });
    _OnSuccess();
    return nKvTotal;
}
/// @brief Enumerate all PageIDs encountered during reading the Value of a KV-Element
/// @param stKey
/// @param pfun
/// @return
ara::core::Vector< uint32_t > PManagerKvStore::EnumReadValuePageID(ara::core::StringView const &stKey) noexcept
{
    ara::core::Vector< uint32_t > vecPageID;
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return vecPageID;
    }
    // Record Key-Hash page
    vecPageID.push_back(pageOptHash_.GetPageID());
    uint32_t nFindPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    if (false == _FindKvElement(stKey, true, nFindPageID, nFindKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return vecPageID;
    }
    // Load the found target page
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = managerPage_.LoadCachePage(nFindPageID, false);
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return vecPageID;
    }
    // Record the found Key-Value page
    vecPageID.push_back(nFindPageID);
    // Record possible cross-page Value data
    std::ignore = GetKvValueData(pFindPageKv, nFindKvIndex,
                                 [&vecPageID](int32_t, uint8_t const *const, int32_t const nReadLen,
                                              uint32_t const nPageID, uint16_t const) -> int32_t {
                                     vecPageID.push_back(nPageID);
                                     return nReadLen;
                                 });
    _OnSuccess();
    return vecPageID;
}
//***************/
/// @brief Check if there is any KV-Element (true if there is at least one, false if none)
/// @return
bool PManagerKvStore::IsHaveKvElement() const noexcept
{
    if (false == IsAccessReady()) {
        return false;
    }
    bool bFindKv{false};
    std::ignore
        = pageOptHash_.ForEachHash(true, [this, &bFindKv](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
              std::ignore = nIndex;
              PCachePagePtr pKvPage{nullptr};
              pKvPage = managerPage_.LoadCachePage(nPageID, false);
              PPageOptKvStore pageOptKv{pKvPage};
              if (pageOptKv.IsAccessReady()) {
                  bFindKv = 0 != pageOptKv.GetKvElementCount();
              }
              if (bFindKv) {
                  return -1;
              }
              return 1;
          });
    return bFindKv;
}
/// @brief Get the key value
/// @param pKvPage
/// @param nKvIndex
/// @param pfun
/// @return
bool PManagerKvStore::GetKvKeyData(PCachePagePtr const &pKvPage,
                                   int32_t const nKvIndex,
                                   PPageOptBase::CB_ReadData const &pfun) const noexcept
{
    return _GetKvKeyData(pKvPage, nKvIndex, pfun);
}
/// @brief Get the value value: without CRC/Hash redundancy check
/// @param pKvPage
/// @param nKvIndex
/// @param pfun
/// @return
bool PManagerKvStore::GetKvValueData(PCachePagePtr const &pKvPage,
                                     int32_t const nKvIndex,
                                     PPageOptBase::CB_ReadData const &pfun) const noexcept
{
    // Redundancy check
    if (false == _CheckReddData(pKvPage, nKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvReddCheck);
        return false;
    }
    return _GetKvValueData(pKvPage, nKvIndex, pfun);
}
/// @brief Get the length of the value
/// @param stKey
/// @param pReturnDataType
/// @return
int32_t PManagerKvStore::GetKvValueLen(ara::core::StringView const &stKey, int32_t *const pReturnDataType) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return -1;
    }
    uint32_t nFindKvPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    if (false == _FindKvElement(stKey, true, nFindKvPageID, nFindKvIndex)) {
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return -1;
    }
    // Load the found target page
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = managerPage_.LoadCachePage(nFindKvPageID, false);
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return -1;
    }
    return GetKvValueLen(pFindPageKv, nFindKvIndex, pReturnDataType);
}
/// @brief Get the length of the value
/// @param pKvPage
/// @param nKvIndex
/// @param pReturnDataType
/// @return
int32_t PManagerKvStore::GetKvValueLen(PCachePagePtr const &pKvPage,
                                       int32_t const nKvIndex,
                                       int32_t *const pReturnDataType) noexcept
{
    PPageOptKvStore optPageKv{pKvPage};
    if (false == optPageKv.IsAccessReady()) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return -1;
    }
    if (false == optPageKv.CheckElementIndex(nKvIndex)) {
        // Normal logic, so PH_OnOptFailed is not added
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return -1;
    }
    PBlockIndex *pBlockIndex{nullptr};
    PDataLocal *pLocalValue{nullptr};
    if (false == optPageKv.GetKvLocalData(EDataLogic::kLogicValue, nKvIndex, pBlockIndex, pLocalValue)) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return -1;
    }
    int32_t nReadTotal{0};
    // Local data
    if (nullptr != pLocalValue) {
        nReadTotal = static_cast< int32_t >(pLocalValue->cDataLen);
        if (nullptr != pReturnDataType) {
            *pReturnDataType = static_cast< int32_t >(pLocalValue->cDataType);
        }
    } else {
        if ((nullptr == pBlockIndex) || (pBlockIndex->sBlockID <= 0U)) {
            // Error data: should not occur
            PER_OnOptFailed(optPageKv.GetLastError());
            return -1;
        }
        if (false == PPageOptBase::IsValidPageID(pBlockIndex->nPageID)) {
            // Error data: should not occur
            PER_OnOptFailed(optPageKv.GetLastError());
            return -1;
        }
        // If the KV pair is not found locally, look in external pages for the data:
        nReadTotal = managerValue_.GetValueTotalLen(*pBlockIndex, pReturnDataType);
        if (managerValue_.IsHaveError()) {
            PER_OnOptFailed(managerValue_.GetLastError());
            return -1;
        }
    }
    _OnSuccess();
    return nReadTotal;
}
/// @brief Transcode ValueData
/// @param nHaveCopyLen
/// @param pReadBuff
/// @param nBuffLen
/// @param pReadData
/// @param nReadLen
/// @return
int32_t PManagerKvStore::CopyValueData(int32_t const nHaveCopyLen,
                                       uint8_t *const pReadBuff,
                                       int32_t const nBuffLen,
                                       uint8_t const *const pReadData,
                                       int32_t const nReadLen) noexcept
{
    if ((nullptr == pReadData) || (nReadLen <= 0)) {
        return -1;
    }
    int32_t nCopyLen{nReadLen};
    if ((nullptr != pReadBuff) && (nBuffLen > 0)) {
        // Strategy: try to copy as much data as possible to the buffer
        nCopyLen = std::min< int32_t >(nReadLen, nBuffLen - nHaveCopyLen);
        if (nCopyLen > 0) {
            std::ignore = T_Memcpy(pReadBuff + nHaveCopyLen, pReadData, static_cast< uint32_t >(nCopyLen));
        }
    }
    return nCopyLen;
}
/// @brief Get the key value returning MString
/// @param pKvPage
/// @param nKvIndex
/// @return
ara::core::String PManagerKvStore::GetKvKeyString(PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept
{
    ara::core::String stKeyString;
    bool bReturn{false};
    bReturn = GetKvKeyData(pKvPage, nKvIndex,
                           [&stKeyString](int32_t const nDataType, uint8_t const *const pBData, int32_t const nDataLen,
                                          uint32_t, uint16_t) -> int32_t {
                               std::ignore = nDataType;
                               if ((nullptr == pBData) || (nDataLen <= 0)) {
                                   return 0;
                               }
                               stKeyString = ara::core::String{T_TransPtr< uint8_t, char8_t >(pBData),
                                                               static_cast< ara::core::String::size_type >(nDataLen)};
                               return nDataLen;
                           });
    if (false == bReturn) {
        PER_OnOptFailed(EErrorPHKV::kKvElementRead);
    }
    return stKeyString;
}
//********************************/
/// @brief Print the Key values within a KV page
/// @return
int32_t PManagerKvStore::Debug_PrintKvStore() noexcept
{
    PER_Assert(IsAccessReady());
    PConsoleLog consoleLog{"PrintKvStore"};
    Debug_PrintHash(consoleLog, pageOptHash_);
    _LogDebug("[PH_Debug].PrintKvStore", " Begin: ", "KvPageCount",
              static_cast< int32_t >(pageOptHash_.GetKvPageCount()));

    ara::core::Vector< uint32_t > vecPageID{};
    int32_t nFindCount{0};
    std::ignore = pageOptHash_.ForEachHash(true, [&vecPageID](int32_t, uint32_t const nPageID) -> int32_t {
        vecPageID.push_back(nPageID);
        return 1;
    });
    ara::core::Map< ara::core::String, int32_t > mapScanKey;
    for (uint32_t &nPageID : vecPageID) {
        ara::core::Vector< ara::core::String > vecKeyList;
        std::ignore = ScanKvByPageID(
            nPageID, true, [this, &vecKeyList](PCachePagePtr const &pKvPage, int32_t const nKvIndex) -> int32_t {
                ara::core::String stKey;
                if (false == _GetKvKeyData(pKvPage, nKvIndex, stKey)) {
                    return 0;
                }
                int32_t const nReadTotal{GetKvValueLen(pKvPage, nKvIndex, nullptr)};
                PER_Assert(nReadTotal > 0);
                vecKeyList.push_back(std::move(stKey));
                return 1;
            });
        nFindCount += static_cast< int32_t >(vecKeyList.size());
        for (auto &it : vecKeyList) {
            int32_t &nKeyCount{mapScanKey[it]};
            nKeyCount += 1;
            PER_Assert(nKeyCount == 1);
        }
    }
    _LogDebug("[PH_Debug].PrintKvStore", " End  :", "KvElementTotal", nFindCount);
    return nFindCount;
}
//********************************/
/// @brief Find the PageID and ArrayIndex where KvElement is located:
/// nFindPageID is the expected saved PageID; nFindPageID and nFindKvIndex both being non -1 indicates a successful find
/// @param stKey
/// @param bCheckValid
/// @param nFindPageID
/// @param nFindKvIndex
/// @return
bool PManagerKvStore::_FindKvElement(ara::core::StringView const &stKey,
                                     bool const bCheckValid,
                                     uint32_t &nFindPageID,
                                     int32_t &nFindKvIndex) noexcept
{
    std::ignore  = nFindPageID;
    std::ignore  = nFindKvIndex;
    nFindPageID  = kInvalidPageID;
    nFindKvIndex = -1;
    // Start searching from the first page of KV data pages
    uint32_t nScanKvPageID{pageOptHash_.GetHashValue(stKey)};
    // Handle cross-page
    while (PPageOptBase::IsValidPageID(nScanKvPageID)) {
        PCachePagePtr pKvPage{nullptr};
        pKvPage = managerPage_.LoadCachePage(nScanKvPageID, false);
        if (nullptr == pKvPage) {
            PER_OnOptFailed(EErrorPHKV::kPageNotFindPage);
            return false;
        }
        PPageOptKvStore pageOptKv{pKvPage};
        if (false == pageOptKv.IsAccessReady()) {
            printf("false == pageOptKv.IsAccessReady(), PageID = %u\n", pKvPage->GetMemoryPage()->GetMainPageID());
            PER_OnOptFailed(pageOptKv.GetLastError());
            return false;
        }
        uint64_t const nSrcKeyHash{PPageOptKeyHash::ComputeHashKey(stKey)};
        ara::core::Vector< int32_t > vecFind;
        PPageOptKvStore *const pPageOptKv{&pageOptKv};
        std::ignore = pageOptKv.ForEachKv(
            false,
            [pPageOptKv, nSrcKeyHash, &vecFind](int32_t const nKvIndex, PBlockElementKv const *const pKvElement,
                                                PElementKvData const *) -> int32_t {
                if (false == pPageOptKv->IsMatchHashKey(pKvElement, nSrcKeyHash)) {
                    return 0;
                }
                vecFind.push_back(nKvIndex);
                return 1;
            });
        // Iterate through the candidate list of Keys, searching for the corresponding KV-Element
        for (int32_t &nFindIndex : vecFind) {
            ara::core::String stTempKey;
            if (false == _GetKvKeyData(pKvPage, nFindIndex, stTempKey)) {
                continue;
            }
            if (stKey == std::move(T_StringView(stTempKey))) {
                nFindPageID  = pageOptKv.GetPageID();
                nFindKvIndex = nFindIndex;
                break;
            }
        }
        if ((PPageOptBase::IsValidPageID(nFindPageID)) && (nFindKvIndex >= 0)) {
            break;
        }
        nFindPageID   = nScanKvPageID;  // Record the traversed page number (useful when inserting a new KV)
        nScanKvPageID = pageOptKv.GetNextPageID();
    }
    _OnSuccess();
    if (bCheckValid)  // Whether to check the validity of the found data: even if not found, if traversal completes successfully, execution will reach here
    {
        return (PPageOptBase::IsValidPageID(nFindPageID)) && (nFindKvIndex >= 0);
    }
    return true;  // If true is returned, the caller must check if nFindPageID is valid
}
/// @brief Check if stKey exists
/// @param stKey
/// @return
bool PManagerKvStore::_FindKvElement(ara::core::StringView const &stKey) noexcept
{
    uint32_t nFindPageID{kInvalidPageID};
    int32_t nFindKvIndex{-1};
    bool const bReturn{_FindKvElement(stKey, true, nFindPageID, nFindKvIndex)};
    if ((nFindPageID == kInvalidPageID) || (-1 == nFindKvIndex)) {
        return false;
    }
    return bReturn;
}
/// @brief Return the corresponding KV.key
/// @param pKvPage
/// @param nKvIndex
/// @param stKey
/// @return
bool PManagerKvStore::_GetKvKeyData(PCachePagePtr const &pKvPage,
                                    int32_t const nKvIndex,
                                    ara::core::String &stKey) const noexcept
{
    bool bReturn{false};
    bReturn = GetKvKeyData(pKvPage, nKvIndex,
                           [&stKey](int32_t const nDataType, uint8_t const *const pBData, int32_t const nDataLen,
                                    uint32_t, uint16_t) -> int32_t {
                               std::ignore = nDataType;
                               if ((nullptr == pBData) || (nDataLen <= 0)) {
                                   return 0;
                               }
                               stKey = T_String(T_TransPtr< char8_t >(pBData), static_cast< uint32_t >(nDataLen));
                               return nDataLen;
                           });
    return bReturn;
}
/// @brief Delete the corresponding KV.Value
/// @param pKvPage
/// @param nKvIndex
/// @return
bool PManagerKvStore::_DelKvValueData(PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept
{
    // Load the found target page
    if (nullptr == pKvPage) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
        return false;
    }
    PPageOptKvStore optPageKv{pKvPage};
    if (false == optPageKv.IsAccessReady()) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // First delete possibly existing data in the ValuePage page
    EDataLogic const eDataLogic[static_cast< int32_t >(EDataLogic::kLogicCount)]{
        EDataLogic::kLogicKey, EDataLogic::kLogicValue, EDataLogic::kLogicRedd};

    for (auto const &it : eDataLogic) {
        PBlockIndex *pBlockIndex{nullptr};
        PDataLocal *pLocalData{nullptr};
        if (false == optPageKv.GetKvLocalData(it, nKvIndex, pBlockIndex, pLocalData)) {
            PER_OnOptFailed(optPageKv.GetLastError());
            return false;
        }
        if ((nullptr == pBlockIndex) || (pBlockIndex->sBlockID <= 0U)) {
            continue;
        }
        if (false == PPageOptBase::IsValidPageID(pBlockIndex->nPageID)) {
            continue;
        }
        if (false == managerValue_.DelBlockValue(*pBlockIndex)) {
            LogError() << "[PManagerKvStore::_DelKvValueData].DelBlockValue, nPageID =" << pBlockIndex->nPageID
                       << ", sBlockID =" << pBlockIndex->sBlockID;
        }
    }
    if (false == optPageKv.DelKvElement(nKvIndex)) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // 2021-09-03 Added optimization logic: automatically move a KV from the last page to the current page
    if (false == optPageKv.IsSimplePage()) {
        uint32_t const nKvPageID{optPageKv.GetPageID()};
        uint32_t const nFirstKvID{optPageKv.GetKvStoreID()};
        uint32_t nLastKvPageID{0U};
        if (nFirstKvID == nKvPageID) {
            nLastKvPageID = optPageKv.GetLastKvPageID();
        } else {
            PCachePagePtr const pFirstPageKv{managerPage_.LoadCachePage(nFirstKvID, true)};
            PER_Assert(nullptr != pFirstPageKv);
            PPageOptKvStore optFirstKv{pFirstPageKv};
            PER_Assert(optFirstKv.IsAccessReady());
            nLastKvPageID = optFirstKv.GetLastKvPageID();
        }
        // Itself is not the last page
        if (nLastKvPageID != nKvPageID) {
            PCachePagePtr const pLastPageKv{managerPage_.LoadCachePage(nLastKvPageID, true)};
            PER_Assert(nullptr != pLastPageKv);
            PPageOptKvStore optLastKv{pLastPageKv};
            PER_Assert(optLastKv.IsAccessReady());
            uint16_t const nKvFreeLen{optPageKv.GetKvSpaceFree()};
            int32_t nFindKvIndex{0};
            nFindKvIndex = optLastKv.FindKvElementByLen(nKvFreeLen);
            if (nFindKvIndex >= 0) {
                PBlockElementKv *const pKvElement{optLastKv.GetKvElement(nFindKvIndex)};
                PElementKvData *const pKvData{optLastKv.GetKvData(pKvElement)};
                PER_Assert(nullptr != optPageKv.AddKvElement(pKvElement, pKvData));
                PER_Assert(optLastKv.DelKvElement(nFindKvIndex));
                // Maintain the free space list in the management page (boss)
                std::ignore = _UpdateFreeListKv(optLastKv);
            }
        }
    } else {
        // Maintain the free space list in the management page (boss)
        if (false == _UpdateFreeListKv(optPageKv)) {
            LogError() << "[PManagerKvStore::_DelKvValueData]._UpdateFreeListKv, nPageID =" << optPageKv.GetPageID();
        }
    }
    _OnSuccess();
    return true;
}
/// @brief Copy all Elements from the old KV page to the new page
/// @param pPageOptOld
/// @param pPageOptNew
/// @return
bool PManagerKvStore::_CopyKvElement(PPageOptKvStore *const pPageOptOld,
                                     PPageOptKvStore *const pPageOptNew) const noexcept
{
    PER_Assert(nullptr != pPageOptOld);
    PER_Assert(nullptr != pPageOptNew);
    // Verify space
    if (pPageOptNew->GetKvSpaceFree() < pPageOptOld->GetKvSpaceUse()) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return false;
    }
    std::ignore = pPageOptOld->ForEachKv(false,
                                         [pPageOptNew](int32_t, PBlockElementKv const *const pKvElement,
                                                       PElementKvData const *const pKvData) -> int32_t {
                                             std::ignore = pPageOptNew->AddKvElement(pKvElement, pKvData);
                                             return 1;
                                         });
    return true;
}
/// @brief Maintain the free space list in the management page (boss)
/// @param optKvPage
/// @return
bool PManagerKvStore::_UpdateFreeListKv(PPageOptKvStore const &optKvPage) noexcept
{
    // Maintain free space
    if (optKvPage.GetSoloBlockID() > 0U) {
        if (optKvPage.GetKvElementCount() > 0) {
            return true;
        }
    }
    if (false == managerPage_.UpdateFreeList(optKvPage)) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return false;
    }
    return true;
}
/// @brief Connect the new KV page to the old KV page
/// @param pKvPage
/// @param pKvPageNew
void PManagerKvStore::_LinkKvPage(PCachePagePtr const &pKvPage, PCachePagePtr const &pKvPageNew) noexcept
{
    PPageOptKvStore optKvPrev{pKvPage};
    PPageOptKvStore optKvNext{pKvPageNew};
    PER_Assert(optKvPrev.IsAccessReady());
    PER_Assert(optKvNext.IsAccessReady());
    optKvPrev.SetNextPageID(optKvNext.GetPageID());
    optKvNext.SetPrevPageID(optKvPrev.GetPageID());
    uint32_t const nKvStoreID{optKvPrev.GetKvStoreID()};
    optKvNext.SetKvStoreID(nKvStoreID);
    PPageOptKvStore optKvFirst{managerPage_.LoadCachePage(nKvStoreID, true)};
    PER_Assert(optKvFirst.IsAccessReady());
    optKvFirst.SetLastKvPageID(optKvNext.GetPageID());
}
/// @brief Attempt to move a KV-Element from the last page of a cross-page chain to the current page
/// @param pOptKvPage
/// @return
bool PManagerKvStore::_TryToFillKvPage(PPageOptKvStore *const pOptKvPage) noexcept
{
    PER_Assert(nullptr != pOptKvPage);
    PPageOptKvStore &optPageKv{*pOptKvPage};
    uint32_t const nKvPageID{optPageKv.GetPageID()};
    // 2021-09-03 Added optimization logic: automatically move a KV from the last page to the current page
    if (optPageKv.IsSimplePage()) {
        // Maintain free space
        std::ignore = _UpdateFreeListKv(optPageKv);
    } else {
        // If it is a cross-page KV, look for a suitable KV in the last page to move to the current page
        uint32_t const nFirstKvID{optPageKv.GetKvStoreID()};
        uint32_t nLastKvPageID{0U};
        if (nFirstKvID == nKvPageID) {
            nLastKvPageID = optPageKv.GetLastKvPageID();
        } else {
            PPageOptKvStore optFirstKv{managerPage_.LoadCachePage(nFirstKvID, true)};
            PER_Assert(optFirstKv.IsAccessReady());
            nLastKvPageID = optFirstKv.GetLastKvPageID();
        }
        // Itself is not the last page
        if (nLastKvPageID != nKvPageID) {
            PPageOptKvStore optLastKv{managerPage_.LoadCachePage(nLastKvPageID, true)};
            PER_Assert(optLastKv.IsAccessReady());
            uint16_t const nKvFreeLen{optPageKv.GetKvSpaceFree()};
            int32_t nFindKvIndex{0};
            nFindKvIndex = optLastKv.FindKvElementByLen(nKvFreeLen);
            if (nFindKvIndex >= 0) {
                PBlockElementKv *const pKvElement{optLastKv.GetKvElement(nFindKvIndex)};
                PElementKvData *const pKvData{optLastKv.GetKvData(pKvElement)};
                PER_Assert(nullptr != optPageKv.AddKvElement(pKvElement, pKvData));
                PER_Assert(optLastKv.DelKvElement(nFindKvIndex));
                if (optLastKv.GetKvElementCount() <= 0) {
                    uint32_t const nNewLastPageID{optLastKv.GetPrevPageID()};
                    // Update the latest page within the first page of the cross-page chain: information in other pages becomes inaccurate
                    PPageOptKvStore optFirstKv{managerPage_.LoadCachePage(nFirstKvID, true)};
                    PER_Assert(optFirstKv.IsAccessReady());
                    optFirstKv.SetLastKvPageID(nNewLastPageID);
                }
                // Maintain free space
                std::ignore = _UpdateFreeListKv(optLastKv);
                std::ignore = _UpdateFreeListKv(optPageKv);
            }
        }
    }
    return true;
}
/// @brief Attempt to add a KV-Element, returns 1 if successfully added, 0 if insufficient space, -1 on error
/// @param pWorkKvPage
/// @param stKey
/// @param eValueType
/// @param pBValue
/// @param nValueLen
/// @return
int32_t PManagerKvStore::_TryToAddKvElement(PCachePagePtr const &pWorkKvPage,
                                            ara::core::StringView const &stKey,
                                            EDataType const eValueType,
                                            uint8_t const *const pBValue,
                                            int32_t const nValueLen) noexcept
{
    PPageOptKvStore pageOptKv{pWorkKvPage};
    if (false == pageOptKv.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return -1;
    }
    // Try to add the KV-Element
    uint64_t const nSrcKeyHash{PPageOptKeyHash::ComputeHashKey(stKey)};
    // Handle redundancy data (CRC and Hash verification)
    PCalculateReddData calculateReddData{kvConfigMuster_.GetReddType(), pReddAlgorithm_.get()};
    EReddType eReddType{kvConfigMuster_.GetReddType()};
    ara::core::Vector< uint8_t > vecReddData{calculateReddData.CalculateReddData(
        T_TransBytes(stKey.data()), static_cast< uint32_t >(stKey.size()), pBValue, nValueLen)};
    if (calculateReddData.IsHaveError()) {
        PER_OnOptFailed(calculateReddData.GetLastError());
        LogError() << "Find Error When CalculateReddData[" << static_cast< uint32_t >(eReddType)
                   << "], Key =" << stKey.data();
        return -1;
    }
    // Generate current Kv data
    uint16_t const nMaxBuffLenLimit{static_cast< uint16_t >(EDefaultValue::kLimitMaxKvDataLen)};
    isoftkv::PAutoBuff const autoBuf{nMaxBuffLenLimit};
    uint16_t nMaxBuffLen{static_cast< uint16_t >(kvConfigMuster_.GetMaxKvDataLen())};
    if (nMaxBuffLen > nMaxBuffLenLimit) {
        nMaxBuffLen = nMaxBuffLenLimit;
    }
    // Generate a copy of Kv data
    int32_t nBuffLen{0};
    nBuffLen = PPageOptKvStore::AssembleKvData(
        autoBuf.data(), nMaxBuffLen, nSrcKeyHash, stKey, eValueType, pBValue, static_cast< uint32_t >(nValueLen),
        eReddType, static_cast< uint8_t const * >(vecReddData.data()), static_cast< uint32_t >(vecReddData.size()));
    // Try to add to KvStore
    PBlockElementKv *pKvElement{nullptr};
    pKvElement = pageOptKv.AddKvElement(nSrcKeyHash, static_cast< uint8_t >(eReddType), autoBuf.data(),
                                        static_cast< uint16_t >(nBuffLen));
    if (nullptr == pKvElement) {
        if (pageOptKv.GetLastError() == EErrorPHKV::kPageLackSpace) {
            return 0;  // Lack of space, return 0
        }
    }
    // KV-Element added successfully; if there is PBlockIndex data, an empty PBlockIndex structure is written
    int32_t nExtDataTotal{0};
    nExtDataTotal = pageOptKv.ComputeKvExtBlockLen(pKvElement, static_cast< int32_t >(stKey.size()), nValueLen,
                                                   static_cast< int32_t >(vecReddData.size()));
    if (nExtDataTotal < 0) {
        PER_OnOptFailed(pageOptKv.GetLastError());
        return -1;
    }
    if (0 == nExtDataTotal) {
        return 1;
    }
    int32_t nKvIndex{0};
    nKvIndex = pageOptKv.FindKvIndex(pKvElement);  // Must be found
    PER_Assert(nKvIndex >= 0);
    PCachePagePtr pNewValuePage{nullptr};
    PPageOptValue const pageOptValue{pNewValuePage};
    uint32_t const nMaxValueValidLen{static_cast< uint32_t >(managerCache_.GetPageLen())
                                     - static_cast< uint32_t >(pageOptValue.GetMinValidLen())};
    // Value data fits on one page: find free positions for Key, Value, Redd each
    if (nExtDataTotal < static_cast< int32_t >(nMaxValueValidLen)) {  // Three-in-one storage
        pNewValuePage = managerPage_.PreparePageValue(pageOptKv.GetPageID(), static_cast< uint16_t >(nExtDataTotal));
        if (nullptr == pNewValuePage) {
            PER_OnOptFailed(managerPage_.GetLastError());
            return -1;
        }
    }
    if (false
        == _SaveBlockValue(pWorkKvPage, nKvIndex, EDataLogic::kLogicKey, pNewValuePage, T_TransBytes(stKey.data()),
                           static_cast< int32_t >(stKey.size()), EDataType::kString)) {
        return -1;
    }
    if (false
        == _SaveBlockValue(pWorkKvPage, nKvIndex, EDataLogic::kLogicValue, pNewValuePage, pBValue, nValueLen,
                           eValueType)) {
        return -1;
    }
    // Redundancy check may be empty; only non-empty data is saved
    if (false == vecReddData.empty()) {
        if (false
            == _SaveBlockValue(pWorkKvPage, nKvIndex, EDataLogic::kLogicRedd, pNewValuePage,
                               static_cast< uint8_t const * >(vecReddData.data()),
                               static_cast< int32_t >(vecReddData.size()), EDataType::kBinary)) {
            return -1;
        }
    }
    return 1;
}
/// @brief Save external page data
/// @param pWorkKvPage
/// @param nKvIndex
/// @param eDataLogic
/// @param pSavePageValue
/// @param pBData
/// @param nDataLen
/// @param eDataType
/// @return
bool PManagerKvStore::_SaveBlockValue(PCachePagePtr const &pWorkKvPage,
                                      int32_t const nKvIndex,
                                      EDataLogic const eDataLogic,
                                      PCachePagePtr const &pSavePageValue,
                                      uint8_t const *const pBData,
                                      int32_t const nDataLen,
                                      EDataType const eDataType) noexcept
{
    PPageOptKvStore pageOptKv{pWorkKvPage};
    if (false == pageOptKv.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    PBlockIndex *pBlockIndex{pageOptKv.FindBlockIndex(nKvIndex, eDataLogic)};
    if (nullptr == pBlockIndex) {
        _OnSuccess();
        return true;
    }
    PBlockIndex blockIndex;
    if (nullptr != pSavePageValue) {  // This is 3-in-1 storage
        blockIndex = managerValue_.SetBlockValue(pBData, nDataLen, eDataType, pSavePageValue);
    } else  // Re-find its storage location
    {
        blockIndex = managerValue_.SetBlockValue(pBData, nDataLen, eDataType, pageOptKv.GetPageID());
    }
    if (false == PPageOptBase::IsValidPageID(blockIndex.nPageID)) {
        PER_OnOptFailed(EErrorPHKV::kKvElementValue);
        return false;
    }
    _LogDebug("[KV_Manager].SaveBlockValue", ":", "PageID", static_cast< int32_t >(blockIndex.nPageID), "BlockID",
              static_cast< int32_t >(blockIndex.sBlockID), "nDataLen", nDataLen);
    // KV storage location needs to be recalculated because saving Value may cause KV storage area expansion due to the independence of the BossPage page
    pBlockIndex = pageOptKv.FindBlockIndex(nKvIndex, eDataLogic);
    if (nullptr != pBlockIndex) {
        *pBlockIndex = blockIndex;
    }
    if (false == pageOptKv.UpdateKvCRC16(nKvIndex)) {
        PER_OnOptFailed(pageOptKv.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Reclaim all Next pages of the pKvPage page, returns the number of reclaimed pages
/// @param pKvPage
/// @return
int32_t PManagerKvStore::_RecoverAllNextKvPage(PCachePagePtr const &pKvPage) noexcept
{
    PER_Assert(nullptr != pKvPage);
    PPageOptKvStore pageOptKv{pKvPage};
    if (false == pageOptKv.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return 0;
    }
    ara::core::Vector< uint32_t > vecRecoverPage;
    uint32_t nNextPageID{pageOptKv.GetNextPageID()};
    while (PPageOptBase::IsValidPageID(nNextPageID)) {
        PCachePagePtr pScanCache{nullptr};
        pScanCache = managerPage_.LoadCachePage(nNextPageID, true);
        PPageOptKvStore optKvNext{pScanCache};
        if (false == optKvNext.IsAccessReady()) {
            break;
        }
        // First save the next page PageID
        nNextPageID = optKvNext.GetNextPageID();
        uint32_t const nMainPageID{optKvNext.GetPageID()};
        // Release this page
        std::ignore = optKvNext.ResetPage(true);
        std::ignore = optKvNext.InitPageHead(EBlockType::kBaseEmpty, nMainPageID, true);
        // Update free list
        std::ignore = _UpdateFreeListKv(optKvNext);
        vecRecoverPage.push_back(nMainPageID);
    }
    pageOptKv.SetNextPageID(kInvalidPageID);
    int32_t const nRecoverCount{static_cast< int32_t >(vecRecoverPage.size())};
    if (nRecoverCount > 0) {
        pageOptHash_.AddKeyPageCount(-1 * nRecoverCount);
        PPrintLog logEvent{ara::log::LogLevel::kInfo, "[KV_Manager].RecoverPage"};
        logEvent.PrintLog(": WorkPageID =", (*pKvPage)->GetMainPageID());
        logEvent.PrintLog(", RocoverPage =");
        for (uint32_t &nPageID : vecRecoverPage) {
            logEvent.PrintLog(nullptr, nPageID, ",");
        }
    }

    return nRecoverCount;
}
/// @brief Get the key value
/// @param pKvPage
/// @param nKvIndex
/// @param pfun
/// @return
bool PManagerKvStore::_GetKvKeyData(PCachePagePtr const &pKvPage,
                                    int32_t const nKvIndex,
                                    PPageOptBase::CB_ReadData const &pfun) const noexcept
{
    PPageOptKvStore optPageKv{pKvPage};
    if (false == optPageKv.IsAccessReady()) {
        return false;
    }
    // The following code implementation does not consider the case where KeyData exceeds one page
    PBlockIndex *pBlockIndex{nullptr};
    PDataLocal *pLocalKey{nullptr};
    if (false == optPageKv.GetKvLocalData(EDataLogic::kLogicKey, nKvIndex, pBlockIndex, pLocalKey)) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // Local hit
    if (nullptr != pLocalKey) {
        std::ignore = pfun(static_cast< int32_t >(pLocalKey->cDataType), T_TransBytes(pLocalKey + 1),
                           static_cast< int32_t >(pLocalKey->cDataLen), kInvalidPageID, static_cast< uint16_t >(-1));
        _OnSuccess();
        return true;
    }
    // If the KV pair is not found locally, look in external pages for the data
    if ((nullptr == pBlockIndex) || (pBlockIndex->sBlockID <= 0U)) {
        // Error data: should not occur
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    if (false == PPageOptBase::IsValidPageID(pBlockIndex->nPageID)) {
        // Error data: should not occur
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // If the KV pair is not found locally, look in external pages for the data:
    int32_t const nReadTotal{managerValue_.GetBlockValue(*pBlockIndex, pfun)};
    if (nReadTotal <= 0) {
        PER_OnOptFailed(managerValue_.GetLastError());
        return false;
    }

    _OnSuccess();
    return true;
}
/// @brief Get the value value: without checking CRC/Hash redundancy data
/// @param pKvPage
/// @param nKvIndex
/// @param pfun
/// @return
bool PManagerKvStore::_GetKvValueData(PCachePagePtr const &pKvPage,
                                      int32_t const nKvIndex,
                                      PPageOptBase::CB_ReadData const &pfun) const noexcept
{
    PPageOptKvStore optPageKv{pKvPage};
    if (false == optPageKv.IsAccessReady()) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    if (false == optPageKv.CheckElementIndex(nKvIndex)) {
        // Normal logic, so PH_OnOptFailed is not added
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return false;
    }
    PBlockIndex *pBlockIndex{nullptr};
    PDataLocal *pLocalValue{nullptr};
    if (false == optPageKv.GetKvLocalData(EDataLogic::kLogicValue, nKvIndex, pBlockIndex, pLocalValue)) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // Local data
    if (nullptr != pLocalValue) {
        std::ignore = pfun(static_cast< int32_t >(pLocalValue->cDataType), T_TransBytes(pLocalValue + 1),
                           static_cast< int32_t >(pLocalValue->cDataLen), optPageKv.GetPageID(), kInt_0xFFFFU);
        _OnSuccess();
        return true;
    }

    if ((nullptr == pBlockIndex) || (pBlockIndex->sBlockID <= 0U)) {
        // Error data: should not occur
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    if (false == PPageOptBase::IsValidPageID(pBlockIndex->nPageID)) {
        // Error data: should not occur
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // If the KV pair is not found locally, look in external pages for the data:
    int32_t const nReadTotal{managerValue_.GetBlockValue(*pBlockIndex, pfun)};
    if (nReadTotal <= 0) {
        PER_OnOptFailed(managerValue_.GetLastError());
        return false;
    }

    _OnSuccess();
    return true;
}
/// @brief Verify the redundancy information of a specific KV pair
/// @param pKvPage
/// @param nKvIndex
/// @return Whether verification passed
bool PManagerKvStore::_CheckReddData(PCachePagePtr const &pKvPage, int32_t const nKvIndex) const noexcept
{
    if (false == kvConfigMuster_.IsReddCheckType()) {
        return true;
    }
    PCalculateReddData calculateReddData{kvConfigMuster_.GetReddType(), pReddAlgorithm_.get()};
    bool bCalculate{false};
    bCalculate = _GetKvKeyData(pKvPage, nKvIndex,
                               [&calculateReddData](int32_t const, uint8_t const *const pReadData,
                                                    int32_t const nReadLen, uint32_t, uint16_t) -> int32_t {
                                   std::ignore = calculateReddData.CalculateReddData(
                                       pReadData, static_cast< uint32_t >(nReadLen), true);
                                   return nReadLen;
                               });
    if (false == bCalculate) {
        _OnOptFailed(EErrorPHKV::kKvReddCheck);
        return false;
    }
    bCalculate = _GetKvValueData(pKvPage, nKvIndex,
                                 [&calculateReddData](int32_t const, uint8_t const *const pReadData,
                                                      int32_t const nReadLen, uint32_t, uint16_t) -> int32_t {
                                     std::ignore = calculateReddData.CalculateReddData(
                                         pReadData, static_cast< uint32_t >(nReadLen), false);
                                     return nReadLen;
                                 });
    if (false == bCalculate) {
        _OnOptFailed(EErrorPHKV::kKvReddCheck);
        return false;
    }
    ara::core::Vector< uint8_t > const vecReddData{calculateReddData.GetCalculateReddData()};
    if (calculateReddData.IsHaveError()) {
        _OnOptFailed(calculateReddData.GetLastError());
        return false;
    }
    // Compare the calculated CRC/Hash with the stored redundancy information
    bCalculate = _CompareReddData(pKvPage, nKvIndex, vecReddData);
    if (false == bCalculate) {
        _OnOptFailed(EErrorPHKV::kKvReddCheck);
        return false;
    }
    _OnSuccess();
    return bCalculate;
}
/// @brief Compare the Redd data saved in the library with the input Redd data
/// @param pKvPage KV cache page
/// @param nKvIndex KV pair number
/// @param [in] pBReddData Input Redd data
/// @param [in] nReddLen Length of input Redd data
/// @return Whether consistent
bool PManagerKvStore::_CompareReddData(PCachePagePtr const &pKvPage,
                                       int32_t const nKvIndex,
                                       ara::core::Vector< uint8_t > const &vecReddData) const noexcept
{
    ara::core::Vector< uint8_t > vecReddDataRead;
    if (false == _GetReddData(pKvPage, nKvIndex, vecReddDataRead)) {
        return false;
    }
    return vecReddData == vecReddDataRead;
}
/// @brief Get the redundancy information of a specific KV pair
/// @param pKvPage
/// @param nKvIndex
/// @param [out] vecReddData Output Redd data
/// @return Whether verification passed
bool PManagerKvStore::_GetReddData(PCachePagePtr const &pKvPage,
                                   int32_t const nKvIndex,
                                   ara::core::Vector< uint8_t > &vecReddData) const noexcept
{
    PPageOptKvStore optPageKv{pKvPage};
    if (false == optPageKv.IsAccessReady()) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    if (false == optPageKv.CheckElementIndex(nKvIndex)) {
        // Normal logic, so PH_OnOptFailed is not added
        _OnOptFailed(EErrorPHKV::kKvElementNotFind);
        return false;
    }
    PBlockIndex *pBlockIndex{nullptr};
    PDataLocal *pLocalData{nullptr};
    if (false == optPageKv.GetKvLocalData(EDataLogic::kLogicRedd, nKvIndex, pBlockIndex, pLocalData)) {
        PER_OnOptFailed(optPageKv.GetLastError());
        return false;
    }
    // Local hit
    if (nullptr != pLocalData) {
        uint8_t const *pReddData{T_TransBytes(pLocalData + 1)};
        for (uint8_t i = 0; i < pLocalData->cDataLen; i++) {
            vecReddData.push_back(*(pReddData + i));
        }
        _OnSuccess();
        return true;
    }
    // Not found in external pages either: this situation occurs due to Kv library or configuration upgrades causing old data to no longer exist
    if (nullptr == pBlockIndex) {
        PER_OnOptFailed(EErrorPHKV::kPageReadRedd);
        return false;
    }
    // If the KV pair is not found locally, look in external pages for the data
    int32_t const nReadTotal{
        managerValue_.GetBlockValue(*pBlockIndex,
                                    [&vecReddData](int32_t const, uint8_t const *const pReadData,
                                                   int32_t const nReadLen, uint32_t const, uint16_t const) -> int32_t {
                                        for (int32_t i = 0; i < nReadLen; i++) {
                                            vecReddData.push_back(*(pReadData + i));
                                        }
                                        return nReadLen;
                                    })};
    if (nReadTotal <= 0) {
        PER_OnOptFailed(managerValue_.GetLastError());
        return false;
    }

    _OnSuccess();
    return true;
}
//***************/
/// @brief Create a new Kv page
/// @param nCurPageID
/// @param nKvStoreID
/// @return
PCachePagePtr PManagerKvStore::_MakeNewSoloKvPage(uint32_t const nCurPageID, uint32_t const nKvStoreID) noexcept
{
    PCachePagePtr pPageKv{managerPage_.PreparePageSoloKv(nCurPageID, nKvStoreID)};
    if (nullptr == pPageKv) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Initialize the first page of the cross-page KV
    if (false == PPageOptBase::IsValidPageID(nKvStoreID)) {
        PPageOptKvStore optKvPage{pPageKv};
        uint32_t const nNewKvPageID{optKvPage.GetPageID()};
        optKvPage.SetKvStoreID(nNewKvPageID);
        optKvPage.SetLastKvPageID(nNewKvPageID);
    }
    pageOptHash_.AddKeyPageCount(1);
    _OnSuccess();
    return pPageKv;
}
/// @brief Create a new Kv page
/// @param pCacheKv
/// @return
PCachePagePtr PManagerKvStore::_MakeNewSoloKvPage(PCachePagePtr const &pCacheKv) noexcept
{
    PPageOptKvStore pageOptKv{pCacheKv};
    if (false == pageOptKv.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    return _MakeNewSoloKvPage(pageOptKv.GetPageID(), pageOptKv.GetKvStoreID());
}
/// @brief Preparations before writing Kv data
/// @param stKey
/// @param nFindKvIndex
/// @return
PCachePagePtr PManagerKvStore::_PrepareWriteKvPage(ara::core::StringView const &stKey, int32_t &nFindKvIndex) noexcept
{
    std::ignore = nFindKvIndex;
    // First check whether the page found by the Hash table is a cross-page KV
    uint32_t nFindPageID{kInvalidPageID};
    nFindKvIndex = -1;
    if (false == _FindKvElement(stKey, false, nFindPageID, nFindKvIndex)) {
        PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
        return PCachePagePtr{nullptr};
    }
    PCachePagePtr pWorkKvPage{nullptr};
    // If not found, need to create a new KV-Element page
    if (false == PPageOptBase::IsValidPageID(nFindPageID)) {
        pWorkKvPage = _MakeNewSoloKvPage(nFindPageID, kInvalidPageID);
    } else {
        pWorkKvPage = managerPage_.LoadCachePage(nFindPageID, true);
    }
    if (nullptr == pWorkKvPage) {
        PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
        return PCachePagePtr{nullptr};
    }
    // The Hash page also needs to be transferred to the Walog library
    if (false == managerPage_.MoveCachePageToWalog(pageOptHash_.GetCachePage())) {
        PER_OnOptFailed(EErrorPHKV::kKvElementWrite);
        return PCachePagePtr{nullptr};
    }
    return pWorkKvPage;
}
/// @brief Prepare to add a new KV page: may trigger Hash page expansion and independence
/// @param pCacheKv
/// @param stKey
/// @return
PCachePagePtr PManagerKvStore::_DivKvPage(PCachePagePtr const &pCacheKv, ara::core::StringView const &stKey) noexcept
{
    // If the exclusive page really cannot fit, add at least one KV page; this may trigger a KeyHash expansion event
    uint32_t const nKvPageCount{pageOptHash_.GetKvPageCount()};
    // If continuing to expand beyond the current number of Hash pages, trigger Hash expansion
    if (nKvPageCount >= pageOptHash_.GetHashCount()) {
        // Trigger KeyHash expansion event
        int32_t nExpansionCount{pageOptHash_.TryToExpansionHashCount(nKvPageCount + 1U)};
        // If there is an error during expansion, consider whether the space of the mixed page is insufficient
        if (nExpansionCount < 0) {  // Trigger Hash page independence event
            if (false == pageOptHash_.IsHashPageSolo()) {
                if (pageOptHash_.IsLackSpace()) {
                    if (nullptr != _SoloPageHash()) {
                        nExpansionCount = pageOptHash_.TryToExpansionHashCount(nKvPageCount + 1U);
                        _LogDebug("[KV_Manager]._DivKvPage", ": Need Solo HashPage", "nExpansionCount",
                                  nExpansionCount);
                    }
                }
            }
        }
    }
    PPageOptKvStore const pageOptKv{pCacheKv};
    // Get the PageID saved in the Hash table; cannot use the current PageID of pCacheKv
    uint32_t const nHashValuePageID{pageOptKv.GetKvStoreID()};
    // Determine whether the first page of the current KV page is a mixed page (PageID shared by multiple Hash values)
    if (pageOptHash_.GetMatchHashCount(nHashValuePageID) <= 1) {
        _LogDebug("[KV_Manager]._DivKvPage", ": Not Find Same HashValue", "nHashValuePageID",
                  static_cast< int32_t >(nHashValuePageID));
        return PCachePagePtr{nullptr};
    }
    // Split overlapping PageIDs in Hash values
    PCachePagePtr pPageDiv{nullptr};
    // Only KV data with consistent nKeyHashIndex will produce cross-page KV
    int32_t nHashCount{0};
    nHashCount = static_cast< int32_t >(pageOptHash_.GetHashCount());
    int32_t const nKeyHashIndex{pageOptHash_.GetHashIndex(stKey)};
    int32_t const nHashIndexFirst{pageOptHash_.FindFirstHashIndex(nHashValuePageID)};
    if (nHashCount > static_cast< int32_t >(pageOptHash_.GetHashInitCount())) {
        int32_t const nHashIndexFirstPair{pageOptHash_.GetHashIndexPair(nHashIndexFirst)};
        if ((nKeyHashIndex != nHashIndexFirst) && (nKeyHashIndex != nHashIndexFirstPair)) {
            // Here, split all KV pairs of a specific HashIndex from the mixed page
            pPageDiv = _DivKvPageByHashIndex(nHashValuePageID, nKeyHashIndex);
        } else {
            if (nHashValuePageID == pageOptHash_.GetHashValue(static_cast< uint64_t >(nHashIndexFirstPair))) {
                // Paginate by Hash
                pPageDiv = _DivKvPageByHashHalf(nHashValuePageID, nKeyHashIndex);
            }
        }
    } else  // Situation where the number of Kv pages does not exceed the initial Hash count, i.e., the Hash table has not expanded
    {
        int32_t nHashIndexMove{nKeyHashIndex};
        // If the page corresponding to stKey is a mixed page, find another HashIndex within the mixed page for pagination
        if (nKeyHashIndex == nHashIndexFirst) {
            nHashIndexMove
                = pageOptKv.StatMaxHashCountIndex(nKeyHashIndex, static_cast< uint32_t >(pageOptHash_.GetHashCount()));
        }
        // 2021-07-29 Pages appearing first in the Hash table are considered mixed pages (caused by expansion rules)
        if (nHashIndexMove > nHashIndexFirst) {
            // Because pagination has already been experienced with a single page, it can be determined that previous cross-page KvPages are KV pairs under a "specific" hash. / Conclusion: only paginate the current page, regardless of whether it is cross-page
            pPageDiv = _DivKvPageSimple(pCacheKv, nKeyHashIndex, nHashIndexMove);
        }
    }
    return pPageDiv;
}
/// @brief Change a KV page from a mixed page to an independent page, and return the new page
/// @param pKvPageOld
/// @return
PCachePagePtr PManagerKvStore::_SoloPageKv(PCachePagePtr const &pKvPageOld) noexcept
{
    PPageOptKvStore optKvOld{pKvPageOld};
    if (false == optKvOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    // If the old Kv page is not exclusive, transfer all old data to a new exclusive page: trigger Kv page independence event
    if (optKvOld.IsSoloPage()) {
        return pKvPageOld;
    }
    uint8_t const nMultPageType{static_cast< uint8_t >(EBlockType::kKvStore)
                                | static_cast< uint8_t >(EBlockType::kPageBoss)};
    // As long as it is not a mixed page of KV and Boss, the Boss page also needs to be made independent (for mixed pages of KV and Boss, when making the KV page independent, the remainder is the Boss.Solo page)
    if ((optKvOld.IsHavePageType(nMultPageType)) && (false == optKvOld.IsSoloPage(nMultPageType))) {
        if (nullptr == managerPage_.SoloPageBoss(optKvOld.GetPageID())) {
            PER_OnOptFailed(managerPage_.GetLastError());
            return PCachePagePtr{nullptr};
        }
    }
    // Only transfer KV data; KeyPageCount in the Hash page remains unchanged
    PCachePagePtr pNewKvPage{_MakeNewSoloKvPage(optKvOld.GetPageID(), kInvalidPageID)};
    PPageOptKvStore optKvNew{pNewKvPage};
    if (false == optKvNew.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    // Paginate, transfer old data
    if (false == _CopyKvElement(&optKvOld, &optKvNew)) {
        PER_OnOptFailed(optKvOld.GetLastError());
        PER_OnOptFailed(optKvNew.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Delete old data
    if (false == optKvOld.DelBlock(optKvOld.GetSoloBlockID())) {
        PER_OnOptFailed(optKvOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Update KeyHash table
    std::ignore = pageOptHash_.UpdateHashValue(optKvOld.GetPageID(), optKvNew.GetPageID());
    _LogInfo("[KV_Manager].SoloPage_KvStore", ": ", "OldPageID", static_cast< int32_t >(optKvOld.GetPageID()),
             "NewPageID", static_cast< int32_t >(optKvNew.GetPageID()));
    return pNewKvPage;
}
/// @brief Change a Hash page from a mixed page to an independent page, and return the new page
/// @return
PCachePagePtr PManagerKvStore::_SoloPageHash() noexcept
{
    // Move HashBlock to an exclusive page
    PCachePagePtr pOldHashPage{pageOptHash_.GetCachePage()};
    PCachePagePtr pNewHashPage{managerPage_.SoloPageHash(pOldHashPage)};
    if (nullptr == pNewHashPage) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return PCachePagePtr{nullptr};
    }
    // Also move the ExtString data area that saves the Version string to a new page
    if (nullptr == managerPage_.SoloPage_ExtString(pOldHashPage, pNewHashPage)) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageExt);
        return PCachePagePtr{nullptr};
    }
    // Bind the new Hash page
    std::ignore = pageOptHash_.InitCachePage(pNewHashPage);
    return pNewHashPage;
}
/// @brief From the page starting at nFirstPageID, split the KV associated pages into two parts based on the Hash value size, and return the page where stKey should operate
/// @param nFirstPageID
/// @param nKeyHashIndex
/// @return
PCachePagePtr PManagerKvStore::_DivKvPageByHashHalf(uint32_t const nFirstPageID, int32_t const nKeyHashIndex) noexcept
{
    int32_t const nHashCountHalf{static_cast< int32_t >(pageOptHash_.GetHashCountHalf())};
    PCachePagePtr pPageSaveLeft{nullptr};
    PCachePagePtr pPageSaveRight{nullptr};
    pPageSaveRight = _DivKvPageComplex("HashHalf", nFirstPageID, pPageSaveLeft,
                                       [this, nHashCountHalf](int32_t const, PBlockElementKv const *const pKvElement,
                                                              PElementKvData const *const) noexcept -> bool {
                                           return pageOptHash_.GetHashIndex(pKvElement->nKeyHashID) < nHashCountHalf;
                                       });
    PCacheAutoLock const cacheAutoLock{pPageSaveRight->GetMemoryPage(), nullptr};
    if (nullptr != pPageSaveRight) {
        // Update the Hash value of the latter half
        PPageOptKvStore const pageOptRight{pPageSaveRight};
        std::ignore = pageOptHash_.UpdateHashValue(
            nHashCountHalf,
            static_cast< int32_t >(pageOptHash_.GetHashCount()) - static_cast< int32_t >(nHashCountHalf), nFirstPageID,
            pageOptRight.GetKvStoreID());
    }
    // The return value is the page where stKey should operate
    if (nKeyHashIndex < nHashCountHalf) {
        return pPageSaveLeft;
    }
    return pPageSaveRight;
}
/// @brief Split all KV pairs of a specific HashIndex into a new page
/// @param nFirstPageID
/// @param nKeyHashIndex
/// @return
PCachePagePtr PManagerKvStore::_DivKvPageByHashIndex(uint32_t const nFirstPageID, int32_t const nKeyHashIndex) noexcept
{
    PCachePagePtr pPageSaveLeft{nullptr};
    PCachePagePtr pPageSaveRight{nullptr};
    pPageSaveRight = _DivKvPageComplex("HashIndex", nFirstPageID, pPageSaveLeft,
                                       [this, nKeyHashIndex](int32_t const, PBlockElementKv const *const pKvElement,
                                                             PElementKvData const *const) noexcept -> bool {
                                           return pageOptHash_.GetHashIndex(pKvElement->nKeyHashID) != nKeyHashIndex;
                                       });
    if (nullptr != pPageSaveRight) {
        // Update Hash table
        PPageOptKvStore const pageOptRight{pPageSaveRight};
        std::ignore
            = pageOptHash_.UpdateHashValueByIndex(static_cast< uint16_t >(nKeyHashIndex), pageOptRight.GetKvStoreID());
    }

    return pPageSaveRight;
}
/// @brief Split a single page into two pages, and return the page where stKey should operate
/// @param pCacheKv
/// @param nKeyHashIndex
/// @param nHashIndexMove
/// @return
PCachePagePtr PManagerKvStore::_DivKvPageSimple(PCachePagePtr const &pCacheKv,
                                                int32_t const nKeyHashIndex,
                                                int32_t const nHashIndexMove) noexcept
{
    // Without expanding the Hash, any non-first-page single-page expansion will trigger pagination of shared Hash pages
    PPageOptKvStore optKvOld{pCacheKv};
    if (false == optKvOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    // First copy data to the new page
    PCachePagePtr pCacheKvNew{_MakeNewSoloKvPage((*pCacheKv)->GetMainPageID(), kInvalidPageID)};
    PPageOptKvStore pageOptKvNew{pCacheKvNew};
    if (false == pageOptKvNew.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kPageNewPage);
        return PCachePagePtr{nullptr};
    }
    uint32_t const nHashCount{static_cast< uint32_t >(pageOptHash_.GetHashCount())};
    PPrintLog logEvent{ara::log::LogLevel::kInfo, GetLogKvName().data()};
    logEvent.PrintLog("[KV_Manager].DivKvPageSimple");
    logEvent.PrintLog(": OldPageID =", optKvOld.GetPageID());
    logEvent.PrintLog(", MoveKeyIndex =", nHashIndexMove);
    if (optKvOld.StatKvCountByHashIndex(nHashIndexMove, nHashCount) > 0) {
        int32_t nMoveCount{0};
        nMoveCount = optKvOld.ForEachKv(
            false,
            [&pageOptKvNew, nHashIndexMove, nHashCount](int32_t, PBlockElementKv const *const pKvElement,
                                                        PElementKvData const *const pKvData) -> int32_t {
                if (nullptr == pKvElement) {
                    return 0;
                }
                int32_t const nHashIndex{TransHashIndex(pKvElement->nKeyHashID, nHashCount)};
                if (nHashIndex != nHashIndexMove) {
                    return 0;
                }
                std::ignore = pageOptKvNew.AddKvElement(pKvElement, pKvData);
                return 1;
            });
        logEvent.PrintLog(", MoveCount =", nMoveCount);
        // Then delete the old data
        int32_t nDelCount{0};
        nDelCount = optKvOld.DelKvElement([nHashIndexMove, nHashCount](int32_t, PBlockElementKv const *const pKvElement,
                                                                       PElementKvData const *) noexcept -> bool {
            return TransHashIndex(pKvElement->nKeyHashID, nHashCount) == nHashIndexMove;
        });
        logEvent.PrintLog(", DelCount  =", nDelCount);
    }
    logEvent.PrintLog(", NewPageID =", pageOptKvNew.GetPageID());
    logEvent.PrintLog();
    std::ignore
        = pageOptHash_.UpdateHashValueByIndex(static_cast< uint16_t >(nHashIndexMove), pageOptKvNew.GetPageID());
    if (nKeyHashIndex == nHashIndexMove) {
        return pCacheKvNew;
    }
    return pCacheKv;
}
/// @brief Starting from the page at nFirstPageID, split the KV page into two parts according to the callback function (pPageSaveLeft + new KV page)
/// @param nFirstPageID
/// @param pchLogType
/// @param pPageSaveLeft
/// @param pfun
/// @return
PCachePagePtr PManagerKvStore::_DivKvPageComplex(char8_t const *const pchLogType,
                                                 uint32_t const nFirstPageID,
                                                 PCachePagePtr &pPageSaveLeft,
                                                 PPageOptKvStore::CB_EachKvBool const &pfun) noexcept
{
    std::ignore = pPageSaveLeft;
    // Request two caches
    PCachePagePtr const pageTempLeft{
        MakeStackPagePtr(static_cast< uint32_t >(managerCache_.GetPageLen()), &managerCache_)};
    PCachePagePtr const pageTempRight{
        MakeStackPagePtr(static_cast< uint32_t >(managerCache_.GetPageLen()), &managerCache_)};

    _LogDebug("[KV_Manager].DivKvPageComplex", "| Begin : ", "FirstPageID", static_cast< int32_t >(nFirstPageID));
    pPageSaveLeft = nullptr;
    PCachePagePtr pPageSaveRight{nullptr};
    int32_t nDivPageCount{0};
    uint32_t nNextPageID{nFirstPageID};
    while (PPageOptBase::IsValidPageID(nNextPageID)) {
        PCachePagePtr pScanCache{nullptr};
        pScanCache = managerPage_.LoadCachePage(nNextPageID, true);
        PPageOptKvStore optKvScan{pScanCache};
        if (false == optKvScan.IsAccessReady()) {
            PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
            return PCachePagePtr{nullptr};
        }
        if ((nullptr == pPageSaveLeft) && (nNextPageID == nFirstPageID)) {
            pPageSaveLeft = pScanCache;
            (*pPageSaveLeft)->AttachOpt(nullptr);
        }
        // First save the next page PageID, because the content of pScanCache may be modified by subsequent operations
        nNextPageID = optKvScan.GetNextPageID();
        // Split the current page into two pages
        if (false == optKvScan.DivKvElement(pageTempLeft, pageTempRight, pfun)) {
            PER_OnOptFailed(EErrorPHKV::kPageSplitHash);
            return PCachePagePtr{nullptr};
        }
        // Save the smaller Hash value to the original page
        (*pPageSaveLeft)->DetachOpt(nullptr);
        pPageSaveLeft = _SaveSplitKvElementLeft(pageTempLeft, pPageSaveLeft);
        (*pPageSaveLeft)->AttachOpt(nullptr);
        // Save the larger Hash value to the new page
        if (nullptr != pPageSaveRight) {
            (*pPageSaveRight)->DetachOpt(nullptr);
        }
        pPageSaveRight = _SaveSplitKvElementRight(pageTempRight, pPageSaveRight, (*pScanCache)->GetMainPageID());
        if (nullptr != pPageSaveRight) {
            (*pPageSaveRight)->AttachOpt(nullptr);
        }

        nDivPageCount += 1;
        {
            uint32_t nMainPageID{0U};
            if (nullptr != pPageSaveRight) {
                if (nullptr != pPageSaveRight->GetMemoryPage()) {
                    nMainPageID = (*pPageSaveRight)->GetMainPageID();
                }
            }
            _LogDebug("[KV_Manager].DivKvPage: ", pchLogType, "DivPageCount", nDivPageCount, "LeftPageID ",
                      static_cast< int32_t >((*pPageSaveLeft)->GetMainPageID()), "RightPageID",
                      static_cast< int32_t >(nMainPageID));
        }
    }
    // Need to reclaim possible empty pages after pPageSaveLeft
    int32_t nRecoverCount{0};
    if (nullptr != pPageSaveLeft) {
        nRecoverCount = _RecoverAllNextKvPage(pPageSaveLeft);
        (*pPageSaveLeft)->DetachOpt(nullptr);
    }
    if (nullptr != pPageSaveRight) {
        (*pPageSaveRight)->DetachOpt(nullptr);
    }
    _LogDebug("[KV_Manager].DivKvPageComplex", "| End :", "DivCount", nDivPageCount, "RecoverCount", nRecoverCount);
    return pPageSaveRight;
}
/// @brief Save the KV-Element of the sub-page split from the KV page
/// @param pPageTemp
/// @param pPageSave
/// @return
PCachePagePtr PManagerKvStore::_SaveSplitKvElementLeft(PCachePagePtr const &pPageTemp,
                                                       PCachePagePtr const &pPageSave) noexcept
{
    PPageOptKvStore optKvTemp{pPageTemp};
    if (false == optKvTemp.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    if (optKvTemp.GetKvElementCount() <= 0) {
        return pPageSave;
    }
    PPageOptKvStore optKvSave{pPageSave};
    _LogDebug("[KV_Manager].SaveLeft", ":", "TotalCount", optKvTemp.GetKvElementCount());
    int32_t nHaveKvCount{optKvSave.GetKvElementCount()};
    isoftkv::PPageOptKvStore::CB_EachKvIntConst pfunWork;
    pfunWork = [this, &nHaveKvCount, &optKvSave](int32_t, PBlockElementKv const *const pKvElement,
                                                 PElementKvData const *const pKvData) -> int32_t {
        if (false == optKvSave.IsAccessReady()) {
            PER_OnOptFailed(optKvSave.GetLastError());
            return 0;
        }
        // Transfer to KV library
        bool bInsert{nullptr != optKvSave.AddKvElement(pKvElement, pKvData)};
        // Return upon successful transfer to the KV library
        if (bInsert) {
            return 1;
        }

        // Return directly for errors other than insufficient space
        if (false == optKvSave.IsLackSpace()) {
            PER_OnOptFailed(optKvSave.GetLastError());
            return 0;
        }

        // If it fails, continue loading the cross-page data of optKvSave
        uint32_t const nNextPageID{optKvSave.GetNextPageID()};
        // Theoretically, it's impossible for the current page to not have enough space and nNextPageID <= 0
        PER_Assert(PPageOptBase::IsValidPageID(nNextPageID));
        // Load the cross-page data of the left smaller Hash // The candidate storage capacity is necessarily smaller than the total cross-page capacity
        PCachePagePtr pNextPage{managerPage_.LoadCachePage(nNextPageID, true)};
        _LogDebug("    SaveLeft ", ":", "PageID", static_cast< int32_t >(optKvSave.GetPageID()), "SaveCount",
                  optKvSave.GetKvElementCount() - nHaveKvCount, "KvCount", optKvSave.GetKvElementCount(), "NewPageID",
                  static_cast< int32_t >((*pNextPage)->GetMainPageID()));
        nHaveKvCount = 0;
        // Theoretically, AttachCachePage cannot fail
        PER_Assert(optKvSave.InitCachePage(pNextPage));
        bInsert = nullptr != optKvSave.AddKvElement(pKvElement, pKvData);
        if (false == bInsert) {
            return 0;
        }
        return 1;
    };
    std::ignore = optKvTemp.ForEachKv(false, pfunWork);
    _LogDebug("    SaveLeft ", ":", "PageID", static_cast< int32_t >(optKvSave.GetPageID()), "SaveCount",
              optKvSave.GetKvElementCount() - nHaveKvCount, "KvCount", optKvSave.GetKvElementCount());
    return optKvSave.GetCachePage();
}
/// @brief Save the KV-Element of the sub-page split from the KV page
/// @param pPageTemp
/// @param pPageSave
/// @param nCurPageID
/// @return
PCachePagePtr PManagerKvStore::_SaveSplitKvElementRight(PCachePagePtr const &pPageTemp,
                                                        PCachePagePtr const &pPageSave,
                                                        uint32_t const nCurPageID) noexcept
{
    PPageOptKvStore optKvTemp{pPageTemp};
    if (false == optKvTemp.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    if (optKvTemp.GetKvElementCount() <= 0) {
        return pPageSave;
    }
    PCachePagePtr pPageSaveWork{pPageSave};
    // Prevent invalid data on first occurrence
    if (nullptr == pPageSaveWork) {
        pPageSaveWork = _MakeNewSoloKvPage(nCurPageID, kInvalidPageID);
        if (nullptr == pPageSaveWork) {
            _LogError("[KV_Manager].SaveRight", ":", "NewPageID", static_cast< int32_t >(nCurPageID));
            PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
            return PCachePagePtr{nullptr};
        }
    }
    PPageOptKvStore optKvSave{pPageSaveWork};
    if (false == optKvSave.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return PCachePagePtr{nullptr};
    }
    _LogDebug("[KV_Manager].SaveRight ", ":", "TotalCount", optKvTemp.GetKvElementCount());
    int32_t nHaveKvCount{optKvSave.GetKvElementCount()};
    isoftkv::PPageOptKvStore::CB_EachKvIntConst pfunWork;
    pfunWork = [this, &nHaveKvCount, &optKvSave](int32_t, PBlockElementKv const *const pKvElement,
                                                 PElementKvData const *const pKvData) -> int32_t {
        for (uint64_t i{0U}; i < kInt_0x100000000U; i++) {
            if (false == optKvSave.IsAccessReady()) {
                PER_OnOptFailed(optKvSave.GetLastError());
                return 0;
            }
            // Transfer to KV library
            bool const bInsert{nullptr != optKvSave.AddKvElement(pKvElement, pKvData)};
            // Return directly for errors other than insufficient space
            if (bInsert) {
                return 1;
            }
            if (false == optKvSave.IsLackSpace()) {
                PER_OnOptFailed(optKvSave.GetLastError());
                return 0;
            }
            // Create a new KV page
            PCachePagePtr pNewKvPage{_MakeNewSoloKvPage(optKvSave.GetPageID(), optKvSave.GetKvStoreID())};
            if (nullptr == pNewKvPage) {
                PER_OnOptFailed(EErrorPHKV::kPageNewPage);
                return 0;
            }
            _LinkKvPage(optKvSave.GetCachePage(), pNewKvPage);
            _LogDebug("    SaveRight", ":", "PageID", static_cast< int32_t >(optKvSave.GetPageID()), "SaveCount",
                      optKvSave.GetKvElementCount() - nHaveKvCount, "KvCount", optKvSave.GetKvElementCount(),
                      "NewPageID", static_cast< int32_t >((*pNewKvPage)->GetMainPageID()));
            nHaveKvCount = 0;
            // Theoretically, AttachCachePage cannot fail
            PER_Assert(optKvSave.InitCachePage(pNewKvPage));
        }
        return 0;
    };
    std::ignore = optKvTemp.ForEachKv(false, pfunWork);
    _LogDebug("    SaveRight", ":", "PageID", static_cast< int32_t >(optKvSave.GetPageID()), "SaveCount",
              static_cast< int32_t >(optKvSave.GetKvElementCount()) - nHaveKvCount, "KvCount",
              static_cast< int32_t >(optKvSave.GetKvElementCount()));
    return optKvSave.GetCachePage();
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
