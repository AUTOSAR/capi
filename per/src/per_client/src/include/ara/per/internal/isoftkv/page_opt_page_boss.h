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
/// @file       page_opt_page_boss.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Page management
/// @date       2021-05-10
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptPageBoss
/// @unit_description=Logical page for parsing management Boss pages
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

#ifndef ARA_PER_PHKV_PAGE_OPT_PAGE_BOSS_H_
#define ARA_PER_PHKV_PAGE_OPT_PAGE_BOSS_H_

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_solo.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Page management page
class PPageOptPageBoss : public PPageOptSolo
{
public:
    /// @brief
    /// @param a
    PPageOptPageBoss(PPageOptPageBoss const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptPageBoss &operator=(PPageOptPageBoss const &a) = delete;
    /// @brief
    /// @param a
    PPageOptPageBoss(PPageOptPageBoss &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptPageBoss &operator=(PPageOptPageBoss &&a) = delete;
    /// @brief
    /// @param pCachePagePtr
    /// @param nDefBlockID
    explicit PPageOptPageBoss(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID = 0U) noexcept;
    /// @brief
    ~PPageOptPageBoss() noexcept override = default;
    /// @brief
    /// @return
    uint16_t GetMinValidLen()
        const noexcept override;  // Get the minimum length of the current page type: when there is no data
public:
    /// @brief Initialize the BOSSBlock header of the BossPage
    /// @param nGroupID
    /// @param nFreeListCount
    /// @param nPageCountMax
    /// @return
    bool InitBossHead(uint16_t const nGroupID,
                      uint16_t const nGroupPageCapacity,
                      uint16_t const nFreeListCount,
                      uint16_t const nPageCountMax) noexcept;
    /// @brief Transfer the page where nPageIndex resides into a new list nListIndex
    /// @param nPageIndex
    /// @param nFreeLen
    /// @return
    bool UpdateFreePage(uint16_t const nPageIndex, uint16_t const nFreeLen) noexcept;
    /// @brief Does not actually delete page data and the page; actual deletion can only be done during disk defragmentation
    /// @param nPageIndex
    /// @return
    bool DelOldPage(uint16_t const nPageIndex) const noexcept;
    // The following functions must be called only after ensuring IsAccessReady returns true
public:
    /// @brief Get the number of free lists
    /// @return
    int32_t GetFreeListCapacity() const noexcept;
    /// @brief Check whether the capacity of the BossPage can still accommodate a new element
    /// @return
    bool CheckBossPageCapacity() const noexcept;
    /// @brief Get the GroupID in the BlockBoss header
    /// @return
    uint16_t GetGroupID() const noexcept;
    /// @brief Find a free page, return the page number (-1 on failure, 0 indicates need to create a new one)
    /// @param nNeedSpace
    /// @return
    uint16_t FindFreePageID(uint16_t const nNeedSpace) const noexcept;
    /// @brief Find the page management data area
    /// @return
    PBlockHeadPageBoss *GetHeadBoss() const noexcept;

    // Utility functions, will validate parameters
protected:
    /// @brief Calculate the free list index
    /// @param nFreeLen
    /// @return
    int32_t _ComputeFreeListIndex(uint16_t const nFreeLen) const noexcept;
    /// @brief Find the page management data area
    /// @param bCheck Whether to execute PER_Assert check
    /// @return
    PBlockHeadPageBoss *_GetHeadBoss(bool const bCheck = true) const noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
