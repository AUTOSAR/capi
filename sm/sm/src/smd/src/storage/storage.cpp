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
/// @file       storage.cpp
/// @brief      A class represents a persistency storage (key-value database)
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Storage
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002, SR_SM_04004
/// @unit_name=Storage
/// @unit_description=A class represents a persistency storage (key-value database)
/// @endcode
///
/// ================================================================

#include "storage.h"

namespace ara {
namespace sm {
namespace storage {
/// @brief Constructor function
/// @param modelIdentifier
Storage::Storage(core::StringView const &modelIdentifier) noexcept
    : kvsInstanceSpecifier_{modelIdentifier}
    , kvs_{}
    , log_{log::CreateLogger((ara::core::StringView{common::GetkSTO()}),
                             (ara::core::StringView{common::GetkStorageContext()}))}
{
}

/// @brief Open
/// @return
bool Storage::Open() noexcept
{
    /// @brief Type simplification
    using ResultKeyValueStorageHandler = core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > >;
    ResultKeyValueStorageHandler const res{ara::per::OpenKeyValueStorage(kvsInstanceSpecifier_)};
    if (res.HasValue()) {
        kvs_ = res.Value();
        return true;
    }
    log_.LogWarn() << "Storage::Open() error, modelIdentifier:" << kvsInstanceSpecifier_.ToString().data() << "because"
                   << res.Error().Message();
    return false;
}

/// @brief Store
/// @tparam ValueType
/// @param key
/// @param value
/// @return
template < typename ValueType >
inline bool Storage::Store(core::StringView const &key, ValueType const &value) noexcept
{
    log_.LogInfo() << "Storage::Get(), modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                   << "store a value for key:" << key.data();
    if (kvs_) {
        core::Result< void > res{kvs_->SetValue< ValueType >(key, value)};
        if (res.HasValue()) {
            res = kvs_->SyncToStorage();
        }
        return res.HasValue();
    }
    log_.LogWarn() << "Storage::Store() error, modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                   << "because kvs_ is nullptr";
    return false;
}
/// @brief Store
/// @param key Key
/// @param value Value
/// @return true, success; false, failure;
/// @code{.isoft}
/// @tparam ValueType Value type
/// @interface_level=none
/// @needwork = no
/// @endcode
template bool Storage::Store< core::Array< std::int64_t, TWO_UL > >(
    core::StringView const &key, core::Array< std::int64_t, TWO_UL > const &value);  // explicit instantiation

/// @brief Store
/// @param key Key
/// @param value Value
/// @return true, success; false, failure;
/// @code{.isoft}
/// @tparam ValueType Value type
/// @interface_level=none
/// @needwork = no
/// @endcode
template bool Storage::Store< std::set< core::String > >(
    core::StringView const &key, std::set< core::String > const &value);  // explicit instantiation

/// @brief Store
/// @param key Key
/// @param value Value
/// @return true, success; false, failure;
/// @code{.isoft}
/// @tparam ValueType Value type
/// @interface_level=none
/// @needwork = no
/// @endcode
template bool Storage::Store< core::String >(core::StringView const &key,
                                             core::String const &value);  // explicit instantiation

/// @brief Get
/// @tparam ValueType
/// @param key
/// @return Stored value
template < typename ValueType >
inline core::Optional< ValueType > Storage::Get(core::StringView const &key) noexcept
{
    if (kvs_) {
        core::Result< ValueType > const res{std::move(kvs_->GetValue< ValueType >(key))};
        if (res.HasValue()) {
            log_.LogInfo() << "Storage::Get(), modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                           << "get a value for key:" << key;
            return res.Value();
        }
        log_.LogWarn() << "Storage::Get() , modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                       << "can't get a value for key:" << key;
    }
    log_.LogWarn() << "Storage::Get() error, modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                   << "because kvs_ is nullptr";
    return {};
}

/// @brief Get
/// @tparam ValueType Value type
/// @param key Key
/// @return
template core::Optional< core::String > Storage::Get(core::StringView const &key);  // explicit instantiation
/// @brief Get
/// @tparam ValueType Value type
/// @param key Key
/// @return
template core::Optional< core::Array< std::int64_t, TWO_UL > > Storage::Get(
    core::StringView const &key);  // explicit instantiation
/// @brief Get
/// @tparam ValueType Value type
/// @param key Key
/// @return
template core::Optional< std::set< core::String > > Storage::Get(
    core::StringView const &key);  // explicit instantiation

/// @brief Delete
/// @param key
void Storage::Remove(core::StringView const &key) noexcept
{
    log_.LogInfo() << "Storage::Remove(), modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                   << "remove key:" << key;
    if (kvs_) {
        core::Result< void > res{kvs_->RemoveKey(key)};
        if (res.HasValue()) {
            res = kvs_->SyncToStorage();
            if (!res.HasValue()) {
                log_.LogWarn() << "Storage::Remove(), SyncToStorage error, modelIdentifier:"
                               << kvsInstanceSpecifier_.ToString().data() << "because" << res.Error().Message();
            }
        } else {
            log_.LogWarn() << "Storage::Remove(), RemoveKey error, modelIdentifier:"
                           << kvsInstanceSpecifier_.ToString().data() << "because" << res.Error().Message();
        }
        return;
    }
    log_.LogWarn() << "Storage::Remove() error, modelIdentifier:" << kvsInstanceSpecifier_.ToString().data()
                   << "because kvs_ is nullptr";
}
}  // namespace storage
}  // namespace sm
}  // namespace ara