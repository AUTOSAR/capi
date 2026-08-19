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
/// @file       recovery.h
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

#ifndef ARA_PER_RECOVERY_H_
#define ARA_PER_RECOVERY_H_

#include <functional>

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
//********************************/
namespace ara {
namespace per {
//********************************/
/// @brief Data recovery report enumeration
/// @code{.isoft}
/// @unit_name=RecoveryReportKind
/// @traceid {SWS_PER_00432}
/// @endcode
/// @brief Defines the reported recovery actions.
enum class RecoveryReportKind : uint32_t
{
    /// @brief Recover KeyValueStorage failed
    /// A Key-Value Storage was corrupted, an insufficient number of valid copies existed.
    /// storage contains the short-name path of the Key-Value Storage,
    /// reportedElements is empty,
    /// reportedInstances contains the indices of the affected Key-Value Storage copies.
    kKeyValueStorageRecoveryFailed = 1,
    /// @brief Recover KeyValueStorage succeeded
    // A Key-Value Storage was corrupted, but a sufficient number of valid copies existed.
    /// storage contains theshort-name path of the Key-Value Storage,
    /// reportedElements is empty,
    /// reportedInstances contains the indices of the affected Key-Value Storage copies.
    kKeyValueStorageRecovered = 2,
    /// @brief Recover single KV pair failed
    /// A set of Key-Value Pairs was corrupted, an insufficient number of valid copies existed.
    /// storage contains the short-name path of the Key-ValueStorage,
    /// reportedElements contains the list of affected keys,
    /// reportedInstances contains the indices of the affected Key-Value Storage or key copies.
    kKeyRecoveryFailed = 3,
    /// @brief Recover single KV pair succeeded
    /// A set of Key-Value Pairs was corrupted, but a sufficient number of valid copies existed.
    /// storage contains the short-name path of the Key-ValueStorage,
    /// reportedElements contains the list ofaffected keys,
    /// reportedInstances contains the indices of the affected Key-Value Storage or key copies.
    kKeyRecovered = 4,
    /// @brief Recover FileStorage failed
    /// A File Storage was corrupted, an insufficient number of valid copies existed.
    /// storage contains the short-name path of the File Storage,
    /// reportedElements is empty,
    /// reportedInstances contains the indices of the affected File Storage copies.
    kFileStorageRecoveryFailed = 5,
    /// @brief Recover FileStorage succeeded
    /// A File Storage was corrupted, but a sufficient number of valid copies existed.
    /// storage contains the short-name path of the File Storage,
    /// reportedElements is empty,
    /// reportedInstances contains the indices of the affected File Storage copies.
    kFileStorageRecovered = 6,
    /// @brief Recover single file failed
    /// A set of files was corrupted, an insufficient number of valid copies existed.
    /// storage contains the short-name path of the File Storage,
    /// reportedElements contains the list of affected file names,
    /// reportedInstances contains the indices of the affected File Storage or file copies.
    kFileRecoveryFailed = 7,
    /// @brief Recover single file succeeded
    /// A set of files was corrupted, but a sufficient number of valid copies existed.
    /// storage contains the short-name path of the File Storage,
    /// reportedElements contains the list of affected file names,
    /// reportedInstances contains the indices of the affected File Storage or file copies.
    kFileRecovered = 8,
};
//********************************/
/// @brief Data recovery report callback function
using CB_Recovery = std::function< void(ara::core::InstanceSpecifier const &storage,
                                        ara::per::RecoveryReportKind recoveryReportKind,
                                        ara::core::Vector< ara::core::String > reportedElements,
                                        ara::core::Vector< uint8_t > reportedInstances) >;
/// @brief Register a recovery callback function to persistence storage module.
/// @code{.isoft}
/// @unit_name=RegisterRecoveryReportCallback
/// @traceid {SWS_PER_00433}
/// @threadsafety {no}
/// @endcode
/// @brief Register a recovery reporting callback with persistency.
/// This callback can be used in safety-aware applications to detect actions of the Persistency
/// that are related to the correctness of the persisted data and the reliability of the storage.
/// @param[in] recoveryReportCallback
/// The callback function to be called by Persistency to report errors in the stored data
/// that were correctedusing the available redundancy.
/// The function will be called with the shortName path of the affectedKey-Value Storage
/// or File Storage in storage and information on what has been corrected,
/// placed in the parameters recoveryReportKind, reportedElements, and reportedInstances.
/// @returns None
void RegisterRecoveryReportCallback(CB_Recovery const &recoveryReportCallback) noexcept;
/// @brief Call registered recovery callback function
/// @code{.isoft}
/// @unit_name=RecoveryReport
/// @endcode
/// @param[in] storage
/// @param[in] recoveryReportKind
/// @param[in] reportedElements
/// @param[in] reportedInstances
/// @return no
void RecoveryReport(ara::core::InstanceSpecifier const &storage,
                    ara::per::RecoveryReportKind recoveryReportKind,
                    ara::core::Vector< ara::core::String > reportedElements,
                    ara::core::Vector< uint8_t > reportedInstances) noexcept;
/// @brief Whether recovery callback is registered
/// @code{.isoft}
/// @unit_name=IsHaveRecoverReport
/// @endcode
/// @return
bool IsHaveRecoverReport() noexcept;
//********************************/
}  // namespace per
}  // namespace ara
#endif
