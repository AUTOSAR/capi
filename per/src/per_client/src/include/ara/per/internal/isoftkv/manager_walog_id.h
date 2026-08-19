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
/// @file       manager_walog_id.h
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

#ifndef ARA_PER_PHKV_MANAGER_WALOG_ID_H_
#define ARA_PER_PHKV_MANAGER_WALOG_ID_H_

#include "ara/core/map.h"
#include "ara/core/vector.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Walog page ID mapping management class
class PManagerWalogID final
{
private:
    /// @brief Index is Walog.PageID, stores Main.PageID
    ara::core::Vector< uint32_t > vecMainPageID_;
    /// @brief Key is Main.PageID, Value is Walog.PageID
    ara::core::Map< uint32_t, uint32_t > mapWalogPageID_;

public:
    /// @brief
    /// @param a
    /// @return
    PManagerWalogID(PManagerWalogID const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerWalogID &operator=(PManagerWalogID const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerWalogID(PManagerWalogID &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerWalogID &operator=(PManagerWalogID &&a) = delete;
    /// @brief
    PManagerWalogID() noexcept;
    /// @brief
    ~PManagerWalogID() noexcept = default;
    /// @brief Register a new Main page and return the PageID for creating a new page in the Walog library
    /// @param nMainID
    /// @return
    uint32_t RegMainPage(uint32_t const nMainID) noexcept;
    /// @brief Reset the PageID mapping
    void Reset() noexcept;
    /// @brief Find MainID by WalogID
    /// @param nWalogID
    /// @return
    uint32_t FindMainID(uint32_t const nWalogID) const noexcept;
    /// @brief Find WalogID by MainID
    /// @param nMainID
    /// @return
    uint32_t FindWalogID(uint32_t const nMainID) const noexcept;
    /// @brief Get the number of Walog pages
    /// @return
    uint32_t GetWalogCount() const noexcept;

protected:
    /// @brief
    /// @param nMainID
    /// @return
    uint32_t _FindWalogID(uint32_t const nMainID) const noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif