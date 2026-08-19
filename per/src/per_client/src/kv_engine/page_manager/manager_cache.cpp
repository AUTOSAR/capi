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
/// @file       manager_cache.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Page cache manager used by Puhua KvStorage
/// @date       2021-05-10
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Page Cache Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManagerCache
/// @unit_description=Page Cache Management Class
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-10 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/manager_cache.h"

#include <memory>

#include "ara/per/internal/common/isoft_check_redd.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/isoftkv/config_muster.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Load redundant page according to M/N rules
class PManagerCache::PLoadReddPage final
{
private:
    /// @brief
    PManagerCache *pManagerCache_;

public:
    /// @brief Default constructor: deleted
    PLoadReddPage() = delete;
    /// @brief Default constructor: deleted
    ~PLoadReddPage() = default;
    /// @brief
    /// @param a
    PLoadReddPage(PLoadReddPage const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PLoadReddPage &operator=(PLoadReddPage const &a) = delete;
    /// @brief
    /// @param a
    PLoadReddPage(PLoadReddPage &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PLoadReddPage &operator=(PLoadReddPage &&a) = delete;
    /// @brief
    /// @param pManagerCache
    explicit PLoadReddPage(PManagerCache *const pManagerCache) noexcept : pManagerCache_{pManagerCache} {}
    /// @brief Return the index of the correct library
    /// @param pMainPage
    /// @param nPageID
    /// @param nReddCountN
    /// @param nReddCountM
    /// @return
    int32_t LoadReddPage(PMemoryPage *const pMainPage,
                         uint32_t const nPageID,
                         uint32_t const nReddCountN,
                         uint32_t const nReddCountM) noexcept;
    /// @brief Attempt to restore redundant files
    /// @param nReddCountN
    /// @param nReddCountM
    /// @param nPageLen
    /// @return
    bool RecoverRedd_ByFile(uint32_t const nReddCountN,
                            uint32_t const nReddCountM,
                            uint32_t const nPageLen,
                            ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept;
    /// @brief Restore a specific page of a redundant file
    /// @param pFindMemPage Page with correct data
    /// @param nReddCountN Total number of redundancies N
    /// @param nReddCountM Valid number of redundancies M
    /// @return Number of the redundant copy that was executed
    ara::core::Vector< uint8_t > RecoverRedd_ByPage(PMemoryPage *pFindMemPage,
                                                    uint32_t const nReddCountN,
                                                    uint32_t const nReddCountM) noexcept;
    /// @brief Restore a specific page of a redundant file
    /// @param nPageID Page number to restore
    /// @param nReddCountN Total number of redundancies N
    /// @param nReddCountM Valid number of redundancies M
    /// @return Number of the redundant copy that was executed
    ara::core::Vector< uint8_t > RecoverRedd_ByPage(uint32_t const nPageID,
                                                    uint32_t const nReddCountN,
                                                    uint32_t const nReddCountM) noexcept;
    /// @brief Restore a specific page of a redundant file
    /// @param nPageID Page number to restore
    /// @param nSelectIndex Redundancy number of the correct data
    /// @param vecRecoverIndex Redundant page numbers of the erroneous data
    /// @return Success or failure (complete recovery counts as success)
    bool RecoverRedd_ByPage(uint32_t const nPageID,
                            uint8_t const nSelectIndex,
                            ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept;

protected:
    /// @brief Use the file corresponding to nRightReddIndex to restore the file corresponding to nErrorReddIndex
    /// @param nRightReddIndex
    /// @param nErrorReddIndex
    /// @return
    bool _RecorverFile(int32_t const nRightReddIndex, int32_t const nErrorReddIndex) noexcept;
    /// @brief Calculate the Hash value of a specific page of a redundant file
    /// @param nReddIndex
    /// @param nPageLen
    /// @param nPageID
    /// @return
    uint32_t _CalReddPageHashID(int32_t const nReddIndex, uint32_t const nPageLen, uint32_t const nPageID) noexcept;
};
//********************************/
/// @brief Return the index of the correct library
/// @param pMainPage
/// @param nPageID
/// @param nReddCountN
/// @param nReddCountM
/// @return
int32_t PManagerCache::PLoadReddPage::LoadReddPage(PMemoryPage *const pMainPage,
                                                   uint32_t const nPageID,
                                                   uint32_t const nReddCountN,
                                                   uint32_t const nReddCountM) noexcept
{
    PER_Assert(nullptr != pMainPage);
    if (nReddCountN <= 0U) {
        return -1;
    }
    if (nReddCountN < nReddCountM) {
        return -1;
    }

    // If there is an opportunity for recovery, try to restore as much as possible (need to find the most correct page's corresponding database), and return the found correct page
    uint32_t const nPageLen{pMainPage->GetCacheLen()};
    PER_Assert(nPageLen > 0U);
    ara::core::Map< int32_t, uint32_t > mapHashID;
    PSelectMofN const selectMofN{nReddCountM, nReddCountN};
    int32_t nFindReddIndex{0};
    nFindReddIndex = selectMofN.SelectGoal(
        0U, [this, nPageID, nPageLen, &mapHashID](int32_t const nReddIndex) noexcept -> uint64_t {
            uint32_t const nHashID{_CalReddPageHashID(nReddIndex, nPageLen, nPageID)};
            mapHashID[nReddIndex] = nHashID;
            return static_cast< uint64_t >(nHashID);
        });
    if (nFindReddIndex == -1) {
        pManagerCache_->AddReddCheck(nPageID, nFindReddIndex, ara::core::Vector< uint8_t >{});
        return -1;
    }
    ara::core::Vector< uint8_t > vecNeedRecover;
    for (auto &it : mapHashID) {
        if (mapHashID[nFindReddIndex] != it.second) {
            vecNeedRecover.push_back(it.first);
        }
    }
    if (false == vecNeedRecover.empty()) {
        pManagerCache_->AddReddCheck(nPageID, nFindReddIndex, vecNeedRecover);
    }
    // All backup library data is correct: directly read the found correct page
    if (false == pManagerCache_->_ReadFilePage(pMainPage, nPageID, ECacheSource::kMain, nFindReddIndex)) {
        return -1;
    }
    return nFindReddIndex;
}
/// @brief Attempt to restore redundant files
/// @param nReddCountN
/// @param nReddCountM
/// @param nPageLen
/// @return
bool PManagerCache::PLoadReddPage::RecoverRedd_ByFile(uint32_t const nReddCountN,
                                                      uint32_t const nReddCountM,
                                                      uint32_t const nPageLen,
                                                      ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept
{
    // If there is an opportunity for recovery, try to restore as much as possible (need to find the most correct page's corresponding database), and return the found correct page
    vecRecoverRedd.clear();
    if ((0U == nReddCountN) && (0U == nReddCountM)) {
        return true;
    }
    PER_Assert(nPageLen > 0U);
    ara::core::Vector< uint32_t > vecReddFileHash;  // Hash codes of the corresponding pages in the redundant library
    CB_DealInt32 pfun;
    pfun = [this, &vecReddFileHash, nPageLen](int32_t const nReddIndex) -> uint64_t {
        PFileOpt_Page *pKvFileOpt{nullptr};
        pKvFileOpt
            = pManagerCache_->_PrepareFileOpt(static_cast< uint16_t >(nPageLen), ECacheSource::kMain, nReddIndex);
        if (nullptr == pKvFileOpt) {
            return static_cast< uint64_t >(0U);
        }
        // Calculate the hash of the entire file
        uint32_t const nFileHashID{PSelectMofN::CalFileHashID(pKvFileOpt, nPageLen)};
        vecReddFileHash.push_back(nFileHashID);
        return static_cast< uint64_t >(nFileHashID);
    };
    PSelectMofN const selectMofN{nReddCountM, nReddCountN};
    int32_t const nFindReddIndex{selectMofN.SelectGoal(0U, std::move(pfun))};
    if ((nFindReddIndex >= static_cast< int32_t >(vecReddFileHash.size())) || (nFindReddIndex < 0)) {
        return false;
    }
    uint32_t const nFindReddHash{vecReddFileHash[static_cast< std::size_t >(nFindReddIndex)]};
    // If a recovery source is found, attempt to restore all erroneous databases
    for (std::size_t i{0U}; i < vecReddFileHash.size(); i++) {
        if (vecReddFileHash[i] != nFindReddHash) {
            if (_RecorverFile(nFindReddIndex, static_cast< int32_t >(i))) {
                vecRecoverRedd.push_back(static_cast< uint8_t >(i));
            }
        }
    }
    return true;
}
/// @brief Restore a specific page of a redundant file
/// @param pFindMemPage Page with correct data
/// @param nReddCountN Total number of redundancies N
/// @param nReddCountM Valid number of redundancies M
/// @return Number of the redundant copy that was executed
ara::core::Vector< uint8_t > PManagerCache::PLoadReddPage::RecoverRedd_ByPage(PMemoryPage *pFindMemPage,
                                                                              uint32_t const nReddCountN,
                                                                              uint32_t const nReddCountM) noexcept
{
    ara::core::Vector< uint8_t > vecReddIndex;
    if (nReddCountN <= 1U) {
        return vecReddIndex;
    }
    if (nReddCountN < nReddCountM) {
        return vecReddIndex;
    }
    if (nullptr == pFindMemPage) {
        return vecReddIndex;
    }
    uint16_t const nPageLen{pManagerCache_->GetPageLen()};
    uint32_t const nPageID{pFindMemPage->GetMainPageID()};
    uint32_t const nFindPageHashID{CalculateCrc32(pFindMemPage->GetCache(), pFindMemPage->GetCacheLen(), 0U)};
    for (int32_t i = 0; i < static_cast< int32_t >(nReddCountN); i++) {
        uint32_t const nPageHashID{_CalReddPageHashID(i, nPageLen, nPageID)};
        if ((i != pFindMemPage->GetReddIndex()) && (nPageHashID != nFindPageHashID)) {
            bool const bSaveResult{pManagerCache_->_SaveFilePage(pFindMemPage, nPageID, ECacheSource::kMain, i)};
            pManagerCache_->_LogInfo("[PH_System].RecoverRedd_ByPage", ": ", "nPageID", static_cast< int32_t >(nPageID),
                                     "nReddIndex", i, "Result", static_cast< int32_t >(bSaveResult));
            vecReddIndex.push_back(i);
        }
    }
    return vecReddIndex;
}
/// @brief Restore a specific page of a redundant file
/// @param nPageID Page number to restore
/// @param nReddCountN Total number of redundancies N
/// @param nReddCountM Valid number of redundancies M
/// @return Number of the redundant copy that was executed
ara::core::Vector< uint8_t > PManagerCache::PLoadReddPage::RecoverRedd_ByPage(uint32_t const nPageID,
                                                                              uint32_t const nReddCountN,
                                                                              uint32_t const nReddCountM) noexcept
{
    ara::core::Vector< uint8_t > vecReddIndex;
    if (nReddCountN <= 1U) {
        return vecReddIndex;
    }
    if (nReddCountN < nReddCountM) {
        return vecReddIndex;
    }
    uint16_t const nPageLen{pManagerCache_->GetPageLen()};
    // If not in memory, use M/N to elect the correct page
    PER_Assert(nPageLen > 0U);
    ara::core::Map< int32_t, uint32_t > mapPageHashID;
    PSelectMofN const selectMofN{nReddCountM, nReddCountN};
    int32_t nFindReddIndex{0};
    nFindReddIndex = selectMofN.SelectGoal(
        0U, [this, nPageID, nPageLen, &mapPageHashID](int32_t const nReddIndex) noexcept -> uint64_t {
            uint32_t const nPageHashID{_CalReddPageHashID(nReddIndex, nPageLen, nPageID)};
            mapPageHashID[nReddIndex] = nPageHashID;
            return static_cast< uint64_t >(nPageHashID);
        });
    if (nFindReddIndex == -1) {
        return vecReddIndex;
    }
    PMemoryPage memFindPage(0U, nPageLen);
    if (false == pManagerCache_->_ReadFilePage(&memFindPage, nPageID, ECacheSource::kMain, nFindReddIndex)) {
        return vecReddIndex;
    }
    for (int32_t i = 0; i < static_cast< int32_t >(nReddCountN); i++) {
        if ((i != nFindReddIndex) && (mapPageHashID[i] != mapPageHashID[nFindReddIndex])) {
            bool const bSaveResult{pManagerCache_->_SaveFilePage(&memFindPage, nPageID, ECacheSource::kMain, i)};
            pManagerCache_->_LogInfo("[PH_System].RecoverRedd_ByPage", ": ", "nPageID", static_cast< int32_t >(nPageID),
                                     "nReddIndex", i, "Result", static_cast< int32_t >(bSaveResult));
            vecReddIndex.push_back(i);
        }
    }
    return vecReddIndex;
}
/// @brief Restore a specific page of a redundant file
/// @param nPageID Page number to restore
/// @param nSelectIndex Redundancy number of the correct data
/// @param vecRecoverIndex Redundant page numbers of the erroneous data
/// @return Success or failure (complete recovery counts as success)
bool PManagerCache::PLoadReddPage::RecoverRedd_ByPage(uint32_t const nPageID,
                                                      uint8_t const nSelectIndex,
                                                      ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept
{
    uint16_t const nPageLen{pManagerCache_->GetPageLen()};
    PMemoryPage memFindPage(0U, nPageLen);
    if (false == pManagerCache_->_ReadFilePage(&memFindPage, nPageID, ECacheSource::kMain, nSelectIndex)) {
        return false;
    }
    uint32_t nSuccessCount{0};
    for (auto const &i : vecRecoverIndex) {
        bool const bSaveResult{
            pManagerCache_->_SaveFilePage(&memFindPage, nPageID, ECacheSource::kMain, static_cast< int32_t >(i))};
        if (bSaveResult) {
            nSuccessCount += 1;
        }
        pManagerCache_->_LogInfo("[PH_System].RecoverRedd_ByPage", ": ", "nPageID", static_cast< int32_t >(nPageID),
                                 "nReddIndex", static_cast< int32_t >(i), "Result",
                                 static_cast< int32_t >(bSaveResult));
    }
    if (nSuccessCount < static_cast< uint32_t >(vecRecoverIndex.size())) {
        return false;
    }
    return true;
}
//***************/
/// @brief
/// @param nReddIndex
/// @param nPageLen
/// @param nPageID
/// @return
uint32_t PManagerCache::PLoadReddPage::_CalReddPageHashID(int32_t const nReddIndex,
                                                          uint32_t const nPageLen,
                                                          uint32_t const nPageID) noexcept
{
    PMemoryPage pageRedd{0U, nPageLen};
    if (false == pManagerCache_->_ReadFilePage(&pageRedd, nPageID, ECacheSource::kMain, nReddIndex)) {
        return 0U;
    }

    return CalculateCrc32(pageRedd.GetCache(), pageRedd.GetCacheLen(), 0U);
}
/// @brief Use the file corresponding to nRightReddIndex to restore the file corresponding to nErrorReddIndex
/// @param nRightReddIndex
/// @param nErrorReddIndex
/// @return
bool PManagerCache::PLoadReddPage::_RecorverFile(int32_t const nRightReddIndex, int32_t const nErrorReddIndex) noexcept
{
    if (nRightReddIndex == nErrorReddIndex) {
        return true;
    }
    PER_Assert(nullptr != pManagerCache_);
    ara::core::String stFileDst;
    stFileDst = pManagerCache_->GetFileName(ECacheSource::kMain, nErrorReddIndex);

    PFileOpt_Page *pWorkFileOpt{nullptr};
    pWorkFileOpt = pManagerCache_->_PrepareFileOpt(pManagerCache_->GetPageLen(), ECacheSource::kMain, nRightReddIndex);
    if (nullptr == pWorkFileOpt) {
        return false;
    }
    PFileOpt_Page *pFileOptError{nullptr};
    pFileOptError = pManagerCache_->_PrepareFileOpt(pManagerCache_->GetPageLen(), ECacheSource::kMain, nErrorReddIndex);
    if (nullptr != pFileOptError) {
        std::ignore = pFileOptError->CloseFile();
    }
    return pWorkFileOpt->CopyFile(T_StringView(stFileDst), static_cast< uint32_t >(pManagerCache_->GetPageLen()));
}
//********************************/
// Magic word of the PPageTailData structure: length cannot exceed 15 bytes
//********************************/
/// @brief Constructor
/// @param kvConfigMuster
PManagerCache::PManagerCache(PConfigMuster_Kv const &kvConfigMuster) noexcept
    : kvConfigMuster_{kvConfigMuster}
    , nPageLen_{static_cast< uint16_t >(kvConfigMuster.GetPageInitLen())}
    , fileOptWalog_{static_cast< uint16_t >(kvConfigMuster.GetPageInitLen())}
{
    // Initialize redundant libraries
    _NewMainFileOpt();
    if (kvConfigMuster_.IsHaveCrypto_Storage(ECryptoKeySlotUsage::kHaveCrypto)) {
        PConfigData_Crypto const &configCrypto{kvConfigMuster_.GetCryptoConfig()};
        PER_Assert(fileOptWalog_.BuildCrypto(configCrypto.stKeySlotName, configCrypto.stCryptoAlgorithm));
    }
}
/// @brief Destructor
PManagerCache::~PManagerCache() noexcept
{
    _FreeMemoryPagePool();
    _DelMainFileOpt();
}
/// @brief Check if the cache system is ready
/// @return
bool PManagerCache::IsAccessReady() const noexcept
{
    if (vecCachePage_.empty()) {
        return false;
    }
    if (GetLogKvName().empty()) {
        return false;
    }
    return true;
}
/// @brief Get the Kv library name displayed in logs
/// @return
ara::core::StringView PManagerCache::GetLogKvName() const noexcept
{
    return std::move(T_StringView(kvConfigMuster_.GetStorageName()));
}
/// @brief Send a synchronization signal for finding a cache page
/// @return
bool PManagerCache::SignalCondFindCache() noexcept { return threadCondCache_.Signal(); }
/// @brief Initialize the in-memory cache pool
/// @param nMaxCacheCount
/// @param nPageLen
/// @return
bool PManagerCache::InitCachePool(uint32_t const nMaxCacheCount, uint16_t const nPageLen) noexcept
{
    // Prevent not creating the Main file
    PFileOpt_Page *pFileOptMain{nullptr};
    pFileOptMain = _PrepareFileOpt(nPageLen, ECacheSource::kMain, 0);
    if (nullptr == pFileOptMain) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    _InitMemoryPagePool(nMaxCacheCount, static_cast< uint32_t >(nPageLen));
    nPageLen_ = static_cast< uint16_t >(nPageLen);
    _OnSuccess();
    return true;
}
/// @brief Clear all cache pages
/// @return
bool PManagerCache::ClearCachePool() noexcept
{
    for (auto &pMemoryPage : vecCachePage_) {
        pMemoryPage->ResetCacheData();
    }
    return true;
}
/// @brief Get the related full file path according to parameter settings
/// @param stWorkPath
/// @param eCacheSource
/// @param nReddIndex
/// @return
ara::core::String PManagerCache::GetFileName(ara::core::StringView const &stWorkPath,
                                             ECacheSource const eCacheSource,
                                             int32_t const nReddIndex) const noexcept
{
    ara::core::String stFileName{PFileOpt::MakeFileName(stWorkPath, GetLogKvName())};
    switch (eCacheSource) {
        case ECacheSource::kDefault: {
        } break;
        case ECacheSource::kMain: {
            if (0 == nReddIndex) {
                stFileName += kFileExt_Main;
            } else {
                stFileName += kFileExt_Redd;
                stFileName += std::to_string(nReddIndex).c_str();
            }
        } break;
        case ECacheSource::kWalog: {
            stFileName += kFileExt_Walog;
        } break;
    }
    return stFileName;
}
/// @brief Get the related full file path according to parameter settings
/// @param eCacheSource
/// @param nReddIndex
/// @return
ara::core::String PManagerCache::GetFileName(ECacheSource const &eCacheSource, int32_t const nReddIndex) const noexcept
{
    return GetFileName(T_StringView(kvConfigMuster_.GetWorkPath()), eCacheSource, nReddIndex);
}
/// @brief Add a detected redundancy recovery request data
/// @param nPageID
/// @param nSelectIndex
/// @param vecNeedRecover
void PManagerCache::AddReddCheck(uint32_t const nPageID,
                                 uint8_t const nSelectIndex,
                                 ara::core::Vector< uint8_t > const &vecNeedRecover) noexcept
{
    checkReddKv_.AddReddCheck(PReddCheckData_Kv{nPageID, nSelectIndex, vecNeedRecover});
}
/// @brief Delete a pending redundancy recovery request
/// @param nPageID
void PManagerCache::DelReddCheck(uint32_t const nPageID) noexcept { checkReddKv_.DelReddCheck(nPageID); }
/// @brief Check if there is a redundancy recovery request
/// @return Redundancy data that needs recovery
PReddCheckData_Kv PManagerCache::FindRecoverData() const noexcept { return checkReddKv_.FindRecoverData(); }
//***************/
/// @brief Create a page of data
/// @param eCacheSource
/// @param nReddIndex
/// @return
PMemoryPage *PManagerCache::NewFilePage(ECacheSource const eCacheSource, int32_t const nReddIndex) noexcept
{
    PFileOpt_Page *pOptFile{nullptr};
    pOptFile = _PrepareFileOpt(nPageLen_, eCacheSource, nReddIndex);
    if (nullptr == pOptFile) {
        return nullptr;
    }
    PMemoryPage *pFindCache{nullptr};
    pFindCache = _FindFreeCacheByThread();
    if (nullptr == pFindCache) {
        return nullptr;
    }

    uint32_t const nPageID{pOptFile->NewPage(static_cast< int32_t >(GetPageLen()))};
    pFindCache->ResetCacheData();
    pFindCache->SetActiveOptTime(std::chrono::steady_clock::now());
    pFindCache->InitPageData(eCacheSource, nPageID);
    pFindCache->SetOptIndex(nOptIndex_);  // nOptIndex automatically returns if out of bounds
    pFindCache->SetReddIndex(nReddIndex);
    nOptIndex_ += 1U;
    return pFindCache;
}
/// @brief Load a page of data
/// @param eCacheSource
/// @param nPageID
/// @return
PMemoryPage *PManagerCache::LoadFilePage(ECacheSource const eCacheSource, uint32_t const nPageID) noexcept
{
    PMemoryPage *pFindCache{nullptr};
    pFindCache = _FindFreeCacheByThread();
    if (nullptr == pFindCache) {
        PER_OnOptFailed(EErrorPHKV::kPageFindFree);
        return nullptr;
    }
    // Logic: find a cache page (crash and report error if not found), read data into this cache, maintain relevant state, then return this cache structure
    pFindCache->ResetCacheData();
    PCacheAutoLock const autoLockPage{pFindCache, nullptr};  // Automatically bind a no-op to the cache page
    bool bReadResult{false};
    switch (eCacheSource) {
        case ECacheSource::kWalog: {
            bReadResult = _ReadFilePage(pFindCache, nPageID, eCacheSource, 0);
        } break;
        case ECacheSource::kMain: {
            uint32_t const nReddCount{kvConfigMuster_.GetReddCountN()};
            if (nReddCount > 1U) {  // Trigger M/N validation logic
                bReadResult = _ReadFilePageFromRedd(pFindCache, nPageID);
            } else {
                bReadResult = _ReadFilePage(pFindCache, nPageID, ECacheSource::kMain, 0);
            }
        } break;
        default: {
            bReadResult = false;
        } break;
    }
    if (false == bReadResult) {
        return nullptr;
    }

    return pFindCache;
}
/// @brief Find the page corresponding to PageID among all working pages
/// @param nMainPageID
/// @return
PMemoryPage *PManagerCache::FindCachePage(uint32_t const nMainPageID) noexcept
{
    PMemoryPage *pFindCache{nullptr};
    pFindCache  = nullptr;
    std::ignore = _ForEachCachePage([&pFindCache, nMainPageID](PMemoryPage *const pCachePage) noexcept -> int32_t {
        if (nullptr == pCachePage) {
            return 0;
        }
        if (false == pCachePage->IsCacheValid()) {
            return 0;
        }
        if (pCachePage->GetMainPageID() != nMainPageID) {
            return 0;
        }
        pFindCache = pCachePage;
        return -1;
    });
    if (nullptr != pFindCache) {
        pFindCache->SetActiveOptTime(std::chrono::steady_clock::now());
    }
    return pFindCache;
}
/// @brief Read the information at the beginning of the file
/// @return
PPageHeadData PManagerCache::ReadFileHead() noexcept
{
    PPageHeadData pageHeadData;
    if (GetExistMainCount() < kvConfigMuster_.GetReddCountM()) {
        return pageHeadData;
    }
    PMemoryPage memoryPage{0U, PFileOpt::GetNaiSectorSize()};
    PLoadReddPage loadReddPage{this};
    // Need to use M/N redundancy to read the file header
    if (-1
        == loadReddPage.LoadReddPage(&memoryPage, 1U, kvConfigMuster_.GetReddCountN(),
                                     kvConfigMuster_.GetReddCountM())) {
        _OnOptFailed(EErrorPHKV::kPageReadRedd);
        return pageHeadData;
    }
    PPageHeadData const *const pHeadData{T_TransPtr< PPageHeadData >(memoryPage.GetCache())};
    if (pHeadData->sPageLen <= 0U) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPage);
        return pageHeadData;
    }
    pageHeadData = *pHeadData;
    _OnSuccess();
    return pageHeadData;
}
/// @brief Attempt to restore redundant files
/// @return
bool PManagerCache::RecoverReddFile(ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept
{
    if (false == kvConfigMuster_.IsReddMofN()) {
        return false;
    }
    PLoadReddPage loadReddPage{this};
    bool const bResult{loadReddPage.RecoverRedd_ByFile(kvConfigMuster_.GetReddCountN(), kvConfigMuster_.GetReddCountM(),
                                                       static_cast< uint32_t >(GetPageLen()), vecRecoverRedd)};
    if (false == bResult) {
        _OnOptFailed(EErrorPHKV::kPageReadRedd);
    }
    return bResult;
}
/// @brief Reset all files: Main+Walog+Redd
/// @return
bool PManagerCache::DbFileResize() noexcept
{
    bool bSuccess{false};
    bSuccess = _ForEachFileOpt(true, [](PFileOpt_Page *const pFileWork) noexcept -> bool {
        if (nullptr == pFileWork) {
            return false;
        }
        return pFileWork->ResizeFile(0);
    });
    if (false == bSuccess) {
        PER_OnOptFailed(EErrorPHKV::kFileScanAllOpt);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Close files
/// @return
bool PManagerCache::DbFileClose() noexcept
{
    bool bSuccess{false};
    bSuccess = _ForEachFileOpt(false, [](PFileOpt_Page *const pFileWork) noexcept -> bool {
        if (nullptr == pFileWork) {
            return false;
        }
        return pFileWork->CloseFile();
    });
    if (false == bSuccess) {
        PER_OnOptFailed(EErrorPHKV::kFileScanAllOpt);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Delete files
/// @return
bool PManagerCache::DbFileDelete() noexcept
{
    bool bSuccess{false};
    bSuccess = _ForEachFileOpt(false, [](PFileOpt_Page *const pFileWork) noexcept -> bool {
        if (nullptr == pFileWork) {
            return false;
        }
        return pFileWork->DelFile();
    });
    if (false == bSuccess) {
        PER_OnOptFailed(EErrorPHKV::kFileScanAllOpt);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Get the total disk space occupied by the entire KV library: includes Main, Walog, Redd, etc.
/// @return
uint64_t PManagerCache::GetSpaceSize() noexcept
{
    uint64_t nTotalSize{0U};
    bool bSuccess{false};
    bSuccess = _ForEachFileOpt(true, [&nTotalSize](PFileOpt_Page *const pFileWork) noexcept -> bool {
        if (nullptr == pFileWork) {
            return false;
        }
        nTotalSize += static_cast< uint64_t >(pFileWork->GetFileSize());
        return true;
    });
    if (false == bSuccess) {
        return 0U;
    }
    return nTotalSize;
}

/// @brief Get the size of the Main file, regardless of whether the KV library is valid (in case of redundant backup)
/// @return Returns the size of the main file
/// @code{.isoft}
/// @needwork = dda
/// @endcode
int64_t PManagerCache::GetMainFileSize() noexcept
{
    PFileOpt_Page *pFileWork{nullptr};
    pFileWork = _FindFileOpt(ECacheSource::kMain, 0);
    if (pFileWork == nullptr) {
        return -1;
    }
    return pFileWork->GetFileSize();
}
/// @brief Get the number of pages in the main library
/// @return
uint32_t PManagerCache::GetMainPageTotal() const noexcept
{
    ara::core::Vector< uint32_t > vecPageTotal;  // Hash codes of the corresponding pages in the redundant library
    PSelectMofN const selectMofN{kvConfigMuster_.GetReddCountM(), kvConfigMuster_.GetReddCountN()};
    int32_t nSelect{0};
    nSelect = selectMofN.SelectGoal(0U, [this, &vecPageTotal](int32_t const nReddIndex) -> uint64_t {
        PFileOpt_Page *pKvFileOpt{nullptr};
        pKvFileOpt = _PrepareFileOpt(GetPageLen(), ECacheSource::kMain, nReddIndex);
        uint32_t nPageCount{0U};
        if (nullptr != pKvFileOpt) {
            nPageCount = pKvFileOpt->GetPageTotal();
        }
        vecPageTotal.push_back(nPageCount);
        return static_cast< uint64_t >(nPageCount);
    });
    if ((nSelect >= static_cast< int32_t >(vecPageTotal.size())) || (nSelect < 0)) {
        return 0U;
    }
    return vecPageTotal[static_cast< std::size_t >(nSelect)];
}
/// @brief Get the number of pages in the Walog library
/// @return
uint32_t PManagerCache::GetWalogPageTotal() const noexcept
{
    PFileOpt_Page *const pFileWork{_PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0)};
    if (nullptr == pFileWork) {
        return 0U;
    }
    int64_t const nFileSize{pFileWork->GetFileSize()};
    int32_t nPageTotal = static_cast< int32_t >(nFileSize / nPageLen_);

    if (PFileOpt_Page::GetNaiSectorSize() == nPageLen_) {
        PMemoryPage const pageTail{0U, PFileOpt_Page::GetNaiSectorSize()};
        std::ignore = pFileWork->SeekPos(EFileSeekType::kSeekSet, pFileWork->GetLastSectorStartPos());
        int32_t const nReadLen{pFileWork->ReadRawData(pageTail.GetCache(), pageTail.GetCacheLen())};
        if (nReadLen != static_cast< int32_t >(pageTail.GetCacheLen())) {
            PER_OnOptFailed(EErrorPHKV::kFileKvRead);
            return 0;
        }
        // By default, the storage area of the last sector size of the file is the walog tail data, not counted as a data page, so subtract 1
        PPageTailData *const pPageTailData{T_TransPtr< PPageTailData >(pageTail.GetCache())};
        if (pPageTailData->chMagicWord == ara::core::StringView{"ph_kv_tail_flag"}) {
            --nPageTotal;
        }
    }
    return static_cast< uint32_t >(nPageTotal);
}
/// @brief Get the number of cache pages that need to be written to disk
/// @return
int32_t PManagerCache::GetNeedWriteCachePageCount() const noexcept
{
    int32_t nSaveTotal{0};
    for (auto const &pMemoryPage : vecCachePage_) {
        if (nullptr == pMemoryPage) {
            continue;
        }
        if (false == PPageOptBase::IsValidPageID(pMemoryPage->GetWalogPageID())) {
            continue;
        }
        if (false == pMemoryPage->IsWriteRef()) {
            continue;
        }
        nSaveTotal += 1;
    }
    return nSaveTotal;
}
//***************/
/// @brief Initialize the Walog library file
/// @return
bool PManagerCache::WalogInitFile() noexcept
{
    PFileOpt_Page *const pFileWork{_PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0)};
    if (nullptr == pFileWork) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    std::ignore = pFileWork->ResizeFile(0);
    _OnSuccess();
    return true;
}
/// @brief Read the end marker of the Walog library
/// @param tailData
/// @return
bool PManagerCache::WalogReadTail(PPageTailData &tailData) noexcept
{
    std::ignore = tailData;
    ara::core::String stWalogFile;
    stWalogFile = GetFileName(T_StringView(kvConfigMuster_.GetWorkPath()), ECacheSource::kWalog, 0);
    if (false == PFileOpt::IsFileExist(T_StringView(stWalogFile))) {
        return false;
    }
    PFileOpt_Page *const pFileOpt{_PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0)};
    if (nullptr == pFileOpt) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    if (pFileOpt->GetFileSize() <= 0) {
        _OnSuccess();
        return true;
    }
    // 2021-07-15 Use NAI_O_DIRECT flag to ensure the data cache size for each operation is aligned with nai_sector_size
    uint32_t const nTailPageLen{PFileOpt::GetNaiSectorSize()};
    int64_t const nFileSize{pFileOpt->GetFileSize()};
    int64_t nNewSize{pFileOpt->GetFileSizeByPage()};
    if (PFileOpt_Page::GetNaiSectorSize() != nPageLen_) {
        nNewSize += PFileOpt_Page::GetNaiSectorSize();
    }
    if (nFileSize != nNewSize) {
        _OnSuccess();
        return false;
    }
    PMemoryPage const pageTail{0U, nTailPageLen};
    std::ignore = pFileOpt->SeekPos(EFileSeekType::kSeekSet, pFileOpt->GetLastSectorStartPos());
    int32_t const nReadLen{pFileOpt->ReadRawData(pageTail.GetCache(), pageTail.GetCacheLen())};
    if (nReadLen != static_cast< int32_t >(pageTail.GetCacheLen())) {
        PER_OnOptFailed(EErrorPHKV::kFileKvRead);
        return false;
    }
    // Transcode data
    PPageTailData *const pPageTailData{T_TransPtr< PPageTailData >(pageTail.GetCache())};
    uint32_t const nPageTailCrc{pPageTailData->nTailCrc};
    pPageTailData->nTailCrc = 0U;
    pPageTailData->nTailCrc = CalculateCrc32(pPageTailData, sizeof(PPageTailData), 0U);
    if ((false == IsPhTail(*pPageTailData)) || (pPageTailData->nTailCrc != nPageTailCrc)) {
        _OnOptFailed(EErrorPHKV::kPageWalogTailData);
        return false;
    }
    tailData = *pPageTailData;
    _OnSuccess();
    return true;
}
/// @brief Set the end marker of the Walog library
/// @param tailData
/// @return
bool PManagerCache::WalogWriteTail(PPageTailData const &tailData) noexcept
{
    PFileOpt_Page *pFileOpt{nullptr};
    pFileOpt = _PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0);
    if (nullptr == pFileOpt) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    if (pFileOpt->GetFileSize() <= 0) {
        PER_OnOptFailed(EErrorPHKV::kFileSize);
        return false;
    }
    // 2021-07-15 HeadTail is set to the size of one OS page
    uint32_t const nTailPageLen{PFileOpt::GetNaiSectorSize()};
    PMemoryPage const pageTail{0U, nTailPageLen};

    // Verify the file length and move the operation cursor to the end of the "KV page"
    int64_t const nFileSize{pFileOpt->GetFileSize()};
    int64_t nNewSize{pFileOpt->GetLastSectorStartPos()};
    if (nNewSize == 0) {
        nNewSize = nFileSize + nTailPageLen;
    }

    if (nFileSize < nNewSize) {
        std::ignore = pFileOpt->ResizeFile(nNewSize);
    }
    PPageTailData *pPageTailData{nullptr};
    pPageTailData  = T_TransPtr< PPageTailData >(pageTail.GetCache());
    *pPageTailData = tailData;
    std::ignore    = pFileOpt->SeekPos(EFileSeekType::kSeekSet, pFileOpt->GetFileSizeByPage());
    int32_t const nWriteLen{pFileOpt->WriteRawData(pageTail.GetCache(), pageTail.GetCacheLen())};
    if (nWriteLen <= 0) {
        PER_OnOptFailed(EErrorPHKV::kFileKvWrite);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Change all Walog cache to Main cache
/// @return
bool PManagerCache::WalogCacheToMain() noexcept
{
    int32_t nScanCount{0};
    nScanCount = _ForEachCachePage([](PMemoryPage *const pCachePage) noexcept -> int32_t {
        if (nullptr == pCachePage) {
            return 0;
        }
        if (ECacheSource::kWalog != pCachePage->GetPageSource()) {
            return 0;
        }
        pCachePage->SetPageSource(ECacheSource::kMain);
        pCachePage->SetWalogPageID(kInvalidPageID);
        pCachePage->SetWriteRef(0);
        return 1;
    });
    PER_Assert(nScanCount >= 0);  // Pure nonsense, for debugging to see the value of nScanCount
    _OnSuccess();
    return true;
}
/// @brief Write all cache files to disk; unmodified Main pages will be discarded
/// @param bForce
/// @return Number of pages saved: -1 indicates error, 0 indicates no files need to be written to disk
int32_t PManagerCache::WalogSaveCachePage(bool const bForce) noexcept
{
    PAutoTimeRecord const autoTimeRecord{T_StringView("PManagerCache::WalogSaveCachePage")};
    int32_t nSaveTotal{0};
    int32_t nCount{0};
    for (auto &pMemoryPage : vecCachePage_) {
        if (nullptr == pMemoryPage) {
            continue;
        }
        if (false == PPageOptBase::IsValidPageID(pMemoryPage->GetWalogPageID())) {
            continue;
        }
        if (false == pMemoryPage->IsWriteRef()) {
            if ((ECacheSource::kMain == pMemoryPage->GetPageSource())
                || ((false == bForce) && (ECacheSource::kWalog == pMemoryPage->GetPageSource()))) {
                continue;
            }
        }
        nSaveTotal += 1;
        // Update the page's CRC32 before writing to disk
        if (_SaveWalogPage(pMemoryPage)) {
            nCount += 1;
        }
    }
    uint64_t const nTimeElapse{autoTimeRecord.GetTimeElapse_Micro()};  // Unit: microseconds
    if (nCount != nSaveTotal) {
        _LogError("[PH_System].WalogSaveCachePage", ": nSaveTotal != nSaveCount");
        _LogInfo("[PH_System].WalogSaveCachePage", ": ", "nSaveTotal", nSaveTotal, "nSaveCount", nCount, "CostTime",
                 static_cast< int32_t >(nTimeElapse));
        PER_OnOptFailed(EErrorPHKV::kFileKvWrite);
        return -1;
    }
    PER_Assert(nCount == nSaveTotal);
    _LogInfo("[PH_System].WalogSaveCachePage", ": ", "nSaveTotal", nSaveTotal, "nSaveCount", nCount, "CostTime",
             static_cast< int32_t >(nTimeElapse));
    return nSaveTotal;
}
/// @brief Reset all cache pages: old data will be lost
/// @return
bool PManagerCache::WalogResetCache() noexcept
{
    // Clear Walog pages directly, modify the corresponding WalogPageID for Main pages
    std::function< int32_t(PMemoryPage *const) > pfun;
    pfun = [](PMemoryPage *const pCachePage) noexcept -> int32_t {
        if (nullptr == pCachePage) {
            return 0;
        }
        int32_t nReturn{0};
        nReturn = 1;
        switch (pCachePage->GetPageSource()) {
            case ECacheSource::kWalog: {
                pCachePage->ResetCacheData();
            } break;
            case ECacheSource::kMain: {
                pCachePage->SetWalogPageID(kInvalidPageID);
            } break;
            default: {
                nReturn = 0;
            } break;
        }
        return nReturn;
    };
    std::ignore = _ForEachCachePage(std::move(pfun));
    return true;
}
/// @brief Get the mapping from WalogPageID to MainPageID from the Walog file
/// @param vecMainPageID
/// @return
bool PManagerCache::WalogReadAllMainPageID(ara::core::Vector< uint32_t > &vecMainPageID) noexcept
{
    PPageTailData tailData;
    if (false == WalogReadTail(tailData)) {
        return false;
    }
    if (tailData.nPageSaveCount <= 0) {
        return true;
    }
    PFileOpt_Page *pFileOpt{nullptr};
    pFileOpt = _PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0);
    if (nullptr == pFileOpt) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    PMemoryPage memPage{0U, static_cast< uint32_t >(nPageLen_)};
    for (int32_t i{0}; i < tailData.nPageSaveCount; i++) {
        uint32_t const nWalogPageID{static_cast< uint32_t >(i) + 1U};
        memPage.ResetCacheData();
        if (pFileOpt->ReadPage(nWalogPageID, memPage.GetCache(), memPage.GetCacheLen()) > 0) {
            PPageHeadData *const pHeadData{T_TransPtr< PPageHeadData >(memPage.GetCache())};
            vecMainPageID.push_back(pHeadData->nPageID);
        }
    }
    return true;
}
//***************/
/// @brief Get the existing Main library and the number of redundancies
/// @return
uint32_t PManagerCache::GetExistMainCount() const noexcept
{
    uint32_t nCount{0U};
    int32_t const nMainFileCount{static_cast< int32_t >(kvConfigMuster_.GetReddCountN())};
    for (int32_t i{0}; i < nMainFileCount; i++) {
        if (PFileOpt::IsFileExist(T_StringView(GetFileName(ECacheSource::kMain, i)))) {
            nCount += 1U;
        }
    }
    return nCount;
}
/// @brief Write a page of data to disk
/// @param pMemoryPage
/// @param nPageID
/// @return
int32_t PManagerCache::SaveMainPage(PMemoryPage const *const pMemoryPage, uint32_t const nPageID) noexcept
{
    if (nullptr == pMemoryPage) {
        return 0;
    }
    int32_t nReturn{0};
    nReturn = 0;
    int32_t const nMainFileCount{static_cast< int32_t >(kvConfigMuster_.GetReddCountN())};
    for (int32_t i{0}; i < nMainFileCount; i++) {
        if (_SaveFilePage(pMemoryPage, nPageID, ECacheSource::kMain, i)) {
            nReturn += 1;
        }
    }
    // Clear Redd cache after a write event occurs
    if (nMainFileCount > 1) {
        mapReddFindIndex_[nPageID] = -1;
    }
    return nReturn;
}
/// @brief Restore a specific page of a redundant file
/// @param nPageID Page number to restore
/// @param nReddCountN Total number of redundancies N
/// @param nReddCountM Valid number of redundancies M
/// @return Number of the redundant copy that was executed
ara::core::Vector< uint8_t > PManagerCache::RecoverReddPage(uint32_t const nPageID,
                                                            uint32_t const nReddCountN,
                                                            uint32_t const nReddCountM) noexcept
{
    PLoadReddPage loadReddPage(this);
    PMemoryPage *pFindMemPage{FindCachePage(nPageID)};
    if (nullptr != pFindMemPage) {
        return loadReddPage.RecoverRedd_ByPage(pFindMemPage, nReddCountN, nReddCountM);
    }
    return loadReddPage.RecoverRedd_ByPage(nPageID, nReddCountN, nReddCountM);
}
/// @param nPageID Page number to restore
/// @param nSelectIndex Redundancy number of the correct data
/// @param vecRecoverIndex Redundant page numbers of the erroneous data
/// @return Success or failure
bool PManagerCache::RecoverReddPage(uint32_t const nPageID,
                                    uint8_t const nSelectIndex,
                                    ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept
{
    PLoadReddPage loadReddPage(this);
    return loadReddPage.RecoverRedd_ByPage(nPageID, nSelectIndex, vecRecoverIndex);
}
//***************/
/// @brief Iterate over all caches
void PManagerCache::Debug_ScanCache() noexcept
{
    for (auto &pMemoryCache : vecCachePage_) {
        std::ignore = pMemoryCache->GetAttachCount();
    }
}
//********************************/
/// @brief Iterate over each cache page
/// @param pfun
/// @return
int32_t PManagerCache::_ForEachCachePage(std::function< int32_t(PMemoryPage *const) > const &&pfun) noexcept
{
    int32_t nReturn{0};
    nReturn = 0;
    for (auto &pCachePage : vecCachePage_) {
        if (nullptr == pCachePage) {
            continue;
        }
        int32_t const nCall{pfun(pCachePage)};
        if (nCall < 0) {
            break;
        }
        if (0 == nCall) {
            continue;
        }
        nReturn += 1;
    }
    return nReturn;
}
/// @brief Iterate over each file operation object
/// @param bEnableCreate
/// @param pfun
/// @return
bool PManagerCache::_ForEachFileOpt(bool const bEnableCreate,
                                    std::function< bool(PFileOpt_Page *) > const &pfun) noexcept
{
    int32_t nTotalCount{0};
    int32_t nSuccess{0};
    PFileOpt_Page *pFileWork{nullptr};
    // Main library: inevitably exists and counts as one redundancy copy; other redundant libraries may not exist
    uint32_t const nMainFileCount{kvConfigMuster_.GetReddCountN()};
    for (int32_t i{0}; i < static_cast< int32_t >(nMainFileCount); i++) {
        if (bEnableCreate) {
            pFileWork = _PrepareFileOpt(nPageLen_, ECacheSource::kMain, i);
        } else {
            pFileWork = _FindFileOpt(ECacheSource::kMain, i);
        }
        if (nullptr != pFileWork) {
            if (pfun(pFileWork)) {
                nSuccess += 1;
            }
        }
    }
    nTotalCount += static_cast< int32_t >(nMainFileCount);
    // Walog library
    if (bEnableCreate) {
        pFileWork = _PrepareFileOpt(nPageLen_, ECacheSource::kWalog, 0);
    } else {
        pFileWork = _FindFileOpt(ECacheSource::kWalog, 0);
    }
    if (nullptr != pFileWork) {
        if (pfun(pFileWork)) {
            nSuccess += 1;
        }
        nTotalCount += 1;
    }
    if (nSuccess < nTotalCount) {
        return false;
    }
    return true;
}
/// @brief Initialize the buffer
/// @param nCacheCount
/// @param nPageLen
void PManagerCache::_InitMemoryPagePool(uint32_t const nCacheCount, uint32_t const nPageLen) noexcept
{
    bool bNeedCreate{true};
    if ((vecCachePage_.size() >= nCacheCount) && (nCacheCount > 0U)) {
        if (vecCachePage_[0U]->GetCacheLen() == nPageLen) {
            bNeedCreate = false;
        }
    }
    if (bNeedCreate) {
        _FreeMemoryPagePool();
        for (uint32_t i{0U}; i < nCacheCount; i++) {
            PMemoryPage *const pCachePage{new PMemoryPage(i + 1U, nPageLen)};
            pCachePage->ResetCacheData();
            vecCachePage_.push_back(pCachePage);
        }
    } else {
        // Reused CachePage requires clearing attribute data
        for (auto &pCachePage : vecCachePage_) {
            pCachePage->ResetCacheData();
        }
    }
}
/// @brief Check if all CachePages
void PManagerCache::_FreeMemoryPagePool() noexcept
{
    for (auto &pCachePage : vecCachePage_) {
        if (nullptr == pCachePage) {
            continue;
        }
        delete pCachePage;
    }
    vecCachePage_.clear();
    nOptIndex_ = 0U;
}
/// @brief Write a page of data to disk
/// @param pMemoryPage
/// @param nPageID
/// @param eCacheSource
/// @param nReddIndex
/// @return
bool PManagerCache::_SaveFilePage(PMemoryPage const *const pMemoryPage,
                                  uint32_t const nPageID,
                                  ECacheSource const eCacheSource,
                                  int32_t const nReddIndex) noexcept
{
    if (nullptr == pMemoryPage) {
        return false;
    }
    PFileOpt_Page *pWorkFileOpt{nullptr};
    pWorkFileOpt = _PrepareFileOpt(nPageLen_, eCacheSource, nReddIndex);
    if (nullptr == pWorkFileOpt) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    int32_t const nSaveDataLen{static_cast< int32_t >(pMemoryPage->GetCacheLen())};
    if (false == pWorkFileOpt->WritePage(nPageID, pMemoryPage->GetCache(), nSaveDataLen)) {
        _LogError("[PManagerCache::_SaveFilePage]", kvConfigMuster_.GetStorageName().data(), "error",
                  PFileOpt_Page::GetLastError());
        PER_OnOptFailed(EErrorPHKV::kFileKvWrite);
        return false;
    }
    std::ignore = pWorkFileOpt->Flush();
    if ((eCacheSource == ECacheSource::kMain)
        && (nReddIndex > 0))  // Redundant data is closed immediately after operation
    {
        std::ignore = pWorkFileOpt->CloseFile();
    }
    _LogPrint(ara::log::LogLevel::kDebug, "PManagerCache::_SaveFilePage", "", "nMainPageID",
              static_cast< int32_t >(pMemoryPage->GetMainPageID()), "eCacheSource",
              static_cast< int32_t >(eCacheSource), "WalogPageID",
              static_cast< int32_t >(pMemoryPage->GetWalogPageID()));
    _OnSuccess();
    return true;
}
/// @brief Save a memory page to the Walog library, update the CRC checksum before saving
/// @param pMemoryPage
/// @return
bool PManagerCache::_SaveWalogPage(PMemoryPage *const pMemoryPage) noexcept
{
    if (nullptr == pMemoryPage) {
        return false;
    }
    // This will cause one pMemoryPage to be held by two PCachePagePtr instances
    PCachePagePtr const pCachePage{std::make_shared< PCachePage_Proxy >(pMemoryPage, this)};
    PPageOptBase const optPage{pCachePage};
    optPage.UpdatePageCRC32();
    return _SaveFilePage(pMemoryPage, pMemoryPage->GetWalogPageID(), ECacheSource::kWalog, 0);
}
/// @brief Load a page of data from a specific file
/// @param pCacheData
/// @param nPageID
/// @param eCacheSource
/// @param nReddIndex
/// @return
bool PManagerCache::_ReadFilePage(PMemoryPage *const pCacheData,
                                  uint32_t const nPageID,
                                  ECacheSource const eCacheSource,
                                  int32_t const nReddIndex) noexcept
{
    PER_Assert(nullptr != pCacheData);
    // Logic: find a cache page (crash and report error if not found), read data into this cache, maintain relevant state, then return this cache structure
    PFileOpt_Page *pWorkFileOpt{nullptr};
    pWorkFileOpt = _PrepareFileOpt(nPageLen_, eCacheSource, nReddIndex);
    if (nullptr == pWorkFileOpt) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    if (pWorkFileOpt->GetFileSize() <= 0) {
        _OnOptFailed(EErrorPHKV::kFileSize);
        return false;
    }
    PCacheAutoLock const autoLockPage{pCacheData, nullptr};  // Automatically bind a no-op to the cache page
    pCacheData->ResetCacheData();
    if (pWorkFileOpt->ReadPage(nPageID, pCacheData->GetCache(), pCacheData->GetCacheLen()) < 0) {
        PER_OnOptFailed(EErrorPHKV::kFileKvRead);
        return false;
    }
    pCacheData->InitPageData(eCacheSource, nPageID);
    pCacheData->SetWriteRef(0);
    pCacheData->SetOptIndex(nOptIndex_);  // nOptIndex automatically returns if out of bounds
    pCacheData->SetReddIndex(nReddIndex);
    nOptIndex_ += 1U;
    return true;
}
/// @brief Load a specific page from M/N redundant files
/// @param pCacheData
/// @param nPageID
/// @return
bool PManagerCache::_ReadFilePageFromRedd(PMemoryPage *const pCacheData, uint32_t const nPageID) noexcept
{
    if (false == kvConfigMuster_.IsReddMofN()) {
        return false;
    }
    PER_Assert(nullptr != pCacheData);
    ara::core::Map< uint32_t, int32_t >::iterator const itFind{mapReddFindIndex_.find(nPageID)};
    if (itFind != mapReddFindIndex_.end()) {
        if (itFind->second != -1) {
            return _ReadFilePage(pCacheData, nPageID, ECacheSource::kMain, itFind->second);
        }
    }
    PLoadReddPage loadReddPage{this};
    int32_t nFindReddIndex{0};
    nFindReddIndex = loadReddPage.LoadReddPage(pCacheData, nPageID, kvConfigMuster_.GetReddCountN(),
                                               kvConfigMuster_.GetReddCountM());
    if (-1 == nFindReddIndex) {
        _OnOptFailed(EErrorPHKV::kPageReadRedd);
    }
    mapReddFindIndex_[nPageID] = nFindReddIndex;
    return -1 != nFindReddIndex;
}
/// @brief Initialize redundant file pointers
void PManagerCache::_NewMainFileOpt() noexcept
{
    // The Main file naturally counts as one redundancy copy
    uint32_t const nMainFileCount{kvConfigMuster_.GetReddCountN()};
    for (int32_t i{0}; i < static_cast< int32_t >(nMainFileCount); i++) {
        std::unique_ptr< PFileOpt_Page > pFileOptPtr{
            std::make_unique< PFileOpt_Page >(static_cast< uint16_t >(kvConfigMuster_.GetPageInitLen()))};
        if (kvConfigMuster_.IsHaveCrypto_Storage(ECryptoKeySlotUsage::kHaveCrypto)) {
            PConfigData_Crypto const &configCrypto{kvConfigMuster_.GetCryptoConfig()};
            PER_Assert(pFileOptPtr->BuildCrypto(configCrypto.stKeySlotName, configCrypto.stCryptoAlgorithm));
        }
        mapFileOptMain_[i] = std::move(pFileOptPtr);
    }
}
/// @brief Delete redundant file pointers
void PManagerCache::_DelMainFileOpt() noexcept
{
    for (auto &it : mapFileOptMain_) {
        it.second.reset();
    }
    mapFileOptMain_.clear();
}
//***************/
/// @brief Find a file operation class
/// @param eCacheSource
/// @param nReddIndex
/// @return
PFileOpt_Page *PManagerCache::_FindFileOpt(ECacheSource const eCacheSource, int32_t const nReddIndex) const noexcept
{
    PFileOpt_Page *pFileOpt{nullptr};
    switch (eCacheSource) {
        case ECacheSource::kWalog: {
            pFileOpt = &fileOptWalog_;
        } break;
        case ECacheSource::kMain: {
            pFileOpt = nullptr;
            ara::core::Map< int32_t, std::unique_ptr< PFileOpt_Page > >::const_iterator const itFind{
                mapFileOptMain_.find(nReddIndex)};
            if (itFind != mapFileOptMain_.end()) {
                pFileOpt = itFind->second.get();
                break;
            }
            _LogFatal("[PManagerCache::_FindFileOpt]", "ECacheSource::kMain", "ErrorCode = ", nReddIndex);
        } break;
        default: {
            pFileOpt = nullptr;
        } break;
    }

    return pFileOpt;
}
/// @brief Perform all necessary preparations for file operations
/// @param nPageLen
/// @param eCacheSource
/// @param nReddIndex
/// @return
PFileOpt_Page *PManagerCache::_PrepareFileOpt(uint16_t const nPageLen,
                                              ECacheSource const eCacheSource,
                                              int32_t const nReddIndex) const noexcept
{
    ara::core::String const stWorkPath{kvConfigMuster_.GetWorkPath()};
    ara::core::String const stFileMain{GetFileName(T_StringView(stWorkPath), eCacheSource, nReddIndex)};
    if (false == PFileOpt::IsFileExist(T_StringView(stWorkPath))) {
        std::ignore = PFileOpt::MakeDir(stWorkPath, true, kDefDirMode);
    }
    PFileOpt_Page *pWorkFileOpt{nullptr};
    pWorkFileOpt = _FindFileOpt(eCacheSource, nReddIndex);
    if (nullptr == pWorkFileOpt) {
        std::ignore = fprintf(stderr, "[PManagerCache::_FindFileOpt] : WorkPath = %s, StorageName = %s\n",
                              kvConfigMuster_.GetWorkPath().data(), kvConfigMuster_.GetStorageName().data());
        _LogError("[PManagerCache::_FindFileOpt], stFileMain = ", stFileMain.data());
        return nullptr;
    }
    int32_t const nTryCount{3};  // Total of 3 attempts
    for (int32_t i{0}; i < nTryCount; i++) {
        bool const bSuccess{pWorkFileOpt->DoPrepareWork(T_StringView(stFileMain), nPageLen)};
        if (bSuccess) {
            break;
        }
        switch (PFileOpt_Page::GetLastError()) {
            case ENOENT:  // No such file or directory
            {
                // Try again after creating the directory
                if (false == PFileOpt::MakeDir(T_StringView(stWorkPath), true)) {
                    if (EEXIST != PFileOpt_Page::GetLastError()) {
                        _LogInfo("[PManagerCache::_PrepareFileOpt.MakeDir]", stWorkPath.data(), "ErrorCode",
                                 PFileOpt_Page::GetLastError());
                    }
                }
            } break;
            case EINVAL:  // Invalid argument
            {
                // 2022-12-09 hanjingjing discovered: Some development boards do not support the "NAI_O_DIRECT" parameter (direct file read/write without cache), causing file opening errors
                uint32_t const nNewFileFlags{
                    T_AndData< uint32_t >(pWorkFileOpt->GetFileFlags(), static_cast< uint32_t >(~NAI_O_DIRECT))};
                pWorkFileOpt->SetFileFlags(nNewFileFlags);
                _LogInfo("[PManagerCache::_PrepareFileOpt.SetFileFlags]", stFileMain.data(), "nNewFileFlags",
                         static_cast< int32_t >(nNewFileFlags));
            } break;
            case EAGAIN:  // NOLINT // FLockTry error: file is already locked by another process, ,
            {
            } break;
            default: {
            } break;
        }
    }
    if (nullptr != pWorkFileOpt) {
        if (false == pWorkFileOpt->IsLockSuccess()) {
            return nullptr;
        }
    }
    return pWorkFileOpt;
}
/// @brief Find a free cache page: prevent contention in multithreaded scenarios
/// @return
PMemoryPage *PManagerCache::_FindFreeCacheByThread() noexcept
{
    PAutoLock const autoLock{threadLockCache_};
    if (vecCachePage_.empty()) {
        return nullptr;
    }
    PMemoryPage *pFindCache{nullptr};
    pFindCache = _FindFreeCache();
    if (nullptr == pFindCache) {
        while ((nullptr == pFindCache)) {
            EErrorPHKV const errorCode{GetLastError()};
            if ((EErrorPHKV::kSuccess != errorCode) && (EErrorPHKV::kPageCacheCapacity != GetLastError())) {
                break;
            }
            std::ignore = threadCondCache_.Wait(threadLockCache_);
            pFindCache  = _FindFreeCache();
        }
    }
    if (nullptr != pFindCache) {
        pFindCache->SetOwnThreadID(pthread_self());
    }
    return pFindCache;
}
/// @brief Find a free cache page
/// @return
PMemoryPage *PManagerCache::_FindFreeCache() noexcept
{
    PMemoryPage::TimePoint const tmNow{std::chrono::steady_clock::now()};
    PMemoryPage *pFindCache{nullptr};
    int32_t nLockPageCount{0};
    int32_t nScanCount{0};
    for (auto &pCachePage : vecCachePage_) {
        if (nullptr == pCachePage) {
            continue;
        }
        if (false == pCachePage->IsCacheValid()) {
            continue;
        }
        nScanCount += 1;
        // Cannot be locked by an operation
        if (true == pCachePage->IsHaveOpt()) {
            nLockPageCount += 1;
            continue;
        }
        // Cannot be locked by another thread (including itself)
        if (pCachePage->GetOwnThreadID() > 0U) {
            nLockPageCount += 1;
            continue;
        }
        // Activation time is the initial 0
        if (0 == pCachePage->GetActiveOptTime()) {
            pFindCache = pCachePage;
            break;
        }
        if (nullptr == pFindCache) {
            pFindCache = pCachePage;
            continue;
        }
        int64_t const nIdleTime{pCachePage->GetIdleTime(tmNow)};
        int64_t const nFindIdleTime{pFindCache->GetIdleTime(tmNow)};
        if (nIdleTime > nFindIdleTime) {
            pFindCache = pCachePage;
        } else if (nIdleTime == nFindIdleTime) {
            if (pFindCache->GetOptIndex() > pCachePage->GetOptIndex()) {
                pFindCache = pCachePage;
            }
        } else {  // Do nothing, keep the previously selected pFindCache
        }
    }
    if (nullptr == pFindCache) {
        if ((nLockPageCount >= nScanCount) && (nScanCount >= static_cast< int32_t >(vecCachePage_.size()))) {
            _OnOptFailed(EErrorPHKV::kPageCacheCapacity);
        } else {
            PER_OnOptFailed(EErrorPHKV::kPageFindFree);
        }
        return nullptr;
    }
    // Save cold data to disk
    if (false == _TryToSaveColdPage(pFindCache)) {
        PER_OnOptFailed(EErrorPHKV::kPageCacheSave);
        return nullptr;
    }
    if (nullptr != pFindCache) {
        pFindCache->SetActiveOptTime(std::chrono::steady_clock::now());
    }
    return pFindCache;
}
/// @brief Save cold data to disk
/// @param pColdPage
/// @return
bool PManagerCache::_TryToSaveColdPage(PMemoryPage *const pColdPage) noexcept
{
    PER_Assert(nullptr != pColdPage);
    // Automatically bind a no-op to the cache page
    PCacheAutoLock const autoLockPage{pColdPage, nullptr};
    if (false == pColdPage->IsWriteRef()) {
        if (ECacheSource::kDefault == pColdPage->GetPageSource()) {
            return true;
        }
        // If the cache page has not been modified, it does not need to be actually saved; just delete the corresponding relationship from the WalogID list
        if (ECacheSource::kMain == pColdPage->GetPageSource()) {
            return true;
        }
        if (GetWalogPageTotal() < pColdPage->GetWalogPageID()) {
            return true;
        }
    }
    if (false == PPageOptBase::IsValidPageID(pColdPage->GetWalogPageID())) {
        _OnOptFailed(EErrorPHKV::kErrorWalogID);
        return true;
    }
    // Update the page's CRC32 before writing to disk
    if (false == _SaveWalogPage(pColdPage)) {
        return false;
    }
    pColdPage->SetWriteRef(0);
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
