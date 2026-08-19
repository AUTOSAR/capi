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
/// @file       kv_cache_page.h
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

#ifndef ARA_PER_PHKV_KV_CACHE_DATA_H_
#define ARA_PER_PHKV_KV_CACHE_DATA_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <chrono>
#include <memory>

#include "ara/per/internal/common/isoft_data_type.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief
class PPageOptBase;
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=ECacheSource
/// @endcode
enum class ECacheSource : uint32_t
{
    kDefault = 0,
    kMain,   // Main database (0 is main library, others are redundant libraries)
    kWalog,  // Walog log library
};
constexpr const char8_t *kFileExt_Main  = ".main";
constexpr const char8_t *kFileExt_Walog = ".walog";
constexpr const char8_t *kFileExt_Redd  = ".redd";
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=PMemoryPage
/// @endcode
class PMemoryPage final
{
public:
    /// @brief
    using TimePoint = std::chrono::steady_clock::time_point;

private:
    /// @brief Data source
    ECacheSource ePageSource_;
    /// @brief Main page number: -1 is invalid, 0 is valid
    uint32_t nMainPageID_;
    /// @brief Log page number: -1 is invalid, 0 is valid
    uint32_t nWalogPageID_;
    /// @brief Write data count identifier
    int32_t nWriteRef_;
    /// @brief Operation sequence number
    uint32_t nOptIndex_;
    /// @brief Associated thread number
    uint64_t nOwnThreadID_;
    /// @brief Redundant file index
    int32_t nReddIndex_;

private:
    /// @brief Time of last operation: CPU time should be used
    int64_t tmActiveOpt_;
    /// @brief Object currently operating on the data; if non-null, the buffer cannot be swapped out
    ara::core::Vector< PPageOptBase * > vecAttachOpt_;
    /// @brief CachePage unique identifier
    uint32_t nCacheID_;
    /// @brief Use nai_mmap to allocate page-aligned heap memory
    PNaiMapBuff naiMapBuff_;

public:
    /// @brief
    /// @param v
    inline void SetPageSource(ECacheSource const &v) noexcept { ePageSource_ = v; }
    /// @brief
    /// @return
    inline ECacheSource GetPageSource() const noexcept { return ePageSource_; }
    /// @brief
    /// @param v
    inline void SetMainPageID(uint32_t const &v) noexcept { nMainPageID_ = v; }
    /// @brief
    /// @return
    inline uint32_t GetMainPageID() const noexcept { return nMainPageID_; }
    /// @brief
    /// @param v
    inline void SetWalogPageID(uint32_t const &v) noexcept { nWalogPageID_ = v; }
    /// @brief
    /// @return
    inline uint32_t GetWalogPageID() const noexcept { return nWalogPageID_; }
    /// @brief
    /// @param v
    inline void SetWriteRef(int32_t const &v) noexcept { nWriteRef_ = v; }
    /// @brief
    /// @return
    inline int32_t GetWriteRef() const noexcept { return nWriteRef_; }
    /// @brief
    /// @param v
    inline void SetOptIndex(uint32_t const &v) noexcept { nOptIndex_ = v; }
    /// @brief
    /// @return
    inline uint32_t GetOptIndex() const noexcept { return nOptIndex_; }
    /// @brief
    /// @param v
    inline void SetOwnThreadID(uint64_t const &v) noexcept { nOwnThreadID_ = v; }
    /// @brief
    /// @return
    inline uint64_t GetOwnThreadID() const noexcept { return nOwnThreadID_; }
    /// @brief
    /// @param v
    inline void SetReddIndex(int32_t const &v) noexcept { nReddIndex_ = v; }
    /// @brief
    /// @return
    inline int32_t GetReddIndex() const noexcept { return nReddIndex_; }
    /// @brief
    /// @return
    inline uint32_t GetCacheID() const noexcept { return nCacheID_; }

public:
    /// @brief
    /// @param nCacheID
    /// @param nCacheLen
    PMemoryPage(uint32_t const nCacheID, uint32_t const nCacheLen) noexcept;
    /// @brief Default constructor
    PMemoryPage() noexcept = delete;
    /// @brief Destructor
    ~PMemoryPage() noexcept = default;
    /// @brief
    /// @param a
    /// @return
    PMemoryPage(PMemoryPage const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PMemoryPage &operator=(PMemoryPage const &a) = delete;
    /// @brief
    /// @param a
    PMemoryPage(PMemoryPage &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PMemoryPage &operator=(PMemoryPage &&a) = delete;

public:
    /// @brief
    /// @param pPageOpt
    void AttachOpt(PPageOptBase *const pPageOpt) noexcept;
    /// @brief
    /// @param pPageOpt
    void DetachOpt(PPageOptBase const *const pPageOpt) noexcept;
    /// @brief
    void ResetCacheData() noexcept;
    /// @brief Determine if the cache buffer is valid
    /// @return
    inline bool IsCacheValid() const noexcept
    {
        if (nullptr == naiMapBuff_.GetBuff()) {
            return false;
        }
        return naiMapBuff_.GetLen() > 0U;
    }
    /// @brief
    /// @return
    inline uint32_t GetCacheLen() const noexcept { return naiMapBuff_.GetLen(); }
    /// @brief
    /// @return
    inline uint8_t *GetCache() const noexcept { return naiMapBuff_.GetBuff(); }
    /// @brief Set the current activity timestamp
    /// @return
    inline int64_t GetActiveOptTime() const noexcept { return tmActiveOpt_; }
    /// @brief
    /// @param tmNow
    inline void SetActiveOptTime(time_t const &tmNow) noexcept { tmActiveOpt_ = tmNow; }
    /// @brief
    /// @param tmNow
    inline void SetActiveOptTime(TimePoint const &tmNow) noexcept { tmActiveOpt_ = tmNow.time_since_epoch().count(); }
    /// @brief
    /// @return
    inline bool IsWriteRef() const noexcept { return nWriteRef_ > 0; }
    /// @brief Increment write count
    /// @param bAddRef
    inline void AddWriteRef(bool const bAddRef) noexcept
    {
        if (bAddRef) {
            nWriteRef_ += 1;
        }
    }
    /// @brief Whether a PageOpt is currently operating
    /// @return
    inline bool IsHaveOpt() const noexcept
    {
        if (false == vecAttachOpt_.empty()) {
            return true;
        }
        return false;
    }
    /// @brief
    /// @return
    inline uint32_t GetAttachCount() const noexcept { return static_cast< uint32_t >(vecAttachOpt_.size()); }
    /// @brief Determine whether pVoidData is a valid data area
    /// @param pVoidData
    /// @return
    inline bool IsValidData(void const *const pVoidData) const noexcept
    {
        int32_t const nMaxDataLen{static_cast< int32_t >(naiMapBuff_.GetLen())};
        int32_t const nOffset{T_SubPtr(pVoidData, naiMapBuff_.GetBuff())};
        return (nOffset >= 0) && (nOffset < nMaxDataLen);
    }
    /// @brief Out-of-bounds check
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    inline bool IsValidData(void const *const pVoidData, int32_t const nDataLen) const noexcept
    {
        int32_t const nMaxDataLen{static_cast< int32_t >(naiMapBuff_.GetLen())};
        int32_t const nOffset{T_SubPtr(pVoidData, naiMapBuff_.GetBuff())};
        // Only if the start node is within this cache range, then check if the end is also within range
        if ((nOffset < 0) || (nOffset >= nMaxDataLen)) {
            return true;
        }
        return (nOffset + nDataLen - 1) < nMaxDataLen;
    }

public:
    /// @brief Get idle time
    /// @param tmNow
    /// @return
    int64_t GetIdleTime(time_t const tmNow) const noexcept;
    /// @brief Get idle time
    /// @param tmNow
    /// @return
    int64_t GetIdleTime(TimePoint const tmNow) const noexcept;
    /// @brief Set PageID based on source
    /// @param ePageSource
    /// @param nPageID
    void InitPageData(ECacheSource const ePageSource, uint32_t const nPageID) noexcept;
};
//********************************/
/// @brief
class PManagerCache;
/// @brief
/// @code{.isoft}
/// @unit_name=ICachePage
/// @interface_level=unit
/// @endcode
class ICachePage
{
private:
    /// @brief
    PMemoryPage *pMemoryPage_;
    /// @brief
    PManagerCache *pCacheManager_;
    /// @brief
    bool bStackMemory_;

protected:
    /// @brief Constructor
    /// @param nCacheLen
    /// @param pCacheManager
    explicit ICachePage(uint32_t const nCacheLen, PManagerCache *const pCacheManager) noexcept;
    /// @brief Constructor
    /// @param pMemoryPage
    /// @param pCacheManager
    explicit ICachePage(PMemoryPage *const pMemoryPage, PManagerCache *const pCacheManager) noexcept;

public:
    /// @brief Virtual destructor
    virtual ~ICachePage() noexcept;
    /// @brief
    /// @param a
    ICachePage(ICachePage const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    ICachePage &operator=(ICachePage const &a) = delete;
    /// @brief
    /// @param a
    ICachePage(ICachePage &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    ICachePage &operator=(ICachePage &&a) = delete;

public:
    /// @brief Provide a KvName for Log output
    /// @return
    ara::core::StringView GetLogKvName() const noexcept;
    /// @brief Send thread synchronization signal
    /// @return
    bool SendSyncSignal() noexcept;
    /// @brief Determine whether the memory pages of two cache pages are identical
    /// @param pCachPage
    /// @return
    bool IsSamePage(ICachePage const *const pCachPage) const noexcept;
    /// @brief Determine whether the memory pages of two cache pages are identical
    /// @return
    inline PMemoryPage *GetMemoryPage() const noexcept { return pMemoryPage_; }
    /// @brief Return the internal memory cache pointer of the object
    /// @return
    inline PMemoryPage *operator->() noexcept { return pMemoryPage_; }
    /// @brief Return the const version of the internal memory cache pointer of the object
    /// @return
    inline PMemoryPage const *operator->() const noexcept { return pMemoryPage_; }
};
/// @brief Cache page proxy
/// @code{.isoft}
/// @unit_name=PCachePage_Proxy
/// @interface_level=unit
/// @endcode
class PCachePage_Proxy final : public ICachePage
{
public:
    /// @brief
    /// @param pMemoryPage
    /// @param pCacheManager
    explicit PCachePage_Proxy(PMemoryPage *const pMemoryPage, PManagerCache *const pCacheManager) noexcept;
    /// @brief The sole purpose is to leverage the timing of this destructor call to trigger certain actions
    ~PCachePage_Proxy() noexcept final;
    /// @brief
    /// @param a
    PCachePage_Proxy(PCachePage_Proxy const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCachePage_Proxy &operator=(PCachePage_Proxy const &a) = delete;
    /// @brief
    /// @param a
    PCachePage_Proxy(PCachePage_Proxy &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCachePage_Proxy &operator=(PCachePage_Proxy &&a) = delete;
};
//***************/
/// @brief Stack page
/// @code{.isoft}
/// @unit_name=PCachePage_Stack
/// @interface_level=unit
/// @endcode
class PCachePage_Stack final : public ICachePage
{
public:
    /// @brief
    /// @param nCacheLen
    /// @param pCacheManager
    PCachePage_Stack(uint32_t const nCacheLen, PManagerCache *const pCacheManager) noexcept;
    /// @brief
    ~PCachePage_Stack() noexcept final = default;
    /// @brief
    /// @param a
    PCachePage_Stack(PCachePage_Stack const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCachePage_Stack &operator=(PCachePage_Stack const &a) = delete;
    /// @brief
    /// @param a
    PCachePage_Stack(PCachePage_Stack &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCachePage_Stack &operator=(PCachePage_Stack &&a) = delete;
};
//***************/

/// @brief
/// @code{.isoft}
/// @unit_name=PCachePagePtr
/// @endcode
using PCachePagePtr = std::shared_ptr< ICachePage >;
/// @brief Overload comparison function for PCachePagePtr
/// @code{.isoft}
/// @unit_name=operator==
/// @interface_level=unit
/// @endcode
/// @param pageA
/// @param pageB
/// @return
bool operator==(PCachePagePtr const &pageA, PCachePagePtr const &pageB) noexcept;
/// @brief Overload comparison function for PCachePagePtr
/// @code{.isoft}
/// @unit_name=operator!=
/// @interface_level=unit
/// @endcode
/// @param pageA
/// @param pageB
/// @return
bool operator!=(PCachePagePtr const &pageA, PCachePagePtr const &pageB) noexcept;
/// @brief Generate a smart pointer of PCachePage_Proxy from PMemoryPage
/// @code{.isoft}
/// @unit_name=MakeProxyPagePtr
/// @endcode
/// @param pMemoryPage
/// @param pCacheManager
/// @return
PCachePagePtr MakeProxyPagePtr(PMemoryPage *const pMemoryPage, PManagerCache *const pCacheManager) noexcept;
/// @brief Generate a smart pointer of PCachePage_Stack
/// @code{.isoft}
/// @unit_name=MakeStackPagePtr
/// @endcode
/// @param nCacheLen
/// @param pCacheManager
/// @return
PCachePagePtr MakeStackPagePtr(uint32_t const nCacheLen, PManagerCache *const pCacheManager) noexcept;
//***************/

/// @brief
/// @code{.isoft}
/// @unit_name=PCacheAutoLock
/// @endcode
class PCacheAutoLock final
{
private:
    /// @brief
    PMemoryPage *pCachePage_;
    /// @brief
    PPageOptBase *pPageOpt_;

public:
    /// @brief Default binding to a no-op
    /// @param pCachePage
    /// @param pPageOpt
    explicit PCacheAutoLock(PMemoryPage *const pCachePage, PPageOptBase *const pPageOpt = nullptr) noexcept;
    /// @brief
    ~PCacheAutoLock() noexcept;
    /// @brief
    /// @param a
    PCacheAutoLock(PCacheAutoLock const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCacheAutoLock &operator=(PCacheAutoLock const &a) = delete;
    /// @brief
    /// @param a
    PCacheAutoLock(PCacheAutoLock &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PCacheAutoLock &operator=(PCacheAutoLock &&a) = delete;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
