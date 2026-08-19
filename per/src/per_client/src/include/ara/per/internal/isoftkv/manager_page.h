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
/// @file       manager_page.h
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

#ifndef ARA_PER_PHKV_MANAGER_PAGE_H_
#define ARA_PER_PHKV_MANAGER_PAGE_H_

#include <functional>

#include "ara/core/vector.h"
#include "ara/per/internal/isoftkv/config_muster.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/manager_cache.h"
#include "ara/per/internal/isoftkv/manager_walog_id.h"
#include "ara/per/internal/isoftkv/page_opt_page_group.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Logical page manager used by Puhua KvStorage
class PManagerPage : public PKvObject
{
private:
    /// @brief Configuration
    PConfigMuster_Kv const &kvConfigMuster_;
    /// @brief Cache manager
    PManagerCache &managerCache_;
    /// @brief Mapping table from Main library PageID to Walog library PageID
    mutable PManagerWalogID managerWalogID_;
    /// @brief PageGroup operation object
    PPageOptPageGroup pageOptGroup_{nullptr, 0U};
    /// @brief Internally recorded size of the backup library file
    int64_t nBakFileLen_{0};

public:
    /// @brief
    PManagerPage() = delete;
    /// @brief
    /// @param a
    PManagerPage(PManagerPage const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerPage &operator=(PManagerPage const &a) = delete;
    /// @brief
    /// @param a
    PManagerPage(PManagerPage &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerPage &operator=(PManagerPage &&a) = delete;
    /// @brief
    /// @param kvConfigMuster
    /// @param managerCache
    explicit PManagerPage(PConfigMuster_Kv const &kvConfigMuster, PManagerCache &managerCache) noexcept;
    /// @brief
    ~PManagerPage() noexcept override = default;
    /// @brief Check if the cache system is ready
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the KV library name displayed in the log system
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;
    /// @brief Set the internally recorded size of the backup library file
    /// @param nBakFileLen
    inline void SetBakFileLen(int64_t const nBakFileLen) noexcept { nBakFileLen_ = nBakFileLen; }

public:
    /// @brief Initialize pageOptGroup_
    /// @return
    bool InitPageOptGroup() noexcept;
    /// @brief Initialize the Walog library
    /// @return
    bool InitWalogDB() noexcept;
    /// @brief Release the cache bound to pageOptGroup_
    /// @return
    bool ReleasePageOpt() noexcept;
    /// @brief Initialize the mapping table for PManagerWalogID using the Walog library
    /// @param vecWalogID
    /// @return
    int32_t ResetWalogIdByDbFile(ara::core::Vector< uint32_t > const &vecWalogID) noexcept;
    /// @brief Initialize an empty system
    /// @param bSoloHash
    /// @param bSoloBoss
    /// @param bSoloKv
    /// @return
    PCachePagePtr InitEmptySystem(bool const bSoloHash, bool const bSoloBoss, bool const bSoloKv) noexcept;
    /// @brief Find a free page; if none exists, create a new one; return nullptr if capacity is exceeded
    /// @param ePageType
    /// @param nCurPageID
    /// @param nNeedSpace
    /// @return
    PCachePagePtr FindFreePage(EBlockType const ePageType,
                               uint32_t const nCurPageID,
                               uint16_t const nNeedSpace) noexcept;
    /// @brief First search the Walog library, then the Main library
    /// @param nMainPageID
    /// @param bForWrite
    /// @return
    PCachePagePtr LoadCachePage(uint32_t const nMainPageID, bool const bForWrite) const noexcept;
    /// @brief Load HashPage
    /// @return
    PCachePagePtr LoadCacheHash() const noexcept;
    /// @brief Load a page from the Walog library and set the corresponding MainPageID
    /// @param nWalogPageID
    /// @return
    PCachePagePtr LoadWalogFilePage(uint32_t const nWalogPageID) noexcept;
    /// @brief Prepare an independent KV page, create a new one if not found
    /// @param nCurPageID
    /// @param nKvStoreID
    /// @return
    PCachePagePtr PreparePageSoloKv(uint32_t const nCurPageID, uint32_t const nKvStoreID) noexcept;
    /// @brief Find a suitable Value page
    /// @param nCurPageID
    /// @param nValueLen
    /// @return
    PCachePagePtr PreparePageValue(uint32_t const nCurPageID, uint16_t const nValueLen) noexcept;
    /// @brief Check whether the PageBoss page needs to be made independent
    /// @param nCurPageID
    /// @return
    bool IsNeedSoloPageBoss(uint32_t const nCurPageID) noexcept;
    /// @brief Make the PageBoss page independent
    /// @param nCurPageID
    /// @return
    PCachePagePtr SoloPageBoss(uint32_t const nCurPageID) noexcept;
    /// @brief Change a Hash page from a mixed page to an independent page, and return the new page
    /// @param pPageHashOld
    /// @return
    PCachePagePtr SoloPageHash(PCachePagePtr const &pPageHashOld) noexcept;
    /// @brief Move the ExtString page from the mixed page to a new page, and return the new page
    /// @param pPageOld
    /// @param pPageNew
    /// @return
    PCachePagePtr SoloPage_ExtString(PCachePagePtr const &pPageHashOld, PCachePagePtr const &pPageHashNew) noexcept;
    /// @brief Move the free index list of a given page: applicable only for KV and Value page calls
    /// @param pageOptWork
    /// @return
    bool UpdateFreeList(PPageOptBase const &pageOptWork) noexcept;
    /// @brief Change the source of the cache page to the Walog library
    /// @param pCachePagePtr
    /// @return
    bool MoveCachePageToWalog(PCachePagePtr const &pCachePagePtr) const noexcept;
    /// @brief Main.PageID>>=>>Walog.PageID
    /// @param nMainPageID
    /// @return
    uint32_t MainToWalogPageID(uint32_t const nMainPageID) const noexcept;
    /// @brief Walog.PageID>>=>>Main.PageID
    /// @param nWalogPageID
    /// @return
    uint32_t WalogToMainPageID(uint32_t const nWalogPageID) const noexcept;
    /// @brief Get the number of pages in the Walog library
    /// @return
    uint32_t GetWalogPageCount() const noexcept;
    /// @brief Check storage space usage to determine if a new page can be created
    /// @param nBakFileLen
    /// @return
    bool CheckFileSpace(int64_t const nBakFileLen) const noexcept;
    /// @brief Get the PER format version number information saved in the library: persistent data
    /// @return
    PPerVersion const &GetPerVersion_Per() const noexcept;
    /// @brief Get the PER format version number information saved in the library: KvSystem
    /// @return
    PPerVersion const &GetPerVersion_Sys() const noexcept;
    /// @brief Get the string format version saved in the Kv library: KvSystem
    /// @return
    ara::core::String GetStringVersion_Sys() const noexcept;
    /// @brief Get the string format version saved in the Kv library: Per persistent data
    /// @return
    ara::core::String GetStringVersion_Per() const noexcept;
    /// @brief Get the string format version saved in the Kv library: App
    /// @return
    ara::core::String GetStringVersion_App() const noexcept;
    /// @brief Update the APP/PER version numbers within the system
    /// @param stVersionApp APP version number
    /// @param stVersionPer PER version number
    /// @return Success or failure
    bool UpdateVersion(ara::core::String const &stVersionApp, ara::core::String const &stVersionPer) noexcept;
    /// @brief Update the PER version number within the system
    /// @param version
    /// @return
    bool UpdateVersionPer(PPerVersion const &version) noexcept;
    /// @brief Update the 4th field of the KvSystem version number
    /// @param bClear Whether to clear
    /// @return
    bool IncVersionSystemElse(bool const bClear = false) noexcept;

protected:
    /// @brief Initialize PageGroup related attributes
    /// @param pPageGroup
    /// @param nHashPageID
    void _InitPageGroup(PCachePagePtr const &pPageGroup, uint32_t const nHashPageID) noexcept;
    /// @brief Initialize PageHash related attributes
    /// @param pNewPage
    /// @param nDefKvPageID
    void _InitPageHash(PCachePagePtr const &pNewPage, uint32_t const nDefKvPageID) const noexcept;
    /// @brief Initialize PageBoss related attributes
    /// @param pNewPage
    /// @param nGroupID
    void _InitPageBoss(PCachePagePtr const &pNewPage, uint16_t const nGroupID) const noexcept;
    /// @brief Update the free list information managed by the Boss page
    /// @param pPageBoss
    /// @param nNewPageID
    /// @param nFreeLen
    /// @return
    bool _UpdateBossPage(PCachePagePtr const &pPageBoss, uint32_t const nNewPageID, uint16_t const nFreeLen) noexcept;

private:
    /// @brief Generate a smart pointer of PCachePage_Proxy from PMemoryPage
    /// @param pMemoryPage
    /// @return
    PCachePagePtr _makeProxyPagePtr(PMemoryPage *const pMemoryPage) const noexcept;
    /// @brief Create a new page: generate a new Page page in the Walog library, using the latest PageID from the Main library
    /// @param bCheckSpace
    /// @return
    PCachePagePtr _newMainPage(bool const bCheckSpace) noexcept;
    /// @brief Prepare a new page: generate a new Page page in the Walog library, using the latest PageID from the Main library
    /// @param bCheckSpace
    /// @return
    PCachePagePtr _prepareNewMainPage(bool const bCheckSpace) noexcept;
    /// @brief Generate initialization page
    /// @param pBlendPage
    /// @param bSoloPage
    /// @return
    PCachePagePtr _newInitPage(PCachePagePtr const &pBlendPage, bool const bSoloPage) noexcept;
    /// @brief Get the cache for the ExtString page
    /// @return
    PCachePagePtr _getExtStringPage() const noexcept;
    /// @brief Add a new group (including the default Boss page of the new group), returns the Boss page number
    /// @param pNewBoss
    /// @return
    bool _onAddNewBossPage(PCachePagePtr const &pNewBoss) noexcept;
    /// @brief Get the corresponding BossPage using the current PageID
    /// @param nCurPageID
    /// @param bForWrite
    /// @return
    PCachePagePtr _findPageBoss(uint32_t const nCurPageID, bool const bForWrite) noexcept;
    /// @brief Find a free page that meets the capacity requirement
    /// @param nCurPageID
    /// @param nNeedSpace
    /// @return
    PCachePagePtr _findExistFreePage(uint32_t const nCurPageID, uint16_t const nNeedSpace) noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
