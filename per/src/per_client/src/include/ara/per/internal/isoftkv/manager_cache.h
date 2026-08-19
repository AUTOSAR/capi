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
/// @file       manager_cache.h
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

#ifndef ARA_PER_PHKV_KV_CACHE_MANAGER_H_
#define ARA_PER_PHKV_KV_CACHE_MANAGER_H_

#include <ara/core/array.h>
#include <ara/core/map.h>
#include <ara/core/vector.h>

#include <functional>
#include <memory>

#include "ara/per/internal/common/isoft_check_redd.h"
#include "ara/per/internal/common/isoft_file_opt_page.h"
#include "ara/per/internal/common/isoft_thread_lock.h"
#include "ara/per/internal/isoftkv/config_muster.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_base.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Page cache manager used by Puhua KvStorage
class PManagerCache : public PKvObject
{
private:
    /// @brief
    class PLoadReddPage;
    /// @brief
    PConfigMuster_Kv const &kvConfigMuster_;
    /// @brief Page size
    uint16_t nPageLen_;

private:
    /// @brief Main database file operation class
    ara::core::Map< int32_t, std::unique_ptr< PFileOpt_Page > > mapFileOptMain_;
    /// @brief Log library file operation class
    mutable PFileOpt_Page fileOptWalog_;

private:
    /// @brief PMemoryPage operation sequence number (0 to 0x10000000)
    uint32_t nOptIndex_{0U};
    /// @brief PMemoryPage storage area
    ara::core::Vector< PMemoryPage * > vecCachePage_;
    /// @brief Number of unbound CachePages within vecCachePage_
    int32_t nFreeCacheCount_{0};
    /// @brief Lock to prevent Cache exhaustion
    PThreadLock threadLockCache_;
    /// @brief Condition signal lock for multithreaded contention for vecCachePage_
    PThreadCond threadCondCache_;
    /// @brief List of PageIDs with detected redundancy errors, cleared only after recovery attempts have been made
    PCheckRedd_Kv checkReddKv_;

private:
    /// @brief Cache structure for redundant reads
    ara::core::Map< uint32_t, int32_t > mapReddFindIndex_;

public:
    /// @brief
    PManagerCache() = delete;
    /// @brief
    /// @param a
    PManagerCache(PManagerCache const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerCache &operator=(PManagerCache const &a) = delete;
    /// @brief
    /// @param a
    PManagerCache(PManagerCache &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerCache &operator=(PManagerCache &&a) = delete;
    /// @brief Constructor
    /// @param kvConfigMuster
    explicit PManagerCache(PConfigMuster_Kv const &kvConfigMuster) noexcept;
    /// @brief Destructor
    ~PManagerCache() noexcept override;
    /// @brief Check if the cache system is ready
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the Kv library name displayed in logs
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;
    void Debug_19(ara::core::StringView const &stMsg, uint32_t const nPageID) const noexcept
    {
        constexpr uint32_t kInt32_21U{21U};
        constexpr uint32_t kInt32_25U{25U};
        constexpr uint32_t kInt32_27U{27U};
        if ((false == vecCachePage_.empty()) && (nPageID == kInt32_25U)) {
            PMemoryPage *pFind19 = nullptr;
            for (auto const &pCachePage : vecCachePage_) {
                if ((nullptr != pCachePage) && (pCachePage->GetMainPageID() == kInt32_21U)) {
                    pFind19 = pCachePage;
                }
            }
            if (nullptr != pFind19) {
                PPageHeadData *pPageHead{T_TransPtr< PPageHeadData >(pFind19->GetCache())};
                if (kInt32_27U == pFind19->GetCacheID()) {
                    if (pPageHead->cPageType == 0) {
                        int n = 0;
                        n += 1;
                    }
                }
                printf("Debug_19 %s : nPageID = %u, PageType = %d\n", stMsg.data(), nPageID, pPageHead->cPageType);
            }
        }
    }

public:
    /// @brief Get the page length of a single page
    /// @return
    inline uint16_t GetPageLen() const noexcept { return nPageLen_; }
    /// @brief Set the page length of a single page
    /// @param nPageLen
    inline void SetPageLen(uint16_t const nPageLen) noexcept { nPageLen_ = nPageLen; }
    /// @brief Send a synchronization signal for finding a cache page
    /// @return
    bool SignalCondFindCache() noexcept;
    /// @brief Initialize the in-memory cache pool
    /// @param nMaxCacheCount
    /// @param nPageLen
    /// @return
    bool InitCachePool(uint32_t const nMaxCacheCount, uint16_t const nPageLen) noexcept;
    /// @brief Clear all cache pages
    /// @return
    bool ClearCachePool() noexcept;
    /// @brief Get the related full file path according to parameter settings
    /// @param stWorkPath
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    ara::core::String GetFileName(ara::core::StringView const &stWorkPath,
                                  ECacheSource const eCacheSource,
                                  int32_t const nReddIndex = 0) const noexcept;
    /// @brief Get the related full file path according to parameter settings
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    ara::core::String GetFileName(ECacheSource const &eCacheSource, int32_t const nReddIndex = 0) const noexcept;
    /// @brief Add a detected redundancy recovery request data
    /// @param nPageID
    /// @param nSelectIndex
    /// @param vecNeedRecover
    void AddReddCheck(uint32_t const nPageID,
                      uint8_t const nSelectIndex,
                      ara::core::Vector< uint8_t > const &vecNeedRecover) noexcept;
    /// @brief Delete a redundancy recovery request
    /// @param nPageID
    void DelReddCheck(uint32_t const nPageID) noexcept;
    /// @brief Check if there is a redundancy recovery request
    /// @return Redundancy data that needs recovery
    PReddCheckData_Kv FindRecoverData() const noexcept;

public:
    /// @brief Create a page of data
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    PMemoryPage *NewFilePage(ECacheSource const eCacheSource = ECacheSource::kWalog,
                             int32_t const nReddIndex        = 0) noexcept;
    /// @brief Load a page of data
    /// @param eCacheSource
    /// @param nPageID
    /// @return
    PMemoryPage *LoadFilePage(ECacheSource const eCacheSource, uint32_t const nPageID) noexcept;
    /// @brief Find the page corresponding to PageID among all working pages
    /// @param nMainPageID
    /// @return
    PMemoryPage *FindCachePage(uint32_t const nMainPageID) noexcept;
    /// @brief Read the information at the beginning of the file
    /// @return
    PPageHeadData ReadFileHead() noexcept;
    /// @brief Attempt to restore redundant files
    /// @return
    bool RecoverReddFile(ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept;
    /// @brief Reset all files: Main+Walog+Redd
    /// @return
    bool DbFileResize() noexcept;
    /// @brief Close files
    /// @return
    bool DbFileClose() noexcept;
    /// @brief Delete files
    /// @return
    bool DbFileDelete() noexcept;
    /// @brief Get the total disk space occupied by the entire KV library: includes Main, Walog, Redd, etc.
    /// @return
    uint64_t GetSpaceSize() noexcept;
    /// @brief Get the number of pages in the main library
    /// @return
    uint32_t GetMainPageTotal() const noexcept;
    /// @brief Get the number of pages in the Walog library
    /// @return
    uint32_t GetWalogPageTotal() const noexcept;
    /// @brief Get the number of cache pages that need to be written to disk
    /// @return
    int32_t GetNeedWriteCachePageCount() const noexcept;
    /// @brief Get the size of the Main file, regardless of whether the KV library is valid (in case of redundant backup)
    /// @return Returns the size of the main file
    /// @code{.isoft}
    /// @needwork = dda
    /// @endcode
    int64_t GetMainFileSize() noexcept;
    // Operations on Walog library
public:
    /// @brief Initialize the Walog library file
    /// @return
    bool WalogInitFile() noexcept;
    /// @brief Read the end marker of the Walog library
    /// @param tailData
    /// @return
    bool WalogReadTail(PPageTailData &tailData) noexcept;
    /// @brief Set the end marker of the Walog library
    /// @param tailData
    /// @return
    bool WalogWriteTail(PPageTailData const &tailData) noexcept;
    /// @brief Change all Walog cache to Main cache
    /// @return
    bool WalogCacheToMain() noexcept;
    /// @brief Write all cache files to disk; unmodified Main pages will be discarded
    /// @param bForce
    /// @return Number of pages saved: -1 indicates error, 0 indicates no files need to be written to disk
    int32_t WalogSaveCachePage(bool const bForce) noexcept;
    /// @brief Reset all cache pages: old data will be lost
    /// @return
    bool WalogResetCache() noexcept;
    /// @brief Get the mapping from WalogPageID to MainPageID from the Walog file; the index number +1 is the WalogID
    /// @param vecMainPageID
    /// @return
    bool WalogReadAllMainPageID(ara::core::Vector< uint32_t > &vecMainPageID) noexcept;

public:
    /// @brief Get the existing Main library and the number of redundancies
    /// @return
    uint32_t GetExistMainCount() const noexcept;
    /// @brief Write a page of data to disk
    /// @param pMemoryPage
    /// @param nPageID
    /// @return
    int32_t SaveMainPage(PMemoryPage const *const pMemoryPage, uint32_t const nPageID) noexcept;
    /// @brief Restore a specific page of a redundant file
    /// @param nPageID Page number to restore
    /// @param nReddCountN Total number of redundancies N
    /// @param nReddCountM Valid number of redundancies M
    /// @return Number of the redundant copy that was executed
    ara::core::Vector< uint8_t > RecoverReddPage(
        uint32_t const nPageID,
        uint32_t const nReddCountN,
        uint32_t const nReddCountM) noexcept;  /// @brief Restore a specific page of a redundant file
    /// @param nPageID Page number to restore
    /// @param nSelectIndex Redundancy number of the correct data
    /// @param vecRecoverIndex Redundant page numbers of the erroneous data
    /// @return Success or failure
    bool RecoverReddPage(uint32_t const nPageID,
                         uint8_t const nSelectIndex,
                         ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept;

public:
    /// @brief Iterate over all caches
    void Debug_ScanCache() noexcept;

protected:
    /// @brief Iterate over each cache page
    /// @param pfun
    /// @return
    int32_t _ForEachCachePage(std::function< int32_t(PMemoryPage *const) > const &&pfun) noexcept;
    /// @brief Iterate over each file operation object
    /// @param bEnableCreate
    /// @param pfun
    /// @return
    bool _ForEachFileOpt(bool const bEnableCreate, std::function< bool(PFileOpt_Page *const) > const &pfun) noexcept;
    /// @brief Initialize the buffer
    /// @param nCacheCount
    /// @param nPageLen
    void _InitMemoryPagePool(uint32_t const nCacheCount, uint32_t const nPageLen) noexcept;
    /// @brief Check if all CachePages
    void _FreeMemoryPagePool() noexcept;
    /// @brief Write a page of data to disk
    /// @param pMemoryPage
    /// @param nPageID
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    bool _SaveFilePage(PMemoryPage const *const pMemoryPage,
                       uint32_t const nPageID,
                       ECacheSource const eCacheSource,
                       int32_t const nReddIndex = 0) noexcept;
    /// @brief Save a memory page to the Walog library, update the CRC checksum before saving
    /// @param pMemoryPage
    /// @return
    bool _SaveWalogPage(PMemoryPage *const pMemoryPage) noexcept;
    /// @brief Load a page of data from a specific file
    /// @param pCacheData
    /// @param nPageID
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    bool _ReadFilePage(PMemoryPage *const pCacheData,
                       uint32_t const nPageID,
                       ECacheSource const eCacheSource,
                       int32_t const nReddIndex = 0) noexcept;
    /// @brief Load a specific page from M/N redundant files
    /// @param pCacheData
    /// @param nPageID
    /// @return
    bool _ReadFilePageFromRedd(PMemoryPage *const pCacheData, uint32_t const nPageID) noexcept;
    /// @brief Initialize redundant file pointers
    void _NewMainFileOpt() noexcept;
    /// @brief Delete redundant file pointers
    void _DelMainFileOpt() noexcept;

protected:
    /// @brief Find a file operation class
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    PFileOpt_Page *_FindFileOpt(ECacheSource const eCacheSource, int32_t const nReddIndex) const noexcept;
    /// @brief Perform all necessary preparations for file operations
    /// @param nPageLen
    /// @param eCacheSource
    /// @param nReddIndex
    /// @return
    PFileOpt_Page *_PrepareFileOpt(uint16_t const nPageLen,
                                   ECacheSource const eCacheSource,
                                   int32_t const nReddIndex) const noexcept;
    /// @brief Find a free cache page: prevent contention in multithreaded scenarios
    /// @return
    PMemoryPage *_FindFreeCacheByThread() noexcept;
    /// @brief Find a free cache page
    /// @return
    PMemoryPage *_FindFreeCache() noexcept;
    /// @brief Save cold data to disk
    /// @param pColdPage
    /// @return
    bool _TryToSaveColdPage(PMemoryPage *const pColdPage) noexcept;

public:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
