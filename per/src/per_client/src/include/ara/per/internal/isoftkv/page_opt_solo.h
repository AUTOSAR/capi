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
/// @file       page_opt_solo.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Base class for exclusive blocks (exclusive means at most one per page)
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=unit
/// @trace_id_sr=
/// @unit_name=PPageOptSolo
/// @unit_description=Key-Value storage page: Base class for exclusive blocks (exclusive means at most one per page)
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-19 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_PAGE_OPT_SOLO_H_
#define ARA_PER_PHKV_PAGE_OPT_SOLO_H_

#include <functional>

#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_base.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief Exclusive page base class
class PPageOptSolo : public PPageOptBase
{
private:
    /// @brief
    EBlockType eSoloBlockType_;
    /// @brief
    uint16_t nDefBlockID_;

private:
    /// @brief Page management data area, valid only for exclusive pages
    PBlockHead *pSoloBlock_;
    /// @brief
    uint16_t nSoloBlockID_;

public:
    /// @brief
    /// @param pCachePagePtr
    /// @param eBlockType
    /// @param nDefBlockID
    explicit PPageOptSolo(PCachePagePtr const &pCachePagePtr,
                          EBlockType const eBlockType,
                          uint16_t const nDefBlockID) noexcept;
    /// @brief Default constructor: deleted
    PPageOptSolo() = delete;
    /// @brief
    ~PPageOptSolo() noexcept override = default;
    /// @brief
    /// @param a
    PPageOptSolo(PPageOptSolo const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptSolo &operator=(PPageOptSolo const &a) = delete;
    /// @brief
    /// @param a
    PPageOptSolo(PPageOptSolo &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptSolo &operator=(PPageOptSolo &&a) = delete;

public:
    /// @brief Initialize Page data
    /// @param pCachePagePtr
    /// @return
    bool InitCachePage(PCachePagePtr const &pCachePagePtr) noexcept override;
    /// @brief Check if ready for read/write
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;
    /// @brief
    /// @return
    EBlockType PageClassType() const noexcept override;
    /// @brief Reset the page, retaining the PageID
    /// @param bResetData
    /// @return
    bool ResetPage(bool const bResetData) noexcept override;

public:  // The following functions must be called only after ensuring IsAccessReady() returns true
    /// @brief Get the exclusive BlockID
    /// @return
    inline uint16_t GetSoloBlockID() const noexcept { return nSoloBlockID_; }
    /// @brief Create an empty SoloBlock
    /// @param eRangeType
    /// @param nBlockDataLen
    /// @return
    bool InitSoloBlock(EStoreRange const eRangeType, uint16_t const nBlockDataLen) noexcept;
    /// @brief This design is no longer supported as of 2021-06-23, the interface is retained
    /// @return
    PBlockSpread *FindBlockSpread() noexcept;
    /// @brief
    using PPageOptBase::FindBlockSpread;

protected:
    /// @brief
    void _ResetData() noexcept;

protected:
    /// @brief Find the page management data area
    /// @return
    uint8_t *_GetMineBlockData() const noexcept;
    /// @brief Bind Solo related variables
    /// @return
    bool _AttachSoloBlock() noexcept;
    /// @brief Initialize Page data
    /// @param pCachePagePtr
    /// @return
    bool _InitCachePage_Solo(PCachePagePtr const &pCachePagePtr) noexcept;
    /// @brief Check if the remaining space meets the requirement
    /// @param eRangeType
    /// @param nBlockDataLen
    /// @return
    bool _CheckBlockSpace(EStoreRange const eRangeType, uint16_t const nBlockDataLen) const noexcept;
    /// @brief Get the length of the block control data
    /// @param eRangeType
    /// @return
    static uint16_t S_GetBlockHeadLen(EStoreRange const eRangeType) noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
