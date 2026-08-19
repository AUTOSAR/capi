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
/// @file       update.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    AP-Per update module
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
/// @unit_description=Update
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_UPDATE_H_
#define ARA_PER_UPDATE_H_

#include <functional>

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace ara {
namespace per {
//********************************/
/// @brief Register callback function when updating, opening, or creating data center, handling version consistency issues.
/// @code{.isoft}
/// @unit_name=RegisterApplicationDataUpdateCallback
/// @traceid {SWS_PER_00356}
/// @threadsafety {no}
/// @endcode
/// @brief Registers an application data update callback with Persistency.
/// The provided callback function will be called by Persistency if an update of stored application
/// data might be necessary.
/// This decision is based on the Executable versions.
/// The version that last accessed Persistency is provided as an argument to the callback,
/// as well as the InstanceSpecifier referring to the updated Key-Value Storage or File Storage.
/// Based on this information, the application can decide which updates are actually necessary,
/// e.g. amigration from any older version could be supported, with different steps required
/// for each of these.
/// The provided function will be called from the context
/// of UpdatePersistency(), OpenKeyValueStorage(), or OpenFileStorage().
/// @param[in] appDataUpdateCallback The callback function to be called by Persistency
/// after an update of persistent data took place.
/// The function will be called with the shortName path of an updated Key-Value Storage
/// or File Storage, and with the Executable version with which the Persistency was last accessed.
/// @returns None
void RegisterApplicationDataUpdateCallback(
    std::function< void(const ara::core::InstanceSpecifier &storage, ara::core::String version) > const
        &appDataUpdateCallback) noexcept;
/// @brief After manifest installation completes, call this function to update all files and key-value data centers.
/// @code{.isoft}
/// @unit_name=UpdatePersistency
/// @traceid {SWS_PER_00357}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief
/// Updates all Persistency File Storages and Key-Value Storages after a new manifest was installed.
/// This method can be used to update the persistent data of the application during verification phase.
/// @returns A Result of void.
/// In case of an error, it contains any of the errors defined below, or a vendor specific error.
ara::core::Result< void > UpdatePersistency() noexcept;
/// @brief Completely reset all files and key-value data centers.
/// @code{.isoft}
/// @unit_name=ResetPersistency
/// @traceid {SWS_PER_00358}
/// @threadsafety {re-entrant}
/// @endcode
/// @brief Resets all File Storages and Key-Value Storages by entirely removing their content.
/// The File Storages and Key-Value Storages will be re-created when OpenFileStorage
/// or OpenKeyValueStorage is called next time.
/// @returns A Result of void.
/// In case of an error, it contains any of the errors defined below, or a vendor specific error.
ara::core::Result< void > ResetPersistency() noexcept;
//********************************/
/// @brief Clean up unused Storage libraries left after update
/// @code{.isoft}
/// @unit_name=ClearUpPersistency
/// @traceid {SWS_PER_00389 SWS_PER_00393}
/// @endcode
/// @param stEtcPathOld Old version ETC directory
/// @param stEtcPathNew New version ETC directory
/// @return Success or failure
bool ClearUpPersistency(ara::core::StringView const &stEtcPathOld, ara::core::StringView const &stEtcPathNew) noexcept;
//********************************/
}  // namespace per
}  // namespace ara

#endif
