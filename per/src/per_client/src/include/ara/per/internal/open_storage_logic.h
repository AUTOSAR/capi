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
/// @file       open_storage_logic.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Logic when opening Puhua Storage
/// @date       2021-04-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/KV Engine
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Logic when opening Puhua Storage
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-04-13 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_OPEN_STORAGE_LOGIC_H_
#define ARA_PER_OPEN_STORAGE_LOGIC_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <memory>

#include "ara/per/internal/isoftkv/config_muster.h"
#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/shared_ptr_manager.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Update notification callback function
/// @code{.isoft}
/// @unit_name=CB_ApplicationDataUpdate
/// @endcode
using CB_ApplicationDataUpdate
    = std::function< void(ara::core::InstanceSpecifier const& storage, ara::core::String version) >;
/// @brief Register an Update callback function
/// @code{.isoft}
/// @unit_name=RegUpdateCallback
/// @endcode
/// @param pfunc
void RegUpdateCallback(CB_ApplicationDataUpdate pfunc);
/// @brief Return the callback function registered for Update
/// @code{.isoft}
/// @unit_name=GetUpdateCallback
/// @endcode
/// @return
CB_ApplicationDataUpdate GetUpdateCallback() noexcept;
//********************************/
/// @code{.isoft}
/// @unit_name=MakeReddCheckAlgorithm
/// @endcode
/// @brief Create an interface object for performing Redd verification
/// @param configMuster
/// @return
std::unique_ptr< isoftkv::IReddAlgorithm > MakeReddCheckAlgorithm(isoftkv::PConfigMuster const& configMuster) noexcept;
/// @brief Return a newly created unique pointer
/// @code{.isoft}
/// @unit_name=MakeUniqueKvSystem
/// @endcode
/// @param kvConfig
/// @param bForceCreateNew Whether to create a new library if it does not exist
/// @return
std::unique_ptr< isoftkv::PKvSystem > MakeUniqueKvSystem(isoftkv::PConfigMuster_Kv const& kvConfig,
                                                         bool bForceCreateNew) noexcept;
//********************************/
/// @brief White-glove class for operating FileStorage (update, backup, rollback)
/// @code{.isoft}
/// @unit_name=POpenStorageLogic
/// @endcode
template < typename T_Storage >
class POpenStorageLogic final
{
public:
    /// @brief
    POpenStorageLogic() noexcept = delete;
    /// @brief
    ~POpenStorageLogic() noexcept = delete;
    /// @brief
    /// @param[in] a
    POpenStorageLogic(POpenStorageLogic const& a) = delete;
    /// @brief
    /// @param[in] a
    /// @return
    POpenStorageLogic& operator=(POpenStorageLogic const& a) = delete;
    /// @brief
    /// @param[in] a
    POpenStorageLogic(POpenStorageLogic&& a) = delete;
    /// @brief
    /// @param[in] a
    /// @return
    POpenStorageLogic& operator=(POpenStorageLogic&& a) = delete;

public:
    /// @brief Check if Storage exists
    /// @param[in] portIns
    /// @return
    static bool IsStorageExist(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Check if Storage is working
    /// @param[in] portIns
    /// @return
    static bool IsStorageBusy(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Unregister after KvStorage releases
    /// @param[in] stStorageName
    static void DegStorage(ara::core::String const& stStorageName) noexcept;
    /// @brief The function logic consists of the following SWS_PER: SWS_PER_00396, SWS_PER_00387, SWS_PER_00446, SWS_PER_00382, SWS_PER_00383
    static ara::core::Result< std::shared_ptr< T_Storage > >
    /// @brief Execute the logic to open Storage
    /// @param[in] portIns
    /// @return
    DoOpenLogic(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Create new Storage
    /// @param[in] portIns
    /// @return
    static std::shared_ptr< T_Storage > NewStorage(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Delete a File data center: 2023-08-11 internal interface
    /// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
    /// @returns
    /// A Result of void. In case of an error, it contains any of the errors defined below,
    /// or a vendor specific error.
    static ara::core::Result< void > RemoveStorage(ara::core::InstanceSpecifier const& portIns) noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif