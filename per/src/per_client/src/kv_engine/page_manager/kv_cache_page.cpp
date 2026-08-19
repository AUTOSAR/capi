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
/// @file       kv_cache_page.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Cache data class
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Page Cache Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Cache data
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-09 | 1.0.0   | hanjingjing  | Initial version created                   |
/// | 2021-07-13 | 1.0.0   | hanjingjing  | Modified valid PageID to start from 1, 0 is invalid |
///
/// ================================================================

#include "ara/per/internal/isoftkv/kv_cache_page.h"

#include "ara/per/internal/isoftkv/manager_cache.h"

//********************************/
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param nCacheID
/// @param nCacheLen
PMemoryPage::PMemoryPage(uint32_t const nCacheID, uint32_t const nCacheLen) noexcept
    : ePageSource_{ECacheSource::kDefault}
    , nMainPageID_{0U}
    , nWalogPageID_{0U}
    , nWriteRef_{0}
    , nOptIndex_{0U}
    , nOwnThreadID_{0U}
    , nReddIndex_{0}
    , tmActiveOpt_{0}
    , nCacheID_{nCacheID}
    , naiMapBuff_{nCacheLen}
{
}
/// @brief
/// @param pPageOpt
void PMemoryPage::AttachOpt(PPageOptBase* const pPageOpt) noexcept
{
    vecAttachOpt_.push_back(pPageOpt);
    PER_Assert(vecAttachOpt_.size() <= 10U);
}
/// @brief
/// @param pPageOpt
void PMemoryPage::DetachOpt(PPageOptBase const* const pPageOpt) noexcept
{
    PER_Assert(vecAttachOpt_.size() <= 10U);
    // 2023-02-17 Changed reverse-order pop to forward-order // Using reverse-order lookup and pop actually does not cause errors even with forward order
    ara::core::Vector< PPageOptBase* >::const_iterator const itFind{
        std::find(vecAttachOpt_.cbegin(), vecAttachOpt_.cend(), pPageOpt)};
    if (itFind != vecAttachOpt_.end()) {
        std::ignore = vecAttachOpt_.erase(itFind);
    }
}
/// @brief
void PMemoryPage::ResetCacheData() noexcept
{
    ePageSource_  = ECacheSource::kDefault;
    nMainPageID_  = kInvalidPageID;
    nWalogPageID_ = kInvalidPageID;
    nWriteRef_    = 0;
    nOptIndex_    = 0U;
    nReddIndex_   = 0;
    tmActiveOpt_  = 0;
    vecAttachOpt_.clear();
    naiMapBuff_.ResetBuffData(0U);
}
//***************/

/// @brief Get idle time
/// @param tmNow
/// @return
int64_t PMemoryPage::GetIdleTime(time_t const tmNow) const noexcept
{
    if (IsHaveOpt()) {
        return 0;
    }
    return tmNow - tmActiveOpt_;
}
/// @brief Get idle time
/// @param tmNow
/// @return
int64_t PMemoryPage::GetIdleTime(TimePoint const tmNow) const noexcept
{
    if (IsHaveOpt()) {
        return 0;
    }
    return tmNow.time_since_epoch().count() - tmActiveOpt_;
}
/// @brief
/// @param ePageSource
/// @param nPageID
void PMemoryPage::InitPageData(ECacheSource const ePageSource, uint32_t const nPageID) noexcept
{
    SetPageSource(ePageSource);
    switch (ePageSource) {
        case ECacheSource::kMain: {
            SetMainPageID(nPageID);
            SetWalogPageID(kInvalidPageID);
        } break;
        case ECacheSource::kWalog: {
            SetMainPageID(kInvalidPageID);
            SetWalogPageID(nPageID);
        } break;
        default: {
            SetMainPageID(kInvalidPageID);
            SetWalogPageID(kInvalidPageID);
        } break;
    }
}
//********************************/
/// @brief
/// @param nCacheLen
/// @param pCacheManager
ICachePage::ICachePage(uint32_t const nCacheLen, PManagerCache* const pCacheManager) noexcept
    : pMemoryPage_{new PMemoryPage(0U, nCacheLen)}, pCacheManager_{pCacheManager}, bStackMemory_{true}
{
    // PageID directly using 0 indicates a stack-level cache page
}
/// @brief
/// @param pMemoryPage
/// @param pCacheManager
ICachePage::ICachePage(PMemoryPage* const pMemoryPage, PManagerCache* const pCacheManager) noexcept
    : pMemoryPage_{pMemoryPage}, pCacheManager_{pCacheManager}, bStackMemory_{false}
{
}
/// @brief
ICachePage::~ICachePage() noexcept
{
    if (nullptr != pMemoryPage_) {
        if (bStackMemory_) {
            delete pMemoryPage_;
        }
        pMemoryPage_ = nullptr;
    }
}
/// @brief Provide a KvName for Log output
/// @return
ara::core::StringView ICachePage::GetLogKvName() const noexcept
{
    PER_Assert(nullptr != pCacheManager_);
    return std::move(pCacheManager_->GetLogKvName());
}
/// @brief Send thread synchronization signal
/// @return
bool ICachePage::SendSyncSignal() noexcept
{
    if (nullptr == pMemoryPage_) {
        return false;
    }
    if (pMemoryPage_->IsHaveOpt()) {
        return false;
    }
    pMemoryPage_->SetOwnThreadID(0U);
    if (nullptr == pCacheManager_) {
        return false;
    }
    std::ignore = pCacheManager_->SignalCondFindCache();  // Send signal that a free Cache is available
    return true;
}
/// @brief Determine whether the memory pages of two cache pages are identical
/// @param pCachPage
/// @return
bool ICachePage::IsSamePage(ICachePage const* const pCachPage) const noexcept
{
    if (nullptr == pCachPage) {
        return false;
    }
    return pMemoryPage_ == pCachPage->GetMemoryPage();
}
//***************/
/// @brief
/// @param pMemoryPage
/// @param pCacheManager
PCachePage_Proxy::PCachePage_Proxy(PMemoryPage* const pMemoryPage, PManagerCache* const pCacheManager) noexcept
    : ICachePage{pMemoryPage, pCacheManager}
{
}
/// @brief
PCachePage_Proxy::~PCachePage_Proxy() noexcept { std::ignore = SendSyncSignal(); }
//***************/
/// @brief
/// @param nCacheLen
/// @param pCacheManager
PCachePage_Stack::PCachePage_Stack(uint32_t const nCacheLen, PManagerCache* const pCacheManager) noexcept
    : ICachePage{nCacheLen, pCacheManager}
{
}
//***************/
/// @brief Overload comparison function for PCachePagePtr
/// @param pageA
/// @param pageB
/// @return
bool operator==(PCachePagePtr const& pageA, PCachePagePtr const& pageB) noexcept
{
    return pageA->IsSamePage(pageB.get());
}
/// @brief Overload comparison function for PCachePagePtr
/// @param pageA
/// @param pageB
/// @return
bool operator!=(PCachePagePtr const& pageA, PCachePagePtr const& pageB) noexcept
{
    return !(pageA->IsSamePage(pageB.get()));
}
/// @brief Generate a smart pointer of PCachePage_Proxy from PMemoryPage
/// @param pMemoryPage
/// @param pCacheManager
/// @return
PCachePagePtr MakeProxyPagePtr(PMemoryPage* const pMemoryPage, PManagerCache* const pCacheManager) noexcept
{
    if (nullptr == pMemoryPage) {
        return PCachePagePtr{nullptr};
    }
    std::shared_ptr< PCachePage_Proxy > pProxyPage{std::make_shared< PCachePage_Proxy >(pMemoryPage, pCacheManager)};
    return PCachePagePtr{std::move(pProxyPage)};
}
/// @brief Generate a smart pointer of PCachePage_Stack
/// @param nCacheLen
/// @param pCacheManager
/// @return
PCachePagePtr MakeStackPagePtr(uint32_t const nCacheLen, PManagerCache* const pCacheManager) noexcept
{
    if (nCacheLen <= 0U) {
        return PCachePagePtr{nullptr};
    }
    std::shared_ptr< PCachePage_Stack > pStackPage{std::make_shared< PCachePage_Stack >(nCacheLen, pCacheManager)};
    return PCachePagePtr{std::move(pStackPage)};
}
//********************************/

/// @brief
/// @param pCachePage
/// @param pPageOpt
PCacheAutoLock::PCacheAutoLock(PMemoryPage* const pCachePage, PPageOptBase* const pPageOpt) noexcept
    : pCachePage_{pCachePage}, pPageOpt_{pPageOpt}
{
    if (nullptr != pCachePage_) {
        pCachePage_->AttachOpt(pPageOpt_);
    }
}
/// @brief
PCacheAutoLock::~PCacheAutoLock() noexcept
{
    if (nullptr != pCachePage_) {
        pCachePage_->DetachOpt(pPageOpt_);
    }
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
