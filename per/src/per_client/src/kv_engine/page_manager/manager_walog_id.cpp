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
/// @file       manager_walog_id.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Walog page ID mapping management class
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Management
/// @interface_level=unit
/// @trace_id_sr=
/// @unit_name=PManagerWalogID
/// @unit_description=Walog page ID mapping management class
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

#include "ara/per/internal/isoftkv/manager_walog_id.h"

#include "ara/per/internal/isoftkv/page_opt_base.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
PManagerWalogID::PManagerWalogID() noexcept { vecMainPageID_.reserve(kInt_1024U); }
/// @brief Register a new Main page and return the PageID for creating a new page in the Walog library
/// @param nMainID
/// @return
uint32_t PManagerWalogID::RegMainPage(uint32_t const nMainID) noexcept
{
    uint32_t nWalogID{_FindWalogID(nMainID)};
    if (PPageOptBase::IsValidPageID(nWalogID)) {
        return nWalogID;
    }
    vecMainPageID_.push_back(nMainID);
    nWalogID                 = static_cast< uint32_t >(vecMainPageID_.size());
    mapWalogPageID_[nMainID] = nWalogID;
    return nWalogID;
}
/// @brief Reset the PageID mapping
void PManagerWalogID::Reset() noexcept
{
    vecMainPageID_.clear();
    mapWalogPageID_.clear();
}
/// @brief Find MainID by WalogID
/// @param nWalogID
/// @return
uint32_t PManagerWalogID::FindMainID(uint32_t const nWalogID) const noexcept
{
    uint32_t const nWalogIndex{nWalogID - 1U};
    if (nWalogIndex >= vecMainPageID_.size()) {
        return kInvalidPageID;
    }
    return vecMainPageID_[static_cast< std::size_t >(nWalogIndex)];
}
/// @brief Find WalogID by MainID
/// @param nMainID
/// @return
uint32_t PManagerWalogID::FindWalogID(uint32_t const nMainID) const noexcept { return _FindWalogID(nMainID); }
/// @brief Get the number of Walog pages
/// @return
uint32_t PManagerWalogID::GetWalogCount() const noexcept { return static_cast< uint32_t >(vecMainPageID_.size()); }
//********************************/
/// @brief
/// @param nMainID
/// @return
uint32_t PManagerWalogID::_FindWalogID(uint32_t const nMainID) const noexcept
{
    ara::core::Map< uint32_t, uint32_t >::const_iterator itFind;
    itFind = mapWalogPageID_.find(nMainID);
    if (itFind == mapWalogPageID_.end()) {
        return kInvalidPageID;
    }
    return itFind->second;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara