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
/// @file       initialize.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    AP-Per initialization
/// @date       2021-04-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-28  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_INITIALIZE_H_
#define ARA_PER_INITIALIZE_H_

#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/log/logger.h"
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Whether initialized
/// @return
bool IsPerInitialize() noexcept;
/// @brief Initialize AP-Per system, mainly determining manifest.json configuration file
/// @code{.isoft}
/// @unit_name=Initialize
/// @endcode
/// @param[in] stJsonConfig
/// @param[in] pLogger
/// @return
/// @traceid {SWS_PER_00408}
ara::core::Result< void > Initialize(ara::core::StringView const& stJsonConfig = ara::core::StringView{""},
                                     ara::log::Logger* const pLogger           = nullptr) noexcept;
/// @brief Deinitialize AP-Per system
/// @code{.isoft}
/// @unit_name=Deinitialize
/// @endcode
/// @return
/// @traceid {SWS_PER_00409}
ara::core::Result< void > Deinitialize() noexcept;
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=CB_EachReddFile
/// @endcode
using CB_EachReddFile = std::function< bool(ara::core::StringView const& stFileName) >;
/// @brief Get redundant file name
/// @code{.isoft}
/// @unit_name=MakeReddFileName
/// @endcode
/// @param[in] stFileName
/// @param[in] nIndex
/// @return
ara::core::String MakeReddFileName(ara::core::StringView const& stFileName, int32_t const nIndex) noexcept;
/// @brief Traverse each redundant file
/// @code{.isoft}
/// @unit_name=ScanAllReddFile
/// @endcode
/// @param[in] stFileName
/// @param[in] nReddCount
/// @param[in] pfun
/// @return
int32_t ScanAllReddFile(ara::core::StringView const& stFileName,
                        int32_t const nReddCount,
                        CB_EachReddFile const& pfun) noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
