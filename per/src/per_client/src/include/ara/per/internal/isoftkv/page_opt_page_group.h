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
/// @file       page_opt_page_group.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Page group management
/// @date       2021-05-26
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptPageGroup
/// @unit_description=Logical page for parsing page group management
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-26 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_PAGE_OPT_PAGE_GROUP_H_
#define ARA_PER_PHKV_PAGE_OPT_PAGE_GROUP_H_

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_solo.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Page management page
class PPageOptPageGroup : public PPageOptSolo
{
protected:
public:
    /// @brief
    /// @param a
    PPageOptPageGroup(PPageOptPageGroup const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptPageGroup &operator=(PPageOptPageGroup const &a) = delete;
    /// @brief
    /// @param a
    PPageOptPageGroup(PPageOptPageGroup &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptPageGroup &operator=(PPageOptPageGroup &&a) = delete;
    /// @brief
    /// @param pCachePagePtr
    /// @param nDefBlockID
    explicit PPageOptPageGroup(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID = 0U) noexcept;
    /// @brief
    ~PPageOptPageGroup() noexcept override = default;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;

public:
    /// @brief Initialize GroupHead data
    /// @param versionSystem
    /// @param versionPer
    /// @param nSpaceAmountMin
    /// @param nSpaceAmountMax
    /// @param nHashPageID
    /// @param nPageInitLen
    /// @return
    bool InitGroupHead(PPerVersion const versionSystem,
                       PPerVersion const versionPer,
                       uint32_t const nSpaceAmountMin,
                       uint32_t const nSpaceAmountMax,
                       uint32_t const nHashPageID,
                       uint16_t const nPageInitLen) noexcept;
    /// @brief Add a GroupElement node
    /// @param nBossPageID
    /// @param nFreePageCount
    /// @param nMaxFreeIndex
    /// @return
    bool AddGroupElement(uint32_t const nBossPageID,
                         uint16_t const nFreePageCount,
                         uint16_t const nMaxFreeIndex) noexcept;
    /// @brief Modify a data item within a group
    /// @param nPageID
    /// @param nNewFreeCount
    /// @param nMaxFreeIndex
    /// @return
    bool AmendGroupData(uint32_t const nPageID,
                        uint16_t const nNewFreeCount,
                        uint16_t const nMaxFreeIndex) const noexcept;
    /// @brief Maintain the number of completely free pages
    /// @param nPageID
    /// @param nAddCount
    /// @return
    bool AddFreeCount(uint32_t const nPageID, int32_t const nAddCount) const noexcept;
    /// @brief Maintain the maximum free list index
    /// @param nPageID
    /// @param nAddCount
    /// @return
    bool UpdateMaxFreeIndex(uint32_t const nPageID, int32_t const nAddCount) const noexcept;
    /// @brief Update BossPageID
    /// @param nOldBossPageID
    /// @param nNewBossPageID
    /// @return
    bool UpdateBossPageID(uint16_t const nOldBossPageID, uint16_t const nNewBossPageID) const noexcept;
    /// @brief Find GroupElement
    /// @param nPageID
    /// @return
    PBlockElementGroup *FindGroupElement(uint32_t const nPageID) const noexcept;
    // The following functions must be called only after ensuring IsAccessReady returns true
public:
    /// @brief Get the total number of managed pages
    /// @return
    uint32_t GetPageTotal() const noexcept;
    /// @brief Get the page number for creating a new page
    /// @return
    uint32_t GetNewPageID() const noexcept;
    /// @brief Get the initial length of a page
    /// @return
    uint16_t GetPageInitLen() const noexcept;
    /// @brief Get the page capacity per group
    /// @return
    uint16_t GetGroupCapacity() const noexcept;
    /// @brief Get the capacity limit
    /// @return
    uint32_t GetSpaceAmountMax() const noexcept;
    /// @brief Get the number of managed groups
    /// @return
    uint16_t GetGroupCount() const noexcept;
    /// @brief Get the page number of the Hash page
    /// @return
    uint32_t GetHashPageID() const noexcept;
    /// @brief Set HashPageID
    /// @param nHashPageID
    void SetHashPageID(uint32_t const nHashPageID) const noexcept;
    /// @brief Increment the total page count by 1
    void IncreasePageTotal() const noexcept;
    /// @brief Get the GroupID via PageID
    /// @param nPageID
    /// @return
    uint16_t TransGroupID(uint32_t const nPageID) const noexcept;
    /// @brief Convert a 16-bit BossID to a 32-bit value
    /// @param nGroupID
    /// @param nPageIndex
    /// @return
    uint32_t TransPageID(uint16_t const nGroupID, uint16_t const nPageIndex) const noexcept;
    /// @brief Convert a 32-bit PageID to the 16-bit page sequence number within its group
    /// @param nPageID32
    /// @return
    uint16_t TransBossPageIndex(uint32_t const nPageID32) const noexcept;
    /// @brief Find the PageID of the PageBoss to which nCurPageID belongs
    /// @param nCurPageID
    /// @return PageID of the found BossPage
    uint32_t FindBossPageID(uint32_t const nCurPageID) const noexcept;
    /// @brief Get the BossID of the group to which the given PageID belongs
    /// @param nPageID
    /// @return
    uint32_t GetBossPageID(uint32_t const nPageID) const noexcept;
    /// @brief Get GroupHead
    /// @return
    PBlockHeadPageGroup *GetHeadGroup() const noexcept;
    /// @brief Get the PER format version number information saved in the library: persistent data
    /// @return
    PPerVersion const &GetPerVersion_Per() const noexcept;
    /// @brief Get the PER format version number information saved in the library: KvSystem
    /// @return
    PPerVersion const &GetPerVersion_Sys() const noexcept;
    /// @brief Set the PER format version number information saved in the library: persistent data
    /// @param version
    void SetPerVersion_Per(PPerVersion const &version) const noexcept;
    /// @brief Set the PER format version number information saved in the library: KvSystem
    /// @param version
    void SetPerVersion_System(PPerVersion const &version) const noexcept;

protected:
    /// @brief Find GroupElement
    /// @param nGroupID
    /// @return
    PBlockElementGroup *_FindGroupElement(int32_t const nGroupID) const noexcept;
    /// @brief Get GroupHead
    /// @param bCheck Whether to execute PER_Assert
    /// @return
    PBlockHeadPageGroup *_GetHeadGroup(bool const bCheck) const noexcept;
    /// @brief
    /// @param blockHead
    /// @param nIndex
    /// @return
    static bool S_CheckArrayIndex(PBlockHeadPageGroup const &blockHead, int32_t const nIndex) noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
