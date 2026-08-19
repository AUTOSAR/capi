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
/// @file       per_transfer_status_storage.cpp
/// @brief      The PerTransferStatusStorage class which is used to store and get the transfer status.
/// @details
/// @date       2022-06-13
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=PerTransferStatusStorage
/// @unit_description=The PerTransferStatusStorage class which is used to store and get the transfer status.
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/transfer/per_transfer_status_storage.h"

#include "ara/per/key_value_storage.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Creates a persistency KVS with a given model identifier
///
/// @param modelIdentifier Identifier used to access KVS
/// @throws no
PerTransferStatusStorage::PerTransferStatusStorage(AraStringView const modelIdentifier)
    : TransferStatusStorage{}
    , kvs_{ara::per::OpenKeyValueStorage(ara::core::InstanceSpecifier(modelIdentifier)).ValueOrThrow()}
{
}

/// @brief Stores transfer status with the given key
///
/// @param key Associated Key
/// @param status Transfer Status to store
/// @throws no
void PerTransferStatusStorage::StoreStatus(AraStringView const& key, value_type const& status)
{
    kvs_->SetValue< TransferStatusStorage::value_type >(key, status).ValueOrThrow();
    std::ignore = kvs_->SyncToStorage();
}

/// @brief Retrieves stored state with given key
///
/// @param key Associated Key
/// @return Optional Transfer Status
/// @throws no
AraOptional< TransferStatusStorage::value_type > PerTransferStatusStorage::GetStatus(AraStringView const& key)
{
    if (kvs_->KeyExists(key)) {
        AraResult< TransferStatusStorage::value_type > const ret{
            kvs_->GetValue< TransferStatusStorage::value_type >(key)};
        if (ret.HasValue()) {
            return {ret.Value()};
        }
    }
    return {};
}

/// @brief Removes stored status from the storage by a given key`
///
/// @param key Associated key
/// @throws no
void PerTransferStatusStorage::RemoveStatus(AraStringView const& key)
{
    std::ignore = kvs_->RemoveKey(key);
    std::ignore = kvs_->SyncToStorage();
}

/// @brief Stores software package info with the given key
///
/// @param key Associated Key
/// @param packageInfo software package info to store
/// @throws no
void PerTransferStatusStorage::StoreSwPackageInfo(AraStringView const& key, SwPackageInfoValueType const& packageInfo)
{
    kvs_->SetValue< TransferStatusStorage::SwPackageInfoValueType >(key, packageInfo).ValueOrThrow();
    std::ignore = kvs_->SyncToStorage();
}

/// @brief Retrieves stored software package info with given key
///
/// @param key Associated Key
/// @return Optional software package info
/// @throws no
AraOptional< TransferStatusStorage::SwPackageInfoValueType > PerTransferStatusStorage::GetSwPackageInfo(
    AraStringView const& key)
{
    if (kvs_->KeyExists(key)) {
        AraResult< TransferStatusStorage::SwPackageInfoValueType > const ret{
            kvs_->GetValue< TransferStatusStorage::SwPackageInfoValueType >(key)};
        if (ret.HasValue()) {
            return {ret.Value()};
        }
    }
    return {};
}

/// @brief Removes stored software package info from the storage by a given key`
///
/// @param key Associated key
/// @throws no
void PerTransferStatusStorage::RemoveSwPackageInfo(AraStringView const& key)
{
    std::ignore = kvs_->RemoveKey(key);
    std::ignore = kvs_->SyncToStorage();
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
