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
/// @file       page_opt_value.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Raw data
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Value Page Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptValue
/// @unit_description=Value data page
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

#ifndef ARA_PER_PHKV_PAGE_OPT_VALUE_H_
#define ARA_PER_PHKV_PAGE_OPT_VALUE_H_

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_base.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Data page
class PPageOptValue : public PPageOptBase
{
public:
    /// @brief
    /// @param a
    PPageOptValue(PPageOptValue const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptValue &operator=(PPageOptValue const &a) = delete;
    /// @brief
    /// @param a
    PPageOptValue(PPageOptValue &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOptValue &operator=(PPageOptValue &&a) = delete;
    /// @brief
    /// @param pCachePagePtr
    explicit PPageOptValue(PCachePagePtr const &pCachePagePtr) noexcept;
    /// @brief
    ~PPageOptValue() noexcept override = default;
    /// @brief
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;
    /// @brief
    /// @return
    EBlockType PageClassType() const noexcept override;

public:
    /// @brief
    /// @return
    bool InitValueHead() noexcept;
    /// @brief Add a ValueBlock data block: spanning pages
    /// @param pBData
    /// @param nDataLen
    /// @param pSpreadBlock
    /// @param eDataType
    /// @return Return value is the BlockID
    uint16_t AddValueBlock(uint8_t const *const pBData,
                           uint16_t const nDataLen,
                           PBlockSpread const *const pSpreadBlock,
                           EDataType const eDataType) noexcept;
    /// @brief Delete a ValueBlock data block
    /// @param nBlockID
    /// @return
    bool DelValueBlock(uint16_t const nBlockID) noexcept;
    /// @brief Get the value of Value
    /// @param nBlockID
    /// @return
    PValueData GetValueData(uint16_t const nBlockID) const noexcept;
    /// @brief Find cross-page data
    /// @param nBlockID
    /// @return
    PBlockSpread *FindBlockSpread(uint16_t const nBlockID) noexcept;
    /// @brief Get the total length of Value
    /// @param nBlockID
    /// @param pReturnDataType
    /// @return
    uint32_t GetValueTotalLen(uint16_t const nBlockID, int32_t *const pReturnDataType) const noexcept;
    /// @brief
    using PPageOptBase::FindBlockSpread;

public:
protected:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
