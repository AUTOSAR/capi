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
/// @file       manager_page.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Logical page manager used by Puhua KvStorage
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManagerPage
/// @unit_description=Manage the mapping of logical pages to disk space
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

#include "ara/per/internal/isoftkv/manager_page.h"

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/page_opt_ext_string.h"
#include "ara/per/internal/isoftkv/page_opt_key_hash.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"
#include "ara/per/internal/isoftkv/page_opt_page_boss.h"
#include "ara/per/internal/isoftkv/page_opt_value.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief Update the 4th field of the KvSystem version number
/// @param bClear Whether to clear
/// @return
void G_IncVersionSystemElse(PPageOptPageGroup const& pageOptGroup, bool const bClear) noexcept
{
    ara::per::isoftkv::PPerVersion versionSystem{pageOptGroup.GetPerVersion_Sys()};
    if (bClear) {
        versionSystem.sVersionElse = 0;
    } else {
        versionSystem.sVersionElse += 1;
    }
    pageOptGroup.SetPerVersion_System(versionSystem);
}
//********************************/
/// @brief
/// @param kvConfigMuster
/// @param managerCache
PManagerPage::PManagerPage(PConfigMuster_Kv const& kvConfigMuster, PManagerCache& managerCache) noexcept
    : kvConfigMuster_{kvConfigMuster}, managerCache_{managerCache}
{
}
/// @brief Check if the cache system is ready
/// @return
bool PManagerPage::IsAccessReady() const noexcept
{
    // The reason for not checking pageOptWalog_ is: in read-only mode, pageOptWalog_ is never enabled
    if (false == managerCache_.IsAccessReady()) {
        return false;
    }
    if (false == pageOptGroup_.IsAccessReady()) {
        return false;
    }
    return true;
}
/// @brief Get the KV library name displayed in the log system
/// @return
ara::core::StringView PManagerPage::GetLogKvName() const noexcept { return std::move(managerCache_.GetLogKvName()); }
//***************/
/// @brief Initialize pageOptGroup_
/// @return
bool PManagerPage::InitPageOptGroup() noexcept
{
    uint32_t const nDefPageID{static_cast< uint32_t >(EDefaultValue::kDefStartPageID)};
    // Resident memory PageGroup
    PCachePagePtr pPageGroup{nullptr};
    pPageGroup = LoadCachePage(nDefPageID, false);
    if (nullptr == pPageGroup) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    if (false == pageOptGroup_.InitCachePage(pPageGroup)) {
        PER_OnOptFailed(pageOptGroup_.GetLastError());
        return false;
    }
    return true;
}
/// @brief Initialize pageOptWalog_
/// @return
bool PManagerPage::InitWalogDB() noexcept
{
    managerWalogID_.Reset();
    if (false == managerCache_.WalogInitFile()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageWalog);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Release the cache bound to pageOptGroup_
/// @return
bool PManagerPage::ReleasePageOpt() noexcept
{
    pageOptGroup_.DetachCachePage();
    return true;
}
/// @brief Initialize the mapping table for PManagerWalogID using the Walog library
/// @param vecWalogID
/// @return
int32_t PManagerPage::ResetWalogIdByDbFile(ara::core::Vector< uint32_t > const& vecWalogID) noexcept
{
    managerWalogID_.Reset();
    for (uint32_t const& nWalogID : vecWalogID) {
        std::ignore = managerWalogID_.RegMainPage(nWalogID);
    }
    return static_cast< int32_t >(vecWalogID.size());
}
/// @brief Initialize an empty system
/// @param bSoloHash
/// @param bSoloBoss
/// @param bSoloKv
/// @return
PCachePagePtr PManagerPage::InitEmptySystem(bool const bSoloHash, bool const bSoloBoss, bool const bSoloKv) noexcept
{
    // Initialize Walog ID management
    if (false == InitWalogDB()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return PCachePagePtr{nullptr};
    }
    // Group page
    PCachePagePtr pPageGroup{_newInitPage(nullptr, true)};
    if (nullptr == pPageGroup) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return PCachePagePtr{nullptr};
    }
    // Decide whether to reuse Group page based on configuration: Boss page
    PCachePagePtr pPageBoss{_newInitPage(pPageGroup, bSoloBoss)};
    if (nullptr == pPageBoss) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return PCachePagePtr{nullptr};
    }
    // Decide whether to reuse Group page based on configuration: Hash page
    PCachePagePtr const pPageHash{_newInitPage(pPageGroup, bSoloHash)};
    if (nullptr == pPageHash) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return PCachePagePtr{nullptr};
    }
    // Decide whether to reuse Hash page based on configuration: KV storage page
    PCachePagePtr pPageKv{_newInitPage(pPageHash, bSoloKv)};
    if (nullptr == pPageKv) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return PCachePagePtr{nullptr};
    }
    // Initialize PageGroup page
    _InitPageGroup(pPageGroup, static_cast< uint32_t >(EDefaultValue::kDefStartPageID));
    // Update the group number of the first group
    std::ignore = pageOptGroup_.UpdateBossPageID(static_cast< uint16_t >((*pPageGroup)->GetMainPageID()),
                                                 static_cast< uint16_t >((*pPageBoss)->GetMainPageID()));
    // Update the value of the Hash page
    pageOptGroup_.SetHashPageID((*pPageHash)->GetMainPageID());
    // Initialize Hash page
    uint32_t const nDefKvPageID{(*pPageKv)->GetMainPageID()};
    _InitPageHash(pPageHash, nDefKvPageID);
    // Initialize Boss page
    _InitPageBoss(pPageBoss, 0U);
    // Continue initializing Ext data
    PPageOpt_ExtString optPageExt{pPageHash};
    PER_Assert(optPageExt.InitExtHead(static_cast< uint16_t >(EDefaultValue::kMaxExtVersionLenLimit)) == true);
    // The version number of KvSystem can only be set when initializing the DB library file
    ara::core::StringView const stVersionSystem{GetKvSystemVersion()};
    PPerVersion perVersionSystem;
    ara::core::String stVersionSystemDetail;
    if (false == G_TransVersionString(stVersionSystem, perVersionSystem, &stVersionSystemDetail)) {
        PER_OnOptFailed(EErrorPHKV::kPageGroupVersion);
        return PCachePagePtr{nullptr};
    }
    pageOptGroup_.SetPerVersion_System(perVersionSystem);
    optPageExt.SetVersionSystem(stVersionSystemDetail);

    // Initialize KV page: must be initialized last
    PPageOptKvStore optKvPage{pPageKv};
    PER_Assert(optKvPage.InitKvHead(nDefKvPageID, nDefKvPageID, false));
    // Update free management for up to 4 pages
    std::ignore = _UpdateBossPage(pPageBoss, (*pPageGroup)->GetMainPageID(), 0U);
    if (pPageBoss != pPageGroup) {
        std::ignore = _UpdateBossPage(pPageBoss, (*pPageBoss)->GetMainPageID(), 0U);
    }
    if (pPageHash != pPageGroup) {
        std::ignore = _UpdateBossPage(pPageBoss, (*pPageHash)->GetMainPageID(), 0U);
    }
    if (pPageKv != pPageHash) {
        std::ignore = _UpdateBossPage(pPageBoss, (*pPageKv)->GetMainPageID(), 0U);
    }
    return pPageGroup;
}
/// @brief Find a free page; if none exists, create a new one; return nullptr if capacity is exceeded
/// @param ePageType
/// @param nCurPageID
/// @param nNeedSpace
/// @return
PCachePagePtr PManagerPage::FindFreePage(EBlockType const ePageType,
                                         uint32_t const nCurPageID,
                                         uint16_t const nNeedSpace) noexcept
{
    if (false == managerCache_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyCache);
        return PCachePagePtr{nullptr};
    }
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return PCachePagePtr{nullptr};
    }
    PCachePagePtr pFindPage{_findExistFreePage(nCurPageID, nNeedSpace)};
    if (nullptr == pFindPage) {
        // Before creating a new page, determine whether the Boss page needs to be made independent
        if (IsNeedSoloPageBoss(nCurPageID)) {
            if (nullptr == SoloPageBoss(nCurPageID)) {
                PER_OnOptFailed(EErrorPHKV::kPageSoloBoss);
                return PCachePagePtr{nullptr};
            }
        }
        pFindPage = _prepareNewMainPage(true);
        if (nullptr == pFindPage) {
            return PCachePagePtr{nullptr};
        }
        uint16_t nNewFreeLen{0U};
        if (ePageType == EBlockType::kValue) {
            PPageOptBase const optBase{nullptr};
            nNewFreeLen = managerCache_.GetPageLen() - optBase.GetMinValidLen();
        }
        if (false == _UpdateBossPage(PCachePagePtr{nullptr}, (*pFindPage)->GetMainPageID(), nNewFreeLen)) {
            return PCachePagePtr{nullptr};
        }
        _LogInfo("[KV_Manager].PreparePage_New", ":", "ePageType", static_cast< int32_t >(ePageType), "nNeedSpace",
                 static_cast< int32_t >(nNeedSpace), "OldPageID", static_cast< int32_t >(nCurPageID), "NewPageID",
                 static_cast< int32_t >((*pFindPage)->GetMainPageID()));
    } else {
        _LogInfo("[KV_Manager].PreparePage_Old", ":", "ePageType", static_cast< int32_t >(ePageType), "nNeedSpace",
                 static_cast< int32_t >(nNeedSpace), "OldPageID", static_cast< int32_t >(nCurPageID), "NewPageID",
                 static_cast< int32_t >((*pFindPage)->GetMainPageID()));
    }
    _OnSuccess();
    return pFindPage;
}
/// @brief Load a page of data
/// @param nMainPageID
/// @param bForWrite
/// @return
PCachePagePtr PManagerPage::LoadCachePage(uint32_t const nMainPageID, bool const bForWrite) const noexcept
{
    if (false == PPageOptBase::IsValidPageID(nMainPageID)) {
        PER_OnOptFailed(EErrorPHKV::kErrorPageID);
        return PCachePagePtr{nullptr};
    }
    PCachePagePtr pReturnCache{nullptr};
    // First search memory, then the Walog library, then the Main library
    PMemoryPage* pLoadPage{nullptr};
    pLoadPage = managerCache_.FindCachePage(nMainPageID);
    if (nullptr == pLoadPage) {
        // Need to map Main.PageID to Walog.PageIndex here
        uint32_t const nWalogPageID{MainToWalogPageID(nMainPageID)};
        // Read from Walog library
        if (PPageOptBase::IsValidPageID(nWalogPageID)) {
            // A cache page may be discarded from being saved to the Walog library because it hasn't been modified; in this case, nWalogPageID is valid but the page does not actually exist in the Walog library, so it should be read from the main library
            if (nWalogPageID <= managerCache_.GetWalogPageTotal()) {
                pLoadPage = managerCache_.LoadFilePage(ECacheSource::kWalog, nWalogPageID);
            }
        }
        // If not found in the Walog library, read from the Main library
        if (nullptr == pLoadPage) {
            uint32_t const nMainPageCount{managerCache_.GetMainPageTotal()};
            PER_Assert(nMainPageCount >= nMainPageID);
            pLoadPage = managerCache_.LoadFilePage(ECacheSource::kMain, nMainPageID);
        }
        // If still not found here, it truly doesn't exist
        if (nullptr == pLoadPage) {
            PER_OnOptFailed(EErrorPHKV::kPageCacheLoad);
            return PCachePagePtr{nullptr};
        }
        pReturnCache = _makeProxyPagePtr(pLoadPage);
        PPageOptBase const pageOptBase{pReturnCache};
        if (false == pageOptBase.CheckPageCRC32()) {
            PER_OnOptFailed(EErrorPHKV::kErrorPageCRC32);
            return PCachePagePtr{nullptr};
        }
        pLoadPage->SetMainPageID(nMainPageID);
        pLoadPage->SetWalogPageID(nWalogPageID);
    } else {
        pReturnCache = _makeProxyPagePtr(pLoadPage);
    }
    if ((nullptr != pLoadPage) && bForWrite) {
        if (false == MoveCachePageToWalog(pReturnCache)) {
            PER_OnOptFailed(EErrorPHKV::kPageMoveToWalog);
            return PCachePagePtr{nullptr};
        }
    }
    return pReturnCache;
}
/// @brief Load HashPage
/// @return
PCachePagePtr PManagerPage::LoadCacheHash() const noexcept
{
    return LoadCachePage(pageOptGroup_.GetHashPageID(), false);
}
/// @brief Load a page from the Walog library and set the corresponding MainPageID
/// @param nWalogPageID
/// @return
PCachePagePtr PManagerPage::LoadWalogFilePage(uint32_t const nWalogPageID) noexcept
{
    PMemoryPage* pLoadPage{nullptr};
    pLoadPage = managerCache_.LoadFilePage(ECacheSource::kWalog, nWalogPageID);
    if (nullptr == pLoadPage) {
        PER_OnOptFailed(EErrorPHKV::kFileKvRead);
        return PCachePagePtr{nullptr};
    }
    uint32_t const nMainPageID{WalogToMainPageID(nWalogPageID)};
    pLoadPage->SetMainPageID(nMainPageID);
    pLoadPage->SetWalogPageID(nWalogPageID);
    return _makeProxyPagePtr(pLoadPage);
}
/// @brief Find an independent KV page, create a new one if not found
/// @param nCurPageID
/// @param nKvStoreID
/// @return
PCachePagePtr PManagerPage::PreparePageSoloKv(uint32_t const nCurPageID, uint32_t const nKvStoreID) noexcept
{
    PCachePagePtr pFindPageKv{nullptr};
    pFindPageKv = FindFreePage(EBlockType::kKvStore, nCurPageID, managerCache_.GetPageLen());
    if (nullptr == pFindPageKv) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindPage);
        return PCachePagePtr{nullptr};
    }
    // All obtained CachePages need to have KV page initialization
    PPageOptKvStore optKvPage{pFindPageKv};
    if (false == optKvPage.InitKvHead(nKvStoreID, (*pFindPageKv)->GetMainPageID(), false)) {
        PER_OnOptFailed(optKvPage.GetLastError());
        return PCachePagePtr{nullptr};
    }
    _OnSuccess();
    return pFindPageKv;
}
/// @brief Find a suitable Value page
/// @param nCurPageID
/// @param nValueLen
/// @return
PCachePagePtr PManagerPage::PreparePageValue(uint32_t const nCurPageID, uint16_t const nValueLen) noexcept
{
    uint16_t const nNeedSpace{static_cast< uint16_t >(nValueLen + sizeof(PBlockHead))};
    PCachePagePtr pFindPageValue;
    pFindPageValue = FindFreePage(EBlockType::kValue, nCurPageID, nNeedSpace);
    if (nullptr == pFindPageValue) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindPage);
        return PCachePagePtr{nullptr};
    }
    PPageOptValue optValue{pFindPageValue};
    if (false == optValue.IsAccessReady())  // Reinitialize
    {
        if (false == optValue.InitValueHead()) {
            PER_OnOptFailed(optValue.GetLastError());
            return PCachePagePtr{nullptr};
        }
    }
    return pFindPageValue;
}
/// @brief Check whether the PageBoss page needs to be made independent
/// @param nCurPageID
/// @return
bool PManagerPage::IsNeedSoloPageBoss(uint32_t const nCurPageID) noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    if (pageOptGroup_.TransGroupID(nCurPageID)
        > 0U) {  // Starting from the second group, PageBoss pages are already independent
        _OnSuccess();
        return false;
    }
    PCachePagePtr pPageBossOld{_findPageBoss(nCurPageID, true)};
    PPageOptPageBoss optBossOld{pPageBossOld};
    if (false == optBossOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return false;
    }
    _OnSuccess();
    if (optBossOld.IsSoloPage()) {
        return false;
    }
    // Even if there is space in the mixed page, do not trigger the BossPage Solo event
    if (true == optBossOld.CheckBossPageCapacity()) {
        return false;
    }
    return true;
}
/// @brief Make the PageBoss page independent
/// @param nCurPageID
/// @return
PCachePagePtr PManagerPage::SoloPageBoss(uint32_t const nCurPageID) noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return PCachePagePtr{nullptr};
    }
    PCachePagePtr pPageBossOld{_findPageBoss(nCurPageID, true)};
    PPageOptPageBoss optBossOld{pPageBossOld};
    if (false == optBossOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return PCachePagePtr{nullptr};
    }
    if (optBossOld.IsSoloPage()) {
        _OnSuccess();
        return pPageBossOld;
    }
    // Even if there is space in the mixed page, do not trigger the BossPage Solo event
    if (true == optBossOld.CheckBossPageCapacity()) {
        _OnSuccess();
        return pPageBossOld;
    }
    // SoloBoss events only occur within group 0; other groups directly create SoloPageBoss
    PCachePagePtr pPageBossNew{_prepareNewMainPage(true)};
    PPageOptPageBoss optBossNew{pPageBossNew};  // optBossNew contains uninitialized data, cannot be used directly
    if (false == optBossNew.InitPageHead(EBlockType::kPageBoss, (*pPageBossNew)->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return PCachePagePtr{nullptr};
    }
    // Copy old data to new page
    uint16_t const nBlockID{optBossOld.GetSoloBlockID()};
    if (false == optBossOld.CopyBlockToNewPage(nBlockID, &optBossNew)) {
        PER_OnOptFailed(optBossOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Delete old page data
    if (false == optBossOld.DelBlock(nBlockID)) {
        PER_OnOptFailed(optBossOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Modify the default BossPage index within the Group management page
    if (false
        == pageOptGroup_.UpdateBossPageID(static_cast< uint16_t >((*pPageBossOld)->GetMainPageID()),
                                          static_cast< uint16_t >((*pPageBossNew)->GetMainPageID()))) {
        /// Theoretically, we could continue expanding PageGroup to solve the problem, but as of 2023-07-11, the KV library does not yet need to support such large capacity
        PER_OnOptFailed(pageOptGroup_.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Modify the BossPage's own free information
    if (false == _UpdateBossPage(pPageBossNew, (*pPageBossNew)->GetMainPageID(), 0U)) {
        PER_OnOptFailed(EErrorPHKV::kPageNewPage);
        return PCachePagePtr{nullptr};
    }
    _LogInfo("[KV_Manager].SoloPage_Boss", ":", "OldPageID", static_cast< int32_t >((*pPageBossOld)->GetMainPageID()),
             "NewPageID", static_cast< int32_t >((*pPageBossNew)->GetMainPageID()));
    _OnSuccess();
    return pPageBossNew;
}
/// @brief Change a Hash page from a mixed page to an independent page, and return the new page
/// @param pPageHashOld
/// @return
PCachePagePtr PManagerPage::SoloPageHash(PCachePagePtr const& pPageHashOld) noexcept
{
    PER_Assert(nullptr != pPageHashOld);
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return PCachePagePtr{nullptr};
    }
    PPageOptKeyHash pageOptHashOld{pPageHashOld};
    if (false == pageOptHashOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return PCachePagePtr{nullptr};
    }
    if (pageOptHashOld.IsHashPageSolo()) {
        _OnSuccess();
        return pPageHashOld;
    }
    // Move HashBlock to an exclusive page
    PCachePagePtr pPageHashNew;
    pPageHashNew = FindFreePage(EBlockType::kKeyHash, (*pPageHashOld)->GetMainPageID(), managerCache_.GetPageLen());
    if (nullptr == pPageHashNew) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return PCachePagePtr{nullptr};
    }
    // Initialize PageHead
    PPageOptKeyHash pageOptHashNew{pPageHashNew, 0U};
    if (false == pageOptHashNew.InitPageHead(EBlockType::kKeyHash, (*pPageHashNew)->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return PCachePagePtr{nullptr};
    }
    // Copy old data to new page
    uint16_t const nBlockID{pageOptHashOld.GetSoloBlockID()};
    if (false == pageOptHashOld.CopyBlockToNewPage(nBlockID, &pageOptHashNew)) {
        PER_OnOptFailed(pageOptHashOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Delete old page data
    if (false == pageOptHashOld.DelBlock(nBlockID)) {
        PER_OnOptFailed(pageOptHashOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    // Bind the new Hash page
    pageOptGroup_.SetHashPageID(pageOptHashNew.GetPageID());
    _LogInfo("[KV_Manager].SoloPage_Hash", ": ", "OldPageID", static_cast< int32_t >((*pPageHashOld)->GetMainPageID()),
             "NewPageID", static_cast< int32_t >((*pPageHashNew)->GetMainPageID()));
    return pPageHashNew;
}
/// @brief Move the ExtString page from the mixed page to a new page, and return the new page
/// @param pPageOld
/// @param pPageNew
/// @return
PCachePagePtr PManagerPage::SoloPage_ExtString(PCachePagePtr const& pPageOld, PCachePagePtr const& pPageNew) noexcept
{
    PER_Assert(nullptr != pPageOld);
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return PCachePagePtr{nullptr};
    }
    if (pPageNew == pPageOld) {
        _OnSuccess();
        return pPageNew;
    }
    PPageOpt_ExtString pageOptOld{pPageOld};
    if (false == pageOptOld.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageHash);
        return PCachePagePtr{nullptr};
    }
    if (pageOptOld.IsSoloPage()) {
        _OnSuccess();
        return pPageOld;
    }
    // Initialize PageHead
    PPageOpt_ExtString pageOptNew{pPageNew, 0U};
    if (false == pageOptNew.InitPageHead(EBlockType::kKeyHash, (*pPageNew)->GetMainPageID())) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return PCachePagePtr{nullptr};
    }
    // Copy old data to new page
    uint16_t const nBlockID{pageOptOld.GetSoloBlockID()};
    if (false == pageOptOld.CopyBlockToNewPage(nBlockID, &pageOptNew)) {
        PER_OnOptFailed(pageOptOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    pageOptNew.AddPageType(pageOptNew.PageClassType());
    // Delete old page data
    if (false == pageOptOld.DelBlock(nBlockID)) {
        PER_OnOptFailed(pageOptOld.GetLastError());
        return PCachePagePtr{nullptr};
    }
    _LogInfo("[KV_Manager].SoloPage_ExtString", ": ", "OldPageID", static_cast< int32_t >((*pPageOld)->GetMainPageID()),
             "NewPageID", static_cast< int32_t >((*pPageNew)->GetMainPageID()));
    return pPageNew;
}
/// @brief Move the free index list of a given page: applicable only for KV and Value page calls
/// @param pageOptWork
/// @return
bool PManagerPage::UpdateFreeList(PPageOptBase const& pageOptWork) noexcept
{
    uint32_t const nWorkPageID{pageOptWork.GetPageID()};
    uint32_t const nBossPageID{pageOptGroup_.GetBossPageID(nWorkPageID)};
    if (false == PPageOptBase::IsValidPageID(nBossPageID)) {
        PER_OnOptFailed(pageOptGroup_.GetLastError());
        return false;
    }
    PCachePagePtr pPageBoss{nullptr};
    pPageBoss = LoadCachePage(nBossPageID, true);
    if (nullptr == pPageBoss) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return false;
    }
    PPageOptPageBoss pageOptBoss{pPageBoss, 0U};
    if (false == pageOptBoss.IsAccessReady()) {
        PER_OnOptFailed(pageOptBoss.GetLastError());
        return false;
    }
    uint16_t const nWorkPageIndex{pageOptGroup_.TransBossPageIndex(nWorkPageID)};
    if (false == pageOptBoss.UpdateFreePage(nWorkPageIndex, pageOptWork.GetFreeLen())) {
        PER_OnOptFailed(pageOptBoss.GetLastError());
        return false;
    }

    _OnSuccess();
    return true;
}
/// @brief Change the source of the cache page to the Walog library
/// @param pCachePagePtr
/// @return
bool PManagerPage::MoveCachePageToWalog(PCachePagePtr const& pCachePagePtr) const noexcept
{
    if (nullptr == pCachePagePtr) {
        return false;
    }
    PMemoryPage* pMemoryPage{nullptr};
    pMemoryPage = pCachePagePtr->GetMemoryPage();
    if (nullptr == pMemoryPage) {
        return false;
    }
    if (pMemoryPage->GetPageSource() == ECacheSource::kWalog) {
        return true;
    }
    // Modify the page number of the Walog library
    uint32_t const nWalogPageID{managerWalogID_.RegMainPage(pMemoryPage->GetMainPageID())};
    pMemoryPage->SetWalogPageID(nWalogPageID);
    // Change the data source of the cache page to the Walog library
    pMemoryPage->SetPageSource(ECacheSource::kWalog);
    pMemoryPage->AddWriteRef(true);
    _LogDebug("[KV_Page].MoveCachePageToWalog", ":", "MainPageID", static_cast< int32_t >(pMemoryPage->GetMainPageID()),
              "WalogPageID", static_cast< int32_t >(pMemoryPage->GetWalogPageID()));
    return true;
}
/// @brief Main.PageID>>=>>Walog.PageID
/// @param nMainPageID
/// @return
uint32_t PManagerPage::MainToWalogPageID(uint32_t const nMainPageID) const noexcept
{
    return managerWalogID_.FindWalogID(nMainPageID);
}
/// @brief Walog.PageID>>=>>Main.PageID
/// @param nWalogPageID
/// @return
uint32_t PManagerPage::WalogToMainPageID(uint32_t const nWalogPageID) const noexcept
{
    return managerWalogID_.FindMainID(nWalogPageID);
}
/// @brief Get the number of pages in the Walog library
/// @return
uint32_t PManagerPage::GetWalogPageCount() const noexcept { return managerWalogID_.GetWalogCount(); }
/// @brief Check storage space usage to determine if a new page can be created
/// @param nBakFileLen
/// @return
bool PManagerPage::CheckFileSpace(int64_t const nBakFileLen) const noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        return false;
    }
    int64_t const nPageLen{static_cast< int64_t >(managerCache_.GetPageLen())};
    int64_t nFileLen{0};
    // Main library
    if (pageOptGroup_.IsAccessReady()) {
        nFileLen = static_cast< int64_t >(pageOptGroup_.GetPageTotal()) * nPageLen;
    }
    // Main library * Redd.n
    nFileLen = nFileLen * static_cast< int64_t >(kvConfigMuster_.GetReddCountN());
    // Secondary library
    nFileLen += static_cast< int64_t >(managerWalogID_.GetWalogCount()) * nPageLen;
    // Plus backup size
    nFileLen += nBakFileLen;

    return nFileLen < static_cast< int64_t >(pageOptGroup_.GetSpaceAmountMax());
}
/// @brief Get the PER format version number information saved in the library: persistent data
/// @return
PPerVersion const& PManagerPage::GetPerVersion_Per() const noexcept
{
    PER_Assert(pageOptGroup_.IsAccessReady());
    return pageOptGroup_.GetPerVersion_Per();
}
/// @brief Get the PER format version number information saved in the library: KvSystem
/// @return
PPerVersion const& PManagerPage::GetPerVersion_Sys() const noexcept
{
    PER_Assert(pageOptGroup_.IsAccessReady());
    return pageOptGroup_.GetPerVersion_Sys();
}
/// @brief Get the string format version saved in the Kv library: KvSystem
/// @return
ara::core::String PManagerPage::GetStringVersion_Sys() const noexcept
{
    PER_Assert(pageOptGroup_.IsAccessReady());
    PPerVersion const versionSystem{pageOptGroup_.GetPerVersion_Sys()};
    ara::core::String stVersion{Version_PerToString(versionSystem)};
    PPageOpt_ExtString optPageExt{_getExtStringPage()};
    if (optPageExt.IsAccessReady()) {
        stVersion += optPageExt.GetVersionSystem();
    }
    // 2023-08-24 added
    stVersion += "[";
    stVersion += std::to_string(static_cast< uint32_t >(versionSystem.sVersionElse)).c_str();
    stVersion += "]";
    return stVersion;
}
/// @brief Get the string format version saved in the Kv library: Per persistent data
/// @return
ara::core::String PManagerPage::GetStringVersion_Per() const noexcept
{
    PER_Assert(pageOptGroup_.IsAccessReady());
    ara::core::String stVersion{Version_PerToString(pageOptGroup_.GetPerVersion_Per())};
    PPageOpt_ExtString optPageExt{_getExtStringPage()};
    if (optPageExt.IsAccessReady()) {
        stVersion += optPageExt.GetVersionPer();
    }
    return stVersion;
}
/// @brief Get the string format version saved in the Kv library: App
/// @return
ara::core::String PManagerPage::GetStringVersion_App() const noexcept
{
    PPageOpt_ExtString optPageExt{_getExtStringPage()};
    if (optPageExt.IsAccessReady()) {
        return optPageExt.GetVersionApp();
    }
    return ara::core::String{};
}
/// @brief Update the APP/PER version numbers within the system
/// @param stVersionApp APP version number
/// @param stVersionPer PER version number
/// @return Success or failure
bool PManagerPage::UpdateVersion(ara::core::String const& stVersionApp, ara::core::String const& stVersionPer) noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    // pageOptGroup_ is opened read-only during initialization, so we need to pre-set the write environment
    if (false == MoveCachePageToWalog(pageOptGroup_.GetCachePage())) {
        PER_OnOptFailed(EErrorPHKV::kPageMoveToWalog);
        return false;
    }
    PER_Assert(pageOptGroup_.IsAccessReady());
    PPerVersion perVersionPer;
    ara::core::String stPerVersionName;
    if (false == G_TransVersionString(stVersionPer, perVersionPer, &stPerVersionName)) {
        PER_OnOptFailed(EErrorPHKV::kPageGroupVersion);
        return false;
    }
    pageOptGroup_.SetPerVersion_Per(perVersionPer);
    // Continue initializing Ext data
    PPageOpt_ExtString optPageExt{_getExtStringPage()};
    PER_Assert(optPageExt.IsAccessReady());
    optPageExt.SetVersionApp(T_StringView(stVersionApp));
    optPageExt.SetVersionPer(T_StringView(stPerVersionName));
    G_IncVersionSystemElse(pageOptGroup_, true);
    LogInfo() << "KvStorage[" << GetLogKvName().data() << "] UpdateVersion: AppVersion=" << stVersionApp.c_str()
              << ", PerVersion=" << stVersionPer.c_str();
    _OnSuccess();
    return true;
}
/// @brief Set the PER version number saved in the library
/// @param version
/// @return
bool PManagerPage::UpdateVersionPer(PPerVersion const& version) noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    // pageOptGroup_ is opened read-only during initialization, so we need to pre-set the write environment
    if (false == MoveCachePageToWalog(pageOptGroup_.GetCachePage())) {
        PER_OnOptFailed(EErrorPHKV::kPageMoveToWalog);
        return false;
    }
    pageOptGroup_.SetPerVersion_Per(version);
    G_IncVersionSystemElse(pageOptGroup_, true);
    _OnSuccess();
    return true;
}
/// @brief Update the 4th field of the KvSystem version number
/// @param bClear Whether to clear
/// @return
bool PManagerPage::IncVersionSystemElse(bool const bClear) noexcept
{
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return false;
    }
    // pageOptGroup_ is opened read-only during initialization, so we need to pre-set the write environment
    if (false == MoveCachePageToWalog(pageOptGroup_.GetCachePage())) {
        PER_OnOptFailed(EErrorPHKV::kPageMoveToWalog);
        return false;
    }
    G_IncVersionSystemElse(pageOptGroup_, bClear);
    _OnSuccess();
    return true;
}
//********************************/
/// @brief Initialize PageGroup related attributes
/// @param pPageGroup
/// @param nHashPageID
void PManagerPage::_InitPageGroup(PCachePagePtr const& pPageGroup, uint32_t const nHashPageID) noexcept
{
    // Group page
    PER_Assert(nullptr != pPageGroup);
    PPageOptPageGroup& optPageGroup{pageOptGroup_};
    // Initialize Group page
    std::ignore = optPageGroup.InitCachePage(pPageGroup);
    PER_Assert(optPageGroup.InitGroupHead(PPerVersion{}, PPerVersion{},
                                          static_cast< uint32_t >(kvConfigMuster_.GetSpaceAmountMin()),
                                          static_cast< uint32_t >(kvConfigMuster_.GetSpaceAmountMax()), nHashPageID,
                                          static_cast< uint16_t >(kvConfigMuster_.GetPageInitLen())));
    // Add the first group's Group data: default BossPage page
    PER_Assert(optPageGroup.AddGroupElement((*pPageGroup)->GetMainPageID(),
                                            static_cast< uint16_t >(optPageGroup.GetGroupCapacity() - 1U),
                                            static_cast< uint16_t >(kvConfigMuster_.GetFreeListCapacity() - 1U)));
    PER_Assert(pageOptGroup_.IsAccessReady());
}
/// @brief Initialize PageHash related attributes
/// @param pNewPage
/// @param nDefKvPageID
void PManagerPage::_InitPageHash(PCachePagePtr const& pNewPage, uint32_t const nDefKvPageID) const noexcept
{
    PPageOptKeyHash pageOpt{pNewPage};
    PER_Assert(pageOpt.InitHashHead(static_cast< uint16_t >(kvConfigMuster_.GetHashMaxCapacity()),
                                    static_cast< uint16_t >(kvConfigMuster_.GetHashInitCount()), nDefKvPageID,
                                    static_cast< uint8_t >(kvConfigMuster_.GetHashType()),
                                    static_cast< uint8_t >(kvConfigMuster_.GetCompressType())));
}
/// @brief Initialize PageBoss related attributes
/// @param pNewPage
/// @param nGroupID
void PManagerPage::_InitPageBoss(PCachePagePtr const& pNewPage, uint16_t const nGroupID) const noexcept
{
    PPageOptPageBoss pageOpt{pNewPage};
    PER_Assert(pageOpt.InitBossHead(nGroupID, pageOptGroup_.GetGroupCapacity(),
                                    static_cast< uint16_t >(kvConfigMuster_.GetFreeListCapacity()),
                                    static_cast< uint16_t >(kvConfigMuster_.GetMaxPageInGroup())));
}
/// @brief Update the free list information managed by the Boss page
/// @param pPageBoss
/// @param nNewPageID
/// @param nFreeLen
/// @return
bool PManagerPage::_UpdateBossPage(PCachePagePtr const& pPageBoss,
                                   uint32_t const nNewPageID,
                                   uint16_t const nFreeLen) noexcept
{
    PER_Assert(pageOptGroup_.IsAccessReady());
    PCachePagePtr pPageBossWork{pPageBoss};
    if (nullptr == pPageBossWork) {
        pPageBossWork = _findPageBoss(nNewPageID, true);
    }
    PER_Assert(nullptr != pPageBossWork);
    PPageOptPageBoss pageOptBoss{pPageBossWork, 0U};
    PER_Assert(pageOptBoss.IsAccessReady());
    if (false == pageOptBoss.UpdateFreePage(pageOptGroup_.TransBossPageIndex(nNewPageID), nFreeLen)) {
        PER_OnOptFailed(pageOptBoss.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
//***************/
/// @brief Generate a smart pointer of PCachePage_Proxy from PMemoryPage
/// @param pMemoryPage
/// @return
PCachePagePtr PManagerPage::_makeProxyPagePtr(PMemoryPage* const pMemoryPage) const noexcept
{
    return MakeProxyPagePtr(pMemoryPage, &managerCache_);
}
/// @brief Create a new page: generate a new Page page in the Walog library, using the latest PageID from the Main library
/// @param bCheckSpace
/// @return
PCachePagePtr PManagerPage::_newMainPage(bool const bCheckSpace) noexcept
{
    if (bCheckSpace) {
        if (false == CheckFileSpace(nBakFileLen_)) {
            PER_OnOptFailed(EErrorPHKV::kFileSpaceMax);
            return PCachePagePtr{nullptr};
        }
    }
    // All newly created pages default to Walog
    PMemoryPage* pNewCache{nullptr};
    pNewCache = managerCache_.NewFilePage();
    if (nullptr == pNewCache) {
        PER_OnOptFailed(EErrorPHKV::kPageNewPage);
        return PCachePagePtr{nullptr};
    }
    // PageID uses the latest ID from the Main library
    uint32_t nMainPageID{1U};
    if (pageOptGroup_.IsAccessReady()) {
        nMainPageID = pageOptGroup_.GetNewPageID();
    }
    uint32_t nWalogPageID{0U};
    nWalogPageID = managerWalogID_.RegMainPage(nMainPageID);
    pNewCache->SetMainPageID(nMainPageID);
    pNewCache->SetWalogPageID(nWalogPageID);
    pNewCache->AddWriteRef(true);
    _LogDebug("[KV_Page].NewMainPage", ":", "MainPageID", static_cast< int32_t >(nMainPageID), "WalogPageID",
              static_cast< int32_t >(nWalogPageID));
    // When initializing the first page, pageOptGroup_ is not yet ready
    if (pageOptGroup_.IsAccessReady()) {
        // Increment PageID
        pageOptGroup_.IncreasePageTotal();
        if (false == MoveCachePageToWalog(pageOptGroup_.GetCachePage())) {
            PER_OnOptFailed(EErrorPHKV::kPageNewPage);
            return PCachePagePtr{nullptr};
        }
    }
    _OnSuccess();
    return _makeProxyPagePtr(pNewCache);
}
/// @brief Prepare a new page: generate a new Page page in the Walog library, using the latest PageID from the Main library
/// @param bCheckSpace
/// @return
PCachePagePtr PManagerPage::_prepareNewMainPage(bool const bCheckSpace) noexcept
{
    PCachePagePtr pNewPage{_newMainPage(bCheckSpace)};
    if (nullptr == pNewPage) {
        return PCachePagePtr{nullptr};
    }
    // When adding a new page, check whether the PageBoss page needs to be expanded
    if (pageOptGroup_.IsAccessReady()) {
        uint32_t const nMainPageID{(*pNewPage)->GetMainPageID()};
        if (false == PPageOptBase::IsValidPageID(pageOptGroup_.GetBossPageID(nMainPageID))) {
            // Treat the newly created page as a new Boss page
            if (false == _onAddNewBossPage(pNewPage)) {
                return PCachePagePtr{nullptr};
            }
            PPageOptPageBoss optBoss{pNewPage};
            if (false == optBoss.CheckBossPageCapacity()) {
                return PCachePagePtr{nullptr};
            }
            // Create a new page again
            pNewPage = _newMainPage(bCheckSpace);
            if (nullptr == pNewPage) {
                return PCachePagePtr{nullptr};
            }
        }
        std::ignore = pageOptGroup_.AddFreeCount(nMainPageID, -1);
    }
    _OnSuccess();
    return pNewPage;
}
/// @brief Generate initialization page
/// @param pBlendPage
/// @param bSoloPage
/// @return
PCachePagePtr PManagerPage::_newInitPage(PCachePagePtr const& pBlendPage, bool const bSoloPage) noexcept
{
    PER_Assert(!((nullptr == pBlendPage) && (false == bSoloPage)));
    // Decide whether to reuse Group page based on configuration: Boss page
    if (false == bSoloPage) {
        return PCachePagePtr{pBlendPage};
    }
    bool bCheckSpace{false};
    if (nullptr != pBlendPage) {  // pBlendPage being nullptr indicates this is to create the first page
        bCheckSpace = true;
    }
    return _prepareNewMainPage(bCheckSpace);
}
/// @brief Get the cache for the ExtString page
/// @return
PCachePagePtr PManagerPage::_getExtStringPage() const noexcept { return LoadCacheHash(); }
/// @brief Add a new group (including the default Boss page of the new group)
/// @param pNewBoss
/// @return
bool PManagerPage::_onAddNewBossPage(PCachePagePtr const& pNewBoss) noexcept
{
    if (nullptr == pNewBoss) {
        return false;
    }
    _InitPageBoss(pNewBoss, pageOptGroup_.GetGroupCount());
    // Check whether the Group page can continue adding GroupElement;
    // This design does not allow Group data to be mixed with other Blocks when adding a new group. That is, before the capacity of the first group exceeds half, there is a need to force Group, KeyHash, Boss, etc. to be made into independent pages.
    // As of 2021-06-23, the design does not support cross-page Group data. Thus, a 4K page KV library supports at most (4096-24-8-48)/4 ≈ 1000 groups, with a total maximum capacity slightly less than 4G.
    uint32_t const nBossPageID{(*pNewBoss)->GetMainPageID()};
    if (false
        == pageOptGroup_.AddGroupElement(nBossPageID, static_cast< uint16_t >(pageOptGroup_.GetGroupCapacity() - 1U),
                                         static_cast< uint16_t >(kvConfigMuster_.GetFreeListCapacity() - 1U))) {
        if (pageOptGroup_.GetLastError() == EErrorPHKV::kPageLackSpace) {
            _LogFatal("[KV_Manager].AddNewPage_Boss", "Failed to add new page", ": nBossPageID",
                      static_cast< int32_t >(nBossPageID));
            _LogFatal("[KV_Manager].AddNewPage_Boss", "Maximum number of pages supported by this system reached",
                      "GroupID", pageOptGroup_.TransGroupID(nBossPageID));
            // PER_Assert(false);
        }
        PER_OnOptFailed(pageOptGroup_.GetLastError());
        return false;
    }
    _LogInfo("[KV_Manager].AddNewPage_Boss", "", ": nBossPageID", static_cast< int32_t >(nBossPageID), "GroupID",
             pageOptGroup_.TransGroupID(nBossPageID));
    _OnSuccess();
    return true;
}
/// @brief Get the corresponding BossPage using the current PageID
/// @param nCurPageID
/// @param bForWrite
/// @return
PCachePagePtr PManagerPage::_findPageBoss(uint32_t const nCurPageID, bool const bForWrite) noexcept
{
    PCachePagePtr pPageBoss{nullptr};
    uint32_t const nFindBossID{pageOptGroup_.FindBossPageID(nCurPageID)};
    if (false
        == PPageOptBase::IsValidPageID(
            nFindBossID)) {  // Trigger adding a new group: generate the Boss page of the new group
        if (pageOptGroup_.GetGroupCount() >= pageOptGroup_.GetGroupCapacity()) {
            PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
            return pPageBoss;
        }
        pPageBoss   = _prepareNewMainPage(true);
        std::ignore = _onAddNewBossPage(pPageBoss);
    } else {
        pPageBoss = LoadCachePage(nFindBossID, bForWrite);
    }
    return pPageBoss;
}
/// @brief Find a free page that meets the capacity requirement
/// @param nCurPageID
/// @param nNeedSpace
/// @return
PCachePagePtr PManagerPage::_findExistFreePage(uint32_t const nCurPageID, uint16_t const nNeedSpace) noexcept
{
    if (false == managerCache_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyCache);
        return PCachePagePtr{nullptr};
    }
    if (false == pageOptGroup_.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageGroup);
        return PCachePagePtr{nullptr};
    }
    PCachePagePtr pPageBoss{nullptr};
    pPageBoss = _findPageBoss(nCurPageID, true);
    if (nullptr == pPageBoss) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return pPageBoss;
    }
    // Go to the Boss page to find the free page for this group
    PPageOptPageBoss optBossPage{pPageBoss, 0U};
    if (false == optBossPage.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageBoss);
        return PCachePagePtr{nullptr};
    }
    // First look within the pages managed by the BossPage; if not found, allocate a new page from the Group
    PCachePagePtr pFreePage{nullptr};
    uint16_t const nFindFreePageID{optBossPage.FindFreePageID(nNeedSpace)};
    if (PPageOptBase::IsValidPageID(static_cast< uint32_t >(nFindFreePageID))) {
        uint32_t const nExistPageID{pageOptGroup_.TransPageID(optBossPage.GetGroupID(), nFindFreePageID)};
        pFreePage = LoadCachePage(nExistPageID, true);
        _LogDebug("[KV_Page].LoadExistPage", ":", "MainPageID", static_cast< int32_t >((*pFreePage)->GetMainPageID()),
                  "WalogPageID", static_cast< int32_t >((*pFreePage)->GetWalogPageID()), "NeedSpace",
                  static_cast< int32_t >(nNeedSpace));
    }
    _OnSuccess();
    return pFreePage;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
