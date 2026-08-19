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
/// @file       recovery.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Redundancy and data recovery
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

#include "ara/per/recovery.h"

namespace {
/// @brief
/// @return
ara::per::CB_Recovery &G_GetRecoveryReport() noexcept
{
    static ara::per::CB_Recovery s_FunRecoveryReport{nullptr};
    return s_FunRecoveryReport;
}
}  // namespace

namespace ara {
namespace per {
//********************************/
/// @brief Register a recovery callback function to persistence storage module.
/// @param recoveryReportCallback
void RegisterRecoveryReportCallback(CB_Recovery const &recoveryReportCallback) noexcept
{
    ara::per::CB_Recovery &pRecoveryReport{G_GetRecoveryReport()};
    pRecoveryReport = recoveryReportCallback;
}
/// @brief Call registered recovery callback function
/// @param[in] storage Storage identifier for recovery
/// @param[in] recoveryReportKind Recovery type enumeration
/// @param[in] reportedElements Elements involved in recovery (Key of KeyValueStorage, File of FileStorage)
/// @param[in] reportedInstances
/// @return no
void RecoveryReport(ara::core::InstanceSpecifier const &storage,
                    ara::per::RecoveryReportKind recoveryReportKind,
                    ara::core::Vector< ara::core::String > reportedElements,
                    ara::core::Vector< uint8_t > reportedInstances) noexcept
{
    ara::per::CB_Recovery &pRecoveryReport{G_GetRecoveryReport()};
    if (pRecoveryReport) {
        pRecoveryReport(storage, recoveryReportKind, std::move(reportedElements), std::move(reportedInstances));
    }
}
/// @brief Whether recovery callback is registered
/// @return
bool IsHaveRecoverReport() noexcept
{
    if (G_GetRecoveryReport()) {
        return true;
    }
    return false;
}
//********************************/
}  // namespace per
}  // namespace ara
