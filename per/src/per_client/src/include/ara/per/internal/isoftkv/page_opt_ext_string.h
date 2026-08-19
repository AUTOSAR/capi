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
/// @file       page_opt_ext_string.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Extended data: initially designed only to save extended version number information as of 2023-08
/// @date       2023-07-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOpt_ExtString
/// @unit_description=Logical page for parsing data extensions
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-07-12 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_PAGE_OPT_EXT_STRING_H_
#define ARA_PER_PHKV_PAGE_OPT_EXT_STRING_H_

#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_solo.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Data page
class PPageOpt_ExtString : public PPageOptSolo
{
public:
    /// @brief
    /// @param a
    PPageOpt_ExtString(PPageOpt_ExtString const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOpt_ExtString &operator=(PPageOpt_ExtString const &a) = delete;
    /// @brief
    /// @param a
    PPageOpt_ExtString(PPageOpt_ExtString &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPageOpt_ExtString &operator=(PPageOpt_ExtString &&a) = delete;
    /// @brief
    ~PPageOpt_ExtString() noexcept override = default;
    /// @brief
    /// @param pCachePagePtr
    explicit PPageOpt_ExtString(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID = 0U) noexcept;
    /// @brief Get the minimum length of the current page type: when there is no data
    /// @return
    uint16_t GetMinValidLen() const noexcept override;

public:
    /// @brief Initialize the data control header of the extension item
    /// @return
    bool InitExtHead(uint16_t const nCapacityLen) noexcept;
    /// @brief Get the String value of VersionApp within Ext
    /// @return
    ara::core::String GetVersionApp() const noexcept;
    /// @brief Get the String value of VersionPer within Ext
    /// @return
    ara::core::String GetVersionPer() const noexcept;
    /// @brief Get the String value of VersionSystem within Ext
    /// @return
    ara::core::String GetVersionSystem() const noexcept;
    /// @brief Set the String value of VersionApp within Ext
    /// @param stVersion String version number
    /// @return Success or failure
    bool SetVersionApp(ara::core::StringView const &stVersion) noexcept;
    /// @brief Set the String value of VersionPer within Ext
    /// @param stVersion String version number
    /// @return Success or failure
    bool SetVersionPer(ara::core::StringView const &stVersion) noexcept;
    /// @brief Set the String value of VersionSystem within Ext
    /// @param stVersion String version number
    /// @return Success or failure
    bool SetVersionSystem(ara::core::StringView const &stVersion) noexcept;

public:
protected:
    /// @brief Get BlockHead
    /// @param bCheck Whether to execute PER_Assert
    /// @return
    PBlockHead_Ext *_GetBlockHeadExt(bool const bCheck) const noexcept;
    /// @brief Get the String value within Ext
    /// @return
    ara::core::String _GetVersionString(EVersionStringType const eStringType) const noexcept;
    /// @brief Set the String value within Ext
    /// @param eStringType String type
    /// @param stData Version string
    /// @return Success or failure
    bool _SetVersionString(EVersionStringType const eStringType, ara::core::StringView const &stData) noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
